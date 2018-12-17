/**
* @addtogroup 文件系统
* @{
* @defgroup folder folder
* @{
*/

//maintainer lilu
#ifndef __FOLDER_H__
#define __FOLDER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fsinfo.h"

	
/**
 * @brief 文件夹结构体指针
 */
typedef struct	_FOLDER
{
	uint16_t			FolderNum;							/**文件夹号*/
	uint16_t			ValidFolderNum;						/**有效文件夹号*/

	uint32_t	        ParentFoldStartSecNum;	            /**父目录起始扇区号*/
	uint32_t			DirSecNum;							/**FDI所在扇区号*/
	uint8_t			    DirOffset;							/**FDI在所在扇区中的偏移量*/

	uint32_t			EndDirSecNum;						/**文件夹中最后一个文件目录项所在扇区号*/
	uint8_t			    EndDirOffset;						/**文件夹中最后一个文件目录项所在扇区中的偏移量*/

	uint32_t			StartSecNum;
	uint8_t			    ShortName[9];						/**文件夹短文件名*/

	uint16_t			StartFileNum;						/**文件夹中第一个文件在全盘的编号*/

	uint16_t			IncFileCnt;							/**当前文件夹中包含的文件个数*/
	uint16_t			IncFolderCnt;						/**当前文件夹中包含的文件夹个数*/

	uint16_t			RecFileCnt;							/**当前文件夹中包含的文件个数（含子文件夹）*/
	uint16_t			RecFolderCnt;                       /**当前文件夹中包含的文件夹个数（含子文件夹）*/

	FS_CONTEXT*         FsContext;							/**文件系统上下文结构体指针*/

} FOLDER;

/**
 * @brief 设置、获取文件夹时间的结构体
 */
typedef struct
{
	uint16_t Year;	//1980～2107
	uint8_t  Month;	//1～12
	uint8_t  Date;	//1～31
	uint8_t  Hour;	//0～23 
	uint8_t  Min;	//0～59
	uint8_t  Sec;	//0～59

} FOLDER_TIME;

/**
 * @brief 按文件夹号打开文件夹
 * @param *Folder 		成功打开文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder	ParentFolder == NULL: 打开整个设备上的第FolderNum个文件夹。
 *						ParentFolder != NULL: 打开ParentFolder文件夹中的第FolderNum个文件夹。
 * @param FolderNum		文件夹号
 * @return 成功：TRUE 失败：FALSE
 */
bool FolderOpenByNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t FolderNum);

/**
 * @brief 按有效文件夹号（滤除空文件夹后的序号）打开文件夹
 * @param *Folder 		成功打开文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder	ParentFolder == NULL: 打开整个设备上的第FolderNum个文件夹。
 *						ParentFolder != NULL: 打开ParentFolder文件夹中的第ValidFolderNum个文件夹。
 * @param FolderNum		有效文件夹号
 * @return 成功：TRUE 失败：FALSE
 */
bool FolderOpenByValidNum(FOLDER* Folder, FOLDER* ParentFolder, uint16_t ValidFolderNum);

/**
 * @brief 按文件夹名称打开文件夹
 * @param *Folder 		成功打开文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder	ParentFolder == NULL: 在整个设备上查找FolderName文件夹。
 *						ParentFolder != NULL: 在ParentFolder文件夹查找FolderName文件夹。
 * @param *FolderName	文件夹短文件名，长度小于等于8字节
 * @return 成功：TRUE 失败：FALSE
 * @note 根目录名称为"\\"
 */
bool FolderOpenByName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

/**
 * @brief 按长文件夹名打开文件夹
 * @param *Folder 		成功打开文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder	ParentFolder == NULL: 在整个设备上查找FolderName文件夹。
 *						ParentFolder != NULL: 在ParentFolder文件夹查找FolderName文件夹。
 * @param *FolderName	文件夹长文件名，Unicode 16编码
 * @param Len			文件夹名长度
 * @return 成功：TRUE 失败：FALSE
 */
bool FolderOpenByLfName(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName, uint8_t Len);

/**
 * @breaf 获取文件夹的长文件名
 * @param *Folder			文件夹指针
 * @param *LongFolderName	返回的长文件夹名，长文件名最长为66个字节，所以LongFolderName数组至少要有66个字节
 * @param GetMaxLength		最大文件夹名长度
 * @return 成功：TRUE 失败：FALSE
 * @note 存在长文件夹名的条件:
 *			1,文件名长度超过8byte
 *			2,文件名中出现汉字
 *			3,文件名中出现'.'
 */
bool FolderGetLongName(FOLDER* Folder, uint8_t* LongFolderName, uint8_t GetMaxLength);	//LongFileName[]: 66 Bytes

/**
 * @breaf 文件夹创建
 * @param *Folder		成功创建文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder	在ParentFolder文件夹中创建文件，不能为NULL
 * @param *FolderName	文件夹短文件名
 */
bool FolderCreate(FOLDER* Folder, FOLDER* ParentFolder, uint8_t* FolderName);

/**
 * @breaf 长文件名文件夹创建
 * @param *Folder			成功创建文件夹后返回的文件夹结构体，空间由上层开辟
 * @param *ParentFolder		在ParentFolder文件夹中创建文件，不能为NULL
 * @param *FolderLongName	文件夹长文件名 FolderLongName[64]为长文件夹名,Unicode 16编码，长度为128Byte 空闲位置为0xFF
 * @param *FolderShortName	文件夹短文件名 FolderShortName[11]为短文件夹名,Utf-8编码，长度为11Byte 空闲位置为空格"0x20"，最后三位必为0x20
 * @param Len				长文件夹名实际长度，单位为uint16_t
 */
bool FolderCreateByLongName(FOLDER* Folder, FOLDER* ParentFolder, uint16_t* FolderLongName, uint8_t* FolderShortName, uint8_t Len);

/**
 * @brief 修改文件的创建时间、修改时间、访问时间
 * @param *Folder		文件夹指针
 * @param *CreateTime	文件创建时间结构体指针，精度为1秒
 * @param *ModifyTime	文件修改时间结构体指针，精度为2秒
 * @param *AccessTime	文件访问时间结构体指针，只有日期，不包含时间部分
 * @return 成功：TRUE 失败：FALSE
 * @note 如果某个时间信息传入的指针为NULL，则表示不改变该时间信息
 */
bool FolderSetTime(FOLDER* Folder, FOLDER_TIME* CreateTime, FOLDER_TIME* ModifyTime, FOLDER_TIME* AccessTime);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
