example_spis 工程主要演示了P20 spis的基本操作

1、串口引脚配置
    RXD为 GPIOB11
    TXD为 GPIOB10
   波特率115200，数据位8bit，停止位1bit

2、SPI引脚配置
  
	spis_miso      c[0]
	spis_clk(i)    c[1]
	spis_mosi(i)   c[2]
	spis_cs(i)     c[3]
   注意SPI的主机MOSI接从机的MOSI，MISO接从机的MISO，不能有错

3、spis_data.c指定了用于测试的数据，主机做测试请与此一致，以判断数据是否正确，第一个字节'S'用于通讯时同步用

4、SPIS_DMA_TEST为1测试主机通过DMA与从机测试的功能，否则为MCU方式与从机通讯，设置 SPIS_INTERRUPT_TEST为1则可以启用SPI的中断传输功能，用于判断传输是否完成及出错。中断相应例程的函数名必须为void SPIS_Interrupt(void)，与中断向量表中的函数名保持一致。
