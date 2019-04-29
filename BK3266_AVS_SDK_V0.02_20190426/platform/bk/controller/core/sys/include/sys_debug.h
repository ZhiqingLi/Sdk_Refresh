#ifndef __PARTHUS_SYS_DEBUG__
#define __PARTHUS_SYS_DEBUG__

/***********************************************************************
 *
 * MODULE NAME:    sys_debug.h
 * PROJECT CODE:   BlueStream
 * MAINTAINER:     John Nelson
 * DATE:           Wed Jul 14 11:39:10 BST 1999
 *
 * SOURCE CONTROL: $Id $
 *
 * LICENCE:
 *    This source code is copyright (c) 1999-2004 Ceva Inc.
 *    All rights reserved.
 *
 * REVISION HISTORY:
 * 15 June 2001    All debug definitions separated from sys_features.h
 *
 * This file is to be used for debug selectors which must be off for
 * release.
 *
 ***********************************************************************/

/***********************************************************************
 * System Wide: Debug/Logging/Test Configurations
 **********************************************************************/

/*
 * Support endianess checks
 */
#ifndef LINUX
#define __LITTLE_ENDIAN 1234
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN    4321
#endif
#endif

/***********************************************************************
 * Debug Selector definitions (PRH_BS_DBG_<module> should be defined externally  
 **********************************************************************/


/***********************************************************************
 * LC Module: Debug/Logging
 **********************************************************************/
#ifndef PRH_BS_DBG_LC
#define PRH_BS_DBG_LC 1                  /* Default is no LC debug */
#endif

#define PRH_BS_DBG_LC_LSLC_ACCESS        (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_CLK           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_FREQ          (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_HOP           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_IRQ           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_PKT           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_LSLC_SLOT          (PRH_BS_DBG_LC && 0)

/*
 * Always log the statistics if in test mode tester
 */
#define PRH_BS_DBG_LC_LSLC_STAT          ((PRH_BS_DBG_LC && 1) ||\
        (PRH_BS_CFG_SYS_TEST_MODE_TESTER_SUPPORTED == 1))

#define PRH_BS_DBG_LC_USLC_CHAN_CTRL     (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_INQUIRY       (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_INQUIRYSCAN   (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_MSS           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_PAGE          (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_PAGESCAN      (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_PARK_MASTER   (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_PARK_SLAVE    (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_R2P           (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_SCHEDULER     (PRH_BS_DBG_LC && 0)
#define PRH_BS_DBG_LC_USLC_TESTMODE      (PRH_BS_DBG_LC && 0)

#define PRH_BS_DBG_LC_LOG                (PRH_BS_DBG_LC && 0)

/*
 * Packet Log Definitions
 */

#ifndef PRTH_BS_DBG_BB_PACKET_LOG_TYPE
#define PRTH_BS_DBG_BB_PACKET_LOG_TYPE       PACKETLOG_NONE
#endif

#ifndef PRTH_BS_DBG_MAX_BB_MAX_PACKETS_IN_LOG       
#define PRTH_BS_DBG_BB_PACKET_LOG_TYPE       PACKETLOG_NONE
#define PRTH_BS_DBG_MAX_BB_MAX_PACKETS_IN_LOG     0
#endif



/***********************************************************************
 * LMP Module: Debug/Logging
 **********************************************************************/
#ifndef PRH_BS_DBG_LMP
#define PRH_BS_DBG_LMP 0                 /* Default is no LMP debug */
#endif


/***********************************************************************
 * HCI Module: Debug/Logging
 **********************************************************************/
#ifndef PRH_BS_DBG_HCI
#define PRH_BS_DBG_HCI 0                 /* Default is no HCI debug */
#endif

#define PRH_BS_DBG_TRA_HCIT             (PRH_BS_DBG_HCI && 0)


/***********************************************************************
 * SYS Module: Debug/Logging
 **********************************************************************/
#ifndef PRH_BS_DBG_SYS
#define PRH_BS_DBG_SYS 0                  /* Default is no SYS debug */
#endif


#define NON_RF_LINK 0 /* RGB set to 1 if connecting stacks directly 
                         and using hardware emulation */

/***********************************************************************
 * BT Module: Debug/Logging
 **********************************************************************/
#ifndef PRH_BS_DBG_BT
#define PRH_BS_DBG_BT 0                  /* Default is no BT debug */
#endif

/***********************************************************************
 * General Debug Support
 **********************************************************************/
#ifndef NDEBUG
#include <assert.h>
#define SYSdebug_Assert(expr)    assert(expr)
#else
/*
 * Alternatively, can setup a local definition for test purposes
 */
#define SYSdebug_Assert(expr)
#endif

#define PRH_BS_DBG_PUMP_VERIFY_HARDWARE_BEFORE_TX_AFTER_RX  \
    (PRH_BS_DBG_BT && PRH_BS_CFG_SYS_DATA_PUMP_SUPPORTED && 0)

/*
 * Control a debug routine for eSCO where eSCO data is sourced
 * and sank internally in the host controller. When this
 * define is enabled, the host controller will always use
 * a test packet as source of eSCO data. It will also always
 * internally sink any eSCO data --> i.e. it will not forward
 * this eSCO data to the host.
 */
#define PRH_BS_DBG_SOURCE_SINK_SYN_TEST_DATA 0

//#define SYS_DEBUG_INJECT_SCO_BELOW_HCI
//#define SYS_DEBUG_INJECT_ESCO_BELOW_HCI
//#define SYS_DEBUG_STOP_SENDING_SCO_TO_HOST
//#define SYS_DEBUG_INSERT_HEC_ON_ESCO_TX 1


//#define SYS_DEBUG_IMITATE_CSR 1
#endif
