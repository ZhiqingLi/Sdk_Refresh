#ifndef _PARTHUS_USLC_DEV_IMPL_
#define _PARTHUS_USLC_DEV_IMPL_

/******************************************************************************
 * MODULE NAME:    dl_dev_impl.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC_DeviceLink
 * MAINTAINER:     John Nelson,Ivan Griffin, Conor Morris, Gary Fleming
 * DATE:           27 May 1999
 *
 * SOURCE CONTROL: $Id: dl_dev_impl.h,v 1.123 2014/05/19 19:26:21 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

/*
 * Asserts are used here only for test.  An extra check!
 */
#ifndef assert
#include <assert.h>
#endif

#include "sys_config.h"
#include "lc_types.h"
#include "dl_dev.h"
#include "hw_lc.h"
#include "lslc_access.h"

#include "lmp_sco_container.h"

#if (PRAGMA_INLINE==1)
#pragma inline(DL_Initialise,\
DL_Set_Ctrl_State,\
DL_Get_Ctrl_State,\
DL_Set_Role_Peer,\
DL_Get_Role_Peer,\
DL_Get_Role_Local,\
DL_Set_AM_Addr,\
DL_Get_Am_Addr,\
DL_Get_Tx_LT_Am_Addr,\
DL_Get_Rx_LT_Am_Addr,\
DL_Set_Access_Syncword,\
DL_Get_Access_Syncword,\
DL_Get_Access_Syncword_Ref,\
DL_Get_Remote_Rxbuf_Full,\
DL_Set_Remote_Rxbuf_Full,\
DL_Get_Local_Rxbuf_Full,\
DL_Set_Local_Rxbuf_Full,\
DL_Set_Rx_Expected_Seqn,\
DL_Set_Rx_Expected_Broadcast_Seqn,\
DL_Toggle_Rx_Expected_Broadcast_Seqn,\
DL_Get_Rx_Expected_Broadcast_Seqn,\
DL_Get_Tx_Ack_Pending,\
DL_Set_Tx_Ack_Pending,\
DL_Get_Tx_toMaster,\
DL_Set_Tx_toMaster,\
DL_Get_Rx_Status,\
DL_Set_Rx_Status,\
DL_Get_Power_Level,\
DL_Set_Power_Level,\
DL_Get_Local_Slot_Offset,\
DL_Set_Local_Slot_Offset,\
DL_Get_Peer_Slot_Offset,\
DL_Set_Peer_Slot_Offset,\
DL_Get_Piconet_Clock_Last_Access,\
DL_Set_Piconet_Clock_Last_Access,\
DL_Get_Clock_Jitter,\
DL_Set_Clock_Jitter,\
DL_Get_Clock_Drift,\
DL_Set_Clock_Drift,\
DL_Is_Local_Rx_Buffer_Full,\
DL_Get_Rx_Packet_Type,\
DL_Get_Rx_Length,\
DL_Get_Rx_Payload,\
DL_Get_Rx_Qpdu,\
DL_Get_Rx_Message_Type,\
DL_Set_Rx_Packet_Type,\
DL_Set_Rx_Length,\
DL_Set_Rx_Payload,\
DL_Set_Rx_Message_Type,\
DL_Get_Tx_Packet_Type,\
DL_Get_Tx_Length,\
DL_Get_Tx_Payload,\
DL_Get_Tx_Qpdu,\
DL_Get_Tx_Message_Type,\
DL_Set_Tx_Packet_Type,\
DL_Set_Tx_Length,\
DL_Set_Tx_Payload,\
DL_Set_Tx_Qpdu,\
DL_Set_Tx_Message_Type,\
DL_Set_Encryption_Mode,\
DL_Get_Encryption_Mode,\
DL_Set_Encryption_Key_Ref,\
DL_Get_Encryption_Key_Ref,\
DL_Set_Encryption_Key_Length,\
DL_Get_Encryption_Key_Length,\
DL_Get_Local_Device_Ref,\
DL_Get_Local_Device_Index,\
DL_Get_Device_Ref,\
DL_Get_Device_Index,\
DL_Get_Local_Tx_L2CAP_Flow,\
DL_Set_Local_Tx_L2CAP_Flow,\
DL_Get_Local_Rx_L2CAP_Flow,\
DL_Set_Local_Rx_L2CAP_Flow,\
DL_Get_Tx_SCO_Packet_Type,\
DL_Set_Tx_SCO_Packet_Type,\
DL_Get_Active_SCO_Index,\
DL_Set_Active_SCO_Index,\
DL_Decode_Valid_LT_address,\
DL_Set_Active_Tx_LT,\
DL_Set_Active_Rx_LT,\
DL_Get_Active_Rx_LT,\
DL_Get_Active_Tx_LT,\
DL_Set_eSCO_LT_Address,\
DL_Get_eSCO_LT_Address,\
DL_Get_eSCO_Rx_Length,\
DL_Set_eSCO_Rx_Length,\
DL_Set_LT_Address,\
DL_Get_LT_Address,\
DL_Get_Tx_LT_Previous_Arqn,\
DL_Set_Rx_LT_Previous_Arqn,\
DL_Toggle_Rx_LT_Seqn,\
DL_Set_Tx_LT_Seqn,\
DL_Get_Tx_LT_Seqn,\
DL_Toggle_Tx_LT_Seqn,\
DL_Set_Rx_LT_Expected_Seqn,\
DL_Get_Rx_LT_Expected_Seqn,\
DL_Toggle_Rx_LT_Expected_Seqn,\
DL_Get_Tx_LT_Tx_Ack_Pending,\
DL_Set_Tx_LT_Tx_Ack_Pending,\
DL_Get_Rx_LT_Tx_Ack_Pending,\
DL_Set_Rx_LT_Tx_Ack_Pending,\
DL_Set_UAP_LAP,\
DL_Get_UAP_LAP,\
DL_Set_NAP,\
DL_Get_NAP,\
DL_Get_Bd_Addr_Ex,\
DL_Get_Clock_Offset,\
DL_Set_Clock_Offset,\
DL_Set_Tx_Seqn_Active,\
DL_Set_Tx_Seqn_Not_Active,\
DL_Is_Tx_Seqn_Active\
)

