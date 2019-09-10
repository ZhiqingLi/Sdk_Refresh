/**
 **************************************************************************************
 * @file    Key.c
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-01-11 17:30:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include "adc_levels.h"
#include "app_config.h"
#include "app_message.h"
#include "debug.h"
#include "key.h"
#include "ctrlvars.h"
#include "timeout.h"
#ifdef CFG_RES_ADC_KEY_SCAN
#include "adc_key.h"
#endif
#ifdef CFG_RES_IR_KEY_SCAN
#include "ir_key.h"
#endif
#ifdef CFG_RES_CODE_KEY_USE
#include "code_key.h"
#endif
#ifdef CFG_RES_IO_KEY_SCAN
#include "io_key.h"
#endif

const uint16_t KEY_DEFAULT_MAP[5][KEY_MSG_DEFAULT_NUM];
extern void GIE_DISABLE(void); //defined@interrupt.c
extern void GIE_ENABLE(void);

uint16_t gFuncID = 0;
uint16_t gKeyValue;
#ifdef CFG_FUNC_DBCLICK_MSG_EN
KEYBOARD_MSG dbclick_msg;
TIMER	DBclicTimer;
#endif

/*************************************************
* 基础按键属性对应消息列表
*
***************************************************/
const uint16_t KEY_DEFAULT_MAP[5][KEY_MSG_DEFAULT_NUM] =
{	
	{////按下
	  //-----power key----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  //-----adc key1----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
	  /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
	  /*20*/ MSG_NONE,
	  //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      //-----power key----//
      /*00*/ MSG_DEVICE_SERVICE_RECORDER, //MSG_DEVICE_SERVICE_TREB_UP,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK, //MSG_DEVICE_SERVICE_TREB_DW,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,  //MSG_DEVICE_SERVICE_BASS_UP,
      /*03*/ MSG_DEVICE_SERVICE_RTC, //MSG_DEVICE_SERVICE_BASS_DW,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN, //MSG_DEVICE_SERVICE_EFFECTMODE,
	  /*05*/ MSG_DEVICE_SERVICE_RTC_UP, //MSG_DEVICE_SERVICE_EFFECTMODE,
      //-----adc key1----//
      /*00*/ MSG_DEVICE_SERVICE_SWITCH_MODE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_DEVICE_SERVICE_RECORDER,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,//MSG_DEVICE_SERVICE_MUSIC_WHETHER_REC,
	  /*05*/ MSG_NONE,//MSG_NONE,
	  /*06*/ MSG_DEVICE_SERVICE_EFFECTMODE,//MSG_DEVICE_SERVICE_EQ,
      /*07*/ MSG_DEVICE_SERVICE_EQ,//MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*08*/ MSG_DEVICE_SERVICE_VOCAL_CUT,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_DEVICE_SERVICE_MIC_SHUNNING,//MSG_DEVICE_SERVICE_MIC_VOLDOWN,
	  /*10*/ MSG_DEVICE_SERVICE_MUTE,//MSG_DEVICE_SERVICE_MIC_EFFECT_UP,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_DEVICE_SERVICE_PAUSE,
	  /*04*/ MSG_NONE,//MSG_DEVICE_SERVICE_PRE,
	  /*05*/ MSG_NONE,//MSG_DEVICE_SERVICE_NEXT,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*09*/ MSG_NONE,//MSG_MEDIA_PLAY_MODE
	  /*10*/ MSG_NONE,//MSG_RADIO_PLAY_SCAN
	  /*11*/ MSG_DEVICE_SERVICE_1,
	  /*12*/ MSG_DEVICE_SERVICE_2,
	  /*13*/ MSG_DEVICE_SERVICE_3,
	  /*14*/ MSG_DEVICE_SERVICE_4,
	  /*15*/ MSG_DEVICE_SERVICE_5,
	  /*16*/ MSG_DEVICE_SERVICE_6,
	  /*17*/ MSG_DEVICE_SERVICE_7,
	  /*18*/ MSG_DEVICE_SERVICE_8,
	  /*19*/ MSG_DEVICE_SERVICE_9,
      /*20*/ MSG_DEVICE_SERVICE_0,
      //-----Gpio key-releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_DEVICE_SERVICE_RTC_SET,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_NONE,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,//MSG_NONE,
	  /*08*/ MSG_DEVICE_SERVICE_VB,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_NONE,//MSG_DEVICE_SERVICE_MIC_VOLDOWN,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_SOFT_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
	  /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },

}; 
/*************************************************
* U盘，TF卡模式下按键属性对应消息列表
* 
***************************************************/
const uint16_t MEDIA_PLAY_KEY_MAP[5][KEY_MSG_DEFAULT_NUM] =
{
	{////按下
	  //-----power key----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
	  /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
	  /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
	  /*20*/ MSG_NONE,
	  //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      //-----power key----//
      /*00*/ MSG_MEDIA_PLAY_PLAY_PAUSE, //MSG_DEVICE_SERVICE_TREB_UP,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK, //MSG_DEVICE_SERVICE_TREB_DW,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,  //MSG_DEVICE_SERVICE_BASS_UP,
      /*03*/ MSG_DEVICE_SERVICE_RTC, //MSG_DEVICE_SERVICE_BASS_DW,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN, //MSG_DEVICE_SERVICE_EFFECTMODE,
	  /*05*/ MSG_DEVICE_SERVICE_RTC_UP, //MSG_DEVICE_SERVICE_EFFECTMODE,
      //-----adc key1----// 
      /*00*/ MSG_DEVICE_SERVICE_SWITCH_MODE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_MEDIA_PLAY_PRE,
      /*03*/ MSG_MEDIA_PLAY_PRE,//MSG_MEDIA_PLAY_NEXT,
	  /*04*/ MSG_MEDIA_PLAY_NEXT,//MSG_DEVICE_SERVICE_MUTE,
	  /*05*/ MSG_MEDIA_PLAY_PLAY_PAUSE,
	  /*06*/ MSG_DEVICE_SERVICE_EFFECTMODE,//MSG_DEVICE_SERVICE_EQ,
      /*07*/ MSG_DEVICE_SERVICE_EQ,//MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*08*/ MSG_DEVICE_SERVICE_VOCAL_CUT,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_DEVICE_SERVICE_MIC_SHUNNING,//MSG_DEVICE_SERVICE_MIC_VOLDOWN,
	  /*10*/ MSG_DEVICE_SERVICE_MUTE,//MSG_DEVICE_SERVICE_MIC_EFFECT_UP,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_MEDIA_PLAY_PLAY_PAUSE,
	  /*04*/ MSG_MEDIA_PLAY_PRE,
	  /*05*/ MSG_MEDIA_PLAY_NEXT,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*09*/ MSG_MEDIA_PLAY_MODE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_DEVICE_SERVICE_1,
	  /*12*/ MSG_DEVICE_SERVICE_2,
	  /*13*/ MSG_DEVICE_SERVICE_3,
	  /*14*/ MSG_DEVICE_SERVICE_4,
	  /*15*/ MSG_DEVICE_SERVICE_5,
	  /*16*/ MSG_DEVICE_SERVICE_6,
	  /*17*/ MSG_DEVICE_SERVICE_7,
	  /*18*/ MSG_DEVICE_SERVICE_8,
	  /*19*/ MSG_DEVICE_SERVICE_9,
	  /*20*/ MSG_DEVICE_SERVICE_0,
	  //-----Gpio key-Short releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
      /*03*/ MSG_DEVICE_SERVICE_RTC_SET,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_MEDIA_PLAY_FB,
      /*03*/ MSG_MEDIA_PLAY_FB,//MSG_MEDIA_PLAY_FF,
	  /*04*/ MSG_MEDIA_PLAY_FF,//MSG_NONE,
	  /*05*/ MSG_MEDIA_PLAY_MODE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,//MSG_NONE,
	  /*08*/ MSG_DEVICE_SERVICE_VB,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_MEDIA_PLAY_FB,
	  /*05*/ MSG_MEDIA_PLAY_FF,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_SOFT_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_MEDIA_PLAY_FB,
      /*03*/ MSG_MEDIA_PLAY_FB,//MSG_MEDIA_PLAY_FF,
	  /*04*/ MSG_MEDIA_PLAY_FF,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_MEDIA_PLAY_FB,
	  /*05*/ MSG_MEDIA_PLAY_FF,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
      /*03*/ MSG_MEDIA_PLAY_FF_FB_END,
	  /*04*/ MSG_MEDIA_PLAY_FF_FB_END,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_MEDIA_PLAY_FF_FB_END,
	  /*05*/ MSG_MEDIA_PLAY_FF_FB_END,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
};

/*************************************************
* 蓝牙模式下按键属性对应消息列表
*
***************************************************/
const uint16_t BT_PLAY_KEY_MAP[5][KEY_MSG_DEFAULT_NUM] =
{
	{////按下
	  //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      //-----power key----//
      /*00*/ MSG_DEVICE_SERVICE_RECORDER, //MSG_DEVICE_SERVICE_TREB_UP,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK, //MSG_DEVICE_SERVICE_TREB_DW,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,  //MSG_DEVICE_SERVICE_BASS_UP,
      /*03*/ MSG_DEVICE_SERVICE_RTC, //MSG_DEVICE_SERVICE_BASS_DW,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN, //MSG_DEVICE_SERVICE_EFFECTMODE,
      /*05*/ MSG_DEVICE_SERVICE_RTC_UP, //MSG_DEVICE_SERVICE_EFFECTMODE,
      //-----adc key1----// 
      /*00*/ MSG_DEVICE_SERVICE_SWITCH_MODE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_BT_PLAY_PRE,
      /*03*/ MSG_BT_PLAY_PRE,//MSG_BT_PLAY_NEXT,
	  /*04*/ MSG_BT_PLAY_NEXT,//MSG_DEVICE_SERVICE_MUTE,
	  /*05*/ MSG_BT_PLAY_PLAY_PAUSE,
	  /*06*/ MSG_DEVICE_SERVICE_EFFECTMODE,//MSG_DEVICE_SERVICE_EQ,
      /*07*/ MSG_DEVICE_SERVICE_EQ,//MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*08*/ MSG_DEVICE_SERVICE_VOCAL_CUT,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_DEVICE_SERVICE_MIC_SHUNNING,//MSG_DEVICE_SERVICE_MIC_VOLDOWN,
	  /*10*/ MSG_DEVICE_SERVICE_MUTE,//MSG_DEVICE_SERVICE_MIC_EFFECT_UP,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_BT_PLAY_PLAY_PAUSE,
	  /*04*/ MSG_BT_PLAY_PRE,
	  /*05*/ MSG_BT_PLAY_NEXT,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*09*/ MSG_BT_PLAY_MODE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_DEVICE_SERVICE_1,
	  /*12*/ MSG_DEVICE_SERVICE_2,
	  /*13*/ MSG_DEVICE_SERVICE_3,
	  /*14*/ MSG_DEVICE_SERVICE_4,
	  /*15*/ MSG_DEVICE_SERVICE_5,
      /*16*/ MSG_DEVICE_SERVICE_6,
	  /*17*/ MSG_DEVICE_SERVICE_7,
	  /*18*/ MSG_DEVICE_SERVICE_8,
	  /*19*/ MSG_DEVICE_SERVICE_9,
      /*20*/ MSG_DEVICE_SERVICE_0,
      //-----Gpio key-Short releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
      /*03*/ MSG_DEVICE_SERVICE_RTC_SET,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,//MSG_NONE,
	  /*08*/ MSG_DEVICE_SERVICE_VB,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_SOFT_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR- long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_BT_PLAY_FB,
	  /*05*/ MSG_BT_PLAY_FF,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_BT_CONNECT_CTRL,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_BT_HF_VOICE_RECOGNITION,
	  /*05*/ MSG_BT_HF_REDAIL_LAST_NUM,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_BT_PLAY_FF_FB_END,
	  /*05*/ MSG_BT_PLAY_FF_FB_END,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
};


const uint16_t BT_HF_KEY_MAP[5][KEY_MSG_DEFAULT_NUM] =
{
	{////按下
	  //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      //-----power key----//
      /*00*/ MSG_DEVICE_SERVICE_RECORDER,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,
      /*03*/ MSG_DEVICE_SERVICE_RTC,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN,
      /*05*/ MSG_DEVICE_SERVICE_RTC_UP,
      //-----adc key1----// 
      /*00*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,//MSG_BT_PLAY_PRE,//MSG_BT_PLAY_NEXT,
	  /*04*/ MSG_BT_HF_TRANS_CHANGED,//MSG_BT_PLAY_NEXT,
	  /*05*/ MSG_BT_HF_PLAY_PAUSE,
	  /*06*/ MSG_DEVICE_SERVICE_EFFECTMODE,
      /*07*/ MSG_DEVICE_SERVICE_EQ,
	  /*08*/ MSG_DEVICE_SERVICE_VOCAL_CUT,
	  /*09*/ MSG_DEVICE_SERVICE_MIC_SHUNNING,
	  /*10*/ MSG_DEVICE_SERVICE_MUTE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_BT_HF_PLAY_PAUSE,
	  /*04*/ MSG_BT_HF_PRE,
	  /*05*/ MSG_BT_HF_NEXT,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_DEVICE_SERVICE_1,
	  /*12*/ MSG_DEVICE_SERVICE_2,
	  /*13*/ MSG_DEVICE_SERVICE_3,
	  /*14*/ MSG_DEVICE_SERVICE_4,
	  /*15*/ MSG_DEVICE_SERVICE_5,
      /*16*/ MSG_DEVICE_SERVICE_6,
	  /*17*/ MSG_DEVICE_SERVICE_7,
	  /*18*/ MSG_DEVICE_SERVICE_8,
	  /*19*/ MSG_DEVICE_SERVICE_9,
      /*20*/ MSG_DEVICE_SERVICE_0,
      //-----Gpio key-Short releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
      /*03*/ MSG_DEVICE_SERVICE_RTC_SET,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,
	  /*08*/ MSG_DEVICE_SERVICE_VB,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_SOFT_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR- long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_BT_PLAY_FB,
	  /*05*/ MSG_BT_PLAY_FF,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_BT_HF_CALL_REJECT,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_BT_PLAY_FF_FB_END,
	  /*05*/ MSG_BT_PLAY_FF_FB_END,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
};

/*************************************************
* 收音模式下按键属性对应消息列表
* 
***************************************************/
const uint16_t RADIO_PLAY_KEY_MAP[5][KEY_MSG_DEFAULT_NUM] =
{
	{////按下
	  //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      //-----power key----//
      /*00*/ MSG_DEVICE_SERVICE_RECORDER, //MSG_DEVICE_SERVICE_TREB_UP,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK, //MSG_DEVICE_SERVICE_TREB_DW,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,  //MSG_DEVICE_SERVICE_BASS_UP,
      /*03*/ MSG_DEVICE_SERVICE_RTC, //MSG_DEVICE_SERVICE_BASS_DW,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN, //MSG_DEVICE_SERVICE_EFFECTMODE,
      /*05*/ MSG_DEVICE_SERVICE_RTC_UP, //MSG_DEVICE_SERVICE_EFFECTMODE,
      //-----adc key1----// 
	  /*00*/ MSG_DEVICE_SERVICE_SWITCH_MODE,//MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,//MSG_DEVICE_SERVICE_RECORDER,
      /*03*/ MSG_RADIO_PLAY_PRE,//MSG_DEVICE_SERVICE_PLAYBACK,
	  /*04*/ MSG_RADIO_PLAY_NEXT,// MSG_DEVICE_SERVICE_MUTE,
	  /*05*/ MSG_RADIO_PLAY_SCAN,
	  /*06*/ MSG_DEVICE_SERVICE_EFFECTMODE,//MSG_DEVICE_SERVICE_EQ,
      /*07*/ MSG_DEVICE_SERVICE_EQ,
	  /*08*/ MSG_DEVICE_SERVICE_VOCAL_CUT,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_DEVICE_SERVICE_MIC_SHUNNING,//MSG_DEVICE_SERVICE_MIC_VOLDOWN,
	  /*10*/ MSG_DEVICE_SERVICE_MUTE,//MSG_DEVICE_SERVICE_MIC_EFFECT_UP,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_RADIO_PLAY_PRE,
	  /*05*/ MSG_RADIO_PLAY_NEXT,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_RADIO_PLAY_SCAN,
	  /*11*/ MSG_DEVICE_SERVICE_1,
	  /*12*/ MSG_DEVICE_SERVICE_2,
	  /*13*/ MSG_DEVICE_SERVICE_3,
	  /*14*/ MSG_DEVICE_SERVICE_4,
	  /*15*/ MSG_DEVICE_SERVICE_5,
	  /*16*/ MSG_DEVICE_SERVICE_6,
	  /*17*/ MSG_DEVICE_SERVICE_7,
	  /*18*/ MSG_DEVICE_SERVICE_8,
	  /*19*/ MSG_DEVICE_SERVICE_9,
	  /*20*/ MSG_DEVICE_SERVICE_0,
	  //-----Gpio key-Short releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
      /*03*/ MSG_DEVICE_SERVICE_RTC_SET,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,//MSG_NONE,
	  /*08*/ MSG_DEVICE_SERVICE_VB,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_DEEPSLEEP,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_RADIO_PLAY_SCAN_DN,
	  /*05*/ MSG_RADIO_PLAY_SCAN_UP,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_DEVICE_SERVICE_SOFT_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,//MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*02*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
      /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR- long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
};
/*************************************************
* LINEIN模式下按键属性对应消息列表
*  
***************************************************/
const uint16_t LINE_PLAY_KEY_MAP[5][KEY_MSG_DEFAULT_NUM] =
{
	{////按下
	  //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-pressed---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	},
    {////短按释放
      /*00*/ MSG_DEVICE_SERVICE_RECORDER, //MSG_DEVICE_SERVICE_TREB_UP,
      /*01*/ MSG_DEVICE_SERVICE_PLAYBACK, //MSG_DEVICE_SERVICE_TREB_DW,
      /*02*/ MSG_DEVICE_SERVICE_REC_DEL,  //MSG_DEVICE_SERVICE_BASS_UP,
      /*03*/ MSG_DEVICE_SERVICE_RTC, //MSG_DEVICE_SERVICE_BASS_DW,
      /*04*/ MSG_DEVICE_SERVICE_RTC_DOWN, //MSG_DEVICE_SERVICE_EFFECTMODE,
      /*05*/ MSG_DEVICE_SERVICE_RTC_UP, //MSG_DEVICE_SERVICE_EFFECTMODE,
	  //-----adc key1----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
      /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-Short releas---//
	  /*00*/ MSG_SYS_POWER,
	  /*01*/ MSG_DEVICE_SERVICE_SWITCH_MODE,
	  /*02*/ MSG_DEVICE_SERVICE_MUTE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_DEVICE_SERVICE_EQ,
	  /*07*/ MSG_DEVICE_SERVICE_VOLDOWN,
	  /*08*/ MSG_DEVICE_SERVICE_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-Short releas---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {////长按
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
      /*07*/ MSG_DEVICE_SERVICE_3D,//MSG_NONE,
	  /*08*/ MSG_DEVICE_SERVICE_VB,//MSG_DEVICE_SERVICE_MIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press---//
	  /*00*/ MSG_SYS_POWER,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按保持
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR- long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_DEVICE_SERVICE_MUSIC_VOLDOWN,
	  /*08*/ MSG_DEVICE_SERVICE_MUSIC_VOLUP,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press hold---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
    {///长按释放
      //-----power key----//
      /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
      /*04*/ MSG_NONE,
      //-----adc key1----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----adc key2----// 
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
	  /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  //-----IR-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
	  /*03*/ MSG_NONE,
	  /*04*/ MSG_NONE,
	  /*05*/ MSG_NONE,
	  /*06*/ MSG_NONE,
	  /*07*/ MSG_NONE,
      /*08*/ MSG_NONE,
	  /*09*/ MSG_NONE,
	  /*10*/ MSG_NONE,
	  /*11*/ MSG_NONE,
      /*12*/ MSG_NONE,
	  /*13*/ MSG_NONE,
	  /*14*/ MSG_NONE,
	  /*15*/ MSG_NONE,
      /*16*/ MSG_NONE,
	  /*17*/ MSG_NONE,
	  /*18*/ MSG_NONE,
	  /*19*/ MSG_NONE,
      /*20*/ MSG_NONE,
      //-----Gpio key-long press release---//
	  /*00*/ MSG_NONE,
	  /*01*/ MSG_NONE,
	  /*02*/ MSG_NONE,
    },
};

uint16_t (*gKeyMap)[KEY_MSG_DEFAULT_NUM] = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])KEY_DEFAULT_MAP;

void SetKeyMap(uint16_t (*Table)[KEY_MSG_DEFAULT_NUM])
{
	if(Table == (uint16_t(*)[KEY_MSG_DEFAULT_NUM])MEDIA_PLAY_KEY_MAP)
	{
		gKeyMap = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])MEDIA_PLAY_KEY_MAP;
	}
	else if(Table == (uint16_t(*)[KEY_MSG_DEFAULT_NUM])BT_PLAY_KEY_MAP)
	{
		gKeyMap = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])BT_PLAY_KEY_MAP;
	}
	else if(Table == (uint16_t(*)[KEY_MSG_DEFAULT_NUM])BT_HF_KEY_MAP)
	{
		gKeyMap = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])BT_HF_KEY_MAP;
	}
	else if(Table == (uint16_t(*)[KEY_MSG_DEFAULT_NUM])RADIO_PLAY_KEY_MAP)
	{
		gKeyMap = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])RADIO_PLAY_KEY_MAP;
	}
	else//case XXXX:other map table
	{
		gKeyMap = (uint16_t(*)[KEY_MSG_DEFAULT_NUM])KEY_DEFAULT_MAP;
	}
}

