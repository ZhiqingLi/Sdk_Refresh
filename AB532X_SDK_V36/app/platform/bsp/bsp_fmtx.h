#ifndef _BSP_FMTX_H
#define _BSP_FMTX_H

//FM数字音量
#define MAX_FMTX_DIG_VAL         0xffff
#define FMTX_DIG_N0DB           (MAX_FMTX_DIG_VAL / 1.000000)
#define FMTX_DIG_N1DB           (MAX_FMTX_DIG_VAL / 1.122018)
#define FMTX_DIG_N2DB           (MAX_FMTX_DIG_VAL / 1.258925)
#define FMTX_DIG_N3DB           (MAX_FMTX_DIG_VAL / 1.412538)
#define FMTX_DIG_N4DB           (MAX_FMTX_DIG_VAL / 1.584893)
#define FMTX_DIG_N5DB           (MAX_FMTX_DIG_VAL / 1.778279)
#define FMTX_DIG_N6DB           (MAX_FMTX_DIG_VAL / 1.995262)
#define FMTX_DIG_N7DB           (MAX_FMTX_DIG_VAL / 2.238721)
#define FMTX_DIG_N8DB           (MAX_FMTX_DIG_VAL / 2.511886)
#define FMTX_DIG_N9DB           (MAX_FMTX_DIG_VAL / 2.818383)
#define FMTX_DIG_N10DB           (MAX_FMTX_DIG_VAL / 3.162278)
#define FMTX_DIG_N11DB           (MAX_FMTX_DIG_VAL / 3.548134)
#define FMTX_DIG_N12DB           (MAX_FMTX_DIG_VAL / 3.981072)
#define FMTX_DIG_N13DB           (MAX_FMTX_DIG_VAL / 4.466836)
#define FMTX_DIG_N14DB           (MAX_FMTX_DIG_VAL / 5.011872)
#define FMTX_DIG_N15DB           (MAX_FMTX_DIG_VAL / 5.623413)
#define FMTX_DIG_N16DB           (MAX_FMTX_DIG_VAL / 6.309573)
#define FMTX_DIG_N17DB           (MAX_FMTX_DIG_VAL / 7.079458)
#define FMTX_DIG_N18DB           (MAX_FMTX_DIG_VAL / 7.943282)
#define FMTX_DIG_N19DB           (MAX_FMTX_DIG_VAL / 8.912509)
#define FMTX_DIG_N20DB           (MAX_FMTX_DIG_VAL / 10.000000)
#define FMTX_DIG_N21DB           (MAX_FMTX_DIG_VAL / 11.220185)
#define FMTX_DIG_N22DB           (MAX_FMTX_DIG_VAL / 12.589254)
#define FMTX_DIG_N23DB           (MAX_FMTX_DIG_VAL / 14.125375)
#define FMTX_DIG_N24DB           (MAX_FMTX_DIG_VAL / 15.848932)
#define FMTX_DIG_N25DB           (MAX_FMTX_DIG_VAL / 17.782794)
#define FMTX_DIG_N26DB           (MAX_FMTX_DIG_VAL / 19.952623)
#define FMTX_DIG_N27DB           (MAX_FMTX_DIG_VAL / 22.387211)
#define FMTX_DIG_N28DB           (MAX_FMTX_DIG_VAL / 25.118864)
#define FMTX_DIG_N29DB           (MAX_FMTX_DIG_VAL / 28.183829)
#define FMTX_DIG_N30DB           (MAX_FMTX_DIG_VAL / 31.622777)
#define FMTX_DIG_N31DB           (MAX_FMTX_DIG_VAL / 35.481339)
#define FMTX_DIG_N32DB           (MAX_FMTX_DIG_VAL / 39.810717)
#define FMTX_DIG_N33DB           (MAX_FMTX_DIG_VAL / 44.668359)
#define FMTX_DIG_N34DB           (MAX_FMTX_DIG_VAL / 50.118723)
#define FMTX_DIG_N35DB           (MAX_FMTX_DIG_VAL / 56.234133)
#define FMTX_DIG_N36DB           (MAX_FMTX_DIG_VAL / 63.095734)
#define FMTX_DIG_N37DB           (MAX_FMTX_DIG_VAL / 70.794578)
#define FMTX_DIG_N38DB           (MAX_FMTX_DIG_VAL / 79.432823)
#define FMTX_DIG_N39DB           (MAX_FMTX_DIG_VAL / 89.125094)
#define FMTX_DIG_N40DB           (MAX_FMTX_DIG_VAL / 100.000000)
#define FMTX_DIG_N41DB           (MAX_FMTX_DIG_VAL / 112.201845)
#define FMTX_DIG_N42DB           (MAX_FMTX_DIG_VAL / 125.892541)
#define FMTX_DIG_N43DB           (MAX_FMTX_DIG_VAL / 141.253754)
#define FMTX_DIG_N44DB           (MAX_FMTX_DIG_VAL / 158.489319)
#define FMTX_DIG_N45DB           (MAX_FMTX_DIG_VAL / 177.827941)
#define FMTX_DIG_N46DB           (MAX_FMTX_DIG_VAL / 199.526231)
#define FMTX_DIG_N47DB           (MAX_FMTX_DIG_VAL / 223.872114)
#define FMTX_DIG_N48DB           (MAX_FMTX_DIG_VAL / 251.188643)
#define FMTX_DIG_N49DB           (MAX_FMTX_DIG_VAL / 281.838293)
#define FMTX_DIG_N50DB           (MAX_FMTX_DIG_VAL / 316.227766)
#define FMTX_DIG_N51DB           (MAX_FMTX_DIG_VAL / 354.813389)
#define FMTX_DIG_N52DB           (MAX_FMTX_DIG_VAL / 398.107171)
#define FMTX_DIG_N53DB           (MAX_FMTX_DIG_VAL / 446.683592)
#define FMTX_DIG_N54DB           (MAX_FMTX_DIG_VAL / 501.187234)
#define FMTX_DIG_N55DB           (MAX_FMTX_DIG_VAL / 562.341325)
#define FMTX_DIG_N56DB           (MAX_FMTX_DIG_VAL / 630.957344)
#define FMTX_DIG_N57DB           (MAX_FMTX_DIG_VAL / 707.945784)
#define FMTX_DIG_N58DB           (MAX_FMTX_DIG_VAL / 794.328235)
#define FMTX_DIG_N59DB           (MAX_FMTX_DIG_VAL / 891.250938)
#define FMTX_DIG_N60DB           0

#define FMTX_DISPLAY_TIMES       5
#define FMTX_FLICKER_TIMES       2
enum{
    FMTX_IDEL,
    FMTX_SET_FREQ,
    FMTX_DONE,
};
typedef struct {
    u8 set_freq;
    u16 freq;
    u32 freq_tick;
    const u16 *vol_table;
    u8 display_cnt;
    u8 sta;
} fmtx_cb_t;
extern fmtx_cb_t fmtx_cb;

void bsp_fmtx_init(void);
void bsp_fmtx_set_freq(u16 freq);
u16 bsp_fmtx_freq_dec(u16 freq);
u16 bsp_fmtx_freq_inc(u16 freq);
void bsp_fmtx_set_vol(u8 vol);
#endif
