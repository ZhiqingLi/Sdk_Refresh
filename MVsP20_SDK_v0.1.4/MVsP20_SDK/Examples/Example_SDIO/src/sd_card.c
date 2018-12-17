/**
 *****************************************************************************
 * @file     sd_card.c
 * @author   owen
 * @version  V1.0.0
 * @date     4-DEC-2016
 * @brief    sd card module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
#include <string.h>
#include "type.h"
#include "debug.h"
#include "delay.h"
#include "timeout.h"
#include "sdio.h"
#include "sd_card.h"
#include "dma.h"


/**<传输状态 */
#define SD_STATUS_ACCEPTED				0x02
#define SD_STATUS_CRC_ERROR				0x05
#define SD_STATUS_WRITE_ERROR			0x06

/**<卡类型*/
#define	MC_MCDMAS_MMC					1
#define	MC_MCDMAS_SD					2

#define	SD_BLOCK_SIZE					512             /**<块大小固定为512字节*/

/**<卡状态定义*/
#define	CURRENT_STATE_IDLE				0
#define	CURRENT_STATE_READY				1
#define	CURRENT_STATE_IDENT				2              /**<identification*/
#define	CURRENT_STATE_STBY				3
#define	CURRENT_STATE_TRAN				4
#define	CURRENT_STATE_DATA				5              /**<sending data*/
#define	CURRENT_STATE_RCV				6
#define	CURRENT_STATE_PRG				7
#define	CURRENT_STATE_DIS				8              /**<disconnect*/

#define	VOLTAGE_WINDOWS					0x40FF8000     /**<2.7v~3.6v*/
#define VOLTAGE_PATTERN                 0x000001AA     /**<0x01:2.7-3.6V,0xAA:check pattern,pattern can use any 8-bit*/


SD_CARD		    SDCard;
SD_CARD_ID	    SDCardId;

void SDCard_ControllerInit(void)
{
	SDCard.RCA = 0;
	SDCard.IsSDHC = FALSE;
	SDCard.IsCardInit = FALSE;

	SDIO_Init();
}

SD_CARD_ERR_CODE SDCard_SendAppCommand(uint8_t Cmd, uint32_t Param)
{
	if(SDIO_CmdSend(CMD55_APP_CMD, SDCard.RCA, 20) == NO_ERR)
	{
		if(SDIO_CmdSend(Cmd, Param, 20) == NO_ERR)
		{
			return NONE_ERR;
		}
		return CMD_SEND_TIME_OUT_ERR;
	}
	return CMD55_SEND_ERR;
}

SD_CARD_ERR_CODE SDCard_Detect(void)
{
	if(SDCard.IsCardInit)//卡已初始化
	{
		if(SDIO_CmdSend(CMD13_SEND_STATUS, SDCard.RCA, 20)  == NO_ERR)              	//read card status
		{
			return NONE_ERR;
		}
		else
		{
			SDCard_ControllerInit();
			DBG("error 92\n");
			return CMD13_SEND_ERR;
		}
	}
	else//卡未初始化
	{
		SDCard_ControllerInit();
		SDIO_CmdSend(CMD0_GO_IDLE_STATE, 0, 20);
		SDIO_CmdSend(CMD8_SEND_IF_COND, VOLTAGE_PATTERN, 20);
		if(SDCard_SendAppCommand(ACMD41_SD_SEND_OP_COND, VOLTAGE_WINDOWS) == NONE_ERR)
		{
			DBG("SD link!\n");
			SDCard.CardType = MC_MCDMAS_SD;
			return NONE_ERR;
		}

		//check mmc card
		SDIO_CmdSend(CMD0_GO_IDLE_STATE, 0, 20);
		SDIO_CmdSend(CMD8_SEND_IF_COND, VOLTAGE_PATTERN, 20);
		if(SDIO_CmdSend(CMD1_SEND_OP_COND, VOLTAGE_WINDOWS, 20) ==NO_ERR)
		{
			DBG("MMC link!\n");
			SDCard.CardType = MC_MCDMAS_MMC;
			return NONE_ERR;
		}
		DBG("error 118\n");
		return NOCARD_LINK_ERR;
	}
}

