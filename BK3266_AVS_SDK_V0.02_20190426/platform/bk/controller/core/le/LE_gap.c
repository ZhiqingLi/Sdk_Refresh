/***********************************************************************
 *
 * MODULE NAME:    LE_gap.c
 * PROJECT CODE:   CEVA Low Energy Single Mode
 * DESCRIPTION:    Low Energy GAP support.
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
#include "sys_types.h"
#include "tra_queue.h"
#include "sys_features.h"
#include "le_advertise.h"
#include "le_link_layer.h"  
#include "le_gap.h"
#include "le_gap_const.h"
#include "le_const.h"
#include "hw_lc.h"
#include "le_scan.h"
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED == 1)
#include "le_att.h"
#include "le_gatt_server.h"
#endif
#include "le_connection.h"
#include "bt_timer.h"
#include "le_config.h"
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED == 1)
#include "le_att.h"
#endif
#include "lmp_types.h"
#include "lmp_utils.h"
#include "hc_const.h"
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED == 1)
#include "le_smp.h"
#include "le_security_db.h"
#include "le_security.h"
#endif
#include "le_advertise.h"
#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)
#include "le_l2cap.h"
#endif
#include "hw_le_lc.h"
//#include "le_cts_app.h"

u_int8 GAP_Adv_Data[31];
u_int8 GAP_ScanResp_Data[31];
u_int8 GAP_Adv_Data_Length;
u_int8 GAP_ScanResp_Data_Length;
static u_int32 GAP_Adv_Interval_Min = 800;//128;//160;//480;//32;   /*160=100ms;320=200ms;480=300ms,640=400ms;800=500ms*/
static u_int32 GAP_Adv_Interval_Max = 800;//;//160;//32;

//static u_int32 ACL_link_Exist_GAP_Adv_Interval_Min = 120;   /*160=100ms;320=200ms;480=300ms,640=300ms;800=500ms*/
//static u_int32 ACL_Link_Exist_GAP_Adv_Interval_Max = 120;

u_int8 dummy_addr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

u_int8 GAP_Last_Adv_Packet_Reported = 0;
#define GAP_MAX_NUM_PROTOCOL_USERS  2

void* GAP_Event_Callback_Array[GAP_MAX_NUM_PROTOCOL_USERS];
void* GAP_Command_Complete_Callback_Array[GAP_MAX_NUM_PROTOCOL_USERS];

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED == 1)
static u_int8 GAP_proposed_reconnection_address[6];
#endif

#if ((PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS !=1) && ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)))
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
static u_int8 GAP_proposed_reconnection_address[6];
#endif
#endif
typedef struct t_GAP_config {

	u_int32 TGAP_Limited_Advertising_Timeout;
	u_int32 TGAP_Private_Address_Timeout;
	u_int32 TGAP_conn_param_Timeout;
	u_int8 Discoverable_Mode;
	u_int8 Discovery_Mode;
	u_int8 Connectable_Mode;
	u_int8 Connect_Mode;
	u_int8 scan_type;
	u_int8 general_connect_state;
	u_int8 own_addr_type;
	u_int8 state;
	u_int16 max_conn_interval;
	u_int16 min_conn_interval;
	u_int16 latency;
	u_int16 supervision_timeout;
	u_int16 gatt_handle;
	u_int16 pending_handle;
	u_int8 Connect_Params_Update_Pending;
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS==1)
	u_int16 peer_GAP_start;
	u_int16 peer_GAP_end;
#endif
	u_int8 Broadcast_Mode; // Ox01 - Broadcast / 0x02 - Observer
	u_int8 Broadcast_Type; // if Broadcaster - this indicates the Adv Type used.
	                       // if Observer - this indicates Passive/Active Scan

} t_GAP_config;

t_GAP_config     LEgap_Config;

#if (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE == 1)
/* extern t_App_Control LEapp_Config;*/
#define DEFAULT_ADV_DATA_LEN  30//26
#define DEFAULT_SCAN_RESP_DATA_LEN 21

u_int8 default_adv_data[DEFAULT_ADV_DATA_LEN] =
								{ /* Flags Field */ 0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/,
		                          /* Tx Power    */ 0x02,0x0A,0x00,
		                          /* Local name  */ 0x05,0x08,'C','E','V','A',
				                  /* Services    */ 0x0D,0x02, 0x16, 0x18,0x01,0x18,0x0d,0x18,0x03,0x18,0x04,0x18,0x00,0x18 };

// #ifdef _FIND_ME_TI_SIM_
// const u_int8  Find_ME_adv_data[DEFAULT_ADV_DATA_LEN]=
// 													{
// 														/* Flags Field */ 0x02,0x01,0x06 /*GAP_LE_BR_EDR_NOT_SUPPORTED*/,
// 														/* Tx Power    */ 0x02,0x0A,0x00,
// 														///* Local name  */ 0x0B,0x09,'S','e','c','u','r','i','T','a','g','B',
// 														/* Services //0x0D data length,0x02 16-bits UUIDS incomplete */ 0x07,0x02,0x03,0x18,0x02,0x18,0x04,0x18
// 													};
// #endif

u_int8 default_scan_resp_data[DEFAULT_SCAN_RESP_DATA_LEN] =
            					{/*Le Host+No OOB+ Public */ 0x02,0x11,0x02,
                                           /*UUID for GAP */ 0x03,0x16,0x18,0x00,
                                           /*UUID for GATT*/ 0x03,0x16,0x18,0x01,
                                           /* Services    */ 0x07,0x02,0x00,0x18,0x01,0x18,0x0d,0x18 };

// #ifdef _FIND_ME_TI_SIM_
// const u_int8  Find_ME_scan_resp_data[DEFAULT_SCAN_RESP_DATA_LEN] =
//                        {
// 												  0x0A,0x09,'S','e','c','u','r','i','T','a','g'
//                        };												 
// #endif
#endif
u_int8 GAP_Determine_Random_Addr_Type(u_int8 MSB_Addr);
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED == 1)
void GAP_GATT_Event_Handler(u_int8 eventCode, t_gatt_event_params* p_gatt_event_params);
void GAP_GATT_Command_Complete_Event(u_int8 operation_id,t_gatt_command_complete_params* params);
#endif
t_error GAP_Connection_Complete_Generate_Event(u_int8 status,u_int16 handle,u_int8 valid_reconn_addr, u_int8* p_address,u_int8 encrypt,u_int8 role);
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS ==1)
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
void _GAP_Init_GAP_Discovery(t_LE_Connection* p_connection);
#endif
#endif
u_int32 gCurrent_ADV_interval;
/**************************************************************************
 * FUNCTION :- GAP_Init
 *
 * DESCRIPTION
 * This simple function initialises the GAP Config Structure and the AD Data.
 *
 *************************************************************************/

u_int8 GAP_Init(void)
{
	u_int8 i;
	LEgap_Config.Discoverable_Mode = 0x00;
	LEgap_Config.Discovery_Mode = 0x00;
	LEgap_Config.Connectable_Mode = 0x00;
	LEgap_Config.Connect_Mode = 0x00;
	LEgap_Config.own_addr_type = PUBLIC_ADDRESS;
	LEgap_Config.Connect_Params_Update_Pending = 0x00;
	LEgap_Config.Broadcast_Mode = 0x00;
	LEgap_Config.scan_type = LE_ACTIVE_SCAN;
	LEgap_Config.state = GAP_IDLE;
	LEgap_Config.TGAP_Private_Address_Timeout = BT_CLOCK_MAX_TIMEOUT;

#if (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE == 1)

// !! To DO !!
//  Review if any of the below code is needed
//  As advertising data is controled by APPs.
//  Below code may be redundant !!
	for(i=0;i<DEFAULT_ADV_DATA_LEN;i++)
		GAP_Adv_Data[i] = default_adv_data[i];
	GAP_Adv_Data_Length = DEFAULT_ADV_DATA_LEN;
	LEadv_Set_Advertising_Data(GAP_Adv_Data_Length,GAP_Adv_Data);
    GAP_Set_ScanResp_Data(DEFAULT_SCAN_RESP_DATA_LEN,(u_int8*)default_scan_resp_data);
    LEadv_Set_Scan_Response_Data(GAP_ScanResp_Data_Length,default_scan_resp_data);
#endif
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED == 1)
    // Register GATT Event and GATT Command Complete event handlers
    GATT_Register_GAP_Callbacks((void*)GAP_GATT_Event_Handler,(void*)GAP_GATT_Command_Complete_Event);

    GAP_Event_Callback_Array[0] = 0x00;
#endif

	return NO_ERROR;
}

void LEadv_Advertise_Disable_Enable(unsigned char enable)
{
    if(enable)
    {  // enable Advertising
        if(!LEconnection_LE_Connected() && !LEadv_Get_Advertising_Enable())
        {
            if(LMscoctr_Get_Number_eSCO_Connections() > 0)
                return ;
			os_printf("LEadv enable\r\n");
            LEadv_Set_Advertise_Enable(0x01); // enable Advertising
        }
    }
    else
    { // Disable Advertising 
        if(LEadv_Get_Advertising_Enable())
        {
            os_printf("LEadv disable\r\n");
            LEadv_Disable_Connectable_Advertising();
        }

        if(LEconnection_LE_Connected())
        {
            GAP_Disconnect(0xFFFF);
        }
    }
}

