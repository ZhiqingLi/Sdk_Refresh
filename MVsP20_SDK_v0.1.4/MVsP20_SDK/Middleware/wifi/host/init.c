/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#include <os_wrapper.h>
#include <log.h>
#include <host_apis.h>
#include <cli/cmds/cli_cmd_wifi.h>
#if HTTPD_SUPPORT
#include <httpserver_raw/httpd.h>
#endif
#include <SmartConfig/SmartConfig.h>
#include <net_mgr.h>
#include <netstack.h>
#include <dev.h>

u32 g_log_module;
u32 g_log_min_level;
// ----------------------------------------------------
//Configurations
// ----------------------------------------------------
//Mac address
u8 config_mac[] ={0x60,0x11,0x22,0x33,0x44,0x55};

//Max number of AP list
u8 g_max_num_of_ap_list=NUM_AP_INFO;

//Auto channel selection in AP mode
u16 g_acs_channel_mask=ACS_CHANNEL_MASK;
u16 g_acs_channel_scanning_interval=ACS_SCANNING_INTERVAL;
u8  g_acs_channel_scanning_loop=ACS_SCANNING_LOOP;

//Default channel mask in sta and smart config mode
u16 g_sta_channel_mask = DEFAULT_STA_CHANNEL_MASK;

// ------------------------- rate control ---------------------------
struct Host_cfg g_host_cfg;
// ------------- User mode SmartConfig ...............................................
SSV6XXX_USER_SCONFIG_OP ssv6xxx_user_sconfig_op;
u32 g_sconfig_solution=SMART_CONFIG_SOLUTION;
// ----------------------------------------------------

u8 g_sconfig_auto_join=SMART_CONFIG_AUTO_JOIN;
// ----------------------------------------------------
u8 g_lwip_tcp_ignore_cwnd=LWIP_TCP_IGNORE_CWND;

#ifdef NET_MGR_AUTO_RETRY
int  g_auto_retry_times_delay = NET_MGR_AUTO_RETRY_DELAY;
int  g_auto_retry_times_max = NET_MGR_AUTO_RETRY_TIMES;
#endif

#if DHCPD_SUPPORT
extern int udhcpd_init(void);
#endif

#if 0
struct ap_info_st sg_ap_info[20];
#endif // 0

void stop_and_halt (void)
{
    //while (1) {}
    /*lint -restore */
} // end of - stop_and_halt -

int ssv6xxx_add_interface(ssv6xxx_hw_mode hw_mode)
{
    //0:false 1:true
    return 1;

}

//=====================Task parameter setting========================
extern struct task_info_st g_txrx_task_info[];
extern struct task_info_st g_host_task_info[];
extern struct task_info_st g_timer_task_info[];
#if !NET_MGR_NO_SYS
extern struct task_info_st st_netmgr_task[];
#endif
#if DHCPD_SUPPORT
extern struct task_info_st st_dhcpd_task[];
#endif
#if (MLME_TASK==1)
extern struct task_info_st g_mlme_task_info[];
#endif

void ssv6xxx_init_task_para(void)
{
    g_txrx_task_info[0].prio = OS_TX_TASK_PRIO;
    g_txrx_task_info[1].prio = OS_RX_TASK_PRIO;
    g_host_task_info[0].prio = OS_CMD_ENG_PRIO;
    g_timer_task_info[0].prio = OS_TMR_TASK_PRIO;
#if !NET_MGR_NO_SYS
    st_netmgr_task[0].prio = OS_NETMGR_TASK_PRIO;
#endif
#if DHCPD_SUPPORT    
    st_dhcpd_task[0].prio = OS_DHCPD_TASK_PRIO;
#endif
#if (MLME_TASK==1)
    g_mlme_task_info[0].prio = OS_MLME_TASK_PRIO;
#endif
}
//=============================================================

