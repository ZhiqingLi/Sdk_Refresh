/********************************************************************************
 *                              USDK(GL5110)
 *                            Module: KERNEL
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>           <version >             <desc>
 *      wuyueqian     2011-9-10 09:00     1.0             build this file
 ********************************************************************************/
/*!
 * \file     vfs_interface.h
 * \brief    虚拟文件系统接口
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/

#ifndef __VFS_INTERFACE_H__
#define __VFS_INTERFACE_H__

#include <storage_device.h>
#include <vfs.h>
#include <attr.h>
#include <typeext.h>

#define FORMAT_FAT32 0
#define FORMAT_FAT16 1
#define FORMAT_EXFAT 2

//定义CD命令的操作类型
#define  CD_UP			'\\'    //改变当前目录到上一级父目录，目录项指针指向目录起始位置；
#define  CD_BACK		'|'     //改变当前目录到上一级父目录，目录项指针指向之前CD进去的子目录
#define  CD_ROOT		':'     //改变当前目录到根目录     
#define  CD_SUB			'/'     //改变当前目录到当前目录项对应的子目录，此时参数c起作用

//定义DIR命令的操作类型
#define  DIR_NEXT			0x80    //向后检索
#define  DIR_PREV			0x81    //向前检索
#define  DIR_HEAD			0x82    //从目录首向后检索
#define  DIR_TAIL			0x83    //从目录尾向前检索

//定义DIR 扩展名的映射
#define  EXT_BITMAP_BUFF		0x80000000
#define  EXT_NAME_MP3	        (0x80000000>>1)
#define  EXT_NAME_WMA	        (0x80000000>>2)
#define  EXT_NAME_WAV	        (0x80000000>>3)
#define  EXT_NAME_AAC	        (0x80000000>>4)
#define  EXT_NAME_FLAC        	(0x80000000>>5)
#define  EXT_NAME_APE	        (0x80000000>>6)
#define  EXT_NAME_AVI	        (0x80000000>>7)
#define  EXT_NAME_TXT	        (0x80000000>>8)
#define  EXT_NAME_JPG	        (0x80000000>>9)
#define  EXT_NAME_JPEG        	(0x80000000>>10)
#define  EXT_NAME_BMP	        (0x80000000>>11)
#define  EXT_NAME_GIF	        (0x80000000>>12)
#define  EXT_NAME_LRC	        (0x80000000>>13)
#define  EXT_NAME_ASF	        (0x80000000>>14)
#define  EXT_NAME_OGG	        (0x80000000>>15)
#define  EXT_NAME_WMV	        (0x80000000>>16)
#define  EXT_NAME_AMV	        (0x80000000>>17)
#define  EXT_NAME_AAX	        (0x80000000>>18)
#define  EXT_NAME_AA 	        (0x80000000>>19)
#define  EXT_NAME_MP1	        (0x80000000>>20)
#define  EXT_NAME_MP2	        (0x80000000>>21)
#define  EXT_NAME_M4A	        (0x80000000>>22)
#define  EXT_NAME_POS	        (0x80000000>>23)
#define  EXT_NAME_ACT	        (0x80000000>>24)   /* ACT is added by fix FS_ROM rom_ext_name.  2013/3/13 14:42:24. */
#define  EXT_NAME_BMK	        (0x80000000>>25)
#define  EXT_NAME_BIN	        (0x80000000>>26)
#define  EXT_NAME_APK	        (0x80000000>>27)
#define  EXT_NAME_PCM	        (0x80000000>>28)
#define  EXT_NAME_ALL			(0x80000000>>29)
#define  EXT_NAME_ALL_FILE 		(0x80000000>>30)
#define  EXT_NAME_ALL_DIR 		(0x80000000>>31)

//定义OPEN文件的方式
#define  R_NORMAL_SEEK		0x00    //普通seek模式
#define  R_FAST_SEEK		0x01    //快速seek模式(R_FAST_SEEK每次打开后需要先seek到文件末尾)
#define  OPEN_MODIFY		0x02    //修改模式（不允许跨越文件大小写）
#define  LARGE_FILE_SEEK	0x03    //大文件seek模式（大文件打开可能较慢）
#define  OPEN_RECOVER		0x04    //恢复模式（允许恢复写后没有正常close的文件,主要用于录音掉电恢复）