uint32_t GetGlobalKeyValue(void)
{
    return gFuncID;
}

void ClrGlobalKeyValue(void)
{
    gFuncID = 0;
}


#if (defined(CFG_RES_ADC_KEY_SCAN) ||defined(CFG_RES_IO_KEY_SCAN)|| defined(CFG_RES_IR_KEY_SCAN) || defined(CFG_RES_CODE_KEY_USE))

/**
 * @func        SetGlobalKeyValue
 * @brief       SetGlobalKeyValue,本函数主要目的是多种按键类的消息归一化映射
 * @param       uint8_t KeyType:注意 按键事件类型带转译
                uint16_t KeyValue，Uart提供单独一列映射，键值和旋钮一列，按键事件类型一列，如有必要可拆开单列。
 * @Output      gFuncID:
 				gKeyValue
 * @return      void
 * @Others      
 * Record
 * 1.Date        : 20180119
 *   Author      : pi.wang
 *   Modification: Created function
*/
void SetGlobalKeyValue(uint8_t KeyType, uint16_t KeyValue)
{
    gFuncID = gKeyMap[KeyType-1][KeyValue];

    #ifdef CFG_FUNC_BEEP_EN
    if(gFuncID) BeepEnable();
    #endif

    #ifdef CFG_FUNC_DBCLICK_MSG_EN
	if(DbclickGetMsg(gFuncID))
	{
		return;
	}
    #endif

   // DBG("KeyType:%d   , KeyValue:%d  MSG:%d\n",KeyType-1,KeyValue,gFuncID);
}