#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
#pragma inline(DL_Inc_Tx_ACL_Data_Packets,\
DL_Inc_Tx_ACL_Data_Packets_Acked,\
DL_Inc_Rx_ACL_Data_Packets,\
DL_Inc_Rx_ACL_Data_Packets_Nacked,\
DL_Inc_Tx_SYN_Data_Packets,\
DL_Inc_Tx_SYN_Data_Packets_Acked,\
DL_Inc_Rx_SYN_Data_Packets,\
DL_Inc_Rx_SYN_Data_Packets_Nacked,\
DL_Get_Channel_Quality,\
DL_Initialise_Channel_Quality)
#endif  /*(PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)*/


#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#pragma inline(DL_Is_EDR_Enabled,\
DL_Set_PTT_ACL,\
DL_Get_PTT_ACL,\
DL_Set_PTT_ESCO,\
DL_Get_PTT_ESCO)
#endif


#endif

/********************************************************************
 * DL_Get_Local_Device_Ref
 * Get the Device Link ptr for the local device.
 ********************************************************************/
__INLINE__ t_devicelink* DL_Get_Local_Device_Ref(void)
{
    t_devicelink* p_device_link = (t_devicelink*) SYSconfig_Get_Device_Links_Location();
    return &p_device_link[SYSconfig_Get_Local_Device_Index()];
}

/********************************************************************
 * DL_Get_Local_Device_Index
 * Get the Device Link index for the local device.
 * This is stored in the SYSconfig struture.
 ********************************************************************/
__INLINE__ t_deviceIndex DL_Get_Local_Device_Index(void)
{
   return (t_deviceIndex) SYSconfig_Get_Local_Device_Index();
}

/********************************************************************
 * DL_Get_Device_Ref
 * Get the Device Link ptr for the device indicated by deviceIndex
 ********************************************************************/
__INLINE__ t_devicelink* DL_Get_Device_Ref(t_deviceIndex device_index)
{
    t_devicelink* p_device_link = (t_devicelink*) SYSconfig_Get_Device_Links_Location();
    return &p_device_link[device_index];
}

/********************************************************************
 * DL_Get_Device_Index
 * Get the Device Index for the Device Link Pointer (Reference)
 ********************************************************************/
__INLINE__ t_deviceIndex DL_Get_Device_Index(const t_devicelink* p_device_link)
{
   return p_device_link->device_index;
}
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
__INLINE__ t_deviceIndex DL_Get_Device_Index_By_Am_Addr(u_int am_addr)
{
    t_devicelink* p_device_link = (t_devicelink*) SYSconfig_Get_Device_Links_Location();
    t_devicelink* p_tmp_dl;
    u_int8 i;
    for(i=1;i<MAX_ACTIVE_DEVICE_LINKS;i++)
    {
        p_tmp_dl = &p_device_link[i];
        if((p_tmp_dl) && (am_addr == DL_Get_Am_Addr(p_tmp_dl)))
        {
            return p_tmp_dl->device_index;
        }
    }
    return 0;
}
#endif
/********************************************************************
 *
 * Modifiers and Accessors for the members of the device link object
 *
 ********************************************************************/
__INLINE__ void DL_Set_Role_Peer(t_devicelink* pDevLink, u_int role_of_peer)
{
   pDevLink->role_of_peer=role_of_peer;
}

__INLINE__ t_role DL_Get_Role_Peer(const t_devicelink* pDevLink)
{
   return (t_role)pDevLink->role_of_peer;
}

__INLINE__ t_role DL_Get_Role_Local(const t_devicelink* pDevLink)
{
   return (1 ^ (t_role)pDevLink->role_of_peer);
}

__INLINE__ void DL_Set_Ctrl_State(t_devicelink *pDevLink,
                                               t_device_ctrl_state ctrl_state)
{
   pDevLink->link_ctrl_state = ctrl_state;
}

__INLINE__ t_device_ctrl_state DL_Get_Ctrl_State(const t_devicelink *pDevLink)
{
   return pDevLink->link_ctrl_state;
}

__INLINE__ t_am_addr DL_Get_Am_Addr(const t_devicelink* p_device_link)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    return p_device_link->am_addr;
#else
    return p_device_link->lt_info[ACL_SCO_LT].lt_addr;
#endif
}

__INLINE__ void DL_Set_AM_Addr(t_devicelink* p_device_link, u_int am_addr)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    p_device_link->am_addr=am_addr;
#else
    p_device_link->lt_info[ACL_SCO_LT].lt_addr=am_addr;
#endif
}

__INLINE__ t_am_addr DL_Get_Tx_LT_Am_Addr(const t_devicelink* p_device_link)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    return p_device_link->am_addr;
#else
    return p_device_link->lt_info[p_device_link->tx_lt_index].lt_addr;
#endif
}



__INLINE__ t_am_addr DL_Get_Rx_LT_Am_Addr(const t_devicelink* p_device_link)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0)
    return p_device_link->am_addr;
