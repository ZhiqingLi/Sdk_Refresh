#ifndef _DISPLAY_LEDSEG_H
#define _DISPLAY_LEDSEG_H

#if (GUI_SELECT & DISPLAY_LEDSEG)
#define SEG_A           BIT(0)
#define SEG_B           BIT(1)
#define SEG_C           BIT(2)
#define SEG_D           BIT(3)
#define SEG_E           BIT(4)
#define SEG_F           BIT(5)
#define SEG_G           BIT(6)
#define SEG_H           BIT(7)

//icon display Definition
#define ICON_PLAY       SEG_A
#define ICON_PAUSE      SEG_B
#define ICON_USB        SEG_C
#define ICON_SD         SEG_D
#define ICON_DDOT       SEG_E
#define ICON_MP3        SEG_F
#define ICON_FM         SEG_G
#define ICON_AUX        0x00

enum {
    DISP_POWERON = 0,
    DISP_VOL,
    DISP_MUSIC_PLAY,
    DISP_MUSIC_PAUSE,
    DISP_EQ,
    DISP_MODEPLAY,
    DISP_NUM,
    DISP_FREQ,
    DISP_FQ_CHAN,
    DISP_AUX,
    DISP_BT,
    DISP_USBPC,
    DISP_RTCTIME,
    DISP_TIME_SET,
    DISP_REC_TIME,
    DISP_REC_PLAYTIME,
    DISP_REC_PAUSE,
    DISP_SPDIF,
    DISP_FMAM,
    DISP_SPEAKER,
    DISP_PLAYDEV,
    DISP_IDLE,
    DISP_OFF,
};

#if (GUI_SELECT == GUI_LEDSEG_7P7S)
#include "ledseg/ledseg_7p7s.h"
#define ledseg_init()               ledseg_7p7s_init()
#define ledseg_off()                ledseg_7p7s_off()
#define ledseg_scan()               ledseg_7p7s_scan()
#define ledseg_update_dispbuf()     ledseg_7p7s_update_dispbuf()
#else
#define ledseg_init()
#define ledseg_off()
#define ledseg_scan()
#define ledseg_update_dispbuf()
#endif

#endif

#endif
