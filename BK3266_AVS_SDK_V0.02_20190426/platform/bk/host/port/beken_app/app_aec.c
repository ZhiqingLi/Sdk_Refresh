#include "app_beken_includes.h"
#include "driver_beken_includes.h"

#ifdef CONFIG_APP_AEC
// constant definition
CONST static uint16 hwin[FRAME_LEN] = {
	 10, 39, 88,157,245,352,479,625,
	790,974,1178,1400,1641,1901,2179,2475,
	2790,3122,3472,3840,4225,4627,5045,5481,
	5932,6400,6884,7382,7897,8426,8969,9527,
	10098,10684,11282,11893,12517,13153,13800,14459,
	15129,15809,16500,17200,17909,18628,19355,20090,
	20832,21582,22338,23100,23869,24642,25421,26203,
	26990,27780,28574,29369,30167,30966,31767,32568,
	33369,34170,34969,35768,36565,37359,38151,38940,
	39724,40505,41281,42052,42818,43577,44330,45076,
	45815,46546,47268,47982,48687,49383,50068,50743,
	51408,52061,52703,53332,53950,54555,55147,55725,
	56290,56840,57377,57898,58405,58896,59372,59831,
	60275,60702,61112,61506,61882,62241,62582,62906,
	63211,63499,63767,64018,64249,64462,64656,64831,
	64987,65123,65240,65338,65416,65475,65514,65534,
	65534,65514,65475,65416,65338,65240,65123,64987,
	64831,64656,64462,64249,64018,63767,63499,63211,
	62906,62582,62241,61882,61506,61112,60702,60275,
	59831,59372,58896,58405,57898,57377,56840,56290,
	55725,55147,54555,53950,53332,52703,52061,51408,
	50743,50068,49383,48687,47982,47268,46546,45815,
	45076,44330,43577,42818,42052,41281,40505,39724,
	38940,38151,37359,36565,35768,34969,34170,33369,
	32568,31767,30966,30167,29369,28574,27780,26990,
	26203,25421,24642,23869,23100,22338,21582,20832,
	20090,19355,18628,17909,17200,16500,15809,15129,
	14459,13800,13153,12517,11893,11282,10684,10098,
	9527,8969,8426,7897,7382,6884,6400,5932,
	5481,5045,4627,4225,3840,3472,3122,2790,
	2475,2179,1901,1641,1400,1178,974,790,
	625,479,352,245,157, 88, 39, 10};

#ifdef AEC_FFT_SW
CONST static int16 Sinewave[65]= {
	0,804,1608,2410,3212,4011,4808,5602,6393,7179,7962,8739,9512,10278,
	11039,11793,12539,13279,14010,14732,15446,16151,16846,17530,18204,
	18868,19519,20159,20787,21403,22005,22594,23170,23731,24279,24811,
	25329,25832,26319,26790,27245,27683,28105,28510,28898,29268,29621,
	29956,30273,30571,30852,31113,31356,31580,31785,31971,32137,32285,
	32412,32521,32609,32678,32728,32757,32767};
#endif
#if(CONFIG_APP_MSBC_RESAMPLE == 1)
#define ECHO_RX_BUFFER_SIZE           (360)
#define ECHO_TX_BUFFER_SIZE           (256)
#else
#define ECHO_RX_BUFFER_SIZE           (360*2)
#define ECHO_TX_BUFFER_SIZE           (256*2)
#endif
static char *echo_rx_buff;
static char *echo_tx_buff;
static aec_t *s_aec_ptr = 0;
//static app_aec_agc_t app_aec_agc;

#ifdef AEC_FLASH_TEST
static int app_aec_data_print( char *buffer , int length);
#endif

#ifdef AEC_STATIS_TEST
static int rx_input_bytes = 0;
static int rx_buffer_bytes = 0;
static int tx_buffer_bytes = 0;
static int tx_input_bytes = 0;
static int aec_bytes = 0;
#endif

 #ifdef FRAME_MALLOC
int16 *tres_newi, *tres_newq;
int16 *tres_with_hwin;
int16 *techo_with_hwin;
int16 *imagbuf;
//int16 *seed16;
#endif

static __inline int16 f_sat(int din)
{
    if (din>32767)
    	return 32767;
    if (din<-32767)
    	return -32767;
      else
        return(din);
}
static RAM_CODE uint16 f_cabs(int16 din1, int16 din2)
{
    int16 dmax, dmin, abs_din1, abs_din2;

    abs_din1 = ABS(din1);
    abs_din2 = ABS(din2);

    if (abs_din1>abs_din2)
    {
        dmax = abs_din1;
        dmin = abs_din2;
    }
    else
    {
        dmax = abs_din2;
        dmin = abs_din1;
    }

    return ((dmax>>1) + (dmin>>2));

}

