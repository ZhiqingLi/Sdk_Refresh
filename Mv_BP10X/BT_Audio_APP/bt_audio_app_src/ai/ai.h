#ifndef __AI_H__
#define __AI_H__

#include <string.h>
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//AI初始化
void ai_init(void);

//AI数据编码  buf必须为采样率为44100的PCM数据
void ai_audio_encode(int16_t* buf,uint32_t sample,uint8_t ch);

//按键或者其他方式调用
void ai_start(void);


//BLE接收后的数据处理
void ble_rcv_data_proess(uint8_t *p,uint32_t size);

//spp接收后的数据处理
void spp_rcv_data_proess(uint8_t *p,uint32_t size);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__ADC_KEY_H__
