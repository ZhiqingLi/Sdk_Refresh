//#include "mbed.h"
#include <stdio.h>
#include <assert.h>

#include "cmsis_os.h"
#include "cmsis.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_chipid.h"
#include "audioflinger.h"
#include "lockcqueue.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "analog.h"
#include "app_bt_stream.h"
#include "app_overlay.h"
#include "app_audio.h"
#include "app_utils.h"
#include "nvrecord.h"

#include "resources.h"
#ifdef MEDIA_PLAYER_SUPPORT
#include "app_media_player.h"
#endif

#include "bt_drv.h"
extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "a2dp.h"
#include "avdtp.h"
#include "avctp.h"
#include "avrcp.h"
#include "hf.h"
#include "sys/mei.h"
}
#include "app_bt_func.h"

#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"

#ifdef EQ_PROCESS
#include "eq_export.h"
#endif

#include "app_bt_media_manager.h"
#include "app_thread.h"

#include "app_ring_merge.h"

uint8_t btapp_hfp_get_call_state(void);
uint8_t btapp_hfp_incoming_calls(void);
int bt_sco_player_forcemute(bool mic_mute, bool spk_mute);
uint8_t btapp_hfp_get_call_active(void);

int bt_sco_player_get_codetype(void);
extern struct BT_DEVICE_T  app_bt_device;

typedef struct
{
    uint8_t media_active[BT_DEVICE_NUM];
    uint8_t media_current_call_state[BT_DEVICE_NUM];
    uint8_t media_curr_sbc;
    uint8_t media_curr_sco;
    uint8_t curr_active_media;
}BT_MEDIA_MANAGER_STRUCT;


BT_MEDIA_MANAGER_STRUCT  bt_meida;


#ifdef RECORD_LONELY
#undef __SMARTVOICE__
#undef __AMA_VOICE__
#undef __DMA_VOICE__
#endif


uint8_t bt_media_is_media_active_by_type(uint8_t media_type)
{
    uint8_t i;
    for(i=0;i<BT_DEVICE_NUM;i++)
    {
        if(bt_meida.media_active[i] & media_type)
            return 1;

    }
    return 0;
}

static enum BT_DEVICE_ID_T bt_media_get_active_device_by_type(uint8_t media_type)
{
    uint8_t i;
    for(i=0;i<BT_DEVICE_NUM;i++)
    {
        if(bt_meida.media_active[i] & media_type)
            return (enum BT_DEVICE_ID_T)i;

    }
    return BT_DEVICE_NUM;
}


static uint8_t bt_media_is_media_active_by_device(uint8_t media_type,enum BT_DEVICE_ID_T device_id)
{

    if(bt_meida.media_active[device_id] & media_type)
        return 1;
    return 0;
}


static uint8_t bt_media_get_current_media(void)
{
    return bt_meida.curr_active_media;
}


static void bt_media_set_current_media(uint8_t media_type)
{
    TRACE("set current media = %x",media_type);
    bt_meida.curr_active_media =  media_type;
}

static void bt_media_clear_current_media(void)
{
    TRACE("clear current media = %x");

    bt_meida.curr_active_media = 0;
}

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
static void bt_media_clear_all_media_type(void)
{
    uint8_t i;
    for(i=0;i<BT_DEVICE_NUM;i++)
    {
        bt_meida.media_active[i] &= (~BT_STREAM_MEDIA);
    }
}
#endif

static uint8_t bt_media_set_media_type(uint8_t media_type,enum BT_DEVICE_ID_T device_id)
{
    bt_meida.media_active[device_id] |= media_type;
    return 0;
}

static void bt_media_clear_media_type(uint8_t media_type,enum BT_DEVICE_ID_T device_id)
{
    if (media_type==BT_STREAM_MEDIA){
        for(uint8_t i=0;i<BT_DEVICE_NUM;i++){
            bt_meida.media_active[i] &= (~media_type);
        }
    }else{
        bt_meida.media_active[device_id] &= (~media_type);
    }
}

static enum BT_DEVICE_ID_T bt_media_get_active_sbc_device(void)
{
    enum BT_DEVICE_ID_T  device = BT_DEVICE_NUM;
    uint8_t i;
    for(i=0;i<BT_DEVICE_NUM;i++)
    {
        if((bt_meida.media_active[i] & BT_STREAM_SBC)  && (i==bt_meida.media_curr_sbc))
            device = (enum BT_DEVICE_ID_T)i;
    }
    return device;
}


#ifdef MEDIA_PLAYER_RBCODEC

bool  bt_media_rbcodec_start_process(uint8_t stream_type,enum BT_DEVICE_ID_T device_id,AUD_ID_ENUM media_id, uint32_t param, uint32_t ptr)
{
    int ret_SendReq2AudioThread = -1;
    bt_meida.media_active[device_id] |= stream_type;

#if 0
    //priority
    //vioce
    //rbcodec
    //sbc
    //media
    if(bt_media_is_media_active_by_type(BT_STREAM_VOICE)) {
        //voice on then return it
        TRACE("voice is on");
        goto exit;
    }
#ifdef MEDIA_PLAYER_SUPPORT
    else if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
    {
        TRACE("media is on");
        goto exit;
    }
#endif
    ////if sbc active, need suspend it then play
    else if(bt_media_is_media_active_by_type(BT_STREAM_SBC)) {
        if(bt_media_get_current_media() == BT_STREAM_SBC) {
            ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
#ifdef __TWS__
            notify_tws_player_status(APP_BT_SETTING_CLOSE);
#endif
        }
        ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_RBCODEC, (uint8_t)APP_BT_SETTING_OPEN, 0);
        bt_media_set_current_media(BT_STREAM_RBCODEC);
    }
    /// just play the media
    else {
        ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_RBCODEC, (uint8_t)APP_BT_SETTING_OPEN, media_id);
        bt_media_set_current_media(BT_STREAM_RBCODEC);
    }
#endif

    ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_RBCODEC, (uint8_t)APP_BT_SETTING_OPEN, media_id);
    bt_media_set_current_media(BT_STREAM_RBCODEC);
    return true;
