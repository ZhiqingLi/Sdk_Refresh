#ifndef _BSP_MUSIC_H
#define _BSP_MUSIC_H

enum {
    NORMAL_MODE,
    SINGLE_MODE,
    FLODER_MODE,
    RANDOM_MODE,
};

typedef struct {
    u8 sta;
    u8 nch;
    u8 spr;
    u8 fade_sta;
    u8 buf[512];
} wavres_cb_t;

bool pf_scan_music(u8 new_dev);
void music_playmode_next(void);
void mp3_res_play(u32 addr, u32 len);
void wav_res_play(u32 addr, u32 len,wavres_cb_t *wavres_cb);

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_init(void);
void bsp_music_breakpoint_save(void);
void bsp_music_breakpoint_clr(void);

#define music_breakpoint_init()   bsp_music_breakpoint_init()
#define music_breakpoint_save()   bsp_music_breakpoint_save()
#define music_breakpoint_clr()    bsp_music_breakpoint_clr()
#else
#define music_breakpoint_init()
#define music_breakpoint_save()
#define music_breakpoint_clr()
#endif // MUSIC_BREAKPOINT_EN

#endif //_BSP_MUSIC_H
