#include "plat_types.h"
#include "aud_section.h"
#include "aud_section_inc.h"
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "anc_process.h"
#include "hal_location.h"
#include "hal_trace.h"

#ifdef AUDIO_ANC_FB_MC
#include "cmsis.h"
#endif

#ifdef ANC_COEF_LIST_NUM
#if (ANC_COEF_LIST_NUM < 1)
#error "Invalid ANC_COEF_LIST_NUM configuration"
#endif
#else
#define ANC_COEF_LIST_NUM                   (1)
#endif

extern const struct_anc_cfg * anc_coef_list_50p7k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_48k[ANC_COEF_LIST_NUM];
extern const struct_anc_cfg * anc_coef_list_44p1k[ANC_COEF_LIST_NUM];

const struct_anc_cfg * WEAK anc_coef_list_50p7k[ANC_COEF_LIST_NUM] = { };
const struct_anc_cfg * WEAK anc_coef_list_48k[ANC_COEF_LIST_NUM] = { };
const struct_anc_cfg * WEAK anc_coef_list_44p1k[ANC_COEF_LIST_NUM] = { };

static enum ANC_INDEX cur_coef_idx=ANC_INDEX_0;
static enum AUD_SAMPRATE_T cur_coef_samprate;


#ifdef AUDIO_ANC_FB_MC
#define AUD_IIR_NUM   (8)

typedef struct {
   	aud_item anc_cfg_mc_l;
  	aud_item anc_cfg_mc_r;
    float mc_history_l[AUD_IIR_NUM][4];
    float mc_history_r[AUD_IIR_NUM][4];
} IIR_MC_CFG_T;

static IIR_MC_CFG_T mc_iir_cfg;

#endif

int anc_load_cfg(void)
{
    int res = 0;
    const struct_anc_cfg **list;

#ifdef __AUDIO_RESAMPLE__
    res = anccfg_loadfrom_audsec(anc_coef_list_50p7k, anc_coef_list_48k, ANC_COEF_LIST_NUM);
    list = anc_coef_list_50p7k;
	TRACE("50.7k!!!!");

	if(res) {
		TRACE("[%s] WARNING(%d): Can not load anc coefficient from audio section!!!", __func__, res);
	} else {
		TRACE("[%s] Load anc coefficient from audio section.", __func__);
#if defined(CHIP_BEST1000)
		TRACE("[%s] L: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_l.total_gain, list[0]->anc_cfg_ff_l.fir_len, list[0]->anc_cfg_ff_l.dac_gain_offset, list[0]->anc_cfg_ff_l.adc_gain_offset);
		TRACE("[%s] R: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_r.total_gain, list[0]->anc_cfg_ff_r.fir_len, list[0]->anc_cfg_ff_r.dac_gain_offset, list[0]->anc_cfg_ff_r.adc_gain_offset);
#else
		for(int i = 0; i < ANC_COEF_LIST_NUM; i++) {
			TRACE("appmode%d,FEEDFORWARD,L:gain %d,R:gain %d",i, list[i]->anc_cfg_ff_l.total_gain, list[i]->anc_cfg_ff_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef ff 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[2]);
			}

			TRACE("appmode%d,FEEDBACK,L:gain %d,R:gain %d",i, list[i]->anc_cfg_fb_l.total_gain, list[i]->anc_cfg_fb_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef fb 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[2]);
			}
		}	
#endif
	}

#else
    res = anccfg_loadfrom_audsec(anc_coef_list_48k, anc_coef_list_44p1k, ANC_COEF_LIST_NUM);
    list = anc_coef_list_44p1k;
	TRACE("44.1k!!!!");

	if(res) {
		TRACE("[%s] WARNING(%d): Can not load anc coefficient from audio section!!!", __func__, res);
	} else {
		TRACE("[%s] Load anc coefficient from audio section.", __func__);
#if defined(CHIP_BEST1000)
		TRACE("[%s] L: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_l.total_gain, list[0]->anc_cfg_ff_l.fir_len, list[0]->anc_cfg_ff_l.dac_gain_offset, list[0]->anc_cfg_ff_l.adc_gain_offset);
		TRACE("[%s] R: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_r.total_gain, list[0]->anc_cfg_ff_r.fir_len, list[0]->anc_cfg_ff_r.dac_gain_offset, list[0]->anc_cfg_ff_r.adc_gain_offset);
#else
		for(int i = 0; i < ANC_COEF_LIST_NUM; i++) {
			TRACE("appmode%d,FEEDFORWARD,L:gain %d,R:gain %d",i, list[i]->anc_cfg_ff_l.total_gain, list[i]->anc_cfg_ff_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef ff 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[2]);
			}

			TRACE("appmode%d,FEEDBACK,L:gain %d,R:gain %d",i, list[i]->anc_cfg_fb_l.total_gain, list[i]->anc_cfg_fb_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef fb 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[2]);
			}
		}	
#endif
	}

    res = anccfg_loadfrom_audsec(anc_coef_list_48k, anc_coef_list_44p1k, ANC_COEF_LIST_NUM);
    list = anc_coef_list_48k;
	TRACE("48k!!!!");

	if(res) {
		TRACE("[%s] WARNING(%d): Can not load anc coefficient from audio section!!!", __func__, res);
	} else {
		TRACE("[%s] Load anc coefficient from audio section.", __func__);
#if defined(CHIP_BEST1000)
		TRACE("[%s] L: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_l.total_gain, list[0]->anc_cfg_ff_l.fir_len, list[0]->anc_cfg_ff_l.dac_gain_offset, list[0]->anc_cfg_ff_l.adc_gain_offset);
		TRACE("[%s] R: gain = %d, len = %d, dac = %d, adc = %d", __func__, list[0]->anc_cfg_ff_r.total_gain, list[0]->anc_cfg_ff_r.fir_len, list[0]->anc_cfg_ff_r.dac_gain_offset, list[0]->anc_cfg_ff_r.adc_gain_offset);
#else
		for(int i = 0; i < ANC_COEF_LIST_NUM; i++) {
			TRACE("appmode%d,FEEDFORWARD,L:gain %d,R:gain %d",i, list[i]->anc_cfg_ff_l.total_gain, list[i]->anc_cfg_ff_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef ff 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_ff_l.iir_coef[j].coef_a[2]);
			}

			TRACE("appmode%d,FEEDBACK,L:gain %d,R:gain %d",i, list[i]->anc_cfg_fb_l.total_gain, list[i]->anc_cfg_fb_r.total_gain);
			for(int j = 0; j <AUD_IIR_NUM; j++)
			{
				TRACE("appmode%d,iir coef fb 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x", i, \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_b[2], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[0], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[1], \
				list[i]->anc_cfg_fb_l.iir_coef[j].coef_a[2]);
			}
		}	
#endif
	}
#endif
	return res;
}



