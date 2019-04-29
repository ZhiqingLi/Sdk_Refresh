/***********************************************************************
 *
 * MODULE NAME:    le_config.c
 * PROJECT CODE:   CEVA Low Energy Single Mode
 * DESCRIPTION:    Low Energy Configuration file.
 * MAINTAINER:     Gary Fleming
 * CREATION DATE:  September 2011
 *
 *
 * LICENSE:
 *     This source code is copyright (c) 2011-2012 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
#include "sys_config.h"
#include "hc_const.h"
#include "hci_params.h"
#include "lmp_scan.h"
#include "lmp_ch.h"
#include "lmp_acl_container.h"

#include "le_advertise.h"
#include "le_scan.h"
#include "le_const.h"
#include "le_config.h"
#include "le_connection.h"
#include "le_link_layer.h"
#include "le_white_list.h"
#include "le_frequency.h"
#include "le_scan.h"
#include "sys_rand_num_gen.h"
#include "hw_radio.h"

#include "bt_timer.h"
#include "lslc_irq.h"
#include "le_white_list.h"

#include "hw_lc.h"
#include "hw_le_lc.h"

#include "tc_event_gen.h"
#include "tc_const.h"

#include "app_beken_includes.h"
#include "lmp_sco_container.h"

#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)//merge CEVA 0517
#include "le_att.h"//merge CEVA 0517
#endif//merge CEVA 0517
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)//merge CEVA 0517
#include "le_smp.h"//merge CEVA 0517
#include "le_security_db.h"//merge CEVA 0517
#endif//merge CEVA 0517
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)//merge CEVA 0517
#include "le_gap.h"//merge CEVA 0517
#endif//merge CEVA 0517
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)//merge CEVA 0517
#include "le_gatt_server.h"//merge CEVA 0517
#endif//merge CEVA 0517
#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)//merge CEVA 0517
#include "le_l2cap.h"//merge CEVA 0517
#endif//merge CEVA 0517
#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES == 1)
#include "le_cts_app.h"
#endif
#include "uslc_scheduler.h"

#include "sys_types.h"
#include "hw_hab_defs.h"
#include "hw_macro_defs.h"
#include "hw_habanero_impl.h"
#if (PRH_BS_CFG_SYS_LE_CONTROLLER_SUPPORTED==1)
t_LE_Config LE_config;
extern volatile u_int8 le_mode;
u_int16 backup_Encryp_mode;
#define LE_MAX_NUMBER_OF_EVENTS 20
extern t_LMconfig g_LM_config_info;

/*
 * Initial release is a fully functional system with all states and state combinations supported.
 * Later to be combined with Build options which will define the supported states in any given
 * configuration.
 */
const u_int8 LEconfig_Classic_BT_Features[8] = {0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x00}; /* LE supported, BR/EDR not supported */
// BT 4.0 Supported States
//const u_int8 _LEconfig_supported_states[8] = {0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00};
// BT 4.1 Supported States
// 10 june - dont allow initiating and scanning
const u_int8 _LEconfig_supported_states[8] = {0xFF,0xFF,0x3F,0xFF,0x00,0x00,0x00,0x00};

const u_int8 _LEconfig_local_features[8]   = {PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED,
		                                      0x00,0x00,0x00,0x00,0x00,0x00,0x00};

u_int8 LEconfig_device_address[6] = {0x66,0x32,0xEE,0x66,0x32,0xEE};//LE address distinguish by BT address,modified by yangyang

void LEconfig_Complete_Switch_To_Classic(void);
//void *le_call_back_list[LE_MAX_NUMBER_OF_EVENTS];
u_int8 g_LE_Config_Classic_Dev_Index = 0;
u_int8 g_LE_switch_to_BT_Classic_Pending=0;
u_int8 g_need_to_switch_to_classic = 0x00;


/**************************************************************************
 * FUNCTION :- LEconfig_Init
 *
 * DESCRIPTION
 * Initialisation of the LE_config container and all its elements.
 *
 **************************************************************************/

