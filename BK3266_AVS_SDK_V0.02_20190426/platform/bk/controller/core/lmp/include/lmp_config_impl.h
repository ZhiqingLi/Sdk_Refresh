#ifndef _PARTHUS_LMP_CONFIG_IMPL_
#define _PARTHUS_LMP_CONFIG_IMPL_

/***********************************************************************
 *
 * MODULE NAME:    lmp_config_impl.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LM device state macros
 * MAINTAINER:     Tom Kerwick
 * DATE:           08 May 2003
 *
 * SOURCE CONTROL: $Id: lmp_config_impl.h,v 1.20 2014/07/03 17:27:27 garyf Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    08 May 2003 -   TK       - first version
 *
 *
 ***********************************************************************/

#include "sys_config.h"
#include "lc_interface.h"
#include "lmp_afh.h"
#include "lmp_link_qos.h"
#include "lmp_acl_container.h"
#include "config.h"

#ifdef BT_DUALMODE

#include "le_link_layer.h"
#endif
#if (PRAGMA_INLINE==1)
#pragma inline(\
LMconfig_LM_Clear_Device_State,\
LMconfig_LM_Set_Device_State,\
LMconfig_LM_Page_Scans_Allowed,\
LMconfig_LM_Inquiry_Scans_Allowed,\
LMconfig_LM_Periodic_Inquiry_Allowed,\
LMconfig_LM_Check_Device_State,\
LMconfig_LM_Enforce_Scans_Disabled\
LMconfig_LM_Connected_As_Master\
LMconfig_LM_Connected_As_Slave\
LMconfig_LM_Connected_As_Scatternet\
LMconfig_LM_Num_Piconets\
LMconfig_LM_Connected_As_SCO_Master\
LMconfig_LM_Connected_As_SCO_Slave)
#endif

#define LM_CONFIG_PAGE_SCANS_ALLOWED        (PRH_DEV_STATE_ACLS_ACTIVE \
                                            |PRH_DEV_STATE_SUSPEND_INQUIRY_SCANS \
                                            |PRH_DEV_STATE_SUSPEND_PERIODIC_INQUIRY \
											|PRH_DEV_STATE_INQUIRY_PENDING )

#define LM_CONFIG_INQUIRY_SCANS_ALLOWED     (PRH_DEV_STATE_ACLS_ACTIVE \
                                            |PRH_DEV_STATE_SUSPEND_PAGE_SCANS \
                                            |PRH_DEV_STATE_SUSPEND_PERIODIC_INQUIRY \
											|PRH_DEV_STATE_INQUIRY_PENDING)

#define LM_CONFIG_PERIODIC_INQUIRY_ALLOWED  (PRH_DEV_STATE_ACLS_ACTIVE \
                                            |PRH_DEV_STATE_SUSPEND_PAGE_SCANS \
                                            |PRH_DEV_STATE_SUSPEND_INQUIRY_SCANS)

#if (PRH_BS_DEV_DONT_FORCE_SCANS_DISABLE == 1)
#define LM_CONFIG_ENFORCE_SCANS_DISABLED    0 /* #2741 */
#else
#define LM_CONFIG_ENFORCE_SCANS_DISABLED    (PRH_DEV_STATE_MAX_SLAVE_ACLS \
                                            |PRH_DEV_STATE_MAX_MASTER_ACLS \
                                            |PRH_DEV_STATE_TEST_MODE \
                                            |PRH_DEV_STATE_LOOPBACK_MODE)
#endif

extern u_int32 g_LM_device_state;

/********************************************************************
 *
 * LMconfig_LM_Clear_Device_State
 *
 ********************************************************************/
__INLINE__ void LMconfig_LM_Clear_Device_State(u_int32 state)
{
    if( g_LM_device_state & state & (PRH_DEV_STATE_PAGE_PENDING|PRH_DEV_STATE_INQUIRY_PENDING) )
    {
#if (PRH_BS_DEV_EXTENDED_QOS_NOTIFICATIONS_SUPPORTED==1)
        LMqos_Notify_QOS_All_Slaves(1/*re-enable QOS poll interval*/);
#endif
#if (PRH_BS_DEV_SUSPEND_SLAVE_CLASSIFICATIONS_SUPPORTED==1)
        LMPafh_Request_Classification_Reporting_All_Slaves(1/*re-enable*/);
#endif
    }

    g_LM_device_state &= ~state;
}

/********************************************************************
 *
 * LMconfig_LM_Set_Device_State
 *
 ********************************************************************/
__INLINE__ void LMconfig_LM_Set_Device_State(u_int32 state)
{
    if( ~g_LM_device_state & state & ~LM_CONFIG_PAGE_SCANS_ALLOWED )
    {
        LC_Page_Scan_Cancel();
    }
    
    if( ~g_LM_device_state & state & ~LM_CONFIG_INQUIRY_SCANS_ALLOWED )
    {
        LC_Inquiry_Scan_Cancel();
    }

    if( ~g_LM_device_state & state & (PRH_DEV_STATE_PAGE_PENDING|PRH_DEV_STATE_INQUIRY_PENDING) )
    {
#if (PRH_BS_DEV_EXTENDED_QOS_NOTIFICATIONS_SUPPORTED==1)
        LMqos_Notify_QOS_All_Slaves(0/*disable QOS poll interval - emergency polling only*/);
#endif
#if (PRH_BS_DEV_SUSPEND_SLAVE_CLASSIFICATIONS_SUPPORTED==1)
        LMPafh_Request_Classification_Reporting_All_Slaves(0/*disable*/);
#endif
    }

    g_LM_device_state |= state;
}

