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
#ifndef __MMM_MP_H__
#define __MMM_MP_H__

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

/*! \cond MMM_MP_API */

/*! 最大音效参数个数 */
#define MAX_EQ_SEG                  12
/*! vps级别设置范围为[-vps_max_level，vps_max_level]，负数为慢放，正数为快放，0表示vps关闭*/
#define VPS_MAX_LEVEL               8

/*!
 *  \brief
 *      歌曲类型
 */
typedef enum
{
   UNSUPPORT_TYPE = 0,
   MP3_TYPE,
   WMA_TYPE,
   WAV_TYPE,
   FLAC_TYPE,
   APE_TYPE,
   SBC_TYPE,
   OGG_TYPE,
   AAX_TYPE,
   AUD_TYPE,   
   AAC_TYPE,         
   AIN_TYPE,
   ACT_TYPE,

}music_type_t;

/*!
 *  \brief
 *  音乐播放错误号，在调用接口返回错误或者中间件（引擎）状态为错误时有效
 */
typedef enum
{
    /*! 打开文件失败 */
    MMM_MP_ERR_OPEN_FILE = 0x10001,
    /*! 文件格式不支持 */
    MMM_MP_ERR_FILE_NOT_SUPPORT = 0x10002,
    /*! 文件被损坏*/
    MMM_MP_ERR_FILE_DAMAGED = 0x10003,
    /*! 解码出错 */
    MMM_MP_ERR_DECODER_ERROR = 0x10004,
    /*! DRM无license */
    MMM_MP_ERR_NO_LICENSE = 0x10005,
    /*! DRM时钟错误 */
    MMM_MP_ERR_SECURE_CLOCK = 0x10006,
    /*! DRM时钟错误 */
    MMM_MP_ERR_LICENSE_INFO = 0x10007,
    /*! 无效操作 */
    MMM_MP_ERR_CMD_ILLEGAL = 0x10008,
    /*! 命令字不存在 */
    MMM_MP_ERR_CMD_NOTEXIST = 0x10009,
    /*! seek返回时间在期望时间之后 */
    MMM_MP_ERR_SEEK_BEHIND = 0x1000A,
    /*! parser加载错误 */
    MMM_MP_ERR_PARSER_LOAD = 0x1000B,
    /*! decoder加载错误 */
    MMM_MP_ERR_DECODER_LOAD = 0x1000C,
    /*! 创建fifo错误 */
    MMM_MP_ERR_FIFO_CREATE = 0x1000D,
    /*! 解码时间超过总时间 */
    MMM_MP_ERR_TIME_OVERFLOW = 0x1000E,
    /*! 输入源和输出设备不匹配 */
    MMM_MP_ERR_IO_MISMATCH = 0x1000F,
    /*! 快进到文件尾不停止 */
    MMM_MP_ERR_FF_END = 0x10010,
    /*! 快进到文件头不停止 */
    MMM_MP_ERR_FB_START = 0x10011,
    /*! parser出错 */
    MMM_MP_ERR_PARSER_ERROR = 0x10012,
    /*! 输出设备支持出错 */
    MMM_MP_ERR_DEVICE_ERROR = 0x10013,
    /*! 采样率不支持 */
    MMM_MP_SAMPLE_RATE_NOT_SUPPORT = 0x10014,
    /*! 未知错误 */
    MMM_MP_ERR_UNKNOWN = 0x100FF
} mmm_mp_err_t;
/*!
 * \brief
 *      音乐播放提供给应用层的命令字
 */
    typedef enum
    {
        /*! 分块处理以提高效率 */
    
        /*! 打开音乐中间件 */
        MMM_MP_OPEN = 0,
        /*! 关闭音乐中间件 */
        MMM_MP_CLOSE,
        /*! 打开声音输出 */
        MMM_MP_AOUT_OPEN,
        /*! 关闭声音输出 */
        MMM_MP_AOUT_CLOSE,
    
        /*! 设置音乐播放配置信息 */
        MMM_MP_SET_MUSIC_SETTING,
        /*! 获取音乐播放配置信息 */
        MMM_MP_GET_MUSIC_SETTING,
        /*! 设置音量表格首地址 */
        MMM_MP_VOLUME_TBL_ADDR,
        /*! 设置淡入时间 */
        MMM_MP_FADE_IN_TIME,
        /*! 设置淡出时间 */
        MMM_MP_FADE_OUT_TIME,
        /*! 设置快进退步长 */
        MMM_MP_FFB_STEP,
        /*! 设置快进退播放时间 */
        MMM_MP_FFB_PLAY_TIME,
        /*! 设置快进退间隔 */
        MMM_MP_FFB_WAIT_TIME,
        /*! 设置AB播放次数 */
        MMM_MP_AB_COUNT,
        /*! 设置B点等待时间 */
        MMM_MP_AB_WAIT_TIME,
        /*! 设置断句检测的能量门槛 */
        MMM_MP_AB_ENERGY_LIMIT,
        /*! 设置断句检测的时间门槛 */
        MMM_MP_AB_TIME_LIMIT,
    
        /*! 设置播放文件 */
        MMM_MP_SET_FILE,
        /*! 清除播放文件 */
        MMM_MP_CLEAR_FILE,
        /*! 获取媒体信息 */
        MMM_MP_MEDIA_INFO,
        /*! 获取DRM信息 */
        MMM_MP_GET_DRM_INFO,
        /*! 设置断点播放信息 */
        MMM_MP_SET_BREAK_POINT,
        /*! 获取断点播放信息 */
        MMM_MP_GET_BREAK_POINT,
        /*! 设置AB播放信息 */
        MMM_MP_SET_AB_INFO,
        /*! 获取AB播放信息 */
        MMM_MP_GET_AB_INFO,
        /*! 播放 */
        MMM_MP_PLAY,
        /*! 停止 */
        MMM_MP_STOP,
        /*! 获取播放性能 */
        MMM_MP_GET_PERFORMANCE,
    
        /*! 获取播放状态 */
        MMM_MP_GET_STATUS,
        /*! 获取错误信息号 */
        MMM_MP_GET_ERRORNO,
    
        /*! 设置音量 */
        MMM_MP_SET_VOLUME,
        /*! 设置音效 */
        MMM_MP_SET_EQ,
        /*! 设置快进退 */
        MMM_MP_SET_FFB,
        /*! 设置A点 */
        MMM_MP_SET_A,
        /*! 设置B点 */
        MMM_MP_SET_B,
        /*! 取消AB播放 */
        MMM_MP_CLEAR_AB,
        /*! 设置章节播放 */
        MMM_MP_SET_CHAPTER,
    
        /* 预留，未作实现 */
        /*! 定位 */
        MMM_MP_SEEK,
        /*! 设置AOUT SETTING */
        MMM_MP_SET_AOUT,
        /*! 单线程测试 */
        MMM_MP_SINGLE_THREAD,
        
        /*! 设置回调函数 */
        MMM_MP_SET_CALLBACK,    
    
        /*! 获取能量检测相关信息：通过获取地址，然后就可以直接快速访问 */
        MMM_MP_GET_ENERGY_INFO,
    
        /*! 设置输出声道处理：左右声道互换，左右声道混合等 */
        MMM_MP_SET_OUTPUT_CHANNEL,
        /*设置SD区数据操作接口*/
        MMM_MP_SET_SDD_FILE_IO,
        /*设置采样率固定*/
        MMM_MP_FIX_SAMPLE_RATE,
        
        /*是否支持音乐文件解密*/	
        MMM_MP_DECRYPT_STATUS = 0x90,
        /* 解密函数指针 */	
        MMM_MP_SET_DECRYPT_FUNC,
    } mmm_mp_cmd_t;