#else
    return p_device_link->lt_info[p_device_link->rx_lt_index].lt_addr;
#endif
}


__INLINE__ void  DL_Set_Piconet_Index(t_devicelink* p_device_link, u_int piconet_index)
{
    p_device_link->piconet_index = piconet_index;
}

__INLINE__ u_int8 DL_Get_Piconet_Index(const t_devicelink* p_device_link)
{
    return p_device_link->piconet_index;
}

__INLINE__ boolean DL_Max_Piconets(void)
{
    extern u_int8 DL_piconet_index_pool;
    return (DL_piconet_index_pool==0);
}

__INLINE__ t_syncword DL_Get_Access_Syncword(const t_devicelink* pDevLink)
{
    return pDevLink->access_syncword;
}

__INLINE__ const t_syncword* DL_Get_Access_Syncword_Ref(const t_devicelink* pDevLink)
{
    return &pDevLink->access_syncword;
}

__INLINE__ void DL_Set_Access_Syncword(t_devicelink* pDevLink, t_syncword sync)
{
    pDevLink->access_syncword = sync;
}

/*
 * Channel control arqn, flow, seqn and force transmit (POLL received)
 */
__INLINE__ u_int DL_Get_Remote_Rxbuf_Full(const t_devicelink* pDevLink)
{
    return (pDevLink->remote_rx_buffer_full);
}

__INLINE__ void DL_Set_Remote_Rxbuf_Full(t_devicelink* pDevLink, u_int status)
{
    pDevLink->remote_rx_buffer_full = status;
}

__INLINE__ void DL_Set_Local_Rxbuf_Full(t_devicelink* pDevLink, u_int status)
{
    pDevLink->local_rx_buffer_full = status;
}

__INLINE__ u_int DL_Get_Local_Rxbuf_Full(const t_devicelink* pDevLink)
{
    return (pDevLink->local_rx_buffer_full);
}


__INLINE__ void DL_Set_Rx_Expected_Broadcast_Seqn(t_devicelink* pDevLink, u_int seqn)
{
    pDevLink->rx_expected_broadcast_seqn = seqn;
}

__INLINE__ void DL_Toggle_Rx_Expected_Broadcast_Seqn(t_devicelink* pDevLink)
{
    pDevLink->rx_expected_broadcast_seqn ^= 1;
}

__INLINE__ u_int DL_Get_Rx_Expected_Broadcast_Seqn(const t_devicelink* pDevLink)
{
    return pDevLink->rx_expected_broadcast_seqn;
}

/*
 *
 * CM: 22 SEP 03
 * Ideally this interface should be removed and 
 * replaced with accessors that use the active
 * tx_lt and rx_lt.
 *
 */
__INLINE__ u_int DL_Get_Tx_Ack_Pending(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return pDevLink->lt_info[ACL_SCO_LT].tx_ack_pending;
#else
    return pDevLink->tx_ack_pending;
#endif
}

/*
 *
 * CM: 22 SEP 03
 * Ideally this interface should be removed and 
 * replaced with accessors that use the active
 * tx_lt and rx_lt.
 *
 */
__INLINE__ void DL_Set_Tx_Ack_Pending(t_devicelink* pDevLink, u_int ack_pending)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[ACL_SCO_LT].tx_ack_pending = ack_pending;
#else
    pDevLink->tx_ack_pending = ack_pending;
#endif
}

/*
 *    DL_Set_Tx_LT_Tx_Ack_Pending
 */
__INLINE__ void DL_Set_Tx_LT_Tx_Ack_Pending(t_devicelink* pDevLink, u_int8 tx_ack_pending)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->tx_lt_index].tx_ack_pending = tx_ack_pending;
#else
    pDevLink->tx_ack_pending = tx_ack_pending;
#endif
}

/*
 *    DL_Get_Tx_LT_Tx_Ack_Pending
 */
__INLINE__ u_int DL_Get_Tx_LT_Tx_Ack_Pending(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return pDevLink->lt_info[pDevLink->tx_lt_index].tx_ack_pending;
#else
    return pDevLink->tx_ack_pending;
#endif
}

/*
 *    DL_Get_Rx_LT_Tx_Ack_Pending
 */
__INLINE__ u_int DL_Get_Rx_LT_Tx_Ack_Pending(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return pDevLink->lt_info[pDevLink->rx_lt_index].tx_ack_pending;
#else
    return pDevLink->tx_ack_pending;
#endif
}

/*
 *    DL_Set_Rx_LT_Tx_Ack_Pending
 */
__INLINE__ void DL_Set_Rx_LT_Tx_Ack_Pending(t_devicelink* pDevLink, u_int ack_pending)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->rx_lt_index].tx_ack_pending = ack_pending;
#else
    pDevLink->tx_ack_pending = ack_pending;
#endif
}

__INLINE__ t_TXRXstatus DL_Get_Rx_Status(const t_devicelink* pDevLink)
{
    return pDevLink->rx_status;
}

__INLINE__ void DL_Set_Rx_Status(t_devicelink* pDevLink, t_TXRXstatus status)
{
    pDevLink->rx_status = status;
}

__INLINE__ u_int8 DL_Get_Power_Level(const t_devicelink* pDevLink)
{
    return pDevLink->power_level;
}

