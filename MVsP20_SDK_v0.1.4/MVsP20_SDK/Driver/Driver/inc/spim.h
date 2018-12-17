/**
  *****************************************************************************
  * @file:			spim.h
  * @author			Lilu
  * @version		V1.0.0
  * @data			2014-11-20
  * @Brief			SPI Master driver interface
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

/**
 * @addtogroup SPIM
 * @{
 * @defgroup spim spim.h
 * @{
 */
 
 
#ifndef __SPIM_H__
#define __SPIM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define	SPIM_CLK_DIV_30M	0x0			/**< SPI master clock Div 30MHz*/
#define	SPIM_CLK_DIV_15M	0x1			/**< SPI master clock Div 15MHz*/
#define	SPIM_CLK_DIV_7M5	0x2			/**< SPI master clock Div 7.5MHz*/
#define	SPIM_CLK_DIV_3M7        0x3			/**< SPI master clock Div 3.75MHz*/
#define	SPIM_CLK_DIV_1M8	0x4			/**< SPI master clock Div 1.875MHz*/
#define	SPIM_CLK_DIV_937K	0x5			/**< SPI master clock Div 937.5KHz*/
#define	SPIM_CLK_DIV_468K	0x6			/**< SPI master clock Div 438.75KHz*/

/**
 * err code define
 */
typedef enum _SPI_MASTER_ERR_CODE
{
    ERR_SPIM_TIME_OUT = -255,			/**<function execute time out*/
    ERR_SPIM_DATALEN_OUT_OF_RANGE,		/**<data len is out of range < 0*/
    SPIM_NONE_ERR = 0,                  /**<no error*/
} SPI_MASTER_ERR_CODE;


/**
 * @brief
 *		初始化SPIM模块
 * @param	Mode
 *				0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *				2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1, CPHA = 1
 * @param	ClkDiv 	分频系数:0 ~ 11
 * 				spi clock = system clock / (2 EXP (ClkDivExp)).
 * @return
 *		无
 * @note
 */
bool SPIM0_Init(uint8_t Mode, uint8_t ClkDiv);

/**
 * @brief
 *		初始化SPIM模块
 * @param	Mode
 *				0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *				2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1, CPHA = 1
 * @param	ClkDiv 	分频系数:0 ~ 11
 * 				spi clock = system clock / (2 EXP (ClkDivExp)).
 * @return
 *		无
 * @note
 */
bool SPIM1_Init(uint8_t Mode, uint8_t ClkDiv);

//--------------------------------MCU MODE-------------------------------------
/**
 * @brief
 *		MCU模式发送数据
 * @param	SendBuf		发送数据首地址
 * @param	Length		发送数据长度（单位：Byte）
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM0_SendNBytes(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		MCU模式发送数据
 * @param	SendBuf		发送数据首地址
 * @param	Length		发送数据长度（单位：Byte）
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM1_SendNBytes(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		MCU模式接收数据
 * @param	RecvBuf		接收数据首地址
 * @param	Length		接收数据长度（单位：Byte）
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM0_RecvNBytes(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief
 *		MCU模式接收数据
 * @param	RecvBuf		接收数据首地址
 * @param	Length		接收数据长度（单位：Byte）
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM1_RecvNBytes(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief
 *		MCU模式发送1字节数据
 * @param	SendByte  单字节数据
 * @return
 *		无
 */
void SPIM0_SendByte(uint8_t SendByte);

/**
 * @brief
 *		MCU模式发送1字节数据
 * @param	SendByte  单字节数据
 * @return
 *		无
 */
void SPIM1_SendByte(uint8_t SendByte);

/**
 * @brief
 *		MCU模式接收1字节数据
 * @param	无
 * @return
 *		返回接收的单字节数据
 */
uint8_t SPIM0_RecvByte(void);

/**
 * @brief
 *		MCU模式接收1字节数据
 * @param	无
 * @return
 *		返回接收的单字节数据
 */
uint8_t SPIM1_RecvByte(void);

//--------------------------------DMA MODE-------------------------------------
/**
 * @brief
 *		DMA模式发送数据
 * @param	SendBuf	数据缓冲区首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		返回错误号
 * @Note
 */
SPI_MASTER_ERR_CODE SPIM0_DMA_SendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		DMA模式发送数据
 * @param	SendBuf	数据缓冲区首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		返回错误号
 * @Note
 */
SPI_MASTER_ERR_CODE SPIM1_DMA_SendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		DMA模式接收数据
 * @param	RecvBuf 数据首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM0_DMA_RecvData(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief
 *		DMA模式接收数据
 * @param	RecvBuf 数据首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		返回错误号
 * @note
 */
SPI_MASTER_ERR_CODE SPIM1_DMA_RecvData(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief
 *		启动DMA模式发送数据，非阻塞
 * @param	SendBuf	数据缓冲区首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		无
 * @Note SPIM中断标志置位标志发送数据完成，可使用查询或中断方式
 */
void SPIM0_DMA_StartSendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		启动DMA模式发送数据，非阻塞
 * @param	SendBuf	数据缓冲区首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		无
 * @Note SPIM中断标志置位标志发送数据完成，可使用查询或中断方式
 */
void SPIM1_DMA_StartSendData(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		启动DMA模式接收数据，非阻塞
 * @param	RecvBuf 数据首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		无
 * @note DMA D标志置位标志接收数据完成，可使用查询或中断方式
 */
void SPIM0_DMA_StartRecvData(uint8_t* RecvBuf, uint32_t Length);

/**
 * @brief
 *		启动DMA模式接收数据，非阻塞
 * @param	RecvBuf 数据首地址
 * @param	Length	数据长度（单位：Byte），最大65536 byte
 * @return
 *		无
 * @note DMA D标志置位标志接收数据完成，可使用查询或中断方式
 */
void SPIM1_DMA_StartRecvData(uint8_t* RecvBuf, uint32_t Length);


/**
 * @brief
 *		中断使能
 * @param	无
 * @return
 *		无
 */
void SPIM0_IntEn();

/**
 * @brief
 *		中断使能
 * @param	无
 * @return
 *		无
 */
void SPIM1_IntEn();

/**
 * @brief
 *		获取中断标志
 * @param	无
 * @return
 *		返回中断标志
 */
bool SPIM0_GetIntFlag();

/**
 * @brief
 *		获取中断标志
 * @param	无
 * @return
 *		返回中断标志
 */
bool SPIM1_GetIntFlag();

/**
 * @brief
 *		清除中断标志
 * @param	无
 * @return
 *		无
 */
void SPIM0_ClrIntFlag();

/**
 * @brief
 *		清除中断标志
 * @param	无
 * @return
 *		无
 */
void SPIM1_ClrIntFlag();

/**
 * @brief
 *		获取过速标志
 * @param	无
 * @return
 *		1-过速
 *      0-无过速
 */
bool SPI0_Get_OverWr();

/**
 * @brief
 *		获取过速标志
 * @param	无
 * @return
 *		1-过速
 *      0-无过速
 */
bool SPI1_Get_OverWr();

/**
 * @brief
 *		获取过速标志
 * @param	无
 * @return
 *		无
 */
void SPI0_Set_OverWr();

/**
 * @brief
 *		获取过速标志
 * @param	无
 * @return
 *		无
 */
void SPI1_Set_OverWr();
#ifdef __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
