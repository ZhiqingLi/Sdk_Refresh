/**
 *****************************************************************************
 * NAME:
 *	gd_flash_ctrl.c
 *
 * DESCRIPTION:
 *	The functions used only for GD flash
 *****************************************************************************
 */

#include "type.h"
#include "app_config.h"
#include "stdio.h"
#include "spi_flash.h"
#include "gd_flash_drv.h"

#include "irqs.h"

#ifdef FLASH_ERASE_IN_TCM
__attribute__((section("CACHE.4KBMEM")))
int32_t SpiFlashEraseInTcm(uint32_t Offset, uint32_t Size);

__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashSendCmd(uint8_t FlashCmd, uint8_t CmdLen, uint32_t AddrLen, uint8_t CmdType, uint16_t DataLen);

__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashResetCRM(void);

__attribute__((section("CACHE.4KBMEM")))
static bool GDSpiCodeFlashIsFifoEmpty(void);

__attribute__((section("CACHE.4KBMEM")))
static uint32_t GDSpiCodeFlashReadStatus(void);

__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashControllerBusyStatusSet(uint32_t BusyFlag);

__attribute__((section("CACHE.4KBMEM")))
static bool GDSpiCodeFlashIsBusy(void);

__attribute__((section("CACHE.4KBMEM")))
static int32_t GDSpiCodeFlashHpmModeSet(uint32_t HpmMode);
#ifdef FUNC_WIFI_EN
__attribute__((section("CACHE.4KBMEM")))
int32_t FuartRecvByte(uint8_t* Buf);
#endif

__attribute__((section("CACHE.4KBMEM")))	
void SpiCodeFlashErase(SPI_FLASH_INFO* FlashInfo,ERASE_TYPE_ENUM erase_type, uint32_t index, bool IsSuspend)
{
	uint8_t Cmd;
	uint8_t CmdLen;
	uint32_t Addr;
	uint32_t AddrLen;
	uint32_t CmdType;
	//uint8_t i;
	bool hpm_flag = (*(volatile unsigned long *) 0x40006040);//REG_FSHC_BYPASS_HPM;

	__DISABLE_FAULT_IRQ();

	if((*(volatile unsigned long *)0x40006050) & 0x01)
	{
			GDSpiCodeFlashResetCRM( );
	}

	switch(erase_type)
	{
		case CHIP_ERASE:
			return ;	//SDKÖÐ½ûÖ¹chip erase
			//Cmd = FLASH_CE;
			//Addr = 0;
			//AddrLen = 0;
			//CmdType = CMD_ONLY_1BIT;
			//break;
		case SECTOR_ERASE:
			Cmd = 0x20;//FLASH_SE;
			Addr = index * (4 * 1024);//SECTOR_SIZE;
			AddrLen = 0x17;//ADDR_LEN;
			CmdType = 0x60;//CMD_ADDR_1BIT;
			break;
		case BLOCK_ERASE:
			Cmd = 0xD8;//FLASH_BE_64;
			Addr = index * (44 * 1024);//BLOCK_SIZE;
			AddrLen = 0x17;//ADDR_LEN;
			CmdType = 0x60;//CMD_ADDR_1BIT;
			break;
		default:
			return;
	}

	CmdLen = 0x07;//CMD_LEN;
	
	if(AddrLen != 0)
	{
		(*(volatile unsigned long *) 0x40006014) = Addr; //REG_FSHC_ADDR_REG = Addr;
	}

	GDSpiCodeFlashSendCmd(0x06, 0x07, 0, 0xE0, 0);
	GDSpiCodeFlashSendCmd(Cmd, CmdLen, AddrLen, CmdType, 0);

	GDSpiCodeFlashControllerBusyStatusSet(1);
		
	if(!IsSuspend || erase_type == CHIP_ERASE)
    {
		while(!GDSpiCodeFlashIsBusy( ))
		{
			/*
				do something in flash erasing if needed;
				NOTE:
					THE CODE MUST BE RUN IN SECTION ("CACHE.4KBMEM")!!!
					THE CODE MUST BE RUN IN SECTION ("CACHE.4KBMEM")!!!
					THE CODE MUST BE RUN IN SECTION ("CACHE.4KBMEM")!!!
					(The important thing must be repeated 3 times ^_^ )
			*/
#ifdef FUNC_WIFI_EN
 			{		
				uint8_t Temp;				
				extern uint8_t RxQueue[];
				extern uint16_t RxQueueHead;
				extern uint16_t RxQueueTail;	 

				if(FuartRecvByte(&Temp) != -1)
				{
					RxQueue[RxQueueTail++] = Temp;			//save data into queue
					if(RxQueueTail > (1024 - 1))
					{
						RxQueueTail = 0;
					}
				}
			}
#endif
		}
		GDSpiCodeFlashControllerBusyStatusSet(0);
    }
	
	if(!hpm_flag)
	{
			GDSpiCodeFlashHpmModeSet(1);
	}

	__ENABLE_FAULT_IRQ();

}

