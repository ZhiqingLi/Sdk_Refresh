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

#ifndef _MUSB_TRANSPORT_FN_H_
#define _MUSB_TRANSPORT_FN_H_

/*
 * Header for companion peripheral to transport driver
 * $Revision: 1.6 $
 */

#include "mu_cdi.h"

/** Get a pointer to the singleton transport function client */
extern MUSB_FunctionClient* MGC_GetTransportFunctionClient(void);

#endif /** Protection against multiple inclusion */
