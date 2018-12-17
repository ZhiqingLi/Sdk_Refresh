/**
 **************************************************************************************
 * @file    flashfs_main.c
 * @brief   flash file system demo
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-11-30 11:30:00$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "clk.h"
#include "gpio.h"
#include "debug.h"
#include "delay.h"
#include "cache_tcm.h"
#include "timeout.h"
#include "uarts_interface.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "file.h"

#define   FILE_NAME       "test.txt"

void FlashFSWriteRead()
{
	uint8_t sendbuf[512];
	uint8_t readbuf[512];
	uint16_t cnt=0;
	FFILE *fp = NULL;

	memset(sendbuf, 0, 512);
	memset(readbuf, 0, 512);
	for(cnt=0; cnt<512; cnt++)
	{
		sendbuf[cnt] = (uint8_t)cnt;
	}

	if( (fp = Fopen( FILE_NAME, "w" )) != NULL )  //OPEN成功
	{
		DBG("FlashFS: open file success!\n");
		Fwrite( &sendbuf[0], 512, 1, fp );
		Fclose( fp );

		DBG("FlashFS: write data finished!\n");

		if( (fp = Fopen( FILE_NAME, "r" )) != NULL )  //如果读取无误
		{
			Fread(&readbuf[0], 512, 1, fp);
			Fclose( fp );
			DBG("FlashFS: read data finished!\n");

			for(cnt=0; cnt<512; cnt++)
			{
				if((cnt%16 == 0)&&(cnt != 0))
				{
					DBG("\n");
				}
				DBG("%02x ", readbuf[cnt]);
			}
			
			DBG("\n");
			if(memcmp(&sendbuf[0],&readbuf[0],512) == 0)//读出来的参数与写的参数一样
			{
				DBG("FlashFS: Save success!\n");
			}
			else
			{
				DBG("FlashFS: Save err!\n");
			}
		}
		else
		{
			 Fclose( fp );
			DBG("FlashFS: open file err!\n");
		}
	}
	else
	{
		Fclose( fp );
		DBG("FlashFS: open file err!\n");
	}

}


int main(void)
{
	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);
	
	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	//uart0 C2/C3
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);
	UARTS_Init(0,  115200, 8,  0,  1);

	WDG_Disable();

	DBG("\n");
	DBG("------------------------------------------------------\n");
	DBG("       Flash File System Demo              \n");
	DBG("       Mountain View Silicon Technology Co.,Ltd.      \n");
	DBG("------------------------------------------------------\n\n");
	
	FlashFSInit();       //FS文件系统初始化

	FlashFSWriteRead();
	while(1);
	return 0;
}