__INLINE__ void DL_Set_Power_Level(t_devicelink* pDevLink, u_int8 power_units)
{
#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==0)
	pDevLink->power_level = power_units;
#else
	// first assign in the new power units -
    pDevLink->power_level = pDevLink->power_level_2Mbits = pDevLink->power_level_3Mbits = power_units;
	// upper bound check -
	if (pDevLink->power_level > MAX_POWER_LEVEL_1MBITS)
		pDevLink->power_level = MAX_POWER_LEVEL_1MBITS;
	if (pDevLink->power_level_2Mbits > MAX_POWER_LEVEL_2MBITS) 
		pDevLink->power_level_2Mbits = MAX_POWER_LEVEL_2MBITS;
	if (pDevLink->power_level_3Mbits > MAX_POWER_LEVEL_3MBITS) 
		pDevLink->power_level_3Mbits = MAX_POWER_LEVEL_3MBITS;
	// lower bound check -
#if MIN_POWER_LEVEL_1MBITS
	if (pDevLink->power_level < MIN_POWER_LEVEL_1MBITS)
		pDevLink->power_level = MIN_POWER_LEVEL_1MBITS;
#endif
#if MIN_POWER_LEVEL_2MBITS
	if (pDevLink->power_level_2Mbits < MIN_POWER_LEVEL_2MBITS) 
		pDevLink->power_level_2Mbits = MIN_POWER_LEVEL_2MBITS;
#endif
#if MIN_POWER_LEVEL_3MBITS
	if (pDevLink->power_level_3Mbits < MIN_POWER_LEVEL_3MBITS) 
		pDevLink->power_level_3Mbits = MIN_POWER_LEVEL_3MBITS;
#endif
#endif
}

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
__INLINE__ void DL_Enable_EPC(t_devicelink* pDevLink)
{
	pDevLink->epc_enabled = TRUE;
}

__INLINE__ boolean DL_Is_EPC_Enabled(t_devicelink* pDevLink)
{
	extern boolean epc_enabled /* global TCI override */;
	return ((pDevLink->epc_enabled) && (epc_enabled));
}
#endif

__INLINE__ u_int16 DL_Get_Local_Slot_Offset(const t_devicelink* p_device_link)
{
    return p_device_link->local_slot_offset_us;
}
__INLINE__ void DL_Set_Local_Slot_Offset(t_devicelink* p_device_link, u_int16 offset)
{
    p_device_link->local_slot_offset_us = offset;
}

__INLINE__ u_int16 DL_Get_Peer_Slot_Offset(const t_devicelink* p_device_link)
{
    return p_device_link->peer_slot_offset_us;
}

__INLINE__ void DL_Set_Peer_Slot_Offset(t_devicelink* p_device_link, u_int16 offset)
{
    p_device_link->peer_slot_offset_us = offset;
}

__INLINE__ u_int32
    DL_Get_Piconet_Clock_Last_Access(const t_devicelink* p_device_link)
{
    return p_device_link->piconet_clock_last_access;
}

__INLINE__ void
    DL_Set_Piconet_Clock_Last_Access(t_devicelink* p_device_link, u_int32 bt_clock)
{
    p_device_link->piconet_clock_last_access = bt_clock;

}

__INLINE__ u_int8  DL_Get_Clock_Jitter(const t_devicelink* p_device_link)
{
    return p_device_link->clock_jitter_us;
}
__INLINE__ void
    DL_Set_Clock_Jitter(t_devicelink* p_device_link, u_int8 jitter)
{
    p_device_link->clock_jitter_us = jitter;
}

__INLINE__ u_int8
    DL_Get_Clock_Drift(const t_devicelink* p_device_link)
{
    return p_device_link->clock_drift_ppm;
}
__INLINE__ void
    DL_Set_Clock_Drift(t_devicelink* p_device_link, u_int8 drift)
{
    p_device_link->clock_drift_ppm = drift;
}


__INLINE__ u_int DL_Is_Local_Buffer_Full(const t_devicelink* pDevLink)
{
/*
 * For now take status of local_rx_buf bit
 * Later may actually check again and see if this can be over-ruled.
 */

    return DL_Get_Local_Rxbuf_Full(pDevLink);
}

/*
 *  deviceLink tx descriptor and rx information  related
 */
__INLINE__ t_packet DL_Get_Rx_Packet_Type(const t_devicelink* pDevLink)
{
    return (t_packet) pDevLink->packet_type;
}

__INLINE__ void DL_Set_Rx_Packet_Type(t_devicelink* pDevLink,t_packet type)
{
    pDevLink->packet_type = (u_int8) type;
}

/*
 * Setups up the descriptor to the given transmit descriptor
 */
__INLINE__ void DL_Set_Tx_Descriptor_Ref(                                    
     t_devicelink* pDevLink, struct q_desc *tx_descr)
{
    pDevLink->tx_descriptor = tx_descr;
}

__INLINE__ t_packet DL_Get_Tx_Packet_Type(const t_devicelink* pDevLink)
{
    assert(pDevLink != NULL);
    assert(pDevLink->tx_descriptor != 0);
    return (t_packet)pDevLink->tx_descriptor->packet_type;
}

__INLINE__ t_length DL_Get_Tx_Length(const t_devicelink* pDevLink)
{
    return pDevLink->tx_descriptor->length;
}


__INLINE__ u_int8* DL_Get_Tx_Qpdu(const t_devicelink* pDevLink)
{
    return pDevLink->tx_descriptor->data;
}
 #ifdef UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK
__INLINE__ u_int8 DL_Get_Tx_Opcode_Pending(const t_devicelink* pDevLink)
{
    return pDevLink->tx_descriptor->opcode_pending;
}
#endif
__INLINE__ t_LCHmessage DL_Get_Tx_Message_Type(const t_devicelink* pDevLink)
{
    return (t_LCHmessage)pDevLink->tx_descriptor->message_type;
}

