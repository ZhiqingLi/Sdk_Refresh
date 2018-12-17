///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_device_cfg.c
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#include "type.h"
#include "bt_config.h"
#include "gpio.h"

#if (BT_RF_DEVICE == BTUartDeviceRTK8761)
#include "bt_rtk_driver.h"
#elif BT_RF_DEVICE == BTUartDeviceMTK662X
#include "bt_mtk_driver.h"
#endif


static void BtDevicePinConfig(void)
{
	GPIO_UartRxIoConfig(1, 1);
	GPIO_UartTxIoConfig(1, 1);
	GPIO_UartRtsIoConfig(1, 0);
	GPIO_Clk32kOutIoConfig(0);
}

void BtLDOEn(bool enable)
{
	GPIO_RegOneBitSet(GPIO_A_OE, GPIOA4);
	GPIO_RegOneBitClear(GPIO_A_IE, GPIOA4);

	if(enable)
	{
		GPIO_RegOneBitSet(GPIO_A_OUT, GPIOA4);
	}
	else
	{
		GPIO_RegOneBitClear(GPIO_A_OUT, GPIOA4);
	}
}


uint8_t btDevAddr[6] = BT_ADDRESS;

bool BtDeviceInit(void)
{
	bool	ret;

	BtDevicePinConfig();

#if BT_RF_DEVICE == BTUartDeviceRTK8761
	
	{
		RtkConfigParam			params;

		memcpy(params.bdAddr, btDevAddr, 6);
		params.uartSettings.uartBaudrate	= PARAMS_UART_BAUDRATE;

		params.pcmSettings.scoInterface		= PARAMS_SCO_INTERFACE;
		params.pcmSettings.pcmFormat		= PARAMS_PCM_FORMAT;
		params.pcmSettings.pcmWidth			= PARAMS_PCM_WIDTH;

		params.radioPower.txPower			= PARAMS_TX_POWER;
		params.radioPower.txDac				= PARAMS_TX_DAC;

		params.enableFlowCtrl				= PARAMS_FLOW_CTRL;
		params.enableExtClk					= PARAMS_EXT_CLOCK;

		ret = BTDeviceInit_RTK8761(&params);
	}

#elif BT_RF_DEVICE == BTUartDeviceMTK662X
	{
		MtkConfigParam			params;

		/*uart config*/
		params.uartSettings.baudrate		= PARAMS_UART_BAUDRATE;
		params.uartSettings.dataBits		= PARAMS_UART_DATA_BITS;
		params.uartSettings.stopBits		= PARAMS_UART_STOP_BITS;
		params.uartSettings.parityChk		= PARAMS_UART_PARITY_CHECK;
		params.uartSettings.flowCtrl		= PARAMS_UART_FLOW_CONTROL;

		/*pcm config*/
		params.pcmSettings.pcmMode			= PARAMS_PCM_MODE;
		params.pcmSettings.pcmClock			= PARAMS_PCM_CLOCK;
		params.pcmSettings.pcmInClkAbi		= PARAMS_PCM_CLK_ABI;
		params.pcmSettings.pcmSyncFormat	= PARAM_PCM_SYNC_FORMAT;
		params.pcmSettings.pcmEndianFormat	= PARAMS_PCM_ENDIAN;
		params.pcmSettings.pcmSignExt		= PARAMS_PCM_ENDIAN;

		/*local features*/
		params.localFeatures.enableAutoSniff		= PARAMS_AUTO_SNIFF;
		params.localFeatures.enableEV3				= PARAMS_EV3;
		params.localFeatures.enable3EV3				= PARAMS_3EV3;
		params.localFeatures.enableSimplePairing	= PARAMS_SIMPLE_PAIRING;

		/*tx power*/
		params.txPower		= PARAMS_TX_POWER;

		/*xo trim*/
		params.trimValue	= PARAMS_XO_TRIM;

		/* device address*/
		memcpy(params.bdAddr, btDevAddr, 6);

		ret = BTDeviceInit_MTK662x(&params);
	}
#else
	#error "Select one bluetooth RF device";
#endif

	return ret;
}

