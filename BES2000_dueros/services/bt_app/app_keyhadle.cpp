//#include "mbed.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "analog.h"

extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "hs.h"
#include "hid.h"
#include "mei.h"
}

#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "app_key.h"
#include "app_audio.h"

#include "apps.h"
#include "app_bt_stream.h"
#include "app_bt_media_manager.h"
#include "app_bt.h"
#include "app_bt_func.h"
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
#include "nvrecord_env.h"
#endif

extern struct BT_DEVICE_T  app_bt_device;
//BT_DEVICE_ID_T g_current_device_id=BT_DEVICE_NUM;  //used to change sco by one-bring-two 
//BT_DEVICE_ID_T g_another_device_id=BT_DEVICE_NUM;  //used to change sco by one-bring-two 


#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
extern int app_hfp_siri_report();
extern int app_hfp_siri_voice(bool en);
int open_siri_flag = 0;
void bt_key_handle_siri_key(enum APP_KEY_EVENT_T event)
{
     switch(event)
     {
        case  APP_KEY_EVENT_NONE:
            if(open_siri_flag == 1){
                TRACE("open siri");
                app_hfp_siri_voice(true);
                open_siri_flag = 0;
            } /*else {
                TRACE("evnet none close siri");
                app_hfp_siri_voice(false);
            }*/
            break;
        case  APP_KEY_EVENT_LONGLONGPRESS:
        case  APP_KEY_EVENT_UP:
            //TRACE("long long/up/click event close siri");
            //app_hfp_siri_voice(false);
            break;
        default:
            TRACE("unregister down key event=%x",event);
            break;
        }
}

#endif

//HfCommand hf_command;
//bool hf_mute_flag = 0;
extern int app_hfp_hfcommand_mempool_calloc(HfCommand **hf_cmd_p);
extern int app_hfp_hfcommand_mempool_free(HfCommand *hf_cmd_p);

#if defined (__HSP_ENABLE__)

extern XaStatus app_hs_handle_cmd(HsChannel *Chan,uint8_t cmd_type);

