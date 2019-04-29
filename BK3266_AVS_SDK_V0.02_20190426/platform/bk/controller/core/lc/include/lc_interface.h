#ifndef __PARTHUS_LC_INTERFACE
#define __PARTHUS_LC_INTERFACE

/*************************************************************************
 * MODULE NAME:    lc_interface.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Baseband Link Controller Interface Module
 * MAINTAINER:     Gary Fleming, John Nelson
 * CREATION DATE:  2 February 2000
 *
 * SOURCE CONTROL: $Id: lc_interface.h,v 1.118 2014/07/02 15:39:29 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 *************************************************************************/

#include "sys_config.h"
#include "sys_types.h"
#include "lc_types.h"
#include "dl_dev.h"
#include "hc_event_gen.h"
#include "hw_le_lc.h"
#include "lslc_access.h" /* for LSLCacc_Sync_Setup_SCO_CFG_EX() */
/*
 * For LC_Get_Piconet_Clock(), LC_Get_Native_Clock()
 */
#include "hw_lc.h"
/*
 * To resolve LC_Get_R2P_Early_Wakeup_Slots() 
 */
#include "uslc_return_to_piconet.h"

typedef t_error (*t_hl_callback)(t_LC_Event_Info*);

typedef t_devicelink* (*t_callback)(void);

void LC_Register_Event_Handler(t_LC_Event_Id event_id, void* func_ptr);
void LC_Force_Context(t_role role);
void LC_Register_Access_Window_Callback_Handler(t_callback func_ptr);

extern void* lc_call_back_list[]; 
#define LC_Callback_Event_Handler(event_id, lc_event_info) (\
        (lc_call_back_list[event_id] != 0) ? \
            ((t_hl_callback)(lc_call_back_list[event_id]))(lc_event_info) : 0)

RAM_CODE void LC_Report_Device_Activity(t_deviceIndex device_index);


/*
 * Initialisation of LC and of a previously allocated device link
 */
void LC_Initialise(void);
void LC_Reset_All_Callbacks(void);

/*
 * Device link management
 */
t_error LC_Alloc_Link(t_deviceIndex *device_index);
void LC_Free_Link(t_deviceIndex device_index);
t_error LC_Alloc_AM_ADDR(t_am_addr* am_addr); 

/************************************************************************
 * LC_Get_Radio_Setup_Time
 *
 * Return setup time for radio, currently defaults to maximum
 ************************************************************************/
#define LC_Get_Radio_Setup_Time_Us()                220


/************************************************************************
 * LC_Get_Max_Sleep_Time_Slots
 *
 * Return maximum sleep time for presented device_index in Slots
 ************************************************************************/
u_int16 LC_Get_Max_Sleep_Time_Slots(
    t_deviceIndex device_index    /* i - index of device link    */);

/*************************************************************************
 * LC_Sleep
 *
 * Put LC to sleep. 
 *************************************************************************/
t_error LC_Sleep(t_slots slots, t_deviceIndex device_index);

/*************************************************************************
 * LC_Wakeup
 *
 * Wakeup the LC.
 * timeout - specifies a timeout value - should be zero for Return from Hold,
 * and a number of slots for Sniff etc.
 * instance - specifices the precice clock instance the wakup should occur.
 *************************************************************************/
t_error LC_Wakeup(t_deviceIndex device_index, t_slots timeout, t_clock instance);


/*************************************************************************
 ******            Piconet Control Related Methods                 *******
 *************************************************************************/
t_error LC_Restore_Piconet(void);


/*
 * Power Control Related Methods
 */
u_int8 LC_Read_Power(t_deviceIndex device_index);
boolean LC_Incr_Power(t_deviceIndex device_index, u_int8 power_units);
boolean LC_Decr_Power(t_deviceIndex device_index, u_int8 power_units);

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
/*************************************************************************
 * LC_Read_Current_Transmit_Power_Level
 *
 * Read the values for the Enhanced_Transmit_Power_Level parameters
 * Transmit Power Type is defined:
 *	  0x00 Read Current Transmit Power Level
 *    0x01 Read Maximum Transmit Power Level
 *
 * Returns  the transmit power levels via the ptr *transmit_power_level 
 *************************************************************************/