exit:
    return false;
}
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
static bool  bt_media_smartvoice_start_process(uint8_t stream_type,enum BT_DEVICE_ID_T device_id,AUD_ID_ENUM media_id, uint32_t param, uint32_t ptr)
{
    bt_meida.media_active[device_id] |= stream_type;
	app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_OPEN, media_id);
	bt_media_set_current_media(BT_STREAM_AI_VOICE);
    return true;
}
#endif
//only used in iamain thread ,can't used in other thread or interrupt
void  bt_media_start(uint8_t stream_type,enum BT_DEVICE_ID_T device_id,AUD_ID_ENUM media_id)
{
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T other_device_id = (device_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
#endif

    bt_meida.media_active[device_id] |= stream_type;



    TRACE("STREAM MANAGE bt_media_start type= %x,device id = %x,media_id = %x",stream_type,device_id,media_id);
#ifdef __BT_ONE_BRING_TWO__
    TRACE("bt_media_start media_active = %x,%x,curr_active_media = %x",
            bt_meida.media_active[0],bt_meida.media_active[1], bt_meida.curr_active_media);
#else
    TRACE("bt_media_start media_active = %x,curr_active_media = %x",
            bt_meida.media_active[0], bt_meida.curr_active_media);

#endif
    switch(stream_type)
    {
#ifdef MEDIA_PLAYER_RBCODEC
        case BT_STREAM_RBCODEC:
            if(!bt_media_rbcodec_start_process(stream_type,device_id,media_id, NULL, NULL))
                goto exit;
            break;
#endif
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
        case BT_STREAM_AI_VOICE:
#ifdef MEDIA_PLAYER_SUPPORT
            if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
            {
                return;
            }
#endif
            if(!bt_media_smartvoice_start_process(stream_type,device_id,media_id, (uint32_t)NULL, (uint32_t)NULL))
                goto exit;
            break;
#endif	
        case BT_STREAM_SBC:
            ////because voice is the highest priority and media report will stop soon
            //// so just store the sbc type
            if(bt_meida.media_curr_sbc == BT_DEVICE_NUM)
                bt_meida.media_curr_sbc = device_id;
#ifdef MEDIA_PLAYER_SUPPORT
            if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
            {
                goto exit;
            }
#endif
#ifdef MEDIA_PLAYER_RBCODEC
            if(bt_media_is_media_active_by_type(BT_STREAM_RBCODEC)) {
                // app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_208M);
                goto exit;
            }
#endif

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
            if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
                goto exit;
            }
#endif
#ifdef AUDIO_LINEIN           
            if(bt_media_is_media_active_by_type(BT_STREAM_LINEIN))
            {
                if(bt_media_get_current_media() == BT_STREAM_LINEIN)
                {
                    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP, (uint8_t)BT_STREAM_LINEIN, 0,0);
                    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START, BT_STREAM_SBC, device_id, 0);
                    return;
                }       
            }
#endif   

            if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
            {
                ////sbc and voice is all on so set sys freq to 104m
                app_sysfreq_req(APP_SYSFREQ_USER_APP_0, APP_SYSFREQ_104M);
                return;
            }
#ifdef __BT_ONE_BRING_TWO__
            else if (btapp_hfp_get_call_state())
            {
                //do nothing
                TRACE("skip sbc start, because HF_CALL_ACTIVE");
            }
            else if(bt_media_is_media_active_by_device(BT_STREAM_SBC,other_device_id))
            {
                //if another device is the active stream do nothing
                if(bt_meida.media_curr_sbc == other_device_id)
                {

                    ///2 device is play sbc,so set sys freq to 104m
                    app_audio_manager_switch_a2dp((enum BT_DEVICE_ID_T)bt_meida.media_curr_sbc);
                    app_audio_sendrequest_param((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_RESTART, 0, APP_SYSFREQ_104M);
                    return;
                }
                ////if curr active media is not sbc,wrong~~
                if(bt_media_get_current_media() != BT_STREAM_SBC)
                {
                    ASSERT(0,"curr_active_media is wrong!");
                }
                ///stop the old audio sbc and start the new audio sbc
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
            }
#endif
            else{
                //start audio sbc stream
                app_audio_manager_switch_a2dp(device_id);
                bt_media_set_current_media(BT_STREAM_SBC);
                app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                        (uint8_t)(APP_BT_SETTING_SETUP),
                        (uint32_t)(app_bt_device.sample_rate[device_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
            }
            break;
#ifdef MEDIA_PLAYER_SUPPORT
        case BT_STREAM_MEDIA:
#ifdef AUDIO_LINEIN            
            if(bt_media_is_media_active_by_type(BT_STREAM_LINEIN))
            {
                if(bt_media_get_current_media() == BT_STREAM_LINEIN)
                {
                    APP_AUDIO_STATUS aud_status;
                    aud_status.aud_id = media_id;
                    app_play_audio_lineinmode_start(&aud_status);
                    bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
                }       
            }else
#endif                    
            //first,if the voice is active so  mix "dudu" to the stream
            if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
            {

                if(bt_media_get_current_media() == BT_STREAM_VOICE)
                {
                    //if call is not active so do media report
                    if((btapp_hfp_get_call_state() && !btapp_hfp_incoming_calls()) ||
                            (app_bt_stream_isrun(APP_BT_STREAM_HFP_PCM)))
                    {
                        //todo ..mix the "dudu"
                        TRACE("BT_STREAM_VOICE-->app_ring_merge_start\n");
                        app_ring_merge_start();
                        //meida is done here
                        bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
                    }
                    else
                    {
                        TRACE("stop sco and do media report\n");
                        bt_media_set_current_media(BT_STREAM_MEDIA);
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                        app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                    }
                }
                else if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                {
                    bt_media_set_current_media(BT_STREAM_MEDIA);
                    app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                }
                else
                {
                    ///if voice is active but current is not voice something is unkown
                    bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
#ifdef __BT_ONE_BRING_TWO__
                    TRACE("STREAM MANAGE voice  active media_active = %x,%x,curr_active_media = %x",
                            bt_meida.media_active[0],bt_meida.media_active[1], bt_meida.curr_active_media);
#else
                    TRACE("STREAM MANAGE voice  active media_active = %x,curr_active_media = %x",
                            bt_meida.media_active[0], bt_meida.curr_active_media);

#endif
                }
            }
            else if (btapp_hfp_get_call_state())
            {
                bt_media_set_current_media(BT_STREAM_MEDIA);
                app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
            }
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)		 
            else if( bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE) ) {
                if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                {
                    app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                }
                else {
                    goto exit;
                }
            }
#endif
            ////if sbc active so
            else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
            {
                if(bt_media_get_current_media() == BT_STREAM_SBC)
                {
#ifdef __BT_WARNING_TONE_MERGE_INTO_STREAM_SBC__
                    if (media_id == AUD_ID_BT_WARNING)
                    {
                        TRACE("BT_STREAM_SBC-->app_ring_merge_start\n");
                        app_ring_merge_start();
                        //meida is done here
                        bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
                    }
                    else
#endif
                    {
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                        bt_media_set_current_media(BT_STREAM_MEDIA);
                        app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                    }
                }
                else if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                {
                    app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                }
                else if(bt_media_get_current_media() == 0)
                {
                    app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
                }
                else
                {
                    ASSERT(0,"media in sbc  current wrong");
                }
            }
            /// just play the media
            else
            {
                bt_media_set_current_media(BT_STREAM_MEDIA);
                app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, media_id);
            }
            break;
