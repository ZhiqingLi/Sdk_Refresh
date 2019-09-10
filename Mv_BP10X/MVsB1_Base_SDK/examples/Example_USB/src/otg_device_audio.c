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
#include "otg_device_standard_request.h"
#include "audio_adc.h"
#include "dac.h"
#include "clk.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "mcu_circular_buf.h"
#include "otg_device_audio.h"

#ifdef CFG_APP_CONFIG
#include "resampler.h"
#include "app_config.h"
#include "audio_core_api.h"
#endif

#ifdef CFG_APP_USB_AUDIO_MODE_EN
#define AUDIO_MAX_VOLUME	999

extern MCU_CIRCULAR_CONTEXT SpeakerCircularBuf;
extern uint8_t Setup[];
extern uint8_t Request[];
	
UsbAudioSpeaker_ UsbAudioSpeaker;
UsbAudioSpeaker_ UsbAudioMic;

uint8_t iso_dac_buf[256];
uint8_t iso_adc_buf[256];

#ifdef CFG_APP_CONFIG
uint32_t UsbAduioSampleRate = CFG_PARA_SAMPLE_RATE;
uint32_t UsbAduioMicSampleRate = CFG_PARA_SAMPLE_RATE;

#ifdef CFG_FUNC_MIXER_SRC_EN
ResamplerContext*	ResamplerCt1;
int16_t* SRCOutBuf11;
#endif
#endif

extern void OTG_DeviceSendResp(uint16_t Resp, uint8_t n);
extern uint8_t UsbAudioSourceNum(void);

/**
 * @brief  USB声卡模式下，发送反向控制命令
 * @param  Cmd 反向控制命令
 * @return 1-成功，0-失败
 */
#define AUDIO_STOP        BIT(7) 
#define AUDIO_PP          BIT(6) 

#define AUDIO_MUTE        BIT(4)

#define AUDIO_NEXT        BIT(2) 
#define AUDIO_PREV        BIT(3) 

#define AUDIO_VOL_UP      BIT(0) 
#define AUDIO_VOL_DN      BIT(1)

/////////////////////////
void PCAudioStop(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_STOP);
}
void PCAudioPP(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_PP);
}
void PCAudioNext(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_NEXT);
}
void PCAudioPrev(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_PREV);
}

void PCAudioVolUp(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_VOL_UP);
}

void PCAudioVolDn(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_VOL_DN);
}

bool OTG_DeviceAudioSendPcCmd(uint8_t Cmd)
{
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	Cmd = 0;
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	return TRUE;
}

//转采样直接在中断中处理，转采样时间大约是180us。
//注意一下需要4字节对齐
void OnDeviceAudioRcvIsoPacket(void)
{
#ifdef CFG_FUNC_MIXER_SRC_EN
	int32_t SRCDoneLen; 			//SRC之后存放于outbuf的数据长度
	//int32_t MaxLen = MAX_FRAME_SAMPLES;
#endif
	uint32_t Len;
	OTG_DeviceISOReceive(DEVICE_ISO_OUT_EP, iso_dac_buf, 256, &Len);
#ifndef CFG_APP_CONFIG
	AudioDAC_DataSet(DAC0, iso_dac_buf, Len/4);
#else
#ifdef CFG_FUNC_MIXER_SRC_EN
	if(UsbAduioSampleRate != CFG_PARA_SAMPLE_RATE)
	{
		SRCDoneLen = resampler_apply(ResamplerCt1, (int16_t*)iso_dac_buf, (int16_t*)SRCOutBuf11, Len / 4);
		MCUCircular_PutData(&SpeakerCircularBuf, SRCOutBuf11, SRCDoneLen * 2 * 2);
	}
	else
	{
		MCUCircular_PutData(&SpeakerCircularBuf, iso_dac_buf,Len);
	}
#else
	MCUCircular_PutData(&SpeakerCircularBuf, iso_dac_buf,Len);
#endif
#endif
}

void OnDeviceAudioSendIsoPacket(void)
{
	uint32_t RealLen;
	RealLen = AudioADC_DataGet(ADC1_MODULE,iso_adc_buf, 48);//unit：sample
	RealLen *= 4;//转化为Byte
	OTG_DeviceISOSend(DEVICE_ISO_IN_EP,iso_adc_buf,RealLen);
}

#if 1
//声卡Speaker音量控制接口
bool     AudioMuteFlag = FALSE;
bool     AudioMicMuteFlag = FALSE;