int anc_select_coef(enum AUD_SAMPRATE_T rate,enum ANC_INDEX index,enum ANC_TYPE_T anc_type,ANC_GAIN_TIME anc_gain_delay)
{
    const struct_anc_cfg **list=NULL;

    if (index >= ANC_COEF_LIST_NUM) {
        return 1;
    }

#ifdef CHIP_BEST1000
    switch(rate)
    {
        case AUD_SAMPRATE_96000:
            list=anc_coef_list_48k;
            break;

        case AUD_SAMPRATE_88200:
            list=anc_coef_list_44p1k;
            break;        
            
        default:
            break;
    }

#else    
    switch(rate)
    {
        case AUD_SAMPRATE_48000:
            list=anc_coef_list_48k;
            break;

        case AUD_SAMPRATE_44100:
            list=anc_coef_list_44p1k;
            break;
            
#ifdef __AUDIO_RESAMPLE__
        case AUD_SAMPRATE_50781:
            list=anc_coef_list_50p7k;
            break;
#endif

        default:
            break;
    }
#endif

    ASSERT(list!=NULL&&list[index]!=NULL,"The coefs of Samprate %d is NULL",  rate);

    if(anc_opened(anc_type))
    {
        anc_set_cfg(list[index],anc_type,anc_gain_delay);
#ifdef AUDIO_ANC_FB_MC        
        mc_iir_cfg.anc_cfg_mc_l=(*list[index]).anc_cfg_mc_l;
        mc_iir_cfg.anc_cfg_mc_r=(*list[index]).anc_cfg_mc_r;
#endif        
    }

    cur_coef_idx = index;
    cur_coef_samprate=rate;

    return 0;
}

enum ANC_INDEX anc_get_current_coef_index(void)
{
    return cur_coef_idx;
}

enum AUD_SAMPRATE_T anc_get_current_coef_samplerate(void)
{
    return cur_coef_samprate;
}

#ifdef AUDIO_ANC_FB_MC

