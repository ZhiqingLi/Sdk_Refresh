/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * Companion peripheral to transport driver
 * $Revision: 1.19 $
 */
#if 0
#include "mu_arch.h"
#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_diag.h"
#include "class/mu_trans.h"

#define MGC_TRANS_MAX_BUFFER	8*1024

/******************************************************************
Forwards
******************************************************************/
static void MGC_TransNewUsbState(void* hClient, 
				 MUSB_BusHandle hBus, 
				 MUSB_State State);

static uint8_t MGC_TransDeviceRequest(void* hClient, 
				      MUSB_BusHandle hBus, 
				      uint32_t dwSequenceNumber, 
				      const uint8_t* pSetup, 
				      uint16_t wRequestLength);


static uint8_t MGC_TransDeviceConfigSelected(void* hClient, 
					     MUSB_BusHandle hBus, 
					     uint8_t bConfigurationValue, 
					     MUSB_Pipe* ahPipe);

static uint8_t MGC_TransOpenSinkEnd(MUSB_BusHandle hBus, uint8_t bOutEnd, 
				    const MUSB_TransportConfigureEnd* pConfigureEnd);
static uint8_t MGC_TransOpenSourceEnd(MUSB_BusHandle hBus, uint8_t bOutEnd, 
				      const MUSB_TransportConfigureEnd* pConfigureEnd);

static void MGC_TransCloseSinkEnd(uint8_t bEnd);
static void MGC_TransCloseSourceEnd(uint8_t bEnd);

static void MGC_TransSinkIrpComplete(void *pPrivateData, MUSB_Irp *pIrp);
static void MGC_TransSourceIrpComplete(void *pPrivateData, MUSB_Irp *pIrp);

#ifdef MUSB_ISO
static void MGC_TransSinkIsochIrpComplete(void* pPrivateData, MUSB_IsochIrp* pIrp);
static void MGC_TransSourceIsochIrpComplete(void* pPrivateData, MUSB_IsochIrp* pIrp);
#endif

static void MGC_TransSourceData(uint8_t bInEnd, uint8_t* pBuffer);

/******************************************************************
Globals
******************************************************************/

/* UCDI variables */
static uint8_t MGC_bTransSelfPowered = TRUE;
static MUSB_State MGC_eTransUsbState = MUSB_POWER_OFF;

static uint8_t MGC_aControlData[256];

static const uint8_t MGC_aTransDescriptorData[] = 
{
    /* Device Descriptor */
    0x12,                      /* bLength              */
    MUSB_DT_DEVICE,            /* DEVICE               */
    0x00,0x02,                 /* USB 2.0              */
    0x00,                      /* CLASS                */
    0x00,                      /* Subclass             */
    0x00,                      /* Protocol             */
    0x40,                      /* bMaxPktSize0         */
    0xd6,0x04,                 /* idVendor             */
    0xff,0xff,                 /* idProduct            */
    0x00,0x02,                 /* bcdDevice            */
    0x00,                      /* iManufacturer        */
    0x00,                      /* iProduct             */
    0x00,                      /* iSerial Number       */
    0x01,                      /* One configuration    */

    /* configuration */
    0x09,                                   /* bLength              */
    0x02,                                   /* CONFIGURATION        */
    0x09,                                   /* length               */
    0x0,                                    /* length               */
    0x00,                                   /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    0xC0,                                   /* bmAttributes (required + self-powered) */
    0x2                                     /* power                */

};

static const uint8_t MGC_aTransHighSpeedDescriptorData[] = 
{
    /* device qualifier */
    0x0a,                      /* bLength              */
    MUSB_DT_DEVICE_QUALIFIER,  /* DEVICE Qualifier     */
    0x00,0x02,                 /* USB 2.0              */
    0,                         /* CLASS                */
    0,                         /* Subclass             */
    0x00,                      /* Protocol             */
    0x40,                      /* bMaxPacketSize0      */
    0x01,                      /* One configuration    */
    0x00,                      /* bReserved            */

    /* configuration */
    0x09,                                   /* bLength              */
    MUSB_DT_OTHER_SPEED,                    /* CONFIGURATION        */
    0x09,                                   /* length               */
    0x0,                                    /* length               */
    0x00,                                   /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    0xC0,                                   /* bmAttributes (required + self-powered) */
    0x2                                     /* power                */
};

static MUSB_Pipe MGC_aTransInPipe[16];
static MUSB_Pipe MGC_aTransOutPipe[16];