int ssv6xxx_start()
{
    u32 wifi_mode;
    ssv6xxx_drv_start();

    /* Reset MAC & Re-Init */

    /* Initialize ssv6200 mac */
    if(-1==ssv6xxx_init_mac())
    {
    	return SSV6XXX_FAILED;
    }

    //Set ap or station register
    wifi_mode = ssv6xxx_wifi_get_mode();
    if ((SSV6XXX_HWM_STA == wifi_mode)||(SSV6XXX_HWM_SCONFIG == wifi_mode))
    {
        if(-1==ssv6xxx_init_sta_mac(wifi_mode))
        {
            return SSV6XXX_FAILED;
        }
    }
    else
    {
        if(-1==ssv6xxx_init_ap_mac())
        {
            return SSV6XXX_FAILED;
        }
    }

    return SSV6XXX_SUCCESS;
}

#define SSV_START_DEMO

#ifdef SSV_START_DEMO
#define JOIN_DEFAULT_SSID    "Default_ap" //"china"
#define JOIN_DEFAULT_PSK     "12345678" //"12345678"

#define AP_DEFAULT_SSID    "ssv-6030-AP"
#define AP_DEFAULT_PSK     "12345678"

ssv6xxx_result STAmode_default(bool bJoin)
{
    int ret;
    wifi_mode mode;
    wifi_sta_join_cfg join_cfg;

    MEMSET((void * )&join_cfg, 0, sizeof(join_cfg));
    mode = SSV6XXX_HWM_STA;
    if(bJoin)
    {
        MEMCPY((void *)join_cfg.ssid.ssid,JOIN_DEFAULT_SSID,STRLEN(JOIN_DEFAULT_SSID));
        join_cfg.ssid.ssid_len=STRLEN(JOIN_DEFAULT_SSID);
        STRCPY((void *)join_cfg.password, JOIN_DEFAULT_PSK);  
        ret = netmgr_wifi_switch_async(mode, NULL, &join_cfg);
    }
    else
    {
        Sta_setting sta;
        MEMSET(&sta, 0 , sizeof(Sta_setting));

        sta.status = TRUE;
        ret = netmgr_wifi_control_async(mode, NULL, &sta);
    }
    if (ret != 0)
    {
        LOG_PRINTF("STA mode error\r\n");
        return SSV6XXX_FAILED;
    }
    
    LOG_PRINTF("STA mode success\r\n");
    return SSV6XXX_SUCCESS;
}

ssv6xxx_result APmode_default()
{
    wifi_mode mode;
    wifi_ap_cfg ap_cfg;
    int ret = 0;
    MEMSET((void * )&ap_cfg, 0, sizeof(ap_cfg));
    mode = SSV6XXX_HWM_AP;
    ap_cfg.status = TRUE;
    MEMCPY((void *)ap_cfg.ssid.ssid,AP_DEFAULT_SSID,STRLEN(AP_DEFAULT_SSID));
    ap_cfg.ssid.ssid_len = STRLEN(AP_DEFAULT_SSID);
    STRCPY((void *)(ap_cfg.password),AP_DEFAULT_PSK);
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
    return SSV6XXX_SUCCESS;
}

extern u16 g_SconfigChannelMask;