void hsp_handle_key(uint8_t hsp_key)
{
    HsCommand *hs_cmd_p;
    HsChannel *hs_channel_tmp = NULL;
    uint8_t ret= 0;
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T another_device_id =  (app_bt_device.curr_hs_channel_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
    TRACE("!!!hsp_handle_key curr_hf_channel=%d\n",app_bt_device.curr_hs_channel_id);
    hs_channel_tmp = (app_bt_device.curr_hs_channel_id == BT_DEVICE_ID_1) ? &(app_bt_device.hs_channel[BT_DEVICE_ID_1]) : &(app_bt_device.hs_channel[BT_DEVICE_ID_2]);
#else
    hs_channel_tmp = &(app_bt_device.hs_channel[BT_DEVICE_ID_1]);
#endif	

    if(hsp_key == HSP_KEY_CKPD_CONTROL)
    {
	TRACE("hsp_key = HSP_KEY_CKPD_CONTROL");			 
	if(app_hs_handle_cmd(hs_channel_tmp,APP_CPKD_CMD) !=0)
            TRACE("app_hs_handle_cmd err");

    }
    else if(hsp_key == HSP_KEY_CHANGE_TO_PHONE)
    {
           TRACE("hsp_key = HSP_KEY_CHANGE_TO_PHONE");
	HS_DisconnectAudioLink(hs_channel_tmp);	
    }
    else if(hsp_key == HSP_KEY_ADD_TO_EARPHONE)
    {
        TRACE("hsp_key = HSP_KEY_ADD_TO_EARPHONE");
	HS_CreateAudioLink(hs_channel_tmp);	
    }
    

}
#endif

void hfp_handle_key(uint8_t hfp_key)
{
    HfCommand *hf_cmd_p;
    HfChannel *hf_channel_tmp = NULL;
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T another_device_id =  (app_bt_device.curr_hf_channel_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
    TRACE("!!!hfp_handle_key curr_hf_channel=%d\n",app_bt_device.curr_hf_channel_id);
    hf_channel_tmp = (app_bt_device.curr_hf_channel_id == BT_DEVICE_ID_1) ? &(app_bt_device.hf_channel[BT_DEVICE_ID_1]) : &(app_bt_device.hf_channel[BT_DEVICE_ID_2]);
#else
    hf_channel_tmp = &(app_bt_device.hf_channel[BT_DEVICE_ID_1]);
#endif
    switch(hfp_key)
    {
        case HFP_KEY_ANSWER_CALL:
            ///answer a incomming call
            TRACE("avrcp_key = HFP_KEY_ANSWER_CALL\n");            
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);
            if (hf_cmd_p){
                if (HF_AnswerCall(hf_channel_tmp,hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_HANGUP_CALL:
            TRACE("avrcp_key = HFP_KEY_HANGUP_CALL\n");            
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);
            if (hf_cmd_p){
                if (HF_Hangup(hf_channel_tmp,hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_REDIAL_LAST_CALL:
            ///redail the last call
            TRACE("avrcp_key = HFP_KEY_REDIAL_LAST_CALL\n");
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_Redial(hf_channel_tmp,hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_CHANGE_TO_PHONE:
            ///remove sco and voice change to phone
            TRACE("avrcp_key = HFP_KEY_CHANGE_TO_PHONE\n");
            app_bt_HF_DisconnectAudioLink(hf_channel_tmp);
            break;
        case HFP_KEY_ADD_TO_EARPHONE:
            ///add a sco and voice change to earphone
            TRACE("avrcp_key = HFP_KEY_ADD_TO_EARPHONE\n");
            app_bt_HF_CreateAudioLink(hf_channel_tmp);
            break;
        case HFP_KEY_MUTE:
            TRACE("avrcp_key = HFP_KEY_MUTE\n");
            app_bt_device.hf_mute_flag = 1;
            break;
        case HFP_KEY_CLEAR_MUTE:
            TRACE("avrcp_key = HFP_KEY_CLEAR_MUTE\n");
            app_bt_device.hf_mute_flag = 0;
            break;
        case HFP_KEY_THREEWAY_HOLD_AND_ANSWER:
            TRACE("avrcp_key = HFP_KEY_THREEWAY_HOLD_AND_ANSWER\n");            
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_CallHold(hf_channel_tmp,HF_HOLD_HOLD_ACTIVE_CALLS,0,hf_cmd_p) != BT_STATUS_PENDING)                
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_THREEWAY_HANGUP_AND_ANSWER:
            TRACE("avrcp_key = HFP_KEY_THREEWAY_HOLD_SWAP_ANSWER\n");
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_CallHold(hf_channel_tmp,HF_HOLD_RELEASE_ACTIVE_CALLS,0,hf_cmd_p) != BT_STATUS_PENDING)                
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_THREEWAY_HOLD_REL_INCOMING:
            TRACE("avrcp_key = HFP_KEY_THREEWAY_HOLD_REL_INCOMING\n");
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_CallHold(hf_channel_tmp,HF_HOLD_RELEASE_HELD_CALLS,0,hf_cmd_p) != BT_STATUS_PENDING)
					app_hfp_hfcommand_mempool_free(hf_cmd_p);
			}
            break;
#ifdef __BT_ONE_BRING_TWO__
        case HFP_KEY_DUAL_HF_HANGUP_ANOTHER:////////double click
            TRACE("avrcp_key = HFP_KEY_DUAL_HF_HANGUP_ANOTHER\n");
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_Hangup(&app_bt_device.hf_channel[another_device_id],hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER://////////click
            TRACE("avrcp_key = HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER\n");
//            HF_Hangup(&app_bt_device.hf_channel[another_device_id],&app_bt_device.hf_command[another_device_id]);
//            HF_AnswerCall(&app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id],&app_bt_device.hf_command[app_bt_device.curr_hf_channel_id]);
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);
            if (hf_cmd_p){
                if (HF_Hangup(&app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id],hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_AnswerCall(&app_bt_device.hf_channel[another_device_id],hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
        case HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER:///////////long press
            TRACE("avrcp_key = HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER\n");
            //HF_AnswerCall(&app_bt_device.hf_channel[app_bt_device.curr_hf_channel_id],&app_bt_device.hf_command[BT_DEVICE_ID_1]);
            app_hfp_hfcommand_mempool_calloc(&hf_cmd_p);            
            if (hf_cmd_p){
                if (HF_AnswerCall(&app_bt_device.hf_channel[another_device_id],hf_cmd_p) != BT_STATUS_PENDING)
                    app_hfp_hfcommand_mempool_free(hf_cmd_p);
            }
            break;
#endif

        default :
            break;
    }
}

//bool a2dp_play_pause_flag = 0;
void a2dp_handleKey(uint8_t a2dp_key)
{
    AvrcpChannel *avrcp_channel_tmp = NULL;
    if(app_bt_device.a2dp_state[app_bt_device.curr_a2dp_stream_id] == 0)
        return;

#ifdef __BT_ONE_BRING_TWO__
    TRACE("!!!a2dp_handleKey curr_a2dp_stream_id=%d\n",app_bt_device.curr_a2dp_stream_id);
    avrcp_channel_tmp = (app_bt_device.curr_a2dp_stream_id == BT_DEVICE_ID_1) ? &(app_bt_device.avrcp_channel[BT_DEVICE_ID_1]) : &(app_bt_device.avrcp_channel[BT_DEVICE_ID_2]);
#else
    avrcp_channel_tmp = &app_bt_device.avrcp_channel[BT_DEVICE_ID_1];
#endif
    switch(a2dp_key)
    {
        case AVRCP_KEY_STOP:
            TRACE("avrcp_key = AVRCP_KEY_STOP");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_STOP,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_STOP,FALSE);
            app_bt_device.a2dp_play_pause_flag = 0;
            break;
        case AVRCP_KEY_PLAY:
            TRACE("avrcp_key = AVRCP_KEY_PLAY");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_PLAY,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_PLAY,FALSE);
            app_bt_device.a2dp_play_pause_flag = 1;
            break;
        case AVRCP_KEY_PAUSE:
            TRACE("avrcp_key = AVRCP_KEY_PAUSE");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_PAUSE,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_PAUSE,FALSE);
            app_bt_device.a2dp_play_pause_flag = 0;
            break;
        case AVRCP_KEY_FORWARD:
            TRACE("avrcp_key = AVRCP_KEY_FORWARD");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_FORWARD,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_FORWARD,FALSE);
            app_bt_device.a2dp_play_pause_flag = 1;
            break;
        case AVRCP_KEY_BACKWARD:
            TRACE("avrcp_key = AVRCP_KEY_BACKWARD");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_BACKWARD,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_BACKWARD,FALSE);
            app_bt_device.a2dp_play_pause_flag = 1;
            break;
        case AVRCP_KEY_VOLUME_UP:
            TRACE("avrcp_key = AVRCP_KEY_VOLUME_UP");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_VOLUME_UP,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_VOLUME_UP,FALSE);
            break;
        case AVRCP_KEY_VOLUME_DOWN:
            TRACE("avrcp_key = AVRCP_KEY_VOLUME_DOWN");
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_VOLUME_DOWN,TRUE);
            AVRCP_SetPanelKey(avrcp_channel_tmp,AVRCP_POP_VOLUME_DOWN,FALSE);
            break;
        default :
            break;
    }
}

