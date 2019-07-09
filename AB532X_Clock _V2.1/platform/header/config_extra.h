#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/
#if !AUX_2_SDADC_EN
#undef AUX_REC_EN
#define AUX_REC_EN                  0
#endif

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN      0
#endif // SYS_MODE_BREAKPOINT_EN

#ifndef ENERGY_LED_EN
#define ENERGY_LED_EN               0
#endif

#ifndef PLUGIN_SYS_INIT_FINISH_CALLBACK
#define PLUGIN_SYS_INIT_FINISH_CALLBACK  0
#endif

#ifndef PLUGIN_FUNC_IDLE_ENTER_CHECK
#define PLUGIN_FUNC_IDLE_ENTER_CHECK     0
#endif

#ifndef CHARGE_FINISH_INTO_LOWPOWER
#define CHARGE_FINISH_INTO_LOWPOWER   0
#endif

#ifndef CHARGE_OUT_POWER_ON
#define CHARGE_OUT_POWER_ON 0
#endif

#if !CHARGE_EN
#undef CHARGE_FINISH_INTO_LOWPOWER
#undef CHARGE_OUT_POWER_ON

#define CHARGE_FINISH_INTO_LOWPOWER   0
#define CHARGE_OUT_POWER_ON    0
#endif
/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN
#undef  MUSIC_SDCARD1_EN

#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_WMA_SUPPORT
#undef  MUSIC_APE_SUPPORT
#undef  MUSIC_FLAC_SUPPORT
#undef  MUSIC_SBC_SUPPORT
#undef  MUSIC_ID3_TAG_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_SDCARD1_EN             0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_WMA_SUPPORT           0
#define MUSIC_APE_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define MUSIC_SBC_SUPPORT           0
#define MUSIC_ID3_TAG_EN            0
#endif // FUNC_MUSIC_EN

#if !CHARGE_EN
#undef  CHARGE_TRICK_EN
#undef  CHARGE_DC_IN
#undef  CHARGE_DC_NOT_PWRON
#define CHARGE_TRICK_EN             0
#define CHARGE_DC_NOT_PWRON         0
#define CHARGE_DC_IN()              0
#endif

#if !EARPHONE_DETECT_EN
#undef SDCMD_MUX_DETECT_EARPHONE
#undef EARPHONE_DETECT_INIT
#undef EARPHONE_IS_ONLINE
#undef IS_DET_EAR_BUSY
#define EARPHONE_DETECT_INIT()
#define SDCMD_MUX_DETECT_EARPHONE   0
#define EARPHONE_IS_ONLINE()        0
#define IS_DET_EAR_BUSY()           0
#endif

#if !LINEIN_DETECT_EN
#undef SDCMD_MUX_DETECT_LINEIN
#undef MICL_MUX_DETECT_LINEIN
#undef LINEIN_DETECT_INIT
#undef LINEIN_IS_ONLINE
#undef IS_DET_LINEIN_BUSY

#define LINEIN_DETECT_INIT()
#define SDCMD_MUX_DETECT_LINEIN     0
#define MICL_MUX_DETECT_LINEIN      0
#define LINEIN_IS_ONLINE()          0
#define IS_DET_LINEIN_BUSY()        0
#endif

#ifndef MICL_MUX_DETECT_LINEIN
#define MICL_MUX_DETECT_LINEIN      0
#endif // MICL_MUX_DETECT_LINEIN

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef DOUBLE_KEY_TIME
#define DOUBLE_KEY_TIME             1
#endif // DOUBLE_KEY_TIME

#ifndef SYS_LIMIT_VOLUME
#define SYS_LIMIT_VOLUME            SYS_INIT_VOLUME
#endif // SYS_LIMIT_VOLUME

#ifndef LINEIN_2_PWRDOWN_TONE_EN
#define LINEIN_2_PWRDOWN_TONE_EN    0   //插入Linein直接关机，是否播放关机提示音。
#endif // LINEIN_2_PWRDOWN_TONE_EN

