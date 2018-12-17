/*! \cond MMM_MP_API*/
/********************************************************************************
 *                              5110
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       jpliao    2010-08-27 15:00     1.0             build this file
 ********************************************************************************/
/*!
 * \file     mmm_mp.h
 * \brief    定义音乐播放中间提供的接口及参数
 * \author   jpliao
 * \version 1.0
 * \date  2010/08/27
 *******************************************************************************/
/*! \endcond*/
#ifndef __MMM_HFP_H__
#define __MMM_HFP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * \brief
 *      加载系统头文件
 */
#ifdef WIN32
/* VC */
#define __FAR__
#else
/* MIPS */
#include <psp_includes.h>
#endif

/*! 最大音效参数个数 */
#define MAX_EQ_SEG                  12

typedef enum
{
    /*! 打开文件失败 */
    MMM_HFP_ERR_OPEN_FILE = 0x10001,
    MMM_HFP_ERR_DECODER_ERROR,
    MMM_HFP_ERR_DECODER_LOAD,
    MMM_HFP_ERR_DECODER,
} mmm_hfp_err_t;
/*!
 * \brief
 *      音乐播放提供给应用层的命令字
 */
typedef enum
{
    /*! 分块处理以提高效率 */

    /*! 打开音乐中间件 */
    MMM_HFP_OPEN = 0,
     /*! 打开声音输出 */
    MMM_HFP_AOUT_OPEN,
     /*! 打开mic输入 */
    MMM_HFP_AIN_OPEN,

    /*开始播放*/
    MMM_HFP_PLAY,
     /*获取状态*/
    MMM_HFP_GET_STATUS,
    /*停止播放*/
    MMM_HFP_STOP_PLAY,
     /*! 关闭mic输入 */
    MMM_HFP_AIN_CLOSE,
    /*! 关闭声音输出 */
    MMM_HFP_AOUT_CLOSE,
    /*! 关闭音乐中间件 */
    MMM_HFP_CLOSE,
    /*! 固定采样率 */
    MMM_HFP_FIX_SAMPLE_RATE,
   
    
    /*单线程测试*/
    MMM_HFP_SINGLE_THREAD,
} mmm_hfp_cmd_t;

typedef struct
{
    /*! eq的类型，详见mmm_mp_eqmode_t*/
    int eq_type;
    /*! eq的具体参数 */
    char eq_para[MAX_EQ_SEG];
} mmm_hfp_eq_info_t;

typedef struct
{
    /*! 耳机驱动模式，0: 非直驱，1:直驱，默认值0*/
    int drive_mode;
    /*! 默认采样率，单位：KHz，默认值44 */
    int default_samplerate;
    /*! 默认通道数，默认值2 */
    int default_channels;
    /*! 默认音量 单位：级别，默认值24 */
    int default_volume;
} mmm_hfp_aout_setting_t;

/*!
 *  \brief
 *     音乐输出的设置信息
 */
typedef struct
{
    /*! 输入来源 ，参见 mmm_ai_type_t*/
    int input_source;
    /*! 是否监听，0: 不监听, 1:监听 */
    int listening_flag;
    /*! 输出方式， 0:非直驱，1:直驱 */
    int drive_mode;
    /*! 输入音源增益级别，0-7,推荐值：FM/LINE IN 2, MIC IN 7; step 1.5db*/
    int input_gain;
    /*! ADC增益级别，0-0x3f,推荐值：FM/LINE IN 0x2f, MIC IN 0x33; step 1.5db*/
    int adc_gain;
    /*! 数字增益级别，0,1,2, 零：disable，非零: enable，step 6db */
    int digital_gain;
} mmm_mr_ain_setting_t;

typedef struct
{
    /*! 经验值，代码里面固定，不对外提供设置的接口*/

    /*! 系统默认参数，对所有歌曲适用，有设置接口，原则上只设置一次*/
    mmm_hfp_aout_setting_t aout_setting;
    
    /*!输入配置设置*/
    mmm_mr_ain_setting_t ain_setting;
    
    /*! 播放通道选择 */
    /*! 播放通道模式，0：正常，1：只输出左声道，2：只输出右声道，3：左右声道互换 */
    int chanel_select_mode;
    /*! 音量表格起始地址 */
    /*! 音量表格，包含硬音量和软音量级别 */
    void *vloume_tbl_addr;

    /*! 淡入淡出 */
    /*! 淡入时间，单位毫秒 */
    int fade_in_time;
    /*! 淡出时间，单位毫秒 */
    int fade_out_time;

  
    /*! 用户可调节参数，当前设定适用于下一首歌，有设置接口，可以设置多次*/
    /*! 音量级别 */
    int volume_level;
    /*! 音效设置，参见mmm_mp_eq_info_t*/
    mmm_hfp_eq_info_t eq_setting;

          
} hfp_music_setting_t;

typedef struct
{
    /*! 0: vfs, 1: sd file, 2: file handle*/
    int fs_type;
    /*! need mount id when fs_type == 0*/
    void *file_mount_id;
    /*! need file handle when fs_type == 2, need file name when fs_type == 1*/
    void *file_handle;
} mmm_mfp_fs_para_t;

extern int mmm_hfp_cmd(void *, mmm_hfp_cmd_t, unsigned int) __FAR__;

/*! \endcond */
#ifdef __cplusplus
}
#endif
#endif 
