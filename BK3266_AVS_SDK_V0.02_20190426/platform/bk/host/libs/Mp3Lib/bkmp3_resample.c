#include "bkmp3_resample.h"
#include "spr_defs.h"
#include "bautil.h"
#include "bk3000_reg.h"
#include "mp3dec.h"
#include "app_beken_includes.h"
#if (CONFIG_APP_MP3PLAYER == 1)
#if (CONFIG_NOUSE_RESAMPLE == 0)
#ifdef FIR_HARD_CORE   //hardcore fir filter;
CONST int16_t core_firCoef_2[CORE_NFIR] = {
0,
0,
0,
0,
-1,
0,
2,
0,
-3,
0,
4,
0,
-6,
0,
9,
0,
-12,
0,
17,
0,
-23,
0,
31,
0,
-41,
0,
53,
0,
-68,
0,
86,
0,
-107,
0,
133,
0,
-165,
0,
202,
0,
-246,
0,
299,
0,
-363,
0,
441,
0,
-539,
0,
663,
0,
-827,
0,
1058,
0,
-1410,
0,
2028,
0,
-3442,
0,
10419,
16384,
10419,
0,
-3442,
0,
2028,
0,
-1410,
0,
1058,
0,
-827,
0,
663,
0,
-539,
0,
441,
0,
-363,
0,
299,
0,
-246,
0,
202,
0,
-165,
0,
133,
0,
-107,
0,
86,
0,
-68,
0,
53,
0,
-41,
0,
31,
0,
-23,
0,
17,
0,
-12,
0,
9,
0,
-6,
0,
4,
0,
-3,
0,
2,
0,
-1,
0,
0,
0,
0
};

