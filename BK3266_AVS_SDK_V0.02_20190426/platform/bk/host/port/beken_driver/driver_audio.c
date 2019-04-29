#include "hw_leds.h"
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "coder.h"
#include "bkmp3_resample.h"
#include "lp_pdd_main.h" // add by cai.zhong 20190220 for audio data

#if (CONFIG_DEBUG_PCM_TO_UART == 1)
extern void uart_send_ppp(unsigned char *buff, unsigned char fid,unsigned short len);
#endif

#ifdef CONFIG_TWS
sync_data_TypeDef sync_data;
#endif

#if 0//(CONFIG_APP_TOOLKIT_5 == 0)
#if (CONFIG_EXT_AUDIO_PA_EN == 1)
    static aud_volume_t aud_vol_table[1+16] =
    {
        {0x00,44},            // 0
    	{0x01,45},            // 1
    	{0x03,45},            // 1+
        {0x05,45},            // 2
        {0x07,45},            // 3
        {0x09,45},            // 4
        {0x0b,45},            // 5
        {0x0d,45},            // 6
        {0x0f,45},            // 7
        {0x11,45},            // 8
        {0x13,45},            // 9
        {0x15,45},            // A
        {0x17,45},            // B
        {0x19,45},            // C
        {0x1B,45},            // D
        {0x1D,45},            // E
        {0x1F,45},            // F
    };
#else
    static aud_volume_t aud_vol_table[1+16] =
    {
        {0x1A,0},            // 0
    	{0x1A,2},            // 1
    	{0x1A,4},            // 1+
        {0x1A,6},            // 2
        {0x1A,9},            // 3
        {0x1A,12},            // 4
        {0x1A,15},            // 5
        {0x1A,18},            // 6
        {0x1A,21},            // 7
        {0x1A,24},            // 8
        {0x1A,27},            // 9
        {0x1A,30},            // A
        {0x1A,33},            // B
        {0x1A,36},            // C
        {0x1A,39},            // D
        {0x1A,42},            // E
        {0x1A,45},            // F
    };
#endif
#endif
uint8 pa_delay_unmute=0;
uint8 pa_delay_mute=0;
volatile uint32 BK3000_AUD_AUDIO_CONF_BAK = 0;
AUDIO_CTRL_BLK  audio_ctrl_blk;
PCM_CTRL_BLK    pcm_ctrl_blk;
static aud_mute_cfg_t aud_mute_cfg;
static int8 mute_fast_shift = 6;   // range: 3 -10
static int8 mute_slow_shift = 9; // range: 5 -  12
extern volatile uint8 flag_sbc_buffer_play;
#if (CONFIG_EXT_AUDIO_PA_EN == 1)
static uint32 mute_amp = 0;
#endif
#define MUTE_LOW_THD        2
#define MUTE_HIGH_THD       7
#if (ADJUST_AEC_DELAY == 1)
static u_int8 s_audio_open_stat = 0;
#endif
//static uint8 extPA_open_req = 0;
#if (CONFIG_APP_MP3PLAYER == 1)
extern wavInfo wav_info;
extern short  *aulawsmpl;
extern short  *alawtbl;
extern short  *ulawtbl;
#endif
struct AUDIO_TypeDef AUDIO = {
    (volatile AUDIO_REG0x0_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 0*4),
    (volatile AUDIO_REG0x1_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 1*4),
    (volatile AUDIO_REG0x2_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 2*4),
    (volatile AUDIO_REG0x3_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 3*4),
    (volatile AUDIO_REG0x4_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 4*4),
    (volatile AUDIO_REG0x5_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 5*4),
    (volatile AUDIO_REG0x6_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 6*4),
    (volatile AUDIO_REG0x7_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 7*4),
    (volatile AUDIO_REG0x8_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 8*4),
    (volatile AUDIO_REG0x9_TypeDef *)  (BK3000_AUD_BASE_ADDR+ 9*4),
    (volatile AUDIO_REG0xA_TypeDef *)  (BK3000_AUD_BASE_ADDR+10*4),
    (volatile AUDIO_REG0xB_TypeDef *)  (BK3000_AUD_BASE_ADDR+11*4),
    (volatile AUDIO_REG0xC_TypeDef *)  (BK3000_AUD_BASE_ADDR+12*4),
    (volatile AUDIO_REG0xD_TypeDef *)  (BK3000_AUD_BASE_ADDR+13*4),
    (volatile AUDIO_REG0xE_TypeDef *)  (BK3000_AUD_BASE_ADDR+14*4),
    (volatile AUDIO_REG0xF_TypeDef *)  (BK3000_AUD_BASE_ADDR+15*4),
    (volatile AUDIO_REG0x10_TypeDef *) (BK3000_AUD_BASE_ADDR+16*4),
    (volatile AUDIO_REG0x11_TypeDef *) (BK3000_AUD_BASE_ADDR+17*4),
    (volatile AUDIO_REG0x12_TypeDef *) (BK3000_AUD_BASE_ADDR+18*4),
};
#if(CONFIG_AUD_FADE_IN_OUT == 1)
#define AUD_FADE_STEP   2           // about 640ms =  10ms * 128/2
static t_aud_fade_state s_aud_fade_status = AUD_FADE_NONE;
int16_t s_fade_scale = 0;
__INLINE__ void  set_aud_fade_in_out_state(t_aud_fade_state state)
{
    if(state == AUD_FADE_IN)
        s_fade_scale = 0;
    else if(state == AUD_FADE_OUT)
        s_fade_scale = 128;
    s_aud_fade_status = state;        
}
__INLINE__ t_aud_fade_state get_aud_fade_in_out_state(void)
{
    return s_aud_fade_status;
}
void aud_fade_status_debug(void)
{
#if 0
    os_printf("---------Fade in/out status------------\r\n");
    os_printf("| Fade in/out state:%d\r\n",s_aud_fade_status);
    os_printf("| Fade in/out step :%d\r\n",s_fade_scale);
    os_printf("---------Fade in/out end---------------\r\n");
#endif
}
void aud_fade_in_out_process(void)
{
    t_aud_fade_state status;
    status = get_aud_fade_in_out_state();
    if(status == AUD_FADE_IN)
    {
        s_fade_scale += AUD_FADE_STEP;
        if(s_fade_scale == 128)
        {
            set_aud_fade_in_out_state(AUD_FADE_FINISHED);
        }
    }
    else if(status == AUD_FADE_OUT)
    {
        s_fade_scale -= AUD_FADE_STEP;
        if(s_fade_scale == 0)
        {
            set_aud_fade_in_out_state(AUD_FADE_FINISHED);
        }
    }
    else
    {
        s_fade_scale = 128;
        set_aud_fade_in_out_state(AUD_FADE_NONE);
    }
}
#endif
static __inline int16 f_sat(int32 din)
{
    if (din>32767)
        return 32767;
    if (din<-32768)
        return -32768;
    else
        return(din);
}

static void aud_mute_func_init( int8 fast_shift, int8 slow_shift )
{
    mute_fast_shift = fast_shift;
    mute_slow_shift = slow_shift;
}

#define  PA_MUTE_STATUS 1
#define  PA_UNMUTE_STATUS 0
void aud_mute_update( int16 samplel, int16 sampler )
{
#if (CONFIG_EXT_AUDIO_PA_EN == 1)

    if(aud_mute_cfg.auto_mute_flag)
    {
        int8 mute_shift;
        uint32 abs_samplel = (ABS(samplel) << 16);
        uint32 abs_sampler = (ABS(sampler) << 16);
        uint32 mute_tmp = mute_amp;
        uint32 abs_sample = (abs_samplel >> 1) + (abs_sampler >> 1);

        if(abs_sample > mute_amp)
            mute_shift = mute_fast_shift;
        else
            mute_shift = mute_slow_shift;

        mute_amp -= (mute_amp >> mute_shift);
        mute_amp += (abs_sample >> mute_shift);

        mute_tmp = mute_amp >> 16;

        if((mute_tmp > MUTE_HIGH_THD) && (aud_mute_cfg.mute_status==PA_MUTE_STATUS))
        {
            bt_flag2_operate(APP_FLAG2_MUTE_FUNC_MUTE, 0);
            if(!bt_flag2_is_set(APP_FLAG2_VOL_MUTE))
                aud_PAmute_oper(0);
        }
        else if((mute_tmp < MUTE_LOW_THD) &&(aud_mute_cfg.mute_status==PA_UNMUTE_STATUS))
        {
            bt_flag2_operate(APP_FLAG2_MUTE_FUNC_MUTE, 1);
            aud_PAmute_oper(1);
            
        }
    }

#endif
}

#if 0//def BEKEN_DEBUG
void aud_dump( void )
{
    os_printf("aud isr empty count: %d\r\n", audio_ctrl_blk.aud_empty_count);
}
#endif
#if(CONFIG_AUDIO_TRANSFER_DMA == 1)
void aud_dma_initial(void)
{
    if (audio_ctrl_blk.aud_dmaconf.dma_enable)
    {
        return;
    }

    audio_ctrl_blk.aud_dmaconf.request_select  = 6                 ;  // 0(Software) 1(Uart_rx) 2(Uart_tx) 3(Pcm_rx) 4(Pcm_tx) 5(Aud_rx) 6(Aud_tx)
	audio_ctrl_blk.aud_dmaconf.transfer_mode   = 2                 ;  // 0(Single) 1(Block) 2(SingleRepeat) 3(BlockRepeat)
	audio_ctrl_blk.aud_dmaconf.dst_addr_incr   = 0                 ;  // 0/1(No change) 2(Decrease) 3(Increase)
	audio_ctrl_blk.aud_dmaconf.src_addr_incr   = 3                 ;  // 0/1(No change) 2(Decrease) 3(Increase)
	audio_ctrl_blk.aud_dmaconf.dst_data_type   = 2                 ;  // 0(8 bits) 1(16 bits) 2(32 bits)
	audio_ctrl_blk.aud_dmaconf.src_data_type   = 2                 ;  // 0(8 bits) 1(16 bits) 2(32 bits)
	audio_ctrl_blk.aud_dmaconf.trigger_type    = 1                 ;  // 0(Posedge)  1(High Level)
	audio_ctrl_blk.aud_dmaconf.interrupt_en    = 0                 ;  // 0(Disable)  1(Enable)
	audio_ctrl_blk.aud_dmaconf.soft_request    = 0                 ;  // 0(Hardware) 1(Software)
	audio_ctrl_blk.aud_dmaconf.transfer_size   = 0                 ;  // DMA Transfer Length
	audio_ctrl_blk.aud_dmaconf.src_address     = (uint32) &(audio_ctrl_blk.data_buff[0])       ;  // DMA Source Address
	audio_ctrl_blk.aud_dmaconf.dst_address     = (uint32) &(BK3000_AUD_DAC_PORT) ;  // DMA Destination Address
	audio_ctrl_blk.aud_dmaconf.src_addr_top    = (uint32) &(audio_ctrl_blk.data_buff[0]) ;  // DMA Source Top Boundary
	audio_ctrl_blk.aud_dmaconf.src_addr_bottom = (uint32) (&(audio_ctrl_blk.data_buff[0]) + AUDIO_BUFF_LEN-4);  // DMA Source Bottom Boundary
	audio_ctrl_blk.aud_dmaconf.dst_addr_top    = 0                 ;  // DMA Destination Top Boundary
	audio_ctrl_blk.aud_dmaconf.dst_addr_bottom = 0                 ;  // DMA Destination Bottom Boundary
	audio_ctrl_blk.aud_dmaconf.dma_enable      = 0                 ;
//#ifdef ADJUST_AEC_DELAY
	aud_dma_reset();
//#endif
}
void aud_dma_start(void)
{
    if (audio_ctrl_blk.aud_dmaconf.dma_enable)
    {
        return;
    }

    audio_ctrl_blk.aud_dmaconf.dma_enable = 1;
#ifdef ADJUST_AEC_DELAY
	struct dma_struct *dma_config;
	dma_config = &audio_ctrl_blk.aud_dmaconf;
    reg_DMA_CH0_CONTROL = ((dma_config->request_select & 0x07) <<16) +
                                  ((dma_config->transfer_mode & 0x03) <<14) +
                                  ((dma_config->dst_addr_incr & 0x03) <<12) +
                                  ((dma_config->src_addr_incr & 0x03) <<10) +
                                  ((dma_config->dst_data_type & 0x03) <<8 ) +
                                  ((dma_config->src_data_type & 0x03) <<6 ) +
                                  ((dma_config->trigger_type & 0x01)  <<5 ) +
                                  ((dma_config->interrupt_en & 0x01)  <<4 ) +
                                  ((dma_config->dma_enable & 0x01)  <<2 ) +
                                  ((dma_config->soft_request & 0x01) <<1 ) ;

#else
    start_dma_transfer(0,&audio_ctrl_blk.aud_dmaconf);
#endif
    return;
}
void aud_dma_stop(void)
{
    audio_ctrl_blk.aud_dmaconf.dma_enable = 0;
    start_dma_transfer(0,&audio_ctrl_blk.aud_dmaconf);
}
void aud_dma_reset(void)
{
    //os_printf("aud_dma_reset\r\n");
    audio_ctrl_blk.aud_dmaconf.dma_enable = 1;
    start_dma_transfer(0,&audio_ctrl_blk.aud_dmaconf);
    audio_ctrl_blk.aud_dmaconf.dma_enable = 0;
    start_dma_transfer(0,&audio_ctrl_blk.aud_dmaconf);
}

