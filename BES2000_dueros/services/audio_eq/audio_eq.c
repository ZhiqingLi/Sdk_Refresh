/*******************************************************************************
** namer：	Audio_eq
** description:	fir and iir eq manager
** version：V0.9
** author： Yunjie Huo
** modify：	2016.12.26
** todo: 	1.
*******************************************************************************/
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "string.h"
#include "audio_eq.h"
#include "stdbool.h"
#include "hal_location.h"
#include "hw_codec_iir_process.h"
#include "hw_iir_process.h"
#include "tgt_hardware.h"



#ifdef __PC_CMD_UART__
#include "communication_svr.h"

#if defined(__SW_IIR_EQ_PROCESS__)
#define AUDIO_EQ_SW_IIR_DYNAMIC    1
#endif

#if defined(__HW_DAC_IIR_EQ_PROCESS__) 
#define AUDIO_EQ_HW_DAC_IIR_DYNAMIC    1
#endif

#if defined(__HW_IIR_EQ_PROCESS__)
#define AUDIO_EQ_HW_IIR_DYNAMIC    1
#endif

#ifdef __HW_FIR_EQ_PROCESS__
#define AUDIO_EQ_HW_FIR_DYNAMIC    1
#endif

#if defined(__AUDIO_DRC__)
#define AUDIO_EQ_DRC_DYNAMIC    1
#endif
#endif

#if defined(AUDIO_EQ_SW_IIR_DYNAMIC) || defined(AUDIO_EQ_HW_FIR_DYNAMIC)|| defined(AUDIO_EQ_HW_DAC_IIR_DYNAMIC)|| defined(AUDIO_EQ_HW_IIR_DYNAMIC) ||defined(AUDIO_EQ_DRC_DYNAMIC)
#define AUDIO_EQ_DYNAMIC        1
#endif

typedef signed int          audio_eq_sample_24bits_t;
typedef signed short int    audio_eq_sample_16bits_t;

typedef struct{
    enum AUD_BITS_T     sample_bits;
    enum AUD_SAMPRATE_T sample_rate;
    enum AUD_CHANNEL_NUM_T ch_num; 

#if defined(__SW_IIR_EQ_PROCESS__)
    bool sw_iir_enable;
#endif

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    bool hw_dac_iir_enable;
#endif

#if defined(__HW_IIR_EQ_PROCESS__)
    bool hw_iir_enable;
#endif

#if defined(__HW_FIR_EQ_PROCESS__)
    bool hw_fir_enable;
#endif

#ifdef __AUDIO_DRC__
	bool drc_enable;
#endif

#ifdef AUDIO_EQ_DYNAMIC
    bool update_cfg;
#endif

#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    IIR_CFG_T sw_iir_cfg;
#endif

#ifdef AUDIO_EQ_HW_DAC_IIR_DYNAMIC
    IIR_CFG_T hw_dac_iir_cfg;
#endif

#ifdef AUDIO_EQ_HW_IIR_DYNAMIC
    IIR_CFG_T hw_iir_cfg;
#endif

#ifdef AUDIO_EQ_HW_FIR_DYNAMIC
    FIR_CFG_T hw_fir_cfg;
#endif

#ifdef AUDIO_EQ_DRC_DYNAMIC
	drc_settings_t First_level_drc;
	drc_2_settings_t Sencond_level_drc;
#endif
} AUDIO_EQ_T;

