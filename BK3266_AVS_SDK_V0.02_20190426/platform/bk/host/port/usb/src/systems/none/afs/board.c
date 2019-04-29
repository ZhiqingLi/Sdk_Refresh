/******************************************************************
 *                                                                *
 *      Copyright Mentor Graphics Corporation 2003-2004           *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * Non-OS board-specific code for any target supported by the ARM Firmware Suite
 * $Revision: 1.21 $
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "mu_diag.h"
#include "mu_mem.h"
#include "mu_none.h"
#include "brd_cnf.h"
#include "board.h"
#include "driver_icu.h"
//#include "uhal.h"
//#include "mmu_h.h"
//#include "pcilib.h"

/* 
 * Define this to log diagnostics to a RAM buffer and upload later with your debugger, etc.
#define MUSB_MSG_BUF
 */

/***************************** TYPES ******************************/

/**
 * @field iVector uHAL's vector for reverse-lookup
 * @field iIndex uHAL's timer index
 * @field pfExpired expiration callback
 * @field pParam expiration callback parameter
 * @field dwTime remaining time, due to uHAL's MAX_PERIOD limitation
 * @field bPeriodic whether currently set for periodic
 */
typedef struct
{
    unsigned int iVector;
    unsigned int iIndex;
    MUSB_pfTimerExpired pfExpired;
    void* pParam;
    uint32_t dwTime;//定时值
    uint8_t bPeriodic;
	uint8_t bTimerStart;//表示定时器是否起作用，1:工作,0:不工作
} MGC_AfsTimerWrapper;

/**
 * MGC_AfsUds.
 * Board-specific UDS instance data.
 * @param pNonePrivateData non-OS UDS instance data
 * @param pfNoneIsr non-OS UDS ISR
 * @field aTimerWrapper timer wrappers
 * @field wTimerCount how many wrappers
 * @field bIndex our index into the global array
 */
typedef struct
{
    char aIsrName[8];
    void* pNonePrivateData;
    MUSB_NoneIsr pfNoneIsr;
    void* pPciAck;
    MGC_AfsTimerWrapper* aTimerWrapper;
    unsigned int dwIrq;
    uint16_t wTimerCount;
    uint8_t bIndex;
} MGC_AfsUds;

/*************************** FORWARDS *****************************/

void MGC_AfsUdsIsr(uint16_t wIntrTxValue,uint16_t wIntrRxValue,uint8_t bIntrUsbValue);
void MGC_AfsTimerExpired(void);

/**************************** GLOBALS *****************************/

/** since AFS doesn't allow for instance data on ISRs or timer callbacks */
static MGC_AfsUds* MGC_apAfsUds[sizeof(MUSB_aNoneController) / sizeof(MUSB_NoneController)];

/** since AFS doesn't allow for instance data on ISRs or timer callbacks */
static uint8_t MGC_bAfsUdsCount = 0;

static uint8_t MGC_bBoardInitDone = FALSE;

#ifdef MUSB_PCI
static uint8_t MGC_bPciInitDone = FALSE;
#endif

#ifdef MUSB_MSG_BUF
unsigned int MGC_iMsgBufSize = 16*1024*1024;
char* MGC_pMsgBuf = NULL;
unsigned int MGC_iMsgBuf = 0;
unsigned int MGC_iWraps = 0;
#endif

/*************************** FUNCTIONS ****************************/

#ifdef MUSB_MSG_BUF
static void MGC_MsgChar(const char c)
{
    if((MGC_iMsgBuf + 3) >= MGC_iMsgBufSize)
    {
	MGC_iMsgBuf = 0;
	MGC_iWraps++;
    }
    MGC_pMsgBuf[MGC_iMsgBuf++] = c;
    MGC_pMsgBuf[MGC_iMsgBuf] = '>';
    MGC_pMsgBuf[MGC_iMsgBuf+1] = '>';
}

