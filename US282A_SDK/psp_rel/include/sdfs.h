/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sd 文件信息定义
 * 作者：liminxian
 ********************************************************************************/


#ifndef _SDFS_H
#define _SDFS_H
#include <attr.h>
#include <typeext.h>

#define MAX_HEANDLE_NUM 8

#define MODE_READ   0
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#define SDFILE_CLOSED   0xFF
#define  MFSLOCATEPAGE  0xf0  


#define SD_DIR_OFFSET 0x200
#define SD_DIR_NAME_LEN 0xb
#define SD_DIR_ENTRY_LEN 0x20

#ifndef PC
typedef struct
{
    unsigned char   isvalid;
    unsigned char   reserve[3];
    unsigned int    startaddr;
    unsigned int   endaddr;
    unsigned int   rwpointer;
} sd_file_t;
#else
typedef struct
{
    unsigned char   isvalid;
    unsigned char   reserve[3];
    unsigned int    startaddr;
    unsigned int   endaddr;
    unsigned int   rwpointer;
    char    filename[13];
} sd_file_t;
#endif

typedef struct
{
    unsigned char     fname[11];
    unsigned char     fattr;
    unsigned char     reserve0[2];
    unsigned short    version;
    unsigned int   offset;
    unsigned int   size;
    unsigned int   real_size;
    unsigned int   checksum;
} sd_dir_t;

typedef sd_file_t* sd_handle;


#endif
