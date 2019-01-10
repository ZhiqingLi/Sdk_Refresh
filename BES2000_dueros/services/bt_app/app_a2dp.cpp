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
#include "bt_drv.h"
#include "app_audio.h"
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrecord_dev.h"
#include "hal_location.h"

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


#include "rtos.h"
#include "besbt.h"

#include "cqueue.h"
#include "btapp.h"
#include "app_bt.h"
#include "apps.h"
#include "resources.h"
#include "app_bt_media_manager.h"
#include "tgt_hardware.h"
#include "bt_drv_interface.h"

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
#include "app_a2dp_source.h"
#endif

#ifdef __THIRDPARTY
#include "app_thirdparty.h"
#endif

#ifdef __DMA_VOICE__
#include "app_dma_handle.h"
#endif


typedef uint8_t tx_done_flag;

#define TX_DONE_FLAG_INIT           ((uint8_t)0)
#define TX_DONE_FLAG_SUCCESS        ((uint8_t)1)
#define TX_DONE_FLAG_FAIL           ((uint8_t)2)
#define TX_DONE_FLAG_TXING          ((uint8_t)3)
tx_done_flag vol_ctrl_done_flag[BT_DEVICE_NUM] = {TX_DONE_FLAG_INIT};

static int a2dp_volume_get(enum BT_DEVICE_ID_T id);

static int a2dp_volume_set(enum BT_DEVICE_ID_T id, uint8_t vol);

#ifdef __A2DP_AVDTP_CP__
AvdtpContentProt a2dp_avdtpCp;
U8 a2dp_avdtpCp_securityData[AVDTP_MAX_CP_VALUE_SIZE]=
{
};
#endif

AvdtpCodec a2dp_avdtpcodec;
#if defined(A2DP_AAC_ON)
AvdtpCodec a2dp_aac_avdtpcodec;
#endif
#if defined(A2DP_SCALABLE_ON)
AvdtpCodec a2dp_scalable_avdtpcodec;
#endif

//A2dpStream a2dp_stream;
/* avrcp */
//AvrcpChannel avrcp_channel;
#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
typedef struct {
    AvrcpAdvancedPdu pdu;
    uint8_t para_buf[40];
}APP_A2DP_AVRCPADVANCEDPDU;
osPoolId   app_a2dp_avrcpadvancedpdu_mempool = NULL;

osPoolDef (app_a2dp_avrcpadvancedpdu_mempool, 10, APP_A2DP_AVRCPADVANCEDPDU);

#define app_a2dp_avrcpadvancedpdu_mempool_init() do{ \
                                                    if (app_a2dp_avrcpadvancedpdu_mempool == NULL) \
                                                        app_a2dp_avrcpadvancedpdu_mempool = osPoolCreate(osPool(app_a2dp_avrcpadvancedpdu_mempool)); \
                                                  }while(0);

#define app_a2dp_avrcpadvancedpdu_mempool_calloc(buf)  do{ \
                                                        APP_A2DP_AVRCPADVANCEDPDU * avrcpadvancedpdu; \
                                                        avrcpadvancedpdu = (APP_A2DP_AVRCPADVANCEDPDU *)osPoolCAlloc(app_a2dp_avrcpadvancedpdu_mempool); \
                                                        buf = &(avrcpadvancedpdu->pdu); \
                                                        buf->parms = avrcpadvancedpdu->para_buf; \
                                                     }while(0);

#define app_a2dp_avrcpadvancedpdu_mempool_free(buf)  do{ \
                                                        osPoolFree(app_a2dp_avrcpadvancedpdu_mempool, buf); \
                                                     }while(0);
#endif

void get_value1_pos(U8 mask,U8 *start_pos, U8 *end_pos)
{
    U8 num = 0;
    
    for(U8 i=0;i<8;i++){
        if((0x01<<i) & mask){
            *start_pos = i;//start_pos,end_pos stands for the start and end position of value 1 in mask
            break;
        }
    }
    for(U8 i=0;i<8;i++){
        if((0x01<<i) & mask)
            num++;//number of value1 in mask
    }
    *end_pos = *start_pos + num - 1;
}
U8 get_valid_bit(U8 elements, U8 mask)
{
    U8 start_pos,end_pos;
    
    get_value1_pos(mask,&start_pos,&end_pos);
//    TRACE("!!!start_pos:%d,end_pos:%d\n",start_pos,end_pos);
    for(U8 i = start_pos; i <= end_pos; i++){
        if((0x01<<i) & elements){
            elements = ((0x01<<i) | (elements & (~mask)));
            break;
        }
    }
    return elements;
}


struct BT_DEVICE_T  app_bt_device;

#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
void a2dp_init(void)
{
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	struct nvrecord_env_t *nvrecord_env;
	nv_record_env_get(&nvrecord_env);
	app_bt_device.src_or_snk=nvrecord_env->src_snk_flag.src_snk_mode;
//	app_bt_device.src_or_snk=BT_DEVICE_SRC;
//	app_bt_device.src_or_snk=BT_DEVICE_SNK;
	TRACE("src_or_snk:%d\n",app_bt_device.src_or_snk);
	app_bt_device.input_onoff=0;
#endif

    for(uint8_t i=0; i<BT_DEVICE_NUM; i++)
    {
        app_bt_device.a2dp_state[i]=0;
        app_bt_device.a2dp_streamming[i] = 0;
        app_bt_device.avrcp_get_capabilities_rsp[i] = NULL;
        app_bt_device.avrcp_control_rsp[i] = NULL;
        app_bt_device.avrcp_notify_rsp[i] = NULL;
        app_bt_device.avrcp_cmd1[i] = NULL;
        app_bt_device.avrcp_cmd2[i] = NULL;
        app_bt_device.a2dp_connected_stream[i] = NULL;
    }

    app_a2dp_avrcpadvancedpdu_mempool_init();

    app_bt_device.a2dp_state[BT_DEVICE_ID_1]=0;
    app_bt_device.a2dp_play_pause_flag = 0;
    app_bt_device.curr_a2dp_stream_id= BT_DEVICE_ID_1;
}

FRAM_TEXT_LOC uint8_t bt_sbc_player_get_codec_type(void)
{
    enum BT_DEVICE_ID_T st_id = app_bt_device.curr_a2dp_stream_id;
    if(app_bt_device.a2dp_connected_stream[st_id])
        return app_bt_device.a2dp_connected_stream[st_id]->stream.codec->codecType;
    else
        return AVDTP_CODEC_TYPE_SBC;
}

void avrcp_callback(AvrcpChannel *chnl, const AvrcpCallbackParms *Parms)
{
    TRACE("avrcp_callback : chnl %p, Parms %p\n", chnl, Parms);
    TRACE("::Parms->event %d\n", Parms->event);
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T device_id = (chnl == &app_bt_device.avrcp_channel[0])?BT_DEVICE_ID_1:BT_DEVICE_ID_2;
#else
    enum BT_DEVICE_ID_T device_id = BT_DEVICE_ID_1;
#endif
    switch(Parms->event)
    {
        case AVRCP_EVENT_CONNECT_IND:
            TRACE("::AVRCP_EVENT_CONNECT_IND %d\n", Parms->event);
            AVRCP_ConnectRsp(chnl, 1);
            break;
        case AVRCP_EVENT_CONNECT:
            if(0)//(chnl->avrcpVersion >=0x103)
            {
                TRACE("::AVRCP_GET_CAPABILITY\n");
                if (app_bt_device.avrcp_cmd1[device_id] == NULL)
                    app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd1[device_id]);
                AVRCP_CtGetCapabilities(chnl,app_bt_device.avrcp_cmd1[device_id],AVRCP_CAPABILITY_EVENTS_SUPPORTED);
            }
#if 1
			if (app_bt_device.avrcp_cmd2[device_id] == NULL)                
                app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd1[device_id]);            
            AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd1[device_id],AVRCP_EID_MEDIA_STATUS_CHANGED,0);
#endif
			
#ifdef AVRCP_TRACK_CHANGED            
            if (app_bt_device.avrcp_cmd2[device_id] == NULL)                
                app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd2[device_id]);            
            AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd2[device_id],AVRCP_EID_TRACK_CHANGED,0);
#endif    
            TRACE("::AVRCP_EVENT_CONNECT %x\n", chnl->avrcpVersion);
            break;
        case AVRCP_EVENT_DISCONNECT:
            TRACE("::AVRCP_EVENT_DISCONNECT");
            if (app_bt_device.avrcp_get_capabilities_rsp[device_id]){
                app_a2dp_avrcpadvancedpdu_mempool_free(app_bt_device.avrcp_get_capabilities_rsp[device_id]);
                app_bt_device.avrcp_get_capabilities_rsp[device_id] = NULL;
            }
            if (app_bt_device.avrcp_control_rsp[device_id]){
                app_a2dp_avrcpadvancedpdu_mempool_free(app_bt_device.avrcp_control_rsp[device_id]);
                app_bt_device.avrcp_control_rsp[device_id] = NULL;
            }
            if (app_bt_device.avrcp_notify_rsp[device_id]){
                app_a2dp_avrcpadvancedpdu_mempool_free(app_bt_device.avrcp_notify_rsp[device_id]);
                app_bt_device.avrcp_notify_rsp[device_id] = NULL;
            }
     
            if (app_bt_device.avrcp_cmd1[device_id]){
                app_a2dp_avrcpadvancedpdu_mempool_free(app_bt_device.avrcp_cmd1[device_id]);
                app_bt_device.avrcp_cmd1[device_id] = NULL;
            }       
            if (app_bt_device.avrcp_cmd2[device_id]){
                app_a2dp_avrcpadvancedpdu_mempool_free(app_bt_device.avrcp_cmd2[device_id]);
                app_bt_device.avrcp_cmd2[device_id] = NULL;
            }                   
            app_bt_device.volume_report[device_id] = 0;