#define LC_Read_Current_Transmit_Power_Level(device_index, requested_transmit_power_level, transmit_power_levels) \
        DL_Read_Current_Transmit_Power_Level(device_index, requested_transmit_power_level, transmit_power_levels)

/*************************************************************************
 * LC_Power_Control_Req
 *
 * Sets the values for the Enhanced_Transmit_Power_Level parameters
 *
 * Returns  
 *************************************************************************/
#define LC_Power_Control_Req(device_index, allowed_pkt_types, power_adjustment_req, power_adjustment_resp) \
        DL_Power_Control_Req(device_index, allowed_pkt_types, power_adjustment_req, power_adjustment_resp)
#endif

/*
 * Timing Related Methods
 */
u_int32 LC_Read_Clock_Offset(t_deviceIndex device_index);
u_int16 LC_Read_Local_Slot_Offset(t_deviceIndex device_index);
void LC_Write_Remote_Slot_Offset(t_deviceIndex device_index, u_int16 slot_offset);
u_int8 LC_Read_Local_Device_Jitter(void);
u_int8 LC_Read_Local_Device_Drift(void);
void LC_Write_Device_Jitter(t_deviceIndex device_index, u_int8 jitter);
void LC_Write_Device_Drift(t_deviceIndex device_index, u_int8 drift);

/*
 * General accessors and modifiers
 */
t_classDevice LC_Read_Local_Device_Class(void);
void LC_Write_Local_Device_Class(t_classDevice class_of_device);

t_error LC_Write_Supervision_Timeout(t_deviceIndex device_index, t_slots timeout);


/*
 * Channel Proocedures Activation/Deactivation
 */
/************************************************************************
 * LC_Create_Connection
 * LC_Page_Cancel
 *
 * LC Page (Create Connection) Procedures
 ************************************************************************/
#if (PRH_BS_CFG_SYS_PAGE_SUPPORTED==1)
t_error LC_Create_Connection(const t_bd_addr *p_bd_addr, t_clock bt_clk_offset,
              t_deviceIndex device_index,  u_int16 train_reps, t_slots pageTO);
t_error LC_Page_Cancel(void);
t_error LC_Page_Resume(t_slots slots);
boolean LC_Page_expired(void);
#else
#define LC_Create_Connection(p_bd_addr, bt_clk_offset, device_index, \
        train_reps, pageTO)             UNSUPPORTED_FEATURE
#define LC_Page_Cancel()             UNSUPPORTED_FEATURE
#define LC_Page_expired()            UNSUPPORTED_FEATURE
#endif

/************************************************************************
 * LC_Inquiry_Request
 * LC_Inquiry_Cancel
 *
 * LC Inquiry Procedures
 ************************************************************************/
#if (PRH_BS_CFG_SYS_INQUIRY_SUPPORTED==1)
t_error LC_Inquiry_Request(t_lap inquiry_lap, t_slots window, u_int16 train_reps);
t_error LC_Inquiry_Cancel(void);
t_error LC_Inquiry_Resume(t_slots slots);
#else
#define LC_Inquiry_Request(inquiry_lap, window, train_reps) \
                                        UNSUPPORTED_FEATURE
#define LC_Inquiry_Cancel()             UNSUPPORTED_FEATURE
#endif

/************************************************************************
 * LC_Inquiry_Scan_Request
 * LC_Inquiry_Scan_Cancel
 *
 * LC Inquiry Scan Procedures
 ************************************************************************/
#if (PRH_BS_CFG_SYS_INQUIRY_SCAN_SUPPORTED==1)
t_error LC_Inquiry_Scan_Request(t_lap inquiry_lap, t_slots window,
                                                        t_scan_type scan_type);
void    LC_Inquiry_Scan_Cancel(void);
#else
#define LC_Inquiry_Scan_Request(inquiry_lap, window, scan_type) \
                                        UNSUPPORTED_FEATURE
#define LC_Inquiry_Scan_Cancel()        UNSUPPORTED_FEATURE
#endif


/************************************************************************
 * LC_Page_Scan_Request
 * LC_Page_Scan_Cancel
 *
 * LC Page Scan Procedures
 ************************************************************************/
