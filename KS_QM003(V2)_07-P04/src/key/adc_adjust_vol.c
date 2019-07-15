#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "adc.h"
#include "wakeup.h"
#include "wifi_control.h"

#ifdef FUNC_ADC_ADJUST_VOLUME_EN

#define 	ADC_ADJUST_VOL_SCAN_COUNT	5

static const uint16_t AdcAdjustVolIndexVal[MAX_VOLUME+1] = 
{
	50,
	156, 	251, 	316, 	422, 	529, 	628, 	725, 	818,
	931, 	1027,	1125,	1227,	1333,	1434,	1540,	1652,
	1752,	1851,	1970,	2091,	2206,	2318,	2430,	2545,
	2668,	2773,	2886,	3001,	3124,	3236,	3350,	3460
};	

uint32_t AdcAdjustSampleSum = 0; 
uint16_t AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
uint16_t AdcAdjustLevelAverage = 0;		//褰撳墠ADC骞冲潎鍊?

uint8_t GetAdcAdjustVolIndexVal(uint32_t AdcAdjustSampleVal)
{
	uint8_t KeyIndex = 0;

	for (KeyIndex = 0; KeyIndex < MAX_VOLUME; KeyIndex++)
	{
	   if(AdcAdjustSampleVal <= AdcAdjustVolIndexVal[KeyIndex])
	   {
	   		break;
	   }
	}

	return KeyIndex;
}


// adc key scan
// return: 0---no key, else---key msg
void AdcAdjustVolScan(void)
{
	static uint8_t	CurVolumeIndex = 0;
	static uint16_t PrevAverageValue = 0;

	if(AdcAdjustSampleCnt > 0)
	{
		AdcAdjustSampleSum += SarAdcChannelGetValue(ADC_ADJUST_VOLUME_PORT);
		AdcAdjustSampleCnt--;
	}

	if(AdcAdjustSampleCnt == 0)
	{
		AdcAdjustLevelAverage = (AdcAdjustSampleSum/ADC_ADJUST_VOL_SCAN_COUNT);
		AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
		AdcAdjustSampleSum = 0;

		if((abs(AdcAdjustLevelAverage - PrevAverageValue) > 100)
		&& (CurVolumeIndex != GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage)))
		{
			CurVolumeIndex = GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage);
			gSys.Volume = CurVolumeIndex;
			if (WIFI_PLAY_KAISHU_RADIO_SLEEP != gWiFi.KaiShuRadio) {
				SetSysVol();
				McuSyncWiFiVolume(gSys.Volume);
			}
			APP_DBG("AdcAdjustvolume = %d:%d:%d;\n", PrevAverageValue, AdcAdjustLevelAverage, gSys.Volume);
			PrevAverageValue = AdcAdjustLevelAverage;
		}
	}
}

int32_t AdcAdjustVolInit(void)
{
	DBG("AdcAdiustVolInit()!\n");

	AdcAdjustSampleSum = 0;
	AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
	AdcAdjustLevelAverage = 0;

	SarAdcGpioSel(ADC_ADJUST_VOLUME_PORT);
	//开机音量检查。
	while (AdcAdjustSampleCnt > 0) {
		AdcAdjustSampleSum += SarAdcChannelGetValue(ADC_ADJUST_VOLUME_PORT);
		AdcAdjustSampleCnt--;
		WaitMs(10);
	}
	AdcAdjustVolScan();

	return 0;
}

#endif


