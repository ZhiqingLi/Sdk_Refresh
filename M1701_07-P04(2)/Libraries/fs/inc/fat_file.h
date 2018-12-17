
/**
* @addtogroup 文件系统
* @{
* @defgroup fat_file fat_file
* @{
*/

//maintainer lilu
#ifndef __FAT_FILE_H__
#define __FAT_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"
#include "folder.h"
#include "fsinfo.h"

#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08

#define	SEEK_FILE_SET				0		/**FileSeek函数的基准位置：文件起始位置*/
#define	SEEK_FILE_CUR				1		/**FileSeek函数的基准位置：文件当前位置*/
#define	SEEK_FILE_END				2		/**FileSeek函数的基准位置：文件末尾位置*/

typedef enum _FILE_TYPE
{
    FILE_TYPE_MP3,
    FILE_TYPE_WMA,
    FILE_TYPE_WAV,
    FILE_TYPE_SBC,
    FILE_TYPE_FLAC,
    FILE_TYPE_AAC,
    FILE_TYPE_AIF,
    FILE_TYPE_AMR,
    FILE_TYPE_UNKNOWN = 0xFF

} FILE_TYPE;



/**
 * @Brief	文件结构体
 */
typedef struct _FAT_FILE
{
	uint16_t	FileNumInDisk;			/**在整个设备上的文件号*/
	uint16_t	FileNumInFolder;		/**在当前文件夹中的文件号*/
	uint16_t	FolderNum;				/**所在文件夹的编号*/

	uint32_t	ParentFoldStartSecNum;	/**父目录起始扇区号*/
	uint32_t 	DirSecNum;				/**文件目录项所在扇区号*/
	uint32_t 	DirOffset;				/**FDI 所在扇区中的偏移量*/

	uint8_t		ShortName[11];			/**短文件名*/
	uint8_t	    FileType;				/**文件夹类型*/
	uint32_t	Size;					/**文件的总字节数*/
                                        
	uint32_t 	FptrIndex;              
	uint32_t 	FptrCluster;            
	uint32_t	StartClusNum;			/**文件第一个簇号*/
	uint32_t	SecterOffset;			/** Sector offset in the cluster*/
                                        
	uint32_t    Fptr;					/**当前位置在文件中偏移字节数*/
	uint8_t     Flag;				    /**标记文件状态*/

	FS_CONTEXT* FsContext;				/**文件系统上下文指针*/
} FAT_FILE;


/**
 * @Brief 文件时间的结构体
 */
typedef struct
{
	uint16_t Year;	//1980～2107
	uint8_t  Month;	//1～12
	uint8_t  Date;	//1～31
	uint8_t  Hour;	//0～23 
	uint8_t  Min;	//0～59
	uint8_t  Sec;	//0～59

} FILE_TIME;


/**
 * @brief 打开文件
 * @param *File		成功打开文件后返回的文件结构体，空间由上层开辟
 * @param file_name	包含路径的文件名   如: \\mp3\\short.mp3
 * @param mode		打开模式 FA_READ、FA_WRITE、FA_CREATE_NEW、FA_CREATE_ALWAYS
 * @return 成功：TRUE 失败：FALSE
 * @note 支持长文件名,但不支持中文
 */
bool FileOpen(FAT_FILE* File, const uint8_t* FileName, const uint8_t Mode);

/**
 * @brief 获取文件类型
 * @param *FsContext	文件系统上下文指针
 * @return 文件类型，参见enum _FILE_TYPE
 */
uint8_t FileGetType(FS_CONTEXT* FsContext);

/**
 * @brief 查询文件内部指针是否到达文件末尾
 * @param *File	文件结构体指针
 * @return 是：TRUE 否：FALSE （注：此处返回值类型应为bool而非int，此类API层的历史问题为保持向前兼容性未做修正）
 */
int32_t FileEOF(FAT_FILE* File);

/**
 * @brief 将文件内部指针重新指回文件头，恢复到文件刚打开时的状态
 * @param *File	文件结构体指针
 * @return 无
 */
void FileRewind(FAT_FILE* File);

/**
 * @brief 按文件号打开文件
 * @param *File		成功打开文件后返回的文件结构体，空间由上层开辟
 * @param *Folder	Folder == NULL: 打开整个设备上的第FileNum个文件。
 *					Folder != NULL: 打开Folder文件夹中的第FileNum个文件夹。
 * @param FileNum	文件号
 * @return 成功：TRUE 失败：FALSE
 */
bool FileOpenByNum(FAT_FILE* File, FOLDER* Folder, uint16_t FileNum);

/**
 * @brief 按文件名打开文件
 * @param *File		成功打开文件后返回的文件结构体，空间由上层开辟
 * @param *Folder	打开Folder文件夹中文件，Folder不能为NULL
 * @param *FileName	文件名
 * @return 成功：TRUE 失败：FALSE
 */
bool FileOpenByName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

/**
 * @brief 按长文件名打开文件
 * @param *File		成功打开文件后返回的文件结构体，空间由上层开辟
 * @param *Folder	打开Folder文件夹中文件，Folder不能为NULL
 * @param *FileName	文件名，需为为Unicode 16编码
 * @param Len		文件名长度
 * @return 成功：TRUE 失败：FALSE
 */
bool FileOpenByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName, uint8_t Len);

/**
 * @brief 文件内部指针移动函数
 * @param *File		文件指针
 * @param Offset	移动字节数
 * @param Base		移动基准点，SEEK_FILE_SET SEEK_FILE_CUR SEEK_FILE_END
 * @return 成功：0 失败：非0，参见fat_errno.h
 */
