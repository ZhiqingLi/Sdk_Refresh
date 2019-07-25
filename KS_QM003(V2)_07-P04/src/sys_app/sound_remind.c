///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: sound_remind.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "sound_remind.h"
#include "task_decoder.h"
#include "dac.h"
#include "clk.h"
#include "bt_app_func.h"
#include "pcm_fifo.h"
#include "spi_flash.h"
#include "mixer.h"
#include "sys_app.h"
#include "recorder_control.h"
#include "sys_vol.h"
#include "wifi_control.h"

//#ifdef FUNC_AUDIO_VB_EFFECT_EN
//extern bool gVbEffectInited; 
//#endif

#ifdef FUNC_SOUND_REMIND
//extern const uint16_t gDecVolArr[MAX_VOLUME + 1];
extern bool UsbAudioPlayFlag;	//播放提示音时禁止声卡的声音 
extern xTaskHandle AudioProcessHandle;
#ifdef FUNC_RADIO_DIV_CLK_EN
extern void SysClkSet(uint8_t clk_sel, uint8_t clk_div);
#endif
//#ifdef FUNC_RECORD_EN
//extern uint8_t RecordState;
//#endif

bool IsMsgInQueue(void);

bool InterruptSoundRemind = FALSE;

//中英文双语提示音
typedef struct _SOUND_TABLE_ITEM
{
 	uint16_t Id;
	uint32_t IdChn;
	uint32_t IdEng;

} SOUND_TABLE_ITEM;

