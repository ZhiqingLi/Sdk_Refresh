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
 * DMA implementation for AHB bridge for full-speed controllers.
 * $Revision: 1.12 $
 */

#include "mu_impl.h"

#include "mu_fadcf.h"

/****************************** CONSTANTS ********************************/

#define MGC_O_FADMA_BASE    0x200
#define MGC_O_FADMA_INTR    0x200

#define MGC_O_FADMA_CONTROL 4
#define MGC_O_FADMA_ADDRESS 8
#define MGC_O_FADMA_COUNT   0xc

#define MGC_FADMA_CHANNEL_OFFSET(_bChannel, _bOffset) (MGC_O_FADMA_BASE + (_bChannel << 4) + _bOffset)

/* control register (16-bit): */
#define MGC_S_FADMA_ENABLE	0
#define MGC_S_FADMA_TRANSMIT	1
#define MGC_S_FADMA_MODE1	2
#define MGC_S_FADMA_IRQENABLE	3
#define MGC_S_FADMA_ENDPOINT	4
#define MGC_S_FADMA_MAXSIZEDIV8	8
#define MGC_S_FADMA_BUSERROR	15

/******************************** TYPES **********************************/

#if defined(MUSB_DMA) && defined(MUSB_FADMA) && (MUSB_FADMA_CHANNELS > 0)

struct _MGC_FaDmaController;

typedef struct
{
    MUSB_DmaChannel Channel;
    struct _MGC_FaDmaController* pController;
    uint32_t dwStartAddress;
    uint32_t dwCount;
    uint16_t wMaxPacketSize;
    uint8_t bIndex;
    uint8_t bEnd;
    uint8_t bProtocol;
    uint8_t bTransmit;
    uint8_t bMode;
} MGC_FaDmaChannel;

typedef struct _MGC_FaDmaController
{
    MUSB_DmaController Controller;
    MGC_FaDmaChannel aChannel[MUSB_FADMA_CHANNELS];
    MUSB_pfSystemToBusAddress pfSystemToBusAddress;
    void* pSystemPrivate;
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged;
    void* pDmaPrivate;
    uint8_t* pCoreBaseIsr;
    uint8_t* pCoreBaseBsr;
    uint8_t bChannelCount;
    uint8_t bmUsedChannels;
} MGC_FaDmaController;

#endif	/* MUSB_FADMA && have at least one channel */

/******************************* FORWARDS ********************************/

#if defined(MUSB_DMA) && defined(MUSB_FADMA) && (MUSB_FADMA_CHANNELS > 0)
static uint8_t MGC_FaDmaStartController(void* pPrivateData);

static uint8_t MGC_FaDmaStopController(void* pPrivateData);

static MUSB_DmaChannel* MGC_FaDmaAllocateChannel(void* pPrivateData,
	uint8_t bLocalEnd, uint8_t bTransmit, uint8_t bProtocol, 
	uint16_t wMaxPacketSize);

static void MGC_FaDmaReleaseChannel(MUSB_DmaChannel* pChannel);

static uint8_t* MGC_FaDmaAllocateBuffer(MUSB_DmaChannel* pChannel,
	uint32_t dwLength);

static uint8_t MGC_FaDmaReleaseBuffer(MUSB_DmaChannel* pChannel,
	uint8_t* pBuffer);

static uint8_t MGC_FaDmaProgramChannel(MUSB_DmaChannel* pChannel, 
				       uint16_t wPacketSize, uint8_t bMode,
				       const uint8_t* pBuffer, uint32_t dwLength);

static MUSB_DmaChannelStatus MGC_FaDmaGetChannelStatus(
	MUSB_DmaChannel* pChannel);

static uint8_t MGC_FaDmaControllerIsr(void* pPrivateData);
#endif	/* MUSB_FADMA && have at least one channel */

static MUSB_DmaController* MGC_FaNewDmaController(
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged,
    void* pDmaPrivate,
    MUSB_pfSystemToBusAddress pfSystemToBusAddress, void* pSystemPrivate,
    uint8_t* pCoreBaseIsr, uint8_t* pCoreBaseBsr);