static RAM_CODE void nlp(void)
{
    int16 k;
	uint32 techo_fft_mag;
    uint16 tres_mag;
    int tmp1, abstmp1, absshift;
    uint16 tres_mag_sub_echo;
    uint16 Gw;
    uint16 postsnr;
    int16 tmp2;

#ifndef FRAME_MALLOC
    int16 tres_newi[FRAME_LEN], tres_newq[FRAME_LEN];
    int16 tres_with_hwin[FRAME_LEN];
    int16 techo_with_hwin[FRAME_LEN];
    int16 imagbuf[FRAME_LEN];
    int16 seed16[FRAME_LEN];
#endif

    {
       for(k=(s_aec_ptr->p_state)->nlp_index;k<=FRAME_LEN-1;k++)
            techo_with_hwin[k-(s_aec_ptr->p_state)->nlp_index] = ((s_aec_ptr->p_state)->ear[k]
                *hwin[k-(s_aec_ptr->p_state)->nlp_index]>>16);
        for(k=0;k<=(s_aec_ptr->p_state)->nlp_index-1;k++)
            techo_with_hwin[k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index] = ((s_aec_ptr->p_state)->ear[k]
                *hwin[k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index]>>16);
    }

    os_memset( imagbuf, 0, FRAME_LEN * sizeof(uint16));

#ifdef AEC_FFT_SW
	my_fft(techo_with_hwin, imagbuf, 8, 0);
#else
	fft_enable( FFT_MODE_IFFT, techo_with_hwin, imagbuf, 256 );
    while( fft_busy() );
#endif
{
	int m;

    for(k=(s_aec_ptr->p_state)->nlp_index;k<=FRAME_LEN-1;k++)
    {
        int index = k-(s_aec_ptr->p_state)->nlp_index;
    	techo_fft_mag = f_cabs(techo_with_hwin[index], imagbuf[index]);


			for(m=0;m<s_aec_ptr->decay_times;m++)
				techo_fft_mag += (s_aec_ptr->p_state)->echo_fft_mem[index]>>(m+1);
			(s_aec_ptr->p_state)->echo_fft_mem[index] = f_sat(techo_fft_mag);
    	tres_with_hwin[index] =
            ((s_aec_ptr->p_state)->nlms_resi_echo[k]*hwin[index]>>16);
    }

    for(k=0;k<=(s_aec_ptr->p_state)->nlp_index-1;k++)
    {
        int index = k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index;
    	techo_fft_mag = f_cabs(techo_with_hwin[index], imagbuf[index]);
			for(m=0;m<s_aec_ptr->decay_times;m++)
				techo_fft_mag += (s_aec_ptr->p_state)->echo_fft_mem[index]>>(m+1);
			(s_aec_ptr->p_state)->echo_fft_mem[index] = f_sat(techo_fft_mag);
    	tres_with_hwin[index] =
            ((s_aec_ptr->p_state)->nlms_resi_echo[k]*hwin[index]>>16);
    }
}
    os_memset( imagbuf, 0, FRAME_LEN * sizeof(uint16));

#ifdef AEC_FFT_SW
	my_fft(tres_with_hwin, imagbuf, 8, 0);
#else
    fft_enable( FFT_MODE_IFFT, tres_with_hwin, imagbuf, 256 );
    while( fft_busy() );
#endif

	for (k=0;k<=FRAME_LEN-1;k++)
	{
		// nlp
		tres_mag = f_cabs(tres_with_hwin[k], imagbuf[k]);
		if(s_aec_ptr->fft_shift>=0)
		{
			if (tres_mag < ((((s_aec_ptr->p_state)->echo_fft_mem[k]+0)<<s_aec_ptr->fft_shift))) // s_aec_ptr->fft_shift //echo_fft_mag, additional 1 to remove quantizaiton error
				tres_mag_sub_echo = 0;
			else
				tres_mag_sub_echo = tres_mag - (((s_aec_ptr->p_state)->echo_fft_mem[k]<<s_aec_ptr->fft_shift));//s_aec_ptr->fft_shift
		}
		else
		{
			if (tres_mag < ((((s_aec_ptr->p_state)->echo_fft_mem[k]+0)>>(-s_aec_ptr->fft_shift)))) // s_aec_ptr->fft_shift //echo_fft_mag, additional 1 to remove quantizaiton error
				tres_mag_sub_echo = 0;
			else
				tres_mag_sub_echo = tres_mag - (((s_aec_ptr->p_state)->echo_fft_mem[k]>>(-s_aec_ptr->fft_shift)));//s_aec_ptr->fft_shift
		}

		postsnr = (tres_mag_sub_echo << 8)/((s_aec_ptr->p_state)->echo_fft_mem[k]+1); // echo_fft_mag
		Gw = (postsnr<<(s_aec_ptr->mic_gain+8)) /((1<<8)+postsnr); // now Gw 1 = 2^14
		if(s_aec_ptr->fft_shift<=0)
		{
		//earphone
			if (tres_mag<=2)
				Gw = (1<<7);
		}
		else
		{
		//soundbox
			if (tres_mag<=2)
			{
				Gw = (1<<3);
			}
			else if(tres_mag==3)
				Gw = (1<<2);
			else if(tres_mag==4)
				Gw = (1<<1);
		}
#if 0
		if((k<5)||(251<=k))
			Gw = 0;
		else if((5<=k && k<7) ||(249<=k && k< 251))
			Gw = (Gw*1280)>>10;
		else if((7<=k && k<20) ||(236<=k && k< 249))
			Gw = (Gw*1430)>>10;
		else if((20<=k && k<28)||(228<=k && k< 236))
			Gw = (Gw*1515)>>10;//1792
		else if((28<=k && k<32) ||(224<=k && k< 228))
			Gw = (Gw*1611)>>10;//1280
		else if((32<=k&&k<64)||(192<=k&&k<224))
			Gw = (Gw*512)>>10;
		else if(64<=k && k<192)
			Gw = 0;
#endif
		(s_aec_ptr->p_state)->Gw_mem[k] = f_sat((s_aec_ptr->p_state)->Gw_mem[k] - ((s_aec_ptr->p_state)->Gw_mem[k]>>1) + (Gw)); // scale *2
//		(s_aec_ptr->p_state)->Gw_mem[k] = f_sat((Gw*2)); // scale *2

		tmp1 = (tres_with_hwin[k] * (s_aec_ptr->p_state)->Gw_mem[k]);
		if (tmp1>0)
			abstmp1 = tmp1;
		else
			abstmp1 = 0-tmp1;
		absshift = abstmp1>>8; // Gw scale 2^8, and Gw_mem scale *2
		tmp2 = (absshift>>1) + (absshift&1);
//		tmp2 = (abs(tmp1)+(1<<8))>>9;
		if (tmp1>0)
			tres_newi[k] = tmp2;
		else
			tres_newi[k] = 0 - tmp2;

		tmp1 = (imagbuf[k] * (s_aec_ptr->p_state)->Gw_mem[k]);
		if (tmp1>0)
			abstmp1 = tmp1;
		else
			abstmp1 = 0-tmp1;
		absshift = abstmp1>>8;
		tmp2 = (absshift>>1) + (absshift&1);
//		tmp2 = (abs(tmp1)+(1<<8))>>9;
		if (tmp1>0)
			tres_newq[k] = tmp2;
		else
			tres_newq[k] = 0 - tmp2;
	}

#ifdef AEC_FFT_SW
	my_fft(tres_newi, tres_newq, 8, 1); // ifft
#else
    fft_enable( FFT_MODE_FFT, tres_newi, tres_newq, 256 );
    while( fft_busy() );
#endif
	// cni

	for(k=(s_aec_ptr->p_state)->nlp_index;k<=FRAME_LEN-1;k++)
	{

		(s_aec_ptr->p_state)->nlp_out[k] = f_sat((s_aec_ptr->p_state)->nlp_out[k]
				    + ((s_aec_ptr->p_state)->ifft_mem[k-(s_aec_ptr->p_state)->nlp_index]));
	    (s_aec_ptr->p_state)->ifft_mem[k-(s_aec_ptr->p_state)->nlp_index] = -tres_newi[k-(s_aec_ptr->p_state)->nlp_index];
	 }
	 for(k=0;k<=(s_aec_ptr->p_state)->nlp_index-1;k++)
	 {
	    (s_aec_ptr->p_state)->nlp_out[k] = f_sat((s_aec_ptr->p_state)->nlp_out[k]
				  + ((s_aec_ptr->p_state)->ifft_mem[k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index]));

	    (s_aec_ptr->p_state)->ifft_mem[k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index] = -tres_newi[k+FRAME_LEN-(s_aec_ptr->p_state)->nlp_index];
	 }


}