ssv6xxx_result SCONFIGmode_default()
{
    wifi_mode mode;
    Sta_setting sta;
    int ret = 0;
    u32 start_tick = 0;
    bool timeout = false;

    mode = SSV6XXX_HWM_SCONFIG;
    MEMSET(&sta, 0 , sizeof(Sta_setting));

    sta.status = TRUE;
    ret = netmgr_wifi_control_async(mode, NULL, &sta);
    if (ret != 0)
    {
        LOG_PRINTF("SCONFIG mode error\r\n");
    }

    start_tick = OS_GetSysTick();
    timeout = false;
    while (!netmgr_wifi_check_sconfig())
    {
        // wait 3 second timeout
        if (((OS_GetSysTick() - start_tick) * OS_TICK_RATE_MS) >= 3000)
        {
            timeout = true;
            break;
        }
        OS_TickDelay(1);
    }

    if (timeout)
    {
        LOG_PRINTF("SCONFIG mode timeout\r\n");
        return SSV6XXX_FAILED;
    }

    g_SconfigChannelMask=0x3FFE; //This variable is only for RD use, customers don't need to modify it.
    g_sconfig_solution=SMART_CONFIG_SOLUTION;
    ret = netmgr_wifi_sconfig_async(g_SconfigChannelMask);
    if (ret != 0)
    {
        LOG_PRINTF("SCONFIG async error\r\n");
        return SSV6XXX_FAILED;
    }

    LOG_PRINTF("SCONFIG mode success\r\n");
    return SSV6XXX_SUCCESS;
}
//ssv6xxx_result SCONFIGmode_default()
//{
//    wifi_mode mode;
//    Sta_setting sta;
//    int ret = 0;
//
//    mode = SSV6XXX_HWM_SCONFIG;
//    MEMSET(&sta, 0 , sizeof(Sta_setting));
//
//    sta.status = TRUE;
//    ret = netmgr_wifi_control_async(mode, NULL, &sta);
//    if (ret != 0)
//    {
//        LOG_PRINTF("SCONFIG mode error\r\n");
//    }
//
//    g_SconfigChannelMask=0x3FFE; //This variable is only for RD use, customers don't need to modify it.
//    g_sconfig_solution=SMART_CONFIG_SOLUTION;
//    ret = netmgr_wifi_sconfig_async(g_SconfigChannelMask);
//    if (ret != 0)
//    {
//        LOG_PRINTF("SCONFIG async error\r\n");
//        return SSV6XXX_FAILED;
//    }
//
//    LOG_PRINTF("SCONFIG mode success\r\n");
//    return SSV6XXX_SUCCESS;
//}

ssv6xxx_result wifi_start(wifi_mode mode, bool static_ip, bool dhcp_server)
{

    ssv6xxx_result res;
    /* station mode */
    if (mode == SSV6XXX_HWM_STA)
    {
        if (static_ip)
        {
            netmgr_dhcpc_set(false);
            {
                ipinfo info;
                
                info.ipv4 = ipaddr_addr("192.168.100.100");
                info.netmask = ipaddr_addr("255.255.255.0");
                info.gateway = ipaddr_addr("192.168.100.1");
                info.dns = ipaddr_addr("192.168.100.1");
                
                netmgr_ipinfo_set(WLAN_IFNAME, &info, false);
            }
        }
        else
        {
            netmgr_dhcpc_set(true);
        }

        res = STAmode_default(false); //false: don't auto join JOIN_DEFAULT_SSID
    }

    /* ap mode */
    if (mode == SSV6XXX_HWM_AP)
    {   
        if (static_ip)
        {
            ipinfo info;
            
            info.ipv4 = ipaddr_addr("172.16.10.1");
            info.netmask = ipaddr_addr("255.255.255.0");
            info.gateway = ipaddr_addr("172.16.10.1");
            info.dns = ipaddr_addr("172.16.10.1");

            netmgr_ipinfo_set(WLAN_IFNAME,&info, true);
        }


        if (dhcp_server)
        {
            netmgr_dhcpd_set(true);
        }
        else
        {
            netmgr_dhcpd_set(false);
        }

        res = APmode_default();
//         res = SSV6XXX_SUCCESS;
    }

    /* smart link mode */
    if (mode == SSV6XXX_HWM_SCONFIG)
    {
        res = SCONFIGmode_default();
    }

    LOG_PRINTF("wifi start \r\n");
    return res;
}
#endif

/**
 *  Entry point of the firmware code. After system booting, this is the
 *  first function to be called from boot code. This function need to
 *  initialize the chip register, software protoctol, RTOS and create
 *  tasks. Note that, all memory resources needed for each software
 *  modulle shall be pre-allocated in initialization time.
 */
/* return int to avoid from compiler warning */