/**************************************************************************
* FUNCTION :- GAP_Register_Callbacks
*
* DESCRIPTION
* This function is responsible for registering callbacks functions the GAP_Event
*
*************************************************************************/

void GAP_Register_Callbacks(void* event_funct_ptr, void* command_complete_funct_ptr)
{
	GAP_Event_Callback_Array[0] = event_funct_ptr;
}

/**************************************************************************
* FUNCTION :- GAP_DeRegister_Callbacks
*
* DESCRIPTION
* This function is responsible for De-registering callbacks functions the GAP_Event
*
*************************************************************************/

void GAP_DeRegister_Callbacks(void)
{
	GAP_Event_Callback_Array[0] = 0x00;
}

#if ((PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE == 1) || (PRH_BS_CFG_SYS_LE_BROADCAST_DEVICE==1))
/**************************************************************************
 * FUNCTION :- GAP_Set_AD_Data
 *
 * DESCRIPTION
 * Sets the Advertising Data Fields.
 *
 *************************************************************************/

void GAP_Set_AD_Data(u_int8 length,u_int8* p_data)
{
	u_int8 i=0;
	if (length > 0x1f)
		length =  0x1f;
    
	for (i=0;i<length;i++)
		GAP_Adv_Data[i] = p_data[i];

	GAP_Adv_Data_Length = length;

#if 1 // UPF 44 - GF Added to ensure that Adv Data is written correctly by Applications
	LEadv_Set_Advertising_Data(GAP_Adv_Data_Length,GAP_Adv_Data);
#endif
}

/**************************************************************************
 * FUNCTION :- GAP_Set_ScanResp_Data
 *
 * DESCRIPTION
 * Sets the Scan Response Data Fields.
 *
 *************************************************************************/
void GAP_Set_ScanResp_Data(u_int8 length,u_int8* p_data)
{
	u_int8 i;
	if (length > 0x1f)
		length =  0x1f;
    
	for (i=0;i<length;i++)
		GAP_ScanResp_Data[i] = p_data[i];

	GAP_ScanResp_Data_Length = length;

    // UPF 44 - GF Added to ensure that Adv Data is written correctly by Applications
	LEadv_Set_Scan_Response_Data(GAP_ScanResp_Data_Length,GAP_ScanResp_Data);

}
u_int8 GAP_Set_Advertising_Interval(u_int32 min, u_int32 max)
{
	if(min>max)
	{
		return 1;
	}
	
	/*The advInterval shall be an integer multiple of 0.625 ms in the range of 20 ms
	to 10.24 s.*/
	if((min<32) || (min>16384))
	{
		return 2;
	}
	if((max<32) || (max>16384))
	{
		return 2;
	}
	
	GAP_Adv_Interval_Min = min;
	GAP_Adv_Interval_Max = max;
	return 0;
}

/**************************************************************************
 * FUNCTION :- GAP_Discoverable_Mode
 *
 * DESCRIPTION
 * Sets the Discoverable mode of the Device, and the advertising type which should be
 * used if the device is discoverable.
 *
 * INPUTS :
 * u_int8 mode     :- The Discoverable mode, one of the following.
 * 			GAP_NON_DISCOVERABLE        - 0x00
 * 			GAP_LIMITED_DISCOVERABLE    - 0x01
 * 			GAP_GENERAL_DISCOVERABLE    - 0x02
 *
 * u_int8 adv_type :- The advertising type to be used.
 *          ADV_IND         = 0x00,
 *          ADV_DIRECT_IND  = 0x01,
 *          ADV_SCAN_IND    = 0x02,
 *          ADV_NONCONN_IND = 0x03
 *
 *************************************************************************/

u_int8 GAP_Discoverable_Mode(u_int8 mode,u_int8 adv_type,u_int8 Dual_mode)

{
	t_adv_params adv_params;
	u_int8 i = 0;

	// Find the flags field
#if 1
	while(i < GAP_Adv_Data_Length)
	{
		if ((GAP_Adv_Data[i]==0x02) && (GAP_Adv_Data[i+1] == 0x01))
		{
			GAP_Adv_Data[i+2] = (mode + GAP_LE_BR_EDR_NOT_SUPPORTED);
			i+= (GAP_Adv_Data[i]+1);
			break;
		}
		else
		{
			i+= (GAP_Adv_Data[i]+1);
		}
	}


	if ( i >= GAP_Adv_Data_Length )
	{
		GAP_Adv_Data[GAP_Adv_Data_Length] = 0x02;
		GAP_Adv_Data[GAP_Adv_Data_Length+1] = 0x01;
		GAP_Adv_Data[GAP_Adv_Data_Length+2] = (mode + GAP_LE_BR_EDR_NOT_SUPPORTED);

		GAP_Adv_Data_Length+=3;
	}

#else
	GAP_Adv_Data[2] = (mode + GAP_LE_BR_EDR_NOT_SUPPORTED);
#endif

	if (mode == GAP_NON_DISCOVERABLE )
	{
		LEadv_Set_Advertise_Enable(0x00);
		LEadv_Set_Advertising_Data(GAP_Adv_Data_Length,GAP_Adv_Data);
		goto write_adv_parameter;/* TP/BROB/BCST/BV-01-C  GAP layer ,add this code */

	}
	else if ((mode == GAP_LIMITED_DISCOVERABLE) || (mode == GAP_GENERAL_DISCOVERABLE))
	{
		// If LIMITED_DISCOVERABLE :- Limited Discoverable Flag = 1,  General Discoverable Flag  = 0
		// If GENERAL_DISCOVERABLE :- Limited Discoverable Flag = 0,  General Discoverable Flag  = 1

		// A device in the general/limited discoverable mode sends either non-connectable advertising
		// events, scannable undirected advertising events or connectable undirected
		// advertising events. Thus all types EXCEPT Direct Advertising


        // Disable Advertising
		LEadv_Set_Advertise_Enable(0x00);

		// Update the Advertising Data.
		LEadv_Set_Advertising_Data(GAP_Adv_Data_Length,GAP_Adv_Data);


		if (mode == GAP_LIMITED_DISCOVERABLE)
		{
			// The Host should set the minimum advertising interval to TGAP(lim_disc_adv_int_min)
			// The Host should set the maximum advertising interval to TGAP(lim_disc_adv_int_max).

			adv_params.min_interval = TGAP_lim_disc_adv_int_min;
			adv_params.max_interval = TGAP_lim_disc_adv_int_max;

			// Set the Limited Discoverable Timer - TGAP(lim_adv_timeout).
			LEgap_Config.TGAP_Limited_Advertising_Timeout = HW_Get_Native_Clk_Avoid_Race() + ((TGAP_lim_adv_timeout)*2);

		}
		else
		{
			// The Host should set the minimum advertising interval to TGAP(gen_disc_adv_int_min)
			// The Host should set the maximum advertising interval to TGAP(gen_disc_adv_int_max)

			// user CSA-3 Values
			adv_params.min_interval = GAP_Adv_Interval_Min;
			adv_params.max_interval = GAP_Adv_Interval_Max;

		}
write_adv_parameter :
		 gCurrent_ADV_interval = adv_params.min_interval;
		adv_params.adv_type = adv_type; /* ADV_IND = 0x00, ADV_DIRECT_IND = 0x01, ADV_SCAN_IND = 0x02, ADV_NONCONN_IND = 0x03, */
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)&& (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
		if ((adv_type != 0x01) && (GATTserver_Is_Privacy_Flag_Enabled()) && LEsmp_IRK_Distributed())
		{
			u_int8  resolvable_address[6];

			 /* If a device has privacy enabled as defined in Table 10.7, the Host should send
				it�s IRK to the peer device and request the IRK of the peer device during the
				pairing procedure. The Host can abort the pairing procedure if the authentication
				requirements are not sufficient to distribute the IRK. If the pairing procedure
				fails due to authentication requirements and IRK distribution was
				requested, the pairing procedure should be retried without requesting IRK distribution. */

			// Generate a Resolvable address using the IRK.
			LEsmp_Generate_Resolvable_Address(resolvable_address);

			// Set the Limited Discoverable Timer - TGAP(lim_adv_timeout).
			LEgap_Config.TGAP_Private_Address_Timeout = HW_Get_Native_Clk_Avoid_Race() + ((TGAP_private_addr_int)*2);
			LEconfig_Set_Random_Address(resolvable_address);
			adv_params.adv_add_type = RANDOM_ADDRESS;
		}
		else
#endif
		{
			adv_params.adv_add_type = PUBLIC_ADDRESS;
		}

		adv_params.channel_map = 0x07;
		adv_params.filter_policy = 0x00;
		// The Host shall set the advertising filter policy to �process scan and connection
		// requests from all devices.

		LEadv_Set_GAP_Advertising_Params(&adv_params);
		Set_Dual_mode(Dual_mode);
	    // Enable Advertising
		LEadv_Set_Advertise_Enable(0x01);

		// The device shall remain in general discoverable mode until a connection is
		// established or the Host terminates the mode.
		// If a device is in the general discoverable mode and in the directed connectable
		// mode or the non-connectable mode, the advertising interval values should be
		// set as defined in Section 9.3.3.

		// Note: Data that change frequently should be placed in the advertising data and
		// static data should be placed in the scan response data.
	}

	LEgap_Config.Discoverable_Mode = mode;
	return 0;
}

