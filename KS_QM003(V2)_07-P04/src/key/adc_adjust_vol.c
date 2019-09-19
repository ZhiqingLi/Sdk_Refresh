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

#define 	ADC_ADJUST_VOL_SCAN_COUNT	20
#define 	ADC_ADJUST_VALUE_COUNT		5

static const uint16_t AdcAdjustVolIndexVal[MAX_VOLUME+1] = 
{
	5,
	50, 	351, 	655, 	952, 	1275, 	1562, 	1885, 	2108,
	2280, 	2437,	2555,	2657,	2727,	2787,	2845,	2897,
	2900,	2951,	3002,	3046,	3080,	3115,	3149,	3185,
	3209,	3229,	3250,	3272,	3305,	3336,	3355,	3380
};	

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


#if   1
uint16_t jitter_cnt = 0;
uint16_t adc_max[ADC_ADJUST_VALUE_COUNT];
uint16_t adc_val_cnt[ADC_ADJUST_VALUE_COUNT];

void AdcAdjustVolScan(void)
{
	//水泵电流是交流峰值 
    uint8_t adc_index = 0, val_index = 0;
    uint16_t adc_val = 0;
    static uint16_t PrevAverageValue = 0;
	uint16_t AverageVal, AdcVolVal;

	adc_val = SarAdcChannelGetValue(ADC_ADJUST_VOLUME_PORT);
	
	if (jitter_cnt++ < ADC_ADJUST_VOL_SCAN_COUNT) {
		//数据按照大到小
		for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
        	if (adc_max[adc_index] <= adc_val) {
        		break;
        	}
        }
        //数据放入队列
        if ((adc_index < ADC_ADJUST_VALUE_COUNT) && (adc_max[adc_index] != adc_val)) {
        	val_index = adc_index;

        	for (adc_index = (ADC_ADJUST_VALUE_COUNT-1); adc_index    > val_index; adc_index--) {
        		adc_max[adc_index] = adc_max[adc_index-1];
        		adc_val_cnt[adc_index] = adc_val_cnt[adc_index-1];
        	}
            adc_max[val_index] = adc_val;
            adc_val_cnt[val_index] = 0;
        }

        for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
        	if (adc_max[adc_index] == adc_val) {
        		adc_val_cnt[adc_index]++;
        		break;
        	}
        }
        
		//printf ("adc value = %d; \n", adc_val);
    } 
    else {
    	//查找最多数据的
    	val_index = 0;
    	for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
			if (adc_val_cnt[adc_index] >= adc_val_cnt[val_index]) {
				val_index = adc_index;
			}
        } 
		
		adc_index = GetAdcAdjustVolIndexVal(adc_max[val_index]);

		AverageVal = abs(adc_max[val_index]-PrevAverageValue);
		if (adc_index > gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume+1]-AdcAdjustVolIndexVal[gSys.Volume]);
		}
		else if (adc_index < gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume]-AdcAdjustVolIndexVal[gSys.Volume-1]);
		}
		
		if((gSys.Volume != adc_index) && !IS_RTC_WAKEUP()
		&& ((AverageVal >= AdcVolVal) || !adc_index))
		{
			gSys.Volume = adc_index;
			if (1) {	//!WiFiKaiShuSleepModeGet()) {
				SetSysVol();
				McuSyncWiFiVolume(gSys.Volume);
			}
			for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
				//APP_DBG ("AdcAdjustvolume adc_max[%d]= %d; adc_val_cnt[%d] = %d;\n",
				//adc_index, adc_max[adc_index], adc_index, adc_val_cnt[adc_index]);
			}
			PrevAverageValue = adc_max[val_index];
			APP_DBG("AdcAdjustvolume param = %d:%d:%d;\n", PrevAverageValue, AdcVolVal, gSys.Volume);
		}
        memset(adc_max, 0x00, sizeof(adc_max));
        memset(adc_val_cnt, 0x00, sizeof(adc_val_cnt));
        jitter_cnt = 0;
    }
}

int32_t AdcAdjustVolInit(void)
{
	DBG("AdcAdiustVolInit()!\n");

	memset(adc_max, 0x00, sizeof(adc_max));
	memset(adc_val_cnt, 0x00, sizeof(adc_val_cnt));
	jitter_cnt = 0;

	SarAdcGpioSel(ADC_ADJUST_VOLUME_PORT);
	while (jitter_cnt++ < ADC_ADJUST_VOL_SCAN_COUNT) {
		uint8_t adc_index = 0, val_index = 0;
		uint16_t adc_val = 0;

		WaitMs(5);
		adc_val = SarAdcChannelGetValue(ADC_ADJUST_VOLUME_PORT);
		//数据按照大到小
		for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
        	if (adc_max[adc_index] <= adc_val) {
        		break;
        	}
        }
        //数据放入队列
        if ((adc_index < ADC_ADJUST_VALUE_COUNT) && (adc_max[adc_index] != adc_val)) {
        	val_index = adc_index;

        	for (adc_index = (ADC_ADJUST_VALUE_COUNT-1); adc_index    > val_index; adc_index--) {
        		adc_max[adc_index] = adc_max[adc_index-1];
        		adc_val_cnt[adc_index] = adc_val_cnt[adc_index-1];
        	}
            adc_max[val_index] = adc_val;
            adc_val_cnt[val_index] = 0;
        }

        for (adc_index = 0; adc_index < ADC_ADJUST_VALUE_COUNT; adc_index++) {
        	if (adc_max[adc_index] == adc_val) {
        		adc_val_cnt[adc_index]++;
        		break;
        	}
        }
    } 

	AdcAdjustVolScan();

	return 0;
}


#else

uint32_t AdcAdjustSampleSum = 0; 
uint16_t AdcAdjustSampleCnt = ADC_ADJUST_VOL_SCAN_COUNT;
uint16_t AdcAdjustLevelAverage = 0;		//褰撳墠ADC骞冲潎鍊?


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
		
		VolumeIndex = GetAdcAdjustVolIndexVal(AdcAdjustLevelAverage);
		AverageVal = abs(AdcAdjustLevelAverage-PrevAverageValue);
		if (VolumeIndex > gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume+1]-AdcAdjustVolIndexVal[gSys.Volume]);
		}
		else if (VolumeIndex < gSys.Volume) {
			AdcVolVal = abs(AdcAdjustVolIndexVal[gSys.Volume]-AdcAdjustVolIndexVal[gSys.Volume-1]);
		}

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
#endif