static AUDIO_EQ_T audio_eq = {
    .sample_bits = AUD_BITS_NULL,
    .sample_rate = AUD_SAMPRATE_NULL,
    .ch_num = AUD_CHANNEL_NUM_NULL,
    
#if defined(__SW_IIR_EQ_PROCESS__) 
    .sw_iir_enable =  false,
#endif
#if defined(__HW_DAC_IIR_EQ_PROCESS__)
    .hw_dac_iir_enable =  false,
#endif
#if defined(__HW_IIR_EQ_PROCESS__)   
    .hw_iir_enable =  false,
#endif
#if defined(__HW_FIR_EQ_PROCESS__)   
    .hw_fir_enable =  false,
#endif
#ifdef __AUDIO_DRC__
	.drc_enable=false,
#endif
#ifdef AUDIO_EQ_DYNAMIC
    .update_cfg = false,
#endif

#if 0
#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    .sw_iir_cfg={},
#endif
#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    .hw_dac_iir_cfg={},
#endif
#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    .hw_iir_cfg={},
#endif
#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    .hw_fir_cfg={},
#endif
#ifdef AUDIO_EQ_DRC_DYNAMIC
	.First_level_drc={},
	.Sencond_level_drc={},
#endif
#endif
};


extern const IIR_CFG_T audio_eq_iir_cfg;
extern const IIR_CFG_T audio_eq_hw_dac_iir_cfg;
extern const drc_settings_t audio_eq_drc_cfg;
extern const drc_2_settings_t audio_eq_second_drc_cfg;



int audio_eq_set_cfg(const FIR_CFG_T *fir_cfg,const IIR_CFG_T *iir_cfg,AUDIO_EQ_TYPE_T audio_eq_type)
{

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__)|| defined(__HW_DAC_IIR_EQ_PROCESS__)|| defined(__HW_IIR_EQ_PROCESS__)
    switch (audio_eq_type)
    {
#if defined(__SW_IIR_EQ_PROCESS__)
        case AUDIO_EQ_TYPE_SW_IIR:
        {
            if(iir_cfg)
            {
                audio_eq.sw_iir_enable = false;
                iir_set_cfg(iir_cfg);
                audio_eq.sw_iir_enable = true;
            }
            else
            {
                audio_eq.sw_iir_enable = false;
            }
        }           
        break;
#endif

#if defined(__HW_FIR_EQ_PROCESS__)
        case AUDIO_EQ_TYPE_HW_FIR:
        {
            if(fir_cfg)
            {
                audio_eq.hw_fir_enable = false;
                fir_set_cfg(fir_cfg);
                audio_eq.hw_fir_enable = true;
            }
            else
            {
                audio_eq.hw_fir_enable = false;
            }
        }                
        break;
#endif

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
        case AUDIO_EQ_TYPE_HW_DAC_IIR:
        {
            if(iir_cfg)
            {
                HW_CODEC_IIR_CFG_T *hw_iir_cfg_dac=NULL; 
#ifdef __AUDIO_RESAMPLE__
                enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
                enum AUD_SAMPRATE_T sample_rate_hw_iir=audio_eq.sample_rate;
#endif
                audio_eq.hw_dac_iir_enable = false;
                hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_iir,iir_cfg);
                ASSERT(hw_iir_cfg_dac != NULL, "[%s] %d codec IIR parameter error!", __func__, hw_iir_cfg_dac);
                hw_codec_iir_set_cfg(hw_iir_cfg_dac, sample_rate_hw_iir, HW_CODEC_IIR_DAC);
                audio_eq.hw_dac_iir_enable = true;
            }
            else
            {
                audio_eq.hw_dac_iir_enable = false;
            }
        }
        break;
#endif

#if defined(__HW_IIR_EQ_PROCESS__)
        case AUDIO_EQ_TYPE_HW_IIR:
        {
            if(iir_cfg)
            {
                HW_IIR_CFG_T *hw_iir_cfg=NULL; 
#ifdef __AUDIO_RESAMPLE__
                enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
                enum AUD_SAMPRATE_T sample_rate_hw_iir=audio_eq.sample_rate;
#endif
                audio_eq.hw_iir_enable = false;
                hw_iir_cfg = hw_iir_get_cfg(sample_rate_hw_iir,iir_cfg);
                ASSERT(hw_iir_cfg != NULL, "[%s] %d codec IIR parameter error!", __func__, hw_iir_cfg);
                hw_iir_set_cfg(hw_iir_cfg);
                audio_eq.hw_iir_enable = true;
            }
            else
            {
                audio_eq.hw_iir_enable = false;
            }
            
        } 
        break;
