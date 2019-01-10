#ifndef __BTAPP_H__
#define   __BTAPP_H__

#include "hid.h"
#include "hs.h"
#include "dip.h"
#include "app_key.h"
#include "hal_aud.h"
//#define __EARPHONE_STAY_BOTH_SCAN__


/* bt config */
#ifdef _SCO_BTPCM_CHANNEL_
#define SYNC_CONFIG_PATH (0<<8|1<<4|1<<0) /* all links use hci */
#else
#define SYNC_CONFIG_PATH (0<<8|0<<4|0<<0) /* all links use hci */
#endif
#define SYNC_CONFIG_MAX_BUFFER (0) /* (e)sco use Packet size */
#ifdef _CVSD_BYPASS_
#define SYNC_CONFIG_CVSD_BYPASS (1) /* use pcm over hci */
#else
#define SYNC_CONFIG_CVSD_BYPASS (0) /* use pcm over hci */
#endif
#define INQ_EXT_RSP_LEN 240


/////app sec.h
extern BtHandler pair_handler;

void pair_handler_func(const BtEvent* evt);

#if BT_SECURITY == XA_ENABLED
extern BtHandler auth_handler;
void auth_handler_func(const BtEvent* evt);
#endif




///a2dp app include
/* a2dp */
/* Default SBC codec configuration */
/* data type for the SBC Codec Information Element*/
/*****************************************************************************
**  Constants
*****************************************************************************/
/* the length of the SBC Media Payload header. */
#define A2D_SBC_MPL_HDR_LEN         1

/* the LOSC of SBC media codec capabilitiy */
#define A2D_SBC_INFO_LEN            6

/* for Codec Specific Information Element */
#ifdef A2DP_SCALABLE_ON
#define A2D_SBC_IE_SAMP_FREQ_MSK    0xF0    /* b7-b0 sampling frequency */
#else
#define A2D_SBC_IE_SAMP_FREQ_MSK    0xF0    /* b7-b4 sampling frequency */
#endif

#define A2D_SBC_IE_SAMP_FREQ_16     0x80    /* b7:16  kHz */
#define A2D_SBC_IE_SAMP_FREQ_32     0x40    /* b6:32  kHz */
#define A2D_SBC_IE_SAMP_FREQ_44     0x20    /* b5:44.1kHz */
#define A2D_SBC_IE_SAMP_FREQ_48     0x10    /* b4:48  kHz */

#ifdef A2DP_SCALABLE_ON
#define A2D_SBC_IE_SAMP_FREQ_96     0xF0    /* b4:48  kHz */
#endif

#define A2D_SBC_IE_CH_MD_MSK        0x0F    /* b3-b0 channel mode */
#define A2D_SBC_IE_CH_MD_MONO       0x08    /* b3: mono */
#define A2D_SBC_IE_CH_MD_DUAL       0x04    /* b2: dual */
#define A2D_SBC_IE_CH_MD_STEREO     0x02    /* b1: stereo */
#define A2D_SBC_IE_CH_MD_JOINT      0x01    /* b0: joint stereo */

#define A2D_SBC_IE_BLOCKS_MSK       0xF0    /* b7-b4 number of blocks */
#define A2D_SBC_IE_BLOCKS_4         0x80    /* 4 blocks */
#define A2D_SBC_IE_BLOCKS_8         0x40    /* 8 blocks */
#define A2D_SBC_IE_BLOCKS_12        0x20    /* 12blocks */
#define A2D_SBC_IE_BLOCKS_16        0x10    /* 16blocks */

#define A2D_SBC_IE_SUBBAND_MSK      0x0C    /* b3-b2 number of subbands */
#define A2D_SBC_IE_SUBBAND_4        0x08    /* b3: 4 */
#define A2D_SBC_IE_SUBBAND_8        0x04    /* b2: 8 */

#define A2D_SBC_IE_ALLOC_MD_MSK     0x03    /* b1-b0 allocation mode */
#define A2D_SBC_IE_ALLOC_MD_S       0x02    /* b1: SNR */
#define A2D_SBC_IE_ALLOC_MD_L       0x01    /* b0: loundess */

#define A2D_SBC_IE_MIN_BITPOOL      2
#define A2D_SBC_IE_MAX_BITPOOL      250



#if defined(A2DP_AAC_ON)
extern AvdtpCodec a2dp_aac_avdtpcodec;
extern const unsigned char a2dp_codec_aac_elements[A2DP_AAC_OCTET_NUMBER];
#endif
#if defined(A2DP_SCALABLE_ON)
extern AvdtpCodec a2dp_scalable_avdtpcodec;
extern const unsigned char a2dp_codec_scalable_elements[A2DP_SCALABLE_OCTET_NUMBER];
#endif

//extern A2dpStream a2dp_stream;
extern AvdtpCodec a2dp_avdtpcodec;
extern const unsigned char a2dp_codec_elements[];
//extern AvrcpChannel avrcp_channel;
extern enum AUD_SAMPRATE_T a2dp_sample_rate;

