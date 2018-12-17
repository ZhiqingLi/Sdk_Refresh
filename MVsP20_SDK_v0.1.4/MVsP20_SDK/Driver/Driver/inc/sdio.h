/**
 *****************************************************************************
 * @file     sdio.h
 * @author   Owen
 * @version  V1.0.0
 * @date     2017-03-27
 * @brief    declare sdio controller driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

 /**
 * @addtogroup SDIO
 * @{
 * @defgroup sdio sdio.h
 * @{
 */
 
#ifndef __SDIO_H__
#define __SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
	
#define SD_SECTOR_SIZE			512

#define NO_ERR                  (0)
#define SEND_CMD_TIME_OUT_ERR   (1)
/**
 * SDIO BUS MODE
 * In terms of operating supply voltage, two types of SD Memory Cards are defined:
 * SD Memory Cards which supports initialization/identification process with a range of 2.0-3.6v
 * and operating voltage within this range as defined in the CSD register.
 * SDLV Memory Cards - Low Voltage SD Memory Cards, that can be operate in voltage range of
 * 1.6-3.6V. The SDLV Memory Cards will be labeled differently then SD Memory Cards.
 */
#define 	CMD0_GO_IDLE_STATE			0   /**<reset command and sets each card into Idle State regardless of the current card state*/
#define 	CMD1_SEND_OP_COND			1	/**<in idle state, only CMD1,ACMD41 and CMD58 are legal host CMD,response R1*/
#define	CMD2_ALL_SEND_CID				2	/**<to each card to get its unique card identification (CID) number*/
#define	CMD3_SEND_RELATIVE_ADDR			3   /**<asks the card to publish a new relative card address*/
#define	CMD4_SET_DSR					4   /**<programs their DSP registers*/
#define	CMD7_SELECT_DESELECT_CARD		7   /**<used to select one card and put it into thansfer states */
#define	CMD8_SEND_IF_COND				8	/**<get the range of the volatage of the card support*/
#define	CMD9_SEND_CSD					9
#define	CMD10_SEND_CID					10
#define	CMD12_STOP_TRANSMISSION			12
#define	CMD13_SEND_STATUS				13
#define	CMD15_GO_INACTIVE_STATE			15

/**<block oriented read commands (class 2)*/
#define	CMD16_SET_BLOCKLEN				16
#define	CMD17_READ_SINGLE_BLOCK			17
#define	CMD18_READ_MULTIPLE_BLOCK		18

/**<block oriented write commands (class 4)*/
#define	CMD24_WRITE_BLOCK				24
#define	CMD25_WRITE_MULTIPLE_BLOCK		25
#define	CMD27_PROGRAM_CSD				27

/**<block oriented write protection commands (class 6)*/
#define	CMD28_SET_WRITE_PROT			28
#define	CMD29_CLR_WRITE_PROT			29
#define	CMD30_SEND_WRITE_PROT			30

/**<erase commands (class 5)*/
#define	CMD32_ERASE_WR_BLK_START		32
#define	CMD33_ERASE_WR_BLK_END			33
#define	CMD38_ERASE						38

/**<lock card (class 7)*/
#define	CMD42_LOCK_UNLOCK				42

/**<application specific commands (class 8)*/
#define	CMD55_APP_CMD					55
#define	CMD56_GEN_CMD					56

/**<define ACMD index (ACMD6 ~ ACMD51)*/
/**<application specific commands used/reserved by SD memory card*/
#define	ACMD6_SET_BUS_WIDTH				70	//6  + 64
#define	ACMD13_SD_STATUS				77	//13 + 64
#define	ACMD22_SEND_NUM_WR_BLOCKS		86	//22 + 64
#define	ACMD23_SET_WR_BLK_ERASE_COUNT	87	//23 + 64
#define	ACMD41_SD_SEND_OP_COND			105	//41 + 64             /**<The response to ACMD41 is the operation condition register of the card*/
#define	ACMD42_SET_CLR_CARD_DETECT		106	//42 + 64
#define	ACMD51_SEND_SCR					115	//51 + 64

typedef enum
{
	SDIO_RESP_TYPE_R0 = 0,
	SDIO_RESP_TYPE_R1,
	SDIO_RESP_TYPE_R1B,
	SDIO_RESP_TYPE_R2,
	SDIO_RESP_TYPE_R3,
	SDIO_RESP_TYPE_R6,
	SDIO_RESP_TYPE_R7
}SDIO_RESP_TYPE;