#define STR_TO_UINT(s)	((s[0] << 0) | (s[1] << 8) | (s[2] << 16) | (s[3] << 24))	
static const SOUND_TABLE_ITEM SoundTable[] = 
{
//   ID                                 中文ID                			英文ID
	{SOUND_NUM0,					    STR_TO_UINT("NUM0"),	STR_TO_UINT("ZERO")},	//0  
	{SOUND_NUM1,					    STR_TO_UINT("NUM1"),	STR_TO_UINT("ONE ")},	//1    
	{SOUND_NUM2,					    STR_TO_UINT("NUM2"),	STR_TO_UINT("TWO ")},	//2   
	{SOUND_NUM3,					    STR_TO_UINT("NUM3"),	STR_TO_UINT("THRE")},	//3    
	{SOUND_NUM4,					    STR_TO_UINT("NUM4"),	STR_TO_UINT("FOUR")},	//4  
	{SOUND_NUM5,					    STR_TO_UINT("NUM5"),	STR_TO_UINT("FIVE")},	//5   
	{SOUND_NUM6,					    STR_TO_UINT("NUM6"),	STR_TO_UINT("SIX ")},	//6 
	{SOUND_NUM7,					    STR_TO_UINT("NUM7"),	STR_TO_UINT("SEVN")},	//7  
	{SOUND_NUM8,					    STR_TO_UINT("NUM8"),	STR_TO_UINT("EIGT")},	//8    
	{SOUND_NUM9,					    STR_TO_UINT("NUM9"),	STR_TO_UINT("NINE")},	//9   
	{SOUND_PLUS5,					    STR_TO_UINT("JIA5"),	STR_TO_UINT("PLS5")},	//加5
	{SOUND_SPEECH_OP,				 	STR_TO_UINT("YYDK"),	STR_TO_UINT("SPOP")},	//语音打开
	{SOUND_SPEECH_CL,				  	STR_TO_UINT("YYGB"),	STR_TO_UINT("SPCL")},	//语音关闭	
	{SOUND_SPEECH_CL_FORCE,	  			STR_TO_UINT("YYQZ"),	STR_TO_UINT("COCS")},	//语音强制关闭
	{SOUND_NEXT_FOLD,				  	STR_TO_UINT("XYML"),	STR_TO_UINT("NXFD")},	//下一目录
	{SOUND_REP_1,					    STR_TO_UINT("DQXH"),	STR_TO_UINT("REP1")},	//单曲循环
	{SOUND_REP_ALL,					  	STR_TO_UINT("QBXH"),	STR_TO_UINT("RPAL")},	//全部循环
	{SOUND_START_REC,				  	STR_TO_UINT("KSLY"),	STR_TO_UINT("STRE")},	//开始录音
	{SOUND_SAVE_REC,				 	STR_TO_UINT("LYBC"),	STR_TO_UINT("SARE")},	//录音保存
	{SOUND_REC_PLAYBACK,				STR_TO_UINT("LYBF"),	STR_TO_UINT("REPL")},	//录音播放
	{SOUND_DELETE_REC,					STR_TO_UINT("SCLY"),	STR_TO_UINT("DERE")},	//删除录音
	{SOUND_LINEIN,					  	STR_TO_UINT("YPSR"),	STR_TO_UINT("LINE")},	//音频输入
	{SOUND_UDISC_PLAYBACK,	  			STR_TO_UINT("YPBF"),	STR_TO_UINT("UDPB")},	//u盘播放
	{SOUND_CARD_PLAYBACK,				STR_TO_UINT("C BF"),	STR_TO_UINT("CDPB")},	//卡播放
	{SOUND_RADIO_MODE,					STR_TO_UINT("SYMS"),	STR_TO_UINT("RDMD")},	//收音模式
	{SOUND_PREV_TRACK,					STR_TO_UINT("S YS"),	STR_TO_UINT("PRSO")},	//上一曲
	{SOUND_NEXT_TRACK,					STR_TO_UINT("X YS"),	STR_TO_UINT("NESN")},	//下一曲
	{SOUND_PAUSE,					    STR_TO_UINT("Z  T"),	STR_TO_UINT("PAUS")},	//暂停
	{SOUND_STOP,					    STR_TO_UINT("T  Z"),	STR_TO_UINT("STOP")},	//停止
	{SOUND_ECHO_REGULATE,				STR_TO_UINT("HXTJ"),	STR_TO_UINT("ECRE")},	//混响调节
	{SOUND_ECHO_PLUS,				  	STR_TO_UINT("HXAD"),	STR_TO_UINT("ECPL")},	//混响加
	{SOUND_ECHO_REDUCE,					STR_TO_UINT("HXDE"),	STR_TO_UINT("ECRD")},	//混响减
	{SOUND_DELAY_REGULATE,				STR_TO_UINT("YSTJ"),	STR_TO_UINT("DLRE")},	//延时调节
	{SOUND_DELAY_PLUS,					STR_TO_UINT("YSAD"),	STR_TO_UINT("DLPL")},	//延时加
	{SOUND_DELAY_REDUCE,				STR_TO_UINT("YSDE"),	STR_TO_UINT("DLRD")},	//延时减
	{SOUND_TREB_REGULATE,				STR_TO_UINT("GYTJ"),	STR_TO_UINT("TRRE")},	//高音调节
	{SOUND_TREB_PLUS,				  	STR_TO_UINT("GYAD"),	STR_TO_UINT("TRPL")},	//高音加
	{SOUND_TREB_REDUCE,					STR_TO_UINT("GYDE"),	STR_TO_UINT("TRRD")},	//高音减
	{SOUND_BASS_REGULATE,				STR_TO_UINT("DYTJ"),	STR_TO_UINT("BARE")},	//低音调节
	{SOUND_BASS_PLUS,				  	STR_TO_UINT("DYAD"),	STR_TO_UINT("BAPL")},	//低音加
	{SOUND_BASS_REDUCE,					STR_TO_UINT("DYDE"),	STR_TO_UINT("BARD")},	//低音减
	{SOUND_MIC_OPEN,				  	STR_TO_UINT("HTDK"),	STR_TO_UINT("MIOP")},	//话筒打开
	{SOUND_MIC_CLOSE,				  	STR_TO_UINT("HTGB"),	STR_TO_UINT("MICL")},	//话筒关闭
	{SOUND_EQ_FLAT,					  	STR_TO_UINT("EQZR"),	STR_TO_UINT("FLAT")},	//自然
	{SOUND_EQ_CLASSIC,					STR_TO_UINT("EQGD"),	STR_TO_UINT("CLAS")},	//古典
	{SOUND_EQ_POP,					  	STR_TO_UINT("EQLX"),	STR_TO_UINT("POP ")},	//流行
	{SOUND_EQ_ROCK,					  	STR_TO_UINT("EQYG"),	STR_TO_UINT("ROCK")},	//摇滚
	{SOUND_EQ_JAZZ,					  	STR_TO_UINT("EQJS"),	STR_TO_UINT("JAZZ")},	//爵士
	{SOUND_EQ_SOFT,					  	STR_TO_UINT("EQRH"),	STR_TO_UINT("SOFT")},	//柔和
	{SOUND_SUBBASS,					  	STR_TO_UINT("Z DY"),	STR_TO_UINT("SUBA")},	//重低音
	{SOUND_MENU,					    STR_TO_UINT("C  D"),	STR_TO_UINT("MENU")},	//菜单
	{SOUND_BT_MODE,					  	STR_TO_UINT("BTMS"),	STR_TO_UINT("BTMD")},	//蓝牙模式
	{SOUND_BT_CONNECT,					STR_TO_UINT("BTLJ"),	STR_TO_UINT("BTCN")},	//蓝牙连接
	{SOUND_BT_DISCONNECT,				STR_TO_UINT("BTDK"),	STR_TO_UINT("BTDN")},	//蓝牙断开
	{SOUND_CALL_INCOM,			  		STR_TO_UINT("CALL"),	STR_TO_UINT("INCM")},	//电话呼入
	{SOUND_POINT,					    STR_TO_UINT("DIAN"),	STR_TO_UINT("POIT")},	//点
	{SOUND_LANG_MODE,				  	STR_TO_UINT("ZWMS"),	STR_TO_UINT("ENMD")},	//中文模式/英文模式
	{SOUND_USB_AUDIO_MODE,				STR_TO_UINT("SKMS"),	STR_TO_UINT("UAMD")},	//声卡模式
	{SOUND_USB_AUDIO_READER_MODE,		STR_TO_UINT("YX T"),	STR_TO_UINT("ARMD")},	//一线通模式
	{SOUND_PLAY,		        		STR_TO_UINT("B  F"),	STR_TO_UINT("PLAY")},	//播放
	{SOUND_FF,		        			STR_TO_UINT("K  J"),	STR_TO_UINT("F  F")},	//快进
	{SOUND_FB,						    STR_TO_UINT("K  T"),	STR_TO_UINT("F  B")},	//快退
	{SOUND_MHZ,						    STR_TO_UINT("ZHHZ"),	STR_TO_UINT("M HZ")},	//兆赫兹
	{SOUND_REP_FOLD,				  	STR_TO_UINT("MLXH"),	STR_TO_UINT("RPFD")},	//目录循环
	{SOUND_INTRO,					    STR_TO_UINT("LLBF"),	STR_TO_UINT("INTR")},	//浏览播放
	{SOUND_RANDOM,					  	STR_TO_UINT("SJBF"),	STR_TO_UINT("RAND")},	//随机播放
	{SOUND_AUTO_SCAN,				  	STR_TO_UINT("ZDST"),	STR_TO_UINT("ATSC")},	//自动搜台
	{SOUND_PRE_STATION,					STR_TO_UINT("SYDT"),	STR_TO_UINT("PRST")},	//上一电台
	{SOUND_NXT_STATION,					STR_TO_UINT("XYDT"),	STR_TO_UINT("NXST")},	//下一电台
	{SOUND_PWR_OFF,					  	STR_TO_UINT("YIGJ"),	STR_TO_UINT("PWOF")},	//已关机
	{SOUND_PWR_ON,					  	STR_TO_UINT("YIKJ"),	STR_TO_UINT("PWON")},	//已开机
	{SOUND_GUITAR_VOL,					STR_TO_UINT("GTYL"),	STR_TO_UINT("GUVL")},	//吉他音量
	{SOUND_SEARCHING,				  	STR_TO_UINT("SSZH"),	STR_TO_UINT("SEAR")},	//搜索中
	{SOUND_MUTE_ON,					  	STR_TO_UINT("JYDK"),	STR_TO_UINT("MUON")},	//静音打开
	{SOUND_MUTE_OFF,				  	STR_TO_UINT("JYGB"),	STR_TO_UINT("MUOF")},	//静音关闭
	{SOUND_MIC_VOL,					  	STR_TO_UINT("HTYL"),	STR_TO_UINT("MIVL")},	//话筒音量
	{SOUND_MAIN_VOL,				  	STR_TO_UINT("ZHYL"),	STR_TO_UINT("MAVL")},	//主音量
	{SOUND_MIC_MODE_OPEN,				STR_TO_UINT("HTMK"),	STR_TO_UINT("MMOP")},	//话筒模式开
	{SOUND_MIC_MODE_CLOSE,				STR_TO_UINT("HTMG"),	STR_TO_UINT("MMCL")},	//话筒模式关
	{SOUND_MIC_FIRST_OPEN,				STR_TO_UINT("HTYK"),	STR_TO_UINT("MFOP")},	//MIC优先开
	{SOUND_MIC_FIRST_CLOSE,				STR_TO_UINT("HTYG"),	STR_TO_UINT("MFCL")},	//MIC优先关
	{SOUND_OPEN,					    STR_TO_UINT("D  K"),	STR_TO_UINT("OPEN")},	//打开
	{SOUND_CLOSE,					    STR_TO_UINT("G  B"),	STR_TO_UINT("CLOS")},	//关闭
	{SOUND_BAT_LOW_PWR,					STR_TO_UINT("DLDI"),	STR_TO_UINT("LPWR")},	//电池电量低，请充电
	{SOUND_BAT_LOW_PWR1,				STR_TO_UINT("DLD1"),	STR_TO_UINT("LPW1")},	//电池电量低，请充电
	{SOUND_CHARGING,				  	STR_TO_UINT("ZZCD"),	STR_TO_UINT("CHAG")},	//正在充电
	{SOUND_BAT_FULL,				 	STR_TO_UINT("DLYM"),	STR_TO_UINT("FULL")},	//电量已充满
	{SOUND_WAIT_PAIR,				  	STR_TO_UINT("DDPD"),	STR_TO_UINT("WTPA")},	//等待配对设备
	{SOUND_SHI,						    STR_TO_UINT("S HI"),	STR_TO_UINT("T EN")},	//十
	{SOUND_BAI,						    STR_TO_UINT("B AI"),	STR_TO_UINT("HDRD")},	//百
	{SOUND_MIC_INSERT,					STR_TO_UINT("HTCR"),	STR_TO_UINT("MCIN")},	//话筒插入
	{SOUND_MIC_PULLOUT,					STR_TO_UINT("HTBC"),	STR_TO_UINT("MCOU")},	//话筒拔出
	{SOUND_WIFI_MODE,					STR_TO_UINT("WFMS"),	STR_TO_UINT("WFMD")},	//WIFI模式
	{SOUND_WIFI_START,					STR_TO_UINT("WFQD"),	STR_TO_UINT("WFST")},	//系统启动中
	{SOUND_WIFI_WPS,					STR_TO_UINT("WFSS"),	STR_TO_UINT("WFPS")},	//网络配置启动
	{SOUND_WIFI_DONE,					STR_TO_UINT("WFWC"),	STR_TO_UINT("WFDO")},	//WIFI启动完成
	{SOUND_WIFI_CONNECT,		  		STR_TO_UINT("WFLJ"),	STR_TO_UINT("WFCN")},	//WIFI连接成功
	{SOUND_WIFI_CONNECTING,				STR_TO_UINT("WFLZ"),	STR_TO_UINT("WFCG")},	//WIFI连接中
	{SOUND_WIFI_FAIL,		      		STR_TO_UINT("WFSB"),	STR_TO_UINT("WFFL")},	//WIFI连接失败
	{SOUND_COLLECT_SUCC,		  		STR_TO_UINT("SCCG"),	STR_TO_UINT("SCSC")},	//WIFI收藏成功
	{SOUND_PLAY_COLLECT,		  		STR_TO_UINT("BFSC"),	STR_TO_UINT("BFCE")},	//WIFI播放收藏
	{SOUND_ADD_SUCCEED,					STR_TO_UINT("ADCG"),	STR_TO_UINT("ADSC")},	//添加成功
	{SOUND_WELCOME,					  	STR_TO_UINT("HYHL"),	STR_TO_UINT("WECM")},	//欢迎回来
	{SOUND_UPDATE_SUCC,					STR_TO_UINT("GXWC"),	STR_TO_UINT("UPSC")},	//更新完成
	{SOUND_UPDATING,					STR_TO_UINT("GX Z"),	STR_TO_UINT("UPGR")},	//更新中
	{SOUND_REST_FACTROY,				STR_TO_UINT("HFCC"),	STR_TO_UINT("REFC")},	//恢复出厂设置
	{SOUND_SYNC_PLAYER,			  		STR_TO_UINT("TBBF"),	STR_TO_UINT("SYNC")},	//同步播放
	{SOUND_ALONE_PLAYER,				STR_TO_UINT("DLBF"),	STR_TO_UINT("ALOE")},	//独立播放
	{SOUND_LEFT_PLAYER,			  		STR_TO_UINT("ZDBF"),	STR_TO_UINT("LEPY")},	//左声道播放
	{SOUND_RIGHT_PLAYER,				STR_TO_UINT("YDBF"),	STR_TO_UINT("RIPY")},	//右声道播放
	{SOUND_STEREO_PLAYER,				STR_TO_UINT("LTBF"),	STR_TO_UINT("STPY")},	//立体声播放
	{SOUND_MAX_VOL,           			STR_TO_UINT("ZDYL"),  	STR_TO_UINT("ZDYL")},   //最大音量
	{SOUND_MIN_VOL,           			STR_TO_UINT("ZXYL"),  	STR_TO_UINT("ZXYL")},   //最小音量
	{SOUND_LOCK_ON,           			STR_TO_UINT("TSON"),  	STR_TO_UINT("LCON")},   //童锁打开
	{SOUND_ALARM_RING1,				    STR_TO_UINT("NZS1"),	STR_TO_UINT("NZS1")},	//50
	{SOUND_ALARM_RING2,			    	STR_TO_UINT("NZS2"),	STR_TO_UINT("NZS2")},	//100
	{SOUND_ALARM_RING3,			    	STR_TO_UINT("NZS3"),	STR_TO_UINT("NZS3")},	//150
	{SOUND_ALARM_RING4,			    	STR_TO_UINT("NZS4"),	STR_TO_UINT("NZS4")},	//200  	
	{SOUND_ALARM_RING5,			    	STR_TO_UINT("NZS5"),	STR_TO_UINT("NZS5")},	//200  	
	{SOUND_SLEEP_RING,			    	STR_TO_UINT("XMSY"),	STR_TO_UINT("SLPY")},	//开机提示音8	
};

