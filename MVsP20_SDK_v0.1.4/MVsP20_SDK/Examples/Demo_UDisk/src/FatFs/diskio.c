/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sd_card.h"
#include "gpio.h"
#include "clk.h"
#include "dma.h"
#include "ff.h"

#include "otg_host_udisk.h"
#include "otg_detect.h"

uint8_t FATFS_DmaChannelMap[27] =
{
		255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
		255,//PERIPHERAL_ID_SPIS_TX,		//1
		0,//PERIPHERAL_ID_SD_RX,			//2
		1,//PERIPHERAL_ID_SD_TX,			//3
		255,//PERIPHERAL_ID_SPIM0_RX,		//4
		255,//PERIPHERAL_ID_SPIM0_TX,		//5
		255,//PERIPHERAL_ID_SPIM1_RX,		//6
		255,//PERIPHERAL_ID_SPIM1_TX,		//7
		255,//PERIPHERAL_ID_UART0_RX,		//8
		255,//PERIPHERAL_ID_UART1_RX,		//9
		255,//PERIPHERAL_ID_UART2_RX,		//10
		255,//PERIPHERAL_ID_TIMER3,			//11
		255,//PERIPHERAL_ID_TIMER4,			//12
		255,//PERIPHERAL_ID_I2S_TX,			//13
		255,//PERIPHERAL_ID_I2S_RX,			//14
		255,//PERIPHERAL_ID_SARADC,			//15
		255,//PERIPHERAL_ID_UART0_TX,		//16
		255,//PERIPHERAL_ID_UART1_TX,		//17
		255,//PERIPHERAL_ID_UART2_TX,		//18
		255,//PERIPHERAL_ID_TIMER5,			//19
		255,//PERIPHERAL_ID_TIMER6,			//20
		255,//PERIPHERAL_ID_TIMER7,			//21
		255,//PERIPHERAL_ID_TIMER8,			//22
		255,//PERIPHERAL_ID_LACC_SRC,		//23
		255,//PERIPHERAL_ID_LACC_DST,		//24
		255,//PERIPHERAL_ID_LACC_DSTRD,     //25
		255,//PERIPHERAL_ID_SOFTWARE,		//26
};



/* Definitions of physical drive number for each drive */

#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_USB		1	/* Example: Map USB MSD to physical drive 1 */
#define DEV_RAM		2	/* Example: Map Ramdisk to physical drive 2 */



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;
//	int result;

	switch (pdrv) {
	case DEV_RAM :
//		result = RAM_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
//		result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
//		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
//	int result;
	static uint8_t flag = 1;

	switch (pdrv) {
	case DEV_RAM :
//		result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
//		result = MMC_disk_initialize();

		GPIO_SDIOConfig(3);
		Clock_ModuleEnable(SD_CLK_EN);
		printf("SDIO Card init...\n");
		DMA_ChannelAllocTableSet(FATFS_DmaChannelMap);
		if(SDCard_Init() == 0)
		{
			//printf("SDCard Init Success!\n");
			stat = 0;
		}
		else
		{
			printf("SdInit Failed!\n");
			stat = STA_NOINIT;
		}


		return stat;

	case DEV_USB :
			OTG_PortSetDetectMode(1,1);

			uint16_t linkCnt = 10;

			while(linkCnt--) OTG_PortLinkCheck();

			if(OTG_PortHostIsLink())
			{
				if(OTG_HostInit() == 1)//枚举成功
				{
					stat = 0;
				}
				else
				{
					printf("枚举MASSSTORAGE接口error\n");
					stat = STA_NOINIT;
					flag = 1;//失败允许再次初始化
				}
			}
			else//U盘未插入
			{
				printf("USB 设备未有插入\n");
				stat = STA_NOINIT;
				flag = 1;//失败允许再次初始化
			}
		// translate the reslut code here
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_ERROR;
//	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

//		result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

//		result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here
		if(!SDCard_ReadBlock(sector, buff, count))
		{
			res	= RES_OK;
		}
		else
		{
			printf("SDCard_ReadBlock err\n");
			res	= RES_ERROR;
		}
		return res;

	case DEV_USB :
		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

		res = UDiskReadBlock(sector, (void*)buff, count) ? RES_OK : RES_ERROR;
		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_ERROR;
//	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

//		result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

//		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here
		if(!SDCard_WriteBlock(sector, buff, count))
		{
			res	= RES_OK;
		}
		else
		{
			res	= RES_ERROR;
		}
		return res;

	case DEV_USB :
		// translate the arguments here

		res = UDiskWriteBlock(sector, (void*)buff, count) ? RES_OK : RES_ERROR;
		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
//	int result;

	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

DWORD get_fattime (void)
{
	/***********************************
		bit31:25 年 （0~127）（从1980 开始）
		bit24:21 月 （1~12）
		bit20:16 日 （1~31）
		bit15:11 小时（0~23）
		bit10:5 分钟 （0~59）
		bit4:0 秒       （0~29）
	************************************/
	return ((2017UL-1980) << 25) /* Year = 2010 */
	| (7UL << 21) /* Month = 7 */
	| (2UL << 16) /* Day = 2 */
	| (15U << 11) /* Hour = 15 */
	| (0U << 5) /* Min = 0 */
	| (0U >> 1) /* Sec = 0 */
	;
}