//void app_aec_init( uint8 mic_delay, int16 ear_gain, int16 mic_gain )
void app_aec_init( uint16 mic_delay, int8 fft_shift,
	uint8 decay_times,int16 ear_gain, int16 mic_gain )
{
    nlp_state *p_st;
#ifdef CONFIG_APP_EQUANLIZER
    //aud_equ_conf_t conf, conf1;
    app_env_handle_t env_h = app_env_get_handle();
#endif
    uint32_t interrupts_info;
    //AEC_PRT("app_aec_init\r\n");
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
    SYSirq_Disable_Interrupts_Except(&interrupts_info,(1<<VIC_CEVA_ISR_INDEX));
    //VICMR_disable_interrupts(&interrupts_info, &mask);
    if(NULL == s_aec_ptr)
        s_aec_ptr = (aec_t *)jmalloc(sizeof(aec_t), M_ZERO);

    p_st = s_aec_ptr->p_state;

    #ifdef FRAME_MALLOC
    if(!(tres_newi && tres_newq && tres_newq && tres_newq && tres_newq && tres_newq))
    {
        tres_newi = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);
        tres_newq = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);
        techo_with_hwin = tres_with_hwin = tres_newi;
        imagbuf = tres_newq;
//	    tres_with_hwin = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);
//	    techo_with_hwin = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);
//	    imagbuf = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);
//	    seed16 = (int16 *)jmalloc(FRAME_LEN * sizeof(uint16), M_ZERO);

	    if(!(tres_newi && tres_newq && tres_newq && tres_newq && tres_newq && tres_newq))
	    {
            //os_printf("malloc_failed_at_nlp_function\r\n");
			return;
	    }
    }
    #endif

    if (p_st == NULL)
        s_aec_ptr->p_state = (nlp_state  *)jmalloc(sizeof(nlp_state), M_ZERO);
    else
    {
        os_memset(p_st, 0, sizeof(nlp_state));
        s_aec_ptr->p_state = p_st;
    }

    s_aec_ptr->rin_buffer_wr_ind = mic_delay;
    s_aec_ptr->rin_buffer_rd_ind = 0;
    s_aec_ptr->ear_gain = ear_gain;
    s_aec_ptr->mic_gain = mic_gain;
	s_aec_ptr->decay_times = decay_times;
	s_aec_ptr->fft_shift = fft_shift;
	(s_aec_ptr->mic_analyse).flag = 0;

    if(0 == echo_rx_buff)
        echo_rx_buff = (char *)jmalloc(ECHO_RX_BUFFER_SIZE, M_ZERO);

    if(0 == echo_tx_buff)
        echo_tx_buff = (char *)jmalloc(ECHO_TX_BUFFER_SIZE, M_ZERO);

    if((0 == echo_rx_buff) || (0 == echo_tx_buff)
            || (((uint32)echo_rx_buff) & 0x01)
            || (((uint32)echo_tx_buff) & 0x01)
            )
    {
        //os_printf("ptr_exception_echo_rx-tx_buff\r\n");
        return;
    }

    rb_init( &s_aec_ptr->rx_samples, (uint8 *)echo_rx_buff, 0,ECHO_RX_BUFFER_SIZE);
    rb_init( &s_aec_ptr->tx_samples, (uint8 *)echo_tx_buff, 0,ECHO_TX_BUFFER_SIZE);

