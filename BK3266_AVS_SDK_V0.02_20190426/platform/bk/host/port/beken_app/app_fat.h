#ifndef __DRIVER_FAT_H
#define DRIVER_FAT_H


#if 0
#define     FAT_TABLE_FILE_END      0xFFFFFFF

int fat_init( void );
int fat_get_next_file( fat_file_info_t *file_info, char *ext, int index );
int fat_get_prev_file( fat_file_info_t *file_info, char *ext, int index ); //need test, may take high mcu cost
uint32 fat_get_file_next_cluster( uint32 cluster );
uint32 fat_blk_from_cluster( uint32 cluster );
uint32 fat_cluster_from_blk( uint32 blk );
int fat_get_prev_subdirs( fat_file_info_t *file_info, char *ext, int index );
int fat_get_next_subdirs( fat_file_info_t *file_info, char *ext, int index );
int fat_change_cur_dir( uint32 cluster );
int fat_change_dir_to_parent( void );
uint32 fat_get_root_cluster( void );

uint32 fat_get_blocks_per_cluster( void );

//add by zjw 
uint8 fat_get_ft_file_flag( void );
void fat_get_fat_info_print( void );
void fat_exact_info_from_index_print( uint32 index );
int fat_change_to_root_dir( void );
int fat_set_int_flag_for_first_search( int value );
int fat_check_file_exist( fat_file_info_t *file_info, char *ext, int index );
#endif
#endif