#ifdef AVRCP_TRACK_CHANGED            
            app_bt_device.track_changed[device_id] = 0;
#endif    
            break;       
        case AVRCP_EVENT_RESPONSE:
            TRACE("::AVRCP_EVENT_ADV_RESPONSE op=%x,status=%x\n", Parms->advOp,Parms->status);
        
            break;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
/*For Sony Compability Consideration*/            
        case AVRCP_EVENT_PANEL_PRESS:
            TRACE("::avrcp_callback_TG AVRCP_EVENT_PANEL_PRESS %x,%x,device_id=%d",
                Parms->p.panelCnf.operation,Parms->p.panelInd.operation,device_id);
				
            switch(Parms->p.panelInd.operation)
            {
                case AVRCP_POP_VOLUME_UP:
                    TRACE("avrcp_callback_TG avrcp_key = AVRCP_KEY_VOLUME_UP");
                    app_bt_stream_volumeup();
                    break;
                case AVRCP_POP_VOLUME_DOWN:
                    TRACE("avrcp_callback_TG avrcp_key = AVRCP_KEY_VOLUME_DOWN"); 
                    app_bt_stream_volumedown();
                    break;
				//SRC:for accept play/pause command from snk
				case AVRCP_POP_PLAY:
					TRACE("::avrcp start stream !");
					app_a2dp_start_stream();
					break;
				case AVRCP_POP_PAUSE:
					TRACE("::acrcp stop stream !");
					app_a2dp_suspend_stream();
					break;
                default :
                    break;
            }
            break;
#else
/*For Sony Compability Consideration*/            
        case AVRCP_EVENT_PANEL_PRESS:
            TRACE("::avrcp_callback_TG AVRCP_EVENT_PANEL_PRESS %x,%x,device_id=%d",
                Parms->p.panelCnf.operation,Parms->p.panelInd.operation,device_id);
            switch(Parms->p.panelInd.operation)
            {
                case AVRCP_POP_VOLUME_UP:
                    TRACE("avrcp_callback_TG avrcp_key = AVRCP_KEY_VOLUME_UP");
                    app_bt_stream_volumeup();
                    break;
                case AVRCP_POP_VOLUME_DOWN:
                    TRACE("avrcp_callback_TG avrcp_key = AVRCP_KEY_VOLUME_DOWN"); 
                    app_bt_stream_volumedown();
                    break;
                default :
                    break;
            }
            break;

#endif
        case AVRCP_EVENT_PANEL_HOLD:
            TRACE("::avrcp_callback_TG AVRCP_EVENT_PANEL_HOLD %x,%x",
                Parms->p.panelCnf.operation,Parms->p.panelInd.operation);
            break;
        case AVRCP_EVENT_PANEL_RELEASE:
            TRACE("::avrcp_callback_TG AVRCP_EVENT_PANEL_RELEASE %x,%x",
                Parms->p.panelCnf.operation,Parms->p.panelInd.operation);
            break;      
/*For Sony Compability Consideration End*/               
        case AVRCP_EVENT_PANEL_CNF:
            TRACE("::AVRCP_EVENT_PANEL_CNF %x,%x,%x",
                Parms->p.panelCnf.response,Parms->p.panelCnf.operation,Parms->p.panelCnf.press);
            break;
        case AVRCP_EVENT_ADV_TX_DONE:
            TRACE("::AVRCP_EVENT_ADV_TX_DONE op:%d\n", Parms->p.adv.txPdu->op);
            if (Parms->p.adv.txPdu->op == AVRCP_OP_GET_CAPABILITIES){
                if (app_bt_device.avrcp_get_capabilities_rsp[device_id] == Parms->p.adv.txPdu){
                    app_bt_device.avrcp_get_capabilities_rsp[device_id] = NULL;
                    app_a2dp_avrcpadvancedpdu_mempool_free(Parms->p.adv.txPdu);
                }
            }
            if(app_bt_device.avrcp_notify_rsp[device_id]->ctype ==AVCTP_RESPONSE_INTERIM)
            {
                    vol_ctrl_done_flag[device_id] = TX_DONE_FLAG_SUCCESS;
                    //LOG_E("send AVCTP_RESPONSE_INTERIM successful device id %d = TX_DONE_FLAG_SUCCESS",device_id);
            }
#if 0
            if (Parms->p.adv.txPdu->op == AVRCP_OP_SET_ABSOLUTE_VOLUME){
                if (Parms->p.adv.txPdu->ctype != AVCTP_RESPONSE_INTERIM){
                    if (app_bt_device.avrcp_control_rsp[device_id] == Parms->p.adv.txPdu){
                        app_bt_device.avrcp_control_rsp[device_id] = NULL;
                        app_a2dp_avrcpadvancedpdu_mempool_free(Parms->p.adv.txPdu);
                    }
                }
            }
            if (Parms->p.adv.txPdu->op == AVRCP_OP_REGISTER_NOTIFY){
                if (Parms->p.adv.txPdu->ctype != AVCTP_RESPONSE_INTERIM){
                    if (Parms->p.adv.txPdu->parms[0] == AVRCP_EID_VOLUME_CHANGED){
                        app_bt_device.avrcp_notify_rsp[device_id] = NULL;
                        app_a2dp_avrcpadvancedpdu_mempool_free(Parms->p.adv.txPdu);
                    }
                }
            }
#endif

            break;
        case AVRCP_EVENT_ADV_RESPONSE:
            TRACE("::AVRCP_EVENT_ADV_RESPONSE op=%x,status=%x\n", Parms->advOp,Parms->status);
            if(Parms->advOp == AVRCP_OP_GET_CAPABILITIES && Parms->status == BT_STATUS_SUCCESS)
            {
                TRACE("::AVRCP eventmask=%x\n", Parms->p.adv.rsp.capability.info.eventMask);
                chnl->adv.rem_eventMask = Parms->p.adv.rsp.capability.info.eventMask;
                if(chnl->adv.rem_eventMask & AVRCP_ENABLE_PLAY_STATUS_CHANGED)
                {
                    TRACE("::AVRCP send notification\n");
					TRACE("SUPPORT PLAY STATUS CHANGE!!!!!!!!");
                    if (app_bt_device.avrcp_cmd1[device_id] == NULL)
                        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd1[device_id]);
                    AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd1[device_id],AVRCP_EID_MEDIA_STATUS_CHANGED,0);

                }
                if(chnl->adv.rem_eventMask & AVRCP_ENABLE_PLAY_POS_CHANGED)
                {
                    if (app_bt_device.avrcp_cmd2[device_id] == NULL)
                        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd2[device_id]);
                
                    AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd2[device_id],AVRCP_EID_PLAY_POS_CHANGED,1);
                    
                }
            }
            else if(Parms->advOp == AVRCP_OP_REGISTER_NOTIFY && Parms->status == BT_STATUS_SUCCESS)
            {
                   if(Parms->p.adv.notify.event == AVRCP_EID_MEDIA_STATUS_CHANGED) 
                   {
                   		TRACE("==============PLAY STATUS CHANGE=============");
                        TRACE("::ACRCP notify rsp playback states=%x",Parms->p.adv.notify.p.mediaStatus);
                   // app_bt_device.a2dp_state = Parms->p.adv.notify.p.mediaStatus;
                   }
                   else if(Parms->p.adv.notify.event == AVRCP_EID_PLAY_POS_CHANGED)
                  {
                        TRACE("::ACRCP notify rsp play pos =%x",Parms->p.adv.notify.p.position);
                  }
                   else if(Parms->p.adv.notify.event == AVRCP_EID_VOLUME_CHANGED){
                        TRACE("::ACRCP notify rsp volume =%x",Parms->p.adv.notify.p.volume);
                        a2dp_volume_set(device_id, Parms->p.adv.notify.p.volume);
                   }
#ifdef AVRCP_TRACK_CHANGED                   
                    else if(Parms->p.adv.notify.event == AVRCP_EID_TRACK_CHANGED){                        
                        if (app_bt_device.avrcp_notify_rsp[device_id] == NULL)
                            app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_notify_rsp[device_id]);
                        app_bt_device.avrcp_notify_rsp[device_id]->ctype = AVCTP_RESPONSE_INTERIM;
                        app_bt_device.track_changed[device_id] = AVCTP_RESPONSE_INTERIM;
                        AVRCP_CtGetMediaInfo(chnl, app_bt_device.avrcp_notify_rsp[device_id],0x7f);
                    }
#endif                   
            }
#ifdef AVRCP_TRACK_CHANGED            
            else if(Parms->advOp == AVRCP_OP_GET_MEDIA_INFO && Parms->status == BT_STATUS_SUCCESS){
                TRACE("AVRCP_TRACK_CHANGED numid=%d",Parms->p.adv.rsp.element.numIds);
                for(uint8_t i=0;i<7;i++)
                {
                    if(Parms->p.adv.rsp.element.txt[i].length>0)
                    {
                        TRACE("Id=%d,%s\n",i,Parms->p.adv.rsp.element.txt[i].string);
                    }
                }
                
            }
