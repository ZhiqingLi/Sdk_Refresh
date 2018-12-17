/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：文件选择器相关接口
 * 作者：Gary Wang
 ********************************************************************************/

//名词解释：
//  磁盘中的序号：
//      按照FAT表得出来的序号。
//      假设：FAT表的第一项是一个TXT文件，第二项是一个mp3文件，
//      第三项是一个TXT，第四项是一个mp3文件，
//      那么第一个mp3文件的磁盘中的序号就是1，第二个mp3文件的磁盘中的序号就是2。
//
//  节目单：
//      记录在vram里面的一张表。
//      假设这张表的内容是：5, 3, 6...
//      表示第一个“上台表演”的是磁盘中的序号为5的文件；
//      第二个“上台表演”的是磁盘中的序号为3的文件；
//      第三个“上台表演”的是磁盘中的序号为6的文件；
//      ...
//
//  目录列表
//      记录在vram里面的一张表。
//      它有表头、表项；
//      每个表项对应一个目录；
//      包含了音频文件的目录才会进这张表。(“包含”指的是“直接包含”，
//          如果这个目录下没有音频文件，但它的子目录下有，
//          那么，这个目录不能进表)
//
//详细描述：
//  本模块的功能是：文件的定位、统计。
//  有三种方式：
//      不依赖目录列表、
//      依赖目录列表，但不依赖“重排了的节目单”、
//      依赖“重排节目单”。
//

#ifndef _EH_FSEL_ENTRY_
#define _EH_FSEL_ENTRY_

#include "enhanced.h"
#include "eh_globle_data.h"

//先随机定位到组，再随机定位到组内的成员，播放完一组再切到下一组

#define EH_GET_EXT_FILE EXT_NAME_ALL_FILE    //前两字节为“*f”，表示dir所有文件
#define EH_GET_EXT_DIR  EXT_NAME_ALL_DIR     //前两字节为“*d”，表示dir所有目录
#define EH_GET_EXT_ALL  EXT_NAME_ALL         //前两字节为“**”，表示dir所有目录和文件
#define EH_DIR_FLAG   ('d'*0x100+'*')
#define EH_FILE_FLAG  ('f'*0x100+'*')

typedef bool (*fsel_get_next_p)(uint8 *strfile);
typedef bool (*fsel_get_prev_p)(uint8 *strfile);
typedef bool (*fsel_get_byno_p)(uint8 *strfile, uint16 num);
typedef bool (*fsel_set_location_p)(void *location);
typedef bool (*fsel_get_location_p)(void *location);
typedef bool (*fsel_get_nextdir_p)(uint8 *strdir);
typedef bool (*fsel_get_prevdir_p)(uint8 *strdir);
typedef uint16 (*fsel_get_total_p)(void);

//
extern void fsel_init_sysdata(void);

//for shffule
extern bool fsel_init_shuffle(uint16 num_total);

extern uint16 fsel_shuffle_next_file(void);

extern uint16 fsel_shuffle_prev_file(void);
//end for shuufle

//for directory functions
//extern uint16 fsel_total_file_cur(uint8* dir_info, uint8 check_flag);

extern uint16 fsel_get_all_dirs_total_files(void);

extern bool fsel_dir_get_byno(uint8 *strfile, uint16 num);

extern bool fsel_dir_get_byno_all(uint8 *strfile, uint16 num);

extern bool fsel_dir_get_location(file_location_t *location);

extern bool fsel_dir_set_location(file_location_t *location);

extern bool fsel_dir_get_next(uint8 *strfile);

extern bool fsel_dir_next_dir(uint8 *strdir);

extern bool fsel_dir_next_layer(uint8 *strfile);

extern bool fsel_dir_get_prev(uint8 *strfile);

extern bool fsel_dir_prev_dir(uint8 *strdir);

extern bool fsel_dir_prev_layer(uint8 *strfile);

//for dirlist functions

extern bool fsel_list_get_byno_all(uint8 *strfile, uint16 num, uint16 show_num);

extern uint16 fsel_total_file_curdir(void);

extern uint16 fsel_total_file_alldir(void);

extern bool fsel_list_get_location(file_location_t *location);

extern bool fsel_list_set_location(file_location_t *location);

extern bool fsel_list_get_next(uint8 *strfile);

extern bool fsel_list_next_dir(uint8 *strdir);

extern bool fsel_list_get_prev(uint8 *strfile);

extern bool fsel_list_prev_dir(uint8 *strdir);

//for sorted dir list functions
extern uint16 fsel_check_index_num(uint16 cur_num);

extern uint16 fsel_sortlsit_vram_read(uint16 src_num);

extern bool fsel_sortlist_get_byno_all(uint8 *strfile, uint16 num);

extern bool fsel_sortlist_get_location(file_location_t *location);

extern bool fsel_sortlist_set_location(file_location_t *location);

extern bool fsel_sortlist_get_next(uint8 *strfile);

extern bool fsel_sortlist_get_prev(uint8 *strfile);

extern bool fsel_sortlist_next_dir(uint8 *strdir);

extern bool fsel_sortlist_prev_dir(uint8 *strdir);

extern uint16 fsel_total_file_sortlist(void);

bool filtrate_dir(uint8 dir_type);
bool fsel_last_list_entry(void);
bool fsel_first_list_entry(void);

#endif //_EH_FSEL_ENTRY_
