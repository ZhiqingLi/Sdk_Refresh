#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
//#include <tgmath.h>  //add by zjw 131008


#ifdef CONFIG_APP_EQUANLIZER
#define AUD_EQU_NUM 5   //music 5 and aec 2
#define AUD_EQU_HF_START 5
#define AUD_EQU_HF_STOP 6


static aud_equ_t aud_equ[AUD_EQU_NUM + 2];
static int equ_enable_count = 0;

extern int a2dp_get_freq( void );

static int16 aud_equalizer_one_sample(int index, int lr_mode,  int16 sample )
{
    int out_sample = 0;
    int out_sample1 = 0;


    aud_equ[index].x[lr_mode][0] = aud_equ[index].x[lr_mode][1];
    aud_equ[index].x[lr_mode][1] = aud_equ[index].x[lr_mode][2];
    aud_equ[index].x[lr_mode][2] = sample/2;

    out_sample += (int)aud_equ[index].b[0] * aud_equ[index].x[lr_mode][2]; 
    out_sample += (int)aud_equ[index].b[1] * aud_equ[index].x[lr_mode][1];
    out_sample += (int)aud_equ[index].b[2] * aud_equ[index].x[lr_mode][0]; 
    out_sample -= (int)aud_equ[index].a[0] * aud_equ[index].y[lr_mode][1];
    out_sample -= (int)aud_equ[index].a[1] * aud_equ[index].y[lr_mode][0];

    out_sample = out_sample/4096;
    
    aud_equ[index].y[lr_mode][0] = aud_equ[index].y[lr_mode][1];
    if( out_sample > 32767 )
        aud_equ[index].y[lr_mode][1] = 32767;
    else if( out_sample < -32767 )
        aud_equ[index].y[lr_mode][1] = -32767;
    else
        aud_equ[index].y[lr_mode][1] = out_sample;

    out_sample1 = aud_equ[index].y[lr_mode][1]*2;
    if( out_sample1 > 32767 )
    	out_sample1 = 32767;
    else if( out_sample1 < -32767 )
    	out_sample1 = -32767;
    return out_sample1;
}