#endif            
            break;
        case AVRCP_EVENT_COMMAND:
#ifndef __AVRCP_EVENT_COMMAND_VOLUME_SKIP__
            TRACE("::AVRCP_EVENT_COMMAND ctype=%x,subunitype=%x\n", Parms->p.cmdFrame->ctype,Parms->p.cmdFrame->subunitType);
            TRACE("::AVRCP_EVENT_COMMAND subunitId=%x,opcode=%x\n", Parms->p.cmdFrame->subunitId,Parms->p.cmdFrame->opcode);
            TRACE("::AVRCP_EVENT_COMMAND operands=%x,operandLen=%x\n", Parms->p.cmdFrame->operands,Parms->p.cmdFrame->operandLen);
            TRACE("::AVRCP_EVENT_COMMAND more=%x\n", Parms->p.cmdFrame->more);
            if(Parms->p.cmdFrame->ctype == AVRCP_CTYPE_STATUS)
            {
                uint32_t company_id = *(Parms->p.cmdFrame->operands+2) + ((uint32_t)(*(Parms->p.cmdFrame->operands+1))<<8) + ((uint32_t)(*(Parms->p.cmdFrame->operands))<<16);
                TRACE("::AVRCP_EVENT_COMMAND company_id=%x\n", company_id);
                if(company_id == 0x001958)  //bt sig
                {
                    AvrcpOperation op = *(Parms->p.cmdFrame->operands+3);
                    uint8_t oplen =  *(Parms->p.cmdFrame->operands+6)+ ((uint32_t)(*(Parms->p.cmdFrame->operands+5))<<8);
                    TRACE("::AVRCP_EVENT_COMMAND op=%x,oplen=%x\n", op,oplen);
                    switch(op)
                    {
                        case AVRCP_OP_GET_CAPABILITIES:
                        {
                                uint8_t event = *(Parms->p.cmdFrame->operands+7);
                                if(event==AVRCP_CAPABILITY_COMPANY_ID)
                                {
                                    TRACE("::AVRCP_EVENT_COMMAND send support compay id");
                                }
                                else if(event == AVRCP_CAPABILITY_EVENTS_SUPPORTED)
                                {
                                    TRACE("::AVRCP_EVENT_COMMAND send support event transId:%d", Parms->p.cmdFrame->transId);
                                    chnl->adv.eventMask = AVRCP_ENABLE_VOLUME_CHANGED;   ///volume control
                                    if (app_bt_device.avrcp_get_capabilities_rsp[device_id] == NULL)
                                        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_get_capabilities_rsp[device_id]);
                                    app_bt_device.avrcp_get_capabilities_rsp[device_id]->transId = Parms->p.cmdFrame->transId;
                                    app_bt_device.avrcp_get_capabilities_rsp[device_id]->ctype = AVCTP_RESPONSE_IMPLEMENTED_STABLE;
                                    TRACE("::AVRCP_EVENT_COMMAND send support event transId:%d", app_bt_device.avrcp_get_capabilities_rsp[device_id]->transId);
                                    AVRCP_CtGetCapabilities_Rsp(chnl,app_bt_device.avrcp_get_capabilities_rsp[device_id],AVRCP_CAPABILITY_EVENTS_SUPPORTED,chnl->adv.eventMask);
                                }
                                else
                                {
                                    TRACE("::AVRCP_EVENT_COMMAND send error event value");
                                }
                        }
                        break;
                    }
                    
                }
                
            }else if(Parms->p.cmdFrame->ctype == AVCTP_CTYPE_CONTROL){        
                TRACE("::AVRCP_EVENT_COMMAND AVCTP_CTYPE_CONTROL\n");
                DUMP8("%02x ", Parms->p.cmdFrame->operands, Parms->p.cmdFrame->operandLen);
                if (Parms->p.cmdFrame->operands[3] == AVRCP_OP_SET_ABSOLUTE_VOLUME){
                    TRACE("::AVRCP_EID_VOLUME_CHANGED transId:%d\n", Parms->p.cmdFrame->transId);
                    a2dp_volume_set(device_id, Parms->p.cmdFrame->operands[7]);                    
                    if (app_bt_device.avrcp_control_rsp[device_id] == NULL)
                        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_control_rsp[device_id]);
                    app_bt_device.avrcp_control_rsp[device_id]->transId = Parms->p.cmdFrame->transId;
                    app_bt_device.avrcp_control_rsp[device_id]->ctype = AVCTP_RESPONSE_ACCEPTED;
                    DUMP8("%02x ", Parms->p.cmdFrame->operands, Parms->p.cmdFrame->operandLen);
                    AVRCP_CtAcceptAbsoluteVolume_Rsp(chnl, app_bt_device.avrcp_control_rsp[device_id], Parms->p.cmdFrame->operands[7]);
                }
            }else if (Parms->p.cmdFrame->ctype == AVCTP_CTYPE_NOTIFY){
                BtStatus status;
                TRACE("::AVRCP_EVENT_COMMAND AVCTP_CTYPE_NOTIFY\n");
                DUMP8("%02x ", Parms->p.cmdFrame->operands, Parms->p.cmdFrame->operandLen);
                if (Parms->p.cmdFrame->operands[7] == AVRCP_EID_VOLUME_CHANGED){
					TRACE("VOLUME CHANGED<<<<<<<<<<<<<<====================>>>>>>>>>>>>>>>>>>>====================");
                    TRACE("::AVRCP_EID_VOLUME_CHANGED transId:%d\n", Parms->p.cmdFrame->transId);
                    if (app_bt_device.avrcp_notify_rsp[device_id] == NULL)
                        app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_notify_rsp[device_id]);
                    app_bt_device.avrcp_notify_rsp[device_id]->transId = Parms->p.cmdFrame->transId;
                    app_bt_device.avrcp_notify_rsp[device_id]->ctype = AVCTP_RESPONSE_INTERIM;
                    app_bt_device.volume_report[device_id] = AVCTP_RESPONSE_INTERIM;
                    status = AVRCP_CtGetAbsoluteVolume_Rsp(chnl, app_bt_device.avrcp_notify_rsp[device_id], a2dp_volume_get(device_id));
                    TRACE("::AVRCP_EVENT_COMMAND AVRCP_EID_VOLUME_CHANGED nRet:%x\n",status);
                }
            }
#endif             
            break;
        case AVRCP_EVENT_ADV_NOTIFY:
            if(Parms->p.adv.notify.event == AVRCP_EID_VOLUME_CHANGED)
            {
                    TRACE("::ACRCP notify  vol =%x",Parms->p.adv.notify.p.volume);
                    AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_notify_rsp[device_id],AVRCP_EID_VOLUME_CHANGED,0);
            }
           else if(Parms->p.adv.notify.event == AVRCP_EID_MEDIA_STATUS_CHANGED) 
           {
                TRACE("::ACRCP notify  playback states=%x",Parms->p.adv.notify.p.mediaStatus);
                if (app_bt_device.avrcp_cmd1[device_id] == NULL)
                    app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd1[device_id]);
                AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd1[device_id],AVRCP_EID_MEDIA_STATUS_CHANGED,0);
           // app_bt_device.a2dp_state = Parms->p.adv.notify.p.mediaStatus;
           }
           else if(Parms->p.adv.notify.event == AVRCP_EID_PLAY_POS_CHANGED)
          {
                TRACE("::ACRCP notify  play pos =%x",Parms->p.adv.notify.p.position);
                if (app_bt_device.avrcp_cmd2[device_id] == NULL)
                    app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd2[device_id]);
                AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd2[device_id],AVRCP_EID_PLAY_POS_CHANGED,1);
          }  
#ifdef AVRCP_TRACK_CHANGED            
         else if(Parms->p.adv.notify.event == AVRCP_EID_TRACK_CHANGED){
            TRACE("::AVRCP notify track msU32=%x, lsU32=%x",Parms->p.adv.notify.p.track.msU32,Parms->p.adv.notify.p.track.lsU32);
            if (app_bt_device.avrcp_cmd2[device_id] == NULL)
                app_a2dp_avrcpadvancedpdu_mempool_calloc(app_bt_device.avrcp_cmd2[device_id]);
            AVRCP_CtRegisterNotification(chnl,app_bt_device.avrcp_cmd2[device_id],AVRCP_EID_TRACK_CHANGED,0);
            }
#endif       
            
            break;
    }
}
#else
void a2dp_init(void)
{
    for(uint8_t i=0; i<BT_DEVICE_NUM; i++)
    {
        app_bt_device.a2dp_state[i]=0;
    }

    app_bt_device.a2dp_state[BT_DEVICE_ID_1]=0;
    app_bt_device.a2dp_play_pause_flag = 0;
    app_bt_device.curr_a2dp_stream_id= BT_DEVICE_ID_1;
}

