//#ifdef USE_DMA_

#include "driver_beken_includes.h"
#include "beken_external.h"
#define FFT_INT_MODE    0
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
void dma_initial(void)
{
       reg_DMA_GLOBAL_CONFIG = 0x00008000 ;	
       BK3000_PMU_PERI_PWDS &= ~bit_PMU_DMA_PWD;
       aud_dma_initial();
}

void start_dma_transfer(uint8 dma_channel, struct dma_struct *dma_config)
{     
    uint32 dma_control_word;
    
    if(dma_config == NULL)
        return;
    
    dma_control_word = ((dma_config->request_select & 0x07) <<16) + 
                                  ((dma_config->transfer_mode & 0x03) <<14) +
                                  ((dma_config->dst_addr_incr & 0x03) <<12) + 
                                  ((dma_config->src_addr_incr & 0x03) <<10) + 
                                  ((dma_config->dst_data_type & 0x03) <<8 ) + 
                                  ((dma_config->src_data_type & 0x03) <<6 ) + 
                                  ((dma_config->trigger_type & 0x01)  <<5 ) + 
                                  ((dma_config->interrupt_en & 0x01)  <<4 ) +
                                  ((dma_config->dma_enable & 0x01)  <<2 ) +
                                  ((dma_config->soft_request & 0x01) <<1 ) ;
       
        if(dma_channel==0x00)  // DMA 0 config
        { 
            if(dma_config->dma_enable)
            {
                reg_DMA_CH0_XFR_SIZE   = (uint32)dma_config->transfer_size   ;
                reg_DMA_CH0_SRC_ADDR   = (uint32)dma_config->src_address     ;
                reg_DMA_CH0_DST_ADDR   = (uint32)dma_config->dst_address     ;
                reg_DMA_CH0_SRC_TOP    = (uint32)dma_config->src_addr_top    ;
                reg_DMA_CH0_SRC_BOTTOM = (uint32)dma_config->src_addr_bottom ; 
                reg_DMA_CH0_DST_TOP    = (uint32)dma_config->dst_addr_top    ;
                reg_DMA_CH0_DST_BOTTOM = (uint32)dma_config->dst_addr_bottom ; 
            }
            reg_DMA_CH0_CONTROL    = (uint32)dma_control_word            ;
        }
        
        if(dma_channel==0x01) // DMA 1 config
        {
            if(dma_config->dma_enable)
            {
                reg_DMA_CH1_XFR_SIZE   = (uint32)dma_config->transfer_size   ;
                reg_DMA_CH1_SRC_ADDR   = (uint32)dma_config->src_address     ;
                reg_DMA_CH1_DST_ADDR   = (uint32)dma_config->dst_address     ;
                reg_DMA_CH1_SRC_TOP    = (uint32)dma_config->src_addr_top    ;
                reg_DMA_CH1_SRC_BOTTOM = (uint32)dma_config->src_addr_bottom ; 
                reg_DMA_CH1_DST_TOP    = (uint32)dma_config->dst_addr_top    ;
                reg_DMA_CH1_DST_BOTTOM = (uint32)dma_config->dst_addr_bottom ; 
            }
            reg_DMA_CH1_CONTROL    = (uint32)dma_control_word            ;
        }
        
        if(dma_channel==0x02) // DMA 2 config
        {
            if(dma_config->dma_enable)
            {
                reg_DMA_CH2_XFR_SIZE   = (uint32)dma_config->transfer_size   ;
                reg_DMA_CH2_SRC_ADDR   = (uint32)dma_config->src_address     ;
                reg_DMA_CH2_DST_ADDR   = (uint32)dma_config->dst_address     ;
                reg_DMA_CH2_SRC_TOP    = (uint32)dma_config->src_addr_top    ;
                reg_DMA_CH2_SRC_BOTTOM = (uint32)dma_config->src_addr_bottom ; 
                reg_DMA_CH2_DST_TOP    = (uint32)dma_config->dst_addr_top    ;
                reg_DMA_CH2_DST_BOTTOM = (uint32)dma_config->dst_addr_bottom ; 
            }
            reg_DMA_CH2_CONTROL    = (uint32)dma_control_word            ;
        }
        
        if(dma_channel==0x03) // DMA 3 config
        {
            if(dma_config->dma_enable)
            {
                reg_DMA_CH3_XFR_SIZE   = (uint32)dma_config->transfer_size   ;
                reg_DMA_CH3_SRC_ADDR   = (uint32)dma_config->src_address     ;
                reg_DMA_CH3_DST_ADDR   = (uint32)dma_config->dst_address     ;
                reg_DMA_CH3_SRC_TOP    = (uint32)dma_config->src_addr_top    ;
                reg_DMA_CH3_SRC_BOTTOM = (uint32)dma_config->src_addr_bottom ; 
                reg_DMA_CH3_DST_TOP    = (uint32)dma_config->dst_addr_top    ;
                reg_DMA_CH3_DST_BOTTOM = (uint32)dma_config->dst_addr_bottom ; 
            }
            reg_DMA_CH3_CONTROL    = (uint32)dma_control_word            ;
        }
        
        if(dma_channel==0x04) // DMA 4 Config
        {
            if(dma_config->dma_enable)
            {
                reg_DMA_CH4_XFR_SIZE   = (uint32)dma_config->transfer_size   ;
                reg_DMA_CH4_SRC_ADDR   = (uint32)dma_config->src_address     ;
                reg_DMA_CH4_DST_ADDR   = (uint32)dma_config->dst_address     ;
                reg_DMA_CH4_SRC_TOP    = (uint32)dma_config->src_addr_top    ;
                reg_DMA_CH4_SRC_BOTTOM = (uint32)dma_config->src_addr_bottom ; 
                reg_DMA_CH4_DST_TOP    = (uint32)dma_config->dst_addr_top    ;
                reg_DMA_CH4_DST_BOTTOM = (uint32)dma_config->dst_addr_bottom ; 
            }
            reg_DMA_CH4_CONTROL    = (uint32)dma_control_word            ;
        }
}
#endif


