clk_example工程主要演示了P20 SDIO模块



注意：
GPIO复用关系


硬件环境要求:
    1. P20系列开发板
	2. 串口波特率115200 TX接GPIOB10
	3. SD_DAT-B20,SD_CLK-B21,SD_CMD-B22

软件使用说明:
	1. 上电启动之后,会自动初始化SD卡；
	2. 对SD卡进行读写操作；
	