#ifndef USER_PWRKEY_EXT
#define USER_PWRKEY_EXT             0
#define IS_PWRKEY_PRESS()			(0 == (RTCCON & BIT(19)))
#endif // USER_PWRKEY_EXT

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef LED_LOWBAT_EN
#define LED_LOWBAT_EN              0
#endif // RLED_LOWBAT_EN

#ifndef RLED_LOWBAT_FOLLOW_EN
#define RLED_LOWBAT_FOLLOW_EN       0
#endif // RLED_LOWBAT_FOLLOW_EN

#ifndef SLEEP_DAC_OFF_EN
#define SLEEP_DAC_OFF_EN            1
#endif // SLEEP_DAC_OFF_EN

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LED_DISP_EN
#undef LED_LOWBAT_EN
#undef RLED_LOWBAT_FOLLOW_EN
#undef BLED_FFT_EN
#undef BLED_LOW2ON_EN
#undef BLED_CHARGE_FULL_EN
#undef BT_RECONN_LED_EN
#define LED_LOWBAT_EN              0
#define RLED_LOWBAT_FOLLOW_EN       0
#define BLED_FFT_EN                 0
#define BLED_LOW2ON_EN              0
#define BLED_CHARGE_FULL_EN         0
#define BT_RECONN_LED_EN            0
#endif

#if !LED_DISP_EN
#undef LED_INIT
#define LED_INIT()
#undef LED_SET_ON
#define LED_SET_ON()
#undef LED_SET_OFF
#define LED_SET_OFF()
#endif

#if !LED_PWR_EN
#undef LED_PWR_INIT
#define LED_PWR_INIT()
#undef LED_PWR_SET_ON
#define LED_PWR_SET_ON()
#undef LED_PWR_SET_OFF
#define LED_PWR_SET_OFF()
#endif

#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#endif

#ifndef IR_INPUT_NUM_MAX
#define IR_INPUT_NUM_MAX                999         //最大输入数字9999
#endif // IR_INPUT_NUM_MAX

#ifndef FMRX_THRESHOLD_VAL
#define FMRX_THRESHOLD_VAL              128
#endif // FMRX_THRESHOLD_VAL

#ifndef FMRX_SEEK_DISP_CH_EN
#define FMRX_SEEK_DISP_CH_EN            1
#endif // FMRX_SEEK_DISP_CH_EN

#ifndef SD_SOFT_DETECT_EN
#define SD_SOFT_DETECT_EN               0
#undef SD_IS_SOFT_DETECT
#define SD_IS_SOFT_DETECT()             0
#endif // SD_SOFT_DETECT_EN

#if USER_ADKEY_MUX_LED
#undef  USER_ADKEY
#undef  USER_ADKEY_MUX_SDCLK
#undef  ADKEY_PU10K_EN

#define USER_ADKEY                      1
#define USER_ADKEY_MUX_SDCLK            0
#define ADKEY_PU10K_EN                  0
#endif // USER_ADKEY_MUX_LED

#if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD_EN
#define SD_USB_MUX_IO_EN				1
#elif((SD1_MAPPING == SD0MAP_G5) || (SD1_MAPPING == SD0MAP_G4)) && MUSIC_UDISK_EN && MUSIC_SDCARD1_EN
#define SD_USB_MUX_IO_EN				1
#else
#define SD_USB_MUX_IO_EN				0
#endif

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#if !FUNC_REC_EN
#undef  FMRX_REC_EN
#undef  AUX_REC_EN
#undef  MIC_REC_EN
#undef  REC_TYPE_SEL
#undef  REC_AUTO_PLAY
#undef  REC_FAST_PLAY
#undef  BT_REC_EN
#undef  BT_HFP_REC_EN
#undef  REC_STOP_MUTE_1S

#define FMRX_REC_EN                 0
#define AUX_REC_EN                  0
#define MIC_REC_EN                  0
#define REC_AUTO_PLAY               0
#define REC_FAST_PLAY               0
#define BT_REC_EN                   0
#define BT_HFP_REC_EN               0
#define REC_TYPE_SEL                REC_NO
#define REC_STOP_MUTE_1S            0
#endif //FUNC_REC_EN


