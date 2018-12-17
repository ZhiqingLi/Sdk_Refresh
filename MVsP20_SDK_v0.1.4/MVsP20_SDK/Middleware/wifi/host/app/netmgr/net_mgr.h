/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef NET_MGR_H_
#define NET_MGR_H_

//#include "host_cmd_engine_priv.h"
#include <host_apis.h>
#include <netstack.h>

#define WLAN_IFNAME "wlan0"

#define NET_MGR_NO_SYS 0

#define NET_MGR_AUTO_RETRY

//#define NET_MGR_AUTO_JOIN

typedef enum link_status_en {
    LINK_DOWN           = 0,
	LINK_UP             = 1,
} link_status;

typedef enum e_netmgr_msg
{
    MSG_SCAN_REQ, // when station mode, save ap list
    MSG_SCONFIG_REQ,
    MSG_JOIN_REQ, // when station mode and connect ok, if dhcpc flag is 1, netmgr will start dhcpc enable, or do nothing.
    MSG_JOIN_OTHER_REQ,
    MSG_LEAVE_REQ,
    MSG_CONTROL_REQ,
    MSG_SWITCH_REQ,

    MSG_SCAN_DONE,
    MSG_SCAN_RESULT, // when station mode, save ap list
    MSG_JOIN_RESULT, // when station mode and connect ok, if dhcpc flag is 1, netmgr will start dhcpc enable, or do nothing.
    MSG_LEAVE_RESULT, // when station mode
    MSG_SCONFIG_SCANNING_DONE, // when station mode, save ap list
    MSG_SCONFIG_DONE,
    //MSG_DHCPD_CHANGE, // when ap mode, enable or disable dhcpd.
    //MSG_DHCPC_CHANGE, // when station mode, set dhcpc flag 1 or 0.
    MSG_AP_IP_CHANGE, // when ap mode, user change ip info, netmgr need to restart dhcpd
    MSG_PS_SETUP_OK,
    MSG_PS_WAKENED,
    MSG_SCONFIG_PROCESS,
}netmgr_msg;

typedef enum{
	WIFI_SEC_NONE,
	WIFI_SEC_WEP,
	WIFI_SEC_WPA_PSK,		//8~63	ASCII
	WIFI_SEC_WPA2_PSK,		//8~63	ASCII
	WIFI_SEC_WPS,
	WIFI_SEC_MAX,
}wifi_sec_type;

typedef struct st_ipinfo
{
    u32 ipv4;
    u32 netmask;
    u32 gateway;
    u32 dns;
}ipinfo;


typedef ssv6xxx_hw_mode wifi_mode;
typedef Ap_setting wifi_ap_cfg;
typedef Sta_setting wifi_sta_cfg;
typedef Ap_sta_status wifi_info;

typedef struct st_wifi_sta_join_cfg
{
    //wifi_sec_type    sec_type;
    struct cfg_80211_ssid ssid;
    u8                  password[MAX_PASSWD_LEN+1];
}wifi_sta_join_cfg;

typedef struct st_netmgr_cfg
{
    u32 ipaddr;
    u32 netmask;
    u32 gw;
    u32 dns;
    struct st_dhcps_info dhcps;
}netmgr_cfg;

typedef struct st_wifi_sconfig_result
{
    struct cfg_80211_ssid ssid;
    u8                  password[MAX_PASSWD_LEN+1];
}wifi_sconfig_result;

/**
 *  struct SSV6XXX_SCONFIG_OP - For user to registers that their call back function
 */
typedef struct{
    /*
    Init function.
    return value
    0: meason ok, -1: means fail
    */
    int (*UserSconfigInit)(void);

    /*
    state machine, user shoulde monitor  the status and select channel in this function
    return value:
    1: Keep going
    0: Finish
    -1: Error
    if the return value is not "1", net_mgr will stop to run the UserSconfigSM.
    */
    int (*UserSconfigSM)(void);

    /*force to stop and break the smart configuration*/
    void (*UserSconfigDeinit)(void);

    /*
    Parser the rx data
    */
    void (*UserSconfigPaserData)(u8 channel, u8 *rx_buf, u32 len);

    /*Do something after get the IP address*/
    void (*UserSconfigConnect)(void);

}SSV6XXX_USER_SCONFIG_OP;

extern void netmgr_cfg_get(netmgr_cfg *p_cfg);
extern void netmgr_cfg_set(netmgr_cfg *p_cfg);
extern void ssv_netmgr_init(bool default_cfg);
extern int netmgr_igmp_enable(bool on);
extern int netmgr_ipinfo_get(char *ifname, ipinfo *info);
extern int netmgr_ipinfo_set(char *ifname, ipinfo *info, bool auto_dhcpd_info);
extern int netmgr_hwmac_get(char *ifname, void *mac);
extern bool netmgr_wifi_is_connected();
extern void netmgr_netif_status_set(bool on);
extern bool netmgr_netif_status_get();
extern int netmgr_dhcps_info_set(dhcps_info *if_dhcps);
extern int netmgr_dhcpc_set(bool dhcpc_enable);
extern int netmgr_dhcpd_set(bool dhcpd_enable);
extern int netmgr_dhcp_status_get(bool *dhcpd_status, bool *dhcpc_status);
extern int netmgr_dhcd_ipmac_get(dhcpdipmac *ipmac, int *size_count);
extern bool netmgr_wifi_check_mac(unsigned char * mac);
extern bool netmgr_wifi_check_sconfig();
extern bool netmgr_wifi_check_sta();
extern bool netmgr_wifi_check_ap();
extern bool netmgr_wifi_check_sta_connected();
extern void netmgr_netif_status_set(bool on);
extern void netmgr_netif_link_set(bool on);

//Join a AP that is already in the AP list
extern int netmgr_wifi_join_async(wifi_sta_join_cfg *join_cfg);

//Join a AP that is not in the AP list, this function will do the scanning first, and then join. It cause more times on running time.
extern int netmgr_wifi_join_other_async(wifi_sta_join_cfg *join_cfg);
extern int netmgr_wifi_leave_async(void);
extern int netmgr_wifi_control_async(wifi_mode mode, wifi_ap_cfg *ap_cfg, wifi_sta_cfg *sta_cfg);

//Join a AP that is already in the AP list
extern int netmgr_wifi_join(wifi_sta_join_cfg *join_cfg);
//Join a AP that is not in the AP list, this function will do the scanning first, and then join. It cause more times on running time.
extern int netmgr_wifi_join_other(wifi_sta_join_cfg *join_cfg);
extern int netmgr_wifi_leave(void);
extern int netmgr_wifi_control(wifi_mode mode, wifi_ap_cfg *ap_cfg, wifi_sta_cfg *sta_cfg);
extern void netmgr_wifi_station_off();
extern void netmgr_wifi_ap_off();

//If you assign th 0 to channel_mask, we use the defulat value.
//The default value is defined in DEFAULT_STA_CHANNEL_MASK
extern u16 g_sta_channel_mask;
extern int netmgr_wifi_scan_async(u16 channel_mask, char *ssids[], int ssids_count);
extern int netmgr_wifi_scan(u16 channel_mask, char *ssids[], int ssids_count);
extern int netmgr_wifi_sconfig(u16 channel_mask);
extern int netmgr_wifi_sconfig_async(u16 channel_mask);

#ifdef  NET_MGR_AUTO_JOIN
extern void netmgr_apinfo_clear();
extern void netmgr_apinfo_remove(char *ssid);
extern void netmgr_apinfo_show();

#endif

int netmgr_wifi_get_sconfig_result(wifi_sconfig_result *res);

#endif //NET_MGR_H_