static void MGC_FaDestroyDmaController(MUSB_DmaController* pController);

/******************************* GLOBALS *********************************/

MUSB_DmaControllerFactory MUSB_FdrcDmaControllerFactory =
{
    0x300,
    MGC_FaNewDmaController,
    MGC_FaDestroyDmaController
};

/****************************** FUNCTIONS ********************************/

#if defined(MUSB_DMA) && defined(MUSB_FADMA) && (MUSB_FADMA_CHANNELS > 0)

static uint8_t MGC_FaDmaStartController(void* pPrivateData)
{
    /* nothing to do */
    return TRUE;
}

static uint8_t MGC_FaDmaStopController(void* pPrivateData)
{
    uint8_t bChannel;
    MGC_FaDmaChannel* pImplChannel;
    MGC_FaDmaController* pController = (MGC_FaDmaController*)pPrivateData;
    uint8_t* pBase = pController->pCoreBaseBsr;

    for(bChannel = 0; bChannel < MUSB_FADMA_CHANNELS; bChannel++)
    {
	MGC_Write16(pBase, 
	    MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_CONTROL), 0);
	pImplChannel = (MGC_FaDmaChannel*)&(pController->aChannel[bChannel]);
	pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
	pImplChannel->Channel.dwActualLength = 0;
    }
    pController->bmUsedChannels = 0;
    return TRUE;
}

static MUSB_DmaChannel* MGC_FaDmaAllocateChannel(void* pPrivateData,
	uint8_t bLocalEnd, uint8_t bTransmit, uint8_t bProtocol, 
	uint16_t wMaxPacketSize)
{
    uint8_t bBit;
    MUSB_DmaChannel* pChannel = NULL;
    MGC_FaDmaChannel* pImplChannel = NULL;
    MGC_FaDmaController* pController = (MGC_FaDmaController*)pPrivateData;

    if(bLocalEnd && (MUSB_ENDPOINT_XFER_BULK == bProtocol))
    {
	for(bBit = 0; bBit < MUSB_FADMA_CHANNELS; bBit++)
	{
	    if(!(pController->bmUsedChannels & (1 << bBit)))
	    {
		pController->bmUsedChannels |= (1 << bBit);
		pImplChannel = &(pController->aChannel[bBit]);
		pImplChannel->pController = pController;
		pImplChannel->wMaxPacketSize = wMaxPacketSize;
		pImplChannel->bIndex = bBit;
		pImplChannel->bEnd = bLocalEnd;
		pImplChannel->bProtocol = bProtocol;
		pImplChannel->bTransmit = bTransmit;
		pChannel = &(pImplChannel->Channel);
		pChannel->pPrivateData = pImplChannel;
		pChannel->bStatus = MUSB_DMA_STATUS_FREE;
		pChannel->dwMaxLength = bTransmit ? 0x10000 : wMaxPacketSize;
		/* Tx => mode 1; Rx => mode 0 */
		pChannel->bDesiredMode = bTransmit;
		break;
	    }
	}
    }
    return pChannel;
}

static void MGC_FaDmaReleaseChannel(MUSB_DmaChannel* pChannel)
{
    MGC_FaDmaChannel* pImplChannel = (MGC_FaDmaChannel*)pChannel->pPrivateData;
    MGC_FaDmaController* pController = pImplChannel->pController;
    uint8_t* pBase = pController->pCoreBaseBsr;

    MGC_Write16(pBase, 
	MGC_FADMA_CHANNEL_OFFSET(pImplChannel->bIndex, MGC_O_FADMA_CONTROL), 0);
    pImplChannel->pController->bmUsedChannels &= ~(1 << pImplChannel->bIndex);
    pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
}

static uint8_t* MGC_FaDmaAllocateBuffer(MUSB_DmaChannel* pChannel,
	uint32_t dwLength)
{
    /* TODO: this may require platform-specific implementation */
    return MUSB_MemAlloc(dwLength);
}

