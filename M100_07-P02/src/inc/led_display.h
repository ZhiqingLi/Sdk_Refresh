#ifndef __LED_DISPLAY_H__
#define __LED_DISPLAY_H__
#include "seg_panel.h"
//#include "display/led_tm162x.h"
typedef enum _DISP_DEV
{
    DISP_DEV_NONE = 0,
    DISP_DEV_SLED,
    DISP_DEV_LED1888,
    DISP_DEV_LED8888,
    DISP_DEV_LED57,
    DISP_DEV_LCD58

} DISP_DEV;

typedef enum _DISP_MSG_ID
{
    DISP_MSG_NONE = 0,
    DISP_MSG_PWR_ON,
    DISP_MSG_PWR_OFF,
    DISP_MSG_READ,
    DISP_MSG_DEV,
    DISP_MSG_FILE,
    DISP_MSG_ERR,
    DISP_MSG_MVOL,
    DISP_MSG_EQ,
    DISP_MSG_PLAY_MODE,
    DISP_MSG_REP_AB,
    DISP_MSG_MIC_VOL,
    DISP_MSG_GUITAR_VOL,
    DISP_MSG_TREB,
    DISP_MSG_BASS,
    DISP_MSG_DEC_MIX,
#ifdef FUNC_MULTI_UDISK_PORT_EN
    DISP_MSG_UDISK_PORT,
#endif
#ifdef FUNC_RADIO_EN
    DISP_MSG_RADIO_CH,
#endif
#ifdef FUNC_SOUND_REMIND
    DISP_MSG_ALARM_REMIND1,
    DISP_MSG_ALARM_REMIND2,
#endif
    DISP_MSG_ECHO_DEPTH,
    DISP_MSG_ECHO_DELAY,
    DISP_MSG_SEL_NUM,
#ifdef FUNC_RECORD_FILE_DELETE_EN
    DISP_MSG_FILE_DEL,
#endif
#ifdef FUNC_MIC_FIRST_EN
    DISP_MSG_MIC_FIRST,
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
    DISP_MSG_3D,
#endif
#ifdef FUNC_PROGRAM_EN
    DISP_MSG_PROG_FULL,
    DISP_MSG_PROG_NUM,
#endif

    DISP_MSG_VER,
    DISP_MSG_NO,
    DISP_MSG_MUTE

} DISP_MSG_ID;


typedef enum _DISP_STATE
{
    DISP_STATE_IDLE = 0,	//此状态下允许显示播放时间，Radio频率等
    DISP_STATE_DEV,
    DISP_STATE_FILE	//MP3才有此状态

} DISP_STATE;
#ifdef FUNC_TM1628_LED_EN
typedef enum _ICON_IDX
{
    ICON_USB = 0,  	// 0
    ICON_SD,	       // 1
    ICON_AUX,	       // 2
    ICON_FM,	       // 3
    ICON_BT,	       // 4
    ICON_MHZ,	       // 5
    ICON_BAT1,	       // 6
    ICON_BAT2,	       // 7
    ICON_BAT3,	       // 8
    ICON_DP,	       // 9
    ICON_COLON,	       // 10
    ICON_FFT_L1,	       // 11
    ICON_FFT_L2,	       // 12
    ICON_FFT_L3,	       // 13
    ICON_FFT_L4,	       // 14
    ICON_FFT_L5,	       // 15
    ICON_FFT_R1,	       // 16
    ICON_FFT_R2,	       // 17
    ICON_FFT_R3,	       // 18
    ICON_FFT_R4,	       // 19
    ICON_FFT_R5,	       // 20
    ICON_NUM,

    ICON_PLAY,  	// 0
    ICON_PAUSE,	  		// 1
    ICON_DP_FM_MHZ,		//5
    ICON_MP3,		  	//6
    ICON_REP,		  	//7
    ICON_RAN,		  	//8
    ICON_INTO,	  		//9
    ICON_MUTE,	  		//10
    ICON_L1,		  	//11
    ICON_L2,		  	//12
    ICON_L3,		  	//13
    ICON_L4,		  	//14
    ICON_SLEEP,			//15
    ICON_BATFUL,		//16
    ICON_BATHAF,		//17
#ifdef AU6210K_BOOMBOX_DEMO
    ICON_STERO,
    ICON_MEMORY,
    ICON_ALARM1,
    ICON_ALARM2,
#endif

} ICON_IDX;


typedef enum _ICON_STATU
{
    LIGHT_OFF,
    LIGHT_ON,
    LIGHT_BLINK

} ICON_STATU;
#endif
typedef struct _ST_BLINK
{
	uint8_t MuteBlink		: 1;	//复用做SLED MP3 PAUSE/STOP/MUTE闪烁标志(慢速闪烁)
	uint8_t HourSet		: 1;	//复用做SLED MP3快进快退/Radio搜台闪烁标志(快速闪烁)
	uint8_t MinSet			: 1;	//复用做SLED MP3播放闪烁标志(常速闪烁)
	uint8_t RepeatBlink	: 1;
	uint8_t MuteBlankScreen: 1;
	uint8_t HourBlankScreen: 1;
	uint8_t MinBlankScreen	: 1;
	uint8_t RepeatOff		: 1;

} ST_BLINK;