#ifdef AEC_STATIS_TEST
    rx_input_bytes = 0;
    rx_buffer_bytes = 0;
    tx_buffer_bytes = 0;
    tx_input_bytes = 0;
    aec_bytes = 0;
#endif

#ifdef CONFIG_APP_EQUANLIZER
//    if( env_h->env_cfg.used )
//    {
        // equalizer init
       //aud_euqalizer_enable( 5, env_h->env_cfg.env_aec_cfg.hfp_eq_para[0] );
//        aud_apply_equalizer( 5, &conf );

        //aud_euqalizer_enable( 6, env_h->env_cfg.env_aec_cfg.hfp_eq_para[1] );
//        aud_apply_equalizer( 6, &conf1 );
//    }
//    else
    {
        // equalizer init
        if(env_h->env_cfg.env_aec_cfg.hfp_eq_para[0].flag)
        {
            aud_euqalizer_enable( 5, 1 );
            //conf.flag_type = 1;
            //conf.flag_low_high = 0;
            //conf.gain = 3;
            //conf.fc = 200;
            //conf.fs = 8000;
            //conf.bandwidth = 600;
            //aud_apply_equalizer( 5, &conf );
        }
        if(env_h->env_cfg.env_aec_cfg.hfp_eq_para[1].flag)
        {
            aud_euqalizer_enable( 6, 1 );
            //conf1.flag_type = 1;
            //conf1.flag_low_high = 1;
            //conf1.gain = 3;
            //conf1.fc = 1000; //2000; //1000;
            //conf1.fs = 8000;
            //conf1.bandwidth = 1200;
            //aud_apply_equalizer(6, &conf1 );
        }
    }
#endif

//    app_aec_agc_init( &app_aec_agc );

#ifdef CONFIG_APP_DATA_CAPTURE
    s_aec_ptr->sync_send_flag = 0;
#endif
#ifdef ADJUST_AEC_DELAY
    s_aec_ptr->sync_adjusted_flag = 0;
#endif

    //VICMR_restore_interrupts(interrupts_info, mask);
    SYSirq_Enable_All_Interrupts(interrupts_info);
#ifdef JMALLOC_STATISTICAL
    os_printf("JMALLOC_STATISTICAL: CALL\r\n");
    memory_usage_show();
#endif
}
#ifdef ADJUST_AEC_DELAY
void aec_set_sync_adjused_flag(int a)
{
    if(NULL == s_aec_ptr)
        return;
 	s_aec_ptr->sync_adjusted_flag = a;
}
#endif

RAM_CODE void aec_set_latency(uint16 mic_delay)
{
    if(NULL != s_aec_ptr)
    {
        if(mic_delay >=(Mic_Delay_Size-2))
            mic_delay = (Mic_Delay_Size-2);
        mic_delay += s_aec_ptr->rin_buffer_rd_ind;
        if(mic_delay >= Mic_Delay_Size)
            mic_delay -= Mic_Delay_Size;

        s_aec_ptr->rin_buffer_wr_ind = mic_delay;
        //AEC_PRT("aec_rx_0:\r\n");
    }
}