#if FMRX_INSIDE_EN
#undef  FMRX_QN8035_EN
#define FMRX_QN8035_EN              0
#else
#undef  I2C_EN
#define I2C_EN                      1
#endif

#if !I2C_EN
#undef  I2C_MUX_SD_EN
#define I2C_MUX_SD_EN               0
#endif

#if ((GUI_SELECT & DISPLAY_LCD) == DISPLAY_LCD)
#define GUI_LCD_EN                      1
#else
#define GUI_LCD_EN                      0
#endif

/*****************************************************************************
 * Module    : karaok相关配置
 *****************************************************************************/
#if SYS_KARAOK_EN
#undef FUNC_SPEAKER_EN
#undef DAC_DNR_EN
#undef BT_PLC_EN
#undef FUNC_REC_EN
#undef MUSIC_WMA_SUPPORT
#undef MUSIC_FLAC_SUPPORT
#undef AUX_2_SDADC_EN
#undef AUX_SNR_EN

#define BT_PLC_EN                   0   //暂时先关掉PLC
#define FUNC_SPEAKER_EN             0
#define DAC_DNR_EN                  0   //暂时先关掉动态降噪
#define FUNC_REC_EN                 0   //Karaok不支持录音
#define MUSIC_WMA_SUPPORT           0   //Karaok不支持WMA
#define MUSIC_FLAC_SUPPORT          0   //Karaok不支持FLAC
#define AUX_2_SDADC_EN              0   //Karaok AUX设为直通（一个ADC）
#define AUX_SNR_EN                  0   //Karaok AUX直通，ADC用于MIC，不能通过SDADC的数据动态降噪
#endif

#if SYS_HOWLING_EN
#undef SYS_MAGIC_VOICE_EN
#define SYS_MAGIC_VOICE_EN          1
#endif
/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/
#ifndef BT_A2DP_RECON_EN
#define BT_A2DP_RECON_EN            0
#endif

#if !BT_TWS_EN
#undef BT_TWS_LR_EN
#define BT_TWS_LR_EN                0
#endif

#if !LE_EN
#undef LE_LIGHTING_EN
#undef LE_MUSIC_CTRL_EN
#define LE_LIGHTING_EN              0
#define LE_MUSIC_CTRL_EN            0
#endif

#if LE_LIGHTING_EN || LE_MUSIC_CTRL_EN
    #define BT_DUAL_MODE_EN         1
#else
    #define BT_DUAL_MODE_EN         0
#endif

#ifndef BT_PAIR_SLEEP_EN
#define BT_PAIR_SLEEP_EN            0
#endif // BT_PAIR_SLEEP_EN

/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PB2)
    #if (SD0_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if (SD1_MAPPING == SD0MAP_G2)
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PA7)
    #if (SD0_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if (SD1_MAPPING == SD0MAP_G1)
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif
#elif (UART0_PRINTF_SEL == PRINTF_PB3)
    #if ((SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
    #undef  MUSIC_SDCARD_EN
    #define MUSIC_SDCARD_EN         0
    #endif

    #if ((SD1_MAPPING == SD0MAP_G5) || (SD1_MAPPING == SD0MAP_G6))
    #undef  MUSIC_SDCARD1_EN
    #define MUSIC_SDCARD1_EN        0
    #endif

    #undef  FUNC_USBDEV_EN
    #undef  MUSIC_UDISK_EN
    #define FUNC_USBDEV_EN          0
    #define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
    //关闭所以打印信息
    #undef printf
    #undef vprintf
    #undef print_r
    #undef print_r16
    #undef print_r32
    #undef printk
    #undef vprintk
    #undef print_kr
    #undef print_kr16
    #undef print_kr32

    #define printf(...)
    #define vprintf(...)
    #define print_r(...)
    #define print_r16(...)
    #define print_r32(...)
    #define printk(...)
    #define vprintk(...)
    #define print_kr(...)
    #define print_kr16(...)
    #define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SDCLK_MUX_DETECT_SD
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef IS_DET_SD_BUSY