#endif

        default:
        {
            ASSERT(false,"[%s]Error eq type!",__func__);
        }
    }  
#endif

    return 0;
}

#ifdef __AUDIO_DRC__
int audio_drc_set_cfg(const drc_settings_t* drc1_cfg,const drc_2_settings_t *drc2_cfg,AUDIO_DRC_TYPE_T audio_drc_type)
{
	switch (audio_drc_type)
	{
	   case AUDIO_DRC_TYPE_FIRST:
       {
			if(drc1_cfg)
			{
		        TRACE("FIRST DRC LOCAL SETING SUCCESS!!!");
		        drc_set_cfg(drc1_cfg);
			    audio_eq.drc_enable = true;
	 		}
			else
	 		{
				audio_eq.drc_enable = false;
			}
	   }
	   break;
       case AUDIO_DRC_TYPE_SECOND:
       {
			if(drc2_cfg)
			{
				TRACE("SECOND DRC SETTING SUCCESS!!!");
				speech_drc_setup(drc2_cfg);
				audio_eq.drc_enable = true;
			}
			else
	 		{
				audio_eq.drc_enable = false;
			}
       }
	   break;
	   default:
	   {
		   ASSERT(false,"[%s]Error eq type!",__func__);
	   }

	}

	return 0;
}
#endif

//extern void af_codec_playback_post_handler_ext_proc(void);

int SRAM_TEXT_LOC audio_eq_run(uint8_t *buf, uint32_t len)
{
    int POSSIBLY_UNUSED eq_num = 0;
    
    if(audio_eq.sample_bits == AUD_BITS_16)
    {
        eq_num = len/sizeof(audio_eq_sample_16bits_t);
    }
    else if(audio_eq.sample_bits == AUD_BITS_24)
    {
        eq_num = len/sizeof(audio_eq_sample_24bits_t);
    }
    else
    {
        ASSERT(0, "[%s] bits(%d) is invalid", __func__, audio_eq.sample_bits);
    }

//    int32_t s_time,e_time;
//    s_time = hal_sys_timer_get();
#ifdef __ANKER_UI__	
	//af_codec_playback_post_handler_ext_proc();//volume set
#endif
#ifdef __SW_IIR_EQ_PROCESS__
    if(audio_eq.sw_iir_enable)
    {
        iir_run(buf, eq_num);
    }
#endif

#ifdef __HW_FIR_EQ_PROCESS__
    if(audio_eq.hw_fir_enable)
    {
        fir_run(buf, eq_num);
    }
#endif

#ifdef __HW_IIR_EQ_PROCESS__
    if(audio_eq.hw_iir_enable)
    {
        hw_iir_run(buf, eq_num);
    }
#endif

#ifdef __AUDIO_DRC__
	if(audio_eq.drc_enable)
	{
		dynamic_drc_set_cfg();//updata drc element
		dynamic_second_drc_set_cfg();

		//drc1
		audio_drc_process((short*)buf, 0);
        // drc2
        audio_second_drc_process(buf, 0);
	}
#endif

//    e_time = hal_sys_timer_get();
//    TRACE("[%s] Sample len = %d, used time = %d ticks", __func__, eq_len, (e_time - s_time));
    return 0;
}