__INLINE__ void DL_Set_Tx_Packet_Type(t_devicelink* pDevLink, t_packet type)
{
    assert(pDevLink != 0);
    assert(pDevLink->tx_descriptor != 0);
    pDevLink->tx_descriptor->packet_type = type;
}

__INLINE__ void DL_Set_Tx_Length(t_devicelink* pDevLink, t_length length)
{
    assert(pDevLink != 0);
    assert(pDevLink->tx_descriptor != 0);
    pDevLink->tx_descriptor->length = length;
}

__INLINE__ void DL_Set_Tx_Qpdu(t_devicelink* pDevLink, u_int8* buffer)
{
    assert(pDevLink != 0);
    assert(pDevLink->tx_descriptor != 0);
    pDevLink->tx_descriptor->data = buffer;
}

__INLINE__ void DL_Set_Tx_Message_Type(t_devicelink* pDevLink, t_LCHmessage TYPE)
{
    assert(pDevLink != 0);
    assert(pDevLink->tx_descriptor != 0);
    pDevLink->tx_descriptor->message_type = TYPE;
}

#ifdef UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK
__INLINE__ void DL_Set_Tx_Callback(t_devicelink* pDevLink, void* ptr )
{
    assert(pDevLink != 0);
    assert(pDevLink->tx_descriptor != 0);
    pDevLink->tx_descriptor->callback = ptr;
}
#endif

__INLINE__ t_flow DL_Get_Local_Tx_L2CAP_Flow(const t_devicelink* pDevLink)
{
    return (t_flow) pDevLink->local_tx_L2CAP_flow;
}

__INLINE__ void DL_Set_Local_Tx_L2CAP_Flow(t_devicelink* pDevLink, t_flow flow)
{
    assert(pDevLink != 0);
    pDevLink->local_tx_L2CAP_flow = (u_int8) flow;
}

__INLINE__ t_flow DL_Get_Local_Rx_L2CAP_Flow(const t_devicelink* pDevLink)
{
    return (t_flow) pDevLink->local_rx_L2CAP_flow;
}

__INLINE__ void DL_Set_Local_Rx_L2CAP_Flow(t_devicelink* pDevLink, t_flow flow)
{
    assert(pDevLink != 0);
    pDevLink->local_rx_L2CAP_flow = (u_int8) flow;
}

/*
 * SCO related, From P7D only active_sco_pkt and active_sco_fifo are appropriate
 * There is a one to one correspondence of LM sco_index and LC sco_fifo
 */
__INLINE__ t_packet DL_Get_Tx_SCO_Packet_Type(const t_devicelink* pDevLink)
{
    return pDevLink->active_sco_pkt;
}

__INLINE__ void DL_Set_Tx_SCO_Packet_Type(t_devicelink* t_devicelink, t_packet type)
{
    t_devicelink->active_sco_pkt = type;
}

__INLINE__ u_int8 DL_Get_Active_SCO_Index(const t_devicelink* p_device_link)
{
    return p_device_link->active_sco_fifo;
}

__INLINE__ void DL_Set_Active_SCO_Index(t_devicelink* p_device_link, u_int8 sco_index)
{
    p_device_link->active_sco_fifo = sco_index;
}

/*
 *    DL_Decode_Valid_LT_address
 */

__INLINE__ boolean DL_Decode_Valid_LT_address(t_devicelink* p_device_link, u_int8 lt_addr)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    boolean valid_addr = FALSE;

    if (lt_addr == p_device_link->lt_info[ACL_SCO_LT].lt_addr)
    {
        valid_addr = TRUE;
        p_device_link->rx_lt_index = ACL_SCO_LT;
    }
    
    /*
     * An eSCO LT_ADDR is only valid if the received LT_ADDR matches
     * the expected LT_ADDR of the active_rx_lt_index.
     * Therefore, eSCO packets received when not expected will be 
     * rejected (ie active_rx_lt_index will not be pointing to an eSCO
     * LT).
     */

    else if ( lt_addr == p_device_link->lt_info[p_device_link->rx_lt_index].lt_addr)
    {
        valid_addr = TRUE;
        p_device_link->rx_lt_index = ESCO_LT;
    }
    
    return valid_addr;

#else /* (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 0) */

    if (lt_addr == p_device_link->am_addr)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif /* (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1) */
}


/*
 *    DL_Set_Active_Tx_LT
 */
__INLINE__ void DL_Set_Active_Tx_LT(t_devicelink* p_device_link, u_int8 active_lt)
{ 
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    p_device_link->tx_lt_index = active_lt;
#endif
}

/*
 *    DL_Get_Active_Tx_LT
 */
__INLINE__ u_int8 DL_Get_Active_Tx_LT(t_devicelink* p_device_link)
{ 
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return p_device_link->tx_lt_index;
#else
    return 0;
#endif
}

/*
 *    DL_Set_Active_Rx_LT
 */
__INLINE__ void DL_Set_Active_Rx_LT(t_devicelink* p_device_link, u_int8 active_lt)
{ 
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    p_device_link->rx_lt_index = active_lt;
#endif
}

__INLINE__ u_int8 DL_Get_Active_Rx_LT(t_devicelink* p_device_link)
{ 
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return p_device_link->rx_lt_index;
#else
    return 0;
#endif
}

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)

/*
 *
 * CM: 22 SEP 03
 * DL_Set_LT_Address(pDL, ?, ?)
 *
 */
/*
 *    DL_Set_eSCO_LT_Address
 */
