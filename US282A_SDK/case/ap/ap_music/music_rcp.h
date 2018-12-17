/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：播放窗口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef __APP_MUSIC_RCP_H__
#define __APP_MUSIC_RCP_H__

#include "psp_includes.h"
#include "case_include.h"
#include "music_common.h"
#include "enhanced.h"
#include "dir_list.h"

//for rcp get info
typedef struct
{
    //保存切换过的歌曲
    uint16 file_seq[2];
    //保存扫描线程状态(1表示扫描完成)
    uint8 scan_status;
    //保存是否已经发生过切换歌曲，RCP读状态后将其清0
    uint8 file_switch;
    //保存当前歌曲是否有歌词
    uint8 lyric_exist;

} rcp_file_status_t;

//最大后缀名个数
#define MUSIC_EXT_NUM 7

//#define LIST_BUF_SIZE 1024

typedef struct
{
    id3_info_t p_id3_info;
    /*! 文件名buffer */
    uint8 *pfile_buf;
    /*! 文件名buffer长度 */
    uint8 name_len;

} music_id3_info_t;

/*typedef struct
 {
 uint16 dest_num;//获取id3的文件的序号（未排序）
 music_id3_info_t p_id3_info;
 }mrcp_id3_infor_t;*/
//RCP 接口（回调函数）
typedef struct
{
    /*! 循环模式，请参见 fsel_mode_e 定义 */
    uint8 repeat_mode;
    /*! 播放状态，请参见 play_status_e 定义 */
    uint8 play_status;
    /*! AB复读状态，请参见 ab_status_e 定义  */
    uint8 ab_status;
    /*! 快进退状态，请参见 fast_status_e 定义  */
    uint8 fast_status;
    /*! 错误状态，请参见 eg_err_e 定义  */
    uint8 err_status;
    /*! 歌词存在标志 */
    uint8 lyric;
    /*! 文件切换标志  */
    uint8 file_switch;
    uint8 reserve;
    /*! 当前播放时间，以ms为单位 */
    uint32 cur_time;
    /*! 当前歌曲总时间，以ms为单位 */
    uint32 total_time;
    /*! 当前歌曲序号 */
    uint32 file_seq;
    /*! 歌曲总数 */
    uint32 file_total;
} music_rcp_info_t;

typedef struct
{
    com_rcp_info_t com_info;
    music_rcp_info_t app_info;
} music_global_rcp_info_t;

typedef enum
{
    MUSIC_TYPE_MP3, //mp3
    MUSIC_TYPE_WMA, //wma
    MUSIC_TYPE_WAV, //wav
    MUSIC_TYPE_FLA, //flac
    MUSIC_TYPE_APE, //ape
    MUSIC_TYPE_MP1, //mp3
    MUSIC_TYPE_MP2,
} music_type_e;

typedef struct
{
    /*! 当前歌曲序号 */
    uint32 file_seq;
    /*! 音乐类型，请参见 music_type_e 定义 */
    uint8 music_type;
#if (SUPPORT_PLIST_FILE == 1)
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 file_type;
    /*! 文件名buffer */
    uint8 file_buffer[64];
#endif
#if (SUPPORT_PLIST_TITLE == 1)
    /*! 标题编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 tit2_type;
    /*! 标题buffer */
    uint8 tit2_buffer[Id3BufSIZE];
#endif
#if (SUPPORT_PLIST_ARTIST == 1)
    /*! 演唱者编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 tpe1_type;
    /*! 演唱者buffer */
    uint8 tpe1_buffer[ArtBufSIZE];
#endif
#if (SUPPORT_PLIST_ALBUM == 1)
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 talb_type;
    /*! 专辑buffer */
    uint8 talb_buffer[ArtBufSIZE];
#endif
#if (SUPPORT_PLIST_GENRE == 1)
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 genre_type;
    /*! 流派buffer */
    uint8 genre_buffer[ArtBufSIZE];
#endif
} music_rcp_plist_info_t;