/********************************************************************
 *
 * LMconfig_LM_Page_Scans_Allowed
 *
 ********************************************************************/
__INLINE__ u_int32 LMconfig_LM_Page_Scans_Allowed(void)
{
    return ( (!(g_LM_device_state & ~LM_CONFIG_PAGE_SCANS_ALLOWED))
#if (PRH_BS_DEV_DISALLOW_SCANNING_AS_SLAVE == 1)
		&& (!LMconfig_LM_Connected_As_Slave())
#else
        && (!(LMconfig_LM_Connected_As_Scatternet() && LMeScoctr_Get_Number_eSCO_Connections()))
#endif
		&& !(g_LM_device_state & PRH_DEV_STATE_TEST_MODE)
		);
}

/********************************************************************
 *
 * LMconfig_LM_Inquiry_Scans_Allowed
 *
 ********************************************************************/
__INLINE__ u_int32 LMconfig_LM_Inquiry_Scans_Allowed(void)
{
    return ((!(g_LM_device_state & ~LM_CONFIG_INQUIRY_SCANS_ALLOWED))
#if (PRH_BS_DEV_DISALLOW_SCANNING_AS_SLAVE == 1)
		&& (!LMconfig_LM_Connected_As_Slave())
#else
		&& (!(LMconfig_LM_Connected_As_Scatternet() && LMeScoctr_Get_Number_eSCO_Connections()))
#endif
		&& !(g_LM_device_state & PRH_DEV_STATE_TEST_MODE)
		);
}

/********************************************************************
 *
 * LMconfig_LM_Periodic_Inquiry_Allowed
 *
 ********************************************************************/
__INLINE__ u_int32 LMconfig_LM_Periodic_Inquiry_Allowed(void)
{
    return !(g_LM_device_state & ~LM_CONFIG_PERIODIC_INQUIRY_ALLOWED);
}

/********************************************************************
 *
 * LMconfig_LM_Check_Device_State
 *
 ********************************************************************/
__INLINE__ u_int32 LMconfig_LM_Check_Device_State(u_int32 query_state)
{
    return (g_LM_device_state & query_state);
}

/********************************************************************
 *
 * LMconfig_LM_Enforce_Scans_Disabled
 *
 * Determines whether scans should be reported disabled, and command
 * to enable disallowed at the HCI level.
 *
 ********************************************************************/
__INLINE__ u_int32 LMconfig_LM_Enforce_Scans_Disabled(void)
{
    return (g_LM_device_state & LM_CONFIG_ENFORCE_SCANS_DISABLED);
}

/********************************************************************
 *
 * LMconfig_LM_Connected_As_Master
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Connected_As_Master(void)
{
    return (g_LM_config_info.num_master_acl_links > 0);
}

/********************************************************************
 *
 * LMconfig_LM_Connected_As_Slave
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Connected_As_Slave(void)
{
    return (g_LM_config_info.num_slave_acl_links > 0);
}

/********************************************************************
 *
 * LMconfig_LM_Connected_As_Scatternet
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Connected_As_Scatternet(void)
{
#if (PRH_BS_CFG_SYS_SCATTERNET_SUPPORTED==1)
    return ((LMconfig_LM_Connected_As_Master() && LMconfig_LM_Connected_As_Slave())
            || (g_LM_config_info.num_slave_acl_links > 1));
#else
	return FALSE;
#endif
}

#ifdef BT_DUALMODE

__INLINE__ boolean LMconfig_LM_Connected_As_Classic_LE_Scatternet(void)
{
	return (LMconfig_LM_Connected_As_Slave() && LEconnection_Does_Slave_Link_Exist());
}
#endif
/********************************************************************
 *
 * LMconfig_LM_Num_Piconets
 *
 ********************************************************************/
__INLINE__ u_int8 LMconfig_LM_Num_Piconets(void)
{
    return (u_int8)((LMconfig_LM_Connected_As_Master()?(1):(0))
            + (g_LM_config_info.num_slave_acl_links));
}

/********************************************************************
 *
 * LMconfig_LM_Connected_As_SCO_Master
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Connected_As_SCO_Master(void)
{
    return (g_LM_config_info.num_master_sco_links > 0);
}

/********************************************************************
 *
 * LMconfig_LM_Connected_As_SCO_Slave
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Connected_As_SCO_Slave(void)
{
    return (g_LM_config_info.num_slave_sco_links > 0);
}


/********************************************************************
 *
 * LMconfig_LM_Performing_Inquiry
 *
 ********************************************************************/
__INLINE__ boolean LMconfig_LM_Inquiry_Active(void)
{
    return ((g_LM_device_state & PRH_DEV_STATE_INQUIRY_PENDING)!= 0);
}

#endif