//定义文件及目录操作的类型
#define  FS_DIR				0x00
#define  FS_FILE			0x01


//定义时间操作的类型
#define  FS_TIME_CREATE		0X00
#define  FS_TIME_MODIFY		0x01


//定义文件SEEK的方式
#define SEEK_SET    0   // 从文件首往文件尾定位，offset为正数
#define SEEK_CUR    1   //从当前位置往文件头或尾定位；正数表示向文件尾seek，负数表示向文件头seek
#define SEEK_END    2   // 从文件尾往文件首定位，offset为负数


//定义获取分区空间的类型
#define  GET_TOTAL_SPACE			0x00
#define  GET_SPARE_SPACE			0x01

//定义文件属性
#define         ATTR_NORMAL         0x00    // normal file
#define         ATTR_READONLY       0x01    // file is readonly
#define         ATTR_HIDDEN         0x02    // file is hidden
#define         ATTR_SYSTEM         0x04    // file is a system file
#define         ATTR_VOLUME         0x08    // entry is a volume label
#define         ATTR_LONG_FILENAME  0x0f    // this is a long filename entry
#define         ATTR_DIRECTORY      0x10    // entry is a directory name
#define         ATTR_ARCHIVE        0x20    // file is new or modified

//定义最大支持目录层级
#define  MAX_DIR_LAYER		9

#define  LONG_NAME_TAG		0xfeff

#define  UDISK_VRAM_MAGIC    (0x5aa5)

typedef struct {
	uint16 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 minute;
	uint8 second;
} file_time_t;

//定义目录层级结构的内存占用情况
typedef struct {
	uint8 buffer[MAX_DIR_LAYER*8+1];
} pdir_layer_t;

typedef struct {
	uint8 buffer[MAX_DIR_LAYER*8+2];
} pdir_layer_ext_t;
typedef struct {
	uint8 buffer[8];
} pfile_offset_t;

typedef struct
{   
    uint16 magic;
    uint16 write_flag;
} udisk_vram_info_t;

//定义文件系统错误类型
#define  ERR_PHY_R          		0x02   //磁盘读错误   
#define  ERR_PHY_W          		0x03   //磁盘写错误  
#define  ERR_NO_FORMAT          	0x04   //磁盘未格式化   
#define  ERR_WRITE_PROTECTED    	0x05   //磁盘写保护     
#define  ERR_FILE_OVER_OFFSET		0x06   //文件操作超出文件边界   
#define  ERR_DIR_OVER_OFFSET    	0x07   //目录操作超出目录边界    
#define  ERR_FILE_NO_EXIST    		0x08   //文件不存在   
#define  ERR_DIR_NO_EXIST    		0x09   //目录不存在       
#define  ERR_FILE_OP_SPACE    		0x10   //表示文件操作时没有磁盘空间,不能写数据   
#define  ERR_DIR_OP_SPACE    		0x11   //目录操作时没有磁盘空间,不能扩展目录,新建子目录   
#define  ERR_FILE_OP_ROOT_FULL		0x12   //目录不存在       
#define  ERR_DEL_DIR_NO_EMPTY		0x13   //删除目录时返回,表示删除的目录非空        
#define  ERR_DIRENTRY_ATTR			0x14
#define  ERR_DIR_TYPE               0x15  
#define  ERR_FHANDLE_FULL			0x16
#define  ERR_DIR_EXIST              0x17   //目录已存在(建立目录时)
#define  ERR_FILE_EXIST             0x18   //文件已存在（建立文件时）
#define  ERR_VAILD_PARA				0x19   //非法的参数

