/********************************************************************************
 *                            Module: music_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-08 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __MUSIC_COMMON_H__
#define __MUSIC_COMMON_H__

#include "psp_includes.h"
#include "enhanced.h"
#include "mmm_mp.h"

//#define AB_SUPPORT

//在继续从断点播放之前，检查这个文件所在的目录的名字是否改了，如果改了，清除断点信息
#define CHECK_BK_DIR

/*!
 * \brief
 *  当前播放状态
 */
typedef enum
{
    /*! 停止 */
    StopSta = 0,
    /*! 暂停 */
    PauseSta,
    /*! 播放 */
    PlaySta,
    /*! AB复读 */
    PlayAB,
    /*! 快进退 */
    PlayFast
} play_status_e;

/*!
 * \brief
 *  AB复读状态
 */
typedef enum
{
    /*! 非AB复读状态 */
    PlayAB_Null,
    /*! 等待设置A点 */
    PlayAB_A,
    /*! 等待设置B点 */
    PlayAB_B,
    /*! AB复读完成状态 */
    PlayAB_Do
} ab_status_e;

/*!
 * \brief
 *  快进退状态
 */
typedef enum
{
    /*! 正常播放 */
    FBPlay_Null,
    /*! 快进状态 */
    FFPlaySta,
    /*! 快退状态 */
    FBPlaySta
} fast_status_e;

//音乐播放模式
typedef enum
{
    //正常播放
    PLAY_NORMAL,
    //续播
    PLAY_RESUME,
    //继续快进
    PLAY_FFON,
    //快退播放
    PLAY_FBON,
    //不需要播放
    PLAY_NO_PLAY
} play_mode_e;

/*!
 * \brief
 *  错误信息
 */
typedef enum
{
    /*! 没有错误 */
    EG_ERR_NONE = 0,
    /*磁盘无文件*/
    EG_ERR_NO_FILE,
    /*介质拔出*/
    EG_ERR_DISK_OUT,
    /*! 打开文件失败 */
    EG_ERR_OPEN_FILE,
    /*! 文件格式不支持 */
    EG_ERR_NOT_SUPPORT,
    /*! 解码出错 */
    EG_ERR_DECODER_ERROR,
    /*! 无license，播放次数已满或是播放时间已经到期  */
    EG_ERR_NO_LICENSE,
    /*! DRM时钟错误  */
    EG_ERR_SECURE_CLOCK,
    /*! DRM信息错误  */
    EG_ERR_DRM_INFO,
    /*! 读卡超时  */
    EG_ERR_RAED_CARD_TIMEOUT
} eg_err_e;

/*! 引擎初始化状态机 */
typedef enum
{
    /*! 正在初始化，除了读取状态，不能对引擎进行任何操作 */
    EG_CTL_INITING,
    /*! 已经初始化完毕 */
    EG_CTL_NORMAL,
    EG_CTL_MAX
} eg_ctl_e;

/*!
 * \brief
 *  播放的文件状态
 */
typedef enum
{
    /*! 从断点开始 */
    FIRST_PLAY_FROM_BK,
    /*!从头开始 */
    FIRST_PLAY_FROM_START,
    /*!illegal */
    PLAY_FILE_MAX
} first_play_e;

/*!
 * \brief
 *  引擎状态
 */
typedef struct
{
    /*! 播放状态  */
    play_status_e play_status;
    /*! AB复读状态  */
    ab_status_e ab_status;
    /*! 快进退状态  */
    fast_status_e fast_status;
    /*! 错误状态,获取后清除  */
    eg_err_e err_status;
    /*!当前引擎状态*/
    eg_ctl_e ctl_status;
    /*!第一次播放的文件状态*/
    first_play_e first_file_status;
} mengine_status_t;

/*!
 * \brief
 *  当前播放文件信息
 */
typedef struct
{
    /*! 当前时间 */
    uint32 cur_time;
    /*! 比特率 */
    uint32 cur_bitrate;
    /*! 歌词存在标志 */
    uint8 cur_lyric;
    /*! 文件切换标志 bit0: 歌曲是否切换 bit1:audible章节是否切换  */
    uint8 cur_file_switch;
    /*! 保留字节：4字节对齐 */
    uint8 reserve[2];
} mengine_playinfo_t;

/*!
 * \brief
 *  引擎信息结构体变量定义
 */
typedef struct
{
    /******************************************************************
     *          引擎状态信息
     *******************************************************************/
    mengine_status_t eg_status;

    /******************************************************************
     *          播放文件的时间和bit率信息
     *******************************************************************/
    mengine_playinfo_t eg_playinfo;
} mengine_info_t;

/*!
 * \brief
 *  music文件总数及序号计数
 */
typedef struct
{
    /*! 当前文件序号 */
    uint16 num;
    /*! 当前文件总数 */
    uint16 total;
} musfile_cnt_t;

/*!
 * \brief
 *  开启music引擎进入模式
 */
//因为在有些情况下，它跟DISK_H等组成一个字节
//要注意：他们组成一个字节后，还能否分开的问题
//所以本类型变量的值是多少，是有讲究的，不能无限制增加。
typedef enum
{
    /*! 选择歌曲播放 */
    ENTER_MUSIC_START = 0, ENTER_RECOD_PLAY,
    /*! alarm歌曲播放  */
    ENTER_ALARM,
} mengine_enter_mode_e;

//bit5 card; bit6 uhost。
typedef enum
{
    /*! 选择歌曲播放 */
    ENTER_MUSIC_DISK_C = (0 << 5),

    ENTER_MUSIC_DISK_H = (1 << 5),

    ENTER_MUSIC_DISK_U = (1 << 6),

} mengine_enter_disk_e;

//bit7 used for if need bk
#define MUSIC_NEED_BK (1<<7)

/*!
 * \brief
 *  引擎配置变量结构体定义
 */
typedef enum _shuffle_e
{
    SHUFFLE_NONE = 0,
    SHUFFLE = (1 << 4),
} shuffle_e;

#define FILE_NAME_LEN   (32)

typedef struct
{
    /*! 魔数 */
    uint16 magic;
    /*! 记录当前文件的路径信息/列表文件的层级关系*/
    musfile_location_u location;
    /*! 断点信息*/
    mmm_mp_bp_info_t bk_infor;
    //for file name
    uint8 name_buf[FILE_NAME_LEN];
#ifdef CHECK_BK_DIR
    //文件所在目录的名字
    uint8 dir_name[32];
#endif
} mengine_config_t;

typedef struct
{
    /*! 文件名缓冲区 */
    uint8 *fname_buffer;
    /*! 文件名最大长度 */
    uint8 fname_length;
} music_file_name_t;

//进行扫描时，会把各个有音乐文件的目录，记录到vram
//最多，记录几个扇区(每个扇区定义为512字节)
#define DIR_INFO_SECTOR_SIZE    (512)
#define DIR_INFO_SECTOR_COUNT   (8)

#endif //__MUSIC_COMMON_H__
