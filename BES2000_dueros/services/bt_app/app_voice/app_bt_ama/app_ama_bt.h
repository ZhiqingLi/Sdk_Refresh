#ifndef __APP_AMA_BT_H__
#define __APP_AMA_BT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPP1 0
#define SPP2 1
#define SPP_NUM 2


typedef void(*app_ama_rfcomm_tx_done_t)(void);

//void app_ama_rfcomm_server_init(void);

void app_ama_register__rfcomm_tx_done(app_ama_rfcomm_tx_done_t callback);

void app_ama_rfcomm_send(uint8_t* ptrData, uint32_t length);

#ifdef __cplusplus
}
#endif


#endif