#endif


#if (PRH_BS_CFG_SYS_LE_OBSERVER_DEVICE==1) || (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1)

/**************************************************************************
 * FUNCTION :- GAP_Discovery_Mode
 *
 * DESCRIPTION
 * Sets the Discovery mode of the Device. This controls the scanning behavior in the LC.
 *
 * INPUTS :
 * u_int8 mode     :- The Discovery mode, one of the following.
 * 			GAP_NON_DISCOVERY        - 0x00
 * 			GAP_LIMITED_DISCOVERY    - 0x01
 * 			GAP_GENERAL_DISCOVERY    - 0x02
 *
 *
 *************************************************************************/

u_int8 GAP_Discovery_Mode(u_int8 mode)
{
	u_int16 scan_int;
	u_int16 scan_win;

	LEscan_Set_Scan_Enable(0x00 /* Disabled */, 0x00 /* Filter Duplicates - dont care */);
	GAP_Last_Adv_Packet_Reported = 0;

	if (mode == GAP_LIMITED_DISCOVERY)
	{
		scan_int = TGAP_lim_disc_scan_int;
		scan_win = TGAP_lim_disc_scan_wind;
	}
	else if (mode == GAP_GENERAL_DISCOVERY)
	{
		scan_int = TGAP_gen_disc_scan_int;
		scan_win = TGAP_gen_disc_scan_wind;
	}
	LEscan_Set_GAP_Scan_Parameters(scan_int,scan_win,LE_ACTIVE_SCAN, PUBLIC_ADDRESS,
			                           0x00 /*scan_filter_policy*/);

	if (mode != GAP_NON_DISCOVERY)
	{
		LEscan_Set_Scan_Enable(0x01 /* Enabled */, 0x01 /* Filter Duplicates */);
	}

	LEgap_Config.Discovery_Mode = mode;
	return 0;
}
#endif



#if ((PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1) || (PRH_BS_CFG_SYS_LE_OBSERVER_DEVICE==1))

/**************************************************************************
 * FUNCTION :- GAP_Handle_Advertising_Events
 *
 * DESCRIPTION
 * This function handles advertising events from the LC. The actions to be taken
 * depend on the different modes the device is in.
 *
 * 1/ Discovery Mode
 * If the device is in Limited or General discovery the Flags of the Advertising data are
 * checked to determine if this Event should be reported to the user.
 *
 * 2/ Broadcast Mode
 * If the device is configured as an Observer - then all Advertising Events are reported to
 * the user.
 *
 * 3/ Connect Mode
 * If we are in the FIND_CONNECTABLE_DEVICES state when the user selects GAP_GENERAL_CONNECT, the
 * and the advertising event type is Adv_Ind OR Adv_Direct_Ind. If the address type is RANDOM
 * then and private we resolve it. We present the Address to the user for possible selection.
 *
 * If the Connect Mode is GAP_SELECTIVE_CONNECT we disable scanning and initiate a connection to the
 * device which sent the Advertising event.
 *
 * NOTE :- Resolvable Addresses are only applicable in Observer mode or General Connection Establishment
 *************************************************************************/
// Cases in Which Advertising Events have to be handled
//
//  1/ Limited Discovery
//  2/ General Discovery
//  3/ General Connection Establishment
//  4/ Selective Connection Establishment

u_int8 GAP_Handle_Advertising_Events(t_advert_event* p_Advert_Event)
{
	  u_int8* pdu =  p_Advert_Event->data;
	  u_int8 cur_adv_event_len= p_Advert_Event->lenData;
	  u_int8 flags_found = 0x00;
	  u_int8 type_len;
	  u_int8 type;
	  u_int8 report_pdu = 0x0;

	  if (LEgap_Config.Discovery_Mode != GAP_NON_DISCOVERY)
	  {
		  //report_pdu = 0x01;
		  while ((cur_adv_event_len) && !flags_found)
		  {
			  type_len = pdu[0];
			  type = pdu[1];

			  // Scan the Ad Data for the flags..
			  if (type == 0x01) /* Flags - for Limited Discovery */
			  {
				  if ((pdu[2] & 0x01) ||
					 ((pdu[2] & 0x02) && (LEgap_Config.Discovery_Mode == GAP_GENERAL_DISCOVERY)))
				  {
					  // Report PDU ..
					  flags_found = 0x01;
					  report_pdu = 0x01;
					  break;
				  }
			  }
			  pdu+= type_len+1;
			  cur_adv_event_len -= (type_len+1);

		  }
		  // If this is an Scan_Response then the decision to report the packet is dependent on if the last
		  // Rx ADV packet was reported. As the Adv Packet preceeding the Scan_Rsp will normally be from the
		  // same device.
		  //

		  if (p_Advert_Event->event_type == SCAN_RSP_EVENT_TYPE )
		  {
		  	 if (GAP_Last_Adv_Packet_Reported)
		  	 {
		  	 	report_pdu = 1;
		  	 	GAP_Last_Adv_Packet_Reported = 0;
		  	 }
		  }
		  else
		  {
		  	  GAP_Last_Adv_Packet_Reported = report_pdu;
		  }
		  return report_pdu;
	  }

#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1)
	  if ((LEgap_Config.Broadcast_Mode == 0x02 /* Observer Mode */) ||
	     ((LEgap_Config.Connect_Mode == GAP_FIND_CONNECTABLE_DEVICES) &&
		 (p_Advert_Event->event_type < 0x02 /* Adv_Ind OR  Adv_Direct_Ind */)))
	  {
		  // Check if address is resolvable and if it is resolve it.
		  // Present option to the user to Select address

		  report_pdu = 0x01;
#if(PRH_BS_CFG_SYS_SMP_SUPPORTED == 1)
		  if (p_Advert_Event->addressType == RANDOM_ADDRESS)
		  {
			  u_int8 rand_addr_type;
			  // Could be Non-Resolvable or Resolvable - or static.

			  // Determine the Address Type :
			  //   Static              2 MSBs = 1,1
			  //   Non-Resolvable      2 MSBs = 0,0
			  //   Resolvable          2 MSBs = 0,1

			  rand_addr_type = GAP_Determine_Random_Addr_Type(p_Advert_Event->address[5]);

			  if (rand_addr_type == RESOLVABLE_RANDOM_ADDRESS )
			  {
				  if(!LEsmp_Resolve_Address(SLAVE,p_Advert_Event->address))
				  {
					  report_pdu = 0x00;
				  }
			  }
			  else if (rand_addr_type == NON_RESOLVABLE_RANDOM_ADDRESS)
			  {
				  // Does the Non-Resolvable Address Match what we have
				  // Stored in our GATT D

					u_int8 non_resolvable_address[6];

					if (NO_ERROR == LEsmp_Get_NonResolvable_Address(non_resolvable_address))
					{
						u_int8 i;
						for(i=0;i<6;i++)
						{
							if (non_resolvable_address[i] != p_Advert_Event->address[i])
							{
									return 0;
							}

						}

					}
			  }

#if 1 // Temp stub for trial of Ptr Priv_bv03
			  if  ((LEgap_Config.Connect_Mode == GAP_FIND_CONNECTABLE_DEVICES) && report_pdu &&
					  (p_Advert_Event->event_type == 0x01 /* Adv_Direct_Ind */) && ((rand_addr_type == RESOLVABLE_RANDOM_ADDRESS ) || (rand_addr_type == NON_RESOLVABLE_RANDOM_ADDRESS )))
			  {
				  GAP_Connection_Establishment(GAP_GENERAL_CONNECT,p_Advert_Event->address,p_Advert_Event->addressType,
						  		                            120, 100,0x0, 0x800);

			  }
#endif
		  }
#endif
	  }
	  else if  (LEgap_Config.Connect_Mode == GAP_SELECTIVE_CONNECT)
	  {
		  // If any advertising packet gets through - connect to it.
		  //   Disable Scanning
		  LEscan_Set_Scan_Enable(0x00 /* Disabled */, 0x00);

		  // Begin a directed connection establishment to the Address in the
		  // Advertising packet. ~~~~~~~

		  report_pdu = 0x00;
		  GAP_Connection_Establishment(GAP_DIRECTED_CONNECT,p_Advert_Event->address,p_Advert_Event->addressType,
		  LEgap_Config.max_conn_interval,LEgap_Config.min_conn_interval,LEgap_Config.latency,LEgap_Config.supervision_timeout);
	  }
#endif/* #if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1) */
	  return report_pdu;
}
#endif


#if (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE == 1)

