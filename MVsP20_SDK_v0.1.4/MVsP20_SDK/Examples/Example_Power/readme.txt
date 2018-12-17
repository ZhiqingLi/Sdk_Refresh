power_example工程主要演示了P20系列芯片的功耗。


硬件环境要求:
    1. P20系列开发板
    2. 外接串口小板，TX/RX/GND 接至 C2/C3/GND 引脚 (波特率 115200)

软件使用说明:
主要测试下面10种情况的功耗：
	1.关闭PLL，CPU用RC时钟，sleep功耗 
	2.PLL开环，CPU用RC时钟，sleep功耗 
	3.PLL闭环，CPU用RC时钟，sleep功耗 
	4.PLL闭环，CPU用PLL时钟，系统时钟10分频，sleep功耗 
	5.PLL闭环，CPU用PLL时钟，sleep功耗 
	6.PLL闭环，CPU用PLL时钟，while(1)功耗 
	7.PLL闭环，CPU用PLL时钟，UART循环打印字符时的功耗 
	8.PLL开环，CPU用PLL时钟，while(1)功耗 
	9.PLL关闭，CPU用RC时钟，while(1)功耗 
	10.DeepSleep功耗 

说明：


	