volatile bool second_drc_updata=false;
int audio_eq_open(enum AUD_SAMPRATE_T sample_rate, enum AUD_BITS_T sample_bits, enum AUD_CHANNEL_NUM_T ch_num,void *eq_buf, uint32_t len)
{
    TRACE("[%s] sample_rate = %d, sample_bits = %d", __func__, sample_rate, sample_bits);

    audio_eq.sample_rate = sample_rate;
    audio_eq.sample_bits = sample_bits;
    audio_eq.ch_num = ch_num;

#if defined(__HW_FIR_EQ_PROCESS__) && defined(__HW_IIR_EQ_PROCESS__)
    void *fir_eq_buf=eq_buf;
    uint32_t fir_len=len/2;
    void *iir_eq_buf=(uint8_t *)eq_buf+fir_len;
    uint32_t iir_len=len/2;
#elif defined(__HW_FIR_EQ_PROCESS__) && !defined(__HW_IIR_EQ_PROCESS__)
    void *fir_eq_buf=eq_buf;
    uint32_t fir_len=len;
#elif !defined(__HW_FIR_EQ_PROCESS__) && defined(__HW_IIR_EQ_PROCESS__)
    void *iir_eq_buf=eq_buf;
    uint32_t iir_len=len;
#endif
    
#ifdef __SW_IIR_EQ_PROCESS__
    iir_open(sample_rate, sample_bits);
#endif

#ifdef __HW_FIR_EQ_PROCESS__
#if defined(CHIP_BEST1000) && defined(FIR_HIGHSPEED)
    hal_cmu_fir_high_speed_enable(HAL_CMU_FIR_USER_EQ);
#endif
    fir_open(sample_rate, sample_bits,ch_num, fir_eq_buf, fir_len);
#endif

#ifdef __HW_DAC_IIR_EQ_PROCESS__
#ifdef __AUDIO_RESAMPLE__
    enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
    enum AUD_SAMPRATE_T sample_rate_hw_iir=sample_rate;
#endif
    hw_codec_iir_open(sample_rate_hw_iir,HW_CODEC_IIR_DAC);
#endif

#ifdef __HW_IIR_EQ_PROCESS__
#ifdef __AUDIO_RESAMPLE__
    enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
    enum AUD_SAMPRATE_T sample_rate_hw_iir=sample_rate;
#endif
    hw_iir_open(sample_rate_hw_iir, sample_bits,ch_num, iir_eq_buf, iir_len);
#endif

#ifdef __AUDIO_DRC__
	audio_drc_open(sample_rate, len/2/4);
	speech_drc_open(sample_rate,len/2/4);
#endif

//set eq and drc local param
#if defined(__SW_IIR_EQ_PROCESS__)	
#ifdef AUDIO_EQ_SW_IIR_DYNAMIC
    if (audio_eq.sw_iir_cfg.num)
    {
        audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq.sw_iir_cfg),AUDIO_EQ_TYPE_SW_IIR);
    }
    else
    {
		audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq_iir_cfg),AUDIO_EQ_TYPE_SW_IIR);
    }
#else
	audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq_iir_cfg),AUDIO_EQ_TYPE_SW_IIR);
#endif
#endif

#if defined(__HW_DAC_IIR_EQ_PROCESS__)
#ifdef AUDIO_EQ_HW_DAC_IIR_DYNAMIC
    if (audio_eq.hw_dac_iir_cfg.num)
    {
        audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq.hw_dac_iir_cfg),AUDIO_EQ_TYPE_HW_DAC_IIR);
    }
    else
    {
		audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq_hw_dac_iir_cfg),AUDIO_EQ_TYPE_HW_DAC_IIR);
    }
#else
	audio_eq_set_cfg(NULL,(IIR_CFG_T *)&(audio_eq_hw_dac_iir_cfg),AUDIO_EQ_TYPE_HW_DAC_IIR);
#endif
#endif

#ifdef __AUDIO_DRC__
#ifdef AUDIO_EQ_DRC_DYNAMIC
    if (audio_eq.First_level_drc.band_num)
    {
        audio_drc_set_cfg(&audio_eq.First_level_drc,NULL,AUDIO_DRC_TYPE_FIRST);
    }
    else
    {
		audio_drc_set_cfg(&audio_eq_drc_cfg,NULL,AUDIO_DRC_TYPE_FIRST);
    }
#else
	audio_drc_set_cfg(&audio_eq_drc_cfg,NULL,AUDIO_DRC_TYPE_FIRST);
#endif

