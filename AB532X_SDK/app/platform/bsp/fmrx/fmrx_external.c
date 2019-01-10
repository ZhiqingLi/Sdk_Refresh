#include "include.h"
#include "fmrx_external.h"

//外接收音芯片的相关函数
#if !FMRX_INSIDE_EN

#if FMRX_REC_EN
AT(.text.fmrx)
void fmrx_rec_start_ext(void)
{
    fmrx_cb.rec_en = 1;
}

AT(.text.fmrx)
void fmrx_rec_stop_ext(void)
{
    fmrx_cb.rec_en = 0;
}

AT(.text.fmrx)
void fmrx_rec_init_ext(void)
{
    rec_src.spr = SPR_44100;
    rec_src.source_start = fmrx_rec_start_ext;
    rec_src.source_stop  = fmrx_rec_stop_ext;
    fmrx_cb.rec_en = 0;
    rec_src.nchannel = 0x01;
}
#endif // FMRX_REC_EN

AT(.com_text.func.fmrx)
void fmrx_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if FMRX_REC_EN
    if (fmrx_cb.rec_en) {
        puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
    }
#endif // FMRX_REC_EN
    sdadc_pcm_2_dac(ptr, samples, ch_mode);
}

#endif // FMRX_INSIDE_EN