//#ifdef CONFIG_APP_AEC
typedef struct _driver_fft_s
{
    int16 *in;
    int16 *out;
    int32 *mac_out;
    volatile uint8 busy_flag;
    uint16 size;
}driver_fft_t;

static driver_fft_t driver_fft;

/* static int fft_dma_cb( void *arg ) */
/* { */
/*     Fft_Set_Start(); */
/*     return 0; */
/* } */

/* static int fft_out_cb( void *arg) */
/* { */
/*     driver_fft.busy_flag = 0; */
/*     BK3000_FFT_FFT_CONF = 0; */
/*     BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD; */
/*     return 0; */
/* } */

static __inline int16 f_sat(int din)
{
	if (din>32767)
		return 32767;
	if (din<-32768)
		return -32768;
      else
	    return(din);
}
#ifdef BT_DUALMODE
int  RAM_CODE fft_busy( void )
#else
int  DRAM_CODE fft_busy( void )
#endif
{
#if (FFT_INT_MODE == 1)
    return driver_fft.busy_flag;
#else
    uint32 int_status;
    do
    {
        int_status = BK3000_FFT_FFT_STATUS;
    }while(!(int_status & bit_FFT_STATUS_FFT_DONE));
    //fft done
    {
        int i;
        int bit_ext;
        int32 temp, temp_out;

        bit_ext = (BK3000_FFT_FFT_STATUS & 0x00001fff) >> 7;

        if( bit_ext & 0x20 )
            bit_ext = 64 - bit_ext;
        else
            bit_ext = 0;


    	for( i = 0; i < driver_fft.size; i++ )
    	{
    		temp              = BK3000_FFT_DATA_PORT;
            temp_out = ((temp << 16) >> 16) <<  bit_ext ;
        	driver_fft.out[i] = f_sat(temp_out);
            temp_out = (temp  >> 16) << bit_ext;
        	driver_fft.in[i]  = f_sat(temp_out);
    	}

        driver_fft.busy_flag = 0;
        BK3000_FFT_FFT_CONF = 0;
        BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD;
    }
    BK3000_FFT_FFT_STATUS = int_status;
    return 0;
#endif
}