static void MGC_MsgString(const char* pString)
{
    size_t len = strlen(pString);
    if((MGC_iMsgBuf + len + 4) >= MGC_iMsgBufSize)
    {
	MGC_pMsgBuf[MGC_iMsgBuf] = 0;
	MGC_iMsgBuf = 0;
	MGC_iWraps++;
    }
    strcpy(&(MGC_pMsgBuf[MGC_iMsgBuf]), pString);
    MGC_iMsgBuf += len;
    MGC_pMsgBuf[MGC_iMsgBuf++] = '\r';
    MGC_pMsgBuf[MGC_iMsgBuf++] = '\n';
    MGC_pMsgBuf[MGC_iMsgBuf] = '>';
    MGC_pMsgBuf[MGC_iMsgBuf+1] = '>';
}
#endif

#if MUSB_DIAG >= 3

uint8_t MGC_NoneRead8(uint8_t* pBase, uint16_t wOffset)
{
    uint8_t bDatum = *(volatile uint8_t*)(pBase + wOffset);
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Read8(", wOffset, ") = ", bDatum, 16, 4);
#else
    printf("Read8(%lx, %04x) = %02x\n", pBase, wOffset, bDatum);
#endif
    return bDatum;
}

uint16_t MGC_NoneRead16(uint8_t* pBase, uint16_t wOffset)
{
    uint16_t wDatum = *(volatile uint16_t*)(pBase + wOffset);
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Read16(", wOffset, ") = ", wDatum, 16, 4);
#else
    printf("Read16(%lx, %04x) = %04x\n", pBase, wOffset, wDatum);
#endif
    return wDatum;
}

uint32_t MGC_NoneRead32(uint8_t* pBase, uint16_t wOffset)
{
    uint32_t dwDatum = *(volatile uint32_t*)(pBase + wOffset);
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Read32(", wOffset, ") = ", dwDatum, 16, 8);
#else
    printf("Read32(%lx, %04x) = %08x\n", pBase, wOffset, dwDatum);
#endif
    return dwDatum;
}

