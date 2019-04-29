#ifndef _PARTHUS_SYS_CONFIG_IMPL_
#define _PARTHUS_SYS_CONFIG_IMPL_

/***********************************************************************
 *
 * MODULE NAME:    sys_config_impl.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    t_SYS_Config Wrapper Implementation
 * MAINTAINER:     Ivan Griffin
 * DATE:           25 March 2000
 *
 * SOURCE CONTROL: $Id: sys_config_impl.h,v 1.43 2014/05/19 19:26:53 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    07 January 2000 -   IG       - first version
 *
 *
 ***********************************************************************/
#include "sys_config.h"
#include "hci_params.h"

#if (PRAGMA_INLINE==1)
#pragma inline(SYSconfig_Get_Local_BD_Addr,\
SYSconfig_Get_Local_BD_Addr_Ex,\
SYSconfig_Get_Local_BD_Addr_Ref,\
SYSconfig_Get_Local_BD_Addr_U32,\
SYSconfig_Get_Local_Syncword,\
SYSconfig_Get_Local_Syncword_Ex,\
SYSconfig_Get_Local_Syncword_Ref,\
SYSconfig_Set_Local_Syncword,\
SYSconfig_Get_HC_Commands_Ref,\
SYSconfig_Get_LMP_Features_Ref,\
SYSconfig_Get_LMP_Extended_Features_Ref, \
SYSconfig_Set_LMP_Features,\
SYSconfig_Is_Feature_Supported,\
SYSconfig_Is_Command_Supported,\
SYSconfig_Disable_Feature,\
SYSconfig_Enable_Feature,\
SYSconfig_Get_Ext_Features_Ref,\
SYSconfig_Set_Ext_Features,\
SYSconfig_Get_Hopping_Mode,\
SYSconfig_Get_Tx_Freq,\
SYSconfig_Get_Rx_Freq,\
SYSconfig_Get_Clock_Jitter,\
SYSconfig_Set_Clock_Jitter,\
SYSconfig_Get_Clock_Drift,\
SYSconfig_Set_Clock_Drift,\
SYSconfig_Get_Max_Active_Devices,\
SYSconfig_Set_Max_Active_Devices,\
SYSconfig_Get_Max_Parked_Devices,\
SYSconfig_Set_Max_Parked_Devices,\
SYSconfig_Get_Device_Links_Location,\
SYSconfig_Set_Device_Links_Location,\
SYSconfig_Get_Local_Device_Index,\
SYSconfig_Set_Local_Device_Index,\
SYSconfig_Get_Max_Active_Devices_In_Piconet,\
SYSconfig_Set_Max_Active_Devices_In_Piconet,\
SYSconfig_Get_SYS_Rand_Seed,\
#if (PRH_BS_CFG_SYS_DEPRECATED_BT12_FEATURES_SUPPORTED==1)
SYSconfig_Get_Unit_Key_Ref,\
SYSconfig_Set_Unit_Key\
#endif
SYSconfig_Get_Erroneous_Data_Reporting\
SYSconfig_Set_Erroneous_Data_Reporting\
#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
SYSconfig_Get_AGC_Mode\
SYSconfig_Set_AGC_Mode\
#endif

)
#endif


__INLINE__ t_bd_addr SYSconfig_Get_Local_BD_Addr(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.local_device_address;
}

__INLINE__ void SYSconfig_Get_Local_BD_Addr_Ex(t_bd_addr* addr)
{
    extern t_SYS_Config g_sys_config;
    *addr = g_sys_config.local_device_address;
}

__INLINE__ const t_bd_addr *SYSconfig_Get_Local_BD_Addr_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return &(g_sys_config.local_device_address);
}

__INLINE__ void SYSconfig_Get_Local_BD_Addr_U32(u_int32* addr)
{
    extern t_SYS_Config g_sys_config;
    BDADDR_Convert_to_U32x2(&g_sys_config.local_device_address, addr);
}

__INLINE__ void SYSconfig_Set_Local_BD_Addr(t_bd_addr bd_addr)
{
    extern t_SYS_Config g_sys_config;
    int i;

    for(i = 0; i < 6; i++)
    {
        g_sys_config.local_device_address.bytes[i] = bd_addr.bytes[i];
    }
    SYShalconfig_BD_ADDR_Was_Set();
}