#if 0
int aud_apply_equalizer( int index, aud_equ_conf_t *conf )
{
    float pi = 3.1415926f;
    float K, V0;
    float t1, t2, t4, t5;   
    float Q;

	if( conf->flag_type )  //peak
	{
		Q = (float)conf->fc/conf->bandwidth;
		K = tanf(pi*conf->fc/conf->fs);
		t1 = K*K;

		if ( conf->gain > 0 )
		{
			V0 = powf( 10, (float)conf->gain/20 );
			t4 = 1/Q*K;  
			aud_equ[index].b[0] = (int)(4096/(1+t4+t1)*(1+V0*t4+t1));
			aud_equ[index].b[1] = (int)(4096/(1+t4+t1)*(2*(t1-1)));
			aud_equ[index].b[2] = (int)(4096/(1+t4+t1)*(1-V0*t4+t1));
			aud_equ[index].a[0] = (int)(4096/(1+t4+t1)*(2*(t1-1)));
			aud_equ[index].a[1] = (int)(4096/(1+t4+t1)*(1-t4+t1));   
		}            
		else
		{
			V0 = powf( 10, (float)(-conf->gain)/20 );
			t4 = 1/Q*K;
			aud_equ[index].b[0] = (int)(4096/(1+V0*t4+t1)*(1+t4+t1));
			aud_equ[index].b[1] = (int)(4096/(1+V0*t4+t1)*(2*(t1-1)));
			aud_equ[index].b[2] = (int)(4096/(1+V0*t4+t1)*(1-t4+t1));
			aud_equ[index].a[0] = (int)(4096/(1+V0*t4+t1)*(2*(t1-1)));
			aud_equ[index].a[1] = (int)(4096/(1+V0*t4+t1)*(1-V0*t4+t1));
		}           
	}
	else  //shelving
	{
		K = tanf(pi*conf->fc/conf->fs);
		t1 = K*K;
		t5 = sqrtf(2)*K; // sqrt(2)*K

		if( conf->flag_low_high == 0 ) // low
		{
			if( conf->gain > 0 )
			{
				V0 = powf( 10, (float)conf->gain/20 );
				t4 = sqrtf(2*V0)*K; // sqrt(2*V0)*K
				aud_equ[index].b[0] = (int)(4096/(1+t5+t1)*( 1 + t4+V0*t1));
				aud_equ[index].b[1] = (int)(4096/(1+t5+t1)*( 2 *(V0*t1-1)));
				aud_equ[index].b[2] = (int)(4096/(1+t5+t1)*( 1-t4+V0*t1));
				aud_equ[index].a[0] = (int)(4096/(1+t5+t1)*( 2*(t1-1)));
				aud_equ[index].a[1] = (int)(4096/(1+t5+t1)*( 1-t5+t1));
			}
			else
			{
				V0 = powf( 10, (float)(-conf->gain)/20 );
				t4 = sqrtf(2*V0)*K; // sqrt(2*V0)*K
				aud_equ[index].b[0] = (int)(4096/(1+t4+V0*t1)*(1+t5+t1));
				aud_equ[index].b[1] = (int)(4096/(1+t4+V0*t1)*(2*(t1-1)));
				aud_equ[index].b[2] = (int)(4096/(1+t4+V0*t1)*(1-t5+t1));
				aud_equ[index].a[0] = (int)(4096/(1+t4+V0*t1)*(2*(V0*t1-1)));
				aud_equ[index].a[1] = (int)(4096/(1+t4+V0*t1)*(1-t4+V0*t1));
			}
		}
		else
		{
			if( conf->gain > 0 )
			{
				V0 = powf( 10, (float)conf->gain/20 );
				t4 = sqrtf(2*V0)*K; // sqrt(2*V0)*K
				aud_equ[index].b[0] = (int)(4096/(1+t5+t1)*( V0+t4+t1));
				aud_equ[index].b[1] = (int)(4096/(1+t5+t1)*( 2*(t1-V0)));
				aud_equ[index].b[2] = (int)(4096/(1+t5+t1)*(V0-t4+t1));
				aud_equ[index].a[0] = (int)(4096/(1+t5+t1)*( 2*(t1-1)));
				aud_equ[index].a[1] = (int)(4096/(1+t5+t1)*( 1-t5+t1));
			}
			else
			{
				V0 = powf( 10, (float)(-conf->gain)/20 );
				t2 = sqrtf(2/V0)*K;
				t4 = sqrtf(2*V0)*K; // sqrt(2*V0)*K
				aud_equ[index].b[0] = (int)(4096/(V0+t4+t1)*(1+t5+t1));
				aud_equ[index].b[1] = (int)(4096/(V0+t4+t1)*(2*(t1-1)));
				aud_equ[index].b[2] = (int)(4096/(V0+t4+t1)*(1-t5+t1));
				aud_equ[index].a[0] = (int)(4096/(1+t2+t1/V0)*(2*(t1/V0-1)));
				aud_equ[index].a[1] = (int)(4096/(1+t2+t1/V0)*(1-t2+t1/V0));
			}
		}
	}
    return 0;
}
#endif

void aud_euqalizer_enable( int index, uint8 enable )
{
    aud_equ[index].flag_enable = enable;

    memset( (uint8 *)&aud_equ[index].x[0][0], 0, sizeof(aud_equ[index].x));
    memset( (uint8 *)&aud_equ[index].y[0][0], 0, sizeof(aud_equ[index].y));

    if( index < AUD_EQU_NUM )
    {
        equ_enable_count = aud_equ[0].flag_enable + aud_equ[1].flag_enable
            + aud_equ[2].flag_enable + aud_equ[3].flag_enable + aud_equ[4].flag_enable;  
    }

}

int app_equalizer_get_enable_count( void )
{
    return equ_enable_count;
}

