#ifndef _BSP_FS_H
#define _BSP_FS_H

u8 music_file_filter(void);
u8 music_dir_filter(void);
u8 music_num_file_filter(void);
u8 music_only_record_file_filter(void);                     //只扫描录音文件
u8 music_only_record_dir_filter(void);                      //只扫描录音文件
u8 music_rm_record_dir_filter(void);                        //不扫描录音文件
u8 music_only_name_file_filter(void);                       //只扫描特定文件
void bsp_update_final_rec_file_num(void);

int stream_read(void *buf, unsigned int size);
bool stream_seek(unsigned int ofs, int whence);
void str_tolower(char *str, uint len);

#endif //_BSP_FS_H


