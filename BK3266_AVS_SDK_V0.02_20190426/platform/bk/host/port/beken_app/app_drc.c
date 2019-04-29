/**
 * @file   drc.c
 * @author Aixing.Li
 *
 * @brief  A fixed-point implementation for Dynamic Range Compression
 *
 * (c) 2018 BEKEN Corporation, Ltd. All Rights Reserved
 */
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "app_drc.h"
#if (CONFIG_DRC == 1)

#define __INLINE                    __inline

#define DRC_GAIN_FRA_BITS           (12)
#define DRC_GAIN_MIN                (1 << (DRC_GAIN_FRA_BITS - 4))
#define DRC_GAIN_MAX                (1 << DRC_GAIN_FRA_BITS)

#define DRC_GAIN_ATTACK_FRA_BITS    (15)
#define DRC_GAIN_ATTACK_COEF        (1024)
#define DRC_GAIN_DECAY_FRA_BITS     (15)
#define DRC_GAIN_DECAY_COEF         (512)
#define DRC_GATING_DECAY_FRA_BITS   (15)
#define DRC_GATING_DECAY_COEF       (2048)

#define DRC_MAX_SAMPLE              ((uint32_t)(0.95 * 32767 * (1 << DRC_GAIN_FRA_BITS)))

#define DRC_PEAK_THD                ((uint32_t)(0.80 * 32767 * (1 << DRC_GAIN_FRA_BITS)))
#define DRC_PEAK_DECAY_FRA_BITS     (15)
#define DRC_PEAK_DECAY_COEF         (256)

#define DRC_RSSI_ATTACK_FRA_BITS	(15)
#define DRC_RSSI_ATTACK_RSB         (9)
#define DRC_RSSI_DECAY_FRA_BITS		(15)
#define DRC_RSSI_DECAY_RSB          (9)
#define DRC_RSSI_THD_HIGH           ((uint32_t)(0.320 * 32767 * (1 << DRC_GAIN_FRA_BITS)))
#define DRC_RSSI_THD_LOW            ((uint32_t)(0.200 * 32767 * (1 << DRC_GAIN_FRA_BITS)))
#define DRC_RSSI_GATING             ((uint32_t)(0.005 * 32767 * (1 << DRC_GAIN_FRA_BITS)))

#define DRC_MIN(a, b)               ((a) < (b) ? (a) : (b))
#define DRC_MAX(a, b)               ((a) > (b) ? (a) : (b))

DRCContext app_drc;
app_drc_para_t app_drc_para;

#ifndef _MSC_VER
__attribute__((section(".rodata")))
#endif
static const uint16_t DB_2_GAIN_TABLE[] =
{
    0x0004, 0x0005, 0x0005, 0x0006, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000c,
    0x000d, 0x000f, 0x0010, 0x0012, 0x0015, 0x0017, 0x001a, 0x001d, 0x0021, 0x0025,
    0x0029, 0x002e, 0x0034, 0x003a, 0x0041, 0x0049, 0x0052, 0x005c, 0x0067, 0x0073,
    0x0082, 0x0091, 0x00a3, 0x00b7, 0x00cd, 0x00e6, 0x0102, 0x0122, 0x0145, 0x016d,
    0x019a, 0x01cc, 0x0204, 0x0243, 0x0289, 0x02d8, 0x0331, 0x0395, 0x0405, 0x0482,
    0x050f, 0x05ad, 0x065f, 0x0726, 0x0805, 0x08ff, 0x0a18, 0x0b54, 0x0cb6, 0x0e43,
    0x1000, 0x11f4, 0x1425, 0x169a, 0x195c, 0x1c74, 0x1fed, 0x23d2, 0x2831, 0x2d18,
    0x3299, 0x38c5, 0x3fb2, 0x4778, 0x5031, 0x59fa, 0x64f4, 0x7145, 0x7f18, 0x8e9a,
    0xa000, 0xb386, 0xc96e, 0xe202, 0xfd95
};

static __INLINE int32_t drc_abs(int32_t x)
{
    int32_t y = x - (x < 0);

    return y ^= (y >> 31);
}

