#ifndef _PARTHUS_HW_JAL_DEFS_
#define _PARTHUS_HW_JAL_DEFS_

/******************************************************************************
 * MODULE:          hw_jal_defs.h
 * PROJECT:         BlueStream
 * MAINTAINER:      John Nelson, Ivan Griffin
 * CREATION NAME:   7.July.1999
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * SOURCE CONTROL: $Id: hw_jal_defs.h,v 1.104 2016/03/29 12:59:06 vladimirf Exp $
 *
 * COMMENTS:
 *
 *     There will be the requirement to present two definitions based on
 *     the register/memory addressing approach adopted.
 *     Currently a 32 bit approach is adopted. The associated HW module
 *     is generic of the approach, but will depend on correct constants.
 *
 * REVISION HISTORY:
 *     7 July 1999    Jalapeno Revision 1.0, 30 June 1999
 *    30 July 1999      Jalapeno Revision 2.4, 14 July 1999
 *    07 Oct  1999      Jalapeno Revision (jal006v2.2)
 *    15 Nov  1999      Restructured as a result of habanero introduction.
 *     3 Mar  2000      Added definitions to support register caching
 ******************************************************************************/

/*
 * the error directive had a hash include in it -- some customer
 * complain at this !!
 */
#ifndef JAL_BASE_ADDR
#error Must include "sys_config.h" for BASE_ADDR of Jalapeno/Habanero 
#endif

#include "sys_features.h"

#if defined(TEAKLITE4) && !defined(TEAKLITE4_IO_BB_PORT)
#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs).
#define ADDR_DIV	2 //(CHAR_BIT/8)
#else
#define ADDR_DIV	1
#endif

#if (PRH_BS_CFG_SYS_LE_DUAL_MODE == 1)
/****************************************************************
*
*  Define the addresses and bit positions for the Transmit Control
*  Registers. The Addresss assume a 32 bit address space.
*  The 16 bit address bus mappings are taken care of by the macros in
*  the HW_lbl.h.
*
*****************************x***********************************/

#define JAL_BD_ADDR_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_BD_ADDR_MASK     0xFFFFFFFF
#define JAL_BD_ADDR_SHFT     0

#define JAL_UAP_LAP_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_NAP_ADDR         (0x00000004/ADDR_DIV + JAL_BASE_ADDR)

#define JAL_SYNC_ADDR        (0x00000008/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC_MASK        0xFFFFFFFF
#define JAL_SYNC_SHFT        0

#define JAL_SYNC2_ADDR       (0x0000000C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC2_MASK       0xFFFFFFFF
#define JAL_SYNC2_SHFT       0

#define JAL_INTRASLOT_OFFSET_ADDR        (0x00000010/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_INTRASLOT_OFFSET_MASK        0x000007FF
#define JAL_INTRASLOT_OFFSET_SHFT        0

#define JAL_BT_CLK_OFFSET_ADDR      (0x00000018/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_BT_CLK_OFFSET_MASK      0x0FFFFFFF
#define JAL_BT_CLK_OFFSET_SHFT      0

#define JAL_NATIVE_CLK_ADDR  (0x0000001c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_NATIVE_CLK_MASK  0x0FFFFFFF
#define JAL_NATIVE_CLK_SHFT  0

#define JAL_AM_ADDR_ADDR     (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AM_ADDR_MASK     0x00000007
#define JAL_AM_ADDR_SHFT     0

#define JAL_ENCRYPT_ADDR     (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ENCRYPT_MASK     0x00000018
#define JAL_ENCRYPT_SHFT     3

#define JAL_USE_LF_ADDR      (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_USE_LF_MASK      0x00000020
#define JAL_USE_LF_SHFT      5

#define JAL_SLAVE_ADDR       (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SLAVE_MASK       0x00000040
#define JAL_SLAVE_SHFT       6

#define JAL_PAGE_ADDR        (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PAGE_MASK        0x00000080
#define JAL_PAGE_SHFT        7

#define JAL_SCO_CFG_ADDR   (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_CFG_MASK   0x000FFF00
#define JAL_SCO_CFG_SHFT   8

#define JAL_SCO_CFG0_ADDR   (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_CFG0_MASK   0x00000F00
#define JAL_SCO_CFG0_SHFT   8

#define JAL_SCO_CFG1_ADDR   (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_CFG1_MASK   0x0000F000
#define JAL_SCO_CFG1_SHFT   12

#define JAL_SCO_CFG2_ADDR   (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_CFG2_MASK   0x000F0000
#define JAL_SCO_CFG2_SHFT   16

#define JAL_SCO_FIFO_ADDR    (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_FIFO_MASK    0x00300000
#define JAL_SCO_FIFO_SHFT    20

#define JAL_VCI_CLK_SEL_ADDR    (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_CLK_SEL_MASK    0x00c00000
#define JAL_VCI_CLK_SEL_SHFT    22

#define JAL_CRC_INIT_ADDR    (0x00000020/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_CRC_INIT_MASK    0xff000000
#define JAL_CRC_INIT_SHFT    24

#define JAL_LOOP_ADDR        (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_LOOP_MASK        0x00000001
#define JAL_LOOP_SHFT        0

#define JAL_TEST_ECO_ADDR    (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_ECO_MASK    0x00000002
#define JAL_TEST_ECO_SHFT    1

#define JAL_ECO_ERR_ADDR     (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ECO_ERR_MASK     0x0000003C
#define JAL_ECO_ERR_SHFT     2

#define JAL_TEST_CRC_ADDR    (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_CRC_MASK    0x00000040
#define JAL_TEST_CRC_SHFT    6

#define JAL_TEST_HEC_ADDR    (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_HEC_MASK    0x00000080
#define JAL_TEST_HEC_SHFT    7

#define JAL_TEST_RADIO_ADDR  (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_RADIO_MASK  0x00000100
#define JAL_TEST_RADIO_SHFT  8

#define JAL_VCI_CLK_SEL_MAP_ADDR     (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_CLK_SEL_MAP_MASK     0x00000200
#define JAL_VCI_CLK_SEL_MAP_SHFT     9

#define JAL_TEST_FORCE_NO_DDW_ADDR (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_FORCE_NO_DDW_MASK 0x00000400
#define JAL_TEST_FORCE_NO_DDW_SHFT 10

#define JAL_TEST_MSG_ADDR       (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_MSG_MASK       0x00000800
#define JAL_TEST_MSG_SHFT       11

#define JAL_TEST_TX_SHIFT_ADDR  (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TEST_TX_SHIFT_MASK  0x00007000
#define JAL_TEST_TX_SHIFT_SHFT  12

#define JAL_USE_HAB_CTRL_ADDR   (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_USE_HAB_CTRL_MASK   0x00008000
#define JAL_USE_HAB_CTRL_SHFT   15

/*
 * Setup Full DWH register to allow fast clearance of all DWH registers
 */
#define JAL_DWH_ALL_REG_ADDR      (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_DWH_ALL_REG_MASK      0xFFBF0000
#define JAL_DWH_ALL_REG_SHFT      16

#define JAL_DWH_INIT_ADDR    (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_DWH_INIT_MASK    0x003f0000
#define JAL_DWH_INIT_SHFT    16

/* 
 * RGB 22/08/01 - new bit added for switching between codec types 
 */
//#if defined(TERASIC)
#define JAL_CODEC_TYPE_ADDR    (0x0000003C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_CODEC_TYPE_MASK    0x00000007
#define JAL_CODEC_TYPE_SHFT    0
//#else
//#define JAL_CODEC_TYPE_ADDR    (0x00000024 + JAL_BASE_ADDR)
//#define JAL_CODEC_TYPE_MASK    0x00400000
//#define JAL_CODEC_TYPE_SHFT    22
//#endif

#define JAL_DWH_BY_BT_CLK_ADDR    (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_DWH_BY_BT_CLK_MASK    0x00800000
#define JAL_DWH_BY_BT_CLK_SHFT    23

#define JAL_DWH2_INIT_ADDR   (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_DWH2_INIT_MASK   0x3f000000
#define JAL_DWH2_INIT_SHFT   24

#define JAL_WHITEN_ADDR      (0x00000024/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_WHITEN_MASK      0x80000000
#define JAL_WHITEN_SHFT      31

/*
 * Interrupt related
 */
#define JAL_TIM_INTR_MSK_ADDR        (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM_INTR_MSK_MASK        0x0000000f
#define JAL_TIM_INTR_MSK_SHFT        0

#define JAL_TIM0_INTR_MSK_ADDR       (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_MSK_MASK       0x00000001
#define JAL_TIM0_INTR_MSK_SHFT       0

#define JAL_TIM1_INTR_MSK_ADDR       (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_MSK_MASK       0x00000002
#define JAL_TIM1_INTR_MSK_SHFT       1

#define JAL_TIM2_INTR_MSK_ADDR       (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_MSK_MASK       0x00000004
#define JAL_TIM2_INTR_MSK_SHFT       2

#define JAL_TIM3_INTR_MSK_ADDR       (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_MSK_MASK       0x00000008
#define JAL_TIM3_INTR_MSK_SHFT       3

#define JAL_PKD_INTR_MSK_ADDR        (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_INTR_MSK_MASK        0x00000010
#define JAL_PKD_INTR_MSK_SHFT        4

#define JAL_AUX_TIM_INTR_MSK_ADDR    (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_MSK_MASK    0x00000020
#define JAL_AUX_TIM_INTR_MSK_SHFT    5

#define JAL_PKA_INTR_MSK_ADDR        (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKA_INTR_MSK_MASK        0x00000040
#define JAL_PKA_INTR_MSK_SHFT        6

#define JAL_PKD_RX_HDR_INTR_MSK_ADDR (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_MSK_MASK 0x00000080
#define JAL_PKD_RX_HDR_INTR_MSK_SHFT 7

#define JAL_AES_INTR_MSK_ADDR		(0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AES_INTR_MSK_MASK		 0x00000200
#define JAL_AES_INTR_MSK_SHFT        9

#define JAL_TIM_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM_INTR_CLR_MASK   0x000f0000
#define JAL_TIM_INTR_CLR_SHFT   16

#define JAL_TIM0_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_CLR_MASK   0x00010000
#define JAL_TIM0_INTR_CLR_SHFT   16

#define JAL_TIM1_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_CLR_MASK   0x00020000
#define JAL_TIM1_INTR_CLR_SHFT   17

#define JAL_TIM2_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_CLR_MASK   0x00040000
#define JAL_TIM2_INTR_CLR_SHFT   18

#define JAL_TIM3_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_CLR_MASK   0x00080000
#define JAL_TIM3_INTR_CLR_SHFT   19

#define JAL_PKD_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_INTR_CLR_MASK   0x00100000
#define JAL_PKD_INTR_CLR_SHFT   20

#define JAL_AUX_TIM_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_CLR_MASK   0x00200000
#define JAL_AUX_TIM_INTR_CLR_SHFT   21

#define JAL_PKA_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKA_INTR_CLR_MASK   0x00400000
#define JAL_PKA_INTR_CLR_SHFT   22

#define JAL_AES_INTR_CLR_ADDR		(0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AES_INTR_CLR_MASK		 0x10000000
#define JAL_AES_INTR_CLR_SHFT        28
#define JAL_AES_INTR_ADDR (0x0000002C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AES_INTR_SHFT 8
#define JAL_AES_INTR_MASK 0x00000100

#define JAL_SPI_NOW_CONFLICT_ADDR (0x0000002C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SPI_NOW_CONFLICT_SHFT 9
#define JAL_SPI_NOW_CONFLICT_MASK 0x00000200

#define JAL_NO_PKT_RCVD_INTR_ADDR    (0x0000002C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_SHFT    13
#define JAL_NO_PKT_RCVD_INTR_MASK    0x00002000

#define JAL_SYNC_DET_INTR_ADDR    (0x0000002C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_SHFT    14
#define JAL_SYNC_DET_INTR_MASK    0x00004000

#define JAL_SLEEP_STATUS_ADDR    (0x0000002C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SLEEP_STATUS_SHFT    15
#define JAL_SLEEP_STATUS_MASK    0x00008000

#define JAL_NO_PKT_RCVD_INTR_MSK_ADDR (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_MSK_SHFT 13
#define JAL_NO_PKT_RCVD_INTR_MSK_MASK 0x00002000

#define JAL_SYNC_DET_INTR_MSK_ADDR (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_MSK_SHFT 14
#define JAL_SYNC_DET_INTR_MSK_MASK 0x00004000

#define JAL_NO_PKT_RCVD_INTR_CLR_ADDR  (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_CLR_SHFT  29 
#define JAL_NO_PKT_RCVD_INTR_CLR_MASK  0x20000000

#define JAL_SPI_NOW_CONFLICT_CLR_ADDR  (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SPI_NOW_CONFLICT_CLR_SHFT  25
#define JAL_SPI_NOW_CONFLICT_CLR_MASK  0x02000000

#define JAL_SER0_WR_CLR_ADDR  (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER0_WR_CLR_SHFT  27
#define JAL_SER0_WR_CLR_MASK  0x08000000

#define JAL_SYNC_DET_INTR_CLR_ADDR  (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_CLR_SHFT  30 
#define JAL_SYNC_DET_INTR_CLR_MASK  0x40000000

#define JAL_SYNC_DIR_ADDR  (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYNC_DIR_SHFT  31 
#define JAL_SYNC_DIR_MASK  0x80000000


#define JAL_PKD_RX_HDR_INTR_CLR_ADDR   (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_CLR_MASK   0x00800000
#define JAL_PKD_RX_HDR_INTR_CLR_SHFT   23

