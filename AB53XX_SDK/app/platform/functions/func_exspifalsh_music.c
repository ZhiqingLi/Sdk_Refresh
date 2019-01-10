#include "include.h"
#include "func.h"
#include "func_music.h"
#include "func_exspifalsh_music.h"

#if (EX_SPIFLASH_SUPPORT)
exspifalsh_msc_t exspi_msc AT(.buf.func.music);

AT(.text.func.music)
void func_exspifalsh_music_process(void)
{
    func_process();
    if (get_music_dec_sta() == MUSIC_STOP)  {
#if SPIFALSH_MUSIC_PLAY_REC
    if (!exspi_msc.rec_no_file) {
        exspifalsh_rec_switch_file(1);
    }
#else
    exspifalsh_music_switch_file(1);
#endif
    }
}

AT(.text.func.music)
void func_exspifalsh_music_display(void)
{

}

AT(.text.func.music)
u32 exspifalsh_music_get_file_total(void)
{
    u8 tagbuf[10];
    u32 offset = 0, total_num;
    memset(tagbuf,0x00, sizeof(tagbuf));
    spiflash1_read(tagbuf,  0x00,8);
    printf("tag = %s\n",tagbuf);
    if ((tagbuf[0] != 'B') || (tagbuf[1] != 'l')|| (tagbuf[2] != 'u')|| (tagbuf[3] != 'e')
        || (tagbuf[4] != 'T')|| (tagbuf[5] != 'r')|| (tagbuf[6] != 'u')|| (tagbuf[7] != 'm')){
        printf("err Tag ERR\n");
        return 0;
    }
    spiflash1_read(&total_num,  0x10 + offset,4);
    printf("total num = %d\n",total_num);
    return total_num;
}

AT(.text.func.music)
void exspifalsh_music_num_kick(u32 num)
{
    u32 addr, len;
    if (num > 0) {
        num--;
    }
    spiflash1_read(&addr, 0x20 + (num*8),4);
    spiflash1_read(&len, 0x24 + (num*8),4);
    printf("num = %d, addr = 0x%X, len = 0x%X\n",num+1, addr, len);
    mp3_res_play_kick(addr, len);
}

AT(.text.func.music)
void exspifalsh_music_switch_file(u8 direction)   //1 next , 0 prev
{
    music_control(MUSIC_MSG_STOP);
    if (direction) {
        if (++exspi_msc.cur_num > exspi_msc.total_num) {
            exspi_msc.cur_num = 1;
        }
    } else {
        if (exspi_msc.cur_num >= 1) {
            exspi_msc.cur_num--;
        }
        if (exspi_msc.cur_num == 0) {
            exspi_msc.cur_num = exspi_msc.total_num;
        }
    }
    printf("music_num = %d\n",exspi_msc.cur_num);
    exspifalsh_music_num_kick(exspi_msc.cur_num);
}



AT(.text.func.music)
void exspifalsh_rec_switch_file(u8 direction)   //1 next , 0 prev
{
    u32 addr = 0, len = 0;
    u8 ret = 1;
    printf("rec next file, direction = %d\n",direction);
    music_control(MUSIC_MSG_STOP);
    if (direction) {
        ret = spifalsh_rec_open_next_file(&addr, &len);
    } else {
        ret = spifalsh_rec_open_prev_file(&addr, &len);
    }
    exspi_msc.rec_no_file = ret;
    printf("open file ret = %d, addr = 0x%X, len = %d\n",ret, addr, len);
    if (!ret) {
         mp3_res_play_kick(addr, len);
    }
}



AT(.text.func.music)
bool exspifalsh_music_image_check(void)
{
    if (is_exspiflash_online()) {
        exspi_msc.total_num = exspifalsh_music_get_file_total();
        if (exspi_msc.total_num > 0) {
            return true;
        }
    }
    return false;
}

AT(.text.func.music)
void func_exspifalsh_mp3_res_play(u32 addr, u32 len)
{
    msc_breakpiont_t brkpt;
    music_get_breakpiont(&brkpt);           //保存当前播放位置
    u32 cur_time = music_get_cur_time();
    music_control(MUSIC_MSG_STOP);
    register_spi_read_function(NULL);  //恢复内部SPI读接口.
    mp3_res_play(addr, len);
    register_spi_read_function(spiflash1_read);
    music_set_cur_time(cur_time);
    exspifalsh_music_num_kick(exspi_msc.cur_num);
    music_set_jump(&brkpt);                 //恢复播放位置
}

AT(.text.func.music)
static void func_exspifalsh_music_enter(void)
{
    msg_queue_clear();
#if SPIFALSH_MUSIC_PLAY_REC
    u32 addr, len;
    u32 rec_total = spifalsh_rec_get_total_file();
    printf("func_exspifalsh_music_enter, rec_total = %d\n",rec_total);
    func_cb.mp3_res_play = func_exspifalsh_mp3_res_play;
    register_spi_read_function(spiflash1_read);
    if ((!rec_total) ||spifalsh_rec_open_last_file(&addr,&len)) {  //打开最后一个录音文件.
        func_cb.sta = FUNC_NULL;
        return;
    }
    bsp_change_volume(sys_cb.vol);
    exspi_msc.pause = false;
    mp3_res_play_kick(addr, len);
#else
    if ((!exspifalsh_music_image_check())){
        func_cb.sta = FUNC_NULL;
        return;
    }
    func_cb.mp3_res_play = func_exspifalsh_mp3_res_play;
    register_spi_read_function(spiflash1_read);
    bsp_change_volume(sys_cb.vol);
    exspi_msc.cur_num = 0;            //默认从0首开始播放
    exspi_msc.pause = false;
    exspifalsh_music_num_kick(exspi_msc.cur_num);
#endif
}

AT(.text.func.music)
static void func_exspifalsh_music_exit(void)
{
    music_control(MUSIC_MSG_STOP);
    register_spi_read_function(NULL);  //恢复内部SPI读接口.
    func_cb.last = FUNC_MUSIC;
}


AT(.text.func.music)
void func_exspifalsh_music(void)
{
    printf("%s\n", __func__);
    func_exspifalsh_music_enter();
    while (func_cb.sta == FUNC_EXSPIFLASH_MUSIC) {
        func_exspifalsh_music_process();
        func_exspifalsh_music_message(msg_dequeue());
        func_exspifalsh_music_display();
    }
    func_exspifalsh_music_exit();
}
#endif

