#ifndef _BSP_LRC_H
#define _BSP_LRC_H

#define LRC_FNAME_DISP_SECS                3        //文件名保持显示n秒后再显示歌词

///Character encoding
enum {
    LRC_GBK,
    LRC_UTF16,
    LRC_UTF8,
};

///libs lrc api functions
u8 lrc_init(u8 *ibuf);
u32 lrc_next_lable_time(void);
u32 lrc_cur_lable_time(void);
u32 lrc_first_lable_time(void);
u8 music_get_lrc(char *fname, u32 mtime);

void bsp_lrc_init(void);
void bsp_lrc_get_content(void);
void bsp_lrc_sta_process(void);

#if MUSIC_LRC_EN
bool bsp_lrc_is_ready(void);
#else
#define bsp_lrc_is_ready()              (0)
#endif // MUSIC_LRC_EN

#endif // _BSP_LRC_H
