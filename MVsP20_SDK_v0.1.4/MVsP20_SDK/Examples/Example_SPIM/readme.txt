example_spim 工程主要演示了P20 spim的基本操作

1、串口引脚配置
    RXD为 GPIOB11
    TXD为 GPIOB10
   波特率115200，数据位8bit，停止位1bit

2、SPI引脚配置,通过函数GPIO_Spim0IoConfig(0)，其他参数请查看头文件的说明
  
	spim0_miso     b[0]
        spim0_clk      b[1]
        spim0_mosi     b[2]
   注意SPI的主机MOSI接从机的MOSI，MISO接从机的MISO，不能有错

3、spim_interface.c指定了用于测试的数据TestData[4096]，从机做测试请与此一致，以判断数据是否正确，第一个字节'S'用于通讯时同步用

4、Spim0DataTransfer展示通过MCU及DMA两种方式发送数据的示例，Spim0InterruptTest则打开SPIOM的中断，重新实现void SPIM0_Interrupt(void)以检测SPI0M的数据传输情况及是否出错。