//uint8_t phone_earphone_mark = 0;

#if 0
//uint8_t phone_earphone_mark = 0;
uint8_t iic_mask=0;
extern "C" void pmu_sleep_en(unsigned char sleep_en);

void btapp_change_to_iic(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_iic[] = {
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_I2C_SCL, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_I2C_SDA, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };

    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)pinmux_iic, sizeof(pinmux_iic)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
}

void btapp_change_to_uart0(void)
{
    const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_uart0[] = {
        {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_UART0_RX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
        {HAL_IOMUX_PIN_P3_1, HAL_IOMUX_FUNC_UART0_TX, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    };


    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)pinmux_uart0, sizeof(pinmux_uart0)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
}
#endif

#ifdef __BT_ONE_BRING_TWO__
#define HF_CHANNEL_SWITCH_HOOK_USER APP_BT_GOLBAL_HANDLE_HOOK_USER_0

enum HF_CHANNEL_SWITCH_STATUS{
    HF_CHANNEL_SWITCH_STATUS_IDLE,
    HF_CHANNEL_SWITCH_STATUS_ONPROCESS,
};

struct HF_CHANNEL_SWITCH_ENV_T{
    BT_DEVICE_ID_T old_device_id;
    HfChannel *hf_channel_old;
    BT_DEVICE_ID_T new_device_id;
    HfChannel *hf_channel_new;    
    enum HF_CHANNEL_SWITCH_STATUS status;
}hf_channel_switch_env;

static void bt_key_hf_channel_switch_hook(const BtEvent *Event);

static void bt_key_hf_channel_switch_stop(void)
{
    hf_channel_switch_env.status = HF_CHANNEL_SWITCH_STATUS_IDLE;
    hf_channel_switch_env.old_device_id = BT_DEVICE_NUM;
    hf_channel_switch_env.hf_channel_old = NULL;
    hf_channel_switch_env.new_device_id = BT_DEVICE_NUM;
    hf_channel_switch_env.hf_channel_new  = NULL;
    app_bt_golbal_handle_hook_set(HF_CHANNEL_SWITCH_HOOK_USER, NULL);
}

#ifdef __HF_KEEP_ONE_ALIVE__
static void bt_key_hf_channel_switch_active(BT_DEVICE_ID_T current_id, BT_DEVICE_ID_T another_id)
{
    TRACE("switch_active %d-->%d", current_id, another_id);
    
    hf_channel_switch_env.old_device_id = current_id;
    hf_channel_switch_env.hf_channel_old = &(app_bt_device.hf_channel[current_id]);
    
    hf_channel_switch_env.new_device_id = another_id;
    hf_channel_switch_env.hf_channel_new = &(app_bt_device.hf_channel[another_id]);

    app_bt_HF_DisconnectAudioLink(hf_channel_switch_env.hf_channel_old);
    app_bt_HF_CreateAudioLink(hf_channel_switch_env.hf_channel_new);
}
#endif

static void bt_key_hf_channel_switch_start(void)
{
    uint8_t i;

    TRACE("%s status:%d enter",__func__, hf_channel_switch_env.status );
    if (hf_channel_switch_env.status == HF_CHANNEL_SWITCH_STATUS_ONPROCESS){
        return;
    }

    for (i=0; i<BT_DEVICE_NUM; i++){
        if (app_bt_device.hf_voice_en[i] == HF_VOICE_ENABLE){
            break;
        }
    }

    if (i < BT_DEVICE_NUM){        
        TRACE("%s switch to %d",__func__, i);
        hf_channel_switch_env.status = HF_CHANNEL_SWITCH_STATUS_ONPROCESS;

        app_audio_manager_set_active_sco_num((enum BT_DEVICE_ID_T)i);

        hf_channel_switch_env.old_device_id = (enum BT_DEVICE_ID_T)i;
        hf_channel_switch_env.hf_channel_old = &(app_bt_device.hf_channel[i]);
        
        hf_channel_switch_env.new_device_id = i==BT_DEVICE_ID_1 ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
        hf_channel_switch_env.hf_channel_new = &(app_bt_device.hf_channel[hf_channel_switch_env.new_device_id]);
        app_bt_HF_DisconnectAudioLink(hf_channel_switch_env.hf_channel_old);
        app_bt_golbal_handle_hook_set(HF_CHANNEL_SWITCH_HOOK_USER, bt_key_hf_channel_switch_hook);
    }else{
        hf_channel_switch_env.status = HF_CHANNEL_SWITCH_STATUS_IDLE;        
        TRACE("%s not found",__func__);
    }
}

static void bt_key_hf_channel_switch_init(void)
{
    memset(&hf_channel_switch_env, 0, sizeof(struct HF_CHANNEL_SWITCH_ENV_T));
}

static void bt_key_hf_channel_switch_hook(const BtEvent *Event)
{
    switch (Event->eType) {
        case BTEVENT_LINK_DISCONNECT:
            TRACE("%s DISCONNECT",__func__);
            bt_key_hf_channel_switch_stop();
            break;
        case BTEVENT_SCO_CONNECT_IND:
        case BTEVENT_SCO_CONNECT_CNF:            
            TRACE("%s connect another ok",__func__);
            if ((CMGR_GetRemoteDevice(&hf_channel_switch_env.hf_channel_new->cmgrHandler) == Event->p.remDev)||
                (app_bt_device.hf_voice_en[hf_channel_switch_env.new_device_id] == HF_VOICE_ENABLE)){
                app_bt_device.curr_hf_channel_id = hf_channel_switch_env.new_device_id;
                app_bt_device.hf_voice_en[app_bt_device.curr_hf_channel_id] = HF_VOICE_ENABLE;            
                app_audio_manager_set_active_sco_num(app_bt_device.curr_hf_channel_id);
                bt_key_hf_channel_switch_stop();
            }
            break;
        case BTEVENT_SCO_DISCONNECT:            
            TRACE("%s try connect another",__func__);
            app_audio_manager_set_active_sco_num(BT_DEVICE_NUM);
            app_bt_device.hf_voice_en[hf_channel_switch_env.old_device_id] = HF_VOICE_DISABLE;
            if (app_bt_device.hf_voice_en[hf_channel_switch_env.new_device_id] == HF_VOICE_DISABLE){
                app_bt_HF_CreateAudioLink(hf_channel_switch_env.hf_channel_new);
            }else{
                bt_key_hf_channel_switch_stop();
            }
            break;
    }
}
#endif
BtStatus LinkDisconnectDirectly(void);

void bt_key_handle_func_key(enum APP_KEY_EVENT_T event)
{
#ifdef __BT_ONE_BRING_TWO__
    //if(g_current_device_id == BT_DEVICE_NUM)
    BT_DEVICE_ID_T current_device_id;
    BT_DEVICE_ID_T another_device_id;

    current_device_id = app_bt_device.curr_hf_channel_id;
    another_device_id = (current_device_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
#endif

    switch(event)
    {
#ifndef __BT_ONE_BRING_TWO__
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
#if 0            
            if(iic_mask ==0)
            {
                iic_mask=1;
                btapp_change_to_iic();
                
            }
            else if(iic_mask == 1)
            {
                pmu_sleep_en(0);
                iic_mask = 2;
            }
            else
            {
                iic_mask=0;
                btapp_change_to_uart0();
            }
#endif            
#if defined (__HSP_ENABLE__)
            if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&
                    (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&
                    (app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == HF_AUDIO_DISCON)&&
                    (app_bt_device.hs_conn_flag[app_bt_device.curr_hs_channel_id]  != 1)){
#else
             if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&
                    (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&
                    (app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == HF_AUDIO_DISCON)){
#endif
                if(app_bt_device.a2dp_play_pause_flag == 0){   /////if no connect?
                    a2dp_handleKey(AVRCP_KEY_PLAY);
                }else{
                    a2dp_handleKey(AVRCP_KEY_PAUSE);
                }
            }
            if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)){
                hfp_handle_key(HFP_KEY_ANSWER_CALL);
            }
            if(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE){
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }
            if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_OUT)||(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_ALERT)){
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }
#if defined (__HSP_ENABLE__)            
            if(app_bt_device.hs_conn_flag[app_bt_device.curr_hs_channel_id]  == 1){         //now we know it is HSP active !
                    hsp_handle_key(HSP_KEY_CKPD_CONTROL);
            }
#endif            
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            //TRACE("powerkey close siri"); 
            //app_hfp_siri_voice(false);
            open_siri_flag = 0;
#endif
            break;
        case  APP_KEY_EVENT_DOUBLECLICK:
            TRACE("!!!APP_KEY_EVENT_DOUBLECLICK\n");
            #if 1
            LinkDisconnectDirectly();
           #else
            if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)){
                hfp_handle_key(HFP_KEY_REDIAL_LAST_CALL);
            }
            if(app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == HF_AUDIO_CON){
                if(app_bt_device.hf_mute_flag == 0){
                    hfp_handle_key(HFP_KEY_MUTE);
                    app_bt_device.hf_mute_flag = 1;
                }else{
                    hfp_handle_key(HFP_KEY_CLEAR_MUTE);
                    app_bt_device.hf_mute_flag = 0;
                }
            }
            #endif
            break;
        case  APP_KEY_EVENT_LONGPRESS:

