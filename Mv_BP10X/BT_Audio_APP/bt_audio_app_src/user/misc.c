/*
 **************************************************************************************
 * @file    misc.c
 * @brief    
 * 
 * @author  
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "remap.h"
#include "spi_flash.h"
#include "gpio.h"
#include "uarts.h"
#include "irqn.h"
#include "uarts_interface.h"
#include "timeout.h"
#include "debug.h"
#include "watchdog.h"
//#include "config.h"
#include "clk.h"
#include "audio_adc.h"
#include "dac.h"
#include "adc.h"
#include "delay.h"
#include "ctrlvars.h"
#include "app_config.h"
#include "adc_interface.h"
#include "dac_interface.h"
//#include "key_process.h"
#include "audio_effect.h"
#include "chip_info.h"
#include "otg_device_stor.h"
#include "otg_device_standard_request.h"
//#include "power_management.h"
#include "audio_adc.h"
//#include "audio_effect_interface.h"
//#include "key_comm.h"
#include "hw_interface.h"
#include "misc.h"
#include "audio_encryption.h"
#include "i2s.h"
#include "timer.h"
#if CFG_REMINDSOUND_EN
//#include "sound_remind.h"
#endif

#if (defined(CFG_FUNC_DETECT_MIC_EN) || defined(CFG_FUNC_DETECT_PHONE_EN))
static TIMER MuteOffTimer;
#endif
/*
****************************************************************
*常用应用处理函数列表，用户可在此扩展应用
*
*
****************************************************************
*/
void (*HWdeviceList[3])(void)={
     //LedDisplay,
     DetectEarPhone,
     DetectMic,
     DetectMic3Or4Line,
     //DetectLineIn,
     //PowerMonitor,
     //PowerProcess,
     //DACVolumeSmoothProcess,
     //MicVolSmoothProcess, 
};
/*
****************************************************************
*
*
*
****************************************************************
*/
void HWDeviceDected_Init(void)
{
	DetectEarPhone();
	
	DetectMic();
	
	DetectMic3Or4Line();
	
	#if CFG_DMA_GPIO_SIMULATE_PWM_EN
	DmaTimerCircleMode_GPIO_Init();
	DmaTimerCircleMode_GPIO_Config(50, 70, 90);
	#endif

	#if CFG_HW_PWM_EN
	HwPwmInit();
	HwPwmConfig(50);
	#endif
}
/*
****************************************************************
*常用应用处理主循环
*
*
****************************************************************
*/
void HWDeviceDected(void)
{
   ///////此函数仅调用对时间不作要求的功能////要求偶数次定时器//
    #define  HW_DECTED_DLY	 2
  	static TIMER HWDeviceTimer;
	static uint8_t HWDeviceInit=0;
	static uint8_t HWDeviceConut;
    
	if(HWDeviceInit==0)
	{
		HWDeviceInit = 1;

		HWDeviceConut = 0;

		TimeOutSet(&HWDeviceTimer, 0); 

		return ;
	}
	
    ShunningModeProcess();//闪避功能处理
    
	if(!IsTimeOut(&HWDeviceTimer)) return;

    TimeOutSet(&HWDeviceTimer, HW_DECTED_DLY); 

    #if 0
	if(gCtrlVars.UsbRevTimerout)////
	{
		gCtrlVars.UsbRevTimerout--;
	}
	if(gCtrlVars.RemaindTimerout)////
	{
		gCtrlVars.RemaindTimerout--;
	}	 
	if(gCtrlVars.I2s0Timerout)////
	{
		gCtrlVars.I2s0Timerout--;
	}
	if(gCtrlVars.I2s1Timerout)////
	{
		gCtrlVars.I2s1Timerout--;
	}
	if(gCtrlVars.SpdifTimerout)////
	{
		gCtrlVars.SpdifTimerout--;
	}
	#endif
    HWDeviceConut++;

    //HWDeviceConut &= 0x8;/////8 *2ms = 16ms
    if(HWDeviceConut > 2)
	{
		HWDeviceConut = 0;
	}
    (*HWdeviceList[HWDeviceConut])();
}
/*
****************************************************************
* MIC插拔检测处理demo
*
*
****************************************************************
*/
void DetectMic(void)
{
	
#ifdef CFG_FUNC_DETECT_MIC_EN
    #define MIC2_EN  1///for 3or4 line jack detecd.....
    //---------mic1 var--通用三节-----------------//
	static uint8_t MicInit=0;
	static uint8_t MicCnt = 0;
	//--------mic2 var-------------------///
	#if MIC2_EN
	static uint8_t Mic2Cnt = 0;
	#endif	
	uint8_t val;
	uint32_t msg;

	if(!MicInit)
	{
		MicInit = 1;
		MicCnt = 0;

		gCtrlVars.MicOnlin = 0;////default offline
		GPIO_RegOneBitClear(MIC1_DETECT_OE, MIC1_DETECT_PIN);
		GPIO_RegOneBitSet(MIC1_DETECT_IE, MIC1_DETECT_PIN);
		///PULL enable
		GPIO_RegOneBitSet(MIC1_DETECT_PU, MIC1_DETECT_PIN);
		GPIO_RegOneBitClear(MIC1_DETECT_PD, MIC1_DETECT_PIN);	
		DelayUs(50);
		if(GPIO_RegOneBitGet(MIC1_DETECT_IN,MIC1_DETECT_PIN))
		{
			gCtrlVars.MicOnlin = 1;
			DBG("MIC 1 IN\n");
			TimeOutSet(&MuteOffTimer, 1000); 
		}
		////////mic2 detected/////////////////////
        #if MIC2_EN
		Mic2Cnt = 0;
		gCtrlVars.Mic2Onlin = 0;////default offline
		GPIO_RegOneBitClear(MIC2_DETECT_OE, MIC2_DETECT_PIN);
		GPIO_RegOneBitSet(MIC2_DETECT_IE, MIC2_DETECT_PIN);
		///PULL enable
		GPIO_RegOneBitSet(MIC2_DETECT_PU, MIC2_DETECT_PIN);
		GPIO_RegOneBitClear(MIC2_DETECT_PD, MIC2_DETECT_PIN); 
		DelayUs(50);
		if(!GPIO_RegOneBitGet(MIC2_DETECT_IN,MIC2_DETECT_PIN))
		{
			gCtrlVars.Mic2Onlin = 1;	 
			DBG("MIC 2 IN\n");
			TimeOutSet(&MuteOffTimer, 1000); 
		}
        #endif
	}
	else
	{
		//------mic1检测处理--------------------------------------------------//
		if(GPIO_RegOneBitGet(MIC1_DETECT_IN,MIC1_DETECT_PIN))
		{
			if(++MicCnt > 50)
			{
				MicCnt = 0;
				if(gCtrlVars.MicOnlin ==0  )
				{
					gCtrlVars.MicOnlin = 1;	
					DBG("MIC 1 IN\n");
					MUTE_ON();
					TimeOutSet(&MuteOffTimer, 1000); 
				} 			
			}
		}
		else
		{
			MicCnt = 0;
			if(gCtrlVars.MicOnlin )
			{
				gCtrlVars.MicOnlin = 0; 
				DBG("MIC 1 OUT\n");
				MUTE_ON();
				TimeOutSet(&MuteOffTimer, 1000); 
			}
		}
		//------mic2 检测处理---------------------------------------------------//
        #if MIC2_EN
		if(!GPIO_RegOneBitGet(MIC2_DETECT_IN,MIC2_DETECT_PIN))
		{
			if(++Mic2Cnt > 50)
			{
				if(gCtrlVars.Mic2Onlin ==0  )
				{
					gCtrlVars.Mic2Onlin = 1;  
					DBG("MIC 2 IN\n");
					MUTE_ON();
					TimeOutSet(&MuteOffTimer, 1000); 
				}
			}
		}
		else
		{
			Mic2Cnt = 0;
			if(gCtrlVars.Mic2Onlin )
			{
				gCtrlVars.Mic2Onlin = 0;	 
				DBG("MIC 2 Out\n");
				MUTE_ON();
				TimeOutSet(&MuteOffTimer, 1000); 
			}
		}
        #endif
		if(IsTimeOut(&MuteOffTimer)) 
		{
			if(gCtrlVars.MicOnlin
			#ifdef CFG_FUNC_DETECT_PHONE_EN	
				|| gCtrlVars.EarPhoneOnlin
			#endif
			)
			{
				MUTE_OFF();
			}
		}
	}	
#endif	
}

/*************************************************
*    闪避处理函数
*
*
***************************************************/
void ShunningModeProcess(void)
{
	////20ms调用一次此函数//增益值(dyn_gain)闪避功能专用///0~4096 分16级，总为0~16 256为一级  
#ifdef CFG_FUNC_SHUNNING_EN
	static uint16_t shnning_recover_dly = 0;
	static uint16_t shnning_up_dly      = 0;
	static uint16_t shnning_down_dly    = 0;
	static uint16_t cnt = 0;
	 
	if( (gCtrlVars.ShunningMode == 0 )|| (gCtrlVars.MicAudioSdct_unit.enable==0) )
	{
	    if(shnning_up_dly)
		{
			shnning_up_dly--;
			return; 
		}
		shnning_up_dly = SHNNIN_UP_DLY;
	    if(gCtrlVars.aux_out_dyn_gain !=  gCtrlVars.aux_gain_control_unit.gain)//////阀值
		{
			if(gCtrlVars.aux_out_dyn_gain < gCtrlVars.aux_gain_control_unit.gain)
			{
				gCtrlVars.aux_out_dyn_gain += SHNNIN_STEP;
			}
			else if(gCtrlVars.aux_out_dyn_gain > gCtrlVars.aux_gain_control_unit.gain)
			{
			    if(gCtrlVars.aux_out_dyn_gain >= SHNNIN_STEP)
		    	{
					gCtrlVars.aux_out_dyn_gain -= SHNNIN_STEP;
		    	}
			}
			if(gCtrlVars.aux_out_dyn_gain > gCtrlVars.aux_gain_control_unit.gain)
			{
				gCtrlVars.aux_out_dyn_gain = gCtrlVars.aux_gain_control_unit.gain;
			}
		}
		#if CFG_REMINDSOUND_EN
		if(gCtrlVars.remind_out_dyn_gain !=  gCtrlVars.remind_effect_gain_control_unit.gain)//////阀值
		{
			if(gCtrlVars.remind_out_dyn_gain < gCtrlVars.remind_effect_gain_control_unit.gain)
			{
				gCtrlVars.remind_out_dyn_gain += SHNNIN_STEP;
			}
			else if(gCtrlVars.remind_out_dyn_gain > gCtrlVars.remind_effect_gain_control_unit.gain)
			{
			    if(gCtrlVars.remind_out_dyn_gain >= SHNNIN_STEP)
		    	{
					gCtrlVars.remind_out_dyn_gain -= SHNNIN_STEP;
		    	}
			}
			if(gCtrlVars.remind_out_dyn_gain > gCtrlVars.remind_effect_gain_control_unit.gain)
			{
				gCtrlVars.remind_out_dyn_gain = gCtrlVars.remind_effect_gain_control_unit.gain;
			}
		}
		#endif
		return;
	}
    cnt++;
	cnt &= 0x07;
	//if(cnt==0) DBG("Sdct level:%ld\n",gCtrlVars.AudioSdct_unit.level);
	 	 
	if(gCtrlVars.MicAudioSdct_unit.level > SHNNIN_VALID_DATA)///vol----
	{
		shnning_recover_dly = SHNNIN_VOL_RECOVER_TIME;
		if(gCtrlVars.freq_shifter_unit.fade_step == 4)
		{
	        gCtrlVars.freq_shifter_unit.fade_step = 5;//完善移频开启后的啾啾干扰声现象
		}
        if(shnning_down_dly)
		{
			shnning_down_dly--;
			return; 
		}
		//----完善移频开启后的啾啾干扰声现象--//
		
		if((!gCtrlVars.freq_shifter_unit.auto_on_flag) && gCtrlVars.freq_shifter_unit.enable)
		{
			gCtrlVars.freq_shifter_unit.auto_on_flag = 1;
			gCtrlVars.freq_shifter_unit.fade_step = 5;
		}
		//------------------------------------//
        shnning_down_dly = SHNNIN_DOWN_DLY;

		if(gCtrlVars.aux_gain_control_unit.gain > SHNNIN_THRESHOLD)
		{
			if(gCtrlVars.aux_out_dyn_gain > SHNNIN_THRESHOLD)//////阀值 	     
			{
				if(gCtrlVars.aux_out_dyn_gain >= SHNNIN_STEP)
				{
					gCtrlVars.aux_out_dyn_gain -= SHNNIN_STEP;
				}
				DBG("Aux Shunning start\n");
			}
		}
		else
		{
			gCtrlVars.aux_out_dyn_gain = gCtrlVars.aux_gain_control_unit.gain;
		}
		#if CFG_REMINDSOUND_EN
		if(gCtrlVars.remind_effect_gain_control_unit.gain > SHNNIN_THRESHOLD)
		{
			if(gCtrlVars.remind_out_dyn_gain > SHNNIN_THRESHOLD)//////阀值 	     
			{
				if(gCtrlVars.remind_out_dyn_gain >= SHNNIN_STEP)
		    	{
					gCtrlVars.remind_out_dyn_gain -= SHNNIN_STEP;
		    	}

				DBG("Remind Shunning start\n");
			}
		}
		else
		{
			gCtrlVars.remind_out_dyn_gain = gCtrlVars.remind_effect_gain_control_unit.gain;
		}
		#endif
	}
	else/////vol+++
	{
		if(shnning_up_dly)
		{
			shnning_up_dly--;
			return; 
		}

		if(shnning_recover_dly)
		{
			shnning_recover_dly--;
			return;
		}
		//----完善移频开启后的啾啾干扰声现象--//
		if(gCtrlVars.freq_shifter_unit.auto_on_flag  && gCtrlVars.freq_shifter_unit.enable)
		{
			gCtrlVars.freq_shifter_unit.auto_on_flag = 0;
			gCtrlVars.freq_shifter_unit.fade_step = 1;
		}
		//------------------------------------//
		shnning_up_dly = SHNNIN_UP_DLY;

		if(gCtrlVars.aux_out_dyn_gain !=  gCtrlVars.aux_gain_control_unit.gain)//////阀值
		{
			if(gCtrlVars.aux_out_dyn_gain < gCtrlVars.aux_gain_control_unit.gain)
			{
				gCtrlVars.aux_out_dyn_gain += SHNNIN_STEP;
			}
			else if(gCtrlVars.aux_out_dyn_gain > gCtrlVars.aux_gain_control_unit.gain)
			{
				if(gCtrlVars.aux_out_dyn_gain >= SHNNIN_STEP)
				{
					gCtrlVars.aux_out_dyn_gain -= SHNNIN_STEP;
				}
			}
			if(gCtrlVars.aux_out_dyn_gain > gCtrlVars.aux_gain_control_unit.gain)
			{
				gCtrlVars.aux_out_dyn_gain  = gCtrlVars.aux_gain_control_unit.gain;
			}
		}
		#if CFG_REMINDSOUND_EN
		if(gCtrlVars.remind_out_dyn_gain !=  gCtrlVars.remind_effect_gain_control_unit.gain)//////阀值
		{
			if(gCtrlVars.remind_out_dyn_gain < gCtrlVars.remind_effect_gain_control_unit.gain)
			{
				gCtrlVars.remind_out_dyn_gain += SHNNIN_STEP;
			}
			else if(gCtrlVars.remind_out_dyn_gain > gCtrlVars.remind_effect_gain_control_unit.gain)
			{
				if(gCtrlVars.remind_out_dyn_gain >= SHNNIN_STEP)
		    	{
					gCtrlVars.remind_out_dyn_gain -= SHNNIN_STEP;
		    	}
			}
			if(gCtrlVars.remind_out_dyn_gain > gCtrlVars.remind_effect_gain_control_unit.gain)
			{
				gCtrlVars.remind_out_dyn_gain  = gCtrlVars.remind_effect_gain_control_unit.gain;
			}
		}
		#endif
	}
#endif
}