typedef enum {
    VFS_CD = 0,
    VFS_DIR,
    VFS_GET_NAME,
    VFS_FILE_DIR_OFFSET,
    VFS_MAKE_DIR,
    VFS_FILE_OPEN,
    VFS_FILE_CREATE,
    VFS_FILE_CLOSE,
    VFS_FILE_DIR_REMOVE,
    VFS_FILE_GET_SIZE,
    VFS_GET_TIME,
    VFS_FILE_SEEK,
    VFS_FILE_TELL,
    VFS_FILE_READ,
    VFS_FILE_WRITE,
    VFS_GET_SPACE,
    VFS_GET_ERR_INFO,
    VFS_FILE_DIR_EXIST,
    VFS_FILE_CUT,
    VFS_FILE_ATTRALTER,
    VFS_FILE_RENAME,
    VFS_CREATEVOLUME,
    VFS_SET_TIME,
    VFS_SET_PRIVATE_DATA,
    VFS_CLEAR_PRIVATE_DATA,
    VFS_FILE_MOVE,
    VFS_DIR_FILE,
    VFS_JUMP_DIR,
    VFS_CUT_FILE_TAIL,
    VFS_FILE_DIVIDED,
    VFS_FILE_INSERT   
}vfs_cmd_e;

extern void * vfs_op_entry(void *vfs_mount_index,void *param1, void *param2, void *param3,  vfs_cmd_e cmd) __FAR__;

/*
bool vfs_cd(vfs_mount_t *vfs_mount, uint8 mode, uint8* input_name)
功能描述：根据用户输入将当前目录指向当前目录的子目录，父目录或直接返回根目录
参数说明：
vfs_mount：挂载文件系统后返回的句柄
mode：cd的操作命令类型，具体命令请参照vfs_interface.h 第31行下的定义
ptr_input_name：子目录名的buffer指针；为NULL的时候默认为进入当前目录。前两个字符为"0xfffe"则表示通过长名切换目录
返回值：0-失败；1-成功
*/
#define vfs_cd(vfs_mount,mode,input_name)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(mode), (void*)(input_name), (void*)(0), VFS_CD)

/*
uint32 vfs_dir(vfs_mount_t *vfs_mount, uint8 mode, uint8* input_name,uint32 ext_name_bitmap)
功能描述：在当前目录按各种方式检索目录或文件
参数说明：
vfs_mount：挂载文件系统后返回的句柄
mode: 检索的命令方式，具体命令请参照vfs_interface.h 第37行下的定义
ptr_input_name：用来存放输入文件名的buffer指针。如指针为NULL，则直接从当前目录项开始dir或不起作用
ext_name_bitmap：如是有效内存地址，则表示输入扩展名字符串的地址；如不是则表示要dir的bitmap。此参数如最高位为1表示传入的内存地址，否则则是bitmap值
		 具体的bitmap对应的意义请参照vfs_interface.h 第43行下的定义
返回值：0-失败；非0-成功
*/
#define vfs_dir(vfs_mount,mode,input_name,ext_name_bitmap)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(mode), (void*)(input_name), (void*)(ext_name_bitmap), VFS_DIR)

/*
uint16 vfs_get_name(vfs_mount_t *vfs_mount, uint8* output_name, uint16 longname_length)
功能描述：取当前的文件名（优先返回长名，没有长名则返回短名）或后缀名
参数说明：
vfs_mount：挂载文件系统后返回的句柄
output_name：用来存放输出文件名的buffer指针
longname_length：输入为要获取的长名字符数（包括长名标记和结束符），如为0表示获取后缀名
返回值：0-失败；非0-实际返回的获取到的文件名字符数。为取长名时返回实际函数输出的长名字符的个数（包括长名标记和结束符0x0000），为短名时返回11；获取后缀名时返回3
*/
#define vfs_get_name(vfs_mount,output_name,longname_length)  (uint16)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(output_name), (void*)(uint32)(longname_length), (void*)(0), VFS_GET_NAME)

/*
bool vfs_file_dir_offset(vfs_mount_t *vfs_mount, pdir_layer_t* ptr_layer, pfile_offset_t* ptr_file, uint8 mode)
功能描述：获取或设置当前目录项的位置信息
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_layer: 存储要获取或设置的目录层级信息buffer指针；为NULL表示当前目录层的信息 
ptr_file ：存储要获取或设置的文件位置信息buffer指针
mode     ：操作方式：0-表示获取，1-表示设置
返回值：0-失败；1-成功
*/
#define vfs_file_dir_offset(vfs_mount,ptr_layer,ptr_file,mode)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_layer), (void*)(ptr_file), (void*)(uint32)(mode), VFS_FILE_DIR_OFFSET)