void KeyInit(void)
{
#ifdef CFG_RES_ADC_KEY_SCAN
	AdcKeyInit();
#endif

#ifdef CFG_RES_IO_KEY_SCAN
	IOKeyInit();
#endif

#ifdef CFG_RES_IR_KEY_SCAN
	IRKeyInit();
#endif

#ifdef CFG_RES_CODE_KEY_USE
	CodeKeyInit();
#endif

#ifdef CFG_ADC_LEVEL_KEY_EN
	ADCLevelsKeyInit();
#endif

#ifdef CFG_FUNC_DBCLICK_MSG_EN
	DbclickInit();
#endif

}

inline bool GIE_STATE_GET(void);

/**
 * @func        KeyScan
 * @brief       KeyScan,根据键值和事件类型查表，输出消息值
 * @param       None  
 * @Output      None
 * @return      MessageId
 * @Others      
 * Record
 * 1.Date        : 20180123
 *   Author      : pi.wang
 *   Modification: Created function
*/
MessageId KeyScan(void)
{
	MessageId KeyMsg = MSG_NONE;

#ifdef CFG_RES_IO_KEY_SCAN//bkd add
	IOKeyMsg ioKeyMsg;
#endif
	
#ifdef CFG_RES_ADC_KEY_SCAN//bkd add
	AdcKeyMsg AdcKeyMsg;
#endif

#ifdef CFG_RES_IR_KEY_SCAN
	IRKeyMsg IRKeyMsg;
#endif
#ifdef CFG_RES_CODE_KEY_USE
	CodeKeyType CodeKey = CODE_KEY_NONE;
#endif
#ifdef CFG_RES_ADC_KEY_SCAN
	AdcKeyMsg = AdcKeyScan();
	if(AdcKeyMsg.index != ADC_CHANNEL_EMPTY && AdcKeyMsg.type != ADC_KEY_UNKOWN_TYPE)
	{
		SetGlobalKeyValue(AdcKeyMsg.type,AdcKeyMsg.index);
	}
#endif

#ifdef CFG_RES_IO_KEY_SCAN
	ioKeyMsg = IOKeyScan();
	if(ioKeyMsg.index != IO_CHANNEL_EMPTY && ioKeyMsg.type != IO_KEY_UNKOWN_TYPE)
	{
		SetGlobalKeyValue(ioKeyMsg.type,ioKeyMsg.index);
	}

#endif


#ifdef CFG_RES_IR_KEY_SCAN
	IRKeyMsg = IRKeyScan();
	if(IRKeyMsg.index != IR_KEY_NONE && IRKeyMsg.type != IR_KEY_UNKOWN_TYPE)
	{
		SetGlobalKeyValue(IRKeyMsg.type,IRKeyMsg.index);
		DBG("IRKeyindex = %d, %d\n", IRKeyMsg.index, IRKeyMsg.type);
	}
#endif

#ifdef CFG_RES_CODE_KEY_USE
	CodeKey = CodeKeyScan();
	if(CodeKey != CODE_KEY_NONE)
	{
		SetGlobalKeyValue(CodeKey, KEY_CODE_KEY1);
	}
#endif

#ifdef CFG_ADC_LEVEL_KEY_EN
	KeyMsg=AdcLevelKeyProcess();

	if(KeyMsg != MSG_NONE)
	{
		gFuncID = KeyMsg;
	}			
#endif
	{

#ifdef CFG_FUNC_DBCLICK_MSG_EN
		DbclickProcess();
#endif

		KeyMsg = GetGlobalKeyValue();

		ClrGlobalKeyValue();
//		if(ret)
//		{
//			GIE_ENABLE();
//		}
	}
	return KeyMsg;
}

