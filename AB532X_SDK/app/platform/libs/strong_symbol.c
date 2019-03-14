/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

#if !FUNC_USBDEV_EN
void usb_dev_isr(void){}
void ude_ep_reset(void){}
void ude_control_flow(void){}
void ude_isoc_tx_process(void){}
void ude_isoc_rx_process(void){}
void lock_code_usbdev(void){}
#endif //FUNC_USBDEV_EN

#if (REC_TYPE_SEL != REC_MP3)
int mpa_encode_frame(void) {return 0;}
#endif //(REC_TYPE_SEL != REC_MP3)

#if (REC_TYPE_SEL != REC_ADPCM && !BT_HFP_REC_EN)
void adpcm_encode_process(void){}
#endif //(REC_TYPE_SEL != REC_ADPCM)

#if !MUSIC_WAV_SUPPORT
int wav_dec_init(void){return 0;}
bool wav_dec_frame(void){return false;}
void lock_code_wavdec(void){}
#endif // MUSIC_WAV_SUPPORT

#if !MUSIC_WMA_SUPPORT
int wma_dec_init(void){return 0;}
bool wma_dec_frame(void){return false;}
void lock_code_wmadec(void){}
#endif // MUSIC_WMA_SUPPORT

#if !MUSIC_APE_SUPPORT
int ape_dec_init(void){return 0;}
bool ape_dec_frame(void){return false;}
void lock_code_apedec(void){}
#endif // MUSIC_APE_SUPPORT

#if !MUSIC_FLAC_SUPPORT
int flac_dec_init(void){return 0;}
bool flac_dec_frame(void){return false;}
void lock_code_flacdec(void){}
#endif // MUSIC_FLAC_SUPPORT

#if !MUSIC_SBC_SUPPORT
int sbcio_dec_init(void){return 0;}
bool sbcio_dec_frame(void){return false;}
#endif // MUSIC_SBC_SUPPORT

#if !FMRX_REC_EN
void fmrx_rec_start(void){}
void fmrx_rec_stop(void){}
#endif // FMRX_REC_EN

#if !BT_REC_EN
void bt_music_rec_start(void) {}
void bt_music_rec_stop(void) {}
#endif

#if !USB_SUPPORT_EN
void usb_isr(void){}
void usb_init(void){}
#endif

#if ((!SD_SUPPORT_EN) && (!FUNC_USBDEV_EN))
void sd_disk_init(void){}
void sdctl_isr(void){}
void sd_disk_switch(u8 index){}
bool sd0_stop(bool type){return false;}

bool sd0_init(void){return false;}
bool sd0_read(void *buf, u32 lba){return false;}
bool sd0_write(void* buf, u32 lba){return false;}

#endif

#if !FUNC_MUSIC_EN
u32 fs_get_file_size(void){return 0;}
void fs_save_file_info(unsigned char *buf){}
void fs_load_file_info(unsigned char *buf){}
#endif // FUNC_MUSIC_EN

#if !BT_TWS_EN
void btstack_tws_init(void){}
void sbc_play_init(void){}
AT(.bcom_text.sbc.play)
void sbc_play_reset(void){}
AT(.bcom_text.sbc.play)
void sbc_cache_reset(void){}

AT(.bcom_text.sbc_cache)
bool sbc_cache_fill(uint8_t *packet, uint16_t size) {
    return true;
}

AT(.bcom_text.sbc_cache)
uint8_t avdtp_fill_tws_buffer(u8 *ptr, uint len) {
    return 0;
}

AT(.bcom_text.sbc_cache)
uint8_t sbc_cache_before_rx(uint8_t *data_ptr, uint16_t data_len) {
    return 0;
}

AT(.sbcdec.code)
void sbc_cache_free(void) {
}

AT(.sbcdec.code)
size_t sbc_cache_read(uint8_t **buf) {
    return 0;
}

AT(.com_text.bb.tws)
void tws_timer_isr(void) {
}

AT(.bcom_text.bb.btisr)
void lc_tws_hssi_instant(uint8_t Lid) {
}

AT(.bcom_text.bb.btisr)
uint8_t lc_get_ticks_status(void) {
    return 0x80;
}

uint8_t lc_tws_set_spr(uint8_t Lid, uint SprIdx, uint32_t Hssi) {
    return 0;
}

AT(.bcom_text.sbc.play)
void tws_ticks_trigger(uint32_t ticks) {
}

AT(.bcom_text.sbc.play)
uint32_t tws_get_play_ticks(uint32_t frame_num) {
    return 0;
}

AT(.com_text.sbc.play)
void tws_trigger_isr(void) {
}

AT(.bcom_text.sbc.play)
bool tws_cache_is_empty(void) {
    return false;
}

