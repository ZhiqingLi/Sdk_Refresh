//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: micro_adjust.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////
/*

*/

#include "type.h"
#include "app_config.h"
#include "sra.h"
#include "dac.h"
#include "pcm_fifo.h"
#include "bt_stack_api.h"
#include "mixer.h"
#include "micro_adjust.h"

#define  PCM_FIFO_SBC_SAMPLES   4096

#ifdef FUNC_SOFT_ADJUST_EN
int16_t AudioSoftAdjust[130*4];
SRAContext  gSRAContext;
#endif

//static uint32_t DacAdjustCnt = 0;


#define PCM_HIGH_LEVEL				3192
#define PCM_MIDDLE_LEVEL			2048
#define PCM_LOW_LEVEL				1024

#define ADJUST_NONE					0
#define ADJUST_SLOW					1
#define ADJUST_FAST					2

#define ADJUST_VALUE				1000


#define COUNT_OF_GET_SAMPLES		32

void DacAdjust(void)
{
	static uint8_t start_adjust = ADJUST_NONE;
	static uint8_t i = 1;
	static uint8_t count_of_get_samples = 0;
	static uint32_t sum_samples = 0;
	static uint32_t average_samples = 0;
	
//	DacSampRateAdjust(TRUE, 0);
//	APP_DBG("%d, %d\n", MixerGetRemainSamples(MIXER_SOURCE_BT), start_adjust);
//	return;
	if(count_of_get_samples < COUNT_OF_GET_SAMPLES)
	{
		sum_samples += MixerGetRemainSamples(MIXER_SOURCE_BT);
		count_of_get_samples++;
		return;
	}

	average_samples = sum_samples / COUNT_OF_GET_SAMPLES;
	count_of_get_samples = 0;
	sum_samples = 0;

//	APP_DBG("--- %d ---\n", average_samples);
	
	switch(start_adjust)
	{
		case ADJUST_SLOW:
			{
				if(average_samples < PCM_LOW_LEVEL)
				{
					i++;
					if(i >= 20)
						i = 20;
					DacSampRateAdjust(TRUE, -ADJUST_VALUE*i);
					return;
				}
				
				if(average_samples > PCM_MIDDLE_LEVEL)
				{
					i = 1;
//					APP_DBG("SLOW ADJUST STOP\n");
					start_adjust = ADJUST_NONE;
					DacSampRateAdjust(TRUE, 0);
					return;
				}
			}
			break;

		case ADJUST_FAST:
			{
				if(average_samples > PCM_HIGH_LEVEL)
				{
					i++;
					if(i >= 20)
						i = 20;
					DacSampRateAdjust(TRUE, ADJUST_VALUE*i);
					return;
				}

				if(average_samples < PCM_MIDDLE_LEVEL)
				{
					i = 1;
//					APP_DBG("FAST ADJUST STOP\n");
					start_adjust = ADJUST_NONE;
					DacSampRateAdjust(TRUE, 0);
					return;
				}
			}
			break;

		default:
			break;
	}

	if(start_adjust != ADJUST_NONE)
		return;

	if(average_samples < PCM_LOW_LEVEL)
	{
//		APP_DBG("SLOW ADJUST\n");
		start_adjust = ADJUST_SLOW;
		DacSampRateAdjust(TRUE, -ADJUST_VALUE);
		return;
	}

	if(average_samples > PCM_HIGH_LEVEL)
	{
//		APP_DBG("FAST ADJUST\n");
		start_adjust = ADJUST_FAST;
		DacSampRateAdjust(TRUE, ADJUST_VALUE);
		return;
	}

}
#ifdef FUNC_SOFT_ADJUST_EN
void SoftAdjustInit(uint32_t ChannelNum)
{
    init_sra(&gSRAContext, ChannelNum);   
}

static int32_t  AdjustStep = 0;
//static int32_t  RcvIdx = 0;

//该函数仅适用于蓝牙接收，因为GetRcvListItemNum
uint32_t BtSoftAdjustApply(uint32_t SampleLevel, int16_t *PcmInAddr, uint32_t PcmLen, uint32_t CurChannelNum)
{
    uint32_t PcmRemainLen;
        
    PcmRemainLen = MixerGetRemainSamples(MIXER_SOURCE_BT);
    
    if((PcmRemainLen < 0x400))
    {
        AdjustStep++;
        if(AdjustStep > 1)
        {
            AdjustStep = 1;
        }   
    }
    else if(PcmRemainLen > 0xfd0)
    {
        AdjustStep--;
        if(AdjustStep < -1)
        {
            AdjustStep = -1;
        }
    }
    else
    {
        AdjustStep = 0;
    }
    
    
    apply_sra(&gSRAContext, PcmInAddr, (int16_t *)AudioSoftAdjust, AdjustStep);
    memcpy(PcmInAddr, AudioSoftAdjust, ((PcmLen + AdjustStep) << CurChannelNum));
    return (PcmLen + AdjustStep);
}

#endif
//