void anc_mc_run_init(enum AUD_SAMPRATE_T rate)
{
    const struct_anc_cfg **list=NULL;

    switch(rate)
    {
        case AUD_SAMPRATE_48000:
        list=anc_coef_list_48k;
        break;

        case AUD_SAMPRATE_44100:
        list=anc_coef_list_44p1k;
        break;
            
#ifdef __AUDIO_RESAMPLE__
        case AUD_SAMPRATE_50781:
        list=anc_coef_list_50p7k;
        break;
#endif
        default:
            break;
    }
    
    ASSERT(list!=NULL&&list[cur_coef_idx]!=NULL,"The coefs of Samprate %d is NULL",  rate);

    mc_iir_cfg.anc_cfg_mc_l=(*list[cur_coef_idx]).anc_cfg_mc_l;
    mc_iir_cfg.anc_cfg_mc_r=(*list[cur_coef_idx]).anc_cfg_mc_r;

    for(int j=0;j<AUD_IIR_NUM;j++)
    {
        for(int i=0;i<4;i++)
        {
            mc_iir_cfg.mc_history_l[j][i]=0.0f;
            mc_iir_cfg.mc_history_r[j][i]=0.0f;
        }
    }
    return;
}


void anc_mc_run_setup(enum AUD_SAMPRATE_T rate)
{
    const struct_anc_cfg **list=NULL;

    switch(rate)
    {
        case AUD_SAMPRATE_48000:
        list=anc_coef_list_48k;
        break;

        case AUD_SAMPRATE_44100:
        list=anc_coef_list_44p1k;
        break;
            
#ifdef __AUDIO_RESAMPLE__
        case AUD_SAMPRATE_50781:
        list=anc_coef_list_50p7k;
        break;
#endif
        default:
            break;
    }
    
    ASSERT(list!=NULL&&list[cur_coef_idx]!=NULL,"The coefs of Samprate %d is NULL",  rate);

    mc_iir_cfg.anc_cfg_mc_l=(*list[cur_coef_idx]).anc_cfg_mc_l;
    mc_iir_cfg.anc_cfg_mc_r=(*list[cur_coef_idx]).anc_cfg_mc_r;

    return;
}


static inline int32_t iir_ssat_20bits(float in)
{
    int res = 0;
    int32_t out;

    res = (int)in;
    out = __SSAT(res,20);

    return out;
}

static inline int32_t iir_ssat_16bits(float in)
{
    int res = 0;
    int32_t out;

    res = (int)in;
    out = __SSAT(res,16);

    return out;
}

