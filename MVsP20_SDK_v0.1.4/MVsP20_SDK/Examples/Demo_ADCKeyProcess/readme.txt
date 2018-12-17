Demo_ADCKeyProcess工程主要演示了P20系列芯片的ADC按键功能。


硬件环境要求:
    1. P20系列开发板
    2. 外接串口小板，TX/RX/GND 接至 C2/C3/GND 引脚 (波特率 115200)
    3. GPIOA3接线到J11上

软件使用说明:
    1. 上电启动之后，系统会不停查询是有ADC Key按下，通过UART输出；
    2. 按下KEY2-KEY11中的其中一个时，UART打印对应Key编号。
    3. 不支持组合按键。

说明：
    1、注意ADC复用管脚的选择，开发板默认没有接任何管脚，需要飞线链接到对应GPIO。
    2、如果要跟换复用管脚，需要修改2处代码。
 	a、void AdcKeyInit(void)函数内部，需要将对应的GPIO管脚配置为模拟输入。
	b、uint16_t AdcKeyGetIndex(uint8_t Channel)调用的地方，修改传递参数，参数见枚举变量 ADC_DC_CHANNEL_SEL
    3、ADC按键功能配置会比较灵活，客户需要根据实际情况调整，该工程仅仅是演示。

	