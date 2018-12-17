/********************************************************************************
*                              5003
*                            Module: storage io
*                 Copyright(c) 2003-2008 Actions Semiconductor,
*                            All Rights Reserved.
*
* History:
*      <author>    <time>           <version >             <desc>
*       kkli     2008-09-02 10:00     1.0             build this file
********************************************************************************/
/*!
* \file     storageio.h
* \brief    定义对存储设备的输入输出接口
* \author   kkli
* \version 1.0
* \date  2008/09/02
*******************************************************************************/
#ifndef __STORAGEIO_H__
#define __STORAGEIO_H__

#define LOOP_PLAY_ENERGY  400 //无缝播放过滤能量门槛值



typedef struct{
    char *file_name;//扩展区的文件的文件名
    uint16 id;//文件的id
    uint16 loop_interval_ms;//两首歌间隔时间，为0是无疑
}sdd_open_arg_t;



//无缝播放配置值
typedef struct{
    uint32 *mp3_id3_size;//mp3的id3大小，也是mp3的真正数据开始地址
    uint32 *clr_Energy;//设置能量值，需要实时设置
}sdd_loop_play_t;


/*!
 * \brief
 *      提供和标准文件系统操作接口一致的存储设备输入输出接口
 */
typedef struct storage_io_s {
    /*! 读数据 ,返回实际读到的大小，读完返回-1*/
    int (*read)(void *buffer, int size, int len, struct storage_io_s *io);
    /*! 写数据 */
    int (*write)(void *buffer, int size, int len, struct storage_io_s *io);
    /*! 定位 */
    int (*seek)(struct storage_io_s *io, int offset, int whence);
    /*! 文件位置 */
    int (*tell)(struct storage_io_s *io);
    /*关闭文件*/
   // int (*close)(struct storage_io_s *io);
    /*打开文件*/
    int (*open)(void*arg);
    /*open函数的参数*/
    void* open_arg;
    
    /* 文件类型 0: VFS文件，1：SD文件 */
    int file_type;
    /* 文件句柄 */
    void *handel;

    sdd_loop_play_t loop_play;
    
} storage_io_t;

extern sdd_open_arg_t g_sdd_open_parm;

extern int sdd_open(sdd_open_arg_t* arg);
extern int sdd_read(uint8*buff, uint32 size, uint32 len, storage_io_t *io);
extern int sdd_seek(storage_io_t *io, int32 ofs, int32 whence) ;       
extern int sdd_tell(storage_io_t *io);


#endif // __STORAGEIO_H__