//FIX ME 
	if(!second_drc_updata)
	{
		audio_drc_set_cfg(NULL,&audio_eq_second_drc_cfg,AUDIO_DRC_TYPE_SECOND);
	}
#endif

	return 0;
}

int audio_eq_close(void)
{
#ifdef __SW_IIR_EQ_PROCESS__
    audio_eq.sw_iir_enable = false;
    iir_close();
#endif

#ifdef __AUDIO_DRC__
	audio_eq.drc_enable = false;
#endif

#ifdef __HW_FIR_EQ_PROCESS__
    audio_eq.hw_fir_enable = false;
    fir_close();
#if defined(CHIP_BEST1000) && defined(FIR_HIGHSPEED)
    hal_cmu_fir_high_speed_disable(HAL_CMU_FIR_USER_EQ);
#endif
#endif

#ifdef __HW_DAC_IIR_EQ_PROCESS__
    audio_eq.hw_dac_iir_enable = false;
    hw_codec_iir_close(HW_CODEC_IIR_DAC);
#endif

#ifdef __HW_IIR_EQ_PROCESS__
    audio_eq.hw_iir_enable = false;
    hw_iir_close();
#endif
   
#ifdef __PC_CMD_UART__	
	communication_deinit();
#endif
    
    return 0;
}

#ifdef __PC_CMD_UART__

int audio_tool_ping_callback(uint8_t *buf, uint32_t len)
{
    //TRACE("");
    return 0;
}


#ifdef AUDIO_EQ_SW_IIR_DYNAMIC

#ifndef USB_AUDIO_APP
int audio_eq_sw_iir_callback(uint8_t *buf, uint32_t  len)
{    
    memcpy(&audio_eq.sw_iir_cfg, buf, sizeof(IIR_CFG_T));
    TRACE("band num:%d gain0:%d, gain1:%d", 
                                                (int32_t)audio_eq.sw_iir_cfg.num,
                                                (int32_t)(audio_eq.sw_iir_cfg.gain0*10),      
                                                (int32_t)(audio_eq.sw_iir_cfg.gain1*10));
    for (uint8_t i = 0; i<audio_eq.sw_iir_cfg.num; i++){
        TRACE("band num:%d type %d gain:%d fc:%d q:%d", i,
			                           (int32_t)(audio_eq.sw_iir_cfg.param[i].type), 
                                                (int32_t)(audio_eq.sw_iir_cfg.param[i].gain*10), 
                                                (int32_t)(audio_eq.sw_iir_cfg.param[i].fc*10), 
                                                (int32_t)(audio_eq.sw_iir_cfg.param[i].Q*10));
    }

#ifdef __SW_IIR_EQ_PROCESS__
    {
        iir_set_cfg(&audio_eq.sw_iir_cfg);
        audio_eq.sw_iir_enable = true;
    }
#endif
   
    return 0;
}
#else
int audio_eq_sw_iir_callback(uint8_t *buf, uint32_t  len)
{
    // IIR_CFG_T *rx_iir_cfg = NULL;

    // rx_iir_cfg = (IIR_CFG_T *)buf;

    // TRACE("[%s] left gain = %f, right gain = %f", __func__, rx_iir_cfg->gain0, rx_iir_cfg->gain1);

    // for(int i=0; i<rx_iir_cfg->num; i++)
    // {
    //     TRACE("[%s] iir[%d] gain = %f, f = %f, Q = %f", __func__, i, rx_iir_cfg->param[i].gain, rx_iir_cfg->param[i].fc, rx_iir_cfg->param[i].Q);
    // }

    // audio_eq_set_cfg(NULL,(const IIR_CFG_T *)rx_iir_cfg,AUDIO_EQ_TYPE_SW_IIR);

    update_iir_cfg_tbl(buf, len);

    return 0;
}
#endif
#endif


