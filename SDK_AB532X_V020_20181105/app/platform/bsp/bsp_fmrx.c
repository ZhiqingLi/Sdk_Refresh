#include "include.h"

#if FUNC_FMRX_EN
AT(.com_text.fmrx.isr)
void fmrx_tmr1ms_isr(void)
{
#if FMRX_INSIDE_EN
    if (func_cb.sta == FUNC_FMRX) {
        fmrx_dac_sync();
    }
#endif // FMRX_INSIDE_EN
}

AT(.text.bsp.fmrx)
bool fmrx_is_playing(void)
{
    if (fmrx_cb.sta == FMRX_PLAY) {
        return true;
    }
    return false;
}

AT(.text.bsp.fmrx)
u8 get_fmrx_seek_staus(void)
{
    return fmrx_cb.seek_start;
}

AT(.text.bsp.fmrx)
void set_fmrx_seek_staus(u8 val)
{
    fmrx_cb.seek_start = val;
}

#if FMRX_INSIDE_EN
AT(.text.bsp.fmrx)
void fmrx_threshold_init(void)
{
    fmrx_thd.r_val = FMRX_THRESHOLD_VAL;
    fmrx_thd.z_val = FMRX_THRESHOLD_Z;
    fmrx_thd.fz_val = FMRX_THRESHOLD_FZ;
    fmrx_thd.d_val = FMRX_THRESHOLD_D;
}

AT(.text.bsp.fmrx)
void fmrx_inside_init(void)
{
    fmrx_power_on(FMRX_THRESHOLD_VAL);
    fmrx_set_rf_cap(FMRX_RFCAP_VAL);         //FMFE input capacitor
#if FMRX_REC_EN
    rec_src.spr = SPR_44100;
    if (DAC_OUT_SPR == DAC_OUT_48K) {
        rec_src.spr = SPR_48000;
    }
    rec_src.nchannel = 0x02;
    rec_src.source_start = fmrx_rec_start;
    rec_src.source_stop  = fmrx_rec_stop;
#endif
}
#endif // FMRX_INSIDE_EN

//设置收音LOC/DX, 本地电台/远程电台
AT(.text.bsp.fmrx)
void bsp_fmrx_set_loc_dx(u8 loc_flag)
{
#if FMRX_INSIDE_EN
    if (loc_flag) {
        fmrx_thd.r_val = 896;
        fmrx_thd.z_val = 100;
        fmrx_thd.fz_val = 80;
        fmrx_thd.d_val = 1000;
    } else {
        fmrx_threshold_init();
    }
#endif // FMRX_INSIDE_EN
}

AT(.text.bsp.fmrx)
void bsp_fmrx_get_type(void)
{
    u8 fmrx_type = FMRX_NONE;

#if FMRX_INSIDE_EN
    fmrx_type = FMRX_INSIDE;
#endif // FMRX_INSIDE_EN

#if FMRX_QN8035_EN
    if (fmrx_type == FMRX_NONE) {
        if (qn8035_is_online()) {
            fmrx_type = FMRX_QN8035;
        }
    }
#endif // FMRX_QN8035_EN

    sys_cb.fmrx_type = fmrx_type;
}

AT(.text.bsp.fmrx)
void bsp_fmrx_init(void)
{
    printf("%s\n", __func__);

#if FMRX_INSIDE_EN
    if (sys_cb.fmrx_type == FMRX_INSIDE) {
        fmrx_inside_init();
        return;
    }
#endif

#if FMRX_QN8035_EN
    if (sys_cb.fmrx_type == FMRX_QN8035) {
        qn8035_init();
    }
#endif

//外置收音录音配置
#if (FMRX_REC_EN && (!FMRX_INSIDE_EN))
    if (sys_cb.fmrx_type != FMRX_NONE) {
        fmrx_rec_init_ext();
    }
#endif
}

AT(.text.bsp.fmrx)
void bsp_fmrx_exit(void)
{
#if FMRX_INSIDE_EN
    if (sys_cb.fmrx_type == FMRX_INSIDE) {
        fmrx_power_off();
    }
#endif

#if FMRX_QN8035_EN
    if (sys_cb.fmrx_type == FMRX_QN8035) {
        qn8035_off();
    }
#endif
}

AT(.text.bsp.fmrx)
void bsp_fmrx_set_volume(u8 vol)
{
#if FMRX_QN8035_EN
    if (sys_cb.fmrx_type == FMRX_QN8035) {
        qn8035_set_vol(vol);
    }
#endif
}

AT(.text.bsp.fmrx)
void bsp_fmrx_set_freq(u16 freq)
{
#if FMRX_INSIDE_EN
    if (sys_cb.fmrx_type == FMRX_INSIDE) {
        fmrx_set_freq(freq);
    }
#endif

#if FMRX_QN8035_EN
    if (sys_cb.fmrx_type == FMRX_QN8035) {
        qnd8035_set_freq(freq);
    }
#endif
}


AT(.text.bsp.fmrx)
u8 bsp_fmrx_check_freq(u16 freq)
{
    u8 ret = 0;
#if FMRX_INSIDE_EN
    if (sys_cb.fmrx_type == FMRX_INSIDE) {
        ret = fmrx_check_freq(freq);
    }
#endif

#if FMRX_QN8035_EN
    if (sys_cb.fmrx_type == FMRX_QN8035) {
        ret = qn8035_seek(freq);
    }
#endif

    return ret;
}

//AT(.text.bsp.fmrx)
//void bsp_fmrx_logger_out(void)
//{
//    fmrx_logger_out();
//    printf("Seek out radio stations(%d):\n", fmrx_cb.ch_cnt);
//    for (u8 j = 1; j <= fmrx_cb.ch_cnt; j++) {
//        printf("%d, %d\n", get_ch_freq(j), j);
//    }
//}

#endif // FUNC_FMRX_EN