__INLINE__ t_syncword SYSconfig_Get_Local_Syncword(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.local_device_syncword;
}

__INLINE__ void SYSconfig_Get_Local_Syncword_Ex(t_syncword* p_syncword)
{
    extern t_SYS_Config g_sys_config;
    *p_syncword = g_sys_config.local_device_syncword;
}

__INLINE__ const t_syncword *SYSconfig_Get_Local_Syncword_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return &(g_sys_config.local_device_syncword);
}

__INLINE__ void SYSconfig_Set_Local_Syncword(t_syncword syncword)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.local_device_syncword = syncword;
}

/*****************************************************************************
 * SYSconfig_Get_HC_Commands_Ref
 *
 * Returns a reference to the hci command set 
 * (always the first 64 bytes in command array) 
 *****************************************************************************/
__INLINE__ u_int8* SYSconfig_Get_HC_Commands_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.hci_command_set;
}


/*****************************************************************************
 * SYSconfig_Get_LMP_Features_Ref
 *
 * Returns a reference to the lmp feature set 
 * (always the first 8 bytes in features array) 
 *****************************************************************************/
__INLINE__ u_int8* SYSconfig_Get_LMP_Features_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    SYShal_config_set_EDR3_feature();
    return g_sys_config.feature_set;
}

/*****************************************************************************
 * SYSconfig_Set_LMP_Features
 *
 * Sets the lmp feature set (always 8 bytes) to those defined in array
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_LMP_Features(const u_int8 lmp_features_array[8])
{
    extern t_SYS_Config g_sys_config;
    u_int8 *p_lmp_features = g_sys_config.feature_set;
    while (p_lmp_features < g_sys_config.feature_set+8 )
    {
        *p_lmp_features++ = *lmp_features_array++;
    }
}

/*****************************************************************************
 * SYSconfig_Get_LMP_Extended_Features_Ref
 *
 * Returns a reference to the correct page of the lmp feature set 
 * (in multiples of 8 bytes. 8 bytes carried in each page) 
 *****************************************************************************/
__INLINE__ u_int8* SYSconfig_Get_LMP_Extended_Features_Ref(u_int8 page)
{
    extern t_SYS_Config g_sys_config;
    return &(g_sys_config.feature_set[page<<3]);
}


/*****************************************************************************
 * SYSconfig_Is_Feature_Supported
 *
 * Returns a TRUE if feature is supported i.e. corresponding bit is 1
 *
 * lmp_feature     Byte 1    Defines the byte postion in feature set
 *                 Byte 0    Defines the bit position in identified byte
 *****************************************************************************/
__INLINE__ boolean SYSconfig_Is_Feature_Supported(u_int32 lmp_feature_id)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.feature_set[lmp_feature_id>>8] & (lmp_feature_id);
}

/*****************************************************************************
 * SYSconfig_Is_Command_Supported
 *
 * Returns a TRUE if command is supported i.e. corresponding bit is 1
 *
 * hci_command     Byte 1    Defines the byte postion in feature set
 *                 Byte 0    Defines the bit position in identified byte
 *****************************************************************************/
__INLINE__ boolean SYSconfig_Is_Command_Supported(u_int32 hci_command_id)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.hci_command_set[hci_command_id>>8] & (hci_command_id);  
}

/*****************************************************************************
 * SYSconfig_Enable_Feature
 *
 * Enables feature i.e. corresponding bit is set to 1
 *
 * lmp_feature     Byte 1    Defines the byte postion in feature set
 *                 Byte 0    Defines the bit position in identified byte
 *****************************************************************************/
__INLINE__ void SYSconfig_Enable_Feature(u_int32 lmp_feature_id)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.feature_set[lmp_feature_id>>8] |= (u_int8) _SYS_GET_CHAR_8_BITS(lmp_feature_id);
}

/*****************************************************************************
 * SYSconfig_Disable_Feature
 *
 * Disables feature i.e. corresponding bit is set to 0
 *
 * lmp_feature     Byte 1    Defines the byte postion in feature set
 *                 Byte 0    Defines the bit position in identified byte
 *****************************************************************************/
