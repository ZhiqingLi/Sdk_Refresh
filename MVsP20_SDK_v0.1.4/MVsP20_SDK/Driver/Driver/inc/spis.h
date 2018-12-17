/**
  *****************************************************************************
  * @file:			spis.h
  * @author			Lilu
  * @version		V1.0.0
  * @data			18-June-2013
  * @Brief			SPI Slave driver header file.
  * @note			For sdio and spi can't access memory simultaneously
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
 * @addtogroup SPIS
 * @{
 * @defgroup spis spis.h
 * @{
 */
#ifndef __SPIS_H__
#define __SPIS_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus
	
/**
 * @brief
 *    初始化SPIS模块
 * @param	ClkMode		
 *					0 - CPOL = 0 & CPHA = 0, 1 - CPOL = 0 & CPHA = 1,
 *					2 - CPOL = 1 & CPHA = 0, 3 - CPOL = 1 & CPHA = 1
 * @return
 *   无
 * @note
 */
void SPIS_Init(uint8_t ClkMode);




//--------------------------------MCU MODE-------------------------------------
/**
 * @brief
 *    MCU方式发送数据，在TimeOut时间内等待发送完成，超时返回实际发送数据个数，单位Byte
 * @param	SendBuf			数据首地址
 * @param	BufLen			发送数据长度
 * @param	TimeOut			超时时间
 *
 * @return
 *   返回实际发送数据长度
 * @note
 */
uint32_t SPIS_Send(uint8_t* SendBuf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief
 *    MCU方式接收数据，在TimeOut时间内等待接收完成，超时返回实际接收数据个数，单位Byte
 * @param	SendBuf			数据首地址
 * @param	BufLen			接收数据长度
 * @param	TimeOut			超时时间
 *
 * @return
 *   返回实际接收数据长度
 * @note
 */
uint32_t SPIS_Recv(uint8_t* RecvBuf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief
 *    MCU方式发送1字节数据
 * @param	SendBuf			单字节数据
 *
 * @return
 *   无
 * @note
 */
void SPIS_SendByte(uint8_t SendByte);

/**
 * @brief
 *    MCU方式接收1字节数据
 * @param	SendBuf			单字节数据
 *
 * @return
 *   无
 * @note
 */
uint8_t SPIS_RecvByte(void);

//--------------------------------DMA MODE-------------------------------------
/**
 * @brief
 *    初始化SPIS DMA功能
 * @param	RxBufferAddr  接收缓冲区首地址		
 * @param	RxBufferLen   接收缓冲区长度				
 * @param	TxBufferAddr  发送缓冲区首地址
 * @param   TxBufferLen   发送缓冲区长度
 * @return
 *   TRUE   成功，  FALSE 失败
 * @note
 */
bool SPIS_DMA_Init(void* RxBufferAddr, uint16_t RxBufferLen, void* TxBufferAddr, uint16_t TxBufferLen);


/**
 * @brief
 *    DMA方式发送数据，向FIFO中填入数据，在TimeOut时间内等待发送完成，超时返回实际发送数据个数，单位Byte
 * @param	SendBuf			数据首地址
 * @param	BufLen			发送数据长度
 * @param	TimeOut			超时时间
 *
 * @return
 *   返回实际发送数据长度
 * @note
 */
uint32_t SPIS_DMA_Send(uint8_t* SendBuf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief
 *    DMA方式接收数据，从FIFO中取出数据，在TimeOut时间内等待接收完成，超时返回实际接收数据个数，单位Byte
 * @param	SendBuf			数据首地址
 * @param	BufLen			接收数据长度
 * @param	TimeOut			超时时间
 *
 * @return
 *   返回实际接收数据长度
 * @note
 */
uint32_t SPIS_DMA_Recv(uint8_t* RecvBuf, uint32_t BufLen, uint32_t TimeOut);

/**
 * @brief
 *    接收中断使能
 * @param	En			1：开启中断，0：关闭中断
 *
 * @return
 *   无
 * @note
 *	 每接收一个Byte，起一次中断
 */
void SPIS_RxIntEn(bool En);

/**
 * @brief
 *    获取接收中断标志
 * @param	
 *
 * @return
 *   返回接收中断标志
 * @note
 *	 每接收一个Byte，起一次中断
 */
bool SPIS_GetRxIntFlag(void);

/**
 * @brief
 *   清除接收中断标志
 * @param	
 *
 * @return
 *   无
 * @note
 *	 每接收一个Byte，起一次中断
 */
void SPIS_ClrRxIntFlag(void);

/**
 * @brief
 *    获取接收溢出标志
 * @param	
 *
 * @return
 *   返回接收溢出标志
 * @note
 *	 SPIS 内部4Byte 硬件Rx FIFO满后再次收到一个Byte，起接收溢出标志
 */
bool SPIS_GetRxOverrunFlag(void);

/**
 * @brief
 *    清除接收溢出标志
 * @param	
 *
 * @return
 *   无
 * @note
 *	 SPIS 内部4Byte 硬件Rx FIFO满后再次收到一个Byte，起接收溢出标志
 */
void SPIS_ClrRxOverrunFlag(void);


/**
 * @brief
 *    DMA方式接收数据模式设置
 * @param	1--使用DMA接收数据，
 *          0--不使用DMA接收数据
 * @return
 *   无
 * @note
 *	 
 */
void SPIS_RX_DMA_MODE_SET(bool EN);

/**
 * @brief
 *    LSB/MSB传输设置
 * @param	1--LSB先行传输
 *          0--MSB先行传输
 * @return
 *   无
 * @note
 *   在SPIS_Init()之后设置，若不设置则默认MSB先行传输
 */
void SPIS_LSBFirst_SET(bool EN);

/**
 * @brief
 *   清空接收缓冲区FIFO
 * @param
 *   无
 * @return
 *   无
 * @note
 *
 */
void SPIS_ClrRxFIFO(void);
/**
 * @brief
 *   清空发送缓冲区FIFO
 * @param
 *   无
 * @return
 *   无
 * @note
 *
 */
void SPIS_ClrTxFIFO(void);

/**
 * @brief
 *    DMA方式发送数据模式设置
 * @param	1--使用DMA接收数据，
 *          0--不使用DMA接收数据
 * @return
 *   无
 * @note
 *
 */
void SPIS_TX_DMA_MODE_SET(bool EN);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */

