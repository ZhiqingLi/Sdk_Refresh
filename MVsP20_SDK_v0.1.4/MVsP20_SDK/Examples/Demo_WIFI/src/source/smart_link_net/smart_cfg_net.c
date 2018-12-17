
#include "type.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmd_def.h>
#include <log.h>
#include <ssv_lib.h>
#include <os_wrapper.h>
#include <netmgr/net_mgr.h>
#include "ssv_dev.h"
#include "spi_flash.h"
#include "debug.h"
#include "sockets.h"
#include "gpio.h"
#include "timeout.h"
//#include "smart_cfg_net.h"


#define TOTAL_WIFI 10
#define SSID_LEN 40
#define PASSWORD_LEN 40


struct  AP_SSID_PWD {
    uint8_t ssid[SSID_LEN];
    uint8_t ssid_len;
    uint8_t password[PASSWORD_LEN];
    uint8_t password_len;
};

struct AP_RECORD {
    uint32_t total_items;
    uint32_t current_item;
    uint32_t flag;
    struct  AP_SSID_PWD ssid_pwd[TOTAL_WIFI];
};



#define MEDIA_RECORD_FLAG 		          0x56784321

#define AP_RECORD_FLAG 			            0x87651234

#define VOLUME_RECORD_FLAG 		          0x67891234

#define WIFIAUDIO_RECORD_FLAG	          0x78901234

#define DEVICEUSER_FLAG 	   	          0x54321023

#define WIFIAUDIO_RECORD_COLLECT_FLAG 	0x78901234

#define DEVICE_LICENSE_FLAG	          	0x78901234

#define DISK_PLAYER_FLAG				0x67891234


//extern QueueHandle_t KeyMSGQueue;
//extern QueueHandle_t PlaySoundRemindEndMSG;
extern int16_t SCONFIGmode_default(void);

//struct AP_RECORD ap_joining_record;
struct AP_RECORD *pApJoiningRecord;




int16_t smart_link_net(char sl);


uint16_t WifiStatus;
int AirkissFlag = 0;
int16_t SmartlinkStatus = 0;
TIMER GetIPWaitTimer;
void SmatlinkTask(void)
{
    WifiStatus = 0;
    vTaskDelay(100);
    smart_link_net(1);
    vTaskDelay(100);
    TimeOutSet(&GetIPWaitTimer,45*1000);
    while(netif_status_get() == 0)
    {
        if(IsTimeOut(&GetIPWaitTimer))
        {
//            SoundRemind(SOUND_LJSB,1);
//            xQueueReset(PlaySoundRemindEndMSG);
//            osMessageRecv(PlaySoundRemindEndMSG,2000);
            goto smat_link_end;
        }
        vTaskDelay(400);
        DBG("Getting ip address\n");
    }
    WifiStatus = 1;
//    SoundRemind(SOUND_LWCG,1);
//    xQueueReset(PlaySoundRemindEndMSG);
//    osMessageRecv(PlaySoundRemindEndMSG,2000);
smat_link_end:
    AirkissFlag = 1;
    SmartlinkStatus = 0;
    while(1)
    {
        vTaskDelay(100);
    }
}

uint8_t find_ap_in_wifi_list(uint8_t n, uint32_t ap_count, struct ssv6xxx_ieee80211_bss *ap_list)
{
    u32 i=0;
    uint8_t ret=0;

    if((ap_list==NULL) || (ap_count==0))
    {
        LOG_PRINTF("AP list empty!\r\n");
        return 0;
    }
    for (i=0; i<ap_count; i++)
    {
        if(ap_list[i].channel_id!= 0)
        {
            if(pApJoiningRecord->ssid_pwd[n].ssid_len == ap_list[i].ssid.ssid_len)
            {
                if(memcmp((void *)&pApJoiningRecord->ssid_pwd[n].ssid[0], (void*)ap_list[i].ssid.ssid, pApJoiningRecord->ssid_pwd[n].ssid_len) == 0)
                {
                    ret = 1;
                    break;
                }
            }
        }
    }
    return ret;
}