/**************************************************************************
 * FUNCTION :- GAP_Connectable_Mode
 *
 * DESCRIPTION
 * This function controls the connectable mode of the device.
 * It uses Advertising to make the device connectable/non-connectable. The following 3 modes
 * are allowed :
 *
 * 		1/ GAP_NON_CONNECTABLE
 * 		This disables connectable advertising and prevents the device from allowing incoming connections.
 * 		If the device is Discoverable - it should be forced to use ADV_NONCONN_IND to prevent incoming
 * 		connections.
 *
 * 		2/ GAP_DIRECT_CONNECTABLE
 * 		This forces the device to go advertising using ADV_DIRECT_IND. If privacy is enabled and a reconnection
 *      address exists, then this is retrieved from the GATT server and programmed into the LC.
 *
 *		If privacy is not enabled or a reconnection address does not exist, then the address types and device address
 *		passed into this function are used.
 *
 *		3/ GAP_UNDIRECTED_CONNECTABLE
 *		This forces the device to go advertising using ADV_IND. If privacy is enabled than a resolvable address should
 *		be generated and used.
 *
 * INPUTS
 *
 * u_int8 mode  :- GAP_NON_CONNECTABLE - 0, GAP_DIRECT_CONNECTABLE - 1, GAP_UNDIRECTED_CONNECTABLE - 2
 *
 * u_int8 peer_addr_type :- Address type of the peer to use, if privacy is not enabled. ????
 *                          REVIEW - as spec and test specs somewhat unclear on what to do here.
 *
 * u_int8 own_addr_type :-  Own Address Type, Spec and PTS tester conflict in usage of this !!!!
 * 							REVIEW - not sure if this is even needed as a parameter !!!
 *
 * u_int8 p_bdaddr      :-  A pointer to the device address of the peer.
 *
 ***********************************************************************************************/
u_int8 GAP_Connectable_Mode(u_int8 mode, u_int8 peer_addr_type,u_int8 own_addr_type, u_int8* p_bdaddr)
{
	t_adv_params adv_params;

	/*
	 * This controls what advertising packets can be sent from the Peripheral
	 */

	if (mode == GAP_NON_CONNECTABLE)
	{
		GAP_Discoverable_Mode(LEgap_Config.Discoverable_Mode,0x02 /* ADV_NONCONN_IND */,1);
		//GAP_Discoverable_Mode(LEgap_Config.Discoverable_Mode,0x00 /* ADV_IND */);
	}
	else if ((mode == GAP_DIRECT_CONNECTABLE) || (mode == GAP_UNDIRECTED_CONNECTABLE))
	{

		LEadv_Set_Advertise_Enable(0x00);

		// Spec is rather unclear about how to handle Max/Min interval for Direct Advertising
		// The HCI spec says the Max/Min are not relevant for Direct Advertising .. However, the
		// GAP says the Min/Max should be set to 250ms/500ms.
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5!=1)
		//
		// !!!! Should we make this a CSA-3 define ????
		adv_params.min_interval =  TGAP_adv_fast_interval_min;
		adv_params.max_interval =  TGAP_adv_fast_interval_max;
#else
		// PTS 4.5 uses the old interval values (in BT 4.0 spec) which are Pre CSA 3.0
		adv_params.min_interval =  1024;
		adv_params.max_interval =  2048;
#endif

		if (mode == GAP_DIRECT_CONNECTABLE)
		{
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
			u_int8* p_reconnect_address;
#endif
#endif

			adv_params.adv_type = 0x01; /* ADV_DIRECT_IND */
			//Peripheral Host checks for the presence of �Peripheral Privacy Flag characteristic? in the local GATT server
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)&& (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1))
			p_reconnect_address = GATTserver_Find_Reconnection_Address();
			if (GATTserver_Is_Privacy_Flag_Enabled() && (p_reconnect_address!=0x00))
			{

				/* The privacy feature is not used in the GAP discovery mode and procedures
				 * but it is used when supported during connection mode and connection procedures.
				 * In order for devices using the privacy feature to reconnect to known devices,
				 * the device addresses used when the privacy feature is enabled, private address, must
				 *  be resolvable to the other devices?identity.
				 */

#if 0 // Turned off for test GAP_PRIV_07 //(PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5==1) // Conformance -- CONN_DCON_BV03
		  // This seems to conflict with the Spec of Direct Connectable
				LEconfig_Set_Random_Address(p_reconnect_address);
				adv_params.adv_add_type = RANDOM_ADDRESS;
				adv_params.init_add_type = PUBLIC_ADDRESS;
				adv_params.p_init_add = p_bdaddr;

#else
				adv_params.p_init_add = p_reconnect_address;
				LEconfig_Set_Random_Address(p_reconnect_address);
				adv_params.adv_add_type = RANDOM_ADDRESS;
				adv_params.init_add_type = RANDOM_ADDRESS;
#endif
			}
			else
#endif
			{
				adv_params.adv_add_type = own_addr_type;
				adv_params.init_add_type = peer_addr_type;
				adv_params.p_init_add = p_bdaddr;
			}
		}
		else if (mode == GAP_UNDIRECTED_CONNECTABLE)
		{
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)&&(PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
			if ((GATTserver_Is_Privacy_Flag_Enabled()) && LEsmp_IRK_Distributed() && (GATTserver_Find_Reconnection_Address() != 0))/* TP/PRIV/CONN/BV-07,TP/PRIV/CONN/BV-08 GAP layer  */
			{
				u_int8  resolvable_address[6];

			 /* If a device has privacy enabled as defined in Table 10.7, the Host should send
				it�s IRK to the peer device and request the IRK of the peer device during the
				pairing procedure. The Host can abort the pairing procedure if the authentication
				requirements are not sufficient to distribute the IRK. If the pairing procedure
				fails due to authentication requirements and IRK distribution was
				requested, the pairing procedure should be retried without requesting IRK distribution. */

				// Generate a Resolvable address using the IRK.
				LEsmp_Generate_Resolvable_Address(resolvable_address);

				// Set the Limited Discoverable Timer - TGAP(lim_adv_timeout).
				LEgap_Config.TGAP_Private_Address_Timeout = HW_Get_Native_Clk_Avoid_Race() + ((TGAP_private_addr_int)*2);
				LEconfig_Set_Random_Address(resolvable_address);
				adv_params.adv_add_type = RANDOM_ADDRESS;
			}
			else
#endif
			{
				adv_params.adv_add_type = PUBLIC_ADDRESS;
			}
			adv_params.init_add_type = PUBLIC_ADDRESS;
			adv_params.adv_type = 0x00;  /* ADV_IND = 0x00 */
		}

		adv_params.channel_map = 0x07;
		adv_params.filter_policy = 0x00;

		LEadv_Set_GAP_Advertising_Params(&adv_params);

		LEadv_Set_Advertise_Enable(0x01);

	}
	LEgap_Config.Connectable_Mode = mode;

	return 0;
}
#endif


#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1)

/**************************************************************************
 * FUNCTION :- GAP_Connection_Establishment
 *
 * DESCRIPTION
 * This function supports the General Connection Establishment Procedure. 4 different connection
 * procedures are supported :-
 *
 * 1/ GAP_AUTO_CONNECT  :- Auto Connection Establishment
 *    In this case the device connects to the first Advertiser which matchs an entry
 *    in the white list. In this function it is assumed that the Host/User/Application
 *    has already written a list of the Advertisers to connect to into the white list.
 *
 *    The Max/Min Connection interval and the connection latency are taken from the GAP, and
 *    the max_conn_int,min_conn_int and latency parameters are ignored.
 *
 *    If privacy is enabled a non-resolvable (reconnection) address is used, and this is retrieved
 *    from the GATT server - via the SMP.
 *
 * 2/ GAP_GENERAL_CONNECT :- General Connection Establishment
 * 	  In this case the User has been presented with a list of the Advertising devices and
 *    selects one to connect to. The gathering and presentation of the list of devices has
 *    already been performed in GAP_FIND_CONNECTABLE_DEVICES.
 *
 *    The device begins initiating a connection to the device selected by the user, using the
 *    connection parameters passed into the fuction.
 *
 *    If privacy is enabled a non-resolvable (reconnection) address is used, and this is retrieved
 *    from the GATT server - via the SMP.
 *
 * 3/ GAP_DIRECTED_CONNECT :- Directed Connection Establishment
 *    In this case the User requests a connection to be established to a particular device with the
 *    connection parameters given by the user.
 *
 *    If privacy is enabled a non-resolvable (reconnection) address is used, and this is retrieved
 *    from the GATT server - via the SMP.
 *
 *    This connection procedure is very similar to the GAP_GENERAL_CONNECT, except it is not preceeded by
 *    the GAP_FIND_CONNECTABLE_DEVICES.
 *
 *    NOTE : TO DO combine this state with GAP_GENERAL_CONNECT
 *
 * 4/ GAP_SELECTIVE_CONNECT :- Selective Connection Establishment
 *    This allows the host to connect to one of a set of devices which are already in the white list.
 *    Unlike the GAP_AUTO_CONNECT the connection paramters are determined by the host/user. Scanning is
 *    performed initially with the Scan interval and window defined in the GAP.
 *
 *    When the Host discovers one of the peer devices it is connecting to, the Host shall stop scanning,
 *    and initiate a connection using the direct connection establishment procedure with the connection
 *    configuration parameters for that peer device.
 *
 *
 * INPUTS
 * u_int8 mode   :-  GAP_NO_CONNECT      - 0x00
 * 					 GAP_AUTO_CONNECT    - 0x01
 *					 GAP_GENERAL_CONNECT - 0x02
 *					 GAP_DIRECTED_CONNECT- 0x03
 *					 GAP_SELECTIVE_CONNECT 0x04
 *
 * u_int8 p_peer_addr :- pointer to the address of the peer device to connect to.
 *
 * u_int8 peer_address_type :-  RANDOM or PUBLIC Address
 *
 * u_int16 max_conn_int     :-  Maximum connection interval for the connection
 * u_int16 min_conn_int     :-  Minimum connection interval for the connection
 * u_int16 latency          :-  Latency for ther connection
 * u_int16 supervision_to   :-  Supervision Timeout for the connection
 *
 **************************************************************************/