/*
bool vfs_make_dir(vfs_mount_t *vfs_mount, uint8* input_name)
功能描述：在当前目录下生成一个用户程序指定目录名的子目录。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name: 要创建的目录名指针 
返回值：0-失败；1-成功
*/
#define vfs_make_dir(vfs_mount,input_name)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name), (void*)(0), (void*)(0), VFS_MAKE_DIR)

/*
uint32 vfs_file_open(vfs_mount_t *vfs_mount, uint8* input_name,uint8 mode)
功能描述：根据用户输入的文件名在当前目录中打开一个已存在的文件。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name: 待打开文件的文件名的输入指针；为null表示直接打开当前目录项指向的文件
mode ：    打开方式，具体参照vfs_interface.h 第77行下的定义 
返回值：0-失败；非0-打开的文件句柄
*/
#define vfs_file_open(vfs_mount,input_name,mode)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name), (void*)(uint32)(mode), (void*)(0), VFS_FILE_OPEN)

/*
uint32 vfs_file_create(vfs_mount_t *vfs_mount, uint8* input_name,uint32 lentgh)
功能描述：根据用户输入的文件名创建一个文件,用户可获得当前操作文件的句柄。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name: 为将要创建的文件的文件名字符串指针
lentgh ：   创建空文件的长度（单位为byte,不足512byte以512计算），为0表示创建空文件 
返回值：0-失败；非0-创建的文件句柄
*/
#define vfs_file_create(vfs_mount,input_name,lentgh)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name),(void*)(lentgh), (void*)(0), VFS_FILE_CREATE)

/*
bool vfs_file_close(vfs_mount_t *vfs_mount, uint32 fhandle)
功能描述：关闭文件,用户输入需要关闭的文件句柄。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
fhandle: 需要关闭的文件句柄 
返回值：0-失败；1-成功
说明：fhandle是vfs_file_open或vfs_file_create返回的句柄
*/
#define vfs_file_close(vfs_mount,fhandle)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(fhandle), (void*)(0), (void*)(0), VFS_FILE_CLOSE)

/*
bool vfs_file_dir_remove(vfs_mount_t *vfs_mount, uint8* input_name, uint8 type)
功能描述：在当前目录下删除一个用户程序指定目录或文件，删除目录时要求目录为空。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name: 要删除的目录或文件名；如为null则直接删除当前目录项指向的目录或文件；
type  ：要操作的类型：0-删除目录；1-删除文件
返回值：0-失败；1-成功
*/
#define vfs_file_dir_remove(vfs_mount,input_name,type)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name), (void*)(uint32)(type), (void*)(0), VFS_FILE_DIR_REMOVE)


/*
bool vfs_file_get_size(vfs_mount_t *vfs_mount, uint32* output_length, uint8* ptr_file, uint8 mode)
功能描述：取文件的长度，字节为单位
参数说明：
vfs_mount：挂载文件系统后返回的句柄
output_length: 返回的文件长度（以字节为单位）
ptr_file： 待获取长度的文件句柄或文件名输入指针
mode  ：表示参数ptr_file的意义。0-表示ptr_file为文件句柄；1-表示ptr_file为文件名指针;ptr_file为null表示当前目录项指向的文件

返回值：0-失败；1-成功
*/
#define vfs_file_get_size(vfs_mount,output_length,ptr_file,mode) (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(output_length), (void*)(ptr_file), (void*)(uint32)(mode), VFS_FILE_GET_SIZE)

/*
bool vfs_get_time(vfs_mount_t *vfs_mount, file_time_t* output_time, uint8* input_name, uint8 type)
功能描述：获取文件的创建时间或修改时间
参数说明：
vfs_mount：挂载文件系统后返回的句柄
output_time:  时间输出buf指针
ptr_input_name： 待获取时间的文件的文件名输入指针；为null表示当前目录项指向的文件
type  ：FS_TIME_CREATE-获取创建时间；FS_TIME_MODIFY-获取修改时间
返回值：0-失败；1-成功
*/
#define vfs_get_time(vfs_mount,output_time,input_name,type)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(output_time), (void*)(input_name), (void*)(uint32)(type), VFS_GET_TIME)

