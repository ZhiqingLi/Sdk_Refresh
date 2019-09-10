#ifndef _BSP_FMRX_H
#define _BSP_FMRX_H

#include "fmrx/fmrx_external.h"

enum {
    FMRX_NONE,
    FMRX_INSIDE,
    FMRX_QN8035,
};

//内置收音判台阈值
typedef struct {
    u16 r_val;
    u16 z_val;
    u16 fz_val;
    u16 d_val;
} fmrx_thd_t;
extern fmrx_thd_t fmrx_thd;

void fmrx_tmr1ms_isr(void);
void bsp_fmrx_init(void);
void bsp_fmrx_exit(void);
void bsp_fmrx_get_type(void);
void bsp_fmrx_set_volume(u8 vol);
u8 bsp_fmrx_check_freq(u16 freq);
void bsp_fmrx_set_freq(u16 freq);
void bsp_fmrx_set_loc_dx(u8 loc_flag);
void bsp_fmrx_logger_out(void);
void bsp_fmrx_rssi_print(void);
void bsp_fmrx_rssi_info_clear(void);

#endif // _BSP_FMRX_H