CONST int16_t core_firCoef_4[CORE_NFIR] = {
0,
0,
0,
0,
0,
0,
0,
0,
-1,
-1,
-1,
0,
2,
4,
3,
0,
-5,
-8,
-7,
0,
10,
17,
14,
0,
-20,
-33,
-27,
0,
36,
58,
47,
0,
-60,
-97,
-77,
0,
98,
155,
123,
0,
-154,
-242,
-191,
0,
236,
371,
292,
0,
-362,
-570,
-451,
0,
569,
911,
734,
0,
-986,
-1656,
-1426,
0,
2429,
5187,
7365,
8192,
7365,
5187,
2429,
0,
-1426,
-1656,
-986,
0,
734,
911,
569,
0,
-451,
-570,
-362,
0,
292,
371,
236,
0,
-191,
-242,
-154,
0,
123,
155,
98,
0,
-77,
-97,
-60,
0,
47,
58,
36,
0,
-27,
-33,
-20,
0,
14,
17,
10,
0,
-7,
-8,
-5,
0,
3,
4,
2,
0,
-1,
-1,
-1,
0,
0,
0,
0,
0,
0,
0,
0
};
int16_t fir_hardcore_config(uint16_t nRatio)
{
    int16_t i;
    int16_t *coef;
    if(nRatio == UPSMPL4_1)
        coef = (int16_t *)&core_firCoef_4[0];
    else
        coef = (int16_t *)&core_firCoef_2[0];
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_FFT_PWD;
    BK3000_FFT_FFT_CONF = 0x00;
    //BK3000_FFT_FIR_CONF = (CORE_NFIR) | (5L<<sft_FIR_CONF_FIR_DWIDTH) | (1L<<sft_FIR_CONF_FIR_MODE) | (1L<<sft_FIR_CONF_FIR_ENABLE);
    BK3000_FFT_FIR_CONF = (CORE_NFIR) 
            | (1L<<sft_FIR_CONF_FIR_MODE) | (1L<<sft_FIR_CONF_FIR_ENABLE);
            
    for(i=0;i<CORE_NFIR;i++)
    {
        BK3000_FFT_COEF_PORT = coef[i];
    }
    return 0;
}
int16_t fir_hardcore_free(void)
{
    BK3000_FFT_FIR_CONF = 0x00;
    BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD;
    return 0;
}
int16_t fir_hardcore_resample_mono(int16_t *smpl,uint16_t nSmpls,uint16_t nRatio)
{
	int i;
	int32_t tmp;
	switch(nRatio)
	{
		case DOWNSMPL1_4:break;				    /* 1/4 down sample  Not Used */
		case DOWNSMPL1_2:				        /* 1/2 down sample */
			
			for(i=0;i<nSmpls>>1;i++) //��ȡ
			{
                BK3000_FFT_DATA_PORT = smpl[2*i]; 
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[i] = tmp;//fir shift bits
			}
			return nSmpls>>1;
			
		case NORESMPL:break;			        /* not resample */
		case UPSMPL4_1:;				        /* 4* up sample */
		    for(i=nSmpls-1;i>=0;i--)   //��ֵ
			{
                smpl[4*i+3] = smpl[i];
				smpl[4*i+2] = smpl[i];
				smpl[4*i+1] = smpl[i];
				smpl[4*i] = smpl[i];
			}
			for(i=0;i<nSmpls<<2;i++)
			{
                BK3000_FFT_DATA_PORT = smpl[i];
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[i] = tmp;//fir shift bits
            }
			return nSmpls<<2;

    	case UPSMPL2_1:					        /* 2* up sample */
			for(i=nSmpls-1;i>=0;i--)	 //��ֵ
			{
				smpl[2*i+1] = smpl[i];
				smpl[2*i] = smpl[i];
			}
			for(i=0;i<nSmpls<<1;i++)
			{
                BK3000_FFT_DATA_PORT = smpl[i];
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[i] = tmp;//fir shift bits
            }
			return nSmpls<<1;
		default:break;
	}
	return nSmpls;
	
}
int16_t fir_hardcore_resample(int16_t *smpl,uint16_t nSmpls,uint16_t nRatio)
{
	int i;
	int32_t tmp;
	switch(nRatio)
	{
		case DOWNSMPL1_4:break;				    /* 1/4 down sample  Not Used */
		case DOWNSMPL1_2:				        /* 1/2 down sample */
			
			for(i=0;i<nSmpls>>2;i++)
			{
                BK3000_FFT_DATA_PORT = (smpl[4*i]+smpl[4*i+1])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[2*i] = tmp;//fir shift bits

                BK3000_FFT_DATA_PORT = (smpl[4*i+2]+smpl[4*i+3])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
    			smpl[2*i+1] = smpl[2*i];
			}
			return nSmpls>>1;
		case NORESMPL:break;			        /* not resample */
		case UPSMPL4_1:;				        /* 4* up sample */
		    if(nSmpls > 576)
		    {
		        return 0;
		    }
            for(i=(nSmpls>>1)-1;i>=0;i--)
			{
                smpl[8*i+7] = smpl[2*i+1];
				smpl[8*i+6] = smpl[2*i];
				smpl[8*i+5] = smpl[2*i+1];
				smpl[8*i+4] = smpl[2*i];
				smpl[8*i+3] = smpl[2*i+1];
				smpl[8*i+2] = smpl[2*i];
				smpl[8*i+1] = smpl[2*i+1];
				smpl[8*i] = smpl[2*i];
			}
			for(i=0;i<nSmpls<<1;i++)
			{
                BK3000_FFT_DATA_PORT = (smpl[2*i]+smpl[2*i+1])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[2*i] = tmp;//fir shift bits
                smpl[2*i+1] = smpl[2*i];
            }
			return nSmpls<<2;

    	case UPSMPL2_1:					        /* 2* up sample */
			for(i=(nSmpls>>1)-1;i>=0;i--)
			{
				smpl[4*i+3] = smpl[2*i+1];
				smpl[4*i+1] = smpl[2*i+1];
				smpl[4*i+2] = smpl[2*i];
				smpl[4*i] = smpl[2*i];
			}
			for(i=0;i<nSmpls;i++)
			{
                BK3000_FFT_DATA_PORT = (smpl[2*i]+smpl[2*i+1])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[2*i] = tmp;//fir shift bits
                smpl[2*i+1] = smpl[2*i];
            }
			return nSmpls<<1;
		default:break;
	}
	return nSmpls;
	
}
#else  // hardcore + soft fir filter;