void avrcp_callback(AvrcpChannel *chnl, const AvrcpCallbackParms *Parms)
{
    TRACE("avrcp_callback : chnl %p, Parms %p\n", chnl, Parms);
    TRACE("::Parms->event %d\n", Parms->event);
    switch(Parms->event)
    {
        case AVRCP_EVENT_CONNECT_IND:
            TRACE("::AVRCP_EVENT_CONNECT_IND %d\n", Parms->event);
            AVRCP_ConnectRsp(chnl, 1);
            break;
        case AVRCP_EVENT_CONNECT:
            TRACE("::AVRCP_EVENT_CONNECT %d\n", Parms->event);
            break;
        case AVRCP_EVENT_RESPONSE:
            TRACE("::AVRCP_EVENT_RESPONSE %d\n", Parms->event);
            
            break;
        case AVRCP_EVENT_PANEL_CNF:
            TRACE("::AVRCP_EVENT_PANEL_CNF %x,%x,%x",
                Parms->p.panelCnf.response,Parms->p.panelCnf.operation,Parms->p.panelCnf.press);
#if 0            
            if((Parms->p.panelCnf.response == AVCTP_RESPONSE_ACCEPTED) && (Parms->p.panelCnf.press == TRUE))
            {
                AVRCP_SetPanelKey(chnl,Parms->p.panelCnf.operation,FALSE);
            }
#endif            
            break;
    }
}
#endif
//void avrcp_init(void)
//{
//  hal_uart_open(HAL_UART_ID_0,NULL);
//    TRACE("avrcp_init...OK\n");
//}

#if defined(A2DP_AAC_ON)
const unsigned char a2dp_codec_aac_elements[A2DP_AAC_OCTET_NUMBER] =
{
    A2DP_AAC_OCTET0_MPEG2_AAC_LC,
    A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100,
    A2DP_AAC_OCTET2_CHANNELS_1|A2DP_AAC_OCTET2_CHANNELS_2 |A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000,
    A2DP_AAC_OCTET3_VBR_SUPPORTED,
    /* left bit rate 0 for unkown */
    0,0
};
#endif

#if defined(A2DP_SCALABLE_ON)
const unsigned char a2dp_codec_scalable_elements[A2DP_SCALABLE_OCTET_NUMBER] =
{
    0x75,0x0,0x0,0x0, // vendor id
    0x03,0x01,          // vendor specific codec id
    0xf8,                 // vendor specific value
};
#endif

const unsigned char a2dp_codec_elements[] =
{
    A2D_SBC_IE_SAMP_FREQ_48 | A2D_SBC_IE_SAMP_FREQ_44 | A2D_SBC_IE_CH_MD_STEREO | A2D_SBC_IE_CH_MD_JOINT,
    A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 | A2D_SBC_IE_SUBBAND_8 | A2D_SBC_IE_ALLOC_MD_L,
    A2D_SBC_IE_MIN_BITPOOL,
    BTA_AV_CO_SBC_MAX_BITPOOL
};

int store_sbc_buffer(unsigned char *buf, unsigned int len);
int a2dp_audio_sbc_set_frame_info(int rcv_len, int frame_num);

void btapp_send_pause_key(enum BT_DEVICE_ID_T stream_id)
{
    TRACE("btapp_send_pause_key id = %x",stream_id);
    AVRCP_SetPanelKey(&(app_bt_device.avrcp_channel[stream_id]),AVRCP_POP_PAUSE,TRUE);
    AVRCP_SetPanelKey(&(app_bt_device.avrcp_channel[stream_id]),AVRCP_POP_PAUSE,FALSE);
 //   app_bt_device.a2dp_play_pause_flag = 0;
}


void btapp_a2dp_suspend_music(enum BT_DEVICE_ID_T stream_id)
{
    TRACE("btapp_a2dp_suspend_music id = %x",stream_id);

    btapp_send_pause_key(stream_id);
}

extern enum AUD_SAMPRATE_T a2dp_sample_rate;

#define A2DP_TIMESTAMP_TRACE(s,...)
//TRACE(s, ##__VA_ARGS__)

#define A2DP_TIMESTAMP_DEBOUNCE_DURATION (1000)
#define A2DP_TIMESTAMP_MODE_SAMPLE_THRESHOLD (2000)

#define A2DP_TIMESTAMP_SYNC_LIMIT_CNT (100)
#define A2DP_TIMESTAMP_SYNC_TIME_THRESHOLD (60)
#define A2DP_TIMESTAMP_SYNC_SAMPLE_THRESHOLD ((int64_t)a2dp_sample_rate*A2DP_TIMESTAMP_SYNC_TIME_THRESHOLD/1000)

#define RICE_THRESHOLD
#define RICE_THRESHOLD

struct A2DP_TIMESTAMP_INFO_T{
    uint16_t rtp_timestamp;
    uint32_t loc_timestamp;
    uint16_t frame_num;    
    int32_t rtp_timestamp_diff_sum;
};

enum A2DP_TIMESTAMP_MODE_T{    
    A2DP_TIMESTAMP_MODE_NONE,
    A2DP_TIMESTAMP_MODE_SAMPLE,
    A2DP_TIMESTAMP_MODE_TIME,    
};

enum A2DP_TIMESTAMP_MODE_T a2dp_timestamp_mode = A2DP_TIMESTAMP_MODE_NONE;

struct A2DP_TIMESTAMP_INFO_T a2dp_timestamp_pre = {0,0,0};
bool a2dp_timestamp_parser_need_sync = false;

int a2dp_timestamp_parser_init(void)
{
    a2dp_timestamp_mode = A2DP_TIMESTAMP_MODE_NONE;
    a2dp_timestamp_pre.rtp_timestamp = 0;
    a2dp_timestamp_pre.loc_timestamp = 0;
    a2dp_timestamp_pre.frame_num = 0;
    a2dp_timestamp_pre.rtp_timestamp_diff_sum = 0;
    a2dp_timestamp_parser_need_sync = false;
    return 0;
}

int a2dp_timestamp_parser_needsync(void)
{
    a2dp_timestamp_parser_need_sync = true;
    return 0;
}