RAM_CODE uint16 aec_get_latency(void)
{
    uint16 wr_ind=0xffff,rd_ind;
    if(NULL != s_aec_ptr)
    {
        wr_ind = s_aec_ptr->rin_buffer_wr_ind;
        rd_ind = s_aec_ptr->rin_buffer_rd_ind;
        if(wr_ind<rd_ind)
            wr_ind += Mic_Delay_Size;
        wr_ind = wr_ind - rd_ind;
    }
    return wr_ind;
}

void app_aec_uninit(void)
{
    uint32_t interrupts_info;

    //AEC_PRT("app_aec_uninit\r\n");

#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
    SYSirq_Disable_Interrupts_Except(&interrupts_info,(1<<VIC_CEVA_ISR_INDEX));
    //VICMR_disable_interrupts(&interrupts_info, &mask);
    if(s_aec_ptr != NULL)
    {
        if(s_aec_ptr->p_state)
        {
            //os_printf("free s_aec_ptr->p_state:%p\r\n", s_aec_ptr->p_state);
            jfree(s_aec_ptr->p_state);
            s_aec_ptr->p_state = NULL;
        }

        //os_printf("free s_aec_ptr:%p\r\n", s_aec_ptr);
        jfree(s_aec_ptr);
        s_aec_ptr = NULL;
    }

    if(echo_rx_buff)
    {
        jfree(echo_rx_buff);
        echo_rx_buff = 0;
    }

    if(echo_tx_buff)
    {
        jfree(echo_tx_buff);
        echo_tx_buff = 0;
    }

    #ifdef FRAME_MALLOC
    if(tres_newi)
    {
	    jfree(tres_newi);
	    tres_newi = 0;
    }

    if(tres_newq)
    {
	    jfree(tres_newq);
	    tres_newq = 0;
    }

    if(tres_with_hwin)
    {
	    //jfree(tres_with_hwin);
	    tres_with_hwin = 0;
    }

    if(techo_with_hwin)
    {
	    //jfree(techo_with_hwin);
	    techo_with_hwin = 0;
    }

    if(imagbuf)
    {
	    //jfree(imagbuf);
	    imagbuf = 0;
    }

    #endif
    SYSirq_Enable_All_Interrupts(interrupts_info);
    //VICMR_restore_interrupts(interrupts_info, mask);
}
#if 0
iir_first_order_state de_lpf_state;

static void de_lpf(int16 *dout, int16 din,
iir_first_order_state *state)
{
    int mac = 0;
    mac = (din + state->oldx)<<4;
    mac = f_mshift(mac, 1);
    mac = mac + (state->oldy<<4);
    mac = mac - state->oldy;
    mac = f_sat(f_mshift(mac, 4));
    *dout = mac;

    state->oldx = din;
    state->oldy = mac;
}
#endif

static RAM_CODE void app_aec_do_aec( int16 rin, int16 sin )
{
//    int16 secho, sout;
    if (!s_aec_ptr)
        return;
	mic_analyse_t *mic_analyse;
	mic_analyse = &(s_aec_ptr->mic_analyse);

	if(mic_analyse->flag)
	{
		if(ABS(sin)>30000)
			os_printf("mic_full:%d\r\n",sin);
		mic_analyse->mic_max = MAX(ABS(sin),mic_analyse->mic_max);
	}
    //AEC_PRT("app_aec_do_aec\r\n");
//	de_lpf(&(s_aec_ptr->rin_buffer[s_aec_ptr->rin_buffer_wr_ind]), rin, &de_lpf_state);
    s_aec_ptr->rin_buffer[s_aec_ptr->rin_buffer_wr_ind] = rin;
    s_aec_ptr->rin_buffer_wr_ind += 1;
	if(s_aec_ptr->rin_buffer_wr_ind>=Mic_Delay_Size)
		s_aec_ptr->rin_buffer_wr_ind = 0;
	rin = s_aec_ptr->rin_buffer[s_aec_ptr->rin_buffer_rd_ind];
    s_aec_ptr->rin_buffer_rd_ind += 1;
	if(s_aec_ptr->rin_buffer_rd_ind >=Mic_Delay_Size)
		s_aec_ptr->rin_buffer_rd_ind = 0;

    if( s_aec_ptr->init_flag == 0 )
    {
        (s_aec_ptr->p_state)->ear[(s_aec_ptr->p_state)->nlp_cnt64] = rin;
        (s_aec_ptr->p_state)->nlms_resi_echo[(s_aec_ptr->p_state)->nlp_cnt64] = sin; //sout;

        if ((s_aec_ptr->p_state)->nlp_cnt64==FRAME_LEN-1)
        {
            s_aec_ptr->init_flag = 1;
            (s_aec_ptr->p_state)->nlp_cnt64 = 0;

            (s_aec_ptr->p_state)->nlp_index = 0;
        }
        else
            (s_aec_ptr->p_state)->nlp_cnt64 += 1;

        return;
    }
    else
    {
        int tmp32, k;
        int16 tmp16;

        if ((s_aec_ptr->p_state)->nlp_cnt64==NLPOUT_LEN)
        {
            nlp();

            // add de-emphasis filter
            for (k=0;k<=NLPOUT_LEN-1;k++)
            {
                tmp32 = (s_aec_ptr->p_state)->nlp_out[(s_aec_ptr->p_state)->nlp_index+k];
                tmp16 = f_sat(tmp32);

            #ifdef CONFIG_APP_EQUANLIZER
                tmp16 = app_equalizer_calc_hfp(tmp16);
            #endif

                s_aec_ptr->nlpout[k] = tmp16;
            }

            os_memcpy( &(s_aec_ptr->p_state)->ear[(s_aec_ptr->p_state)->nlp_index], &(s_aec_ptr->p_state)->tear[0], NLPOUT_LEN*2 );
            os_memcpy( &(s_aec_ptr->p_state)->nlms_resi_echo[(s_aec_ptr->p_state)->nlp_index], &(s_aec_ptr->p_state)->tres[0], NLPOUT_LEN*2 );
            os_memset( &(s_aec_ptr->p_state)->nlp_out[(s_aec_ptr->p_state)->nlp_index], 0, NLPOUT_LEN*2 );
            (s_aec_ptr->p_state)->nlp_cnt64 = 0;
            (s_aec_ptr->p_state)->nlp_index += NLPOUT_LEN;
            (s_aec_ptr->p_state)->nlp_index &= 0xFF;
        #ifdef AEC_FLASH_TEST
            app_aec_data_print( (uint8 *)s_aec_ptr->nlpout, sizeof(s_aec_ptr->nlpout));
        #else
            pcm_fill_buffer((uint8 *)s_aec_ptr->nlpout, NLPOUT_LEN*2);
        #endif
        }

        (s_aec_ptr->p_state)->tear[(s_aec_ptr->p_state)->nlp_cnt64] = rin;
        (s_aec_ptr->p_state)->tres[(s_aec_ptr->p_state)->nlp_cnt64] = sin; //sout;
        (s_aec_ptr->p_state)->nlp_cnt64 += 1;

        return;
    }

}