#ifdef AUDIO_EQ_HW_FIR_DYNAMIC
int audio_eq_hw_fir_callback(uint8_t *buf, uint32_t  len)
{
    FIR_CFG_T *rx_fir_cfg = NULL;

    rx_fir_cfg = (FIR_CFG_T *)buf;

    TRACE("[%s] left gain = %d, right gain = %d", __func__, rx_fir_cfg->gain0, rx_fir_cfg->gain1);

    TRACE("[%s] len = %d, coef: %d, %d......%d, %d", __func__, rx_fir_cfg->len, rx_fir_cfg->coef[0], rx_fir_cfg->coef[1], rx_fir_cfg->coef[rx_fir_cfg->len-2], rx_fir_cfg->coef[rx_fir_cfg->len-1]);

    rx_fir_cfg->gain0 = 6;
    rx_fir_cfg->gain1 = 6;

    if(rx_fir_cfg)
    {
        memcpy(&audio_eq.fir_cfg, rx_fir_cfg, sizeof(audio_eq.fir_cfg));
        audio_eq.fir_enable = true;
        fir_set_cfg(&audio_eq.fir_cfg);
    }
    else
    {
        audio_eq.fir_enable = false;
    }
    
    return 0;
}
#endif

#ifdef AUDIO_EQ_HW_DAC_IIR_DYNAMIC
int audio_eq_hw_dac_iir_callback(uint8_t *buf, uint32_t  len)
{
    memcpy(&audio_eq.hw_dac_iir_cfg, buf, sizeof(IIR_CFG_T));
    TRACE("band num:%d gain0:%d, gain1:%d", 
                                                (int32_t)audio_eq.hw_dac_iir_cfg.num,
                                                (int32_t)(audio_eq.hw_dac_iir_cfg.gain0*10),      
                                                (int32_t)(audio_eq.hw_dac_iir_cfg.gain1*10));
    for (uint8_t i = 0; i<audio_eq.hw_dac_iir_cfg.num; i++){
        TRACE("band num:%d type %d gain:%d fc:%d q:%d", i,
			                           (int32_t)(audio_eq.hw_dac_iir_cfg.param[i].type), 
                                                (int32_t)(audio_eq.hw_dac_iir_cfg.param[i].gain*10), 
                                                (int32_t)(audio_eq.hw_dac_iir_cfg.param[i].fc*10), 
                                                (int32_t)(audio_eq.hw_dac_iir_cfg.param[i].Q*10));
    }

#ifdef __HW_DAC_IIR_EQ_PROCESS__
    {
        HW_CODEC_IIR_CFG_T *hw_iir_cfg_dac=NULL; 
#ifdef __AUDIO_RESAMPLE__
        enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
        enum AUD_SAMPRATE_T sample_rate_hw_iir=audio_eq.sample_rate;
#endif
        hw_iir_cfg_dac = hw_codec_iir_get_cfg(sample_rate_hw_iir,&audio_eq.hw_dac_iir_cfg);
        ASSERT(hw_iir_cfg_dac != NULL, "[%s] %d codec IIR parameter error!", __func__, hw_iir_cfg_dac);
        hw_codec_iir_set_cfg(hw_iir_cfg_dac, sample_rate_hw_iir, HW_CODEC_IIR_DAC);
        audio_eq.hw_dac_iir_enable = true;
    }
#endif
    
    return 0;
}
#endif


#ifdef AUDIO_EQ_HW_IIR_DYNAMIC