/*
****************************************************************
* 耳机插拔检测处理demo
*
*
****************************************************************
*/
void DetectEarPhone(void)
{
#ifdef CFG_FUNC_DETECT_PHONE_EN
	static uint8_t PhoneTimeInit=0;
	static uint8_t PhoneCnt = 0;
	uint32_t msg;

	if(!PhoneTimeInit)
	{
		PhoneTimeInit = 1;
		PhoneCnt = 0;
		gCtrlVars.EarPhoneOnlin = 0;
		GPIO_RegOneBitClear(PHONE_DETECT_OE, PHONE_DETECT_PIN);
		GPIO_RegOneBitSet(PHONE_DETECT_IE, PHONE_DETECT_PIN);
		///PULL enable
		GPIO_RegOneBitSet(PHONE_DETECT_PU, PHONE_DETECT_PIN);
		GPIO_RegOneBitClear(PHONE_DETECT_PD, PHONE_DETECT_PIN);
		DelayUs(50);
		if(!GPIO_RegOneBitGet(PHONE_DETECT_IN,PHONE_DETECT_PIN))
		//if(GPIO_RegOneBitGet(PHONE_DETECT_IN,PHONE_DETECT_PIN))
		{
			gCtrlVars.EarPhoneOnlin = 1;			
			TimeOutSet(&MuteOffTimer, 1000); 
			DBG("Ear Phone In\n");
		}
	}
    else
	{
		if(!GPIO_RegOneBitGet(PHONE_DETECT_IN,PHONE_DETECT_PIN))
		//if(GPIO_RegOneBitGet(PHONE_DETECT_IN,PHONE_DETECT_PIN))
		{
			if(++PhoneCnt > 50)//消抖处理
			{
				PhoneCnt = 0;
				if(gCtrlVars.EarPhoneOnlin == 0)
				{
					gCtrlVars.EarPhoneOnlin = 1;	
					TimeOutSet(&MuteOffTimer, 1000); 
					//msg = FUNC_ID_EARPHONE_IN;
					//SendMessage(&msg,NULL);
					DBG("Ear Phone In\n");
				}		
			}
		}
		else
		{
			PhoneCnt = 0;
			if(gCtrlVars.EarPhoneOnlin)
			{
				//msg =	FUNC_ID_EARPHONE_OUT;
				gCtrlVars.EarPhoneOnlin = 0;
				MUTE_ON();
				DBG("Ear Phone Out\n");
				TimeOutSet(&MuteOffTimer, 1000); 
			}
		}
		if(IsTimeOut(&MuteOffTimer)) 
		{
			if(gCtrlVars.EarPhoneOnlin
			#ifdef CFG_FUNC_DETECT_MIC_EN	
				|| gCtrlVars.MicOnlin
			#endif
			)
			{
				MUTE_OFF();
			}
		}
	}
#endif	
}

/*
****************************************************************
* 3线，4线耳机类型检测处理demo
*
*
****************************************************************
*/
void DetectMic3Or4Line(void)
{
	
#ifdef CFG_FUNC_DETECT_MIC_SEG_EN
    //---------mic1 var-------------------//
	static uint8_t MicSegmentInit=0;
	static uint8_t MicSegCnt = 0;
	uint8_t val;
	uint32_t msg;

	if(!MicSegmentInit)
	{
		MicSegmentInit = 1;
		MicSegCnt = 0;
		gCtrlVars.MicSegment = 0;////default 0 line
		GPIO_RegOneBitClear(MIC_SEGMENT_PU, MIC_SEGMENT_PIN);
		GPIO_RegOneBitSet(MIC_SEGMENT_PD, MIC_SEGMENT_PIN);			   
		GPIO_RegOneBitClear(MIC_SEGMENT_OE, MIC_SEGMENT_PIN);
		GPIO_RegOneBitSet(MIC_SEGMENT_IE, MIC_SEGMENT_PIN);
		DelayUs(50);
		if(GPIO_RegOneBitGet(MIC_SEGMENT_IN,MIC_SEGMENT_PIN))
		{
			gCtrlVars.MicSegment = 4;
			DBG("Mic segmen is 4 line\n");
		}	   
	}
	else
	{
		if(!gCtrlVars.MicOnlin)
	    {
			MIC_MUTE_ON();
		}
		else
		{
			if(gCtrlVars.MicSegment == 4 )
			{
				MIC_MUTE_OFF();
			}
			else
			{
				MIC_MUTE_ON();
			}
		}
		if(GPIO_RegOneBitGet(MIC_SEGMENT_IN,MIC_SEGMENT_PIN))
		{
			MicSegCnt = 0;
			if(gCtrlVars.MicSegment != 4  )
			{
				gCtrlVars.MicSegment = 4;
				//msg =	FUNC_ID_MIC_3Or4_LINE;
				DBG("Mic segmen is 4 line\n");
			}
		}
		else
		{
			if(++MicSegCnt > 50)
			{
				MicSegCnt = 0;
				if(gCtrlVars.MicSegment != 3 )//////H=4line l=3line
				{
					gCtrlVars.MicSegment = 3;	 
					//msg = FUNC_ID_MIC_3Or4_LINE;
					//SendMessage(&msg,NULL);
					DBG("Mic segmen is 3 line\n");
				}	 
			}
		}
	}
#endif		
}
#if 0

//-------------------------------------------//
extern uint8_t  effect_sum;
extern uint16_t effect_list[];
extern uint32_t effect_addr[];
extern uint16_t effect_list_addr[];

extern uint8_t  communic_buf[512];
extern uint32_t communic_buf_w ;
uint8_t  eff_addr[40];
#if CFG_ADC_LEVEL_KEY_EN
extern uint8_t    ADCLevelChannelTotal;
extern uint16_t   ADCLevelsValue[];
#endif

void AudioDAC_ClearBuff(void);
void MiscDeviceProcess(void);
void HWDeviceNull(void);

/*
****************************************************************
*
*
*
****************************************************************
*/
void MiscDeviceProcess(void)
{
	///////此函数仅调用对时间不作要求的功能//////要求奇数次定时器//
    #define  MISC_PROCESS_DLY   3
	static TIMER MISCTimer;
	static uint8_t MISCInit=0;
	static uint8_t MISCConut;
#if CFG_DEBUG_EN
	static uint8_t Hour,Minutes,Second;
	static uint16_t Mills;
#endif
	
	if(MISCInit==0)
	{
		MISCInit = 1;

		MISCConut = 0;
        #if CFG_DEBUG_EN
		Hour=0;
		Minutes=0;
		Second=0;
		Mills = 0;
        #endif
		TimeOutSet(&MISCTimer, MISC_PROCESS_DLY); 
		   
		return ;
	}
	if(!IsTimeOut(&MISCTimer)) return;
	TimeOutSet(&MISCTimer, MISC_PROCESS_DLY);
    ////////////////////////////////
	IsPcLink();
	///////此函数仅调用对时间不作要求的功能/==大约 3*8=24ms调用一次/////
	MISCConut++;
	MISCConut &= 0x7;
	//if(MISCConut == 0 )
	{
		ShunningModeProcess();//闪避功能处理
	}
//---------------------------------------------//
#if CFG_DEBUG_EN
	Mills++;
	if(Mills> 1000/MISC_PROCESS_DLY)///1s
	{
#if CFG_USB_EN
		if(Second % 6 == 0)
		{
			DBG("\n");
			DBG("USB STATUS :UP     DOWN     ONLINE       OS\n");
			DBG("            %02x     %02x         %02x        %02x\n",gCtrlVars.usb_uploading,gCtrlVars.usb_downloading, gCtrlVars.usb_online,gCtrlVars.usb_android);
		}
#endif
		Mills = 0;
		Second++;
		if(Second > 59)
		{
			Second = 0;
			Minutes++;
			if(Minutes > 59)
			{
				Minutes = 0;
				Hour++;
				if(Hour > 23)
				{
					Hour = 0;
				}
			}

			DBG("Run Time = %02d:%02d:%02d\n",Hour,Minutes,Second);
		}
	}
#endif
//----------------------------------------------------//

}
/*
****************************************************************
*
*
*
****************************************************************
*/
void HWDeviceNull(void)
{
     //DBG("Device Null\n");
}

/*
****************************************************************
*
*
*
****************************************************************
*/
void LedDisplayOff(void)
{
#if CFG_LED_EN
    EAR_LED_OFF();
    
    SONG_LED_OFF();
    
    TALK_LED_OFF();
    
    KTV_LED_OFF();
    
    AUTO_LED_OFF();
    
    SHUNNING_LED_OFF();
#endif	

#ifdef CFG_FUNC_DETECT_MIC_SEG_EN
    MIC_MUTE_OFF();
#endif
}
/*
****************************************************************
*
*
*
****************************************************************
*/
void LedDisplay(void)
{
#if CFG_LED_EN
    static uint8_t   LedTimeInit=0;
    
    if(!LedTimeInit)
    {
		LedTimeInit = 1;

		LedDisplayOff();
    }
    else
    {	  
		SONG_LED_OFF();

		TALK_LED_OFF();

		KTV_LED_OFF();

		AUTO_LED_OFF();

		SHUNNING_LED_OFF();
        
		if(gCtrlVars.EffectMode == EFFECT_MODE_REVERB)
		{
			SONG_LED_ON();
		}
		else if(gCtrlVars.EffectMode == EFFECT_MODE_NORMAL)
		{
			TALK_LED_ON();
		}
		else if(gCtrlVars.EffectMode == EFFECT_MODE_KTV)
		{
			KTV_LED_ON();
		}
		else if(gCtrlVars.EffectMode == EFFECT_MODE_AUTO_TUNE)
		{
			AUTO_LED_ON();
		}
		else
		{
		}

		if(gCtrlVars.ShunningMode  == TRUE)
		{
			SHUNNING_LED_ON();
		}
   }
#endif	  
}