static uint8_t MGC_FaDmaReleaseBuffer(MUSB_DmaChannel* pChannel,
	uint8_t* pBuffer)
{
    /* TODO: this may require platform-specific implementation */
    MUSB_MemFree(pBuffer);
    return TRUE;
}

static uint8_t MGC_FaDmaProgramChannel(MUSB_DmaChannel* pChannel, 
				       uint16_t wPacketSize, uint8_t bMode,
				       const uint8_t* pBuffer, uint32_t dwLength)
{
    MGC_FaDmaChannel* pImplChannel = (MGC_FaDmaChannel*)pChannel->pPrivateData;
    MGC_FaDmaController* pController = pImplChannel->pController;
    uint8_t* pBase = bMode ? pController->pCoreBaseBsr : pController->pCoreBaseIsr;
    uint16_t wCsr = (pImplChannel->bEnd << MGC_S_FADMA_ENDPOINT) | (1 << MGC_S_FADMA_ENABLE);
    uint8_t bChannel = pImplChannel->bIndex;

    /* remember the core's mode */
    pImplChannel->bMode = bMode;

    if(bMode)
    {
	wCsr |= 1 << MGC_S_FADMA_MODE1;
	if(dwLength < wPacketSize)
	{
	    return FALSE;
	}
    }
    if(pImplChannel->bTransmit)
    {
	wCsr |= 1 << MGC_S_FADMA_TRANSMIT;
    }
    wCsr |= 1 << MGC_S_FADMA_IRQENABLE;
    wCsr |= (pImplChannel->wMaxPacketSize >> 3) << MGC_S_FADMA_MAXSIZEDIV8;

    /* address/count */
    MGC_Write32(pBase, 
	MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_ADDRESS), 
	(uint32_t)pController->pfSystemToBusAddress(pController->pSystemPrivate, pBuffer));
    MGC_Write32(pBase, 
	MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_COUNT), dwLength);

    /* control (this should start things) */
    pChannel->dwActualLength = 0L;
    pImplChannel->dwStartAddress = (uint32_t)pBuffer;
    pImplChannel->dwCount = dwLength;
    MGC_Write16(pBase, 
	MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_CONTROL), wCsr);

    return TRUE;
}

static MUSB_DmaChannelStatus MGC_FaDmaGetChannelStatus(
	MUSB_DmaChannel* pChannel)
{
    uint32_t dwCount;
    MGC_FaDmaChannel* pImplChannel = (MGC_FaDmaChannel*)pChannel->pPrivateData;
    MGC_FaDmaController* pController = pImplChannel->pController;
    uint8_t* pBase = pController->pCoreBaseIsr;
    uint8_t bChannel = pImplChannel->bIndex;
    uint16_t wCsr = MGC_Read16(pBase, 
	MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_CONTROL));

    if(wCsr & (1 << MGC_S_FADMA_BUSERROR))
    {
	return MUSB_DMA_STATUS_BUS_ABORT;
    }
    dwCount = MGC_Read32(pBase, MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_COUNT));
    pChannel->dwActualLength = pImplChannel->dwCount - dwCount;
    if(dwCount)
    {
	return MUSB_DMA_STATUS_BUSY;
    }
    return MUSB_DMA_STATUS_FREE;
}

