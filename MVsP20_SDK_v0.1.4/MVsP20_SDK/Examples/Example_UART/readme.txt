uart_example工程主要演示了P2 uart的数据收发基本功能

硬件环境要求:
    1. P2系列开发板
    2. 外接串口小板，TX/RX/GND 打印IO为   GPIOC2/GPIOC3引脚 按配置情况而定，波特率按配置情况而定


软件使用说明:
上电前先修改程序：
	1. uart_example里有两个main函数main1和main2，请选择其中的一个把函数名改为main
	2. 如果将main1改为main，测试的项目会是由PC端发送数据，然后P2开发板收到后会发送接收到的数据
 * 修改定义：#define USE_UART_INT			是否使用中断，可注释掉
 * 			#define TEST_UART_NUM			测试的串口编号
 * 			#define GPIO_MULTI_USE_TX 		选择串口TX引脚
 * 			#define GPIO_MULTI_USE_RX 		选择串口RX引脚	
	3.如果将main2改为main，则会测试uart的中断、DMA传输、RTS和CTS等功能，可修改参数
/*************************************************
 * 修改定义：#define TEST_UART_NUM			测试的串口编号
 * 			#define GPIO_MULTI_USE_TX 		选择串口TX引脚
 * 			#define GPIO_MULTI_USE_RX 		选择串口RX引脚
 * 			#define GPIO_MULTI_USE_RTS 		选择串口RTS引脚
 * 			#define GPIO_MULTI_USE_CTS 		选择串口CTS引脚
 * 注意：程序初始化UART0后会提示输入0000-4095范围内的字符，请输入四个字符，
 * 将这个数字按二进制格式分解就是波特率停止位等相关设置位，详见代码。
 ***************************************************/	
			A.首先配置信息打印串口，可自由设置串口0—2，引脚0-1
				GPIO_UartTxIoConfig(0, 1);//GPIO_C2
				GPIO_UartRxIoConfig(0, 1);//GPIO_C3
				UARTS_Init(0,  115200, 8,  0,  1);
				默认设置为UART0，数据收发脚选择GPIO_C2和GPIO_C3
			B.程序修改完毕下载后重新上电，PC端接受到提示输入数据，并要求输入0000-4096范围数字发送，
			     软件会按数字解析出每个配置 位去配置串口，接下来软件就会周期性的发送一串经CRC校验后的数
			     据，接收到的数据也会进行CRC校验，向外打印信息提示接收数据的情况。
			

	
