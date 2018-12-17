Demo_Bluetooth工程主要演示了P20 传统蓝牙的连接，断开操作流程


注意：
	适用于Realtek8761蓝牙芯片


硬件环境要求:
	1. P20系列开发板
	2. 串口波特率115200 RX接GPIO C2
	3. 蓝牙LDO脚，接GPIOA4
	4. 蓝牙BUART_TXD接GPIOA29
	5. 蓝牙BUART_RXD接GPIOA28
	6. BT供电P17跳针连接下面2PIN

软件使用说明:
	1.上电启动之后，打印如下信息，代表蓝牙已启动进入配对模式
		log如下：
			Realtek Patch down Success!
			AccesMode = 3
			
	2.手机端蓝牙可以进行搜索以及配对、连接、断开动作。

	3.有关蓝牙的BLE操作的相关Demo，请参照Demo_Ble_Peripheral。

	与蓝牙相关配置参数信息，详见"bt_config.h"