#endif
static void audio_dac_digital_gain_set(uint32 new_gain)
{
    uint32 dac_dig_reg;
    int8 step;
    uint32 old_gain = ((BK3000_AUD_DAC_CONF0 >> 18) & 0x03f);
    if(old_gain == new_gain)
        return;
    else
    {
        step = 2 *(new_gain > old_gain) - 1;
        dac_dig_reg = BK3000_AUD_DAC_CONF0;
        while(old_gain != new_gain)
        {
            old_gain += step;
            dac_dig_reg &= ~0x00FC0000;
            dac_dig_reg |= (((old_gain) << 18));//|(1 << 16)
            BK3000_AUD_DAC_CONF0 = dac_dig_reg;
            os_delay_ms(1);
        }
    }
}
#ifdef CONFIG_DRIVER_DAC

/***************driver dac part *****************/
//static void dac_volume_mute( uint8 enable );
#ifdef CONFIG_BLUETOOTH_HFP
static int adc_initial(uint32 freq, uint32 bits_per_sample)
{
    sdadc_init();
    // add begin by cai.zhong 20190220 for audio data
    /* Step 1: open clk */
	BK3000_PMU_PERI_PWDS &= ~(bit_PMU_AUD_PWD);
	/* Step 2: Disable adc & disable interupt */
    BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
    BK3000_AUD_AUDIO_CONF_BAK &=  ~(1<<sft_AUD_ADC_ENABLE);
    BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    /* Step 3: set freq */
    BK3000_AUD_AUDIO_CONF_BAK &= ~( 0x3 << sft_AUD_SAMP_ADC_RATE );
    if(freq == 44100)
    {
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_ADC_RATE );
    }
    else if(freq == 48000)                       
    {
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_ADC_RATE );
    }
    else if( freq == 8000 )
    {
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_ADC_RATE );
    }
    else if( freq == 16000 )
    {
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_ADC_RATE );
    }
    BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    // add end by cai.zhong 20190220 for audio data
    return 0;
}
#if 0
static void adc_close( void )
{
#ifdef CONFIG_DRIVER_ADC
    sdadc_enable(0);
#endif
}
#endif
#endif

#ifdef CONFIG_TWS

void dac_init_clk(void)
{
	if((BK3000_AUD_AUDIO_CONF & 0x3 ) == 2) //44.1K
			BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K;
	else //48K
			BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K;
}
void show_dac_clk(int pos)
{
	os_printf("clk%d:%x\r\n",pos,BK3000_AUD_DAC_FRACCOEF);
}
void RAM_CODE dac_set_clk( uint32_t clk_val)
{
	BK3000_AUD_DAC_FRACCOEF = clk_val;
}

#endif

static int dac_initial( uint32 freq, uint32 bits_per_sample )
{
     uint8 k;
#ifdef CONFIG_APP_EQUANLIZER
    BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 2 ); //close eq
    BK3000_AUD_DAC_CONF0 |= ( 1 << sft_AUD_DAC_HPF2_BYPASS );
#endif
#if defined(CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE)
	if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON)
		&& app_env_check_sniff_mode_Enable())
	{
		BK3000_A6_CONFIG &= ~(1 << 0);
	}
#endif
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    aud_dma_stop();
#else
    BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));	//disable intr
#endif

    /* Digital Dac Open */
    /* Step 1: open clk */
    BK3000_PMU_PERI_PWDS &= ~(bit_PMU_AUD_PWD);
	/* Step 2: delay */
	os_delay_us(10);
    /* Step 3: Disable dac for reset Sigma&Delta state*/
    BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
    BK3000_AUD_AUDIO_CONF_BAK |=  (1<<sft_AUD_ADC_ENABLE);
    BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
	BK3000_AUD_AUDIO_CONF_BAK &= ~(1<<sft_AUD_DAC_ENABLE);
    BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    jtask_stop( app_get_audio_task_handle() );
#endif
    /* Step 4: set sample rate*/

#if (CONFIG_DAC_DELAY_OPERATION == 1)
    /*
    * It is very important that you change sample rate !!! :)
    * Clear FIFO data and Sigma&Delta state;
    * Delay Time >= 50ms;
    */
    while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
//    os_delay_ms(50);
    for(k=0;k<16;k++)
        AUD_WRITE_DACL(0, 0);
	while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
    for(k=0;k<16;k++)
        AUD_WRITE_DACL(0, 0);
    os_delay_ms(50);
#endif

    BK3000_AUD_AUDIO_CONF_BAK &= ~( 0x3 << sft_AUD_SAMP_DAC_RATE );
    BK3000_AUD_AUDIO_CONF_BAK &= ~( 0x3 << sft_AUD_SAMP_ADC_RATE );
    //BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    BK3000_AUD_AUDIO_CONF2 |= ( 1 << 0 );

    BK3000_AUD_FIFO_CONF &= ~( 0x3FF << sft_AUD_DAC_DACR_RD_THRE );
    BK3000_AUD_FIFO_CONF |= (( 8 << sft_AUD_DAC_DACR_RD_THRE )|(8 << sft_AUD_DAC_DACL_RD_THRE) );
#if (CONFIG_NOUSE_RESAMPLE == 0)
    if(freq == 44100)
    {
        //BK3000_AUD_AUDIO_CONF |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_DAC_RATE ); //48M/(128*44.1K) /* Andre, 2012-2-1 */
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_ADC_RATE );
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K;
    }
    else if(freq == 48000)                        //48M/(128*48K)
    {
        //BK3000_AUD_AUDIO_CONF |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_ADC_RATE );
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K;
    }
    else if( freq == 8000 )
    {
        //BK3000_AUD_AUDIO_CONF |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_ADC_RATE );

        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 0 );
    }
    else if( freq == 16000 )
    {
        //BK3000_AUD_AUDIO_CONF |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_DAC_RATE );
        BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_ADC_RATE );

        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 0 );
    }
    else
        return -1;

#else
    // disable dac handset bit again, to make sure this bit unset.
    BK3000_AUD_AUDIO_CONF2 &= ~(1L<<0); 
    switch(freq)
    {
        case 11025:
            BK3000_AUD_AUDIO_CONF2 |= 1L<<0;
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_441K<<2);
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;
        case 22050:
            BK3000_AUD_AUDIO_CONF2 |= 1L<<0;
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_441K<<1); 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK; 
            break;
        case 44000:
        case 44100:
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_441K); 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_441K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;
            
        case 12000:
            BK3000_AUD_AUDIO_CONF2 |= 1L<<0;
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_48K<<2); ; 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;
        case 24000:
            BK3000_AUD_AUDIO_CONF2 |= 1L<<0;
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_48K<<1); 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK; 
            break;
        case 48000:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K; 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_48K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;

        case 8000:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_8K; 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_8K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;
        case 16000:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_16K; 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK; 
            break;
        case 32000:
            BK3000_AUD_AUDIO_CONF2 |= 1L<<0;
            BK3000_AUD_DAC_FRACCOEF = (AUDIO_DIV_16K>>1); 
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_DAC_RATE );
            BK3000_AUD_AUDIO_CONF_BAK |= ( AUDIO_CLK_DIV_16K << sft_AUD_SAMP_ADC_RATE );
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            break;    
        default:
            os_printf("DAC init: unsupport samplerate!!\r\n");
            return -1;
    }
#endif

    if(bt_flag1_is_set(APP_FLAG_LINEIN))
    {
        //BK3000_AUD_DAC_CONF0 &= ~0x00FC0000;
    #ifdef CONFIG_APP_EQUANLIZER
        if((app_equalizer_get_enable_count() < 5)||bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
            BK3000_AUD_DAC_CONF0 |= (1 << 16);//((45 << 18)|(1 << 16));
        else
	    {
	    	BK3000_AUD_DAC_CONF0 &= ~(1 << 16);
    	     //BK3000_AUD_DAC_CONF0 |= ((45 << 18));
	    }
    #else // when line in,set dig_gain = 0x2d;
        //BK3000_AUD_DAC_CONF0 |= ((45 << 18)|(1 << 16));
    #endif

        BK3000_AUD_DAC_CONF0 |= 0x20000;  // 需绕开HPF1 ，否则会对低频影响很大
    }
    else
    {
    #ifdef CONFIG_APP_EQUANLIZER
        //BK3000_AUD_DAC_CONF0 &= ~0x00FC0000;
        if((app_equalizer_get_enable_count() < 5)
			||bt_flag1_is_set(APP_FLAG_WAVE_PLAYING|APP_FLAG_SCO_CONNECTION))
        {
            BK3000_AUD_DAC_CONF0 |= (1 << 16);//((45 << 18)|(1 << 16));
	    }
        else
	    {
	    	BK3000_AUD_DAC_CONF0 &= ~(1 << 16);
	        //BK3000_AUD_DAC_CONF0 |= ((45 << 18));
	    }
    //#else  // why set dig_gain = 0x2d????
        //BK3000_AUD_DAC_CONF0 |= ((45 << 18)|(1 << 16));
    #endif
        BK3000_AUD_DAC_CONF0 |= 0x20000;  // default close equ
    }
    //BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));  //disable intr
#if 1//(CONFIG_DAC_DELAY_OPERATION == 1)
    if(freq > 44000)
    {
    #if (CONFIG_DAC_DELAY_OPERATION == 1)
        while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
		for(k=0;k<16;k++)
			AUD_WRITE_DACL(0, 0);
		while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
		for(k=0;k<16;k++)
			AUD_WRITE_DACL(0, 0);
		os_delay_ms(50);
    #endif
    #ifdef CONFIG_APP_EQUANLIZER
        if(app_equalizer_get_enable_count())
        {
            if(app_equalizer_get_enable_count() >= 5)
   	        {
   	           BK3000_AUD_DAC_CONF0 &= ~( 1 << sft_AUD_DAC_HPF2_BYPASS );
               BK3000_AUD_DAC_CONF0 |= ( 0 << sft_AUD_DAC_HPF2_BYPASS );
		        os_delay_ms(50);
   	        }
            BK3000_AUD_AUDIO_CONF2 |= ( 1 << 2 );//open eq
        }
    #endif
    }
#endif
	/* Step 5: enable dac */
	BK3000_AUD_AUDIO_CONF_BAK |= (1<<sft_AUD_DAC_ENABLE);
	BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
	BK3000_AUD_AUDIO_CONF_BAK &= ~(1<<sft_AUD_ADC_ENABLE);
	BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
{
    /* Step 6: Digital DAC FIFO will be filled by all zeros, to output DC value to analog DAC,and lowers 'POP' noise */
	while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
	for(k=0;k<16;k++)
		AUD_WRITE_DACL((k&0x01), (k&0x01));
	while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
	for(k=0;k<16;k++)
		AUD_WRITE_DACL(0, 0);
	/* Step 7: Analog DAC initial */
    BK3000_Ana_Dac_enable(1);
}
    return 0;
}