int a2dp_timestamp_parser_run(uint16_t timestamp, uint16_t framenum)
{
    static int skip_cnt = 0;
    struct A2DP_TIMESTAMP_INFO_T curr_timestamp;
    int skipframe = 0;
    uint16_t rtpdiff;
    int32_t locdiff;
    bool needsave_rtp_timestamp = true;
    bool needsave_loc_timestamp = true;

    curr_timestamp.rtp_timestamp = timestamp;
    curr_timestamp.loc_timestamp = hal_sys_timer_get();
    curr_timestamp.frame_num = framenum;
 
    switch(a2dp_timestamp_mode) {
        case A2DP_TIMESTAMP_MODE_NONE:            

//            TRACE("parser rtp:%d loc:%d num:%d prertp:%d preloc:%d\n", curr_timestamp.rtp_timestamp, curr_timestamp.loc_timestamp, curr_timestamp.frame_num,
//                                                   a2dp_timestamp_pre.rtp_timestamp, a2dp_timestamp_pre.loc_timestamp);       
            if (a2dp_timestamp_pre.rtp_timestamp){
                locdiff = curr_timestamp.loc_timestamp - a2dp_timestamp_pre.loc_timestamp;
                if (TICKS_TO_MS(locdiff) > A2DP_TIMESTAMP_DEBOUNCE_DURATION){                    
                    rtpdiff = curr_timestamp.rtp_timestamp - a2dp_timestamp_pre.rtp_timestamp;
                    if (ABS((int16_t)TICKS_TO_MS(locdiff)-rtpdiff)>A2DP_TIMESTAMP_MODE_SAMPLE_THRESHOLD){
                        a2dp_timestamp_mode = A2DP_TIMESTAMP_MODE_SAMPLE;
                        TRACE("A2DP_TIMESTAMP_MODE_SAMPLE\n");
                    }else{
                        a2dp_timestamp_mode = A2DP_TIMESTAMP_MODE_TIME;                        
                        TRACE("A2DP_TIMESTAMP_MODE_TIME\n");
                    }
                }else{
                    needsave_rtp_timestamp = false;
                    needsave_loc_timestamp = false;
                }
            }
            break;
        case A2DP_TIMESTAMP_MODE_SAMPLE:            
            if (a2dp_timestamp_parser_need_sync){                
                skip_cnt++;                
                rtpdiff = curr_timestamp.rtp_timestamp - a2dp_timestamp_pre.rtp_timestamp;
                locdiff = curr_timestamp.loc_timestamp - a2dp_timestamp_pre.loc_timestamp;
                a2dp_timestamp_pre.rtp_timestamp_diff_sum += rtpdiff;
                
                A2DP_TIMESTAMP_TRACE("%d-%d=%d",  curr_timestamp.rtp_timestamp, a2dp_timestamp_pre.rtp_timestamp, rtpdiff);
                
                A2DP_TIMESTAMP_TRACE("%d-%d=%d",  curr_timestamp.loc_timestamp , a2dp_timestamp_pre.loc_timestamp, locdiff);

                A2DP_TIMESTAMP_TRACE("%d-%d=%d", (int32_t)((int64_t)(TICKS_TO_MS64(locdiff))*(uint32_t)a2dp_sample_rate/1000), 
                                     a2dp_timestamp_pre.rtp_timestamp_diff_sum, 
                                     (int32_t)((TICKS_TO_MS64(locdiff)*a2dp_sample_rate/1000) - a2dp_timestamp_pre.rtp_timestamp_diff_sum));
                
                
                A2DP_TIMESTAMP_TRACE("A2DP_TIMESTAMP_MODE_SAMPLE SYNC diff:%d cnt:%d\n", (int32_t)((int64_t)(TICKS_TO_MS64(locdiff)*a2dp_sample_rate/1000) - a2dp_timestamp_pre.rtp_timestamp_diff_sum), skip_cnt);
                if (((int64_t)(TICKS_TO_MS64(locdiff)*a2dp_sample_rate/1000) - a2dp_timestamp_pre.rtp_timestamp_diff_sum) < (int32_t)A2DP_TIMESTAMP_SYNC_SAMPLE_THRESHOLD){                    
                    TRACE("A2DP_TIMESTAMP_MODE_SAMPLE RESYNC OK cnt:%d\n", skip_cnt);
                    skip_cnt = 0;                    
                    a2dp_timestamp_parser_need_sync = false; 
                }else if (skip_cnt > A2DP_TIMESTAMP_SYNC_LIMIT_CNT){
                    TRACE("A2DP_TIMESTAMP_MODE_SAMPLE RESYNC FORCE END\n");
                    skip_cnt = 0;
                    a2dp_timestamp_parser_need_sync = false;                
                }else{
                    needsave_loc_timestamp = false;
                    skipframe = 1;
                }
            }else{
                a2dp_timestamp_pre.rtp_timestamp_diff_sum = 0;
            }
            break;
        case A2DP_TIMESTAMP_MODE_TIME:
            if (a2dp_timestamp_parser_need_sync){                
                skip_cnt++;
                rtpdiff = curr_timestamp.rtp_timestamp - a2dp_timestamp_pre.rtp_timestamp;
                locdiff = curr_timestamp.loc_timestamp - a2dp_timestamp_pre.loc_timestamp;
                a2dp_timestamp_pre.rtp_timestamp_diff_sum += rtpdiff;
                
                A2DP_TIMESTAMP_TRACE("%d/%d/ %d/%d %d\n",  rtpdiff,a2dp_timestamp_pre.rtp_timestamp_diff_sum, 
                                            a2dp_timestamp_pre.loc_timestamp, curr_timestamp.loc_timestamp,
                                            TICKS_TO_MS(locdiff));
                A2DP_TIMESTAMP_TRACE("A2DP_TIMESTAMP_MODE_TIME SYNC diff:%d cnt:%d\n", (int32_t)ABS(TICKS_TO_MS64(locdiff) - a2dp_timestamp_pre.rtp_timestamp_diff_sum), skip_cnt);
                if (((int64_t)TICKS_TO_MS64(locdiff) - a2dp_timestamp_pre.rtp_timestamp_diff_sum) < A2DP_TIMESTAMP_SYNC_TIME_THRESHOLD){
                    TRACE("A2DP_TIMESTAMP_MODE_TIME RESYNC OK cnt:%d\n", skip_cnt);
                    skip_cnt = 0;                    
                    needsave_loc_timestamp = false;
                    a2dp_timestamp_parser_need_sync = false;                
                }else if (skip_cnt > A2DP_TIMESTAMP_SYNC_LIMIT_CNT){                
                    TRACE("A2DP_TIMESTAMP_MODE_TIME RESYNC FORCE END\n");
                    skip_cnt = 0;
                    a2dp_timestamp_parser_need_sync = false;                
                }else{            
                    needsave_loc_timestamp = false;
                    skipframe = 1;
                }
            }else{
                a2dp_timestamp_pre.rtp_timestamp_diff_sum = 0;
            }
            break;
    }

    if (needsave_rtp_timestamp){
        a2dp_timestamp_pre.rtp_timestamp = curr_timestamp.rtp_timestamp;
    }
    
    if (needsave_loc_timestamp){
        a2dp_timestamp_pre.loc_timestamp = curr_timestamp.loc_timestamp;
    }

    return skipframe;
}

static struct BT_DEVICE_ID_DIFF stream_id_flag;
#ifdef __BT_ONE_BRING_TWO__
void a2dp_stream_id_distinguish(A2dpStream *Stream)
{
    if(Stream == app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1]){
        stream_id_flag.id = BT_DEVICE_ID_1;
        stream_id_flag.id_other = BT_DEVICE_ID_2;
    }else if(Stream == app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_2]){
        stream_id_flag.id = BT_DEVICE_ID_2;
        stream_id_flag.id_other = BT_DEVICE_ID_1;
    }
}

bool a2dp_stream_push_connected_stream(A2dpStream *Stream)
{
    bool ret = 0;
    if(app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1] == Stream ||
        app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_2] == Stream){
        TRACE("a2dp_stream_push_connected_stream Stream in record st=0x%x\n", Stream);
        return ret;
    }
    
    if(app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1] == NULL){
        app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1] = Stream;
        ret = TRUE;
    }
    else if(app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_2] == NULL){
        app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_2] = Stream;
        ret = TRUE;
    }
    TRACE("a2dp_stream_push_connected_stream ret = %d 0x%x st=0x%x 0x%x\n", ret, Stream, app_bt_device.a2dp_connected_stream[0],
        app_bt_device.a2dp_connected_stream[1]);
    return ret;
}

#ifdef A2DP_AAC_ON
uint8_t is_aac_stream(A2dpStream *Stream)
{
    if((Stream == &app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_1]) 
        || (Stream == &app_bt_device.a2dp_aac_stream[BT_DEVICE_ID_2]))
        return TRUE;
    else
        return FALSE;
}
#endif
#endif


#if defined( __EARPHONE__) && defined(__BT_RECONNECT__)

#ifdef __BT_ONE_BRING_TWO__
extern BtDeviceRecord record2_copy;
extern uint8_t record2_avalible;
#endif

#endif

extern void app_bt_profile_connect_manager_a2dp(enum BT_DEVICE_ID_T id, A2dpStream *Stream, const A2dpCallbackParms *Info);

void a2dp_callback(A2dpStream *Stream, const A2dpCallbackParms *Info)
{
    int header_len = 0;
    AvdtpMediaHeader header;

#if defined(__BT_RECONNECT__)
    static AvdtpCodec   setconfig_codec;
    static u8 tmp_element[10];
#ifdef __A2DP_AVDTP_CP__    
    static AvdtpContentProt   setconfig_cp;
#endif
#endif

//    struct AF_STREAM_CONFIG_T *stream_cfg = NULL;
#if 0 //def __BT_ONE_BRING_TWO__
    if((app_bt_device.hfchan_call[BT_DEVICE_ID_2] == HF_CALL_ACTIVE) || (app_bt_device.hf_audio_state[BT_DEVICE_ID_2] == HF_AUDIO_CON)){
        switch(Info->event) {
            case A2DP_EVENT_STREAM_START_IND:
                TRACE("!!!HF_CALL_ACTIVE  A2DP_EVENT_STREAM_START_IND\n");
                A2DP_StartStreamRsp((app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1]), A2DP_ERR_NO_ERROR);
                app_bt_device.curr_a2dp_stream_id = BT_DEVICE_ID_1;
                app_bt_device.a2dp_play_pause_flag = 1;
                return;
            case A2DP_EVENT_STREAM_STARTED:
                TRACE("!!!HF_CALL_ACTIVE  A2DP_EVENT_STREAM_STARTED\n");
                app_audio_sendrequest((uint8_t)APP_BT_STREAM_A2DP_SBC, (uint8_t)APP_BT_SETTING_CLOSE, 0);
                return;
            case A2DP_EVENT_STREAM_DATA_IND:
                return;
            default:
                break;
        }
    }
#endif

#ifdef __BT_ONE_BRING_TWO__
    if((Info->event== A2DP_EVENT_STREAM_OPEN) 
        || (Info->event== A2DP_EVENT_STREAM_OPEN_IND)){
        a2dp_stream_push_connected_stream(Stream);
    }
    a2dp_stream_id_distinguish(Stream);
#else
    stream_id_flag.id = BT_DEVICE_ID_1;
    app_bt_device.a2dp_connected_stream[BT_DEVICE_ID_1] = Stream;