int hf_sco_handle_process( int oper );
//int app_aec_get_rx_size(void)
//{
//	return ECHO_RX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->rx_samples );
//}
//int app_aec_get_tx_size(void)
//{
//    return ECHO_TX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->tx_samples );
//}
#ifdef BT_DUALMODE
void RAM_CODE app_aec_fill_rxbuff( uint8 *buff, uint8 fid,uint16 len )
#else
void DRAM_CODE app_aec_fill_rxbuff( uint8 *buff, uint8 fid,uint16 len )
#endif
{
    if (app_wave_playing())
        return;
#ifdef AEC_STATIS_TEST
    int bytes = 0;
#endif
    //AEC_PRT("app_aec_fill_rxbuff\r\n");
#ifdef CONFIG_APP_DATA_CAPTURE
    if(app_aec_get_data_capture_mode())
    {
        if(!s_aec_ptr->sync_send_flag)
        {
            s_aec_ptr->sync_send_flag = 1;
            os_memset( buff+8, 0x70, 4);
        }
    }
#endif

    #if(CONFIG_AUDIO_TRANSFER_DMA == 1)
        cvsd_fill_buffer((uint8 *)buff,fid,len );
    #else
        aud_fill_buffer((uint8 *)buff, len );

//	 #ifdef ADJUST_AEC_DELAY
	    if( aud_get_buffer_size() < AUDIO_BUFF_LEN - 256 )
	    {
		//if(!(BK3000_AUD_FIFO_CONF &((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ))))
		//{
		//}
	        aud_open();
	        aud_PAmute_oper(0);
	    }
//	 #endif
    #endif
    if((s_aec_ptr->debug_flag == 0) && (fid != 0x0e))
    {
        #ifdef AEC_STATIS_TEST
            bytes = rb_fill_buffer(&s_aec_ptr->rx_samples, buff, len, AEC_RX_FILL);
        #elif defined(ADJUST_AEC_DELAY)
        {
            int16 cpy_size;
            cpy_size = rb_fill_buffer(&s_aec_ptr->rx_samples, buff, len, AEC_RX_FILL);

            if(cpy_size<len)
            {
                s_aec_ptr->sync_adjusted_flag = 0;
            }
            else if(s_aec_ptr->sync_adjusted_flag == 0)
            {
                s_aec_ptr->sync_adjusted_flag = 1;
            #if 0
            {
                uint32 tmp,tmp1,tmp2;

            #if(CONFIG_AUDIO_TRANSFER_DMA == 1)
                tmp = reg_DMA_CH0_SRC_ADDR;
                if(tmp<reg_DMA_CHO_SRC_CUR_PTR) tmp += AUDIO_BUFF_LEN;
                tmp -= reg_DMA_CHO_SRC_CUR_PTR;
            #else
                tmp = AUDIO_BUFF_LEN - aud_get_buffer_size();
            #endif
                tmp = tmp>>1;
                tmp1 = app_aec_get_rx_size();
                tmp2 = app_aec_get_tx_size();
                tmp = tmp - tmp1 + tmp2;
                tmp = tmp >> 1;

                aec_set_latency(tmp);
                //UART_WRITE_BYTE(0x77);
                //UART_WRITE_BYTE(0x77);
                //UART_WRITE_BYTE((tmp>>0)&0xff);
                //UART_WRITE_BYTE((tmp>>8)&0xff);
            }
            #endif
            }

        }
        #else
            rb_fill_buffer(&s_aec_ptr->rx_samples, buff, len, AEC_RX_FILL);
        #endif
    }
#ifdef AEC_STATIS_TEST
    rx_input_bytes += len;
    rx_buffer_bytes += bytes;
#endif
    return;
}