void app_equalizer_recover_enable( void )
{
    //os_printf("---app_equalizer_recover_enable()\r\n");

    if(app_equalizer_get_enable_count())
    {
        if((app_equalizer_get_enable_count() < 5)||(app_wave_playing()))
        {
            BK3000_AUD_DAC_CONF0 |= ((42 << 18)|(1 << 16));
        }
        else if(app_equalizer_get_enable_count() >= 5)
        {
            BK3000_AUD_DAC_CONF0 &= ~( 1 << sft_AUD_DAC_HPF2_BYPASS );
            BK3000_AUD_DAC_CONF0 |= ( 0 << sft_AUD_DAC_HPF2_BYPASS );
        }
        BK3000_AUD_AUDIO_CONF2 |= ( 1 << 2 );//open eq
    }
}

void app_equalizer_init( void )
{
    int i;
    for( i = 0; i < AUD_EQU_NUM; i++ )
    {
        memset( (uint8 *)&aud_equ[i], 0, sizeof(aud_equ_t));
        aud_equ[i].b[0] = 0x4000<<6;
    }
}
void app_equalizer_a2dp_apply(void)
{
    int i;
    app_eq_para_t eq_para;
    BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 2 );
    BK3000_AUD_DAC_CONF0 |= ( 1 << sft_AUD_DAC_HPF2_BYPASS );
    for( i = 0; i < AUDIO_MAX_FILT_NUM; i++ )
    {
        memcpy( (uint8 *)eq_para.b, (uint8 *)aud_equ[i].b, 12);
        memcpy( (uint8 *)eq_para.a, (uint8 *)aud_equ[i].a, 8);
        aud_filt_conf(i, &eq_para);
    }
    if(equ_enable_count)
    {
    	aud_filt_enable(0, 1);
    	if(equ_enable_count>=5)
    		aud_filt_enable(4, 1);
    }
    return;
}
void app_equalizer_hfp_apply(void)
{
    int i;
    app_eq_para_t eq_para;
    memset( (uint8 *)&eq_para, 0, sizeof(app_eq_para_t));
    eq_para.b[0] = 0x4000<<6;
    for( i = 0; i < AUDIO_MAX_FILT_NUM; i++ )
    {
        aud_filt_conf( i, &eq_para );
    }
    return;
}

void app_equalizer_calc( int16 samplel, int16 sampler, int16 *outl, int16 *outr )
{
    int k;

    for( k = 0; k < AUD_EQU_NUM; k++ )
    {
        if( aud_equ[k].flag_enable == 1 )
        {
            *outl = aud_equalizer_one_sample(k, 0, samplel);
            *outr = aud_equalizer_one_sample(k, 1, sampler);
            samplel = *outl;
            sampler = *outr;
        }
    }
		
    return;
}


int16 app_equalizer_calc_hfp( int16 sample )
{
    int k;
    int16 sample_out = sample;

    for( k = AUD_EQU_HF_START; k <= AUD_EQU_HF_STOP; k++ )
    {
        if( aud_equ[k].flag_enable == 1 )
        {
            sample_out = aud_equalizer_one_sample(k, 0, sample_out);
        }
    }
		
	return sample_out;
}

void app_equalizer_a2dp_init( app_eq_para_t * app_a2dp_equ )
{
    int i;
    int count = 0;

    for( i = 0; i < AUD_EQU_NUM; i++ )
    {
        memcpy( aud_equ[i].a, app_a2dp_equ[i].a, sizeof(aud_equ[i].a)+sizeof(aud_equ[i].b));
        aud_euqalizer_enable( i, app_a2dp_equ[i].flag );
        if( app_a2dp_equ[i].flag == 1 )
            count++;
        else
            app_a2dp_equ[i].flag = 0;
    }   

    equ_enable_count = count;
}


void app_equalizer_show( void )
{
    int i;

    for( i = 0; i < 7; i++ )
    {
        os_printf("equ index: %d\r\n", i );
        os_printf("a[0]: %d, a[1]: %d, b[0]: %d, b[1]: %d, b[2]: %d\r\n",
            aud_equ[i].a[0], aud_equ[i].a[1], aud_equ[i].b[0], aud_equ[i].b[1],
            aud_equ[i].b[2] );
        os_printf("equ enable: %d\r\n", aud_equ[i].flag_enable );
    }
}

#endif
