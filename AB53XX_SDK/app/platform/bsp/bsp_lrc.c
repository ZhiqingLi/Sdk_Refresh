#include "include.h"

#if MUSIC_LRC_EN

u8 lrc_buf[512] AT(.buf.lrc);
char lrc_lfn[100] AT(.lrc_buf);
extern char fname_buf[100];

AT(.text.lrc.init)
void bsp_lrc_init(void)
{
    f_msc.lrc_sta = 0;
    if (!(f_msc.fname[99] & BIT(1))) {
        fs_get_filename(f_msc.fname, sizeof(fname_buf));        //获取长文件名
        f_msc.fname[99] |= BIT(1);
    }
    memcpy(lrc_lfn, f_msc.fname, sizeof(fname_buf));
    if (fs_open_lrc(lrc_lfn) == FR_OK) {
        f_msc.lrc_sta = 1;
        f_msc.lrc_encoding = lrc_init(lrc_buf);
        if (f_msc.lrc_sta == LRC_FNAME_DISP_SECS) {
            f_msc.lrc_update = music_get_lrc(fname_buf, lrc_cur_lable_time());
            if (f_msc.lrc_update) {
        #if GUI_LCD_EN
                if (f_msc.lrc_encoding != LRC_GBK) {
                    utf16_convert_to_gbk(fname_buf, 48);
                }
        #endif // GUI_LCD_EN
            }
        }
        printf("got lrc file: %d, %d\n", f_msc.lrc_sta, f_msc.lrc_encoding);
    }
}

AT(.text.lrc)
void bsp_lrc_get_content(void)
{
    u32 mtime;
    if (f_msc.lrc_sta == LRC_FNAME_DISP_SECS) {
        mtime = music_get_cur_time() + 3;
        if (music_get_lrc(fname_buf, mtime)) {
            f_msc.lrc_update = 1;
    #if GUI_LCD_EN
            if (f_msc.lrc_encoding != LRC_GBK) {
                utf16_convert_to_gbk(fname_buf, 48);
            }
    #endif // GUI_LCD_EN
            printf("[%02d:%02d.%d] %s\n", mtime/600, (mtime%600)/10, mtime%10, fname_buf);
        }
    }
}

AT(.text.lrc)
void bsp_lrc_sta_process(void)
{
    if (f_msc.lrc_sta && (f_msc.lrc_sta != LRC_FNAME_DISP_SECS)) {
        f_msc.lrc_sta++;
        if (f_msc.lrc_sta == LRC_FNAME_DISP_SECS) {
            if (lrc_first_lable_time() <= music_get_cur_time()) {
                f_msc.lrc_update = music_get_lrc(fname_buf, lrc_first_lable_time());
                if (f_msc.lrc_update) {
            #if GUI_LCD_EN
                    if (f_msc.lrc_encoding != LRC_GBK) {
                        utf16_convert_to_gbk(fname_buf, 48);
                    }
            #endif // GUI_LCD_EN
                }
            } else {
                f_msc.lrc_sta = LRC_FNAME_DISP_SECS - 1;
            }
        }
    }
}

AT(.text.lrc)
bool bsp_lrc_is_ready(void)
{
    if (f_msc.lrc_sta == LRC_FNAME_DISP_SECS) {
        return true;
    }
    return false;
}

#endif // MUSIC_LRC_EN
