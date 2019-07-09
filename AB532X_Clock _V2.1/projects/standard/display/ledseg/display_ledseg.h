#ifndef _DISPLAY_LEDSEG_H
#define _DISPLAY_LEDSEG_H

#if (GUI_SELECT == GUI_LEDSEG_7P3S_HBS7642)
#define SEG_A           BIT(6)
#define SEG_B           BIT(1)
#define SEG_C           BIT(4)
#define SEG_D           BIT(3)
#define SEG_E           BIT(2)
#define SEG_F           BIT(0)
#define SEG_G           BIT(5)
#define SEG_H           BIT(7)


#define ICON_PLAY       0x00
#define ICON_PAUSE      0x00
#define ICON_USB        0x00
#define ICON_SD         BIT(1)		//GR4
#define ICON_DDOT       BIT(4)		//GR5
#define ICON_MP3        0x00
#define ICON_FM         BIT(5)		//GR4
#define ICON_AUX        BIT(0)		//GR4
#define ICON_BT         BIT(4)		//GR4
#define ICON_CLK1       BIT(2)		//GR4
#define ICON_CLK2       BIT(3)		//GR4
#define ICON_CHARGE     BIT(6)		//GR4
#define ICON_BAT1	    BIT(3)		//GR5
#define ICON_BAT2	    BIT(2)		//GR5
#define ICON_BAT3	    BIT(6)		//GR5
#define ICON_BAT4	    BIT(1)		//GR5
#else
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
#endif
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
    DISP_ALARM_1,
    DISP_ALARM_2,
};

#if (GUI_SELECT == GUI_LEDSEG_7P7S)
#include "ledseg/ledseg_7p7s.h"
#define ledseg_init()               ledseg_7p7s_init()
#define ledseg_off()                ledseg_7p7s_off()
#define ledseg_scan()               ledseg_7p7s_scan()
#define ledseg_update_dispbuf()     ledseg_7p7s_update_dispbuf()
#elif (GUI_SELECT == GUI_LEDSEG_7P3S_HBS7642)
#include "ledseg/ledseg_7p3s_HBS7642.h"
#define ledseg_init()               ledseg_7p3s_HBS7642_init()
#define ledseg_off()                ledseg_7p3s_HBS7642_off()
#define ledseg_scan()               ledseg_7p3s_HBS7642_scan()
#define ledseg_update_dispbuf()     ledseg_7p3s_HBS7642_update_dispbuf()
#else
#define ledseg_init()
#define ledseg_off()
#define ledseg_scan()
#define ledseg_update_dispbuf()
#endif

#endif