static void dac_close( void )
{
    audio_dac_digital_gain_set(0);
   
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    aud_dma_stop();
#else
    BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));  //disable intr
#endif

{

    /* BK3000_Ana_Dac_enable(0); */
    int16 k;
    os_delay_ms(10);
	while(!(BK3000_AUD_FIFO_STATUS&(bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY)));
	for(k=0;k<16;k++)
		AUD_WRITE_DACL(0, 0);
    os_delay_ms(10);
	set_system_idle_cnt(500); // 10 * 10ms = 100ms
#if 0
	BK3000_Ana_Dac_enable(0);
    //GPIO_Ax.a7->BTAudStbEn   = 1;
	BK3000_AUD_AUDIO_CONF_BAK &= ~(1 << sft_AUD_DAC_ENABLE );
	#if (CONFIG_APP_TOOLKIT_5 == 1)
		if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
		{
			BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_ADC_ENABLE );
			BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
		}else{
			BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
			BK3000_PMU_PERI_PWDS |= (bit_PMU_AUD_PWD);
		}
	#elif (CONFIG_AUDIO_DAC_ALWAYSOPEN == 1)
		BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_ADC_ENABLE );
		BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
	#else
    BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    BK3000_PMU_PERI_PWDS |= (bit_PMU_AUD_PWD);
	#endif
#endif
}
}

static void dac_open( void )
{
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    aud_dma_start();
    //BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_DAC_ENABLE );
    //BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
#else
    BK3000_AUD_FIFO_CONF |= ((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));
#endif
}

static void dac_volume_set( int8 volume )
{
    env_aud_dc_offset_data_t* dc_offset = app_get_env_dc_offset_cali();
    aud_volume_t *vol_tbl_ptr;
    int16 dig_gain = 0;
    //uint32 dac_dig_reg = BK3000_AUD_DAC_CONF0;
    app_env_handle_t env_h = app_env_get_handle();
     app_handle_t app_h = app_get_sys_handler();
#ifdef CONFIG_APP_EQUANLIZER
    if(env_h->env_cfg.eq_flag0 == 0xc5)
    {
        dig_gain = (int16)env_h->env_cfg.eq_gain;
    }
#endif
    if(bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
    {
        vol_tbl_ptr = (aud_volume_t *)&env_h->env_cfg.feature.a2dp_vol;
    #ifdef CONFIG_APP_EQUANLIZER
        dig_gain = 0;
    #endif
    os_printf("vol:wave=%d\r\n",volume);
    }
    else if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
    {
        vol_tbl_ptr = (aud_volume_t *)&env_h->env_cfg.feature.hfp_vol;
    #ifdef CONFIG_APP_EQUANLIZER
        dig_gain = 0;
    #endif
    os_printf("vol:hfp=%d\r\n",volume);
    }
    else if(SYS_WM_LINEIN_MODE == app_h->sys_work_mode)
    {
        vol_tbl_ptr = (aud_volume_t *)&env_h->env_cfg.feature.linein_vol;
        os_printf("vol:line=%d\r\n",volume);
    }
#if (CONFIG_APP_MP3PLAYER == 1)
    else if( (SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
    #ifdef CONFIG_APP_USB 
        ||(SYS_WM_UDISK_MUSIC_MODE == app_h->sys_work_mode)
    #endif
        )
    {
        vol_tbl_ptr = (aud_volume_t *)&env_h->env_cfg.feature.a2dp_vol;
        os_printf("vol:sd/usb=%d\r\n",volume);
    }
#endif
    else // SYS_WM_BT_MODE
    {
        vol_tbl_ptr = (aud_volume_t *)&env_h->env_cfg.feature.a2dp_vol;
        os_printf("vol:a2dp=%d\r\n",volume);
    }

    uint32_t ana_gain = vol_tbl_ptr[volume & 0x1F].ana_gain & 0x1f;
    dig_gain += vol_tbl_ptr[volume & 0x1F].dig_gain & 0x3f;
#ifdef CONFIG_APP_EQUANLIZER
    if(dig_gain<0) 
        dig_gain = 0;
#endif
	bt_flag2_operate(APP_FLAG2_SW_MUTE, !volume);

    if( volume > 0 )
    {
        aud_PAmute_oper(0);

        if( volume > AUDIO_VOLUME_MAX )
            volume = AUDIO_VOLUME_MAX;
        /***
        dac_dig_reg &= ~0x00FC0000;
        dac_dig_reg |= (((dig_gain) << 18)|(1 << 16));
        BK3000_AUD_DAC_CONF0 = dac_dig_reg;
        **/
        
        audio_dac_digital_gain_set(dig_gain);
        audio_dac_analog_gain_set(ana_gain, dc_offset->dac_l_dc_offset[ana_gain], dc_offset->dac_r_dc_offset[ana_gain]);
    }
    else  // volume = 0;
    {
        aud_PAmute_oper(1);

        if( volume <= AUDIO_VOLUME_MIN )
            volume = AUDIO_VOLUME_MIN;
		audio_dac_analog_gain_set(ana_gain, dc_offset->dac_l_dc_offset[ana_gain], dc_offset->dac_r_dc_offset[ana_gain]);
        if( aud_mute_cfg.mute_outside )
        {
            /*
            dac_dig_reg &= ~0x00FC0000;
            dac_dig_reg |= (((dig_gain) << 18)|(1 << 16));
            BK3000_AUD_DAC_CONF0 = dac_dig_reg;
            */
            audio_dac_digital_gain_set(dig_gain);

        }
		else
		{
            /*
            dac_dig_reg &= ~0x00FC0000;
            dac_dig_reg |= (((dig_gain) << 18)|(1 << 16));
            BK3000_AUD_DAC_CONF0 = dac_dig_reg;
            */
            audio_dac_digital_gain_set(dig_gain);

		}
    }
}

#if (CONFIG_AUDIO_TRANSFER_DMA == 0)
static void DRAM_CODE dac_isr( AUDIO_CTRL_BLK *ablk )
{
    uint32 sts = BK3000_AUD_FIFO_STATUS;
    static uint16 ptr[2] = {0};
    int16 len = 0;
//    int16 cnt = 0;
    if(sts&(bit_DACL_INTR_FLAG|bit_DACR_INTR_FLAG )  )
    {
        if( ablk->channels == 2 )
        {
        #ifdef CONFIG_TWS
            if( bt_flag2_is_set( APP_FLAG2_STEREO_PLAYING) )
            {
                uint32_t audio_out_cnt = 0;
                sync_data.u.sync_send.bt_clk = app_bt_read_CLKN(); //BK3000_BT_CLKN;
                sync_data.u.sync_send.aud_num += sync_data.aud_num_tmp;
                if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
                {
                    while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                    {
                        len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[0]);
                        if( len == -1 )
                            break;
                        len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[1]);
                        AUD_WRITE_DACL(ptr[0],ptr[0]);
                        sts = BK3000_AUD_FIFO_STATUS;
                        audio_out_cnt++;
                    }
                #if 1
                    if( len == -1 )
                    {
                        //os_printf("e");
                        while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                        {
                            AUD_WRITE_DACL(0,0);
                            sts = BK3000_AUD_FIFO_STATUS;
                            //audio_out_cnt++;
                        }
                    }
                #endif
                    //aud_mute_update( ptr[0], ptr[0]);
                }
                else
                {
                    while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                    {
                        len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[0]);
                        if( len == -1 )
                            break;
                        len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[1]);
                        AUD_WRITE_DACL(ptr[1],ptr[1]);
                        sts = BK3000_AUD_FIFO_STATUS;
                        audio_out_cnt++;
                    }
                #if 1
                    if( len == -1 )
                    {
                    //os_printf("e");
                    while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                    {
                        AUD_WRITE_DACL(0,0);
                        sts = BK3000_AUD_FIFO_STATUS;
                        //audio_out_cnt++;
                    }
                    }
                #endif
                    //aud_mute_update( ptr[1], ptr[1]);
                }

                sync_data.aud_num_tmp = audio_out_cnt;
            }
            else
        #endif
            {
                while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                {
                    len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[0]);
                    if( len == -1 )
                    {
                    //os_printf("e:%d",aud_get_buffer_size());
                        break;
                    }
                    len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[1]);
                #ifdef CONFIG_TWS
                    int32 aud_data_tmp;
                    aud_data_tmp = (int16)ptr[0]+(int16)ptr[1];
                    if(aud_data_tmp > 32767)
                        aud_data_tmp = 32767;
                    else if(aud_data_tmp < -32767)
                        aud_data_tmp = -32767;
                    ptr[0] = ptr[1] = (uint16)aud_data_tmp;
                #endif
                    /* os_printf("AUD_WRITE_DACL - L:%x, R:%x\r\n", ptr[0], ptr[1]); */
                    AUD_WRITE_DACL(ptr[0],ptr[1]);

                    sts = BK3000_AUD_FIFO_STATUS;
                }

                aud_mute_update( ptr[0], ptr[1] );
            }
        }
        else
        {
        #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
            uint32_t audio_out_cnt = 0;

            sync_data.u.sync_send.bt_clk = app_bt_read_CLKN(); //BK3000_BT_CLKN;

            sync_data.u.sync_send.aud_num += sync_data.aud_num_tmp;
        #endif
            while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
            {
                len = rb_get_one_sample( &ablk->aud_rb, (uint16 *)&ptr[0]);
                if( len == -1 )
                    break;
                AUD_WRITE_DACL(ptr[0], ptr[0]);
                sts = BK3000_AUD_FIFO_STATUS;
            #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
                audio_out_cnt++;
            #endif
            }
        #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
            sync_data.aud_num_tmp = audio_out_cnt;
            if(( len == -1 )&&bt_flag1_is_set( APP_FLAG_LINEIN ))
            {
                os_printf("e");
                while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
                {
                    AUD_WRITE_DACL(0,0);
                    sts = BK3000_AUD_FIFO_STATUS;
                //audio_out_cnt++;
                }
            }
        #endif

        }
    }
