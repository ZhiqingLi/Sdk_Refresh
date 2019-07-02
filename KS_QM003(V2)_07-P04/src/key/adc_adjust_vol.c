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
	206, 	351, 	516, 	662, 	809, 	948, 	1087, 	1218,
	1341, 	1467,	1575,	1677,	1773,	1874,	1970,	2062,
	2202,	2351,	2470,	2591,	2706,	2818,	2940,	3015,
	3068,	3113,	3166,	3214,	3264,	3306,	3350,	3600
};	

uint32_t AdcAdjustSampleSum = 0; 
uint16_t AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
uint16_t AdcAdjustLevelAverage = 0;		//褰撳墠ADC骞冲潎鍊?

uint8_t GetAdcAdjustVolIndexVal(uint32_t AdcAdjustSampleVal)
{
	uint8_t KeyIndex = 0;

	for (KeyIndex = 0; KeyIndex < (MAX_VOLUME+1); KeyIndex++)
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
	static uint8_t	CurVolumeIndex = MAX_VOLUME;
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

		if((abs(AdcAdjustLevelAverage - PrevAverageValue) > 50)
		&& (CurVolumeIndex != GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage)))
		{
			CurVolumeIndex = GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage);
			gSys.Volume = CurVolumeIndex;
			SetSysVol();
			McuSyncWiFiVolume(gSys.Volume);
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


