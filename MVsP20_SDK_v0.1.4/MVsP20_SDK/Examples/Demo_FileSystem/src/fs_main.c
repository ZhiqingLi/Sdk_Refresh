#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "irqn.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "clk.h"
#include "gpio.h"
#include "sd_card.h"
#include "timeout.h"
#include "watchdog.h"
#include "debug.h"
#include "spi_flash.h"
#include "timeout.h"
#include "cache_tcm.h"
#include "dma.h"

#include "fsinfo.h"
#include "fat_file.h"
#include "fs_mount.h"
#include "dir.h"
#include "folder.h"

#include "character_encoding.h"

 static FAT_FILE  TestFile;
 static FAT_FILE  TestFile2;
 static FOLDER	  RootFolder;
 static FOLDER	  TestFolder;
 static uint16_t LongName[FAT_NAME_MAX/2];
 static uint8_t FolderShortName[11] = {'1', '2', '3', '4', '5', '6', '~', '1', ' ', ' ', ' '};


 uint8_t SDIO_DmaChannelMap[27] =
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

uint8_t write_buf[128] = {'A', 'B', 'C', 'D', 'E', 'F'};
uint8_t read_buf[1024] = {0};

void case_create_file(void)
{
	static uint8_t loop_cnt = 8;
	if(FileOpen(&TestFile, "\\test1.txt", FA_CREATE_NEW|FA_WRITE))
	{
		//uint8_t cc[512] = {1,2,3};
		uint16_t len;
		FileFlush(&TestFile);
		DBG("\nCreate file success!\n");
		while(loop_cnt--)
		{
			len = FileWrite((void*)&write_buf, 1, 128, &TestFile);
			//FileFlush(&TestFile);
			DBG("Wrt: %d\n", len);
			FileSave(&TestFile);
		}
	}
	else
	{
		DBG("File already existed, or create failed...\n");
	}
}

void case_read_file(void)
{
	if(FileOpen(&TestFile, "\\read.txt", FA_READ))
	{
		if(FileRead((void*)&read_buf, 1, 1024, &TestFile))
		{
			DBG("\n\nRead data: %s\n", read_buf);
		}
		else
		{
			DBG("Read failed, empty file?\n");
		}
	}
	else
	{
		DBG("File not existed...\n");
	}
}


void case_delete_file(void)
{
	if(FileOpen(&TestFile, "\\test1.txt",FA_READ))
	{
		if(!FileDelete(&TestFile))
		{
			DBG("FileDelete() Failed!\n");
		}else
		{
			DBG("FileDelete() Success\n");
		}
	}
	else
	{
		DBG("test1.txt not exist\n");
	}
}

// create one long name folder named 中国        (Unicode码：0x4E2D,0x56FD 字节序：小端)
void case_create_folder(void)
{
	//number 1 is always root folder
	FolderOpenByNum(&RootFolder, NULL, 1);

	memset((void* )LongName, 0xFF, FAT_NAME_MAX);
	LongName[0] = 0x4E2D;
	LongName[1] = 0x56FD;
	if(FolderCreateByLongName(&TestFolder, &RootFolder, LongName, FolderShortName, 2))
	{
		DBG("LongNameFolder Create Success!\n");
		if(FolderOpenByLfName(&TestFolder, &RootFolder, (uint8_t *)LongName, 4))
		{
			DBG("folder open success\n");
			if(FileCreate(&TestFile, &TestFolder, "test.txt"))
			{
				DBG("test.txt create success\n");
			}
			else
			{
				DBG("test.txt create failed\n");
			}
		}
		else
		{
			DBG("folder open failed\n");
		}
	}
	else
	{
		DBG("LongNameFolder Create Failed!\n");
	}
}

void FileBrowse(FS_CONTEXT* FsContext)
{	
	uint8_t	EntryType;
	DirSetStartEntry(FsContext, FsContext->gFsInfo.RootStart, 0, TRUE);
	FsContext->gFolderDirStart = FsContext->gFsInfo.RootStart;
	while(1)
	{
		EntryType = DirGetNextEntry(FsContext);
		switch(EntryType)
		{
			case ENTRY_FILE:
				DBG("%-.11s  %d年%d月%d日 %d:%d:%d  %d 字节\n",
					FsContext->gCurrentEntry->FileName,
					1980+(FsContext->gCurrentEntry->CreateDate >> 9),
					(FsContext->gCurrentEntry->CreateDate >> 5) & 0xF,
					(FsContext->gCurrentEntry->CreateDate) & 0x1F,
					FsContext->gCurrentEntry->CreateTime >> 11,
					(FsContext->gCurrentEntry->CreateTime >> 5) & 0x3F,
					((FsContext->gCurrentEntry->CreateTime << 1) & 0x3F) + (FsContext->gCurrentEntry->CrtTimeTenth / 100),
					FsContext->gCurrentEntry->Size);
				break;
			case ENTRY_FOLDER:
				FsContext->gFsInfo.FolderNumInFolder++;
				break;
			case ENTRY_END:
				RewindFolderStart(FsContext);
				if(!FindNextFolder(FsContext))
				{
					return;
				}
				FsContext->gFsInfo.FolderSum++;
				FsContext->gFsInfo.FolderNumInFolder = 0;
				FsContext->gFsInfo.FileSumInFolder = 0;
				if(!DirEnterSonFolder(FsContext))
				{
					FsContext->gFsInfo.FolderSum--;
				}
				break;
			default:
				break;
		}
	}
}


int test_case = 4;

int main(void)
{	
//	 unsigned int i = 0;

	 //Disable watchDog
	WDG_Disable();

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

	//注意：晶体如果没有选择，则此时UART波特率不正确
	//GPIO_UartTxIoConfig(0, 0);
	//GPIO_UartRxIoConfig(0, 0);
	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);
	UARTS_Init(0, 115200, 8, 0, 1);

	SysTickInit();

	GIE_ENABLE();

	DBG("------------------------------------------------------\n");
	DBG("        FileSystem on SD card Operation Example              \n");
	DBG("       Mountain View Silicon Technology Co.,Ltd.               \n");
	DBG("------------------------------------------------------\r\n\r\n");

	GPIO_SDIOConfig(3);

	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);

	DBG("SDIO Memory Card Test\n");
	DMA_ChannelAllocTableSet(SDIO_DmaChannelMap);
	if(SDCard_Init() == 0)
	{
		DBG("SDCard Init Success!\n");
		if(FSInit(DEV_ID_SD))
		{
			DBG("FileSystem Init Success!\n");
		}
		else
		{
			DBG("UnKnow filesystem type!\n");
			goto error_ret;
		}
	}
	else
	{
		DBG("SdInit Failed!\n");
		goto error_ret;
	}

	FolderOpenByNum(&RootFolder, NULL, 1);
	FileBrowse(RootFolder.FsContext);
	
	switch (test_case)
	{
		case 0:
			case_create_file();
			break;
		case 1:
			case_read_file();
			break;
		case 3:
			case_delete_file();
			break;
		case 4:
			case_create_folder();
			break;
		
		
		default:
			break;
	}

error_ret:
	while(1);
	
	return 0;
}