SRAM_TEXT_LOC int anc_mc_run_stereo(uint8_t *buf, int len,float left_gain,float right_gain,enum AUD_BITS_T sample_bit)
{
    int len_mono;
    int num;
    float gain_l = 0, gain_r = 0;
    int32_t *coefs = NULL;
    float *history = NULL;
    
    // Left
    float x0, x1, x2;
    float y0, y1, y2;

    // Right
    float X0, X1, X2;
    float Y0, Y1, Y2;

    // Coefs
    float POSSIBLY_UNUSED a0, a1, a2;
    float b0, b1, b2;
    
    ASSERT(mc_iir_cfg.anc_cfg_mc_l.iir_counter==mc_iir_cfg.anc_cfg_mc_r.iir_counter, "mc need the same counter in left and right ch");

    if(sample_bit==AUD_BITS_16)
    {
        int16_t *iir_buf;

        len_mono = len>>2;
        
        gain_l = (mc_iir_cfg.anc_cfg_mc_l.total_gain/512.0f)*left_gain;
        gain_r = (mc_iir_cfg.anc_cfg_mc_r.total_gain/512.0f)*right_gain;
        num = mc_iir_cfg.anc_cfg_mc_l.iir_counter;


        if(num==0||mc_iir_cfg.anc_cfg_mc_l.iir_bypass_flag)
        {
            iir_buf = (int16_t *)buf;

            for(int j=0; j<len_mono; j++)
            {
                x0 = *iir_buf*gain_l;
                *iir_buf++ = iir_ssat_16bits(x0);

                x0 = *iir_buf*gain_r;
                *iir_buf++ = iir_ssat_16bits(x0);
            }

            return 0;
        }

        for(int i=0; i<num; i++)
        {
            // Coef
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_a;
            a0 = *coefs++;
            a1 = *coefs++;
            a2 = *coefs;
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_b;
            b0 = *coefs++;
            b1 = *coefs++;
            b2 = *coefs;

            a1=a1/a0;
            a2=a2/a0;
            b0=b0/a0;
            b1=b1/a0;
            b2=b2/a0;

          //TRACE("[%d] %f, %f, %f, %f, %f, %f", i, a0, a1, a2, b0, b1, b2);

            // Left
            history = mc_iir_cfg.mc_history_l[i];
            x1 = *history++;
            x2 = *history++;
            y1 = *history++;
            y2 = *history;

            // Right
            history = mc_iir_cfg.mc_history_r[i];
            X1 = *history++;
            X2 = *history++;
            Y1 = *history++;
            Y2 = *history;

            iir_buf = (int16_t *)buf;
            if(i==0)
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf*gain_l;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_16bits(y0);

                    // Righy channel
                    X0 = *iir_buf*gain_r;
                    Y0 = X0*b0 + X1*b1 + X2*b2 - Y1*a1 - Y2*a2;
                    Y2 = Y1;
                    Y1 = Y0;
                    X2 = X1;
                    X1 = X0;
                    *iir_buf++ = iir_ssat_16bits(Y0);
                }
            }
            else
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_16bits(y0);
                    // Righy channel
                    X0 = *iir_buf;
                    Y0 = X0*b0 + X1*b1 + X2*b2 - Y1*a1 - Y2*a2;
                    Y2 = Y1;
                    Y1 = Y0;
                    X2 = X1;
                    X1 = X0;
                    *iir_buf++ = iir_ssat_16bits(Y0);

                }
            }
            // Left
            history = mc_iir_cfg.mc_history_l[i];
            *history++ = x1;
            *history++ = x2;
            *history++ = y1;
            *history = y2;
            // Right
            history = mc_iir_cfg.mc_history_r[i];
            *history++ = X1;
            *history++ = X2;
            *history++ = Y1;
            *history = Y2;
        }

    }
    else if(sample_bit==AUD_BITS_24)
    {
        int32_t *iir_buf;
        
        len_mono = len>>3;

        gain_l = (mc_iir_cfg.anc_cfg_mc_l.total_gain/512.0f)*left_gain;
        gain_r = (mc_iir_cfg.anc_cfg_mc_r.total_gain/512.0f)*right_gain;
        num = mc_iir_cfg.anc_cfg_mc_l.iir_counter;

        if(num==0||mc_iir_cfg.anc_cfg_mc_l.iir_bypass_flag)
        {
            iir_buf = (int32_t *)buf;

            for(int j=0; j<len_mono; j++)
            {
                x0 = *iir_buf*gain_l;
                *iir_buf++ = iir_ssat_20bits(x0);

                x0 = *iir_buf*gain_r;
                *iir_buf++ = iir_ssat_20bits(x0);
            }

            return 0;
        }

        for(int i=0; i<num; i++)
        {
            // Coef
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_a;
            a0 = *coefs++;
            a1 = *coefs++;
            a2 = *coefs;
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_b;
            b0 = *coefs++;
            b1 = *coefs++;
            b2 = *coefs;
            
            a1=a1/a0;
            a2=a2/a0;
            b0=b0/a0;
            b1=b1/a0;
            b2=b2/a0;

           // TRACE("[%d] %f, %f, %f, %f, %f, %f", i, a0, a1, a2, b0, b1, b2);

            // Left
            history = mc_iir_cfg.mc_history_l[i];
            x1 = *history++;
            x2 = *history++;
            y1 = *history++;
            y2 = *history;

            // Right
            history = mc_iir_cfg.mc_history_r[i];
            X1 = *history++;
            X2 = *history++;
            Y1 = *history++;
            Y2 = *history;

            iir_buf = (int32_t *)buf;
            if(i==0)
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf*gain_l;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_20bits(y0);

                    // Righy channel
                    X0 = *iir_buf*gain_r;
                    Y0 = X0*b0 + X1*b1 + X2*b2 - Y1*a1 - Y2*a2;
                    Y2 = Y1;
                    Y1 = Y0;
                    X2 = X1;
                    X1 = X0;
                    *iir_buf++ = iir_ssat_20bits(Y0);
                }
            }
            else
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_20bits(y0);
                    // Righy channel
                    X0 = *iir_buf;
                    Y0 = X0*b0 + X1*b1 + X2*b2 - Y1*a1 - Y2*a2;
                    Y2 = Y1;
                    Y1 = Y0;
                    X2 = X1;
                    X1 = X0;
                    *iir_buf++ = iir_ssat_20bits(Y0);

                }
            }
            // Left
            history = mc_iir_cfg.mc_history_l[i];
            *history++ = x1;
            *history++ = x2;
            *history++ = y1;
            *history = y2;
            // Right
            history = mc_iir_cfg.mc_history_r[i];
            *history++ = X1;
            *history++ = X2;
            *history++ = Y1;
            *history = Y2;
        }
        
    }
    else
    {
        ASSERT(false, "Can't support sample bit mode:%d",sample_bit);
    }

    return 0;
}