#if (PRH_BS_CFG_SYS_PAGE_SCAN_SUPPORTED==1)
t_error LC_Page_Scan_Request(t_deviceIndex device_index, t_slots window, t_scan_type scan_type);
void    LC_Page_Scan_Cancel(void);
#else
#define LC_Page_Scan_Request(device_index, window, scan_type) \
                                        UNSUPPORTED_FEATURE
#define LC_Page_Scan_Cancel()           UNSUPPORTED_FEATURE
#endif

/************************************************************************
 * LC_Testmode_Request
 *      Request testmode activation based on the LMP test control PDU
 * LC_Testmode_Exit
 *      Exit testmode activation (if supported and active)
 *
 * LC Test Mode Procedures (if supported)
 *************************************************************************/
#if (PRH_BS_CFG_SYS_TEST_MODE_SUPPORTED==1)
#include "uslc_testmode.h"
#define LC_Testmode_Request(device_index, p_LMP_test_control_PDU) \
        USLCtm_Testmode_Request(device_index, p_LMP_test_control_PDU)
#define LC_Testmode_Exit()    USLCtm_Testmode_Exit()
#else
#define LC_Testmode_Request(deviceIndex, p_LMP_test_control_PDU) UNSUPPORTED_FEATURE
#define LC_Testmode_Exit()
#endif

/************************************************************************
 * LC_Role_Switch
 * 
 *
 * LC Role Switch Procedure
 ************************************************************************/
t_error LC_Role_Switch(t_deviceIndex device_index);

/************************************************************************
 * LC_Sniff_Request
 * LC_Sniff_Cancel
 *
 * LC Sniff Procedures
 ************************************************************************/
t_error LC_Sniff_Request(t_deviceIndex device_index, t_clock window);
void    LC_Sniff_Cancel(void);

/************************************************************************
 * LC_Hold_Return_Request
 * LC_Hold_Return_Cancel
 *
 * LC Hold Procedures
 ************************************************************************/
t_error LC_Hold_Return_Request(t_deviceIndex device_index, u_int16 search_window);
void    LC_Hold_Return_Cancel(void);

t_error LC_Null_Callback(t_LC_Event_Info* eventInfo);

/*
 * Park related interface.
 */
#if (PRH_BS_CFG_SYS_LMP_PARK_SUPPORTED==1)
t_error LC_Park_Request(t_deviceIndex device_index, t_pm_addr pm_addr);

t_error LC_Unpark_Request(t_deviceIndex* device_index, t_parkDeviceIndex park_device_index, t_role role_of_peer, 
                    t_am_addr am_addr, t_unpark_type unpark_type, t_clock start_timer_ref, t_slots n_poll);

t_error LC_Access_Window_Request(u_int n_acc_slot, t_slots t_access, u_int m_access, 
                 u_int8 n_poll, t_deviceIndex device_index, t_ar_addr ar_addr);


t_error LC_Unpark_Rollback(void);

t_error LC_Unpark_Commit(void);

#endif /* Park */

/*
 * SCO Codec Control
 */ 
boolean LC_Is_Valid_SCO_Conversion(u_int16 voice_setting);
u_int8 LC_Get_Hardware_SCO_CFG(u_int16 voice_setting);
void LC_Enable_Codec(void);
void LC_Disable_Codec(void);
void LC_Increase_Volume(void);
void LC_Decrease_Volume(void);
u_int8 LC_Get_Vci_Clk_Sel(void);
u_int8 LC_Get_Vci_Clk_Sel_Map(void);


#if (PRH_BS_CFG_SYS_LMP_SCO_SUPPORTED==1)
#define LC_Sync_Setup_SCO_CFG_Ex LSLCacc_Sync_Setup_SCO_CFG_Ex
#elif (PRH_BS_CFG_SYS_LMP_SCO_SUPPORTED==0)
#define LC_Sync_Setup_SCO_CFG_Ex(sco_fifo, sco_cfg) 
#endif

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_SCO_SUPPORTED == 1)
#define LC_Sync_Setup_SCO_ROUTE_Ex LSLCacc_Sync_Setup_SCO_ROUTE_Ex
#endif

/*
 * Deferred payload extraction
 */
#if (LC_DEFERRED_PAYLOAD_EXTRACTION_SUPPORTED==1)
void LC_Perform_Deferred_Payload_Extraction_If_Reqd(void);
#endif

