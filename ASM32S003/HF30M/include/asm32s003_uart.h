/*
  ******************************************************************************
  * @file    asm32s003_uart.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/
 
/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef _asm32s003_uart_H
#define _asm32s003_uart_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"


/**
  * @brief  UART IO setting
  */
typedef enum
{
    IO_UART0 = 0,
    IO_UART1 = 1,
}UART_NUM_TypeDef;
/*****************************************************************************
************************** UART Function defined *****************************
******************************************************************************/
#define UART_RESET_VALUE  (0x00000000)
/**  SR : UART Status Register											  */
#define UART_TX_FULL     (0x01ul << 0)  /**< Transmitter full             */
#define UART_RX_FULL     (0x01ul << 1)  /**< Receiver full                */
#define UART_TX_OVER     (0x01ul << 2)  /**< Transmitter buff over        */
#define UART_RX_OVER     (0x01ul << 3)  /**< Receiver buff over           */

/** CTRL : UART Control Register										  	  */ 
#define UART_TX         (0x01ul << 0)  /**< Transmitter Enable/disable        */
#define UART_RX         (0x01ul << 1)  /**< Receiver Enable/disable           */
#define UART_TX_INT     (0x01ul << 2)  /**< Transmitter INT Enable/disable    */
#define UART_RX_INT     (0x01ul << 3)  /**< Receiver INT Enable/disable       */
#define UART_TX_IOV     (0x01ul << 4)  /**< Transmitter INTOver Enable/disable*/
#define UART_RX_IOV     (0x01ul << 5)  /**< Receiver INTOver Enable/disable   */
#define UART_TEST       (0x01ul << 6)  /**< =1 Test mode                      */									

/** ISR : UART Interrupt Status Register									*/
#define UART_TX_INT_S    (0x01ul << 0)  /**< Transmitter INT Status         */
#define UART_RX_INT_S    (0x01ul << 1)  /**< Receiver INTStatus             */
#define UART_TX_IOV_S    (0x01ul << 2)  /**< Transmitter INTOver Status     */
#define UART_RX_IOV_S    (0x01ul << 3)  /**< Receiver INTOver Status        */
/** Set DATA register                                                       */
#define CSP_UART_SET_DATA(uart, val)  ((uart)->DATA = (val))
/** Get DATA register                                                       */
#define CSP_UART_GET_DATA(uart)       ((uart)->DATA)

/** Set SR register                                                         */
#define CSP_UART_SET_SR(uart, val)  ((uart)->SR = (val))
/** Get SR register                                                         */
#define CSP_UART_GET_SR(uart)       ((uart)->SR)

/** Set CTRL register                                                       */
#define CSP_UART_SET_CTRL(uart, val)  ((uart)->CTRL = (val))
/** Get CTRL register                                                       */
#define CSP_UART_GET_CTRL(uart)       ((uart)->CTRL)

/** Set ISR register                                                        */
#define CSP_UART_SET_ISR(uart, val)  ((uart)->ISR = (val))
/** Get ISR register                                                        */
#define CSP_UART_GET_ISR(uart)       ((uart)->ISR)

/** Set BRDIV register                                                      */
#define CSP_UART_SET_BRDIV(uart, val)  ((uart)->BRDIV = (val))
/** Get BRDIV register                                                      */
#define CSP_UART_GET_BRDIV(uart)       ((uart)->BRDIV)
/** UART External Variable Declaration 										*/
extern	volatile U16_T RxDataBuf[12];
extern	volatile U16_T RxDataPtr;
extern	volatile U16_T TxDataPtr;

 /** UART External Functions Declaration  									*/
extern void UARTInit(CSP_UART_T *uart,U16_T baudrate_u16);
extern void UARTClose(CSP_UART_T *uart);
extern void UARTInitRxIntEn(CSP_UART_T *uart,U16_T baudrate_u16);
extern void UARTInitRxTxIntEn(CSP_UART_T *uart,U16_T baudrate_u16);
extern void UARTTxByte(CSP_UART_T *uart,U8_T txdata_u8);
extern void UARTTransmit(CSP_UART_T *uart,U8_T *sourceAddress_u16,U16_T length_u16);
extern U16_T UARTRxByte(CSP_UART_T *uart,U16_T *Rxdata_u16);
extern U8_T UART_ReturnRxByte(CSP_UART_T *uart);
extern U16_T UARTReceive(CSP_UART_T *uart,U16_T *destAddress_u16,U16_T length_u16);	
extern void UART_DeInit(void);
extern void UART_IO_Init(UART_NUM_TypeDef IO_UART_NUM , U8_T UART_IO_G);
extern void UART0_Int_Enable(void);
extern void UART1_Int_Enable(void);
extern void UART0_Int_Disable(void);
extern void UART1_Int_Disable(void);
extern void UART0_WakeUp_Enable(void);
extern void UART1_WakeUp_Enable(void);
extern void UART0_WakeUp_Disable(void);
extern void UART1_WakeUp_Disable(void);
extern U8_T GetDebugDataLen(void);
extern U8_T GetDebugBufferData(void);
extern void UartAppDebugConfig(void);
#endif		/**< asm32s003_types_local_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/