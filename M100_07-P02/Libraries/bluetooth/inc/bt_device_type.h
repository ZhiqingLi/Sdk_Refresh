/**
 *******************************************************************************
 * @file    bt_device_type.h
 * @author  Halley
 * @version V1.0.0
 * @date    26-June-2012
 * @brief   Bluetooth LE releated APIs
 * @maintainer Halley
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
 
/**
* @addtogroup А¶СА
* @{
* @defgroup bt_device_type bt_device_type
* @{
*/
 
#ifndef __BT_DEVICE_TYPE_H__
#define __BT_DEVICE_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
enum BtDeviceType
{
    BtNoneDevice = 0,
    BTUsbDevice,
    BTUartDeviceRda5875,
    BTUartDeviceCsrBC6,
    BTUartDeviceBcm20702,
    BTUartDeviceMTK662X,
    BTUartDeviceRTL8761,
};

typedef enum
{
    GPIO_PORT_A = 1, /**< Use GPIOA for LDOEN/RST */
    GPIO_PORT_B,	 /**< Use GPIOB for LDOEN/RST */
    GPIO_PORT_C	     /**< Use GPIOC for LDOEN/RST */
} GPIO_PORT_SEL;


/**
 * @brief
 *  	Config GPIO with BT
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *		This function should be called before BTDeviceInit()
 */
void BTDevicePinCFG(void);

/**
 * @brief
 *  	Initialize BT RF chipset
 *
 * @param BTDeviceType Chip Type
 * @param BdAdd Local BT address
 *
 * @return
 *		TRUE for successful
 *
 * @note
 *	
 */	
bool BTDeviceInit(uint8_t BTDeviceType, int8_t* BdAdd/*6Bytes, LE*/);

 /**
 * @brief
 *  	Power off BT RF
 *
 * @param NONE
 *
 * @return
 *		NONE
 *
 * @note
 *	
 */	
void BTDevicePowerOff(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