#endif

    switch(Info->event) {
        case A2DP_EVENT_AVDTP_CONNECT:
            TRACE("::A2DP_EVENT_AVDTP_CONNECT %d st = 0x%x\n", Info->event, Stream);
            break;
        case A2DP_EVENT_STREAM_OPEN:
            TRACE("::A2DP_EVENT_STREAM_OPEN stream_id:%d, sample_rate codec.elements 0x%x\n", stream_id_flag.id,Info->p.configReq->codec.elements[0]);  
#ifdef __DMA_VOICE__
		dma_a2dp_connected();
#endif
#ifdef __A2DP_AVDTP_CP__
            A2DP_SecurityControlReq(Stream,(U8 *)&a2dp_avdtpCp_securityData,1);
#endif
            a2dp_timestamp_parser_init();
            app_bt_stream_volume_ptr_update((uint8_t *)Stream->stream.conn.remDev->bdAddr.addr);
//            app_bt_stream_a2dpvolume_reset();
#if defined(A2DP_AAC_ON)
            TRACE("codecType 0x%d\n", Info->p.configReq->codec.codecType);
            if (Info->p.configReq->codec.codecType == AVDTP_CODEC_TYPE_MPEG2_4_AAC) {
                TRACE("::A2DP_EVENT_STREAM_OPEN stream_id:%d, aac sample_rate codec.elements 0x%x\n", stream_id_flag.id,Info->p.configReq->codec.elements[1]);
                app_bt_device.codec_type[stream_id_flag.id] = AVDTP_CODEC_TYPE_MPEG2_4_AAC;
                // convert aac sample_rate to sbc sample_rate format
                if (Info->p.configReq->codec.elements[1] & A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100) {
                    TRACE("::A2DP_EVENT_STREAM_OPEN stream_id:%d, aac sample_rate 44100\n", stream_id_flag.id);
                    app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_44;
                }
                else if (Info->p.configReq->codec.elements[2] & A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000) {
                    TRACE("::A2DP_EVENT_STREAM_OPEN stream_id:%d, aac sample_rate 48000\n", stream_id_flag.id);
                    app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_48;
                }
                else {
                    TRACE("::A2DP_EVENT_STREAM_OPEN stream_id:%d, aac sample_rate not 48000 or 44100, set to 44100\n", stream_id_flag.id);
                    app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_44;
                }
            }
            else
#endif
#if defined(A2DP_SCALABLE_ON)
            if (Info->p.configReq->codec.codecType == AVDTP_CODEC_TYPE_NON_A2DP) {
                TRACE("##codecType scalable\n");
                app_bt_device.codec_type[stream_id_flag.id] = AVDTP_CODEC_TYPE_NON_A2DP;
                app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_44;
                //app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_96;
            }else
#endif
            {
			    TRACE("app_bt_device.sample_rate::elements[0] %d\n", Info->p.codec->elements[0]);
				
                app_bt_device.codec_type[stream_id_flag.id] = AVDTP_CODEC_TYPE_SBC;
                app_bt_device.sample_rate[stream_id_flag.id] = (Info->p.configReq->codec.elements[0] & A2D_SBC_IE_SAMP_FREQ_MSK);
            }
            app_bt_device.a2dp_state[stream_id_flag.id] = 1;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
			if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
			{
				if(Info->p.configReq->codec.elements[1] & A2D_SBC_IE_SUBBAND_MSK == A2D_SBC_IE_SUBBAND_4)
					TRACE("numSubBands is only support 8!");
				a2dp_source.sample_rate =  bt_parse_sbc_sample_rate(Stream->stream.codecCfg.elements[0]);
				//TRACE("::AVRCP_Connect\n", __LINE__);
				//AVRCP_Connect(&app_bt_device.avrcp_channel[stream_id_flag.id], &Stream->stream.conn.remDev->bdAddr);
			}
			else
			{
				AVRCP_Connect(&app_bt_device.avrcp_channel[stream_id_flag.id], &Stream->stream.conn.remDev->bdAddr);
			}
#else
			AVRCP_Connect(&app_bt_device.avrcp_channel[stream_id_flag.id], &Stream->stream.conn.remDev->bdAddr);
#endif						
			app_bt_profile_connect_manager_a2dp(stream_id_flag.id, Stream, (A2dpCallbackParms *)Info);
            break;
        case A2DP_EVENT_STREAM_OPEN_IND:
            TRACE("::A2DP_EVENT_STREAM_OPEN_IND %d\n", Info->event);
            A2DP_OpenStreamRsp(Stream, A2DP_ERR_NO_ERROR, AVDTP_SRV_CAT_MEDIA_TRANSPORT);
            break;
        case A2DP_EVENT_STREAM_STARTED:
            a2dp_timestamp_parser_init();                  
            app_bt_device.a2dp_streamming[stream_id_flag.id] = 1;
#ifdef __BT_ONE_BRING_TWO__
            TRACE("::A2DP_EVENT_STREAM_STARTED %d  stream_id:%d %d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0], app_bt_device.a2dp_streamming[1]);            
            if(app_bt_device.a2dp_connected_stream[stream_id_flag.id_other] &&
                (app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]->stream.state != AVDTP_STRM_STATE_STREAMING)){
                app_bt_device.curr_a2dp_stream_id = stream_id_flag.id;
                app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id,MAX_RECORD_NUM); 
            }else if(!app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]){
                 app_bt_device.curr_a2dp_stream_id = stream_id_flag.id;
                app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id,MAX_RECORD_NUM); 
            }
#else
            TRACE("::A2DP_EVENT_STREAM_STARTED %d  stream_id:%d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0]);            
            app_bt_device.curr_a2dp_stream_id = BT_DEVICE_ID_1;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
			if(app_bt_device.src_or_snk==BT_DEVICE_SNK)
			{
			  app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id,MAX_RECORD_NUM);
			}
			else
			{
				TRACE("::APP_A2DP_SOURCE START \n");
#if defined(APP_LINEIN_A2DP_SOURCE)
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_OPEN,0);
#else
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_I2S_AUDIO, (uint8_t)APP_BT_SETTING_OPEN,0);
#endif
				app_bt_device.input_onoff=1;
			}			
#else
			app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id,MAX_RECORD_NUM);			
#endif

#endif            
#ifdef __THIRDPARTY
			//app_thirdparty_specific_lib_event_handle(THIRDPARTY_ID_NO2,THIRDPARTY_A2DP_STREAMING);
#endif

            break;
        case A2DP_EVENT_STREAM_START_IND:
            btdrv_set_powerctrl_rssi_low(0xffff);
            TRACE("::A2DP_EVENT_STREAM_START_IND stream_id:%d\n", stream_id_flag.id);
#ifdef __BT_ONE_BRING_TWO__            
            TRACE("::A2DP_EVENT_STREAM_START_IND %d  stream_id:%d %d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0], app_bt_device.a2dp_streamming[1]);
#else
            TRACE("::A2DP_EVENT_STREAM_START_IND %d  stream_id:%d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0]);
#endif
            A2DP_StartStreamRsp(Stream, A2DP_ERR_NO_ERROR);
            app_bt_device.a2dp_play_pause_flag = 1;
            break;
        case A2DP_EVENT_STREAM_SUSPENDED:
#ifdef __BT_ONE_BRING_TWO__                        
            TRACE("::A2DP_EVENT_STREAM_SUSPENDED %d  stream_id:%d %d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0], app_bt_device.a2dp_streamming[1]);
#else
            TRACE("::A2DP_EVENT_STREAM_SUSPENDED %d  stream_id:%d %d %d\n",Stream->stream.codec->codecType, stream_id_flag.id, app_bt_device.curr_a2dp_stream_id,
                            app_bt_device.a2dp_streamming[0]);
#endif
            a2dp_timestamp_parser_init();
            app_bt_device.a2dp_streamming[stream_id_flag.id] = 0;
#ifdef __BT_ONE_BRING_TWO__
            app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,stream_id_flag.id,0);
            if((app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]) && 
                (app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]->stream.state == AVDTP_STRM_STATE_STREAMING)){
                app_bt_device.curr_a2dp_stream_id = stream_id_flag.id_other;
                app_bt_device.a2dp_play_pause_flag = 1;
                app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id_other,0);
            }else{
                app_bt_device.a2dp_play_pause_flag = 0;
            }       
#else

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
			if(app_bt_device.src_or_snk==BT_DEVICE_SNK)
			{
			  app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,BT_DEVICE_ID_1,MAX_RECORD_NUM);
			}
			else
			{
				TRACE("::APP_A2DP_SOURCE SUSPEND \n");
#if defined(APP_LINEIN_A2DP_SOURCE)
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#else
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_I2S_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
				app_bt_device.input_onoff=0;
			}			
#else
			app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,BT_DEVICE_ID_1,MAX_RECORD_NUM);			
#endif
            app_bt_device.a2dp_play_pause_flag = 0;
#endif
            break;
        case A2DP_EVENT_STREAM_DATA_IND:
#ifdef __BT_ONE_BRING_TWO__
                ////play music of curr_a2dp_stream_id
                if(app_bt_device.curr_a2dp_stream_id  == stream_id_flag.id && 
                   app_bt_device.hf_audio_state[stream_id_flag.id]==HF_AUDIO_DISCON && 
                   app_bt_device.hf_audio_state[stream_id_flag.id_other]==HF_AUDIO_DISCON &&
                   app_bt_device.hfchan_callSetup[stream_id_flag.id_other] == HF_CALL_SETUP_NONE){
                    header_len = AVDTP_ParseMediaHeader(&header, Info->p.data);
                    if (app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC) && (Stream->stream.state == AVDTP_STRM_STATE_STREAMING)){
#ifdef __A2DP_TIMESTAMP_PARSER__
                        if (a2dp_timestamp_parser_run(header.timestamp,(*(((unsigned char *)Info->p.data) + header_len)))){
                            TRACE("::A2DP_EVENT_STREAM_DATA_IND skip frame\n");
                        }else
#endif
                        {
                            a2dp_audio_sbc_set_frame_info(Info->len - header_len - 1, (*(((unsigned char *)Info->p.data) + header_len)));
#if defined(A2DP_AAC_ON)
                            if (app_bt_device.codec_type[stream_id_flag.id] == AVDTP_CODEC_TYPE_MPEG2_4_AAC) {
                            	store_sbc_buffer(((unsigned char *)Info->p.data) + header_len , Info->len - header_len);
                            }
                            else
#endif
#if defined(A2DP_SCALABLE_ON)
                            if (app_bt_device.codec_type[stream_id_flag.id] == AVDTP_CODEC_TYPE_NON_A2DP) {
                                store_sbc_buffer(((unsigned char *)Info->p.data) + header_len , Info->len - header_len);
                            }else
#endif
                            store_sbc_buffer(((unsigned char *)Info->p.data) + header_len + 1 , Info->len - header_len - 1);
                        }
                    }
                }
