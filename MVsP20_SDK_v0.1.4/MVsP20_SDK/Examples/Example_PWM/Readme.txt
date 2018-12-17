Example_PWM工程主要演示了P2 PWM的工作流程。

硬件环境要求:
    1. P2系列开发板
    2. 外接串口小板，TX/RX/GND 接至 C2/C3/GND 引脚 (波特率 115200)
    3. 上位机串口调试工具:SSCOM3.2
    4. 示波器

软件使用说明:
	上电启动之后，芯片初始化；	
	通过串口可以看到PWM模式选择,通过输入数字1~6选择不同的模式，如：
	-------------------------------------------------------
	Example Menu:
	1: Force output example            强制输出模式
	2: combined(OR) mode example       PWM'或'输出
	3: combined(AND) mode example      PWM'与'输出
	4: Asymmetric mode example         PWM同步输出
	5: One pulse output example        PWM单脉冲输出
	6: Break in and dead time example  死区及Break in功能
	-------------------------------------------------------
	