__INLINE__ void DL_Set_eSCO_LT_Address(t_devicelink* p_device_link, u_int8 esco_lt_index, u_int8 lt_addr)
{
    p_device_link->lt_info[esco_lt_index].lt_addr = lt_addr;
}


/*
 *    DL_Get_eSCO_LT_Address
 */
__INLINE__ u_int8 DL_Get_eSCO_LT_Address(const t_devicelink* p_device_link)
{
    return p_device_link->lt_info[ESCO_LT].lt_addr;
}


__INLINE__ void DL_Set_eSCO_Rx_Length(t_devicelink* p_device_link, u_int16 rx_length)
{
    p_device_link->esco_rx_len = rx_length;
}


/*
 *    DL_Get_eSCO_Rx_Length
 */
__INLINE__ u_int16 DL_Get_eSCO_Rx_Length(const t_devicelink* p_device_link)
{
    return p_device_link->esco_rx_len;
}

#else   /*(PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)*/

/*
 *  Dummy accessor definitions to allow building the stack without extended SCO enabled.
 */
#define DL_Set_eSCO_LT_Address(X, Y, Z)

#endif  /*(PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)*/

/*
 *  Generic Logical Transport macros
 */
/*

 *    DL_Set_LT_Address

 */

__INLINE__ void DL_Set_LT_Address(t_devicelink* pDevLink, u_int8 lt_addr)
{

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->tx_lt_index].lt_addr = lt_addr;

#endif
}


/*
 *    DL_Get_LT_Address
 */
__INLINE__ u_int8 DL_Get_LT_Address(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return pDevLink->lt_info[pDevLink->tx_lt_index].lt_addr;
#else
    return pDevLink->am_addr;
#endif
}

/*
 *    DL_Get_Tx_LT_Previous_Arqn
 */
__INLINE__ u_int DL_Get_Tx_LT_Previous_Arqn(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return (pDevLink->lt_info[pDevLink->tx_lt_index].rx_previous_arqn);
#else
    return (pDevLink->rx_previous_arqn);
#endif
}

/*
 *    DL_Set_Rx_LT_Previous_Arqn
 */
__INLINE__ void DL_Set_Rx_LT_Previous_Arqn(t_devicelink* pDevLink, u_int arqn)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
	/*
	 * The arqn should not be saved if it is a Nak in response to a Poll on the esco addr.
	 * Only save the arqn on esco address if there is an receive eSCO pending.
	 */
	if ( is_eSCO_LT(pDevLink->rx_lt_index) )
	{
		if (  DL_Get_Rx_eSCO_Pending(pDevLink))
			pDevLink->lt_info[pDevLink->rx_lt_index].rx_previous_arqn = arqn;		
		
	}
	else
		pDevLink->lt_info[pDevLink->rx_lt_index].rx_previous_arqn = arqn;
#else
    pDevLink->rx_previous_arqn = arqn;
#endif
}

/*
 *    DL_Toggle_Rx_LT_Seqn
 */
__INLINE__ void DL_Toggle_Rx_LT_Seqn(t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->rx_lt_index].tx_seqn ^= 1;
#else
    pDevLink->tx_seqn ^= 1;
#endif
}

/*
 *    DL_Set_Tx_LT_Seqn
 */
__INLINE__ void DL_Set_Tx_LT_Seqn(t_devicelink* pDevLink, u_int seqn)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->tx_lt_index].tx_seqn = seqn;
#else
    pDevLink->tx_seqn = seqn;
#endif
}

/*
 *    DL_Get_Tx_LT_Seqn
 */
__INLINE__ u_int DL_Get_Tx_LT_Seqn(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return (pDevLink->lt_info[pDevLink->tx_lt_index].tx_seqn);
#else
    return(pDevLink->tx_seqn);
#endif
}



/*
 *    DL_Toggle_Tx_LT_Seqn
 */
__INLINE__ void DL_Toggle_Tx_LT_Seqn(t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->tx_lt_index].tx_seqn ^= 1;
#else
    pDevLink->tx_seqn ^= 1;
#endif
}

#if (PRH_BS_DEV_TOGGLE_TX_SEQN_ON_TX_CRC_PACKET ==1)
/*
 * DL_Set_Tx_Seqn_Active
 */
__INLINE__ void DL_Set_Tx_Seqn_Active(t_devicelink* pDevLink)
{
    pDevLink->tx_seqn_active=TRUE;
}

/*
 * DL_Set_Tx_Seqn_Not_Active
 */
__INLINE__ void DL_Set_Tx_Seqn_Not_Active(t_devicelink* pDevLink)
{
    pDevLink->tx_seqn_active=FALSE;
}

/*
 * DL_Is_Tx_Seqn_Active
 */
__INLINE__ u_int8 DL_Is_Tx_Seqn_Active(t_devicelink* pDevLink)
{
    return pDevLink->tx_seqn_active;
}

#endif

/*
 *    DL_Set_Rx_LT_Expected_Seqn
 */
__INLINE__ void DL_Set_Rx_LT_Expected_Seqn(t_devicelink* pDevLink, u_int seqn)
{
#if(PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)
    pDevLink->lt_info[pDevLink->rx_lt_index].rx_expected_seqn = seqn;
#else
    pDevLink->rx_expected_seqn = seqn;
#endif
}

/*
 *    DL_Get_Rx_LT_Expected_Seqn
 */
__INLINE__ u_int DL_Get_Rx_LT_Expected_Seqn(const t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    return pDevLink->lt_info[pDevLink->rx_lt_index].rx_expected_seqn;
#else
    return pDevLink->rx_expected_seqn;
#endif
}