/*
bool vfs_file_seek(vfs_mount_t *vfs_mount, int32 offset, uint8 type, uint32 fhandle)
功能描述：定位文件的字节偏移，实现：
	a.根据相对文件首的偏移量，实现用户程序的顺序读和随机读。
	b.实现从当前位置往文件首或文件尾偏移，实现用户程序的顺序读和随机读。
	c.根据相对文件首的偏移量，实现用户程序的对指定位置数据的修改。
	d.配合read支持用户程序顺序读数据，快进快退以及随机定位读数据，另外可以方便实现回写修改已经生成的文件。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
offset:  对应SEEK偏移量，范围是2GB
type： 对应SEEK类型，具体参照vfs_interface.h 第95行下的定义
fhandle  ：已打开的文件句柄 
返回值：0-失败；1-成功
*/
#define vfs_file_seek(vfs_mount,offset,type,fhandle)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(offset), (void*)(uint32)(type), (void*)(fhandle), VFS_FILE_SEEK)

/*
bool vfs_file_tell(vfs_mount_t *vfs_mount, uint32* ptr_offset ,uint32 fhandle)
功能描述：取当前读写操作的指针，指针是指相对文件头的字节偏移量。读数据时用户调用该函数记录AB点，配合seek和read实现数据的AB读取。
	  写数据时，支持用户程序修改已生成的文件。
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_offset:  文件当前读写位置相对文件头的偏移量
fhandle  ：已打开的文件句柄 
返回值：0-失败；1-成功
*/
#define vfs_file_tell(vfs_mount,ptr_offset,fhandle)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_offset), (void*)(fhandle), (void*)(0), VFS_FILE_TELL)

/*
uint32 vfs_file_read(vfs_mount_t *vfs_mount, uint8* ptr_data_buffer, uint32 byte_count, uint32 fhandle)
功能描述：从文件的当前位置读取一定大小的数据
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_data_buffer:  读操作数据输出buffer的指针，buffer的大小一定不能小于后面指定的byte_count
byte_count：读的字节数
fhandle  ：已打开的文件句柄 
返回值：0-失败；大于0-实际读到的字节数
*/
#define vfs_file_read(vfs_mount,ptr_data_buffer,byte_count,fhandle)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_data_buffer), (void*)(byte_count), (void*)(fhandle), VFS_FILE_READ)

/*
uint32 vfs_file_write(vfs_mount_t *vfs_mount, uint8* ptr_data_buffer, uint32 byte_count, uint32 fhandle)
功能描述：从文件的当前位置读取一定大小的数据
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_data_buffer:  写数据buffer的指针
byte_count：写的字节数
fhandle  ：已打开的文件句柄 
返回值：0-失败；大于0-实际写入的字节数
*/
#define vfs_file_write(vfs_mount,ptr_data_buffer,byte_count,fhandle)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_data_buffer), (void*)(byte_count), (void*)(fhandle), VFS_FILE_WRITE)

/*
bool vfs_get_space(vfs_mount_t *vfs_mount, uint32* ptr_sector_count, uint8 type)
功能描述：获取磁盘分区空间，根据输入参数不同选择要求返回磁盘分区总的扇区数还是剩余扇区数
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_sector_count:  返回分区的扇区数
type：0-表示调用将返回表示当前磁盘分区总空间的扇区数；1-表示返回当前磁盘分区剩余空间的扇区数
返回值：0-失败；1-成功
*/
#define vfs_get_space(vfs_mount,ptr_sector_count,type)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_sector_count), (void*)(uint32)(type), (void*)(0), VFS_GET_SPACE)

/*
uint8 vfs_get_err_info(vfs_mount_t *vfs_mount)
功能描述：获取文件系统出错信息
参数说明：
vfs_mount：挂载文件系统后返回的句柄
返回值：0-没有错误；非0-错误信息，具体参照vfs_interface.h 第143行下的定义
*/
#define vfs_get_err_info(vfs_mount)  (uint8)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(0), (void*)(0), (void*)(0), VFS_GET_ERR_INFO)

/*
uint32 vfs_file_dir_exist(vfs_mount_t *vfs_mount, uint8* input_name, uint8 type)
功能描述：判断当前目录是否有指定的子目录或文件
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name:要判断的文件或目录名指针
type    :要判断的类型：0-目录；1-文件
返回值：0-不存在；非0-存在，并返回该文件或目录所在的目录的首簇号
*/
#define vfs_file_dir_exist(vfs_mount,input_name,type)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name), (void*)(uint32)(type), (void*)(0), VFS_FILE_DIR_EXIST)

