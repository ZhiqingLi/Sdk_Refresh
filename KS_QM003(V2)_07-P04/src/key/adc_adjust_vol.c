#include "app_config.h"
#include "os.h"
#include "gpio.h"
#include "irqs.h"
#include "msgq.h"
#include "app_msg.h"
#include "sys_app.h"
#include "sys_vol.h"
#include "adc.h"
#include "wifi_control.h"

#ifdef FUNC_ADC_ADJUST_VOLUME_EN

#define 	ADC_ADJUST_VOL_SCAN_COUNT	10

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
uint32_t AdcAdjustLevelAverage = 0;		//当前ADC平均�?

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
	static uint8_t	CurVolumeIndex = 0;

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

		if(CurVolumeIndex != GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage))
		{
			CurVolumeIndex = GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage);
			gSys.Volume = CurVolumeIndex;
			//APP_DBG("AdcAdjustvolume = %d;\n", gSys.Volume);
			SetSysVol();
			McuSyncWiFiVolume(gSys.Volume);
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

	return 0;
}

#endif