#define JAL_PG_TIMEOUT_INTR_CLR_ADDR (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PG_TIMEOUT_INTR_CLR_SHFT 0x20000000
#define JAL_PG_TIMEOUT_INTR_CLR_MASK 29

/****************************************************************
*  Define the bit positions for the Common Status Registers
****************************************************************/

#define JAL_TIM_INTR_ADDR         (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM_INTR_MASK         0x0000000f
#define JAL_TIM_INTR_SHFT         0

#define JAL_TIM0_INTR_ADDR        (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_MASK        0x00000001
#define JAL_TIM0_INTR_SHFT        0

#define JAL_TIM1_INTR_ADDR        (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_MASK        0x00000002
#define JAL_TIM1_INTR_SHFT        1

#define JAL_TIM2_INTR_ADDR        (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_MASK        0x00000004
#define JAL_TIM2_INTR_SHFT        2

#define JAL_TIM3_INTR_ADDR        (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_MASK        0x00000008
#define JAL_TIM3_INTR_SHFT        3

#define JAL_PKD_INTR_ADDR         (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_INTR_MASK         0x00000010
#define JAL_PKD_INTR_SHFT         4

#define JAL_AUX_TIM_INTR_ADDR     (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_MASK     0x00000020
#define JAL_AUX_TIM_INTR_SHFT     5

#define JAL_PKA_INTR_ADDR         (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKA_INTR_MASK         0x00000040
#define JAL_PKA_INTR_SHFT         6

#define JAL_PKD_RX_HDR_INTR_ADDR  (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_MASK  0x00000080
#define JAL_PKD_RX_HDR_INTR_SHFT  7

#define JAL_PG_TIMEOUT_INTR_ADDR  (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PG_TIMEOUT_INTR_MASK  0x00002000
#define JAL_PG_TIMEOUT_INTR_SHFT  13

#define JAL_SLEEP_ADDR        (0x0000002c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SLEEP_MASK        0x00008000
#define JAL_SLEEP_SHFT        15

#define JAL_NAT_BIT_CNT_ADDR        (0x00000038/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_NAT_BIT_CNT_MASK        0x000007FF
#define JAL_NAT_BIT_CNT_SHFT        0