typedef struct
{
    /*! 音乐类型，请参见 music_type_e 定义 */
    uint8 music_type;
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 file_type;
    /*! 文件名buffer */
    uint8 file_buffer[64];
    /*! 标题编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 tit2_type;
    /*! 标题buffer */
    uint8 tit2_buffer[Id3BufSIZE];
    /*! 演唱者编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 tpe1_type;
    /*! 演唱者buffer */
    uint8 tpe1_buffer[ArtBufSIZE];
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 talb_type;
    /*! 专辑buffer */
    uint8 talb_buffer[ArtBufSIZE];
    /*! 文件名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 genre_type;
    /*! 流派buffer */
    uint8 genre_buffer[ArtBufSIZE];
} music_rcp_id3_info_t;

//每个vram扇区记录的目录项的信息
typedef struct _sector_info_t
{
    //第一个完整的list_dir_info_t是记录在g_sector_buf的什么位置
    uint8 offset;
    //第一个完整的list_dir_info_t的编号
    uint8 begin; //1开始(如果为0，表示无效)
    //最后一个完整的list_dir_info_t的编号
    uint8 end;
    //最后面还多出几个字节
    uint8 tail;
}
sector_info_t;
extern sector_info_t g_sector_info[DIR_INFO_SECTOR_COUNT];

#define SETTING_ID_MS_LOOP  0x00  //设置循环模式

void init_id3_info(void) __FAR__;
void music_rcp_var_init(void) __FAR__;
void music_rcp_var_exit(void) __FAR__;
bool music_rcp_fs_init(uint8 disk_type, uint8 drv_mode) __FAR__;
bool music_rcp_fs_exit(uint8 disk_type, int32 file_sys_id) __FAR__;
app_result_e music_get_global_rcp_info(uint32 data1, uint32 data2) __FAR__;
app_result_e music_get_music_info(uint32 data1, uint32 data2) __FAR__;
app_result_e music_get_music_lyric(uint32 data1, uint32 data2) __FAR__;
app_result_e music_get_music_plist(uint32 data1, uint32 data2) __FAR__;
app_result_e music_set_music_setting(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
app_result_e music_set_song_seq(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
app_result_e music_set_music_plist(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;
app_result_e music_set_music_disk(uint32 data1, uint32 data2, void *data, uint16 data_len) __FAR__;

void music_upload_music_plist(void) __FAR__;


//APK/APP上的按键的处理
app_result_e music_apk_key_play(uint32 data1, uint32 data2) __FAR__;
app_result_e music_apk_key_pause(uint32 data1, uint32 data2) __FAR__;
app_result_e music_apk_key_prev(uint32 data1, uint32 data2) __FAR__;
app_result_e music_apk_key_next(uint32 data1, uint32 data2) __FAR__;

extern uint8 g_id3_title_buf[64];
extern uint8 g_id3_artist_buf[32];
extern uint8 g_id3_album_buf[32];
extern uint8 g_id3_filename_buf[64];
extern pfile_offset_t g_temp_pfile_offset;
extern pdir_layer_t g_temp_pdir_layer;

//文件系统句柄
extern int32 eh_vfs_mount;
//文件总数
extern uint16 g_vmfile_total;
//目录总数
extern uint16 g_vmdir_total;

//vram record disk
extern uint16 g_vmdisk_flag;

//已经下载播放列表文件数
extern uint16 g_plist_file_count;

//当前目录
extern list_dir_info_t g_cur_dir;
extern uint16 g_cur_dir_no;
extern uint8 g_sector_buf[DIR_INFO_SECTOR_SIZE];

//读到了第几个扇区(0表示没有读过)
extern uint8 g_sector_no;
//上次定位到的目录
extern uint16 g_last_dir_no;
//上次定位的文件的序号(1开始)
extern uint16 g_last_get_index;

//for id3 get
extern music_id3_info_t g_cur_id3_info;

uint16 music_rcp_first_get(void);
bool music_rcp_fs_exit(uint8 disk_type, int32 file_sys_id);
bool music_rcp_get_id3(music_id3_info_t *rcp_id3_info, uint16 file_index);
bool music_rcp_get_cur_info(music_id3_info_t *rcp_id3_info);
bool music_get_name_info(music_id3_info_t *rcp_id3_info);
bool music_rcp_get_list_id3(music_id3_info_t *rcp_id3_info, uint16 file_index);
HANDLE music_lyric_check(uint32* len_val);
bool music_is_has_lyric(void);

#endif //__APP_MUSIC_RCP_H__
