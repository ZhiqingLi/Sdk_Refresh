#ifndef _EH_GLOBLE_DATA_H
#define _EH_GLOBLE_DATA_H

#include "enhanced.h"
#include "dir_list.h"

#define SET_GROUP_MAX   64
#define SET_MEMBER_MAX  256
#define SET_PREV_MAX    16

extern uint8 eh_fsel_type; //模块类型

extern fsel_param_t fsel_init_val; //模块参数
extern fsel_error_e fsel_error; //错误类型
extern uint8 eh_dir_layer; //当前目录或列表的层次，0表示根目录，1表示根目录的子目录，以此类推
extern uint8 eh_shuffle_sw; //SHUFFLE 开关

extern uint16 eh_file_no; //当前文件在目录或列表中的序号
extern uint16 eh_file_total; //当前目录或列表的总文件个数

extern uint16 eh_file_no_all; //当前文件在磁盘中的序号
extern uint16 eh_file_total_all; //当前磁盘总文件个数

extern uint32 eh_cur_ext_name; //当前选择歌曲的扩展名
//extern uint16 eh_root_dir; //根目录的子目录指针，1,2，。。。

extern int32 eh_vfs_mount; //FS 识别号

extern uint16 eh_dir_no; //dir的个数

//存放目录路径信息
extern pdir_layer_t pdir_layer_buffer;
//存放文件位置信息
extern pfile_offset_t pfile_offset_buffer;

//for total all save use
//存放目录路径信息
extern pdir_layer_t pdir_layer_total;
//存放文件位置信息
extern pfile_offset_t pfile_offset_total;

//for shuffle
extern uint16 shuffle_total;//shuffle的总个数
extern uint8 group_map[SET_GROUP_MAX / 8];//分组，共64组(bit为1-未用,0-已用)
extern uint8 member_map[SET_MEMBER_MAX / 8];//组内成员，共256(bit为1-未用,0-已用)
extern uint8 member_avg;//成员平均值，商值
extern uint8 member_avg_group;//平均值成员的组号起始
extern uint8 group_max;//有效的组号
extern uint8 member_max;//当前组的最大成员个数
extern uint8 use_member_flag;//使用组内shuffle功能
extern uint8 cur_group;//当前的组号（1，2，。。。
extern uint16 cur_member;//当前组内的成员号（1，2，。。。
extern uint16 filtrate_shuffle_no; //shuffle过滤播放序号，避免重复；取值为0,1~n

extern uint16 play_num_list[SET_PREV_MAX];
extern uint8 cur_save_pt; //当前存的指针(0~)
extern uint8 cur_save_prev_count;
extern uint8 cur_save_next_count;
extern uint8 cur_overturn_flag; //表翻转标记,1-yes,0-no

//end for shuffle

//for dir list

//列表存储位置
extern uint8 *plist_start_addr;
//目录列表项信息
extern list_dir_info_t list_dir_info;
//指针
extern list_dir_info_t* plist_info;
//目录列表表头信息
extern list_head_info_t list_head_info;
//指针
extern list_head_info_t*plist_head;

//end for dirlist

//record long name
extern uint8 name_buf[16];

//for sort list file num 
extern uint16 eh_disk_file_no;
#endif //#ifndef _EH_GLOBLE_DATA_H