#endif
        case BT_STREAM_VOICE:
            app_audio_manager_set_active_sco_num(device_id);

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
            if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
                if(bt_media_get_current_media() == BT_STREAM_AI_VOICE) {
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    bt_media_clear_current_media();
                }
            }
#endif
#ifdef MEDIA_PLAYER_SUPPORT
            if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
            {
                //if call is active ,so disable media report
                if(btapp_hfp_get_call_active())
                {
                    //if meida is open ,close media clear all media type
                    if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                    {
                        TRACE("call active so start sco and stop media report\n");
#ifdef __AUDIO_QUEUE_SUPPORT__
                        app_audio_list_clear();
#endif
                        app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);

                    }
                    bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
                    //     bt_media_set_media_type(BT_STREAM_VOICE, device_id);
                    bt_media_set_current_media(BT_STREAM_VOICE);
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_OPEN, 0);
                }
                else
                {
                    ////call is not active so media report continue
                }
            }
            else
#endif
                if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
                {
                    ///if sbc is open  stop sbc
                    if(bt_media_get_current_media() == BT_STREAM_SBC)
                    {
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    }
                    ////start voice stream
                    bt_media_set_current_media(BT_STREAM_VOICE);
                    app_audio_sendrequest_param((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_OPEN, 0, APP_SYSFREQ_104M);
                }
                else
                {
                    //voice is open already so do nothing
                    if(bt_media_get_current_media() == BT_STREAM_VOICE)
                    {
#if defined(__BT_ONE_BRING_TWO__)
                        if(Besbt_get_sco_num()>1
#ifdef CHIP_BEST1000
                                && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2
#endif
                          ){
                            app_audio_manager_swap_sco(device_id);
#if defined(__HF_KEEP_ONE_ALIVE__)
                            if (HF_IsAudioConnected(&app_bt_device.hf_channel[other_device_id])){
                                TRACE("%s Disconnect another AudioLink", __func__);
                                app_bt_HF_DisconnectAudioLink(&app_bt_device.hf_channel[other_device_id]);
                            }
#endif
                        }
#endif
                    }
                    else
                    {
                        bt_media_set_current_media(BT_STREAM_VOICE);
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_OPEN, 0);
                    }
                }

            break;
#ifdef AUDIO_LINEIN
        case BT_STREAM_LINEIN:
            if(!bt_media_is_media_active_by_type(BT_STREAM_SBC | BT_STREAM_MEDIA | BT_STREAM_VOICE))
            { 	
                 app_audio_sendrequest((uint8_t)APP_PLAY_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, 0);
                 bt_media_set_current_media(BT_STREAM_LINEIN);
            }
            break;
#endif
            
        default:
            ASSERT(0,"bt_media_open ERROR TYPE");
            break;

    }

exit:
    return;
}

#ifdef MEDIA_PLAYER_RBCODEC

static bool bt_media_rbcodec_stop_process(uint8_t stream_type,enum BT_DEVICE_ID_T device_id, uint32_t ptr)
{
    int ret_SendReq2AudioThread = -1;
    bt_media_clear_media_type(stream_type,device_id);
    //if current stream is the stop one ,so stop it
    if(bt_media_get_current_media() == BT_STREAM_RBCODEC ) {
        ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_RBCODEC, (uint8_t)APP_BT_SETTING_CLOSE, ptr);
        bt_media_clear_current_media();
        TRACE(" RBCODEC STOPED! ");
    }

    if(bt_media_is_media_active_by_type(BT_STREAM_SBC)) {
        enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
        TRACE("sbc_id %d",sbc_id);
        if(sbc_id < BT_DEVICE_NUM) {
            bt_meida.media_curr_sbc = sbc_id;
        }
    } else {
        bt_meida.media_curr_sbc = BT_DEVICE_NUM;
    }

    TRACE("bt_meida.media_curr_sbc %d",bt_meida.media_curr_sbc);

    if(bt_media_is_media_active_by_type(BT_STREAM_VOICE)) {
        //ASSERT(bt_media_get_current_media() == BT_STREAM_VOICE);
    } else if(bt_media_is_media_active_by_type(BT_STREAM_SBC)) {
        // __BT_ONE_BRING_TWO__
        enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
        if(sbc_id < BT_DEVICE_NUM) {
#ifdef __TWS__
            bt_media_clear_media_type(BT_STREAM_SBC,sbc_id);
            bt_media_clear_current_media();
            notify_tws_player_status(APP_BT_SETTING_OPEN);
#else
            bt_parse_store_sbc_sample_rate(app_bt_device.sample_rate[sbc_id]);
            ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
            bt_media_set_current_media(BT_STREAM_SBC);
#endif
        }
    } else if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA)) {
        //do nothing
    }
}
#endif

#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)

