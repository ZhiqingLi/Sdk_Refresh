#ifndef FUNC_EXSPIFALSH_MUSIC_H
#define FUNC_EXSPIFALSH_MUSIC_H

#include "bsp_spiflash1.h"
#include "spiflash_cyc_fs.h"

typedef struct {
    u16 total_num;
    u16 cur_num;
    u8  pause;
    u8  rec_no_file;
} exspifalsh_msc_t;
extern exspifalsh_msc_t exspi_msc;


void func_exspiflash_music_message(u16 msg);
void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));
void func_exspifalsh_music_message(u16 msg);
void exspifalsh_music_switch_file(u8 direction);
void exspifalsh_rec_switch_file(u8 direction);
extern void mp3_res_play_kick(u32 addr, u32 len);



#endif