uint32_t SpeakerSampleRateGet()
{
#ifdef CFG_FUNC_MIXER_SRC_EN
	return UsbAduioSampleRate;
#else
	return AudioDAC_SampleRateGet(DAC0);
#endif
}

uint32_t SpeakerSampleRateSet(uint32_t SampleRate)
{
#ifdef CFG_FUNC_MIXER_SRC_EN
	UsbAduioSampleRate = SampleRate;
#else
	//DBG("SampleRate = %d\n", SampleRate);
	if((SampleRate == 11025) || (SampleRate == 22050) || (SampleRate == 44100))
	{
		Clock_AudioMclkSel(AUDIO_DAC0, PLL_CLOCK1);
	}
	else
	{
		Clock_AudioMclkSel(AUDIO_DAC0, PLL_CLOCK2);
	}
    //digital
	AudioDAC_SampleRateSet(ADC0_MODULE, SampleRate);
#endif
	return 0;
}

uint32_t SpeakerMuteGet()
{
	return AudioMuteFlag;
}

uint32_t SpeakerMuteSet(uint32_t Mute)
{
	AudioMuteFlag = Mute;
#ifndef CFG_APP_CONFIG
	AudioDAC_SoftMute(DAC0, AudioMuteFlag, AudioMuteFlag);
#else
	if(AudioMuteFlag)
	{
		AudioCoreSinkMute(0, TRUE, TRUE);
	}
	else
	{
		AudioCoreSinkUnmute(0, TRUE, TRUE);
	}
#endif
	return 0;
}

uint32_t SpeakerLeftVolumeGet()
{
	uint16_t l,r;
	l = 0;
	r = 0;
#ifndef CFG_APP_CONFIG
	AudioDAC_VolGet(DAC0,&l,&r);
#else
	AudioCoreSinkVolGet(0, &l, &r);
#endif
	return l;
}

uint32_t SpeakerRightVolumeGet()
{
	uint16_t l,r;
	l = 0;
	r = 0;
#ifndef CFG_APP_CONFIG
	AudioDAC_VolGet(DAC0,&l,&r);
#else
	AudioCoreSinkVolGet(0, &l, &r);
#endif
	return r;
}

uint32_t SpeakerLeftVolumeSet(uint32_t Vol)
{
	uint16_t l,r;
	l = 0;
	r = 0;
#ifndef CFG_APP_CONFIG
	AudioDAC_VolGet(DAC0,&l,&r);
	l = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioDAC_VolSet(DAC0,l,r);
#else
	AudioCoreSinkVolGet(0, &l, &r);
	l = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioCoreSinkVolSet(0, l, r);
#endif
	return 0;
}

uint32_t SpeakerRigthVolumeSet(uint32_t Vol)
{
	uint16_t l,r;
	l = 0;
	r = 0;
#ifndef CFG_APP_CONFIG
	AudioDAC_VolGet(DAC0,&l,&r);
	r = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioDAC_VolSet(DAC0,l,r);
#else
	AudioCoreSinkVolGet(0, &l, &r);
	r = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioCoreSinkVolSet(0, l, r);
#endif
	return 0;
}

uint32_t MicSampleRateGet()
{
#ifdef CFG_FUNC_MIXER_SRC_EN
	return UsbAduioMicSampleRate;
#else
	return AudioADC_SampleRateGet(AUDIO_ADC1);
#endif
}

uint32_t MicSampleRateSet(uint32_t SampleRate)
{
#ifdef CFG_FUNC_MIXER_SRC_EN
	UsbAduioMicSampleRate = SampleRate;
#else
	if((SampleRate == 11025) || (SampleRate == 22050) || (SampleRate == 44100))
	{
		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK1);
	}
	else
	{
		Clock_AudioMclkSel(AUDIO_ADC1, PLL_CLOCK2);
	}
	AudioADC_SampleRateSet(AUDIO_ADC1, SampleRate);
#endif
	return 0;
}

uint32_t MicMuteGet()
{
	return AudioMicMuteFlag;
}

uint32_t MicMuteSet(uint32_t mute)
{
	AudioMicMuteFlag = mute;
#ifndef CFG_APP_CONFIG
	AudioADC_SoftMute(AUDIO_ADC1, mute, mute);
#else
	if(AudioMicMuteFlag)
	{
		AudioCoreSourceMute(UsbAudioSourceNum(), TRUE, TRUE);
	}
	else
	{
		AudioCoreSourceUnmute(UsbAudioSourceNum(), TRUE, TRUE);
	}
#endif
	return 0;
}