#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            open_siri_flag=0;
#endif
            if((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE) &&
                ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN))) {
#ifndef FPGA
                app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
#endif
                hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
            } else  if(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE){
                if(app_bt_device.phone_earphone_mark == 0){
                    //call is active, switch from earphone to phone
                    hfp_handle_key(HFP_KEY_CHANGE_TO_PHONE);
                }else if(app_bt_device.phone_earphone_mark == 1){
                    //call is active, switch from phone to earphone
                    hfp_handle_key(HFP_KEY_ADD_TO_EARPHONE);
                }
            } else if(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN){
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }
#if defined (__HSP_ENABLE__)
           else if(app_bt_device.hs_conn_flag[app_bt_device.curr_hs_channel_id]  == 1){         //now we know it is HSP active !
                    if(app_bt_device.phone_earphone_mark == 0){
                    //call is active, switch from earphone to phone
                    hsp_handle_key(HSP_KEY_CHANGE_TO_PHONE);
                }else if(app_bt_device.phone_earphone_mark == 1){
                    //call is active, switch from phone to earphone
                    hsp_handle_key(HSP_KEY_ADD_TO_EARPHONE);
                }
            } 
#endif

#if HID_DEVICE == XA_ENABLED
            else if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)){
                Hid_Send_capture(&app_bt_device.hid_channel[BT_DEVICE_ID_1]);
            }
