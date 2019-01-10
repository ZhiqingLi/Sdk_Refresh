#ifndef __APP_ANC_H__
#define __APP_ANC_H__

#ifdef __cplusplus
extern "C" {
#endif
void app_anc_set_playback_samplerate(enum AUD_SAMPRATE_T sample_rate);
void app_anc_init(enum AUD_IO_PATH_T input_path, enum AUD_SAMPRATE_T playback_rate, enum AUD_SAMPRATE_T capture_rate);
void app_anc_close(void);
void app_anc_enable(void);
void app_anc_disable(void);
bool anc_enabled(void);
void test_anc(void);
void app_anc_resample(uint32_t res_ratio, uint32_t *in, uint32_t *out, uint32_t samples);
void app_anc_key(APP_KEY_STATUS *status, void *param);
int app_anc_open_module(void);
int app_anc_close_module(void);
enum AUD_SAMPRATE_T app_anc_get_play_rate(void);
bool app_anc_work_status(void);
void app_anc_send_howl_evt(void);
uint32_t app_anc_get_anc_status(void);
bool app_pwr_key_monitor_get_val(void);
bool app_anc_switch_get_val(void);
void app_anc_ios_init(void);

void app_anc_set_init_done(void);
bool app_anc_set_reboot(void);

#ifdef __cplusplus
}
#endif
#endif
