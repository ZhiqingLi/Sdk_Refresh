#ifndef _PARTHUS_LSLC_AFH_
#define _PARTHUS_LSLC_AFH_

/******************************************************************************
 * MODULE NAME:    lslc_afh.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    LSLC Adaptive Frequency Hopping
 * MAINTAINER:     John Nelson
 * DATE:           10 Feb 2003
 *
 * SOURCE CONTROL: $Id: lslc_afh.h,v 1.7 2004/11/19 14:37:45 namarad Exp $
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
#include "lc_types.h"
#include "lslc_hop.h"

/*
 * AFH_mode values: 0 Disabled, 1 Enabled
 */
#define AFH_MODE_DISABLED               0
#define AFH_MODE_ENABLED                1

/* 
 * Extended AFH mode values
 */
#define AFH_MODE_EX_79                  2
#define AFH_MODE_EX_ENABLED_ON_ACK      (AFH_MODE_ENABLED+AFH_MODE_EX_79)
#define AFH_MODE_EX_INACTIVE            8

/*
 * AFH channel classificaton 
 */
#define AFH_CH_CLASS_UNKNOWN            0
#define AFH_CH_CLASS_GOOD               1
#define AFH_CH_CLASS_RESERVED           2
#define AFH_CH_CLASS_BAD                3

/*
 * AFH channel map 
 */
#define AFH_CHANNEL_UNUSED              0
#define AFH_CHANNEL_USED                1

/*
 * AFH parameters
 */
#define AFH_N_MIN_USED_CHANNELS         20
#define AFH_N_MAX_USED_CHANNELS         79
#define AFH_BYTES_PER_CHANNEL_MAP       10
#define AFH_BYTES_PER_CLASSIFICATION    10


typedef struct s_afh_kernel
{
    /*
     * Maintain information for the hop selection kernel
     */
    t_hs_kernel hs_kernel;

    /*
     * AFH_channel_map [C0028] 10 multiple bytes -
     * If AFH_mode is AFH_enabled, this parameter contains 79 1-bit fields, 
     * otherwise the contents are reserved.
     * The nth (numbering from 0) such field (in the range 0 to 78) contains 
     * the value for channel n.
     * Bit 79 is reserved (set to 0 when transmitted and ignored when received)
     * The 1-bit field is interpreted as follows:
     *  0: channel n is unused, 1: channel n is used
     */
    u_int8  afh_current_channel_map[AFH_BYTES_PER_CHANNEL_MAP];
    u_int8  afh_current_num_used_chns;
    u_int8  afh_current_master_tx_freq;

    /*
     * AFH_instant[C0028] 4 u_int32 slots Bits 27:1 of the Bluetooth
     * master clock value at the time of switching hop sequences
     */
    u_int8  afh_next_channel_map[AFH_BYTES_PER_CHANNEL_MAP];
    u_int8  afh_next_num_used_chns;
    u_int32 afh_instant;
    
    /*
     * Channel Classification 
     * AFH_reporting_mode    0: AFH_reporting_disabled  1: AFH_reporting_enabled
     * AFH_interval_min/max  Valid Range is 0x0640 to 0xBB80 slots (1 to 30s)
     */
    u_int16 afh_min_interval;
    u_int16 afh_max_interval;
    u_int8  afh_reporting_mode;

    /*
     * 40 2-bit fields.  The nth (numbering from 0) such field defines 
     * the classification of channels 2n and 2n+1, other than the
     * 39th field which just contains the classification of channel 78.
     * Each field interpreted as an integer whose values indicate:
     *  0 = unknown   1 = good    2 = reserved    3 = bad
     */
    u_int8  afh_channel_classification[10];

    /*
     * Mapping tables (optimisation is possible)
     */
    u_int8  afh_mapping_table_1[AFH_N_MAX_USED_CHANNELS];
    u_int8  afh_mapping_table_2[AFH_N_MAX_USED_CHANNELS];

    u_int8  *p_current_mapping_table;
    u_int8  *p_next_mapping_table;
} t_afh_kernel;

typedef struct s_afh_device_link
{
    t_afh_kernel *p_afh_kernel;

    u_int8  afh_current_mode;
    u_int8  afh_next_mode;
} t_afh_link;

#if PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED

void LSLCafh_Initialise(void);
void LSLCafh_Assign_AFH_Kernel(u_int8 piconet_index,
             t_deviceIndex device_index, t_uap_lap uap_lap);

DRAM_CODE void LSLCafh_Backup_AFH_Link_State(t_deviceIndex device_index);
void LSLCafh_Restore_AFH_Link_State(t_deviceIndex device_index);

t_error LSLCafh_set_AFH(t_deviceIndex device_index, const t_p_pdu p_LMP_set_AFH_PDU);
RAM_CODE t_freq  LSLCafh_Get_Frequency_Channel(t_deviceIndex device_index, t_clock clk);

const u_int8* LSLCafh_LM_Read_AFH_Channel_Map(
    t_deviceIndex device_index, u_int8 *p_afh_mode);

u_int8 LSLCafh_Count_1_Bits(const u_int8 bit_array[]);

RAM_CODE void LSLCafh_Process_Baseband_Ack(t_deviceIndex device_index);

/*
 * Allow access to 79 bit vector
 */
extern const u_int8 _LSLCafh_channel_bit_vector[AFH_BYTES_PER_CHANNEL_MAP+1];

#define mLSLCafh_Get_79_Bit_Vector_Set_Ref()    (_LSLCafh_channel_bit_vector+1)    
#define mLSLCafh_Get_80_Bit_Vector_Set_Ref()    (_LSLCafh_channel_bit_vector)    

/*
 * Allow access to AFH links current mode
 */
extern t_afh_link _LSLCafh_AFH_link[];

#define mLSLCafh_Get_AFH_Current_Mode(i) (_LSLCafh_AFH_link[(i)].afh_current_mode)

#elif !PRH_BS_CFG_SYS_LMP_AFH_CAPABLE_SUPPORTED
/*
 * Empty function calls
 */
#define LSLCafh_Initialise()
#define LSLCafh_Assign_AFH_Kernel(piconet_index, device_index, uap_lap)
#define LSLCafh_Backup_AFH_Link_State(device_index)
#define LSLCafh_Restore_AFH_Link_State(device_index)
#endif

#endif
