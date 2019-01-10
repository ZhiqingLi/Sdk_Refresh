/***************************************************************************
 *
 * File:
 *     $Id: parseopts.h 1039 2009-03-13 22:10:37Z dliechty $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1039 $
 *
 * Description:
 *     This file the header file for the code which parses and exports the 
 *     command line options for the stack.
 *
 * Created:
 *     October 12, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __PARSEOPTS_H_
#define __PARSEOPTS_H_

#if BT_STACK == XA_ENABLED
#include "bttypes.h"

/* Table of known radio types */

#define RADIO_TYPE_DEFAULT           0
#define RADIO_TYPE_ERICSSON_UART     1
#define RADIO_TYPE_ERICSSON_H3W      2
#define RADIO_TYPE_SIW_UART          3
#define RADIO_TYPE_SIW_H3W           4
#define RADIO_TYPE_SIW_USB           5 /* Deprecated */ 
#define RADIO_TYPE_SIW_BCSP          6 /* Deprecated */ 
#define RADIO_TYPE_TI_UART           7
#define RADIO_TYPE_TI_H3W            8
#define RADIO_TYPE_INFINEON_UART     9
#define RADIO_TYPE_INFINEON_H3W     10
#define RADIO_TYPE_CSR_UART         11
#define RADIO_TYPE_CSR_H3W          12
#define RADIO_TYPE_CSR_USB          13 /* Deprecated */
#define RADIO_TYPE_CSR_IA_USB       13
#define RADIO_TYPE_CSR_BCSP         14
#define RADIO_TYPE_MOTOROLA_UART    15
#define RADIO_TYPE_MOTOROLA_H3W     16
#define RADIO_TYPE_ESI_INET         17
#define RADIO_TYPE_IA_INET          17
#define RADIO_TYPE_BROADCOM_UART    18
#define RADIO_TYPE_BROADCOM_H3W     19
#define RADIO_TYPE_BROADCOM_IA_USB  20
#define RADIO_TYPE_GENERIC_UART     21
#define RADIO_TYPE_GENERIC_H3W      22
#define RADIO_TYPE_ST_MICRO_UART    23
#define RADIO_TYPE_ST_MICRO_H3W     24
#define RADIO_TYPE_IA_USB           25
#define RADIO_TYPE_IFU_UART         26 /* IFU is another infineon radio */ 
#define RADIO_TYPE_IFU_H3W          27
#define RADIO_TYPE_ISSC_IA_USB      28
#define RADIO_TYPE_QUALCOMM_UART    29
#define RADIO_TYPE_QUALCOMM_H3W     30
#define RADIO_TYPE_QUALCOMM_IA_USB  31
#define RADIO_TYPE_ATHEROS_IA_USB   32
#define RADIO_TYPE_IA_USB2          33
#define RADIO_TYPE_CSR_IA_USB2      34
#define RADIO_TYPE_BROADCOM_IA_USB2 35
#define RADIO_TYPE_ISSC_IA_USB2     36
#define RADIO_TYPE_QUALCOMM_IA_USB2 37
#define RADIO_TYPE_ATHEROS_IA_USB2  38
#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/* Table of known IrDA adapter types*/
#define ESI_9680_7401_ADAPTER   1 /* Default */
#define NULL_ADAPTER            2    

/* Frame types */
#define FRM_TYPE_COM            1 /* Default */
#define FRM_TYPE_INET           2 

#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * Local types 
 */
#if BT_STACK == XA_ENABLED
typedef struct _UserOpts 
{
    U16 portNum;   /* User-selected port number */
    U16 speed;     /* UART Baud rate (115 or 57) */
    U16 radioType; /* Brand of radio */
    U16 tranType;  /* Transport type */
    BOOL startSniffer;  /* Start sniffer during initialization */

} UserOpts;
#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
typedef struct _IrUserOpts 
{
    U16 portNum;     /* User-selected port number */
    U16 adapterType; /* Brand of radio */
    U16 framerType;  /* Framer type to use */
} IrUserOpts;
#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * ParseStandardOptions()
 *
 *     Parses option pointed to by 'opt' parameter. If the parameter is
 * recognized, the 'opt' pointer is advanced past the option.
 */
BOOL ParseStandardOptions(char **opt);

#if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getPortNumOption(void);

/*---------------------------------------------------------------------------
 * getSpeedOption()
 *     Returns the user-selected bps rate.
 */
U16 getSpeedOption(void);

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the user-selected radio type.
 */
U16 getRadioType(void);

/*---------------------------------------------------------------------------
 * getSnifferOption()
 *     Returns the user-selected open sniffer at startup setting.
 */
BOOL getSnifferOption(void);

#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getIrPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getIrPortNumOption(void);

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the IrDA adapter type
 */
U16 getIrAdapterType(void);

/*---------------------------------------------------------------------------
 * getFramerType()
 *     Returns the framer type selected (FRM_TYPE_*)
 */
U16 getFramerType();

#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * getErrorMesage()
 *     Returns the user-selected bps rate.
 */
const char *getErrorMessage(void);

/*---------------------------------------------------------------------------
 * getStandardOptions()
 *     Returns a string which describes the standard options
 */
//const char *getStandardOptions();

#endif /* __PARSEOPTS_H_ */