uint32_t MicLeftVolumeGet()
{
	uint16_t l,r;
	l=0;
	r=0;
#ifndef CFG_APP_CONFIG
	AudioADC_VolGet(AUDIO_ADC1,&l,&r);
#else
	AudioCoreSourceVolGet(UsbAudioSourceNum(), &l, &r);
#endif
	return l;
}

uint32_t MicLeftVolumeSet(uint32_t Vol)
{
#ifndef CFG_APP_CONFIG
	AudioADC_VolSetChannel(AUDIO_ADC1,CHANNEL_LEFT,(Vol*0xFFF)/AUDIO_MAX_VOLUME);
#else
	uint16_t l,r;
	l = 0;
	r = 0;
	AudioCoreSourceVolGet(UsbAudioSourceNum(), &l, &r);
	l = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioCoreSourceVolSet(UsbAudioSourceNum(), l, r);
#endif
	return 0;
}

uint32_t MicRightVolumeGet()
{
	uint16_t l,r;
	l=0;
	r=0;
#ifndef CFG_APP_CONFIG
	AudioADC_VolGet(AUDIO_ADC1,&l,&r);
#else
	AudioCoreSourceVolGet(UsbAudioSourceNum(), &l, &r);
#endif
	return r;
}

uint32_t MicRigthVolumeSet(uint32_t Vol)
{
#ifndef CFG_APP_CONFIG
	AudioADC_VolSetChannel(AUDIO_ADC1,CHANNEL_RIGHT,(Vol*0xFFF)/AUDIO_MAX_VOLUME);
#else
	uint16_t l,r;
	l = 0;
	r = 0;
	AudioCoreSourceVolGet(UsbAudioSourceNum(), &l, &r);
	r = (Vol*0xFFF)/AUDIO_MAX_VOLUME;
	AudioCoreSourceVolSet(UsbAudioSourceNum(), l, r);
#endif
	return 0;
}
#endif