#endif

#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            else if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE) && 
                    (app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE) && 
                    (open_siri_flag == 0 )){
                    app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
                    open_siri_flag = 1;
                    }
#endif            
            break;
#else
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
#if 0            
            if(iic_mask ==0)
            {
                iic_mask=1;
                btapp_change_to_iic();
                
            }
            else
            {
                iic_mask=0;
                btapp_change_to_uart0();
            }         
#endif            
            TRACE("!!!APP_KEY_EVENT_CLICK  callSetup1&2:%d,%d, call1&2:%d,%d, audio_state1&2:%d,%d\n",app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1],app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2],
                                                                                                      app_bt_device.hfchan_call[BT_DEVICE_ID_1],app_bt_device.hfchan_call[BT_DEVICE_ID_2],
                                                                                                      app_bt_device.hf_audio_state[BT_DEVICE_ID_1],app_bt_device.hf_audio_state[BT_DEVICE_ID_2]);
            if((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == HF_AUDIO_DISCON)&&
                (app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)&&(app_bt_device.hf_audio_state[BT_DEVICE_ID_2] == HF_AUDIO_DISCON)){
                if(app_bt_device.a2dp_play_pause_flag == 0){
                    a2dp_handleKey(AVRCP_KEY_PLAY);
                }else{
                    a2dp_handleKey(AVRCP_KEY_PAUSE);
                }
            }

            if(((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] != HF_CALL_SETUP_ALERT))||
                ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_IN)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] != HF_CALL_SETUP_ALERT))){
                hfp_handle_key(HFP_KEY_ANSWER_CALL);
            }

            if((((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_OUT)||(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_ALERT))&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE))||
                (((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_OUT)||(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_ALERT))&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE))){
                TRACE("!!!!call out hangup\n");
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }

            if(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE){
                if((app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)){
                    hfp_handle_key(HFP_KEY_HANGUP_CALL);
                }else if(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_IN){
                    hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
                }
            }else if(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE){
                if((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)){
                    hfp_handle_key(HFP_KEY_HANGUP_CALL);
                }else if(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN){
                    hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER);
                }
            }

            if((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE)){
                TRACE("!!!two call both active\n");
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            //TRACE("powerkey close siri");
            //app_hfp_siri_voice(false);
            open_siri_flag = 0;
#endif

            break;
        case  APP_KEY_EVENT_DOUBLECLICK:

            if(((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hf_channel[BT_DEVICE_ID_2].state == HF_STATE_CLOSED))||
                ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE))){
                hfp_handle_key(HFP_KEY_REDIAL_LAST_CALL);
            }

            if(((app_bt_device.hf_audio_state[BT_DEVICE_ID_1] == HF_AUDIO_CON)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE))||
                ((app_bt_device.hf_audio_state[BT_DEVICE_ID_2] == HF_AUDIO_CON)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE))){
                if(app_bt_device.hf_mute_flag == 0){
                    hfp_handle_key(HFP_KEY_MUTE);
                }else{
                    hfp_handle_key(HFP_KEY_CLEAR_MUTE);
                }
            }

            if(((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_IN))||
                ((app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN))){
                hfp_handle_key(HFP_KEY_DUAL_HF_HANGUP_ANOTHER);
            }

            break;
        case  APP_KEY_EVENT_LONGPRESS:
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            open_siri_flag=0;
#endif


            
//one bring two: long press switch sco
            TRACE("app_bt_device.curr_hf_channel_id=%d, g_curr=%d curr=%d another:%d scoHciHandle:%x/%x",  app_bt_device.curr_hf_channel_id,current_device_id,
                                                                                  current_device_id, another_device_id,
                                                                                  app_bt_device.hf_channel[current_device_id].cmgrHandler.scoConnect->scoHciHandle
                                                                                  ,app_bt_device.hf_channel[another_device_id].cmgrHandler.scoConnect->scoHciHandle);
              if((app_bt_device.hfchan_call[current_device_id] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_call[another_device_id] != HF_CALL_ACTIVE)
                &&(app_bt_device.hfchan_callSetup[another_device_id] == HF_CALL_SETUP_IN)){//A is active, B is incoming call
                TRACE("HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER: current=%d, g_current_device_id=%d",app_bt_device.curr_hf_channel_id, current_device_id);
#ifndef FPGA
                app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
#endif
                hfp_handle_key(HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER);//hold and answer
                app_bt_device.curr_hf_channel_id = another_device_id;
            } else if((app_bt_device.hfchan_call[current_device_id] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_call[another_device_id] == HF_CALL_ACTIVE)
                &&(app_bt_device.hfchan_callSetup[current_device_id] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_callSetup[another_device_id] == HF_CALL_SETUP_NONE)){//A is active, B is active
                TRACE("AB is active: current=%d, g_current_device_id=%d",app_bt_device.curr_hf_channel_id, current_device_id);
#ifndef FPGA
                app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
#endif
                if (Besbt_get_sco_num()>1){   
#ifdef __HF_KEEP_ONE_ALIVE__
                    bt_key_hf_channel_switch_active(current_device_id, another_device_id);
#else
                    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_SWAP_SCO,BT_STREAM_SBC, another_device_id, 0);          
#endif
                    app_bt_device.hf_voice_en[current_device_id] = HF_VOICE_DISABLE;
                    app_bt_device.hf_voice_en[another_device_id] = HF_VOICE_ENABLE;
                    app_bt_device.curr_hf_channel_id = another_device_id;
                }else{            
                    bt_key_hf_channel_switch_start();
                }
            } else if((app_bt_device.hfchan_call[current_device_id] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_call[another_device_id] == HF_CALL_ACTIVE)
                &&((app_bt_device.hfchan_callSetup[current_device_id] == HF_CALL_SETUP_IN)||(app_bt_device.hfchan_callSetup[another_device_id] == HF_CALL_SETUP_IN))){
                TRACE("AB is active and incoming call: current=%d, g_current_device_id=%d",app_bt_device.curr_hf_channel_id, current_device_id);
#ifndef FPGA
                app_voice_report(APP_STATUS_INDICATION_WARNING, 0);         
#endif
                if (Besbt_get_sco_num()>1){
#ifdef __HF_KEEP_ONE_ALIVE__
                    bt_key_hf_channel_switch_active(current_device_id, another_device_id);
#else
                    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_SWAP_SCO, BT_STREAM_SBC, another_device_id, 0);          
#endif
                    app_bt_device.hf_voice_en[current_device_id] = HF_VOICE_DISABLE;
                    app_bt_device.hf_voice_en[another_device_id] = HF_VOICE_ENABLE;
                    app_bt_device.curr_hf_channel_id = another_device_id;
                }else{            
                    bt_key_hf_channel_switch_start();
                }
            } else if(((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE))||
                ((app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE))){
//three call 
                TRACE("three way call");
#ifndef FPGA
                app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
#endif
                hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);             
