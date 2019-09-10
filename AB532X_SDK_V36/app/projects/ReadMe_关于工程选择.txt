1. standard工程:默认带7脚LED显示屏，蓝牙音箱方案一般在这个方案上开发。
2. earphone工程：蓝牙耳机专用工程，不支持U盘/SD/内置FM等(为了省功耗，已关闭相关时钟等)
3. karaok工程：K歌宝示例工程, karaok音箱方案也可以由standard工程开发。
4. 5327A_27C_25F工程：5327A,5327C,5325F这三颗芯片不支持内置SPIFLASH提速,这里单独列出来防止客户选错， 此工程与standard工程仅有一句宏SPIFLASH_SPEED_UP_EN 配不一样。