u_int8 GAP_Connection_Establishment(u_int8 mode, u_int8* p_peer_address,u_int8 peer_address_type,
		                            u_int16 max_conn_int, u_int16 min_conn_int,u_int16 latency, u_int16 super_to)
{
	t_connect_params conn_params;

#if (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1)
	u_int8* p_reconnect_address = GATTserver_Find_Reconnection_Address();
#endif

	/* Default Initiating parameter */
	conn_params.scan_interval = TGAP_scan_fast_interval_max;
	conn_params.scan_window = TGAP_scan_fast_window;

	conn_params.conn_interval_min = min_conn_int;
	conn_params.conn_interval_max = max_conn_int;
	conn_params.filter_policy = 0x00; // White List not used;
	if (super_to != 0x00)
		conn_params.supervision_timeout = super_to;

	conn_params.latency = latency;

	if (mode == GAP_NO_CONNECT)
	{
		LEscan_Set_Scan_Enable(0x00,0x00);
		if ((LEgap_Config.Connect_Mode != GAP_FIND_CONNECTABLE_DEVICES ) &&
			(LEgap_Config.Connect_Mode != GAP_NO_CONNECT))
		{
			LEconnection_GAP_Create_Connection_Cancel();
		}
		LEgap_Config.Connect_Mode = GAP_NO_CONNECT;
	}
	else if ((mode == GAP_AUTO_CONNECT) || (mode == GAP_GENERAL_CONNECT) || (mode == GAP_DIRECTED_CONNECT ))
	{
		if (mode == GAP_AUTO_CONNECT)
		{
			/* Override the default settings for the connection interval and latency
			 * also enable the white list filter policy.
			 */
			conn_params.filter_policy = 0x01;
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5!=1)
			// CSA-3 -- These values do not work against the PTS 4.5 tester
			conn_params.conn_interval_min = TGAP_initial_conn_interval_min;
			conn_params.conn_interval_max = TGAP_initial_conn_interval_max;
#else
			conn_params.scan_interval = 2048;
			conn_params.scan_window = 18;

			conn_params.conn_interval_min = 800;
			conn_params.conn_interval_max = 2048;
#endif
			conn_params.latency = TGAP_conn_est_latency;
		}
		else if (mode == GAP_GENERAL_CONNECT)
		{
			// Host has selected a device to connect to :
			// Stop Scanning
			LEscan_Set_Scan_Enable(0x00,0x01);
		}
		else if (mode == GAP_DIRECTED_CONNECT)
		{
			conn_params.p_peer_address = p_peer_address;
		}

		conn_params.connection_own_address_type = PUBLIC_ADDRESS;
		conn_params.peer_address_type = PUBLIC_ADDRESS;


#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)&&(PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1))

		if (GATTserver_Is_Privacy_Flag_Enabled() && (p_reconnect_address!=0x00))
		{
#if 0 // GF 23 Oct 2012 - Bypass for test against PTS 4.5.3
	 // This is the CORRECT BEHAVIOR BUT PTS has a different view of the World !!
	  //

			conn_params.connection_own_address_type = RANDOM_ADDRESS;

			LEconfig_Set_Random_Address(p_reconnect_address);
#endif
		}
#endif
		/* Note :- When the White list is used (as it is in this case) the peer address
		 * and the peer address type are ignored.
		 */
		conn_params.peer_address_type = peer_address_type;
		conn_params.p_peer_address = p_peer_address;

		LEconnection_GAP_Create_Connection(&conn_params);
	}
	else if (mode == GAP_SELECTIVE_CONNECT)
	{
		/* When a Host performs the selective connection establishment procedure, the
		 * Host configures the Controller as follows:
		 *   1. The Host shall write the list of device addresses that are to be selectively
		 *   connected to into the White List.
		 *   2. The Host shall set the scanner filter policy to �process advertising packets
		 *   only from devices in the White List?
		 *   3. The Host should set the scan interval to TGAP(sel_conn_est_scan_int).
		 *   4. The Host should set the scan window to TGAP(sel_conn_est_scan_wind).
		 *   5. The Host shall start active scanning or passive scanning.
		 *   When the Host discovers one of the peer devices it is connecting to, the Host
		 *   shall stop scanning, and initiate a connection using the direct connection
		 *   establishment procedure with the connection configuration parameters for that
		 *   peer device.
		 */

		LEgap_Config.supervision_timeout = super_to;
		LEgap_Config.min_conn_interval = min_conn_int;
		LEgap_Config.max_conn_interval = max_conn_int;
		LEgap_Config.latency = latency;

		LEscan_Set_Scan_Enable(0x00,0x01);
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)&& (PRH_BS_CFG_SYS_LE_GATT_INCLUDED==1))
		if (GATTserver_Is_Privacy_Flag_Enabled() && (p_reconnect_address!=0x00))
		{
			LEconfig_Set_Random_Address(p_reconnect_address);
			LEgap_Config.own_addr_type = RANDOM_ADDRESS;
		}
#endif

			// Assume the devices are already written into the white list.
		LEscan_Set_GAP_Scan_Parameters(TGAP_scan_fast_interval_max,TGAP_scan_fast_window,LEgap_Config.scan_type,LEgap_Config.own_addr_type,0x00);

		LEscan_Set_Scan_Enable(0x01,0x01);

	}

	LEgap_Config.Connect_Mode = mode;

	return NO_ERROR;
}
#endif

/**************************************************************************
 * FUNCTION :- GAP_Check_GAP_Timers
 *
 * DESCRIPTION
 * This function checks if any GAP timers have expired. The following 3 timers are
 * checked.
 *
 * 		1. LIMITED_DISCOVERABLE Timer        - TGAP_Limited_Advertising_Timeout
 * 		2. GAP_UNDIRECTED_CONNECTABLE Timer  - TGAP_Private_Address_Timeout
 *      3. Connection Parameter Update Timer - TGAP_conn_param_Timeout
 *
 **************************************************************************/