/*
 *    DL_Toggle_Rx_LT_Expected_Seqn
 */
__INLINE__ void DL_Toggle_Rx_LT_Expected_Seqn(t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
    pDevLink->lt_info[pDevLink->rx_lt_index].rx_expected_seqn ^= 1;
#else
    pDevLink->rx_expected_seqn ^= 1;
#endif
}

/*
 *    DL_Toggle_Tx_LT_Expected_Seqn
 */
__INLINE__ void DL_Toggle_Tx_LT_Expected_Seqn(t_devicelink* pDevLink)
{
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
/*    pDevLink->lt_info[pDevLink->tx_lt_index].rx_expected_seqn ^= 1; */
    pDevLink->lt_info[ESCO_LT].rx_expected_seqn ^= 1;

#else
    pDevLink->rx_expected_seqn ^= 1;
#endif
}

__INLINE__ void DL_Set_UAP_LAP(t_devicelink* pDevLink, t_uap_lap uap_lap)
{
    pDevLink->uap_lap = uap_lap;
}

__INLINE__ t_uap_lap DL_Get_UAP_LAP(const t_devicelink* pDevLink)
{
    return pDevLink->uap_lap;
}

__INLINE__ void DL_Set_NAP(t_devicelink* pDevLink, t_nap nap)
{
    pDevLink->nap = nap;
}

__INLINE__ t_nap DL_Get_NAP(const t_devicelink* pDevLink)
{
    return pDevLink->nap;
}

__INLINE__ void DL_Get_Bd_Addr_Ex(const t_devicelink* pDevLink, t_bd_addr* bda)
{
    BDADDR_Set_LAP_UAP_NAP(bda, (pDevLink->uap_lap & 0x00FFFFFF),
                           (pDevLink->uap_lap >> 24), pDevLink->nap);
}

/*
 *  Clock Offset of Remote Device Accessor/Modifier (relative to native clock)
 */

__INLINE__ t_clock DL_Get_Clock_Offset(const t_devicelink* pDevLink)
{
   return pDevLink->clock_offset;
}

__INLINE__ void DL_Set_Clock_Offset(t_devicelink* pDevLink, t_clock clk)
{
    pDevLink->clock_offset = clk;
}

/*
 * Encryption related methods
 */
__INLINE__ t_encrypt_mode DL_Get_Encryption_Mode(const t_devicelink* pDevLink)
{
    return pDevLink->encrypt_mode;
}

__INLINE__ void DL_Set_Encryption_Mode(t_devicelink* pDevLink,t_encrypt_mode encrypt_mode)
{
    pDevLink->encrypt_mode = encrypt_mode;
}

__INLINE__ void
    DL_Set_Encryption_Key_Ref(t_devicelink* pDevLink, u_int8 *p_encryption_key)
{
    pDevLink->p_encryption_key = p_encryption_key;
}

__INLINE__ u_int8 *
    DL_Get_Encryption_Key_Ref(const t_devicelink* pDevLink)
{
    return pDevLink->p_encryption_key;
}

/*
 * Hardware encryption key length is actual required length - 1
 * Note anomaly on subtracting 1 below when 0.
 */
__INLINE__ void
    DL_Set_Encryption_Key_Length(t_devicelink* pDevLink, u_int8 encryption_key_length)
{
    pDevLink->hw_encryption_key_len = _SYS_GET_CHAR_8_BITS(encryption_key_length - 1);
}
__INLINE__ u_int8 DL_Get_Encryption_Key_Length(const t_devicelink* pDevLink)
{
    return pDevLink->hw_encryption_key_len;
}

   
#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
/*
 * For Channel Driven Quality Support must monitor number Tx/Rx retransmissions
 */
__INLINE__ void   DL_Inc_Tx_ACL_Data_Packets(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_tx_acl_packets; 
}

__INLINE__ void   DL_Inc_Tx_ACL_Data_Packets_Acked(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_tx_acl_packets_acked;
}

__INLINE__ void   DL_Inc_Rx_ACL_Data_Packets(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_rx_acl_packets; 
}

__INLINE__ void   DL_Inc_Rx_ACL_Data_Packets_Nacked(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_rx_acl_packets_nacked;
}

__INLINE__ void   DL_Inc_Tx_SYN_Data_Packets(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_tx_syn_packets; 
}

__INLINE__ void   DL_Inc_Tx_SYN_Data_Packets_Acked(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_tx_syn_packets_acked;
}

__INLINE__ void   DL_Inc_Rx_SYN_Data_Packets(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_rx_syn_packets; 
}

__INLINE__ void   DL_Inc_Rx_SYN_Data_Packets_Nacked(t_devicelink* p_device_link)
{
    ++p_device_link->channel_quality.num_rx_syn_packets_nacked;
}

__INLINE__ const t_ch_quality *DL_Get_Channel_Quality_Ref(const t_devicelink* p_device_link)
{
    return &p_device_link->channel_quality;
}

__INLINE__ void   DL_Initialise_Channel_Quality(t_devicelink* p_device_link)
{
    t_ch_quality *p_ch_quality = &p_device_link->channel_quality;
    p_ch_quality->num_tx_acl_packets = 0;
    p_ch_quality->num_tx_acl_packets_acked = 0;
    p_ch_quality->num_rx_acl_packets = 0;
    p_ch_quality->num_rx_acl_packets_nacked =0;
    p_ch_quality->num_tx_syn_packets = 0;
    p_ch_quality->num_tx_syn_packets_acked = 0;
    p_ch_quality->num_rx_syn_packets = 0;
    p_ch_quality->num_rx_syn_packets_nacked =0;
}
#endif  /*(PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)*/