int FlashEraseInTCM(SPI_FLASH_INFO* FlashInfo,uint32_t Offset,uint32_t Size)
{
	//uint8_t Mode;
	int i,Echo,Round;

	Echo = -22;//EINVAL;
	if(FlashInfo->Capacity < Offset + Size)
		return Echo;

	if(Size >= 64 * 1024 && FlashInfo->Mid != 0xBF){
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
			/*
			* judge the offset and size 
			*/
			if(FlashInfo->Capacity < Offset + Size)
			   return Echo;

			Round = (((0x10000 - Offset) & 0xFFFF)) >> 12;
			for(i = 0;i < Round;i ++){
				SpiCodeFlashErase(FlashInfo, SECTOR_ERASE, Offset / 0x1000,0);
				Size -= 0x1000;
				Offset += 0x1000;
			}
		}

		if(Size >= 64 * 1024){
			Round = Size >> 16;
			for(i = 0;i < Round;i ++){
				SpiCodeFlashErase(FlashInfo,BLOCK_ERASE,Offset / 0x10000,0);
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

	if(FlashInfo->Capacity < Offset + Size)
		return Echo;

	Round = Size >> 12;
	for(i = 0;i < Round;i ++){
		SpiCodeFlashErase(FlashInfo, SECTOR_ERASE, Offset / 0x1000,0);
		Offset += 0x1000;
	}
	Echo = 0;

	return Echo;
}

__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashSendCmd(uint8_t FlashCmd, uint8_t CmdLen, uint32_t AddrLen, uint8_t CmdType, uint16_t DataLen)
{
	 (*(volatile unsigned long *)0x40006010) = FlashCmd;
	 (*(volatile unsigned long *)0x40006018) = (CmdLen << 6) | AddrLen;
	 (*(volatile unsigned long *)0x4000601C) = CmdType;
	 (*(volatile unsigned long *)0x4000600C) = DataLen;

	 (*(volatile unsigned long *)0x40006020) = 0x01;
	 (*(volatile unsigned long *)0x40006020) = 0x00;

	 while((*(volatile unsigned long *)0x4000602C) & 0x01);
}

__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashResetCRM(void)
{
	GDSpiCodeFlashSendCmd(0xFF, 0x07, 0, 0xE0, 0);
}
__attribute__((section("CACHE.4KBMEM")))
static bool GDSpiCodeFlashIsFifoEmpty(void)
{
	return ((*(volatile unsigned long*)0x40006024) & 0x01);	
}
__attribute__((section("CACHE.4KBMEM")))
static uint32_t GDSpiCodeFlashReadStatus(void)
{
	uint32_t	Status = 0;
	uint32_t	Tmp;

	if((*(volatile unsigned long *)0x40006050) & 0x01)
	{
		GDSpiCodeFlashResetCRM();
	}

	GDSpiCodeFlashSendCmd(0x05, 0x07, 0, 0x0C0, 0);
	while(GDSpiCodeFlashIsFifoEmpty());
	Status = *(volatile unsigned long *)0x40006000;
	
	GDSpiCodeFlashSendCmd(0x35, 0x07, 0, 0xC0, 0);
	while(GDSpiCodeFlashIsFifoEmpty());
	Tmp = *(volatile unsigned long *)0x40006000;

	Status = (Tmp << 8) | Status;

	 return Status;
}
__attribute__((section("CACHE.4KBMEM")))
static void GDSpiCodeFlashControllerBusyStatusSet(uint32_t BusyFlag)
{
	if(BusyFlag)
    {
	    *(volatile unsigned long *)0x40006008 |= 0x08;
    }
    else
    {
        *(volatile unsigned long *)0x40006008 &= ~0x08;   
    }
}
__attribute__((section("CACHE.4KBMEM")))
static bool GDSpiCodeFlashIsBusy(void)
{
	uint32_t		    Status;	

	Status = GDSpiCodeFlashReadStatus();
	if(Status & 0x01)
	{
		return FALSE;	
	}
	else
	{
		return TRUE;
	}
}
__attribute__((section("CACHE.4KBMEM")))
static int32_t GDSpiCodeFlashHpmModeSet(uint32_t HpmMode)
{
	int32_t Echo = 0;

	switch(HpmMode)
	{
	case 0:
		(*(volatile unsigned long *)0x40006040) = 0;
		break;

	case 1:
		if((*(volatile unsigned long *)0x40006040) == 0)
			GDSpiCodeFlashSendCmd(0xA3,0x18 + 7,0,0xE0,0);
		break;

	case 2:
		GDSpiCodeFlashSendCmd(0xA3,0x18 + 7,0,0xE0,0);
		(*(volatile unsigned long *)0x40006040) = 0;
		break;

	case 3:
		(*(volatile unsigned long *)0x40006040) = 1;		
		(*(volatile unsigned long *)0x40006048) = 1;
		(*(volatile unsigned long *)0x40006048) = 0;
		break;

	default:
		Echo = -38;//ENOSYS;
		break;
	}

	return Echo;
}

__attribute__((section("CACHE.4KBMEM")))
extern SPI_FLASH_INFO gFlashInfo;

int32_t SpiFlashEraseInTcm(uint32_t Offset, uint32_t Size)
{
	return FlashEraseInTCM(&gFlashInfo, (Offset), (Size));
}

#endif

