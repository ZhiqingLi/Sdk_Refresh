#ifndef _PARTHUS_HC_FLOW_CONTROL
#define _PARTHUS_HC_FLOW_CONTROL

/**************************************************************************
 * MODULE NAME:    hc_flow_control.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Host Controller to Host Flow Control
 * AUTHOR:         John Nelson
 * DATE:           03-01-2002
 *
 * LICENSE:
 *     This source code is copyright (c) 2002-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    02-02-2002 -   JN Initial Version
 *
 * SOURCE CONTROL: $Id $:
 * 
 *************************************************************************/

#include "tra_queue.h"

/*
 * Constants
 */
 #if (SEAN_MODIFY_REF_BQB == 1)
#define HCfc_COMPLETED_DATA_PACKETS_TIMEOUT_DEFAULT_SLOTS 400 /*0.25 seconds*/
#define HCfc_COMPLETED_ACL_DATA_PACKETS_REPORT_THRESHOLD 1
// GF 11 Aug 2015
#else
#define HCfc_COMPLETED_DATA_PACKETS_TIMEOUT_DEFAULT_SLOTS 100 /*0.25 seconds*/
#define HCfc_COMPLETED_ACL_DATA_PACKETS_REPORT_THRESHOLD 3
#endif

#define HCfc_COMPLETED_SYN_DATA_PACKETS_REPORT_THRESHOLD 1

/*
 * Flow control enable values for HC to Host Flow
 */
#define HC_TO_HOST_FLOW_CONTROL_OFF     0x00
#define HC_TO_HOST_ACL_FLOW_CONTROL     0x01
#define HC_TO_HOST_SYN_FLOW_CONTROL     0x02
#define HC_TO_HOST_BOTH_FLOW_CONTROL    0x03

/*
 * Flow control persistent variables structure
 */
typedef struct s_hc_flow_crtl
{
    /*
     * Host Controller to Host flow control parameters
     */
    u_int16  host_ACL_Data_Packet_Length;
    u_int8   host_SYN_Data_Packet_Length;
    u_int16  host_Total_Num_ACL_Data_Packets;
    u_int16  host_Total_Num_SYN_Data_Packets;

    u_int16  host_available_number_ACL_data_packets;
    u_int16  host_available_number_SYN_data_packets;
    u_int8   hc2host_flow_control_enable;

    /*
     * Host to Host Controller flow control parameters
     */
    u_int8   host2hc_SYN_flow_control_Enable;
    t_timer  hc_completed_data_packets_report_timer;
    t_slots  hc_completed_data_packets_report_timeout;
    u_int8   hc_completed_ACL_data_packets_report_threshold;
    u_int8   hc_completed_SYN_data_packets_report_threshold;

}  t_hc_flow_ctrl;

/*
 * General operations
 */
void    HCfc_Initialise(void);
u_int8  HCfc_Read_SYN_Flow_Control_Enable(void);
t_error HCfc_Write_SYN_Flow_Control_Enable(u_int8 sco_flow_control_enable);
t_error HCfc_Host_Buffer_Size(t_p_pdu p_hci_payload);
void    HCfc_Handle_Host_To_Host_Controller_Flow_Control(void);

RAM_CODE t_q_descr *HCfc_Get_Next_HC_to_Host_Data_Descriptor(t_deviceIndex device_index);
void    HCfc_Release_All_Data_Queue_Packets(t_deviceIndex device_index);

/*
 * SCO over the HCI operations
 */
#if (PRH_BS_CFG_SYS_SYN_VIA_HCI_SUPPORTED==1)
t_q_descr *HCfc_Get_Next_HC_to_Host_SYN_Descriptor(u_int8 sco_index);
void    HCfc_Release_All_SYN_Queue_Packets(t_deviceIndex sco_index);
#else
#define HCfc_Release_All_SYN_Queue_Packets(sco_index)
#endif

/*
 * HC to Host Operations
 */
#if (PRH_BS_CFG_SYS_HC_TO_HOST_FLOW_CONTROL_SUPPORTED==1)
t_error HCfc_Set_Host_Controller_To_Host_Flow_Control(u_int8 flow_control_enable);
u_int8  HCfc_Read_Host_Controller_To_Host_Flow_Control(void);
t_error    HCfc_Host_Number_Of_Completed_Packets(t_p_pdu p_hci_payload );
#else
/*
 * Not supported, use sensible defaults
 */
#define HCfc_Set_Host_Controller_To_Host_Flow_Control(flow_control_enable) UNSUPPORTED_FEATURE
#define HCfc_Host_Number_Of_Completed_Packets(p_hci_payload)               NO_ERROR
#endif

/*****************************************************************************
 * HCfc_Set_Host_To_Host_Controller_Flow_Control_Thresholds
 *
 * Sets the reporting thresholds for Host to Host Controller.
 *
 * This is an extension to the specification that allows the reporting on
 * Number Of Completed Packets event to be controlled.  The thresholds for
 * ACL and SCO packets can be set and an associated periodic timeout defined
 * where events will be sent periodically if the threshold is not reached.
 *
 * num_ACL_packets_threshold    Report after this number of ACL data packets 
 * num_SCO_packets_threshold    Report after this number of SCO data packets 
 * timeout_threshold_slots      Report after timeout value if non 0
 *****************************************************************************/
t_error HCfc_Set_Host_To_Host_Controller_Flow_Control_Thresholds(
    u_int8 num_ACL_packets_threshold, u_int8 num_SYN_packets_threshold, 
    u_int16 timeout_threshold_slots);

#endif /*_PARTHUS_HC_FLOW_CONTROL*/