#else
                header_len = AVDTP_ParseMediaHeader(&header, Info->p.data);
                if (app_bt_stream_isrun(APP_BT_STREAM_A2DP_SBC) && (Stream->stream.state == AVDTP_STRM_STATE_STREAMING)){
#ifdef __A2DP_TIMESTAMP_PARSER__
                    if (a2dp_timestamp_parser_run(header.timestamp,(*(((unsigned char *)Info->p.data) + header_len)))){
                        TRACE("::A2DP_EVENT_STREAM_DATA_IND skip frame\n");
                    }else
#endif
                    {
                        a2dp_audio_sbc_set_frame_info(Info->len - header_len - 1, (*(((unsigned char *)Info->p.data) + header_len)));
#if defined(A2DP_AAC_ON)
                    //TRACE("%s:%d pt 0x%x, app_bt_device.codec_type[stream_id_flag.id] %d\n", 
                    //		__func__, __LINE__, header.payloadType, app_bt_device.codec_type[stream_id_flag.id]);
                        if (app_bt_device.codec_type[stream_id_flag.id] == AVDTP_CODEC_TYPE_MPEG2_4_AAC) {
                        	store_sbc_buffer(((unsigned char *)Info->p.data) + header_len , Info->len - header_len);
                        }
                        else
#endif
#if defined(A2DP_SCALABLE_ON)
                        if (app_bt_device.codec_type[stream_id_flag.id] == AVDTP_CODEC_TYPE_NON_A2DP) {
                            store_sbc_buffer(((unsigned char *)Info->p.data) + header_len , Info->len - header_len);
                        }else
#endif
                        store_sbc_buffer(((unsigned char *)Info->p.data) + header_len + 1 , Info->len - header_len - 1);
                    }
                }
#endif
            break;
        case A2DP_EVENT_STREAM_CLOSED:
            TRACE("::A2DP_EVENT_STREAM_CLOSED stream_id:%d, reason = %x\n", stream_id_flag.id,Info->discReason);
#ifdef __DMA_VOICE__
			dma_a2dp_disconnected();
#endif			
            a2dp_timestamp_parser_init();            
#ifdef __BT_ONE_BRING_TWO__          
            app_bt_device.curr_a2dp_stream_id = stream_id_flag.id_other;
            app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,stream_id_flag.id,0);
            if((app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]) && 
                (app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]->stream.state == AVDTP_STRM_STATE_STREAMING)){
         //       app_bt_device.curr_a2dp_stream_id = stream_id_flag.id_other;
                app_bt_device.a2dp_play_pause_flag = 1;
                app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_START,BT_STREAM_SBC,stream_id_flag.id_other,0);
            }     
            else
            {
                app_bt_device.a2dp_play_pause_flag = 0;   
            }
            app_bt_device.a2dp_connected_stream[stream_id_flag.id] = NULL;            
#else
			app_bt_device.a2dp_state[stream_id_flag.id] = 0;



#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
		if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
		{
			if(app_bt_device.input_onoff!=0)
			{
#if defined(APP_LINEIN_A2DP_SOURCE)
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_LINEIN_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#else
				app_audio_sendrequest((uint8_t)APP_A2DP_SOURCE_I2S_AUDIO, (uint8_t)APP_BT_SETTING_CLOSE,0);
#endif
				app_bt_device.input_onoff=0;
			}
			app_bt_device.a2dp_streamming[BT_DEVICE_ID_1] = 0;
			a2dp_source_notify_send();
		}
		else
		{
			app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,stream_id_flag.id,0);
			app_bt_profile_connect_manager_a2dp(stream_id_flag.id, Stream, (A2dpCallbackParms *)Info);
		}		
#else
		app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,stream_id_flag.id,0);
		app_bt_profile_connect_manager_a2dp(stream_id_flag.id, Stream, (A2dpCallbackParms *)Info);			
#endif

#endif

#ifdef __BT_ONE_BRING_TWO__
        ///a2dp disconnect so check the other stream is playing or not
        if((app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]) && 
            (app_bt_device.a2dp_connected_stream[stream_id_flag.id_other]->stream.state  != AVDTP_STRM_STATE_STREAMING))
        {
            app_bt_device.a2dp_play_pause_flag = 0;    
        }
#endif
        break;
#if defined(A2DP_SCALABLE_ON)
        case A2DP_EVENT_STREAM_RECONFIG_IND:
            TRACE("::##UHQ A2DP_EVENT_STREAM_RECONFIG_IND %d\n", Info->event);
            DUMP8(" %02x", Info->p.configReq->codec.elements, 7);
            if(Info->p.configReq->codec.elements[6]==0x88){
                app_bt_device.sample_rate[stream_id_flag.id] = A2D_SBC_IE_SAMP_FREQ_96;
            }
            A2DP_ReconfigStreamRsp(Stream,A2DP_ERR_NO_ERROR,0);
            break;

        case A2DP_EVENT_STREAM_RECONFIG_CNF:
            TRACE("::A2DP_EVENT_STREAM_RECONFIG_CNF %d\n", Info->event);
            DUMP8(" %x", Info->p.configReq->codec.elements, 7);
            break;
#endif
#if defined(__BT_RECONNECT__)
        case A2DP_EVENT_CODEC_INFO:
            TRACE("::A2DP_EVENT_CODEC_INFO %d\n", Info->event);
            setconfig_codec.codecType = Info->p.codec->codecType;
            setconfig_codec.discoverable = Info->p.codec->discoverable;
            setconfig_codec.elemLen = Info->p.codec->elemLen;
            setconfig_codec.elements = tmp_element;
            memset(tmp_element, 0, sizeof(tmp_element));
            if(Info->p.codec->codecType == AVDTP_CODEC_TYPE_SBC){
                setconfig_codec.elements[0] = (Info->p.codec->elements[0]) & (a2dp_codec_elements[0]);
                setconfig_codec.elements[1] = (Info->p.codec->elements[1]) & (a2dp_codec_elements[1]);

                if(Info->p.codec->elements[2] <= a2dp_codec_elements[2])
                    setconfig_codec.elements[2] = a2dp_codec_elements[2];////[2]:MIN_BITPOOL
                else
                    setconfig_codec.elements[2] = Info->p.codec->elements[2];

                if(Info->p.codec->elements[3] >= a2dp_codec_elements[3])
                    setconfig_codec.elements[3] = a2dp_codec_elements[3];////[3]:MAX_BITPOOL
                else
                    setconfig_codec.elements[3] = Info->p.codec->elements[3];

                ///////null set situation:
                if(setconfig_codec.elements[3] < a2dp_codec_elements[2]){
                    setconfig_codec.elements[2] = a2dp_codec_elements[2];
                    setconfig_codec.elements[3] = a2dp_codec_elements[3];
                }
                else if(setconfig_codec.elements[2] > a2dp_codec_elements[3]){
                    setconfig_codec.elements[2] = a2dp_codec_elements[3];
                    setconfig_codec.elements[3] = a2dp_codec_elements[3];
                }
                TRACE("!!!setconfig_codec.elements[2]:%d,setconfig_codec.elements[3]:%d\n",setconfig_codec.elements[2],setconfig_codec.elements[3]);    

                setconfig_codec.elements[0] = get_valid_bit(setconfig_codec.elements[0],A2D_SBC_IE_SAMP_FREQ_MSK);
                setconfig_codec.elements[0] = get_valid_bit(setconfig_codec.elements[0],A2D_SBC_IE_CH_MD_MSK);
                setconfig_codec.elements[1] = get_valid_bit(setconfig_codec.elements[1],A2D_SBC_IE_BLOCKS_MSK);
                setconfig_codec.elements[1] = get_valid_bit(setconfig_codec.elements[1],A2D_SBC_IE_SUBBAND_MSK);
                setconfig_codec.elements[1] = get_valid_bit(setconfig_codec.elements[1],A2D_SBC_IE_ALLOC_MD_MSK);
            }
#if defined(A2DP_AAC_ON)            
            else if(Info->p.codec->codecType == AVDTP_CODEC_TYPE_MPEG2_4_AAC){
                setconfig_codec.elements[0] = a2dp_codec_aac_elements[0];
                if(Info->p.codec->elements[1] & A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100)
                    setconfig_codec.elements[1] |= A2DP_AAC_OCTET1_SAMPLING_FREQUENCY_44100;
                else if(Info->p.codec->elements[2] & A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000)
                    setconfig_codec.elements[2] |= A2DP_AAC_OCTET2_SAMPLING_FREQUENCY_48000;

                if(Info->p.codec->elements[2] & A2DP_AAC_OCTET2_CHANNELS_2)
                    setconfig_codec.elements[2] |= A2DP_AAC_OCTET2_CHANNELS_2;
                else if(Info->p.codec->elements[2] & A2DP_AAC_OCTET2_CHANNELS_1)
                    setconfig_codec.elements[2] |= A2DP_AAC_OCTET2_CHANNELS_1;
            }