/*
uint32 fat_file_cut(vfs_mount_t *vfs_mount, uint32 reserve1, uint32 reserve2, uint32 reserve3)
功能描述：该函数暂时保留，无实现任何功能
参数说明：
vfs_mount：挂载文件系统后返回的句柄
reserve1:保留
reserve2:保留
reserve3:保留
返回值：0-失败；非0-成功
*/
#define vfs_file_cut(vfs_mount,reserve1,reserve2,reserve3)  (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(reserve1), (void*)(reserve3), (void*)(reserve3), (void*)(0), VFS_FILE_CUT)

/*
uint8 vfs_file_attralter(vfs_mount_t *vfs_mount, uint8 attr, uint8* ptr_file, uint8 mode)
功能描述：获取或修改文件的属性
参数说明：
vfs_mount：挂载文件系统后返回的句柄
attr:0-获取文件的属性；非0-设置成的文件属性，具体参照vfs_interface.h 第105行下的定义
ptr_file    :待获取或修改属性的文件句柄或文件名输入指针；
mode：表示参数ptr_file的意义。0-表示ptr_file为文件句柄；1-表示ptr_file为文件名指针;ptr_file为null表示当前目录项指向的文件或目录。
返回值：0-失败；非0-返回要获取或设置的属性值
*/
#define vfs_file_attralter(vfs_mount,attr,ptr_file,mode)  (uint8)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(attr), (void*)(ptr_file), (void*)(uint32)(mode), VFS_FILE_ATTRALTER)

/*
bool vfs_file_rename(vfs_mount_t *vfs_mount, uint8* ptr_new_name, uint8* ptr_file, uint8 mode)
功能描述：获取或修改文件的属性
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_new_name:新文件名字符串指针
ptr_file    :待重命名的文件句柄或文件名指针；
mode：表示参数ptr_file的意义。0-表示ptr_file为文件句柄；1-表示ptr_file为文件名指针;ptr_file为null表示当前目录项指向的文件或目录
返回值：0-失败；1-成功
*/
#define vfs_file_rename(vfs_mount,ptr_new_name,ptr_file,mode)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_new_name), (void*)(ptr_file), (void*)(uint32)(mode), VFS_FILE_RENAME)

/*
bool vfs_create_volume(vfs_mount_t *vfs_mount, uint8* input_name)
功能描述：创建卷标
参数说明：
vfs_mount：挂载文件系统后返回的句柄
input_name: 卷标名字符串指针;input_name=NULL表示查询是否存在卷标，ptr_input_name=非NULL直接修改卷标
返回值：0-失败；1-成功
*/
#define vfs_create_volume(vfs_mount,input_name)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(input_name), (void*)(0), (void*)(0), VFS_CREATEVOLUME)

/*
bool vfs_set_time(vfs_mount_t *vfs_mount, file_time_t* ptr_input_time, uint8* input_name, uint8 type)
功能描述：创建卷标
参数说明：
vfs_mount：挂载文件系统后返回的句柄
ptr_input_time：需要设置的时间输入buf指针
input_name: 待设置时间的文件的文件名输入指针；为null表示前目录项指向的文件
type：FS_TIME_CREATE-设置创建时间;FS_TIME_MODIFY-设置修改时间
返回值：0-失败；1-成功
*/
#define vfs_set_time(vfs_mount,ptr_input_time,input_name,type)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(ptr_input_time), (void*)(input_name), (void*)(uint32)(type), VFS_SET_TIME)

/*
bool vfs_file_move(vfs_mount_t *vfs_mount, void *fat_direntry, uint8 *ptr_file_name, uint8 mode)
功能描述：删除或增加目录项
参数说明：
vfs_mount：挂载文件系统后返回的句柄
fat_direntry：目录项信息指针
ptr_file_name: 待增加或删除的目录名字符串指针；当mode==0时，如果为null表示前目录项指向的文件;否则参数无效
mode：0-删除目录项；1-增加目录项
返回值：0-失败；1-成功
*/
#define vfs_file_move(vfs_mount,fat_direntry,ptr_file_name,mode)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(fat_direntry), (void*)(ptr_file_name), (void*)(uint32)(mode), VFS_FILE_MOVE)