t_error LEconfig_Init(void)
{
#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1)
	u_int8 i;
#endif

#ifdef BT_DUALMODE
    HW_set_XVR_SW_reset(1);
#endif

	BDADDR_Set_to_Zero(&LE_config.random_address);
	g_LE_Config_Classic_Dev_Index = 0;
	LEadv_Init();
	LEscan_Init();

	LE_config.hc_data_tester_present = 0;
	LE_config.next_initiator_scan_timer = BT_TIMER_OFF;
	LE_config.initiating_active = 0;
	LE_config.state = STANDBY_STATE;
	LE_config.sub_state = SUBSTATE_IDLE;
	LE_config.event_mask = 0x3F;
	LE_config.sleep_clock_accuracy = PRH_BS_CFG_MASTER_SLEEP_CLOCK_ACCURACY;
	LE_config.sleep_request_pending = 0;
#if (PRH_BS_CFG_SYS_LE_BQB_TESTER_SUPPORT==1)
	LE_config.tester_test_config = 0;
	LE_config.tester_introduce_mic_error = 0;
	LE_config.tester_introduce_header_error = 0;
#endif

#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1)
	LE_config.link_id_pool = 0xFFFFFFFF;
	LE_config.next_connection_event_time = BT_TIMER_OFF;

	for (i=0;i<4;i++)
		LE_config.data_channel_map[i]=0xFF;

	LE_config.data_channel_map[4] = 0x1F;
	LE_config.num_le_links=0;
	LE_config.connection_complete_pending = 0;
	LE_config.p_connection_complete_pending = 0;
	
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
	LE_config.link_key_request_timer = BT_TIMER_OFF;
	LE_config.encrypt_pending_flag=0x00;
	LE_config.device_index_for_ltk = 0x00;
#endif
	LEconnection_Init();
#endif

	LE_config.device_sleep=0;
	LE_config.tabasco_asleep = 0;
	LE_config.test_mode = 0;
	LE_config.slave_links_active = 0;
	LE_config.master_link_active = 0;
	LEwl_Clear_White_List();
	
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
	LEsmp_Init_SMP();
#if ((PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_SMP_PERIPHERAL_SUPPORTS_KEY_HIERARCHY!=1))
	LEsecdb_Clear_Security_DB();
#endif
#endif

	HWle_set_whitening_enable();

	// Full investigation of below options required for Each Specific Radio
	//!!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// This one of the most sensitive parameter settings in HW for LE

	HWle_set_delay_search_win(55);
    HWle_set_tifs_delay(TIFS_TX_ADJUSTMENT-1); /* 150 us */

	/* Ensure that no TIFS count is running	 */
	HWle_abort_tifs_count();
	HWle_clear_tifs_default();
	
	LE_config.Auto_Widen_MultiSlot = 0;
#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
	LE_config.auto_advertise = 0x00;
	LE_config.auto_initiate = 0x00;
	LE_config.slave_listen_outside_latency = 0;
	LE_config.TCI_transmit_win_config = 0x00;
#endif
	LE_config.trace_level = 0x31;
	LE_config.resuming_scan = 0;
	LE_config.initiating_enable = 0;

#if (SYS_CFG_LE_ADVERTISE_BY_DEFAULT_TO_OVERCOME_ORCA_RF_ISSUE==1)
#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
	LE_config.auto_advertise = 0x01;
#endif
	LEadv_Set_Advertise_Enable(1);
#else
#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)
	LE_config.auto_advertise = 0x00;
#endif
#endif

	LEfreq_Init();

#if (PRH_BS_CFG_SYS_CHANNEL_ASSESSMENT_SCHEME_SUPPORTED==1)
	LE_config.device_channel_map_update_timer = BT_TIMER_OFF;
	LE_config.device_channel_map_update_timeout = DEFAULT_LE_DEVICE_CHANNEL_MAP_UPDATE_TIMEOUT;
#endif

#ifdef BT_DUALMODE
/* charles add start */
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
	GATT_init();
	GATTserver_Init();
#endif
	
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
	GAP_Init();
#endif
	
#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)
	LE_L2CAP_Init();
#endif
#if ((PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1) && (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1))
	LE_APP_Init();
	//GAP_Discoverable_Mode(GAP_GENERAL_DISCOVERABLE,ADV_IND);
#endif
/* charles add end */
#endif
	return NO_ERROR;
}

/*************************************************************************
 * LEconfig_Register_Event_Handler
 *
 * Register the presented event handler for immediate call back on event
 *************************************************************************/
void LEconfig_Register_Event_Handler(u_int8 event_id,
                                 void *func_ptr)
{
  // Registration for HCI not complete yet - optional extra //(PRH_BS_CFG_SYS_LE_HCI_INCLUDED)
  //  le_call_back_list[event_id] = func_ptr;
}


