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
 * Extra function-specific functionality.
 * $Revision: 1.5 $
 */

#include "mu_impl.h"
#include "mu_funpr.h"

MUSB_Pipe MUSB_BindFunctionEnd(MUSB_BusHandle hBus, 
			       const MUSB_EndpointDescriptor* pEndDesc)
{

	return NULL;
#if 0
    MGC_Pipe* pPipe;

    MUSB_DeviceEndpoint Endpoint;
    MGC_EndpointResource* pResource;
    MGC_Port* pPort = (MGC_Port*)hBus;

    pPipe = (MGC_Pipe*)MUSB_MemAlloc(sizeof(MGC_Pipe));
    if(pPipe)
    {
	MUSB_MemCopy((void*)&(Endpoint.UsbDescriptor), 
	    (void*)pEndDesc, sizeof(MUSB_EndpointDescriptor));
	pResource = pPort->pfBindEndpoint(pPort, &Endpoint, NULL, TRUE);
	if(pResource)
	{
	    MGC_FunctionPreparePipe(pPort, pPipe, 
		(MUSB_BusHandle)pPort, pResource, pEndDesc);
	}
	else
	{
	    MUSB_MemFree((void*)pPipe);
	    pPipe = NULL;
	}
    }

    return (MUSB_Pipe)pPipe;
#endif
}

void MUSB_CloseFunctionPipe(MUSB_Pipe hPipe)
{
#if 0
    MGC_Pipe* pPipe = (MGC_Pipe*)hPipe;
    MGC_Port* pPort = (MGC_Port*)pPipe->hSession;

    pPort->pfProgramFlushEndpoint(pPort, pPipe->pLocalEnd, 
	(pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? MUSB_DIR_IN : MUSB_DIR_OUT, FALSE);
    MUSB_MemFree((void*)pPipe);
#endif
}
