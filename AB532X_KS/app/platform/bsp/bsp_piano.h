#ifndef  _BSP_PIANO_H_
#define  _BSP_PIANO_H_

enum {
    WARNING_PIANO,
    WARNING_TONE,
};

//与warning_tone_tbl数组对应
enum {
    TONE_POWER_ON,
    TONE_POWER_OFF,
    TONE_PAIR,
    TONE_BT_DISCONNECT,
    TONE_BT_CONNECT,
    TONE_BT_RING,
    TONE_MAX_VOL,
    TONE_LOW_BATTERY,
    TONE_REDIALING,
    TONE_BT_MODE,
};

//与warning_piano_tbl数组对应
enum {
    PIANO_POWER_ON,
    PIANO_POWER_OFF,
    PIANO_BT_MODE,
    PIANO_CAMERA_MODE,
    PIANO_AUX_MODE,
};

void bsp_piano_warning_play(uint type, uint index);
u8 get_piano_tone_sta(void);
void piano_warning_play(uint type, void *res);
#endif
