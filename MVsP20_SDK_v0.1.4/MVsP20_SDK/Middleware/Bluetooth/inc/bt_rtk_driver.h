/*
bluetooth realtek 8761 hci driver 
file name: bt_rtk_driver.h
*/


/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_rtk_driver bt_rtk_driver.h
* @{
*/


#ifndef __BT_RTK_DRIVER_H__
#define __BT_RTK_DRIVER_H__

#include "type.h"


/***************************************************************
 *
 * UART releated defines
 * 
 */

/**
 *Uart baudrate
 */
typedef uint8_t			UartBaudrate;

#define UART_BAUDRATE_1P5M			1		/*!< 1.5M bps     */


typedef struct _RtkUartSettings
{
	UartBaudrate	uartBaudrate;
}RtkUartSettings;


/*****************************************************************
 *
 * PCM releated defines
 *
 */

/*
 * SCO interface selection.
 * UART means the SCO data transfered to HOST via UART(Same as BUART)
 * PCM means the SCO data transfered to HOST via 4-lines IIS
 *
 */
typedef uint8_t			ScoInterface;

#define SCO_INTERFACE_UART			0		/*!< Use Buart TX & RX */
#define SCO_INTERFACE_PCM			1		/*!< Use PCM */

/*
 * The following PCM setting will be useless if sco interface select HCI
 *
 */

/**
 * Audio Codec PCM data format
 */
typedef uint8_t			PcmFormat; 

#define PCM_FORMAT_LINEAR			0		/*!< linear */
#define PCM_FORMAT_ULAW				1		/*!< u-law  */
#define PCM_FORMAT_ALAW				2		/*!< a-law  */
#define PCM_FORMAT_CVSD				3		/*!< CVSD   */

/**
 * PCM sampling data width
 */
typedef uint8_t			PcmWidth;

#define PCM_WIDTH_16BIT				0		/*!< 16 bit */
#define PCM_WIDTH_8BIT				1		/*!< 8 bit  */


typedef struct _RtkPcmSettings
{
	ScoInterface	scoInterface;
	PcmFormat		pcmFormat;
	PcmWidth		pcmWidth;
}RtkPcmSettings;

/*****************************************************************
 *
 * Radio releated defines
 *
 */

/**
 * Tx Power: value range ( 0x06 ~ 0x0d )
 */
typedef uint8_t			TxPower;

/**
 * Tx Power: value range ( 0x11 ~ 0x15 )
 */
typedef uint8_t			TxDac;

typedef struct _RtkRadioPower
{
	TxPower		txPower;
	TxDac		txDac;
}RtkRadioPower;


/*****************************************************************
 *
 * RTK Config Parameters
 *
 */
typedef struct _RtkConfigParam
{
	uint8_t				bdAddr[6];
	RtkUartSettings		uartSettings;
	RtkPcmSettings		pcmSettings;
	RtkRadioPower		radioPower;
	bool 				enableFlowCtrl;
	bool 				enableExtClk;
}RtkConfigParam;


bool BTDeviceInit_RTK8761(RtkConfigParam * configParams);

bool BTDeviceDeInit_RTK8761(void);


#endif