/**************************************************************************
 * FUNCTION :- LEconfig_Set_Event_Mask
 *
 * DESCRIPTION
 * Sets the HCI event mask for LE events.
 *
 **************************************************************************/

void LEconfig_Set_Event_Mask(u_int8* p_pdu)
{
	/*
	 * As only one of the eight bytes in the event mask is actually used.
	 * We restrict our event mask to a single byte.
	 */

    LE_config.event_mask = p_pdu[0];
}

/**************************************************************************
 * FUNCTION :- LEconfig_Get_LE_Features_Ref
 *
 * DESCRIPTION
 * Returns a pointer to the set of LE supported features.
 *
 **************************************************************************/


const u_int8* LEconfig_Get_LE_Features_Ref(void)
{
	return _LEconfig_local_features;
}

/**************************************************************************
 * FUNCTION :- LEconfig_Is_Features_Encryption_Supported
 *
 * DESCRIPTION
 * Used to determine if encryption is supported in the current device.
 * Should be replaced by a build parameter as this is a waste of code.
 *
 **************************************************************************/


u_int8 LEconfig_Is_Features_Encryption_Supported(void)
{
	return (_LEconfig_local_features[0] & PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED);
}

/**************************************************************************
 * FUNCTION :- LEconfig_Set_Random_Address
 *
 * DESCRIPTION
 * Sets the RANDOM Address to be used by a device.
 *
 **************************************************************************/

void LEconfig_Set_Random_Address(u_int8* p_random_address)
{
	BDADDR_Assign_from_Byte_Array(&LE_config.random_address, p_random_address);
}


/***************************************************************************
 * Function :- LEconfig_Read_Supported_States
 *
 * Description :-
 * Simple function to return the current combination of states supported in
 * an LE device.
 *
 * Context :- Schedular
 ***************************************************************************/

const u_int8* LEconfig_Read_Supported_States(void)
{
	return _LEconfig_supported_states;
}

/***************************************************************************
 * Function :- LEconfig_Check_Current_Address
 *
 * Description :-
 * This fuction checks if a given address and address type match the local
 * address type and address.
 *
 * Context :- IRQ
 ***************************************************************************/

