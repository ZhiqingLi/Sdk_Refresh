#ifndef _PARTHUS_USLC_DEV_
#define _PARTHUS_USLC_DEV_

/******************************************************************************
 * MODULE NAME:    dl_dev.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC_DeviceLink
 * MAINTAINER:     John Nelson, Ivan Griffin, Conor Morris,Gary Fleming
 * CREATION DATE:  27 May 1999
 *
 * SOURCE CONTROL: $Id: dl_dev.h,v 1.102 2013/10/25 16:42:20 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

#include "sys_config.h"
#include "lc_types.h"
#include "bt_fhs.h"

#include "tra_queue.h"

/*
 * Constants for LC LINK and AMADDR allocation
 */
/*
 * Local Device Link is always allocated for the local device operations.
 */
#define LOCAL_DEVICE_INDEX              0
#define NO_LC_LINK_AVAILABLE            MAX_ACTIVE_DEVICE_LINKS
#define ALL_LC_LINKS_FREE               (( 1 << MAX_ACTIVE_DEVICE_LINKS) - \
                                         ( 1 << LOCAL_DEVICE_INDEX)  - 1 )

/*
 * AMADDR 0 is always reserved for the local device for broadcast.
 */
#define MAX_NUM_AMADDR                  8
#define NO_AMADDR_AVAILABLE             MAX_NUM_AMADDR
#define ALL_AMADDRS_FREE                (( 1 << MAX_NUM_AMADDR) - (1<<0) - 1)

/*
 * Piconet Index 0 is always reserved for the default master piconet
 */
#define MASTER_PICONET_INDEX            0
#define ALL_PICONETS_FREE              ((1 << PRH_BS_CFG_SYS_MAX_PICONETS) - \
                                        (1 << MASTER_PICONET_INDEX) - 1)

#define ESCO_LINK_ALLOCATED MAX_NUM_AMADDR
#define ESCO_LINK_FREE      0

/******************************************************************************
 * Non-inline functions defined in dl_dev.c
 ******************************************************************************/
void DL_Initialise_All(void);

t_deviceIndex DL_Alloc_Link(void);
void DL_Free_Link(t_deviceIndex device_index);

t_am_addr DL_Alloc_Am_Addr(void);
RAM_CODE void DL_Free_Am_Addr(t_am_addr am_addr);

t_am_addr DL_Alloc_eSCO_LT_Addr(void);
t_am_addr DL_Alloc_Next_Shared_eSCO_LT_Addr(void);
void DL_Record_eSCO_LT_Addr(t_am_addr lt_addr);
boolean DL_Is_eSCO_LT_Addr_Shared(t_am_addr lt_addr);
void DL_Free_eSCO_LT_Addr(t_am_addr lt_addr);

t_piconet_index DL_Assign_Piconet_Index(t_devicelink* p_device_link);
void DL_Free_Piconet_Index(t_piconet_index piconet_index);

void DL_Use_Local_Tx_Descriptor(t_devicelink* p_device_link);
void DL_Reset_New_Connection(t_devicelink *p_device_link, t_role role_of_peer);
void DL_Reset_RSSI_History(t_devicelink *p_device_link);

u_int8 DL_LT_ADDR_to_LT_INDEX_Mapping(t_devicelink* p_device_link, t_lt_addr lt_addr);


/******************************************************************************
 * Inline functions defined in dl_dev_impl.h
 ******************************************************************************/
__INLINE__ void DL_Set_UAP_LAP(t_devicelink* pDevLink, t_uap_lap uap_lap);
__INLINE__ t_uap_lap DL_Get_UAP_LAP(const t_devicelink* pDevLink);
__INLINE__ void DL_Set_NAP(t_devicelink* pDevLink, t_nap nap);
__INLINE__ t_nap DL_Get_NAP(const t_devicelink* pDevLink);
__INLINE__ void DL_Get_Bd_Addr_Ex(const t_devicelink* pDevLink, t_bd_addr* bda);