void GAP_Check_GAP_Timers(void)
{
	u_int32 current_clk = HW_Get_Native_Clk_Avoid_Race();

#if ((PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1) || (PRH_BS_CFG_SYS_LE_BROADCAST_DEVICE==1))
	if (LEgap_Config.Discoverable_Mode == GAP_LIMITED_DISCOVERABLE)
	{
		if ((LEgap_Config.TGAP_Limited_Advertising_Timeout!= BT_TIMER_OFF) &&
			(((LEgap_Config.TGAP_Limited_Advertising_Timeout - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT))
		{
			// Disable Advertising
			LEadv_Set_Advertise_Enable(0x00);
			LEgap_Config.Discoverable_Mode = 0x00;
		}
	}
#endif
#if (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1)
	if (((LEgap_Config.Connectable_Mode == GAP_UNDIRECTED_CONNECTABLE) || (LEgap_Config.Broadcast_Mode==0x01/* Broadcast Mode */)  )
		&& (LEgap_Config.TGAP_Private_Address_Timeout != BT_TIMER_OFF))
	{
		if (((LEgap_Config.TGAP_Private_Address_Timeout  - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT)
		{
			// Need to handle the Timeout
			if (LEgap_Config.Connectable_Mode == GAP_UNDIRECTED_CONNECTABLE)
				GAP_Connectable_Mode(GAP_UNDIRECTED_CONNECTABLE, 0x00 /* Doesnt matter here */,PUBLIC_ADDRESS, 0x00);
			else
			{
				LEgap_Config.Broadcast_Mode = 0x00;
				GAP_Set_Broadcast_Mode(0x01,LEgap_Config.Broadcast_Type);
			}
		}
	}
#endif
#if (PRH_BS_CFG_SYS_LE_L2CAP_INCLUDED==1)
	if ((LEgap_Config.Connect_Params_Update_Pending) &&(LEgap_Config.TGAP_conn_param_Timeout!=BT_TIMER_OFF))
	{
		if (((LEgap_Config.TGAP_conn_param_Timeout  - current_clk - 1) & BT_CLOCK_MAX_TICKS) >= BT_CLOCK_MAX_TIMEOUT)
		{
			LEgap_Config.Connect_Params_Update_Pending = 0x00;
			LE_L2CAP_Generate_Response_Event(LEgap_Config.pending_handle,0x3B);
			{
				t_LE_Connection* p_connection;

				p_connection = LEconnection_Find_P_Connection(LEgap_Config.pending_handle);
				LEconnection_Disconnect(p_connection,0x3B);
			}
		}
	}
#endif
}

/**************************************************************************
 * FUNCTION :- GAP_Set_Connect_Params_Update_Pending
 *
 * DESCRIPTION
 * This function is used to indicate to the GAP that a L2CAP connection update
 * is pending. An L2CAP_CONNECTION_PARAMETER_UPDATE_REQUEST has been sent from this
 * device (acting as a Slave).
 *
 * A timer is used to inform the User/Application if no response is recieved from the
 * master. If this timer expires prior to a response from the master the link is disconnected
 * The ACL handle is also stored.
 *
 * INPUTS
 * u_int16 handle :- Identifier for the ACL link on which the update is requested
 **************************************************************************/

u_int8 GAP_Set_Connect_Params_Update_Pending(u_int16 handle)
{
	LEgap_Config.Connect_Params_Update_Pending = 0x01;
	LEgap_Config.pending_handle = handle;
	LEgap_Config.TGAP_conn_param_Timeout = HW_Get_Native_Clk_Avoid_Race() + ((TGAP_conn_param_timeout)*2);

	return NO_ERROR;
}

/**************************************************************************
 * FUNCTION :- GAP_Clear_Connect_Params_Update_Pending
 *
 * DESCRIPTION
 * Clears the flag used in the slave to indicate that a Connection Parameter update is
 * pending. The Timeout is also cleared.
 *
 **************************************************************************/

void GAP_Clear_Connect_Params_Update_Pending(void)
{
	LEgap_Config.Connect_Params_Update_Pending = 0x00;
	LEgap_Config.TGAP_conn_param_Timeout = BT_TIMER_OFF;
}

#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE == 1)
/**************************************************************************
 * FUNCTION :- GAP_Find_Connectable_Devices
 *
 * DESCRIPTION
 * This is the first part of the GAP_GENERAL_CONNECT procedure. We initially
 * scan to find all available devices. As they are found the devices are presented
 * to the user for selection (in the Advertising Events).
 *
 * Once a user selects a device we exit the GAP_FIND_CONNECTABLE_DEVICES state and
 * enter the GAP_GENERAL_CONNECT state.
 *
 **************************************************************************/

u_int8 GAP_Find_Connectable_Devices(void)
{
	u_int8 own_addr_type = PUBLIC_ADDRESS;

	// GENERAL CONNECT is broken into two phases.
	//   1/ Scanning to determine List of Connectable Devices
	//   2/ Initiating to a user selected device.

	LEgap_Config.Connect_Mode = GAP_FIND_CONNECTABLE_DEVICES;
    // Set the scanner filter to process all advertising packets
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))

#if 0 // GF 3 Nov 2012 - ReEnabled for GCEP_BV04 - GF 23 Oct 2012 - Not sure if this is relevant !!
	if ((GATTserver_Is_Privacy_Flag_Enabled()) && (LEgap_Config.scan_type == LE_ACTIVE_SCAN))
	{
		u_int8 non_resolvable_address[6];

		if (NO_ERROR == LEsmp_Get_NonResolvable_Address(non_resolvable_address))
		{
			LEconfig_Set_Random_Address(non_resolvable_address);
			own_addr_type = RANDOM_ADDRESS;
		}
	}
#endif
#endif

	LEscan_Set_GAP_Scan_Parameters(TGAP_scan_fast_interval_max,TGAP_scan_fast_window,LEgap_Config.scan_type,own_addr_type,0x00);
	LEscan_Set_Scan_Enable(0x01,0x01);

	return 0;
}
#endif

/**************************************************************************
 * FUNCTION :- GAP_Connection_Complete
 *
 * DESCRIPTION
 * This is the handler for connection complete event from the host controller/ link control.
 * Actions performed are :
 *
 * 1/ If we are a master we search the security DB for the LTK - to check if the device has already
 *    bonded. If it has then we also retrieve the EDIV and RANDOM from the security database and
 *    initiate Encryption in the Host Controller/ Link Controller.
 *
 * 2/ If the connect mode is GAP_GENERAL_CONNECT,GAP_SELECTIVE_CONNECT,GAP_DIRECTED_CONNECT then
 *    the privacy settings of the Local GAP server are checked, if privacy is localy enabled then
 *    a GATT request is sent to the peer to determines it's Privacy settings.
 *
 *    The device sets LEgap_Config.state = GAP_W4_PRIVACY_FLAG_READ and waits for the response
 *    from the peer - which will be handled in GAP_GATT_Event_Handler.
 *
 **************************************************************************/

u_int8 GAP_Connection_Complete(t_LE_Connection* p_connection, u_int8 status)
{
	// Once a connection complete is recieved the action to be performed is
	// dependant on the state of the GAP ...
#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1)
	u_int8 generate_connection_complete=1;
#endif
	u_int16 handle;
	u_int8 mode=0;

	if (p_connection != 0)
	{
		handle = LEconnection_Find_Handle(p_connection->device_index);
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
		if (p_connection->current_security_state & LE_ENCRYPTION_ACTIVE)
			mode = 0x01;
#endif
	}
	else
		handle = 0x00;


#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1)
	if (status == NO_ERROR)
	{
		if (LEconnection_Get_Role(p_connection) == MASTER)
		{
#if (PRH_BS_CFG_SYS_ENCRYPTION_SUPPORTED==1)
			u_int8 ltk[16];
			u_int16 ediv;
			u_int8 random[8];

			if(LEsecdb_Search_DB(LEconnection_Get_Peer_Addr_Type(p_connection),p_connection->peer_address.bytes,
					SLAVE,0x01,&ediv, ltk ))
			{

				LEsecdb_Search_DB(LEconnection_Get_Peer_Addr_Type(p_connection),p_connection->peer_address.bytes,
						SLAVE,0x02,&ediv, random );

				LEsecdb_Search_DB(LEconnection_Get_Peer_Addr_Type(p_connection),p_connection->peer_address.bytes,
						SLAVE,0x03,&ediv, random );

				LEsecurity_GAP_Start_Encryption(p_connection,random,ediv,ltk);

				LEgap_Config.state = GAP_W4_HCI_ENCRYPTION_CHANGE_EVENT;

				generate_connection_complete = 0x00;
			}
#endif
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS ==1)
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
			else if (GATTserver_Is_Privacy_Flag_Enabled())
			{
				_GAP_Init_GAP_Discovery(p_connection);
				generate_connection_complete = 0x00;
			}
#endif
#endif

		}
	}

	if (generate_connection_complete)
	{

#if ((PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS !=1) && (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)&&(PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1))
		if ((GATTserver_Is_Privacy_Flag_Enabled()) && (p_connection->role == MASTER))
		{
			u_int16  reconn_addr_handle = GATTserver_Get_Reconnection_Address_Handle();
			LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
			GATTserver_Characteristic_Write_Local_Value(reconn_addr_handle,0x06, GAP_proposed_reconnection_address);
			GAP_Connection_Complete_Generate_Event(status,handle,0x01,GAP_proposed_reconnection_address,mode,LEconnection_Get_Role(p_connection));
		}
		else
		{
			GAP_Connection_Complete_Generate_Event(status,handle,0x00,dummy_addr,0x00,LEconnection_Get_Role(p_connection));
		}
#else
		GAP_Connection_Complete_Generate_Event(status,handle,0x00,dummy_addr,0x00,LEconnection_Get_Role(p_connection));
#endif
	}
#else
	GAP_Connection_Complete_Generate_Event(status,handle,0x00,dummy_addr,0x00,LEconnection_Get_Role(p_connection));

#endif/*#if (PRH_BS_CFG_SYS_LE_CENTRAL_DEVICE==1) */
    (void)mode;
    
    os_printf("BLE_connected\r\n");
    
	return NO_ERROR;
}

/****************************************************************************************
 * Function :- LEgap_Handle_Encryption_Change_Event
 *
 * Description
 * This function handles a call from the lower layers (in the LE HC) to indicate there has been a change
 * in encryptio. This is equivalent to LE_SUBEVENT_ENCRYPTION_CHANGE_EVENT.
 *
 * t_LE_Connection* p_connection  :- a pointer to the link
 * u_int8 status :- indicates success/failure
 * u_int8 mode     :- ENCRYPTION ACTIVE/INACTIVE
 ****************************************************************************************/

t_error LEgap_Handle_Encryption_Change_Event(t_LE_Connection* p_connection,u_int8 status, u_int8 mode )
{
	u_int16 handle =  LEconnection_Determine_Connection_Handle(p_connection);

	if(LEgap_Config.state == GAP_W4_HCI_ENCRYPTION_CHANGE_EVENT)
	{
		/*
		 * Ensure that the encryption was successful.
		 */
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS ==1)
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
		if ((status == NO_ERROR) &&  (GATTserver_Is_Privacy_Flag_Enabled()))
		{
			_GAP_Init_GAP_Discovery(p_connection);
		}
		else
#endif
#endif
		{

#if ((PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)&&(PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS !=1) && ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1)))
			if (GATTserver_Is_Privacy_Flag_Enabled())
			{
				u_int16  reconn_addr_handle = GATTserver_Get_Reconnection_Address_Handle();

				LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
				GATTserver_Characteristic_Write_Local_Value(reconn_addr_handle,0x06, GAP_proposed_reconnection_address);
				GAP_Connection_Complete_Generate_Event(status,handle,0x01,GAP_proposed_reconnection_address,mode,LEconnection_Get_Role(p_connection));
			}
			else
			{
				GAP_Connection_Complete_Generate_Event(status,handle,0x00,dummy_addr,mode,LEconnection_Get_Role(p_connection));
			}
#else
			GAP_Connection_Complete_Generate_Event(status,handle,0x00,dummy_addr,mode,LEconnection_Get_Role(p_connection));
#endif
		}
	}

	return NO_ERROR;
}