typedef struct _SOUND_REMIND_CONTROL
{
	bool     IsRunning;

	uint32_t ConstDataAddr;
	uint32_t ConstDataSize;
	uint32_t ConstDataOffset;

} SOUND_REMIND_CONTROL;

static SOUND_REMIND_CONTROL SoundRemindControl;
static MemHandle SoundRemindFile;		//提示音文件
static uint8_t SoundRemindFileBuf[256];	//提示音文件缓冲区

void SoundRemindInit(void)
{
	memset(&SoundRemindControl, 0x00, sizeof(SoundRemindControl));
	memset(&SoundRemindFile, 0x00, sizeof(SoundRemindFile));
}

uint32_t SoundRemindFillStreamCallback(void *buffer, uint32_t length)
{
    int32_t RemainBytes = SoundRemindControl.ConstDataSize - SoundRemindControl.ConstDataOffset;
    int32_t ReadBytes   = length > RemainBytes ? RemainBytes : length;

    if(ReadBytes == 0)
	{
		return 0;	//此次不加载数据
	}
	
    if(SpiFlashConstDataRead((uint8_t*)buffer,
	                         ReadBytes,
	                         SoundRemindControl.ConstDataAddr + SoundRemindControl.ConstDataOffset) < 0)
	{
		ReadBytes = 0;
        APP_DBG("read const data error!\r\n");
	}

	SoundRemindControl.ConstDataOffset += ReadBytes;

	return ReadBytes;
}

