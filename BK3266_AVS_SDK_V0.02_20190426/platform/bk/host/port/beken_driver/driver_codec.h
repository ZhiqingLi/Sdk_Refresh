#ifndef _DRIVER_CODEC_H
#define _DRIVER_CODEC_H

void codec_init(void);
int codec_cfg(unsigned int fs, unsigned char datawidth);
void codec_close(void);
void codec_volume_control(unsigned char volume);
void codec_adc_enable(uint8 enable);
void codec_volume_mute(uint8 enable);
void codec_adc_volume_adjust(uint8 volume);
void codec_open(void);

#endif