extern s32 AP_Init(u32 max_sta_num);
extern chip_def_S chip_list[];
extern chip_def_S* chip_sel;
extern void ssv_netmgr_init_netdev(bool default_cfg);
int ssv6xxx_dev_init(ssv6xxx_hw_mode hmode)
{
    ssv6xxx_result res;

#ifndef __SSV_UNIX_SIM__
    platform_ldo_en_pin_init();
    //ldo_en 0 -> 1 equal to HW reset.
    platform_ldo_en(0);
    OS_MsDelay(10);
    platform_ldo_en(1);
#endif

//	g_log_module = CONFIG_LOG_MODULE;
//	g_log_min_level = CONFIG_LOG_LEVEL;

	g_log_module = 0;
	g_log_min_level = 0;

    chip_sel = &chip_list[CONFIG_CHIP_ID];

    //This function must be assigned before netmgr_init
    #if(SMART_CONFIG_SOLUTION==CUSTOMER_SOLUTION)
    // Register customer operation function
    // ssv6xxx_user_sconfig_op.UserSconfigInit= xxx ;
    // ssv6xxx_user_sconfig_op.UserSconfigPaserData= xxx ;
    // ssv6xxx_user_sconfig_op.UserSconfigSM= xxx;
    // ssv6xxx_user_sconfig_op.UserSconfigConnect= xxx;
    // ssv6xxx_user_sconfig_op.UserSconfigDeinit= xxx;
    #else
    ssv6xxx_user_sconfig_op.UserSconfigInit=SmartConfigInit;
    ssv6xxx_user_sconfig_op.UserSconfigPaserData=SmartConfigPaserData;
    ssv6xxx_user_sconfig_op.UserSconfigSM=SmartConfigSM;
    ssv6xxx_user_sconfig_op.UserSconfigConnect=SmartConfigConnect;
    ssv6xxx_user_sconfig_op.UserSconfigDeinit=SmartConfigDeinit;
    #endif
    g_host_cfg.rate_mask= RC_DEFAULT_RATE_MSK;
    g_host_cfg.resent_fail_report= RC_DEFAULT_RESENT_REPORT;
    g_host_cfg.upgrade_per= RC_DEFAULT_UP_PF;
    g_host_cfg.downgrade_per= RC_DEFAULT_DOWN_PF;
    g_host_cfg.pre_alloc_prb_frm= RC_DEFAULT_PREPRBFRM;
    g_host_cfg.upper_fastestb= RC_DEFAULT_PER_FASTESTB;
    g_host_cfg.direct_rc_down= RC_DIRECT_DOWN;
    g_host_cfg.rc_drate_endian=RC_DEFAULT_DRATE_ENDIAN;
    g_host_cfg.tx_power_mode = CONFIG_TX_PWR_MODE;
    g_host_cfg.pool_size = POOL_SIZE;
    g_host_cfg.recv_buf_size = RECV_BUF_SIZE;
    g_host_cfg.bcn_interval = AP_BEACON_INT;
    g_host_cfg.trx_hdr_len = TRX_HDR_LEN;
    g_host_cfg.ampdu_rx_enable= AMPDU_RX_ENABLE;
    g_host_cfg.ampdu_rx_buf_size= AMPDU_RX_BUF_SIZE;
    g_host_cfg.ampdu_tx_enable= AMPDU_TX_ENABLE;
    g_host_cfg.erp = AP_ERP;
    g_host_cfg.b_short_preamble= AP_B_SHORT_PREAMBLE;
    g_host_cfg.tx_sleep = TX_TASK_SLEEP;
    g_host_cfg.tx_retry_cnt= TX_TASK_RETRY_CNT;    
    g_host_cfg.tx_res_page = TX_RESOURCE_PAGE;
    g_host_cfg.rx_res_page = RX_RESOURCE_PAGE;
    g_host_cfg.ap_rx_short_GI = AP_RX_SHORT_GI;
    g_host_cfg.ap_rx_support_legacy_rate_msk = AP_RX_SUPPORT_BASIC_RATE;
    g_host_cfg.ap_rx_support_mcs_rate_msk = AP_RX_SUPPORT_MCS_RATE;

	/**
        * On simulation/emulation platform, initialize RTOS (simulation OS)
        * first. We use this simulation RTOS to create the whole simulation
        * and emulation platform.
        */
    ASSERT( OS_Init() == OS_SUCCESS );
    host_global_init();
    ssv6xxx_init_task_para();

	LOG_init(true, true, LOG_LEVEL_ON, LOG_MODULE_MASK(LOG_MODULE_EMPTY), false);
#ifdef __SSV_UNIX_SIM__
	LOG_out_dst_open(LOG_OUT_HOST_TERM, NULL);
	LOG_out_dst_turn_on(LOG_OUT_HOST_TERM);
#endif

    ASSERT( msg_evt_init(g_host_cfg.pool_size) == OS_SUCCESS );

#if (CONFIG_USE_LWIP_PBUF==0)
//    ASSERT( PBUF_Init(POOL_SIZE) == OS_SUCCESS );
#endif//#if CONFIG_USE_LWIP_PBUF

#if (BUS_TEST_MODE == 0)
    netstack_init(NULL);
#endif
    /**
        * Initialize Host simulation platform. The Host initialization sequence
        * shall be the same as the sequence on the real host platform.
        *   @ Initialize host device drivers (SDIO/SIM/UART/SPI ...)
        */
    {

        ASSERT(ssv6xxx_drv_module_init() == true);
        LOG_PRINTF("Try to connecting CABRIO via %s...\n\r",INTERFACE);
        if (ssv6xxx_drv_select(INTERFACE) == false)
        {

            {
            LOG_PRINTF("==============================\n\r");
        	LOG_PRINTF("Please Insert %S wifi device\n",INTERFACE);
			LOG_PRINTF("==============================\n\r");
        	}
			return -1;
        }

        ASSERT(ssv_hal_init() == true);

        if(ssv6xxx_wifi_init()!=SSV6XXX_SUCCESS)
			return -1;

        {
            os_timer_init();
        }
    }

#if (BUS_TEST_MODE == 0)

    
#if (AP_MODE_ENABLE == 1)
    AP_Init(WLAN_MAX_STA);
#endif
    /**
    * Initialize TCP/IP Protocol Stack. If tcpip is init, the net_app_ask()
    * also need to be init.
    */

    
#if DHCPD_SUPPORT
    if(udhcpd_init() != 0)
    {
        LOG_PRINTF("udhcpd_init fail\r\n");
        return SSV6XXX_FAILED;
    }
#endif

#if HTTPD_SUPPORT
    httpd_init();
#endif
    if(net_app_init()== 1)
    {
        LOG_PRINTF("net_app_init fail\n\r");
        return SSV6XXX_FAILED;
    }

    /* netmgr int */
    ssv_netmgr_init(true);

    switch (hmode)
    {
        case SSV6XXX_HWM_STA: //AUTO_INIT_STATION	    
        {
            res = wifi_start(SSV6XXX_HWM_STA,false,false);
            break;
        }
        case SSV6XXX_HWM_AP:
        {
#if (AP_MODE_ENABLE == 1)
           res = wifi_start(SSV6XXX_HWM_AP,false,true);
#else
           res = SSV6XXX_FAILED;
#endif
           break;
        }
        case SSV6XXX_HWM_SCONFIG:
        {
            res = wifi_start(SSV6XXX_HWM_SCONFIG,false,true);
            break;
        }
        default:
        {
            res = wifi_start(SSV6XXX_HWM_STA,false,false);
        }
    }
#else
    res = SSV6XXX_SUCCESS;
#endif
    if (1)
    {        
#if 1
        /* we can set default ip address and default dhcpd server config before netmgr netdev init */
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
#else
        /* we can set default ip address and default dhcpd server config */

        ssv_netmgr_init_netdev(true);


#endif
    }

    Cli_Init(0, NULL);
#if (BUS_TEST_MODE == 0)
    ssv6xxx_wifi_cfg();
#if(MLME_TASK ==1)
    mlme_init();    //MLME task initial
#endif
#endif
    return res;
}