void MGC_NoneWrite8(uint8_t* pBase, uint16_t wOffset, uint8_t bDatum)
{
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Write8(", wOffset, ", ", bDatum, 16, 4);
#else
    printf("Write8(%lx, %04x, %02x)\n", pBase, wOffset, bDatum);
#endif
    *(volatile uint8_t*)(pBase + wOffset) = bDatum;
}

void MGC_NoneWrite16(uint8_t* pBase, uint16_t wOffset, uint16_t wDatum)
{
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Write16(", wOffset, ", ", wDatum, 16, 4);
#else
    printf("Write16(%lx, %04x, %04x)\n", pBase, wOffset, wDatum);
#endif
    *(volatile uint16_t*)(pBase + wOffset) = wDatum;
}

void MGC_NoneWrite32(uint8_t* pBase, uint16_t wOffset, uint32_t dwDatum)
{
#ifdef MUSB_MSG_BUF
    MUSB_DIAG2(3, "Write32(", wOffset, ", ", dwDatum, 16, 8);
#else
    printf("Write32(%lx, %04x, %08x)\n", pBase, wOffset, dwDatum);
#endif
    *(volatile uint32_t*)(pBase + wOffset) = dwDatum;
}

#endif	/* MUSB_DIAG >= 3 */

char MUSB_ReadConsole()
{
    char bData;
#if 0
    while(!uHALr_CharAvailable())
    {
	MUSB_NoneRunBackground();
    }

    bData = uHALr_getchar();
    /* Echo back the data entered by user */
#ifdef MUSB_MSG_BUF
    MGC_MsgChar(bData);
#else
    uHALr_putchar(bData);
#endif
    if('\r' == bData)
    {
#ifdef MUSB_MSG_BUF
	MGC_MsgChar('\n');
#else
	uHALr_putchar('\n');
#endif
    }
	#endif
	bData = 0;
    return bData;
}  
       
void MUSB_WriteConsole(const char bChar) 
{
#ifdef MUSB_MSG_BUF
    MGC_MsgChar(bChar);
#else
//    uHALr_putchar(bChar);
#endif
}

/* Reallocate memory */
void* MGC_AfsMemRealloc(void* pBlock, uint32_t iSize)
{
    /* no realloc */
    void* pNewBlock = MUSB_MemAlloc(iSize);
    if(pNewBlock)
    {
        MUSB_MemCopy(pNewBlock, pBlock, iSize);
        MUSB_MemFree(pBlock);
    }
    return (pNewBlock);
}

uint8_t MUSB_BoardMessageString(char* pMsg, uint16_t wBufSize, const char* pString)
{
    if((strlen(pMsg) + strlen(pString)) >= wBufSize)
    {
	return FALSE;
    }
    strcat(pMsg, pString);
    return TRUE;
}

uint8_t MUSB_BoardMessageNumber(char* pMsg, uint16_t wBufSize, uint32_t dwNumber, 
			      uint8_t bBase, uint8_t bJustification)
{
    char type;
    char format[8];
    char fmt[16];
    char number[32];

    switch(bBase)
    {
    case 8:
	type = 'i';
	break;
    case 10:
	type = 'd';
	break;
    case 16:
	type = 'x';
	break;
    default:
	return FALSE;
    }
    if(bJustification)
    {
	sprintf(format, "0%d%c", bJustification, type);
    }
    else
    {
	sprintf(format, "%c", type);
    }
    fmt[0] = '%';
    fmt[1] = (char)0;
    strcat(fmt, format);
    sprintf(number, fmt, dwNumber);

    return MUSB_BoardMessageString(pMsg, wBufSize, number);
}

uint32_t MUSB_BoardGetTime()
{
    return 0L;
}

void MGC_AfsUdsIsr(uint16_t wIntrTxValue,uint16_t wIntrRxValue,uint8_t bIntrUsbValue)
{
    uint8_t bIndex;
    MGC_AfsUds* pUds;
//	os_printf("count = %x\r\n",MGC_bAfsUdsCount);
    for(bIndex = 0; bIndex < MGC_bAfsUdsCount; bIndex++)
    {
		pUds = MGC_apAfsUds[bIndex];
		if(pUds)
		{
	//			MGC_NoneControllerIsr()
			pUds->pfNoneIsr(pUds->pNonePrivateData,wIntrTxValue,wIntrRxValue,bIntrUsbValue);
		    if(pUds->pPciAck)
		    {
				*((uint32_t*)pUds->pPciAck) = 3;
		    }
		}
    }
}

static void MGC_BoardInit()
{
    MUSB_MemSet(MGC_apAfsUds, 0, sizeof(MGC_apAfsUds));
#if 0
    /* init the MMU library */
    uHALr_InitMMU(IC_ON | DC_ON | WB_ON | EnableMMU);

    uHALr_ResetMMU();

    /* init the interrupt library */
    uHALr_InitInterrupts();

    /* init timers */
    uHALr_InitTimers();

    /* Initialize UART (default is 38400 baud for UART 0) */
    uHALr_ResetPort();

    uHALr_printf("\n\r\n\r MUSBStack-S v2.0 on ARM Integrator\n\r");
    uHALr_printf("Mentor Graphics Inventra Division \n\r");

#ifdef MUSB_MSG_BUF
    MGC_pMsgBuf = MUSB_MemAlloc(MGC_iMsgBufSize);
#endif
#endif
    MGC_bBoardInitDone = TRUE;

}

/*动态分配结构体MGC_AfsUds，并进行初始化
初始化isr,静态结构体指针数组MGC_apAfsUds[]指向此结构体
*/
void* MUSB_BoardInitController(void* pPrivateData, MUSB_NoneIsr pfIsr,
			       const MUSB_NoneController* pControllerInfo,
			       uint8_t** ppBaseIsr, uint8_t** ppBaseBsr)
{
   static MGC_AfsUds pUds;

    if(!MGC_bBoardInitDone)
    {
	MGC_BoardInit();
    }

//    pUds = (MGC_AfsUds*)MUSB_MemAlloc(sizeof(MGC_AfsUds));
//    if(!pUds)
//    {
	/* no memory */
//	os_printf("nomemeory1\r\n");
//	return NULL;
//    }
    MUSB_MemSet(&pUds, 0, sizeof(MGC_AfsUds));

    pUds.dwIrq = pControllerInfo->dwInfo;
    pUds.pNonePrivateData = pPrivateData;
    pUds.pfNoneIsr = pfIsr;

 //   uHALr_DisableInterrupt(pUds->dwIrq);

    /* assign the interrupt */
    strcpy(pUds.aIsrName, "MUSB-");
    pUds.aIsrName[5] = '0' + MGC_bAfsUdsCount;
    pUds.aIsrName[6] = (char)0;
//    uHALr_RequestInterrupt (pUds->dwIrq, MGC_AfsUdsIsr, (unsigned char *)pUds->aIsrName);

 //   uHALr_EnableInterrupt(pUds->dwIrq);

    pUds.bIndex = MGC_bAfsUdsCount;
    MGC_apAfsUds[MGC_bAfsUdsCount++] = &pUds;
    return &pUds;
}

uint8_t MUSB_BoardInitTimers(void* pPrivateData, uint16_t wTimerCount, 
			     const uint32_t* adwTimerResolutions)
{

    unsigned int iTimerCount;
    unsigned int iTimerIndex;
    int iIndex;
    unsigned int iTimerAvail = 0;
    MGC_AfsUds* pUds = (MGC_AfsUds*)pPrivateData;
	static MGC_AfsTimerWrapper Timerwrapper;
    iTimerCount = 1;//uHALir_CountTimers();
    if(iTimerCount < wTimerCount)
    {
		/* insufficient # timers */
		return FALSE;
    }
	pUds->aTimerWrapper = &Timerwrapper;
/*	os_printf("aTimerWrapper:wtimercount=%x\r\n",wTimerCount);
    pUds->aTimerWrapper = (MGC_AfsTimerWrapper*)MUSB_MemAlloc(wTimerCount * 
	sizeof(MGC_AfsTimerWrapper));
    if(!pUds->aTimerWrapper)
    {
		return FALSE;
    }*/
    /* ensure enough free timers */
    for(iTimerIndex = 0; 
	(iTimerAvail < wTimerCount) && (iTimerIndex < iTimerCount);iTimerIndex++)
    {
/*
Returns one of the following:
state        If the timer is found, the current state is one of:
T_FREE      Available.
T_ONESHOT   Single-shot timer (in use).
T_INTERVAL  Repeating timer (in use).
T_LOCKED    Not available for use by μHAL.
-1 If the timer is not found.
*/
	//	iTimerState = uHALr_GetTimerState(iTimerIndex);
	//	if(T_FREE == iTimerState)
		{
		    pUds->aTimerWrapper[iTimerAvail++].iIndex = iTimerIndex;
		}
    }
    if(iTimerAvail < wTimerCount)
    {
		/* insufficient good timers */
		MUSB_MemFree(pUds->aTimerWrapper);
		return FALSE;
    }
    /* allocate timers now */
    for(iTimerIndex = 0; iTimerIndex < wTimerCount; iTimerIndex++)
    {
 /*
 This function gets the next available timer and installs a handler. 
 On return, the timer is initialized but not running.
int uHALr_RequestTimer(PrHandler handler, const unsigned char *devname)

handler  :   Is a pointer to the routine that will process the interrupt.
devname  :   Is a pointer to a string identifying the function of the interrupt.
Return value
Returns one of the following:
timer     If successful, the timer number is returned.
-1        If the timer is unknown or already assigned.
 */

// 是否可以直接在TIMER中断中调用 MGC_AfsTimerExpired()函数???
		iIndex = 0;// uHALr_RequestTimer(MGC_AfsTimerExpired, (unsigned char*)"timer");
		if(iIndex >= 0)
		{
		    pUds->aTimerWrapper[iTimerIndex].iIndex = iIndex;

/*
This function allows the application to determine the correct interrupt for the specified
timer. Different target systems can assign different interrupts to the timer.

Return value
Returns one of the following:
interrupt     If the timer is found, the interrupt number is returned.
-1            If the timer is not found.
*/
		//    pUds->aTimerWrapper[iTimerIndex].iVector = uHALir_GetTimerInterrupt(iIndex);
		    pUds->aTimerWrapper[iTimerIndex].pfExpired = NULL;
/*This function disables the specified timer.*/
//		    uHALir_DisableTimer(iIndex);
			pUds->aTimerWrapper[iTimerIndex].bTimerStart = 0;
/*This function starts the specified timer by enabling the timer and the associated
interrupt.*/
//		    uHALr_InstallTimer(iIndex);
		}
		else
		{
		    /* TODO: back out */
		}
    }

    pUds->wTimerCount = wTimerCount;
    return TRUE;
}

void MUSB_BoardDestroyController(void* pPrivateData)
{
    MGC_AfsUds* pUds = (MGC_AfsUds*)pPrivateData;

 //   uHALr_FreeInterrupt(pUds->dwIrq);

    /* TODO: timers? */

    MGC_apAfsUds[pUds->bIndex] = NULL;
    MUSB_MemFree(pPrivateData);
}

void MUSB_BoardRunBackground(void* pPrivateData)
{
    /* nothing to do */
}

void MGC_AfsTimerExpired(void)
{

    uint16_t wIndex;
    volatile MGC_AfsUds* pUds;
    MGC_AfsTimerWrapper* pWrapper;

	wIndex = 0;
	pUds = MGC_apAfsUds[0];
	//os_printf("exp:%p\r\n", MGC_apAfsUds[0]);

	if(!pUds)
		{
//		os_printf("00000000  null\r\n");
		return;
		}
	pWrapper=&(pUds->aTimerWrapper[0]);
//	os_printf("++++ time=%x,bTimerStart=%d +++\r\n",pWrapper->dwTime,pWrapper->bTimerStart);
	if(pWrapper->bTimerStart)
	{

		if((--pWrapper->dwTime)==0)
		{
//			os_printf("-----timer expiered, function is called------\r\n");
			pWrapper->bTimerStart=0;//stop count
			pWrapper->pfExpired(pWrapper->pParam, wIndex);
//			ICU_config_module_isr(8,0);//disable interrupt
		}
	}

#if 0
    /* uHAL provides us only a vector number, so we must find the system and timer */
    for(bIndex = 0; bIndex < MGC_bAfsUdsCount; bIndex++)
    {
	pUds = MGC_apAfsUds[bIndex];
	if(pUds)
	{
	    for(wIndex = 0; wIndex < pUds->wTimerCount; wIndex++)
	    {
		pWrapper = &(pUds->aTimerWrapper[wIndex]);
		if((iVector == pWrapper->iVector) && pWrapper->pfExpired)
		{
		    if(pWrapper->dwTime)
		    {
				/* time remains, so continue */
				dwInterval = MUSB_MIN(pWrapper->dwTime, MAX_PERIOD);
				pWrapper->dwTime -= dwInterval;
				status = uHALr_SetTimerInterval(pWrapper->iIndex, dwInterval);
				if(status)
				{
				    MUSB_DIAG1(1, "SetTimerInterval returned ", status, 10, 0);
				}
				uHALr_EnableTimer(pWrapper->iIndex);
		    }
		    else
		    {
				/* if one-shot, disable since uHAL's one-shot does NOT work */
				if(!pWrapper->bPeriodic)
				{
				    uHALir_DisableTimer(pWrapper->iIndex);
				}
				/* callback */
				pWrapper->pfExpired(pWrapper->pParam, wIndex);
		    }
		    return;
		}
	    }
	}
    }
#endif
}

uint8_t MUSB_BoardArmTimer(void* pPrivateData, uint16_t wIndex, 
			     uint32_t dwTime, uint8_t bPeriodic, 
			     MUSB_pfTimerExpired pfExpireCallback,
			     void* pParam)
{


    MGC_AfsUds* pUds = (MGC_AfsUds*)pPrivateData;
    MGC_AfsTimerWrapper* pWrapper = &(pUds->aTimerWrapper[wIndex]);

//	os_printf("MUSB_BoardArmTimer\r\n");

	//intterupt disable
    pWrapper->pParam = pParam;
    pWrapper->pfExpired = pfExpireCallback;
    pWrapper->dwTime = dwTime;
    pWrapper->bPeriodic = bPeriodic;
	pWrapper->bTimerStart=1;
	// intterupt enable
    /*status = uHALr_SetTimerState(iIndex, pWrapper->bPeriodic ? T_INTERVAL : T_ONESHOT);*/
    /* T_ONESHOT does NOT work */

/*
This function sets the timer state.
Syntax
int uHALr_SetTimerState(unsigned int timer, enum uHALe_TimerState state)
where:
timer: Is the timer number for which the state is being set.
state: Is a valid timer state which is one of:
T_ONESHOT       Single-shot timer (in use).
T_INTERVAL       Repeating timer (in use).
Return value
Returns one of the following:
0 If the timer is found.
-1 If the timer is not found.
	*/
#if 0
    status = uHALr_SetTimerState(iIndex, T_INTERVAL);//T_INTERVAL表示repeating timer
    if(status)
    {
	MUSB_DIAG1(1, "SetTimerState returned ", status, 10, 0);
    }

    dwInterval = MUSB_MIN(pWrapper->dwTime, MAX_PERIOD);
    pWrapper->dwTime -= dwInterval;

/*
This function sets the interval, in microseconds, for the specified timer.
Syntax
int uHALr_SetTimerInterval(unsigned int timer, unsigned int interval)
where:
timer :Is the timer number for which the interval is to be set.
interval: Is the number of microseconds between events.
Return value
Returns one of the following:
0 If the timer is found.
-1 If the timer is not found
*/
    status = uHALr_SetTimerInterval(iIndex, dwInterval);
    if(status)
    {
	MUSB_DIAG1(1, "SetTimerInterval returned ", status, 10, 0);
    }
#endif
 //   uHALr_EnableTimer(iIndex);
	
    return TRUE;
}

uint8_t MUSB_BoardCancelTimer(void* pPrivate, uint16_t wIndex)
{
    MGC_AfsUds* pUds = (MGC_AfsUds*)pPrivate;
    MGC_AfsTimerWrapper* pWrapper = &(pUds->aTimerWrapper[wIndex]);
    uint32_t tmp, mask;

    VICMR_disable_interrupts(&tmp, &mask);
    //	cpu_set_interrupts_enabled(0);
    pWrapper->pfExpired = NULL;
    pWrapper->bTimerStart=0;
    //	cpu_set_interrupts_enabled(1);
    VICMR_restore_interrupts(tmp, mask);
    return TRUE;
}

/*
* Controller calls this to print a diagnostic message
*/
uint8_t MUSB_BoardPrintDiag(void* pPrivate, const char* pMessage)
{
    /*
    printf("%s\n", pMessage);
    */
#ifdef MUSB_MSG_BUF
    MGC_MsgString(pMessage);
#else
 //   uHALr_printf("%s\n", pMessage);
#endif
    return TRUE;
}

/*
* Controller calls this to get a bus address (for DMA) from a system address
*/
void* MUSB_BoardSystemToBusAddress(void* pPrivate, const void* pSysAddr)
{
#if 0
    MGC_AfsUds* pUds = (MGC_AfsUds*)pPrivate;

    if(pUds->pPciAck)
    {
	return (void*)((uint8_t*)pSysAddr + MUSB_PCIMASTER_OFFSET);
    }
    else
    {
	return (void*)((uint8_t*)pSysAddr + MUSB_AHBMASTER_OFFSET);
    }
#endif
    return NULL;
}



void *print_jmalloc(uint32_t isize)
{
	void *ptr;
	
	ptr = jmalloc(isize,0);
//	os_printf("******usb alloc:%p,%x******\r\n",ptr,isize);
	return ptr;
}
void print_jfree(void *addr)
{
//	os_printf("********usb jfree:%p********\r\n",addr);
	jfree(addr);
}
