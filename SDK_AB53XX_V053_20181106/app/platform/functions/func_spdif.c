#include "include.h"
#include "func.h"
#include "func_spdif.h"
#if FUNC_SPDIF_EN

spd_cfg_t spd_cfg;

#if SPF_REC_EN
void spf_rec_start(void);
void spf_rec_stop(void);

AT(.text.bsp.spf)
void spf_rec_init(void)
{
    rec_src.spr = SPR_44100;
    rec_src.source_start = spf_rec_start;
    rec_src.source_stop  = spf_rec_stop;
#if KARAOK_REC_EN
    rec_src.nchannel = 0x22;    //karaok的话录mp2
#else
    rec_src.nchannel = 2;
#endif
}
#endif

static void func_spdif_process(void)
{
    func_process();
    spdif_smp_detect();
    if (!spdif_is_online()) {
        func_cb.sta = FUNC_NULL;
    }
}

static void func_spdif_enter(void)
{
    memset(&spd_cfg, 0, sizeof(spd_cfg_t));
    spd_cfg.channel = SPDIF_CH;

    spdif_init(&spd_cfg);

    if (!spdif_is_online()) {
        func_cb.sta = FUNC_NULL;
        return;
    }
    func_spdif_enter_display();

#if WARNING_FUNC_SPDIF
    spdif_exit();
    mp3_res_play(RES_BUF_SPDIF_MODE_MP3, RES_LEN_SPDIF_MODE_MP3);
    spdif_init(&spd_cfg);
#endif // WARNING_FUNC_SPDIF

    spdif_start();

#if SPF_REC_EN
    spf_rec_init();
#endif
}

static void func_spdif_exit(void)
{
    dac_fade_out();
    func_spdif_exit_display();
    spdif_exit();
    func_cb.last = FUNC_SPDIF;
}

AT(.text.func.spdif)
void func_spdif(void)
{
    printf("%s\n", __func__);

    func_spdif_enter();

    while (func_cb.sta == FUNC_SPDIF) {
        func_spdif_process();
        func_spdif_message(msg_dequeue());
        func_spdif_display();
    }

    func_spdif_exit();
}

#endif