static MUSB_Irp MGC_aSinkIrp[16];
static MUSB_Irp MGC_aSourceIrp[16];
static uint8_t MGC_aSourcePattern[16];
static uint32_t MGC_adwSourceLength[16];
static MUSB_TransportConfigureEnd MGC_aConfigureInEnd[16];
static MUSB_TransportConfigureEnd MGC_aConfigureOutEnd[16];

#ifdef MUSB_ISO
static MUSB_IsochIrp MGC_aSinkIsochIrp[16];
static MUSB_IsochIrp MGC_aSourceIsochIrp[16];
#endif

static uint8_t MGC_aSourceBuffer[MGC_TRANS_MAX_BUFFER][16];
static uint8_t MGC_aSinkBuffer[MGC_TRANS_MAX_BUFFER][16];

/*
 * registration
 */
MUSB_FunctionClient MGC_TransFunctionClient =
{
    NULL,   /* filled in init */
    MGC_aTransDescriptorData,
    sizeof(MGC_aTransDescriptorData),
    0,      /* strings per language */
    MGC_aTransHighSpeedDescriptorData,
    sizeof(MGC_aTransHighSpeedDescriptorData),
    sizeof(MGC_aControlData),
    MGC_aControlData,
    &MGC_bTransSelfPowered,
    MGC_TransDeviceRequest,
    MGC_TransDeviceConfigSelected,
    NULL,
    MGC_TransNewUsbState,
};

/******************************************************************
Functions
******************************************************************/

/* Get the singleton client */
MUSB_FunctionClient* MGC_GetTransportFunctionClient()
{
    return &MGC_TransFunctionClient;
}

/******************************************************************
UCDI callbacks
******************************************************************/
static void MGC_TransNewUsbState (void* hClient, 
				  MUSB_BusHandle    hBus, 
				  MUSB_State        State)
{
    MGC_eTransUsbState = State;

    /* TODO: anything? */
}

/* UCD callback for configuration selected */
static uint8_t MGC_TransDeviceConfigSelected(void*          hClient, 
					     MUSB_BusHandle hBus, 
					     uint8_t        bConfigurationValue, 
					     MUSB_Pipe*     ahPipe)
{
    return (TRUE);
}

/*
 * Device request API. UCD calls this function for each device request it 
 * doesn't handle automatically.
 */
static uint8_t MGC_TransDeviceRequest(void* hClient, 
				      MUSB_BusHandle hBus, 
				      uint32_t dwSequenceNumber, 
				      const uint8_t* pSetupBuffer, 
				      uint16_t wRequestLength)
{
    const MUSB_TransportConfigureEnd* pConfigureEnd;
    uint8_t bResult = FALSE;
    uint8_t bInEnd = 0;
    uint8_t bOutEnd = 0;
    const MUSB_DeviceRequest* pRequest = (const MUSB_DeviceRequest*)pSetupBuffer;
    
    if(((MUSB_TYPE_VENDOR | MUSB_RECIP_OTHER) == pRequest->bmRequestType) && 
	(MUSB_TRANSPORT_REQ_CONFIGURE_END == pRequest->bRequest))
    {
	pConfigureEnd = (const MUSB_TransportConfigureEnd*)&(pSetupBuffer[8]);
	switch(pConfigureEnd->bType)
	{
	case MUSB_TRANSPORT_REQ_SET_NONE:
	    bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
	    bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;
	    if(bInEnd)
	    {
		MGC_TransCloseSourceEnd(bInEnd);
	    }
	    if(bOutEnd)
	    {
		MGC_TransCloseSinkEnd(bOutEnd);
	    }
	    bResult = TRUE;
	    break;

	case MUSB_TRANSPORT_REQ_SET_LOOPBACK:
	    bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
	    bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;
	    if(bInEnd && bOutEnd)
	    {
		bResult = MGC_TransOpenSinkEnd(hBus, bOutEnd, pConfigureEnd);
		if(bResult)
		{
		    bResult = MGC_TransOpenSourceEnd(hBus, bInEnd, pConfigureEnd);
		}
		else
		{
		    MGC_TransCloseSinkEnd(bOutEnd);
		}
	    }
	    break;

	case MUSB_TRANSPORT_REQ_SET_SINK:
	    bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;
	    if(bOutEnd)
	    {
		bResult = MGC_TransOpenSinkEnd(hBus, bOutEnd, pConfigureEnd);
	    }
	    break;

	case MUSB_TRANSPORT_REQ_SET_SOURCE:
	    bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
	    if(bInEnd)
	    {
		bResult = MGC_TransOpenSourceEnd(hBus, bInEnd, pConfigureEnd);
	    }
	    break;

	}
    }

    if(bResult)
    {
	MUSB_DeviceResponse(hBus, dwSequenceNumber, NULL, 0, FALSE);
    }

    return bResult;
}