#ifdef CONFIG_TWS
    sts = BK3000_AUD_FIFO_STATUS;
    if( sts & (bit_DACL_FIFO_EMPTY|bit_DACR_FIFO_EMPTY))
    {
        ablk->empty_count++;
        if( ablk->empty_count >= AUDIO_EMPTY_COUNT_THRE )
        {
        //			os_printf("e:%d",aud_get_buffer_size());
            BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));  //disable intr
        #if (ADJUST_AEC_DELAY == 1)
            s_audio_open_stat = 0;
        #endif

            ablk->empty_count = 0;
        #ifdef BEKEN_DEBUG
            ablk->aud_empty_count++;
        #endif
        }
    }
    else
    {
        ablk->empty_count = 0;
    }
#else
    if(len == -1)
    {
        sts = BK3000_AUD_FIFO_STATUS;
        //cnt = 0;
        while( ! (sts & (bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
        {
            AUD_WRITE_DACL(0, 0);
            //目前是填充全"0"，可以考虑输出舒适噪声或者背景噪声等
            //或者是加入处理算法，可能会减少POP音；
            /****建议移除此段CODE以提高填充FIFO的效率****/
            //if(extPA_get_req()&& IS_INTERNAL_PA_OPEN)
            //{
            //}
            /****可以考虑放在中断服务入口的地方调用一次*****/
            sts = BK3000_AUD_FIFO_STATUS;
            //cnt++;
       }
       /* os_printf("e:%d\r\n",cnt); */
    }
#endif
}
#endif
#endif

#ifdef ADC_FIRST_DELAY
uint16 adc_first_discard_cnt = 0;   //the adc first discard data count for the noise while the adc open
boolean adc_first_discard_flag = 0;
#endif

#ifdef CONFIG_BLUETOOTH_HFP
#ifdef BT_DUALMODE
static void RAM_CODE adc_isr( void )
#else
static void DRAM_CODE adc_isr( void )
#endif
{
    uint32 sts = BK3000_AUD_FIFO_STATUS;
    int32 pcmval = 0;
    //uint8 pcmbuf[16];
    int16 pcmbuf[16+2];
    int16 pcm_tmp = 0;
#if defined(TWS_CONFIG_LINEIN_BT_A2DP_SOURCE)
    uint32 pcm_data32[16+2];
#endif
    int16 i = 0,size = 0;
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);
#if 1
    if(bt_flag1_is_set( APP_FLAG_LINEIN )
    #ifdef CONFIG_BLUETOOTH_COEXIST
        &&(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
    #endif
    )
    {
    #ifndef CONFIG_BLUETOOTH_COEXIST
    #if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
        BK3000_set_clock(1, 0); // xtal 26M 
    #endif
    #endif
        if(sts  & bit_ADCL_INTR_FLAG)
        {
            for(i = 0; i <8; i++)
            //while(!(sts & (bit_ADCL_FIFO_EMPTY|bit_DACR_FIFO_FULL|bit_DACL_FIFO_FULL)))
            {
                pcmval = BK3000_AUD_ADC_PORT;
                if(bt_flag2_is_set(APP_FLAG2_SW_MUTE) || (adcmute_cnt>0))
                    BK3000_AUD_DAC_PORT = 0;
                else
                {
                #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
                    if(flag_L_is_LplusR)
                    {
                        pcmbuf[0] = (int16) ((pcmval & 0xffff0000) >> 16);
                        pcmbuf[1] = (int16) (pcmval & 0x0000ffff);
                        pcmval = (pcmbuf[0] - pcmbuf[1]) >> 1;
                        pcmbuf[0] = f_sat(pcmval);
                        pcmbuf[1] = f_sat(pcmval * (-1));
                        pcmval = (((uint16)pcmbuf[0] << 16) | (uint16)pcmbuf[1]);
                        BK3000_AUD_DAC_PORT = pcmval;
                    }
                    else
                    {
                        if((pcmval & 0x0ffff) == 0x8000) // for: (-32768) * (-1) = 32767
                            pcm_tmp = (pcmval ^ 0x0000ffff);
                        else
                            pcm_tmp = ((pcmval ^ 0x0000ffff) + 1);
                        BK3000_AUD_DAC_PORT = (pcmval & 0xffff0000 ) | ((uint16) pcm_tmp & 0xffff) ;
                    }
                #elif(CONFIG_EXT_PA_DIFF_EN == 1)
                    pcmbuf[0] = (int16) ((pcmval & 0xffff0000) >> 16);
                    pcmbuf[1] = (int16) (pcmval & 0x0000ffff);
                    pcmval = (pcmbuf[0] - pcmbuf[1]) >> 1;
                    pcmbuf[0] = f_sat(pcmval);
                    pcmbuf[1] = f_sat(pcmval * (-1));
                    pcmval = (((uint16)pcmbuf[0] << 16) | (uint16)pcmbuf[1]);
                    BK3000_AUD_DAC_PORT = pcmval;
                #else
                    if((pcmval & 0x0ffff) == 0x8000) // for: (-32768) * (-1) = 32767
                        pcm_tmp = (pcmval ^ 0x0000ffff);
                    else
                        pcm_tmp = ((pcmval ^ 0x0000ffff) + 1);
                    BK3000_AUD_DAC_PORT = (pcmval & 0xffff0000 ) | ((uint16) pcm_tmp & 0xffff) ;
                #endif
                }
                //sts = BK3000_AUD_FIFO_STATUS;
            }
        }
    }
    else
#endif
    if(sts  & bit_ADCL_INTR_FLAG)
    {
        for(i=0;i<8;i++)
        //while(!(sts & (bit_ADCL_FIFO_EMPTY)))
        {
            pcmval = BK3000_AUD_ADC_PORT;
        #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
            pcm_data32[i] = pcmval;
        #endif
            //pcmbuf[2*i] = (pcmval >> 0) & 0x00ff;
            //pcmbuf[2*i + 1] = (pcmval >> 8) & 0x00ff;
            //*(pcmbuf+i) = (int16) (pcmval & 0x0000ffff);
            pcm_tmp = (int16) ((pcmval & 0xffff0000) >> 16);
            /* Don't saturate adc pcm, its value may be adjusted by mic gain */
		    //pcm_tmp = ((pcm_tmp + (1<<2)) >> 3) << 3;
            /*
		    if(pcm_tmp>=0) pcm_tmp&=(~0x7);
			else pcm_tmp |= 0x7;
            */
            *(pcmbuf+i) = pcm_tmp;
            //i++;
            //sts = BK3000_AUD_FIFO_STATUS;
        }
        size = i;
    #ifdef ADC_FIRST_DELAY
        if((adc_first_discard_flag == 0)&&(adc_first_discard_cnt < 1300))//we discard the adc data about the first 1300ms time for the opening noise
        {
            adc_first_discard_cnt++;

            for(i=0;i<size;i++)
            {
                pcmbuf[i]=0;
            }

            if(adc_first_discard_cnt >= 1300)
            {
                adc_first_discard_flag = 1;
            }
        }
    #endif

        if( bt_flag1_is_set( APP_FLAG_SCO_CONNECTION) )
        {
        #ifdef CONFIG_APP_AEC
            if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AEC_ENABLE))
                app_aec_fill_txbuff((uint8 *)&pcmbuf[0],size*2); /* app_aec_do_aec -> pcm_fill_buffer */
            else
        #endif
                pcm_fill_buffer((uint8 *)&pcmbuf[0],size*2);
        }
        // add begin by cai.zhong 20190220 for audio data
        else if(app_get_mic_state())
        {
            pcm_fill_buffer((uint8 *)&pcmbuf[0],size*2);
        }
        // add end by cai.zhong 20190220 for audio data
    }
}
#endif

void RAM_CODE dac_clk_adjust( int mode )
{
    if( (BK3000_AUD_AUDIO_CONF & 0x3 ) == 2 ) // 44.1k
    {
        switch( mode )
        {
        case 0:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K;
            break;
        case 1:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K_FAST;
            break;
        case 2:
        default:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K_SLOW;
            break;
        }
    }
    else if( (BK3000_AUD_AUDIO_CONF & 0x3 ) == 3 ) // 48k
    {
        switch( mode )
        {
        case 0:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K;
            break;
        case 1:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K_FAST;
            break;
        case 2:
        default:
            BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K_SLOW;
            break;
        }
    }
}

#ifdef CONFIG_TWS
void RAM_CODE dac_clk_adjust_tws( int mode )
{
    switch( mode )
    {
        case 1: //fast
		BK3000_AUD_DAC_FRACCOEF -= BK3000_AUD_DAC_FRACCOEF>>16;//15.258ppm
		break;
	 case 3: //fast fast
		BK3000_AUD_DAC_FRACCOEF -= BK3000_AUD_DAC_FRACCOEF>>14;// 4*15.258ppm
		break;
	 case 2: //slow
		BK3000_AUD_DAC_FRACCOEF += BK3000_AUD_DAC_FRACCOEF>>16;//15.258ppm
		break;
	 case 4: //slow slow
		BK3000_AUD_DAC_FRACCOEF += BK3000_AUD_DAC_FRACCOEF>>14;// 4*15.258ppm
		break;
	 #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
	//not change
	 case 9:
		break;
	 #endif
	 case 0:
	 default:
		if( (BK3000_AUD_AUDIO_CONF & 0x3 ) == 2 ) // 44.1k
			BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K;
		else
			BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_48K;
			break;
	}
}
#endif
/*******************************************/
/*
* if don't care current in idle, then modify to close analog DAC before sleep mode.
* this may avoid 'pop',when voice/audio source switched.
*/

#if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 1)
static boolean s_dig_dac_closed = FALSE;
void BK3000_dig_dac_close(void)
{
    if(!s_dig_dac_closed)
    {
        if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
        {
            BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_DAC_ENABLE | 1 << sft_AUD_ADC_ENABLE);
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        }
        else
        {
            BK3000_AUD_AUDIO_CONF_BAK &= ~(1 << sft_AUD_DAC_ENABLE | 1 << sft_AUD_ADC_ENABLE);
            BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
            BK3000_PMU_PERI_PWDS |= (bit_PMU_AUD_PWD);
        }
        s_dig_dac_closed =  TRUE;
    }

}

void aud_ana_dac_close_in_idle(void)
{
    if(!bt_flag1_is_set(APP_AUDIO_WORKING_FLAG_SET) && (1 == get_system_idle_cnt())
        && !app_get_mic_state())//change by cai.zhong 20190226 for smartvoice mic state
    {
        set_system_idle_cnt(0);
        BK3000_Ana_Dac_enable(0);
        BK3000_dig_dac_close();
    }
}
#endif

#ifdef BT_DUALMODE
uint16 RAM_CODE aud_get_buffer_size(void)
#else
uint16 DRAM_CODE aud_get_buffer_size(void)
#endif
{
    return rb_get_buffer_size( &audio_ctrl_blk.aud_rb );
}

uint16 RAM_CODE pcm_get_buffer_size(void)
{
    return rb_get_buffer_size( &pcm_ctrl_blk.aud_rb );
}
int aud_initial(uint32 freq, uint32 channels, uint32 bits_per_sample)
{
    aud_PAmute_oper(1);
    os_delay_ms(10);
#if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 1)
    s_dig_dac_closed = FALSE;
#endif
#if (ADJUST_AEC_DELAY == 1)
    s_audio_open_stat = 0;
#endif
#ifdef CONFIG_DRIVER_DAC
    if( dac_initial( freq, bits_per_sample ) )
        return -1;
#endif
#ifdef CONFIG_DRIVER_I2S
    i2s_cfg( freq, bits_per_sample );
#endif

#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    aud_dma_initial();
#endif
    rb_init( &audio_ctrl_blk.aud_rb, (uint8 *)audio_ctrl_blk.data_buff,1,AUDIO_BUFF_LEN );
    audio_ctrl_blk.channels = channels;
    
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if(bt_flag1_is_set( APP_FLAG_LINEIN ))
    {
        audio_ctrl_blk.channels = 1;
    //aud_fill_buffer((uint8 *)audio_ctrl_blk.data_buff, (AUDIO_BUFF_LEN>>2));
    }
#endif
    
#ifdef ADJUST_AEC_DELAY
    if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
        aud_fill_buffer((uint8 *)audio_ctrl_blk.data_buff, (AUDIO_BUFF_LEN>>1));
    aec_set_sync_adjused_flag(0);
#else
    aud_fill_buffer((uint8 *)audio_ctrl_blk.data_buff, (AUDIO_BUFF_LEN >> 1));
#endif
    aud_mute_func_init(6, 9);
    os_printf("audio DAC init:%d,%d,%d\r\n",freq,channels,bits_per_sample);
//#if(CONFIG_AUD_FADE_IN_OUT == 1)
//    set_aud_fade_in_out_state(AUD_FADE_IN);
//#endif
    return(0);
}