__INLINE__ void
    DL_Set_Ctrl_State(t_devicelink *p_device_link, t_device_ctrl_state ctrlState);
__INLINE__ t_device_ctrl_state
    DL_Get_Ctrl_State(const t_devicelink *p_device_link);

__INLINE__ void   DL_Set_Role_Peer(t_devicelink *p_device_link, u_int role);
__INLINE__ t_role DL_Get_Role_Peer(const t_devicelink *p_device_link);

__INLINE__ t_role DL_Get_Role_Local(const t_devicelink *p_device_link);

__INLINE__ void  DL_Set_AM_Addr(t_devicelink* p_device_link, u_int am_addr);
__INLINE__ t_am_addr DL_Get_Am_Addr(const t_devicelink* p_device_link);

__INLINE__ void  DL_Set_Piconet_Index(t_devicelink* p_device_link, u_int piconet_index);
__INLINE__ t_am_addr DL_Get_Piconet_Index(const t_devicelink* p_device_link);
__INLINE__ boolean DL_Max_Piconets(void);

__INLINE__ void
    DL_Set_Access_Syncword(t_devicelink* p_device_link, t_syncword sync);
__INLINE__ t_syncword
    DL_Get_Access_Syncword(const t_devicelink* p_device_link);
__INLINE__ const t_syncword*
    DL_Get_Access_Syncword_Ref(const t_devicelink* pDevLink);
__INLINE__ void
    DL_Get_Access_Syncword_Ex(const t_devicelink* pDevLink, t_syncword *p_sync);