void drc_init(DRCContext* drc, uint32_t sample_rate, uint32_t channels)
{
    drc->rssi           = 0;
	drc->rssi_attack    = 64;
	drc->rssi_decay     = 64;
	drc->rms_value      = 0;
	drc->rms_detect_cnt = 0;
	drc->rms_detect_time= sample_rate / 1000;
    drc->gain           = 1 << DRC_GAIN_FRA_BITS;
    drc->gain_min       = DRC_GAIN_MIN;
    drc->gain_max       = DRC_GAIN_MAX;
    drc->channels       = (uint16_t)channels;
    drc->sample_rate    = sample_rate;

    drc->_gain_attack   = (1 << DRC_GAIN_ATTACK_FRA_BITS)  + (1 << DRC_GAIN_ATTACK_FRA_BITS)  / DRC_GAIN_ATTACK_COEF;
    drc->_gain_decay    = (1 << DRC_GAIN_DECAY_FRA_BITS)   - (1 << DRC_GAIN_DECAY_FRA_BITS)   / DRC_GAIN_DECAY_COEF;
    drc->_gating_decay  = (1 << DRC_GATING_DECAY_FRA_BITS) - (1 << DRC_GATING_DECAY_FRA_BITS) / DRC_GATING_DECAY_COEF;
    drc->_peak_decay    = (1 << DRC_PEAK_DECAY_FRA_BITS)   - (1 << DRC_PEAK_DECAY_FRA_BITS)   / DRC_PEAK_DECAY_COEF;

    drc->max_sample     = DRC_MAX_SAMPLE;
    drc->peak_threshold = DRC_PEAK_THD;

    drc->rssi_threshold_high   = DRC_RSSI_THD_HIGH;
    drc->rssi_threshold_low    = DRC_RSSI_THD_LOW;
    drc->rssi_threshold_gating = DRC_RSSI_GATING;

    if(app_drc_para.flag)
    {
        drc->flag = app_drc_para.flag;
        drc->pre_vol = app_drc_para.pre_vol;
        drc->post_vol = app_drc_para.post_vol;
        drc->rms_detect_time = app_drc_para.rms_time * (sample_rate / 1000);

        drc_ctrl(drc,DRC_CTRL_CMD_RSSI_THRESHOLD_HIGH_SET,app_drc_para.threshold + (app_drc_para.threshold >> 4));
        drc_ctrl(drc,DRC_CTRL_CMD_RSSI_THRESHOLD_LOW_SET, app_drc_para.threshold - (app_drc_para.threshold >> 4));

        drc_ctrl(drc,DRC_CTRL_CMD_GAIN_ATTACK_SET,app_drc_para.attack_time);
        drc_ctrl(drc,DRC_CTRL_CMD_GAIN_RELEASE_SET,app_drc_para.release_time);
    }

    drc->init = 1;

}

void drc_ctrl(DRCContext* drc, uint32_t cmd, uint32_t arg)
{
    switch(cmd)
    {
    case DRC_CTRL_CMD_GAIN_MIN_SET:
        {
            int32_t db = (int32_t)arg;
            if(db >= - 60 && db <= -24)
            {
                db += 60;
                drc->gain_min = DB_2_GAIN_TABLE[db];
            }
        }
        break;
    case DRC_CTRL_CMD_GAIN_MAX_SET:
        {
            int32_t db = (int32_t)arg;
            if(db >= - 60 && db <= -24)
            {
                db += 60;
                drc->gain_max = DB_2_GAIN_TABLE[db];
            }
        }
        break;
    case DRC_CTRL_CMD_GAIN_ATTACK_SET:
        drc->_gain_attack = (1 << DRC_GAIN_ATTACK_FRA_BITS) + arg;
        break;
    case DRC_CTRL_CMD_GAIN_DECAY_SET:
        drc->_gain_decay = (1 << DRC_GAIN_DECAY_FRA_BITS) - arg;
        break;
    case DRC_CTRL_CMD_GATING_DECAY_SET:
        drc->_gating_decay = (1 << DRC_GATING_DECAY_FRA_BITS) - arg;
        break;
    case DRC_CTRL_CMD_PEAK_DECAY_SET:
        drc->_peak_decay = (1 << DRC_PEAK_DECAY_FRA_BITS) - arg;
        break;
    case DRC_CTRL_CMD_MAX_SAMPLE_SET:
        drc->max_sample = arg << DRC_GAIN_FRA_BITS;
        break;
    case DRC_CTRL_CMD_PEAK_THRESHOLD_SET:
        drc->peak_threshold = arg << DRC_GAIN_FRA_BITS;
        break;
    case DRC_CTRL_CMD_RSSI_THRESHOLD_HIGH_SET:
        drc->rssi_threshold_high = arg << DRC_GAIN_FRA_BITS;
        break;
    case DRC_CTRL_CMD_RSSI_THRESHOLD_LOW_SET:
        drc->rssi_threshold_low = arg << DRC_GAIN_FRA_BITS;
        break;
    case DRC_CTRL_CMD_RSSI_THRESHOLD_GATING_SET:
        drc->rssi_threshold_gating = arg << DRC_GAIN_FRA_BITS;
        break;
	case DRC_CTRL_CMD_RSSI_ATTACK_SET:
		drc->rssi_attack = arg;
		break;
	case DRC_CTRL_CMD_RSSI_DECAY_SET:
		drc->rssi_decay = arg;
		break;
	case DRC_CRTL_CMD_RMS_DETECT_TIME_SET:
		drc->rms_value       = 0;
		drc->rms_detect_cnt  = 0;
		drc->rms_detect_time = arg * (drc->sample_rate / 1000);
		break;
    default:
        break;
    }
}

