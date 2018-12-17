I2C_Example工程主要演示了P2 I2C读写24C04的流程。

硬件环境要求:
    1. P2系列开发板(可以是两块P20开发板，一主一从)
    2. 外接串口小板，TX/RX/GND 接至 C2/C3/GND 引脚 (波特率 115200)
    3. 通讯线接发，IIC_SCL/IIC_SDA/GND 接至 A30/A31/GND 引脚

软件使用说明:
	试验中是主从机相互传了16个数据：00-0F，传输成功后会有相应的打印信息显示成功
	上电启动之后，直接进入例程，在串口打印出观察打印信息；
	根据提示串口可知当前测试模式为主机或从机：
	主机与从机的测试模式有例程开头宏定义MASTER_SLAVE决定，从机是否使用中断收发由宏定义SLAVE_INT决定
	
特殊说明：
		客户在使用时，需要将使用的I2C相关引脚软件上配置上拉，或者硬件上将IO接上拉电阻
		在使用收发函数时需要注意：
		I2C_MasterSendBuffer(uint8_t SlaveAddr, void* SendBuf, uint32_t BufLen, uint32_t timeout)
		I2C_MasterReceiveBuffer(uint8_t SlaveAddr, void* RecvBuf, uint32_t BufLen, uint32_t timeout)
		此两个函数是不需要额外寄存器的主机收发方式
		
		I2C_MasterSendData(uint8_t SlaveAddr, uint8_t RegAddr, void* SendBuf, uint32_t BufLen, uint32_t timeout)
		I2C_MasterReceiveData(uint8_t SlaveAddr, uint8_t RegAddr, void* RecvBuf, uint32_t BufLen, uint32_t timeout)
		此两个函数是需要额外寄存器的主机收发方式
		
		注意i2c_interface.h里相关API函数说明。
