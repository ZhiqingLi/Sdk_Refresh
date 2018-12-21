/****************************************************************************************
*
*                       车机四声道DAC及音效控制
*   1、总音量调数字音量，宏SYS_ADJ_DIGVOL_EN需要置1
*   2、可以分别调前，后， 左， 右音量。
*   3、配置工具DAC输声道输出要选择四声道
*   4、AUX需要进SDADC通路(配置工具上选择)
*****************************************************************************************/

#include "include.h"

#if DAC_AUCAR_EN
aucar_cb_t aucar_cb AT(.buf.car_audio);

AT(.text.bsp.aucar.table)
const u8 aucar_fad_vol_table[16 + 1] = {
    //取值范围：(54-54) ~ (54+5),共60个值;
    //说明：值(54-54)对应音量-54dB; 54对应音量0dB;
    54-54, 54-43, 54-32, 54-26, 54-22, 54-18, 54-15, 54-12,
    54-10, 54-8,  54-6,  54-5, 54-4,  54-3,  54-2,  54-1,  54,
};

AT(.text.bsp.aucar.table)
const u16 aucar_bal_vol_table[16 + 1] = {
    DIG_N60DB,  DIG_N43DB,  DIG_N32DB,  DIG_N26DB,  DIG_N24DB,  DIG_N22DB,  DIG_N20DB,  DIG_N18DB, DIG_N16DB,
    DIG_N14DB,  DIG_N12DB,  DIG_N10DB,  DIG_N8DB,   DIG_N6DB,   DIG_N4DB,   DIG_N2DB,   DIG_N0DB,
};

//前后音量平衡控制
AT(.text.bsp.aucar)
void bsp_aucar_set_fad_vol(u8 f_vol, u8 b_vol)
{
//    printf("fad_vol: %d, %d\n", f_vol, b_vol);
    aucar_cb.f_vol = f_vol;
    aucar_cb.b_vol = b_vol;
    dac_set_volume(PA1_ANL_SET | aucar_fad_vol_table[aucar_cb.f_vol]);   //设置前置音量
    dac_set_volume(PA2_ANL_SET | aucar_fad_vol_table[aucar_cb.b_vol]);   //设置后置音量
}

//左右音量平衡控制
AT(.text.bsp.aucar)
void bsp_aucar_set_bal_vol(u8 l_vol, u8 r_vol)
{
//    printf("bal_vol: %d, %d\n", l_vol, r_vol);
    aucar_cb.l_vol = l_vol;
    aucar_cb.r_vol = r_vol;
    dac_set_balance(aucar_bal_vol_table[aucar_cb.l_vol], aucar_bal_vol_table[aucar_cb.r_vol]);
}

AT(.text.bsp.aucar)
void bsp_aucar_fad_inc(void)
{
    u8 f_vol = 16, b_vol = 16;

    aucar_cb.fad_level++;
    if (aucar_cb.fad_level > 32) {
        aucar_cb.fad_level = 32;
    }

    if (aucar_cb.fad_level > 16) {
        f_vol = 16;
        b_vol = 32 - aucar_cb.fad_level;
    } else if (aucar_cb.fad_level < 16) {
        f_vol = aucar_cb.fad_level;
        b_vol = 16;
    }
    bsp_aucar_set_fad_vol(f_vol, b_vol);
}

AT(.text.bsp.aucar)
void bsp_aucar_fad_dec(void)
{
    u8 f_vol = 16, b_vol = 16;

    if (aucar_cb.fad_level > 0) {
        aucar_cb.fad_level--;
    }

    if (aucar_cb.fad_level > 16) {
        f_vol = 16;
        b_vol = 32 - aucar_cb.fad_level;
    } else if (aucar_cb.fad_level < 16) {
        f_vol = aucar_cb.fad_level;
        b_vol = 16;
    }
    bsp_aucar_set_fad_vol(f_vol, b_vol);
}

AT(.text.bsp.aucar)
void bsp_aucar_bal_inc(void)
{
    u8 l_vol = 16, r_vol = 16;

    aucar_cb.bal_level++;
    if (aucar_cb.bal_level > 32) {
        aucar_cb.bal_level = 32;
    }

    if (aucar_cb.bal_level > 16) {
        l_vol = 16;
        r_vol = 32 - aucar_cb.bal_level;
    } else if (aucar_cb.bal_level < 16) {
        l_vol = aucar_cb.bal_level;
        r_vol = 16;
    }
    bsp_aucar_set_bal_vol(l_vol, r_vol);
}

AT(.text.bsp.aucar)
void bsp_aucar_bal_dec(void)
{
    u8 l_vol = 16, r_vol = 16;

    if (aucar_cb.bal_level > 0) {
        aucar_cb.bal_level--;
    }

    if (aucar_cb.bal_level > 16) {
        l_vol = 16;
        r_vol = 32 - aucar_cb.bal_level;
    } else if (aucar_cb.bal_level < 16) {
        l_vol = aucar_cb.bal_level;
        r_vol = 16;
    }
    bsp_aucar_set_bal_vol(l_vol, r_vol);
}

AT(.text.bsp.aucar)
void bsp_aucar_init(void)
{
    bsp_aucar_set_fad_vol(16, 16);
    bsp_aucar_set_bal_vol(16, 16);
    aucar_cb.fad_level = 16;
    aucar_cb.bal_level = 16;
}

#endif // DAC_AUCAR_EN