static uint32_t drc_gain_update(DRCContext* drc, uint32_t abss)
{
	drc->rms_value += abss;

	if(++drc->rms_detect_cnt >= drc->rms_detect_time)
	{
		uint32_t gain = drc->gain;
		uint32_t rssi = drc->rssi;
		uint32_t tabs = drc->rms_value / drc->rms_detect_time * gain;//(1 << DRC_GAIN_FRA_BITS);//gain;

		drc->rms_value      = 0;
		drc->rms_detect_cnt = 0;

		if(tabs > rssi)
		{
			//rssi = rssi - (rssi >> DRC_RSSI_ATTACK_RSB) + (tabs >> DRC_RSSI_ATTACK_RSB);
			rssi = tabs - (tabs >> DRC_RSSI_ATTACK_RSB) + (rssi >> DRC_RSSI_ATTACK_RSB);
			//rssi += ((uint64_t)(tabs - rssi) * drc->rssi_attack) >> DRC_RSSI_ATTACK_FRA_BITS;
		}
		else
		{
			//rssi = rssi - (rssi >> DRC_RSSI_DECAY_RSB) + (tabs >> DRC_RSSI_DECAY_RSB);
			rssi = tabs - (tabs >> DRC_RSSI_DECAY_RSB) + (rssi >> DRC_RSSI_DECAY_RSB);
			//rssi -= ((uint64_t)(rssi - tabs) * drc->rssi_decay) >> DRC_RSSI_DECAY_FRA_BITS;
		}

		if(rssi < drc->rssi_threshold_low)
		{
			if(rssi > drc->rssi_threshold_gating)
			{
				gain = DRC_MIN(drc->gain_max, (gain * drc->_gain_attack) >> DRC_GAIN_ATTACK_FRA_BITS);
			}
			else
			{
				gain = DRC_MAX(drc->gain_min, (gain * drc->_gating_decay) >> DRC_GATING_DECAY_FRA_BITS);
			}
		}
		else if(rssi > drc->rssi_threshold_high)
		{
			gain = DRC_MAX(drc->gain_min, (gain * drc->_gain_decay) >> DRC_GAIN_DECAY_FRA_BITS);
		}

		if(tabs > drc->peak_threshold)
		{
			gain = DRC_MAX(drc->gain_min, (gain * drc->_peak_decay) >> DRC_PEAK_DECAY_FRA_BITS);

			tabs = abss * gain;
			while(tabs > drc->max_sample)
			{
				gain = gain * drc->_peak_decay >> DRC_PEAK_DECAY_FRA_BITS;
				tabs = abss * gain;
			}
		}

		drc->rssi = rssi;
		drc->gain = gain;

	//	printf("[RSSI=%d, GAIN=%d]\n", rssi / 4096, gain);
		//os_printf("%d\n", gain);

		return gain;
	}
	else
	{
		return drc->gain;
	}
}

void drc_apply(DRCContext* drc, int16_t* pcm_in, int16_t* pcm_out, uint32_t samples)
{
    uint32_t s;

    if(drc->channels == 1)
    {
        for(s = 0; s < samples; s++)
        {
            uint32_t abss = drc_abs(pcm_in[s]);
            uint32_t gain = drc_gain_update(drc, abss);

			pcm_out[s] = (int16_t)(pcm_in[s] * gain >> DRC_GAIN_FRA_BITS);
        }
    }
    else
    {
        for(s = 0; s < samples; s++)
        {
            uint32_t abss = DRC_MAX(drc_abs(pcm_in[0]), drc_abs(pcm_in[1]));
            uint32_t gain = drc_gain_update(drc, abss);

            *pcm_out++ = (int16_t)(*pcm_in++ * gain >> DRC_GAIN_FRA_BITS);
            *pcm_out++ = (int16_t)(*pcm_in++ * gain >> DRC_GAIN_FRA_BITS);
        }
    }
}

void app_show_drc_para(void)
{
	os_printf("flag:%x,pre:%d,post:%d,thd:%d,rms:%d,attack:%d,release:%d\r\n",app_drc_para.flag,
		app_drc_para.pre_vol,app_drc_para.post_vol,
		app_drc_para.threshold,app_drc_para.rms_time,
		app_drc_para.attack_time,app_drc_para.release_time
		);
}
void app_set_drc_para(uint8 *para)
{
    memcpy((uint8 *)&app_drc_para,
                        (uint8 *)&para[1],
                        sizeof(app_drc_para_t));
	app_show_drc_para();
	app_drc.init = 0;
}


// Test code which is run @ VC6++
#ifdef _MSC_VER

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE* fin  = fopen("444.wav", "rb");
    FILE* fout = fopen("dddd_drc.wav", "wb");

    if(fin && fout)
    {
        unsigned char* wav;

        int   len;
        int   samples;

        DRCContext drc;

        fseek(fin, 0, SEEK_END);
        len = ftell(fin);
        fseek(fin, 0L, SEEK_SET);

        wav = (unsigned char*)malloc(len);

        fread(wav, sizeof(char), len, fin);

        samples = (int)((wav[43] << 24) |  (wav[42] << 16) | (wav[41] << 8) | wav[40]) / 2;

        drc_init(&drc, 44100, 1);
        drc_apply(&drc, (int16_t*)&wav[44], (int16_t*)&wav[44], samples);

        fwrite(wav, 1, len, fout);
    }
	else
	{
		printf("Open input or output file failed\n");
	}

    if(fin)
    {
        fclose(fin);
    }

    if(fout)
    {
        fclose(fout);
    }

    return 0;
}

#endif
#endif

