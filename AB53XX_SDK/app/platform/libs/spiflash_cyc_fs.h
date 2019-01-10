#ifndef _SPIFLASH_CYC_FS_H_
#define _SPIFLASH_CYC_FS_H_

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
