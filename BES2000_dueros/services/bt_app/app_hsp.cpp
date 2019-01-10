#include <stdio.h>
#include "cmsis_os.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "hal_chipid.h"
#include "analog.h"
#include "app_audio.h"
#include "app_status_ind.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"


extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hs.h"
#include "btalloc.h"
}


#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "app_bt.h"

#include "nvrecord.h"

#include "apps.h"
#include "resources.h"

#include "app_bt_media_manager.h"

#if defined(__HSP_ENABLE__) && defined(__BT_ONE_BRING_TWO__)
#error can not defined at the same time.
#endif

#if defined (__HSP_ENABLE__)

#define MODIFY_HS_CALL_SETUP_IN  	HF_CALL_SETUP_IN
#define MODIFY_HS_CALL_ACTIVE 		HF_CALL_ACTIVE
#define MODIFY_HS_CALL_NONE		HF_CALL_NONE


extern struct BT_DEVICE_ID_DIFF chan_id_flag;
extern struct BT_DEVICE_T  app_bt_device;

#if defined(SCO_LOOP)
#define HF_LOOP_CNT (20)
#define HF_LOOP_SIZE (360)


extern uint8_t hf_loop_buffer[HF_LOOP_CNT*HF_LOOP_SIZE];
extern uint32_t hf_loop_buffer_len[HF_LOOP_CNT];
extern uint32_t hf_loop_buffer_valid ;
extern uint32_t hf_loop_buffer_size ;
extern char hf_loop_buffer_w_idx ;

#endif

#ifndef _SCO_BTPCM_CHANNEL_
extern struct HF_SENDBUFF_CONTROL_T  hf_sendbuff_ctrl;
#endif



#ifdef __BT_ONE_BRING_TWO__
extern void hfp_chan_id_distinguish(HfChannel *Chan);
#endif
extern osPoolId   app_hfp_hfcommand_mempool ;
extern void app_bt_profile_connect_manager_hf(enum BT_DEVICE_ID_T id, HfChannel *Chan, HfCallbackParms *Info);
extern void hfp_app_status_indication(enum BT_DEVICE_ID_T chan_id,HfCallbackParms *Info);
extern int app_hfp_battery_report_reset(uint8_t bt_device_id);
extern int app_hfp_hfcommand_mempool_init(void);
extern int app_hfp_hfcommand_mempool_calloc(HfCommand **hf_cmd_p);
extern int app_hfp_hfcommand_mempool_free(HfCommand *hf_cmd_p);


extern int store_voicebtpcm_m2p_buffer(unsigned char *buf, unsigned int len);
extern int get_voicebtpcm_p2m_frame(unsigned char *buf, unsigned int len);
extern int store_voicepcm_buffer(unsigned char *buf, unsigned int len);
extern int store_voicecvsd_buffer(unsigned char *buf, unsigned int len);
extern int store_voicemsbc_buffer(unsigned char *buf, unsigned int len);
extern int hfp_volume_get(enum BT_DEVICE_ID_T id);
extern void hfp_volume_local_set(int8_t vol);
extern int hfp_volume_set(enum BT_DEVICE_ID_T id, int vol);
extern void app_bt_profile_connect_manager_hs(enum BT_DEVICE_ID_T id, HsChannel *Chan, HsCallbackParms *Info);;

#ifdef __BT_ONE_BRING_TWO__
void hsp_chan_id_distinguish(HsChannel *Chan)
{
    if(Chan == &app_bt_device.hs_channel[BT_DEVICE_ID_1]){
        chan_id_flag.id = BT_DEVICE_ID_1;
        chan_id_flag.id_other = BT_DEVICE_ID_2;
    }else if(Chan == &app_bt_device.hsf_channel[BT_DEVICE_ID_2]){
        chan_id_flag.id = BT_DEVICE_ID_2;
        chan_id_flag.id_other = BT_DEVICE_ID_1;
    }
}
#endif

int app_hsp_hscommand_mempool_init(void)
{
    app_hfp_hfcommand_mempool_init();
    return 0;
}

int app_hsp_hscommand_mempool_calloc(HsCommand **hs_cmd_p)
{
   app_hfp_hfcommand_mempool_calloc(hs_cmd_p);
    return 0;
}

int app_hsp_hscommand_mempool_free(HsCommand *hs_cmd_p)
{
    app_hfp_hfcommand_mempool_free(hs_cmd_p);
    return 0;
}
#if 0
#define app_hsp_hscommand_mempool               app_hfp_hfcommand_mempool
#define app_hsp_hscommand_mempool_init      app_hfp_hfcommand_mempool_init
#define app_hsp_hscommand_mempool_calloc    app_hfp_hfcommand_mempool_calloc
#define app_hsp_hscommand_mempool_free      app_hfp_hfcommand_mempool_free
#endif