#ifdef BT_DUALMODE
void RAM_CODE fft_enable( int mode, int16 *inbuf, int16* outbuf, uint16 size )
#else
void DRAM_CODE fft_enable( int mode, int16 *inbuf, int16* outbuf, uint16 size )
#endif
{
    int i;

    BK3000_PMU_PERI_PWDS &= ~bit_PMU_FFT_PWD;
    BK3000_FFT_FIR_CONF = 0;
    BK3000_FFT_FFT_CONF = 0x00;

    if( mode == FFT_MODE_FFT )
    {
        BK3000_FFT_FFT_CONF = (
                ( 1 << sft_FFT_CONF_FFT_ENABLE )
#if (FFT_INT_MODE == 1)
                |( 1 << sft_FFT_CONF_FFT_INT_EN ) 
#endif
                );
        
    }
    else
    {
        BK3000_FFT_FFT_CONF = ( 
            ( 1 << sft_FFT_CONF_FFT_ENABLE )
            | ( 1 << stf_FFT_CONF_IFFT)
#if (FFT_INT_MODE == 1)
            | (1 << sft_FFT_CONF_FFT_INT_EN ) 
#endif
            );
        
    }


    driver_fft.in  = inbuf;
    driver_fft.out = outbuf;
    driver_fft.size = size;
#if (FFT_INT_MODE == 1)
    driver_fft.busy_flag = 1;
#endif

    for( i = 0; i < size; i++ )
    {
    	uint32 temp = (uint16)inbuf[i] << 16 | (uint16)outbuf[i];
        BK3000_FFT_DATA_PORT = temp;
    }

    Fft_Set_Start();
}

#if 0
void fir_single_enable( driver_fir_conf_t *fir_conf, int16 *coef, int16* input, int32 *mac )
{
    int i;

    if( fir_conf->fir_len > 64 )
        return;

    BK3000_PMU_PERI_PWDS &= ~bit_PMU_FFT_PWD;
    BK3000_FFT_FIR_CONF = 0;
    BK3000_FFT_FFT_CONF = 0;

    BK3000_FFT_FIR_CONF = ( fir_conf->fir_len << sft_FIR_CONF_FIR_LEN ) 
        | ( fir_conf->fir_cwidth << sft_FIR_CONF_FIR_CWIDTH )
        | ( fir_conf->fir_dwidth << sft_FIR_CONF_FIR_DWIDTH )
        | ( 1 << sft_FIR_CONF_FIR_INT_EN )
        | ( 1 << sft_FIR_CONF_FIR_ENABLE );

    driver_fft.mac_out = mac;
    driver_fft.busy_flag = 1;

    for( i = 0; i < fir_conf->fir_len + 1; i++ )
    {
        BK3000_FFT_COEF_PORT = coef[i];
        BK3000_FFT_DATA_PORT = input[i];
    }

    Fft_Set_Start();

    return;
}
#endif
#if(FFT_INT_MODE == 0)
void fft_isr( void )
#else
void DRAM_CODE fft_isr( void )
#endif
{
    uint32 int_status;

    int_status = BK3000_FFT_FFT_STATUS;

    if( int_status & bit_FFT_STATUS_FFT_DONE )
    {
        int i;
        int bit_ext;
        int32 temp, temp_out;

        bit_ext = (BK3000_FFT_FFT_STATUS & 0x00001fff) >> 7;

        if( bit_ext & 0x20 )
            bit_ext = 64 - bit_ext;
        else
            bit_ext = 0;


    	for( i = 0; i < driver_fft.size; i++ )
    	{
    		temp              = BK3000_FFT_DATA_PORT;
            temp_out = ((temp << 16) >> 16) <<  bit_ext ;
        	driver_fft.out[i] = f_sat(temp_out);
            temp_out = (temp  >> 16) << bit_ext;
        	driver_fft.in[i]  = f_sat(temp_out);
    	}

        driver_fft.busy_flag = 0;
        BK3000_FFT_FFT_CONF = 0;
        BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD;
    }
    if( int_status & bit_FFT_STATUS_FIR_DONE )
    {
        *driver_fft.mac_out = BK3000_FFT_MAC_LOW;

        driver_fft.busy_flag = 0;
        BK3000_FFT_FIR_CONF = 0;
        BK3000_PMU_PERI_PWDS |= bit_PMU_FFT_PWD;
    }

    BK3000_FFT_FFT_STATUS = int_status;
}