#ifdef __A2DP_AVDTP_CP__
extern AvdtpContentProt a2dp_avdtpCp;
extern U8 a2dp_avdtpCp_securityData[AVDTP_MAX_CP_VALUE_SIZE];
#endif

void a2dp_callback(A2dpStream *Stream, const A2dpCallbackParms *Info);
void avrcp_init(void);
void avrcp_callback(AvrcpChannel *chnl, const AvrcpCallbackParms *Parms);

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
void a2dp_callback_source(A2dpStream *Stream, const A2dpCallbackParms *Info);
void app_a2dp_source_init(void);
void app_a2dp_source_find_sink(void);
void avrcp_source_callback_TG(AvrcpChannel *chnl, const AvrcpCallbackParms *Parms);
void app_a2dp_start_stream(void);
void app_a2dp_suspend_stream(void);
#endif



//#define AVRCP_TRACK_CHANGED

#define AVRCP_KEY_NULL                  0
#define AVRCP_KEY_STOP                  1
#define AVRCP_KEY_PLAY                  2
#define AVRCP_KEY_PAUSE                 3
#define AVRCP_KEY_FORWARD               4
#define AVRCP_KEY_BACKWARD              5
#define AVRCP_KEY_VOLUME_UP             6
#define AVRCP_KEY_VOLUME_DOWN           7

#define HFP_KEY_ANSWER_CALL             8
#define HFP_KEY_HANGUP_CALL             9
#define HFP_KEY_REDIAL_LAST_CALL        10
#define HFP_KEY_CHANGE_TO_PHONE         11
#define HFP_KEY_ADD_TO_EARPHONE         12
#define HFP_KEY_MUTE                    13
#define HFP_KEY_CLEAR_MUTE              14
//3way calls oper
#define HFP_KEY_THREEWAY_HOLD_AND_ANSWER              15
#define HFP_KEY_THREEWAY_HANGUP_AND_ANSWER             16
#define HFP_KEY_THREEWAY_HOLD_REL_INCOMING            17
#ifdef __BT_ONE_BRING_TWO__
#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER      18
#define HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER  19
#define HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER    20
#endif

//hsp
#define HSP_KEY_CKPD_CONTROL     	21
#define HSP_KEY_ADD_TO_EARPHONE 	22
#define HSP_KEY_CHANGE_TO_PHONE	23

///////hfp app include

#define HF_SENDBUFF_SIZE (320)
#define HF_SENDBUFF_MEMPOOL_NUM (2)

struct HF_SENDBUFF_CONTROL_T {
	struct HF_SENDBUFF_MEMPOOL_T {
		BtPacket packet;
		uint8_t buffer[HF_SENDBUFF_SIZE];
	} mempool[HF_SENDBUFF_MEMPOOL_NUM];
	uint8_t index;
};


//extern HfChannel hf_channel;
#ifndef _SCO_BTPCM_CHANNEL_
extern struct HF_SENDBUFF_CONTROL_T  hf_sendbuff_ctrl;
#endif

void hfp_callback(HfChannel *Chan, HfCallbackParms *Info);
void hsp_callback(HsChannel *Chan, HsCallbackParms *Info);
#define APP_REPORT_SPEAKER_VOL_CMD 	0x01
#define APP_CPKD_CMD				0x02

void btapp_hfp_report_speak_gain(void);

#if HF_VERSION_1_6 == XA_ENABLED
uint8_t btapp_hfp_check_msbc_status(HfChannel *Chan);
#endif

/* HfAudioConnectState */
typedef U8 HfAudioConnectState;

#define HF_AUDIO_CON           1
#define HF_AUDIO_DISCON        0

/* End of HfAudioConnectState */


#define HF_VOICE_DISABLE  XA_DISABLED
#define HF_VOICE_ENABLE   XA_ENABLED

enum BT_DEVICE_ID_T{
    BT_DEVICE_ID_1 = 0,
#ifdef __BT_ONE_BRING_TWO__
    BT_DEVICE_ID_2,
#endif
    BT_DEVICE_NUM
};

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
enum BT_DEVICE_SRC_SNK_T{
	BT_DEVICE_SNK=0,
	BT_DEVICE_SRC
};
#endif

struct BT_DEVICE_T{
    A2dpStream a2dp_stream[BT_DEVICE_NUM];
#if defined(A2DP_AAC_ON)    
    A2dpStream a2dp_aac_stream[BT_DEVICE_NUM];
#endif
#if defined(A2DP_SCALABLE_ON)
    A2dpStream a2dp_scalable_stream[BT_DEVICE_NUM];
#endif
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	uint8_t src_or_snk;//src or snk fkag
	uint8_t input_onoff;
	BT_BD_ADDR         inquried_snk_bdAddr;           /* Device Address */
	//BT_BD_ADDR         rmt_bdAddr;           /* Device Address */
#endif