typedef union _UN_BLINK
{
	ST_BLINK 	Blink;
	uint8_t		BlinkFlag;

} UN_BLINK;


#define AMPLITUDE_INTERVAL		100
#define FFFB_BLINK_INTERVAL		300
#define PLAY_BLINK_INTERVAL		500
#define MUTE_BLINK_INTERVAL		1000
#define SCAN_BLINK_INTERVAL		300
#define DEV_SYMBOL_INTERVAL		1000
#define NORMAL_INTERVAL			3000//1000
#define FM_NORMAL_INTERVAR              1500
#define FM_CH_INTERVAR                       500
#define INTERIM_INTERVAL		2000	// EQ、Repeat、Volume显示保持状态
#define SET_INTERVL				5000	// RTC设置等显示状态保持时间
#define POWERON_INTERVL				5000
//typedef struct _DISP_INFO_
//{
// DISP_DEV	gDisplayMode;
//DISP_STATE gDispState;
//TIMER		DispTmr;
//TIMER		DispMsgTmr;
//uint8_t             DispMsgId;
//#ifdef FUNC_TM1628_LED_EN
//DISP_TYPE	gDispBuff[MAX_COM_NUM*2];
//#else
//DISP_TYPE	gDispBuff[MAX_COM_NUM];
//#endif
//UN_BLINK gBlink;
//bool gLedDispRefreshFlag;

//}DISP_INFO;

//extern DISP_INFO * gDisp;
extern DISP_TYPE	gDispBuff[];
extern DISP_TYPE	gDispBuffBak[];
extern UN_BLINK gBlink;
extern TIMER	DispTmr;
extern TIMER		DispMsgTmr;
//extern TIMER            LedFlashTmr;
//extern bool             LedFlashOn;
extern uint8_t gDispPrevRep;
extern uint8_t gDispPrevEQ;
extern uint8_t gDispCurVol;
extern uint8_t gDispPrevVol;
#if 1//def FUNC_KARAOKE_EN
extern uint8_t 		gDispCurMicVol;
extern uint8_t 		gDispPrevMicVol;
#endif
extern uint8_t gDispPrevPlayState;
extern DISP_DEV	gDisplayMode;
extern bool gLedDispRefreshFlag;
extern uint8_t             gDispMsgId;

void DispSendMessage(uint16_t DispMsgTime, uint8_t DispMsgId);
// Clear Screen
void ClearScreen(void);
void ClearAllIcon(void);
// Display initilization
// Indicate whether the system in "STAND-BY" state or not.
void DispInit(bool IsStandBy);

// Display de-initialization.
// Indicate whether the system in "STAND-BY" state or not.
void DispDeInit(void);

// 显示刷新.
// Call by display refresh ISR.
void LedFlushDisp(void);

// 显示刷新.
// Lcd display refresh ISR.
void LcdFlushDisp(void);

// 设置Repeat模式时调用.
void DispRepeat(bool IsForced);

// 设置EQ时调用.
void DispEQ(bool IsForced);

// 播放状态变化时调用(Play/Pause/Stop/FF/FB).
void DispPlayState(void);

// 文件夹功能打开/关闭时调用.
void DispFoldState(void);
#ifdef FUNC_KARAOKE_EN
void DispEchoDelayStep(bool IsForced);
// MIC音量设置时调用.
void DispMicVolume(bool IsForced);
void DispDecoderMix(bool IsForced);
#endif

#ifdef FUNC_TREB_BASS_EN
void DispTreb(bool IsForced);
void DispBass(bool IsForced);
#endif

// 音量设置时调用.
void DispVolume(bool IsForced);
#ifdef FUNC_PT231X_EN
// 高音设置时调用.
void DispTreb(bool IsForced);

// 低音设置时调用.
void DispBass(bool IsForced);
#endif
// Mute状态变化时调用.
void DispMute(void);

//" LOD"
void DispLoad(void);

// 文件切换时调用.
void DispDev(void);

#ifdef FUNC_RTC_EN
void DispRtc(void);
#endif

void DispNum(uint16_t Num);

// 设备切换时调用.
void DispFileNum(void);

#define DIS_PLAYTIME_PALY    0
#define DIS_PLAYTIME_PAUSE   1
void DispPlayTime(uint8_t Type);
void DispResume(void);

// 主函数调用.
void Display(void);

//显示Power on信息
void DispPowerOn(void);
void DispPowerOff(void);
// 进入Audio显示时调用
void DispAudio(void);
#ifdef FUNC_SEG_LCD_EN
void LCD_Drv_Init(void);
#endif
#ifdef FUNC_SLEEP_EN
void DispSleepRemainTime(void);
void DispSleepTime(void);
#endif
#endif
