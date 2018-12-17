/**
 **************************************************************************************
 * @file    SpiFlashExample.c
 * @brief   Spi Flash operation example
 * 
 * @author  Lujiangang
 * @version V1.0.0 	initial release
 * 
 * $Id$
 * $Created: 2016-10-16 17:31:10$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

 //这部分代码用于示例Flash 读写、擦除、加解锁等操作，
 //使用命令行交互的方式来进行操作, 命令使用ASCII码方式进行发送
 
 //注意: Flash 写操作中，写数据的buf 的地址不能处于code空间，如果数据内容在code 空间
 //需要先将该数据从code空间复制到data空间的变量中，再进行Flash写操作。


#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "clk.h"
#include "gpio.h"
#include "timeout.h"
#include "watchdog.h"
#include "debug.h"
#include "spi_flash.h"
#include "delay.h"
#include "cache_tcm.h"

#define APP_DBG DBG
extern char EILM_SIZE;
#define TEST_FLASH_WR_ADDR_BEGIN  &EILM_SIZE
#define MAX_RDBUF_LEN	1024
#define MAX_CMD_LEN	1024
#define TIMEOUT_200MS 200
#define TIMEOUT_100MS 100

//FLASH_NONE_ERROR 为0， Flash 驱动中错误码都为负数
#define ERR_ILLEGAL_PARA	1
#define ERR_VERIFY_FAIL		2
#define ERR_ILLEGAL_ADDR	3


char * test_string = "hello world!\r\n";
uint8_t rd_buf[MAX_RDBUF_LEN];
SPI_FLASH_INFO*  FlashInfo;


#define SKIP_SPACE(p) do{\
	while((*p)== ' ' )	\
		{p++;}			\
}while(0)


int32_t cmd_receive(uint8_t* buf, uint32_t BufLen, uint32_t Wait)
{
	int32_t cnt = 0;
	TIMER t;
	TimeOutSet(&t, Wait);
	while(!IsTimeOut(&t))
	{
		int32_t n = 0;
		n = UART0_Recv(buf, BufLen);
		buf += n;
		BufLen -= n;
		cnt += n;
	}
	return cnt;
}

int32_t FlashErase(uint32_t Offset,uint32_t Size)
{
	int32_t i, Round;

	if(FlashInfo->Capacity < Offset + Size)
		return ERR_ILLEGAL_ADDR;

	if(Size >= 64 * 1024 && FlashInfo->Mid != FLASH_PCT){
		if(Offset & (0x10000 - 1)){
			/*
			 * sector size align
			 */
			if(Size & (0x1000 - 1))
				Size = ((Size >> 12) + 1) << 12;

			if(Offset & (0x1000 - 1)){
				Offset = (Offset >> 12) << 12;
				Size += 0x1000;
			}

			Round = (((0x10000 - Offset) & 0xFFFF)) >> 12;
			for(i = 0;i < Round;i ++){
				SpiFlashErase(SECTOR_ERASE, Offset / 0x1000,0);
				Size -= 0x1000;
				Offset += 0x1000;
			}
		}

		if(Size >= 64 * 1024){
			Round = Size >> 16;
			for(i = 0;i < Round;i ++){
				SpiFlashErase(BLOCK_ERASE,Offset / 0x10000,0);
				Size -= 0x10000;
				Offset += 0x10000;
			}
		}
	}

	/*
	 * sector size align
	 */
	if(Size & (0x1000 - 1))
		Size = ((Size >> 12) + 1) << 12;

	if(Offset & (0x1000 - 1)){
		Offset = (Offset >> 12) << 12;
		Size += 0x1000;
	}

	Round = Size >> 12;
	for(i = 0;i < Round;i ++){
		SpiFlashErase(SECTOR_ERASE, Offset / 0x1000,0);
		Offset += 0x1000;
	}

	return 0;
}