/*
****************************************************************
*
*
*
****************************************************************
*/
void DACVolumeSmoothProcess(void)
{
#if CFG_DAC_VOL_FADE_EN
   //---------仅只是在上电时，DAC的音量进行淡入操作，其它情况不需要------------//
	static uint8_t power_fade_f = 0;

	if(power_fade_f == 0)
   	{
   	   power_fade_f                 = 1;
   	   gCtrlVars.dac1_dig_vol		= 0;
	   gCtrlVars.dac0_dig_l_vol 	= 0;
       gCtrlVars.dac0_dig_r_vol 	= 0;
	   #if CFG_DAC0_EN
	   AudioDAC_VolSet(DAC0, gCtrlVars.dac0_dig_l_vol, gCtrlVars.dac0_dig_r_vol);
	   #endif
	   #if CFG_DAC1_EN
	   AudioDAC_VolSet(DAC1, gCtrlVars.dac1_dig_vol, gCtrlVars.dac1_dig_vol);
	   #endif		
   	}
    #if CFG_DAC0_EN && CFG_DAC1_EN
	if(power_fade_f == 4)  return;
    #elif CFG_DAC0_EN 
	if(power_fade_f == 3)  return;
    #else
	if(power_fade_f == 2)  return;
    #endif

    #if CFG_DAC0_EN
	if(gCtrlVars.dac0_dig_l_vol < gCtrlVars.max_dac0_dig_l_vol)
	{
		gCtrlVars.dac0_dig_l_vol  += 200; 

		if(gCtrlVars.dac0_dig_l_vol > gCtrlVars.max_dac0_dig_l_vol)
		{
			gCtrlVars.dac0_dig_l_vol = gCtrlVars.max_dac0_dig_l_vol;
		}
		if(gCtrlVars.dac0_dig_l_vol == gCtrlVars.max_dac0_dig_l_vol)
		{
			power_fade_f++;
		}

		AudioDAC_VolSet(DAC0, gCtrlVars.dac0_dig_l_vol, gCtrlVars.dac0_dig_r_vol);
	}
 
	if(gCtrlVars.dac0_dig_r_vol < gCtrlVars.max_dac0_dig_r_vol)
	{
		gCtrlVars.dac0_dig_r_vol += 200; 

		if(gCtrlVars.dac0_dig_r_vol > gCtrlVars.max_dac0_dig_r_vol)
		{
			gCtrlVars.dac0_dig_r_vol = gCtrlVars.max_dac0_dig_r_vol;
		}
		if(gCtrlVars.dac0_dig_r_vol == gCtrlVars.max_dac0_dig_r_vol)
		{
			power_fade_f++;
		}

		AudioDAC_VolSet(DAC0, gCtrlVars.dac0_dig_l_vol, gCtrlVars.dac0_dig_r_vol);
	}
    #endif

    #if CFG_DAC1_EN
	if(gCtrlVars.dac1_dig_vol < gCtrlVars.max_dac1_dig_vol)
	{
		gCtrlVars.dac1_dig_vol += 200; 

		if(gCtrlVars.dac1_dig_vol > gCtrlVars.max_dac1_dig_vol)
		{
			gCtrlVars.dac1_dig_vol = gCtrlVars.max_dac1_dig_vol;
		}	
		if(gCtrlVars.dac1_dig_vol == gCtrlVars.max_dac1_dig_vol)
		{
			power_fade_f++;
		}		
		AudioDAC_VolSet(DAC1, gCtrlVars.dac1_dig_vol, gCtrlVars.dac1_dig_vol);
	}
    #endif
#endif  
}
/*
****************************************************************
* 电位器调节的平滑处理的变量初始化
*
*
****************************************************************
*/
void MicVolSmoothInit(void)
{
#if CFG_ADC_LEVEL_KEY_EN
    uint32_t msg;

    gCtrlVars.init_param		  |= 0x01;/////init adclevel flag

    gCtrlVars.mic_vol			  = 0;
	gCtrlVars.mic_vol_bak	      = 0;

	gCtrlVars.mic_echo_delay	  = 0;
	gCtrlVars.mic_echo_delay	  = 0;
	
	gCtrlVars.aux_gain	          = 0;
	gCtrlVars.aux_gain_bak	      = 0;	
	
	gCtrlVars.rec_gain	          = 0;
	gCtrlVars.rec_gain_bak	      = 0;
	
	gCtrlVars.mic_out_gain_control_unit.gain = 0;
    gCtrlVars.aux_gain_control_unit.gain = 0;
	gCtrlVars.rec_out_gain_control_unit.gain = 0;
	
    gCtrlVars.BassGain 		         = 0;
    gCtrlVars.BassGainBak	         = 0;
    gCtrlVars.TrebGain 		         = 0;
    gCtrlVars.TrebGainBak	             = 0;
    BassTrebAjust();

    gCtrlVars.ReverbRoom		         = 0;
	gCtrlVars.ReverbRoomBak              = 0;
    gCtrlVars.plate_reverb_unit.decay    = 0;
	gCtrlVars.plate_reverb_unit.wetdrymix= 0;
	gCtrlVars.echo_unit.delay_samples    = 0;
	gCtrlVars.echo_unit.attenuation      = 0;
    gCtrlVars.reverb_unit.wet_scale      = 0;
	gCtrlVars.reverb_unit.roomsize_scale = 0;
	gCtrlVars.reverb_pro_unit.wet        = -70;	
	gCtrlVars.reverb_pro_unit.erwet      = -70;
    msg = FUNC_ID_REVERB_CONFIG;

    SendMessage(&msg,NULL);		

	#if CFG_EQ_MODE_SET_EN
	EqModeSet(gCtrlVars.EqMode);	
	#endif

#else
    #if CFG_ADC_KEY_EN
    uint32_t msg;
    uint16_t step,r;

	BassTrebAjust();
		
	gCtrlVars.mic_out_gain_control_unit.gain = DigVolTab_16[gCtrlVars.mic_vol];

	gCtrlVars.aux_gain_control_unit.gain = DigVolTab_16[gCtrlVars.aux_gain];

	gCtrlVars.rec_out_gain_control_unit.gain = DigVolTab_16[gCtrlVars.rec_gain];
		

	step = gCtrlVars.max_chorus_wet / MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.chorus_unit.wet = gCtrlVars.max_chorus_wet;
	}
	else
	{
		gCtrlVars.chorus_unit.wet = gCtrlVars.ReverbRoom * step;
	}
	step = gCtrlVars.max_plate_reverb_roomsize / MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.plate_reverb_unit.decay = gCtrlVars.max_plate_reverb_roomsize;
	}
	else
	{
		gCtrlVars.plate_reverb_unit.decay = gCtrlVars.ReverbRoom * step;
	}
	//DBG("mic_wetdrymix   = %d\n",gCtrlVars.max_plate_reverb_wetdrymix);
	step = gCtrlVars.max_plate_reverb_wetdrymix / MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.plate_reverb_unit.wetdrymix = gCtrlVars.max_plate_reverb_wetdrymix;
	}
	else
	{
		gCtrlVars.plate_reverb_unit.wetdrymix = gCtrlVars.ReverbRoom * step;
	}
	
    step = gCtrlVars.max_echo_delay / MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.echo_unit.delay_samples = gCtrlVars.max_echo_delay;
	}
	else
	{
		gCtrlVars.echo_unit.delay_samples = gCtrlVars.ReverbRoom * step;
	}
    step = gCtrlVars.max_echo_depth/ MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.echo_unit.attenuation= gCtrlVars.max_echo_depth;
	}
	else
	{
		gCtrlVars.echo_unit.attenuation = gCtrlVars.ReverbRoom * step;
	}

    step = gCtrlVars.max_reverb_wet_scale/ MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.reverb_unit.wet_scale = gCtrlVars.max_reverb_wet_scale;
	}
	else
	{
		gCtrlVars.reverb_unit.wet_scale = gCtrlVars.ReverbRoom * step;
	}
    step = gCtrlVars.max_reverb_roomsize/ MAX_ADCLEVL_STEP;
	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.reverb_unit.roomsize_scale = gCtrlVars.max_reverb_roomsize;
	}
	else
	{
		gCtrlVars.reverb_unit.roomsize_scale = gCtrlVars.ReverbRoom * step;
	}
		
    //+0  ~~~ -70
    r = abs(gCtrlVars.max_reverb_pro_wet);
    r = 70-r;
    step = r / MAX_ADCLEVL_STEP;

	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet;
	}
	else
	{
		r = MAX_ADCLEVL_STEP - 1 - gCtrlVars.ReverbRoom;

		r*= step;

		gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet - r;

		if(gCtrlVars.ReverbRoom == 0) gCtrlVars.reverb_pro_unit.wet = -70;
	}


    r = abs(gCtrlVars.max_reverb_pro_erwet);
    r = 70-r;
    step = r / MAX_ADCLEVL_STEP;

	if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
	{
		gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet;
	}
	else
	{
		r = MAX_ADCLEVL_STEP - 1 - gCtrlVars.ReverbRoom;

		r*= step;

		gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet - r;

		if(gCtrlVars.ReverbRoom == 0) gCtrlVars.reverb_pro_unit.erwet = -70;

	}
		
    msg = FUNC_ID_REVERB_CONFIG;

    SendMessage(&msg,NULL);
	#endif
	#if CFG_EQ_MODE_SET_EN
	EqModeSet(gCtrlVars.EqMode);	
	#endif
#endif
}
 /*
 ****************************************************************
 *电位器调节中的参数值的平滑处理
 *提供的电位器demo的调节步长是:0~15
 *
 ****************************************************************
 */
void MicVolSmoothProcess(void)
{
#if CFG_ADC_LEVEL_KEY_EN

    #define  VOL_DLY_5MS   5
	uint16_t step,r;
	uint32_t msg;
	//---------mic vol 电位器渐变调节-------------------//
	if(gCtrlVars.mic_vol != gCtrlVars.mic_vol_bak)
	{
		if(gCtrlVars.mic_vol > gCtrlVars.mic_vol_bak)
		{
			gCtrlVars.mic_vol--;
		}
		else if(gCtrlVars.mic_vol < gCtrlVars.mic_vol_bak)
		{
			gCtrlVars.mic_vol++;
		}

		gCtrlVars.mic_out_gain_control_unit.gain = DigVolTab_16[gCtrlVars.mic_vol];
	}

    //-------------bass 电位器渐变调节-----------------------------------------//
	if(gCtrlVars.BassGain !=  gCtrlVars.BassGainBak)
	{
		if(gCtrlVars.BassGain >  gCtrlVars.BassGainBak)
		{
			gCtrlVars.BassGain -= BASS_TREB_GAIN_STEP;
    					 
		}
		else if(gCtrlVars.BassGain <  gCtrlVars.BassGainBak)
		{
			gCtrlVars.BassGain += BASS_TREB_GAIN_STEP;
    					 
		}
		
		BassTrebAjust();
		//DBG("bass = %d\n",gCtrlVars.BassGain);
	}
    //-------------treb 电位器渐变调节-----------------------------------------//
	if(gCtrlVars.TrebGain !=  gCtrlVars.TrebGainBak)
	{
		if(gCtrlVars.TrebGain >  gCtrlVars.TrebGainBak)
		{
			gCtrlVars.TrebGain -= BASS_TREB_GAIN_STEP;
    					 
		}
		else if(gCtrlVars.TrebGain <  gCtrlVars.TrebGainBak)
		{
			gCtrlVars.TrebGain += BASS_TREB_GAIN_STEP;
    					 
		}
		
		BassTrebAjust();
		//DBG("treb = %d\n",gCtrlVars.TrebGain);
	}	
	//-------------reverb-gain 电位器渐变调节---------------------------------------//
	if(gCtrlVars.ReverbRoom !=  gCtrlVars.ReverbRoomBak)
	{
		if(gCtrlVars.ReverbRoom >  gCtrlVars.ReverbRoomBak)
		{
			gCtrlVars.ReverbRoom -= 1;
						 
		}
		else if(gCtrlVars.ReverbRoom <  gCtrlVars.ReverbRoomBak)
		{
			gCtrlVars.ReverbRoom += 1;
						 
		}					
		//DBG("mic_ReverbRoom  = %d\n",gCtrlVars.ReverbRoom);
		step = gCtrlVars.max_chorus_wet / MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.chorus_unit.wet = gCtrlVars.max_chorus_wet;
		}
		else
		{
			gCtrlVars.chorus_unit.wet = gCtrlVars.ReverbRoom * step;
		}		
		step = gCtrlVars.max_plate_reverb_roomsize / MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.plate_reverb_unit.decay = gCtrlVars.max_plate_reverb_roomsize;
		}
		else
		{
			gCtrlVars.plate_reverb_unit.decay = gCtrlVars.ReverbRoom * step;
		}
		//DBG("mic_wetdrymix   = %d\n",gCtrlVars.max_plate_reverb_wetdrymix);
		step = gCtrlVars.max_plate_reverb_wetdrymix / MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.plate_reverb_unit.wetdrymix = gCtrlVars.max_plate_reverb_wetdrymix;
		}
		else
		{
			gCtrlVars.plate_reverb_unit.wetdrymix = gCtrlVars.ReverbRoom * step;
		}			
        step = gCtrlVars.max_echo_delay / MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.echo_unit.delay_samples = gCtrlVars.max_echo_delay;
		}
		else
		{
			gCtrlVars.echo_unit.delay_samples = gCtrlVars.ReverbRoom * step;
		}
		step = gCtrlVars.max_echo_depth/ MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.echo_unit.attenuation= gCtrlVars.max_echo_depth;
		}
		else
		{
			gCtrlVars.echo_unit.attenuation = gCtrlVars.ReverbRoom * step;
		}

	    step = gCtrlVars.max_reverb_wet_scale/ MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.reverb_unit.wet_scale = gCtrlVars.max_reverb_wet_scale;
		}
		else
		{
			gCtrlVars.reverb_unit.wet_scale = gCtrlVars.ReverbRoom * step;
		}
	    step = gCtrlVars.max_reverb_roomsize/ MAX_ADCLEVL_STEP;
		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.reverb_unit.roomsize_scale = gCtrlVars.max_reverb_roomsize;
		}
		else
		{
			gCtrlVars.reverb_unit.roomsize_scale = gCtrlVars.ReverbRoom * step;
		}
		
		//+0  ~~~ -70
	    r = abs(gCtrlVars.max_reverb_pro_wet);
	    r = 70-r;
	    step = r / MAX_ADCLEVL_STEP;

		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet;
		}
		else
		{
			r = MAX_ADCLEVL_STEP - 1 - gCtrlVars.ReverbRoom;

			r*= step;

			gCtrlVars.reverb_pro_unit.wet = gCtrlVars.max_reverb_pro_wet - r;

			if(gCtrlVars.ReverbRoom == 0) gCtrlVars.reverb_pro_unit.wet = -70;
		}

	    r = abs(gCtrlVars.max_reverb_pro_erwet);
	    r = 70-r;
	    step = r / MAX_ADCLEVL_STEP;

		if(gCtrlVars.ReverbRoom >= (MAX_ADCLEVL_STEP-1))
		{
			gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet;
		}
		else
		{
			r = MAX_ADCLEVL_STEP - 1 - gCtrlVars.ReverbRoom;

			r*= step;

			gCtrlVars.reverb_pro_unit.erwet = gCtrlVars.max_reverb_pro_erwet - r;

			if(gCtrlVars.ReverbRoom == 0) gCtrlVars.reverb_pro_unit.erwet = -70;

		}

		msg = FUNC_ID_REVERB_CONFIG;

		SendMessage(&msg,NULL);
	}				
    //-------------mic delay 电位器渐变调节-----------------------------------------//
	if(gCtrlVars.mic_echo_delay !=  gCtrlVars.mic_echo_delay_bak)
	{
		if(gCtrlVars.mic_echo_delay >  gCtrlVars.mic_echo_delay_bak)
		{
			gCtrlVars.mic_echo_delay -= BASS_TREB_GAIN_STEP;
    					 
		}
		else if(gCtrlVars.mic_echo_delay <  gCtrlVars.mic_echo_delay_bak)
		{
			gCtrlVars.mic_echo_delay += BASS_TREB_GAIN_STEP;
    					 
		}
		
		gCtrlVars.echo_unit.delay_samples = EchoDlyTab_16[gCtrlVars.mic_echo_delay];
	}
    //------------aux gain 电位器渐变调节-----------------------------------------//
	if(gCtrlVars.aux_gain !=  gCtrlVars.aux_gain_bak)
	{
		if(gCtrlVars.aux_gain >  gCtrlVars.aux_gain_bak)
		{
			gCtrlVars.aux_gain -= BASS_TREB_GAIN_STEP;
    					 
		}
		else if(gCtrlVars.aux_gain <  gCtrlVars.aux_gain_bak)
		{
			gCtrlVars.aux_gain += BASS_TREB_GAIN_STEP;
    					 
		}
		
		gCtrlVars.aux_gain_control_unit.gain = DigVolTab_16[gCtrlVars.aux_gain];
	}
    //-------------rec gain 电位器渐变调节-----------------------------------------//
	if(gCtrlVars.rec_gain !=  gCtrlVars.rec_gain_bak)
	{
		if(gCtrlVars.rec_gain >  gCtrlVars.rec_gain_bak)
		{
			gCtrlVars.rec_gain -= BASS_TREB_GAIN_STEP;
    					 
		}
		else if(gCtrlVars.rec_gain <  gCtrlVars.rec_gain_bak)
		{
			gCtrlVars.rec_gain += BASS_TREB_GAIN_STEP;
    					 
		}
		
		gCtrlVars.rec_out_gain_control_unit.gain = DigVolTab_16[gCtrlVars.rec_gain];
	}	