#ifdef FIR_HARD_SOFT
#define CORE_NFIR   127
CONST int16_t core_firCoef[CORE_NFIR] = {
0,
0,
0,
0,
-1,
0,
2,
0,
-3,
0,
4,
0,
-6,
0,
9,
0,
-12,
0,
17,
0,
-23,
0,
31,
0,
-41,
0,
53,
0,
-68,
0,
86,
0,
-107,
0,
133,
0,
-165,
0,
202,
0,
-246,
0,
299,
0,
-363,
0,
441,
0,
-539,
0,
663,
0,
-827,
0,
1058,
0,
-1410,
0,
2028,
0,
-3442,
0,
10419,
16384,
10419,
0,
-3442,
0,
2028,
0,
-1410,
0,
1058,
0,
-827,
0,
663,
0,
-539,
0,
441,
0,
-363,
0,
299,
0,
-246,
0,
202,
0,
-165,
0,
133,
0,
-107,
0,
86,
0,
-68,
0,
53,
0,
-41,
0,
31,
0,
-23,
0,
17,
0,
-12,
0,
9,
0,
-6,
0,
4,
0,
-3,
0,
2,
0,
-1,
0,
0,
0,
0
};
#endif
CONST int16_t firCoef[NFIR] = {
-523,
0,
427,
0,
-612,
0,
870,
0,
-1257,
0,
1915,
0,
-3371,
0,
10395,
16384,
10395,
0,
-3371,
0,
1915,
0,
-1257,
0,
870,
0,
-612,
0,
427,
0,
-523
};
#ifdef FIR_HARD_SOFT
static int16_t fir_hard_core_config(int16_t *coef,int16_t nfir)
{
    int16_t i;
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_FFT_PWD;
    BK3000_FFT_FFT_CONF = 0x00;
    BK3000_FFT_FIR_CONF = (nfir) | (5L<<11) | (1L<<14) | (1L<<16);
    for(i=0;i<nfir;i++)
    {
        BK3000_FFT_COEF_PORT = coef[i];
    }
    return 0;
}
#endif
static int16_t fir_filter_sample_op(pFIR_STATE pfir_stat,int16_t smpl)
{
	int16_t i;
    int32_t y;
    int16_t offset1;
    int16_t offset2;
    pfir_stat->laststate[pfir_stat->curr_pos] = smpl;
    offset2 = pfir_stat->curr_pos;
    offset1 = pfir_stat->taps - offset2;
    if(offset1 <=  (pfir_stat->taps>>1))
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) - offset1];
    else
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) + offset2];
    
    for (i = pfir_stat->taps - 1;  i >= offset1;  i-=2)
        if(pfir_stat->laststate[i - offset1]!=0)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i - offset1];
		
    for (  ;  i >= 0;  i-=2)
        if(pfir_stat->laststate[i + offset2]!=0)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i + offset2];
		
    if (pfir_stat->curr_pos <= 0)
    	pfir_stat->curr_pos = pfir_stat->taps;
    pfir_stat->curr_pos--;

    if(pfir_stat->ratio)
		y = (y + (1L<<13)) >> 14;
	else
		y = (y + (1L<<14)) >> 15;
	if(y > 32767)  
	    y = 32767;
	else if(y < -32768) 
	    y = -32768;
    return  y;
}
static int16_t fir_filter_down_sample_op(pFIR_STATE pfir_stat,int16_t smpl)
{
	int16_t i;
    int32_t y;
    int16_t offset1;
    int16_t offset2;
    pfir_stat->laststate[pfir_stat->curr_pos] = smpl;
    offset2 = pfir_stat->curr_pos;
    offset1 = pfir_stat->taps - offset2;
    if(offset1 <=  (pfir_stat->taps>>1))
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) - offset1];
    else
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) + offset2];
    
    for (i = pfir_stat->taps - 1;  i >= offset1;  i-=2)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i - offset1];
		
    for (  ;  i >= 0;  i-=2)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i + offset2];
		
    if (pfir_stat->curr_pos <= 0)
    	pfir_stat->curr_pos = pfir_stat->taps;
    pfir_stat->curr_pos--;

   	y = (y + (1L<<14)) >> 15;
	if(y > 32767)  
	    y = 32767;
	else if(y < -32768) 
	    y = -32768;
    return  y;
}
static int16_t fir_filter_up_sample_op(pFIR_STATE pfir_stat,int16_t smpl)
{
	int16_t i;
    int32_t y;
    int16_t offset1;
    int16_t offset2;
    pfir_stat->laststate[pfir_stat->curr_pos] = smpl;
    offset2 = pfir_stat->curr_pos;
    offset1 = pfir_stat->taps - offset2;
    if(offset1 <=  (pfir_stat->taps>>1))
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) - offset1];
    else
        y = pfir_stat->coef[pfir_stat->taps>>1]*pfir_stat->laststate[(pfir_stat->taps>>1) + offset2];
    
    for (i = pfir_stat->taps - 1;  i >= offset1;  i-=2)
        if(pfir_stat->laststate[i - offset1]!=0)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i - offset1];
		
    for (  ;  i >= 0;  i-=2)
        if(pfir_stat->laststate[i + offset2]!=0)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i + offset2];
		
    if (pfir_stat->curr_pos <= 0)
    	pfir_stat->curr_pos = pfir_stat->taps;
    pfir_stat->curr_pos--;

	y = (y + (1L<<13)) >> 14;
	if(y > 32767)  
	    y = 32767;
	else if(y < -32768) 
	    y = -32768;
    return  y;
}
static int16_t fir_filter_sample(pFIR_STATE pfir_stat,int16_t smpl)
{
	int16_t i;
    int32_t y;
    int16_t offset1;
    int16_t offset2;
    pfir_stat->laststate[pfir_stat->curr_pos] = smpl;
    offset2 = pfir_stat->curr_pos;
    offset1 = pfir_stat->taps - offset2;
    y = 0;
    for (i = pfir_stat->taps - 1;  i >= offset1;  i--)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i - offset1];
		
    for (  ;  i >= 0;  i--)
        y += pfir_stat->coef[i]*pfir_stat->laststate[i + offset2];
		
    if (pfir_stat->curr_pos <= 0)
    	pfir_stat->curr_pos = pfir_stat->taps;
    pfir_stat->curr_pos--;

	if(pfir_stat->ratio)
		y = (y + (1L<<13)) >> 14;
	else
		y = (y + (1L<<14)) >> 15;
	if(y > 32767)  
	    y = 32767;
	else if(y < -32768) 
	    y = -32768;
    return  y;
}

