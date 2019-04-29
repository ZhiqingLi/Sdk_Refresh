#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_DRIVER_ADC
void sdadc_init( void )
{   
    //os_printf("sdadc_init()\r\n");

    //BK3000_AUD_ADC_CONF0  = 0x00E93A22;
    BK3000_AUD_ADC_CONF0  = (58<<18)|(1<<16)|0x00003A22; //  QFN40:58     QFN48:8    <23-18>
    BK3000_AUD_ADC_CONF1  = 0x8BBF3A22;
    BK3000_AUD_ADC_CONF2  = 0xC9E6751C;
    BK3000_AUD_AGC_CONF0  = 0x4A019465;
    BK3000_AUD_AGC_CONF1  = 0x02016C01;
    BK3000_AUD_AGC_CONF2  = 0x0F020940;
    BK3000_AUD_FIFO_CONF &= ~(0x1F << sft_AUD_ADC_WR_THRE);
    BK3000_AUD_FIFO_CONF |=  (8 << sft_AUD_ADC_WR_THRE);

    //BK3000_A5_CONFIG |= 0x20000000;

    //sdadc_volume_adjust(60); // 36
}

void sdadc_enable( uint8 enable )
{
    if ((BK3000_AUD_AUDIO_CONF_BAK & (1 << sft_AUD_ADC_ENABLE)) && enable)
        return;
    //os_printf("sdadc_enable: 0x%x,0x%x,0x%x \r\n", BK3000_AUD_AUDIO_CONF_BAK, (BK3000_AUD_AUDIO_CONF_BAK & (1 << sft_AUD_ADC_ENABLE)),enable);

#ifdef CONFIG_BLUETOOTH_HFP
    BK3000_Ana_Adc_enable(enable);
#endif

    if(enable)
    {
        //BK3000_AUD_FIFO_CONF |=  (1 << sft_AUD_ADC_INT_EN);
        BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_ADC_ENABLE);
        BK3000_AUD_AUDIO_CONF_BAK |= (1 << sft_AUD_LINE_ENABLE);
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
        BK3000_AUD_FIFO_CONF |=  (1 << sft_AUD_ADC_INT_EN);
    }
    else
    {
        BK3000_AUD_FIFO_CONF &= ~(1 << sft_AUD_ADC_INT_EN);
        BK3000_AUD_AUDIO_CONF_BAK &= ~(1 << sft_AUD_ADC_ENABLE);
        BK3000_AUD_AUDIO_CONF_BAK &= ~(1 << sft_AUD_LINE_ENABLE);
        BK3000_AUD_AUDIO_CONF = BK3000_AUD_AUDIO_CONF_BAK;
    }
}


// direct volume adjust, not linear
void sdadc_volume_adjust( uint8 volume )
{  
    int high, low;

    if(volume > SDADC_VOLUME_MAX)
        volume = SDADC_VOLUME_MAX;

	high = volume & 0x70;
	low = volume & 0x0f;
	if(low >0x0c)
	low = 0x0c;
	volume = high | low;


	BK3000_AUD_AGC_CONF2 &= ~(0x7F << sft_AUD_MAN_PGA_VALUE);
	BK3000_AUD_AGC_CONF2 |= (volume << sft_AUD_MAN_PGA_VALUE); // volume

	/*int high, low;

	if(volume > SDADC_VOLUME_MAX)
		volume = SDADC_VOLUME_MAX;

	high = volume / 12;
	low = volume - high*12;

	BK3000_AUD_AGC_CONF2 &= ~(0x7F << sft_AUD_MAN_PGA_VALUE);
	BK3000_AUD_AGC_CONF2 |= (high << (sft_AUD_MAN_PGA_VALUE + 4));
	BK3000_AUD_AGC_CONF2 |= (low << sft_AUD_MAN_PGA_VALUE);*/
}

void sdadc_isr( void )
{
}
#endif
