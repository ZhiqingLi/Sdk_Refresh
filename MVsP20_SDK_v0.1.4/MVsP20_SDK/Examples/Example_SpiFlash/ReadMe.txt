example_spiflash 工程主要演示了P20 spi flash的读、写、擦除等操作

Flash读/写/擦除/加/解锁	使用说明：

所需硬件环境：
P2064F2_DEVELOPMENT_V1.0系列开发板
DC 9V-12V电源
串口配置：
TX: GPIOC2， RX: GPIOC3
115200 8 1 none


该例程中有两部分示例：
1，用于示例 Flash 写操作中，写数据的buf 的地址不能处于code空间，如果数据内容在code 空间，需要先将该数据从code空间复制到data空间的变量中，再进行Flash写操作
目前这部分代码被#if 0 屏蔽



2，用于示例Flash 读写、擦除、加解锁等操作，使用命令行交互的方式来进行操作, 命令使用ASCII码方式进行发送

示例代码中Flash 写/擦除的操作地址需从 0x1A0000+64K+16K = 0x1B4000 开始

命令行格式示例：
U				Flash解锁。
L range[1-4]    		Flash加锁。	加锁范围1-4,1：加锁1/2、2：加锁3/4、3：加锁7/8、4：加锁全部区域
				如：L 1 加锁一半Flash的区域

E address(hex.) length(dec.)	Flash擦除。	地址和长度都需要4KB对齐，如果命令中没有4KB对齐，那么驱动中会自动进行4KB对齐
				如：E 0x1b4000 4096，擦除地址0x1b4000开始，4KB长度的区域

W address data(string)		Flash写。	包含要写的数据在内，总命令行长度为1024
				如：W 0x1B4000 abcdefg 往0x1b4000的地址，写abcdefg数据

R address(hex) len(dec)		Flash读。	最大读取数据长度为1024
				如： R 0x1B4000 200 读取地址0x1b4000开始的200个字节的数据

?/help				打印help信息

