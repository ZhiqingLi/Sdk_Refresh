#include "include.h"
#include "bsp_spiflash1.h"
#include "spiflash_cyc_fs.h"
#include "bsp_spiflash_rec_play.h"

#if REC_TO_SPIFALSH

extern void register_spi_read_function(void (* read_func)(void *buf, u32 addr, u32 len));

#define SPIFLASH_ID                 0x40170000
#define SPIFLASH_REC_BEGIN_ADDR     0
#define SPIFLASH_REC_END_ADDR       (1024*100)   //100K


bool is_spiflash_online(void)
{
    u32 flash_id;
    u32 i = 5;
    while(i--) {
        flash_id = spiflash1_id_read();
        printf("flash id = 0x%X\n",flash_id);
        if (SPIFLASH_ID == flash_id) {
            return true;
        }
    }
    return false;
}


bool spiflash_rec_fs_mount(void)
{
    printf("spiflash_rec_fs_mount\n");
    spiflash1_init();

    if (!is_spiflash_online()) {
        return false;
    }
    cycrec_fs_init(SPIFLASH_REC_BEGIN_ADDR, SPIFLASH_REC_END_ADDR, spiflash1_read, spiflash1_write, spiflash1_erase);
    return true;
}

//播放外接spifalsh中的最后一个录音
bool spifalsh_rec_play_last_file(void)
{
    u32 addr, len;
    if (spifalsh_rec_open_last_file(&addr,&len)) {  //打开最后一个录音文件.
        return false;
    }
    if (func_cb.mp3_res_play != NULL) {
        printf("spifalsh_rec_play_last_file : addr = %d, len = %d,0x%X\n",addr,len,func_cb.mp3_res_play);
        register_spi_read_function(spiflash1_read);
        func_cb.mp3_res_play(addr, len);
        register_spi_read_function(NULL);
    }
    return true;
}

#endif  //REC_TO_SPIFALSH

































