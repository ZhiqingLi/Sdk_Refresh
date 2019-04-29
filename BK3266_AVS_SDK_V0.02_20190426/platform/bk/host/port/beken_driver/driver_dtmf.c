#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_DRIVER_DTMF

#define DTMF_DEFAULT_FIFO_THRE      24

int8_t dtmf_tone_config( uint8 index, driver_dtmf_tone_conf_t *conf )
{
    volatile uint32 *reg_ptr;

    if( index > 2 || index < 1 || conf == NULL )
        return -1;

    conf->gain &= 0xf;
    conf->fstep &= 0xFFFF;
    conf->enable &= 0x01;

    reg_ptr = (volatile uint32 *)(BK3000_AUD_BASE_ADDR+ (index + 1)*4);

    *reg_ptr &= ~( 0xf << sft_AUD_DTMF_TONE_ATTU);
    *reg_ptr |= ( ( conf->gain ) << sft_AUD_DTMF_TONE_ATTU);
    *reg_ptr &= ~( 0xFFFF << sft_AUD_DTMF_TONE_STEP );
    *reg_ptr |= (conf->fstep << sft_AUD_DTMF_TONE_STEP );
    *reg_ptr &= ~( 0x01 << sft_AUD_DTMF_TONE_ENABLE);
    *reg_ptr |= (conf->enable << sft_AUD_DTMF_TONE_ENABLE );

    return 0;

}


int8_t dtmf_mode_config( driver_dtmf_mode_t * mode )
{
    if( mode == NULL )
        return -1;

    mode->tone_turation &= 0xf;
    mode->tone_pause &= 0xf;
    mode->tone_patten &= 0x01;
    mode->tone_mode &= 0x1;

    BK3000_AUD_DTMF_CONF0 = ((mode->tone_turation << sft_AUD_DTMF_TONE_ACT)
        |(mode->tone_pause << sft_AUD_DTMF_TONE_PAUSE)
        |(mode->tone_patten << sft_AUD_DTMF_TONE_PATTERN)
        |(mode->tone_mode << sft_AUD_DTMF_TONE_MODE));

    return 0;
}

void dtmf_enable( int enable )
{
    BK3000_AUD_FIFO_CONF &= ~((0x1f << sft_AUD_DTMF_WR_THRE)|(0x1 << sft_AUD_DTMF_INT_EN));
    BK3000_AUD_FIFO_CONF |= ( DTMF_DEFAULT_FIFO_THRE << sft_AUD_DTMF_WR_THRE );
    BK3000_AUD_FIFO_CONF |= ( 0x1 << sft_AUD_DTMF_INT_EN );
    if( enable )
    {
        BK3000_AUD_AUDIO_CONF|= (0x1 << sft_AUD_DTMF_ENABLE );
    }
    else
    {
        BK3000_AUD_AUDIO_CONF &= ~(0x1 << sft_AUD_DTMF_ENABLE );
    }

    return;
}

void dtmf_isr(void)
{
    uint32 sts = BK3000_AUD_FIFO_STATUS;
    uint16   data[DTMF_DEFAULT_FIFO_THRE];
    int i;

    if( sts & (bit_DTMF_INTR_FLAG) )
    {
        for( i = 0; i < DTMF_DEFAULT_FIFO_THRE; i++ )
            data[i] = BK3000_AUD_DTMF_PORT;

        aud_fill_buffer((uint8 *)data, sizeof(data));

        if( aud_get_buffer_size() < AUDIO_BUFF_LEN -256 )
        {
            aud_open();
            aud_PAmute_oper(0);
        }
    }

    BK3000_AUD_FIFO_STATUS  |= sts;

    return;
}
#endif