#endif	  
}

/*
****************************************************************
* Line in插拔检测处理
*
*
****************************************************************
*/
void DetectLineIn(void)
{
#if CFG_LINE_IN_INSERT_DETECT_EN
	static uint8_t LineInTimeInit = 0;
	static uint8_t LineInCnt = 0;
	uint32_t msg;

	if(!LineInTimeInit)
	{
		LineInTimeInit = 1;
		LineInCnt = 0;
		gCtrlVars.LineInOnlin = 0;
		////line dectecd init
        do{ \
		GPIO_PortBModeSet(GPIOB8, 0); \
		GPIO_RegBitsSet(LINEIN_DETECT_PORT_IE, LINEIN_DETECT_BIT_MASK); \
		GPIO_RegBitsClear(LINEIN_DETECT_PORT_OE, LINEIN_DETECT_BIT_MASK); \
		GPIO_RegBitsSet(LINEIN_DETECT_PORT_PU, LINEIN_DETECT_BIT_MASK); \
		GPIO_RegBitsClear(LINEIN_DETECT_PORT_PD, LINEIN_DETECT_BIT_MASK); \
		}while(0);
		DelayUs(50);
		if(!GPIO_RegOneBitGet(LINEIN_DETECT_PORT_IN, LINEIN_DETECT_BIT_MASK))
		{
			gCtrlVars.LineInOnlin = 1;
			DBG("Line In Plug in\n");
		}
	}
	else
	{
		if(!GPIO_RegOneBitGet(LINEIN_DETECT_PORT_IN, LINEIN_DETECT_BIT_MASK))
		{
			if(++LineInCnt > 50) //消抖处理
			{
				LineInCnt = 0;
				if(gCtrlVars.LineInOnlin == 0)
				{
					gCtrlVars.LineInOnlin = 1;	
					msg = FUNC_ID_LINEIN_IN;
					SendMessage(&msg,NULL);
					DBG("Line In Plug in\n");
				}		
			}
		}
		else
		{
			LineInCnt = 0;
			if(gCtrlVars.LineInOnlin)
			{
				msg =	FUNC_ID_LINEIN_OUT;
				gCtrlVars.LineInOnlin = 0;
				DBG("Line In Plug Out\n");
			}
		}
	}
#endif	
}

/*
****************************************************************
* 写调音参数到flash中，在线调音下载用
*
*
****************************************************************
*/
void UpdataParamasToSpi(uint8_t Control)
{
	if(gCtrlVars.debug_effect==0) return;

	switch(Control)
	{
		case LOAD_START:
			if(StartWriteCmd < LOAD_BUSY) 
			{
				StartWriteCmd = LOAD_START;////start load cfg file from SPI,and to PC,and updata to ram
				//DBG("Load Start\n");
			}
			break;
		case LOAD_END:
			if(StartWriteCmd < LOAD_BUSY)
			{
				StartWriteCmd = LOAD_END;/////load cfg file end
				//DBG("Load End\n");
			}
			break; 	 
		case SAVE_START:
			if(StartWriteCmd < LOAD_BUSY)
			{
				StartWriteCmd = SAVE_START;////start RX from PC data,
				//DBG("Save Start\n");
			}
			break;
		case SAVE_END:
			if(StartWriteCmd < LOAD_BUSY)
			{
				StartWriteCmd = SAVE_END;////RX OK
				//DBG("Save End\n");
			}
			break;
		default:
			break;
	}

}
/*
****************************************************************
* 写调音参数到flash中，在线调音下载用
*
*
****************************************************************
*/

void WriteEffectParamas(uint32_t mode,uint8_t *buff)
{
	//SPI_FLASH_INFO *gSpiFlashInfo;

	//uint32_t sector_total;

	DBG("[SYS]: Saving Effect Mode to flash, mode=%d\n",(uint8_t)(mode &0xff));

   // gSpiFlashInfo = SpiFlashInfoGet();

	//sector_total = gSpiFlashInfo->Capacity/4096;

    PrintAudioEffectParamaesList(4096,buff);

	if(gCtrlVars.crypto_en) AudioEffectEncryption(buff,4096,gCtrlVars.password);

    if(mode>9) mode = 9;///0~~~~9
	
    SpiFlashErase(SECTOR_ERASE, (CFG_EFFECT_PARAME_FLASH_ADDR / 4096) + mode, 0);

    mode *= 4096;
	
    SpiFlashWrite(CFG_EFFECT_PARAME_FLASH_ADDR + mode, buff, 4096, 200);
}
/*
****************************************************************
* 获取flash中记忆的调音参数
*
*
****************************************************************
*/

void ReadEffectParamas(uint32_t mode, uint8_t *buff)
{
	//SPI_FLASH_INFO *gSpiFlashInfo;

	DBG("[SYS]: Read Effect Mode from flash mode=%d\n",(uint8_t)(mode &0xff));

	if(mode>9) mode = 9;///0~~~~9

	mode *= 4096;
	
    //gSpiFlashInfo = SpiFlashInfoGet();

	SpiFlashRead(CFG_EFFECT_PARAME_FLASH_ADDR +  mode, buff, 4096, 200);

	if(gCtrlVars.crypto_en) AudioEffectDecryption(buff,4096,gCtrlVars.password);

	PrintAudioEffectParamaesList(4096,buff);

}
/*
****************************************************************
*  对导入的调音参数做合法性判断处理
*
*
****************************************************************
*/
bool  AudioEffectListJudge(uint16_t len, uint8_t *buff)
{	 
	uint16_t cmd_len,packet_len,k,i;
	
	uint8_t end_code,Control,eff_sum;
	
	uint16_t position;

	uint32_t line_channel;
	uint32_t mic_channel;


    for(i = 0; i < 40;i++)
	{
		eff_addr[i] = 0x00;
	}

	line_channel = 0;

	mic_channel  = 0;
	
	position = 0;

    eff_sum = 0;
	
	while(position < len)
	{
		if ((buff[position] ==0xa5) && (buff[position+1] ==0x5a))
		{
			//ctl_code   = buff[position + 2];//

			packet_len = buff[position + 3];

			end_code	= buff[position + 4 + packet_len];//0x16

			if(end_code == 0x16)////end code ok
			{
				cmd_len = packet_len + 5;

				for(k = 0; k < cmd_len; k++)//get signal COMMAND
				{
					communic_buf[k] = buff[position++];
					//if((ctl_code==LOAD_START)||(ctl_code==SAVE_START)||(ctl_code==LOAD_END)||(ctl_code==SAVE_END))
					//{
						//DBG("%02X ",communic_buf[k]);
					//}
				}

				//if((ctl_code==LOAD_START)||(ctl_code==SAVE_START)||(ctl_code==LOAD_END)||(ctl_code==SAVE_END))
				//{
					//DBG("\n");
				//}
				Control = communic_buf[2];
				
				//DBG("%02X\n",Control);

				if(Control == 0x03)
				{
					if(communic_buf[5])
					{
						line_channel |= BIT(INPUT_LINEIN1_LEFT);
					}
					if(communic_buf[7])
					{
						line_channel |= BIT(INPUT_LINEIN1_RIGHT);
					}	
					if(communic_buf[9])
					{
						line_channel |= BIT(INPUT_LINEIN2_LEFT);
					}
					if(communic_buf[11])
					{
						line_channel |= BIT(INPUT_LINEIN2_RIGHT);
					}
					
					if(communic_buf[13])
					{
						mic_channel |= BIT(INPUT_MIC4);
					}
					if(communic_buf[15])
					{
						mic_channel |= BIT(INPUT_MIC3);
					}
				}
				
				if(Control == 0x06)
				{
					if(communic_buf[5])
					{
						line_channel |= BIT(INPUT_LINEIN3_LEFT);
					}
					if(communic_buf[7])
					{
						line_channel |= BIT(INPUT_LINEIN3_RIGHT);
					}	
					if(communic_buf[9])
					{
						line_channel |= BIT(INPUT_LINEIN2_LEFT);
					}
					if(communic_buf[11])
					{
						line_channel |= BIT(INPUT_LINEIN2_RIGHT);
					}
					if(communic_buf[13])
					{
						line_channel |= BIT(INPUT_FMIN_LEFT);
					}
					
					if(communic_buf[15])
					{
						line_channel |= BIT(INPUT_FMIN_RIGHT);
					}

					if(communic_buf[17])
					{
						mic_channel |= BIT(INPUT_MIC2);
					}
					if(communic_buf[19])
					{
						mic_channel |= BIT(INPUT_MIC1);
					}
				 }

				if((Control >= 0x81) && (Control < 0xfe))
				{
					if(Control != 0xfc)///user define tag
					{
						eff_addr[eff_sum++] = Control;
					}
				}
			}
			else  // end code error
			{
				position++;
			}
		}
		else // serch start code....
		{
			position++;
		}
	}
    
	if(eff_sum != effect_sum)
	{
		DBG("audio effect list illega,read list= %d, local_list = %d\n",eff_sum,effect_sum);
        #if CFG_DEBUG_EFFECT_EN
		for(i = 0; i < len;i++)
		{
			buff[i]= 0xff;
		}
		#endif
		return FALSE;
	}
	
	for(i = 0; i < effect_sum;i++)
	{
		//DBG("list read_list= %d, local_list = %ld\n",eff_addr[i],effect_list_addr[i]);
		if( eff_addr[i]!= effect_list_addr[i])
		{
		    #if CFG_DEBUG_EFFECT_EN
		    for(i = 0; i < len;i++)
			{
			     buff[i]= 0xff;
			}
			#endif


			DBG("audio effect list not equal\n");
			return FALSE;
		}
	}
#if !CFG_MULTI_MUSIC_CHANNEL_SWITCH_EN
	if(line_channel != gCtrlVars.line_channel)
	{
		#if CFG_DEBUG_EFFECT_EN
		for(i = 0; i < len;i++)
		{
			buff[i]= 0xff;
		}
		#endif
		DBG("Line input channel not equal\n");
		return FALSE;
	}
#endif	
	if(mic_channel != gCtrlVars.mic_channel)
	{
	    #if CFG_DEBUG_EFFECT_EN
		for(i = 0; i < len;i++)
		{
			buff[i]= 0xff;
		}
		#endif
		DBG("Mic input channel not equal\n");
		return FALSE;
	}	
    
  	DBG("audio effect OK \n");
	return TRUE;

}

