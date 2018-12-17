
///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_device_cfg.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __BT_DEVICE_CFG_H__
#define __BT_DEVICE_CFG_H__
#include "type.h"


#define ENABLE						TRUE
#define DISABLE						FALSE

/*****************************************************************
 * Bluetooth RF type
 */
#define BTUartDeviceMTK662X			1	
#define BTUartDeviceRTK8761			2

#define BT_RF_DEVICE				BTUartDeviceRTK8761 //BTUartDeviceMTK662X //BTUartDeviceRTK8761 // bt chip type


/*****************************************************************
 * Bluetooth UART work mode
 */

#define BT_UART_WORKMODE_LOOP	0		/*!< Buart in loop check mode*/

#define BT_UART_WORKMODE_INTR	1		/*!< Buart in interrupt mode*/

#define BT_UART_WORKMODE		BT_UART_WORKMODE_LOOP



/*****************************************************************
 *
 * BLE config
 *
 */
#define BLE_SUPPORT					DISABLE



/*****************************************************************
 *
 * Bluetooth stack common config
 *
 */
#define BT_ADDRESS					{0x04, 0x07, 0x13, 0x24, 0x05, 0x06}

#define BT_NAME						"NewBtStack"


/**
 * Profile support. One of following profiles MUST be supported
 */
#define BT_HFP_SUPPORT				ENABLE

#define BT_A2DP_SUPPORT				ENABLE

#define BT_AVRCP_SUPPORT			ENABLE

#define BT_SPP_SUPPORT				DISABLE

#define BT_OPP_SUPPORT				DISABLE

#define BT_HID_SUPPORT				DISABLE

#define BT_MFI_SUPPORT				DISABLE


/*****************************************************************
 *
 * A2DP config
 *
 */
#if BT_A2DP_SUPPORT == ENABLE

#include "bt_a2dp_api.h"

#define BT_A2DP_AUDIO_DATA				A2DP_AUDIO_DATA_PCM

#endif /* BT_A2DP_SUPPORT == ENABLE */

/*****************************************************************
 *
 * AVRCP config
 *
 */
#if BT_A2DP_SUPPORT == ENABLE

#include "bt_avrcp_api.h"

#define BT_AVRCP_ADVANCED				ENABLE

/*
 * If it doesn't support Advanced AVRCP, TG side will be ignored
 */
#define BT_AVRCP_TG_SIDE				ENABLE

#endif /* BT_A2DP_SUPPORT == ENABLE */

/*****************************************************************
 *
 * HFP config
 *
 */
#if BT_HFP_SUPPORT == ENABLE

#include "bt_hfp_api.h"

#define BT_HFP_SUPPORT_WBS				ENABLE

/*
 * If it doesn't support WBS, only PCM format data can be
 * transfered to application.
 */
#define BT_HFP_AUDIO_DATA				HFP_AUDIO_DATA_mSBC

#endif /* BT_HFP_SUPPORT == ENABLE */



/*****************************************************************
 *
 * RealTek 8761AT config
 *
 */
#if BT_RF_DEVICE == BTUartDeviceRTK8761

#include "bt_rtk_driver.h"

#define EXT_CLOCK					DISABLE
#define FLOW_CTRL					DISABLE


/**
 *
 * GPIO config
 *
 */
//#define BT_UART_RX_PORT				(3)					// 0 -- A13; 1 -- A24; 2 -- B8; 3 -- B29
//#define BT_UART_TX_PORT				(3)					// 0 -- A16; 1 -- A25; 2 -- B9; 3 -- B28
//#if EXT_CLOCK == ENABLE
//#define BT_UART_EXT_CLK_PORT		(1)					// 0 -- A11; 1 -- B30; 2 -- C0
//#else
//#define BT_UART_EXT_CLK_PORT		(0xFF)				// disable
//#endif
//
//#if FLOW_CTRL == ENABLE
//#define BT_UART_RTS_PORT			(1)					// 0 -- A12; 1 -- B31; 2 -- C1
//#else
//#define BT_UART_RTS_PORT			(0xFF)				// disable
//#endif
//
//#define BT_LDOEN_GPIO_PORT			(2)					// 1 -- A; 2 -- B; 3 -- C
//#define BT_LDOEN_GPIO_PIN			(23)				//

/**
 *
 * RealTek 8761AT parameters config
 *
 */

/** uart baudrate */
#define PARAMS_UART_BAUDRATE		UART_BAUDRATE_1P5M

