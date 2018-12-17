#ifndef __MUSIC_DIR_FILE_H__
#define __MUSIC_DIR_FILE_H__

#include "music.h"

typedef struct
{
    //起始曲目号
    uint16 begin_index;
    //结束曲目号
    uint16 end_index;
    /*! 文件名buffer */
    uint8 *pdir_buf;
    /*! 文件名buffer长度 */
    uint8 name_len;
} music_dir_info_t;

typedef enum
{
    FILE_LIST_VIEW = 0,     //单级列表显示
    DIR_LIST_VIEW = 1,               //目录列表显示
    OTHER,     //其他显示方式保留
} list_show_way_e; 	//支持显示方式

typedef struct
{
    uint8 list_show_way;	//支持显示方式
    uint8 dir_amount;   //目录总数
    uint8 error_num;    //错误号
} music_rcp_dir_list_status_t;  //  目录list状态

typedef struct
{
    uint8 sys_random[4];
    uint8 bl_random[6];
}music_rcp_dlist_random_t;//随机数，包括6位蓝牙地址和4位系统时间随机数

typedef struct
{
    /*! 起始曲目号 */
    uint16 begin_index;
    /*! 结束曲目号 */
    uint16 end_index;
    /*! 目录名编码类型：0表示ANSI，1表示UNICODE little endian，2表示UNICODE bit endian，3表示UTF-8 */
    uint8 enctype;
    /*! 目录名buffer */
    uint8 dir_buffer[65];
} music_rcp_dir_list_info_t;    //目录列表数据包

#define MUSIC_LIST_SHOW_WAY DIR_LIST_VIEW

#endif //__MUSIC_H__