#if (PRH_BS_CFG_SYS_LMP_RSSI_SUPPORTED==1)
__INLINE__ s_int8 DL_Get_RSSI(t_devicelink* p_device_link)
{
    return p_device_link->rssi_average;
}



#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
__INLINE__ s_int8 DL_Get_1Mb_RSSI(t_devicelink* p_device_link)
{
    return p_device_link->rssi_1Mbits_average;
}

__INLINE__ s_int8 DL_Get_2Mb_RSSI(t_devicelink* p_device_link)
{
    return p_device_link->rssi_2Mbits_average;
}

__INLINE__ s_int8 DL_Get_3Mb_RSSI(t_devicelink* p_device_link)
{
    return p_device_link->rssi_3Mbits_average;
}
#endif

__INLINE__ s_int8 DL_Get_RSSI_For_Last_Packet_Received(t_devicelink* p_device_link)
{
    return p_device_link->rssi_lastpkt;
}

#endif

/********************************************************************
 *
 * eSCO Related
 *
 ********************************************************************/
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED==1)

#if (ESCO_LINK_CRC_ERROR_CHANGE_TO_GOOD == 1)                
__INLINE__ u_int DL_Get_Rx_eSCO_To_Good(const t_devicelink* p_device_link)
{
    return p_device_link->rx_crc_err2good;
}

__INLINE__ void  DL_Set_Rx_eSCO_To_Good(t_devicelink* p_device_link,u_int good)
{
    p_device_link->rx_crc_err2good = good;
}
#endif
__INLINE__ void  DL_Init_Rx_eSCO_Pending(t_devicelink* p_device_link,t_packet packet)
{
    /* #2551 */
    if (packet<=POLLpkt)
    {
        p_device_link->rx_esco_pending = FALSE;
    }
    else
    {
        p_device_link->rx_esco_pending = TRUE;
    }
}

__INLINE__ u_int DL_Get_Rx_eSCO_Pending(const t_devicelink* p_device_link)
{
    return p_device_link->rx_esco_pending;
}

__INLINE__ void  DL_Set_Rx_eSCO_Pending(t_devicelink* p_device_link,u_int rxPending)
{
    p_device_link->rx_esco_pending = rxPending;
}

__INLINE__ void DL_Free_eSCO_Link(t_devicelink* p_device_link, u_int8 esco_lt_index)
{
    p_device_link->lt_info[esco_lt_index].lt_addr = ESCO_LINK_FREE;
}

__INLINE__ void DL_Set_eSCO_Tx_Length(t_devicelink* p_device_link, u_int16 tx_length)
{
    p_device_link->eSCO_tx_descr.tx_length = tx_length;
}

__INLINE__ u_int16 DL_Get_eSCO_Tx_Length(const t_devicelink* p_device_link)
{
    return p_device_link->eSCO_tx_descr.tx_length;
}

__INLINE__ void DL_Set_eSCO_Tx_Data(t_devicelink* p_device_link, u_int8* data)
{
    p_device_link->eSCO_tx_descr.data = data;
}

__INLINE__ u_int8* DL_Get_eSCO_Tx_Data(const t_devicelink* p_device_link)
{
    return p_device_link->eSCO_tx_descr.data;
}
#endif

#endif

/*
 * EDR related
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
/*
 * DL_Is_EDR_Enabled
 */
__INLINE__ boolean DL_Is_EDR_Enabled(const t_devicelink* pDevLink)
{
   return ((pDevLink->ptt_acl) | (pDevLink->ptt_esco)) ;
}

/*
 * DL_Get_PTT_ACL
 */
__INLINE__ boolean DL_Get_PTT_ACL(const t_devicelink* pDevLink)
{
	return pDevLink->ptt_acl;
}
/*
 * DL_Set_PTT_ACL
 */
__INLINE__ void DL_Set_PTT_ACL(t_devicelink* pDevLink, boolean ptt_acl)
{
	pDevLink->ptt_acl=ptt_acl;
}

/*
 * DL_Get_PTT_ESCO
 */
__INLINE__ boolean DL_Get_PTT_ESCO(const t_devicelink* pDevLink)
{
	return pDevLink->ptt_esco;
}

/*
 * DL_Set_PTT_ESCO
 */
__INLINE__ void DL_Set_PTT_ESCO(t_devicelink* pDevLink, boolean ptt_esco)
{
	pDevLink->ptt_esco=ptt_esco;
}
#endif

#if (PRH_BS_CFG_SYS_LMP_ERRONOUS_DATA_REPORTING_SUPPORTED == 1)

__INLINE__ void DL_Set_Rx_SYNC_Erroneous_Data_Descriptor(t_devicelink* pDevLink,t_q_descr *p_descriptor)
{
	pDevLink->p_CurrentSCOdescriptor = p_descriptor;
}
__INLINE__ t_q_descr* DL_Get_Rx_SYNC_Erroneous_Data_Descriptor(t_devicelink* pDevLink)
{
	return pDevLink->p_CurrentSCOdescriptor;
}
#endif

__INLINE__ u_int8 DL_Get_Rx_SYNC_Erroneous_Data_Status(t_devicelink* pDevLink)
{
	return pDevLink->ErroneousDataStatus;
}

__INLINE__ void DL_Set_Rx_SYNC_Erroneous_Data_Status(t_devicelink* pDevLink,u_int8 status)
{
	pDevLink->ErroneousDataStatus = status;
}
