/********************************************************************************
 *                            Module: radio_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     mikeyang   2011-09-08 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __FMENGINE_H__
#define __FMENGINE_H__

#include "psp_includes.h"
#include "fm_interface.h"
#include "mmm_pp.h"

typedef enum
{
    STATUS_PLAY = 0, // 正常播放状态
    STATUS_PAUSE, //正常暂停状态(静音状态)
    STATUS_FMENGINE_ERROR, //硬件出错
} FM_Playstatus_e;

typedef enum
{
    FM_MONO = 0, //[1]单声道
    STERE, //[0]立体声
} FM_Audio_e;

/*硬件seek 状态枚举*/
typedef enum
{
    HARDSEEK_NOT_START = 0, //未启动硬件搜台或者此轮硬件搜台未搜到有效台
    HARDSEEK_DOING, //硬件搜台过程中
    HARDSEEK_COMPLETE, //此轮硬件搜台过程结束并搜到有效台
    HARDSEEK_COMPLETE_NOT_VALID, //此轮硬件搜台过程结束但没有搜到有效台
    HARDSEEK_FALSE = 4, //非真正硬件搜台（for the module have no hard seek）
} hardseek_status_e;

typedef struct
{
    UINT32 FM_CurrentFreq; //当前频率
    UINT16 FM_SignalStg; //当前频点信号强度
    FM_Audio_e FM_Stereo_Status; //0，立体声；非0，单声道
    radio_band_e FM_CurBand; //当前波段
    FM_Playstatus_e FM_VolStatus; //当前所处状态
    hardseek_status_e STC_flag; //硬件seek 是否完成标志，主要用于硬件seek 需要
} Engine_Status_t;

typedef enum
{
    PA_MODE = 0, //非直驱
    PA_DDV_MODE, //直驱
} PA_DDV_Mode_e;

/* FM 引擎配置信息结构体*/
typedef struct
{
    uint16 magic;
    uint8 fm_threshold; //模组默认搜台门限值
    /*! wav/mp3/wma音效参数 */
    mmm_pp_eq_info_t eq_info;
} FMEngine_cfg_t;

/* FM 模组状态枚举*/
typedef enum
{
    MODULE_NO_INIT = 0, //模组处于未初始化状态
    MODULE_WORK, //模组初始化完成，处于正常工作状态
    MODULE_STANDBY, //模组处于Standby 状态
} Module_Status_e;

//搜台参数
typedef struct
{
    uint32 freq;
    uint8 dir;
} seek_param_t;

//重新设置参数
typedef struct
{
    uint32 freq;
    uint8 band;
    uint8 threshold;
} reinit_param_t;
#endif //__FMENGINE_H__