#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS ==1)
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))

void _GAP_Init_GAP_Discovery(t_LE_Connection* p_connection)
{
	if (LEgap_Config.Connect_Mode != GAP_NO_CONNECT)
	{
		u_int8 gap_UUID[2] = {0x00,0x18};
		u_int16 handle = LEconnection_Determine_Connection_Handle(p_connection);

		// First need to get the Handle Range for GAP on the peer device.
		// Then need to determine the Handles for the Privacy Flag and Re-Connection
		// address
		LEgap_Config.state = GAP_W4_PEER_GAP_HANDLE_RANGE;

		GATT_Discover_Primary_Service(handle,0x00,gap_UUID,GAP_PROTOCOL);
	}
}

#endif
#endif
#if (PRH_BS_CFG_SYS_LE_GATT_INCLUDED == 1)
/**************************************************************************
 * FUNCTION :- GAP_GATT_Event_Handler
 *
 * DESCRIPTION
 * This is handler for GATT events. The main two events handled are :
 *
 * 		GATT_READ_BY_TYPE_RESPONSE_EVENT
 * 		GAP_W4_READ_RECONNECTION_ADDRESS
 *
 * If we recieve a READ_BY_TYPE_RESPONSE_EVENT, we retrieve the Attribute Handle from
 * the event PDU and check the current LEgap_Config.state. Depending on the state
 * we store the attribute_handle, and advance the LEgap_Config.state to wait for the
 * corresponding COMMAND_COMPLETE event.
 *
 * If we get a GAP_W4_READ_RECONNECTION_ADDRESS we advance the LEgap_Config.state to wait for
 * COMMAND_COMPLETE event.
 *
 *
 **************************************************************************/


void GAP_GATT_Event_Handler(u_int8 eventCode, t_gatt_event_params* p_gatt_event_params)
{
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS==1)

	if (eventCode == GATT_READ_RESPONSE_EVENT)
	{
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
		LEgap_Config.gatt_handle = HCIparam_Get_Uint16((u_int8*)(p_gatt_event_params->payload));
		if ((LEgap_Config.state == GAP_W4_PRIVACY_FLAG_READ ) && (*(p_gatt_event_params->payload+2) != 0))
		{
			LEgap_Config.state = GAP_W4_PRIVACY_FLAG_READ_COMMAND_COMPLETE;
		}
		else if (LEgap_Config.state == GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE)
		{
			LEgap_Config.state = GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE_COMPLETE;
		}
#endif
	}
	else if (eventCode == GATT_WRITE_COMPLETE_EVENT)
	{
#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
		if (LEgap_Config.state == GAP_W4_WRITE_RECONNECTION_ADDRESS_COMPLETE)
		{
			// Find ReConnectionAddress Handle
			/************************/
			u_int16  reconn_addr_handle = GATTserver_Get_Reconnection_Address_Handle();

			GATTserver_Characteristic_Write_Local_Value(reconn_addr_handle,0x06, GAP_proposed_reconnection_address);
		}
#endif
#endif
	}
	else if (eventCode == GATT_DISCOVER_CHARACTERISTICS_EVENT)
	{
		if (LEgap_Config.state == GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE)
		{
			LEgap_Config.state = GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE_COMPLETE;
			LEgap_Config.gatt_handle = HCIparam_Get_Uint16((u_int8*)(p_gatt_event_params->payload+3));
		}
	}
	else if (eventCode == GATT_DISCOVER_SERVICES_BY_UUID_EVENT )
	{
		LEgap_Config.peer_GAP_start = HCIparam_Get_Uint16((u_int8*)(p_gatt_event_params->payload));
		LEgap_Config.peer_GAP_end = HCIparam_Get_Uint16((u_int8*)(p_gatt_event_params->payload+2));
	}
#endif
}

/**************************************************************************
 * FUNCTION :- GAP_GATT_Command_Complete_Event
 *
 * DESCRIPTION
 * This is handler for GATT Command Complete Event. It is used in the GAP
 * to trigger the Generation of a non-resolvable address and the Reading/Writing
 * of a ReConnection Address.
 *
 * If we receive a Command Complete for the Privacy Flag read
 **************************************************************************/

void GAP_GATT_Command_Complete_Event(u_int8 operation_id,t_gatt_command_complete_params* p_params)
{
#if (PRH_BS_CFG_SYS_LE_GAP_AUTO_WRITE_RECONNECTION_ADDRESS==1)

	u_int8 status = p_params->status;

	if ((status == NO_ERROR) || (status == 0x0A /* Attrib not found */))
	{
#if ((PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1) && (PRH_BS_CFG_SYS_LE_GAP_RECONNECTION_ADDRESS_SUPPORTED==1))
		u_int8 uuid[2] = {0x03,0x2A};
		u_int16 handle = p_params->handle;

		switch(LEgap_Config.state)
		{
#if (PRH_BS_CFG_SYS_LE_GATT_CLIENT_SUPPORTED==1)
		case GAP_W4_PEER_GAP_HANDLE_RANGE :
			// Now Search for the Handle of the Privacy Flags
			// Initite Check on Peers Privacy Flag.
			LEgap_Config.state = GAP_W4_PRIVACY_FLAG_READ;

			{
				u_int8 privacy_flags_uuid[2] = {0x02,0x2A}; // UUID for Peripheral Privacy Flag;
				GATT_Characteristic_Value_Read(handle,0x00,0x00,privacy_flags_uuid,LEgap_Config.peer_GAP_start,LEgap_Config.peer_GAP_end,GAP_PROTOCOL);
			}
#endif
			break;

		case GAP_W4_PRIVACY_FLAG_READ :

			/*
			 * The Reconnection Address characteristic value shall not be readable.
			 * The Reconnection Address characteristic value shall be writable and shall require
			 * authentication. Authorization may be defined by a higher layer specification or is
			 * implementation specific.
		 */
#if 1 // BT 4.0 Spec - has Reconnection Address as Read/Write
			LEgap_Config.state = GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE;
			LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
			GATT_Characteristic_Discovery(handle,0x00,uuid,LEgap_Config.peer_GAP_start,LEgap_Config.peer_GAP_end,GAP_PROTOCOL);

#else // in CSA-3 the Reconnection Address is Write only
			LEgap_Config.state = GAP_W4_WRITE_RECONNECTION_ADDRESS_COMPLETE;
			LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
			GATT_Characteristic_Value_Write(handle,0x01 /* Ack Needed */, 0x00 /* offset */, LEgap_Config.gatt_handle,
					                                0x06 /* Len */, GAP_proposed_reconnection_address, GAP_PROTOCOL );
#endif
		break;

#if 1 // BT 4.0 Spec - has Reconnection Address as Read/Write - Redundant in CSA-3
		case GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE_COMPLETE :
			// Generate ReConnection Address and Write to the Peripheral
			LEgap_Config.state = GAP_W4_WRITE_RECONNECTION_ADDRESS_COMPLETE;
			LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
			GATT_Characteristic_Value_Write(handle,0x01 /* Ack Needed */, 0x00 /* offset */, LEgap_Config.gatt_handle,
					                                0x06 /* Len */, GAP_proposed_reconnection_address, GAP_PROTOCOL );
		break;
#endif
		case GAP_W4_WRITE_RECONNECTION_ADDRESS_COMPLETE :
			{
				u_int8 mode = 0x00;
				u_int16  reconn_addr_handle = GATTserver_Get_Reconnection_Address_Handle();
				t_LE_Connection* p_connection;

				p_connection = LEconnection_Find_P_Connection(handle);
				if (p_connection->current_security_state & LE_ENCRYPTION_ACTIVE)
					mode = 0x01;

				LEsmp_Generate_Non_Resolvable_Address(GAP_proposed_reconnection_address);
				GATTserver_Characteristic_Write_Local_Value(reconn_addr_handle,0x06, GAP_proposed_reconnection_address);

				GAP_Connection_Complete_Generate_Event(NO_ERROR,handle,0x00,GAP_proposed_reconnection_address,mode,LEconnection_Get_Role(p_connection));

			}
			break;

		}
#endif
	}
#endif
}
#endif
/**************************************************************************
 * FUNCTION :- GAP_Determine_Random_Addr_Type
 *
 * DESCRIPTION
 * This determine what type a RANDOM address is, based on the Most Significant Byte of the
 * address. It is assumed that it is already know that the address is RANDOM.
 *
 ***************************************************************************/

u_int8 GAP_Determine_Random_Addr_Type(u_int8 MSB_Addr)
{
	if ((MSB_Addr & 0xC0) == 0xC0)
	{
		return STATIC_RANDOM_ADDRESS;
	}
	else if ((MSB_Addr & 0x40) == 0x40)
	{
		return RESOLVABLE_RANDOM_ADDRESS;
	}
	else
	{
		// The two most significant bits of the address shall be equal to ?
		return NON_RESOLVABLE_RANDOM_ADDRESS;
	}
}

