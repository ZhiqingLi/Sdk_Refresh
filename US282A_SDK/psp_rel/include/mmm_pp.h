/*! \cond MMM_PP_API*/
/********************************************************************************
 *                              GL5116
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona    2014-12-07 15:00     1.0             build this file
 ********************************************************************************/

/*! \endcond*/
#ifndef __MMM_PP_H__
#define __MMM_PP_H__

/*!
 * \brief
 *      加载系统头文件
 */

#include <psp_includes.h>

/*! \cond MMM_PP_API */

/*! 最大音效参数个数 */
#define MAX_EQ_SEG                  12

/*!
 *  \brief
 *  音乐播放错误号，在调用接口返回错误或者中间件（引擎）状态为错误时有效
 */
typedef enum
{
    /*! 解码出错 */
    MMM_PP_ERR_DECODER_ERROR = 0x10001,
    /*! 无效操作 */
    MMM_PP_ERR_CMD_ILLEGAL = 0x10002,
    /*! 命令字不存在 */
    MMM_PP_ERR_CMD_NOTEXIST = 0x10003,
    /*! decoder加载错误 */
    MMM_PP_ERR_DECODER_LOAD = 0x10004,
    /*! 未知错误 */
    MMM_PP_ERR_UNKNOWN = 0x100FF
} mmm_pp_err_e;

/*!
 * \brief
 *      音乐播放提供给应用层的命令字
 */
typedef enum
{

    /*! 打开音乐中间件 */
    MMM_PP_OPEN = 0,
    /*! 关闭音乐中间件 */
    MMM_PP_CLOSE,
    /*! 打开声音输出 */
    MMM_PP_AINOUT_OPEN,
    /*! 关闭声音输出 */
    MMM_PP_AINOUT_CLOSE,

    /*! 设置淡入时间 */
    MMM_PP_FADE_IN_TIME,
    /*! 设置淡出时间 */
    MMM_PP_FADE_OUT_TIME,

    /*! 设置播放参数(sample rate?) */
    MMM_PP_SET_PARAM,
    /*! 设置dac sample rate */
    MMM_PP_SET_DAC_RATE,
    /*! 设置adc sample rate */
    MMM_PP_SET_ADC_RATE,
    
    /*! 获取媒体信息 */
    MMM_PP_MEDIA_INFO,
    /*! 设置噪音衰减参数 */    
    MMM_PP_SET_NOISE_REDUCTION_PARA,
    /*! 播放 */
    MMM_PP_PLAY,
    /*! 停止 */
    MMM_PP_STOP,
    /*! 开始录音 */
    MMM_PP_RECORD_START,
    /*! 开始录音 */
    MMM_PP_RECORD_STOP,
    /*! 获取播放状态 */
    MMM_PP_GET_STATUS,
    /*! 获取错误信息号 */
    MMM_PP_GET_ERRORNO,

    /*! 开始送数据 */
    MMM_PP_START_SEND,
    /*! 停止送数据 */
    MMM_PP_STOP_SEND,

    /*! 设置音量 */
    MMM_PP_SET_VOLUME,
    /*! 设置音效 */
    MMM_PP_SET_EQ,

    /*! 设置回调函数 */
    MMM_PP_SET_CALLBACK,

    /*! 获取能量检测相关信息：通过获取地址，然后就可以直接快速访问 */
    MMM_PP_GET_ENERGY_INFO,

    /*! 设置输出声道处理：左右声道互换，左右声道混合等 */
    MMM_PP_SET_OUTPUT_CHANNEL,

    /*! 设置是否作采样率固定 */
    MMM_PP_FIX_SAMPLE_RATE,
} mmm_pp_cmd_e;

/*!
 *  \brief
 *      音乐播放引擎的状态
 */
typedef enum
{
    /*! 停止状态 */
    MMM_PP_ENGINE_STOPPED = 0,
    /*! 播放状态 */
    MMM_PP_ENGINE_PLAYING,
    /*! 暂停状态 */
    MMM_PP_ENGINE_PAUSED,
    /*! 出错状态 */
    MMM_PP_ENGINE_ERROR
} mmm_pp_engine_e;