#if 0
                if(app_bt_device.phone_earphone_mark == 0){
                    hfp_handle_key(HFP_KEY_CHANGE_TO_PHONE);
                }else if(app_bt_device.phone_earphone_mark == 1){
                    hfp_handle_key(HFP_KEY_ADD_TO_EARPHONE);
                }
#endif
            } else if(((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE))||
                ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_IN)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE))){
                hfp_handle_key(HFP_KEY_HANGUP_CALL);
            }
#if HID_DEVICE == XA_ENABLED
            else if(((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hf_channel[BT_DEVICE_ID_2].state == HF_STATE_CLOSED))||
                ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE))){

                Hid_Send_capture(&app_bt_device.hid_channel[BT_DEVICE_ID_1]);
            }
#endif
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            else if((open_siri_flag == 0) &&
                    (((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hf_channel[BT_DEVICE_ID_2].state == HF_STATE_CLOSED))||
                    ((app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_NONE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_NONE)&&(app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_NONE)))){
#ifndef FPGA
                    app_voice_report(APP_STATUS_INDICATION_WARNING, 0);
#endif
                    open_siri_flag = 1;
            }
#endif 


            
 #if 0
            if((app_bt_device.hfchan_call[BT_DEVICE_ID_1] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_2] == HF_CALL_SETUP_IN)){
                hfp_handle_key(HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER);
            }else if((app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE)&&(app_bt_device.hfchan_callSetup[BT_DEVICE_ID_1] == HF_CALL_SETUP_IN)){
                hfp_handle_key(HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER);
            }