u_int8 LEconfig_Check_Current_Address(u_int8 RxAdd,t_p_pdu p_pdu)
{
	// 
	//  Checks if there is a match to the current device address..
	// 
	u_int8 i;

	if (
#if (PRH_BS_CFG_SYS_LE_OBSERVER_DEVICE==1) || (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1)
		((LE_config.state == SCANNING_STATE) && ( RxAdd == LEscan_Get_Scanning_Own_Address_Type())) ||
		((LE_config.state == INITIATING_STATE) && ( RxAdd == LEscan_Get_Scanning_Own_Address_Type())) ||
#endif
		 ((LE_config.state == ADVERTISING_STATE) && (RxAdd == LEadv_Get_Advertising_Own_Address_Type())))

	{
		const t_bd_addr* p_bd_addr;

		if (RxAdd == 0x01 /* RANDOM_ADDRESS */)
		{
			p_bd_addr = &LE_config.random_address;
		}
		else
		{
			//p_bd_addr = SYSconfig_Get_Local_BD_Addr_Ref();
			p_bd_addr = (t_bd_addr *)LEconfig_device_address;
		}

		for (i=0;i<6;i++)
		{
			if(p_pdu[i] != p_bd_addr->bytes[i])
				return 0;
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1)

/***************************************************************************
 * Function :- LEconfig_Allocate_Link_Id
 *
 * Description :-
 * This function allocates an Identifier for an LE link.
 *
 ***************************************************************************/

u_int8 LEconfig_Allocate_Link_Id(u_int8* p_link_id)
{
	u_int8 i=0;
	u_int32 pool_entry;
	// This allocates a free link id.
	// use a u_int32 to store 32 link id as a bitfield.

	while( i < DEFAULT_MAX_NUM_LE_LINKS)
	{
		pool_entry = (1 << i);
		if ( LE_config.link_id_pool & pool_entry)
		{
			LE_config.link_id_pool &= ~pool_entry;
			*p_link_id = i;
			return NO_ERROR;
		}
		else
		{
			i++;
		}
	}
	return UNSPECIFIED_ERROR;
}

/***************************************************************************
 * Function :- LEconfig_Free_Link_Id
 *
 * Description :-
 * This function allocates an Identifier for an LE link.
 *
 ***************************************************************************/

void LEconfig_Free_Link_Id(t_LE_Connection* p_connection)
{
	LE_config.link_id_pool |=  (1 << p_connection->link_id);
}


/***************************************************************************
 * Function :- LEconnections_Link_In_Use
 *
 * Description :-
 * This funtion checks if a link id is in use.
 *
 ***************************************************************************/

u_int8 LEconnections_Link_In_Use(u_int8 link_id)
{
	return !(LE_config.link_id_pool & (1 << link_id));
}

#if (PRH_BS_CFG_SYS_EXTENDED_HCI_COMMANDS_SUPPORTED==1)

/***************************************************************************
 * Function :- LEconfig_TCI_Set_TIFS_Tx_Adjustment
 *
 * Description :-
 * This functions is used to support the equivalent TCI command. This writes
 * the TIFS adjustment directly to the HW.
 *
 ***************************************************************************/

t_error LEconfig_TCI_Set_TIFS_Tx_Adjustment(u_int8 tifs_adjustment)
{
	HWle_set_tifs_delay(tifs_adjustment);

	TCeg_Command_Complete_Event(TCI_LE_SET_TIFS_TX_ADJUSTMENT, NO_ERROR);
	return NO_ERROR;
}

/***************************************************************************
 * Function :- LEconfig_TCI_Set_Search_Window_Delay
 *
 * Description :-
 * This functions is used to support the equivalent TCI command. This writes
 * the Search Delay directly to the HW.
 *
 ***************************************************************************/

t_error LEconfig_TCI_Set_Search_Window_Delay(u_int8 search_win_delay)
{
	HWle_set_delay_search_win(31-search_win_delay);

	TCeg_Command_Complete_Event(TCI_LE_SET_SEARCH_WINDOW_DELAY, NO_ERROR);
	return NO_ERROR;
}

#endif


void LEconfig_Delay_Next_Initiating(u_int8 delay)
{
	if (LE_config.next_initiator_scan_timer != BT_TIMER_OFF)
	{
		LE_config.next_initiator_scan_timer = BTtimer_Safe_Clockwrap(LE_config.next_initiator_scan_timer + delay);
	}
}

void LEconfig_Switch_To_LE(u_int8 device_index,u_int8 immediate)
{
	// Determine the Next LE Activity --
	// This will determine what Role the device should Take
	//    1/ Advertising -- Slave Role
	//    2/ Scanning   -- Master Role
	//    3/ Initiating -- Master Role
	//    4/ Slave ACL  -- Slave Role
	//    5/ Master ACL -- Master Role
    // Note :- If we are switching from a Classic Slave Role - then we need to ensure
	// we are in Classic Master Mode before we attempt the Switch to the LE activity.
	
	t_LE_Connection* p_connection ; 
	p_connection = LEconnection_Find_Link_Entry(LE_config.active_link);
	g_LE_Config_Classic_Dev_Index = device_index;
	
	if(immediate == 0)
	{
		HW_set_slave(1);
		HWle_set_le_mode();
		le_mode = 0x01;

		/* Clear the TX and RX modes */
		HW_set_rx_mode(0x00);
	    HW_set_tx_mode(0x00);
	    
		/* Disable All IRQs except Tim 0 and Tim 2,On the Tim 0/2 complete the remainder of the Switch */
		LSLCirq_Disable_All_Tim_Intr_Except(JAL_TIM0_INTR_MSK_MASK | JAL_TIM2_INTR_MSK_MASK);
		LE_config.state = LE_W4_MASTER_TIM0_OR_TIM2;
	}
	else
	{
		/* LEconfig_Switch_To_LE immediate == 0 part*/
        le_mode = 0x01;
		HW_set_slave(1);//for BLE slave only
		HWle_set_le_mode();
        HWle_clear_adv_state();
		HW_set_page(0);
        HWhab_Set_Sync_Error(0x2);

		/* HWradio_Switch_To_LE_Mode() part */ 
		HWradio_SetRadioMode(RADIO_MODE_STANDBY);
		HW_set_rx_mode(0x00);
        HW_set_tx_mode(0x00);
        HWle_clear_tx_enable();
        HWle_abort_tifs_count();
		HWle_clear_tifs_default();
        backup_Encryp_mode = HW_get_encrypt();
		HW_set_encrypt(0);
        
    	/* fully decouple LE power levels from BT Classic power levels  */
		HWradio_Init_Tx_Power_Level(MAX_POWER_LEVEL_UNITS);
    	HW_set_whiten(1);
		HWle_set_whitening_enable();
         
		/* LEconnection_Handle_Connection_Event */
        p_connection->latency_count=0;
		p_connection->current_data_channel = LEfreq_Get_Next_Data_Channel_Frequency(p_connection);
		p_connection->connection_event_rf_channel = LEfreq_Map_Channel_Index_2_RF_Freq(p_connection->current_data_channel);
		BK3000_Write_RF_CH(p_connection->connection_event_rf_channel);
		HWle_set_whitening_init(p_connection->current_data_channel);
        HWle_set_crc_init(p_connection->crc_init);
	    HWle_set_acc_addr(p_connection->access_address);
		HW_set_intraslot_offset(p_connection->intraslot);
        
	    HWradio_Setup_cur_cfg_win(0);/* charles modify */
		HW_set_win_ext(32);
		HWle_set_tifs_default(); 
        
        if(LEconnection_Get_Initial_Anchor_Point_Obtained(p_connection) == 0)
        {
            HW_set_rx_mode(0x03);
            p_connection->transmit_window_remaining = p_connection->transmit_window_size+1;
        }
        else
        {
            HW_set_rx_mode(0x01);
            p_connection->transmit_window_remaining = 1;
        }
		
        LE_config.state = CONNECTION_STATE;
        LEconnection_Set_Substate(p_connection, W4_SLAVE_CONN_FIRST_RX);
        LEconnection_Set_First_Sync_In_Connection_Event(p_connection,0x00);
        LSLCirq_Disable_All_Tim_Intr_Except(JAL_TIM0_INTR_MSK_MASK | JAL_TIM2_INTR_MSK_MASK);/* charles modify */
	}
	
}

void LEconfig_Complete_Switch_To_LE(void)
{
	HWradio_Switch_To_LE_Mode();
	HW_set_page(0);

	// Determine what the next LE activity is
	//  ACL Link (Master or Slave), Scanning, Initiating or Advertising

	if(LEadv_Get_Advertising_Enable())
		HW_set_slave(0);
	else
		HW_set_slave(1);
    backup_Encryp_mode = HW_get_encrypt();
	HW_set_encrypt(0);
}

void LEconfig_Switch_To_BT_Classic(void)
{
	t_lmp_link *p_link;

#if (BT_MODE == BT_MODE_BLE)
	return;
#endif

#ifdef BEKEN_BLE_OTA_3266
    /* If ota updating, forbid sleep */
    extern uint8 app_ota_is_updating(void);// OTAÕýÔÚÉý¼¶
    if(app_ota_is_updating()&& (!bt_flag1_is_set(APP_FLAG_ACL_CONNECTION)))
        return;
#endif 

	// Note we should only switch back to Classic if the next Classic Activity is closer in time than
	// the next LE activity.
	// However - we have to consider that Sleep is performed as a Classic Activity - so if the LE requests
	// Sleep we should return to the Classic.
	
#ifdef LE_MINI_INTERV_WITH_MUSIC_SOLUTION_ENBALE
	if (1)
	{
		extern bool_t le_mini_conn_interval_flag;
		extern u_int16 le_mini_conn_interval_timeout;
		
	  	if((le_mini_conn_interval_flag) && (le_mini_conn_interval_timeout < 7)) //(7-1)*7.5ms = 45ms,yangyang,2018/6/29
			return;
	}
#endif

	LE_config.sleep_request_pending = 0x00;
	p_link = LMaclctr_Find_Device_Index(g_LE_Config_Classic_Dev_Index);

    /* If scanning is the only thing active in classic. Then we need to set the Slave bit when switching to Classic */
	HW_set_sync_det_intr_mask(0);
	HW_set_pkd_intr_mask(0);

   	LSLCirq_Disable_All_Tim_Intr_Except(JAL_TIM0_INTR_MSK_MASK |JAL_TIM2_INTR_MSK_MASK | JAL_TIM1_INTR_MSK_MASK |JAL_TIM3_INTR_MSK_MASK);
	HWradio_Switch_To_Classic_BT_Mode();
    HW_set_encrypt(backup_Encryp_mode);
	LMch_Change_Piconet(p_link);

    if(LMscoctr_Get_Number_eSCO_Connections() > 0)
    {
        g_need_to_switch_to_classic = 0x01;
    }
    else
    {
        g_need_to_switch_to_classic = 0x00;
    }

	if(g_need_to_switch_to_classic == 1)
	{
		g_need_to_switch_to_classic = 2;
		USLCsched_Force_switch_piconet();
		return;
	}
}
#if (SEAN_MODIFY_REF_BQB == 0)

void LEconfig_Complete_Switch_To_Classic(void)
{
	t_lmp_link *p_link = LMaclctr_Find_Device_Index(g_LE_Config_Classic_Dev_Index);

	g_LE_switch_to_BT_Classic_Pending = 0;
	// However - we have to consider that Sleep is performed as a Classic Activity - so if the LE requests
	// Sleep we should return to the Classic.
	if (LE_config.sleep_request_pending == 0)
	{
		u_int32 native_clk = HW_Get_Native_Clk_Avoid_Race();//no used
		t_slots slots_to_next_classic_activity = LMpol_Check_Slots_To_Next_Classic_Activity(native_clk);
		if (slots_to_next_classic_activity==0xFFFF)
		{
			// No Activity on Classic so stay on LE if LE has an activity
			if (LE_LL_Is_LE_Active() &&  (g_LM_config_info.num_acl_links==0)/* No ACL Links */
				&& !(LMconfig_LM_Check_Device_State(PRH_DEV_STATE_INQUIRY_PENDING|PRH_DEV_STATE_PAGE_PENDING)))
				return;
		}
		else // We have an LE and Classic Activity
		{
			if (LE_LL_Is_LE_Active())
			{
				u_int32 slots_to_next_LE_activity = LE_LL_Slots_To_Next_LE_Activity(native_clk);

				if ((g_LM_config_info.num_acl_links==0x00) && !(LMconfig_LM_Check_Device_State(PRH_DEV_STATE_INQUIRY_PENDING|PRH_DEV_STATE_PAGE_PENDING)))
				{
					if (slots_to_next_LE_activity < slots_to_next_classic_activity )
						return;
				}
			}
		}
	}
	LE_config.sleep_request_pending = 0x00;


    // If scanning is the only thing active in classic. Then we need to set the
	// Slave bit when switching to Classic.
	HW_set_sync_det_intr_mask(0);
	HW_set_pkd_intr_mask(0);

   	LSLCirq_Disable_All_Tim_Intr_Except(JAL_TIM0_INTR_MSK_MASK |JAL_TIM2_INTR_MSK_MASK | JAL_TIM1_INTR_MSK_MASK |JAL_TIM3_INTR_MSK_MASK);

  	HWradio_Switch_To_Classic_BT_Mode();

	if ((p_link) &&  (p_link->state != LMP_IDLE))
		LMch_Change_Piconet(p_link);
	else
	{
		// If master links active - set the local device link as active ...
		// However if Slave links are active - we need to switch to the explict piconet.
		
		if(!LMconfig_LM_Connected_As_Slave())
		{
			LMch_Change_Piconet(0);
		}
		else
		{

			// Find the slave device index to switch to.
			extern t_link_entry link_container[PRH_MAX_ACL_LINKS];
			t_lmp_link* p_link = 0;
			int i;

			for (i = 0; i < PRH_MAX_ACL_LINKS; i++)
			{
				if (link_container[i].used)
				{
				    //os_printf("link_container[i].used=%d\n",);
					p_link = &(link_container[i].entry);
					if (p_link && (SLAVE == p_link->role) && (p_link->state != LMP_IDLE))
						break;
				}
			}
			if (p_link)
			{
              //os_printf("LC_Set_Active_Piconet 1\n");//xhe
				LC_Set_Active_Piconet(p_link->device_index,TRUE);
			}

		}

	}
	{
		//extern u_int8 g_LE_switch_to_LE_on_NO_PKD;
		//extern u_int8 g_LE_switch_to_LE_pending;

		//g_LE_switch_to_LE_on_NO_PKD=0;
		//g_LE_switch_to_LE_pending=0;
	}
	
	
}
#endif
#endif
#endif
#endif