/*!
 *  \brief
 *      音乐播放引擎的状态
 */
typedef enum
{
    /*! 停止状态 */
    MMM_MP_ENGINE_STOPPED = 0,
    /*! 播放状态 */
    MMM_MP_ENGINE_PLAYING,
    /*! 暂停状态 */
    MMM_MP_ENGINE_PAUSED,
    /*! 出错状态 */
    MMM_MP_ENGINE_ERROR
} mmm_mp_engine_t;
/*!
 *  \brief
 *      DRM类型
 */
typedef enum
{
    /*! 不是DRM文件 */
    MMM_NO_DRM = 0,
    /*! ACTIONS的DRM视频文件 */
    MMM_ACT_DRM,
    /*! 微软的DRM视频文件 */
    MMM_MS_DRM,
    /*! 其他类型的DRM文件 */
    MMM_DRM_OTHER,
    /*! DRM文件但缺License */
    MMM_DRM_NO_LICENCE
} mmm_drm_type_t;
/*!
 * \brief
 *      定义EQ频带数
 */
enum
{
    /*! 5个频带 */
    EQ_BAND5 = 5,
    /*! 7个频带 */
    EQ_BAND7 = 7,
};
/*!
 *  \brief
 *     EQ模式
 */
typedef enum
{
    /*! 普通模式*/
    EQ_NORMAL = 0,
    /*! EQ用户模式*/
    EQ_EQ_USR_MODE = 7,
    /*! 音效自定义模式，待定*/
    EQ_TYPE_USR_MODE = 0x4f,
    /*! 变速用户模式*/
    EQ_VPS_USR_MODE = 0x77,
    /*! 卡拉O K 用户模式*/
    EQ_KARAOKE_USR_MODE = 0x88,
    /*! FULLSOUND模式*/
    EQ_FULLSOUND_MODE = 0x99,    
} mmm_mp_eqmode_t;
/*!
 *  \brief
 *      AB播放状态
 */