/*
****************************************************************
* 导入调音参数，并解析处理
*
*
****************************************************************
*/
void LoadAudioMode(uint16_t len,const uint8_t *buff)
{
	uint16_t cmd_len,packet_len,k;
	uint8_t end_code,Control;
	uint16_t position;

	StartWriteCmd= LOAD_BUSY;//disable init command pares,curr is local command,not PC command,
	
	memset(communic_buf ,0,sizeof(communic_buf));

	communic_buf_w = 0;

	position = 0;
	 
	while(position < len)
	{
		if ((buff[position] ==0xa5) && (buff[position+1] ==0x5a))
		{
			Control   = buff[position + 2];//

			packet_len = buff[position + 3];

			end_code   = buff[position + 4 + packet_len];//0x16

			if(end_code == 0x16)////end code ok
			{
				cmd_len = packet_len + 5;

				for(k = 0; k < cmd_len; k++)//get signal COMMAND
				{
					communic_buf[k]=buff[position++];
					//DBG("%02X ",communic_buf[k]);
				}
				if(Control < 0x80)
				{
					if(gCtrlVars.init_one_digital==1)/////仅切换模式时 ADC，DAC只设置一次，可以防止声音断一下
					{
				       Communication_Effect_Config(communic_buf[2], &communic_buf[4], communic_buf[3]);
					}
				}
				else
				{
				      Communication_Effect_Config(communic_buf[2], &communic_buf[4], communic_buf[3]);
					  GetAudioEffectMaxValue();
                      #if CFG_WATCH_DOG_EN
                      WDG_Feed();
                      #endif
				}

				for(k = 0; k < cmd_len; k++)//
				{
					communic_buf[k]=0;
				}

				communic_buf_w = 0;
			}
			else  // end code error
			{
				position++;
			}
	 	}
		else // serch start code....
		{
			position++;
		}
	}
	 
	StartWriteCmd= LOAD_IDLE;

	gCtrlVars.init_one_digital = 0;//若需要只初始化一次硬件参数，则需要恢复这个变量的调用（减小切换的POP声）

	gCtrlVars.AutoRefresh = 1;//////调音时模式发生改变，上位机会自动读取音效数据，1=允许上位读，0=不需要上位机读取


	
}
/*
****************************************************************
* USB导入调音参数，并解析处理
*
*
****************************************************************
*/
void UsbLoadAudioMode(uint16_t len,uint8_t *buff)
{
#if CFG_COMMUNICATION_BY_USB

	uint16_t cmd_len,packet_len,k;
	uint8_t end_code;
	uint16_t position;
	
	for(k = 0; k < sizeof(communic_buf); k++)//
	{
		communic_buf[k]=0;
	}
	
	communic_buf_w = 0;

	position = 0;
	 
	while(position < len)
	{
		if ((buff[position] ==0xa5) && (buff[position+1] ==0x5a))
		{
			packet_len = buff[position + 3];

			end_code   = buff[position + 4 + packet_len];//0x16

			if(end_code == 0x16)////end code ok
			{
				cmd_len = packet_len + 5;

				communic_buf_w = 0;

				for(k = 0; k < cmd_len; k++)//get signal COMMAND
				{
					communic_buf[k]=buff[position++];
				}

				//Communication_Effect_Process(cmd_len);
				Communication_Effect_Config(communic_buf[2], &communic_buf[4], communic_buf[3]);
                SaveAudioParamasToRam(&communic_buf[0],communic_buf[3] + 5);
				for(k = 0; k < cmd_len; k++)//
				{
					communic_buf[k]=0;
				}

				communic_buf_w = 0;
			}
			else  // end code error
			{
				position++;
			}
	 	}
		else // serch start code....
		{
			position++;
		}
	}
//++++++++++++++++++++++++++//	
#endif	
}

/*
****************************************************************
* 保存指定的调音参数组到flash指定位置
*
*
****************************************************************
*/
void SaveAudioParamasToRam(uint8_t *buf, uint32_t len)///debug
{
#if CFG_DEBUG_EFFECT_EN
	uint32_t i;

	uint32_t msg;
	uint8_t prg[4];

	static uint32_t position = 0;

	if(StartWriteCmd == SAVE_START)//start
	{
		DBG("Save to flash:....\n");

		StartWriteCmd = SAVE_RUNING;

		position = 0;

		for(i = 0; i < sizeof(Audio_mode_buff); i++)
		{
			Audio_mode_buff[i]= 0xff;
		}
	}
	else if(StartWriteCmd == SAVE_RUNING)//runing
	{
		for(i = 0; i < len; i++)
		{
			Audio_mode_buff[i + position]= buf[i];
		}	

		position  += len;

	}	
	else if(StartWriteCmd == SAVE_END)//end
	{
		WriteEffectParamas(buf[4]&0x07,&Audio_mode_buff[0]);

		StartWriteCmd= LOAD_IDLE;

		DBG("Save to flash ok\n");

		AudioDAC_ClearBuff();
	}
	else if(StartWriteCmd == LOAD_START)//////start load cfg file from SPI,and to PC,and updata to ram
	{
		msg = FUNC_ID_LOAD_EFFECT_MODE;

		prg[3] = buf[4]&0x07;

		SendMessage(&msg,&prg[0]);

		StartWriteCmd= LOAD_IDLE;

		DBG("Load from flash:....\n");
	}	
	else////parse delay????
	{
		position  = len;
	}
#endif
}
/*
****************************************************************
* 实时获取reverb相关参数，以此为电位器调节的最大值
* 只是在装载参数时，才可以获取最大值，用于调节参数
* 用户根据需要获取相应的值
****************************************************************
*/
void GetAudioEffectMaxValue(void)
{
	if(StartWriteCmd == LOAD_BUSY)//////
	{	
		gCtrlVars.max_chorus_wet               = gCtrlVars.chorus_unit.wet;
		
	    gCtrlVars.max_plate_reverb_roomsize    = gCtrlVars.plate_reverb_unit.decay;

		gCtrlVars.max_plate_reverb_wetdrymix   = gCtrlVars.plate_reverb_unit.wetdrymix;

		gCtrlVars.max_reverb_wet_scale         = gCtrlVars.reverb_unit.wet_scale;

		gCtrlVars.max_reverb_roomsize    = gCtrlVars.reverb_unit.roomsize_scale;
		
		gCtrlVars.max_echo_delay        = gCtrlVars.echo_unit.delay_samples;
	
        gCtrlVars.max_echo_depth        = gCtrlVars.echo_unit.attenuation;
	
		//gCtrlVars.ReverbRoom            = 0;

		gCtrlVars.max_dac0_dig_l_vol    = gCtrlVars.dac0_dig_l_vol;

		gCtrlVars.max_dac0_dig_r_vol    = gCtrlVars.dac0_dig_r_vol;

		gCtrlVars.max_dac1_dig_vol      = gCtrlVars.dac1_dig_vol;  

		gCtrlVars.max_reverb_pro_wet     = gCtrlVars.reverb_pro_unit.wet ;
		gCtrlVars.max_reverb_pro_erwet   = gCtrlVars.reverb_pro_unit.erwet ;

    }
}
#if CFG_AUDIO_EFFECT_SWITCH_FADEOUT_FADEIN_EN
/*
****************************************************************
* MIC音效模式切换时淡入淡出处理，改善POP声效果
*
*
****************************************************************
*/
void DigitalFadeProcess(int16_t *PcmIn, uint16_t n, uint32_t ChNum)
{
	if(gCtrlVars.fade_ctl)///Fade_gain_control_unit
	{
		gCtrlVars.Fade_gain_control_unit.enable = 1;
		gCtrlVars.Fade_gain_control_unit.mute   = 0;

		if(gCtrlVars.fade_ctl == 0x81)////Fade Out
		{
			gCtrlVars.fade_ctl &= 0x0f;
			gCtrlVars.Fade_gain_control_unit.gain = 4096;///0db

		}

		if(gCtrlVars.fade_ctl == 0x82)////Fade In
		{
			gCtrlVars.fade_ctl &= 0x0f;
			gCtrlVars.Fade_gain_control_unit.gain = 0;///mute
		}

		if(gCtrlVars.fade_ctl == 0x84)////Fade Ining
		{
			gCtrlVars.fade_ctl = 0x01;
		}

		if(gCtrlVars.fade_ctl == 1)////Fade Out
		{
			if(gCtrlVars.Fade_gain_control_unit.gain)
			{
			    if(gCtrlVars.Fade_gain_control_unit.gain >= FADEOUT_FADEIN_STEP)
		    	{
					gCtrlVars.Fade_gain_control_unit.gain-=FADEOUT_FADEIN_STEP;/////淡出速度可调
		    	}
			}
			if(gCtrlVars.Fade_gain_control_unit.gain == 0)
			{
				gCtrlVars.fade_ctl = 0x82;
				AudioEffectModeSel(gCtrlVars.EffectMode);
			}
		}

		if(gCtrlVars.fade_ctl == 2)////Fade In
		{
			gCtrlVars.Fade_gain_control_unit.gain+=FADEOUT_FADEIN_STEP;/////淡入速度可调

			if(gCtrlVars.Fade_gain_control_unit.gain > 4095)
			{
				gCtrlVars.fade_ctl = 0;
				gCtrlVars.Fade_gain_control_unit.enable = 0;
			}
		}
		AudioEffectPregainApply(&gCtrlVars.Fade_gain_control_unit, PcmIn, PcmIn, n, ChNum);
	}
}
#endif

