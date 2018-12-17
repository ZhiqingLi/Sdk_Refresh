#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "watchdog.h"
#include "get_bootup_info.h"
#include "uarts_interface.h"
#include "debug.h"
#include "irqn.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "type.h"
#include "delay.h"
#include "string.h"
#include "otg_detect.h"
#include "timer.h"

#define UPDATE_ADDR 0X100000//双BANK升级文件存放写入位置


void Timer_OTG(void)
{
	static uint16_t count=0;
	OTG_PortLinkCheck();
	count++;
/*	if(count>3000)
	{
		count=0;
		if(OTG_PortHostIsLink())
		{
			DBG("U盘已连接\n");
		}
		else
		{
			DBG("U盘已拔出\n");
		}
	}*/
}

__attribute__((section(".driver.isr"))) void Timer4Interrupt(void)
{
	Timer_OTG();
	Timer_InterruptFlagClear(TIMER4, UPDATE_INTERRUPT_SRC);
}



#include "ff.h"
FATFS myfs[2]; /* 逻辑驱动器的工作区(文件系统对象) */
FIL fsrc, fdst; /* 文件对象 */
BYTE buffer[40960]={"0123456789;"}; /* 文件拷贝缓冲区 */

uint8_t FilseCopy(uint8_t * srcPath,uint8_t *destPath)//文件拷贝
{
	FRESULT res; /* FatFs 函数公共结果代码 */
	UINT br, bw; /* 文件读/写字节计数 */
	FIL fsrc, fdst; /* 文件对象 */

	QWORD curFsize = 0;//已经读文件大小
	UINT tmpPercent,prePercent=0;

	/* 打开驱动器 0 上的源文件 */
	res = f_open(&fsrc, (const char *)srcPath, FA_OPEN_ALWAYS   | FA_WRITE | FA_READ);
	if (res != FR_OK)
	{
		DBG("f_open  %s err!\n",srcPath);
		f_close(&fsrc);
		return res;
	}
	/* 在驱动器 1 上创建目标文件 */
	res = f_open(&fdst,(const char *)destPath, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	if (res != FR_OK)
	{
		DBG("f_open  %s err!\n",destPath);
		f_close(&fdst);
		return res;
	}

	DBG("File size = %dKB,[0%%",(int)(f_size(&fsrc)/1000));
	while(1)
	{
		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
		if (res || br == 0)
		{
			break; /* 文件结束错误 */
		}
		else
		{
//			DBG("READ:%s\n",buffer);
//			DBG("f_read OK\n");
		}

		res = f_write(&fdst, buffer, br, &bw);
		if (res || bw < br)
		{
			DBG("f_write err\n");
			break; /* 磁盘满错误 */
		}
		else
		{
			//DBG("f_write OK\n");
		}

		//以下为显示进度条
		curFsize += br;
		tmpPercent = (10*curFsize)/f_size(&fsrc);//计算已经读文件的百分比
		if(tmpPercent > prePercent)
		{
			prePercent = tmpPercent;
			DBG("..%d%%",tmpPercent*10);//显示当前百分比,只显示10%，20%等
		}
	}
	f_close(&fsrc);
	f_close(&fdst);
	DBG("]\n");
   return FR_OK;
}






//遍历文件并拷贝
//path:路径
//返回值:执行结果
uint8_t SD_USB_Copy(uint8_t * srcPath,uint8_t *destPath)
{
	FRESULT res;
	FILINFO fileinfo;
	DIR dir;
	uint16_t np,desNP;

	res = f_opendir(&dir,(const TCHAR*)srcPath); //打开一个目录
	if (res == FR_OK)
	{
		np = strlen((const char *)srcPath);
		desNP = strlen((const char *)destPath);
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
			//if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
			DBG("%s/%s\n", srcPath,fileinfo.fname);//打印路径和文件(夹)名

			if(fileinfo.fattrib & AM_DIR)//目录
			{
                *(srcPath + np) = '/';
                strcpy((char *)(srcPath + np + 1), &fileinfo.fname[0]);
                *(destPath + desNP) = '/';
                strcpy((char *)(destPath + desNP + 1), &fileinfo.fname[0]);

				res=f_mkdir((const char *)destPath);
				if (res == FR_OK)
				{
					DBG("f_mkdir ok:%s!\n",destPath);
				}
				else//文件夹已存在或创建文件夹失败
				{

				}

                res = SD_USB_Copy(srcPath,destPath);//递归搜索
                *(srcPath + np) = '\0';
                *(destPath + desNP) = '\0';
                if(res != FR_OK) break;
			}
			else//文件
			{
                *(srcPath + np) = '/';
                strcpy((char *)(srcPath + np + 1), &fileinfo.fname[0]);//将文件名添加到路径
                *(destPath + desNP) = '/';
                strcpy((char *)(destPath + desNP + 1), &fileinfo.fname[0]);

//                char* pExt = strrchr((const char *)srcPath,'.');//获取后缀
//                if(strcasecmp(pExt,".mp3") == 0
//                  || strcasecmp(pExt,".doc") == 0
//				  || strcasecmp(pExt,".docx") == 0
//				  || strcasecmp(pExt,".zip") == 0
//				  || strcasecmp(pExt,".rar") == 0)//后缀名判断（忽略大小写）,全盘拷贝时，可以将此处注释掉
                {
					uint32_t t0,t1;

					t0 = GetSysTick1MsCnt();
					res = FilseCopy(srcPath,destPath);//拷贝文件
					t1 = GetSysTick1MsCnt();//计算拷贝文件所用时间，单位ms

					*(srcPath + np) = '\0';
					*(destPath + desNP) = '\0';

					if(res != FR_OK)
					{
						DBG("Copy File err\n");
						break;
					}
					else
					{
						DBG("Copy File ok,time = %dms\n",(int)(t1-t0));
					}
                }

			}
		}
	}
	f_closedir(&dir);

	return res;
}