__INLINE__ void SYSconfig_Disable_Feature(u_int32 lmp_feature_id)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.feature_set[lmp_feature_id>>8] &= (u_int8) _SYS_GET_CHAR_8_BITS(~lmp_feature_id);
}

/*****************************************************************************
 * SYSconfig_Get_Ext_Features_Ref
 *
 * Returns a reference to the extended feature set (always 8 byte offset)
 *****************************************************************************/
__INLINE__ u_int8* SYSconfig_Get_Ext_Features_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.feature_set+8;
}

/*****************************************************************************
 * SYSconfig_Set_Ext_Features
 *
 * Sets the extended feature set to those defined in array
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Ext_Features(const u_int8 ext_features_array[8])
{
    extern t_SYS_Config g_sys_config;
    u_int8 *p_ext_features = g_sys_config.feature_set+8;
    while (p_ext_features < g_sys_config.feature_set + 
        sizeof(g_sys_config.feature_set))
    {
        *p_ext_features++ = *ext_features_array++;
    }
}

/*****************************************************************************
 * SYSconfig_Get_Version_Info_Ref
 *
 * Returns a reference to the version info
 *****************************************************************************/
__INLINE__ const t_versionInfo* SYSconfig_Get_Version_Info_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return &g_sys_config.version_info;
}

/*****************************************************************************
 * SYSconfig_Get_HC_Buffer_Size_Ref
 *
 * Returns a reference to the Host Controller ACL/SCO Buffer Sizes/Number of
 *****************************************************************************/
__INLINE__ const t_bufferSize* SYSconfig_Get_HC_Buffer_Size_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return &g_sys_config.hc_buffer_size;
}

/*****************************************************************************
 * SYSconfig_Get_Device_Class
 *
 * Returns the device class for this device
 *****************************************************************************/
__INLINE__ t_classDevice SYSconfig_Get_Device_Class(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.device_class;
}

/*****************************************************************************
 * SYSconfig_Set_Device_Class
 *
 * Sets the device class for this device
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Device_Class(t_classDevice device_class)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.device_class = device_class;
    SYShalconfig_Device_Class_Was_Set();
}

/*****************************************************************************
 * SYSconfig_Get_Clock_Jitter
 *
 * Gets the Clock Jitter in us (default = 10us) for this device
 *****************************************************************************/
__INLINE__ u_int8 SYSconfig_Get_Clock_Jitter(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.clock_jitter_us;
}

/*****************************************************************************
 * SYSconfig_Set_Clock_Jitter
 *
 * Sets the Clock Jitter in us for this device
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Clock_Jitter(u_int8 jitter)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.clock_jitter_us = jitter;
}

/*****************************************************************************
 * SYSconfig_Get_Clock_Drift
 *
 * Gets the clock drift in ppm (default=250ppm) for this device
 *****************************************************************************/
__INLINE__ u_int8 SYSconfig_Get_Clock_Drift(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.clock_drift_ppm;
}

/*****************************************************************************
 * SYSconfig_Set_Clock_Drift
 *
 * Sets the clock drift in ppm (default=250ppm) for this device
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Clock_Drift(u_int8 drift)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.clock_drift_ppm = drift;
}

__INLINE__ u_int32 SYSconfig_Get_Data_Whitening_Enable(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.data_whitening_enable;
}

__INLINE__ void SYSconfig_Set_Data_Whitening_Enable(boolean dwh_enable)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.data_whitening_enable = dwh_enable;
}

__INLINE__ u_int32 SYSconfig_Get_Hopping_Mode(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.hopping_mode;
}

__INLINE__ u_int32 SYSconfig_Get_Tx_Freq(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.tx_freq;
}

__INLINE__ u_int32 SYSconfig_Get_Rx_Freq(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.rx_freq;
}

__INLINE__ void    SYSconfig_Set_Hopping_Mode(t_rfSelection hopping_mode,
               t_freq txMasterFixedFreq, t_freq rxMasterFixedFreq)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.hopping_mode = hopping_mode;
    g_sys_config.tx_freq = txMasterFixedFreq;
    g_sys_config.rx_freq = rxMasterFixedFreq;
}

__INLINE__ u_int32 SYSconfig_Get_Win_Ext(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.win_ext;
}

/*****************************************************************************
 * SYSconfig_Get_Local_Device_Index
 *
 * Gets the local device index. 
 *****************************************************************************/
