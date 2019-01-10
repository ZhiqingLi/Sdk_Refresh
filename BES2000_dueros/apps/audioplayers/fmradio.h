#ifndef __APP_AUDIO_H__
#define __FMRADIO_H__

#ifdef __cplusplus
extern "C" {
#endif

int fm_radio_player(bool on);

void fm_tune(uint32_t freqkhz);


#ifdef __cplusplus
}
#endif

#endif//__FMRADIO_H__