uint8_t scan_falg;
uint8_t sconfig_flag=0,connect_wifi_flag=0;

void sconfig_scan_done (void *data)
{
    struct resp_evt_result *sconfig_done = (struct resp_evt_result *)data;
    if(sconfig_done->u.sconfig_done.result_code==0) {
        if(pApJoiningRecord->total_items == 0) {
            pApJoiningRecord->current_item = 0;
        }
        else {
            pApJoiningRecord->current_item  += 1;
            pApJoiningRecord->current_item %= TOTAL_WIFI;
        }

        if(pApJoiningRecord->total_items < TOTAL_WIFI)
        {
            pApJoiningRecord->total_items++;
        }
        memset(pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].ssid,0,SSID_LEN);
        memcpy((void*)pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].ssid, (void*)sconfig_done->u.sconfig_done.ssid, sconfig_done->u.sconfig_done.ssid_len);
        pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].ssid_len = sconfig_done->u.sconfig_done.ssid_len;
        memset(pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].password,0,PASSWORD_LEN);
        sprintf(pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].password,"%s",sconfig_done->u.sconfig_done.pwd);
        pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].password_len =strlen(pApJoiningRecord->ssid_pwd[pApJoiningRecord->current_item].password);
        sconfig_flag = 1;
    }
    return;
}

bool FlashUnlock(void);
void cmd_wifi_list(void);

#define CONNECT_READ            1
#define CONNECT_SCAN  		      2
#define CONNECT_JOIN            3
#define CONNECT_SMARTLINK       4
#define CONNECT_BURN            5
#define CONNECT_WAIT_SCAN       6
#define CONNECT_WIFI_LIST       7
#define CONNECT_WIFI_WAIT       8
#define CONNECT_SMARTLINK_WAIT0  9
#define CONNECT_SMARTLINK_WAIT1  10