__INLINE__ u_int16 DL_Get_Local_Slot_Offset(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Peer_Slot_Offset(t_devicelink* p_device_link, u_int16 offset);


/*
 *    Logical Transport address related DL accessors
 */

__INLINE__ boolean DL_Decode_Valid_LT_address(t_devicelink* p_device_link, u_int8 lt_addr);
__INLINE__ void DL_Set_Active_Tx_LT(t_devicelink* p_device_link, u_int8 active_lt);
__INLINE__ void DL_Set_Active_Rx_LT(t_devicelink* p_device_link, u_int8 active_lt);
__INLINE__ u_int8 DL_Get_Active_Rx_LT(t_devicelink* p_device_link);
__INLINE__ u_int8 DL_Get_Active_Tx_LT(t_devicelink* p_device_link);
__INLINE__ void DL_Set_LT_Address(t_devicelink* pDevLink, u_int8 lt_addr);
__INLINE__ u_int8 DL_Get_LT_Address(const t_devicelink* pDevLink);
__INLINE__ t_am_addr DL_Get_Tx_LT_Am_Addr(const t_devicelink* pDevLink);
__INLINE__ t_am_addr DL_Get_Rx_LT_Am_Addr(const t_devicelink* pDevLink);


#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
void DL_Set_Default_Rx_LT_All_Links(void);
void DL_Set_Default_Tx_LT_All_Links(void);
#else
/*
 *  Dummy function definitions to allow building the stack without eSCO.
 */
#define DL_Set_Default_Rx_LT_All_Links()
#define DL_Set_Default_Tx_LT_All_Links()
#endif

/*
 *    eSCO related DL accessors.
 */

__INLINE__ void DL_Set_eSCO_LT_Address(t_devicelink* p_device_link, u_int8 esco_lt_index, u_int8 lt_addr);

__INLINE__ u_int8 DL_Get_eSCO_LT_Address(const t_devicelink* p_device_link);

__INLINE__ void DL_Set_eSCO_Tx_Length(t_devicelink* p_device_link, u_int16 tx_length);

__INLINE__ u_int16 DL_Get_eSCO_Tx_Length(const t_devicelink* p_device_link);

__INLINE__ void DL_Set_eSCO_Rx_Length(t_devicelink* p_device_link, u_int16 rx_length);

__INLINE__ u_int16 DL_Get_eSCO_Rx_Length(const t_devicelink* p_device_link);

__INLINE__ void DL_Set_eSCO_Tx_Data(t_devicelink* p_device_link, u_int8* data);

__INLINE__ u_int8* DL_Get_eSCO_Tx_Data(const t_devicelink* p_device_link);

/*
 * Channel control arqn, flow, seqn and force transmit (POLL received)
 */

__INLINE__ u_int DL_Get_Tx_LT_Previous_Arqn(const t_devicelink* pDevLink);

__INLINE__ void DL_Set_Rx_LT_Previous_Arqn(t_devicelink* pDevLink, u_int arqn);



__INLINE__ void DL_Toggle_Rx_LT_Seqn(t_devicelink* pDevLink);

__INLINE__ void DL_Set_Tx_LT_Seqn(t_devicelink* pDevLink, u_int seqn);

__INLINE__ u_int DL_Get_Tx_LT_Seqn(const t_devicelink* pDevLink);

__INLINE__ void DL_Toggle_Tx_LT_Seqn(t_devicelink* pDevLink);

#if (PRH_BS_DEV_TOGGLE_TX_SEQN_ON_TX_CRC_PACKET ==1)
__INLINE__ void DL_Set_Tx_Seqn_Active(t_devicelink* pDevLink);
__INLINE__ void DL_Set_Tx_Seqn_Not_Active(t_devicelink* pDevLink);
__INLINE__ u_int8 DL_Is_Tx_Seqn_Active(t_devicelink* pDevLink);
#endif

__INLINE__ u_int DL_Get_Remote_Rxbuf_Full(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Remote_Rxbuf_Full(t_devicelink* p_device_link, u_int status);

__INLINE__ u_int DL_Get_Local_Rxbuf_Full(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Local_Rxbuf_Full(t_devicelink* p_device_link, u_int status);


__INLINE__ void DL_Set_Rx_LT_Expected_Seqn(t_devicelink* pDevLink, u_int seqn);

__INLINE__ u_int DL_Get_Rx_LT_Expected_Seqn(const t_devicelink* pDevLink);

__INLINE__ void DL_Toggle_Rx_LT_Expected_Seqn(t_devicelink* pDevLink);
__INLINE__ void DL_Toggle_Tx_LT_Expected_Seqn(t_devicelink* pDevLink);




__INLINE__ void  DL_Set_Rx_Expected_Broadcast_Seqn(t_devicelink* p_device_link, u_int seqn);
__INLINE__ void  DL_Toggle_Rx_Expected_Broadcast_Seqn(t_devicelink* p_device_link);
__INLINE__ u_int DL_Get_Rx_Expected_Broadcast_Seqn(const t_devicelink* p_device_link);

__INLINE__ u_int DL_Get_Tx_Ack_Pending(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Tx_Ack_Pending(t_devicelink* p_device_link, u_int ackPending);


__INLINE__ u_int DL_Get_Tx_LT_Tx_Ack_Pending(const t_devicelink* pDevLink);
__INLINE__ u_int DL_Get_Rx_LT_Tx_Ack_Pending(const t_devicelink* pDevLink);

__INLINE__ void DL_Set_Tx_LT_Tx_Ack_Pending(t_devicelink* pDevLink, u_int8 tx_ack_pending);
__INLINE__ void DL_Set_Rx_LT_Tx_Ack_Pending(t_devicelink* pDevLink, u_int ack_pending);


__INLINE__ t_TXRXstatus DL_Get_Rx_Status(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Rx_Status(t_devicelink* p_device_link, t_TXRXstatus status);

__INLINE__ u_int8 DL_Get_Power_Level(const t_devicelink* p_device_link);
__INLINE__ void   DL_Set_Power_Level(t_devicelink* p_device_link, u_int8 power_units);

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
u_int8 DL_Get_EPC_Power_Level(const t_devicelink* p_device_link, t_packet packetType);
__INLINE__ void DL_Enable_EPC(t_devicelink* pDevLink);
__INLINE__ boolean DL_Is_EPC_Enabled(t_devicelink* pDevLink);
boolean DL_Read_Current_Transmit_Power_Level(t_deviceIndex device_index, t_requested_transmit_power_level requested_transmit_power_level, 
											                           s_int8 *transmit_power_levels);

boolean DL_Power_Control_Req(t_deviceIndex device_index, t_packetTypes allowed_pkt_types, 
										   t_power_adjustment_req power_adjustment_req, t_powerAdjustmentResp *power_adjustment_resp);
#endif


__INLINE__ u_int16
    DL_Get_Local_Slot_Offset(const t_devicelink* p_device_link);
__INLINE__ void
    DL_Set_Local_Slot_Offset(t_devicelink* p_device_link, u_int16 offset);
__INLINE__ u_int16
    DL_Get_Peer_Slot_Offset(const t_devicelink* p_device_link);
__INLINE__ void
    DL_Set_Peer_Slot_Offset(t_devicelink* p_device_link, u_int16 offset);

__INLINE__ u_int32
    DL_Get_Piconet_Clock_Last_Access(const t_devicelink* p_device_link);
__INLINE__ void
    DL_Set_Piconet_Clock_Last_Access(t_devicelink* p_device_link, u_int32 clock);

__INLINE__ u_int8
    DL_Get_Clock_Jitter(const t_devicelink* p_device_link);
__INLINE__ void
    DL_Set_Clock_Jitter(t_devicelink* p_device_link, u_int8 jitter);

__INLINE__ u_int8
    DL_Get_Clock_Drift(const t_devicelink* p_device_link);
__INLINE__ void
    DL_Set_Clock_Drift(t_devicelink* p_device_link, u_int8 drift);


__INLINE__ u_int
    DL_Is_Local_Buffer_Full(const t_devicelink* p_device_link);


/*
 *  deviceLink tx and rx descriptor related
 */
__INLINE__ t_packet DL_Get_Rx_Packet_Type(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Rx_Packet_Type(t_devicelink* p_device_link,t_packet type);
__INLINE__ void DL_Set_Tx_Descriptor_Ref(t_devicelink* pDevLink, struct q_desc *tx_desc);
__INLINE__ t_packet DL_Get_Tx_Packet_Type(const t_devicelink* p_device_link);
__INLINE__ t_length DL_Get_Tx_Length(const t_devicelink* p_device_link);
__INLINE__ u_int8*  DL_Get_Tx_Qpdu(const t_devicelink* p_device_link);
 #ifdef UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK
__INLINE__ u_int8 DL_Get_Tx_Opcode_Pending(const t_devicelink* pDevLink);
 #endif
__INLINE__ t_LCHmessage DL_Get_Tx_Message_Type(const t_devicelink* p_device_link);

__INLINE__ void DL_Set_Tx_Packet_Type(t_devicelink* p_device_link, t_packet type);
__INLINE__ void DL_Set_Tx_Length(t_devicelink* p_device_link, t_length length);
__INLINE__ void DL_Set_Tx_Qpdu(t_devicelink* p_device_link, u_int8* buffer);
__INLINE__ void DL_Set_Tx_Message_Type(t_devicelink* p_device_link, t_LCHmessage type);
#ifdef UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK
__INLINE__ void DL_Set_Tx_Callback(t_devicelink* pDevLink, void* ptr );
#endif
/*
 * Local device Rx/Tx L2CAP Flow
 */
__INLINE__ t_flow   DL_Get_Local_Rx_L2CAP_Flow(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Local_Rx_L2CAP_Flow(t_devicelink* p_device_link, t_flow flow);
__INLINE__ t_flow   DL_Get_Local_Tx_L2CAP_Flow(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Local_Tx_L2CAP_Flow(t_devicelink* p_device_link, t_flow flow);

/*
 * SCO related, From P7D only active_sco_pkt and active_sco_fifo are appropriate
 * There is a one to one correspondence of LM sco_index and LC sco_fifo
 */
__INLINE__ t_packet DL_Get_Tx_SCO_Packet_Type(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Tx_SCO_Packet_Type(t_devicelink* p_device_link, t_packet type);
__INLINE__ u_int8 DL_Get_Active_SCO_Index(const t_devicelink* p_device_link);
__INLINE__ void DL_Set_Active_SCO_Index(t_devicelink* p_device_link, u_int8 index);

/*
 * Encryption related methods
 */
__INLINE__ void
    DL_Set_Encryption_Mode(t_devicelink* p_device_link,t_encrypt_mode encrypt_mode);
__INLINE__ t_encrypt_mode DL_Get_Encryption_Mode(const t_devicelink* p_device_link);

__INLINE__ void
    DL_Set_Encryption_Key_Ref(t_devicelink* p_device_link, u_int8 *p_encryption_key);
__INLINE__ u_int8* DL_Get_Encryption_Key_Ref(const t_devicelink* p_device_link);

__INLINE__ void
    DL_Set_Encryption_Key_Length(t_devicelink* p_device_link, u_int8 encryption_key_length);

__INLINE__ u_int8 DL_Get_Encryption_Key_Length(const t_devicelink* p_device_link);

/*
 *  Clock Offset of Remote Device Accessor/Modifier (relative to native clock)
 */
__INLINE__ void DL_Set_Clock_Offset(t_devicelink* p_device_link, t_clock clk);
__INLINE__ t_clock DL_Get_Clock_Offset(const t_devicelink* p_device_link);

/*
 * Functions to return device link index/reference for local device
 * and functions to map device link indexes to device link references and vice versa
 */
__INLINE__ t_devicelink* DL_Get_Local_Device_Ref(void);
__INLINE__ t_deviceIndex DL_Get_Local_Device_Index(void);
__INLINE__ t_devicelink* DL_Get_Device_Ref(t_deviceIndex device_index);
__INLINE__ t_deviceIndex DL_Get_Device_Index(const t_devicelink* p_device_link);

#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)

t_parkDeviceIndex DL_Park(t_deviceIndex device_index, t_pm_addr pm_addr);
t_deviceIndex DL_Unpark(t_parkDeviceIndex park_device_index, t_role role_of_peer, 
                                        t_am_addr am_addr);
t_parkDeviceIndex DL_Alloc_Park_Link(void);
void DL_Free_Park_Link(t_parkDeviceIndex park_device_index);

void DL_Unpark_Commit(t_parkDeviceIndex park_device_index, t_role role_of_peer);
void DL_Unpark_Rollback(t_deviceIndex device_index);

__INLINE__ u_int DL_Parked_Device_Exists(void);

#endif /* (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1) */

#if (PRH_BS_CFG_SYS_LMP_RSSI_SUPPORTED==1)
__INLINE__ s_int8 DL_Get_RSSI(t_devicelink* p_device_link);
__INLINE__ s_int8 DL_Get_RSSI_For_Last_Packet_Received(t_devicelink* p_device_link);
__INLINE__ s_int8 DL_Get_1Mb_RSSI(t_devicelink* p_device_link);
__INLINE__ s_int8 DL_Get_2Mb_RSSI(t_devicelink* p_device_link);
__INLINE__ s_int8 DL_Get_3Mb_RSSI(t_devicelink* p_device_link);
void DL_Update_RSSI_Average(t_devicelink* p_device_link, s_int8 rssi);
#endif

#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
/*
 * For Channel Driven Quality Support must monitor number Tx/Rx retransmissions
 */
__INLINE__ void  DL_Inc_Tx_ACL_Data_Packets(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Tx_ACL_Data_Packets_Acked(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Tx_SYN_Data_Packets(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Tx_SYN_Data_Packets_Acked(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Rx_ACL_Data_Packets(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Rx_ACL_Data_Packets_Nacked(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Rx_SYN_Data_Packets(t_devicelink* p_device_link);
__INLINE__ void  DL_Inc_Rx_SYN_Data_Packets_Nacked(t_devicelink* p_device_link);
__INLINE__ const t_ch_quality *DL_Get_Channel_Quality_Ref(const t_devicelink* p_device_link);
__INLINE__ void  DL_Initialise_Channel_Quality(t_devicelink* p_device_link);
/* __INLINE__ t_packet DL_Get_L2CAP_Rx_Packet_Type(const t_devicelink* p_device_link); */
#else
/*
 * Dummy procedures for Channel Driven Quality
 */
#define DL_Inc_Tx_ACL_Data_Packets(p_device_link)
#define DL_Inc_Tx_ACL_Data_Packets_Acked(p_device_link)
#define DL_Inc_Tx_SYN_Data_Packets(p_device_link)
#define DL_Inc_Tx_SYN_Data_Packets_Acked(p_device_link)
#define DL_Inc_Rx_ACL_Data_Packets(p_device_link)
#define DL_Inc_Rx_ACL_Data_Packets_Nacked(p_device_link)
#define DL_Inc_Rx_SYN_Data_Packets(p_device_link)
#define DL_Inc_Rx_SYN_Data_Packets_Nacked(p_device_link)
#define DL_Get_Channel_Quality_Ref(p_device_link)
#define DL_Initialise_Channel_Quality(p_device_link)
/* #define DL_Get_L2CAP_Rx_Packet_Type(p_device_link); */

#endif  /*(PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)*/

/*
 * Channel control arqn, seqn for eSCO
 */
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
#if (ESCO_LINK_CRC_ERROR_CHANGE_TO_GOOD == 1)
__INLINE__ u_int DL_Get_Rx_eSCO_To_Good(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Rx_eSCO_To_Good(t_devicelink* p_device_link, u_int good);
#endif
__INLINE__ void  DL_Init_Rx_eSCO_Pending(t_devicelink* p_device_link,t_packet packet);
__INLINE__ u_int DL_Get_Rx_eSCO_Pending(const t_devicelink* p_device_link);
__INLINE__ void  DL_Set_Rx_eSCO_Pending(t_devicelink* p_device_link, u_int rxPending);
RAM_CODE u_int8 DL_Map_LT_ADDR_to_LT_Handle(t_devicelink* p_device_link, t_lt_addr lt_addr);
u_int8 DL_Allocate_eSCO_Link(t_devicelink* p_device_link);
__INLINE__ void DL_Free_eSCO_Link(t_devicelink* p_device_link, u_int8 esco_lt_index);
#endif

/*
 * EDR related
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
__INLINE__ boolean DL_Is_EDR_Enabled(const t_devicelink* pDevLink);
__INLINE__ void DL_Set_PTT_ACL(t_devicelink* pDevLink, boolean  ptt);
__INLINE__ boolean DL_Get_PTT_ACL(const t_devicelink* pDevLink);
__INLINE__ void DL_Set_PTT_ESCO(t_devicelink* pDevLink, boolean ptt);
__INLINE__ boolean DL_Get_PTT_ESCO(const t_devicelink* pDevLink);
#endif

// Erroneous Data Transfer
__INLINE__ void DL_Set_Rx_SYNC_Erroneous_Data_Descriptor(t_devicelink* pDevLink,t_q_descr *p_descriptor);
__INLINE__ t_q_descr* DL_Get_Rx_SYNC_Erroneous_Data_Descriptor(t_devicelink* pDevLink);

__INLINE__ u_int8 DL_Get_Rx_SYNC_Erroneous_Data_Status(t_devicelink* pDevLink);
__INLINE__ void DL_Set_Rx_SYNC_Erroneous_Data_Status(t_devicelink* pDevLink,u_int8 status);

#ifdef __USE_INLINES__
#include "dl_dev_impl.h"
#endif

#endif