typedef enum
{
    /*! AB播放被清除 */
    AB_PLAY_CLEAR = 0,
    /*! 当前到达B点，包括等待的状态 */
    AB_PLAY_B,
    /*! AB播放状态中 */
    AB_PLAY_ING
} mmm_mp_ab_t;

typedef enum _FILE_SOURCE_TYPE
{
    FILE_SOURCE_NORMAL = 0,//普通文件
    FILE_SOURCE_SD,//SD区文件
    FILE_SOURCE_LINEIN_AA,//占位，
    FILE_SOURCE_LINEIN_ADDA,//LINEIN AD-DA通道
    FILE_SOURCE_BT_SBC,//蓝牙sbc数据
} FILE_SOURCE_TYPE;

/*!
 *  \brief
 *     打开文件所需的参数信息
 */
typedef struct
{
    /*! 0: vfs, 1: sd file system 2: line in aa mode 3: line in ad da mode 4: sbc input mode */
    int fs_type;
    /*! vfs mount id or sbc read function handle */
    void *file_mount_id;
    /*! file name */
    char *file_name;
} mmm_mp_fs_para_t;
/*!
 *  \brief
 *     音乐文件信息
 */
typedef struct
{
    /*! 音乐总时间,以毫秒为单位*/
    int total_time;
    /*! 音乐比特率，以Kbps为单位*/
    int bitrate;
    /*! 音乐采样率，以KHz为单位*/
    int sample_rate;
//    /*! 音乐声道数*/
//    int channels;
    /*! 音乐格式类型*/
    int music_type;    
    /*! drm类型，详见mmm_drm_type_t */
    int drm_type;
} mmm_mp_file_info_t;
/*!
 *  \brief
 *     DRM日期信息
 */
typedef struct
{
    short year;
    char month;
    char day;
} drm_date_t;
/*!
 *  \brief
 *     DRM时间信息
 */
typedef struct
{
    short hour;
    char minute;
    char second;
} drm_time_t;
/*!
 *  \brief
 *     DRM信息
 */
typedef struct
{
    /*! 开始播放日期 */
    drm_date_t BeginDate;
    /*! 开始播放时间 */
    drm_time_t BeginTime;

    /*! 截止播放日期 */
    drm_date_t ExpDate;
    /*! 截止播放日期 */
    drm_time_t ExpTime;

    /*! 第一次用过之后还能用的小时数，对同一首歌是固定值 */
    int RemainTime;
    /*! 余下可存储的时长(天数), 现在保留此接口，但暂时不加 */
    int RemainStoreTime;
    /*! 0: 时钟回调时删除 1：时钟回调失效 */
    int DealOnClkBck;

    /*! 开关以上各项(为1的位表示对应项打开，为0时表示关闭) */
    int switchflag;
} mmm_mp_drm_info_t;
/*!
 *  \brief
 *     音乐播放实时信息
 */
