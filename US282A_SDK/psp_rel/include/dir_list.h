/********************************************************************************
 *                            Module: music_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2012-12-25 15:00     1.0             build this file
********************************************************************************/
#ifndef __DIR_LIST_H__
#define __DIR_LIST_H__

#include "vfs_interface.h"

//最大目录个数
#define SUPPORT_MUSIC_DIR 204

//最大文件个数
#define SUPPORT_MUSIC_FILE 5000

//向前定位标志
#define PREV_FLAG 0x80000000

//目录列表项信息
typedef struct
{
    //当前目录的起始曲目号(从1开始计数)
    uint16 begin_index;
    //当前目录的层级
    uint8 cur_layer;
    uint8 reserve;
    //当前目录的父目录目录项的簇号
    uint32 far_cluster;
    //当前目录的父目录目录项的偏移
    uint32 far_direntry;
    //当前文件的目录项簇号
    uint32 cur_cluster;
    //当前文件的目录项偏移
    uint32 cur_direntry;
} list_dir_info_t;//共20个字节

//列表头信息
typedef struct
{
	//文件总数
	uint16 file_total;
	//目录总数
	uint16 dir_total;
	//当前磁盘序号
	uint8 cur_disk;
	//保留方便扩展
	uint8  retain[3];
    uint32 file_newest_time;
}list_head_info_t;//共12个字节

#endif //__DIR_LIST_H__