int aud_adc_initial(uint32 freq, uint32 channels, uint32 bits_per_sample)
{
 #ifdef CONFIG_BLUETOOTH_HFP
    if( adc_initial( freq, bits_per_sample ) )
        return -1;
    rb_init( &pcm_ctrl_blk.aud_rb,(uint8 *)pcm_ctrl_blk.data_buff,0,PCM_BUFF_LEN);
    pcm_ctrl_blk.channels = channels;
    pcm_fill_buffer((uint8 *)pcm_ctrl_blk.data_buff, (PCM_BUFF_LEN>>1));

 #endif
    os_printf("audio ADC init:%d,%d,%d\r\n",freq,channels,bits_per_sample);
    return 0;
}

int8 aud_min_volume_get( void )
{
    //if( aud_mute_cfg.mute_outside )
    //    return -5;
    //else
        return 0;
}

void aud_mute_init( void )
{
    app_env_handle_t env_h = app_env_get_handle();

    if( env_h->env_cfg.used == 0x01 )
    {
        aud_mute_cfg.mute_pin = env_h->env_cfg.system_para.pamute_pin;
        aud_mute_cfg.mute_high_flag
            = ((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PAMUTE_HIGH) >> 6 );
        aud_mute_cfg.mute_status = 2;
        aud_mute_cfg.mute_outside =
            ((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PA_ENABLE) >> 5 );
        aud_mute_cfg.shade_flag =
            ((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_SHADE_OUT) >> 10 );

		aud_mute_cfg.auto_mute_flag =
            (!!(env_h->env_cfg.feature.feature_flag & APP_ENV_FEATURE_FLAG_FAST_MUTE));
        //if((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_DAC_DIFFER) >> 7 )
        //    aud_mute_cfg.mute_mask = 0x1E00;
        //else
        //{
        //    if((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_DAC_N) >> 8 )
        //        aud_mute_cfg.mute_mask = 0x1400;
        //    else
        //        aud_mute_cfg.mute_mask = 0xA00;
        //}
    }
    else
    {
        aud_mute_cfg.mute_pin       = PAMUTE_GPIO_PIN;
        aud_mute_cfg.mute_high_flag = 0;
        aud_mute_cfg.mute_status    = 2;
        aud_mute_cfg.mute_outside   = 0;
        aud_mute_cfg.shade_flag     = 0;
		aud_mute_cfg.auto_mute_flag = 0;
        //aud_mute_cfg.mute_mask      = 0xA00;
    }

    if( aud_mute_cfg.mute_outside )
    {
        gpio_config(aud_mute_cfg.mute_pin, 1);
        if (aud_mute_cfg.mute_high_flag)
            gpio_output(aud_mute_cfg.mute_pin, 1);
        else
            gpio_output(aud_mute_cfg.mute_pin, 0);
    }
}

#if 0
void aud_volume_mute_ana( uint8 enable )
{
    if( enable )
    {
        BK3000_A5_CONFIG |= 0x1E00;
    }
    else
    {
        //BK3000_A5_CONFIG &= ~aud_mute_cfg.mute_mask;
    }
    //os_printf("aud_mute_cfg.mute_mask : 0x%x\r\n",aud_mute_cfg.mute_mask);
}
#endif

void dac_init_buf(void)
{
    memset(&audio_ctrl_blk, 0, sizeof(AUDIO_CTRL_BLK));
    rb_init(&audio_ctrl_blk.aud_rb, (uint8 *)audio_ctrl_blk.data_buff,1, AUDIO_BUFF_LEN);
}

int aud_close(void)
{
#ifdef BEKEN_DEBUG
    int aud_empty_isr = audio_ctrl_blk.aud_empty_count;
    int pcm_empty_isr = pcm_ctrl_blk.aud_empty_count;
#endif

    aud_PAmute_oper(1);
    os_delay_ms(10);
#if(CONFIG_AUD_FADE_IN_OUT == 1)
    set_aud_fade_in_out_state(AUD_FADE_NONE);    
#endif
    flag_sbc_buffer_play = 0;
#if (ADJUST_AEC_DELAY == 1)
	s_audio_open_stat = 0;
#endif
#ifdef CONFIG_DRIVER_DAC
    /*#ifdef CONFIG_BLUETOOTH_HFP
	adc_close();
	#endif*/
    dac_close();
#endif

#ifdef CONFIG_DRIVER_I2S
    i2s_close();
#endif

#if (CONFIG_AUDIO_TRANSFER_DMA == 1)

#endif
	/*
    rb_init( &audio_ctrl_blk.aud_rb, (uint8 *)audio_ctrl_blk.data_buff,1,AUDIO_BUFF_LEN );
    rb_init( &pcm_ctrl_blk.aud_rb,(uint8 *)pcm_ctrl_blk.data_buff,0,PCM_BUFF_LEN);
	*/

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if(bt_flag1_is_set( APP_FLAG_LINEIN ))
    {
//        rb_init( &linein_data_blk.data_rb, (uint8 *)audio_ctrl_blk.data_buff, AUDIO_BUFF_LEN );
        rb_init( get_line_ringbuf(), get_linein_buffer_base(),0, get_linein_buffer_length());
    }
#endif

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if(bt_flag1_is_set( APP_FLAG_LINEIN ))
    {
//        rb_init( &linein_data_blk.data_rb, (uint8 *)audio_ctrl_blk.data_buff, AUDIO_BUFF_LEN );
        rb_init( get_line_ringbuf(), get_linein_buffer_base(),0, get_linein_buffer_length());
    }
#endif

#ifdef BEKEN_DEBUG
    audio_ctrl_blk.aud_empty_count = aud_empty_isr;
    audio_ctrl_blk.aud_empty_count = pcm_empty_isr;
#endif

    os_printf("audio close !!!\r\n");

    return(0);
}
/**************************************
static int16 sinwav[16] = {
        0,
    5.7925,
    8.1920,
    5.7925,
         0,
   -5.7925,
   -8.1920,
   -5.7925
};
static uint16 fifo_idx = 0;
void DRAM_CODE fill_dac_fifo_dc(uint32 l,uint32 r)
{
    uint32 sts = BK3000_AUD_FIFO_STATUS;
    if(sts&(bit_DACL_INTR_FLAG|bit_DACR_INTR_FLAG ))
    {
	    while(!(sts&(bit_DACL_FIFO_FULL|bit_DACR_FIFO_FULL)))
        {
            //os_printf("===l,r:%d,%d\r\n",l,r);
		    AUD_WRITE_DACL(sinwav[fifo_idx],sinwav[fifo_idx]);
            fifo_idx++;
            fifo_idx &= 0x07;
            sts = BK3000_AUD_FIFO_STATUS;
        }
    }
}
*********************************************/
#ifdef BT_DUALMODE
void RAM_CODE aud_isr(int mode_sel )
#else
void DRAM_CODE aud_isr(int mode_sel )
#endif
{
#ifdef CONFIG_DRIVER_DAC
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
#endif
    
    if( mode_sel == 0 )
    {
        #if (CONFIG_AUDIO_TRANSFER_DMA == 0)
        dac_isr( &audio_ctrl_blk );
        //fill_dac_fifo_dc();
        #endif
        
        #ifdef CONFIG_BLUETOOTH_HFP
        adc_isr();
        #endif
    }
#endif

#ifdef CONFIG_DRIVER_I2S
    if( mode_sel == 1 )
        i2s_isr( &audio_ctrl_blk );
#endif

}

#ifdef BT_DUALMODE
void RAM_CODE aud_fill_buffer( uint8 *buff, uint16 size )
#else
void DRAM_CODE aud_fill_buffer( uint8 *buff, uint16 size )
#endif
{
    rb_fill_buffer( &audio_ctrl_blk.aud_rb, buff, size, DAC_FILL_WITH_COMP);
}

//int app_aec_get_rx_size(void);
//int app_aec_get_tx_size(void);
#ifdef BT_DUALMODE
uint8 RAM_CODE aud_discard_sco_data(void)
#else
uint8 DRAM_CODE aud_discard_sco_data(void)
#endif
{
    if(bt_flag1_is_set( APP_FLAG_WAVE_PLAYING))
        return 1;
    else if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DISABLE_IOS_INCOMING_RING)
		&&bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
		&& !bt_flag1_is_set( APP_FLAG_CALL_ESTABLISHED))
    {
        return 1;
    }
    else
        return 0;
}

#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
#ifdef BT_DUALMODE
void RAM_CODE cvsd_fill_buffer(uint8 *buff, uint8 fid,uint16 size )
#else
void DRAM_CODE cvsd_fill_buffer(uint8 *buff, uint8 fid,uint16 size )
#endif
{
    uint16 i = 0;
    /* Don't use jmalloc/jfree function, it will produce many fragments */
    //uint8 *buff_st = jmalloc(size*2,M_ZERO);

    /* Now,max length of sco packet is 240 bytes,if extended to stereo,then length is 240*2, 
       it will take up 480 bytes space in stack.
    */
    uint8 buff_st[240*2];
    uint8 *src = (uint8 *)buff;
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
    uart_send_ppp(src,fid,size);
#endif
    while(i < size)
    {
        *(buff_st + 2*i) = *(src + i);
        *(buff_st + 2*i + 1) = *(src + i + 1);
        *(buff_st + 2*i + 2) = *(src + i);
        *(buff_st + 2*i + 3) = *(src + i + 1);
        i += 2;
    }
    rb_fill_buffer( &audio_ctrl_blk.aud_rb, buff_st, size*2, DAC_FILL_WITH_COMP);
#ifdef ADJUST_AEC_DELAY
    if( aud_get_buffer_size() < (AUDIO_BUFF_LEN - 256*2) )
    {
        aud_open();
        aud_PAmute_oper(0);
    }
#endif
    //jfree(buff_st);
    //buff_st = NULL;
}
#endif

#ifdef BT_DUALMODE
void RAM_CODE pcm_fill_buffer( uint8 *buff, uint16 size )
#else
void DRAM_CODE pcm_fill_buffer( uint8 *buff, uint16 size )
#endif
{
    rb_fill_buffer( &pcm_ctrl_blk.aud_rb, buff, size, ADC_FILL);
}

// add begin by cai.zhong 20190227 for get pcm ring buffer
driver_ringbuff_t *get_pcm_ringbuf(void)
{
    return &pcm_ctrl_blk.aud_rb;
}

uint8 *get_pcm_buffer_base(void)
{
    return pcm_ctrl_blk.data_buff;
}
// add end by cai.zhong 20190227 for get pcm ring buffer

