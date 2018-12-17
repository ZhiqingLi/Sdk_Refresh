
/*
* bt_spp_api.h
*/

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_spp_api bt_spp_api.h
* @{
*/

#ifndef __BT_SPP_API_H__
#define __BT_SPP_API_H__

bool SppConnect(uint8_t * addr);

bool SppSendData(uint8_t * data, uint16_t dataLen);

#endif


