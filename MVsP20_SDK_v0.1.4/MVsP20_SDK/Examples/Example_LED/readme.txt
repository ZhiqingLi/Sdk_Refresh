Example_LED工程主要演示了P2 LED模块功能驱动点阵屏，其中运用到了FDMA模块和LACC模块

硬件环境要求:
    1. P2系列开发板
    2. P10点阵屏至少一块(本例程中使用4块串联)
    3. 外接串口小板，TX/RX/GND 引脚 按配置情况而定，波特率按配置情况而定，目前采用TX-GPIOC2 RX-GPIOC3
    4. LED硬件接口：串行控制
    				CLKOutIO-GPIOB0
    				STBIO-GPIOA16
    				OENIO-GPIOA17
    				ALineSelIO-GPIOA24
    				BLineSelIO-GPIOA25
    				
软件使用说明:
软件在系统时钟和串口初始化后会运行show_roll_text_dw();	次函数就是LED刷屏函数。
注意：次函数是以P10点阵屏为基本单元设计
#define EXTEND_BRD_NUM  这个宏定义表示点阵板子的数量，注意数量不要超过512，否则在最高的刷屏频率下也会出现闪烁感
#define TST_ONE_FRAME_LEN 单块点阵板子的数据量，P10是64
#define TST_ONE_FRAME_LEN_DY 64*EXTEND_BRD_NUM全屏刷一次数据量
然后可修改的参数即LED_CONFIG cfg;可以在此修改LED控制参数，时钟周期，时钟占空比等；
配置时注意：
	1.OENCnt的配置为扫描一行数据的总数，譬如1/4扫描，以P10板子16行为例，即同时显示4行，即4*32*EXTEND_BRD_NUM。
	2.OENValidTime有效亮度时间改变会直接影响亮度大小，该值的设置应<OENCnt*dataClkOutCycle,避免出现残影。
	3.如果模式设置为并行输出，则clkOutHighTime<dataClkHighTime。
	
程序运行过程中打开FDMA_THRESHOLD_INT中断，数据传输一半后中断写入另一半。全部传输完毕后静态显示30ms然后移动1bit，
数据移动使用LACC模块搬运，加速搬运。至于移动速度可以修改定时器计时参数。移动方向和移动位数详见程序内功能函数注释。

		