void fatfs_test (void)//FATFS文件系统实现 SD卡与U盘互拷测试
{

	//vTaskDelay(100);//上电延时，否则USB初始化可能失败

	/* 为逻辑驱动器注册工作区 */
#define   USB_PATH          "USB:"
#define   SD_PATH           "SD:"

	f_mount( &myfs[0],USB_PATH,1);//挂载
	f_mount( &myfs[1],SD_PATH,1);


	uint8_t *psrcPath = malloc(2048);//动态申请内存，用于保存路径
	uint8_t *pdestPath = malloc(2048);

	memset(psrcPath,0,2048);
	memset(pdestPath,0,2048);
	strcpy((char *)psrcPath,(const char *)USB_PATH);//拷贝源地址
	strcpy((char *)pdestPath,(const char *)SD_PATH);//拷贝目的地址

	SD_USB_Copy(psrcPath,pdestPath);//SD卡、USB互拷,用的递归法要注意栈溢出

	free(psrcPath);
	free(pdestPath);

	f_mount(NULL,SD_PATH,1);
	f_mount(NULL,USB_PATH,1);


	DBG("END\n");
}

void UDisk_Upgrade(uint8_t * MvaPath)
{

	FRESULT res;
	FILINFO fileinfo;
	DIR dir;
	FIL fsrc;
	UINT br;
	uint32_t np,len;
	uint8_t read_buf[4096];
	uint8_t *Path = malloc(2048);
	strcpy((char *)Path,(const char *)MvaPath);
	np = strlen((const char *)Path);
	res = f_opendir(&dir,(const TCHAR*)Path); //打开一个目录
	if (res == FR_OK)
	{
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
			if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
			DBG("%s/%s\n",Path,fileinfo.fname);				//打印路径和文件(夹)名
			if((fileinfo.fattrib & AM_DIR)==0x00)				//只校验文件（文件夹直接忽略）
			{
				char* pExt = strrchr((const char *)fileinfo.fname,'.');//获取后缀
				 if(strcasecmp(pExt,".MVA") == 0)
				 {
					 *(Path + np) = '/';
					 strcpy((char *)(Path + np + 1), &fileinfo.fname[0]);//将文件名添加到路径
					 res = f_open(&fsrc, (const char *)Path, FA_OPEN_ALWAYS | FA_READ);
					 if(res == FR_OK)
					 {
						 DBG("打开文件%s成功\n",fileinfo.fname);
						 len = fileinfo.fsize;
						 uint32_t i=0;
						 for(i=0;i<(fileinfo.fsize/4096);i++)
						 {
							 f_read(&fsrc,read_buf,4096,&br);//每次读取4K数据
							 SpiFlashErase(SECTOR_ERASE,UPDATE_ADDR/0x1000+i,0);
							 SpiFlashWrite(UPDATE_ADDR+i*4096,read_buf,4096,10);
						 }
						 if((fileinfo.fsize%4096)!=0)
						 {
							 res = f_read(&fsrc,read_buf, fileinfo.fsize%4096, &br);
							 SpiFlashErase(SECTOR_ERASE,UPDATE_ADDR/0x1000+i,0);
							 SpiFlashWrite(UPDATE_ADDR+i*4096,read_buf,fileinfo.fsize%4096,10);
						 }
						 DBG("MVA文件写入OK！\n");
						 DBG("双BANK升级将复位重启！\n");
						 *(volatile uint32_t *)0x4002E800 = 0xFF;
						 *(volatile uint32_t *)0x4002E824 = UPDATE_ADDR;
						 *(volatile uint32_t *)0x4002E800 = 0;
						 *(volatile uint32_t *)0x40022008 = 1;
						 *(volatile uint32_t *)0x40022008 = 0;
					 }
					 else
					 {
						 DBG("打开文件%s失败\n",fileinfo.fname);
					 }
					 f_close(&fsrc);//关闭文件
				 }
			}
		}
	}
	else
	{
		DBG("打开目录%s失败\n",MvaPath);
	}
	f_closedir(&dir);
	return res;
}



