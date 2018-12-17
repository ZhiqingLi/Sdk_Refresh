/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：music引擎相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_MENGINE_H__
#define __APP_MENGINE_H__

#include "psp_includes.h"
#include "case_include.h"
#include "music_common.h"

#define _DEBUG              (0)

//是否通过music引擎获取ID3信息，默认是通过music前台获取
//因此该宏关闭，如果前台不支持获取ID3,可通过后台此接口获取
//#define MENGINE_ID3_SUPPORT

//音乐停止模式
typedef enum
{
    //正常停止
    STOP_NORMAL,
    //暂停
    STOP_PAUSE
} stop_mode_e;

typedef enum
{
    NORMAL_SWITCH_NEXT = 0x0,
    NORMAL_SWITCH_PREV = 0x1,
    FORCE_SWITCH_NEXT = 0x2,
    FORCE_SWITCH_PREV = 0x3,
    END_SWITCH = 0x4
} switch_mode_e;

typedef enum
{
    SWITCH_NO_ERR,
    SWITCH_ERR_STOP,
    SWITCH_ERR_DISK_OUT,
    SWITCH_ERR_SWITCH,
    SWITCH_ERR_READ_CARD_TIMEOUT,
    SWITCH_ERR_PLAY
} switch_result_e;

//for vram write
typedef struct
{
    /*! 记录当前文件的路径信息/列表文件的层级关系*/
    musfile_location_u location;
    /*! 断点信息*/
    mmm_mp_bp_info_t bk_infor;
} mengine_bkinfo_t;

#if ((CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))

#define SD_PW_PIN_SET_OUTPUT()  act_writel(act_readl(GPIOBOUTEN) | (1<<11), GPIOBOUTEN)
#define SD_PW_PIN_SET_INPUT()  act_writel(act_readl(GPIOBOUTEN) & (~(1<<11)), GPIOBOUTEN)
#define Get_SD_PW_PIN_INOUT()  (act_readl(GPIOBOUTEN) & (1<<11))

#define SET_SD_PW_PIN_RESET()  act_writel(act_readl(GPIOBDAT) | (1<<11), GPIOBDAT)
#define SET_SD_PW_PIN_ON()   act_writel(act_readl(GPIOBDAT) & (~(1<<11)), GPIOBDAT)
#define Get_SD_PW_PIN_ONOFF()  (act_readl(GPIOBDAT) & (1<<11))

#else

#define SD_PW_PIN_SET_OUTPUT()  act_writel(act_readl(GPIOAOUTEN) | (1<<6), GPIOAOUTEN)
#define SD_PW_PIN_SET_INPUT()  act_writel(act_readl(GPIOAOUTEN) & (~(1<<6)), GPIOAOUTEN)
#define Get_SD_PW_PIN_INOUT()  (act_readl(GPIOAOUTEN) & (1<<6))

#define SET_SD_PW_PIN_RESET()  act_writel(act_readl(GPIOADAT) | (1<<6), GPIOADAT)
#define SET_SD_PW_PIN_ON()   act_writel(act_readl(GPIOADAT) & (~(1<<6)), GPIOADAT)
#define Get_SD_PW_PIN_ONOFF()  (act_readl(GPIOADAT) & (1<<6))

#endif

#ifdef WAVES_ASET_TOOLS
extern waves_t  g_waves;
#endif

//globle variable
//保存中间件句柄
extern void *g_mp_handle;

//存放文件系统id
extern int32 g_file_sys_id;

//进入模式
extern mengine_enter_mode_e g_mengine_enter_mode;

//引擎信息
extern mengine_info_t g_mengine_info[2];
extern mengine_info_t *g_mengine_info_p;
//中间件播放文件信息
extern mmm_mp_file_info_t g_eg_file_info;
//设置信息(保存配置)
extern mengine_config_t g_eg_config;

//文件名buffer
extern uint8 g_file_name[FILE_NAME_LEN];

//中间件状态检测定时器
extern int8 g_status_timer;

//中间件状态检测标志位
extern uint8 g_check_status_flag;

//music engine延时参数
extern uint8 g_delay_time;

//控件返回值
extern app_result_e g_mengine_result;

//引擎配置变量结构体指针
extern mengine_config_t *g_eg_cfg_p;

//引擎状态变量结构体指针
extern mengine_status_t *g_eg_status_p;

//引擎播放信息结构体指针
extern mengine_playinfo_t *g_eg_playinfo_p;

//私有消息全局变量
//extern private_msg_t g_cur_msg;
//eq data
extern mmm_mp_eq_info_t g_eq_info;

//for clk adjust
extern uint32 g_save_freq;
//访问列表开关
extern bool g_list_access_allow;

//因TTS播报而暂停播放音乐，TTS播报结束后需要恢复播放音乐
extern bool g_stop_by_tts_flag;