/////////////////////////////////////////////////////////////////////////
// stream 缓存数据填充（decoder task每次解帧时调用）
// return -1 -- stream over; 0 -- no fill; other value -- filled bytes
int32_t SoundRemindFillStream(void)
{
	int32_t FillBytes;	//本次要加载的数据长度
	int32_t LeftBytes;	//文件缓冲区中已有的数据长度

	LeftBytes = (SoundRemindFile.mem_len - SoundRemindFile.p);
	FillBytes = SoundRemindControl.ConstDataSize - SoundRemindControl.ConstDataOffset;
	if(FillBytes > SoundRemindFile.mem_capacity - LeftBytes)
	{
		FillBytes = SoundRemindFile.mem_capacity - LeftBytes;
	}

	if((LeftBytes > 0) && (SoundRemindFile.p == 0))
	{
		return 0;	//还有数据，并且数据没有消耗
	} 

	if(FillBytes == 0)
	{
		return -1;	//播放结束
	}

	memcpy(SoundRemindFile.addr, SoundRemindFile.addr + SoundRemindFile.p, LeftBytes);
	if(SpiFlashConstDataRead((uint8_t*)(SoundRemindFile.addr + LeftBytes),
	                         FillBytes,
	                         SoundRemindControl.ConstDataAddr + SoundRemindControl.ConstDataOffset) < 0)
	{
		APP_DBG("read const data error!\n");
	}
		
	SoundRemindFile.p = 0;
	SoundRemindFile.mem_len = LeftBytes + FillBytes;
	SoundRemindControl.ConstDataOffset += FillBytes;

	return FillBytes;
}