/** flow control */
#define PARAMS_FLOW_CTRL			FLOW_CTRL

/** SCO data interface */
#define PARAMS_SCO_INTERFACE		SCO_INTERFACE_UART

/** PCM format */
#define PARAMS_PCM_FORMAT			PCM_FORMAT_LINEAR

/** PCM width */
#define PARAMS_PCM_WIDTH			PCM_WIDTH_16BIT

/** TX power */
#define PARAMS_TX_POWER				0x0b

/** TX dac */
#define PARAMS_TX_DAC				0x12

/** external clock */
#define PARAMS_EXT_CLOCK			EXT_CLOCK





/*****************************************************************
 *
 * MTK 6622 config
 *
 */
#elif BT_RF_DEVICE == BTUartDeviceMTK662X
#include "bt_mtk_driver.h"

#define EXT_CLOCK					ENABLE
#define FLOW_CTRL					DISABLE		// Flow Control MUST be disable for MTK6622

/**
 *
 * GPIO config
 *
 */
#define BT_UART_RX_PORT				(3)			// 0 -- A13; 1 -- A24; 2 -- B8; 3 -- B29 
#define BT_UART_TX_PORT				(3)			// 0 -- A16; 1 -- A25; 2 -- B9; 3 -- B28 
#if EXT_CLOCK == ENABLE
#define BT_UART_EXT_CLK_PORT		(1)			// 0 -- A11; 1 -- B30; 2 -- C0
#else
#define BT_UART_EXT_CLK_PORT		(0xFF)		// disable
#endif

#if FLOW_CTRL == ENABLE
#error "MT6622 MUST disable flow control"
#else
#define BT_UART_RTS_PORT			(0xFF)		// disable
#endif

#define BT_LDOEN_GPIO_PORT			(2)			// 1 -- A; 2 -- B; 3 -- C
#define BT_LDOEN_GPIO_PIN			(23)		// 

#define BT_REST_GPIO_PORT			(2)			// 1 -- A; 2 -- B; 3 -- C
#define BT_REST_GPIO_PIN			(31)

/**
 *
 * MTK 6622 parameters config
 *
 */


/*
 * Uart Settings
 */

/** uart baud rate*/
#define PARAMS_UART_BAUDRATE		UART_BAUDRATE_921P6K

/* Don't modify the following configuration of UART */
/** uart data bits*/
#define PARAMS_UART_DATA_BITS		UART_DATA_BITS_8

/** uart stop bits*/
#define PARAMS_UART_STOP_BITS		UART_STOP_BITS_1

/** uart parity check*/
#define PARAMS_UART_PARITY_CHECK	UART_PARITY_CHECK_NONE

/** uart flow control*/
#define PARAMS_UART_FLOW_CONTROL	UART_FLOW_CONTROL_NONE


/**
 * PCM Settings
 */

/** PCM mode */
#define PARAMS_PCM_MODE				PCM_MODE_MASTER

/** PCM clock */
#define PARAMS_PCM_CLOCK			PCM_CLOCK_256K

/** PCM input clock frequence ability */
#define PARAMS_PCM_CLK_ABI			PCM_IN_CLOCK_LESS_1M

/** PCM sync format */
#define PARAM_PCM_SYNC_FORMAT		PCM_FRAME_SYNC_FORMAT_SHORT

/** PCM frame endian */
#define PARAMS_PCM_ENDIAN			PCM_ENDIAN_LSB

/** PCM Sign Extension */
#define PARAMS_SIGN_EXT				PCM_ZERO_PADDING


/**
 * Local features
 */

/** Enable/Disable auto sniff*/
#define PARAMS_AUTO_SNIFF			ENABLE

/** Enable/Disable EV3*/
#define PARAMS_EV3					DISABLE

/** Enable/Disable 3EV3*/
#define PARAMS_3EV3					DISABLE

/** Enable/Disable simple pairing*/
#define PARAMS_SIMPLE_PAIRING		ENABLE


/**
 * TX power
 */

#define PARAMS_TX_POWER				0x06


/**
 * XO trim
 */
#define PARAMS_XO_TRIM				0x40

#else
#error "BT_RF_DEVICE must be set"
#endif






/*****************************************************************
 *
 * MACRO-defines dependence check
 *
 */


#if ((BT_HFP_SUPPORT_WBS == ENABLE) && (PARAMS_SCO_INTERFACE != SCO_INTERFACE_UART))
#error "WBS MUST use UART transport for SCO"
#endif



#endif /*__BT_DEVICE_CFG_H__*/

