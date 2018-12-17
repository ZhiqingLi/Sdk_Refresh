/********************************************************************************
 *                              5110
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona    2015-02-28 15:00     1.0             build this file
 ********************************************************************************/

/*! \endcond*/
#ifndef __MMM_BP_H__
#define __MMM_BP_H__

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

#include "mmm_dae_def.h"

//for bt data get pip
/*!
 *  \brief
 *  //蓝牙数据管道
 */
typedef struct
{
    uint16 read_ptr;
    uint16 write_ptr;
    uint16 capacity;
    uint16 length;
    uint16 sbc_input_length;
    uint8 *raw_data;
    uint8 *ext_info;
} bt_stack_pipe_t;

/*!
 *  \brief
 *  //TWS play status 
 
 */
typedef enum
{
    TWS_PLAY_IDLE = 0,
    TWS_PLAY_START,
    TWS_PLAY_SYNC,
    TWS_PLAY_MAX
}bt_tws_play_flag_e;


/*!
 *  \brief
 *  //TWS 信息交互
 */
typedef struct
{
    //uint8  start_flag;
    bt_tws_play_flag_e start_flag;
     uint8  tts_start_flag;
     uint8  tts_stop_flag;
     uint8 mmm_init_flag;
	 
    uint8 pri_pause_flag;
     uint8  pri_exit_mmm;
     uint8 sub_exit_mmm;
     uint8  sub_full_exit_flag;
     uint8  sub_back_bl_flag;
     uint8 sub_quit_bl_flag;
     uint8 pri_full_exit_flag;
     uint8 pri_empty_exit_mmm;
     uint8 tws_sync_cmd;
     uint8 asrc_change_flag;  //src 发生调节标志
     uint8 tws_play_sync_flag; //tws 从箱播放同步
     //for linein start time use 
     uint8 tws_start_cnt;//linein start cnt 
     uint16 tws_sub_pipe_length;
     uint16 dsp_dec_err_count;
     uint16 dsp_dec_err_count1;
    uint32 buffer_time_length;
    uint32 sample_count; //主箱播放的sample count
    uint32 sample_count1; //从箱播放的sample count
    uint32 src_value;    //主箱src调节后的值
     uint16 tws_send_buf_len;
    uint32 tws_send_buf_base;
} bt_stack_twsinfo_t;

/*!
 *  \brief
 *  音乐播放错误号，在调用接口返回错误或者中间件（引擎）状态为错误时有效
 */
typedef enum
{
    /*! 解码出错 */
    MMM_BP_ERR_DECODER_ERROR = 0x10001,
    /*! 无效操作 */
    MMM_BP_ERR_CMD_ILLEGAL = 0x10002,
    /*! 命令字不存在 */
    MMM_BP_ERR_CMD_NOTEXIST = 0x10003,
    /*! decoder加载错误 */
    MMM_BP_ERR_DECODER_LOAD = 0x10004,
    /*! 未知错误 */
    MMM_BP_ERR_UNKNOWN = 0x100FF
} mmm_bp_err_e;

/*!
 * \brief
 *      音乐播放提供给应用层的命令字
 */
typedef enum
{

    /*! 打开音乐中间件 */
    MMM_BP_OPEN = 0,
    /*! 关闭音乐中间件 */
    MMM_BP_CLOSE,
    /*! 打开声音输出 */
    MMM_BP_AOUT_OPEN,
    /*! 关闭声音输出 */
    MMM_BP_AOUT_CLOSE,

    /*! 设置淡入时间 */
    MMM_BP_FADE_IN_TIME,
    /*! 设置淡出时间 */
    MMM_BP_FADE_OUT_TIME,    
    /*! 设置噪音衰减参数 */    
    MMM_BP_SET_NOISE_REDUCTION_PARA,
    /*! 播放 */
    MMM_BP_PLAY,
    /*! 停止 */
    MMM_BP_STOP,

    /*! 获取播放状态 */
    MMM_BP_GET_STATUS,
    /*! 获取错误信息号 */
    MMM_BP_GET_ERRORNO,

    /*! 设置音效 */
    MMM_BP_SET_EQ,

    /*! 设置播放文件格式 */
    MMM_BP_SET_MUSIC_TYPE,

    /*! 设置回调函数 */
    MMM_BP_SET_CALLBACK,

    /*! 获取能量检测相关信息：通过获取地址，然后就可以直接快速访问 */
    MMM_BP_GET_ENERGY_INFO,

    /*! 设置输出声道处理：左右声道互换，左右声道混合等 */
    MMM_BP_SET_OUTPUT_CHANNEL,
    /*! TWS SYNC START */
    MMM_BP_TWS_SYNC_START,
    /*! tws sample count*/
    MMM_BP_TWS_GET_SAMPLE_COUNT,
    /*! fix sample rate*/
    MMM_BP_FIX_SAMPLE_RATE,
    /*! 抑制噪声:只是对SBC编码的信号进行处理*/
    MMM_BP_RESTRAIN_NOISE, 
} mmm_bp_cmd_e;

