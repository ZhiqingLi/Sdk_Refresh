#ifndef _PARTHUS_LSLC_ACC_
#define _PARTHUS_LSLC_ACC_
/***********************************************************************************************
 * MODULE NAME:    lslc_access.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    LSLC_Access
 * MAINTAINER:     John Nelson
 * DATE:           1 Jun 1999
 *
 * SOURCE CONTROL: $Id: lslc_access.h,v 1.72 2014/03/11 20:16:16 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     1 June 1999 -   jn       - Initial Version V0.9
 *
 * SOURCE: Based on V1_4 Design
 * ISSUES:
 * NOTES TO USERS:
 *
 *********************************************************************************************/
#include "sys_types.h"
#include "dl_dev.h"
#include "hw_radio.h"

/*
 * Packet determination macros based on packet type and bit packet type
 * Note macros are optimised by bit packing and comparing
 */

#define is_Control_Packet(pkt)                                 \
 ( ( (1 <<  (pkt)) & (  (1<<NULLpkt)|(1<<POLLpkt) |                       \
                      (1<<FHSpkt) |(1<<IDpkt)  ) ) )
#define is_Poll_Null_Packet(pkt)          ((pkt)<=POLLpkt)

#define is_SCO_Packet(pkt)                ((pkt)>=HV1 && (pkt)<=DV)

#define is_SYN_Packet(pkt)                (is_SCO_Packet((pkt))) || (is_eSCO_Packet((pkt)))

#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define mLSLCacc_Is_ACL_Packet(pkt)                                     \
 ( ( (1 <<  (pkt)) & (  ACL_PACKET_MASK | EDR_ACL_PACKET_MASK ) ) != 0)

#define is_Packet_Multi_Slot(pkt)                              \
 ( ( (1 <<  (pkt)) & (  ACL_MULTI_SLOT_PACKET_MASK | EDR_ACL_MULTI_SLOT_PACKET_MASK) )  != 0)

#define is_eSCO_Packet(pkt)         ( ( (1 <<  (pkt)) & (ESCO_PACKET_MASK | EDR_ESCO_PACKET_MASK)) != 0)

#define is_eSCO_Packet_3_Slot(pkt)        ( ( (1 <<  (pkt)) & (ESCO_THREE_SLOT_PACKET_MASK | EDR_ESCO_THREE_SLOT_PACKET_MASK  ) ) != 0)

#define is_eSCO_Packet_1_Slot(pkt)         ( ( (1 <<  (pkt)) & (ESCO_SINGLE_SLOT_PACKET_MASK | EDR_ESCO_SINGLE_SLOT_PACKET_MASK | (1<<NULLpkt) | (1<<POLLpkt)) )  != 0)

#define is_Packet_1_Slot(pkt)                                           \
 ( ( (1 <<  pkt) & (  (1<<NULLpkt)|(1<<POLLpkt)   |                     \
                      (1<<FHSpkt) |(1<<IDpkt)     |                     \
                      ACL_SINGLE_SLOT_PACKET_MASK |                     \
					  EDR_ACL_SINGLE_SLOT_PACKET_MASK |                 \
                      (1<<HV1)|(1<<HV2)|(1<<HV3)|(1<<DV) )  ) != 0)

#define is_ACL_CRC_Packet(pkt)                                          \
 ( ( (1 <<  (pkt)) & (  ACL_CRC_PACKET_MASK | EDR_ACL_CRC_PACKET_MASK ) ) != 0)

#define is_Packet_3_Slot(pkt)   ((pkt)==DM3 || (pkt)==DH3 || (pkt)==EDR_2DH3 || (pkt)==EDR_3DH3)

#define is_Packet_5_Slot(pkt)   ((pkt)==DM5 || (pkt)==DH5 || (pkt)==EDR_2DH5 || (pkt)==EDR_3DH5)

#define is_EDR_Packet(pkt) (((1 <<  (pkt)) &  EDR_PACKET_MASK) !=0)

#define is_EDR_ACL_packet(pkt) (((1 <<  (pkt)) &  EDR_ACL_PACKET_MASK) !=0)

#define is_EDR_2Mbit_packet(pkt) (((1 <<  (pkt)) &  EDR_2MBITS_PACKET_MASK) !=0)

#define is_EDR_3Mbit_packet(pkt) (((1 <<  (pkt)) &  EDR_3MBITS_PACKET_MASK) !=0)

