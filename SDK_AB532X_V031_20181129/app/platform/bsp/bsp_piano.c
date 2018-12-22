#include "include.h"

//          1/8  4/4   3/4   2/4      1/4  4/4   3/4   2/4
//Rhythm = [    0.062 0.094 0.125         0.125 0.187 0.250];
//           Do     Re     Mi     Fa     Sol    La    Si
//Fin    = [261.63 293.67 329.63 349.23 391.99 440 493.88 0 0 0];
#define PIANO_BASEKEYDLY        (int)(48000*0.125)
#define TONE_DELAY(x)           (int)(48*x)
#define TONE_FREQUENCY(a, b)    (u32)((a << 16) | (u16)((1 << 15) / (48000 / b)))

struct tone_tbl_t {
    u32 res;
    u32 last_time;  //持续时间
};

struct warning_tone_tbl_t {
    u32 len;
    const struct tone_tbl_t *res_tbl;
};

struct warning_piano_tbl_t {
    u32 len;
    const u32 *res_tbl;
};

extern const u16 dac_dvol_table[16 + 1];

//频率：    380Hz  间隔40mS  380Hz
//持续时间：103mS            103mS
//窗函数：  fade out         fade out
//窗持续：  1/2              1
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_disconnected_tbl[3] = {
    {TONE_FREQUENCY(0x7ff, 380),        TONE_DELAY(103)},
    {0,                                 TONE_DELAY(40)},    //静音
    {TONE_FREQUENCY(0x7ff, 380),        TONE_DELAY(103)},
};

//频率：    760Hz
//持续时间：100mS
//窗函数：  fade out
//窗持续：  1/2
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_connected_tbl[1] = {
    {TONE_FREQUENCY(0x7ff, 760),        TONE_DELAY(100)},
};

//频率：    501Hz
//持续时间：100mS
//窗函数：  fade out
//窗持续：  1/2
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_pair_tbl[1] = {
    {TONE_FREQUENCY(0x7ff, 501),        TONE_DELAY(100)},
};

//频率：    200Hz  间隔20mS  302Hz   间隔20mS  400Hz   间隔20mS  501Hz
//持续时间：160mS            160mS             157mS             157mS
//窗函数：  fade out         fade out          fade out          fade out
//窗持续：  1/2              1/2               1/2               1
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_power_on_tbl[7] = {
    {TONE_FREQUENCY(0x7ff, 200),        TONE_DELAY(160)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 302),        TONE_DELAY(160)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 400),        TONE_DELAY(157)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 501),        TONE_DELAY(157)},
};

//频率：    501Hz  间隔20mS  400Hz   间隔20mS  302Hz   间隔20mS  200Hz
//持续时间：160mS            160mS             157mS             157mS
//窗函数：  fade out         fade out          fade out          fade out
//窗持续：  1/2              1/2               1/2               1
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_power_off_tbl[7] = {
    {TONE_FREQUENCY(0x7ff, 501),        TONE_DELAY(160)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 400),        TONE_DELAY(160)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 302),        TONE_DELAY(157)},
    {0,                                 TONE_DELAY(20)},    //静音
    {TONE_FREQUENCY(0x7ff, 200),        TONE_DELAY(157)},
};

//频率：    450Hz
//持续时间：1365mS
//窗函数：  sine
//窗调制：  15Hz
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_ring_tbl[2] = {
    {TONE_FREQUENCY(0x800a, 450),       (TONE_DELAY(1364)+24)},
    {0,                                 TONE_DELAY(20)},    //静音
};

//频率：    2600Hz
//持续时间：100mS
//窗函数：  fade out
//窗调制：  1
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_maxvol_tbl[1] = {
    {TONE_FREQUENCY(0x7ff, 2600),        TONE_DELAY(100)},
};