#ifdef BT_DUALMODE
void RAM_CODE app_aec_fill_txbuff( uint8 *buff, uint16 len )
#else
void DRAM_CODE app_aec_fill_txbuff( uint8 *buff, uint16 len )
#endif
{
#ifdef AEC_STATIS_TEST
    int bytes = 0;
#endif
    if (!s_aec_ptr)
        return;

    //AEC_PRT("app_aec_fill_txbuff\r\n");
    if( s_aec_ptr->debug_flag == 0 )
    {
        #ifdef AEC_STATIS_TEST
            bytes = rb_fill_buffer(&s_aec_ptr->tx_samples, buff, len, AEC_TX_FILL);
        #else
        {
            /*
            int16 filled_len;
            filled_len = rb_fill_buffer(&s_aec_ptr->tx_samples, buff, len, AEC_TX_FILL);
            if(filled_len < len)
            {
                BK3000_GPIO_19_CONFIG = 0x02;
                BK3000_GPIO_19_CONFIG = 0x00;
            }
            */
            rb_fill_buffer(&s_aec_ptr->tx_samples, buff, len, AEC_TX_FILL);
            
        }
        #endif

        //msg_put(MSG_SDADC);
    }

#ifdef AEC_STATIS_TEST
    tx_input_bytes += len;
    tx_buffer_bytes += bytes;
#endif
    return;
}

#ifdef BT_DUALMODE
void RAM_CODE app_aec_swi( void )
#else
void DRAM_CODE app_aec_swi( void )
#endif
{
    int rx_size, tx_size, size, i;
    uint16 rx_tmp[16], tx_tmp[16];

    if(s_aec_ptr == NULL)
        return;
    if(s_aec_ptr->p_state == NULL)
        return;

    rx_size = ECHO_RX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->rx_samples );
    tx_size = ECHO_TX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->tx_samples );
    if( rx_size > tx_size )
    {
        size = tx_size >> 1;
    }
    else
    {
        size = rx_size >> 1;
    }

    if( size < 8 )
    {
        return;
    }
    else
    {
    #if 1
        app_aec_adj_latency(rx_size ,tx_size);
    #else
        static int aec_cnt=0;
        if(++aec_cnt>200)
        {
            uint32 tmp,tmp1;
            aec_cnt = 0;


        #if(CONFIG_AUDIO_TRANSFER_DMA == 1)
        	tmp = reg_DMA_CH0_SRC_ADDR;
        	if(tmp<reg_DMA_CHO_SRC_CUR_PTR) tmp += AUDIO_BUFF_LEN;
        	tmp -= reg_DMA_CHO_SRC_CUR_PTR;
        	tmp = tmp>>1;
        #else
            tmp = AUDIO_BUFF_LEN - aud_get_buffer_size();
        #endif

            tmp = tmp - rx_size + tx_size;
            tmp = tmp >> 1;

            tmp1 = aec_get_latency();
            if(tmp1>tmp)tmp1 = tmp1 -tmp;
            else tmp1 = tmp - tmp1;
            if(tmp1 > 100)
            aec_set_latency(tmp);
        }
        //msg_put(MSG_SDADC);
    #endif
    }
    if( size > 16 )
        size = 16;
#ifdef AEC_STATIS_TEST
    aec_bytes += size*2;
#endif

#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif

#ifdef CONFIG_APP_DATA_CAPTURE
    if(app_aec_get_data_capture_mode())
    {
        if(!UART_TX_FIFO_EMPTY)
        //	if(UART_TX_FIFO_COUNT>20)
        {
            //msg_put(MSG_SDADC);
            return;
        }
    }