#define is_EDR_ESCO_packet(pkt) (((1 <<  (pkt)) &  EDR_ESCO_PACKET_MASK) !=0)

#else /* Non - EDR */

#define mLSLCacc_Is_ACL_Packet(pkt)                                     \
 ( ( (1 <<  (pkt)) & (  (1<<DM1)|(1<<DM3)|(1<<DM5) | (1<<DV) |            \
                      (1<<DH1)|(1<<DH3)|(1<<DH5) | (1<<AUX1) ) ) != 0)

#define is_Packet_Multi_Slot(pkt)                              \
 ( ( (1 <<  (pkt)) & (  (1<<DM3)|(1<<DM5)|(1<<DH3)|(1<<DH5) ) ) != 0)

#define is_eSCO_Packet(pkt)               (((pkt)==EV3) || ((pkt)==EV4) || ((pkt)==EV5) )

#define is_eSCO_Packet_3_Slot(pkt)                (((pkt)==EV4) || ((pkt)==EV5))

#define is_eSCO_Packet_1_Slot(pkt)         (((pkt)==EV3) || ((pkt)==NULLpkt) || ((pkt)==POLLpkt) || ((pkt)==HV3))

#define is_Packet_1_Slot(pkt)                                           \
 ( ( (1 <<  (pkt)) & (  (1<<NULLpkt)|(1<<POLLpkt)   |                     \
                      (1<<FHSpkt) |(1<<IDpkt)     |                     \
                      (1<<DM1)|(1<<DH1)|(1<<AUX1) |                     \
                      (1<<HV1)|(1<<HV2)|(1<<HV3)|(1<<DV) )  ) != 0)

#define is_ACL_CRC_Packet(pkt)                                          \
 ( ( (1 <<  (pkt)) & (  (1<<DM1)|(1<<DM3)|(1<<DM5) | (1<<DV) |            \
                      (1<<DH1)|(1<<DH3)|(1<<DH5) ) ) != 0)

/*
 * could also check on type_code[2:1] - 10 => 3 slot, 11 => 5 slot
 */
#define is_Packet_3_Slot(pkt)           ((pkt)==DM3 || (pkt) == DH3)
#define is_Packet_5_Slot(pkt)           ((pkt)==DM5 || (pkt) == DH5)

#define is_EDR_Packet(pkt) (0) 

#define is_EDR_ACL_packet(pkt) (0) 

#define is_EDR_ESCO_packet(pkt) (0) 

#endif

extern const u_int8 LSLCacc_packet_slots[];
#define Packet_Slots(pkt) (LSLCacc_packet_slots[((pkt)&0xF)])

#define Derive_Packet_Bit_from_Type(pkt)    (1<<(pkt))

#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)