int16_t resample_block(pFIR_STATE L_fir,pFIR_STATE R_fir,int16_t *smpl,uint16_t nSmpls,uint16_t nRatio)
{
	int i;
	int32_t tmp;
	
	switch(nRatio)
	{
		case DOWNSMPL1_4:break;				/* 1/4 down sample  Not Used */
		case DOWNSMPL1_2:				    /* 1/2 down sample */
			
			for(i=0;i<nSmpls>>2;i++)
			{
#ifdef FIR_HARD_SOFT
                BK3000_FFT_DATA_PORT = (smpl[4*i]+smpl[4*i+1])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;
                smpl[2*i] = tmp<<1;

                BK3000_FFT_DATA_PORT = (smpl[4*i+2]+smpl[4*i+3])>>1;
                BK3000_FFT_FFT_START = 0x01;
                while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                tmp = BK3000_FFT_DATA_PORT;

                
#else
				smpl[2*i] = fir_filter_down_sample_op(L_fir,(smpl[4*i]+smpl[4*i+1])>>1);
				fir_filter_down_sample_op(L_fir,(smpl[4*i+2]+smpl[4*i+3])>>1);
#endif
				//smpl[2*i] = smpl[4*i];
				//smpl[2*i+1] = smpl[4*i+1];
				smpl[2*i+1] = smpl[2*i];
				
				//fir_filter_sample(R_fir,smpl[4*i+3]);
			}
			return nSmpls>>1;
			
		case NORESMPL:break;			        /* not resample */

		case UPSMPL4_1:;				        /* 4* up sample */
			break;
		case UPSMPL2_1:					        /* 2* up sample */
			for(i=(nSmpls>>1)-1;i>=0;i--)
			{
				smpl[4*i+3] = 0;//smpl[2*i+1];
				smpl[4*i+1] = smpl[2*i+1];
				smpl[4*i+2] = 0;//smpl[2*i];
				smpl[4*i] = smpl[2*i];
			}
#ifdef FIR_HARD_SOFT	
            if(L_fir->ratio == 2)
            {
			    for(i=0;i<nSmpls;i++)
			    {

                    BK3000_FFT_DATA_PORT = (smpl[2*i]+smpl[2*i+1])>>1;
                    BK3000_FFT_FFT_START = 0x01;
                    while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
                    tmp = BK3000_FFT_DATA_PORT;
                    smpl[2*i] = tmp<<2;
                    smpl[2*i+1] = smpl[2*i];
                }
            }
            else
            {  
                for(i=0;i<nSmpls;i++)
			    {                
                    smpl[2*i] = fir_filter_up_sample_op(L_fir,(smpl[2*i]+smpl[2*i+1])>>1); 
                    smpl[2*i+1] = smpl[2*i];
                }
            }
            
#else
            for(i=0;i<nSmpls;i++)
		    {
			    smpl[2*i] = fir_filter_up_sample_op(L_fir,(smpl[2*i]+smpl[2*i+1])>>1);
			    smpl[2*i+1] = smpl[2*i];
			}
#endif
			return nSmpls<<1;
		default:break;
	}
	return nSmpls;
}
/*
 * NAME:	resample_init()
 * DESCRIPTION:	initialize resampling state
 */
int16_t resample_init(pFIR_STATE pfir_stat)
{
    unsigned int i;
#ifdef FIR_HARD_SOFT
	fir_hard_core_config(&core_firCoef[0],CORE_NFIR);
#endif
	pfir_stat->coef = &firCoef[0];
	pfir_stat->taps = NFIR;
	pfir_stat->curr_pos = NFIR - 1;
	pfir_stat->ratio = 0;
	for(i=0;i<NFIR;i++)
	{
		pfir_stat->laststate[i] = 0;
	}
	return 0;

}
#endif /* CONFIG_APP_MP3PLAYER */
#endif

#endif