SD_CARD_ERR_CODE SDCard_Identify(void)
{
	TIMER Timer;
	uint8_t  resp[16];
	uint8_t  i;

	//设置电压窗口
	TimeOutSet(&Timer, 2000);

	while(1)
	{
		if(SDCard.CardType == MC_MCDMAS_SD)
		{
			WaitMs(300);
			if(SDCard_SendAppCommand(ACMD41_SD_SEND_OP_COND, VOLTAGE_WINDOWS))
			{
				DBG("error 136\n");
				return ACMD41_SEND_ERR;
			}
		}
		if(SDCard.CardType == MC_MCDMAS_MMC)
		{
			if(SDIO_CmdSend(CMD1_SEND_OP_COND, VOLTAGE_WINDOWS, 20))
			{
				DBG("error 144\n");
				return CMD1_SEND_ERR;
			}
		}		
		SDIO_CmdRespGet(resp, SDIO_RESP_TYPE_R3);
		
		if(*(uint32_t *)resp & 0x80000000)//ocr register bit definition, bit31-Card power up status bit (busy) bit30-Card Capacity Status (CCS)
		{
			if(*(uint32_t *)resp & 0x40000000)//for SDHC
			{
				SDCard.IsSDHC = TRUE;
			}
			break;
		}

		if(IsTimeOut(&Timer))
		{
			DBG("CMD_SEND_TIME_OUT_ERR\n");
			return CMD_SEND_TIME_OUT_ERR;
		}
	}

	//获取CID(unique card identification)
	if(SDIO_CmdSend(CMD2_ALL_SEND_CID, 0, 20))      //success card will from ready switch to identification
	{
		DBG("SdCardInit() ERROR 005!\n");
		return CMD2_SEND_ERR;
	}
	SDIO_CmdRespGet(resp, SDIO_RESP_TYPE_R2);
	for(i = 0; i < 15; i++)
	{
		*((uint8_t *)(&SDCardId) + i + 1) = resp[14 - i];
	}

	//获取相对地址，最多重试10次
	for(i = 0; i < 10; i++)
	{
		if(SDIO_CmdSend(CMD3_SEND_RELATIVE_ADDR, 0, 20) == NO_ERR)
		{
			SDIO_CmdRespGet(resp, SDIO_RESP_TYPE_R6);
			if(((*(uint32_t *)(resp)) & 0x00001E00) >> 9 == CURRENT_STATE_STBY)
			{
				SDCard.RCA = *(uint32_t *)resp;
				break;
			}
		}
	}

	DBG("RCA: %08x\n", SDCard.RCA);	

	//获取CSD寄存器（Card Specific Data）里面的描述值，譬如，块长度，卡容量信息等。
	if(SDIO_CmdSend(CMD9_SEND_CSD, SDCard.RCA, 20))
	{
		return CMD9_SEND_ERR;
	}
	SDIO_CmdRespGet(resp, SDIO_RESP_TYPE_R2);
	//根据CSD计算BlockNum
	if(resp[14])	//CSD V2
	{
		SDCard.BlockNum = (((resp[7] & 0x3F) << 16) + (resp[6] << 8) + resp[5] + 1) << 10;	// memory capacity = (C_SIZE+1) * 512KByte
	}
	else			//CSD V1
	{
		uint8_t exp;
		SDCard.BlockNum = ((resp[8] & 0x03) << 10) + (resp[7] << 2) + ((resp[6] & 0xC0) >> 6) + 1;
		
		exp = ((resp[5] & 0x03) << 1) + ((resp[4] & 0x80) >> 7) + 2 + (resp[9] & 0x0F) - 9;
		SDCard.BlockNum <<= exp;	// (C_SIZE + 1) * 2 ^ (C_SIZE_MULT + 2)
	}
	DBG("BlockNum:%d\n", SDCard.BlockNum);
	//get card max transfer data speed
	i = (resp[11] >> 3) & 0x0F;

	DBG("resp:%X\n", resp[11]);
	if((resp[11] & 0x07) == 1)	//base 1M
	{
		if(i >= 0x0E)
		{
			SDCard.MaxTransSpeed = 0x03;	//频率设为6.25MHz   FPGA阶段和芯片阶段不同，需调整
		}
		else
		{
			SDCard.MaxTransSpeed = 0x04;	//频率设为3.125MHz
		}
	}
	else if((resp[11] & 0x07) == 2)	//base 10M
	{
		if(i < 0x02)
		{
			SDCard.MaxTransSpeed = 0x03;	//频率设为6.25MHz
		}
		else if(i < 0x06)
		{
			SDCard.MaxTransSpeed = 0x02;	//频率设为12.5MHz
		}
		else
		{
			SDCard.MaxTransSpeed = 0x01;	//频率设为25MHz
		}
	}
	
	DBG("SDCard.MaxTransSpeed:%d\n", SDCard.MaxTransSpeed);
	//进入transfer state
	if(SDIO_CmdSend(CMD7_SELECT_DESELECT_CARD, SDCard.RCA, 20))
	{
		DBG("erron 244\n");
		return CMD7_SEND_ERR;
	}
	//获取状态
	if(SDIO_CmdSend(CMD13_SEND_STATUS, SDCard.RCA, 20))
	{
		DBG("erron 250\n");
		if(SDIO_CmdSend(CMD13_SEND_STATUS, SDCard.RCA, 20))
		{
			DBG("erron 253\n");
			return CMD13_SEND_ERR;
		}
		///return CMD13_SEND_ERR;
	}
	SDIO_CmdRespGet(resp, SDIO_RESP_TYPE_R1);
	if(((*(uint32_t *)(resp)) & 0x00001E00) >> 9  != CURRENT_STATE_TRAN)
	{
		DBG("erron 261\n");
		return GET_RESPONSE_STATUS_ERR;
	}

	//设置块长度
	if(SDIO_CmdSend(CMD16_SET_BLOCKLEN, SD_BLOCK_SIZE, 20))
	{
		DBG("erron 268\n");
		return CMD16_SEND_ERR;
	}

	return NONE_ERR;
}