/*
 * Provide Clock Access to Higher levels
 */

#define LC_Get_Native_Clock()    ( (t_clock) HW_Get_Native_Clk_Avoid_Race() )
#define LC_Get_Piconet_Clock(device_index) ( (t_clock) LC_Get_Piconet_Clock_Ex((device_index),FALSE) )

/*
 * Link policy procedure activation
 * For now just revector to LMP equivalence
 */
#if (PRH_BS_DEV_LC_NEXT_ACTIVITY_CALLBACKS_SUPPORTED==1)
    #if (0 && (BUILD_TYPE!=UNIT_TEST_BUILD))
        /* silent check for NULL registrations!! */
        #define LC_Get_Next_Master_Tx_Activity() (t_devicelink*)(\
            (lc_call_back_list[LC_GET_NEXT_MASTER_TX_ACTIVITY] != 0) ? \
                ((t_devicelink* (*)(void))(lc_call_back_list[LC_GET_NEXT_MASTER_TX_ACTIVITY]))() : 0 )
        #define LC_Is_SCO_Next_Activity() (t_devicelink*)(\
            (lc_call_back_list[LC_IS_SCO_NEXT_ACTIVITY] != 0) ? \
                ((t_devicelink* (*)(void))(lc_call_back_list[LC_IS_SCO_NEXT_ACTIVITY]))() : 0 )
        #define LC_Get_Next_Slave_Tx_Activity(dev_index) (boolean)(\
            (lc_call_back_list[LC_GET_NEXT_SLAVE_TX_ACTIVITY] != 0) ? \
                ((boolean(*)(t_deviceIndex))(lc_call_back_list[LC_GET_NEXT_SLAVE_TX_ACTIVITY]))(dev_index) : 0 )
        #define LC_Get_Next_Slave_Rx_Activity(dev_index) (boolean)(\
            (lc_call_back_list[LC_GET_NEXT_SLAVE_RX_ACTIVITY] != 0) ? \
                ((boolean(*)(t_deviceIndex))(lc_call_back_list[LC_GET_NEXT_SLAVE_RX_ACTIVITY]))(dev_index) : 0 )
        #define LC_Get_Next_SCO_Tx_Activity(pDL) \
            if (lc_call_back_list[LC_GET_NEXT_SCO_TX_ACTIVITY] != 0) \
                ((void(*)(t_devicelink*))(lc_call_back_list[LC_GET_NEXT_SCO_TX_ACTIVITY]))(pDL)
        #define LC_Ack_Last_Tx_Packet(dev_index) \
            if (lc_call_back_list[LC_ACK_LAST_TX_PACKET] != 0) \
                ((void(*)(t_deviceIndex))(lc_call_back_list[LC_ACK_LAST_TX_PACKET]))(dev_index)
    #else
        /* No Checks for NULL registrations => seg fault instead */
        #define LC_Get_Next_Master_Tx_Activity() ((t_devicelink* (*)(void))lc_call_back_list[LC_GET_NEXT_MASTER_TX_ACTIVITY])()
        #define LC_Is_SCO_Next_Activity() ((t_devicelink* (*)(void))lc_call_back_list[LC_IS_SCO_NEXT_ACTIVITY])()
        #define LC_Get_Next_Slave_Tx_Activity(dev_index) ((boolean(*)(t_deviceIndex))lc_call_back_list[LC_GET_NEXT_SLAVE_TX_ACTIVITY])(dev_index)
        #define LC_Get_Next_Slave_Rx_Activity(dev_index) ((boolean(*)(t_deviceIndex))lc_call_back_list[LC_GET_NEXT_SLAVE_RX_ACTIVITY])(dev_index)
        #define LC_Get_Next_SCO_Tx_Activity(pDL) ((void(*)(t_devicelink*))(lc_call_back_list[LC_GET_NEXT_SCO_TX_ACTIVITY]))(pDL)
        #define LC_Ack_Last_Tx_Packet(dev_index) ((void(*)(t_deviceIndex))(lc_call_back_list[LC_ACK_LAST_TX_PACKET]))(dev_index)
        #define LC_Invalidate_Reserved_Slot(pDL) ((void(*)(t_devicelink*))(lc_call_back_list[LC_GET_NEXT_SCO_TX_ACTIVITY]))(pDL)
    #endif