#endif
            break;
#endif/* __BT_ONE_BRING_TWO__ */
        default:
            TRACE("unregister func key event=%x",event);
            break;
    }
}

#if defined(__APP_KEY_FN_STYLE_A__)
void bt_key_handle_up_key(enum APP_KEY_EVENT_T event)
{
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	struct nvrecord_env_t *nvrecord_env=NULL;
#endif
    switch(event)
    {
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
//            a2dp_handleKey(AVRCP_KEY_VOLUME_UP);
            app_bt_stream_volumeup();
            btapp_hfp_report_speak_gain();
            btapp_a2dp_report_speak_gain();
            break;
        case  APP_KEY_EVENT_LONGPRESS:
            a2dp_handleKey(AVRCP_KEY_FORWARD);
            break;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
		case  APP_KEY_EVENT_DOUBLECLICK:
			//debug switch src mode 
			nv_record_env_get(&nvrecord_env);
			if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
			{
				nvrecord_env->src_snk_flag.src_snk_mode =BT_DEVICE_SNK;
			}
			else
			{
				nvrecord_env->src_snk_flag.src_snk_mode =BT_DEVICE_SRC;
			}
			nv_record_env_set(nvrecord_env);
			app_reset();
			break;
#endif
        default:
            TRACE("unregister up key event=%x",event);
            break;
    }
}