typedef struct
{
    /*! 引擎状态，详见mmm_mp_engine_t */
    int status;
    /*! 出错的错误号，详见mmm_mp_err_t */
    int err_no;
    /*! 自动AB播放时的状态， 详见mmm_mp_ab_t*/
    int ab_play_flag;
    /*! 当前的播放时间，单位毫秒 */
    int cur_time;
    /*! 当前比特率，单位kbps */
    int cur_bitrate;
    /*! 当前能量级别 */
    int cur_energy;
    /*! Todo spectrum*/
} mmm_mp_status_t;
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
} mmm_mp_performance_t;
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
} mmm_mp_eq_info_t;
/*!
 *  \brief
 *     音乐播放断点信息
 *     跟music_bp_info_t一致
 */
typedef struct
{
    /*! 断点位置的时间，单位ms */
    int bp_time_offset;
    /*! 断点位置在文件中的位置，单位字节 */
    int bp_file_offset;
    /*! 断点播放所需要的辅助信息*/
    /*! 比如章节号码适用于Audible  */
    int bp_info_ext;
} mmm_mp_bp_info_t;
/*!
 *  \brief
 *     音乐播放AB点信息
 */
typedef struct
{
    /*! 当前ab播放还剩余的次数 */
    int ab_play_count_left;
    /*! 当前到达B点后是否需要延时 */
    int ab_play_delay_toggle;
    /*! 当前ab播放所处的状态，详见mmm_mp_ab_t*/
    int ab_play_flag;
    /*! a点附近的时间戳，单位毫秒 */
    int a_point_time_offset;
    /*! a点附近的文件位置，单位字节 */
    int a_point_file_offset;
    /*! a点附近辅助信息 */
    int a_point_info_ext;
    /*! b点附近的时间戳，单位毫秒 */
    int b_point_time_offset;
} mmm_mp_ab_info_t;
/*!
 *  \brief
 *     音乐输出的设置信息
 */
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
} mmm_mp_aout_setting_t;
/*!
 *  \brief
 *     音乐播放的设置信息
 */
typedef struct
{
    /*! 经验值，代码里面固定，不对外提供设置的接口*/

    /*! 系统默认参数，对所有歌曲适用，有设置接口，原则上只设置一次*/
    mmm_mp_aout_setting_t aout_setting;
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

    /*! 快进退 */
    /*! 快进退步长，单位毫秒 */
    int ffb_step;
    /*! 快进退延时，单位毫秒 */
    int ffb_delay;
    /*! 快进退播放时间，单位毫秒 */
    int ffb_play_time;

    /*! AB复读 */
    /*! AB 播放次数 */
    int ab_play_count;
    /*! 到达B点后的延时时间，单位10ms */
    int ab_play_delay;

    /*! 自动复读 */
    /*! 断句检测时间门槛，单位毫秒 */
    int sent_dec_energy_limit;
    /*! 断句检测能量门槛，单位级别 */
    int sent_dec_time_limit;

    /*! 用户可调节参数，当前设定适用于下一首歌，有设置接口，可以设置多次*/
    /*! 音量级别 */
    int volume_level;
    /*! 音效设置，参见mmm_mp_eq_info_t*/
    mmm_mp_eq_info_t eq_setting;

    /*! 预留，未作实现  */
    /*! 起始播放位置，单位字节*/
    int start_pos;
    /*! 结束播放位置，单位字节*/
    int end_pos;
} music_setting_t;

/*!
 *  \brief
 *     接口函数声明
 *     接口函数定义在kernel_api.S，加载驱动以后使用，驱动API号在api.h
 */
extern int mmm_mp_cmd(void *, mmm_mp_cmd_t, unsigned int) __FAR__;

/*! \endcond */
#ifdef __cplusplus
}
#endif

#endif // __MMM_MP_H__
