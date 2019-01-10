#include "include.h"
#include "bsp_spiflash1.h"
#include "spiflash_cyc_fs.h"
#include "bsp_spiflash_rec_play.h"

#if EX_SPIFLASH_SUPPORT


bool exspiflash_init(void)
{
    printf("exspiflash_init\n");
    spiflash1_init();
    if (!is_exspiflash_online()) {
        return false;
    }
#if  SPIFALSH_MUSIC_BIN_WRITE_TEST    //默认的FLASH_MUSIC.BIN写入SPIFLASH, 可以用于测试外接SPIFALSH播放MP3.
    write_music_bin_to_spiflash();
#endif

#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
    cycrec_fs_init(SPIFLASH_REC_BEGIN_ADDR, SPIFLASH_REC_END_ADDR, spiflash1_read, spiflash1_write, spiflash1_erase);
#endif
    return true;
}


#if (EX_SPIFLASH_SUPPORT & EXSPI_REC)
//播放外接spifalsh中的最后一个录音
bool spifalsh_rec_play_last_file(void)
{
    u32 addr, len;
    if (spifalsh_rec_open_last_file(&addr,&len)) {  //打开最后一个录音文件.
        return false;
    }
    if (func_cb.mp3_res_play != NULL) {
        printf("spifalsh_rec_play_last_file : addr = %d, len = %d\n",addr,len);
        register_spi_read_function(spiflash1_read);
        func_cb.mp3_res_play(addr, len);
        register_spi_read_function(NULL);
    }
    return true;
}
#endif  //(EX_SPIFLASH_SUPPORT & EXSPI_REC)

#endif  //EX_SPIFLASH_SUPPORT

