/*
uint32 fat_dir_current_entry_file(vfs_mount_t *vfs_mount, uint32 mode, uint8* input_name, uint32 ext_name_bitmap)
功能描述：通过后缀检索定位到文件位置
参数说明：
vfs_mount：
mode：0x80000000-从尾往前检索；其他值：从头往后检索；如果为0或0x80000000时，表示检索定位到第一个配置后缀的文件的位置。
      其他值:mode=n则表示检索定位到第N个配置后缀的文件的位置。
ptr_input_name：存储检索定位到的文件位置信息。
ext_name_bitmap: 如是有效内存地址，则表示输入扩展名字符串的地址；如不是则表示要dir的bitmap。此参数如最高位为1表示传入的内存地址，否则则是bitmap值
		 具体的bitmap对应的意义请参照vfs_interface.h 第43行下的定义
返回值：0-失败；非0-成功
*/
#define vfs_dir_current_entry_file(vfs_mount,mode,input_name,ext_name_bitmap) (uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(mode), (void*)(input_name), (void*)(uint32)(ext_name_bitmap), VFS_DIR_FILE)

/*
bool fat_jump_to_direntry(vfs_mount_t *vfs_mount, uint32 mode, uint8* direntry_info,uint8* reserve)
功能描述：根据提供的目录信息，直接跳转到该目录的起始位置
参数说明：
vfs_mount：挂载文件系统后返回的句柄
mode：0x80000000-从尾往前检索；其他值：从头往后检索；
direntry_info：需要跳转到的目录信息
reserve: 保留，暂时无用
返回值：0-失败；1-成功
*/
#define vfs_jump_to_direntry(vfs_mount,mode,direntry_info,reserve) (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(mode), (void*)(direntry_info), (void*)(reserve), VFS_JUMP_DIR)


/*
bool vfs_cut_file_tail(vfs_mount_t *vfs_mount, uint32 cut_length, fhandle_t *fhandle)
功能描述：关闭文件之前去掉文件的尾部的部分数据
参数说明：
vfs_mount：挂载文件系统后返回的句柄
cut_length：要丢弃的长度，以字节为单位，数值最好应该是512的整数倍
fhandle：操作文件的句柄。
返回值：0-失败；非0-成功
说明：该接口必须在文件关闭之前调用
*/
#define vfs_cut_file_tail(vfs_mount,cut_length,fhandle)  (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(fhandle), (void*)(fhandle), (void*)(0), VFS_CUT_FILE_TAIL)

/*
bool vfs_file_divided(vfs_mount_t *vfs_mount, uint8* source_file, uint8* new_file, uint32 div_length)
功能描述：将文件分割成两个
参数说明：
vfs_mount：挂载文件系统后返回的句柄
source_file：源文件，也就是被分割文件的文件名
new_file：分割产生的新文件的文件名
div_length：将原文件分割的长度，如，一个文件的长度为4096 byte，divided_length为1024，那么源文件会被分割为长度为1024的文件，而新的文件的长度就是3072byte
返回值：0-失败；非0-成功
*/
#define vfs_file_divided(vfs_mount,source_file,new_file,div_length) (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(source_file), (void*)(new_file), (void*)(uint32)(div_length), VFS_FILE_DIVIDED)

/*
bool vfs_ file_insert(vfs_mount_t *vfs_mount, fhandle_t *fhandle, uint8* insert_file_name)
功能描述：将一个文件的数据插入到另外一个文件，也就是文件合并
参数说明：
vfs_mount：挂载文件系统后返回的句柄
fhandle：已打开的源文件句柄
insert_file_name：需要被合并的文件名，合并后该文件在目录项的内容将被删除
返回值：0-失败；非0-成功
*/
#define vfs_file_insert(vfs_mount,fhandle,insert_file_name) (bool)(uint32)vfs_op_entry((void*)(vfs_mount), (void*)(uint32)(fhandle), (void*)(insert_file_name), (void*)(0), VFS_FILE_INSERT)

#endif