int audio_eq_hw_iir_callback(uint8_t *buf, uint32_t  len)
{
    memcpy(&audio_eq.hw_iir_cfg, buf, sizeof(IIR_CFG_T));
    TRACE("band num:%d gain0:%d, gain1:%d", 
                                                (int32_t)audio_eq.hw_iir_cfg.num,
                                                (int32_t)(audio_eq.hw_iir_cfg.gain0*10),      
                                                (int32_t)(audio_eq.hw_iir_cfg.gain1*10));
    
    for (uint8_t i = 0; i<audio_eq.hw_iir_cfg.num; i++)
    {
        TRACE("band num:%d type %d gain:%d fc:%d q:%d", i,
			                                    (int32_t)(audio_eq.hw_iir_cfg.param[i].type), 
                                                (int32_t)(audio_eq.hw_iir_cfg.param[i].gain*10), 
                                                (int32_t)(audio_eq.hw_iir_cfg.param[i].fc*10), 
                                                (int32_t)(audio_eq.hw_iir_cfg.param[i].Q*10));
    }

#ifdef __HW_IIR_EQ_PROCESS__
    {
        HW_IIR_CFG_T *hw_iir_cfg=NULL; 
#ifdef __AUDIO_RESAMPLE__
        enum AUD_SAMPRATE_T sample_rate_hw_iir=AUD_SAMPRATE_50781;
#else
        enum AUD_SAMPRATE_T sample_rate_hw_iir=audio_eq.sample_rate;
#endif
        hw_iir_cfg = hw_iir_get_cfg(sample_rate_hw_iir,audio_eq.hw_iir_cfg);
        ASSERT(hw_iir_cfg != NULL, "[%s] %d codec IIR parameter error!", __func__, hw_iir_cfg);
        hw_iir_set_cfg(hw_iir_cfg);
        audio_eq.hw_iir_enable = true;
    }
#endif
    return 0;
}
#endif

#ifdef AUDIO_EQ_DRC_DYNAMIC
int audio_eq_drc_callback(uint8_t *buf, uint32_t  len)
{
	TRACE("*******audio_eq_drc_callback***** len=%d",len);
	memcpy((uint8_t *)&audio_eq.First_level_drc,buf,len);
	update_drc_cfg(&audio_eq.First_level_drc, sizeof(drc_settings_t));
    return 0;
}

int audio_eq_drc_2_callback(uint8_t *buf, uint32_t  len)
{
	TRACE("*******audio_eq_drc_2_callback***** len=%d",len);
	second_drc_updata=true;
	memcpy((uint8_t *)&audio_eq.Sencond_level_drc,buf,len);
	speech_drc_update(&audio_eq.Sencond_level_drc);
    return 0;
}

#endif


#endif

typedef struct {
    uint8_t type;
    uint8_t maxEqBandNum;
    uint16_t sample_rate_num;
    uint8_t sample_rate[50];
} query_eq_info_t;


extern int getMaxEqBand(void);
extern int getSampleArray(uint8_t* buf, uint16_t *num);


#define CMD_TYPE_QUERY_DUT_EQ_INFO  0x00




int audio_cmd_callback(uint8_t *buf, uint32_t len)
{
    uint8_t type;
    //uint32_t* sample_rate;
    //uint8_t* p;
    query_eq_info_t  info;

    type = buf[0];
    //p = buf + 1;

    TRACE("%s type: %d", __func__, type);
    switch (type) {
        case CMD_TYPE_QUERY_DUT_EQ_INFO:
            info.type = CMD_TYPE_QUERY_DUT_EQ_INFO;
            info.maxEqBandNum = getMaxEqBand();
            getSampleArray(info.sample_rate, &info.sample_rate_num);
#ifdef __PC_CMD_UART__			
			eq_reply_payload_updata((uint8_t*)&info, 4 + info.sample_rate_num * 4);
#endif
            break;
        default:
            break;
    }

    return 0;
}

int audio_eq_init(void)
{
#ifdef __PC_CMD_UART__	
	communication_init();
#endif
    return 0;
}

#ifdef __PC_CMD_UART__
eq_keyword_table eq_table[EQ_KEY_WORD_NUM]=
{
	{"cmd",audio_cmd_callback},
	{"ping",audio_tool_ping_callback},
	{"iir_eq",audio_eq_sw_iir_callback},
	{"hw_iir_eq",audio_eq_hw_dac_iir_callback},
	{"drc_eq",audio_eq_drc_callback},
	{"drc_2_eq",audio_eq_drc_2_callback}
};
#endif