const char * fileData = "U盘读写文件测试:在U盘根目录下创建Udisk.txt文件，并写入本段文字;";
uint8_t UDisk_FileTest(uint8_t * srcPath)
{
	FRESULT res; /* FatFs 函数公共结果代码 */
	UINT br, bw; /* 文件读/写字节计数 */
	FIL fsrc; /* 文件对象 */

	/* 驱动器  上创建源文件 */
	res = f_open(&fsrc, (const char *)srcPath, FA_CREATE_ALWAYS   | FA_WRITE | FA_READ);
	if (res != FR_OK)
	{
		DBG("f_open  %s err!\n",srcPath);
		f_close(&fsrc);
		return res;
	}

	do
	{
		/****写文件**/
		br = strlen(fileData);
		res = f_write(&fsrc, fileData, br, &bw);
		if (res || bw < br)
		{
			DBG("f_write err\n");/* 磁盘满错误 */
			break;
		}
		else
		{
			DBG("f_write OK\n");
		}

		/****读文件**/
		f_lseek(&fsrc,0);//指向文件头

		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
		if (res || br == 0)
		{
			DBG("READ ERR\n");
			break; /* 文件结束错误 */
		}
		else
		{
			DBG("Read: %s\n",buffer);//文件内容打印
			DBG("f_read OK\n");
		}
	}
	while(0);
	f_close(&fsrc);

   return FR_OK;
}

/*
void fatfs_USB (void)//FATFS文件系统实现 U盘测试
{
#define   USB_PATH          "USB:"

	f_mount( &myfs[0],USB_PATH,1);//挂载


	UDisk_FileTest("USB:Udisk.txt");

	f_mount(NULL,USB_PATH,1);

	DBG("END\n");
}
*/

void UDisk_Fatfs_Test()
{
	if(OTG_PortHostIsLink())
	{
		DBG("U盘已连接\n");
#ifndef USB_PATH
	#define   USB_PATH          "USB:"
#endif
	FATFS fs;
	f_mount( myfs,USB_PATH,1);//挂载文件系统
	UDisk_FileTest("USB:Udisk.txt");//检查U盘中是否存在Udisk.txt文件，存在该文件即打印文件内容否则创建该文件并写入内容
	UDisk_Upgrade(USB_PATH);//在U盘中查找是否存在MVA格式文件，存在则将文件读入flash进行双BANK代码升级
	f_mount(NULL,USB_PATH,1);
	DBG("END\n");
	}
	else
	{
		DBG("U盘未连接\n");
	}
}







void Udisk_Iint(void)
{
	OTG_PortSetDetectMode(1,1);
}


void TIMER4_INIT(void)
{
	NVIC_EnableIRQ(TMR4_IRQn);
	Timer_Config(TIMER4, 1000, 0);
	Timer_Start(TIMER4);
}


void InitZiArea(void)
{
	/* Use compiler builtin and memset */
	#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
	return;
}

int main()
{
	WDG_Disable();

	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 16/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
	InitZiArea();

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);
	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);
	mem_init();
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);
	DbgUartInit(0,  115200, 8,  0,  1);

	SysTickInit();
	GIE_ENABLE();

	NVIC_EnableIRQ(SWI_IRQn);

	DBG("****************************************************************\n");
	DBG("               MVsP2_UDiskUpgrade_Demo 2018 MVSilicon           \n");
	DBG("****************************************************************\n");
	TIMER4_INIT();
	Udisk_Iint();
	DelayMs(1000);
	//UDisk_Upgrade();
	//fatfs_USB();
	while(1)
	{
		UDisk_Fatfs_Test();
		DelayMs(1000);
	}

}

