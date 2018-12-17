#include "enhanced.h"
#include "eh_globle_data.h"
#include "dir_list.h"

//文件按照什么顺序播放(按照原始的磁盘顺序播放，
//  还是按节目单(存在vram里面)的顺序播放)
uint8 eh_fsel_type = FSEL_TYPE_COMMONDIR;

fsel_param_t fsel_init_val; //模块参数

fsel_error_e fsel_error; //错误类型

uint8 eh_dir_layer; //当前目录或列表的层次，0表示根目录，1表示根目录的子目录，以此类推
uint8 eh_shuffle_sw; //SHUFFLE 开关

uint16 eh_file_no; //当前文件在目录或列表中的序号
uint16 eh_file_total; //当前目录的总文件个数

uint16 eh_file_no_all; //磁盘顺序模式下，表示磁盘序号；在节目单模式下，表示第几个节目
uint16 eh_file_total_all; //当前磁盘总文件个数

uint32 eh_cur_ext_name; //当前选择歌曲的扩展名
//uint16 eh_root_dir; //根目录的子目录指针，1,2，。。。

int32 eh_vfs_mount = -1; //vFS 识别号

uint16 eh_dir_no = 1; //目录序号(目录列表中的第几个)

//存放目录路径信息
pdir_layer_t pdir_layer_buffer;
//存放文件位置信息
pfile_offset_t pfile_offset_buffer;

//for total all save use
//存放目录路径信息
pdir_layer_t pdir_layer_total;
//存放文件位置信息
pfile_offset_t pfile_offset_total;

//for shuffle
uint16 shuffle_total;//shuffle的总个数
uint8 group_map[SET_GROUP_MAX / 8];//分组，共64组(bit为1-未用,0-已用)
uint8 member_map[SET_MEMBER_MAX / 8];//组内成员，共256(bit为1-未用,0-已用)
uint8 member_avg;//成员平均值，商值
uint8 member_avg_group;//平均值成员的组号起始
uint8 group_max;//有效的组号
uint8 member_max;//当前组的最大成员个数
uint8 use_member_flag;//使用组内shuffle功能
uint8 cur_group;//当前的组号（1，2，。。。
uint16 cur_member;//当前组内的成员号（1，2，。。。
uint16 filtrate_shuffle_no; //shuffle过滤播放序号，避免重复；取值为0,1~n

uint16 play_num_list[SET_PREV_MAX];
uint8 cur_save_pt; //当前存的指针(0~)
uint8 cur_save_prev_count;
uint8 cur_save_next_count;
uint8 cur_overturn_flag; //表翻转标记,1-yes,0-no
//end for shuffle

//for dir list

//列表存储位置
uint8 *plist_start_addr = (uint8*) PLIST_INFO_ADDR;

//目录列表项信息
list_dir_info_t list_dir_info;
//指针
list_dir_info_t* plist_info;
//目录列表表头信息
list_head_info_t list_head_info;
//指针
list_head_info_t* plist_head;
//end for list

//record long name
uint8 name_buf[16];

//该文件的磁盘中的序号(即按照FAT,它是第几个播放的文件)
uint16 eh_disk_file_no = 0;