#endif
#if defined(A2DP_SCALABLE_ON)
            else if(Info->p.codec->codecType == AVDTP_CODEC_TYPE_NON_A2DP){
                setconfig_codec.elements = (U8*)&a2dp_codec_scalable_elements;
            }
#endif
            break;
#ifdef __A2DP_AVDTP_CP__
        case A2DP_EVENT_CP_INFO:
            TRACE("::A2DP_EVENT_CP_INFO %d\n", Info->event);
            TRACE("cpType:%x\n",Info->p.cp->cpType);
            setconfig_cp.cpType = Info->p.cp->cpType;
            setconfig_cp.data = Info->p.cp->data;
            setconfig_cp.dataLen = Info->p.cp->dataLen;
            break;
#endif
        case A2DP_EVENT_GET_CONFIG_IND:
            TRACE("::A2DP_EVENT_GET_CONFIG_IND %d\n", Info->event);
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
			//I2S or LineIn for 44.1K
			setconfig_codec.elements[0] = (setconfig_codec.elements[0]&0xef) |  A2D_SBC_IE_SAMP_FREQ_44;
#endif
#ifdef __A2DP_AVDTP_CP__
            A2DP_SetStreamConfig(Stream, &setconfig_codec, &setconfig_cp);
#else
            A2DP_SetStreamConfig(Stream, &setconfig_codec, NULL);
#endif
            break;
#endif

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
		case A2DP_EVENT_STREAM_RECONFIG_IND:
			SOURCE_DBLOG("::A2DP_EVENT_STREAM_RECONFIG_IND %d\n", Info->event);
			A2DP_ReconfigStreamRsp(Stream,A2DP_ERR_NO_ERROR,0);
			break;
		case A2DP_EVENT_STREAM_RECONFIG_CNF:
			SOURCE_DBLOG("::A2DP_EVENT_STREAM_RECONFIG_CNF %d\n", Info->event);
			break;
		case A2DP_EVENT_STREAM_SBC_PACKET_SENT:
			//TRACE("@sbc be send succ");
			if(app_bt_device.src_or_snk==BT_DEVICE_SRC)
			{
				a2dp_source_notify_send();
			}		
			break;
#endif

#ifdef __A2DP_AVDTP_CP__
        case A2DP_EVENT_STREAM_SECURITY_IND:
            TRACE("::A2DP_EVENT_STREAM_SECURITY_IND %d\n", Info->event);
            DUMP8("%x ",Info->p.data,Info->len);
            A2DP_SecurityControlRsp(Stream,&Info->p.data[1],Info->len-1,Info->error);
            break;
        case A2DP_EVENT_STREAM_SECURITY_CNF:
            TRACE("::A2DP_EVENT_STREAM_SECURITY_CNF %d\n", Info->event);
            break;
#endif
    }
}

void a2dp_suspend_music_force(void)
{
    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,BT_DEVICE_ID_1,0);
#ifdef __BT_ONE_BRING_TWO__
    app_audio_manager_sendrequest(APP_BT_STREAM_MANAGER_STOP,BT_STREAM_SBC,BT_DEVICE_ID_2,0);
#endif
}

static int a2dp_volume_get(enum BT_DEVICE_ID_T id)
{
    int vol = TGT_VOLUME_LEVEL_15;
#ifndef __BT_ONE_BRING_TWO__
    vol = app_bt_stream_volume_get_ptr()->a2dp_vol;
#else
    if (app_audio_manager_a2dp_is_active(id)){
        vol = app_bt_stream_volume_get_ptr()->a2dp_vol;
    }else{
        BtRemoteDevice *remDev = NULL;        
        nvrec_btdevicerecord *record = NULL;        
        BT_BD_ADDR bdAdd;
        remDev = A2DP_GetRemoteDevice(app_bt_device.a2dp_connected_stream[id]);
        if (remDev){
            CMGR_GetBdAddr(remDev, &bdAdd);
            if (!nv_record_btdevicerecord_find(&bdAdd,&record)){
                vol = record->device_vol.a2dp_vol;
            }
        }else{
            vol = TGT_VOLUME_LEVEL_15;
        }
    }
#endif

    if (vol == TGT_VOLUME_LEVEL_MUTE){
        vol = 0;
    }else{
        if (vol >= TGT_VOLUME_LEVEL_15){
            vol = TGT_VOLUME_LEVEL_15;
        }else if (vol <= TGT_VOLUME_LEVEL_0){
            vol = TGT_VOLUME_LEVEL_0;
        }
        vol = 8*(vol-1);
    }
 
    if (vol > 0x7f)
        vol = 0x7f;

    TRACE("get vol raw:%d %d/127", app_bt_stream_volume_get_ptr()->a2dp_vol, vol);

    return (vol);
}

int app_bt_stream_volumeset(int8_t vol);

void a2dp_volume_local_set(int8_t vol)
{
    app_bt_stream_volume_get_ptr()->a2dp_vol = vol;
    #ifndef FPGA
    nv_record_touch_cause_flush();
    #endif
}

static int a2dp_volume_set(enum BT_DEVICE_ID_T id, uint8_t vol)
{
    int dest_vol;

    if (!vol){
        dest_vol = TGT_VOLUME_LEVEL_MUTE;
    }else{
        dest_vol = (((int)vol&0x7f)<<4)/128 + 2;
        if (dest_vol > TGT_VOLUME_LEVEL_15){
            dest_vol = TGT_VOLUME_LEVEL_15;
        }else if (dest_vol < TGT_VOLUME_LEVEL_0){
            dest_vol = TGT_VOLUME_LEVEL_0;
        }
    }

#ifndef __BT_ONE_BRING_TWO__    
    a2dp_volume_local_set(dest_vol);
    app_bt_stream_volumeset(dest_vol);
#else
    if (app_audio_manager_a2dp_is_active(id)){
        a2dp_volume_local_set(dest_vol);
        app_bt_stream_volumeset(dest_vol);
    }else{
        BtRemoteDevice *remDev = NULL;        
        nvrec_btdevicerecord *record = NULL;        
        BT_BD_ADDR bdAdd;
        remDev = A2DP_GetRemoteDevice(app_bt_device.a2dp_connected_stream[id]);
        if (remDev){
            CMGR_GetBdAddr(remDev, &bdAdd);
            if (!nv_record_btdevicerecord_find(&bdAdd,&record)){
                record->device_vol.a2dp_vol = dest_vol;
            }
        }
    }
#endif
    TRACE("put vol raw:%d/127 %d", vol, dest_vol);

    return (vol);
}

void btapp_a2dp_report_speak_gain(void)
{
#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED
    uint8_t i;
    int vol;
    BtRemoteDevice *remDev = NULL;
    BtLinkMode mode = BLM_SNIFF_MODE;

    for(i=0; i<BT_DEVICE_NUM; i++){
        TRACE("btapp_a2dp_report_speak_gain transId:%d a2dp_state:%d streamming:%d report:%02x\n", 
        app_bt_device.avrcp_notify_rsp[i]->transId,
        app_bt_device.a2dp_state[i],
        app_bt_device.a2dp_streamming[i],
        app_bt_device.volume_report[i]);
        OS_LockStack();
        if((vol_ctrl_done_flag[i]==TX_DONE_FLAG_INIT)||(vol_ctrl_done_flag[i]==TX_DONE_FLAG_SUCCESS)){
            remDev = A2DP_GetRemoteDevice(app_bt_device.a2dp_connected_stream[i]);
            if (remDev){
                mode = ME_GetCurrentMode(remDev);
            }else{
                mode = BLM_SNIFF_MODE;
            }
            if ((app_bt_device.a2dp_state[i] == 1) && 
                (app_bt_device.a2dp_streamming[i] == 1) &&
                (app_bt_device.volume_report[i] == AVCTP_RESPONSE_INTERIM) &&
                (mode == BLM_ACTIVE_MODE)){
                app_bt_device.volume_report[i] = AVCTP_RESPONSE_CHANGED;
                TRACE("btapp_a2dp_report_speak_gain transId:%d\n", app_bt_device.avrcp_notify_rsp[i]->transId);
                if (app_bt_device.avrcp_notify_rsp[i] != NULL){
                    app_bt_device.avrcp_notify_rsp[i]->ctype = AVCTP_RESPONSE_CHANGED;                
                    vol = a2dp_volume_get((enum BT_DEVICE_ID_T)i);
                    AVRCP_CtGetAbsoluteVolume_Rsp(&app_bt_device.avrcp_channel[i], app_bt_device.avrcp_notify_rsp[i], vol);
                        vol_ctrl_done_flag[i] = TX_DONE_FLAG_TXING;
                }
            }
        }
        OS_UnlockStack();
    }
#endif
}





bool avdtp_Get_aacEnable_Flag( BtRemoteDevice* remDev, AvdtpStream *strm)
{
    TRACE("%s,version=%x",remDev->remoteVersion[1] );
#if defined(A2DP_AAC_ON)    
    //if(((remDev->remoteVersion[1] == 0x0f) && (remDev->remoteVersion[2] == 0)) || strm->codec->codecType !=AVDTP_CODEC_TYPE_MPEG2_4_AAC){
    if((app_bt_device.app_vid_of_phone==2) || (strm->codec->codecType !=AVDTP_CODEC_TYPE_MPEG2_4_AAC)){
        return TRUE;
    }else
        return FALSE;    
#else
    return TRUE;
#endif

    
}