#define SYNC_DET_FLAG_MASK		0x01
#define NO_PKT_RCVD_FLAG_MASK   0x02
#define HEC_ERR_FLAG_MASK       0x04
#define CRC_ERR_FLAG_MASK       0x08
#define mSetChnlClsFlagBit(BitField)\
    {(chnl_cls_flag |= BitField##_MASK);}
#define mClrChnlClsFlagBit(BitField)\
    {(chnl_cls_flag &= (~BitField##_MASK));}
#define mIfChnlClsFlagBit(BitField)\
    (chnl_cls_flag & BitField##_MASK)
#define mClrChnlClsFlag()\
    {chnl_cls_flag = 0;}
    
#endif


DRAM_CODE void LSLCacc_Set_Am_Addr(t_am_addr am_addr);

DRAM_CODE void LSLCacc_Build_AccessCode(const t_syncword *syncword);
DRAM_CODE void LSLCacc_Build_PacketHeader(t_devicelink *pDevLink, t_packet packetType);

DRAM_CODE void LSLCacc_Build_FHS_Payload(t_devicelink *pDevLink);
DRAM_CODE void LSLCacc_Build_FHS_Payload_Ex(t_devicelink *pDevLink, t_clock native_clk);
void LSLCacc_Build_EIR_Payload(void);
void LSLCacc_Build_ACL_Payload(t_devicelink *pDevLink);
DRAM_CODE void LSLCacc_Build_SCO_Payload(const t_devicelink *pDevLink);

RAM_CODE void LSLCacc_Discard_SCO_Payload(const t_devicelink *p_dev_link);

void LSLCacc_Sync_Setup_SCO_CFG_Ex(t_sco_fifo sco_fifo, t_sco_cfg sco_cfg);

#if (PRH_BS_CFG_TABASCO_VERSION >= PRH_BS_HW_TABASCO_VERSION_T1_0_0)
void LSLCacc_Sync_Setup_SCO_ROUTE_Ex(t_sco_fifo sco_fifo, boolean sco_route);
__INLINE__ boolean LSLCacc_Get_SCO_ROUTE(t_sco_fifo sco_fifo);
#endif

DRAM_CODE boolean LSLCacc_Is_SCO_CFG_Via_ACL_Buffer(t_sco_fifo sco_fifo);
DRAM_CODE boolean LSLCacc_Is_SCO_CFG_Eight_Bit_Input_Mode(t_sco_fifo sco_fifo);

DRAM_CODE void LSLCacc_Prepare_Rx_ID(t_state context);
DRAM_CODE void LSLCacc_Prepare_Rx_FHS(t_state context, u_int crc_hec_init);
DRAM_CODE void LSLCacc_Prepare_Rx_Connection(const t_devicelink *p_dev_link);

#if (PRH_BS_CFG_SYS_RETURN_TO_PICONET_SUPPORTED==1)
RAM_CODE void LSLCacc_Prepare_Rx_Return_To_Piconet(const t_devicelink *p_dev_link);
#endif

void LSLCacc_Prepare_Tx_ID(t_state context);
RAM_CODE void LSLCacc_Prepare_Tx_Rx_FHS(t_state context, u_int crc_hec_init);
RAM_CODE void LSLCacc_Prepare_Tx_Rx_Connection(const t_devicelink *p_dev_link,
                                      u_int crc_hec_init,
                                      t_encrypt_mode encrypt_mode,
                                      t_state context);

void LSLCacc_Prepare_Tx_Rx_Encryption(
     const u_int8 *p_encryption_key, u_int8 encryption_key_length);

RAM_CODE void LSLCacc_Enable_Transmitter(void);
DRAM_CODE void LSLCacc_Disable_Transmitter(void);
DRAM_CODE void LSLCacc_Enable_Receiver(void);
RAM_CODE void LSLCacc_Disable_Receiver(void);

/*
 * Interrupt associated Receive Slot Processing
 */
DRAM_CODE void LSLCacc_Process_Rx_Header(t_devicelink *pDevLink);
RAM_CODE void LSLCacc_Process_No_Rx_Packet(t_devicelink *pDevLink);
DRAM_CODE void LSLCacc_Extract_Packet(t_devicelink *pDevLink);
void LSLCacc_Extract_FHS_Payload(t_FHSpacket *p_fhs_packet);
void LSLCacc_Extract_EIR_Payload(u_int8 *p_eir_packet, u_int8 rx_length);

void LSLCacc_Initialise(void);

#if (LC_DEFERRED_PAYLOAD_EXTRACTION_SUPPORTED==1)
void LSLCacc_Enable_Deferred_Extraction(boolean status);
void LSLCacc_Perform_Deferred_Payload_Extraction_If_Reqd(void);
#endif

#if (PRH_BS_CFG_SYS_LOW_POWER_MODE_SUPPORTED==1)
void LSLCacc_Enable_Low_Power_Mode(void);
void LSLCacc_Disable_Low_Power_Mode(void);
#endif

RAM_CODE void LSLCacc_Process_RSSI(t_devicelink *p_dev_link);
RAM_CODE void LSLCacc_Attempt_RSSI_Read(boolean);

#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
void _LSLCacc_Read_AGC_Param(void);
#endif
void _LSLCacc_Handle_AGC_Initialise(void);
void _LSLCacc_Handle_AGC_Deinitialise(void);
RAM_CODE void _LSLCacc_Handle_AGC_Channel_Assessment(t_freq rx_freq);
RAM_CODE void _LSLCacc_Handle_AGC_Update(t_freq rx_freq);
#endif
#if (CONFIG_CTRL_BQB_TEST_SUPPORT == 1)
#if (PRH_BS_CFG_SYS_LMP_POWER_CONTROL_SUPPORTED == 1)
DRAM_CODE void _LSLCacc_Align_Tx_Status_SEQN( t_devicelink *p_dev_link );
#endif
#endif
#endif