#endif
/*************************************************
* 按键双击初始化处理函数，只支持单击释放的双击
* dbclick_msg 变量如果定义成数组，则可实现多个双击功能
*
***************************************************/
void DbclickInit(void)
{
#ifdef CFG_FUNC_DBCLICK_MSG_EN
	dbclick_msg.dbclick_en            = 1;
	dbclick_msg.dbclick_counter       = 0;
	dbclick_msg.dbclick_timeout       = 0;

	dbclick_msg.KeyMsg                = CFG_PARA_CLICK_MSG;////Single click msg
	dbclick_msg.dbclick_msg           = CFG_PARA_DBCLICK_MSG;//double  click msg
	TimeOutSet(&DBclicTimer, 0);
#endif

}

/*************************************************
* 按键双击或多击处理函数
*
*
***************************************************/
void DbclickProcess(void)
{
#ifdef CFG_FUNC_DBCLICK_MSG_EN
	if(!IsTimeOut(&DBclicTimer))
	{
		return;
	}
	TimeOutSet(&DBclicTimer, 4);

	if(dbclick_msg.dbclick_en)
	{
		if(dbclick_msg.dbclick_timeout)
		{
			dbclick_msg.dbclick_timeout--;
			if(dbclick_msg.dbclick_timeout == 0)
			{
				gFuncID = dbclick_msg.KeyMsg;//Single click msg
				dbclick_msg.dbclick_counter = 0;
				dbclick_msg.dbclick_timeout = 0;
				DBG("shot click_msg \n");
			}
		}
	}
#endif ///#ifdef CFG_FUNC_DBCLICK_MSG_EN
}
/*************************************************
* 判断按键双击键值处理函数
*
*
***************************************************/
uint8_t DbclickGetMsg(uint16_t Msg)
{
#ifdef CFG_FUNC_DBCLICK_MSG_EN
	if(dbclick_msg.dbclick_en == 0)   return 0;
	/////此处可增加工作模式判断，不是所有模式需要双击功能////////
	//if( WORK_MODE != BT_MODE) return 0
	if((dbclick_msg.KeyMsg == Msg))
	{
    	gFuncID = 0;

		if(dbclick_msg.dbclick_timeout == 0)
		{
    	   dbclick_msg.dbclick_timeout =  20;///4ms*20=80ms
			dbclick_msg.dbclick_counter += 1;
    	   DBG("start double click_msg \n");
		}
		else
		{
			gFuncID =  dbclick_msg.dbclick_msg;//double  click msg
			dbclick_msg.dbclick_counter = 0;
			dbclick_msg.dbclick_timeout = 0;
			DBG("double click_msg \n");
		}
		return 1;
	}
	else
	{
		if(Msg != MSG_NONE)
		{
			dbclick_msg.dbclick_timeout = 0;
			dbclick_msg.dbclick_counter = 0;
		}
		return 0;
	}
#else
	return 0;
#endif
}

