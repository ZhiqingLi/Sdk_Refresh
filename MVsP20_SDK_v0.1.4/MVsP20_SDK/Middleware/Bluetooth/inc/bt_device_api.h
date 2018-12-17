/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			bt_device_api.h
* Description:		bt_device_type function h file.
*
******************************************************************************/

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_device_api bt_device_api.h
* @{
*/

#ifndef __BT_DEVICE_API_H__
#define __BT_DEVICE_API_H__

typedef enum
{
    BtNoneDevice = 0,
    BTUartDeviceRda5875, 
    BTUartDeviceCsrBC6, 
    BTUartDeviceBcm20702, 
    BTUartDeviceMTK662X,
	BTUartDeviceRTK8761,
    BTOtherDevice = 0xFF,
} BT_DEVICE_TYPE;


BOOL BTDeviceInit(BT_DEVICE_TYPE BTDeviceType, U8 * BdAdd);

BOOL BTDeviceDeInit(BT_DEVICE_TYPE BTDeviceType);


#endif // __BT_DEVICE_API_H__
