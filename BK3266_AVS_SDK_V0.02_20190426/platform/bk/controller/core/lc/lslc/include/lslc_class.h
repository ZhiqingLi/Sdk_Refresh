#ifndef _PARTHUS_LSLC_CLASS_
#define _PARTHUS_LSLC_CLASS_

/******************************************************************************
 * MODULE NAME:    lslc_class.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    LSLC Adaptive Frequency Classification
 * MAINTAINER:     John Nelson
 * DATE:           10 Feb 2003
 *
 * SOURCE CONTROL: $Id: lslc_class.h,v 1.13 2014/07/02 11:29:14 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2003-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    V1.0     10 Feb 2003 -   jn       - Initial Version based on V1.2 Draft2
 *
 * NOTES TO USERS:
 ******************************************************************************/

/*
 * Interface prototyes: Channel classification Master <-> Slave
 */
#if PRH_BS_CFG_SYS_LMP_AFH_CLASSIFICATION_SUPPORTED

void LSLCcls_Initialise(void);

void LSLCcls_Initialise_Slave_Channel_Classification(t_am_addr am_addr);

void LSLCcls_Reset_Channel_Monitors(void);

void LSLCcls_Record_Slave_Channel_Classification(
    t_am_addr am_addr, u_int8 afh_channel_classification[]);

u_int8 *LSLCcls_Report_Slave_Channel_Classification(t_am_addr am_addr);


void LSLCcls_Record_Device_Channel_Map(const u_int8 afh_device_channel_map[]);

boolean LSLCcls_Build_Channel_Classification(u_int8 afh_channel_classification[], 
    const u_int8 afh_host_channel_classification[]);

boolean LSLCcls_Build_Channel_Map(u_int8 afh_channel_map[], 
    const u_int8 afh_host_channel_classification[]);

#elif !PRH_BS_CFG_SYS_LMP_AFH_CLASSIFICATION_SUPPORTED
/*
 * Empty function or function translation
 */
#define LSLCcls_Initialise()
#define LSLCcls_Initialise_Slave_Channel_Classification(am_addr)
#define LSLCcls_Build_Channel_Map(afh_channel_map, afh_host_channel_classification)\
    hw_memcpy8(afh_channel_map, afh_host_channel_classification, 10)

#endif


/*
 * Interface prototyes: Device Channel Assessment
 */
#if (PRH_BS_CFG_SYS_CHANNEL_ASSESSMENT_SCHEME_SUPPORTED==1)
void LSLCass_Initialise_Device_Channel_Assessment(void);
void LSLCass_Record_Rx_CRC_Event_Channel(t_devicelink* p_dev_link);
void LSLCass_Record_Rx_HEC_Event_Channel(t_devicelink* p_dev_link);
void LSLCass_Record_No_Rx_Packet(t_devicelink* p_dev_link);
void LSLCass_Record_Rx_RSSI_Channel(t_devicelink* p_dev_link, s_int8 rssi);
void LSLCass_Handle_Assessments(void);
void LSLCass_Update_Background_RSSI_Average(s_int8 update_bg_rssi);
boolean LSLCass_Determine_Interference_Presence(s_int8 bg_rssi);

#elif !(PRH_BS_CFG_SYS_CHANNEL_ASSESSMENT_SCHEME_SUPPORTED==1)
/*
 * Empty function or function translation
 */
#define LSLCass_Initialise_Device_Channel_Assessment()
#define LSLCass_Record_Rx_CRC_Event_Channel(p_dev_link)
#define LSLCass_Record_Rx_HEC_Event_Channel(p_dev_link)
#define LSLCass_Record_No_Rx_Packet(p_dev_link)
#define LSLCass_Record_Rx_RSSI_Channel(p_dev_link, rssi)
#define LSLCass_Handle_Assessments(void)
#define LSLCass_Update_Background_RSSI_Average(update_bg_rssi)
#define LSLCass_Determine_Interference_Presence(bg_rssi) (FALSE)
#endif

#endif