bool bt_media_smartvoice_stop_process(uint8_t stream_type,enum BT_DEVICE_ID_T device_id)
{
    int ret_SendReq2AudioThread __attribute__((unused));
	ret_SendReq2AudioThread= -1;
    bt_media_clear_media_type(stream_type,device_id);
    //if current stream is the stop one ,so stop it
    if(bt_media_get_current_media() == BT_STREAM_AI_VOICE) {
        ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_CLOSE, 0);
        bt_media_clear_current_media();
        TRACE(" SMARTVOICE STOPED! ");
    }

    if(bt_media_is_media_active_by_type(BT_STREAM_SBC)) {
        enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
        TRACE("sbc_id %d",sbc_id);
        if(sbc_id < BT_DEVICE_NUM) {
            bt_meida.media_curr_sbc = sbc_id;
        }
    } else {
        bt_meida.media_curr_sbc = BT_DEVICE_NUM;
    }

    TRACE("bt_meida.media_curr_sbc %d",bt_meida.media_curr_sbc);

    if(bt_media_is_media_active_by_type(BT_STREAM_VOICE)) {
        //ASSERT(bt_media_get_current_media() == BT_STREAM_VOICE);
    } 
	else if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA)) {
		//do nothing!!!
        //app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_OPEN, 0);
        //bt_media_set_current_media(BT_STREAM_MEDIA);
	}  
    else if(bt_media_is_media_active_by_type(BT_STREAM_SBC)) {
        // __BT_ONE_BRING_TWO__
        enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
        if(sbc_id < BT_DEVICE_NUM) {
#ifdef __TWS__
            bt_media_clear_media_type(BT_STREAM_SBC,sbc_id);
            bt_media_clear_current_media();
            notify_tws_player_status(APP_BT_SETTING_OPEN);
#else
            //bt_parse_store_sbc_sample_rate(app_bt_device.sample_rate[sbc_id]);
            ret_SendReq2AudioThread = app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
            bt_media_set_current_media(BT_STREAM_SBC);
#endif
        }
    } 
		return true;
}
#endif

/*
   bt_media_stop function is called to stop media by app or media play callback
   sbc is just stop by a2dp stream suspend or close
   voice is just stop by hfp audio disconnect
   media is stop by media player finished call back

*/
void bt_media_stop(uint8_t stream_type,enum BT_DEVICE_ID_T device_id)
{
    TRACE("STREAM MANAGE bt_media_stop type= %x,device id = %x",stream_type,device_id);
#ifdef __BT_ONE_BRING_TWO__
    TRACE("bt_media_stop media_active = %x,%x,curr_active_media = %x",
            bt_meida.media_active[0],bt_meida.media_active[1], bt_meida.curr_active_media);
#else
    TRACE("bt_media_stop media_active = %x,curr_active_media = %x",
            bt_meida.media_active[0], bt_meida.curr_active_media);

#endif

    switch(stream_type)
    {
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
        case BT_STREAM_AI_VOICE:
            bt_media_smartvoice_stop_process(stream_type,device_id);
            break;
#endif	
        case BT_STREAM_SBC:
            {
                uint8_t media_pre_sbc = bt_meida.media_curr_sbc;
                TRACE("SBC STOPPING");
                ////if current media is sbc ,stop the sbc streaming
                bt_media_clear_media_type(stream_type,device_id);
                //if current stream is the stop one ,so stop it
                if(bt_media_get_current_media() == BT_STREAM_SBC && bt_meida.media_curr_sbc  == device_id)
                {
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    bt_media_clear_current_media();
                    TRACE("SBC STOPED!");

                }
                if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
                {
                    enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
                    if(sbc_id < BT_DEVICE_NUM)
                    {
                        bt_meida.media_curr_sbc =sbc_id;
                    }
                }
                else
                {
                    bt_meida.media_curr_sbc = BT_DEVICE_NUM;
                }
                if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
                {
                    //ASSERT(bt_media_get_current_media() == BT_STREAM_VOICE);
                }
                else if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
                {
                    //do nothing
                }
            #if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
    			else if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
                    //do nothing
                }
            #endif
                else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
                {
                    enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_device_by_type(BT_STREAM_SBC);
                    if(sbc_id < BT_DEVICE_NUM && media_pre_sbc!= bt_meida.media_curr_sbc)
                    {
                        app_audio_manager_switch_a2dp(sbc_id);
                        bt_media_set_current_media(BT_STREAM_SBC);
                        app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                                (uint8_t)(APP_BT_SETTING_SETUP),
                                (uint32_t)(app_bt_device.sample_rate[sbc_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
                    }
                }
            }
            break;
#ifdef MEDIA_PLAYER_SUPPORT
        case BT_STREAM_MEDIA:
            bt_media_clear_media_type(BT_STREAM_MEDIA ,device_id);
            // bt_media_set_current_media(0);

            if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
            {
                //also have media report so do nothing
            }
            else if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
            {
                if(bt_media_get_current_media() == BT_STREAM_VOICE)
                {
                    //do nothing
                }
                else if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                {
                    ///media report is end ,so goto voice
                    //  app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    uint8_t curr_sco_id;
                    curr_sco_id = app_audio_manager_get_active_sco_num();
                    if (curr_sco_id!=BT_DEVICE_NUM){
                        bt_media_set_media_type(BT_STREAM_VOICE, (enum BT_DEVICE_ID_T)curr_sco_id);
                        bt_media_set_current_media(BT_STREAM_VOICE);
#ifdef __BT_ONE_BRING_TWO__
                        app_audio_manager_swap_sco((enum BT_DEVICE_ID_T)curr_sco_id);
#endif
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_OPEN, 0);
                    }
                }

            }
            else if (btapp_hfp_get_call_state())
            {
                //do nothing
            }
        #if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
			else if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_OPEN, 0);
                bt_media_set_current_media(BT_STREAM_AI_VOICE);
			} 
        #endif	    
            else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
            {
                ///if another device is also in sbc mode
                enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_sbc_device();
                //app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                bt_media_set_media_type(BT_STREAM_SBC, sbc_id);
                app_audio_manager_switch_a2dp(sbc_id);
                bt_media_set_current_media(BT_STREAM_SBC);
                app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                        (uint8_t)(APP_BT_SETTING_SETUP),
                        (uint32_t)(app_bt_device.sample_rate[sbc_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
            }
            else
            {
                //have no meida task,so goto idle
                //app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                bt_media_set_current_media(0);
            }
            break;
#endif
        case BT_STREAM_VOICE:
            app_audio_manager_set_active_sco_num(BT_DEVICE_NUM);
            bt_media_clear_media_type(stream_type, device_id);
#ifdef MEDIA_PLAYER_SUPPORT
            if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
            {
                if(bt_media_get_current_media() == BT_STREAM_MEDIA)
                {
                    //do nothing
                }
            #if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
				else if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
					bt_media_clear_all_media_type();
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_OPEN, 0);
                    bt_media_set_current_media(BT_STREAM_AI_VOICE);
                }
            #endif	
                else if (bt_media_get_current_media() == BT_STREAM_VOICE)
                {
                    TRACE("!!!!! WARNING VOICE if voice and media is all on,media should be the current media");
                    if(!bt_media_is_media_active_by_type(BT_STREAM_VOICE)){
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    }
                }
                else if (bt_media_get_current_media() == BT_STREAM_SBC)
                {
                    TRACE("!!!!! WARNING SBC if voice and media is all on,media should be the current media");
                    if(!bt_media_is_media_active_by_type(BT_STREAM_SBC)){
                        app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    }

                }

            }
            else
