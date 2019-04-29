#include "driver_beken_includes.h"
#include "app_beken_includes.h"

#ifdef CONFIG_DRIVER_I2S

#define I2S_FIFO_DEPTH_QUARTER      0
#define I2S_FIFO_DEPTH_HALF            1
#define I2S_FIFO_DEPTH_THREE_QUAR   2

enum
{
    I2S_MODE_I2S = 0,
    I2S_MODE_LEFT_JUST = 1,
    I2S_MODE_RIGHT_JUST = 2,
    I2S_MODE_SHORT_SYNC = 4,
    I2S_MODE_LONG_SYNC = 5,
    I2S_MODE_NORMAL_2B_D = 6,
    I2S_MODE_DELAY_2B_D = 7
};

#define I2S_MST_MASTER 1
#define I2S_MST_SLAVE 0

#define I2S_SYS_CLK     24576000


void i2s_init( void )
{
    
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_PCM2_PWD;

    BK3000_I2S_CN = ( I2S_FIFO_DEPTH_QUARTER << sft_I2S_CN_TXFIFO0_LEVEL )
           | ( I2S_FIFO_DEPTH_HALF << sft_I2S_CN_RXFIFO_LEVEL );

    BK3000_I2S_MODE = ( 2 << sft_I2S_TXFIFO_CTL_TXFIFO_MODE )
            | ( 5 << sft_I2S_TXFIFO_CTL_DOWN_SMPRATIO );

    codec_init();

    return;
}


void i2s_cfg( uint32 freq, uint16 datawidth )
{
    uint32 bitratio;

    if( freq != 8000 && freq != 16000 && freq != 44100 && freq != 48000 )
        return;

    if( datawidth > 32 )
        return;

    bitratio = (( I2S_SYS_CLK / (freq*datawidth )/4) - 1) & 0xFF;

    BK3000_I2S_CTRL = ( 0 << sft_I2S_CTRL_MODESEL ) 
       | (( datawidth - 1 ) << sft_I2S_CTRL_DATALEN )
       | (( datawidth - 1 ) << sft_I2S_CTRL_SMPRATIO )
       | ( bitratio << sft_I2S_CTRL_BITRATIO )
       | ( I2S_MST_MASTER << sft_I2S_CTRL_MSTEN );

    codec_cfg(freq, datawidth);
    i2s_volume_mute(0);
    
}

void i2s_open( AUDIO_CTRL_BLK *ablk )
{
    BK3000_I2S_CTRL |= ( 1 << sft_I2S_CTRL_I2SEN );
    BK3000_I2S_CN |=  (( 1 << sft_I2S_CN_TXINT0_EN) |( 1 << sft_I2S_CN_TXUDF0_EN));

    return;
}

void i2s_close( void )
{
    BK3000_I2S_CN &=  ~(( 1 << sft_I2S_CN_TXINT0_EN )|(1 << sft_I2S_CN_TXUDF0_EN )
        |(1 << sft_I2S_CN_RXINT_EN) | ( 1 << sft_I2S_CN_RXOVF_EN));
    BK3000_I2S_CTRL &= ~( 1 << sft_I2S_CTRL_I2SEN );

    return;
}


void i2s_isr( AUDIO_CTRL_BLK *ablk )
{
    uint32 sts = BK3000_I2S_STAT;
    uint16 sample[32];
    uint16 len = 0;
    uint8 empty_flag = 0;
    int i;
    
    if( ablk && (sts& (bit_I2S_STAT_TXINT0|bit_I2S_STAT_TXUDF0))) 
    {
      if( ablk->channels == 2 )
      {
       
        len = rb_get_buffer_with_length( &ablk->aud_rb, (uint8 *)sample, 32*2);

        if( len == 0 )
            empty_flag = 1;
        else
        {
            for( i = 0; i < len/2; i+= 2 )
            {
                BK3000_I2S_DAT0 = sample[i];
                BK3000_I2S_DAT0 = sample[i+1];
            }
        }
      }
      else 
      {
        len = rb_get_buffer_with_length( &ablk->aud_rb, (uint8 *)sample, 4*2);

        if( len == 0 )
            empty_flag = 1;
        else
        {   
            int j;
            for( i = 0; i < len/2; i++ )
            {
                for( j = 0; j < 12; j++)
                    BK3000_I2S_DAT0 = sample[i];
            }
        }
      }
    }

    sts = BK3000_I2S_STAT;

    if( sts & (bit_I2S_STAT_RXINT|bit_I2S_STAT_RXOVF))
    {
        uint16 dummy;
        
        for( i = 0; i < 16; i++ )
        {
            sample[i] = BK3000_I2S_DAT0;
            dummy = BK3000_I2S_DAT0;
        }

#ifdef CONFIG_APP_AEC
        if( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP ) )
            app_aec_fill_txbuff( (uint8 *)&sample[0], 16*2 );
#endif
    }

    sts = BK3000_I2S_STAT;
    if( ablk )
    {
        if(( sts & bit_I2S_STAT_TXUDF0) || empty_flag )
        {
           ablk->empty_count++;
           if( ablk->empty_count >= AUDIO_EMPTY_COUNT_THRE )
           {
              os_printf("e");
              BK3000_I2S_CN &= ~((1 << sft_I2S_CN_TXINT0_EN )|(1 << sft_I2S_CN_TXUDF0_EN)); 
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
    }

    BK3000_I2S_STAT = sts;    
}

void i2s_rx_enable( uint8 enable )
{
    if( enable )
    {
        BK3000_I2S_CN |=  ( 1 << sft_I2S_CN_RXFIFO_CLR );
        BK3000_I2S_CN |=  ( 1 << sft_I2S_CN_RXINT_EN )|( 1 << sft_I2S_CN_RXOVF_EN );
    }
    else
        BK3000_I2S_CN &= ~(( 1 << sft_I2S_CN_RXINT_EN )|( 1 << sft_I2S_CN_RXOVF_EN ));

    return;
    
}

void i2s_volume_mute( uint8 enable )
{
    codec_volume_mute(enable);
}

void i2s_volume_adjust( uint8 volume )
{
    codec_volume_control(volume);
}

void i2s_rx_volume_adjust( uint8 volume )
{
    codec_adc_volume_adjust(volume);
}

#endif
