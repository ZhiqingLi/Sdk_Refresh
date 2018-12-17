Example_PWC工程主要演示了P2 PWC的工作流程。

硬件环境要求:
    1. P2系列开发板
    2. 外接串口小板，TX/RX/GND 接至 C2/C3/GND 引脚 (波特率 115200)
    3. 上位机串口调试工具:SSCOM3.2
    4. 示波器

软件使用说明:
	上电启动之后，芯片初始化；	
	通过串口可以看到PWC工作模式选择,通过输入数字1~6选择不同的模式，如：
	-------------------------------------------------------
	Example Menu:
	1: Single mode example                 单次捕获模式
	2: Continue mode example               连续捕获模式
	3: simultaneous work mode example      两路PWC同时工作模式
	4: XOR mode example		       三路PWC异或输入模式
	-------------------------------------------------------