void OTG_DeviceAudioInit()
{
	UsbAudioSpeaker.FuncLeftVolSet = SpeakerLeftVolumeSet;
	UsbAudioSpeaker.FuncLeftVolGet = SpeakerLeftVolumeGet;
	UsbAudioSpeaker.FuncRightVolSet = SpeakerRigthVolumeSet;
	UsbAudioSpeaker.FuncRightVolGet = SpeakerRightVolumeGet;
	UsbAudioSpeaker.FuncMuteGet = SpeakerMuteGet;
	UsbAudioSpeaker.FuncMuteSet = SpeakerMuteSet;
	UsbAudioSpeaker.FuncSampleRateGet = SpeakerSampleRateGet;
	UsbAudioSpeaker.FuncSampleRateSet = SpeakerSampleRateSet;

	UsbAudioMic.FuncLeftVolSet = MicLeftVolumeSet;
	UsbAudioMic.FuncLeftVolGet = MicLeftVolumeGet;
	UsbAudioMic.FuncRightVolSet = MicRigthVolumeSet;
	UsbAudioMic.FuncRightVolGet = MicRightVolumeGet;
	UsbAudioMic.FuncMuteGet = MicMuteGet;
	UsbAudioMic.FuncMuteSet = MicMuteSet;
	UsbAudioMic.FuncSampleRateGet = MicSampleRateGet;
	UsbAudioMic.FuncSampleRateSet = MicSampleRateSet;

	UsbAudioSpeaker.AltSet = 0;
	UsbAudioSpeaker.FuncLeftVolSet(AUDIO_MAX_VOLUME / 10);
	UsbAudioSpeaker.FuncRightVolSet(AUDIO_MAX_VOLUME / 10);
	UsbAudioSpeaker.FuncMuteSet(0);
	UsbAudioSpeaker.FuncSampleRateSet(44100);

	UsbAudioMic.AltSet = 0;
	UsbAudioMic.FuncLeftVolSet(AUDIO_MAX_VOLUME);
	UsbAudioMic.FuncRightVolSet(AUDIO_MAX_VOLUME);
	UsbAudioMic.FuncMuteSet(0);
	UsbAudioMic.FuncSampleRateSet(44100);
}

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
	#define GET_CUR_EP	0xA281
	
	//AUDIO类请求处理
	if(AudioCmd == SET_CUR_EP)
	{
		OTG_DBG("Set samplerate\n");
		if(Setup[4] == 0x84)
		{
			UsbAudioMic.FuncSampleRateSet(Request[1]*256 + Request[0]);
		}
		else
		{
			UsbAudioSpeaker.FuncSampleRateSet(Request[1]*256 + Request[0]);
		}
		return;
	}
	if(AudioCmd == GET_CUR_EP)
	{
		uint32_t Temp = 0;
		OTG_DBG("Get samplerate\n");
		if(Setup[4] == 0x84)
		{
			Temp = UsbAudioMic.FuncSampleRateGet();
		}
		else
		{
			Temp =UsbAudioSpeaker.FuncSampleRateGet();
		}
		Setup[0] = (Temp>>0 ) & 0x000000FF;
		Setup[1] = (Temp>>8 ) & 0x000000FF;
		Setup[2] = (Temp>>16) & 0x000000FF;
		OTG_DeviceControlSend(Setup,3,3);
		return;
	}

	if((Entity == AUDIO_SPEAKER_FU_ID) && (Control == 0x01))
	{
		//Speaker mute的操作
		if(AudioCmd == GET_CUR)
		{
			Setup[0] = UsbAudioSpeaker.FuncMuteGet();
			OTG_DeviceControlSend(Setup,1,3);
		}
		else if(AudioCmd == SET_CUR)
		{
			OTG_DBG("Set speaker mute: %d\n", Request[0]);
			UsbAudioSpeaker.FuncMuteSet(Request[0]);
		}
		else
		{
			OTG_DBG("%s %d\n",__FILE__,__LINE__);
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
			uint32_t Vol = 0;
			if(Channel == 0x01)
			{
				Vol = UsbAudioSpeaker.FuncLeftVolGet();
			}
			else
			{
				Vol = UsbAudioSpeaker.FuncRightVolGet();
			}
			OTG_DeviceSendResp(Vol, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			uint32_t Temp = 0;
			DBG("Request[1] = %d, Request[0] = %d\n", Request[1], Request[0]);
			Temp = (Request[1] & 0x7F) * 256 + Request[0];
			DBG("Setup[2] = %d, Temp = %d\n", Setup[2], Temp);
			if(Setup[2] == 0x01)
			{
				UsbAudioSpeaker.FuncLeftVolSet(Temp);
			}
			else
			{
				UsbAudioSpeaker.FuncRightVolSet(Temp);
			}
		}
		else
		{
			OTG_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if((Entity == AUDIO_MIC_FU_ID) && (Control == 0x01))
	{
		//Mic mute的操作
		if(AudioCmd == GET_CUR)
		{
			uint32_t AudioMicMuteFlag;
			AudioMicMuteFlag = UsbAudioMic.FuncMuteGet();
			OTG_DeviceSendResp(AudioMicMuteFlag, 1);
			OTG_DBG("Get mic mute: %d\n", (int)AudioMicMuteFlag);
		}
		else if(AudioCmd == SET_CUR)
		{
			OTG_DBG("Set mic mute: %d\n", Request[0]);
			UsbAudioMic.FuncLeftVolSet(Request[0]);
		}
		else
		{
			OTG_DBG("%s %d\n",__FILE__,__LINE__);
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
			uint32_t Vol = 0;
			if(Channel == 0x01)
			{
				Vol = UsbAudioMic.FuncLeftVolGet();
			}
			else
			{
				Vol = UsbAudioMic.FuncRightVolGet();
			}
			OTG_DeviceSendResp(Vol*4, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			uint32_t Vol = ((Request[1] & 0x7F) * 256 + Request[0])/4;;
			if(Setup[2] == 0x01)
			{
				UsbAudioMic.FuncLeftVolSet(Vol);
			}
			else
			{
				UsbAudioMic.FuncRightVolSet(Vol);
			}
		}
		else
		{
			OTG_DBG("%s %d\n",__FILE__,__LINE__);
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
		else
		{
			OTG_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if(AudioCmd == SET_IDLE)
	{
		OTG_DBG("Set idle\n");
	}	
	else
	{
		//其他AUDIO类的输入请求
		OTG_DBG("Unkown command! %02X %02X %02X %02X %02X %02X %02X %02X\n", Setup[0], Setup[1], Setup[2], Setup[3], Setup[4], Setup[5], Setup[6], Setup[7]);
		OTG_DeviceSendResp(0x0000, 1);
	}
}

#endif

