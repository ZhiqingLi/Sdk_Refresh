#ifndef _API_SPIFLASH_EX_H_
#define _API_SPIFLASH_EX_H_
//及镜像文件播放相关函数
u32 exspifalsh_music_get_file_total(void);
void exspifalsh_music_num_kick(u32 num);
//外接SPIFLASH录音相关函数
void cycrec_fs_init(u32 begin_addr, u32 end_addr, void(*read)(void *buf, u32 addr, u32 len),void(*write)(void *buf, u32 addr, u32 len),void (*erase)(u32 addr) );
u8 spifalsh_rec_open_last_file(u32 *addr, u32 *len);
u8 spifalsh_rec_open_prev_file(u32 *addr, u32 *len);
u8 spifalsh_rec_open_next_file(u32 *addr, u32 *len);
void spifalsh_rec_delete_curplay_file(void);
u32 spifalsh_rec_get_total_file(void);
u8 spiflash_rec_creat_file(void);
u8 spiflash_rec_write_file(void *buf, u32 len);
u8 spiflash_rec_close_file(void);

#endif