#endif
                if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
                {
                    bt_media_set_current_media(BT_STREAM_VOICE);
#ifdef __BT_ONE_BRING_TWO__
                    if(Besbt_get_sco_num()>1
#ifdef CHIP_BEST1000
                            && hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2
#endif
                      ){
                        TRACE("!!!!!!!!!bt_media_stop,but another soc need connect");
                        enum BT_DEVICE_ID_T voice_dev_id  = bt_media_get_active_device_by_type(BT_STREAM_VOICE);
                        app_audio_manager_swap_sco(voice_dev_id);
#ifdef __HF_KEEP_ONE_ALIVE__
                        enum BT_DEVICE_ID_T other_voice_dev_id  = (voice_dev_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;;
                        if (HF_IsAudioConnected(&app_bt_device.hf_channel[other_voice_dev_id])){
                            TRACE("%s Disconnect another AudioLink", __func__);
                            app_bt_HF_DisconnectAudioLink(&app_bt_device.hf_channel[other_voice_dev_id]);
                        }
#endif
                    }
#endif
                }else if (btapp_hfp_get_call_state())
                {
                    TRACE("stop in HF_CALL_ACTIVE and no sco need");
                    bt_media_set_current_media(0);
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    bt_media_clear_media_type(BT_STREAM_VOICE, device_id);
                }
#if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)				
            else if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE)) {
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_OPEN, 0);
                bt_media_set_current_media(BT_STREAM_AI_VOICE);
            } 
