Demo_MicApplication工程主要演示了mic输入到DAC输出同时录音到TF卡/U盘。
示例程序代码运行于 BP10xx系列开发板。


功能：
1.演示MIC1/2，MIC2输入，DAC输出过程。
2.上电插入TF卡，自动进行录音，将MIC PCM数据写入TF卡。
3.串口输入字符'r'，停止录音并存盘。


硬件环境要求:
    1. 使用BP10xx系列开发板，外接串口小板，TX/RX/GND 接至 A25/A24/GND 引脚 (波特率 256000)；
    2. 需要有SD卡接口，或USB接口以便插入U盘。

软件使用说明:
1、该工程带OS，一个task读取ADC数据并且送到DAC，同时将数据缓存到写卡fifo；另一个tash负责写卡，将ADC录音数据从缓存fifo中写入TF卡
2、工程中ADC,DAC采样率为16K。
