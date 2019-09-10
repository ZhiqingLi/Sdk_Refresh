#include "include.h"
#include "bsp_fmtx.h"

#if FMTX_EN

typedef struct {
    u8 io_num;
    u8 res[1];
    u16 dac_en   : 1;
    u16 power    : 1;
    u16 fix_bt   : 1;
} fmtx_cfg_t;

fmtx_cb_t fmtx_cb;

fmtx_cfg_t fmtx_cfg = {
    .io_num     = IO_PA6,
    .res[0]     = 0,
    .dac_en     = FMTX_DAC_EN,
    .power      = FMTX_POWER,
    .fix_bt     = FMTX_FIX_BT,
};

static const u16 fmtx_dig_vol_30[30 + 1] = {
    FMTX_DIG_N60DB,
    FMTX_DIG_N58DB, FMTX_DIG_N56DB, FMTX_DIG_N54DB, FMTX_DIG_N52DB, FMTX_DIG_N50DB,
    FMTX_DIG_N48DB, FMTX_DIG_N46DB, FMTX_DIG_N44DB, FMTX_DIG_N42DB, FMTX_DIG_N40DB,
    FMTX_DIG_N38DB, FMTX_DIG_N36DB, FMTX_DIG_N34DB, FMTX_DIG_N32DB, FMTX_DIG_N30DB,
    FMTX_DIG_N28DB, FMTX_DIG_N26DB, FMTX_DIG_N24DB, FMTX_DIG_N22DB, FMTX_DIG_N20DB,
    FMTX_DIG_N18DB, FMTX_DIG_N16DB, FMTX_DIG_N14DB, FMTX_DIG_N12DB, FMTX_DIG_N10DB,
    FMTX_DIG_N8DB,  FMTX_DIG_N6DB,  FMTX_DIG_N4DB,  FMTX_DIG_N2DB,  FMTX_DIG_N0DB,
};

static const u16 fmtx_dig_vol_16[16 + 1] = {
    FMTX_DIG_N60DB,
    FMTX_DIG_N43DB,  FMTX_DIG_N32DB,  FMTX_DIG_N26DB,  FMTX_DIG_N24DB,
    FMTX_DIG_N22DB,  FMTX_DIG_N20DB,  FMTX_DIG_N18DB,  FMTX_DIG_N16DB,
    FMTX_DIG_N14DB,  FMTX_DIG_N12DB,  FMTX_DIG_N10DB,  FMTX_DIG_N8DB,
    FMTX_DIG_N6DB,   FMTX_DIG_N4DB,   FMTX_DIG_N2DB,   FMTX_DIG_N0DB,
};

AT(.text.bsp.fmtx)
u16 bsp_fmtx_freq_inc(u16 freq)
{
    freq += 10;
    if (freq > FM_FREQ_MAX) {
        freq = FM_FREQ_MIN;
    }
    return freq;
}

AT(.text.bsp.fmtx)
u16 bsp_fmtx_freq_dec(u16 freq)
{
    freq -= 10;
    if (freq < FM_FREQ_MIN) {
        freq = FM_FREQ_MAX;
    }
    return freq;
}

AT(.text.bsp.fmtx)
void bsp_fmtx_set_vol(u8 vol)
{
    fmtx_set_vol(fmtx_cb.vol_table[vol]);
}

AT(.text.bsp.fmtx)
void bsp_fmtx_set_freq(u16 freq)
{
    if (fmtx_cb.freq == freq) {
        return;
    }
    printf("fmtx freq:%d\n", freq);
    fmtx_cb.freq = freq;
    fmtx_set_freq(fmtx_cb.freq);
    param_fmtx_freq_write();
}

AT(.text.bsp.fmtx)
void bsp_fmtx_init(void)
{
    memset(&fmtx_cb, 0, sizeof(fmtx_cb_t));
    if (!fmtx_init(&fmtx_cfg)) {
        printf("fmtx init fail\n");
        return;
    }

    if (VOL_MAX == 30) {
        fmtx_cb.vol_table = fmtx_dig_vol_30;
    } else if (VOL_MAX == 16) {
        fmtx_cb.vol_table = fmtx_dig_vol_16;
    } else {
        printf("fmtx vol err\n");
    }

    param_fmtx_freq_read();
    fmtx_cb.freq = 10390;
    if (fmtx_cb.freq == 0xffff || fmtx_cb.freq == 0) {
        fmtx_cb.freq = FM_FREQ_MIN;
    }
    fmtx_set_freq(fmtx_cb.freq);
    param_fmtx_freq_write();
}
#endif