#endif				
                else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
                {
                    ///if another device is also in sbc mode
                    enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_sbc_device();
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                    app_audio_manager_switch_a2dp(sbc_id);
                    bt_media_set_current_media(BT_STREAM_SBC);
                    app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                            (uint8_t)(APP_BT_SETTING_SETUP),
                            (uint32_t)(app_bt_device.sample_rate[sbc_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
                }
                else
                {
                    bt_media_set_current_media(0);
                    app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                }
            break;
#ifdef MEDIA_PLAYER_RBCODEC
        case BT_STREAM_RBCODEC:
            bt_media_rbcodec_stop_process(stream_type, device_id, 0);
            break;
#endif
#ifdef AUDIO_LINEIN
        case BT_STREAM_LINEIN:                  
            if(bt_media_is_media_active_by_type(BT_STREAM_LINEIN))
            { 
                 app_audio_sendrequest((uint8_t)APP_PLAY_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                 if(bt_media_get_current_media() == BT_STREAM_LINEIN)
                    bt_media_set_current_media(0);

                 bt_media_clear_media_type(stream_type,device_id);  
            }
            break;
#endif
        default:
            ASSERT(0,"bt_media_close ERROR TYPE: %x", stream_type);
            break;
    }
#ifdef __BT_ONE_BRING_TWO__
    TRACE("bt_media_stop end media_active = %x,%x,curr_active_media = %x",
            bt_meida.media_active[0],bt_meida.media_active[1], bt_meida.curr_active_media);
#else
    TRACE("bt_media_stop end media_active = %x,curr_active_media = %x",
            bt_meida.media_active[0], bt_meida.curr_active_media);

#endif
}


void app_media_stop_media(uint8_t stream_type,enum BT_DEVICE_ID_T device_id)
{
#ifdef MEDIA_PLAYER_SUPPORT
    if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
    {
#ifdef __AUDIO_QUEUE_SUPPORT__
        ////should have no sbc
        app_audio_list_clear();
#endif
        if(bt_media_get_current_media() == BT_STREAM_MEDIA)
        {
            TRACE("%s stop the media", __func__);
            bt_media_set_current_media(0);
            app_audio_sendrequest((uint8_t)APP_PLAY_BACK_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE, 0);
        }
        bt_media_clear_media_type(BT_STREAM_MEDIA, device_id);
        if(bt_media_is_media_active_by_type(BT_STREAM_VOICE))
        {
            enum BT_DEVICE_ID_T currScoId = BT_DEVICE_NUM;
            currScoId = (enum BT_DEVICE_ID_T)app_audio_manager_get_active_sco_num();

            if (currScoId == BT_DEVICE_NUM){
                for (uint8_t i=0; i<BT_DEVICE_NUM; i++){
                    if (bt_media_is_media_active_by_device(BT_STREAM_VOICE, (enum BT_DEVICE_ID_T)i)){
                        currScoId = device_id;
                        break;
                    }
                }
            }

            TRACE("%s try to resume sco:%d", __func__, currScoId);
            if (currScoId!=BT_DEVICE_NUM){
                bt_media_set_media_type(BT_STREAM_VOICE,currScoId);
                bt_media_set_current_media(BT_STREAM_VOICE);
#ifdef __BT_ONE_BRING_TWO__
                app_audio_manager_swap_sco(currScoId);
#endif
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_OPEN, 0);
            }
        }
    #if defined(__SMARTVOICE__)||defined(__AMA_VOICE__)||defined(__DMA_VOICE__)
		else if(bt_media_is_media_active_by_type(BT_STREAM_AI_VOICE))
		{
			app_audio_sendrequest((uint8_t)APP_BT_STREAM_AI_VOICE, (uint8_t)APP_BT_SETTING_OPEN, 0);
			bt_media_set_current_media(APP_BT_STREAM_AI_VOICE);
		}
    #endif    
        else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
        {
            enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_sbc_device();
            bt_media_set_media_type(BT_STREAM_SBC,device_id);
            app_audio_manager_switch_a2dp(sbc_id);
            bt_media_set_current_media(BT_STREAM_SBC);
            app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                    (uint8_t)(APP_BT_SETTING_SETUP),
                    (uint32_t)(app_bt_device.sample_rate[sbc_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
            app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
        }
    }
#endif
}

void bt_media_switch_to_voice(uint8_t stream_type,enum BT_DEVICE_ID_T device_id)
{

    TRACE("bt_media_switch_to_voice stream_type= %x,device_id=%x ",stream_type,device_id);
#ifdef __BT_ONE_BRING_TWO__
    TRACE("bt_media_switch_to_voice media_active = %x,%x,curr_active_media = %x",
            bt_meida.media_active[0],bt_meida.media_active[1], bt_meida.curr_active_media);
#else
    TRACE("bt_media_switch_to_voice media_active = %x,curr_active_media = %x",
            bt_meida.media_active[0], bt_meida.curr_active_media);

#endif



    ///already in voice ,so return
    if(bt_media_get_current_media() == BT_STREAM_VOICE){
#ifdef __BT_ONE_BRING_TWO__
        app_audio_manager_swap_sco(device_id);
#ifdef __HF_KEEP_ONE_ALIVE__
        enum BT_DEVICE_ID_T other_device_id = (device_id == BT_DEVICE_ID_1) ? BT_DEVICE_ID_2 : BT_DEVICE_ID_1;
        if (HF_IsAudioConnected(&app_bt_device.hf_channel[other_device_id])){
            TRACE("%s Disconnect another AudioLink", __func__);
            app_bt_HF_DisconnectAudioLink(&app_bt_device.hf_channel[other_device_id]);
        }
#endif
#endif
        return;
    }
    app_media_stop_media(stream_type,device_id);


}

void app_media_update_media(uint8_t stream_type,enum BT_DEVICE_ID_T device_id)
{
    TRACE("%s ",__func__);

#ifdef MEDIA_PLAYER_SUPPORT
    if(bt_media_is_media_active_by_type(BT_STREAM_MEDIA))
    {
        //do nothing
        TRACE("skip BT_STREAM_MEDIA");
    }
    else
#endif
        if(bt_media_is_media_active_by_type(BT_STREAM_VOICE) || btapp_hfp_get_call_state())
        {
            //do nothing
            TRACE("skip BT_STREAM_VOICE");
            TRACE("DEBUG INFO actByVoc:%d %d %d",    bt_media_is_media_active_by_type(BT_STREAM_VOICE),
                    btapp_hfp_get_call_state(),
                    btapp_hfp_incoming_calls());
        }
        else if(bt_media_is_media_active_by_type(BT_STREAM_SBC))
        {
            ///if another device is also in sbc mode
            TRACE("try to resume sbc");
            enum BT_DEVICE_ID_T sbc_id  = bt_media_get_active_sbc_device();
            if(bt_media_get_current_media() != BT_STREAM_SBC){
                app_audio_manager_switch_a2dp(sbc_id);
                bt_media_set_current_media(BT_STREAM_SBC);
                app_audio_sendrequest( (uint8_t)APP_BT_STREAM_A2DP_SBC,
                        (uint8_t)(APP_BT_SETTING_SETUP),
                        (uint32_t)(app_bt_device.sample_rate[sbc_id] & A2D_SBC_IE_SAMP_FREQ_MSK));
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_OPEN, 0);
            }
        }else{
            TRACE("skip idle");
        }
}

int app_audio_manager_sco_status_checker(void)
{
    CmgrHandler    *cmgrHandler;
    uint32_t scoTransmissionInterval_reg;

    TRACE("%s enter", __func__);
    if (bt_meida.media_curr_sco != BT_DEVICE_NUM){
        BTDIGITAL_REG_GET_FIELD(0xD0220120, 0xff, 24, scoTransmissionInterval_reg);
        cmgrHandler = &app_bt_device.hf_channel[bt_meida.media_curr_sco].cmgrHandler;
        if (CMGR_IsAudioUp(cmgrHandler)){
            if (cmgrHandler->scoConnect->scoLinkType == BLT_ESCO){
                if (cmgrHandler->scoConnect->scoRxParms.scoTransmissionInterval != scoTransmissionInterval_reg){
                    BTDIGITAL_REG_SET_FIELD(0xD0220120, 0xff, 24, cmgrHandler->scoConnect->scoRxParms.scoTransmissionInterval);
                }
            }
        }
        TRACE("curSco:%d type:%d Interval:%d Interval_reg:%d", bt_meida.media_curr_sco,
                cmgrHandler->scoConnect->scoLinkType,
                cmgrHandler->scoConnect->scoRxParms.scoTransmissionInterval,
                scoTransmissionInterval_reg);

        DUMP8("%02x ", app_bt_device.hf_channel[bt_meida.media_curr_sco].cmgrHandler.remDev->bdAddr.addr, 6);
#if defined(HFP_1_6_ENABLE)
        uint32_t code_type;
        uint32_t code_type_reg;
        code_type = app_audio_manager_get_scocodecid();
        code_type_reg = BTDIGITAL_REG(0xD0222000);
        if (code_type == HF_SCO_CODEC_MSBC) {
            BTDIGITAL_REG(0xD0222000) = (code_type_reg & (~(7<<1))) | (3<<1);
            TRACE("MSBC!!!!!!!!!!!!!!!!!!! REG:0xD0222000=0x%08x B:%d", BTDIGITAL_REG(0xD0222000), (BTDIGITAL_REG(0xD0222000)>>15)&1);

        }else{
            BTDIGITAL_REG(0xD0222000) = (code_type_reg & (~(7<<1))) | (2<<1);
            TRACE("CVSD!!!!!!!!!!!!!!!!!!! REG:0xD0222000=0x%08x B:%d", BTDIGITAL_REG(0xD0222000), (BTDIGITAL_REG(0xD0222000)>>15)&1);
        }
#else
        uint32_t code_type_reg;
        code_type_reg = BTDIGITAL_REG(0xD0222000);
        BTDIGITAL_REG(0xD0222000) = (code_type_reg & (~(7<<1))) | (2<<1);
        TRACE("CVSD!!!!!!!!!!!!!!!!!!! REG:0xD0222000=0x%08x B:%d", BTDIGITAL_REG(0xD0222000), (BTDIGITAL_REG(0xD0222000)>>15)&1);
#endif
    }
    TRACE("%s exit", __func__);
    return 0;
}

int app_audio_manager_swap_sco(enum BT_DEVICE_ID_T id)
{
    BT_BD_ADDR bdAdd;
    if (HF_GetRemoteBDAddr(&app_bt_device.hf_channel[id], &bdAdd)){
        TRACE("%s switch_sco to id:%d sco:%x", __func__, id, app_bt_device.hf_channel[id].cmgrHandler.scoConnect);

        DUMP8("%02x ", app_bt_device.hf_channel[id].cmgrHandler.remDev->bdAddr.addr, 6);

        TRACE("state:%d type:%d hdl:%x ",   app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scostate,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoLinkType,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoHciHandle);
        TRACE("tx[bw:%d rbw:%d ml:%d vs:%d rt:%d pktyp:%d]", app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.transmitBandwidth,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.receiveBandwidth,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.maxLatency,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.voiceSetting,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.retransmissionEffort,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoTxParms.eScoPktType);

        TRACE("rx[itv:%d ret:%d rxl:%d txl:%d]",   app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoRxParms.scoTransmissionInterval,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoRxParms.scoRetransmissionWindow,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoRxParms.scoRxPacketLen,
                app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoRxParms.scoTxPacketLen);

        app_audio_manager_set_active_sco_num(id);
        app_bt_Me_switch_sco(app_bt_device.hf_channel[id].cmgrHandler.scoConnect->scoHciHandle);
        app_bt_stream_volume_ptr_update(bdAdd.addr);
        app_bt_stream_volumeset(app_bt_stream_volume_get_ptr()->hfp_vol);
#if defined(HFP_1_6_ENABLE)
        if (bt_sco_player_get_codetype()!=
                app_audio_manager_get_scocodecid()){
            TRACE("%s try restart", __func__);
            bt_sco_player_forcemute(true,true);
            app_audio_sendrequest((uint8_t)APP_BT_STREAM_HFP_PCM, (uint8_t)APP_BT_SETTING_RESTART, 0);
        }else
#endif
        {
            app_audio_manager_sco_status_checker();
        }
    }
    return 0;
}


static bool app_audio_manager_init = false;


int app_audio_manager_sendrequest(uint8_t massage_id,uint8_t stream_type, uint8_t device_id, uint8_t aud_id)
{
    uint32_t audevt;
    uint32_t msg0;
    APP_MESSAGE_BLOCK msg;

    if(app_audio_manager_init == false)
        return -1;

    msg.mod_id = APP_MODUAL_AUDIO_MANAGE;
    APP_AUDIO_MANAGER_SET_MESSAGE(audevt, massage_id, stream_type);
    APP_AUDIO_MANAGER_SET_MESSAGE0(msg0,device_id,aud_id);
    msg.msg_body.message_id = audevt;
    msg.msg_body.message_ptr = msg0;
    msg.msg_body.message_Param0 = msg0;
    app_mailbox_put(&msg);

    return 0;
}

static int app_audio_manager_handle_process(APP_MESSAGE_BODY *msg_body)
{
    int nRet = -1;

    APP_AUDIO_MANAGER_MSG_STRUCT aud_manager_msg;

    if(app_audio_manager_init == false)
        return -1;

    APP_AUDIO_MANAGER_GET_ID(msg_body->message_id, aud_manager_msg.id);
    APP_AUDIO_MANAGER_GET_STREAM_TYPE(msg_body->message_id, aud_manager_msg.stream_type);
    APP_AUDIO_MANAGER_GET_DEVICE_ID(msg_body->message_Param0, aud_manager_msg.device_id);
    APP_AUDIO_MANAGER_GET_AUD_ID(msg_body->message_Param0, aud_manager_msg.aud_id);

    switch (aud_manager_msg.id ) {
        case APP_BT_STREAM_MANAGER_START:
            bt_media_start(aud_manager_msg.stream_type,(enum BT_DEVICE_ID_T) aud_manager_msg.device_id,(AUD_ID_ENUM)aud_manager_msg.aud_id);
            break;
        case APP_BT_STREAM_MANAGER_STOP:
            bt_media_stop(aud_manager_msg.stream_type, (enum BT_DEVICE_ID_T)aud_manager_msg.device_id);
            break;
        case APP_BT_STREAM_MANAGER_SWITCHTO_SCO:
            bt_media_switch_to_voice(aud_manager_msg.stream_type, (enum BT_DEVICE_ID_T)aud_manager_msg.device_id);
            break;
        case APP_BT_STREAM_MANAGER_STOP_MEDIA:
            app_media_stop_media(aud_manager_msg.stream_type, (enum BT_DEVICE_ID_T)aud_manager_msg.device_id);
            break;
        case APP_BT_STREAM_MANAGER_UPDATE_MEDIA:
            app_media_update_media(aud_manager_msg.stream_type, (enum BT_DEVICE_ID_T)aud_manager_msg.device_id);
            break;
        case APP_BT_STREAM_MANAGER_SWAP_SCO:
            app_audio_manager_swap_sco((enum BT_DEVICE_ID_T)aud_manager_msg.device_id);
            break;
        default:
            break;
    }

    return nRet;
}

void bt_media_volume_ptr_update_by_mediatype(uint8_t stream_type)
{
    BT_BD_ADDR bdAdd;
    BtRemoteDevice *remDev = NULL;
    uint8_t id;

    TRACE("%s enter", __func__);
    if (stream_type == bt_media_get_current_media()){
        switch (stream_type) {
            case BT_STREAM_SBC:
                id = bt_meida.media_curr_sbc;
                ASSERT(id<BT_DEVICE_NUM, "INVALID_BT_DEVICE_NUM");
                remDev = A2DP_GetRemoteDevice(app_bt_device.a2dp_connected_stream[id]);
                if (remDev){
                    CMGR_GetBdAddr(remDev, &bdAdd);
                    app_bt_stream_volume_ptr_update(bdAdd.addr);
                }

                break;
            case BT_STREAM_VOICE:
                id = app_audio_manager_get_active_sco_num();
                ASSERT(id<BT_DEVICE_NUM, "INVALID_BT_DEVICE_NUM");
                if (HF_GetRemoteBDAddr(&app_bt_device.hf_channel[id], &bdAdd)){
                    app_bt_stream_volume_ptr_update(bdAdd.addr);
                }
                break;
            case BT_STREAM_MEDIA:
            default:
                break;
        }
    }
    TRACE("%s exit", __func__);
}

int app_audio_manager_set_active_sco_num(enum BT_DEVICE_ID_T id)
{
    bt_meida.media_curr_sco = id;
    return 0;
}

int app_audio_manager_get_active_sco_num(void)
{
    return bt_meida.media_curr_sco;
}

#if defined(HFP_1_6_ENABLE)
#ifdef __BT_ONE_BRING_TWO__
static uint16_t app_audio_manage_scocodecid[BT_DEVICE_NUM] = {HF_SCO_CODEC_CVSD, HF_SCO_CODEC_CVSD};
#else
static uint16_t app_audio_manage_scocodecid[BT_DEVICE_NUM] = {HF_SCO_CODEC_CVSD};
#endif

int app_audio_manager_set_scocodecid(enum BT_DEVICE_ID_T dev_id, uint16_t codec_id)
{
    app_audio_manage_scocodecid[dev_id] = codec_id;
    return 0;
}

int app_audio_manager_get_scocodecid(void)
{
    int scocodecid = HF_SCO_CODEC_NONE;
    if (bt_meida.media_curr_sco != BT_DEVICE_NUM){
        scocodecid = app_audio_manage_scocodecid[bt_meida.media_curr_sco];
    }
    return scocodecid;
}
#endif


int app_audio_manager_switch_a2dp(enum BT_DEVICE_ID_T id)
{
    BT_BD_ADDR bdAdd;
    BtRemoteDevice *remDev = NULL;
    remDev = A2DP_GetRemoteDevice(app_bt_device.a2dp_connected_stream[id]);
    if (remDev){
        TRACE("%s switch_a2dp to id:%d", __func__, id);
        CMGR_GetBdAddr(remDev, &bdAdd);
        app_bt_stream_volume_ptr_update(bdAdd.addr);
        bt_meida.media_curr_sbc = id;
    }
    return 0;
}

bool app_audio_manager_a2dp_is_active(enum BT_DEVICE_ID_T id)
{
    uint8_t media_type;
    bool nRet = false;

    media_type = bt_media_get_current_media();
    if (media_type == BT_STREAM_SBC){
        if (bt_meida.media_curr_sbc == id){
            nRet = true;
        }
    }

    TRACE("%s nRet:%d type:%d %d/%d", __func__, nRet, media_type, id, bt_meida.media_curr_sbc);
    return nRet;
}

bool app_audio_manager_hfp_is_active(enum BT_DEVICE_ID_T id)
{
    uint8_t media_type;
    bool nRet = false;

    media_type = bt_media_get_current_media();
    if (media_type == BT_STREAM_VOICE){
        if (bt_meida.media_curr_sco == id){
            nRet = true;
        }
    }

    TRACE("%s nRet:%d type:%d %d/%d", __func__, nRet, media_type, id, bt_meida.media_curr_sco);
    return nRet;
}

void app_audio_manager_open(void)
{
    bt_meida.media_curr_sbc = BT_DEVICE_NUM;
    bt_meida.media_curr_sco = BT_DEVICE_NUM;
    bt_meida.curr_active_media = 0;
    app_set_threadhandle(APP_MODUAL_AUDIO_MANAGE, app_audio_manager_handle_process);
    app_audio_manager_init = true;
}

void app_audio_manager_close(void)
{
    app_set_threadhandle(APP_MODUAL_AUDIO_MANAGE, NULL);
    app_audio_manager_init = false;
}

#ifdef MEDIA_PLAYER_RBCODEC

static bool app_rbcodec_play_status = false;

static bool app_rbplay_player_mode = false;

bool app_rbplay_is_localplayer_mode(void)
{
    return app_rbplay_player_mode;
}

bool app_rbplay_mode_switch(void)
{
    return (app_rbplay_player_mode = !app_rbplay_player_mode);
}

void app_rbplay_set_player_mode(bool isInPlayerMode)
{
    app_rbplay_player_mode = isInPlayerMode;
}

void app_rbcodec_ctr_play_onoff(bool on )
{
    TRACE("%s %d ,turnon?%d ",__func__,app_rbcodec_play_status,on);

    if(app_rbcodec_play_status == on)
        return;
    app_rbcodec_play_status = on;
    if(on)
        app_audio_manager_sendrequest( APP_BT_STREAM_MANAGER_START, BT_STREAM_RBCODEC, 0, 0);
    else
        app_audio_manager_sendrequest( APP_BT_STREAM_MANAGER_STOP, BT_STREAM_RBCODEC, 0, 0);
}

void app_rbcodec_ctl_set_play_status(bool st)
{
    app_rbcodec_play_status = st;
    TRACE("%s %d",__func__,app_rbcodec_play_status);
}

bool app_rbcodec_get_play_status(void)
{
    TRACE("%s %d",__func__,app_rbcodec_play_status);
    return app_rbcodec_play_status;
}

void app_rbcodec_toggle_play_stop(void)
{
    if(app_rbcodec_get_play_status()) {
        app_rbcodec_ctr_play_onoff(false);
    } else {
        app_rbcodec_ctr_play_onoff(true);
    }
}

bool app_rbcodec_check_hfp_active(void )
{
    return (bool)bt_media_is_media_active_by_type(BT_STREAM_VOICE);
}
#endif


bool app_smartvoice_check_hfp_active(void)
{
    return (bool)bt_media_is_media_active_by_type(BT_STREAM_VOICE);
}


