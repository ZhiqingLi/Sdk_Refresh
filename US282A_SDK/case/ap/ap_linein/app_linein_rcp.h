/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：LineIn RCP的处理
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_LINEIN_RCP_H__
#define __APP_LINEIN_RCP_H__

#include "psp_includes.h"
#include "case_include.h"

typedef struct
{
    /*! 播放状态，请参见 linein_play_status_e 定义 */
    uint8 play_status;
    uint8 reserve[3];
} linein_rcp_info_t;

typedef struct
{
    com_rcp_info_t com_info;
    linein_rcp_info_t app_info;
} linein_global_rcp_info_t;

app_result_e linein_get_global_rcp_info(uint32 data1, uint32 data2) __FAR__;
app_result_e linein_set_aux_setting(uint32 data1, uint32 data2) __FAR__;
app_result_e linein_apk_key_play(uint32 data1, uint32 data2) __FAR__;

#endif //__APP_LINEIN_RCP_H__
