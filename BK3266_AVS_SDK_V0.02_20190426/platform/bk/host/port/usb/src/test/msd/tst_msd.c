/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Command-line (really menu-driven) simple disk read/write utility,
 * to test mass-storage class driver and everything below it
 * $Revision: 1.34 $
 */

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_stdio.h"
#include "mu_strng.h"
#include "mu_hfi.h"
//#include "mu_ghi.h"
//#include "mu_kii.h"
#include "mu_spi.h"

#include "class/mu_msd.h"

#include "mu_mapi.h"
//#include "mu_hidif.h"

extern MUSB_FunctionClient MGC_MsdFunctionClient;

#define MGC_TEST_MSD_BUFFER_SIZE    1024

/**************************** FORWARDS ****************************/

static void MGC_MsdNewOtgState(void* hClient, MUSB_BusHandle hBus, 
			       MUSB_OtgState State);
static void MGC_MsdOtgError(void* hClient, MUSB_BusHandle hBus, 
			    uint32_t dwStatus);
//static void MGC_TestMsdMediumCheckComplete(MUSB_HfiVolumeHandle hVolume);

/**************************** GLOBALS *****************************/

//static uint8_t MGC_bTimingMode = FALSE;

/* current OTG state */
static MUSB_OtgState MGC_eTestMsdOtgState = MUSB_AB_IDLE;

/* UCDI variables */
static MUSB_Port* MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aMsdPeripheralList[256];

#ifdef MUSB_HUB
static MUSB_DeviceDriver MGC_aDeviceDriver[3];
#else
static MUSB_DeviceDriver MGC_aDeviceDriver[2];
#endif

static MUSB_HostClient MGC_MsdHostClient = 
{
    MGC_aMsdPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    /*sizeof(MGC_aMsdPeripheralList),*/	/* peripheral list length */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_MsdOtgClient = 
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_MsdNewOtgState,
    MGC_MsdOtgError
};

/*************************** FUNCTIONS ****************************/

/* OTG client */
static void MGC_MsdNewOtgState(void* hClient, MUSB_BusHandle hBus, 
			       MUSB_OtgState State)
{
    char aAnswer[4];
    MGC_eTestMsdOtgState = State;

    switch(State)
    {
	case MUSB_AB_IDLE:
		MUSB_PrintLine("S - Start Session");
		MUSB_GetLine(aAnswer, 4);
		if(('s' == aAnswer[0]) || ('S' == aAnswer[0]))
		{
			MUSB_RequestBus(MGC_hCdiBus);
		}
		break;
	case MUSB_A_SUSPEND:
		MUSB_PrintLine("R - Resume bus");
		MUSB_GetLine(aAnswer, 4);
		if(('r' == aAnswer[0]) || ('R' == aAnswer[0]))
		{
			MUSB_ResumeBus(MGC_hCdiBus);
		}
		break;
	default:
		break;
    }
}

static void MGC_MsdOtgError(void* hClient, MUSB_BusHandle hBus, 
			    uint32_t dwStatus)
{
    switch(dwStatus)
    {
    case MUSB_STATUS_UNSUPPORTED_DEVICE:
	MUSB_PrintLine("Device not supported");
	break;
    case MUSB_STATUS_UNSUPPORTED_HUB:
	MUSB_PrintLine("Hubs not supported");
	break;
    case MUSB_STATUS_OTG_VBUS_INVALID:
	MUSB_PrintLine("Vbus error");
	break;
    case MUSB_STATUS_OTG_NO_RESPONSE:
	MUSB_PrintLine("Device not responding");
	break;
    case MUSB_STATUS_OTG_SRP_FAILED:
	MUSB_PrintLine("Device not responding (SRP failed)");
	break;
    }
}

/* medium check completion callback */
/*
static void MGC_TestMsdMediumCheckComplete(MUSB_HfiVolumeHandle hVolume)
{
}
*/

