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

#define 	ADC_ADJUST_VOL_SCAN_COUNT	10

static const uint16_t AdcAdjustVolIndexVal[MAX_VOLUME+1] = 
{
	5,
	50, 	351, 	655, 	952, 	1275, 	1562, 	1885, 	2108,
	2280, 	2437,	2555,	2657,	2727,	2787,	2845,	2897,
	2900,	2951,	3002,	3046,	3080,	3115,	3149,	3185,
	3209,	3229,	3250,	3272,	3305,	3336,	3355,	3380
};	

uint32_t AdcAdjustSampleSum = 0; 
uint16_t AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
uint16_t AdcAdjustLevelAverage = 0;		//褰撳墠ADC骞冲潎鍊?

uint8_t GetAdcAdjustVolIndexVal(uint32_t AdcAdjustSampleVal)
{
	uint8_t KeyIndex = 0;

	for (KeyIndex = MAX_VOLUME; KeyIndex > 0; KeyIndex--)
	{
	   if(AdcAdjustSampleVal >= AdcAdjustVolIndexVal[KeyIndex])
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
	static uint16_t PrevAverageValue = 0;
	uint16_t AverageVal, AdcVolVal;

	if(AdcAdjustSampleCnt > 0)
	{
		AdcAdjustSampleSum += SarAdcChannelGetValue(ADC_ADJUST_VOLUME_PORT);
		AdcAdjustSampleCnt--;
	}

	if(AdcAdjustSampleCnt == 0)
	{
		uint8_t VolumeIndex = 0;
		
		AdcAdjustLevelAverage = (AdcAdjustSampleSum/ADC_ADJUST_VOL_SCAN_COUNT);
		AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
		AdcAdjustSampleSum = 0;

		if (VolumeIndex > gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume+1]-AdcAdjustVolIndexVal[gSys.Volume]);
		}
		else if (VolumeIndex <   gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume]-AdcAdjustVolIndexVal[gSys.Volume-1]);
		}
		
		VolumeIndex = GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage);
		AverageVal = abs(AdcAdjustLevelAverage-PrevAverageValue);

		if(((AverageVal >= AdcVolVal) || !VolumeIndex)
		&& (gSys.Volume != VolumeIndex) && !IS_RTC_WAKEUP())
		{
			gSys.Volume = VolumeIndex;
			if (1) {	//!WiFiKaiShuSleepModeGet()) {
				SetSysVol();
				McuSyncWiFiVolume(gSys.Volume);
			}
			APP_DBG("AdcAdjustvolume = %d:%d:%d;\n", AdcAdjustLevelAverage, AverageVal, gSys.Volume);
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