static uint8_t MGC_FaDmaControllerIsr(void* pPrivateData)
{
    uint8_t bChannel;
    uint16_t wCsr;
    uint32_t dwCount;
    MGC_FaDmaChannel* pImplChannel;
    MGC_FaDmaController* pController = (MGC_FaDmaController*)pPrivateData;
    uint8_t* pBase = pController->pCoreBaseIsr;
    uint8_t bIntr = MGC_Read8(pBase, MGC_O_FADMA_INTR);

    if(!bIntr)
    {
    	return FALSE;
    }
    for(bChannel = 0; bChannel < MUSB_FADMA_CHANNELS; bChannel++)
    {
	if(bIntr & (1 << bChannel))
	{
	    pImplChannel = (MGC_FaDmaChannel*)&(pController->aChannel[bChannel]);
	    wCsr = MGC_Read16(pBase, 
		MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_CONTROL));
	    dwCount = MGC_Read32(pBase, MGC_FADMA_CHANNEL_OFFSET(bChannel, MGC_O_FADMA_COUNT));
	    pImplChannel->Channel.dwActualLength = pImplChannel->dwCount - dwCount;

	    if(wCsr & (1 << MGC_S_FADMA_BUSERROR))
	    {
		pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_BUS_ABORT;
	    }
	    else
	    {
		pImplChannel->Channel.bStatus = MUSB_DMA_STATUS_FREE;
		if(pImplChannel->bMode)
		{
		    pController->pfDmaChannelStatusChanged(pController->pDmaPrivate,
			pImplChannel->bEnd, pImplChannel->bTransmit);
		}
	    }
	}
    }
    return TRUE;
}

#endif	/* MUSB_FADMA && have at least one channel */

static MUSB_DmaController* MGC_FaNewDmaController(
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged,
    void* pDmaPrivate,
    MUSB_pfSystemToBusAddress pfSystemToBusAddress,
    void* pSystemPrivate,
    uint8_t* pCoreBaseIsr, uint8_t* pCoreBaseBsr)
{
    MUSB_DmaController* pResult = NULL;
#if defined(MUSB_DMA) && defined(MUSB_FADMA) && (MUSB_FADMA_CHANNELS > 0)
    MGC_FaDmaController* pController = (MGC_FaDmaController*)MUSB_MemAlloc(
    	sizeof(MGC_FaDmaController));
    if(pController)
    {
    	MUSB_MemSet(pController, 0, sizeof(MGC_FaDmaController));
    	pController->bChannelCount = MUSB_FADMA_CHANNELS;
    	pController->pfSystemToBusAddress = pfSystemToBusAddress;
	pController->pSystemPrivate = pSystemPrivate;
	pController->pfDmaChannelStatusChanged = pfDmaChannelStatusChanged;
	pController->pDmaPrivate = pDmaPrivate;
	pController->pCoreBaseIsr = pCoreBaseIsr;
	pController->pCoreBaseBsr = pCoreBaseBsr;
    	pController->Controller.pPrivateData = pController;
    	pController->Controller.pfDmaStartController = MGC_FaDmaStartController;
    	pController->Controller.pfDmaStopController = MGC_FaDmaStopController;
    	pController->Controller.pfDmaAllocateChannel = MGC_FaDmaAllocateChannel;
    	pController->Controller.pfDmaReleaseChannel = MGC_FaDmaReleaseChannel;
    	pController->Controller.pfDmaAllocateBuffer = MGC_FaDmaAllocateBuffer;
    	pController->Controller.pfDmaReleaseBuffer = MGC_FaDmaReleaseBuffer;
    	pController->Controller.pfDmaProgramChannel = MGC_FaDmaProgramChannel;
    	pController->Controller.pfDmaGetChannelStatus = MGC_FaDmaGetChannelStatus;
    	pController->Controller.pfDmaControllerIsr = MGC_FaDmaControllerIsr;
    	pResult = &(pController->Controller);
    }
#endif	/* MUSB_FADMA && have at least one channel */
    return pResult;
}

static void MGC_FaDestroyDmaController(MUSB_DmaController* pController)
{
#if defined(MUSB_DMA) && defined(MUSB_FADMA) && (MUSB_FADMA_CHANNELS > 0)
    MGC_FaDmaController* pFaController = 
    	(MGC_FaDmaController*)pController->pPrivateData;
    	
    if(pFaController)
    {
    	pFaController->Controller.pPrivateData = NULL;
    	MUSB_MemFree(pFaController);
    }
#endif	/* MUSB_FADMA && have at least one channel */
}