#if (CONFIG_APP_MP3PLAYER == 1)
void RAM_CODE wav_fill_buffer( uint8 *buff, uint16 size )
{
    int32 tmp;
    int16 i;
    int16 fmtTag;
    int16 bits;
    int16 nSmpls;
    uint8 *buff_st = NULL;
    uint8 *tmp_buf=NULL;

    fmtTag = wav_info.fmtTag;
    bits = wav_info.bits;
    switch(bits)
    {
        case 8:
            nSmpls = size;
            if(fmtTag == 0x06)//a-law
            {
                for(i=0;i<nSmpls;i++)
                {
                    aulawsmpl[i] = alawtbl[buff[i]^0x80];
                    tmp = aulawsmpl[i] << 3;
                    aulawsmpl[i]= f_sat(tmp);
                }
            }
            else if(fmtTag == 0x07) //u-law
            {
                for(i=0;i<nSmpls;i++)
                {
                    aulawsmpl[i] = ulawtbl[buff[i]];
                    tmp = aulawsmpl[i] << 2;
                    aulawsmpl[i]= f_sat(tmp);
                    //aulawsmpl[i] <<= 2;
                }

            }
            else//linear
            {
                for(i=0;i<nSmpls;i++)
                {
                    //tmp = (buff[i]-127)<<8;
                    //aulawsmpl[i]= f_sat(tmp);
                    aulawsmpl[i] = ((short)(buff[i]-128))<<8;
                }
            }
            break;
        case 16:
            nSmpls = size>>1;
            for(i=0;i<nSmpls;i++)
            {
                aulawsmpl[i] = (buff[2*i])|(buff[2*i+1]<<8);
            }
            break;
        case 24://Not Support!!!!!
            nSmpls = size / 3;
            for(i=0;i<nSmpls;i++)
            {
                aulawsmpl[i] = (buff[3*i+1])|(buff[3*i+2]<<8);
            }
            break;
        case 32:
            nSmpls = size>>2;
            for(i=0;i<nSmpls;i++)
            {
                aulawsmpl[i] = (buff[4*i+2])|(buff[4*i+3]<<8);
            }
            break;
        default:
            nSmpls = size>>1;
            for(i=0;i<nSmpls;i++)
            {
                aulawsmpl[i] = (buff[2*i])|(buff[2*i+1]<<8);
            }
            break;
    }

    if(wav_info.ch == 1)
    {
        uint16 i = 0;
        nSmpls = nSmpls << 1;
        buff_st = jmalloc( (nSmpls << 1),0);
        uint8 *src = (uint8 *)&aulawsmpl[0];

        if(buff_st == NULL)
        {
            os_printf("wavbuff_st jmalloc fail!!!\r\n");
            return;
        }
        while(i < nSmpls)
        {
            *(buff_st + 2*i) = *(src + i);
            *(buff_st + 2*i + 1) = *(src + i + 1);
            *(buff_st + 2*i + 2) = *(src + i);
            *(buff_st + 2*i + 3) = *(src + i + 1);
            i += 2;
        }
    }

    if(buff_st == NULL)
        tmp_buf = (uint8 *)&aulawsmpl[0];
    else
        tmp_buf = buff_st;

    rb_fill_buffer( &audio_ctrl_blk.aud_rb, tmp_buf, nSmpls<<1, 4 );
    if(buff_st)
        jfree(buff_st);

}

#endif
void aud_open( void )
{
#if (ADJUST_AEC_DELAY == 1)
	if(s_audio_open_stat)
		return;
#endif
aud_PAmute_oper(1);
os_delay_ms(10);
#ifdef CONFIG_DRIVER_DAC
    dac_open();
    aud_volume_mute(0);
#endif
#ifdef CONFIG_DRIVER_I2S
    i2s_open(&audio_ctrl_blk);
#endif
    /*
    * Change audio source to bt audio after delaying 800ms;
    * Avoid for uncertain noise after DAC interrupt open;
    */
#if (ADJUST_AEC_DELAY == 1)
	s_audio_open_stat = 1;
#endif

}

void aud_volume_mute( uint8 enable )
{

}

void aud_volume_set( int8 volume )
{
#ifdef CONFIG_DRIVER_DAC
    dac_volume_set(volume);
#endif

#ifdef CONFIG_DRIVER_I2S
    i2s_volume_adjust(volume + 19);
#endif
}




static void pa_mute(void)
{
    if(aud_mute_cfg.mute_outside == 1)
    {
        if(aud_mute_cfg.mute_high_flag)  // bit7 = 1: high effect
            gpio_output(aud_mute_cfg.mute_pin, 1);
        else
            gpio_output(aud_mute_cfg.mute_pin, 0);

        aud_mute_cfg.mute_status = PA_MUTE_STATUS;
        os_printf("pa mute:%d\r\n",aud_mute_cfg.mute_pin);		
    }
}
extern uint8 mutePa;
static void pa_unmute(void)
{
    if(aud_mute_cfg.mute_outside == 1)
    {
        if( aud_mute_cfg.mute_high_flag )  // bit7 = 1: high effect
            gpio_output(aud_mute_cfg.mute_pin, 0);
        else
            gpio_output(aud_mute_cfg.mute_pin, 1);

        aud_mute_cfg.mute_status = PA_UNMUTE_STATUS;
        os_printf("pa unmute:%d\r\n",aud_mute_cfg.mute_pin);
    }
}

/*  1 -- direct PA mute  0 -- delay PA unmute */
void aud_PAmute_oper(int enable)
{
    app_env_handle_t  env_h = app_env_get_handle();
    if(1 == enable )
    {
        pa_delay_unmute = env_h->env_cfg.feature.pa_unmute_delay_time;
        if(pa_delay_mute)
        {
          //  os_printf("****pa_delay_mute=%d\r\n",pa_delay_mute);	
            pa_delay_mute--;
            return;
        }
        if(aud_mute_cfg.mute_status != PA_MUTE_STATUS)
            pa_mute();
    }
    else if(2 == enable)
    {
        pa_delay_mute  = env_h->env_cfg.feature.pa_mute_delay_time;
        if(pa_delay_unmute)
        {
          //  os_printf("****pa_delay_unmute=%d\r\n",pa_delay_unmute);	
            pa_delay_unmute--;
            return;
        }
        if(bt_flag1_is_set(APP_FLAG_POWERDOWN))
            return;
        if((aud_mute_cfg.mute_status != PA_UNMUTE_STATUS))
         pa_unmute();
    }
    return;
}
extern volatile uint8 linein_audio_flag;
void aud_clr_PAmute(void)
{
    uint32 mode;
    mode = get_app_mode();
    if(mutePa)
    {
        aud_PAmute_oper(1);
    }
    else if(app_wave_playing())
    {
        aud_PAmute_oper(2);
    }
    else if(SYS_WM_BT_MODE == mode)
    {
        if( bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
            ||((bt_flag1_is_set(APP_FLAG_HFP_OUTGOING|APP_FLAG_SCO_CONNECTION|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_HFP_CALLSETUP)))
            ||(a2dp_has_music()&&(player_vol_bt>0)&&(!bt_flag2_is_set(APP_FLAG2_MUTE_FUNC_MUTE))))
        {
            aud_PAmute_oper(2);
        }
        else
        {
            aud_PAmute_oper(1);
        }
    }
#if (CONFIG_APP_MP3PLAYER == 1)
    else if((SYS_WM_SD_MUSIC_MODE == mode)
        #ifdef CONFIG_APP_USB 
            ||(SYS_WM_UDISK_MUSIC_MODE == mode)
        #endif
        )
    {
        if(player_get_play_status()&&player_vol_bt)
            aud_PAmute_oper(2);
    #ifdef CONFIG_BLUETOOTH_COEXIST
        else if( bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
            ||((bt_flag1_is_set(APP_FLAG_HFP_OUTGOING|APP_FLAG_SCO_CONNECTION|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_HFP_CALLSETUP))))
        {
            aud_PAmute_oper(2);
        }
    #endif
        else
            aud_PAmute_oper(1);
    }
    else if(SYS_WM_FM_MODE == mode)
    {

    }
#endif
    else if(SYS_WM_LINEIN_MODE == mode)
    {
        if(linein_get_play_status()&&player_vol_bt&&linein_audio_flag)
            aud_PAmute_oper(2);
    #ifdef CONFIG_BLUETOOTH_COEXIST
        else if( bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
            ||((bt_flag1_is_set(APP_FLAG_HFP_OUTGOING|APP_FLAG_SCO_CONNECTION|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_HFP_CALLSETUP))))
        {
            aud_PAmute_oper(2);
        }
    #endif
        else
            aud_PAmute_oper(1);
    }


}
/*
* if don't care current in idle, then modify to close analog DAC before sleep mode.
* this may avoid 'pop',when voice/audio source switched.
*/



void aud_mic_open( enable )
{
    //os_printf("aud_mic_open. %d  %d  %d \r\n", BK3000_AUD_AUDIO_CONF_BAK, enable, (BK3000_AUD_AUDIO_CONF_BAK & (1 << sft_AUD_ADC_ENABLE)));
#ifdef CONFIG_DRIVER_ADC
    sdadc_enable(enable);
#endif
#ifdef CONFIG_DRIVER_I2S
    i2s_rx_enable(enable);
#endif
}

void aud_mic_volume_set( uint8 volume )
{
#ifdef CONFIG_DRIVER_ADC
    sdadc_volume_adjust(volume);
#endif
#ifdef CONFIG_DRIVER_I2S
    i2s_rx_volume_adjust(volume);
#endif
}

void aud_mic_mute( uint8 enable )
{
    //os_printf("aud_mic_mute. %d  %d  %d \r\n", BK3000_AUD_AUDIO_CONF_BAK, enable, (BK3000_AUD_AUDIO_CONF_BAK & (1 << sft_AUD_ADC_ENABLE)));
#ifdef CONFIG_DRIVER_ADC
    if(enable)
    {
        BK3000_AUD_AUDIO_CONF_BAK &= ~(1 << sft_AUD_ADC_ENABLE);
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    }
    else
    {
        BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_ADC_ENABLE);
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    }
#endif
}

void aud_filt_enable( uint8 index, uint8 enable )
{
    if( index < 4 )
    {
        BK3000_AUD_AUDIO_CONF2 &= ~( 1 << 2 );
        BK3000_AUD_AUDIO_CONF2 |= ( enable << 2 );
    }
    else
    {
        BK3000_AUD_DAC_CONF0 &= ~( 1 << sft_AUD_DAC_HPF2_BYPASS );
        BK3000_AUD_DAC_CONF0 |= ( ( 1 - enable) << sft_AUD_DAC_HPF2_BYPASS );
    }
}

#if 0
static __inline int16 f_inv( int din )
{
    int16 out = f_sat( din );
    int tempout;

    if( out == 0 )
        tempout = out;
    else
        tempout = 0x10000 - out;

    return (int16)tempout;
}
#endif
static __inline int f_sat22(int din)
{
    if (din>((1<<21)-1))
        return ((1<<21)-1);
    if (din<-((1<<21)-1))
        return -((1<<21)-1);
    else
        return(din);
}

static __inline int f_inv22( int din )
{
    int out = f_sat22( din );
    int tempout;

    if( out == 0 )
        tempout = out;
    else
        tempout = (1<<22) - out;

    return (int)tempout;
}