#if 0
void fft_test( void )
{
    int16 test_sample[256] = {             
    	    0x7FFF ,
            0x7C76 ,
            0x720C ,
            0x6154 ,
            0x4B3C ,
            0x30FB ,
            0x1406 ,
            0xF5F5 ,
            0xD872 ,
            0xBD1F ,
            0xA57E ,
            0x92DE ,
            0x8645 ,
            0x8066 ,
            0x8194 ,
            0x89BF ,
            0x9873 ,
            0xACE0 ,
            0xC5E4 ,
            0xE21F ,
            0x0000 ,
            0x1DE1 ,
            0x3A1C ,
            0x5320 ,
            0x678D ,
            0x7641 ,
            0x7E6C ,
            0x7F9A ,
            0x79BB ,
            0x6D22 ,
            0x5A82 ,
            0x42E1 ,
            0x278E ,
            0x0A0B ,
            0xEBFA ,
            0xCF05 ,
            0xB4C4 ,
            0x9EAC ,
            0x8DF4 ,
            0x838A ,
            0x8001 ,
            0x838A ,
            0x8DF4 ,
            0x9EAC ,
            0xB4C4 ,
            0xCF05 ,
            0xEBFA ,
            0x0A0B ,
            0x278E ,
            0x42E1 ,
            0x5A82 ,
            0x6D22 ,
            0x79BB ,
            0x7F9A ,
            0x7E6C ,
            0x7641 ,
            0x678D ,
            0x5320 ,
            0x3A1C ,
            0x1DE1 ,
            0x0000 ,
            0xE21F ,
            0xC5E4 ,
            0xACE0 ,
            0x9873 ,
            0x89BF ,
            0x8194 ,
            0x8066 ,
            0x8645 ,
            0x92DE ,
            0xA57E ,
            0xBD1F ,
            0xD872 ,
            0xF5F5 ,
            0x1406 ,
            0x30FB ,
            0x4B3C ,
            0x6154 ,
            0x720C ,
            0x7C76 ,
            0x7FFF ,
            0x7C76 ,
            0x720C ,
            0x6154 ,
            0x4B3C ,
            0x30FB ,
            0x1406 ,
            0xF5F5 ,
            0xD872 ,
            0xBD1F ,
            0xA57E ,
            0x92DE ,
            0x8645 ,
            0x8066 ,
            0x8194 ,
            0x89BF ,
            0x9873 ,
            0xACE0 ,
            0xC5E4 ,
            0xE21F ,
            0x0000 ,
            0x1DE1 ,
            0x3A1C ,
            0x5320 ,
            0x678D ,
            0x7641 ,
            0x7E6C ,
            0x7F9A ,
            0x79BB ,
            0x6D22 ,
            0x5A82 ,
            0x42E1 ,
            0x278E ,
            0x0A0B ,
            0xEBFA ,
            0xCF05 ,
            0xB4C4 ,
            0x9EAC ,
            0x8DF4 ,
            0x838A ,
            0x8001 ,
            0x838A ,
            0x8DF4 ,
            0x9EAC ,
            0xB4C4 ,
            0xCF05 ,
            0xEBFA ,
            0x0A0B ,
            0x278E ,
            0x42E1 ,
            0x5A82 ,
            0x6D22 ,
            0x79BB ,
            0x7F9A ,
            0x7E6C ,
            0x7641 ,
            0x678D ,
            0x5320 ,
            0x3A1C ,
            0x1DE1 ,
            0x0000 ,
            0xE21F ,
            0xC5E4 ,
            0xACE0 ,
            0x9873 ,
            0x89BF ,
            0x8194 ,
            0x8066 ,
            0x8645 ,
            0x92DE ,
            0xA57E ,
            0xBD1F ,
            0xD872 ,
            0xF5F5 ,
            0x1406 ,
            0x30FB ,
            0x4B3C ,
            0x6154 ,
            0x720C ,
            0x7C76 ,
            0x7FFF ,
            0x7C76 ,
            0x720C ,
            0x6154 ,
            0x4B3C ,
            0x30FB ,
            0x1406 ,
            0xF5F5 ,
            0xD872 ,
            0xBD1F ,
            0xA57E ,
            0x92DE ,
            0x8645 ,
            0x8066 ,
            0x8194 ,
            0x89BF ,
            0x9873 ,
            0xACE0 ,
            0xC5E4 ,
            0xE21F ,
            0x0000 ,
            0x1DE1 ,
            0x3A1C ,
            0x5320 ,
            0x678D ,
            0x7641 ,
            0x7E6C ,
            0x7F9A ,
            0x79BB ,
            0x6D22 ,
            0x5A82 ,
            0x42E1 ,
            0x278E ,
            0x0A0B ,
            0xEBFA ,
            0xCF05 ,
            0xB4C4 ,
            0x9EAC ,
            0x8DF4 ,
            0x838A ,
            0x8001 ,
            0x838A ,
            0x8DF4 ,
            0x9EAC ,
            0xB4C4 ,
            0xCF05 ,
            0xEBFA ,
            0x0A0B ,
            0x278E ,
            0x42E1 ,
            0x5A82 ,
            0x6D22 ,
            0x79BB ,
            0x7F9A ,
            0x7E6C ,
            0x7641 ,
            0x678D ,
            0x5320 ,
            0x3A1C ,
            0x1DE1 ,
            0x0000 ,
            0xE21F ,
            0xC5E4 ,
            0xACE0 ,
            0x9873 ,
            0x89BF ,
            0x8194 ,
            0x8066 ,
            0x8645 ,
            0x92DE ,
            0xA57E ,
            0xBD1F ,
            0xD872 ,
            0xF5F5 ,
            0x1406 ,
            0x30FB ,
            0x4B3C ,
            0x6154 ,
            0x720C ,
            0x7C76 ,
            0x7FFF ,
            0x7C76 ,
            0x720C ,
            0x6154 ,
            0x4B3C ,
            0x30FB ,
            0x1406 ,
            0xF5F5 ,
            0xD872 ,
            0xBD1F ,
            0xA57E ,
            0x92DE ,
            0x8645 ,
            0x8066 ,
            0x8194 ,
            0x89BF
            } ;
    int16 fft_sample[256];
    int16 test_sample1[256];
    /* int16 coef[32]; */
    int i;
    /* driver_fir_conf_t fir_conf; */
    uint32 mac;

    memset( fft_sample, 0, 256*2);
    memcpy( test_sample1, test_sample, 256*2 );
#if 0
    os_printf("Fir test started\r\n");
    fir_conf.fir_len = 31;
    fir_conf.fir_cwidth = 0;
    fir_conf.fir_dwidth = 5;

    for( i = 0; i < 32; i++ )
        coef[i] = 1;
    
    fir_single_enable( &fir_conf, coef, test_sample, &mac );
    while( fft_busy());

    os_printf("mac: %d\r\n", mac );
#endif

    os_printf("FFT test started..\r\n");
    BK3000_GPIO_15_CONFIG = 0x02;
    fft_enable( FFT_MODE_FFT, test_sample, fft_sample, 256 );
    while( fft_busy() );
    BK3000_GPIO_15_CONFIG = 0x00;

    os_printf("FFT calc ended.\r\n");
    for( i = 0; i < 256; i++ )
    {
        os_printf("%04x\r\n", test_sample[i]);
    } 

    fft_enable( FFT_MODE_IFFT, test_sample, fft_sample, 256 );
    while( fft_busy() );

    os_printf("IFFT calc ended.\r\n");
    for( i = 0; i < 256; i++ )
    {
        os_printf("%04x\r\n", fft_sample[i]);
    } 
#if 0
    os_printf("myfft test started..\r\n");
    memset( fft_sample, 0, 256*2);
    my_fft(test_sample1, fft_sample, 8, 0 );
    os_printf("FFT calc ended.\r\n");
    for( i = 0; i < 256; i++ )
    {
        os_printf("%04x\r\n", fft_sample[i]);
    } 

    my_fft(fft_sample, test_sample1, 8, 1);

    os_printf("IFFT calc ended.\r\n");
    for( i = 0; i < 256; i++ )
    {
        os_printf("%04x\r\n", fft_sample[i]);
    } 
#endif
    (void)mac;
}
#endif
//#endif /* CONFIG_APP_AEC */