void bt_key_handle_down_key(enum APP_KEY_EVENT_T event)
{
    switch(event)
    {
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
 //           a2dp_handleKey(AVRCP_KEY_VOLUME_DOWN);
            app_bt_stream_volumedown();
            btapp_hfp_report_speak_gain();
            btapp_a2dp_report_speak_gain();
            break;
        case  APP_KEY_EVENT_LONGPRESS:
            a2dp_handleKey(AVRCP_KEY_BACKWARD);

            break;
        default:
            TRACE("unregister down key event=%x",event);
            break;
    }
}
#else //#elif defined(__APP_KEY_FN_STYLE_B__)
void bt_key_handle_up_key(enum APP_KEY_EVENT_T event)
{
    switch(event)
    {
        case  APP_KEY_EVENT_REPEAT:
//            a2dp_handleKey(AVRCP_KEY_VOLUME_UP);
            app_bt_stream_volumeup();
            btapp_hfp_report_speak_gain();
            btapp_a2dp_report_speak_gain();
            break;
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
            a2dp_handleKey(AVRCP_KEY_FORWARD);
            break;
        default:
            TRACE("unregister up key event=%x",event);
            break;
    }
}


void bt_key_handle_down_key(enum APP_KEY_EVENT_T event)
{
    switch(event)
    {
        case  APP_KEY_EVENT_REPEAT:
 //           a2dp_handleKey(AVRCP_KEY_VOLUME_DOWN);
            app_bt_stream_volumedown();
            btapp_hfp_report_speak_gain();
            btapp_a2dp_report_speak_gain();
            break;
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
            a2dp_handleKey(AVRCP_KEY_BACKWARD);
            break;
        default:
            TRACE("unregister down key event=%x",event);
            break;
    }
}
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
void bt_key_handle_source_func_key(enum APP_KEY_EVENT_T event)
{
    TRACE("%s,%d",__FUNCTION__,event);
    static bool onaudioloop = false;
    switch(event)
    {
        case  APP_KEY_EVENT_UP:
        case  APP_KEY_EVENT_CLICK:
            app_a2dp_source_find_sink();
            break;
        case APP_KEY_EVENT_DOUBLECLICK:
			if(app_bt_device.a2dp_state[0]==1)
			{
				 onaudioloop = onaudioloop?false:true;
				if (onaudioloop)
				{			
					app_a2dp_start_stream();

				}
				else
				{
					app_a2dp_suspend_stream();
				}
			}
			break;
        case APP_KEY_EVENT_TRIPLECLICK:
            app_a2dp_start_stream();
            break;
        default:
            TRACE("unregister down key event=%x",event);
            break;
    }    
}
#endif

APP_KEY_STATUS bt_key;
void bt_key_send(APP_KEY_STATUS *status)
{
    OS_LockStack();
    if(bt_key.code == 0xff){
        TRACE("%s code:%d evt:%d",__func__, status->code, status->event);
        bt_key.code = status->code;
        bt_key.event = status->event;
        OS_NotifyEvm();
    }else{
        TRACE("%s busy",__func__);
    }
    OS_UnlockStack();
}

void bt_key_handle(void)
{
    OS_LockStack();
    if(bt_key.code != 0xff)
    {
        TRACE("%s code:%d evt:%d",__func__, bt_key.code, bt_key.event);
        switch(bt_key.code)
        {
            case BTAPP_FUNC_KEY:
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
				if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
				{
					bt_key_handle_source_func_key((enum APP_KEY_EVENT_T)bt_key.event);
				}
				else
#endif
				{
#ifndef HM_LIVE_UI				
					bt_key_handle_func_key((enum APP_KEY_EVENT_T)bt_key.event);
#endif
				}			
                break;
            case BTAPP_VOLUME_UP_KEY:
                bt_key_handle_up_key((enum APP_KEY_EVENT_T)bt_key.event);
                break;
            case BTAPP_VOLUME_DOWN_KEY:
                bt_key_handle_down_key((enum APP_KEY_EVENT_T)bt_key.event);
                break;
#ifdef HM_LIVE_UI				
			case BT_APP_PLAY_KEY:		
				bt_key_handle_func_key((enum APP_KEY_EVENT_T)bt_key.event);
				break;
#endif				
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
            case BTAPP_RELEASE_KEY:
                bt_key_handle_siri_key((enum APP_KEY_EVENT_T)bt_key.event);
                break;
#endif
            default:
                TRACE("bt_key_handle  undefined key");
                break;
        }
        bt_key.code = 0xff;
    }    
    OS_UnlockStack();
}

void bt_key_init(void)
{
    Besbt_hook_handler_set(BESBT_HOOK_USER_2, bt_key_handle);
#ifdef __BT_ONE_BRING_TWO__
    bt_key_hf_channel_switch_init();
#endif
    bt_key.code = 0xff;
    bt_key.event = 0xff;
}