XaStatus app_hs_handle_cmd(HsChannel *Chan,uint8_t cmd_type)
{
        HsCommand *hs_cmd_p;
        int8_t ret = 0;
        switch(cmd_type){
        case APP_REPORT_SPEAKER_VOL_CMD:
             app_hsp_hscommand_mempool_calloc(&hs_cmd_p);    
             if (hs_cmd_p){
                if(HS_ReportSpeakerVolume(Chan,hfp_volume_get(chan_id_flag.id), hs_cmd_p) != BT_STATUS_PENDING)
                    {
                        app_hsp_hscommand_mempool_free(hs_cmd_p);
                        ret = -1;
                }
          }    	
            break;
        case APP_CPKD_CMD:
            app_hsp_hscommand_mempool_calloc(&hs_cmd_p);    
             if (hs_cmd_p){
                if(HS_CKPD_CONTROL(Chan,hs_cmd_p) != BT_STATUS_PENDING)
                    {
                        app_hsp_hscommand_mempool_free(hs_cmd_p);
                        ret = -1;
                }
          }    	
            break;
        default:
            break;
        }

        return ret ;
        
}



// because hfp and hsp can not exist simultaneously , so we do not need to alloc 2 cmd pool!
void app_hsp_init(void)
{

    
    app_hsp_hscommand_mempool_init();

    app_bt_device.curr_hs_channel_id = BT_DEVICE_ID_1;
    app_bt_device.hs_mute_flag = 0;

    for(uint8_t i=0; i<BT_DEVICE_NUM; i++)
    {
        app_bt_device.hschan_call[i] = 0;
        app_bt_device.hs_audio_state[i] = 0;
        app_bt_device.hs_conn_flag[i] = 0;
        app_bt_device.hs_voice_en[i] = 0;
    }
}