#endif
    rb_get_buffer_with_length(&s_aec_ptr->rx_samples,(uint8 *)&rx_tmp[0],size<<1);
    rb_get_buffer_with_length(&s_aec_ptr->tx_samples,(uint8 *)&tx_tmp[0],size<<1);
    for( i = 0; i < size; i++ )
    {
	#ifdef CONFIG_APP_DATA_CAPTURE
		if(app_aec_get_data_capture_mode())
		{
		        //rb_get_one_sample(&s_aec_ptr->rx_samples, &rx_tmp);
		        //rb_get_one_sample(&s_aec_ptr->tx_samples, &tx_tmp);
				pcm_fill_buffer((uint8 *)&tx_tmp[0],size<<1);
			{
				unsigned int oldmask = get_spr(SPR_VICMR(0));	 //read old spr_vicmr
				set_spr(SPR_VICMR(0), 0x00);					 //mask all/low priority interrupt.
				UART_WRITE_BYTE(rx_tmp[i]&0xff);
				UART_WRITE_BYTE((rx_tmp[i]>>8)&0xff);
				UART_WRITE_BYTE(tx_tmp[i]&0xff);
				UART_WRITE_BYTE((tx_tmp[i]>>8)&0xff);
				set_spr(SPR_VICMR(0), oldmask); 				 //recover the old spr_vicmr.
			}
		}
		else
		{
			//rb_get_one_sample(&s_aec_ptr->rx_samples, &rx_tmp);
			//rb_get_one_sample(&s_aec_ptr->tx_samples, &tx_tmp);
			app_aec_do_aec( rx_tmp[i], tx_tmp[i] );
		}
	#else
		//rb_get_one_sample(&s_aec_ptr->rx_samples, &rx_tmp);
		//rb_get_one_sample(&s_aec_ptr->tx_samples, &tx_tmp);
        
        app_aec_do_aec( rx_tmp[i], tx_tmp[i] );
	#endif
    }

}

#ifdef CONFIG_APP_DATA_CAPTURE
uint8 data_capture_enable;

void app_aec_set_data_capture_enable( uint8 enable )
{
	data_capture_enable = enable;
}
__inline	uint8 app_aec_get_data_capture_mode(void)
{
	return data_capture_enable;
}
#endif





void app_aec_debug( uint8 enable )
{
    s_aec_ptr->debug_flag = enable;
}
uint32_t  *app_aec_get_ptr(void)
{
    return (uint32_t *)s_aec_ptr;
}

#ifdef AEC_FLASH_TEST
static int app_aec_data_print( char *buffer , int length)
{
    int i;

    for( i = 0; i < length; i+=2 )
    {
        os_printf("%04x ", *((uint16 *)(buffer + i)));
        if( ( i % 16 ) == 14 )
        {
            os_printf("\r\n");
            Delay(10);
        }
    }

    return 0;

}
#endif

void app_set_aec_para(uint8 *para)
{
    int rx_size, tx_size;
	uint32 aud_data_size,now_latecy;//last_latency

	(s_aec_ptr->mic_analyse).flag = 1;

		#if(CONFIG_AUDIO_TRANSFER_DMA == 1)
			aud_data_size = reg_DMA_CH0_SRC_ADDR;
			if(aud_data_size<reg_DMA_CHO_SRC_CUR_PTR)
			aud_data_size += AUDIO_BUFF_LEN;
			aud_data_size -= reg_DMA_CHO_SRC_CUR_PTR;
			aud_data_size = aud_data_size>>1;
		#else
			aud_data_size = AUDIO_BUFF_LEN - aud_get_buffer_size();
		#endif

		rx_size = ECHO_RX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->rx_samples );
		tx_size = ECHO_TX_BUFFER_SIZE - rb_get_buffer_size( &s_aec_ptr->tx_samples );

		now_latecy = (aud_data_size - rx_size + tx_size)>>1;
//		last_latency = aec_get_latency();

	if(para[0]||para[1]||para[2]||para[3]||para[4])
	{
		int8 tmpint8;
		tmpint8 = (int8)para[0];
		s_aec_ptr->fft_shift = (int32)tmpint8;
		s_aec_ptr->decay_times = para[1];
		tmpint8 = (int8)para[2];
    	s_aec_ptr->mic_gain = (int16)tmpint8;
		BK3000_AUD_AGC_CONF2 = (BK3000_AUD_AGC_CONF2&(~(0x7f<<20)))|((para[3]&0x7f)<<20);
		BK3000_AUD_ADC_CONF0 = (BK3000_AUD_ADC_CONF0&(~(0x3f<<18)))|((para[4]&0x3f)<<18);
	}
	//THIS printf is used for set AEC para on-line.
	os_printf("\r\nlatency:%d,fft_shift:%d,decay_time:%d,aec_mic_gain:%d,mic_ana:%d,mic_dig:%d,mic_max:%d\r\n",
		now_latecy,s_aec_ptr->fft_shift,s_aec_ptr->decay_times,s_aec_ptr->mic_gain,(BK3000_AUD_AGC_CONF2>>20)&0x7f,(BK3000_AUD_ADC_CONF0>>18)&0x3f,s_aec_ptr->mic_analyse.mic_max);
	s_aec_ptr->mic_analyse.mic_max = 0;
}

#else
void DRAM_CODE app_aec_fill_rxbuff( uint8 *buff, uint8 fid,uint16 len )
{
#if(CONFIG_AUDIO_TRANSFER_DMA == 1)
    cvsd_fill_buffer(buff,fid,len);
#else
    aud_fill_buffer(buff, len);
#endif                            
}

#endif
// EOF