//#define SSV_IGMP_DEMO // multicast udp demo

#ifdef SSV_IGMP_DEMO

struct udp_pcb * g_upcb = NULL;
struct ip_addr ipMultiCast;
#define LISTEN_PORT       52000
#define LISTEN_ADDR       0xef0101ff   // 239.1.1.255
#define DST_PORT          52001

void ssv_test_print(const char *title, const u8 *buf,
                             size_t len)
{
    size_t i;
    LOG_PRINTF("%s - hexdump(len=%d):\r\n    ", title, len);
    if (buf == NULL) {
        LOG_PRINTF(" [NULL]");
    }else{
        for (i = 0; i < 16; i++)
            LOG_PRINTF("%02X ", i);

        LOG_PRINTF("\r\n---\r\n00|");
       for (i = 0; i < len; i++){
            LOG_PRINTF(" %02x", buf[i]);
            if((i+1)%16 ==0)
                LOG_PRINTF("\r\n%02x|", (i+1));
        }
    }
    LOG_PRINTF("\r\n-----------------------------\r\n");
}

int ssv_test_udp_tx(u8_t *data, u16_t len, u16_t port)
{
    int ret = -2;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);

    memcpy(p->payload, data, len);

    ret = udp_sendto(g_upcb, p,(struct ip_addr *) (&ipMultiCast), port);

    pbuf_free(p);

    return ret;
}