/*************************************
 * TX Control Registers
 ************************************/
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_TX_LEN_ADDR      (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_LEN_MASK      0x000003ff
#define JAL_TX_LEN_SHFT      0

#define JAL_TX_TYPE_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_TYPE_MASK     0x00003c00
#define JAL_TX_TYPE_SHFT     10

#define JAL_TX_FLOW_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_FLOW_MASK     0x00004000
#define JAL_TX_FLOW_SHFT     14

#define JAL_TX_ARQN_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_ARQN_MASK     0x00008000
#define JAL_TX_ARQN_SHFT     15

#define JAL_TX_SEQN_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_SEQN_MASK     0x00010000
#define JAL_TX_SEQN_SHFT     16

#define JAL_TX_MODE_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_MODE_MASK     0x000C0000
#define JAL_TX_MODE_SHFT     18

#define JAL_TX_L_CH_ADDR     (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_L_CH_MASK     0x00300000
#define JAL_TX_L_CH_SHFT     20

#define JAL_TX_P_FLOW_ADDR   (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_P_FLOW_MASK   0x00400000
#define JAL_TX_P_FLOW_SHFT   22

/* Obsolete: point to non used register address*/
#define JAL_TX_BUF_ADDR      (0x00000710/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_BUF_MASK      0x00800000
#define JAL_TX_BUF_SHFT      23

#define JAL_TRANSMIT_ADDR    (0x00000030/ADDR_DIV + JAL_BASE_ADDR)  /* Obsolete */
#define JAL_TRANSMIT_MASK    0x00020000
#define JAL_TRANSMIT_SHFT    17

#define JAL_TX_HALF_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)  /* Obsolete */
#define JAL_TX_HALF_MASK     0x00040000
#define JAL_TX_HALF_SHFT     18

#else
/* Non EDR */
#define JAL_TX_LEN_ADDR      (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_LEN_MASK      0x000001ff
#define JAL_TX_LEN_SHFT      0

#define JAL_TX_TYPE_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_TYPE_MASK     0x00001e00
#define JAL_TX_TYPE_SHFT     9

#define JAL_TX_FLOW_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_FLOW_MASK     0x00002000
#define JAL_TX_FLOW_SHFT     13

#define JAL_TX_ARQN_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_ARQN_MASK     0x00004000
#define JAL_TX_ARQN_SHFT     14

#define JAL_TX_SEQN_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_SEQN_MASK     0x00008000
#define JAL_TX_SEQN_SHFT     15

#define JAL_TX_BUF_ADDR      (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_BUF_MASK      0x00010000
#define JAL_TX_BUF_SHFT      16

#define JAL_TX_MODE_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_MODE_MASK     0x00060000
#define JAL_TX_MODE_SHFT     17

#define JAL_TRANSMIT_ADDR    (0x00000030/ADDR_DIV + JAL_BASE_ADDR)  /* Obsolete */
#define JAL_TRANSMIT_MASK    0x00020000
#define JAL_TRANSMIT_SHFT    17

#define JAL_TX_HALF_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)  /* Obsolete */
#define JAL_TX_HALF_MASK     0x00040000
#define JAL_TX_HALF_SHFT     18

#define JAL_TX_L_CH_ADDR     (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_L_CH_MASK     0x00180000
#define JAL_TX_L_CH_SHFT     19

#define JAL_TX_P_FLOW_ADDR   (0x00000030/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_P_FLOW_MASK   0x00200000
#define JAL_TX_P_FLOW_SHFT   21
#endif /* #if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1) */

/*************************************
 * eSCO Control Registers
 ************************************/
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_ESCO_TX_LEN_ADDR   (0x00000720/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_TX_LEN_MASK   0x000003FF
#define JAL_ESCO_TX_LEN_SHFT   0

#define JAL_ESCO_RX_LEN_ADDR   (0x00000720/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_RX_LEN_MASK   0x03FF0000
#define JAL_ESCO_RX_LEN_SHFT   16

#define JAL_ESCO_LT_ADDR_ADDR   (0x00000720/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_LT_ADDR_MASK   0x00001c00
#define JAL_ESCO_LT_ADDR_SHFT   10

#define JAL_SCO_ROUTE_ADDR      (0x00000720/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_ROUTE_MASK      0x00002000
#define JAL_SCO_ROUTE_SHFT      13

#else
/* Non EDR */
#define JAL_ESCO_TX_LEN_ADDR   (0x00000034/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_TX_LEN_MASK   0x000000FF
#define JAL_ESCO_TX_LEN_SHFT   0

#define JAL_ESCO_RX_LEN_ADDR   (0x00000034/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_RX_LEN_MASK   0x0000FF00
#define JAL_ESCO_RX_LEN_SHFT   8

#define JAL_ESCO_LT_ADDR_ADDR   (0x00000034/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ESCO_LT_ADDR_MASK   0x00070000
#define JAL_ESCO_LT_ADDR_SHFT   16

#define JAL_SCO_ROUTE_ADDR      (0x00000034/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_ROUTE_MASK      0x00080000
#define JAL_SCO_ROUTE_SHFT      19
#endif /* #if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1) */

/****************************************************************
*
*  Define the bit positions for the Transmit Status Registers
*
****************************************************************/
#define JAL_TX0_OVER_ADDR   (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX0_OVER_MASK   0x00000001
#define JAL_TX0_OVER_SHFT   0

#define JAL_TX0_UNDER_ADDR  (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX0_UNDER_MASK  0x00000002
#define JAL_TX0_UNDER_SHFT  1

#define JAL_TX1_OVER_ADDR   (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX1_OVER_MASK   0x00000004
#define JAL_TX1_OVER_SHFT   2


#define JAL_TX1_UNDER_ADDR  (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX1_UNDER_MASK  0x00000008
#define JAL_TX1_UNDER_SHFT  3

#define JAL_TX2_OVER_ADDR   (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX2_OVER_MASK   0x00000010
#define JAL_TX2_OVER_SHFT   4

#define JAL_TX2_UNDER_ADDR  (0x00000040/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX2_UNDER_MASK  0x00000020
#define JAL_TX2_UNDER_SHFT  5



/****************************************************************
*
*  Define the bit positions for the Receive Control Rgeisters
*
****************************************************************/

#define JAL_RX_MODE_ADDR    (0x00000048/ADDR_DIV + JAL_BASE_ADDR)   /* Combination of RX_EN and FULL_WIN */
#define JAL_RX_MODE_MASK    0x00000003
#define JAL_RX_MODE_SHFT    0

#define JAL_RX_BUF_ADDR     (0x00000048/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_BUF_MASK     0x00000100
#define JAL_RX_BUF_SHFT     8

#define JAL_ERR_SEL_ADDR    (0x00000048/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ERR_SEL_MASK    0x00000e00
#define JAL_ERR_SEL_SHFT    9

#define JAL_WIN_EXT_ADDR    (0x00000048/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_WIN_EXT_MASK    0x003ff000
#define JAL_WIN_EXT_SHFT    12

#define JAL_FREEZE_BIT_CNT_ADDR    (0x00000048/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_FREEZE_BIT_CNT_MASK    0x00400000
#define JAL_FREEZE_BIT_CNT_SHFT    22

#define JAL_ABORT_ON_WRONG_AM_ADDR_ADDR (0x00000048/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ABORT_ON_WRONG_AM_ADDR_MASK 0x00800000
#define JAL_ABORT_ON_WRONG_AM_ADDR_SHFT 23

/****************************************************************
*
*  Define the bit positions for the Receive Status Rgeisters
*
****************************************************************/
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_RX_LEN_ADDR    (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_LEN_MASK    0x000003ff
#define JAL_RX_LEN_SHFT    0

#define JAL_RX_TYPE_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_TYPE_MASK   0x00003c00
#define JAL_RX_TYPE_SHFT   10

#define JAL_RX_FLOW_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_FLOW_MASK   0x00004000
#define JAL_RX_FLOW_SHFT   14

#define JAL_RX_ARQN_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_ARQN_MASK   0x00008000
#define JAL_RX_ARQN_SHFT   15

#define JAL_RX_SEQN_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_SEQN_MASK   0x00010000
#define JAL_RX_SEQN_SHFT   16

#define JAL_RX_AM_ADDR_ADDR (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_AM_ADDR_MASK 0x000e0000
#define JAL_RX_AM_ADDR_SHFT 17

#define JAL_RX_L_CH_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_L_CH_MASK   0x00300000
#define JAL_RX_L_CH_SHFT   20

#define JAL_RX_P_FLOW_ADDR (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_P_FLOW_MASK 0x00400000
#define JAL_RX_P_FLOW_SHFT 22

#define JAL_RX_PKT_ADDR    (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_PKT_MASK    0x01000000
#define JAL_RX_PKT_SHFT    24

#define JAL_HEC_ERR_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_HEC_ERR_MASK   0x02000000
#define JAL_HEC_ERR_SHFT   25

#define JAL_CRC_ERR_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_CRC_ERR_MASK   0x04000000
#define JAL_CRC_ERR_SHFT   26

#define JAL_RX_HDR_ADDR    (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_HDR_MASK    0x08000000
#define JAL_RX_HDR_SHFT    27

#define JAL_AM_ADDR_ABORT_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AM_ADDR_ABORT_MASK   0x20000000
#define JAL_AM_ADDR_ABORT_SHFT   29

#define JAL_EDR_SYNC_ERR_ADDR   (0x00000730/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_SYNC_ERR_MASK   0x40000000
#define JAL_EDR_SYNC_ERR_SHFT   30
#define JAL_RX_STATUS_GP_REG_JAL_EDR_SYNC_ERR_MASK     JAL_EDR_SYNC_ERR_MASK

#else

#define JAL_RX_LEN_ADDR    (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_LEN_MASK    0x000001ff
#define JAL_RX_LEN_SHFT    0

#define JAL_RX_TYPE_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_TYPE_MASK   0x00001e00
#define JAL_RX_TYPE_SHFT   9

#define JAL_RX_FLOW_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_FLOW_MASK   0x00002000
#define JAL_RX_FLOW_SHFT   13

#define JAL_RX_ARQN_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_ARQN_MASK   0x00004000
#define JAL_RX_ARQN_SHFT   14

#define JAL_RX_SEQN_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_SEQN_MASK   0x00008000
#define JAL_RX_SEQN_SHFT   15

#define JAL_RX_AM_ADDR_ADDR (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_AM_ADDR_MASK 0x00070000
#define JAL_RX_AM_ADDR_SHFT 16

#define JAL_RX_L_CH_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_L_CH_MASK   0x00180000
#define JAL_RX_L_CH_SHFT   19

#define JAL_RX_P_FLOW_ADDR (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_P_FLOW_MASK 0x00200000
#define JAL_RX_P_FLOW_SHFT 21

#define JAL_RX_PKT_ADDR    (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_PKT_MASK    0x01000000
#define JAL_RX_PKT_SHFT    24

#define JAL_HEC_ERR_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_HEC_ERR_MASK   0x02000000
#define JAL_HEC_ERR_SHFT   25

#define JAL_CRC_ERR_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_CRC_ERR_MASK   0x04000000
#define JAL_CRC_ERR_SHFT   26

#define JAL_RX_HDR_ADDR    (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_HDR_MASK    0x08000000
#define JAL_RX_HDR_SHFT    27

#define JAL_AM_ADDR_ABORT_ADDR   (0x00000050/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AM_ADDR_ABORT_MASK   0x20000000
#define JAL_AM_ADDR_ABORT_SHFT   29
#endif  /* #if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1) */

/*
 * PTA Control Registers
 */
#define JAL_PTA_GRANT_RX_DENIED_ADDR   (0x00000734/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_RX_DENIED_MASK   0x00000001
#define JAL_PTA_GRANT_RX_DENIED_SHFT   0

#define JAL_PTA_GRANT_TX_DENIED_ADDR   (0x00000734/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_TX_DENIED_MASK   0x00000002
#define JAL_PTA_GRANT_TX_DENIED_SHFT   1

#define JAL_PTA_MODE_ENABLE_ADDR   (0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_MODE_ENABLE_MASK   0x00000001
#define JAL_PTA_MODE_ENABLE_SHFT   0

#define JAL_PTA_GRANT_TEST_ENABLE_ADDR   (0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_TEST_ENABLE_MASK   0x00000002
#define JAL_PTA_GRANT_TEST_ENABLE_SHFT   1

#define JAL_PTA_TX_ACTIVE_GIO_SEL_ADDR   (0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_TX_ACTIVE_GIO_SEL_MASK   0x0000003C
#define JAL_PTA_TX_ACTIVE_GIO_SEL_SHFT   2

#define JAL_PTA_GRANT_INVERSE_ADDR		   (0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_INVERSE_MASK			0x00000040
#define JAL_PTA_GRANT_INVERSE_SHFT			6

#define JAL_PTA_GRANT_RX_OVERRIDE_ADDR	   (0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_GRANT_RX_OVERRIDE_MASK		0x00000080
#define JAL_PTA_GRANT_RX_OVERRIDE_SHFT		7

#define JAL_PTA_BPKTCTL_GRANT_ADDR			(0x00000738/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PTA_BPKTCTL_GRANT_MASK			0x00000100
#define JAL_PTA_BPKTCTL_GRANT_SHFT			8

#define JAL_RX0_OVER_ADDR  (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX0_OVER_MASK  0x00000001
#define JAL_RX0_OVER_SHFT  0

#define JAL_RX0_UNDER_ADDR (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX0_UNDER_MASK 0x00000002
#define JAL_RX0_UNDER_SHFT 1

#define JAL_RX1_OVER_ADDR  (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX1_OVER_MASK  0x00000004
#define JAL_RX1_OVER_SHFT  2

#define JAL_RX1_UNDER_ADDR (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX1_UNDER_MASK 0x00000008
#define JAL_RX1_UNDER_SHFT 3

#define JAL_RX2_OVER_ADDR  (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX2_OVER_MASK  0x00000010
#define JAL_RX2_OVER_SHFT  4

#define JAL_RX2_UNDER_ADDR (0x00000054/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX2_UNDER_MASK 0x00000020
#define JAL_RX2_UNDER_SHFT 5

#define JAL_ERR_CNT_ADDR   (0x0000005C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ERR_CNT_MASK   0x0000FFFF
#define JAL_ERR_CNT_SHFT   0

#if (PRH_BS_CFG_SYS_HW_WINDOW_WIDENING_SUPPORTED==1)

/***************************************************************************
 *
 * Enable Masks WIN_EXT SER Early/Late Control
 *
 ***************************************************************************/

#define JAL_SER_TIME_EARLY_ADDR         (0x00000068/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_TIME_EARLY_MASK          0x000f0000
#define JAL_SER_TIME_EARLY_SHFT          16

#define JAL_SER_TIME_LATE_ADDR         (0x00000068/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_TIME_LATE_MASK          0x00f00000
#define JAL_SER_TIME_LATE_SHFT          20

#endif

/****************************************************************
*
*  Define the bit positions for the Parallel Interface Adaptor
*
****************************************************************/
#define JAL_P_DATA_0_ADDR      (0x000000A4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_DATA_0_MASK      0x000000FF
#define JAL_P_DATA_0_SHFT      0

#define JAL_P_DATA_1_ADDR      (0x000000A4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_DATA_1_MASK      0x0000FF00
#define JAL_P_DATA_1_SHFT      8

#define JAL_P_DATA_2_ADDR      (0x000000A4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_DATA_2_MASK      0x00FF0000
#define JAL_P_DATA_2_SHFT      16

#define JAL_P_DATA_3_ADDR      (0x000000A4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_DATA_3_MASK      0xFF000000
#define JAL_P_DATA_3_SHFT      24

#define JAL_P_SETUP_ADDR      (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_SETUP_MASK       0x000000FF
#define JAL_P_SETUP_SHFT       0

#define JAL_P_MASK_ADDR        (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_MASK_MASK        0x00000F00
#define JAL_P_MASK_SHFT        8

#define JAL_P_NOW_ADDR         (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_NOW_MASK         0x00001000
#define JAL_P_NOW_SHFT         12

#define JAL_P_SEQ_ADDR         (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_SEQ_MASK         0x00002000
#define JAL_P_SEQ_SHFT         13

#define JAL_P_DATA_DIR_ADDR    (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_DATA_DIR_MASK    0x00004000
#define JAL_P_DATA_DIR_SHFT    14

#define JAL_P_RDATA_ADDR       (0x000000A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_P_RDATA_MASK       0x007F0000
#define JAL_P_RDATA_SHFT       16

/****************************************************************
*
*  Define the bit positions for the Serial Interface Registers
*
****************************************************************/
#define JAL_SER_CFG_ADDR        (0x00000060/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_CFG_MASK        0x7FFFFFFF
#define JAL_SER_CFG_SHFT        0

#define JAL_SER_BUSY_ADDR       (0x00000060/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_BUSY_MASK       0x80000000
#define JAL_SER_BUSY_SHFT       31

#define JAL_SER_TIME_ADDR       (0x00000064/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_TIME_MASK       0x000000FF
#define JAL_SER_TIME_SHFT       0

#define JAL_SER_COMBINE_ADDR    (0x00000064/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_COMBINE_MASK    0x00000700
#define JAL_SER_COMBINE_SHFT    8

#define JAL_SER_MASK_ADDR       (0x00000064/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_MASK_MASK       0x000F0000
#define JAL_SER_MASK_SHFT       16

#define JAL_SER_NOW_ADDR        (0x00000064/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_NOW_MASK        0x00100000
#define JAL_SER_NOW_SHFT        20

#define JAL_SER_SEQ_ADDR        (0x00000064/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_SEQ_MASK        0x00200000
#define JAL_SER_SEQ_SHFT        21

#define JAL_SER_READ_DATA_ADDR  (0x0000006c/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_READ_DATA_MASK  0xFFFFFFFF
#define JAL_SER_READ_DATA_SHFT  0

/* ----------------------------charles modify,DualMode use only--------------------------------- */
#define BEKEN_msb_fst_vif_vsr_ADDR      (0x00000070/ADDR_DIV + JAL_BASE_ADDR)/* 0x00F76070 */
#define BEKEN_msb_fst_vif_vsr_MASK      0x00000002
#define BEKEN_msb_fst_vif_vsr_SHFT      1

#define BEKEN_long_sync_len_ADDR        (0x00000070/ADDR_DIV + JAL_BASE_ADDR)/* 0x00F76070 */
#define BEKEN_long_sync_len_MASK        0x0000001C
#define BEKEN_long_sync_len_SHFT        2

#define BEKEN_data_len_vif_ADDR         (0x00000070/ADDR_DIV + JAL_BASE_ADDR)/* 0x00F76070 */
#define BEKEN_data_len_vif_MASK         0x000001E0
#define BEKEN_data_len_vif_SHFT         5

#define BEKEN_0x181C		         (*((volatile unsigned long *)   (JAL_BASE_ADDR+0x70)))

#define BEKEN_xver_soft_reset_ADDR         (0x00000070/ADDR_DIV + JAL_BASE_ADDR)/* 0x00F76070 */
#define BEKEN_xver_soft_reset_MASK         0x80000000
#define BEKEN_xver_soft_reset_SHFT         31
/* ----------------------------charles modify,DualMode use only--------------------------------- */


#define JAL_SER_DATA0_ADDR      (0x00000070/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_DATA0_MASK      0xFFFFFFFF
#define JAL_SER_DATA0_SHFT      0

#define JAL_SER_DATA1_ADDR      (0x00000074/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_DATA1_MASK      0xFFFFFFFF
#define JAL_SER_DATA1_SHFT      0

#define JAL_SER_DATA2_ADDR      (0x00000078/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_DATA2_MASK      0xFFFFFFFF
#define JAL_SER_DATA2_SHFT      0

#define JAL_SER_DATA3_ADDR      (0x0000007C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER_DATA3_MASK      0xFFFFFFFF
#define JAL_SER_DATA3_SHFT      0

#define JAL_SER0_WRITE_CLR_ADDR       (0x00000028/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SER0_WRITE_CLR_MASK       0x08000000
#define JAL_SER0_WRITE_CLR_SHFT       27

/*******************************************************************
 *
 *     Define the bit positions for the auxilliary timer
 *     registers
 *
 *******************************************************************/
#define JAL_AUX_TIMER_ADDR     (0x000000A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_AUX_TIMER_MASK     0x00003FFF
#define JAL_AUX_TIMER_SHFT     0


/*******************************************************************
*
*      Define the bit positions for the rx_delay and tx_delay
*      control registers
*
*******************************************************************/
#define JAL_RX_DELAY_ADDR      (0x000000b0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RX_DELAY_MASK      0x0000007f
#define JAL_RX_DELAY_SHFT      0

#define JAL_TX_DELAY_ADDR      (0x000000b0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TX_DELAY_MASK      0x00000f00
#define JAL_TX_DELAY_SHFT      8
/*******************************************************************
*
*      Define the bit positions for the enc_key_len and enc_key
*      (16 bytes at C0-CF) control registers
*
*******************************************************************/
#define JAL_ENC_KEY_LEN_ADDR      (0x000000b8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ENC_KEY_LEN_MASK      0x0000000f
#define JAL_ENC_KEY_LEN_SHFT      0

#define JAL_ENC_KEY_ADDR          (0x000000C0/ADDR_DIV + JAL_BASE_ADDR)

/*******************************************************************
 *
 *   Define the bit positions for the hop selection engine
 *
 *******************************************************************/
#define JAL_SYS_ADDR           (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SYS_MASK           0x00000001
#define JAL_SYS_SHFT           0

#define JAL_H_PAGE_ADDR        (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_H_PAGE_MASK        0x00000002
#define JAL_H_PAGE_SHFT        1

#define JAL_H_SLAVE_ADDR       (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_H_SLAVE_MASK       0x00000004
#define JAL_H_SLAVE_SHFT       2

#define JAL_PAGE_OFFSET_ADDR   (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_PAGE_OFFSET_MASK   0x00000008
#define JAL_PAGE_OFFSET_SHFT   3

#define JAL_INDEX_MAP_ADDR     (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_INDEX_MAP_MASK     0x00000010
#define JAL_INDEX_MAP_SHFT     4

#define JAL_CLK_DELTA_ADDR     (0x000000D0/ADDR_DIV  + JAL_BASE_ADDR)
#define JAL_CLK_DELTA_MASK     0x000000E0
#define JAL_CLK_DELTA_SHFT     5

#define JAL_N_COUNT_ADDR       (0x000000D0/ADDR_DIV  + JAL_BASE_ADDR)
#define JAL_N_COUNT_MASK       0x00001F00
#define JAL_N_COUNT_SHFT       8

#define JAL_SEL_ADDR           (0x000000D0/ADDR_DIV  + JAL_BASE_ADDR)
#define JAL_SEL_MASK           0x00006000
#define JAL_SEL_SHFT           13

#define JAL_SUP_BT_CLK_ADDR    (0x000000D0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SUP_BT_CLK_MASK    0xFFFF8000
#define JAL_SUP_BT_CLK_SHFT    15

#define JAL_HOP_ADDR           (0x000000D4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_HOP_MASK           0x0000007F
#define JAL_HOP_SHFT           0


/*******************************************************************
 *
 *   Define the bit positions for the BT Clock Controls(P6 only)
 *   (the clock gating signals are redundant)
 *******************************************************************/
#define JAL_ADD_BT_CLK_RELATIVE_ADDR     (0x000000F0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_ADD_BT_CLK_RELATIVE_MASK     0000000001
#define JAL_ADD_BT_CLK_RELATIVE_SHFT     0

#define JAL_WRITE_ABSOLUTE_BT_CLK_ADDR     (0x000000F0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_WRITE_ABSOLUTE_BT_CLK_MASK     0x00000002
#define JAL_WRITE_ABSOLUTE_BT_CLK_SHFT     1

#define JAL_DELAYED_BT_CLK_UPDATE_ADDR     (0x000000F0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_DELAYED_BT_CLK_UPDATE_MASK     0x00000004
#define JAL_DELAYED_BT_CLK_UPDATE_SHFT     2

#define JAL_FREEZE_BT_CLK_ADDR     (0x000000F0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_FREEZE_BT_CLK_MASK     0x00000008
#define JAL_FREEZE_BT_CLK_SHFT     3

/*******************************************************************
 *
 *   Define the bit positions for the LC Revision Code Register
 *
 *******************************************************************/
#define JAL_MINOR_REVISION_ADDR    (0x000000F8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_MINOR_REVISION_MASK    0x000000FF
#define JAL_MINOR_REVISION_SHFT    0

#define JAL_MAJOR_REVISION_ADDR    (0x000000F8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_MAJOR_REVISION_MASK    0x0000FF00
#define JAL_MAJOR_REVISION_SHFT    8

/*******************************************************************
*
*      Define the bit positions for the reset controls
*
********************************************************************/
#define JAL_RST_CODE_ADDR     (0x000000FC/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_RST_CODE_MASK     0xFF000000
#define JAL_RST_CODE_SHFT     24

#if 1 // NEW_TRANSCODE_CONTROL
/*******************************************************************
********************************************************************
*      New Registers for Codec Control - eSCO retransmissions
********************************************************************
********************************************************************/

/*******************************************************************
	Address		Name									Access	
	0x110		HPF_FILT[15:0]							R/W	
	0x110+4		PF1_FILT_A[31:0]						R/W	
	0x110+8		PF1_FILT_B[31:0]						R/W	
	0x110+c		PF2_FILT[31:0]							R/W	
	0x110+10	PF3_FILT[31:0]							R/W	
	0x110+14	CVSD_FILT_A[25:0]						R/W	
	0x110+18	[LD_RX[26],CVSD_FILT_B[25:0]			R/W	


********************************************************************/

#define JAL_TRANSCODE_START_RX_STATE_HFP_FILT_ADDR     (0x00000110/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_HFP_FILT_MASK     0x0000FFFF
#define JAL_TRANSCODE_START_RX_STATE_HFP_FILT_SHFT     0

#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_A_ADDR     (0x00000114/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_A_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_A_SHFT     0

#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_B_ADDR     (0x00000118/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_B_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_START_RX_STATE_PF1_FILT_B_SHFT     0

#define JAL_TRANSCODE_START_RX_STATE_PF2_FILT_ADDR       (0x0000011C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_PF2_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_START_RX_STATE_PF2_FILT_SHFT       0

#define JAL_TRANSCODE_START_RX_STATE_PF3_FILT_ADDR       (0x00000120/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_PF3_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_START_RX_STATE_PF3_FILT_SHFT       0

#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_A_ADDR       (0x00000124/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_A_MASK       0x03FFFFFF
#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_A_SHFT       0

#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_B_ADDR       (0x00000128/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_B_MASK       0x0FFFFFFF
#define JAL_TRANSCODE_START_RX_STATE_CVSD_FILT_B_SHFT       0

/*******************************************************************
	Address		Name									Access	
	0x130		HPF_FILT[15:0]							R/W	
	0x130+4		PF1_FILT_A[31:0]						R/W	
	0x130+8		PF1_FILT_B[31:0]						R/W	
	0x130+c		PF2_FILT[31:0]							R/W	
	0x130+10	PF3_FILT[31:0]							R/W	
	0x130+14	CVSD_FILT_A[25:0]						R/W	
	0x130+18	[LD_TX[26],CVSD_FILT_B[25:0]			R/W	


********************************************************************/

#define JAL_TRANSCODE_START_TX_STATE_HFP_FILT_ADDR     (0x00000130/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_HFP_FILT_MASK     0x0000FFFF
#define JAL_TRANSCODE_START_TX_STATE_HFP_FILT_SHFT     0

#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_A_ADDR     (0x00000134/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_A_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_A_SHFT     0

#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_B_ADDR     (0x00000138/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_B_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_START_TX_STATE_PF1_FILT_B_SHFT     0

#define JAL_TRANSCODE_START_TX_STATE_PF2_FILT_ADDR       (0x0000013C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_PF2_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_START_TX_STATE_PF2_FILT_SHFT       0

#define JAL_TRANSCODE_START_TX_STATE_PF3_FILT_ADDR       (0x00000140/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_PF3_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_START_TX_STATE_PF3_FILT_SHFT       0

#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_A_ADDR       (0x00000144/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_A_MASK       0x03FFFFFF
#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_A_SHFT       0

#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_B_ADDR       (0x00000148/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_B_MASK       0x0FFFFFFF
#define JAL_TRANSCODE_START_TX_STATE_CVSD_FILT_B_SHFT       0


/*******************************************************************
	Address		Name									Access	
	0x150		TX_HPF_FILT[15:0]						R
	0x150+4		TX_PF1_FILT_A[31:0]						R
	0x150+8		TX_PF1_FILT_B[31:0]						R
	0x150+c		TX_PF2_FILT[31:0]						R
	0x150+10	TX_PF3_FILT[31:0]						R
	0x150+14	TX_CVSD_FILT_A[25:0]					R
	0x150+18	TX_CVSD_FILT_B[25:0]					R

	Tx registers are read at the end of the tx slot
	Read access only
********************************************************************/


#define JAL_TRANSCODE_END_TX_STATE_HFP_FILT_ADDR     (0x00000150/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_HFP_FILT_MASK     0x0000FFFF
#define JAL_TRANSCODE_END_TX_STATE_HFP_FILT_SHFT     0

#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_A_ADDR     (0x00000154/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_A_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_A_SHFT     0

#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_B_ADDR     (0x00000158/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_B_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_END_TX_STATE_PF1_FILT_B_SHFT     0

#define JAL_TRANSCODE_END_TX_STATE_PF2_FILT_ADDR       (0x0000015C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_PF2_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_END_TX_STATE_PF2_FILT_SHFT       0

#define JAL_TRANSCODE_END_TX_STATE_PF3_FILT_ADDR       (0x00000160/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_PF3_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_END_TX_STATE_PF3_FILT_SHFT       0

#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_A_ADDR       (0x00000164/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_A_MASK       0x03FFFFFF
#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_A_SHFT       0

#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_B_ADDR       (0x00000168/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_B_MASK       0x03FFFFFF
#define JAL_TRANSCODE_END_TX_STATE_CVSD_FILT_B_SHFT       0


/*******************************************************************
	Address		Name									Access	
	0x170		RX_HPF_FILT[15:0]						R
	0x170+4		RX_PF1_FILT_A[31:0]						R
	0x170+8		RX_PF1_FILT_B[31:0]						R
	0x170+c		RX_PF2_FILT[31:0]						R
	0x170+10	RX_PF3_FILT[31:0]						R
	0x170+14	RX_CVSD_FILT_A[25:0]					R
	0x170+18	RX_CVSD_FILT_B[25:0]					R

	Rx registers are read at the end of the rx slot
	Read access only

********************************************************************/


#define JAL_TRANSCODE_END_RX_STATE_HFP_FILT_ADDR     (0x00000170/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_HFP_FILT_MASK     0x0000FFFF
#define JAL_TRANSCODE_END_RX_STATE_HFP_FILT_SHFT     0

#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_A_ADDR     (0x00000174/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_A_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_A_SHFT     0

#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_B_ADDR     (0x00000178/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_B_MASK     0xFFFFFFFF
#define JAL_TRANSCODE_END_RX_STATE_PF1_FILT_B_SHFT     0

#define JAL_TRANSCODE_END_RX_STATE_PF2_FILT_ADDR       (0x0000017C/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_PF2_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_END_RX_STATE_PF2_FILT_SHFT       0

#define JAL_TRANSCODE_END_RX_STATE_PF3_FILT_ADDR       (0x00000180/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_PF3_FILT_MASK       0xFFFFFFFF
#define JAL_TRANSCODE_END_RX_STATE_PF3_FILT_SHFT       0

#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_A_ADDR       (0x00000184/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_A_MASK       0x03FFFFFF
#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_A_SHFT       0

#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_B_ADDR       (0x00000188/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_B_MASK       0x03FFFFFF
#define JAL_TRANSCODE_END_RX_STATE_CVSD_FILT_B_SHFT       0

#endif

/****************************************************************
*
* Register definitions for FIFO interface to CODEC (eSCO via VCI)
*
****************************************************************/

#define JAL_VCI_TX_FIFO_FILL_LEVEL_ADDR						(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_TX_FIFO_FILL_LEVEL_MASK						0xFC000000
#define JAL_VCI_TX_FIFO_FILL_LEVEL_SHFT						26

#define JAL_VCI_RX_FIFO_FILL_LEVEL_ADDR						(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_RX_FIFO_FILL_LEVEL_MASK						0x03F00000
#define JAL_VCI_RX_FIFO_FILL_LEVEL_SHFT						20

#define JAL_VCI_TX_FIFO_FILL_THRESHOLD_ADDR					(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_TX_FIFO_FILL_THRESHOLD_MASK					0x000FC000
#define JAL_VCI_TX_FIFO_FILL_THRESHOLD_SHFT					14

#define JAL_VCI_RX_FIFO_FILL_THRESHOLD_ADDR					(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_RX_FIFO_FILL_THRESHOLD_MASK					0x00003F00
#define JAL_VCI_RX_FIFO_FILL_THRESHOLD_SHFT					8

#define JAL_VCI_VTFH_TX_LEVEL_STATUS_ADDR					(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_VTFH_TX_LEVEL_STATUS_MASK                   0x00000010
#define JAL_VCI_VTFH_TX_LEVEL_STATUS_SHFT					4

#define JAL_VCI_VRFL_RX_LEVEL_STATUS_ADDR					(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_VRFL_RX_LEVEL_STATUS_MASK					0x00000008
#define JAL_VCI_VRFL_RX_LEVEL_STATUS_SHFT					3

#define JAL_VCI_VFIR_FIFO_RESET_ADDR						(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_VFIR_FIFO_RESET_MASK						0x00000004
#define JAL_VCI_VFIR_FIFO_RESET_SHFT						2

#define JAL_VCI_VFI16_FIFO_16BIT_MODE_ADDR					(0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_VFI16_FIFO_16BIT_MODE_MASK					0x00000002
#define JAL_VCI_VFI16_FIFO_16BIT_MODE_SHFT					1

#define JAL_VCI_VFIM_FIFO_RGF_MODE_ADDR					    (0x000001A0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_VFIM_FIFO_RGF_MODE_MASK						0x00000001
#define JAL_VCI_VFIM_FIFO_RGF_MODE_SHFT						0

#define JAL_VCI_RD_FIFO_DATA_ADDR                           (0x000001A4/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_RD_FIFO_DATA_MASK                           0xFFFFFFFF
#define JAL_VCI_RD_FIFO_DATA_SHFT                           0

#define JAL_VCI_WR_FIFO_DATA_ADDR                           (0x000001A8/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_VCI_WR_FIFO_DATA_MASK                           0xFFFFFFFF
#define JAL_VCI_WR_FIFO_DATA_SHFT                           0

/****************************************************************
*
*  Define the Transmit and Receive Registers
*
****************************************************************/
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_TX_ACL_BUF_OFFSET   (0x0000740/ADDR_DIV)
#define JAL_RX_ACL_BUF_OFFSET   (0x0000B80/ADDR_DIV)
#define JAL_ACL_BUF_LEN   1022
#else
#define JAL_TX_ACL_BUF_OFFSET   (0x0000200/ADDR_DIV)
#define JAL_RX_ACL_BUF_OFFSET   (0x0000400/ADDR_DIV)
#define JAL_ACL_BUF_LEN   340
#endif /* #if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1) */

/****************************************************************
*
*  Define the addresses of the main registers and associated
*  fields to allow local caching of registers and checking of
*  field assignments to take place
*  To use a local copy of a register
*  1. Give it a unique meaningful name REGISTER_XXX.
*  2. #define REGISTER_XXX_FIELD FIELD for each FIELD that will be
*     used with this register
*  3
*
****************************************************************/
/*
 * BD_ADDR Register        Low/High Address 00H/04H
 */
#define JAL_COM_CTRL_BD_ADDR_LO_REG_ADDR             (JAL_BASE_ADDR+0x00)
#define JAL_COM_CTRL_BD_ADDR_HI_REG_ADDR             (JAL_BASE_ADDR+0x04/ADDR_DIV)

/*
 * Clock Registers         BT_CLK[_OFFSET]  18H/ NATIVE_CLK 1CH
 */
#define JAL_COM_CTRL_BT_CLK_READ_REG_ADDR           (JAL_BASE_ADDR+0x18/ADDR_DIV)
#define JAL_COM_CTRL_BT_CLK_OFFSET_WRITE_REG_ADDR   (JAL_BASE_ADDR+0x18/ADDR_DIV)
#define JAL_COM_CTRL_NATIVE_CLK_REG_ADDR            (JAL_BASE_ADDR+0x1C/ADDR_DIV)

/*
 * Common Control Syncword Low/High Address 08H/12H
 */
#define JAL_COM_CTRL_SYNC_LO_REG_ADDR             (JAL_BASE_ADDR+0x08/ADDR_DIV)
#define JAL_COM_CTRL_SYNC_HI_REG_ADDR             (JAL_BASE_ADDR+0x0C/ADDR_DIV)

/*
 * Common Control General Purpose Register 1  Address 20H
 */
#define JAL_COM_CTRL_GP1_REG_ADDR                 (JAL_BASE_ADDR+0x20/ADDR_DIV)

#define JAL_COM_CTRL_GP1_REG_JAL_PAGE_MASK        JAL_PAGE_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_SLAVE_MASK       JAL_SLAVE_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_USE_LF_MASK      JAL_USE_LF_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_ENCRYPT_MASK     JAL_ENCRYPT_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_AM_ADDR_MASK     JAL_AM_ADDR_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_SCO_CFG_MASK    JAL_SCO_CFG_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_SCO_CFG0_MASK   JAL_SCO_CFG0_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_SCO_CFG1_MASK   JAL_SCO_CFG1_MASK
#define JAL_COM_CTRL_GP1_REG_JAL_SCO_CFG2_MASK   JAL_SCO_CFG2_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_SCO_FIFO_MASK   JAL_SCO_FIFO_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_VCI_CLK_SEL_MASK   JAL_VCI_CLK_SEL_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_CRC_INIT_MASK    JAL_CRC_INIT_MASK

#define JAL_COM_CTRL_GP1_REG_JAL_CLK1M_DIR_MASK JAL_CLK1M_DIR_MASK

/*
 * Common Control General Purpose Register 2  Address 24H
 */
#define JAL_COM_CTRL_GP2_REG_ADDR                      (JAL_BASE_ADDR+0x24/ADDR_DIV)

/*
 * All whitening registers grouped in one u_int32 control word
 */

#define JAL_COM_CTRL_GP2_REG_JAL_TEST_HEC_MASK          JAL_TEST_HEC_MASK

#define JAL_COM_CTRL_GP2_REG_JAL_WHITEN_MASK          JAL_WHITEN_MASK
#define JAL_COM_CTRL_GP2_REG_JAL_DWH_INIT_MASK        JAL_DWH_INIT_MASK
#define JAL_COM_CTRL_GP2_REG_JAL_DWH2_INIT_MASK       JAL_DWH2_INIT_MASK
#define JAL_COM_CTRL_GP2_REG_JAL_DWH_BY_BT_CLK_MASK   JAL_DWH_BY_BT_CLK_MASK
#define JAL_COM_CTRL_GP2_REG_JAL_DWH_ALL_REG_MASK     JAL_DWH_ALL_REG_MASK
#define JAL_COM_CTRL_GP2_REG_JAL_VCI_CLK_SEL_MAP_MASK JAL_VCI_CLK_SEL_MAP_MASK

/*
 * Common Control Interrupt Register          Address 28H
 */
#define JAL_COM_CTRL_IRQ_REG_ADDR                     (JAL_BASE_ADDR+0x28/ADDR_DIV)

#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_RX_HDR_INTR_MSK_MASK   JAL_PKD_RX_HDR_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_INTR_MSK_MASK          JAL_PKD_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKA_INTR_MSK_MASK          JAL_PKA_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM_INTR_MSK_MASK          JAL_TIM_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM0_INTR_MSK_MASK         JAL_TIM0_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM1_INTR_MSK_MASK         JAL_TIM1_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM2_INTR_MSK_MASK         JAL_TIM2_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM3_INTR_MSK_MASK         JAL_TIM3_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_NO_PKT_RCVD_INTR_MSK_MASK  JAL_NO_PKT_RCVD_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_SYNC_DET_INTR_MSK_MASK     JAL_SYNC_DET_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AUX_TIM_INTR_MSK_MASK       JAL_AUX_TIM_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AES_INTR_MSK_MASK           JAL_AES_INTR_MSK_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_RX_HDR_INTR_CLR_MASK    JAL_PKD_RX_HDR_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_INTR_CLR_MASK           JAL_PKD_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKA_INTR_CLR_MASK           JAL_PKA_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM0_INTR_CLR_MASK          JAL_TIM0_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM1_INTR_CLR_MASK          JAL_TIM1_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM2_INTR_CLR_MASK          JAL_TIM2_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM3_INTR_CLR_MASK          JAL_TIM3_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_NO_PKT_RCVD_INTR_CLR_MASK   JAL_NO_PKT_RCVD_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_SYNC_DET_INTR_CLR_MASK      JAL_SYNC_DET_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AUX_TIM_INTR_CLR_MASK       JAL_AUX_TIM_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_SER0_WRITE_CLR_MASK         JAL_SER0_WRITE_CLR_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_AES_INTR_CLR_MASK           JAL_AES_INTR_CLR_MASK
/*
 * Common Control Interrupt Register          Address 2CH
 */
#define JAL_COM_STAT_IRQ_REG_ADDR                     (JAL_BASE_ADDR+0x2C/ADDR_DIV)

#define JAL_COM_STAT_IRQ_REG_JAL_PKD_RX_HDR_INTR_MASK      JAL_PKD_RX_HDR_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_PKD_INTR_MASK             JAL_PKD_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_PKA_INTR_MASK             JAL_PKA_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM0_INTR_MASK            JAL_TIM0_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM1_INTR_MASK            JAL_TIM1_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM2_INTR_MASK            JAL_TIM2_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM3_INTR_MASK            JAL_TIM3_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_NO_PKT_RCVD_INTR_MASK     JAL_NO_PKT_RCVD_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_SYNC_DET_INTR_MASK        JAL_SYNC_DET_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_SLEEP_STATUS_MASK         JAL_SLEEP_STATUS_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_AUX_TIM_INTR_MASK         JAL_AUX_TIM_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_AES_INTR_MASK             JAL_AES_INTR_MASK

/*
 * Transmit Control Register                  Address 30H
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_TX_CTRL_REG_ADDR                      (JAL_BASE_ADDR+0x0710/ADDR_DIV)
#else
#define JAL_TX_CTRL_REG_ADDR                      (JAL_BASE_ADDR+0x30/ADDR_DIV)
#endif

#define JAL_TX_CTRL_REG_JAL_TX_LEN_MASK            JAL_TX_LEN_MASK
#define JAL_TX_CTRL_REG_JAL_TX_TYPE_MASK           JAL_TX_TYPE_MASK

#define JAL_TX_CTRL_REG_JAL_TX_FLOW_MASK           JAL_TX_FLOW_MASK
#define JAL_TX_CTRL_REG_JAL_TX_ARQN_MASK           JAL_TX_ARQN_MASK
#define JAL_TX_CTRL_REG_JAL_TX_SEQN_MASK           JAL_TX_SEQN_MASK

#define JAL_TX_CTRL_REG_JAL_TX_BUF_MASK            JAL_TX_BUF_MASK

#define JAL_TX_CTRL_REG_JAL_TX_MODE_MASK           JAL_TX_MODE_MASK
#define JAL_TX_CTRL_REG_JAL_TX_L_CH_MASK           JAL_TX_L_CH_MASK
#define JAL_TX_CTRL_REG_JAL_TX_P_FLOW_MASK         JAL_TX_P_FLOW_MASK

/*
 * eSCO Control Register                    Address 34H
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_ESCO_CTRL_REG_ADDR       (JAL_BASE_ADDR+0x0720/ADDR_DIV)
#else
#define JAL_ESCO_CTRL_REG_ADDR       (JAL_BASE_ADDR+0x34/ADDR_DIV)
#endif

#define JAL_ESCO_CTRL_REG_JAL_ESCO_TX_LEN_MASK      JAL_ESCO_TX_LEN_MASK
#define JAL_ESCO_CTRL_REG_JAL_ESCO_RX_LEN_MASK      JAL_ESCO_RX_LEN_MASK
#define JAL_ESCO_CTRL_REG_JAL_ESCO_LT_ADDR_MASK      JAL_ESCO_LT_ADDR_MASK
#define JAL_ESCO_CTRL_REG_JAL_SCO_ROUTE_MASK     JAL_SCO_ROUTE_MASK

/*
 * Transmit Status Register                   Address 40H
 */
#define JAL_TX_STATUS_REG_ADDR                    (JAL_BASE_ADDR+0x40/ADDR_DIV)

/*
 * Receive Control Register                   Address 48H
 */
#define JAL_RX_CTRL_REG_ADDR                      (JAL_BASE_ADDR+0x48/ADDR_DIV)

#define JAL_RX_CTRL_REG_JAL_RX_MODE_MASK         JAL_RX_MODE_MASK
#define JAL_RX_CTRL_REG_JAL_SYNC_ERR_MASK        JAL_SYNC_ERR_MASK
#define JAL_RX_CTRL_REG_JAL_ABORT_ON_WRONG_AM_ADDR_MASK     JAL_ABORT_ON_WRONG_AM_ADDR_MASK
#define JAL_RX_CTRL_REG_JAL_RX_BUF_MASK          JAL_RX_BUF_MASK
#define JAL_RX_CTRL_REG_JAL_ERR_SEL_MASK         JAL_ERR_SEL_MASK
#define JAL_RX_CTRL_REG_JAL_WIN_EXT_MASK         JAL_WIN_EXT_MASK

/*
 * Receive Status General Purpose Register    Address 50H
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define JAL_RX_STATUS_GP_REG_ADDR                 (JAL_BASE_ADDR+0x0730/ADDR_DIV)
#else
#define JAL_RX_STATUS_GP_REG_ADDR                 (JAL_BASE_ADDR+0x50/ADDR_DIV)
#endif
#define JAL_RX_STATUS_GP_REG_JAL_RX_LEN_MASK      JAL_RX_LEN_MASK
#define JAL_RX_STATUS_GP_REG_JAL_RX_TYPE_MASK     JAL_RX_TYPE_MASK

#define JAL_RX_STATUS_GP_REG_JAL_RX_FLOW_MASK     JAL_RX_FLOW_MASK
#define JAL_RX_STATUS_GP_REG_JAL_RX_ARQN_MASK     JAL_RX_ARQN_MASK
#define JAL_RX_STATUS_GP_REG_JAL_RX_SEQN_MASK     JAL_RX_SEQN_MASK

#define JAL_RX_STATUS_GP_REG_JAL_RX_AM_ADDR_MASK  JAL_RX_AM_ADDR_MASK

#define JAL_RX_STATUS_GP_REG_JAL_RX_L_CH_MASK     JAL_RX_L_CH_MASK
#define JAL_RX_STATUS_GP_REG_JAL_RX_P_FLOW_MASK   JAL_RX_P_FLOW_MASK

#define JAL_RX_STATUS_GP_REG_JAL_RX_PKT_MASK      JAL_RX_PKT_MASK
#define JAL_RX_STATUS_GP_REG_JAL_HEC_ERR_MASK     JAL_HEC_ERR_MASK
#define JAL_RX_STATUS_GP_REG_JAL_CRC_ERR_MASK     JAL_CRC_ERR_MASK

#define JAL_RX_STATUS_GP_REG_JAL_AM_ADDR_ABORT_MASK     JAL_AM_ADDR_ABORT_MASK


/*
 * Receive Status SCO Register                Address 54H
 */
#define JAL_RX_STATUS_SCO_REG_ADDR                (JAL_BASE_ADDR+0x54/ADDR_DIV)

/*
 * Encryption Key Length Register             Address B8H
 */
#define JAL_ENC_KEY_LENGTH_REG_ADDR               (JAL_BASE_ADDR+0xB8/ADDR_DIV)

/*
 * Parallel Interface Adapter Register        Address A4H
 */
#define JAL_PIA_DATA_REG_ADDR                     (JAL_BASE_ADDR+0xA4/ADDR_DIV)

#define JAL_PIA_DATA_REG_JAL_P_DATA_0_MASK     JAL_P_DATA_0_MASK
#define JAL_PIA_DATA_REG_JAL_P_DATA_1_MASK     JAL_P_DATA_1_MASK
#define JAL_PIA_DATA_REG_JAL_P_DATA_2_MASK     JAL_P_DATA_2_MASK
#define JAL_PIA_DATA_REG_JAL_P_DATA_3_MASK     JAL_P_DATA_3_MASK


/*
 * Serial Interface Adapter Register          Address 60H
 */
#define JAL_SER_CFG_REG_ADDR                     (JAL_BASE_ADDR+0x60/ADDR_DIV)

#define JAL_SER_CFG_REG_JAL_SER_CFG_MASK         JAL_SER_CFG_MASK
#define JAL_SER_CFG_REG_JAL_SER_BUSY_MASK        JAL_SER_BUSY_MASK


#define JAL_SER_CTRL_REG_ADDR                    (JAL_BASE_ADDR+0x64/ADDR_DIV)

#define JAL_SER_CTRL_REG_JAL_SER_TIME_MASK       JAL_SER_TIME_MASK
#define JAL_SER_CTRL_REG_JAL_SER_COMBINE_MASK    JAL_SER_COMBINE_MASK
#define JAL_SER_CTRL_REG_JAL_SER_MASK_MASK       JAL_SER_MASK_MASK
#define JAL_SER_CTRL_REG_JAL_SER_NOW_MASK        JAL_SER_NOW_MASK
#define JAL_SER_CTRL_REG_JAL_SER_SEQ_MASK        JAL_SER_SEQ_MASK


#define JAL_SER_DATA0_REG_ADDR                  (JAL_BASE_ADDR+0x70/ADDR_DIV)
#define JAL_SER_DATA1_REG_ADDR                  (JAL_BASE_ADDR+0x74/ADDR_DIV)
#define JAL_SER_DATA2_REG_ADDR                  (JAL_BASE_ADDR+0x78/ADDR_DIV)
#define JAL_SER_DATA3_REG_ADDR                  (JAL_BASE_ADDR+0x7C/ADDR_DIV)


/*
 * Definitions for the Enhanced SER block
 */
#define JAL_SER_ESER_CTRL_REG_ADDR                    (JAL_BASE_ADDR+0x64/ADDR_DIV)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_COMBINE_MASK    (0x00070000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MASK_MASK       (0x0000FFFF)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_NOW_MASK        (0x00100000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_SEQ_MASK        (0x00200000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_DOUBLE_MASK     (0x00400000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_SEL_MASK    (0x00800000)

#define JAL_SER_ESER_CTRL_REG_JAL_SER_COMBINE_BIT_OFFSET    16
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MASK_BIT_OFFSET        0
#define JAL_SER_ESER_CTRL_REG_JAL_SER_NOW_BIT_OFFSET        20
#define JAL_SER_ESER_CTRL_REG_JAL_SER_SEQ_BIT_OFFSET        21
#define JAL_SER_ESER_CTRL_REG_JAL_SER_DOUBLE_BIT_OFFSET     22
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_SEL_BIT_OFFSET    23
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_BUSY_BIT_OFFSET   31



#define JAL_SER_ESER_TIME_REG_ADDR         (JAL_BASE_ADDR+0x68/ADDR_DIV)
#define JAL_SER_ESER_TIME_0_MASK           (0x000F)
#define JAL_SER_ESER_TIME_1_MASK           (0x00F0)
#define JAL_SER_ESER_TIME_2_MASK           (0x0F00)
#define JAL_SER_ESER_TIME_3_MASK           (0xF000)

#define JAL_SER_ESER_TIME_0_BIT_OFFSET     0
#define JAL_SER_ESER_TIME_1_BIT_OFFSET     4
#define JAL_SER_ESER_TIME_2_BIT_OFFSET     8
#define JAL_SER_ESER_TIME_3_BIT_OFFSET     12



#define JAL_SER_ESER_DATA_ADDR                   (JAL_BASE_ADDR+0x600/ADDR_DIV)
#define JAL_SER_ESER_DATA_0_ADDR                 (JAL_SER_ESER_DATA_ADDR)
#define JAL_SER_ESER_DATA_1_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*1)
#define JAL_SER_ESER_DATA_2_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*2)
#define JAL_SER_ESER_DATA_3_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*3)
#define JAL_SER_ESER_DATA_4_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*4)
#define JAL_SER_ESER_DATA_5_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*5)
#define JAL_SER_ESER_DATA_6_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*6)
#define JAL_SER_ESER_DATA_7_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*7)
#define JAL_SER_ESER_DATA_8_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*8)
#define JAL_SER_ESER_DATA_9_ADDR                 (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*9)
#define JAL_SER_ESER_DATA_10_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*10)
#define JAL_SER_ESER_DATA_11_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*11)
#define JAL_SER_ESER_DATA_12_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*12)
#define JAL_SER_ESER_DATA_13_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*13)
#define JAL_SER_ESER_DATA_14_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*14)
#define JAL_SER_ESER_DATA_15_ADDR                (JAL_SER_ESER_DATA_ADDR+(4/ADDR_DIV)*15)




/*
 * Parallel Interface Adapter Register        Address A8H
 */
#define JAL_PIA_CTRL_REG_ADDR                   (JAL_BASE_ADDR+0xA8/ADDR_DIV)

#define JAL_PIA_CTRL_REG_JAL_P_SETUP_MASK       JAL_P_SETUP_MASK
#define JAL_PIA_CTRL_REG_JAL_P_MASK_MASK        JAL_P_MASK_MASK
#define JAL_PIA_CTRL_REG_JAL_P_NOW_MASK         JAL_P_NOW_MASK
#define JAL_PIA_CTRL_REG_JAL_P_SEQ_MASK         JAL_P_SEQ_MASK
#define JAL_PIA_CTRL_REG_JAL_P_DATA_DIR_MASK    JAL_P_DATA_DIR_MASK
#define JAL_PIA_CTRL_REG_JAL_P_RDATA_MASK       JAL_P_RDATA_MASK


/*
 * Hop Selection Control Register             Address D0H (temp 9CH)
 *                                            see register definitions
 */
#define JAL_HSE_CONTROL_REG_ADDR                 (JAL_BASE_ADDR+0xD0/ADDR_DIV)

#define JAL_HSE_CONTROL_REG_JAL_SYS_MASK          JAL_SYS_MASK
#define JAL_HSE_CONTROL_REG_JAL_H_PAGE_MASK       JAL_H_PAGE_MASK
#define JAL_HSE_CONTROL_REG_JAL_H_SLAVE_MASK      JAL_H_SLAVE_MASK
#define JAL_HSE_CONTROL_REG_JAL_PAGE_OFFSET_MASK  JAL_PAGE_OFFSET_MASK
#define JAL_HSE_CONTROL_REG_JAL_INDEX_MAP_MASK    JAL_INDEX_MAP_MASK
#define JAL_HSE_CONTROL_REG_JAL_CLK_DELTA_MASK    JAL_CLK_DELTA_MASK
#define JAL_HSE_CONTROL_REG_JAL_N_COUNT_MASK      JAL_N_COUNT_MASK
#define JAL_HSE_CONTROL_REG_JAL_SEL_MASK          JAL_SEL_MASK
#define JAL_HSE_CONTROL_REG_JAL_SUP_BT_CLK_MASK   JAL_SUP_BT_CLK_MASK

/*
 * Hop Selection Control Register             Address D4H
 */
#define JAL_HSE_HOP_REG_ADDR                      (JAL_BASE_ADDR+0xD4/ADDR_DIV)
#define JAL_HSE_HOP_REG_JAL_HOP_MASK              JAL_HOP_MASK

/*
 * Hop Selection Registers V1.2               Address D0-DC
 * No fields, since there is only a single one in each register.
 *  Addr       b7     b6     b5     b4     b3     b2     b1     b0
 *  CONTROL +------+------+------+------+------+------+------+------+
 *  D0      |    HSE_BT_CLK[27:0]                                   |
 *          +------+------+------+------+------+------+------+------+
 *  D4      |    HSE_UAP_LAP[23:0]                                  |
 *          +------+------+------+------+------+------+------+------+
 *  D8      |    HSE_SUM[23:0]                                      |
 *          +------+------+------+------+------+------+------+------+
 *  DC      |    HSE_RF_CHAN_INDEX[6:0]                             |
 *          +------+------+------+------+------+------+------+------+
 */
#define JAL_HSE_BT_CLK_REG_ADDR                   (JAL_BASE_ADDR+0xD0/ADDR_DIV)
#define JAL_HSE_UAP_LAP_REG_ADDR                  (JAL_BASE_ADDR+0xD4/ADDR_DIV)
#define JAL_HSE_SUM_REG_ADDR                      (JAL_BASE_ADDR+0xD8/ADDR_DIV)
#define JAL_HSE_RF_CHAN_INDEX_REG_ADDR            (JAL_BASE_ADDR+0xDC/ADDR_DIV)

#if (PRH_BS_CFG_SYS_SCO_REPEATER_SUPPORTED==1)
/*******************************************************************
 *
 *   Define the bit positions for the SCO Repeater Controls(Tama only)
 *   (NT clock phase offset is also adjustable when hit by RX packet)
 *******************************************************************/
#define JAL_SCO_REPEATER_BIT_ADDR     (0x000000F0/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_SCO_REPEATER_BIT_MASK     0x000000010
#define JAL_SCO_REPEATER_BIT_SHFT     4
#endif

#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
/*
 * EDR Control Registers
 */
#define JAL_EDR_CTRL_REG_ADDR (JAL_BASE_ADDR+0x700/ADDR_DIV)

#define JAL_EDR_ENABLE_ADDR     (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_ENABLE_MASK     0x00000001
#define JAL_EDR_ENABLE_SHFT		0

#define JAL_EDR_PTT_ACL_ADDR    (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_PTT_ACL_MASK	0x00000002
#define JAL_EDR_PTT_ACL_SHFT	1

#define JAL_EDR_PTT_ESCO_ADDR   (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_PTT_ESCO_MASK	0x00000004
#define JAL_EDR_PTT_ESCO_SHFT	2

#define JAL_EDR_SYNC_ERROR_ADDR (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_SYNC_ERROR_MASK	0x000000f8
#define JAL_EDR_SYNC_ERROR_SHFT	3

#define JAL_EDR_RX_EDR_DELAY_ADDR (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_RX_EDR_DELAY_MASK	0x00000f00
#define JAL_EDR_RX_EDR_DELAY_SHFT	8

#define JAL_EDR_TX_EDR_DELAY_ADDR (0x00000700/ADDR_DIV + JAL_BASE_ADDR)
#define JAL_EDR_TX_EDR_DELAY_MASK	0x0000f000
#define JAL_EDR_TX_EDR_DELAY_SHFT	12

#define JAL_EDR_CTRL_REG_JAL_EDR_ENABLE_MASK           JAL_EDR_ENABLE_MASK
#define JAL_EDR_CTRL_REG_JAL_EDR_PTT_ACL_MASK           JAL_EDR_PTT_ACL_MASK
#define JAL_EDR_CTRL_REG_JAL_EDR_PTT_ESCO_MASK           JAL_EDR_PTT_ESCO_MASK
#define JAL_EDR_CTRL_REG_JAL_EDR_TX_EDR_DELAY_MASK           JAL_EDR_TX_EDR_DELAY_MASK
#define JAL_EDR_CTRL_REG_JAL_EDR_RX_EDR_DELAY_MASK           JAL_EDR_RX_EDR_DELAY_MASK

#define JAL_EDR_CTRL_REG_JAL_EDR_SYNC_ERROR_MASK  JAL_EDR_SYNC_ERROR_MASK


#endif  /* #if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1) */

#else // SINGLE MODE LE Supported

#define JAL_BD_ADDR_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_BD_ADDR_MASK     0xFFFFFFFF
#define JAL_BD_ADDR_SHFT     0

#define JAL_UAP_LAP_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_NAP_ADDR         (0x00000004 + JAL_BASE_ADDR)

#define JAL_NATIVE_CLK_ADDR  (0x00000008 + JAL_BASE_ADDR)
#define JAL_NATIVE_CLK_MASK  0x0FFFFFFF
#define JAL_NATIVE_CLK_SHFT  0

/*
 * No BT Clock in Single Mode so we point it to the Native Clk instead
 */
#define JAL_BT_CLK_OFFSET_ADDR      JAL_NATIVE_CLK_ADDR
#define JAL_BT_CLK_OFFSET_MASK      JAL_NATIVE_CLK_MASK
#define JAL_BT_CLK_OFFSET_SHFT      JAL_NATIVE_CLK_SHFT


#define JAL_USE_LF_ADDR      (0x0000000c + JAL_BASE_ADDR)
#define JAL_USE_LF_MASK      0x00000001
#define JAL_USE_LF_SHFT      0


#define JAL_LOOP_ADDR        (0x0000000c + JAL_BASE_ADDR)
#define JAL_LOOP_MASK        0x00000002
#define JAL_LOOP_SHFT        1

#define JAL_TEST_RADIO_ADDR  (0x0000000c + JAL_BASE_ADDR)
#define JAL_TEST_RADIO_MASK  0x00000004
#define JAL_TEST_RADIO_SHFT  2

#define JAL_USE_HAB_CTRL_ADDR   (0x0000000c + JAL_BASE_ADDR)
#define JAL_USE_HAB_CTRL_MASK   0x00000010
#define JAL_USE_HAB_CTRL_SHFT   4

#define JAL_TEST_TX_SHIFT_ADDR  (0x0000000c + JAL_BASE_ADDR)
#define JAL_TEST_TX_SHIFT_MASK  0x0000000E0
#define JAL_TEST_TX_SHIFT_SHFT  5

/*
 * Quick Solution to allow Slave bit be written in both single mode and
 * dual mode (without conditional in code). Use a Macro with the mask
 * set to 0x00000000 and an unused address 0x0000000D
 *
 * NOTE - Slave Bit is not present in Single mode.
 */
#define JAL_SLAVE_ADDR       (0x0000000c + JAL_BASE_ADDR)
#define JAL_SLAVE_MASK       0x00000000
#define JAL_SLAVE_SHFT       8


#define JAL_TIM_INTR_MSK_ADDR        (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM_INTR_MSK_MASK        0x0000000f
#define JAL_TIM_INTR_MSK_SHFT        0

#define JAL_TIM0_INTR_MSK_ADDR       (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_MSK_MASK       0x00000001
#define JAL_TIM0_INTR_MSK_SHFT       0

#define JAL_TIM1_INTR_MSK_ADDR       (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_MSK_MASK       0x00000002
#define JAL_TIM1_INTR_MSK_SHFT       1

#define JAL_TIM2_INTR_MSK_ADDR       (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_MSK_MASK       0x00000004
#define JAL_TIM2_INTR_MSK_SHFT       2

#define JAL_TIM3_INTR_MSK_ADDR       (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_MSK_MASK       0x00000008
#define JAL_TIM3_INTR_MSK_SHFT       3

#define JAL_PKD_INTR_MSK_ADDR        (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKD_INTR_MSK_MASK        0x00000010
#define JAL_PKD_INTR_MSK_SHFT        4

#define JAL_AUX_TIM_INTR_MSK_ADDR    (0x00000010 + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_MSK_MASK    0x00000020
#define JAL_AUX_TIM_INTR_MSK_SHFT    5

#define JAL_PKA_INTR_MSK_ADDR        (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKA_INTR_MSK_MASK        0x00000040
#define JAL_PKA_INTR_MSK_SHFT        6

#define JAL_PKD_RX_HDR_INTR_MSK_ADDR (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_MSK_MASK 0x00000080
#define JAL_PKD_RX_HDR_INTR_MSK_SHFT 7

#define JAL_AES_INTR_MSK_ADDR		(0x00000010 + JAL_BASE_ADDR)
#define JAL_AES_INTR_MSK_MASK		 0x00000200
#define JAL_AES_INTR_MSK_SHFT        9

#define JAL_AUXLE1_TMR_INTR_MSK_ADDR	(0x00000010 + JAL_BASE_ADDR)
#define JAL_AUXLE1_TMR_INTR_MSK_MASK	0x00000400
#define JAL_AUXLE1_TMR_INTR_MSK_SHFT    10

#define JAL_AUXLE2_TMR_INTR_MSK_ADDR	(0x00000010 + JAL_BASE_ADDR)
#define JAL_AUXLE2_TMR_INTR_MSK_MASK	0x00000800
#define JAL_AUXLE2_TMR_INTR_MSK_SHFT    11

#define JAL_NO_PKT_RCVD_INTR_MSK_ADDR    (0x00000010 + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_MSK_SHFT    13
#define JAL_NO_PKT_RCVD_INTR_MSK_MASK    0x00002000

#define JAL_SYNC_DET_INTR_MSK_ADDR    (0x00000010 + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_MSK_SHFT    14
#define JAL_SYNC_DET_INTR_MSK_MASK    0x00004000

#define JAL_TIM_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM_INTR_CLR_MASK   0x000f0000
#define JAL_TIM_INTR_CLR_SHFT   16

#define JAL_TIM0_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_CLR_MASK   0x00010000
#define JAL_TIM0_INTR_CLR_SHFT   16

#define JAL_TIM1_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_CLR_MASK   0x00020000
#define JAL_TIM1_INTR_CLR_SHFT   17

#define JAL_TIM2_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_CLR_MASK   0x00040000
#define JAL_TIM2_INTR_CLR_SHFT   18

#define JAL_TIM3_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_CLR_MASK   0x00080000
#define JAL_TIM3_INTR_CLR_SHFT   19


#define JAL_PKD_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKD_INTR_CLR_MASK   0x00100000
#define JAL_PKD_INTR_CLR_SHFT   20

#define JAL_AUX_TIM_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_CLR_MASK   0x00200000
#define JAL_AUX_TIM_INTR_CLR_SHFT   21

#define JAL_PKA_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKA_INTR_CLR_MASK   0x00400000
#define JAL_PKA_INTR_CLR_SHFT   22

#define JAL_PKD_RX_HDR_INTR_CLR_ADDR   (0x00000010 + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_CLR_MASK   0x00800000
#define JAL_PKD_RX_HDR_INTR_CLR_SHFT   23

#define JAL_AUXLE1_TMR_INTR_CLR_ADDR	(0x00000010 + JAL_BASE_ADDR)
#define JAL_AUXLE1_TMR_INTR_CLR_MASK	0x01000000
#define JAL_AUXLE1_TMR_INTR_CLR_SHFT    24

#define JAL_AUXLE2_TMR_INTR_CLR_ADDR	(0x00000010 + JAL_BASE_ADDR)
#define JAL_AUXLE2_TMR_INTR_CLR_MASK	0x04000000
#define JAL_AUXLE2_TMR_INTR_CLR_SHFT    26

#define JAL_SPI_NOW_CONFLICT_CLR_ADDR  (0x00000010 + JAL_BASE_ADDR)
#define JAL_SPI_NOW_CONFLICT_CLR_SHFT  25
#define JAL_SPI_NOW_CONFLICT_CLR_MASK  0x02000000

#define JAL_SER0_WR_CLR_ADDR  (0x00000010 + JAL_BASE_ADDR)
#define JAL_SER0_WR_CLR_SHFT  27
#define JAL_SER0_WR_CLR_MASK  0x08000000

#define JAL_AES_INTR_CLR_ADDR		(0x00000010 + JAL_BASE_ADDR)
#define JAL_AES_INTR_CLR_MASK		 0x10000000
#define JAL_AES_INTR_CLR_SHFT        28


#define JAL_NO_PKT_RCVD_INTR_CLR_ADDR  (0x00000010 + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_CLR_SHFT  29
#define JAL_NO_PKT_RCVD_INTR_CLR_MASK  0x20000000

#define JAL_SYNC_DET_INTR_CLR_ADDR  (0x00000010 + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_CLR_SHFT  30
#define JAL_SYNC_DET_INTR_CLR_MASK  0x40000000

/*******************************************************************
 * Common Status Registers
 ******************************************************************/

/****************************************************************
*  Define the bit positions for the Common Status Registers
****************************************************************/

#define JAL_TIM_INTR_ADDR         (0x00000014 + JAL_BASE_ADDR)
#define JAL_TIM_INTR_MASK         0x0000000f
#define JAL_TIM_INTR_SHFT         0

#define JAL_TIM0_INTR_ADDR        (0x00000014 + JAL_BASE_ADDR)
#define JAL_TIM0_INTR_MASK        0x00000001
#define JAL_TIM0_INTR_SHFT        0

#define JAL_TIM1_INTR_ADDR        (0x00000014 + JAL_BASE_ADDR)
#define JAL_TIM1_INTR_MASK        0x00000002
#define JAL_TIM1_INTR_SHFT        1

#define JAL_TIM2_INTR_ADDR        (0x00000014 + JAL_BASE_ADDR)
#define JAL_TIM2_INTR_MASK        0x00000004
#define JAL_TIM2_INTR_SHFT        2

#define JAL_TIM3_INTR_ADDR        (0x00000014 + JAL_BASE_ADDR)
#define JAL_TIM3_INTR_MASK        0x00000008
#define JAL_TIM3_INTR_SHFT        3

#define JAL_PKD_INTR_ADDR         (0x00000014 + JAL_BASE_ADDR)
#define JAL_PKD_INTR_MASK         0x00000010
#define JAL_PKD_INTR_SHFT         4

#define JAL_AUX_TIM_INTR_ADDR     (0x00000014 + JAL_BASE_ADDR)
#define JAL_AUX_TIM_INTR_MASK     0x00000020
#define JAL_AUX_TIM_INTR_SHFT     5

#define JAL_PKA_INTR_ADDR         (0x00000014 + JAL_BASE_ADDR)
#define JAL_PKA_INTR_MASK         0x00000040
#define JAL_PKA_INTR_SHFT         6

#define JAL_PKD_RX_HDR_INTR_ADDR  (0x00000014 + JAL_BASE_ADDR)
#define JAL_PKD_RX_HDR_INTR_MASK  0x00000080
#define JAL_PKD_RX_HDR_INTR_SHFT  7

#define JAL_AES_INTR_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_AES_INTR_SHFT 8
#define JAL_AES_INTR_MASK 0x00000100

#define JAL_SPI_NOW_CONFLICT_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_SPI_NOW_CONFLICT_SHFT 9
#define JAL_SPI_NOW_CONFLICT_MASK 0x00000200

#define JAL_AUXLE1_TMR_INTR_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_AUXLE1_TMR_INTR_SHFT  10
#define JAL_AUXLE1_TMR_INTR_MASK  0x00000400

#define JAL_AUXLE2_TMR_INTR_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_AUXLE2_TMR_INTR_SHFT  11
#define JAL_AUXLE2_TMR_INTR_MASK  0x00000800

#define JAL_NO_PKT_RCVD_INTR_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_NO_PKT_RCVD_INTR_SHFT 13
#define JAL_NO_PKT_RCVD_INTR_MASK 0x00002000

#define JAL_SYNC_DET_INTR_ADDR (0x00000014 + JAL_BASE_ADDR)
#define JAL_SYNC_DET_INTR_SHFT 14
#define JAL_SYNC_DET_INTR_MASK 0x00004000

#define JAL_SLEEP_ADDR    (0x00000014 + JAL_BASE_ADDR)
#define JAL_SLEEP_SHFT    15
#define JAL_SLEEP_MASK    0x00008000


#define JAL_SLEEP_STATUS_ADDR    (0x00000014 + JAL_BASE_ADDR)
#define JAL_SLEEP_STATUS_SHFT    15
#define JAL_SLEEP_STATUS_MASK    0x00008000

#define JAL_NAT_BIT_CNT_ADDR        (0x00000018 + JAL_BASE_ADDR)
#define JAL_NAT_BIT_CNT_MASK        0x000007FF
#define JAL_NAT_BIT_CNT_SHFT        0

/******************************************************
 * Receive Control Registers
 *****************************************************/

#define JAL_RX_MODE_ADDR    (0x0000001C + JAL_BASE_ADDR)   /* Combination of RX_EN and FULL_WIN */
#define JAL_RX_MODE_MASK    0x00000003
#define JAL_RX_MODE_SHFT    0

#define JAL_SYNC_ERR_ADDR   (0x0000001C + JAL_BASE_ADDR)
#define JAL_SYNC_ERR_MASK   0x0000008c
#define JAL_SYNC_ERR_SHFT   2

#define JAL_WIN_EXT_ADDR    (0x0000001C + JAL_BASE_ADDR)
#define JAL_WIN_EXT_MASK    0x0003ff00
#define JAL_WIN_EXT_SHFT    8

/*
 * SERIAL Interface
 */
/****************************************************************
*
*  Define the bit positions for the Serial Interface Registers
*
****************************************************************/
#define JAL_SER_CFG_ADDR        (0x00000020 + JAL_BASE_ADDR)
#define JAL_SER_CFG_MASK        0x7FFFFFFF
#define JAL_SER_CFG_SHFT        0

#define JAL_SER_BUSY_ADDR       (0x00000020 + JAL_BASE_ADDR)
#define JAL_SER_BUSY_MASK       0x80000000
#define JAL_SER_BUSY_SHFT       31

#define JAL_SER_TIME_SER_MAP_SEL_0_ADDR       (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_TIME_SER_MAP_SEL_0_MASK       0x000000F0
#define JAL_SER_TIME_SER_MAP_SEL_0_SHFT       4

#define JAL_SER_COMBINE_SER_MAP_SEL_0_ADDR    (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_COMBINE_SER_MAP_SEL_0_MASK    0x00000700
#define JAL_SER_COMBINE_SER_MAP_SEL_0_SHFT    8

#define JAL_SER_MASK_SER_MAP_SEL_1_ADDR       (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_MASK_SER_MAP_SEL_1_MASK       0x0000FFFF
#define JAL_SER_MASK_SER_MAP_SEL_1_SHFT       0

#define JAL_SER_MASK_SER_MAP_SEL_0_ADDR       (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_MASK_SER_MAP_SEL_0_MASK       0x000F0000
#define JAL_SER_MASK_SER_MAP_SEL_0_SHFT       16

#define JAL_SER_COMBINE_SER_MAP_SEL_1_ADDR    (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_COMBINE_SER_MAP_SEL_1_MASK    0x00070000
#define JAL_SER_COMBINE_SER_MAP_SEL_1_SHFT    16

#define JAL_SER_ONE_SHOT_SER_MAP_SEL_1_ADDR    (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_ONE_SHOT_SER_MAP_SEL_1_MASK    0x00080000
#define JAL_SER_ONE_SHOT_SER_MAP_SEL_1_SHFT    19



/*******************************************************************
 * New SER Times for Window Extension
 *
 *
 */

#define JAL_SER_TIME_EARLY_ADDR         (0x0000002A + JAL_BASE_ADDR)
#define JAL_SER_TIME_EARLY_MASK          0x0000000f
#define JAL_SER_TIME_EARLY_SHFT          0

#define JAL_SER_TIME_LATE_ADDR         (0x0000002A + JAL_BASE_ADDR)
#define JAL_SER_TIME_LATE_MASK          0x000000f0
#define JAL_SER_TIME_LATE_SHFT          4



#define JAL_SER_NOW_ADDR        (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_NOW_MASK        0x00100000
#define JAL_SER_NOW_SHFT        20

#define JAL_SER_SEQ_ADDR        (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_SEQ_MASK        0x00200000
#define JAL_SER_SEQ_SHFT        21

#define JAL_SER_DOUBLE_ADDR     (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_DOUBLE_MASK     0x00400000
#define JAL_SER_DOUBLE_SHFT     22

#define JAL_SER_MAP_SEL_ADDR     (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_MAP_SEL_MASK     0x00800000
#define JAL_SER_MAP_SEL_SHFT     23


#define JAL_SER_READ_DATA_ADDR  (0x0000002C + JAL_BASE_ADDR)
#define JAL_SER_READ_DATA_MASK  0xFFFFFFFF
#define JAL_SER_READ_DATA_SHFT  0

#define JAL_SER_DATA0_ADDR      (0x00000030 + JAL_BASE_ADDR)
#define JAL_SER_DATA0_MASK      0xFFFFFFFF
#define JAL_SER_DATA0_SHFT      0

#define JAL_SER_DATA1_ADDR      (0x00000034 + JAL_BASE_ADDR)
#define JAL_SER_DATA1_MASK      0xFFFFFFFF
#define JAL_SER_DATA1_SHFT      0

#define JAL_SER_DATA2_ADDR      (0x00000038 + JAL_BASE_ADDR)
#define JAL_SER_DATA2_MASK      0xFFFFFFFF
#define JAL_SER_DATA2_SHFT      0

#define JAL_SER_DATA3_ADDR      (0x0000003C + JAL_BASE_ADDR)
#define JAL_SER_DATA3_MASK      0xFFFFFFFF
#define JAL_SER_DATA3_SHFT      0


#if 0 // seems to be missing in LE Single Mode
#define JAL_SLAVE_ADDR       (0x00000020 + JAL_BASE_ADDR)
#define JAL_SLAVE_MASK       0x00000040
#define JAL_SLAVE_SHFT       6


#endif

/*******************************************************************
 *
 *     Define the bit positions for the auxilliary timer
 *     registers
 *
 *******************************************************************/
#define JAL_AUX_TIMER_ADDR     (0x00000048 + JAL_BASE_ADDR)
#define JAL_AUX_TIMER_MASK     0x00003FFF
#define JAL_AUX_TIMER_SHFT     0

/*******************************************************************
*
*      Define the bit positions for the reset controls
*
********************************************************************/
#define JAL_RST_CODE_ADDR     (0x00000094 + JAL_BASE_ADDR)
#define JAL_RST_CODE_MASK     0x000000FF
#define JAL_RST_CODE_SHFT     0


/*******************************************************************
*
*      Define the bit positions for the rx_delay and tx_delay
*      control registers
*
*******************************************************************/
#define JAL_RX_DELAY_ADDR      (0x0000004C + JAL_BASE_ADDR)
#define JAL_RX_DELAY_MASK      0x0000007f
#define JAL_RX_DELAY_SHFT      0

#define JAL_TX_DELAY_ADDR      (0x0000004C + JAL_BASE_ADDR)
#define JAL_TX_DELAY_MASK      0x00000f00
#define JAL_TX_DELAY_SHFT      8

/*******************************************************************
 *
 *   Define the bit positions for the LC Revision Code Register
 *
 *******************************************************************/
#define JAL_MINOR_REVISION_ADDR    (0x00000090 + JAL_BASE_ADDR)
#define JAL_MINOR_REVISION_MASK    0x000000FF
#define JAL_MINOR_REVISION_SHFT    0

#define JAL_MAJOR_REVISION_ADDR    (0x00000090 + JAL_BASE_ADDR)
#define JAL_MAJOR_REVISION_MASK    0x0000FF00
#define JAL_MAJOR_REVISION_SHFT    8



/****************************************************************
*
*  Define the Transmit and Receive Registers
*
*  KEY COMPONENT
****************************************************************/
//***************************************************************
//***************************************************************
//***************************************************************

#define JAL_TX_ACL_BUF_OFFSET   0x00000A0
#define JAL_RX_ACL_BUF_OFFSET   0x00000D0
#define JAL_ACL_BUF_LEN   0x27

//***************************************************************
//***************************************************************
//***************************************************************

/****************************************************************
*
*  Define the bit positions for the Receive Control Rgeisters
*
****************************************************************/

/****************************************************************
*
*  Define the bit positions for the Serial Interface Registers
*
****************************************************************/


#define JAL_SER_TIME_ADDR       (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_TIME_MASK       0x000000F0
#define JAL_SER_TIME_SHFT       4


#define JAL_SER_COMBINE_ADDR    (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_COMBINE_MASK    0x00000700
#define JAL_SER_COMBINE_SHFT    8

#define JAL_SER_MASK_ADDR       (0x00000024 + JAL_BASE_ADDR)
#define JAL_SER_MASK_MASK       0x000F0000
#define JAL_SER_MASK_SHFT       16
/****************************************************************
*
*  Define the addresses of the main registers and associated
*  fields to allow local caching of registers and checking of
*  field assignments to take place
*  To use a local copy of a register
*  1. Give it a unique meaningful name REGISTER_XXX.
*  2. #define REGISTER_XXX_FIELD FIELD for each FIELD that will be
*     used with this register
*  3
*
****************************************************************/
/*
 * BD_ADDR Register        Low/High Address 00H/04H
 */
#define JAL_COM_CTRL_BD_ADDR_LO_REG_ADDR             (JAL_BASE_ADDR+0x00)
#define JAL_COM_CTRL_BD_ADDR_HI_REG_ADDR             (JAL_BASE_ADDR+0x04)

/*
 * Clock Registers         BT_CLK[_OFFSET]  18H/ NATIVE_CLK 1CH
 */
//#define JAL_COM_CTRL_BT_CLK_READ_REG_ADDR           (JAL_BASE_ADDR+0x18)
//#define JAL_COM_CTRL_BT_CLK_OFFSET_WRITE_REG_ADDR   (JAL_BASE_ADDR+0x18)
#define JAL_COM_CTRL_NATIVE_CLK_REG_ADDR            (JAL_BASE_ADDR+0x08)

/*
 * Common Control Interrupt Register          Address 10H
 */
#define JAL_COM_CTRL_IRQ_REG_ADDR                     (JAL_BASE_ADDR+0x10)

#define JAL_COM_CTRL_IRQ_REG_JAL_TIM_INTR_MSK_MASK          JAL_TIM_INTR_MSK_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_TIM0_INTR_MSK_MASK         JAL_TIM0_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM1_INTR_MSK_MASK         JAL_TIM1_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM2_INTR_MSK_MASK         JAL_TIM2_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM3_INTR_MSK_MASK         JAL_TIM3_INTR_MSK_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_INTR_MSK_MASK          JAL_PKD_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AUX_TIM_INTR_MSK_MASK       JAL_AUX_TIM_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKA_INTR_MSK_MASK          JAL_PKA_INTR_MSK_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_RX_HDR_INTR_MSK_MASK   JAL_PKD_RX_HDR_INTR_MSK_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_AES_INTR_MSK_MASK           JAL_AES_INTR_MSK_MASK
// AUX_LE1_TIMER_INT_MASK
// AUX_LE2_TIMER_INT_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_NO_PKT_RCVD_INTR_MSK_MASK  JAL_NO_PKT_RCVD_INTR_MSK_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_SYNC_DET_INTR_MSK_MASK     JAL_SYNC_DET_INTR_MSK_MASK


#define JAL_COM_CTRL_IRQ_REG_JAL_TIM0_INTR_CLR_MASK          JAL_TIM0_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM1_INTR_CLR_MASK          JAL_TIM1_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM2_INTR_CLR_MASK          JAL_TIM2_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_TIM3_INTR_CLR_MASK          JAL_TIM3_INTR_CLR_MASK

//Missing
#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_RX_HDR_INTR_CLR_MASK    JAL_PKD_RX_HDR_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKD_INTR_CLR_MASK           JAL_PKD_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AUX_TIM_INTR_CLR_MASK       JAL_AUX_TIM_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_PKA_INTR_CLR_MASK           JAL_PKA_INTR_CLR_MASK

#define JAL_COM_CTRL_IRQ_REG_JAL_SER0_WRITE_CLR_MASK         JAL_SER0_WRITE_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_AES_INTR_CLR_MASK           JAL_AES_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_NO_PKT_RCVD_INTR_CLR_MASK   JAL_NO_PKT_RCVD_INTR_CLR_MASK
#define JAL_COM_CTRL_IRQ_REG_JAL_SYNC_DET_INTR_CLR_MASK      JAL_SYNC_DET_INTR_CLR_MASK

/*
 * Common Control Interrupt Register          Address 14H
 */
#define JAL_COM_STAT_IRQ_REG_ADDR                     (JAL_BASE_ADDR+0x14)

#define JAL_COM_STAT_IRQ_REG_JAL_TIM0_INTR_MASK            JAL_TIM0_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM1_INTR_MASK            JAL_TIM1_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM2_INTR_MASK            JAL_TIM2_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_TIM3_INTR_MASK            JAL_TIM3_INTR_MASK

#define JAL_COM_STAT_IRQ_REG_JAL_PKD_INTR_MASK             JAL_PKD_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_AUX_TIM_INTR_MASK         JAL_AUX_TIM_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_PKA_INTR_MASK             JAL_PKA_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_PKD_RX_HDR_INTR_MASK      JAL_PKD_RX_HDR_INTR_MASK

#define JAL_COM_STAT_IRQ_REG_JAL_AES_INTR_MASK             JAL_AES_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_NO_PKT_RCVD_INTR_MASK     JAL_NO_PKT_RCVD_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_SYNC_DET_INTR_MASK        JAL_SYNC_DET_INTR_MASK
#define JAL_COM_STAT_IRQ_REG_JAL_SLEEP_STATUS_MASK         JAL_SLEEP_STATUS_MASK


/*
 * Transmit Status Register                   Address 40H
 */
// Redundant
//#define JAL_TX_STATUS_REG_ADDR                    (JAL_BASE_ADDR+0x40)

/*
 * Receive Control Register                   Address 1CH
 */
#define JAL_RX_CTRL_REG_ADDR                      (JAL_BASE_ADDR+0x1C)

#define JAL_RX_CTRL_REG_JAL_RX_MODE_MASK         JAL_RX_MODE_MASK
#define JAL_RX_CTRL_REG_JAL_SYNC_ERR_MASK        JAL_SYNC_ERR_MASK
#define JAL_RX_CTRL_REG_JAL_WIN_EXT_MASK         JAL_WIN_EXT_MASK


/*
 * Serial Interface Adapter Register          Address 60H
 */
#define JAL_SER_CFG_REG_ADDR                     (JAL_BASE_ADDR+0x20)

#define JAL_SER_CFG_REG_JAL_SER_CFG_MASK         JAL_SER_CFG_MASK
#define JAL_SER_CFG_REG_JAL_SER_BUSY_MASK        JAL_SER_BUSY_MASK


#define JAL_SER_CTRL_REG_ADDR                    (JAL_BASE_ADDR+0x24)

#define JAL_SER_CTRL_REG_JAL_SER_TIME_MASK       JAL_SER_TIME_MASK
#define JAL_SER_CTRL_REG_JAL_SER_COMBINE_MASK    JAL_SER_COMBINE_MASK
#define JAL_SER_CTRL_REG_JAL_SER_MASK_MASK       JAL_SER_MASK_MASK
#define JAL_SER_CTRL_REG_JAL_SER_NOW_MASK        JAL_SER_NOW_MASK
#define JAL_SER_CTRL_REG_JAL_SER_SEQ_MASK        JAL_SER_SEQ_MASK


#define JAL_SER_DATA0_REG_ADDR                  (JAL_BASE_ADDR+0x30)
#define JAL_SER_DATA1_REG_ADDR                  (JAL_BASE_ADDR+0x34)
#define JAL_SER_DATA2_REG_ADDR                  (JAL_BASE_ADDR+0x38)
#define JAL_SER_DATA3_REG_ADDR                  (JAL_BASE_ADDR+0x3C)

/*
 * Definitions for the Enhanced SER block
 */
#define JAL_SER_ESER_CTRL_REG_ADDR                    (JAL_BASE_ADDR+0x24)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_COMBINE_MASK    (0x00070000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MASK_MASK       (0x0000FFFF)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_NOW_MASK        (0x00100000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_SEQ_MASK        (0x00200000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_DOUBLE_MASK     (0x00400000)
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_SEL_MASK    (0x00800000)

#define JAL_SER_ESER_CTRL_REG_JAL_SER_COMBINE_BIT_OFFSET    16
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MASK_BIT_OFFSET        0
#define JAL_SER_ESER_CTRL_REG_JAL_SER_NOW_BIT_OFFSET        20
#define JAL_SER_ESER_CTRL_REG_JAL_SER_SEQ_BIT_OFFSET        21
#define JAL_SER_ESER_CTRL_REG_JAL_SER_DOUBLE_BIT_OFFSET     22
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_SEL_BIT_OFFSET    23
#define JAL_SER_ESER_CTRL_REG_JAL_SER_MAP_BUSY_BIT_OFFSET   31



#define JAL_SER_ESER_TIME_REG_ADDR         (JAL_BASE_ADDR+0x28)
#define JAL_SER_ESER_TIME_0_MASK           (0x000F)
#define JAL_SER_ESER_TIME_1_MASK           (0x00F0)
#define JAL_SER_ESER_TIME_2_MASK           (0x0F00)
#define JAL_SER_ESER_TIME_3_MASK           (0xF000)

#define JAL_SER_ESER_TIME_0_BIT_OFFSET     0
#define JAL_SER_ESER_TIME_1_BIT_OFFSET     4
#define JAL_SER_ESER_TIME_2_BIT_OFFSET     8
#define JAL_SER_ESER_TIME_3_BIT_OFFSET     12



#define JAL_SER_ESER_DATA_ADDR                   (JAL_BASE_ADDR+0x50)
#define JAL_SER_ESER_DATA_0_ADDR                 (JAL_SER_ESER_DATA_ADDR)
#define JAL_SER_ESER_DATA_1_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*1)
#define JAL_SER_ESER_DATA_2_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*2)
#define JAL_SER_ESER_DATA_3_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*3)
#define JAL_SER_ESER_DATA_4_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*4)
#define JAL_SER_ESER_DATA_5_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*5)
#define JAL_SER_ESER_DATA_6_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*6)
#define JAL_SER_ESER_DATA_7_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*7)
#define JAL_SER_ESER_DATA_8_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*8)
#define JAL_SER_ESER_DATA_9_ADDR                 (JAL_SER_ESER_DATA_ADDR+4*9)
#define JAL_SER_ESER_DATA_10_ADDR                (JAL_SER_ESER_DATA_ADDR+4*10)
#define JAL_SER_ESER_DATA_11_ADDR                (JAL_SER_ESER_DATA_ADDR+4*11)
#define JAL_SER_ESER_DATA_12_ADDR                (JAL_SER_ESER_DATA_ADDR+4*12)
#define JAL_SER_ESER_DATA_13_ADDR                (JAL_SER_ESER_DATA_ADDR+4*13)
#define JAL_SER_ESER_DATA_14_ADDR                (JAL_SER_ESER_DATA_ADDR+4*14)
#define JAL_SER_ESER_DATA_15_ADDR                (JAL_SER_ESER_DATA_ADDR+4*15)


/* Following are not used in LE Single Mode but have to be dummied to minismise changes in
 * upper layer SW.
 */



/*******************************************************************
 *
 *   Define the bit positions for the BT Clock Controls(P6 only)
 *   (the clock gating signals are redundant)
 *******************************************************************/
#define JAL_ADD_BT_CLK_RELATIVE_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_ADD_BT_CLK_RELATIVE_MASK     0000000000
#define JAL_ADD_BT_CLK_RELATIVE_SHFT     0

#define JAL_WRITE_ABSOLUTE_BT_CLK_ADDR     (0x00000000 + JAL_BASE_ADDR)
#define JAL_WRITE_ABSOLUTE_BT_CLK_MASK     0x00000000
#define JAL_WRITE_ABSOLUTE_BT_CLK_SHFT     0

#endif

#endif