#define SD_DETECT_INIT()
#define SDCLK_MUX_DETECT_SD         0
#define SD_IS_ONLINE()              0
#define IS_DET_SD_BUSY()            0
#endif

#if !MUSIC_SDCARD1_EN
#undef SD1_DETECT_INIT
#undef SD1_IS_ONLINE
#undef IS_DET_SD1_BUSY

#define SD1_DETECT_INIT()
#define SD1_IS_ONLINE()              0
#define IS_DET_SD1_BUSY()            0
#endif

/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN
    #undef  UDE_STORAGE_EN
    #undef  UDE_SPEAKER_EN
    #undef  UDE_HID_EN
    #undef  UDE_MIC_EN
    #undef  UDE_ENUM_TYPE

    #define UDE_STORAGE_EN              0
    #define UDE_SPEAKER_EN              0
    #define UDE_HID_EN                  0
    #define UDE_MIC_EN                  0
    #define UDE_ENUM_TYPE               0
#else
    #define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08)
#endif

/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_FUNC_MUSIC
#undef WARNING_FUNC_BT
#undef WARNING_FUNC_CLOCK
#undef WARNING_FUNC_FMRX
#undef WARNING_FUNC_AUX
#undef WARNING_FUNC_USBDEV
#undef WARNING_FUNC_SPEAKER
#undef WARNING_LOW_BATTERY
#undef WARNING_BT_CONNECT
#undef WARNING_BT_DISCONNECT
#undef WARNING_BT_INCALL
#undef WARNING_USB_SD
#undef WARNING_BT_HID_MENU
#undef WARNING_BTHID_CONN
#undef WARNING_MAX_VOLUME
#undef WARNING_BT_PAIR

#define WARNING_POWER_ON               0
#define WARNING_POWER_OFF              0
#define WARNING_FUNC_MUSIC             0
#define WARNING_FUNC_BT                0
#define WARNING_FUNC_CLOCK             0
#define WARNING_FUNC_FMRX              0
#define WARNING_FUNC_AUX               0
#define WARNING_FUNC_USBDEV            0
#define WARNING_FUNC_SPEAKER           0
#define WARNING_LOW_BATTERY            0
#define WARNING_BT_CONNECT             0
#define WARNING_BT_DISCONNECT          0
#define WARNING_BT_INCALL              0
#define WARNING_USB_SD                 0
#define WARNING_BT_HID_MENU            0
#define WARNING_BTHID_CONN             0
#define WARNING_MAX_VOLUME             0
#define WARNING_BT_PAIR                0
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN) && (!MUSIC_SDCARD1_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#if SD_USB_MUX_IO_EN == 1
	#undef FUNC_USBDEV_EN
	#define FUNC_USBDEV_EN 			   0
#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

#if ((SD0_MAPPING == SD0MAP_G1) || (SD0_MAPPING == SD0MAP_G4) || (SD0_MAPPING == SD0MAP_G5) || (SD0_MAPPING == SD0MAP_G6))
#define SDCLK_AD_CH                    ADCCH_PA6
#elif (SD0_MAPPING == SD0MAP_G2)
#define SDCLK_AD_CH                    ADCCH_PB1
#else
#define SDCLK_AD_CH                    ADCCH_PE6
#endif
#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN)
#define SD_SUPPORT_EN                  (MUSIC_SDCARD_EN | MUSIC_SDCARD1_EN)

#if MUSIC_SDCARD_EN
#define SDCMD_MUX_DETECT_EN            1
#define SDCLK_MUX_DETECT_EN            1
#else
#undef  USER_ADKEY_MUX_SDCLK
#undef  I2C_MUX_SD_EN

#define I2C_MUX_SD_EN                  0
#define SDCMD_MUX_DETECT_EN            0
#define SDCLK_MUX_DETECT_EN            0
#define USER_ADKEY_MUX_SDCLK           0
#endif // MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#endif

#endif // __CONFIG_EXTRA_H__

