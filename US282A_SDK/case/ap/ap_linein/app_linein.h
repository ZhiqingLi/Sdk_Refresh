/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：LineIn相关接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_LINEIN_H__
#define __APP_LINEIN_H__

#include "psp_includes.h"
#include "case_include.h"

#include "linein_common.h"
#include "app_linein_rcp.h"

typedef struct
{
    /*! 魔数 */
    uint16 magic;
    /*! eq模式 */
    com_eqtype_e eq_type;
    /*!支持linein*/
    uint8 linin_open;
} linein_config_t;

//for globle variables

//当前播放状态信息
extern linein_eg_status_t g_linein_eg_status;
extern linein_config_t g_linein_config;

//系统全局变量
extern comval_t g_comval;
extern bool g_neednot_tts_play;

extern bool g_linein_init_flag;

//for energy detect use 
extern int8 g_energy_detect_cnt_timer ;
//for period cnt save 
extern uint8 g_period_cnt;

extern const key_event_map_t linein_ke_maplist[];
extern const sys_event_map_t linein_se_maplist[];

#ifdef PRINT_BANK_INFO
extern int print_label;
#endif

app_result_e scene_linein(void);
bool linein_get_status(linein_eg_status_t* play_status) __FAR__;
bool linein_play(void) __FAR__;
bool linein_stop(void) __FAR__;
bool linein_send_msg(void *input_param, void *output_param, uint32 msg_type) __FAR__;
void linein_rcp_var_init(void);
void linein_rcp_var_exit(void);

app_result_e key_play_deal(void) __FAR__;
app_result_e key_eq_deal(void) __FAR__;
app_result_e _linein_loop_deal(void) __FAR__;
void linein_modify_timer_proc(void) __FAR__;

#endif //__APP_LINEIN_H__