//阻塞播放提示音
//注意：该函数必须在main task中调用，否则会有重入问题
void SoundRemind(uint16_t SoundId)
{     
	uint16_t i;
	uint32_t ConstDataId;

#ifdef FUNC_RADIO_DIV_CLK_EN	
	uint8_t DivFreqFlag = 0;//升降频相关
//	uint8_t SysClkSel;
	uint8_t SysClkDivNum;
#endif
	//wifi语音中禁止播放提示音。
	if(!gSys.SoundRemindOn || ((gWiFi.MicState != WIFI_AVS_STATUS_IDLE) && (gSys.CurModuleID == MODULE_ID_WIFI))
		|| WiFiFirmwareUpgradeStateGet())
	{
		return;
	}
#ifdef FUNC_RECORD_EN
	if(gSys.CurModuleID == MODULE_ID_RECORDER && (RecordState == RECORD_STATE_REC || RecordState == RECORD_STATE_REC_PAUSE))
	{
		return;
	}
#endif
	if(SoundRemindControl.IsRunning)
	{
		return;
	}
	APP_DBG("SoundRemind id : %d\n", SoundId);

	SoundRemindControl.IsRunning = 1;
	SoundRemindControl.ConstDataSize = 0;
	SoundRemindControl.ConstDataAddr = 0;
	SoundRemindControl.ConstDataOffset = 0;
	
	SoundRemindFile.addr = SoundRemindFileBuf;
	SoundRemindFile.mem_capacity = sizeof(SoundRemindFileBuf);    // 流数据缓存大小
	SoundRemindFile.mem_len = 0;
	SoundRemindFile.p = 0;

	//通过SoundId查找对应的ConstDataId
	for(i = 0; i < sizeof(SoundTable)/sizeof(SOUND_TABLE_ITEM); i++)
	{
		if(SoundTable[i].Id == SoundId)
		{
			ConstDataId = (gSys.LanguageMode == LANG_ZH) ? SoundTable[i].IdChn : SoundTable[i].IdEng; 
			break;	//找到
		}
	}	
	if(i >= sizeof(SoundTable)/sizeof(SOUND_TABLE_ITEM))
	{
		APP_DBG("Can not get the SoundId : %d\n", SoundId);
		SoundRemindControl.IsRunning = 0;
		return;		//未找到
	}
	
	if(SpiFlashConstGetInfo(ConstDataId, &SoundRemindControl.ConstDataSize, &SoundRemindControl.ConstDataAddr) < 0)
	{
		APP_DBG("Get const data info error!\n");
		SoundRemindControl.IsRunning = 0;
		return;
	}

#ifdef FUNC_BT_HF_EN
	if(GetHfModeState() == BT_HF_MODE_STATE_ENTERING)	
	{
		SoundRemindControl.IsRunning = 0;
		return;
	}
#endif

#ifdef FUNC_RADIO_DIV_CLK_EN
	if(0 == gSys.IsSysClk96MHz)
	{
		OSTaskSuspend(AudioProcessHandle);
//		SysClkSel = ClkSysClkSelGet();//1:80M ,  0:96M  //?????RC?????
		SysClkDivNum = ClkModuleDivGet();
//		ClkSysClkSel(0); // 切换到96M
//		SysClkDivison(1);	//ljg, call directly
		SysClkSet(0, 1);
        gSys.IsSysClk96MHz = 1; 
		DivFreqFlag = 1;
	}
#endif
	SoundRemindFillStream();

	// 检查是否有指定的消息
	if(0)//IsMsgInQueue())
	{
		SoundRemindControl.IsRunning = 0;
        //resume the audio task before returning to deal message -Robert 20150204
		OSTaskResume(AudioProcessHandle);
		APP_DBG("指定消息产生，退出提示音播放!\n");
		return;
	}

	if(gSys.CurModuleID == MODULE_ID_BLUETOOTH)
	{
		BTAudioPlayStop();
		WaitMs(100);
		DecoderTaskSyncPlay(&SoundRemindFile, IO_TYPE_MEMORY);
		BTAudioPlayStart();
	}
	else
	{
#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)
		UsbAudioPlayFlag = FALSE;
#endif
	 	MixerMute(MIXER_SOURCE_ANA_MONO);
		MixerMute(MIXER_SOURCE_ANA_STERO);
		DecoderTaskSyncPlay(&SoundRemindFile, IO_TYPE_MEMORY);
		MixerUnmute(MIXER_SOURCE_ANA_MONO);
		MixerUnmute(MIXER_SOURCE_ANA_STERO);
#if defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN)		
		UsbAudioPlayFlag = TRUE;
#endif
	}

#ifdef FUNC_RADIO_DIV_CLK_EN
	if(DivFreqFlag)//恢复原频率
	{
/*
		if(1 == SysClkSel)
		{
			ClkSysClkSel(1); 
		}
		if(SysClkDivNum > 1)
		{
			SysClkDivison(SysClkDivNum);	//ljg, call directly
		}
*/
		SysClkSet(1, SysClkDivNum);
		gSys.IsSysClk96MHz = 0;  
		OSTaskResume(AudioProcessHandle);
	}
#endif
	SoundRemindControl.IsRunning = 0;
	ResetSoundRemindStopFlag();

#ifdef FUNC_AUDIO_VB_EFFECT_EN
	if(gSys.AudioVbEffectFlag)
	{
		gVbEffectInited = FALSE;
		InitVb3dPcmPara();
	}
#endif
}