static MUSB_HfiDevice* MGC_pHfiDevice = NULL;
static const MUSB_HfiMediumInfo* MGC_pHfiMediumInfo = NULL;
static uint8_t MediaIsOk = FALSE;
/*
static void MGC_CheckMedium()
{
    if(MGC_pHfiDevice && !MGC_pHfiMediumInfo)
    {
	MGC_pHfiDevice->pfCheckMediumNotify(MGC_pHfiDevice->pPrivateData, 
		MGC_TestMsdMediumCheckComplete);
    }
}
*/
/* Implementation */
MUSB_HfiStatus 
MUSB_HfiAddDevice(MUSB_HfiVolumeHandle* phVolume,
		  const MUSB_HfiDeviceInfo* pInfo, 
		  MUSB_HfiDevice* pDevice)
{
		MGC_pHfiDevice = pDevice;
		MediaIsOk = TRUE;
	return MUSB_HFI_SUCCESS;

}

/* Implementation */
void 
MUSB_HfiMediumInserted(MUSB_HfiVolumeHandle 	 hVolume,
		       const MUSB_HfiMediumInfo* pMediumInfo)
{
   	MGC_pHfiMediumInfo = pMediumInfo;
}

/* Implementation */
void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume)
{
	MGC_pHfiMediumInfo = NULL;
}

/* Implementation */
void MUSB_HfiDeviceRemoved(void)
{
	MGC_pHfiDevice = NULL;
	MediaIsOk = FALSE;
}

uint8_t MGC_MsdGetMediumstatus(void)
{
	uint8_t Ret = 0;
	if(MGC_pHfiDevice)// && (MGC_pHfiMediumInfo))
	{
		Ret = MediaIsOk;
	}
	return Ret;
}
static uint32_t MGC_MsdRdTransferComplete(MUSB_HfiVolumeHandle hVolume,
					uint16_t wActualBlocks)

{
	return 2;//read ok
}
uint32_t MUSB_HfiRead( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
//	os_printf("====read ==\r\n");
	uint32_t RetValue = 1;
	
	if(MGC_pHfiDevice)
	{
//	MGC_MsdBotReadDevice()
		RetValue = MGC_pHfiDevice->pfReadDevice(MGC_pHfiDevice->pPrivateData,
	    first_block, 0, block_num, dest,MGC_MsdRdTransferComplete, FALSE);
	}
	return RetValue;
}

uint32_t get_HfiMedium_size(void)
{
	if(MGC_pHfiMediumInfo)
		return MGC_pHfiMediumInfo->dwBlockCountLo;
	else
		return 0;
}
uint32_t get_HfiMedium_blksize(void)
{
	if(MGC_pHfiMediumInfo)
		return MGC_pHfiMediumInfo->dwBlockSize;
	else
		return 0;
}
/* Entrypoint */
int usb_sw_init(void)
{
    MUSB_DeviceDriver* pDriver;
    uint8_t* pList;
    uint16_t wCount, wSize, wRemain;
    uint8_t bDriver;	
    /* fill driver table */
    bDriver = 0;
    wSize = wCount = 0;
    wRemain = (uint16_t)sizeof(MGC_aMsdPeripheralList);
    pList = MGC_aMsdPeripheralList;
    wSize = MUSB_FillMsdPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = MUSB_GetStorageClassDriver();//作为HOST,获得对U盘的驱动程序
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_MsdHostClient.aDeviceDriverList[bDriver]),
            pDriver, sizeof(MUSB_DeviceDriver));
            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;
            bDriver++;
        }
    }

    MGC_MsdHostClient.wPeripheralListLength = wCount;
    MGC_MsdHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        os_printf("error ,could not initialize MicroSW\r\n");
        return -1;
    }

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);
/*	
    if(!MGC_pCdiPort)
    {
    	os_printf("MUSB_GetPort fail\r\n");
		MUSB_DestroySystem();
		return -1;
    }*/
    //os_printf("phase 2\r\n");
    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort, 
	&MGC_MsdFunctionClient, &MGC_MsdHostClient, &MGC_MsdOtgClient);
 /*   if(!MGC_hCdiBus)
    {
	    os_printf("MUSB_RegisterOtgClient fail\r\n");
		MUSB_DestroySystem();
		return -1;
    }*/
    return 0;
}