//此处返回值使用的为fat_errno.h定义的错误码，下次改动应将错误码统一为枚举变量。统一当前文件的返回值类型
int32_t FileSeek(FAT_FILE* File, int32_t Offset, uint8_t Base);

/**
 * @brief 文件读取
 * @param *buffer	读取数据缓冲区
 * @param size		每个单位字节数
 * @param count		单位个数（size * count 即为读取的总字节数）
 * @param *File		文件指针
 * @return 实际读取的字节数
 */
uint32_t FileRead(void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

/**
 * @brief 文件写入
 * @param *buffer	写入数据缓冲区
 * @param size		每个单位字节数
 * @param count		单位个数（size * count 即为写入的总字节数）
 * @param File		文件指针
 */
uint32_t FileWrite(const void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

/**
 * @brief 获取文件大小
 * @param *File		文件指针
 * @return 返回当前文件的字节数
 */
int32_t FileSof(FAT_FILE* File);

/**
 * @brief 获取文件内部指针的位置
 * @param *File		文件指针
 * @return 文件内部指针位置
 */
//此处文件类型应为uint32_t
int32_t FileTell(FAT_FILE* File);

/**
 * @brief 文件保存，将文件系统buffer中的DIR信息、FAT信息存盘。
 * @param *File	文件指针
 * @return 成功：0 失败：非0，参见fat_errno.h
 * @note 与FileClose的区别为不清空File结构体内容，如录音中会定时进行FileSave避免掉电或拔盘后录音文件大小为0
 */
int32_t FileSave(FAT_FILE* File);

/**
 * @brief 文件关闭，将文件系统buffer中的DIR信息、FAT信息存盘，清空File结构体内容。
 * @param *File	文件指针
 * @return 成功：0 失败：非0，参见fat_errno.h
 */
int32_t FileClose(FAT_FILE* File);

/**
 * @brief 获取文件长文件名
 * @param *File			文件指针
 * @param *LongFileName	用于返回长文件名的buffer
 * @param GetMaxLength	最大长度
 * @return 成功：TRUE 失败：FALSE
 * @note 长文件名存在的条件：
 *			1,文件名长度超过8byte 
 *			2,扩展名长度超过3byte
 *			3,文件名、扩展名中出现汉字
 *			4,文件名中存在一个以上的'.'
 *			5,文件名无后缀
 */
bool FileGetLongName(FAT_FILE* File, uint8_t* LongFileName, uint8_t GetMaxLength);

/**
 * @brief 文件清空，将文件大小清零，释放申请的簇链
 * @param *File 文件指针
 * @return 成功：TRUE 失败：FALSE
 */
bool FileFlush(FAT_FILE* File);

/**
 * @brief 文件删除
 * @param *File		文件指针
 * @return 成功：TRUE 失败：FALSE
 */
bool FileDelete(FAT_FILE* File);

/**
 * @brief 文件创建
 * @param *File		成功创建文件后返回的文件结构体，空间由上层开辟
 * @param *Folder	在Folder文件夹中创建文件，Folder不能为NULL
 * @param *FileName	短文件名，例如："123.TXT", "ABC123.MP3"
 * @return 成功：TRUE 失败：FALSE
 * @note 短文件名长度不能超过8+3字节，短文件名中不能同时出现大小写字母。
 */
bool FileCreate(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

/**
 * @brief 创建长文件名的文件
 * @param *File			成功创建文件后返回的文件结构体，空间由上层开辟
 * @param *Folder		在Folder文件夹中创建文件，Folder不能为NULL
 * @param *LongFileName	长文件名，Unicode 16编码，LongFileName必需为128字节 字节空闲位置必需置为0xFF
 * @param Len			长文件名长度
 * @return 成功：TRUE 失败：FALSE
 */
bool FileCreateByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* LongFileName, uint8_t Len);

/**
 * @brief 文件系统格式化
 * @param 无
 * @return 成功：TRUE 失败：FALSE
 * @note 不同于标准的格式化操作，该操作不改变分区的格式，只是将分区上所有文件删除，释放所有簇，该操作耗时较长，不同的设备上耗时可能几秒到几十秒
 */
bool FSFormat(void);

/**
 * @brief 修改文件的创建时间、修改时间、访问时间
 * @param *File			文件指针
 * @param *CreateTime	文件创建时间结构体指针，精度为1秒
 * @param *ModifyTime	文件修改时间结构体指针，精度为2秒
 * @param *AccessTime	文件访问时间结构体指针，只有日期，不包含时间部分
 * @return 成功：TRUE 失败：FALSE
 * @note 如果某个时间信息传入的指针为NULL，则表示不改变该时间信息
 */
bool FileSetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

/**
 * @brief 获取文件的创建时间、修改时间、访问时间
 * @param *File			文件指针
 * @param *CreateTime	文件创建时间结构体指针，精度为1秒
 * @param *ModifyTime	文件修改时间结构体指针，精度为2秒
 * @param *AccessTime	文件访问时间结构体指针，只有日期，不包含时间部分
 * @return 成功：TRUE 失败：FALSE
 * @note 如果某个时间信息传入的指针为NULL，则表示不获取该时间信息
 */
bool FileGetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

void SetFsTerminateFuc(TerminateFunc func);

extern TerminateFunc terminate_cur_fs_proc;

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FAT_FILE_H_

/**
 * @}
 * @}
 */
