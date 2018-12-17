Demo_WIFI工程主要演示了P20系列芯片的WIFI协议栈功能。


硬件环境要求:
    1. P20系列开发板
    2. WIFI模块小板，WD0-B27,WCLK-B28,WCMD-B29,WD1-C7,WCS-C3(可不接),WK-C0
    3. 外接串口小板，TX/RX/GND 接至 C2/C3/GND引脚 (波特率 115200)

使用说明:

STA模式（wifi_mode == SSV6XXX_HWM_STA）：
    1、上电启动之后，配置UART相关IO，等待PC UART显示打印信息：“Wait for smartlink”；
    2、微信关注公众号“上海山景集成电路股份有限公司”，公众号->网络配置->开始配置->输入WIFI密码->连接，开始进行配网；
    3、配网成功后，UART会显示打印信息，显示当前IP地址
    4、解压缩《测试网页.zip》，打开test.html在页面填写命令“input cmd”,发送命令到设备，UART会显示接收到的命令,如：[input cmd]
AP模式（wifi_mode == SSV6XXX_HWM_AP）：
    1、上电启动之后，配置UART相关IO，等待UART显示初始化完成;
    2、手机或电脑连接WIFI热点"MV-CS-AP",初始密码“12345678”
    3、连接成功后，浏览器输入"192.168.25.1/LED"，打开"山景LED设置"页面
    4、页面点击"提交内容"，串口可查看提交的参数。


说明：
    1、WIFI的IO口配置在MVsP20_SDK\Middleware\wifi\host\platform\mvsilicon\porting.h头文件中
    2、用户可自己申请注册OneNet设备号，edp.c中修改设备deviceid与api_key即可，iot云地址：http://open.iot.10086.cn/
    3、可生成二维码扫一扫打开

	