#else
    /* Old behaviour */
    #include "lmp_link_policy.h"
    #define LC_Get_Next_Master_Tx_Activity() LMpol_Get_Next_Master_Tx_Activity()
    #define LC_Is_SCO_Next_Activity() LMpol_Is_SCO_Next_Activity()
    #define LC_Get_Next_SCO_Tx_Activity(pDL) LMpol_Get_Next_SCO_Tx_Activity(pDL)
    #define LC_Get_Next_Slave_Tx_Activity(dev_index) LMpol_Get_Next_Slave_Tx_Activity(dev_index)
    #define LC_Get_Next_Slave_Rx_Activity(dev_index) LMpol_Get_Next_Slave_Rx_Activity(dev_index)
    #define LC_Ack_Last_Tx_Packet(dev_index) LMpol_Ack_Last_Tx_Packet(dev_index)
#endif

/************************************************************************
 * LC_Get_Device_FHS_Ref            Return reference to this device's FHS
 *
 ************************************************************************/
t_FHSpacket* LC_Get_Device_FHS_Ref(void);

/************************************************************************
 * LC_Read_Local_Page_Scan_Repetition
 * LC_Write_Local_Page_Scan_Repetition
 *
 * Read/Write Page Scan Repetition for this device
 ************************************************************************/
#define LC_Read_Local_Page_Scan_Repetition()   \
    FHS_Get_Page_Scan_Repetition(LC_Get_Device_FHS_Ref())
#define LC_Write_Local_Page_Scan_Repetition(scan_repetition)      \
    FHS_Set_Page_Scan_Repetition(LC_Get_Device_FHS_Ref(), scan_repetition)

/************************************************************************
 * LC_Read_Local_Page_Scan_Period
 * LC_Write_Local_Page_Scan_Period
 *
 * Read/Write Page Scan Period for this device
 ************************************************************************/
#define LC_Read_Local_Page_Scan_Period()   \
    FHS_Get_Page_Scan_Period(LC_Get_Device_FHS_Ref() )
#define LC_Write_Local_Page_Scan_Period(scan_period)      \
    FHS_Set_Page_Scan_Period(LC_Get_Device_FHS_Ref(), scan_period)

/************************************************************************
 * LC_Read_Local_Page_Scan_Mode
 * LC_Write_Local_Page_Scan_Mode
 *
 * Read/Write Page Scan Mode for this device
 ************************************************************************/
#define LC_Read_Local_Page_Scan_Mode()   \
    FHS_Get_Page_Scan_Mode(LC_Get_Device_FHS_Ref() )
#define LC_Write_Local_Page_Scan_Mode(scan_mode)      \
    FHS_Set_Page_Scan_Mode(LC_Get_Device_FHS_Ref(), scan_mode)

/************************************************************************
 * LC_Read_Encryption_Key_Ref
 * LC_Write_Encryption_Key_Ref
 *
 * Write reference to where encryption key for this device index is held
 ************************************************************************/
#define LC_Write_Encryption_Key_Ref(device_index, p_encryption_key) \
    DL_Set_Encryption_Key_Ref(DL_Get_Device_Ref(device_index), p_encryption_key)
#define LC_Read_Encryption_Key_Ref(device_index) \
    DL_Get_Encryption_Key_Ref(DL_Get_Device_Ref(device_index))

/************************************************************************
 * LC_Write_Encryption_Key_Length
 *
 * Write reference to where encryption key for this device index is held
 ************************************************************************/
#define LC_Write_Encryption_Key_Length(device_index, encryption_key_len) \
    DL_Set_Encryption_Key_Length(DL_Get_Device_Ref(device_index), encryption_key_len)

/************************************************************************
 * LC_Write_Encryption_Key_Ref_And_Length
 *
 * Write reference to where encryption key for this device index is held
 * and its actual length (1-16)
 * Note Hardware's length register is offset by -1.
 ************************************************************************/