int aud_filt_conf( uint8 index, app_eq_para_t *conf )
{
    volatile uint32 *a1a2ptr;
    volatile uint32 *b0b1ptr;
    volatile uint32 *b2ptr;
    volatile uint32 *lbitptr;

    if( index < 0 || index >= AUDIO_MAX_FILT_NUM )
        return -1;

    if( index < 4 )
    {
        a1a2ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_A1_A2 + index*12);
        b0b1ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_B0_B1 + index*12);
        b2ptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_B2 + index*12);
        lbitptr = (volatile uint32 *)((uint32)&BK3000_AUD_FLT0_LBIT + index*4);

        //os_printf("%x %x %x \r\n", a1a2ptr, b0b1ptr, b2ptr );
        //os_printf("%x %x %x %x %x\r\n", f_inv(conf->a[0]), f_inv(conf->a[1]),
        //f_sat(conf->b[0]), f_sat(conf->b[1]), f_sat(conf->b[2]));

        *a1a2ptr = (((f_inv22(conf->a[1])>>6)& 0xFFFF) << 16)|((f_inv22(conf->a[0])>>6)& 0xFFFF );
        *b0b1ptr = (((f_sat22(conf->b[1])>>6)& 0xFFFF) << 16)|((f_sat22(conf->b[0])>>6)& 0xFFFF );
        *b2ptr = (f_sat22(conf->b[2])>>6)&0xFFFF;
        *lbitptr = ((f_inv22(conf->a[0])&0x3F)|((f_inv22(conf->a[1])&0x3F)<<6)
			|((f_sat22(conf->b[0])&0x3F)<<12)|((f_sat22(conf->b[1])&0x3F)<<18)|((f_sat22(conf->b[2])&0x3F)<<24));
    }
    else
    {
        a1a2ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF2);
        b0b1ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF1);
        b2ptr = (volatile uint32 *)(&BK3000_AUD_DAC_CONF0);
        lbitptr = (volatile uint32 *)(&BK3000_AUD_HPF2_LBIT);

        //os_printf("%x %x %x \r\n", a1a2ptr, b0b1ptr, b2ptr );
        //os_printf("%x %x %x %x %x\r\n", f_inv(conf->a[0]), f_inv(conf->a[1]),
        //f_sat(conf->b[0]), f_sat(conf->b[1]), f_sat(conf->b[2]));

        *a1a2ptr = (((f_inv22(conf->a[1])>>6)& 0xFFFF) << 16)|((f_inv22(conf->a[0])>>6)& 0xFFFF );
        *b0b1ptr = (((f_sat22(conf->b[1])>>6)& 0xFFFF) << 16)|((f_sat22(conf->b[0])>>6)& 0xFFFF );
        *b2ptr &= ~0xFFFF;
        *b2ptr |= (f_sat22(conf->b[2])>>6)&0xFFFF;
        *lbitptr = ((f_inv22(conf->a[0])&0x3F)|((f_inv22(conf->a[1])&0x3F)<<6)
			|((f_sat22(conf->b[0])&0x3F)<<12)|((f_sat22(conf->b[1])&0x3F)<<18)|((f_sat22(conf->b[2])&0x3F)<<24));
    }

    return 0;
}
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
void RAM_CODE aud_linein_fill_buffer( uint8 *buff, uint16 size )
{
    if(!bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)||
	!bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
        return;

//    rb_fill_buffer( &linein_data_blk.data_rb, buff, size, LINEIN_FILL);
    rb_fill_buffer( get_line_ringbuf(), buff, size, LINEIN_FILL);
#if 0
    //driver_ringbuff_t *rb;
    driver_ringbuff_t *rb_1,*rb_2;
    rb_fill_buffer( &audio_ctrl_blk.aud_rb, buff, size, LINEIN_FILL);

    rb_1 = (driver_ringbuff_t *)&audio_ctrl_blk.aud_rb;
    rb_2 = (driver_ringbuff_t *)&linein_data_blk.data_rb;
    rb_2->wptr = rb_1->wptr ;
    rb_2->buffer_fill = rb_1->buffer_fill;

//    linein_data.linein_encode_unit_index += size;
//    if(linein_data.linein_encode_unit_index >= SBC_RAW_UINT_LENGTH + 128)
// 128:size*4
//    {
//        linein_data.linein_encode_unit_index -= SBC_RAW_UINT_LENGTH;
//        flag_sbc_encode_allowed = 1;
//    }
#endif
    return;

}

driver_ringbuff_t *get_line_ringbuf(void)
{
    return &linein_data_blk.data_rb;
}
void line_in_fill_aud_buf(uint8 *buff, uint16 size )
{
#if 0
	uint16 tmp[size/4];
	uint16 *ptr;
//	if(aud_get_buffer_size()<size/2)
//	{
//		sync_data.u.sync_send.aud_num += size/4;
//		return;
//	}

	ptr = (uint16 *)buff;
	int i;
	for(i=0;i<size/4;i++)
	{
		tmp[i] = ptr[2*i+0];
	}
	aud_fill_buffer((uint8 *)tmp,size/2);
#else
	aud_fill_buffer(buff,size/2);
#endif
}


#endif

#ifdef CONFIG_APP_AEC
#define CON_AUD_BUF_UP_LMT (Mic_Delay_Size*2-4)
#define CON_AUD_BUF_DN_LMT (256)

RAM_CODE void app_aec_adj_latency(int rx_size,int tx_size)
{
    static int aec_cnt=0;
    if(++aec_cnt>500)
    {
        uint32 aud_data_size,last_latency,now_latecy,delta;
        aec_cnt = 0;
		int16 adj_step;

    #if(CONFIG_AUDIO_TRANSFER_DMA == 1)
        aud_data_size = reg_DMA_CH0_SRC_ADDR;
        if(aud_data_size<reg_DMA_CHO_SRC_CUR_PTR)
            aud_data_size += AUDIO_BUFF_LEN;
        aud_data_size -= reg_DMA_CHO_SRC_CUR_PTR;
        aud_data_size = aud_data_size>>1;
    #else
        aud_data_size = AUDIO_BUFF_LEN - aud_get_buffer_size();
    #endif

		if(aud_data_size>CON_AUD_BUF_UP_LMT)
		{
		//too many aud_buf data
		adj_step = CON_AUD_BUF_DN_LMT-CON_AUD_BUF_UP_LMT;
		if((aud_data_size+adj_step)<rx_size)
			adj_step = rx_size - aud_data_size +4;
		adj_step &= 0xfffe;
		os_printf("111aud:%d,rx:%d,step:%d\r\n",aud_data_size,rx_size,adj_step);
		rb_adj_dummy_data(&audio_ctrl_blk.aud_rb ,(int16)adj_step);
		return;
		}
		if(aud_data_size < rx_size)
		{
			adj_step = rx_size- aud_data_size+CON_AUD_BUF_DN_LMT;
			adj_step &= 0xfffe;
			uint8 tmp[16];
			memset(tmp, 0 , 16);
			for(;adj_step>16;adj_step-=16)
				rb_fill_buffer( &audio_ctrl_blk.aud_rb, tmp, 16, 0 );

//			rb_adj_dummy_data(&audio_ctrl_blk.aud_rb ,(int16)adj_step);
			os_printf("222aud:%d,rx:%d,step:%d\r\n",aud_data_size,rx_size,adj_step);

			return;

		}
        now_latecy = (aud_data_size - rx_size + tx_size)>>1;
        last_latency = aec_get_latency();
        if(last_latency>now_latecy) 
            delta= last_latency -now_latecy;
        else 
            delta = now_latecy - last_latency;

        if(delta > 100)
        {
            aec_set_latency(now_latecy);
            //os_printf("a:%d,r:%d,t:%d,l:%d,n:%d\r\n",aud_data_size,rx_size,tx_size,last_latency,now_latecy);
        }
    }
}
#endif

void audio_dac_analog_init(uint32_t type)
{
    uint32_t reg;
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 flag_L_is_LplusR = !!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_L_is_LplusR);
    os_printf("【AUDIO】：%s(%d),LplusR:%d\r\n", __func__, type,flag_L_is_LplusR);

    // 1. 打开audio DAC的数字电路部分
    //DO NOTHING

    // 2. 打开PGA
    //A6<1>: DAC DWA switch, 0:enable, 1:disable
    reg = BK3000_A6_CONFIG & ~((1 << 0) | (1 << 1) | (0x1f << 7) | (1 << 12));
    reg |= (0x10 << 7) | (1 << 12) | (1 << 27);
    BK3000_A6_CONFIG = reg;

    // 3. 然后打开校准DAC
    //如果是单端模式，则需要将reg35<8:7> enpaonl和enpaonr中的一个关闭，(需要和校准单端模式时关闭的选择相同), 0是关闭，1是打开。
    //如果是音箱的单端模式，只有audio输出的正端被输出给片外PA的，则需要将这两个enpaonl和enpaonr都置成0
    //如果是差分模式，则保持enpaonl和enpaonr都为1
    //(type & 0x1) == 0 ? Differential : Single-End
    reg = (BK3000_A5_CONFIG & ~(1 << 7)) | (((type & 0x1) == 0) << 7) | (1 << 8);
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
    if(type &&flag_L_is_LplusR)
        reg &= ~(1 << 8);
#elif (CONFIG_EXT_AUDIO_PA_EN == 1)
    reg &= ~((1 << 7) | (1 << 8));
    #if(CONFIG_EXT_PA_DIFF_EN == 0)
        reg |= (1 << 8);   // open L-pa 20171111 @jiazhou
    #else
        reg |= (((type & 0x1) == 0) << 8);
    #endif
#endif
    BK3000_A5_CONFIG = reg;

    //将校准时对应模式和对应增益(模拟增益)的校准值分别填入reg35<31:24>dcocdr<7:0> 和reg35<23:16>dcocdl<7:0>, dcocdr表示右声道校准值，dcocdl表示左声道的校准值。
    //TODO

    reg &= ~((1 << 6) | (3 << 10));
    reg |= ((1 << 12) | (1 << 13) | (3 << 14));
    BK3000_A5_CONFIG = reg;

    //设置gpio_reg37<31> audiodac_diffen, 1表示differential, 0表示single end。根据需求设置该寄存器（确保和校准时的配置一样）
    reg  = BK3000_A7_CONFIG & ~((1 << 3) | (3 << 4) | (1 << 8) | (3 << 9) | (1 << 11) | (0x3F << 15) | (1 << 26) | (1 << 31));
    reg |= (1 << 0) | (1 << 2) | (3 << 4) | (1 << 6) | (0 << 7) | (1 << 9) | (1 << 13) | (1 << 14) | (CONFIG_AUDIO_DAC_RAMP_EN << 26) | ((!(type & 0x1)) << 31);
    BK3000_A7_CONFIG = reg;

#if 0 // to void  pop noise when config dac always open,and first open it.  xugaojing 20180103
    if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON))
    {
        BK3000_A7_CONFIG |= (1 << 29) | (1 << 30);
        os_delay_us(2000);
        BK3000_A7_CONFIG |= (1 << 24);
        os_delay_us(2000);
        BK3000_A7_CONFIG |= (1 << 23);
        os_delay_us(2000);
	 BK3000_A7_CONFIG |= (1 << 21);

	 BK3000_A7_CONFIG |= (1 << 25);

    }
#endif
}

void audio_dac_analog_gain_set(uint32_t gain, uint32_t dc_offset_l, uint32_t dc_offset_r)
{
    uint32_t reg;

	//os_printf("【AUDIO】：%s(%d, %d, %d)\r\n", __func__, gain, dc_offset_l, dc_offset_r);
    reg = (BK3000_A5_CONFIG & ~(0xFFFF << 16)) | (((dc_offset_l & 0xFF) << 16) | ((dc_offset_r & 0xFF) << 24));
    BK3000_A5_CONFIG = reg;

    reg = (BK3000_A6_CONFIG & ~(0x1F << 2)) | ((gain & 0x1F) << 2);
    BK3000_A6_CONFIG = reg;
}