bool verify(uint8_t* buf1, uint8_t* buf2, uint32_t len)
{
	uint32_t i;
	for(i=0; i<len; i++)
	{
		if(buf1[i] != buf2[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}


	
int32_t do_cmd_erase(uint8_t* buf, uint32_t len)
{
	int32_t ret;
	int32_t offset;
	char *p = (char*)buf;
	char * tmp;
	
	SKIP_SPACE(p);
	offset = strtol((const char *)p,&tmp,16);
	p = tmp;
	SKIP_SPACE(p);
	ret = strtol((const char *)p,&tmp,10);
	
	if(offset < (int32_t)TEST_FLASH_WR_ADDR_BEGIN || offset > (int32_t)FlashInfo->Capacity)
	{
		DBG("Bad parameter, ");
		return ERR_ILLEGAL_PARA;
	}
	ret = FlashErase(offset, ret);
	return ret;
}

int32_t do_cmd_write(uint8_t* buf, uint32_t len)
{
	int32_t ret;
	int32_t offset,w_len;
	char *p = (char*)buf;
	char * tmp;
	
	SKIP_SPACE(p);
	offset = strtol((const char *)p,&tmp,16);
	p = tmp+1;
	
	w_len = strlen(p);

	if(offset < (int32_t)TEST_FLASH_WR_ADDR_BEGIN || offset + w_len > (int32_t)FlashInfo->Capacity)
	{
		DBG("Bad parameter, ");
		return ERR_ILLEGAL_PARA;
	}
	if((ret = SpiFlashWrite(offset, (uint8_t*)p, w_len, 20)) != FLASH_NONE_ERR)
	{
		DBG("Write Failed, erase Flash and retry!\r\n");
		return ret;
	}
	
	memset(rd_buf, 0, MAX_RDBUF_LEN);
	SpiFlashRead(offset, rd_buf, w_len, 20);
	
	if(verify((uint8_t*)p, rd_buf, w_len))
	{
		ret = FLASH_NONE_ERR;
	}
	else 
	{
		ret = ERR_VERIFY_FAIL;
	}
	
	return ret;
}

int32_t do_cmd_read(uint8_t* buf, uint32_t len)	//(uint32_t offset, uint8_t* buf, uint32_t buf_len)
{
	int32_t ret, i;
	int32_t offset;
	uint32_t rd_len=0;
	char *p = (char*)buf;
	char * tmp;

	SKIP_SPACE(p);
	offset = strtol((const char *)p,&tmp,16);
	p = tmp;
	SKIP_SPACE(p);
	rd_len = strtol((const char *)p,&tmp,10);

	if(offset+rd_len > FlashInfo->Capacity || rd_len > MAX_RDBUF_LEN || rd_len < 1)
	{
		DBG("Bad parameter, ");
		return ERR_ILLEGAL_PARA;
	}
	if((ret = SpiFlashRead(offset, rd_buf, rd_len, 20)) != FLASH_NONE_ERR)
	{
		return ret;
	}
	
	DBG("Flash read at address 0x%08x, length=%d :\r\n", offset,  rd_len);
	for(i=0;i<rd_len;i++)
	{
		DBG("%02X ",rd_buf[i]);
		
	}
	DBG("\r\n");
	return ret;
}

int32_t do_cmd_unlock(void)
{
	int32_t ret;
	char cmd[3] = "\x35\xBA\x69";
	
	// unlock flash
	ret = SpiFlashIOCtrl(IOCTL_FLASH_UNPROTECT, cmd, sizeof(cmd));
	return ret;
}

int32_t do_cmd_lock(uint8_t* buf, uint32_t len)
{
	int32_t ret;
	char *tmp;
	SPI_FLASH_LOCK_RANGE range;
	if(len>=2)
	{
		SKIP_SPACE(buf);
		range = (SPI_FLASH_LOCK_RANGE)strtol((const char *)buf,&tmp,10);
	}
	else
	{
		DBG("Bad parameter, ");
		return ERR_ILLEGAL_PARA;
	}

	if(range<1 || range>4)
	{
		DBG("Bad parameter, ");
		return ERR_ILLEGAL_PARA;
	}
	
	ret = SpiFlashIOCtrl(IOCTL_FLASH_PROTECT, range);
	
	return ret;
}

void do_cmd_help()
{
	DBG("\r\n【Comand line syntax】:\r\n");
	DBG("Flash Erase:	E address(hex) length(dec) address & length will be 4KB aligned\r\n	 \
					eg. E 0x1B4000 4096\r\n");
	DBG("Flash Unlock:	U				eg. U\r\n");
	DBG("Flash Lock:	L lock_range[1-4]		eg. L 2\r\n");
	DBG("Flash Write:	W address data(string)		eg. W 0x1B4000 abcdefg(total command line is 1024 bytes)\r\n");
	DBG("Flash Read:	R address(hex) len(dec)		eg. R 0x1B4000 200(Maximum read length is 1024 bytes)\r\n");
	DBG("Help:		?				print this help\r\n");
}


void GetFlashGD(int32_t protect)
{    
	uint8_t  str[20];
	int32_t FlashCapacity = 0;
		
	switch(FlashInfo->Did)
	{
		case 0x1340:	
			strcpy((char *)str,"GD25Q40(GD25Q40B)");
			FlashCapacity = 0x00080000;
			break;

		case 0x1240:
        	strcpy((char *)str,"GD25Q20(GD25Q20B)");
			FlashCapacity = 0x00040000;
			break;       

		case 0x1540:
			strcpy((char *)str,"GD25Q16(GD25Q16B)");
			FlashCapacity = 0x00200000;			
			break; 

		case 0x1640:
        	strcpy((char *)str,"GD25Q32(GD25Q32B)");
			FlashCapacity = 0x00400000;        
			break;

		case 0x1740:
        	strcpy((char *)str,"GD25Q64B");
			FlashCapacity = 0x00800000;          
			break;

		case 0x1440:
        	strcpy((char *)str,"GD25Q80(GD25Q80B)");
			FlashCapacity = 0x00100000;         
			break;

		case 0x1840:
            strcpy((char *)str,"GD25Q128B");
            FlashCapacity = 0x01000000;         
            break;

		default:
			break;
	}
        
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n", FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }
}

void GetFlashWinBound(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo->Did)
    {
        case 0x1440:
            strcpy((char *)str,"W25Q80BV");
            FlashCapacity = 0x00100000;             
            break;
        case 0x1540:
			strcpy((char *)str,"W25Q16");
			FlashCapacity = 0x00200000;
			break;
        
        case 0x1760:
            strcpy((char *)str,"W25Q64DW");
            FlashCapacity = 0x00800000;             
            break;
        				
        case 0x1740:
            strcpy((char *)str,"W25Q64CV");
            FlashCapacity = 0x00800000; 
            break;
        
        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n",FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }    
}

void GetFlashPct(void)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo->Did)
    {
        case 0x0126:
            strcpy((char *)str,"PCT26VF016");
            FlashCapacity = 0x00200000;        
			break;

        case 0x0226:       
            strcpy((char *)str,"PCT26VF032");
            FlashCapacity = 0x00400000;
            break;

        default:            
			break;
    }
      
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n",FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }  
}

void GetFlashEon(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
	switch(FlashInfo->Did)
    {
        case 0x1430:
            strcpy((char *)str,"EN25Q80A");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1530:
            strcpy((char *)str,"EN25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1830:
            strcpy((char *)str,"EN25Q128");
            FlashCapacity = 0x01000000; 
            break;

        case 0x1630:
            strcpy((char *)str,"EN25Q32A");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1330:
            strcpy((char *)str,"EN25Q40");
            FlashCapacity = 0x00080000; 
            break;

        case 0x1730:
            strcpy((char *)str,"EN25Q64");
            FlashCapacity = 0x00800000; 
            break;

        case 0x1570:
            strcpy((char *)str,"EN25QH16");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1670: 
            strcpy((char *)str,"EN25QH32");
            FlashCapacity = 0x00400000; 
            break;

        default:
            break;
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n",FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }
}

void GetFlashBg(int32_t protect)
{	
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
    
    switch(FlashInfo->Did)
    {
        case 0x1540:
            strcpy((char *)str,"BG25Q16A");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1340:
            strcpy((char *)str,"BG25Q40A");
            FlashCapacity = 0x00080000; 
            break;
        
        case 0x1440:
            strcpy((char *)str,"BG25Q80A");
            FlashCapacity = 0x00100000; 
            break;
        
        default:
			break;         
    }
    
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n",FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }
}

void GetFlashEsmt(int32_t protect)
{
    uint8_t  str[20];
	int32_t FlashCapacity = 0;
       
	switch(FlashInfo->Did)
    {
        case 0x1440:
            strcpy((char *)str,"F25L08QA");
            FlashCapacity = 0x00100000; 
            break;

        case 0x1540:
            strcpy((char *)str,"F25L16QA");
            FlashCapacity = 0x00200000; 
            break;

        case 0x1641:
            strcpy((char *)str,"F25L32QA");
            FlashCapacity = 0x00400000; 
            break;

        case 0x1741:
            strcpy((char *)str,"F25L64QA");
            FlashCapacity = 0x00800000; 
            break;
              
        default:
            break;
    }    
    
    if(FlashCapacity > 0)
    {
        APP_DBG("型号:                         ");
        APP_DBG("%s\r\n",str);
        APP_DBG("容量:                         ");
        APP_DBG("0x%08X\r\n",FlashCapacity);
    }  
    else
    {
        APP_DBG("查找失败\r\n");
    }
}

void GetDidInfo(int32_t protect)
{
	APP_DBG("%-30s","Did:");
	APP_DBG("0x%08X\r\n",FlashInfo->Did);
	APP_DBG("%-30s","加锁范围(BP4~BP0:Bit4~Bit0):");
	APP_DBG("0x%08X\r\n",protect);
}

void PrintFlashInfo(void)
{
	int32_t protect = 0;
   
	APP_DBG("\r\n\r\n****************************************************************\n");
        APP_DBG("%-30s\r\n","Flash信息");

//	if(FlashInfo->Mid != FLASH_PCT)
//	{
//		protect = SpiFlashIOCtrl(3,0);
//		protect = (protect >> 2) & 0x1F;
//	}

    switch(FlashInfo->Mid)
    {
        case FLASH_GD:
            APP_DBG("厂商:                         GD\r\n");
			GetDidInfo(protect);
            GetFlashGD(protect);
            break;
        
        case FLASH_WINBOUND:
            APP_DBG("厂商:                         WINBOUND\r\n");
			GetDidInfo(protect);
            GetFlashWinBound(protect);
            break;
        
        case FLASH_PCT:
            APP_DBG("厂商:                         PCT\r\n");
            GetFlashPct();
            break;
        
        case FLASH_EON:            
            APP_DBG("厂商:                         EN\r\n");
			GetDidInfo(protect);
            GetFlashEon(protect);
            break;
        
        case FLASH_BG:
            APP_DBG("厂商:                         BG\r\n");
			GetDidInfo(protect);
            GetFlashBg(protect);
            break;
        
        case FLASH_ESMT:
            APP_DBG("厂商:                         ESMT\r\n");
			GetDidInfo(protect);
            GetFlashEsmt(protect);
            break;
        
        default:            
            APP_DBG("厂商:                         not found\r\n");
            break;
    }
	APP_DBG("\r\n");
	APP_DBG("****************************************************************\n");
}

//Flash 解锁
//参数: 无
//返回值: 解锁成功返回TRUE，否则返回FALSE
bool FlashUnlock(void)
{
	char cmd[3] = "\x35\xBA\x69";
	
	if(SpiFlashIOCtrl(IOCTL_FLASH_UNPROTECT, cmd, sizeof(cmd)) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
}

#if 0
//Flash加锁
//参数:lock_range ，Flash加锁范围: 
//		 FLASH_LOCK_RANGE_HALF : 			加锁1/2 Flash 空间(从0开始，下同)
//		FLASH_LOCK_RANGE_THREE_QUARTERS: 	加锁3/4 Flash 空间
//		FLASH_LOCK_RANGE_SEVENTH_EIGHT:	加锁7/8 Flash 空间
//		FLASH_LOCK_RANGE_ALL:				加锁全部Flash 空间
//注意: 需要根据定义的USER BANK 的地址来决定加锁的范围，否则可能将USER bank空间加锁而无法写数据
//返回值: 加锁成功返回TRUE，否则返回FALSE
bool FlashLock(SPI_FLASH_LOCK_RANGE lock_range)
{
	if(SpiFlashIOCtl(IOCTL_FLASH_PROTECT, lock_range) != FLASH_NONE_ERR)
	{
		return FALSE;
	}

	return TRUE;
}
#endif




int main(void)
{	
	WDG_Disable();
	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	GPIO_UartTxIoConfig(0, 1);
	GPIO_UartRxIoConfig(0, 1);

	UARTS_Init(0,  115200, 8,  0,  1);

	SysTickInit();

	APP_DBG("------------------------------------------------------\n");
	APP_DBG("             SPI FLASH Operation Example                      \n");
	APP_DBG("       Mountain View Silicon Technology Co.,Ltd.               \n"); 
	APP_DBG("------------------------------------------------------\r\n\r\n");


	//打印Flash信息
	FlashInfo = SpiFlashInfoGet();
	PrintFlashInfo();
	
	

#if 0
	{
		int32_t ret, len;
		//该代码用于示例SpiFlashWrite()函数的参数buf不能来自code空间
		ret = FlashErase(TEST_FLASH_WR_ADDR_BEGIN, 4 * 1024);
		if(ret != 0 )
			APP_DBG("Flash erase error!\r\n");

		{
				uint8_t buf[100];
				len = strlen(test_string);
				memset(buf, 0, 100);
				//SpiFlashWrite()不允许buf来自code空间，test_string在code 空间，所以会失败
				ret = SpiFlashWrite(TEST_FLASH_WR_ADDR_BEGIN, (uint8_t*)test_string, len, 20);
				if(ret)
				{
					//将test_string 复制到data空间的变量buf中，然后再写到Flash中
					memcpy(buf,test_string,strlen(test_string));
					ret = SpiFlashWrite(TEST_FLASH_WR_ADDR_BEGIN, buf, len, 20);
					memset(buf, 0, 100);
				}

				if(!ret)
					ret = SpiFlashRead(TEST_FLASH_WR_ADDR_BEGIN, buf, len, 20);
				if(!ret)
					UART0_Send(buf, len);

		}
		while(1);
	}
#endif



#if 1
	//这部分代码用于示例命令行交互式Flash操作, 命令使用ASCII码方式进行发送
	
	/*
	ret = SpiFlashErase(TEST_FLASH_WR_ADDR_BEGIN, 4 * 1024);
	if(ret != 0 )
		APP_DBG("Flash erase error!\r\n");
		*/
	do_cmd_help();
	WDG_Disable();
	while(1)
	{
		uint8_t cmd_buf[MAX_CMD_LEN] = {0};
		uint16_t cmd_len;
		int32_t ret;

		cmd_len =  cmd_receive(cmd_buf,MAX_CMD_LEN, 200);
		if(cmd_len>0)
		{
			unsigned char cmd_head = cmd_buf[0];
			switch(cmd_head)
			{
				case 'E':
				case 'e':
					ret = do_cmd_erase(&cmd_buf[1],cmd_len-1);
					break;
				case 'W':
				case 'w':
					ret = do_cmd_write(&cmd_buf[1],cmd_len-1);
					break;
				case 'R':
				case 'r':
					ret = do_cmd_read(&cmd_buf[1],cmd_len-1);
					break;
				case 'U':
				case 'u':
					ret = do_cmd_unlock();
					break;
				case 'L':
				case 'l':
					ret = do_cmd_lock(&cmd_buf[1],cmd_len-1);
					break;
				case '?':
					do_cmd_help();
					continue;
				case 'h':
				case 'H':
					if(cmd_len == 1)
					{
						do_cmd_help();	
						continue;
					}
					else if(cmd_len == 4)	//"help" or "Help"
					{
						if(cmd_buf[1] == 'e' && cmd_buf[2] == 'l' && cmd_buf[3] == 'p')
						{
							do_cmd_help();	
							continue;
						}
					}
				default:
					DBG("Unknown command!\r\n");
					continue;
			
			}



			if(ret == FLASH_NONE_ERR)
				DBG("command executed successful!\r\n");
			else if(ret == ERR_ILLEGAL_PARA)
				DBG("Illegal command\r\n");
			else if(ret == ERR_VERIFY_FAIL)
				DBG("Verify failed, erase Flash and retry!\r\n");
			else if(ret == ERR_ILLEGAL_ADDR)
			{
				DBG("Illegal offset or size for flash erasing\r\n");
			}
			else
				DBG("Command executed failed!\r\n");
		
		}
				
		
	}
#endif
	return 0;
}




//
