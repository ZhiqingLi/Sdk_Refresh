#ifndef _DRIVER_DTMF_H_
#define _DRIVER_DTMF_H_

typedef struct _driver_dtmf_tone_conf_s
{
    uint16  fstep;
    uint8   gain;
    uint8   enable;
}driver_dtmf_tone_conf_t;

typedef struct _driver_dtmf_mode_s
{
    uint8   tone_turation;
    uint8   tone_pause;
    uint8   tone_patten;
    uint8   tone_mode;
}driver_dtmf_mode_t;

int8 dtmf_tone_config( uint8 index, driver_dtmf_tone_conf_t *conf );
int8 dtmf_mode_config( driver_dtmf_mode_t * mode );
void dtmf_enable( int enable );
void dtmf_isr(void);


#endif