extern uint8 g_shuffle_flag;
extern fsel_mode_e g_repeat_mode;
//是否支持卡
extern uint8 g_card_support;
//是否支持uhost
extern uint8 g_uhost_support;
extern fsel_type_e g_fsel_type;

#ifdef MENGINE_ID3_SUPPORT
//存放id3信息
extern id3_info_t g_id3_info;

//标题缓冲区
extern uint8 g_title_buf[Id3BufSIZE];

//歌手缓冲区
extern uint8 g_artist_buf[ArtBufSIZE];

//专辑缓冲区
extern uint8 g_album_buf[ArtBufSIZE];

#endif

#ifdef DIR_NAME_SUPPORT
extern uint8 g_dir_name_buf[32];
#endif

extern uint8 g_open_disk;
//MMM_MP_OPEN指令是否发出
extern bool g_mmm_opened;

//for dae adjust freq
extern dae_config_t *g_dae_cfg_shm;

extern bool g_music_replay_by_prev_key;

extern output_channel_e g_output_channel_config;

//functions
bool set_file(void);
bool play(play_mode_e play_mode) __FAR__;
bool stop(stop_mode_e stop_mode) __FAR__;
bool next_or_prev(switch_mode_e switch_mode) __FAR__;
void error_handle2(void) __FAR__;
void error_handle(void) __FAR__;
bool mengine_check_disk_in(void)__FAR__;
void mengine_reply_msg(void* msg_ptr, bool ret_vals) __FAR__;
void m_decrypt_func(void *buf, int offset, int size) __FAR__;
void me_update_share(void) __FAR__;
app_result_e mengine_control_block(void) __FAR__;
app_result_e mengine_message_done(private_msg_t* cur_msg) __FAR__;
app_result_e mengine_message_done_bank(private_msg_t* cur_msg) __FAR__;
switch_result_e mengine_file_switch(stop_mode_e stop_mode, switch_mode_e force_mode,
play_mode_e play_mode) __FAR__;
void mengine_getid3_done(musfile_location_u* path_info) __FAR__;
app_result_e mengine_get_dirname(void* msg_ptr) __FAR__;
app_result_e mengine_clear_fileswitch(void* msg_ptr) __FAR__;
app_result_e mengine_set_sortnumber(void *msg_ptr) __FAR__;
app_result_e mengine_get_disknumber(void *msg_ptr) __FAR__;
bool mengine_file_init(void) __FAR__;

//for event
app_result_e mengine_get_playmode(void* msg_ptr) __FAR__;
app_result_e mengine_get_filepath(void* msg_ptr) __FAR__;
app_result_e mengine_set_eq(void* msg_ptr);
app_result_e mengine_set_ab_count(void* msg_ptr) __FAR__;
app_result_e mengine_set_ab_gap(void* msg_ptr) __FAR__;
app_result_e mengine_start_ab(void* msg_ptr) __FAR__;
app_result_e mengine_set_apoint(void* msg_ptr) __FAR__;
app_result_e mengine_set_bpoint(void* msg_ptr) __FAR__;
app_result_e mengine_clear_ab(void* msg_ptr) __FAR__;
app_result_e mengine_set_playmode(void* msg_ptr);
app_result_e mengine_switch_disk(void* msg_ptr);
app_result_e mengine_list_access_allow(void* msg_ptr);
app_result_e mengine_play_digital_song(void* msg_ptr);
app_result_e mengine_play_folder_next(void* msg_ptr);
app_result_e mengine_play_folder_prev(void* msg_ptr);
app_result_e mengine_play(void* msg_ptr);
app_result_e mengine_stop(void* msg_ptr);
app_result_e mengine_pause(void* msg_ptr);
app_result_e mengine_resume(void* msg_ptr);
app_result_e mengine_fast_forward(void* msg_ptr);
app_result_e mengine_fast_backward(void* msg_ptr);
app_result_e mengine_fastplay_step(void *msg_ptr);
app_result_e mengine_cancel_ffb(void* msg_ptr);
app_result_e mengine_play_next(void* msg_ptr);
app_result_e mengine_play_prev(void* msg_ptr);
app_result_e mengine_set_filepath(void* msg_ptr);
app_result_e mengine_getid3_info(void* msg_ptr);
app_result_e mengine_get_playtype(void* msg_ptr);
app_result_e mengine_get_fileinfo(void* msg_ptr);
app_result_e mengine_tts_start(void* msg_ptr) __FAR__;
app_result_e mengine_tts_stop(void* msg_ptr) __FAR__;
app_result_e mengine_standby(void* msg_ptr) __FAR__;
void mengine_save_errno_no(void) __FAR__;
void mengine_vram_write(uint8 save_disk) __FAR__;
void _clear_vm_deal(uint8 clr_sel) __FAR__;
bool get_bk_info(void)__FAR__;
void set_esd_restart_tag(void)__FAR__;
void clear_esd_restart_tag(void)__FAR__;


#endif //__APP_MENGINE_H__
