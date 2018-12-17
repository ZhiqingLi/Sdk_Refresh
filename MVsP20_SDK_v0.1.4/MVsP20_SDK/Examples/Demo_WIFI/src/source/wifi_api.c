#include <os_wrapper.h>
#include <log.h>
#include <host_apis.h>
#include <cli/cmds/cli_cmd_wifi.h>
//#include <httpserver_raw/httpd.h>
#include <SmartConfig/SmartConfig.h>
#include <net_mgr.h>
#include <lwip/sockets.h>
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include "lwip/udp.h"
#include "lwip/igmp.h"
#include "debug.h"

void swtich2STAmode(void)
{
    Sta_setting sta;
    MEMSET(&sta, 0 , sizeof(Sta_setting));
    sta.status = TRUE;

    if (netmgr_wifi_control_async(SSV6XXX_HWM_STA, NULL, &sta) == SSV6XXX_FAILED)
    {
        DBG("\n-------------------Swtich to STAmode fail\n");
    }
    else
    {
        DBG("\n---------------------Swtich to STAmode success\n");
    }
}

extern u16 g_SconfigChannelMask;
extern u32 g_sconfig_solution;

//int16_t  SCONFIGmode_default()
//{
//    uint16_t channel = 0x3ffe;
//    Sta_setting sta;
//    MEMSET(&sta, 0 , sizeof(Sta_setting));
//    sta.status = TRUE;
//    if(netmgr_wifi_control_async(SSV6XXX_HWM_SCONFIG, NULL, &sta)!= SSV6XXX_SUCCESS)
//    {
//			LOG_PRINTF("SCONFIG mode fail1\r\n");
//        return -1;
//    }
//
//    vTaskDelay(2000);  //¹Ø¼üdelay
//
//    g_sconfig_solution=WECHAT_AIRKISS_IN_FW;
//
//    if(netmgr_wifi_sconfig_async(channel)!= SSV6XXX_SUCCESS)
//    {
//			LOG_PRINTF("SCONFIG mode fail2\r\n");
//        return -1;
//    }
//
//		return 1;
//}

