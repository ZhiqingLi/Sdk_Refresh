Demo_Encoder工程主要演示了mic输入到DAC输出并且编码成msbc数据保存到TF卡。
示例程序代码运行于 BP10系列开发板。

硬件环境要求:
    1. 使用 BP10系列开发板，外接串口小板，TX/RX/GND 接至 A25/A24/GND 引脚 (波特率 256000)；
    2. SDCARD接口使用GPIOA5，GPIOA6，GPIOA7

功能：
1.演示MIC1，MIC2输入，DAC输出过程。
2.上电插入TF卡。
3.DEBUG打印口输入'r'，开始编码录音，再次按下'r'后，结束录音保存文件。

