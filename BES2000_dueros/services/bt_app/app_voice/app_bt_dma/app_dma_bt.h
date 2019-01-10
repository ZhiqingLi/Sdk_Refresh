#ifndef __APP_DMA_BT_H__
#define __APP_DMA_BT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPP1 0
#define SPP2 1
#define SPP_NUM 2


typedef void(*app_dma_rfcomm_tx_done_t)(void);

void app_dma_rfcomm_client_init(BtRemoteDevice *dev); //add by cai.zhong 20190103 for rfcomm
void app_dma_rfcomm_server_init(void);

void app_dma_register__rfcomm_tx_done(app_dma_rfcomm_tx_done_t callback);

void app_dma_rfcomm_send(uint8_t* ptrData, uint32_t length);

void app_dma_link_free_after_rfcomm_dis(); //add by cai.zhong 20190103 for rfcomm

void app_dma_rfcomm_disconnlink(void); //add by cai.zhong 20190103 for rfcomm

#ifdef __cplusplus
}
#endif


#endif