#define LC_Write_Encryption_Key_Ref_And_Length(device_index, \
                                   p_encryption_key, encryption_key_len) \
    DL_Set_Encryption_Key_Length(DL_Get_Device_Ref(device_index), encryption_key_len),  \
    DL_Set_Encryption_Key_Ref(DL_Get_Device_Ref(device_index), p_encryption_key)


/************************************************************************
 * LC_Write_Encryption_Mode
 *
 * Write encrypt mode for specified device index:
 * ENCRYPT_NONE, ENCRYPT_POINT2POINT, ENCRYPT_POINT2POINT_BROADCAST, 
 * ENCRYPT_MASTER_RX_ONLY, ENCRYPT_MASTER_TX_ONLY
 ************************************************************************/
#define LC_Write_Encryption_Mode(device_index, encryption_mode) \
    DL_Set_Encryption_Mode(DL_Get_Device_Ref(device_index), encryption_mode)

#define LC_Read_Encryption_Mode(device_index) DL_Get_Encryption_Mode(DL_Get_Device_Ref(device_index))

/************************************************************************
 * LC_Stop_Peer_Tx_L2CAP_Data
 *
 * Request Peer to stop L2CAP data transmission.
 ************************************************************************/
#define LC_Stop_Peer_Tx_L2CAP_Data(device_index) \
    DL_Set_Local_Rx_L2CAP_Flow(DL_Get_Device_Ref(device_index), STOP)

/************************************************************************
 * LC_Start_Peer_Tx_L2CAP_Data
 *
 * Request Peer to start L2CAP data transmission.
 ************************************************************************/
#define LC_Start_Peer_Tx_L2CAP_Data(device_index) \
    DL_Set_Local_Rx_L2CAP_Flow(DL_Get_Device_Ref(device_index), GO)

/************************************************************************
 * LC_Stop_Local_Tx_L2CAP_Data
 *
 * Request Local device to stop L2CAP data transmission.
 ************************************************************************/
#define LC_Stop_Local_Tx_L2CAP_Data(device_index) \
    DL_Set_Local_Tx_L2CAP_Flow(DL_Get_Device_Ref(device_index), STOP)

/************************************************************************
 * LC_Start_Local_Tx_L2CAP_Data
 *
 * Request Local device to start L2CAP data transmission.
 ************************************************************************/
#define LC_Start_Local_Tx_L2CAP_Data(device_index) \
    DL_Set_Local_Tx_L2CAP_Flow(DL_Get_Device_Ref(device_index), GO)


#if (PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)
/************************************************************************
 * LC_Get_Channel_Quality
 *
 * Get a constant reference to channel quality information for device_index
 *
 * Returns const t_ch_quality * (rf. lc_types.h)
 ************************************************************************/
#define LC_Get_Channel_Quality_Ref(device_index) \
    DL_Get_Channel_Quality_Ref(DL_Get_Device_Ref(device_index))

/************************************************************************
 * LC_Get_Last_Rx_Data_Pkt_Type
 *
 * Get a constant reference to channel quality information for device_index
 *
 * returns last DM/DH packet type received that carried user data
 *
 * Currently unused.
 *
 ************************************************************************/

/*#define LC_Get_L2CAP_Rx_Packet_Type(device_index) \
    DL_Get_L2CAP_Rx_Packet_Type(DL_Get_Device_Ref(device_index))*/

/************************************************************************
 * LC_Initialise_Channel_Quality
 *
 * Initialise channel quality information for given device_index
 ************************************************************************/
#define LC_Initialise_Channel_Quality(device_index) \
    DL_Initialise_Channel_Quality(DL_Get_Device_Ref(device_index))
#endif  /*(PRH_BS_CFG_SYS_LMP_CHANNEL_DRIVEN_QUALITY_SUPPORTED==1)*/

#if (PRH_BS_CFG_SYS_SCO_REPEATER_SUPPORTED==1)
void LC_Set_SCO_Repeater_Bit(u_int32 value);
u_int32 LC_Get_SCO_Repeater_Bit(void);
#endif

#if (PRH_BS_CFG_SYS_LMP_RSSI_SUPPORTED==1)
/************************************************************************
 * LC_Get_RSSI
 *
 * Get the most recent RSSI average on a link
 * maps directly to s_int8 DL_Get_RSSI(t_devicelink* p_device_link);
 ************************************************************************/