/*!
 *  \brief
 *     输入输出参数类型
 */
typedef enum
{
    /*! fm analog */
    MMM_PP_FM_IN = 0,
    /*! lienin analog */
    MMM_PP_LINEIN_IN,
    /*! usb composite */
    MMM_PP_USOUND_IN,
    /*! usb speaker */
    MMM_PP_USPEAKER_IN,
    /*max*/
    MMM_PP_MAX_IN
} mmm_pp_input_e;

/*!
 *  \brief
 *     输入输出参数信息
 */
typedef struct
{
    /*! 见mmm_pp_input_e*/
    int input_type;
    /*!asrc index*/
    int asrc_index;
    /*! sample rate */
    int dac_sample;
    /*!dac param dac chanel*/
    int dac_chanel;
    /*!asrc mode select */
    int asrc_mode_sel;
    /*!adc param adc source select */
    int adc_sample;
    /*!adc source select */
    int adc_insel;//mic or fm or linein    
    /*!adc param adc source select */
    int adc_gain;
    /*!adc source select */
    int ain_gain;//mic or fm or linein

} mmm_pp_ainout_setting_t;

/*!
 *  \brief
 *     音乐播放实时信息
 */
typedef struct
{
    /*! 引擎状态，详见mmm_pp_engine_e */
    int status;
    /*! 是否开始录音 */
    int record_status;
    /*! 出错的错误号，详见mmm_pp_err_e */
    int err_no;
    /*! 当前能量级别 */
    int cur_energy;
    /*! Todo spectrum*/
} mmm_pp_status_t;
/*!
 *  \brief
 *     音乐播放性能
 */
typedef struct
{
    /*! parser耗的时间 */
    int parser_elapsed;
    /*! decoder耗的时间 */
    int dec_elapsed;
    /*! 读取文件耗的时间 */
    int io_elapsed;
    /*! 其它损耗，如format_check */
    int other_elapsed;
} mmm_pp_performance_t;
/*!
 *  \brief
 *     音效设置信息
 */
typedef struct
{
    /*! eq的类型，详见mmm_mp_eqmode_t*/
    int eq_type;
    /*! eq的具体参数 */
    char eq_para[MAX_EQ_SEG];
} mmm_pp_eq_info_t;

/*!
 *  \brief
 *     数据管理
 */
typedef struct
{
    /*! 数据buffer*/
    uint8* data_buf;
    /*! 数据存取指针位置*/
    uint32 ptr_pos;
    /*! 数据字节数 */
    uint32 data_count;
} mmm_pp_data_info_t;

/*!
 *  \brief
 *     音乐播放信息
 */
typedef struct
{

    /*! 经验值，代码里面固定，不对外提供设置的接口*/
    /*! 系统默认参数，对所有歌曲适用，有设置接口，原则上只设置一次*/
    mmm_pp_ainout_setting_t ainout_setting;

    /*! 播放状态*/
    mmm_pp_status_t music_status;

    /*! 播放 数据*/
    mmm_pp_data_info_t send_out_data;

    /*! 采集数据*/
    mmm_pp_data_info_t get_in_data;

    /*! 淡入淡出 */
    /*! 淡入时间，单位毫秒 */
    int fade_in_time;
    /*! 淡出时间，单位毫秒 */
    int fade_out_time;

    /*! 用户可调节参数，当前设定适用于下一首歌，有设置接口，可以设置多次*/
    /*! 音量级别 */
    int volume_level;

    /*!音乐播放性能*/
    mmm_pp_performance_t cur_perform;

    /*! 音效设置，参见mmm_pp_eq_info_t*/
    mmm_pp_eq_info_t eq_setting;

} music_pcm_player_info_t;

/*!
 *  \brief
 *     接口函数声明
 *     接口函数定义在kernel_api.S，加载驱动以后使用，驱动API号在api.h
 */
extern int mmm_pp_cmd(void *, mmm_pp_cmd_e, unsigned int) __FAR__;

#endif // __MMM_PP_H__