int cmd_wifi_list(uint8_t *buffer)
{
	int cheak_flag=0;
    u32 i=0,AP_cnt;
    s32     pairwise_cipher_index=0,group_cipher_index=0;
    u8      sec_str[][7]= {"OPEN","WEP40","WEP104","TKIP","CCMP"};
    u8  ssid_buf[MAX_SSID_LEN+1]= {0};
    Ap_sta_status connected_info;

    struct ssv6xxx_ieee80211_bss *ap_list = NULL;
    AP_cnt = ssv6xxx_get_aplist_info((void *)&ap_list);


    MEMSET(&connected_info , 0 , sizeof(Ap_sta_status));
    ssv6xxx_wifi_status(&connected_info);

    if((ap_list==NULL) || (AP_cnt==0))
    {
        LOG_PRINTF("AP list empty!\r\n");
        return 0;
    }
    for (i=0; i<AP_cnt; i++)
    {

        if(ap_list[i].channel_id!= 0)
        {
            LOG_PRINTF("BSSID: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                       ap_list[i].bssid.addr[0],  ap_list[i].bssid.addr[1], ap_list[i].bssid.addr[2],  ap_list[i].bssid.addr[3],  ap_list[i].bssid.addr[4],  ap_list[i].bssid.addr[5]);
            MEMSET((void*)ssid_buf,0,sizeof(ssid_buf));
            MEMCPY((void*)ssid_buf,(void*)ap_list[i].ssid.ssid,ap_list[i].ssid.ssid_len);
            LOG_PRINTF("SSID: %s %d %d\t", ssid_buf,strlen(ssid_buf),ap_list[i].ssid.ssid_len);
            LOG_PRINTF("MYID: %s %d\t", buffer,strlen(buffer));
            if(strlen(ssid_buf) == (strlen(buffer) - 2))
            {
            	if(memcmp(ssid_buf,buffer+2,strlen(ssid_buf)) == 0)
            	{
            		return 1;
            	}
            }
            LOG_PRINTF("@Channel Idx: %d\r\n", ap_list[i].channel_id);
            if(ap_list[i].capab_info&BIT(4)) {
                LOG_PRINTF("Secure Type=[%s]\r\n",
                           ap_list[i].proto&WPA_PROTO_WPA?"WPA":
                           ap_list[i].proto&WPA_PROTO_RSN?"WPA2":"WEP");

                if(ap_list[i].pairwise_cipher[0]) {
                    pairwise_cipher_index=0;
                    LOG_PRINTF("Pairwise cipher=");
                    for(pairwise_cipher_index=0; pairwise_cipher_index<8; pairwise_cipher_index++) {
                        if(ap_list[i].pairwise_cipher[0]&BIT(pairwise_cipher_index)) {
                            LOG_PRINTF("[%s] ",sec_str[pairwise_cipher_index]);
                        }
                    }
                    LOG_PRINTF("\r\n");
                }
                if(ap_list[i].group_cipher) {
                    group_cipher_index=0;
                    LOG_PRINTF("Group cipher=");
                    for(group_cipher_index=0; group_cipher_index<8; group_cipher_index++) {
                        if(ap_list[i].group_cipher&BIT(group_cipher_index)) {
                            LOG_PRINTF("[%s] ",sec_str[group_cipher_index]);
                        }
                    }
                    LOG_PRINTF("\r\n");
                }
            } else {
                LOG_PRINTF("Secure Type=[NONE]\r\n");
            }

            if(!memcmp((void *)ap_list[i].bssid.addr,(void *)connected_info.u.station.apinfo.Mac,ETHER_ADDR_LEN)) {
                LOG_PRINTF("RSSI=-%d (dBm)\r\n",ssv6xxx_get_rssi_by_mac((u8 *)ap_list[i].bssid.addr));
            }
            else {
                LOG_PRINTF("RSSI=-%d (dBm)\r\n",ap_list[i].rxphypad.rpci);
            }
            LOG_PRINTF("\r\n");
        }

    }
    FREE((void *)ap_list);
    return cheak_flag;
}


int cmd_wifi_scan_done(uint8_t *buffer)
{
    u32 i=0,AP_cnt,j=0;
    s32     pairwise_cipher_index=0,group_cipher_index=0;
    u8      sec_str[][7]= {"OPEN","WEP40","WEP104","TKIP","CCMP"};
    u8  ssid_buf[MAX_SSID_LEN+1]= {0};
    Ap_sta_status connected_info;

    struct ssv6xxx_ieee80211_bss *ap_list = NULL;
    AP_cnt = ssv6xxx_get_aplist_info((void *)&ap_list);


    MEMSET(&connected_info , 0 , sizeof(Ap_sta_status));
    ssv6xxx_wifi_status(&connected_info);

    if((ap_list==NULL) || (AP_cnt==0))
    {
        LOG_PRINTF("AP list empty!\r\n");
        return 0;
    }
    for (i=0; i<AP_cnt; i++)
    {

        if(ap_list[i].channel_id!= 0)
        {
            LOG_PRINTF("BSSID: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                       ap_list[i].bssid.addr[0],  ap_list[i].bssid.addr[1], ap_list[i].bssid.addr[2],  ap_list[i].bssid.addr[3],  ap_list[i].bssid.addr[4],  ap_list[i].bssid.addr[5]);
            MEMSET((void*)ssid_buf,0,sizeof(ssid_buf));
            MEMCPY((void*)ssid_buf,(void*)ap_list[i].ssid.ssid,ap_list[i].ssid.ssid_len);
            LOG_PRINTF("SSID: %s\t", ssid_buf);

            LOG_PRINTF("@Channel Idx: %d\r\n", ap_list[i].channel_id);
            if(ap_list[i].capab_info&BIT(4)) {
                LOG_PRINTF("Secure Type=[%s]\r\n",
                           ap_list[i].proto&WPA_PROTO_WPA?"WPA":
                           ap_list[i].proto&WPA_PROTO_RSN?"WPA2":"WEP");

                if(ap_list[i].pairwise_cipher[0]) {
                    pairwise_cipher_index=0;
                    LOG_PRINTF("Pairwise cipher=");
                    for(pairwise_cipher_index=0; pairwise_cipher_index<8; pairwise_cipher_index++) {
                        if(ap_list[i].pairwise_cipher[0]&BIT(pairwise_cipher_index)) {
                            LOG_PRINTF("[%s] ",sec_str[pairwise_cipher_index]);
                        }
                    }
                    LOG_PRINTF("\r\n");
                }
                if(ap_list[i].group_cipher) {
                    group_cipher_index=0;
                    LOG_PRINTF("Group cipher=");
                    for(group_cipher_index=0; group_cipher_index<8; group_cipher_index++) {
                        if(ap_list[i].group_cipher&BIT(group_cipher_index)) {
                            LOG_PRINTF("[%s] ",sec_str[group_cipher_index]);
                        }
                    }
                    LOG_PRINTF("\r\n");
                }
            } else {
                LOG_PRINTF("Secure Type=[NONE]\r\n");
            }

            if(!memcmp((void *)ap_list[i].bssid.addr,(void *)connected_info.u.station.apinfo.Mac,ETHER_ADDR_LEN)) {
                LOG_PRINTF("RSSI=-%d (dBm)\r\n",ssv6xxx_get_rssi_by_mac((u8 *)ap_list[i].bssid.addr));
            }
            else {
                LOG_PRINTF("RSSI=-%d (dBm)\r\n",ap_list[i].rxphypad.rpci);
            }
            LOG_PRINTF("\r\n");

            if(strlen(ssid_buf)<31)
            {
            	sprintf(buffer+j*32,"S:%s",ssid_buf);
            	j++;
            }
        }

    }
    FREE((void *)ap_list);
    return j;
}

void cmd_wifi_scan (void)
{
    netmgr_wifi_scan_async(0xffff, NULL, 0);
}

void cmd_wifi_leave(void)
{
    int ret = -1;
    ret = netmgr_wifi_leave_async();
    if (ret != 0)
    {
        LOG_PRINTF("netmgr_wifi_leave failed !!\r\n");
    }
}


void cmd_wifi_join(void)
{
		int ret = -1;
    wifi_sta_join_cfg *join_cfg = NULL;
	
	  join_cfg = (wifi_sta_join_cfg *)MALLOC(sizeof(wifi_sta_join_cfg));
	
	  memset(join_cfg,0,sizeof(wifi_sta_join_cfg));
//    memcpy(join_cfg->ssid.ssid, "HUAWEI8201",10);
//    join_cfg->ssid.ssid_len=11;
//	  memcpy(join_cfg->password, "12345678",8);
	
//    memcpy(join_cfg->ssid.ssid, "TP-LINK_525C",12);
//    join_cfg->ssid.ssid_len=12;
//	  memcpy(join_cfg->password, "lllllll1",8);

//    memcpy(join_cfg->ssid.ssid, "WIFI-09EFCC",11);
//    join_cfg->ssid.ssid_len=11;
//	memcpy(join_cfg->password, "qwertyu12345",12);
	
    memcpy(join_cfg->ssid.ssid, "MV-Wireless3",12);
    join_cfg->ssid.ssid_len=12;
		memcpy(join_cfg->password, "123qweASD",9);
		
	 ret = netmgr_wifi_join_async(join_cfg);
    if (ret != 0)
    {
	    LOG_PRINTF("netmgr_wifi_join_async failed !!\r\n");
    }

    FREE(join_cfg);
	
}


void mv_wifi_join(uint8_t *buf)
{
	int ret = -1;
    wifi_sta_join_cfg *join_cfg = NULL;
	join_cfg = (wifi_sta_join_cfg *)MALLOC(sizeof(wifi_sta_join_cfg));
	memset(join_cfg,0,sizeof(wifi_sta_join_cfg));
	memcpy(join_cfg->ssid.ssid, buf+2,strlen(buf+2));
	join_cfg->ssid.ssid_len=strlen(buf+2);
	memcpy(join_cfg->password, buf+34,strlen(buf+34));
	 ret = netmgr_wifi_join_async(join_cfg);
    if (ret != 0)
    {
	    LOG_PRINTF("netmgr_wifi_join_async failed !!\r\n");
    }
    FREE(join_cfg);
}


int16_t netif_status_get(void)
{
    struct netif *netif;

    netif = netif_find(WLAN_IFNAME);
    if (netif)
    {
			return netif_is_up(netif);
    }
		return 0;
}


#define AP_SSID    "MV-CS-AP"
#define AP_PSK     "12345678"

int16_t APmode_config(void)
{
    wifi_mode mode;
    wifi_ap_cfg ap_cfg;
    int ret = 0;
    MEMSET((void * )&ap_cfg, 0, sizeof(ap_cfg));
    mode = SSV6XXX_HWM_AP;
    ap_cfg.status = TRUE;
    MEMCPY((void *)ap_cfg.ssid.ssid,AP_SSID,STRLEN(AP_SSID));
    ap_cfg.ssid.ssid_len = STRLEN(AP_SSID);
    STRCPY((void *)(ap_cfg.password),AP_PSK);
    ap_cfg.security =   SSV6XXX_SEC_WPA2_PSK;
    ap_cfg.proto = WPA_PROTO_RSN;
    ap_cfg.key_mgmt = WPA_KEY_MGMT_PSK ;
    ap_cfg.group_cipher=WPA_CIPHER_CCMP;
    ap_cfg.pairwise_cipher = WPA_CIPHER_CCMP;
    ap_cfg.channel = EN_CHANNEL_AUTO_SELECT;

    ret = netmgr_wifi_control_async(mode , &ap_cfg, NULL);
    if (ret != 0)
    {
        LOG_PRINTF("AP mode error\r\n");
        return SSV6XXX_FAILED;
    }

    LOG_PRINTF("AP mode success\r\n");


#if  0       //

    netmgr_cfg cfg;
    cfg.ipaddr = ipaddr_addr("192.168.25.1");
    cfg.netmask = ipaddr_addr("255.255.255.0");
    cfg.gw = ipaddr_addr("192.168.25.1");
    cfg.dns = ipaddr_addr("0.0.0.0");

    cfg.dhcps.start_ip = ipaddr_addr("192.168.25.101");
    cfg.dhcps.end_ip = ipaddr_addr("192.168.25.110");
    cfg.dhcps.max_leases = 10;
    cfg.dhcps.subnet = ipaddr_addr("255.255.255.0");
    cfg.dhcps.gw = ipaddr_addr("192.168.25.1");
    cfg.dhcps.dns = ipaddr_addr("192.168.25.1");
    cfg.dhcps.auto_time = DEFAULT_DHCP_AUTO_TIME;
    cfg.dhcps.decline_time = DEFAULT_DHCP_DECLINE_TIME;
    cfg.dhcps.conflict_time = DEFAULT_DHCP_CONFLICT_TIME;
    cfg.dhcps.offer_time = DEFAULT_DHCP_OFFER_TIME;
    cfg.dhcps.max_lease_sec = DEFAULT_DHCP_MAX_LEASE_SEC;
    cfg.dhcps.min_lease_sec = DEFAULT_DHCP_MIN_LEASE_SEC;
    netmgr_cfg_set(&cfg);
    ssv_netmgr_init_netdev(false);

    ssv6xxx_wifi_cfg();
#endif




    return SSV6XXX_SUCCESS;
}