//频率：    848Hz  间隔50mS  639Hz
//持续时间：207mS            208mS
//窗函数：  fade out         fade out
//窗调制：  1                1
AT(.piano_com.tbl)
static const struct tone_tbl_t tone_low_battery_tbl[3] = {
    {TONE_FREQUENCY(0x7ff, 848),        TONE_DELAY(207)},
    {0,                                 TONE_DELAY(50)},    //静音
    {TONE_FREQUENCY(0x7ff, 639),        TONE_DELAY(208)},
};

AT(.piano_com.tbl)
static const struct tone_tbl_t tone_redialing_tbl[1] = {
    {TONE_FREQUENCY(0x7ff, 524),       TONE_DELAY(100)},
};

AT(.piano_com.tbl)
static const struct tone_tbl_t tone_bt_mode_tbl[3] = {
    {TONE_FREQUENCY(0x7ff, 392),        TONE_DELAY(100)},
    {0,                                 TONE_DELAY(53)},    //静音
    {TONE_FREQUENCY(0x7ff, 586),        TONE_DELAY(98)},
};

//piano提示音
AT(.piano_com.tbl)
static const u32 piano_bt_mode_tbl[1] = {
    0x00236521
};

AT(.piano_com.tbl)
static const u32 piano_camera_mode_tbl[1] = {
    0x00030201
};

AT(.piano_com.tbl)
static const u32 piano_aux_mode_tbl[1] = {
    0x00131203
};

AT(.piano_com.tbl)
static const u32 piano_power_on_tbl[1] = {
    0x07050301
};

AT(.piano_com.tbl)
static const u32 piano_power_off_tbl[1] = {
    0x01030507
};

AT(.text.piano.table)
static const struct warning_tone_tbl_t warning_tone_tbl[] = {
    {sizeof(tone_power_on_tbl)  / sizeof(struct tone_tbl_t),       tone_power_on_tbl       },       //1
    {sizeof(tone_power_off_tbl) / sizeof(struct tone_tbl_t),       tone_power_off_tbl      },       //2
    {sizeof(tone_pair_tbl)      / sizeof(struct tone_tbl_t),       tone_pair_tbl           },       //3
    {sizeof(tone_disconnected_tbl)  / sizeof(struct tone_tbl_t),   tone_disconnected_tbl   },       //4
    {sizeof(tone_connected_tbl) / sizeof(struct tone_tbl_t),       tone_connected_tbl      },       //5
    {sizeof(tone_ring_tbl)      / sizeof(struct tone_tbl_t),       tone_ring_tbl           },       //6
    {sizeof(tone_maxvol_tbl)    / sizeof(struct tone_tbl_t),       tone_maxvol_tbl         },       //7
    {sizeof(tone_low_battery_tbl) / sizeof(struct tone_tbl_t),     tone_low_battery_tbl    },       //8
    {sizeof(tone_redialing_tbl) / sizeof(struct tone_tbl_t),       tone_redialing_tbl      },       //9
    {sizeof(tone_bt_mode_tbl) / sizeof(struct tone_tbl_t),         tone_bt_mode_tbl        },       //10
};

AT(.text.piano.table)
static const struct warning_piano_tbl_t warning_piano_tbl[] = {
    {sizeof(piano_power_on_tbl) / 4,    piano_power_on_tbl},
    {sizeof(piano_power_off_tbl) / 4,   piano_power_off_tbl},
    {sizeof(piano_bt_mode_tbl) / 4,     piano_bt_mode_tbl},
    {sizeof(piano_camera_mode_tbl) / 4, piano_camera_mode_tbl},
    {sizeof(piano_aux_mode_tbl) / 4,    piano_aux_mode_tbl},
};

//type:  0-->Piano,  1--> Tone
AT(.text.piano)
void bsp_piano_warning_play(uint type, uint index)
{
    void *res;

    if (type == WARNING_PIANO) {
        res = (void *)&warning_piano_tbl[index];
    } else {
        res = (void *)&warning_tone_tbl[index];
    }
    bsp_aux_piano_start();
    piano_warning_play(type, res);
    bsp_aux_piano_exit();
}


