
#ifndef __APP_DMA_HANDLE_H__
#define __APP_DMA_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

//#define DMA_PUSH_TO_TALK

#if 0 // change begin by cai.zhong 20181119 for Customized information in app_dma_info.cpp
#define  DMA_KEY  "R2rx57tF47YsQBPXP5ZtIlZqfbAciobQ" // linkplay KEY
#define  DMA_PRODUCT_ID "MTtSzapshal9iihMGW4yjpk1hmkOVtB1" // linkplay id
#define  DMA_SERIAL_NUM  "11111111"
#define  DMA_DEVICE_NAME  "bumblebee"
#define  DMA_DEVICE_TYPE "DOCK"
#define  DMA_MANUFACTURER  "LinkPlay"
#define  DMA_FIRMWARE_VERSION  "0.0.4"
#define  DMA_SOFTWARE_VERSION  "0.0.4"
#endif // change end by cai.zhong 20181119 for Customized information in app_dma_info.cpp



enum dma_connection_type {
    DMA_CONNECTION_BLE = 0,
    DMA_CONNECTION_RFCOMM,
    DMA_CONNECTION_QTY,
};

enum dma_pair_state {
    DMA_PAIR_UNPAIR = 0,
    DMA_PAIR_PAIR,
};

enum {
    DMA_NO_ERROR = 0,
    DMA_DEVICE_IN_VOICECALL = -1,
    DMA_INVALID_PARAM = -2,
    DMA_UNKNOWN_ERR = -3,
    DMA_SEND_TMO = -4,
    DMA_SEND_ERR = -5,
};

void app_dueros_dma_init();
int app_dueros_dma_deinit(void);


int dma_data_received(unsigned char *buff, unsigned int len);

int dma_ble_ccc_changed(unsigned char value);
int dma_ble_mtu_changed(unsigned int mtu_size);
int dma_ble_connected(void);
int dma_ble_disconnected(void);
int dma_spp_connected(void);
int dma_spp_disconnected(void);




int app_baidu_dma_wake_up();
void app_dma_send_voice_stream();
void dma_a2dp_connected();
void dma_a2dp_disconnected();


#ifdef __cplusplus
}
#endif
#endif
