#ifndef _BSP_KARAOK_H
#define _BSP_KARAOK_H

typedef struct {
    const u16 (*echo_level_gain)[2];
    const u16 *echo_delay;
    const u16 *dvol_table;
    const u16 *micvol_table;
} karaok_vol_t;
extern karaok_vol_t karaok_vol;

typedef struct {
    u8 res[2];
    s16 *buf_1;
    s16 *buf_2;
    s16 *buf_3;
    s16 *buf_4;
    s16 *buf_5;
    s16 *buf_6;
    char *buf_7;
} mav_cfg_t;

typedef struct {
    const s32 (* lp_coef)[2][8];    //滤波器参数组
    s16 *echo_cache;                //混响缓存
    mav_cfg_t *mav;                 //魔音配置
    u8 res[2];
    u16 echo_en      :1;             //ECHO使能
    u16 mic_dnr      :1;             //MIC降噪使能
    u16 rec_en       :1;             //karaok录音使能
    u16 magic_voice  :1;             //魔音功能使能
    u16 rec_no_echo_en  :1;          //录音是否不录混响
    u16 howling_en   :1;             //是否打开防啸叫功能
} kara_cfg_t;
extern kara_cfg_t kara_cfg;

void bsp_karaok_init(u8 path, u8 func);
void bsp_karaok_exit(u8 path);
void bsp_karaok_set_mic_volume(void);
void bsp_karaok_set_music_volume(void);
void bsp_karaok_echo_reset_buf(u8 func);
void bsp_echo_set_level(void);
void bsp_echo_set_delay(void);

void karaok_set_mic_volume(u16 vol);

void bsp_karaok_rec_init(void);
void bsp_karaok_rec_exit(void);
#endif
