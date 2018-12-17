#ifndef _BSP_CAR_AUDIO_H
#define _BSP_CAR_AUDIO_H

typedef struct {
    u8 f_vol;           //前置左右声道音量控制
    u8 b_vol;           //后置左右声道音量控制
    u8 l_vol;           //左音量控制
    u8 r_vol;           //右音量控制

    u8 fad_level;
    u8 bal_level;
} aucar_cb_t;
extern aucar_cb_t aucar_cb;

void bsp_aucar_set_fad_vol(u8 f_vol, u8 b_vol);
void bsp_aucar_set_bal_vol(u8 l_vol, u8 r_vol);
void bsp_aucar_fad_inc(void);
void bsp_aucar_fad_dec(void);
void bsp_aucar_bal_inc(void);
void bsp_aucar_bal_dec(void);
void bsp_aucar_init(void);

#endif // _BSP_CAR_AUDIO_H
