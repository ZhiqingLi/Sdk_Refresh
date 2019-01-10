#ifndef __APP_SPP_H__
#define __APP_SPP_H__


typedef void(*app_spp_tx_done_t)(void);
void app_spp_register_tx_done(app_spp_tx_done_t callback);
void app_spp_init(void);
void app_sv_send_cmd_via_spp(uint8_t* ptrData, uint32_t length);
void app_sv_send_data_via_spp(uint8_t* ptrData, uint32_t length);

uint16_t app_spp_tx_buf_size(void);
void app_spp_init_tx_buf(uint8_t* ptr);
uint8_t* app_spp_fill_data_into_tx_buf(uint8_t* ptrData, uint32_t dataLen);

#endif