AT(.bcom_text.sbc.play)
void sbc_cache_env_reset(void)
{
}

AT(.bcom_text.sbc.send)
void tws_send_pkt(void)
{
}
#endif

#if !BT_TWS_EN || !BT_TSCO_EN
void lc_tsco_init(void) {
}
AT(.bcom_text.bb.tws.sco)
void ext_lc_fill_tsco_dat(void) {
}
AT(.bcom_text.bb.tws.sco)
void ext_lc_tsco_set_ticks(int32_t offset) {
}
AT(.bcom_text.bb.tws.sco)
void ext_lc_tsco_slot_check(uint32_t clock) {
}
AT(.bcom_text.bb.tws.sco)
bool ext_lc_tsco_get_active_acl(uint32_t clock, uint8_t *next_lid) {
    return false;
}
AT(.bcom_text.bb.tws.sco)
bool ext_lc_tsco_rx(void *conn, uint8_t rx_type, uint8_t *ptr) {
    return false;
}
AT(.bcom_text.bb.tws.sco)
bool ext_lc_tsco_tx_ack(void *conn, uint8_t tx_type) {
    return false;
}
AT(.bcom_text.bb.tws.sco)
void ext_lc_tsco_put_txpkt(uint8_t *rx_buf, uint8_t pkt_len, bool pkt_stat) {
}
AT(.bt_voice.bb.tws.sco)
uint8_t ext_lc_tsco_get_status(void) {
    return 0;
}
void lc_tsco_send_setup(uint8_t sco_lid, uint8_t flags, uint8_t dsco, uint8_t tsco, uint8_t pkt_len, uint8_t air_mode) {
}
void lc_tsco_send_kill(uint8_t sco_lid) {
}
#else
bool bt_tsco_is_en(void) {
    return BT_TSCO_EN;
}
#endif

#if !BT_FCC_TEST_EN
void huart_init(void)
{
}
#endif

#if !BT_HFP_REC_EN
AT(.com_text.bt_rec)
void bt_sco_rec_mix_do(u8 *buf, u32 samples) {}
void bt_sco_fill_remote_buf(u16 *buf, u16 samples) {}
#endif

#if !SYS_MAGIC_VOICE_EN
void magic_voice_process(void) {}
void mav_kick_start(void) {}
#endif

#if !FUNC_SPDIF_EN
void spdif_process(void){}
bool spdif_smprate_detect(void) {    return false;}
void spdif_isr(void){}
#endif

#if !I2S_DMA_EN
void i2s_isr(void) {}
void i2s_process(void) {}
#endif

#if !LE_EN
AT(.bcom_text.bb.leisr)
void ble_isr(void) {}
AT(.bcom_text.bb.leisr)
void ble_evt_instant(void *evt, uint32_t *next_basetimecnt, bool role) {}
AT(.bcom_text.bb.leisr)
void ble_evt_schedule(void) {}
AT(.bcom_text.bb.leisr)
uint32_t ble_evt_time_get(void) { return 0; }
AT(.bcom_text.bb.leisr)
uint8_t llm_set_adv_data(void const *param) { return 0; }
AT(.bcom_text.bb.leisr)
void ble_lm_adv_data_update(void) {}
AT(.bcom_text.bb.leisr)
void ble_lc_check_update_evt_sent(uint16_t conhdl, void *evt_new) {}
AT(.bcom_text.bb.leisr)
void ble_evt_free_do(void *evt) {}

void llc_con_update_ind(uint16_t conhdl, void *evt_new) {}
void llc_con_update_cmd_complete_send(uint8_t status, uint16_t conhdl, void *evt) {}
void llc_map_update_ind(uint16_t conhdl) {}
void llc_common_nb_of_pkt_comp_evt_send(uint16_t conhdl, uint8_t nb_of_pkt) {}

void ble_ll_init(void) {}
void ble_ll_reset(void) {}
bool ble_event_cmd_complete(uint8_t *packet, int size) { return false; }
void ble_event_meta(uint8_t *packet, int size) {}
int att_server_notify_do(void) { return 1;}
void hci_run_le_connection(void) {}
void btstack_ble_init(void) {}
void btstack_ble_send(void) {}
void btstack_ble_update_conn_param(void) {}
#endif

#if !BT_PBAP_EN
void pbap_client_init(void) {}
void goep_client_init(uint8_t rfcomm_channel_nr) {}
void btstack_pbap(u8 param) {}
#endif

#if !DAC_DRC_EN
void drc_process(s16 *ldata, s16 *rdata) {}
AT(.sbcdec.code)
void bt_sbc_pcm_output(void) {}
AT(.mp3dec.dac)
void mpeg_pcm_output(void) {}
#endif