void ssv_test_udp_rx(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
    unsigned char g_tData[256];
    int ret = 0;

    //LOG_PRINTF("addr:0x%x port:%d len:%d\r\n", addr->addr, port, p->tot_len);

    //ssv_test_print("ssv_test_udp_rx", OS_FRAME_GET_DATA(p), OS_FRAME_GET_DATA_LEN(p));

    ret = udp_connect(upcb, addr, port);            /* connect to the remote host */
    if (ret != 0)
    {
        LOG_PRINTF("udp_connect: ret = %d\r\n", ret);
    }

    if (p->len >= 256) p->len = 128;

    memcpy(g_tData, p->payload, p->len);
    g_tData[p->len] = 0;
    LOG_PRINTF("rxdata: %s\r\n", g_tData);

    STRCPY((char *)g_tData, "recv it, ok!");
    ssv_test_udp_tx(g_tData, STRLEN((char *)g_tData), port);

    pbuf_free(p);   /* don't leak the pbuf! */
}


int ssv_test_udp_init(void)
{
    int ret = 1;

#if LWIP_IGMP
    ret =  netmgr_igmp_enable(true);
    if (ret != 0)
    {
        LOG_PRINTF("netmgr_igmp_enable: ret = %d\r\n", ret);
        return ret;
    }
#endif /* LWIP_IGMP */

    g_upcb = udp_new();
    if (g_upcb == NULL)
    {
        LOG_PRINTF("udp_new fail\r\n");
        return -1;
    }

    ipMultiCast.addr = lwip_htonl(LISTEN_ADDR);   // 239.1.1.255

#if LWIP_IGMP
    ret = igmp_joingroup(IP_ADDR_ANY,(struct ip_addr *) (&ipMultiCast));
    if (ret != 0)
    {
        LOG_PRINTF("igmp_joingroup: ret = %d\r\n", ret);
        return ret;
    }
#endif

    ret = udp_bind(g_upcb, IP_ADDR_ANY, LISTEN_PORT);
    if (ret != 0)
    {
        LOG_PRINTF("udp_bind: ret = %d\r\n", ret);
        return ret;
    }

    udp_recv(g_upcb, ssv_test_udp_rx, (void *)0);

    LOG_PRINTF("ssv_test_udp_init ok\r\n");

    return ret;
}
#endif