/*
Differ with HFP in HF_EVENT_RING_IND:
    Because hsp lack of some state, like         HF_EVENT_CALL_IND
    								    HF_EVENT_CALLSETUP_IND
    								    (DONE)
    And for the least code modify purpose, meanwhile  keep the state runs ok.
    i put all the state setting in HF_EVENT_RING_IND case 
*/
void hsp_callback(HsChannel *Chan, HsCallbackParms *Info)
{
    uint8_t ret = 0;

#ifdef __BT_ONE_BRING_TWO__
    hsp_chan_id_distinguish(Chan);
#else
    chan_id_flag.id = BT_DEVICE_ID_1;
#endif

	TRACE("[%s] event = %d",__func__,Info->event);

    switch(Info->event)
    {
    case HS_EVENT_SERVICE_CONNECTED:
        TRACE("::HS_EVENT_SERVICE_CONNECTED  Chan_id:%d\n", chan_id_flag.id);        
        app_bt_profile_connect_manager_hs(chan_id_flag.id, Chan, Info);
#if !defined(FPGA) && defined(__EARPHONE__)
        if(Chan->state == HF_STATE_OPEN){
            ////report connected voice
            app_bt_device.hs_conn_flag[chan_id_flag.id] = 1;
            
        }
#endif
        app_bt_stream_volume_ptr_update((uint8_t *)Info->p.remDev->bdAddr.addr);
        if(app_hs_handle_cmd(Chan,APP_REPORT_SPEAKER_VOL_CMD) !=0)
            TRACE("app_hs_handle_cmd err");
      
        break;
    case HS_EVENT_AUDIO_DATA_SENT:
      TRACE("::HF_EVENT_AUDIO_DATA_SENT %d\n", Info->event);
#if defined(SCO_LOOP)
        hf_loop_buffer_valid = 1;
#endif
        break;
    case HS_EVENT_AUDIO_DATA:
	TRACE("HF_EVENT_AUDIO_DATA");	
    {

#ifndef _SCO_BTPCM_CHANNEL_
        uint32_t idx = 0;
        if (app_bt_stream_isrun(APP_BT_STREAM_HFP_PCM)){
            store_voicebtpcm_m2p_buffer(Info->p.audioData->data, Info->p.audioData->len);

            idx = hf_sendbuff_ctrl.index%HF_SENDBUFF_MEMPOOL_NUM;
            get_voicebtpcm_p2m_frame(&(hf_sendbuff_ctrl.mempool[idx].buffer[0]), Info->p.audioData->len);
            hf_sendbuff_ctrl.mempool[idx].packet.data = &(hf_sendbuff_ctrl.mempool[idx].buffer[0]);
            hf_sendbuff_ctrl.mempool[idx].packet.dataLen = Info->p.audioData->len;
            hf_sendbuff_ctrl.mempool[idx].packet.flags = BTP_FLAG_NONE;
            if(!app_bt_device.hf_mute_flag){
                HF_SendAudioData(Chan, &hf_sendbuff_ctrl.mempool[idx].packet);
            }
            hf_sendbuff_ctrl.index++;
        }
#endif

    }

#if defined(SCO_LOOP)
    memcpy(hf_loop_buffer + hf_loop_buffer_w_idx*HF_LOOP_SIZE, Info->p.audioData->data, Info->p.audioData->len);
    hf_loop_buffer_len[hf_loop_buffer_w_idx] = Info->p.audioData->len;
    hf_loop_buffer_w_idx = (hf_loop_buffer_w_idx+1)%HF_LOOP_CNT;
    ++hf_loop_buffer_size;

    if (hf_loop_buffer_size >= 18 && hf_loop_buffer_valid == 1) {
        hf_loop_buffer_valid = 0;
        idx = hf_loop_buffer_w_idx-17<0?(HF_LOOP_CNT-(17-hf_loop_buffer_w_idx)):hf_loop_buffer_w_idx-17;
        pkt.flags = BTP_FLAG_NONE;
        pkt.dataLen = hf_loop_buffer_len[idx];
        pkt.data = hf_loop_buffer + idx*HF_LOOP_SIZE;
        HF_SendAudioData(Chan, &pkt);
    }
#endif
        break;
    case HS_EVENT_SERVICE_DISCONNECTED:
        TRACE("::HS_EVENT_SERVICE_DISCONNECTED Chan_id:%d, reason=%x\n", chan_id_flag.id,Info->p.remDev->discReason);
        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_VOICE,chan_id_flag.id,MAX_RECORD_NUM);

#if !defined(FPGA) && defined(__EARPHONE__)
        if(app_bt_device.hs_conn_flag[chan_id_flag.id] ){
            ////report device disconnected voice
            app_bt_device.hs_conn_flag[chan_id_flag.id] = 0;
            
        }
#endif
        app_bt_stream_volume_ptr_update(NULL);

        app_bt_profile_connect_manager_hs(chan_id_flag.id, Chan, Info);
        for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
            if (Chan == &(app_bt_device.hs_channel[i])){
                app_bt_device.hschan_call[i] = 0;
                app_bt_device.hs_audio_state[i] = 0;
                app_bt_device.hs_conn_flag[i] = 0;
                app_bt_device.hs_voice_en[i] = 0;
            }
        }
        break;
    
    case HS_EVENT_AUDIO_CONNECTED:
		
        if(Info->status == BT_STATUS_SUCCESS){	
		 TRACE("::HS_EVENT_AUDIO_CONNECTED  chan_id:%d\n", chan_id_flag.id);
                   if((Chan->state == HF_STATE_OPEN) && (app_bt_device.hs_conn_flag[chan_id_flag.id] == 1)){            
		    app_bt_device.hschan_call[BT_DEVICE_ID_1] = MODIFY_HS_CALL_ACTIVE;
                    }

		 app_bt_device.curr_hs_channel_id = chan_id_flag.id;

	        app_bt_device.phone_earphone_mark = 0;
	        app_bt_device.hs_mute_flag = 0;

	        app_bt_device.hs_audio_state[chan_id_flag.id] = HF_AUDIO_CON;
            
#if defined(__FORCE_REPORTVOLUME_SOCON__)            
            
            app_hs_handle_cmd(Chan,APP_REPORT_SPEAKER_VOL_CMD);

#endif

	        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_VOICE,BT_DEVICE_ID_1,MAX_RECORD_NUM);

        }

	

        break;
    case HS_EVENT_AUDIO_DISCONNECTED:
        TRACE("::HS_EVENT_AUDIO_DISCONNECTED  chan_id:%d\n", chan_id_flag.id);
	
        if(app_bt_device.hschan_call[chan_id_flag.id] == HF_CALL_ACTIVE){
            app_bt_device.phone_earphone_mark = 1;
        }	

        app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_VOICE,BT_DEVICE_ID_1,MAX_RECORD_NUM);

        break;

    case HS_EVENT_RING_IND:
        TRACE("::HS_EVENT_RING_IND  chan_id:%d\n", chan_id_flag.id);
#if !defined(FPGA) && defined(__EARPHONE__)
//        if(app_bt_device.hs_audio_state[chan_id_flag.id] != HF_AUDIO_CON)
            app_voice_report(APP_STATUS_INDICATION_INCOMINGCALL,chan_id_flag.id);
#endif
	 
        break;
    case HS_EVENT_SPEAKER_VOLUME:
        TRACE("::HS_EVENT_SPEAKER_VOLUME  chan_id:%d,speaker gain = %x\n", chan_id_flag.id,Info->p.ptr);
        hfp_volume_set(chan_id_flag.id, (int)(uint32_t)Info->p.ptr);
        break;

    case HS_EVENT_COMMAND_COMPLETE:
        TRACE("::EVENT_HS_COMMAND_COMPLETE  chan_id:%d %x\n", chan_id_flag.id, (HsCommand *)Info->p.ptr);
        if (Info->p.ptr)
            app_hsp_hscommand_mempool_free((HsCommand *)Info->p.ptr);
        break;
    default:
        break;

    }
}


#endif