bool IsSoundRemindPlaying(void)
{
	return SoundRemindControl.IsRunning;
}

void StopSoundRemind(void)
{
	APP_DBG("StopSoundRemind---\n");
	InterruptSoundRemind = TRUE;
}

void ResetSoundRemindStopFlag(void)
{
	InterruptSoundRemind = FALSE;
}

bool IsMsgInQueue(void)
{
	bool	ret = FALSE;
	
	if(MsgCheck(MSG_USB_PLUGIN)
	|| MsgCheck(MSG_USB_PLUGOUT)
	|| MsgCheck(MSG_SD_PLUGIN)
	|| MsgCheck(MSG_SD_PLUGOUT)
	|| MsgCheck(MSG_LINEIN_PLUGIN)
	|| MsgCheck(MSG_LINEIN_PLUGOUT)
	|| MsgCheck(MSG_MIC_PLUGIN)
	|| MsgCheck(MSG_MIC_PLUGOUT)
	|| MsgCheck(MSG_PC_PLUGIN)
	|| MsgCheck(MSG_PC_PLUGOUT)	
#ifdef FUNC_BT_HF_EN
	|| MsgCheck(MSG_BT_HF_OUT_MODE) 
	|| MsgCheck(MSG_BT_HF_INTO_MODE)
#endif
	) {
		ret = TRUE;
	}

	return ret;
}
#endif