void audio_dac_analog_enable(uint32_t enable)
{
	static uint8 dac_alwaysopen_flag=0;
	if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_ALWAYS_ON) && (dac_alwaysopen_flag))
		return;
    //os_printf("【AUDIO】：%s(%d)\r\n", __func__, enable);

    //针对校准值为0x00或0xFF的芯片，修改ramp步长和次数
#if (CONFIG_AUDIO_DAC_RAMP_EN == 1)
    uint32_t reg;
    int32_t  i, ramp;
    i    = (BK3000_A5_CONFIG >> 16) & 0xFFFF;
    ramp = (i == 0x0000 || i == 0x00FF || i == 0xFF00 || i == 0xFFFF);
    reg  = BK3000_A7_CONFIG & ~(3 << 4);
    reg |= (3 - ramp) << 4;
    BK3000_A7_CONFIG = reg;
    ramp = 0x10 << ramp;
#endif

    if(enable)
    {
    #if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||(CONFIG_EXT_AUDIO_PA_EN == 0)
        if(!aud_mute_cfg.mute_outside)
        {
            BK3000_A5_CONFIG &= ~(1 << 12);
            os_delay_us(2000);
        }
    #endif
        BK3000_A7_CONFIG |= (1 << 29) | (1 << 30);
        os_delay_us(2000);
        BK3000_A7_CONFIG |= (1 << 24);
        os_delay_us(2000);
        BK3000_A7_CONFIG |= (1 << 23);
        os_delay_us(2000);
        BK3000_A7_CONFIG |= (1 << 21);

    #if (CONFIG_AUDIO_DAC_RAMP_EN == 1)
    {
        reg = BK3000_A7_CONFIG;
        for(i = 0; i <= ramp; i++)
        {
            reg &= ~(0x3F << 15);
            reg |= (i << 15);
            BK3000_A7_CONFIG = reg;
            os_delay_us(5000);
        }
        BK3000_A7_CONFIG |= (1 << 25);
    }
    #endif
		dac_alwaysopen_flag = 1;
    }
    else
    {
    #if (CONFIG_AUDIO_DAC_RAMP_EN == 1)
    {
        BK3000_A7_CONFIG &= ~(1 << 25);
        reg = BK3000_A7_CONFIG;
        for(i = ramp; i >= 0; i--)
        {
            reg &= ~(0x3F << 15);
            reg |= (i << 15);
            BK3000_A7_CONFIG = reg;
            os_delay_us(5000);
        }
    }
    #endif

        BK3000_A7_CONFIG &= ~(1 << 21);
        os_delay_us(2000);
        BK3000_A7_CONFIG &= ~(1 << 23);
        os_delay_us(2000);
        BK3000_A7_CONFIG &= ~(1 << 24);
        os_delay_us(2000);
        BK3000_A7_CONFIG &= ~((1 << 29) | (1 << 30));
    #if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||(CONFIG_EXT_AUDIO_PA_EN == 0)
        if(!aud_mute_cfg.mute_outside)
        {
            os_delay_us(2000);
            BK3000_A5_CONFIG |= (1 << 12);
        }
    #endif
    }
}
#ifdef CONFIG_APP_EQUANLIZER
void app_set_eq_gain(int8 gain)
{
	app_env_handle_t env_h = app_env_get_handle();
	env_h->env_cfg.eq_flag0 = 0xc5;
	env_h->env_cfg.eq_gain = (int16)gain;
	aud_volume_set((int8)16);
}
#if (CONFIG_PRE_EQ == 1)
aud_pre_equ_t aud_pre_eqe;
extern SAMPLE_ALIGN aud_pre_equ_para_t tbl_eq_coef[];
void app_set_pre_eq_gain(uint8 *para)
{
	uint16 tmp_gain;
	
	tmp_gain = para[0]|para[1]<<8|para[2]<<16|para[3]<<24;
	os_printf("%02x, %02x %x\r\n",para[0],para[1],tmp_gain);
	aud_pre_eqe.globle_gain = (uint32)tmp_gain ;
}

#define c_preEQ_cnt 10
void app_set_pre_eq(uint8 *para)
{
	int i;
	#if 1
	if(para[0]==0)
	{
		aud_pre_eqe.totle_EQ = 0;
	}
	else if(para[0]==c_preEQ_cnt)
		aud_pre_eqe.online_flag = 0x5a;
	else if (para[0]==c_preEQ_cnt+1)
		aud_pre_eqe.online_flag = 0;
	else if(para[0]==c_preEQ_cnt+2)
	{
		aud_pre_eqe.totle_EQ = 0;
		for(i=0;i<c_preEQ_cnt;i++)
		{
			tbl_eq_coef[i].a[0] = 0;
			tbl_eq_coef[i].a[1] = 0;
			tbl_eq_coef[i].b[0] = 0x100000;
			tbl_eq_coef[i].b[1] = 0;
			tbl_eq_coef[i].b[2] = 0;
		}
		aud_pre_eqe.online_flag = 0;
	}
	if(para[0]>=c_preEQ_cnt)
		return;

//	os_printf("bf:%d:%x,%x,%x,%x,%x\r\n",para[0],tbl_eq_coef[para[0]].a[0],
//		tbl_eq_coef[para[0]].a[1],
//		tbl_eq_coef[para[0]].b[0],
//		tbl_eq_coef[para[0]].b[1],
//		tbl_eq_coef[para[0]].b[2]);
	if(para[1]==0) //enable flag
	{
		tbl_eq_coef[para[0]].a[0] = 0;
		tbl_eq_coef[para[0]].a[1] = 0;
		tbl_eq_coef[para[0]].b[0] = 0x100000;
		tbl_eq_coef[para[0]].b[1] = 0;
		tbl_eq_coef[para[0]].b[2] = 0;
		return;//enable flag == 0
	}

	if(para[0]+1>aud_pre_eqe.totle_EQ)
		aud_pre_eqe.totle_EQ = para[0]+1;

	memcpy(&tbl_eq_coef[para[0]].a[0], &para[2], 4*5);
	aud_pre_eqe.online_flag = 0x5a;
	#endif

}

void app_show_pre_eq(void)
{
	int i;
	os_printf("flag:0x%x,cnt:%d,gain:%d\r\n",aud_pre_eqe.online_flag,aud_pre_eqe.totle_EQ,aud_pre_eqe.globle_gain);
	for(i=0;i<c_preEQ_cnt;i++)
		os_printf("i:%d\r\n,%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n",i,
		tbl_eq_coef[i].a[0],
		tbl_eq_coef[i].a[1],
		tbl_eq_coef[i].b[0],
		tbl_eq_coef[i].b[1],
		tbl_eq_coef[i].b[2]);
}

extern SAMPLE_ALIGN uint32 s_eq_last_XY[];
void app_init_pre_eq(void)
{
	memset((uint8 *)s_eq_last_XY, 0 , 4*4*c_preEQ_cnt);
}
#endif

#endif
#if (CONFIG_APP_MSBC_RESAMPLE == 1)
int16_t fir_coef[NFIR_BT] = {
12,-2,-15,-3,20,12,-26,-27,27,50,-21,-79,0,110,40,-135,-103,144,190,-123,
-296,56,415,75,-536,-303,648,697,-739,-1518,798,5137,7369,5137,798,-1518,
-739,697,648,-303,-536,75,415,56,-296,-123,190,144,-103,-135,40,110,0,
-79,-21,50,27,-27,-26,12,20,-3,-15,-2,12,
};
int16_t b_coef[N_IIR] = {
	125,822,2878,6868,12268,17137,19117,17137,12268,6868,2878,822,125
};
int16_t a_coef[N_IIR] = {
	4096,7223,14909,17593,19093,15553,10841,6022,2727,958,251,44,4
};

int16_t fir_hardcore_init(void)
{
    int16_t i;
    int16_t *coef = (int16_t *)&fir_coef[0];
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_FFT_PWD;
    BK3000_FFT_FFT_CONF = 0x00;
    BK3000_FFT_FIR_CONF = (NFIR_BT+1) | (1L<<sft_FIR_CONF_FIR_ENABLE);
    //BK3000_FFT_FIR_CONF = (CORE_NFIR) | (1L<<sft_FIR_CONF_FIR_MODE) | (1L<<sft_FIR_CONF_FIR_ENABLE);
    for(i=0;i<NFIR_BT;i++)
    {
        BK3000_FFT_COEF_PORT =coef[i];
    }
    return 0;

}
int16_t fir_hardcore_close(void)
{
    BK3000_FFT_FIR_CONF = 0x00;
    BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD;
    return 0;
}
int16_t fir_hardcore_filter(int16_t *smpl,uint16_t nSmpls)
{
	uint16_t i;
	int32_t tmp;
	for(i=0;i<nSmpls;i++) 
	{
        BK3000_FFT_DATA_PORT = smpl[i]; 
        BK3000_FFT_FFT_START = 0x01;
        while(!(BK3000_FFT_FFT_STATUS & bit_FFT_STATUS_FIR_DONE));
        tmp = BK3000_FFT_DATA_PORT;
        smpl[i] = (tmp & 0xffff);
	}
	return 0;
}
int16_t FIR_sw_filter_init(pFIR_BT_STATE p_fir_st)
{
    unsigned int i;
	p_fir_st->coef = &fir_coef[0];
	p_fir_st->taps = NFIR_BT;
	p_fir_st->curr_pos = NFIR_BT - 1;
	p_fir_st->ratio = 0;
	for(i=0;i<NFIR_BT;i++)
	{
		p_fir_st->laststate[i] = 0;
	}
	return 0;
}
static int16_t fir_sw_filter_sample(pFIR_BT_STATE pfir_stat,int16_t smpl)
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
    y >>= 14;
	if(y > 32767)  
	    y = 32767;
	else if(y < -32768) 
	    y = -32768;
    return  y;
}
int16_t FIR_sw_filter_exe(pFIR_BT_STATE p_fir_st,int16_t *in_smpl,int16_t *out_smpl,int16_t len)
{
    int16_t i;
    for(i=0;i<len;i++)
    {
        out_smpl[i] = fir_sw_filter_sample(p_fir_st,in_smpl[i]);
    }
    return 0;
}
void IIR_filter_init(t_IIR_State *st)
{
	int16_t i;
	st->a = (int16_t *)&a_coef[0];
	st->b = (int16_t *)&b_coef[0];
	for(i=0;i<N_IIR;i++)
	{
		st->y[i] = 0;
		st->x[i] = 0;
	}
}
void IIR_filter_exe(t_IIR_State *st,int16_t *x,int16_t *y,int16_t len)
{
	int16_t i,k;
	int32_t smpl_tmp;
	for(i=0; i<len; i++)
  {
   // Shift the register values.
	for(k=N_IIR-1; k>0; k--)st->x[k] = st->x[k-1];
    for(k=N_IIR-1; k>0; k--)st->y[k] = st->y[k-1];

   st->x[0] = x[i];
   smpl_tmp = st->b[0] * st->x[0];
   for(k=1; k<N_IIR; k++)
   {
     smpl_tmp += (st->b[k] * st->x[k] - st->a[k] * st->y[k]);
   }
   smpl_tmp >>= 12;
	if(smpl_tmp > 32767)
		smpl_tmp = 32767;
	else if(smpl_tmp < -32768)
		smpl_tmp = -32768;

   st->y[0] = smpl_tmp;
	// a[0] should = 1
   y[i] = st->y[0];
  }
}
#endif

