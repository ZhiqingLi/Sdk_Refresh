/**
 *****************************************************************************
 * @file     otg_device_audio.c
 * @author   Owen
 * @version  V1.0.0
 * @date     7-September-2015
 * @brief    device audio module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#include <string.h>
#include "type.h"
#include "otg_device_hcd.h"
#include "debug.h"
#include "otg_device_audio.h"

extern int kprintf(const char *fmt, ...);
#define	OTG_DBG(format, ...)		printf(format, ##__VA_ARGS__)

extern void OTG_DeviceSendResp(uint16_t Resp, uint8_t n);
/**
 * @brief  USB声卡模式下，发送反向控制命令
 * @param  Cmd 反向控制命令
 * @return 1-成功，0-失败
 */
bool OTG_DeviceAudioSendPcCmd(uint8_t Cmd)
{
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	Cmd = 0;
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	return TRUE;
}

#define AUDIO_MAX_VOLUME	999
//声卡Speaker音量控制接口
uint16_t AudioLeftVolume = AUDIO_MAX_VOLUME / 10;
uint16_t AudioRightVolume = AUDIO_MAX_VOLUME / 10;
bool     AudioMuteFlag = FALSE;

uint16_t AudioMicLeftVolume = AUDIO_MAX_VOLUME;
uint16_t AudioMicRightVolume = AUDIO_MAX_VOLUME;
bool     AudioMicMuteFlag = FALSE;
extern uint8_t Setup[];
extern uint8_t Request[];
void OTG_DeviceAudioRequest(void)
{
	//AUDIO控制接口组件ID号定义（必须与device_stor_audio_request.c中的定义保持一致！）
	#define AUDIO_SPEAKER_IT_ID		1
	#define AUDIO_SPEAKER_FU_ID		2	//控制MUTE、VOLUME
	#define AUDIO_SPEAKER_OT_ID		3
	#define AUDIO_MIC_IT_ID			4
	#define AUDIO_MIC_FU_ID			5
	#define AUDIO_MIC_SL_ID			6
	#define AUDIO_MIC_OT_ID			7
	
	#define AudioCmd	((Setup[0] << 8) | Setup[1])
	#define Channel		Setup[2]
	#define Control		Setup[3]
	#define Entity		Setup[5]
	
	#define SET_CUR		0x2101
	#define SET_IDLE	0x210A
	#define GET_CUR		0xA181
	#define GET_MIN		0xA182
	#define GET_MAX		0xA183
	#define GET_RES		0xA184
	
	#define SET_CUR_EP	0x2201
	
	//AUDIO类请求处理
	if((Entity == AUDIO_SPEAKER_FU_ID) && (Control == 0x01))
	{
		//Speaker mute的操作
		if(AudioCmd == GET_CUR)
		{
			OTG_DBG("Get speaker mute: %d\n", AudioMuteFlag);
			OTG_DeviceSendResp(AudioMuteFlag, 1);
		}
		else if(AudioCmd == SET_CUR)
		{
			OTG_DBG("Set speaker mute: %d\n", Request[0]);
			if(Request[0] == 0x01)
			{
				AudioMuteFlag = TRUE;
				//gFpDeviceAudioMuteEn();
			}
			else
			{
				AudioMuteFlag = FALSE;
				//gFpDeviceAudioMuteDis();
			}
		}
	}
	else if((Entity == AUDIO_SPEAKER_FU_ID) && (Control == 0x02))
	{
		//Speaker volume的操作
		if(AudioCmd == GET_MIN)
		{
			OTG_DBG("Get speaker min volume\n");
			OTG_DeviceSendResp(0x0000, 2);
		}
		else if(AudioCmd == GET_MAX)
		{
			OTG_DBG("Get speaker max volume\n");
			OTG_DeviceSendResp(AUDIO_MAX_VOLUME, 2);
		}
		else if(AudioCmd == GET_RES)
		{
			OTG_DBG("Get speaker res volume\n");
			OTG_DeviceSendResp(0x0001, 2);
		}
		else if(AudioCmd == GET_CUR)
		{
			OTG_DBG("Get speaker cur volume: %d %d\n", AudioLeftVolume, AudioRightVolume);
			OTG_DeviceSendResp((Channel == 0x01) ? AudioLeftVolume : AudioRightVolume, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			if(Setup[2] == 0x01)
			{
				AudioLeftVolume = (Request[1] & 0x7F) * 256 + Request[0];		//left volume
			}
			else
			{
				AudioRightVolume = (Request[1] & 0x7F) * 256 + Request[0];		//right volume
			}
			OTG_DBG("Set speaker cur volume %d %d\n", AudioLeftVolume, AudioRightVolume);
			//gFpDeviceAudioSetVolume();
		}
	}
	else if((Entity == AUDIO_MIC_FU_ID) && (Control == 0x01))
	{
		//Mic mute的操作
		if(AudioCmd == GET_CUR)
		{
			OTG_DBG("Get mic mute: %d\n", AudioMicMuteFlag);
			OTG_DeviceSendResp(AudioMicMuteFlag, 1);
		}
		else if(AudioCmd == SET_CUR)
		{
			OTG_DBG("Set mic mute: %d\n", Request[0]);
			if(Request[0] == 0x01)
			{
				AudioMicMuteFlag = TRUE;
//				gFpDeviceAudioMicMuteEn();
			}
			else
			{
				AudioMicMuteFlag = FALSE;
//				gFpDeviceAudioMicMuteDis();
			}
		}
	}
	else if((Entity == AUDIO_MIC_FU_ID) && (Control == 0x02))
	{
		//Mic volume的操作
		if(AudioCmd == GET_MIN)
		{
			OTG_DBG("Get mic min volume\n");
			OTG_DeviceSendResp(0x0000, 2);
		}
		else if(AudioCmd == GET_MAX)
		{
			OTG_DBG("Get mic max volume\n");
			OTG_DeviceSendResp(AUDIO_MAX_VOLUME*4, 2);	//此处乘以4的原因请看本文件开头的注释说明
		}
		else if(AudioCmd == GET_RES)
		{
			OTG_DBG("Get mic res volume\n");
			OTG_DeviceSendResp(0x0001, 2);
		}
		else if(AudioCmd == GET_CUR)
		{
			OTG_DBG("Get mic cur volume: %d %d\n", AudioMicLeftVolume, AudioMicRightVolume);
			OTG_DeviceSendResp((Channel == 0x01) ? AudioMicLeftVolume*4 : AudioMicRightVolume*4, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			if(Setup[2] == 0x01)
			{
				AudioMicLeftVolume = ((Request[1] & 0x7F) * 256 + Request[0])/4;		//left volume
			}
			else
			{
				AudioMicRightVolume = ((Request[1] & 0x7F) * 256 + Request[0])/4;		//right volume
			}
			OTG_DBG("Set mic cur volume %d %d\n", AudioMicLeftVolume, AudioMicRightVolume);
//			gFpDeviceAudioMicSetVolume();
		}
	}
	else if(Entity == AUDIO_MIC_SL_ID)
	{
		//Selector的操作
		if(AudioCmd == GET_CUR)
		{
			OTG_DBG("Get selector: 1\n");
			OTG_DeviceSendResp(0x01, 1);
		}
	}
	else if(AudioCmd == SET_IDLE)
	{
		OTG_DBG("Set idle\n");
	}
	else if(AudioCmd == SET_CUR_EP)
	{
		OTG_DBG("Set samplerate\n");
	}	
	else
	{
		//其他AUDIO类的输入请求
		OTG_DBG("Unkown command! %02X %02X %02X %02X %02X %02X %02X %02X\n", Setup[0], Setup[1], Setup[2], Setup[3], Setup[4], Setup[5], Setup[6], Setup[7]);
		OTG_DeviceSendResp(0x0000, 1);
	}
}