TIMER SmartLinkTimer;
int16_t smart_link_net(char sl)
{
    uint8_t i,item, *buf;
    int32_t ret = -1;
    wifi_sta_join_cfg *join_cfg = NULL;
    uint32_t ap_count;
    struct ssv6xxx_ieee80211_bss *ap_list = NULL;
    uint8_t ConnectStage;

    ConnectStage = CONNECT_READ;

    while(1)
    {
        switch(ConnectStage)
        {
        case CONNECT_READ:
            //ReadNetRecord();
            if((pApJoiningRecord->total_items == 0)||(pApJoiningRecord->total_items > 10)||(pApJoiningRecord->flag != AP_RECORD_FLAG)||(sl == 1)) {
                if(pApJoiningRecord->flag != AP_RECORD_FLAG)
                {
                    pApJoiningRecord->total_items = 0;
                    pApJoiningRecord->current_item = 0;
                }
                ConnectStage = CONNECT_SMARTLINK;
            }
            else
            {
                join_cfg = (wifi_sta_join_cfg *)MALLOC(sizeof(wifi_sta_join_cfg));
                memset(join_cfg,0,sizeof(wifi_sta_join_cfg));
                ConnectStage = CONNECT_SCAN;
            }
            CONNECT_SMARTLINK;
            break;

        case CONNECT_SCAN:
            scan_falg = 0;
            swtich2STAmode();
            vTaskDelay(1000);
            cmd_wifi_scan();
//            SoundRemind(SOUND_SSWL, 1);
            ConnectStage = CONNECT_WAIT_SCAN;
            break;

        case CONNECT_WAIT_SCAN:
            if(scan_falg == 0)
            {
                vTaskDelay(1000);
                printf("Wait for scan\n");
            }
            else if(scan_falg == 1)
            {
                printf("Scan success\n");
                ConnectStage = CONNECT_WIFI_LIST;
            }
            else if(scan_falg == 2)
            {
                printf("Scan fail\n");
                ConnectStage = CONNECT_SCAN;
            }
            break;

        case CONNECT_WIFI_LIST:
            vTaskDelay(1500);//¹Ø¼üdelay
            cmd_wifi_list();
            ap_count = ssv6xxx_get_aplist_info((void *)&ap_list);
            ConnectStage = CONNECT_JOIN;
            i = 0;
            break;

        case CONNECT_JOIN:
            if(i >= pApJoiningRecord->total_items)
            {
                FREE((void *)ap_list);
                FREE(join_cfg);
                ConnectStage = CONNECT_SMARTLINK;
                break;

            }

            item = (i+pApJoiningRecord->current_item) % pApJoiningRecord->total_items;
            if(find_ap_in_wifi_list(item, ap_count, ap_list) == 1)
            {
                memcpy(join_cfg->ssid.ssid, (void *)&pApJoiningRecord->ssid_pwd[item].ssid[0],pApJoiningRecord->ssid_pwd[item].ssid_len);
                join_cfg->ssid.ssid_len = pApJoiningRecord->ssid_pwd[item].ssid_len;
                memcpy(join_cfg->password, (void *)&pApJoiningRecord->ssid_pwd[item].password, pApJoiningRecord->ssid_pwd[item].password_len);
                ret = netmgr_wifi_join_async(join_cfg);
                if (ret == 0)
                {
                    connect_wifi_flag = 0;
                    ConnectStage = CONNECT_WIFI_WAIT;
                }
            }
            i++;

            break;
        case CONNECT_WIFI_WAIT:
            if(connect_wifi_flag == 0)
            {
                vTaskDelay(1000);
                printf("Wait for connect wifi\n");
            }
            else if(connect_wifi_flag == 1)
            {
                printf("Connect wifi success\n");

                FREE((void *)ap_list);
                FREE(join_cfg);
                if(item == pApJoiningRecord->current_item)
                {
                    return 1;
                }
                else
                {
                    pApJoiningRecord->current_item = item;
                    ConnectStage = CONNECT_BURN;
                }
            }
            else if(connect_wifi_flag == 2)
            {
                printf("Connect wifi fail\n");
                vTaskDelay(1000);
                ConnectStage = CONNECT_JOIN;
                i++;
            }
            break;

        case CONNECT_SMARTLINK:
            vTaskDelay(1000);
            connect_wifi_flag = 0;
            sconfig_flag = 0;
            if(pApJoiningRecord->total_items > TOTAL_WIFI)
                memset(pApJoiningRecord, 0, sizeof(struct AP_RECORD));

            if(SCONFIGmode_default() == 1)
            {
            	DBG("SCONFIG mode success\n");
                ConnectStage = CONNECT_SMARTLINK_WAIT0;
            }
            else
            {
                ConnectStage = CONNECT_SMARTLINK;
            }
            break;

        case CONNECT_SMARTLINK_WAIT0:
            if(sconfig_flag == 0)
            {
                //SetLed5(1);
                vTaskDelay(500);
                //SetLed5(0);
                vTaskDelay(500);
                DBG("Wait for smartlink\n");
            }
            else if(sconfig_flag == 1)
            {
                ConnectStage = CONNECT_SMARTLINK_WAIT1;
                DBG("Smatlink success\n");
            }
            else if(sconfig_flag == 2)
            {
            	DBG("Smartlink fail\n");
                ConnectStage = CONNECT_SMARTLINK;
            }
            break;

        case CONNECT_SMARTLINK_WAIT1:
            if(connect_wifi_flag == 0)
            {
                vTaskDelay(500);
                DBG("Wait for connect wifi\n");
            }
            else if(connect_wifi_flag == 1)
            {
            	DBG("Connect wifi success\n");
                ConnectStage = CONNECT_BURN;
            }
            else if(connect_wifi_flag == 2)
            {
            	DBG("Connect wifi fail\n");
                ConnectStage = CONNECT_SMARTLINK;
            }
            break;

        case CONNECT_BURN:
            //BurnNetRecord();
            return 1;
            break;

        default:
            break;

        }
    }
}



