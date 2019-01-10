
//#define __BQB_PROFILE_TEST__
#ifdef __BQB_PROFILE_TEST__
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_cmu.h"
#include "analog.h"
#include "bt_drv.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"

#include "rtos.h"

#include "cqueue.h"
#include "apps.h"
#include "tgt_hardware.h"

#include "stdio.h"
#include "cmsis_os.h"
#include "string.h"

#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"

#include "audioflinger.h"
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_utils.h"
#include "app_status_ind.h"
#ifdef __FACTORY_MODE_SUPPORT__
#include "app_factory.h"
#include "app_factory_bt.h"
#endif
#include "bt_drv_interface.h"
#include "besbt.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"

extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "btalloc.h"
#include "hid.h"
}
#include "btapp.h"
#include "app_bt.h"

#ifdef MEDIA_PLAYER_SUPPORT
#include "resources.h"
#include "app_media_player.h"
#endif
#include "app_bt_media_manager.h"
#include "hal_sleep.h"

extern struct BT_DEVICE_T  app_bt_device;
void A2dp_pts_Set_Sink_Delay(APP_KEY_STATUS *status, void *param)
{
    TRACE("!!!A2dp_pts_Set_Sink_Delay\n");
    A2DP_SetSinkDelay(&app_bt_device.a2dp_stream[BT_DEVICE_ID_1],10);
}

void A2dp_pts_Create_Avdtp_Signal_Channel(APP_KEY_STATUS *status, void *param)
{
    TRACE("!!!A2dp_pts_Create_Avdtp_Signal_Channel\n");
    BT_BD_ADDR   bdAddr;

    //PTS addr:13 71 da 7d 1a 0 
    bdAddr.addr[0] = 0x13;
    bdAddr.addr[1] = 0x71;
    bdAddr.addr[2] = 0xda;
    bdAddr.addr[3] = 0x7d;
    bdAddr.addr[4] = 0x1a;
    bdAddr.addr[5] = 0x00;
    A2DP_OpenStream(&app_bt_device.a2dp_stream[BT_DEVICE_ID_1], &bdAddr);
}

void Hfp_pts_create_service_level_channel(APP_KEY_STATUS *status, void *param)
{
    TRACE("!!!Hfp_pts_create_service_level_channel\n");
    BT_BD_ADDR   bdAddr;

    //PTS addr:13 71 da 7d 1a 0 
    bdAddr.addr[0] = 0x13;
    bdAddr.addr[1] = 0x71;
    bdAddr.addr[2] = 0xda;
    bdAddr.addr[3] = 0x7d;
    bdAddr.addr[4] = 0x1a;
    bdAddr.addr[5] = 0x00;
    app_bt_HF_CreateServiceLink(&app_bt_device.hf_channel[BT_DEVICE_ID_1],&bdAddr);
}


typedef struct {
    AvrcpAdvancedPdu pdu;
    uint8_t para_buf[10];
}APP_A2DP_AVRCPADVANCEDPDU;
extern osPoolId   app_a2dp_avrcpadvancedpdu_mempool;
#define app_a2dp_avrcpadvancedpdu_mempool_calloc(buf)  do{ \
                                                        APP_A2DP_AVRCPADVANCEDPDU * avrcpadvancedpdu; \
                                                        avrcpadvancedpdu = (APP_A2DP_AVRCPADVANCEDPDU *)osPoolCAlloc(app_a2dp_avrcpadvancedpdu_mempool); \
                                                        buf = &(avrcpadvancedpdu->pdu); \
                                                        buf->parms = avrcpadvancedpdu->para_buf; \
                                                     }while(0);
void Avrcp_pts_volume_change_notify(APP_KEY_STATUS *status, void *param)
{
    TRACE("!!!Avrcp_pts_volume_change_notify\n");
    if (app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1] == NULL)
        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1]);
    AVRCP_CtRegisterNotification(&app_bt_device.avrcp_channel[BT_DEVICE_ID_1],app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1],AVRCP_EID_VOLUME_CHANGED,0);
}          

//extern int a2dp_volume_get(void);
void Avrcp_pts_set_absolute_volume(APP_KEY_STATUS *status, void *param)
{
    TRACE("app_avrcp_set_absolute_volume\n");
    if (app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1] == NULL)
        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1]);
    
    int vol = app_bt_stream_volume_get_ptr()->a2dp_vol;
    vol = 8*vol-1; 
    if (vol > (0x7f-1))
        vol = 0x7f;

    AVRCP_CtSetAbsoluteVolume(&app_bt_device.avrcp_channel[BT_DEVICE_ID_1],app_bt_device.avrcp_notify_rsp[BT_DEVICE_ID_1],vol);
}

#endif