__INLINE__ u_int16 SYSconfig_Get_Local_Device_Index(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.local_device_index;
}

/*****************************************************************************
 * SYSconfig_Set_Local_Device_Index
 *
 * Sets the local device index. 
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Local_Device_Index(u_int16 local_device_index)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.local_device_index = local_device_index;
}


/*****************************************************************************
 * SYSconfig_Get_Max_Active_Devices
 *
 * Gets the maximum number of active devices supported. 
 *****************************************************************************/
__INLINE__ u_int16 SYSconfig_Get_Max_Active_Devices(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.max_active_devices;
}

/*****************************************************************************
 * SYSconfig_Set_Max_Active_Devices
 *
 * Sets the maximum number of active devices supported. 
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Max_Active_Devices(u_int16 max_active_devices)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.max_active_devices = max_active_devices;
}

/*****************************************************************************
 * SYSconfig_Get_Max_Active_Devices_In_Piconet
 *
 * Gets the maximum number of active devices supported in the piconet. 
 *****************************************************************************/
__INLINE__ u_int16 SYSconfig_Get_Max_Active_Devices_In_Piconet(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.max_active_devices_in_piconet;
}

/*****************************************************************************
 * SYSconfig_Set_Max_Active_Devices_In_Piconet
 *
 * Sets the maximum number of active devices supported in the piconet. 
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Max_Active_Devices_In_Piconet(u_int16 max_active_devices_in_piconet)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.max_active_devices_in_piconet = max_active_devices_in_piconet;
}


/*****************************************************************************
 * SYSconfig_Get_Device_Links_Location
 *
 * Gets the address location of the device_links array.
 *****************************************************************************/
__INLINE__ void* SYSconfig_Get_Device_Links_Location(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.device_links_location;
}


/*****************************************************************************
 * SYSconfig_Set_Device_Links_Location
 *
 * Sets the address location of the device_links array.
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Device_Links_Location(void* pDL)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.device_links_location = pDL;
}


/*****************************************************************************
 * SYSconfig_Get_SYS_Rand_Seed
 *
 * Gets the random number seed
 *****************************************************************************/
__INLINE__ u_int32 SYSconfig_Get_SYS_Rand_Seed(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.rand_seed;
}

#if (PRH_BS_CFG_SYS_DEPRECATED_BT12_FEATURES_SUPPORTED==1)
/*****************************************************************************
 * SYSconfig_Set_Unit_Key
 *
 * Sets the new unit key for a device
 *****************************************************************************/
__INLINE__ void SYSconfig_Set_Unit_Key(u_int8* new_key)
{
    u_int8 i;
    extern t_SYS_Config g_sys_config;
    for(i = 0; i < 16; i++)
        g_sys_config.unit_key[i] = new_key[i];
    SYShalconfig_Unit_Key_Was_Set();
}

/*****************************************************************************
 * SYSconfig_Get_Unit_Key_Ref
 *
 * Gets the unit key
 *****************************************************************************/
__INLINE__ u_int8* SYSconfig_Get_Unit_Key_Ref(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.unit_key;
}

#endif

__INLINE__ u_int8 SYSconfig_Get_Erroneous_Data_Reporting(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.erroneous_data_reporting;
}

__INLINE__ void SYSconfig_Set_Erroneous_Data_Reporting(u_int8 erroneous_dr)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.erroneous_data_reporting = erroneous_dr;

}

#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)
__INLINE__ u_int8 SYSconfig_Get_AGC_Mode(void)
{
    extern t_SYS_Config g_sys_config;
    return g_sys_config.agc_mode;

    
}
__INLINE__ void SYSconfig_Set_AGC_Mode(u_int8 agc_mode)
{
    extern t_SYS_Config g_sys_config;
    g_sys_config.agc_mode = agc_mode;
 
}

#endif
#endif