typedef enum
{
	SDIO_INT_CMD = 0,
	SDIO_INT_DAT,
}SDIO_INT_TYPE;

extern const uint8_t CmdRespType[128];

/**
 * @brief  sdio时钟分频设置
 * @param  0:20MHz,1:10MHz
 * @return NONE
 * @note
 */
void SDIO_ClkSet(uint8_t ClkDiv);

/**
 * @brief  使能sdio时钟
 * @return NONE
 * @note
 */
void SDIO_ClkEnable();


/**
 * @brief  关闭sdio时钟
 * @return NONE
 * @note
 */
void SDIO_ClkDisable();

/**
 * @brief  获取数据传输的状态
 * @param  NONE
 * @return NONE
 * @note
 */
bool SDIO_DataStartStatusGet();

/**
 * @brief  Sdio 控制器初始化
 * @param  NONE
 * @return NONE
 * @note
 */
bool SDIO_Init(void);

/**
 * @brief  SDIO发送命令
 * @param  SDIO需要发送的命令编号
 * @param  对于命令的参数
 * @param  超时时间
 * @return 发送命令成功返回NO_ERR，错误返回SEND_CMD_TIME_OUT_ERR
 * @note
 */	
bool SDIO_CmdSend(uint8_t Cmd, uint32_t Param, uint16_t TimeOut);

/**
 * @brief  获取命令的返回的响应
 * @param  buffer指针
 * @param  相应类型
 * @return NONE
 */
bool SDIO_CmdRespGet(uint8_t* RespBuf, uint8_t RespType);

/**
 * @brief  启动SDIO接收数据
 * @param  buffer指针
 * @param  接收数据的长度
 * @return NONE
 * @note
 */
void SDIO_RXStart(uint8_t *Buf,uint32_t Size);

/**
 * @brief  获取数据是否接收完成
 * @param  NONE
 * @return 1:完成，0未完成
 * @note
 */
bool SDIO_RXIsDone(void);

/**
 * @brief  清除接收完成标志
 * @param  NONE
 * @return NONE
 * @note
 */
void SDIO_RXDoneFlagClear(void);


/**
 * @brief  停止数据接收
 * @param  NONE
 * @return NONE
 * @note
 */
void SDIO_RXEnd(void);


/**
 * @brief  启动数据发送
 * @param  buffer指针
 * @param  接收数据的长度
 * @return NONE
 * @note
 */
void SDIO_TXStart(uint8_t *Buf,uint32_t Size);


/**
 * @brief  获取数据是否发送完成
 * @param  NONE
 * @return 1:完成 0:未完成
 * @note
 */
bool SDIO_TXIsDone(void);

/**
 * @brief   清除发送完成标志
 * @param  NONE
 * @return NONE
 * @note
 */
void SDIO_TXDoneFlagClear(void);

/**
 * @brief  停止数据发送
 * @param  NONE
 * @return NONE
 * @note
 */
void SDIO_TXEnd(void);

/**
 * @brief  获取SDIO是否busy
 * @param  NONE
 * @return 0 is busy, 1 is idle
 * @note
 */
bool SDIO_IsDataLineBusy(void);


/**
 * @brief  使能SDIO中断
 * @param  中断类型 	SDIO_INT_CMD or SDIO_INT_DAT
 * @return 
 * @note
 */
void SDIO_InterruptEnable(SDIO_INT_TYPE IntType);

/**
 * @brief  关闭SDIO中断
 * @param  中断类型 	SDIO_INT_CMD or SDIO_INT_DAT
 * @return 
 * @note
 */
void SDIO_InterruptDisable(SDIO_INT_TYPE IntType);

/**
 * @brief  获取SDIO中断标志
 * @param  中断类型 	SDIO_INT_CMD or SDIO_INT_DAT
 * @return 中断标志
 * @note
 */
bool SDIO_InterruptFlagGet(SDIO_INT_TYPE IntType);

/**
 * @brief  清除SDIO中断标志
 * @param  中断类型 SDIO_INT_CMD or SDIO_INT_DAT
 * @return NONE
 * @note
 */
void SDIO_InterruptFlagClear(SDIO_INT_TYPE IntType);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__SDIO_H__

/**
 * @}
 * @}
 */