/**************************************************************************
 * FUNCTION :- GAP_Set_Broadcast_Mode
 *
 * DESCRIPTION
 * This simple function controls the Broadcast/Observer modes of a device. There are
 * three valid modes :
 *
 * 	NO_BROADCAST           -- Default
 *  BROADCAST_MODE
 *  OBSERVER_MODE
 *
 * There are 4 Values passed into this funtion.
 *		00 -- Turn off Broadcast.
 *		01 -- Turn on Broadcast
 *		02 -- Turn on Observer
 *		03 -- Turn off Observer
 *
 * Turning off - Broadcast mode - involves the disabling of Advertising and Clearing
 * of advertising data.
 * Turning on - Broadcast mode - involves the enabling of Broadcast mode,
 **************************************************************************/

u_int8 GAP_Set_Broadcast_Mode(u_int8 mode, u_int8 type)
{
	u_int8 status = NO_ERROR;

	switch(mode)
	{
	case 0x00 :
		// Turn off Broadcast Mode
		// Disable Advertising
		if (LEgap_Config.Broadcast_Mode == 0x01)
		{
#if ((PRH_BS_CFG_SYS_LE_BROADCAST_DEVICE==1) || (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1))
			//
			LEadv_Set_Advertise_Enable(0x00);
			// Clear any pending Adv Data
			LEadv_Set_Advertising_Data(0x00,0x00);

			LEgap_Config.Broadcast_Mode = 0x00;
#endif
		}
		else
		{
			status = COMMAND_DISALLOWED;
		}
		break;

	case 0x01 :
		// Turn On Broadcast Mode
		// Set the Adv_Type to type parameter.
		// If broadcast Data Exists - write it.
#if ((PRH_BS_CFG_SYS_LE_BROADCAST_DEVICE==1) || (PRH_BS_CFG_SYS_LE_PERIPHERAL_DEVICE==1))
		if (LEgap_Config.Broadcast_Mode == 0x00)
		{
			t_adv_params adv_params;

			LEadv_Set_Advertise_Enable(0x00);

			adv_params.min_interval = TGAP_adv_fast_interval_min;
			adv_params.max_interval = TGAP_adv_fast_interval_max;
			LEgap_Config.Broadcast_Type = adv_params.adv_type = type; /* ADV_SCAN_IND = 0x02, ADV_NONCONN_IND = 0x03, */
			adv_params.adv_add_type = PUBLIC_ADDRESS;
			adv_params.channel_map = 0x07;
			adv_params.filter_policy = 0x00;
#if (PRH_BS_CFG_SYS_LE_GAP_PRIVACY_SUPPORTED==1)
#if (PRH_BS_CFG_SYS_LE_SMP_INCLUDED==1)
			if (GATTserver_Is_Privacy_Flag_Enabled() && LEsmp_IRK_Distributed())
			{
				u_int8 resolvable_address[6];

				adv_params.adv_add_type = RANDOM_ADDRESS;
				LEsmp_Generate_Resolvable_Address(resolvable_address);

				// Set the Limited Discoverable Timer - TGAP(lim_adv_timeout).
				LEgap_Config.TGAP_Private_Address_Timeout = HW_Get_Native_Clk_Avoid_Race() + ((TGAP_private_addr_int)*2);
				LEconfig_Set_Random_Address(resolvable_address);
			}
#endif
#endif
			LEadv_Set_GAP_Advertising_Params(&adv_params);
			LEadv_Set_Advertise_Enable(0x01);
			LEgap_Config.Broadcast_Mode = 0x01;
		}
		else
#endif
		{
			status = COMMAND_DISALLOWED;
		}
		break;

#if (PRH_BS_CFG_SYS_LE_OBSERVER_DEVICE==1)
	case 0x02 :
		// Turn on Observer Mode
		// Set the Scan Type ( Active / Passive ) to the type parameter
		if (LEgap_Config.Broadcast_Mode == 0x00)
		{
			LEscan_Set_Scan_Enable(0x00 /* Disabled */, 0x00 /* Filter Duplicates - dont care */);
			// Configure the Scanning Parameters
			LEscan_Set_GAP_Scan_Parameters(TGAP_scan_fast_interval_max,TGAP_scan_fast_window,type, PUBLIC_ADDRESS,
					                           0x00 /*scan_filter_policy*/);

			LEscan_Set_Scan_Enable(0x01 /* Enabled */, 0x01 /* Filter Duplicates */);
			LEgap_Config.Broadcast_Mode = 0x02;
		}
		else
		{
			status = COMMAND_DISALLOWED;
		}
		break;
#endif
	case 0x03 :
		// Turn off Observer Mode
		if (LEgap_Config.Broadcast_Mode == 0x02)
		{

			LEgap_Config.Broadcast_Mode = 0x00;
		}
		else
		{
			status = COMMAND_DISALLOWED;
		}
		break;
	}
	return status;
}

#if (PRH_BS_CFG_SYS_LE_BROADCAST_DEVICE==1)

/**************************************************************************
 * FUNCTION :- GAP_Set_Broadcast_Data
 *
 * DESCRIPTION
 * This function writes the (Advertising) data which is transmitted when acting as a Broadcaster.
 * The is only applicable when the device has already been configured as a Broadcaster,
 * otherwise the commands is ignored.
 *
 * NOTE :- Turning off boradcast mode will result in the data being cleared.
 *
 * INPUTS :
 * u_int8  length :- The length of data field being passed to this function.
 * u_int8* p_data :- The Broadcast data to be used when Advertising.
 *
 *
 **************************************************************************/

u_int8 GAP_Set_Broadcast_Data(u_int8 length,u_int8* p_data)
{
	if (LEgap_Config.Broadcast_Mode == 0x01)
	{
		LEadv_Set_Advertising_Data(length,p_data);
	}
	else
	{
		return COMMAND_DISALLOWED;
	}
	return NO_ERROR;
}
#endif


void GAP_Disconnect(u_int16 handle)
{
	t_LE_Connection* p_connection;
	extern  t_LE_Config LE_config;


		if(handle != 0xffff)
		{
		  p_connection = LEconnection_Find_P_Connection(handle);
		}
		else
		{
			p_connection = LEconnection_Find_Link_Entry(LE_config.active_link);
		}
	if(p_connection->terminate_timer == BT_TIMER_OFF)
	{
		LEconnection_Disconnect(p_connection,0x16 /* Connection Terminated by Local Host */);
	}
}
/*************************************************************************************
 * Function :- GAP_Connection_Complete_Generate_Event
 *
 * This function generates an GAP Event. The following events are supported :
 *  	GAP_CONNECTION_COMPLETE
 *
 * INPUTs
 * u_int8  event    :- The id of the event to be generated.
 * u_int8  status   :- Indicates the success/failure of the operation.
 * u_int16 handle   :- The handle for the ACL link.
 * u_int8* address  :- Passkey required for SMP_USER_PASSKEY_NOTIFICATION_EVENT
 *
 *************************************************************************************/
t_error GAP_Connection_Complete_Generate_Event(u_int8 status,u_int16 handle,u_int8 valid_reconn_addr, u_int8* p_address,u_int8 encrypt,u_int8 role)
{
#if (PRH_BS_CFG_SYS_LE_GAP_DELIVER_EVENTS_VIA_HCI==1) // GF 29 Aug
	t_length metaEventParaLen=0;
	t_length queue_length;
    t_p_pdu p_buffer =0x0;
    struct q_desc* qd;

    metaEventParaLen = 13; /* 1 + 1 + 2 + 1 + 6 + 1 + 1; */

	queue_length = metaEventParaLen + 2;

	qd = BTq_Enqueue(HCI_EVENT_Q, 0, queue_length);
	if(qd)
	{
		p_buffer=qd->data;
	}
	else
	{
		return MEMORY_FULL;
	}

	if (p_buffer == 0)
	{
		return UNSPECIFIED_ERROR;
	}

	*(p_buffer)= LE_META_EVENT;
	*(p_buffer+1)= metaEventParaLen;
	*(p_buffer+2)= GAP_CONNECTION_COMPLETE_EVENT; // SubEventCode
	*(p_buffer+3)= status;
	LMutils_Set_Uint16((p_buffer+4), handle);
	*(p_buffer+6)=role;
	for(i=0;i<6;i++)
		p_buffer[7+i] = p_address[i];
	*(p_buffer+13) = encrypt;
	*(p_buffer+14) = valid_reconn_addr;

	BTq_Commit(HCI_EVENT_Q,0);

#endif

#if (PRH_BS_CFG_SYS_EMBEDDED_PROFILES==1)
	{
	t_gap_event_params gap_event_params;

	gap_event_params.event = GAP_CONNECTION_COMPLETE_EVENT;
	gap_event_params.handle = handle;
	gap_event_params.status = status;
	gap_event_params.p_address = p_address;
	gap_event_params.encrypt = encrypt;
	gap_event_params.role = role;
	gap_event_params.valid_reconn_addr = valid_reconn_addr;

	if (GAP_Event_Callback_Array[0])
	{
	   
		((void(*)(u_int8,t_gap_event_params*))(GAP_Event_Callback_Array[0]))(GAP_CONNECTION_COMPLETE_EVENT,&gap_event_params);
	}
	}
#endif

	return NO_ERROR;
}

t_error GAP_Set_Privacy(u_int8 privacy)
{
	extern u_int8 GATTprivacy_Flags[];

	if (privacy < 0x02)
		GATTprivacy_Flags[0] = privacy;
	else
		return INVALID_HCI_PARAMETERS;

	return NO_ERROR;
}

#endif