SD_CARD* SDCard_GetCardInfo()
{
	return &SDCard;
}

SD_CARD_ERR_CODE SDCard_Init(void)
{
	uint8_t Retry = 4;

	SDCard_ControllerInit();

	//根据以往SD卡兼容性经验，插入卡需延时一段时间
	WaitMs(150);
	while(1)
	{
		SDCard_Detect();
		if(SDCard_Identify() == NONE_ERR)
		{
			DBG("CardGetInfo OK\n");
			break;
		}
		if(!(--Retry))
		{
			DBG("retry 4 times was still failed\n");
			return GET_SD_CARD_INFO_ERR;
		}
	}

	SDCard.IsCardInit = TRUE;
	SDIO_ClkSet(SDCard.MaxTransSpeed-1);
	SDIO_ClkEnable(0);
	return NONE_ERR;
}


SD_CARD_ERR_CODE SDCard_ReadBlock(uint32_t Block, uint8_t* Buffer, uint8_t Size)
{
	uint32_t i;

	if(Block > SDCard.BlockNum)
	{
		return BLOCK_NUM_EXCEED_BOUNDARY;
	}

	if(!SDCard.IsSDHC)
	{
		Block *= SD_BLOCK_SIZE;
	}


	for(i = 0; i < Size; i++)
	{
		SDIO_RXStart(Buffer + 512*i,SD_BLOCK_SIZE);
		SDIO_ClkEnable();
		if(i==0)
		{
			SDIO_CmdSend(CMD18_READ_MULTIPLE_BLOCK, Block, 20);
			SDIO_ClkEnable();
		}
		while(!SDIO_RXIsDone());
		SDIO_RXDoneFlagClear();
	}
	SDIO_CmdSend(CMD12_STOP_TRANSMISSION, 0, 20);
	SDIO_RXEnd();
	SDIO_ClkDisable();
	return NONE_ERR;
}

SD_CARD_ERR_CODE SDCard_WriteBlock(uint32_t Block, const uint8_t* Buffer, uint8_t Size)
{
	uint32_t i;
	if(Block > SDCard.BlockNum)
	{
		return BLOCK_NUM_EXCEED_BOUNDARY;
	}
	if(!SDCard.IsSDHC)
	{
		Block *= SD_BLOCK_SIZE;
	}

	while(SDIO_IsDataLineBusy());
	SDIO_CmdSend(CMD25_WRITE_MULTIPLE_BLOCK, Block, 20);
	for(i = 0; i < Size; i++)
	{
		SDIO_TXStart((uint8_t*)Buffer+512*i,SD_BLOCK_SIZE);
		SDIO_ClkEnable();
		while(!SDIO_TXIsDone());
		SDIO_TXDoneFlagClear();

		if(i == (Size - 1))
		{
			SDIO_CmdSend(CMD12_STOP_TRANSMISSION, 0, 20);
			SDIO_ClkEnable();
		}
		while(SDIO_IsDataLineBusy());
	}
	SDIO_TXEnd();
	SDIO_ClkDisable();
	return NONE_ERR;
}

uint32_t SDCard_CapacityGet(void)
{
	return SDCard.BlockNum;
}
