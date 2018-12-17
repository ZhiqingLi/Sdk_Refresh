I2S_Example工程主要演示了I2S 基本功能

硬件环境要求:
    1. P2系列开发板
    2. 外接串口小板，TX/RX/GND 接至 B10/B11/GND 引脚 (波特率 115200)
    3. DU361 DEMO板
	4. LRCLK-B12, BCLK-B13, DOUT-B14, DIN-B15

软件使用说明:
	1. 上电运行程序后，默认进入I2S Slave模式。
	2. 通过UART输入"m"或"M"，I2S切换到I2S Master模式；
	3. 通过UART输入"s"或"S"，I2S切换到I2S Slave模式；
	