/******************************************************************
Client Functionality
******************************************************************/

static uint8_t MGC_TransOpenSinkEnd(MUSB_BusHandle hBus, uint8_t bOutEnd, 
				    const MUSB_TransportConfigureEnd* pConfigureEnd)
{
    uint32_t dwStatus, dwLength;
    MUSB_Irp* pIrp;
#ifdef MUSB_ISO
    MUSB_IsochIrp* pIsochIrp;
#endif
    MUSB_EndpointDescriptor EndDescriptor;
    uint8_t bResult = FALSE;

    MUSB_MemCopy(&(MGC_aConfigureOutEnd[bOutEnd]), pConfigureEnd,
	sizeof(*pConfigureEnd));
    EndDescriptor.bLength = MUSB_DT_ENDPOINT_SIZE;
    EndDescriptor.bDescriptorType = MUSB_DT_ENDPOINT;
    EndDescriptor.bEndpointAddress = MUSB_DIR_OUT | bOutEnd;
    EndDescriptor.bmAttributes = pConfigureEnd->bmAttributes;
    EndDescriptor.wMaxPacketSize = MUSB_SWAP16P(&(pConfigureEnd->wMaxPacketSize));
    EndDescriptor.bInterval = 1;

    dwLength = EndDescriptor.wMaxPacketSize * pConfigureEnd->bPacketCount;
    if(dwLength <= MGC_TRANS_MAX_BUFFER)
    {
	MGC_aTransOutPipe[bOutEnd] = MUSB_BindFunctionEnd(hBus, &EndDescriptor);
	if(MGC_aTransOutPipe[bOutEnd])
	{
	    switch(pConfigureEnd->bmAttributes)
	    {
	    case MUSB_ENDPOINT_XFER_BULK:
		pIrp = &(MGC_aSinkIrp[bOutEnd]);
		pIrp->hPipe = MGC_aTransOutPipe[bOutEnd];
		pIrp->pBuffer = MGC_aSinkBuffer[bOutEnd];
		pIrp->dwLength = dwLength;
		pIrp->pfIrpComplete = MGC_TransSinkIrpComplete;
		pIrp->pCompleteParam = (void*)&(MGC_aConfigureOutEnd[bOutEnd]);
		pIrp->bIsrCallback = TRUE;
		dwStatus = MUSB_StartTransfer(pIrp);
		bResult = (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
		break;
#ifdef MUSB_ISO
	    case MUSB_ENDPOINT_XFER_ISOC:
		pIsochIrp = &(MGC_aSinkIsochIrp[bOutEnd]);
		pIsochIrp->hPipe = MGC_aTransOutPipe[bOutEnd];
		pIsochIrp->pBuffer = MGC_aSinkBuffer[bOutEnd];
		pIsochIrp->wFrameCount = pConfigureEnd->bPacketCount;
		/* TODO: set this */
		pIsochIrp->adwLength = NULL;
		pIsochIrp->adwActualLength = NULL;
		pIsochIrp->adwStatus = NULL;
		pIsochIrp->pfIrpComplete = MGC_TransSinkIsochIrpComplete;
		pIsochIrp->pCompleteParam = (void*)&(MGC_aConfigureOutEnd[bOutEnd]);
		pIsochIrp->bIsrCallback = TRUE;
		dwStatus = MUSB_ScheduleIsochTransfer(0, pIsochIrp);
		bResult = (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
		break;
#endif
	    }
	}
    }
    if(!bResult && MGC_aTransOutPipe[bOutEnd])
    {
	MUSB_CloseFunctionPipe(MGC_aTransOutPipe[bOutEnd]);
    }
    return bResult;
}

static uint8_t MGC_TransOpenSourceEnd(MUSB_BusHandle hBus, uint8_t bInEnd, 
				      const MUSB_TransportConfigureEnd* pConfigureEnd)
{
    uint32_t dwStatus, dwLength;
    MUSB_Irp* pIrp;
#ifdef MUSB_ISO
    MUSB_IsochIrp* pIsochIrp;
#endif
    MUSB_EndpointDescriptor EndDescriptor;
    uint8_t bResult = FALSE;

    MUSB_MemCopy(&(MGC_aConfigureInEnd[bInEnd]), pConfigureEnd,
	sizeof(*pConfigureEnd));
    EndDescriptor.bLength = MUSB_DT_ENDPOINT_SIZE;
    EndDescriptor.bDescriptorType = MUSB_DT_ENDPOINT;
    EndDescriptor.bEndpointAddress = MUSB_DIR_IN | bInEnd;
    EndDescriptor.bmAttributes = pConfigureEnd->bmAttributes;
    EndDescriptor.wMaxPacketSize = MUSB_SWAP16P(&(pConfigureEnd->wMaxPacketSize));
    EndDescriptor.bInterval = 1;

    MGC_aSourcePattern[bInEnd] = pConfigureEnd->bStartPattern;
    dwLength = MGC_adwSourceLength[bInEnd] = 
	EndDescriptor.wMaxPacketSize * pConfigureEnd->bPacketCount;
    if(dwLength <= MGC_TRANS_MAX_BUFFER)
    {
	MGC_aTransInPipe[bInEnd] = MUSB_BindFunctionEnd(hBus, &EndDescriptor);
	if(MGC_aTransInPipe[bInEnd])
	{
	    switch(pConfigureEnd->bmAttributes)
	    {
	    case MUSB_ENDPOINT_XFER_BULK:
		pIrp = &(MGC_aSourceIrp[bInEnd]);
		pIrp->hPipe = MGC_aTransInPipe[bInEnd];
		pIrp->pBuffer = MGC_aSourceBuffer[bInEnd];
		pIrp->dwLength = dwLength;
		pIrp->pfIrpComplete = MGC_TransSourceIrpComplete;
		pIrp->pCompleteParam = (void*)&(MGC_aConfigureInEnd[bInEnd]);
		pIrp->bIsrCallback = TRUE;
		dwStatus = MUSB_StartTransfer(pIrp);
		bResult = (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
		break;
#ifdef MUSB_ISO
	    case MUSB_ENDPOINT_XFER_ISOC:
		pIsochIrp = &(MGC_aSourceIsochIrp[bInEnd]);
		pIsochIrp->hPipe = MGC_aTransInPipe[bInEnd];
		pIsochIrp->pBuffer = MGC_aSourceBuffer[bInEnd];
		pIsochIrp->wFrameCount = pConfigureEnd->bPacketCount;
		/* TODO: set this */
		pIsochIrp->adwLength = NULL;
		pIsochIrp->adwActualLength = NULL;
		pIsochIrp->adwStatus = NULL;
		pIsochIrp->pfIrpComplete = MGC_TransSourceIsochIrpComplete;
		pIrp->pCompleteParam = (void*)&(MGC_aConfigureInEnd[bInEnd]);
		pIrp->bIsrCallback = TRUE;
		dwStatus = MUSB_ScheduleIsochTransfer(0, pIsochIrp);
		bResult = (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
		break;
#endif
	    }
	}
    }
    if(!bResult && MGC_aTransInPipe[bInEnd])
    {
	MUSB_CloseFunctionPipe(MGC_aTransInPipe[bInEnd]);
    }
    return bResult;
}

static void MGC_TransCloseSinkEnd(uint8_t bEnd)
{
    if(MGC_aTransOutPipe[bEnd])
    {
	MUSB_CloseFunctionPipe(MGC_aTransOutPipe[bEnd]);
    }
}

static void MGC_TransCloseSourceEnd(uint8_t bEnd)
{
    if(MGC_aTransInPipe[bEnd])
    {
	MUSB_CloseFunctionPipe(MGC_aTransInPipe[bEnd]);
    }
}

static void MGC_TransSinkIrpComplete(void *pPrivateData, MUSB_Irp *pIrp)
{
    const MUSB_TransportConfigureEnd* pConfigureEnd = 
	(const MUSB_TransportConfigureEnd*)pPrivateData;
    uint8_t bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
    uint8_t bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;

    switch(pConfigureEnd->bType)
    {
    case MUSB_TRANSPORT_REQ_SET_LOOPBACK:
	MUSB_MemCopy(MGC_aSourceIrp[bInEnd].pBuffer, pIrp->pBuffer, pIrp->dwActualLength);
	MUSB_StartTransfer(&(MGC_aSourceIrp[bInEnd]));
	break;
    case MUSB_TRANSPORT_REQ_SET_SINK:
	MUSB_StartTransfer(&(MGC_aSinkIrp[bOutEnd]));
	break;
    }
}

static void MGC_TransSourceIrpComplete(void *pPrivateData, MUSB_Irp *pIrp)
{
    const MUSB_TransportConfigureEnd* pConfigureEnd = 
	(const MUSB_TransportConfigureEnd*)pPrivateData;
    uint8_t bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
    uint8_t bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;

    switch(pConfigureEnd->bType)
    {
    case MUSB_TRANSPORT_REQ_SET_LOOPBACK:
	MUSB_StartTransfer(&(MGC_aSinkIrp[bOutEnd]));
	break;
    case MUSB_TRANSPORT_REQ_SET_SOURCE:
	MGC_TransSourceData(bInEnd, pIrp->pBuffer);
	break;
    }
}

#ifdef MUSB_ISO
static void MGC_TransSinkIsochIrpComplete(void *pPrivateData, MUSB_IsochIrp *pIrp)
{
    uint16_t wFrame;
    uint32_t dwTotalLength;
    const MUSB_TransportConfigureEnd* pConfigureEnd = 
	(const MUSB_TransportConfigureEnd*)pPrivateData;
    uint8_t bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
    uint8_t bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;

    switch(pConfigureEnd->bType)
    {
    case MUSB_TRANSPORT_REQ_SET_LOOPBACK:
	for(wFrame = dwTotalLength = 0; wFrame < pIrp->wFrameCount; wFrame++)
	{
	    dwTotalLength += pIrp->adwActualLength[wFrame];
	}
	MUSB_MemCopy(MGC_aSourceIsochIrp[bInEnd].pBuffer, pIrp->pBuffer, dwTotalLength);
	MUSB_ScheduleIsochTransfer(0, &(MGC_aSourceIsochIrp[bInEnd]));
	break;
    case MUSB_TRANSPORT_REQ_SET_SINK:
	MUSB_ScheduleIsochTransfer(0, &(MGC_aSinkIsochIrp[bOutEnd]));
	break;
    }
}

static void MGC_TransSourceIsochIrpComplete(void *pPrivateData, MUSB_IsochIrp *pIrp)
{
    const MUSB_TransportConfigureEnd* pConfigureEnd = 
	(const MUSB_TransportConfigureEnd*)pPrivateData;
    uint8_t bInEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_IN_END) & MUSB_M_TRANSPORT_IN_END;
    uint8_t bOutEnd = (pConfigureEnd->bInOutEnds >> MUSB_S_TRANSPORT_OUT_END) & MUSB_M_TRANSPORT_OUT_END;

    switch(pConfigureEnd->bType)
    {
    case MUSB_TRANSPORT_REQ_SET_LOOPBACK:
	MUSB_ScheduleIsochTransfer(0, &(MGC_aSinkIsochIrp[bOutEnd]));
	break;
    case MUSB_TRANSPORT_REQ_SET_SOURCE:
	MGC_TransSourceData(bInEnd, pIrp->pBuffer);
	break;
    }

}
#endif

static void MGC_TransSourceData(uint8_t bInEnd, uint8_t* pBuffer)
{
#ifdef MUSB_ISO
    MUSB_IsochIrp* pIsochIrp;
#endif
    MUSB_Irp* pIrp;
    uint32_t dwStatus, dwIndex;
    uint8_t bBit;
    uint8_t bDatum = MGC_aSourcePattern[bInEnd];
    MUSB_TransportConfigureEnd* pConfigureEnd = &(MGC_aConfigureInEnd[bInEnd]);

    for(dwIndex = 0; dwIndex < MGC_adwSourceLength[bInEnd]; dwIndex++)
    {
	pBuffer[dwIndex] = bDatum;
	switch(pConfigureEnd->bOptions)
	{
	case MUSB_TRANSPORT_OPTIONS_INCREMENT:
	    bDatum++;
	    break;
	case MUSB_TRANSPORT_OPTIONS_DECREMENT:
	    bDatum--;
	    break;
	case MUSB_TRANSPORT_OPTIONS_ROTATE_LEFT:
	    bBit = bDatum & 0x80;
	    bDatum <<= 1;
	    bDatum |= bBit >> 7;
	    break;
	case MUSB_TRANSPORT_OPTIONS_ROTATE_RIGHT:
	    bBit = bDatum & 1;
	    bDatum >>= 1;
	    bDatum |= bBit << 7;
	    break;
	}

	if(0 == dwIndex)
	{
	    MGC_aSourcePattern[bInEnd] = bDatum;
	}

    }

    switch(pConfigureEnd->bmAttributes)
    {
    case MUSB_ENDPOINT_XFER_BULK:
	pIrp = &(MGC_aSourceIrp[bInEnd]);
	dwStatus = MUSB_StartTransfer(pIrp);
	break;
    case MUSB_ENDPOINT_XFER_INT:
	pIrp = &(MGC_aSourceIrp[bInEnd]);
	dwStatus = MUSB_InterruptReady(pIrp);
	break;
#ifdef MUSB_ISO
    case MUSB_ENDPOINT_XFER_ISOC:
	pIsochIrp = &(MGC_aSourceIsochIrp[bInEnd]);
	dwStatus = MUSB_ScheduleIsochTransfer(0, pIsochIrp);
	break;
#endif
    }

}
#endif