#if CFG_EQ_MODE_SET_EN
const unsigned char Eq_Flat[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x00, 0x00,/*reserved*/
0x00, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0x0A, 0x01,/*filter1_f0*/
0xB7, 0x00,/*filter1_Q*/
0x28, 0x05,/*filter1_gain*/
0x00, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0x60, 0x00,/*filter2_f0*/
0xFA, 0x01,/*filter2_Q*/
0x0D, 0xFB,/*fi lter2_gain*/
0x00, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0x1F, 0x02,/*filter3_f0*/
0x4D, 0x01,/*filter3_Q*/
0x1A, 0x04,/*filter3_gain*/
0x00, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xA3, 0x09,/*filter4_f0*/
0x06, 0x03,/*filter4_Q*/
0x17, 0x01,/*filter4_gain*/
0x00, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0xE3, 0x29,/*filter5_f0*/
0xCD, 0x02,/*filter5_Q*/
0x45, 0xFE,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

const unsigned char Eq_Classic[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x04, 0x00,/*reserved*/
0x01, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0x0A, 0x01,/*filter1_f0*/
0xB7, 0x00,/*filter1_Q*/
0x28, 0x05,/*filter1_gain*/
0x01, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0x7F, 0x02,/*filter2_f0*/
0x42, 0x03,/*filter2_Q*/
0x89, 0xFB,/*fi lter2_gain*/
0x01, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0xEE, 0x06,/*filter3_f0*/
0xD5, 0x02,/*filter3_Q*/
0x2E, 0xFE,/*filter3_gain*/
0x00, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xE8, 0x03,/*filter4_f0*/
0xD4, 0x02,/*filter4_Q*/
0x00, 0x00,/*filter4_gain*/
0x01, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0xB2, 0x0E,/*filter5_f0*/
0x25, 0x01,/*filter5_Q*/
0x91, 0x04,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

const unsigned char Eq_Pop[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x03, 0x00,/*reserved*/
0x01, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0xCC, 0x00,/*filter1_f0*/
0x26, 0x02,/*filter1_Q*/
0xD8, 0xFD,/*filter1_gain*/
0x01, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0xCA, 0x02,/*filter2_f0*/
0xE8, 0x01,/*filter2_Q*/
0xF9, 0x03,/*fi lter2_gain*/
0x00, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0x1F, 0x02,/*filter3_f0*/
0x4D, 0x01,/*filter3_Q*/
0x1A, 0x04,/*filter3_gain*/
0x00, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xA3, 0x09,/*filter4_f0*/
0x06, 0x03,/*filter4_Q*/
0x17, 0x01,/*filter4_gain*/
0x01, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0x33, 0x12,/*filter5_f0*/
0x8C, 0x02,/*filter5_Q*/
0x0F, 0xFE,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

const unsigned char Eq_Rock[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x03, 0x00,/*reserved*/
0x01, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0xC4, 0x00,/*filter1_f0*/
0x84, 0x02,/*filter1_Q*/
0xD1, 0x04,/*filter1_gain*/
0x01, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0xC3, 0x03,/*filter2_f0*/
0x33, 0x01,/*filter2_Q*/
0xBA, 0xFD,/*fi lter2_gain*/
0x00, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0x1F, 0x02,/*filter3_f0*/
0x4D, 0x01,/*filter3_Q*/
0x1A, 0x04,/*filter3_gain*/
0x00, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xA3, 0x09,/*filter4_f0*/
0x06, 0x03,/*filter4_Q*/
0x17, 0x01,/*filter4_gain*/
0x01, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0x8F, 0x0D,/*filter5_f0*/
0x14, 0x02,/*filter5_Q*/
0x40, 0x04,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

const unsigned char Eq_Jazz[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x03, 0x00,/*reserved*/
0x01, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0x70, 0x00,/*filter1_f0*/
0x85, 0x01,/*filter1_Q*/
0x2C, 0x04,/*filter1_gain*/
0x00, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0xC3, 0x03,/*filter2_f0*/
0x33, 0x01,/*filter2_Q*/
0xBA, 0xFD,/*fi lter2_gain*/
0x01, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0x93, 0x02,/*filter3_f0*/
0x6A, 0x03,/*filter3_Q*/
0xE5, 0xFD,/*filter3_gain*/
0x00, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xA3, 0x09,/*filter4_f0*/
0x06, 0x03,/*filter4_Q*/
0x17, 0x01,/*filter4_gain*/
0x01, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0xD8, 0x11,/*filter5_f0*/
0x25, 0x02,/*filter5_Q*/
0xC2, 0x02,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

const unsigned char Eq_VocalBoost[] = 
{
0xA5, 0x5A,/*sync head*/
0x9B,/*EQ*/
0x6B,/*length*/
0xFF,/*all parameter*/
0x01, 0x00,/*enable*/
0x00, 0x00,/*pregain*/
0x04, 0x00,/*reserved*/
0x00, 0x00,/*filter1_enable*/
0x01, 0x00,/*filter1_type*/
0x0A, 0x01,/*filter1_f0*/
0xB7, 0x00,/*filter1_Q*/
0x28, 0x05,/*filter1_gain*/
0x01, 0x00,/*filter2_enable*/
0x00, 0x00,/*filter2_type*/
0x60, 0x00,/*filter2_f0*/
0xFA, 0x01,/*filter2_Q*/
0x0D, 0xFB,/*fi lter2_gain*/
0x01, 0x00,/*filter3_enable*/
0x00, 0x00,/*filter3_type*/
0x1F, 0x02,/*filter3_f0*/
0x4D, 0x01,/*filter3_Q*/
0x1A, 0x04,/*filter3_gain*/
0x01, 0x00,/*filter4_enable*/
0x00, 0x00,/*filter4_type*/
0xA3, 0x09,/*filter4_f0*/
0x06, 0x03,/*filter4_Q*/
0x17, 0x01,/*filter4_gain*/
0x01, 0x00,/*filter5_enable*/
0x02, 0x00,/*filter5_type*/
0xE3, 0x29,/*filter5_f0*/
0xCD, 0x02,/*filter5_Q*/
0x45, 0xFE,/*filter5_gain*/
0x00, 0x00,/*filter6_enable*/
0x00, 0x00,/*filter6_type*/
0xE8, 0x03,/*filter6_f0*/
0xD4, 0x02,/*filter6_Q*/
0x00, 0x00,/*filter6_gain*/
0x00, 0x00,/*filter7_enable*/
0x00, 0x00,/*filter7_type*/
0xE8, 0x03,/*filter7_f0*/
0xD4, 0x02,/*filter7_Q*/
0x00, 0x00,/*filter7_gain*/
0x00, 0x00,/*filter8_enable*/
0x00, 0x00,/*filter8_type*/
0xE8, 0x03,/*filter8_f0*/
0xD4, 0x02,/*filter8_Q*/
0x00, 0x00,/*filter8_gain*/
0x00, 0x00,/*filter9_enable*/
0x00, 0x00,/*filter9_type*/
0xE8, 0x03,/*filter9_f0*/
0xD4, 0x02,/*filter9_Q*/
0x00, 0x00,/*filter9_gain*/
0x00, 0x00,/*filter10_enable*/
0x00, 0x00,/*filter10_type*/
0xE8, 0x03,/*filter10_f0*/
0xD4, 0x02,/*filter10_Q*/
0x00, 0x00,/*filter10_gain*/
0x16,/*sync tail*/
};

#define EQ_PARAM_LEN   sizeof(Eq_Flat)
/*
****************************************************************
* EQ Mode调节函数
*
*
****************************************************************
*/
void LoadEqMode(uint8_t *buff)
{
	uint16_t cmd_len,packet_len,k;
	uint8_t end_code,Control;
	uint16_t position;

	memset(communic_buf ,0,sizeof(communic_buf));

	communic_buf_w = 0;

	position = 0;

	while(position < EQ_PARAM_LEN)
	{
		if ((buff[position] ==0xa5) && (buff[position+1] ==0x5a))
		{
			Control   = buff[position + 2];//

			packet_len = buff[position + 3];

			end_code   = buff[position + 4 + packet_len];//0x16

			if(end_code == 0x16)////end code ok
			{
				cmd_len = packet_len + 5;

				for(k = 0; k < cmd_len; k++)//get signal COMMAND
				{
					communic_buf[k]=buff[position++];
					//DBG("%02X ",communic_buf[k]);
				}
				
				Communication_Effect_Config(communic_buf[2], &communic_buf[4], communic_buf[3]);

				for(k = 0; k < cmd_len; k++)//
				{
					communic_buf[k]=0;
				}

				communic_buf_w = 0;
			}
			else  // end code error
			{
				position++;
			}
	 	}
		else // serch start code....
		{
			position++;
		}
	}	
}

/*
****************************************************************
* EQ Mode调节函数
*
*
****************************************************************
*/
void EqModeSet(uint8_t EqMode)
{
    switch(EqMode)
	{
		case EQ_MODE_FLAT:
			LoadEqMode(&Eq_Flat[0]);
			break;
		case EQ_MODE_CLASSIC:
			LoadEqMode(&Eq_Classic[0]);
			break;
		case EQ_MODE_POP:
			LoadEqMode(&Eq_Pop[0]);
			break;	
		case EQ_MODE_ROCK:
			LoadEqMode(&Eq_Rock[0]);
			break;
		case EQ_MODE_JAZZ:
			LoadEqMode(&Eq_Jazz[0]);
			break;
		case EQ_MODE_VOCAL_BOOST:
			LoadEqMode(&Eq_VocalBoost[0]);
			break;			
	}
}
#endif

/*
****************************************************************
* DMA FiFo Auto Associate调节函数
*
*
****************************************************************
*/
#if CFG_DYNAMIC_DMA_FIFO_EN
#include "usb_audio_interface.h"
#if CFG_USB_IN_EN
extern usb_audio_unit	usb_audio_get;
#endif
#if CFG_USB_OUT_EN
extern usb_audio_unit	usb_audio_put;
#endif

extern int32_t *pcm_buf_1;
extern int32_t *pcm_buf_2;
extern int32_t *pcm_buf_3;
extern int32_t *pcm_buf_4;

uint32_t           DmaDynamicStartAddr = (uint32_t)CFG_AUDIO_EFFECT_START_ADDR;

void DmaFiFoAutoAssociate(void)
{
	uint16_t i;
    uint32_t Len;
	uint32_t dma_fifo,dma_fifo1;
	uint32_t real_fifo_size,real_fifo_size1;

	if(gCtrlVars.SamplesPerFrameBak == gCtrlVars.SamplesPerFrame)
	{
		return;
	}
	gCtrlVars.SamplesPerFrameBak = gCtrlVars.SamplesPerFrame;
	DmaDynamicStartAddr = (uint32_t)CFG_AUDIO_EFFECT_START_ADDR;
	#if CFG_REMINDSOUND_EN
	Len = CFG_AUDIO_EFFECT_END_ADDR-DECODER_BUF_SIZE - DmaDynamicStartAddr;
	#else
	Len = CFG_AUDIO_EFFECT_END_ADDR - DmaDynamicStartAddr;
	#endif
	memset((void *)DmaDynamicStartAddr,0,Len);
	DBG("Remain RAM = %08ld\n",Len);

	if(gCtrlVars.pga0_att)
	{
		AudioADC_AnaInit();
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;
		AudioADC_DigitalInit(ADC0_MODULE, SupportSampleRateList[gCtrlVars.adc0_sample_rate], (void *)dma_fifo, real_fifo_size);
		AudioADC_ChannelSet();
	}

	if(gCtrlVars.pga1_att)
	{
		AudioADC_AnaInit();
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;
		AudioADC_DigitalInit(ADC1_MODULE, SupportSampleRateList[gCtrlVars.adc1_sample_rate], (void *)dma_fifo, real_fifo_size);
		AudioADC_ChannelSet();
	}
    if(gCtrlVars.dac0_en || gCtrlVars.dac1_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;
		dma_fifo1 = DmaDynamicStartAddr;
		real_fifo_size1 = gCtrlVars.SamplesPerFrame*4*1;
		DmaDynamicStartAddr += real_fifo_size1;

        if(gCtrlVars.dac0_en && gCtrlVars.dac1_en)
    	{
    		AudioDAC_Init(ALL, SupportSampleRateList[gCtrlVars.sys_sample_rate], (void *)dma_fifo, real_fifo_size, (void *)dma_fifo1, real_fifo_size1);
    	}
		else if(gCtrlVars.dac0_en)
    	{
    		AudioDAC_Init(DAC0, SupportSampleRateList[gCtrlVars.sys_sample_rate], (void *)dma_fifo, real_fifo_size, (void *)dma_fifo1, real_fifo_size1);
    	}
		else if(gCtrlVars.dac1_en)
    	{
    		AudioDAC_Init(DAC1, SupportSampleRateList[gCtrlVars.sys_sample_rate], (void *)dma_fifo, real_fifo_size, (void *)dma_fifo1, real_fifo_size1);
    	}
	}
	#if CFG_I2S0_IN_EN
	if(gCtrlVars.i2s0_rx_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;		
		AudioI2S_RXInit(I2S0_MODULE,SupportSampleRateList[gCtrlVars.i2s0_sample_rate], (void *)dma_fifo, real_fifo_size);
	}
	#endif
	#if CFG_I2S0_OUT_EN
    if(gCtrlVars.i2s0_tx_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;		
		AudioI2S_TXInit(I2S0_MODULE,SupportSampleRateList[gCtrlVars.i2s0_sample_rate], (void *)dma_fifo, real_fifo_size);
		#if CFG_I2S0_OUT_SOFT_MICRO_ADJUST_EN
		I2S0_OutSraInit();
		#endif
	}
	#endif
	#if CFG_I2S1_IN_EN
	if(gCtrlVars.i2s1_rx_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;		
		AudioI2S_RXInit(I2S1_MODULE,SupportSampleRateList[gCtrlVars.i2s1_sample_rate], (void *)dma_fifo, real_fifo_size);
	}
	#endif	
	#if CFG_I2S1_OUT_EN
	if(gCtrlVars.i2s1_tx_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*2;
		DmaDynamicStartAddr += real_fifo_size;		
		AudioI2S_TXInit(I2S1_MODULE,SupportSampleRateList[gCtrlVars.i2s1_sample_rate], (void *)dma_fifo, real_fifo_size);	    
	}
	#endif
	#if CFG_SPDIF_IN_EN || CFG_SPDIF_OUT_EN
	if(gCtrlVars.spdif_en)
	{
		dma_fifo = DmaDynamicStartAddr;
		real_fifo_size = gCtrlVars.SamplesPerFrame*4*4;
		DmaDynamicStartAddr += real_fifo_size;		
		#if CFG_SPDIF_IN_EN
		AudioSPDIF_RXInit(gCtrlVars.spdif_in_gpio,SupportSampleRateList[gCtrlVars.spdif_sample_rate], (void *)dma_fifo, real_fifo_size);		
		#endif
		#if CFG_SPDIF_OUT_EN
		AudioSPDIF_TXInit(gCtrlVars.spdif_in_gpio,SupportSampleRateList[gCtrlVars.spdif_sample_rate], (void *)dma_fifo, real_fifo_size);	
		#endif
	}  
	#endif
    //usb module
	#if CFG_USB_EN
	#if CFG_USB_IN_EN
	dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4*3;
	DmaDynamicStartAddr += real_fifo_size;
	usb_audio_get.fifo_addr = (uint8_t *)dma_fifo;
	usb_audio_get.fifo_size = real_fifo_size;
	#endif
	#if CFG_USB_OUT_EN
	dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4*3;
	DmaDynamicStartAddr += real_fifo_size;
	usb_audio_put.fifo_addr	= (uint8_t *)dma_fifo;
	usb_audio_put.fifo_size = real_fifo_size;
    #endif

	#if CFG_USB_IN_EN
	AudioUSB_GetInit(&usb_audio_get);
	#endif
	#if CFG_USB_OUT_EN
	AudioUSB_PutInit(&usb_audio_put);	
	#endif
	#endif   

    dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4;
	DmaDynamicStartAddr += real_fifo_size;
	pcm_buf_1	= (uint8_t *)dma_fifo;
	
	dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4;
	DmaDynamicStartAddr += real_fifo_size;
	pcm_buf_2	= (uint8_t *)dma_fifo;

	dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4;
	DmaDynamicStartAddr += real_fifo_size;
	pcm_buf_3	= (uint8_t *)dma_fifo;

	dma_fifo = DmaDynamicStartAddr;
	real_fifo_size = gCtrlVars.SamplesPerFrame*4;
	DmaDynamicStartAddr += real_fifo_size;
	pcm_buf_4	= (uint8_t *)dma_fifo;
	
	if(gCtrlVars.pga0_att)
    {
		AudioADC_Enable(ADC0_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC0_RX);
    }
    
    if(gCtrlVars.pga1_att)
    {
		AudioADC_Enable(ADC1_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_AUDIO_ADC1_RX);
    }

}
#endif
/*
****************************************************************
* DMA Channel auto associate调节函数
*
*
****************************************************************
*/
extern uint8_t DmaChannelMap[28];
void DmaChannelAutoAssociate(void)
{
	uint8_t i,dma_count,adc0_en,adc1_en,timer3_en,timer4_en;
	uint8_t uart0_tx_en,uart0_rx_en,uart1_tx_en,uart1_rx_en;
	uint32_t mic_ch,line_ch;
	
	dma_count = 0;
	for(i = 0; i < sizeof(DmaChannelMap); i++)
	{
		DmaChannelMap[i]= 0xff;
	}
	
	timer3_en = 0;
	timer4_en = 0;
	adc0_en = 0;
	adc1_en = 0;
	uart0_tx_en = 0;
	uart0_rx_en = 0;
	uart1_tx_en = 0;
	uart1_rx_en = 0;
	
    #if CFG_OPTICAL_EN || CFG_COAXIAL_EN || CFG_HDMI_IN_EN
	#if CFG_HDMI_IN_EN
	DmaChannelMap[2] = dma_count++;
    if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
	#endif
    DmaChannelMap[8] = dma_count++;
    if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    #endif

	#if CFG_SPDIF_OUT_EN
	DmaChannelMap[9] = dma_count++;
    if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
	#endif

	#if  CFG_IR_KEY_EN
    #if IR_PIN < 4
	timer3_en = 1;
    #else
	timer4_en = 1;
    #endif
    #endif

	#if CFG_DMA_GPIO_SIMULATE_PWM_EN
	timer3_en = 1;
	#endif
	
    if(timer3_en)//PERIPHERAL_ID_TIMER3
    {
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate\n");
			return;
		}
		DmaChannelMap[2] = dma_count++;
    }
	
	if(timer4_en)//PERIPHERAL_ID_TIMER4
    {
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[17] = dma_count++;

    }
    if(uart0_rx_en)
	{
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[5] = dma_count++;
	}

    if(uart0_tx_en)
	{
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[12] = dma_count++;
	}
	
    #if CFG_COMMUNICATION_BY_UART
	uart1_tx_en = 1;
	uart1_rx_en = 1;
    #endif

    if(uart1_rx_en)
	{
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[13] = dma_count++;
	}

    if(uart1_tx_en)
    {
		if(dma_count > 8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[14] = dma_count++;
    }   

    mic_ch = AUDIO_CH_MIC;
    line_ch = AUDIO_CH_LINEIN;
    adc0_en = 0;

    if(mic_ch & (BIT(INPUT_MIC3)|BIT(INPUT_MIC4)))
    {
    	adc0_en = 1;
    }
	
    if(line_ch & (BIT(INPUT_LINEIN1_LEFT)|BIT(INPUT_LINEIN1_RIGHT)))//
    {
    	adc0_en = 1;
    }

    if(adc0_en == 0)
    {
        if(line_ch & (BIT(INPUT_LINEIN2_LEFT)|BIT(INPUT_LINEIN2_RIGHT)))//
        {
        	adc0_en = 1;
        }
    }

    if(adc0_en)
    {
		if(dma_count>8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[18] = dma_count++;
    }

    mic_ch = AUDIO_CH_MIC;
    line_ch = AUDIO_CH_LINEIN;
    adc1_en = 0;
    if(mic_ch & (BIT(INPUT_MIC1)|BIT(INPUT_MIC2)))
    {
    	adc1_en = 1;
    }
    if(line_ch & (BIT(INPUT_LINEIN3_LEFT)|BIT(INPUT_LINEIN3_RIGHT)))
    {
    	adc1_en = 1;
    }

    if(line_ch & (BIT(INPUT_FMIN_LEFT)|BIT(INPUT_FMIN_RIGHT)))
    {
    	adc1_en = 1;
    }

    if(adc1_en == 0)
    {
        if(line_ch & (BIT(INPUT_LINEIN2_LEFT)|BIT(INPUT_LINEIN2_RIGHT)))
        {
        	adc1_en = 1;
        }
    }

    if(adc1_en)
    {
		if(dma_count>8)
		{
			DBG("DMA NOT Associate");
			return;
		}
		DmaChannelMap[19] = dma_count++;
    }

    #if  CFG_DAC0_EN
    if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[20] = dma_count++;
    #endif

    #if  CFG_DAC1_EN
	if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[21] = dma_count++;
    #endif
	
    #if CFG_I2S0_OUT_EN
	if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[22] = dma_count++;

    #endif

    #if CFG_I2S0_IN_EN
	if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[23] = dma_count++;

    #endif

    #if CFG_I2S1_OUT_EN
	if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[24] = dma_count++;
    #endif

    #if CFG_I2S1_IN_EN
	if(dma_count > 8)
	{
		DBG("DMA NOT Associate");
		return;
	}
    DmaChannelMap[25] = dma_count++;
    #endif
}

#if CFG_HW_PWM_EN
void HwPwmInit(void)
{
	gCtrlVars.hw_pwm_param.CounterMode   		= PWM_COUNTER_MODE_DOWN;
	gCtrlVars.hw_pwm_param.OutputType    		= PWM_OUTPUT_SINGLE_1;
	gCtrlVars.hw_pwm_param.DMAReqEnable  		= PWM_REQ_INTERRUPT_MODE;
	gCtrlVars.hw_pwm_param.FreqDiv				= 120000000 / 3000000;//16000;//12000; //120M系统频率下 12000 = 100us
	gCtrlVars.hw_pwm_param.Duty			        = 0;	

	//GPIO Config
	PWM_GpioConfig(TIMER3_PWM_A23_A27_B0_B2, 1, PWM_IO_MODE_OUT);//A27

	//PWM Config
	PWM_Config(TIMER3,&gCtrlVars.hw_pwm_param);

	PWM_Enable(TIMER3);
}

void HwPwmConfig(uint16_t Pwm_Duty)
{
	PWM_IOCTRL_ARG arg;	
    if(Pwm_Duty == 0)
	{
		PWM_GpioConfig(TIMER3_PWM_A23_A27_B0_B2, 1, PWM_IO_MODE_NONE);//A27

		GPIO_RegOneBitClear(GPIO_A_IE, GPIO_INDEX27);
		GPIO_RegOneBitSet(GPIO_A_OE, GPIO_INDEX27);
		GPIO_RegOneBitClear(GPIO_A_OUT, GPIO_INDEX27);
	}
	else
	{
		PWM_Disable(TIMER3);
		PWM_GpioConfig(TIMER3_PWM_A23_A27_B0_B2, 1, PWM_IO_MODE_OUT);//A27
		gCtrlVars.hw_pwm_param.Duty = Pwm_Duty;
		arg.Duty = gCtrlVars.hw_pwm_param.Duty;
		PWM_IOCTRl(TIMER3,OUTPUT_DUTY_UPDATE,&arg);
		PWM_Config(TIMER3,&gCtrlVars.hw_pwm_param);
		PWM_Enable(TIMER3);
	}
}
#endif

#if CFG_DMA_GPIO_SIMULATE_PWM_EN

uint32_t  DmaTestLen;

#define DUTY 10   //
#define FIFTY_KHZ 2  //在此例程里Timer3定时2US时，GPIO翻转速率为50KHz，客户可以根据自己需要做调整，但必须注意Timer3定时单位是US
#define BUFFER_LEN    1000//注意这个长度必须是10的整数倍
#define ADR_GPIO_A_OUT     (0x40010004) //用户勿动
#define ADR_GPIO_B_OUT     (0x4001003C)//用户勿动
#define REG_TIMER3_CTRL    *(volatile uint32_t*)0x4002F000//用户勿动
#define GPIO_PORTA_MASK   GPIO_A_END
#define GPIO_PORTB_MASK   GPIO_B_END
static uint32_t  DmaTestBuffer[BUFFER_LEN];
	
void DmaTimerCircleMode_GPIO_Init(void)
{
	//Timer3触发DMA Request， 将memory数据toggle到A0~A31
	//可以从示波器上A0~A31上看到方波，周期为TIMER3定时的2倍

	uint32_t i  =0,j = 0, counter = 0;
	DMA_CONFIG DMAConfig;
	uint32_t DmaTestLen;

	GPIO_RegBitsSet(GPIO_PORTA_MASK, (~(GPIO_R_PWM|GPIO_G_PWM|GPIO_B_PWM)));
	//DMA_ChannelAllocTableSet(DmaChannelMap);
    memset(DmaTestBuffer,   0, sizeof(DmaTestBuffer));

	GPIO_RegBitsClear(GPIO_A_IE, (GPIO_R_PWM|GPIO_G_PWM|GPIO_B_PWM));
	GPIO_RegBitsSet(GPIO_A_OE, (GPIO_R_PWM|GPIO_G_PWM|GPIO_B_PWM));

	DmaTestLen = BUFFER_LEN*4;

	DMAConfig.Dir = DMA_CHANNEL_DIR_MEM2PERI;
	DMAConfig.Mode = DMA_CIRCULAR_MODE;
	DMAConfig.SrcAddress = (uint32_t)DmaTestBuffer;
	DMAConfig.SrcDataWidth = DMA_SRC_DWIDTH_WORD;
	DMAConfig.SrcAddrIncremental = DMA_SRC_AINCR_SRC_WIDTH;
	DMAConfig.DstAddress = ADR_GPIO_A_OUT;
	DMAConfig.DstDataWidth = DMA_DST_DWIDTH_WORD;
	DMAConfig.DstAddrIncremental = DMA_DST_AINCR_NO;
	DMAConfig.BufferLen = DmaTestLen;
	DMA_TimerConfig(PERIPHERAL_ID_TIMER3, &DMAConfig);

	DMA_CircularWritePtrSet(PERIPHERAL_ID_TIMER3, DmaTestLen+4);//将写指针指向buffer外
	DMA_ChannelEnable(PERIPHERAL_ID_TIMER3);

	Timer_Config(TIMER3, FIFTY_KHZ, 0);
	Timer_Start(TIMER3);
	REG_TIMER3_CTRL |= (1<<8);
}

void DmaTimerCircleMode_GPIO_Config(uint32_t R_Duty, uint32_t G_Duty,  uint32_t B_Duty)
{
	//Timer3触发DMA Request， 将memory数据toggle到A0~A31
	//可以从示波器上A0~A31上看到方波，周期为TIMER3定时的2倍

	uint32_t i  =0,j = 0, counter = 0;

    memset(DmaTestBuffer,   0, sizeof(DmaTestBuffer));
	//注意不能使用两个相邻的IO
//	////////两个IO的初始的第一个高电平一样的情况下：GPIO_A28占空比30%；GPIO_A30占空比70%/////////
	for(i = 0; i < BUFFER_LEN ; i+=10)
	{
		counter = R_Duty/10;
		if(counter > 10)
		{
			counter = 10;
		}
		for(j = 0; j < counter; j++)
		{
			DmaTestBuffer[i+j] |= GPIO_R_PWM;
		}
		counter = G_Duty/10;
		if(counter > 10)
		{
			counter = 10;
		}
		for(j = 0; j < counter; j++)
		{
			DmaTestBuffer[i+j] |= GPIO_G_PWM;
		}
		counter = B_Duty/10;
		if(counter > 10)
		{
			counter = 10;
		}
		for(j = 0; j < counter; j++)
		{
			DmaTestBuffer[i+j] |= GPIO_B_PWM;
		}
	}

	////////GPIO_A29比GPIO_A28延迟一个脉冲周期起来的情况下：GPIO_A28占空比30%；GPIO_A30占空比70%/////////
	//for(i = 0; i < BUFFER_LEN ; i+=DUTY)
	//{
	//	DmaTestBuffer[i]   = (GPIO_INDEX28);
	//	DmaTestBuffer[i+1] = (GPIO_INDEX28|GPIO_INDEX30);
	//	DmaTestBuffer[i+2]   = (GPIO_INDEX28|GPIO_INDEX30);
	//	DmaTestBuffer[i+3]   = (GPIO_INDEX30);
	//	DmaTestBuffer[i+4]   = (GPIO_INDEX30);
	//	DmaTestBuffer[i+5]   = (GPIO_INDEX30);
	//	DmaTestBuffer[i+6]   = (GPIO_INDEX30);
	//	DmaTestBuffer[i+7]   = (GPIO_INDEX30);
	//}
}
#endif
/*
****************************************************************
* 音效模块使能列表打印信息
*
*
****************************************************************
*/
void printf_effect_status(void)
{
#if 0//CFG_DEBUG_EN
     uint16_t i;
	//audio effect
	
	  if(gCtrlVars.auto_tune_unit.enable)      {DBG("auto tune en\n");}
	  if(gCtrlVars.echo_unit.enable	)	       {DBG("echo en\n");}
	  if(gCtrlVars.pitch_shifter_unit.enable)  {DBG("pitch en\n");}
	  if(gCtrlVars.pitch_shifter_pro_unit.enable)  {DBG("pitch pro en\n");}
	  if(gCtrlVars.voice_changer_unit.enable)  {DBG("voice changer en\n");}
	  if(gCtrlVars.voice_changer_pro_unit.enable)  {DBG("voice changer pro en\n");}
	  if(gCtrlVars.reverb_unit.enable)		   {DBG("reverb en\n");}
	  if(gCtrlVars.freq_shifter_unit.enable)   {DBG("freq shifter en\n");}
	  if(gCtrlVars.howling_dector_unit.enable) {DBG("holling en\n");}
	  if(gCtrlVars.voice_changer_pro_unit.enable){DBG("Reverb Pro en\n");}

	  if(gCtrlVars.plate_reverb_unit.enable){DBG("Plate Reverb en\n");}
	  if(gCtrlVars.MicAudioSdct_unit.enable){DBG("Mic Sdct_unit en\n");}
	  if(gCtrlVars.MusicAudioSdct_unit.enable){DBG("Music Sdct_unit en\n");}

	  if(gCtrlVars.music_threed_unit.enable){DBG("music_threed_unit en\n");}
	  if(gCtrlVars.music_vb_unit.enable){DBG("music_vb_unit en\n");}
	  if(gCtrlVars.music_vb_classic_unit.enable){DBG("music_vb_classic_unit en\n");}

									 
	  //expand
	  for(i = 0; i < sizeof(expander_unit_aggregate)/sizeof(expander_unit_aggregate[0]); i++)
		{
		  if(expander_unit_aggregate[i]->enable) {DBG("expander %d en\n",(i+1));}
		}
	 //drc
	  for(i = 0; i < sizeof(drc_unit_aggregate)/sizeof(drc_unit_aggregate[0]); i++)
		{
	
		 if(drc_unit_aggregate[i]->enable) {DBG("drc %d en\n",(i+1));}
		}
	  //eq
	  for(i = 0; i < sizeof(eq_unit_aggregate)/sizeof(eq_unit_aggregate[0]); i++)
		{
		   if(eq_unit_aggregate[i]->enable) {DBG("eq %d en\n",(i+1));}
		}
	  //gain control
	  for(i = 0; i < sizeof(gain_unit_aggregate)/sizeof(gain_unit_aggregate[0]); i++)
	   {
		 if(gain_unit_aggregate[i]->enable)  {DBG("gain %d en\n",(i+1));}
	   }
#endif
}

/*
****************************************************************
* 音效模块内存分配列表打印信息
*
*
****************************************************************
*/
void PrintMallocMap(uint8_t mode, void *map,uint32_t RamSize)
{
#if CFG_DEBUG_EN
	uint8_t k,i;

	k = 0;
	
	for(i = 0; i < effect_sum; i++)
	{
	    #if CFG_AUDIO_EFFECT_MIC_AEC_EN
		if(mode == EFF_AEC)//aec
		{
			if(effect_list[i] == EFF_AEC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Aec %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		#endif
		if(mode == EFF_PCM_DELAY)//pcm delay
		{
			if(effect_list[i] == EFF_PCM_DELAY) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("PcmDelay %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		if(mode == EFF_EXCITER)//exciter
		{
			if(effect_list[i] == EFF_EXCITER) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Exciter %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		if(mode == EFF_EXPAND)//expand
		{
			if(effect_list[i] == EFF_EXPAND) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Expander %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_FREQ_SHIFTER)
		{
			if(effect_list[i] == EFF_FREQ_SHIFTER) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Freq Shifter %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_HOWLING_DETOR)
		{
			if(effect_list[i] == EFF_HOWLING_DETOR) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Hollowin Dector %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_SILENCE_DETOR)
		{
			k = 1;
			DBG("SILENCE Dector %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
			return;
		}		   
		else if(mode== EFF_PITCH)
		{
			if(effect_list[i] == EFF_PITCH) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Pitch Shifter %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}	
		else if(mode== EFF_PITCH_PRO)
		{
			if(effect_list[i] == EFF_PITCH_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Pitch Shifter Pro %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}	
		else if(mode== EFF_AUTO_TUNE)
		{
			if(effect_list[i] == EFF_AUTO_TUNE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Auto Tune %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_VOICE_CHANGE)
		{
			if(effect_list[i] == EFF_VOICE_CHANGE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Voice Change %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}	
		else if(mode == EFF_VOICE_CHANGE_PRO)
		{
			if(effect_list[i] == EFF_VOICE_CHANGE_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Voice Change Pro %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}	
		else if(mode == EFF_ECHO)
		{
			if(effect_list[i] == EFF_ECHO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Echo %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}	
		else if(mode == EFF_REVERB)
		{
			if(effect_list[i] == EFF_REVERB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Reverb %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_PLATE_REVERB)
		{
			if(effect_list[i] == EFF_PLATE_REVERB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("PlateReverb %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_REVERB_PRO)
		{
			if(effect_list[i] == EFF_REVERB_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Reverb Pro %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}		
		else if(mode == EFF_VOCALCUT)
		{
			if(effect_list[i] == EFF_VOCALCUT) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("VocalCut %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_VOCALREMOVE)
		{
			if(effect_list[i] == EFF_VOCALREMOVE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("VocalRemove %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_CHORUS)
		{
			if(effect_list[i] == EFF_CHORUS) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Chorus %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_THREED)
		{
			if(effect_list[i] == EFF_THREED) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("ThreeD %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_VB)
		{
			if(effect_list[i] == EFF_VB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("VB %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_VBCLASSIC)
		{
			if(effect_list[i] == EFF_VBCLASSIC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("VBClassic %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_DRC)
		{
			if(effect_list[i] == EFF_DRC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("DRC  %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}
		else if(mode == EFF_EQ)
		{
			if(effect_list[i] == EFF_EQ) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("EQ %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}		   
		else if(mode == EFF_GAIN)
		{
			if(effect_list[i] == EFF_GAIN) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("GAIN %d  Malloc OK, Ram Size = %ld Start Addr =  %02lx\n",k,RamSize,DynamicStartAddr);
				return;
			}
		}		   
		else
		{
			return;
		}
	}	
#endif   
}
/*
****************************************************************
* 音效模块内存分配失败列表打印信息
*
*
****************************************************************
*/
void PrintMallocFail(uint8_t mode, void *map,uint32_t RamSize)
{
#if CFG_DEBUG_EN
	uint8_t k,i;

	k = 0;
	
	for(i = 0; i < effect_sum; i++)
	{
		#if CFG_AUDIO_EFFECT_MIC_AEC_EN
		if(mode == EFF_AEC)//aec
		{
			if(effect_list[i] == EFF_AEC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Aec %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		#endif
	    if(mode == EFF_PCM_DELAY)//pcm delay
		{
			if(effect_list[i] == EFF_PCM_DELAY) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("PcmDelay %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		if(mode == EFF_EXCITER)//exciter
		{
			if(effect_list[i] == EFF_EXCITER) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Exciter %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		if(mode == EFF_EXPAND)//expand
		{
			if(effect_list[i] == EFF_EXPAND) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Expander %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_FREQ_SHIFTER)
		{
			if(effect_list[i] == EFF_FREQ_SHIFTER) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("Freq Shifter %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_HOWLING_DETOR)
		{
			if(effect_list[i] == EFF_HOWLING_DETOR) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Hollowin Dector %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_SILENCE_DETOR)
		{
			k = 1;
			DBG("SILENCE Dector %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
			return;
		}		   
		else if(mode== EFF_PITCH)
		{
			if(effect_list[i] == EFF_PITCH) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Pitch Shifter %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode== EFF_PITCH_PRO)
		{
			if(effect_list[i] == EFF_PITCH_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Pitch Shifter Pro%d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode== EFF_AUTO_TUNE)
		{
			if(effect_list[i] == EFF_AUTO_TUNE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Auto Tune %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_VOICE_CHANGE)
		{
			if(effect_list[i] == EFF_VOICE_CHANGE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Voice Change %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode == EFF_VOICE_CHANGE_PRO)
		{
			if(effect_list[i] == EFF_VOICE_CHANGE_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Voice Change Pro%d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode == EFF_ECHO)
		{
			if(effect_list[i] == EFF_ECHO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Echo %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode == EFF_REVERB)
		{
			if(effect_list[i] == EFF_REVERB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Reverb %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_PLATE_REVERB)
		{
			if(effect_list[i] == EFF_PLATE_REVERB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("PlateReverb %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_REVERB_PRO)
		{
			if(effect_list[i] == EFF_REVERB_PRO) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Reverb Pro %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}		
		else if(mode == EFF_VOCALCUT)
		{
			if(effect_list[i] == EFF_VOCALCUT) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("VocalCut %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_VOCALREMOVE)
		{
			if(effect_list[i] == EFF_VOCALREMOVE) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("VocalRemove %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_CHORUS)
		{
			if(effect_list[i] == EFF_CHORUS) k++;

			if(effect_addr[i] == (uint32_t)map)
			{		   	
				DBG("Chorus %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_THREED)
		{
			if(effect_list[i] == EFF_THREED) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("ThreeD %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_VB)
		{
			if(effect_list[i] == EFF_VB) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("VB %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_VBCLASSIC)
		{
			if(effect_list[i] == EFF_VBCLASSIC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("VBClassic %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_DRC)
		{
			if(effect_list[i] == EFF_DRC) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("DRC  %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}
		else if(mode == EFF_EQ)
		{
			if(effect_list[i] == EFF_EQ) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("EQ %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}	
		else if(mode == EFF_GAIN)
		{
			if(effect_list[i] == EFF_GAIN) k++;

			if(effect_addr[i] == (uint32_t)map)
			{
				DBG("GAIN %d  Malloc Fail, Need Size = %ld, Remain size =  %02lx\n",k,RamSize,(CFG_AUDIO_EFFECT_END_ADDR - DynamicStartAddr));
				return;
			}
		}		   
		else
		{
			return;
		}
	}		 
#endif   
}

/*
****************************************************************
*
*
*
****************************************************************
*/
void PrintAudioEffectName(const uint8_t *buff)
{	 
#if CFG_DEBUG_EN

	uint8_t cmd,i;

	char str[10];

	cmd = buff[2];

	if(cmd != 0x7e)
	{
		return;  
	}

	i = 5;

	while(buff[i] != 0x00)
	{
		if((buff[i] <='z') && (buff[i]>='A'))
		{
			str[i]= buff[i];
		}

		if(buff[i]==0x16) break;

		i++;

	}

	DBG("Curr Group Number: %d\n",buff[4]);

	DBG("Group Name: %s\n",str);
#endif	 
}
/*
****************************************************************
* 按键类型及属性打印信息
*
*
****************************************************************
*/
void PrintKeyStatus(uint16_t KeyType, uint16_t KeyValue)
{
#if CFG_DEBUG_EN
    uint16_t key_sta;
	//hight 8bit = key_status :PRESS=1	SHORT_RELEASE=2 LONG_PRESS=3 HOLD_PRESS=4 LONG_PRESS_RELEASE =5
	key_sta  = (KeyValue >> 8)&0x00ff;
	
	//low 8bit = key_Value :0~~~255
	KeyValue = (KeyValue >> 0)&0x00ff;
	
	//KeyType :ADK1=1  ADK2=2  IOKEY=3 IIC_UART_KEY=4 IR_KEY=5
	if(KeyType == ADK1_ID)//ADK1
	{
		DBG("ADK1->      ");
	}
	else if(KeyType == ADK2_ID)//ADK2
	{
		DBG("ADK2->     ");
	}
	else if(KeyType == IOKEY_ID)//io
	{
		DBG("IO KEY->    ");
	} 
	else if(KeyType == UARTKEY_ID)//IIC,UART
	{
		DBG("IIC UART->  ");
	}	
	else if(KeyType == IRKEY_ID)//IR KEY
	{
		DBG("IR KEY->    ");
	}		
	else//ADK0
	{
		DBG("UNKNOW KEY ");
	}  
//------------------------------------------//
    if(key_sta == KEY_UNKOWN_TYPE)//ADK1
    {
		DBG("UNKNOW	            ");
    }
    else if(key_sta == KEY_PRESSED)//ADK2
    {  
		DBG("KEY_PRESSED	        ");
    }
    else if(key_sta == KEY_SHORT_RELEASED)//io
    {
		DBG("KEY_SHORT_RELEASED   ");
    } 
    else if(key_sta == KEY_LONG_PRESSED)//IIC,UART
    {
		DBG("KEY_LONG_PRESSED    ");
    }	
    else if(key_sta == KEY_LONG_PRESS_HOLD)//io
    {
		DBG("KEY_LONG_PRESS_HOLD  ");
    } 
    else if(key_sta == KEY_LONG_RELEASED)//IIC,UART
    {
		DBG("KEY_LONG_RELEASED   ");
    }	
    else//ADK0
    {
		DBG("UNKNOW               ");
    }

	DBG("%02x\n",KeyValue);
#endif	
}
/*
****************************************************************
* 
*
*
****************************************************************
*/
void PrintAnaInputError(void)
{
	while(1)
	{
#if CFG_WATCH_DOG_EN
		WDG_Feed();
#endif
		WaitMs(1000);
		DBG("MIC 或者 LINE输入选择冲突, mic and line is conflict!!!!!!!!!\n");
	}
}

/*
****************************************************************
*  对上位机下载的，从SPI读取的调音参数打印显示
*  显示固定存在Audio_mode_buff[]
*
****************************************************************
*/
void PrintAudioEffectParamaesList(uint32_t  len,uint8_t *buff)
{	 
#if CFG_DEBUG_EN

	uint16_t cmd_len,packet_len,k;
	
	uint8_t end_code,ctl_code,start;
	
	uint16_t position;

	DBG("start printf effect paramase\n ");
	DBG("\n");
	
	position = 0;
	
	while(position < len)
	{
	    if ((buff[position] ==0xa5) && (buff[position+1] ==0x5a))
		{
			packet_len = buff[position + 3];

			end_code   = buff[position + 4 + packet_len];//0x16

			if(end_code == 0x16)////end code ok
			{
				cmd_len = packet_len + 5;
				
				//sync
				start=buff[position++];
				DBG("0x%02X, ",start);
				cmd_len--;
				start=buff[position++];
				DBG("0x%02X,\n",start);
				cmd_len--;

                //control
				start=buff[position++];
				DBG("0x%02X,\n",start);
				cmd_len--;

                //len
				start=buff[position++];
				DBG("0x%02X,\n",start);
				cmd_len--;

                //all parameter
				start=buff[position++];
				DBG("0x%02X,\n",start);
				cmd_len--;

				for(k = 1; k < cmd_len; k++)//get signal COMMAND
				{			
					ctl_code=buff[position++];
					DBG("0x%02X, ",ctl_code);
					if(k % 2 == 0)
					{
						DBG("\n"); 
					}						
				}
				
				///end
				start=buff[position++];
				DBG("0x%02X,\n ",start);
				cmd_len--;
				DBG("\n"); 
                #if CFG_WATCH_DOG_EN
                WDG_Feed();
                #endif
			}
			else  // end code error
			{
				position++;
			}
		}
		else // serch start code....
		{
			position++;
		}

	}
#endif	
}
/*
****************************************************************
*
* 打印出DMA的使用信息，主要防止粗心，找不到问题原因
*
****************************************************************
*/
extern uint8_t DmaChannelMap[28];

void PrintDmaUseInfo(void)
{
#if CFG_DEBUG_EN
    uint8_t k,i,ch;

	for( k = 0; k < 9; k++)
	{
		ch = 0;

		for(i = 0; i < sizeof(DmaChannelMap); i++)
		{
			if(DmaChannelMap[i] == k)
			{
				ch++;
			}
		}

		if(ch > 1)
		{
			DBG("Set DMA channel Conflict:%d\n",k);
			return;
		}

	}

	if(gCtrlVars.pga0_att)///adc0
	{
		if(DmaChannelMap[18] == 255)
		{
			DBG("Waring  ADC0  DMA Not Open\n");
		}
	}

	if(gCtrlVars.pga1_att)///adc1
	{
		if(DmaChannelMap[19] == 255)
		{
			DBG("Waring ADC1 DMA Not Open\n");
		}
	}

	#if CFG_DAC0_EN
	if(DmaChannelMap[20] == 255)
	{
		DBG("Waring  DAC0 TX DMA Not Open\n");
	}
	#endif

	#if CFG_DAC1_EN
	if(DmaChannelMap[21] == 255)
	{
		DBG("Waring  DACX  DMA Not Open\n");
	}
	#endif

	#if CFG_I2S0_IN_EN
	if(DmaChannelMap[22] == 255)
	{
		DBG("Waring  I2S0 TX DMA Not Open\n");
	}
	#endif

	#if CFG_I2S0_OUT_EN
	if(DmaChannelMap[23] == 255)
	{
		DBG("Waring  I2S0 RX DMA Not Open\n");
	}
	#endif

	#if CFG_I2S1_IN_EN
	if(DmaChannelMap[24] == 255)
	{
		DBG("Waring  I2S1 TX DMA Not Open\n");
	}
	#endif

	#if CFG_I2S1_OUT_EN
	if(DmaChannelMap[25] == 255)
	{
		DBG("Waring  I2S1 RX DMA Not Open\n");
	}
	#endif

	#if CFG_SPDIF_IN_EN
	if(DmaChannelMap[8] == 255)
	{
		DBG("Waring  SPDIF RX DMA Not Open\n");
	}
	#endif

	#if CFG_SPDIF_OUT_EN
	if(DmaChannelMap[9] == 255)
	{
		DBG("Waring  SPDIF TX DMA Not Open\n");
	}
	#endif

	#if CFG_COMMUNICATION_BY_UART
	if(DmaChannelMap[13] == 255)
	{
		DBG("Waring  UART1 RX DMA Not Open\n");
	}
	if(DmaChannelMap[14] == 255)
	{
		DBG("Waring  UART1 TX DMA Not Open\n");
	}
	#endif
#endif
}

#endif
