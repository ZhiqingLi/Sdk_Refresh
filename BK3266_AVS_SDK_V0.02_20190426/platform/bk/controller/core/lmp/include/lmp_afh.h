#ifndef _PARTHUS_LMP_AFH_
#define _PARTHUS_LMP_AFH_

/********************************************************************
 * MODULE NAME:    lmp_afh.h
 * PROJECT CODE:   BlueStream
 * AUTHOR:         John Nelson
 * DATE:           01-08-2003
 *
 * LICENSE:
 *     This source code is copyright (c) 2002-2004 Ceva Inc.
 *     All rights reserved.
 *
 * SOURCE CONTROL: $Id: lmp_afh.h,v 1.17 2012/04/26 18:48:01 tomk Exp $
 * 
 * DESCRIPTION:     
 * This module is responsible for supporting AFH capability and
 * AFH classification reporting.
 *
 ********************************************************************/
#include "lmp_acl_connection.h"
#include "lmp_link_policy.h"

#define LMPafh_AFH_UPDATE_DEFAULT_SLOTS (1600*4)    /*1 seconds*/

/*
 * Some control bits (required for testing)
 */
#define LMPafh_CTRL_IDLE                        0x00

#define LMPafh_CTRL_PICONET_UPDATE              0x01
#define LMPafh_CTRL_SLAVE_UPDATE                0x02
#define LMPafh_CTRL_FORCE_AFH_DISABLE           0x04
#define LMPafh_CTRL_FORCE_RECOVERY_TESTMODE     0x08
#define LMPafh_CTRL_DEVICE_UPDATE               0x10

#define LMPafh_AFH_REPORTING_DISABLED           0x00
#define LMPafh_AFH_REPORTING_ENABLED            0x01

/*
 * The parameter AFH_min_interval, defines the minimum amount of time from the
 * last LMP_channel_classification command that was sent before the next
 * LMP_channel_classification PDU may be sent. The parameter
 * AFH_max_interval, defines the maximum amount of time between the change
 * in the radio environment being detected by a slave and its generation of an
 * LMP_channel_classification PDU. The AFH_max_interval shall be equal to or
 * larger than AFH_min_interval.
 */
#define LMPafh_AFH_MIN_INTERVAL_SLOTS           (1600 * 1/*Seconds*/)                
#define LMPafh_AFH_MAX_INTERVAL_SLOTS           (1600 * 5/*Seconds*/)


/*
 * Define the valid AFH states for update
 */
#if (PRH_BS_DEV_EARLY_SET_AFH_ON_CONNECTION_ESTABLISHMENT==1)

#if (PRH_BS_DEV_ACL_DRIVEN_AUTOPARK_SUPPORTED==1)
#define LMP_AFH_UPDATE_STATES (W4_LMP_ACTIVE|LMP_ACTIVE|LMP_SNIFF_MODE|LMP_PARK_MODE)
#else 
#define LMP_AFH_UPDATE_STATES (W4_LMP_ACTIVE|LMP_ACTIVE|LMP_SNIFF_MODE)
#endif

#else

#if (PRH_BS_DEV_ACL_DRIVEN_AUTOPARK_SUPPORTED==1)
#define LMP_AFH_UPDATE_STATES (LMP_ACTIVE|LMP_SNIFF_MODE|LMP_PARK_MODE)
#else
#define LMP_AFH_UPDATE_STATES (LMP_ACTIVE|LMP_SNIFF_MODE)
#endif

#endif


#define mLMafh_Is_AFH_Update_State(state)  (!(state & ~(LMP_AFH_UPDATE_STATES)))
#define mLMafh_Is_Assessment_State(state)  (!(state & ~(LMP_ACTIVE| \
             W4_LMP_ACTIVE|LMP_HOST_ACCEPT|LMP_SECURITY_STATE)))
 
/*
 * AFH Capable Function Prototypes
 */ 
#if PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED

void LMafh_Initialise(void);
t_error LMafh_LMP_Set_AFH(t_lmp_link *p_link, t_p_pdu p_payload);

RAM_CODE void LMafh_LMP_Set_AFH_Ack(t_deviceIndex device_index);
void LMafh_Set_AFH_Control_Byte(u_int8 afh_channel_control);

RAM_CODE void LMafh_Activate_AFH(t_lmp_link *p_link);
void LMafh_Deactivate_AFH(t_lmp_link *p_link);

void LMafh_LMP_Channel_Classification_Ack(t_deviceIndex device_index);

#elif !PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED
/*
 * Empty functions
 */
#define LMafh_Initialise() 
#define LMafh_Set_AFH_Control_Byte(afh_channel_control)
#define LMafh_Activate_AFH(p_link);
#define LMafh_Deactivate_AFH(p_link);
 
#endif


/*
 * AFH Classification Function Prototypes
 */ 
#if PRH_BS_CFG_SYS_LMP_AFH_CLASSIFICATION_SUPPORTED
void LMafh_Initialise_Classification(void);
void LMPafh_Request_Classification_Reporting_All_Slaves(u_int8 mode);
void LMPafh_Request_Classification_Reporting(t_lmp_link *p_link, u_int8 mode);
void LMafh_LMP_Channel_Classification_Req_Ack(t_deviceIndex device_index);
boolean LMafh_Is_LMP_Channel_Classification_Req_Pending(u_int8 mode);


void LMafh_Start_Classification_Reporting(t_lmp_link *p_link);
void LMafh_Stop_Classification_Reporting(t_lmp_link *p_link);
boolean LMafh_Is_Classification_Reporting(t_lmp_link *p_link);

t_error LMafh_LM_Set_AFH_Channel_Classification(
    u_int8 AFH_Host_Channel_Classification[/*10*/]);
t_error LMafh_LM_Write_AFH_Channel_Assessment_Mode(
    u_int8 AFH_Channel_Assessment_Mode);
u_int8 LMafh_LM_Read_AFH_Channel_Assessment_Mode(void);

t_error LMafh_LMP_Channel_Classification_Req(t_lmp_link *p_link, t_p_pdu p_payload);
t_error LMafh_LMP_Channel_Classification(t_lmp_link *p_link, t_p_pdu p_payload);

void LMafh_Set_Request_Channel_Classification(u_int8 piconet_index);
void LMafh_Set_Device_Channel_Map_Update_Notification(void);
void LMafh_Device_Channel_Map_Update_Notification(void);

#elif !PRH_BS_CFG_SYS_LMP_AFH_CLASSIFICATION_SUPPORTED
#define LMafh_Initialise_Classification()
#define LMPafh_Request_Classification_Reporting_All_Slaves(mode)
#define LMPafh_Request_Classification_Reporting(p_link,mode)
#define LMafh_LMP_Channel_Classification_Req_Ack(device_index)
#define LMafh_Is_LMP_Channel_Classification_Req_Pending(mode) 0

#define LMafh_Start_Classification_Reporting(p_link)
#define LMafh_Stop_Classification_Reporting(p_link)

#define LMafh_LM_Set_AFH_Channel_Classification( \
    AFH_Host_Channel_Classification)   0 /*Not Supported*/ 
#define LMafh_LM_Write_AFH_Channel_Assessment_Mode( \
    AFH_Channel_Assessment_Mode) EC_CHANNEL_CLASSIFICATION_NOT_SUPPORTED
#define LMafh_LM_Read_AFH_Channel_Assessment_Mode()  0
#endif


#endif