/*!
 *  \brief
 *      音乐播放引擎的状态
 */
typedef enum
{
    /*! 停止状态：DSP没有加载 */
    MMM_BP_ENGINE_STOPPED = 0,
    /*! 播放状态：DSP已加载，且以适合频率运行 */
    MMM_BP_ENGINE_PLAYING,
    /*! 暂停状态：DSP已加载，但是以很低频率运行 */
    MMM_BP_ENGINE_PAUSED,
    /*! 出错状态 */
    MMM_BP_ENGINE_ERROR
} mmm_bp_engine_e;

/*!
 *  \brief
 *     音乐播放实时信息
 */
typedef struct
{
    /*! 引擎状态，详见mmm_bp_engine_e */
    int status;
    /*! 出错的错误号，详见mmm_bp_err_e */
    int err_no;
    /*! 当前能量级别 */
    int cur_energy;
    /*! Todo spectrum*/
} mmm_bp_status_t;
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
} mmm_bp_performance_t;

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
} mmm_bp_data_info_t;

/*!
 *  \brief
 *     音乐类型参数
 */

typedef enum
{
    /*! sbc  */
    TYPE_SBC = 0,
    /*! aac */
    TYPE_AAC,
    /*! mp3 */
    TYPE_MP3,
    /*! aac */
    TYPE_MAX

} mmm_bp_music_type_e;

/*!类型相关信息*/
typedef struct
{
    /*!播放的文件类型 见 mmm_bp_music_type_e*/
    mmm_bp_music_type_e music_type;
    /*!播放起始延时*/
    uint8 play_delay_start;
    /*!播放最低延时*/
    uint8 play_delay_lowest;
    /*!普通模式或者tws模式*/
    uint8 work_mode;
} mmm_bp_set_type_param_t;

/*!
 *  \brief
 *     输入参数信息
 */
typedef struct
{
    /*! sample rate */
    uint32 dac_sample;
    /*! dac param dac chanel */
    uint32 dac_chanel;
    /*! 每帧采样次数，除以采样率即等于帧时长 */
    uint32 sample_per_frame;
    /*!类型相关信息*/
    mmm_bp_set_type_param_t type_param;
} mmm_bp_music_setting_t;

/*!
 *  \brief
 *     音乐播放信息
 */
typedef struct
{

    /*! 播放设置信息*/
    mmm_bp_music_setting_t music_setting;

    /*! 播放设置信息*/
    mmm_bp_data_info_t raw_data;

    /*! 播放状态*/
    mmm_bp_status_t music_status;

    /*! 淡入淡出 */
    /*! 淡入时间，单位毫秒 */
    int fade_in_time;
    /*! 淡出时间，单位毫秒 */
    int fade_out_time;

    /*!音乐播放性能*/
    mmm_bp_performance_t cur_perform;

} music_bt_player_info_t;

typedef struct
{
    /*!asrc index*/
    int asrc_index;
    /*!dac param dac chanel*/
    int dac_chanel;
    /*!asrc mode select */
    int asrc_mode_sel;

} mmm_bp_aout_setting_t;


/*!
 *  \brief
 *     接口函数声明
 *     接口函数定义在kernel_api.S，加载驱动以后使用，驱动API号在api.h
 */
extern int mmm_bp_cmd(void *, mmm_bp_cmd_e, unsigned int) __FAR__;

/*! \endcond */
#ifdef __cplusplus
}
#endif

#endif // __MMM_BP_H__