    A2dpStream * a2dp_connected_stream[BT_DEVICE_NUM];
    enum BT_DEVICE_ID_T curr_a2dp_stream_id;
    uint8_t a2dp_state[BT_DEVICE_NUM];
    uint8_t a2dp_streamming[BT_DEVICE_NUM];
    uint8_t a2dp_play_pause_flag;
    uint8_t sample_rate[BT_DEVICE_NUM];
    uint8_t codec_type[BT_DEVICE_NUM];
    AvrcpChannel avrcp_channel[BT_DEVICE_NUM];
    HfChannel hf_channel[BT_DEVICE_NUM];
#if defined (__HSP_ENABLE__)
    HsChannel hs_channel[BT_DEVICE_NUM];
#endif
    enum BT_DEVICE_ID_T curr_hf_channel_id;
    HfCallSetupState  hfchan_callSetup[BT_DEVICE_NUM];
    HfCallActiveState hfchan_call[BT_DEVICE_NUM];
    HfAudioConnectState hf_audio_state[BT_DEVICE_NUM];
    uint32_t hf_callsetup_time[BT_DEVICE_NUM];
#if defined (__HSP_ENABLE__)
    enum BT_DEVICE_ID_T curr_hs_channel_id;
    HsCallActiveState hschan_call[BT_DEVICE_NUM];
    HsAudioConnectState hs_audio_state[BT_DEVICE_NUM];	
#endif
#if AVRCP_ADVANCED_CONTROLLER == XA_ENABLED	
    AvrcpAdvancedPdu *avrcp_cmd1[BT_DEVICE_NUM];
    AvrcpAdvancedPdu *avrcp_cmd2[BT_DEVICE_NUM];
    AvrcpAdvancedPdu *avrcp_get_capabilities_rsp[BT_DEVICE_NUM];
    AvrcpAdvancedPdu *avrcp_control_rsp[BT_DEVICE_NUM];
    AvrcpAdvancedPdu *avrcp_notify_rsp[BT_DEVICE_NUM];
    uint8_t volume_report[BT_DEVICE_NUM];
#ifdef AVRCP_TRACK_CHANGED    
    uint8_t track_changed[BT_DEVICE_NUM];
#endif    
#endif
    uint8_t hf_conn_flag[BT_DEVICE_NUM];
    uint8_t hf_voice_en[BT_DEVICE_NUM];
    uint8_t hf_endcall_dis[BT_DEVICE_NUM];
    uint8_t hf_mute_flag;
    uint8_t phone_earphone_mark;
#if defined (__HSP_ENABLE__)
    uint8_t hs_conn_flag[BT_DEVICE_NUM];
    uint8_t hs_voice_en[BT_DEVICE_NUM];
    uint8_t hs_mute_flag;
#endif
#if HID_DEVICE == XA_ENABLED
    HidChannel  hid_channel[BT_DEVICE_NUM];
#endif
#if DIP_DEVICE == XA_ENABLED
    DipClient  dip_client[BT_DEVICE_NUM];
#endif
	uint8_t app_vid_of_phone;

};



struct BT_DEVICE_ID_DIFF{
    enum BT_DEVICE_ID_T id;
#ifdef __BT_ONE_BRING_TWO__
    enum BT_DEVICE_ID_T id_other;
#endif
};





#if  defined(__BT_RECONNECT__)&&defined(__BT_HFP_RECONNECT__)

struct BT_HF_RECONNECT_T{
    u8  Times[BT_DEVICE_NUM];
    osTimerId TimerID[BT_DEVICE_NUM];
    OsTimerNotify TimerNotifyFunc[BT_DEVICE_NUM];
    HfChannel *copyChan[BT_DEVICE_NUM];
    BT_BD_ADDR copyAddr[BT_DEVICE_NUM];
};

#endif





/////app key handle include
void a2dp_handleKey(void);
void hfp_handle_key(void);
void hsp_handle_key(void);
void btapp_a2dp_report_speak_gain(void);

#ifdef __POWERKEY_CTRL_ONOFF_ONLY__
#define   BTAPP_FUNC_KEY			APP_KEY_CODE_FN1
#define   BTAPP_VOLUME_UP_KEY		APP_KEY_CODE_FN2
#define   BTAPP_VOLUME_DOWN_KEY		APP_KEY_CODE_FN3
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
#define   BTAPP_RELEASE_KEY			APP_KEY_CODE_NONE
#endif
#else
#define   BTAPP_FUNC_KEY			APP_KEY_CODE_PWR
#define   BTAPP_VOLUME_UP_KEY		APP_KEY_CODE_FN1
#define   BTAPP_VOLUME_DOWN_KEY		APP_KEY_CODE_FN2
#if HF_CUSTOM_FEATURE_SUPPORT & HF_CUSTOM_FEATURE_SIRI_REPORT
#define   BTAPP_RELEASE_KEY			APP_KEY_CODE_NONE
#endif
#ifdef HM_LIVE_UI
#define BT_APP_PAIR_KEY             APP_KEY_CODE_FN3
#define BT_APP_PLAY_KEY				APP_KEY_CODE_FN4
#endif
#endif
void bt_key_init(void);
void bt_key_send(APP_KEY_STATUS *status);
void bt_key_handle(void);

void bt_sbc_player_set_codec_type(uint8_t type);
uint8_t bt_sbc_player_get_codec_type(void);

bool avdtp_Get_aacEnable_Flag( BtRemoteDevice* remDev, AvdtpStream *strm);

#endif
