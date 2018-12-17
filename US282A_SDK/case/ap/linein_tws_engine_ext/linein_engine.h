/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌引擎应用入口，包括应用初始化，应用退出等。
 * 作者：
 ********************************************************************************/

#ifndef _BTPLAY_ENGINE_H
#define _BTPLAY_ENGINE_H

#include "psp_includes.h"
#include "case_include.h"
#include "btplay_common.h"
#include "sbc_encode_interface.h"

//#define TWS_START_PLAY_SYNC    1
//#define TWS_STOP_PLAY_SYNC     2
//#define TWS_SLAVE_ENTER_SYNC   3
//#define TWS_SET_START_TIME     0x81

typedef struct
{
    uint8 reserve;
} btplay_config_t;

typedef struct
{
    bool codec_flag;
    bool tts_play_flag;
    uint8 media_type;
    uint8 aac_delay_start;
    uint8 aac_delay_lowest;
    uint8 sbc_delay_start;
    uint8 sbc_delay_lowest;
	output_channel_e output_channel_config;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    uint8 tws_m_output_channel;
    uint8 tws_s_output_channel;
#endif
    uint16 filter_after_pause; //用户在小机按暂停后过滤一小段时间AVTDP音频数据包
} btplay_global_variable_struct;

extern app_result_e btplay_deal_app_msg(void);
extern app_result_e get_message_loop(void);

extern uint32 read_sbc_data(uint8 *middle_buffer, uint32 need_data_length, uint32 return_status);
extern void a2dp_decode_init(uint8 media_type);
extern void a2dp_decode_quit(void);

extern btplay_global_variable_struct btplay_gl_var;
extern btplay_config_t g_btplay_cfg;



extern void *mp_handle;

extern bt_stack_pipe_t *g_p_play_pipe;
extern bt_stack_info_t *g_p_bt_stack_cur_info;
extern bt_stack_twsinfo_t *g_p_tws_info;

extern btplay_info_t g_btplay_info[2];
extern btplay_info_t *g_p_btplay_info;

#ifdef WAVES_ASET_TOOLS
extern waves_t g_waves;
#endif

//for dae adjust freq
extern dae_config_t *g_dae_cfg_shm;

//for chage current save
extern uint32 g_cha_need_restore;

//for count 
extern uint32 g_check_count;

extern uint32 g_cha_current;

extern uint8 g_cha_sta;


#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern void send_mmm_exit_cmd(void);
extern void send_mmm_exit_cmd2(uint8 op);
extern void send_tts_stop_cmd(void);
extern void send_tts_start_cmd(void);
extern void Tws_clear_filter(void);


//for tws or no tws
extern uint8 tws_or_notws_flag;
extern uint8 send_once_cmd;
extern uint8 second_empty_flag;
extern uint8 second_empty_flag2;
extern int8 clear_filter_timer_id;

extern sbc_obj_t sbc_obj;
#endif

extern uint16 mov_ptr;
extern uint8 linein_tws_playpause_flag;
extern uint8 adc_init_flag;
extern uint8 sample_start_flag;


extern uint8 g_need_adc_flag;
extern uint8 enable_unfilter_flag;

extern app_result_e app_message_deal(private_msg_t *pri_msg) __FAR__;
extern void btplay_engine_pipe_control(uint8 op) __FAR__;
extern void set_btstack_linein_ext(uint8 flag) __FAR__;
extern void set_start_time_param(void) __FAR__;

extern void adc_set_init(void) __FAR__;
extern void adc_set_exit(void) __FAR__;

typedef void (*update_pipe_entry)(void *p1,uint16 p2);
typedef void(*set_notice_entry)(uint8 set_cnt);

extern update_pipe_entry update_pipe;
extern set_notice_entry set_notice_func;

extern void pcm_data_deal(void);
extern void check_linein_data(void);
extern void sample_sound_init(void);
extern void sample_sound_exit(void) __FAR__;

extern void sbc_encode_init(void);
extern void sbc_encode_exit(void);
extern void tws_sync_cmd_send(uint8 val);
extern void tws_sync_cmd_deal(void);
extern void message_loop_exit(void);
extern void message_loop_init(void);
extern void tws_sync_cmd_deal_sub(void);

extern void deal_linein_usb_noise_sub(void);
extern void deal_linein_usb_noise(void);
extern void send_pause_status(void);
extern void tws_filter_ctrl_send(uint8 val);
extern void deal_linein_clear_filter(void);

#define ADC_SAMPLE_BUFFER  (0x9fc18580)
#define ADC_SAMPLE_BUFFER_SIZE (0x1000) //ADC buff大小
#define ADC_SAMPLE_BUFFER_LEN (0x800) //ADC buff真正使用的大小
#define ADC_SAMPLE_BUFFER_HALF (ADC_SAMPLE_BUFFER_LEN/2) //ADC buff真正使用的大小
#define SBC_ENCODE_FRAME_CNT   (ADC_SAMPLE_BUFFER_HALF/512)

#define SBC_ENC_BUFFER (0x9fc18380)
#define SBC_FRAME_LENGTH (119)

#define BAT_HIGH_VAL            (0x6a)

#define BAT_LOW_VAL             (0x5d)

#define COUNT_BOUND             (200)

//TWS_SYNC_START_TIME range 400000 - 4500000 
//low delay recommand value is 430000 
//no tremble recommand value is 600000 
#define TWS_SYNC_START_TIME    (400000)
#define TWS_SYNC_START_CNT     (TWS_SYNC_START_TIME/3125)

#endif/* _BTPLAY_ENGINE_H */