SRAM_TEXT_LOC int anc_mc_run_mono(uint8_t *buf, int len,float left_gain,enum AUD_BITS_T sample_bit)
{
    int len_mono;
    int num;
    float gain_l = 0;
    int32_t *coefs = NULL;
    float *history = NULL;
    
    // Left
    float x0, x1, x2;
    float y0, y1, y2;

    // Coefs
    float POSSIBLY_UNUSED a0, a1, a2;
    float b0, b1, b2;

    if(sample_bit==AUD_BITS_16)
    {
        int16_t *iir_buf;

        len_mono = len>>1;
        
        gain_l = (mc_iir_cfg.anc_cfg_mc_l.total_gain/512.0f)*left_gain;
        num = mc_iir_cfg.anc_cfg_mc_l.iir_counter;

        if(num==0||mc_iir_cfg.anc_cfg_mc_l.iir_bypass_flag)
        {
            iir_buf = (int16_t *)buf;

            for(int j=0; j<len_mono; j++)
            {
                x0 = *iir_buf*gain_l;
                *iir_buf++ = iir_ssat_16bits(x0);
            }

            return 0;
        }

        for(int i=0; i<num; i++)
        {
            // Coef
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_a;
            a0 = *coefs++;
            a1 = *coefs++;
            a2 = *coefs;
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_b;
            b0 = *coefs++;
            b1 = *coefs++;
            b2 = *coefs;

            a1=a1/a0;
            a2=a2/a0;
            b0=b0/a0;
            b1=b1/a0;
            b2=b2/a0;

    //      TRACE("[%d] %f, %f, %f, %f, %f, %f", i, a0, a1, a2, b0, b1, b2);

            // Left
            history = mc_iir_cfg.mc_history_l[i];
            x1 = *history++;
            x2 = *history++;
            y1 = *history++;
            y2 = *history;
            
            iir_buf = (int16_t *)buf;
            if(i==0)
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf*gain_l;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_16bits(y0);
                }
            }
            else
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_16bits(y0);
                }
            }
            // Left
            history = mc_iir_cfg.mc_history_l[i];
            *history++ = x1;
            *history++ = x2;
            *history++ = y1;
            *history = y2;
        }

    }
    else if(sample_bit==AUD_BITS_24)
    {
        int32_t *iir_buf;
        
        len_mono = len>>2;

        gain_l = (mc_iir_cfg.anc_cfg_mc_l.total_gain/512.0f)*left_gain;
        num = mc_iir_cfg.anc_cfg_mc_l.iir_counter;

        if(num==0)
        {
            iir_buf = (int32_t *)buf;

            for(int j=0; j<len_mono; j++)
            {
                x0 = *iir_buf*gain_l;
                *iir_buf++ = iir_ssat_20bits(x0);
            }

            return 0;
        }

        for(int i=0; i<num; i++)
        {
            // Coef
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_a;
            a0 = *coefs++;
            a1 = *coefs++;
            a2 = *coefs;
            coefs = mc_iir_cfg.anc_cfg_mc_l.iir_coef[i].coef_b;
            b0 = *coefs++;
            b1 = *coefs++;
            b2 = *coefs;

            a1=a1/a0;
            a2=a2/a0;
            b0=b0/a0;
            b1=b1/a0;
            b2=b2/a0;


    //        TRACE("[%d] %f, %f, %f, %f, %f, %f", i, a0, a1, a2, b0, b1, b2);

            // Left
            history = mc_iir_cfg.mc_history_l[i];
            x1 = *history++;
            x2 = *history++;
            y1 = *history++;
            y2 = *history;

            iir_buf = (int32_t *)buf;
            if(i==0)
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf*gain_l;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_20bits(y0);
                }
            }
            else
            {
                for(int j=0; j<len_mono; j++)
                {
                    // Left channel
                    x0 = *iir_buf;
                    y0 = x0*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
                    y2 = y1;
                    y1 = y0;
                    x2 = x1;
                    x1 = x0;
                    *iir_buf++ = iir_ssat_20bits(y0);
                }
            }
            // Left
            history = mc_iir_cfg.mc_history_l[i];
            *history++ = x1;
            *history++ = x2;
            *history++ = y1;
            *history = y2;
        }
        
    }
    else
    {
        ASSERT(false, "Can't support sample bit mode:%d",sample_bit);
    }

    return 0;
}


#endif
