/*! \cond MMM_MR_API */
/*******************************************************************************
 *                              5110
 *                            Module: music
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       jpliao     2011-02-01 15:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     mmm_mr.h
 * \brief    定义音乐录音中间件提供的接口及参数
 * \author   jpliao
 * \version 1.0
 * \date  2011/02/01
 *******************************************************************************/
/*! \endcond*/
#ifndef __MMM_MR_H__
#define __MMM_MR_H__

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

/*! \cond MMM_MR_API */
/*!
 * \brief
 *      编码格式
 */
typedef enum
{
    /*! PCM */
    PCM,
    /*! ADPCM */
    ADPCM,
    /*! MP3 */
    MP3,
    /*! ACT */
	ACT,
} mmm_mr_encode_mode_t;

/*!
 * \brief
 *      当前codec的状态,即mmm_mr_status_t中status取值
 */
typedef enum
{
    /*! 编码中 */
    MMM_MR_ENGINE_ENCODING = 0,
    /*! 暂停  */
    MMM_MR_ENGINE_PAUSED,
    /*! 编码器停止 */
    MMM_MR_ENGINE_STOPPED,
    /*! 编码错误 */
    MMM_MR_ENGINE_ERROR,
} mmm_mr_engine_t;
/*!
 * \brief
 *      错误号定义,即mmm_mr_status_t中err_no取值
 */
typedef enum
{
    /*! 文件操作出错 */
    MMM_MR_ERR_FILE_OP = -9,
    /*! 不存在的命令 */
    MMM_MR_ERR_CMD_NOTEXIST,
    /*! 一般未知错误 */
    MMM_MR_ERR_UNEXPECTED,
    /*! 编码库加载出错 */
    MMM_MR_ERR_LIBLOAD_ERROR,
    /*! 加载了非法的编码库 */
    MMM_MR_ERR_LIB_ERROR,
    /*! 编码出错 */
    MMM_MR_ERR_ENCODER_ERROR,
    /*! 采样率错误 */
    MMM_MR_ERR_FS_ERROR,
    /*! 内存空间不够 */
    MMM_MR_ERR_OUTOFMEMORY,
    /*! 格式或参数不支持，不能编码 */
    MMM_MR_ERR_UNSUPPORTED,
    /*! 正常 */
    MMM_MR_ERR_OK,
    /*! 输出数据超过缓冲区大小 */
    MMM_MR_ERR_DATAOVERFLOW,

} mmm_mr_err_t;

/*!
 * \brief
 *      录音中间件提供给应用层的命令字
 */
typedef enum
{
    /*! 打开 */
    MMM_MR_OPEN = 0,
    /*! 关闭 */
    MMM_MR_CLOSE,
    /*! 打开声音输入 */
    MMM_MR_AIN_OPEN,
    /*! 关闭声音输入 */
    MMM_MR_AIN_CLOSE,

    /*! 开始录音 */
    MMM_MR_START,
    /*! 停止录音 */
    MMM_MR_STOP,
    /*! 暂停录音 */
    MMM_MR_PAUSE,
    /*! 继续录音 */
    MMM_MR_RESUME,
    /*! 设置录音文件 */
    MMM_MR_SET_FILE,
    /*! 关闭录音文件 */
    MMM_MR_CLEAR_FILE,
    /*! 设置录音参数 */
    MMM_MR_AUDIO_PARAM,
    /*! 获取编码状态 */
    MMM_MR_GET_STATUS,
    /*! 获取错误号 */
    MMM_MR_GET_ERRORNO,
    /*! 单线程测试接口 */
    MMM_MR_SINGLE_THREAD,

	/*! 设置降噪参数: 0，不降噪，1-10为降噪级别，越大降噪越厉害*/
	MMM_MR_SET_DENOISE,
	/*! 设置自动增益控制参数：0，无需自动增益控制，1: 控制在-6b左右*/
	MMM_MR_SET_AGC,

} mmm_mr_cmd_t;


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

/*!
 *  \brief
 *     打开文件所需的参数信息
 */
typedef struct
{
    /*! 0: vfs, 1: sd file, 2: file handle*/
    int fs_type;
    /*! need mount id when fs_type == 0*/
    void *file_mount_id;
    /*! need file handle when fs_type == 2, need file name when fs_type == 1*/
    void *file_handle;
} mmm_mr_fs_para_t;

/*!
 *  \brief
 *     音频编码的参数设置
 *
 * \li     wav只需设采样率。比特率计算公式为：ADPCM: 采样率*4*2，PCM：采样率*16*2
 * \li     mp3录音参数设置一览表
 * \par
 <div class="generic_text_mapping_table">
 * <table>
 * <tr>
 * <th>质量</th>
 * <th>可选通道数</th>
 * <th>可选采样率</th>
 * <th>可选比特率</th>
 * </tr>
 * <tr>
 * <td>低</td>
 * <td>1</td>
 * <td>8k,11025,12k,16k</td>
 * <td>[8,64]</td>
 * </tr>
 * <tr>
 * <td>中</td>
 * <td>1,2</td>
 * <td>22050,24k</td>
 * <td>[64,128]</td>
 * </tr>
 * <tr>
 * <td>高</td>
 * <td>1,2</td>
 * <td>32k,44100,48k</td>
 * <td>[64,192]</td>
 * </tr>
 * </table>
 * </div>
 */
typedef struct
{
    /*! 音频编码的模式, 0: pcm-wav, 1: adpcm-wav, 2: mp3*/
    int encode_mode;
    /*! 编码的比特率，如64000,128000等*/
    int bitrate;
    /*! 采样频率如8000，16000，44100等*/
    int sample_rate;
    /*! 音频采样的通道数如:1 单声道；2 双声道*/
    int channel;
    /*! 采样精度，默认16，暂未使用 */
    int bps;
    /*! 控制模式：0:正常录音, 1:声控模式, 2:自动分曲 */
    int vad_mode;
    /*! 静音检测门限值[0,128],用于确定门限值，为零表示采用初始录音的噪声平均值，非零则-0.5dB一级, 默认值 64 */
    int vad_threshold;
    /*! 静音检测延迟时间,单位ms, 默认值 3000 */
    int vad_delay;
    /*! 降噪级别参数: 0，不降噪，1-10为降噪级别，越大降噪越厉害 */
    int denoise_level;
} mmm_mr_audio_param_t;
/*!
 * \brief
 *      录音中间件返回给应用层的状态信息
 */
typedef struct
{
    /*! 当前的录音时间*/
    int time;
    /*! 当前codec的状态*/
    int status;
    /*! 输出当前实时能量级别,暂时不支持*/
    int energy_level;
    /*! 当前是否是静音，在静音检测功能开启时有效, 零：有声，非零：静音*/
    int silence_flag;
    /*! 在出错状态时返回的错误号*/
    int err_no;
    /*! 当前录音写到文件中的数据量，单位：512字节即1个page*/
    int write_pages;
} mmm_mr_status_t;
/*!
 *  \brief
 *     接口函数声明
 *     接口函数定义在kernel_api.S，加载驱动以后使用，驱动API号在api.h
 */
extern int mmm_mr_cmd(void *, mmm_mr_cmd_t, unsigned int) __FAR__;

/*! \endcond */
#ifdef __cplusplus
}
#endif
#endif // __MMM_MR_H__