#define LC_Get_RSSI(device_index) \
    DL_Get_RSSI(DL_Get_Device_Ref(device_index))

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)

#define LC_Get_1Mb_RSSI(device_index) \
    DL_Get_1Mb_RSSI(DL_Get_Device_Ref(device_index))

#define LC_Get_2Mb_RSSI(device_index) \
    DL_Get_2Mb_RSSI(DL_Get_Device_Ref(device_index))

#define LC_Get_3Mb_RSSI(device_index) \
    DL_Get_3Mb_RSSI(DL_Get_Device_Ref(device_index))

#endif
#endif  /*(PRH_BS_CFG_SYS_LMP_RSSI_SUPPORTED==1)*/


#define LC_PAGE_SUSPEND_TIMER    0x20
#define LC_INQUIRY_SUSPEND_TIMER 0x20
/************************************************************************
 * LC_Get_R2P_Early_Wakeup_Slots
 *
 * Get how many slots we should start processing in advance for R2P 
 * 
 * Determine the R2P Wakeup Slots = Search Window in Frames & 0xFE
 *
 * r2p_offset_ticks    adjustment from current clock to actual r2p instant
 ************************************************************************/
#define LC_R2P_EARLY_WAKEUP_SLOTS_CONSTANT_OFFSET  0
#define LC_Get_R2P_Early_Wakeup_Slots(device_index, r2p_offset_ticks)      \
   ((USLCr2p_Get_Search_Window_Frames                                  \
           (DL_Get_Device_Ref(device_index), r2p_offset_ticks) & 0xFE) + \
            LC_R2P_EARLY_WAKEUP_SLOTS_CONSTANT_OFFSET)

#if (PRH_BS_CFG_SYS_LOW_POWER_MODE_SUPPORTED==1)
#include "uslc_chan_ctrl.h"
#include "uslc_sleep.h"
#define LC_Is_Sleep_Pending() (USLCchac_Get_Active_Procedure() == SLEEP_PROCEDURE)

/*
 * when exiting power down the USE_LF is set to 0. However, due to the latency
 * involved in Tabasco, the clearing of the SLEEP beet is delayed.
 * Need to thus check both SLEEP and USE_LF to see if asleep
 */
#define LC_Is_Tabasco_Asleep USLCsleep_Is_Tabasco_Asleep
#endif


RAM_CODE t_clock       LC_Get_Piconet_Clock_Ex(t_deviceIndex device_index, boolean in_context);
void          LC_Set_Active_Piconet(t_deviceIndex device_index, boolean change_piconet);
t_deviceIndex LC_Get_Active_Piconet(void);

boolean LC_Is_Sleep_Possible(t_clock *next_instant);

/*
 * EDR related
 */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
void LC_Set_PTT_ACL(t_deviceIndex device_index, boolean ptt_acl);
boolean LC_Get_PTT_ACL(t_deviceIndex device_index);
void LC_Set_PTT_ESCO(t_deviceIndex device_index, boolean ptt_esco);
boolean LC_Get_PTT_ESCO(t_deviceIndex device_index);
#else
#define LC_Set_PTT_ACL(device_index,ptt)
#define LC_Set_PTT_ESCO(device_index,ptt)
#endif

#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
u_int8 LC_Get_Local_EIR(t_cmd_complete_event* p_event);
u_int8* LC_Get_Local_EIR_Payload(void);
u_int8 LC_Get_Local_EIR_Length(void);
t_packet LC_Get_Local_EIR_Packet_Type(void);
u_int8 LC_Set_Local_EIR(u_int8 fec_required, u_int8* p_eir);
#endif
#if (CONFIG_CTRL_BQB_TEST_SUPPORT == 1)
#if (PRH_BS_CFG_SYS_LMP_POWER_CONTROL_SUPPORTED == 1)
void UPDATE_Power_Level_Sign( t_deviceIndex device_index );
#endif
#endif
void LC_Set_Inquiry_Tx_Power(s_int8 power_level);
s_int8 LC_Get_Inquiry_Response_Tx_Power(void);

u_int8 LC_Is_Inquiry_Suspended(void);
u_int8 LC_Get_Inq_State(void);
u_int8 LC_Is_Page_Suspended(void);
#endif
