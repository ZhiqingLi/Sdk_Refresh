

/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief  btcall头文件
*      Cailizhen          2015-1          creat
*       Wekan        2015-3-30       review
*******************************************************************************/


#ifndef _BLUETOOTH_ENGINE_H
#define _BLUETOOTH_ENGINE_H

#include "psp_includes.h"
#include "case_include.h"
#include "mmm_hfp.h"
#include "btcall_common.h"
#include "mmm_agc_aec_def.h"
#include "btcall_eg_stub_asqt.h"



#define CALLRING_DAC_BUFFER_LENGTH           (0x400*2)
extern uint8 g_btcall_callring_dac_buf[CALLRING_DAC_BUFFER_LENGTH];

#define CALLRING_READ_BUFFER_LENGTH          (0x200*2)
extern uint8 g_btcall_callring_read_buf[CALLRING_READ_BUFFER_LENGTH];

#define  ASQT_TEST_BUFFER_LENGTH (1024*10-4)
extern uint8    g_btcall_asqt_test_buf[ASQT_TEST_BUFFER_LENGTH];

#define DMA_NOT_TRANS 0
#define DMA_START_TRANS 1
#define DMA_HALF_TRANSED 2
#define DMA_TRANS_COMPLECT 3

typedef struct
{
    bool callring_thread_exist;
    sd_handle callring_handle;
    uint16 callring_file_length;   //文件长度
    uint16 callring_read_ptr;
    uint16 callring_silent_time;   //铃声静音持续时间，100ms为单位
    bool callring_silent_flag;
    int8 callring_timer_id;
} callring_data_struct;

extern callring_data_struct g_callring_data;

typedef struct
{
    uint8 g_sco_deal_flag;
    uint8 dac_clock_status;
    bool need_exit_thread;
    uint8 last_dma0_state;
    uint8 tts_play_flag;
    uint8 tts_pb_play_flag;//标志来电信息TTS播报状态，注意:播报电话本时tts_play_flag也会置1
    uint8 callin_ring_mode;//来电铃声模式，0表示仅铃声；1表示仅电话本播报；2表示铃声+电话本播报
    uint8 callin_ring_space_time;//来电铃声间隔时长，以1S为单位
    bool callin_ring_first;
    bool callin_pb_back;

    //需要重启AEC解码
    bool g_actaec_reset_flag;
    //屏蔽掉MIC声音
    bool g_mic_mute_flag;
    uint8 stub_asqt_en;//是否打开asqt 且打开stub

    uint8 ignore_uncall_sco; //是否需要过滤非通话状态下的sco数据，过滤戴尔笔记本刚连接时用SCO播歌的数据
                             //注意:这个过滤肯定会导致一些问题
    uint8 dsp_load_flag;  	 //dsp 是否已经加载 1:表示已经加载; 0表示没有加载                       
} btcall_global_variable_struct;

typedef struct{//最大2k
    uint16 magic;//0xAA55时有效
    uint16 times;//更新次数
    uint32 cs;//校验码
    sv_prms_t dsp_cfg;
}VM_SAVE_t;

extern int ring_dma_isr(void);

extern app_result_e btcall_deal_app_msg(void);
extern app_result_e get_message_loop(void);

extern void dma_dac_start_transfer(void);
extern void callring_dac_dma_config(void);

extern void bt_engine_hfp_sco_connect_ind_handle(void);
extern void bt_engine_hfp_sco_disconnect_ind_handle(void);

extern void callring_start(void);
extern void *callring_thread_handle(void *param);
extern void btcalleg_free_hfp_mmm(void);
extern app_result_e app_message_deal(private_msg_t *pri_msg);


extern btcall_global_variable_struct btcall_gl_var;

extern void *mp_handle;

extern bt_stack_info_t *g_p_bt_stack_cur_info;

extern btcall_info_t g_btcall_info[2];
extern btcall_info_t *g_p_btcall_info;
extern uint8 g_stub_tools_type;

#endif/* _BLUETOOTH_ENGINE_H */

