
#ifndef __UARTS_INTERFACE_H__
#define __UARTS_INTERFACE_H__

#include "type.h"
#include "uarts.h"
#include "dma.h"

/**
 * @brief      UART DMA 接收模式初始化，配置基础参数，接收缓存地址，接收缓存长度，中断触发门槛值，回调函数
 *
 * @param[in]  RxBufAddr  	接收缓冲区地址
 * @param[in]  RxBufLen   	接收缓存区长度设置
 * @param[in]  ThresholdLen 中断触发门槛值设置，注意设置值小于RxBufLen
 * @param[in]  CallBack   	中断回调函数
 * @return     FALSE初始化配置失败	TRUE初始化设置成功
 */
bool UART0_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);
bool UART1_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);
bool UART2_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);
/**
 * @brief      UART DMA 传送模式初始化，配置基础参数，发送缓存地址，发送缓存长度，中断触发门槛值，回调函数
 *
 * @param[in]  TxBufAddr  	传送缓冲区地址
 * @param[in]  TxBufLen   	传送缓存区长度设置
 * @param[in]  ThresholdLen 中断触发门槛值设置，注意设置值小于RxBufLen
 * @param[in]  CallBack   	中断回调函数
 * @return     FALSE初始化配置失败	TRUE初始化设置成功
 */
bool UART0_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);
bool UART1_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);
bool UART2_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);

/**
 * @brief      UART DMA 传送模式初始化，发送缓存区地址，发送缓存长度，中断触发门槛值，回调函数
 *
 * @param[in]  TxBufAddr  	传送缓冲区地址
 * @param[in]  TxBufLen   	传送字符长度
 * @param[in]  TimeOut 		超时时间设置，即达到设置的超时时间还未发送完成，则放弃发送
 * @return     实际发送数据长度
 */
uint32_t UART0_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut);
uint32_t UART1_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut);
uint32_t UART2_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut);
/**
 * @brief      UART DMA 接收数据，设置接收缓存地址，接收字符长度，接收超时设置
 *
 * @param[in]  RecvBuf  	接收缓冲区地址
 * @param[in]  BufLen   	接收长度设置
 * @param[in]  TimeOut		超时时间设置，规定时间内接收没有达到预期长度或者没收到收据则放弃接收
 * @return     接收到数据的实际长度
 */
uint32_t UART0_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut);
uint32_t UART1_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut);
uint32_t UART2_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut);

/**
 * @brief      UART DMA 传送数据，设置传入传送缓存区地址，传送字符长度
 *
 * @param[in]  RecvBuf  	传送缓冲区地址
 * @param[in]  BufLen   	传送字符长度设置
 * @return     无返回值
 */
void UART0_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen);
void UART1_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen);
void UART2_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen);

/**
 * @brief      判断数据传送是否完成
 * @return     TURE传送完成 	 FALSE传送未完成
  */
bool UART0_DMA_TxIsTransferDone(void);
bool UART1_DMA_TxIsTransferDone(void);
bool UART2_DMA_TxIsTransferDone(void);


/**
 * @brief      UART DMA 	非阻塞方式使能接收，设置接收缓存地址，接收字符长度
 *
 * @param[in]  RecvBuf  	接收缓冲区地址
 * @param[in]  BufLen   	接收长度设置
 * @return     接收到数据的实际长度
 */
int32_t UART0_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen);
int32_t UART1_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen);
int32_t UART2_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen);


/**
 * @brief      查询DMA接收缓存区数据长度
 * @return     返回数据长度
  */
uint16_t UART0_DMA_RxDataLen(void);
uint16_t UART1_DMA_RxDataLen(void);
uint16_t UART2_DMA_RxDataLen(void);

/**
 * @brief      查询DMA发送缓存区数据长度
 * @return     返回数据长度
  */
uint16_t UART0_DMA_TxDataLen(void);
uint16_t UART1_DMA_TxDataLen(void);
uint16_t UART2_DMA_TxDataLen(void);

/**
 * @brief      注册UART_DMA模式下的中断回调函数，注意配置中断类型
 *
 * @param[in]  IntType  	中断类型设置	DMA_DONE_INT无中断DMA_THRESHOLD_INT阈值触发中断DMA_ERROR_INT错误中断
 * param[in]   CallBack		需注册的中断回掉函数，若写入NULL，则关闭回调功能
 * @return     返回数据长度
  */
void UART0_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UART1_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UART2_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UART0_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UART1_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UART2_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack);

/**
 * @brief      UART_DMA使能函数
 *
 * @param[in]
 * @return
  */
void UART0_DMA_RxChannelEn(bool enable);
void UART1_DMA_RxChannelEn(bool enable);
void UART2_DMA_RxChannelEn(bool enable);
void UART0_DMA_TxChannelEn(bool enable);
void UART1_DMA_TxChannelEn(bool enable);
void UART2_DMA_TxChannelEn(bool enable);

/**
 * @brief      UART_DMA 中断标志位清除
 *
 * @param[in]   IntType  	中断类型设置	DMA_DONE_INT无中断DMA_THRESHOLD_INT阈值触发中断DMA_ERROR_INT错误中断
 * @return  	DMA_ERROR
 * @note	中断标志在不开启中断的情况下也会置位
  */
int32_t UART0_DMA_TxIntFlgClr(DMA_INT_TYPE IntType);
int32_t UART1_DMA_TxIntFlgClr(DMA_INT_TYPE IntType);
int32_t UART2_DMA_TxIntFlgClr(DMA_INT_TYPE IntType);
//RX flag clear
int32_t UART0_DMA_RxIntFlgClr(DMA_INT_TYPE IntType);
int32_t UART1_DMA_RxIntFlgClr(DMA_INT_TYPE IntType);
int32_t UART2_DMA_RxIntFlgClr(DMA_INT_TYPE IntType);
//add new APIs here...

/*UART MCU MAP*/
extern bool (*UARTS_Init_MAP[])(uint32_t BaudRate, uint8_t DataBits, uint8_t Parity, uint8_t StopBits);

extern uint32_t (*UARTS_Recv_MAP[])(uint8_t* RecvBuf, uint32_t BufLen);

extern bool (*UARTS_RecvByte_MAP[])(uint8_t* Val);

extern uint32_t (*UARTS_Send_MAP[])(uint8_t* SendBuf, uint32_t BufLen);

extern void (*UARTS_SendByte_MAP[])(uint8_t SendByte);
  
extern int32_t (*UARTS_IOCTL_MAP[])(UART_IOCTL_CMD_T Cmd, uint32_t Arg);


//DMA MAP
extern bool (*UARTS_DMA_RxInit_MAP[])(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);

extern bool (*UARTS_DMA_TxInit_MAP[])(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);

extern uint32_t (*UARTS_DMA_Send_MAP[])(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut);

extern uint32_t (*UARTS_DMA_Recv_MAP[])(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut);

extern void (*UARTS_DMA_SendDataStart_MAP[])(uint8_t* SendBuf, uint16_t BufLen);

extern bool (*UARTS_DMA_TxIsTransferDone_MAP[])(void);

extern int32_t (*UARTS_DMA_RecvDataStart_MAP[])(uint8_t* RecvBuf, uint16_t BufLen);

extern bool (*UARTS_DMA_RxIsTransferDone_MAP[])(void);

extern uint16_t (*UARTS_DMA_RxDataLen_MAP[])(void);

extern uint16_t (*UARTS_DMA_TxDataLen_MAP[])(void);

extern void (*UARTS_DMA_RxSetInterruptFun_MAP[])(DMA_INT_TYPE IntType, INT_FUNC CallBack);

extern void (*UARTS_DMA_TxSetInterruptFun_MAP[])(DMA_INT_TYPE IntType, INT_FUNC CallBack);

extern void (*UARTS_DMA_RxChannelEn_MAP[])(bool enable);

extern void (*UARTS_DMA_TxChannelEn_MAP[])(bool enable);

extern int32_t (*UARTS_DMA_TxIntFlgClr_MAP[])(DMA_INT_TYPE IntType);

extern int32_t (*UARTS_DMA_RxIntFlgClr_MAP[])(DMA_INT_TYPE IntType);

extern void (*UARTS_DMA_TxSetTransmitDoneFun_MAP[])(INT_FUNC CallBack);

extern void (*UARTS_DMA_RxSetTransmitDoneFun_MAP[])(INT_FUNC CallBack);

/*UART MCU mode*/
#define UARTS_Init(uartnum,BaudRate,DataBits,Parity,StopBits)  (*UARTS_Init_MAP[uartnum])(BaudRate,DataBits,Parity,StopBits)

#define UARTS_IOCTL(uartnum,cmd,Arg)  (*UARTS_IOCTL_MAP[uartnum])(cmd,Arg)

#define UARTS_Recv(uartnum, RecvBuf, BufLen) (*UARTS_Recv_MAP[uartnum])(RecvBuf, BufLen)

#define UARTS_RecvByte(uartnum, Val) (*UARTS_RecvByte_MAP[uartnum])(Val)

#define UARTS_Send(uartnum, SendBuf, BufLen) (*UARTS_Send_MAP[uartnum])(SendBuf, BufLen)

#define UARTS_SendByte(uartnum, SendByte) (*UARTS_SendByte_MAP[uartnum])(SendByte)

/*UART DMA mode*/
/*INIT*/
#define UARTS_DMA_RxInit(uartnum, RxBufAddr, RxBufLen, ThresholdLen, CallBack) (*UARTS_DMA_RxInit_MAP[uartnum])(RxBufAddr, RxBufLen, ThresholdLen, CallBack)

#define UARTS_DMA_TxInit(uartnum, TxBufAddr, TxBufLen, ThresholdLen, CallBack) (*UARTS_DMA_TxInit_MAP[uartnum])(TxBufAddr, TxBufLen, ThresholdLen, CallBack)


/*Send/Receive*/
#define UARTS_DMA_Send(uartnum, SendBuf, BufLen, TimeOut) (*UARTS_DMA_Send_MAP[uartnum])(SendBuf, BufLen, TimeOut)

#define UARTS_DMA_Recv(uartnum, RecvBuf, BufLen, TimeOut) (*UARTS_DMA_Recv_MAP[uartnum])(RecvBuf, BufLen, TimeOut)

#define UARTS_DMA_SendDataStart(uartnum, SendBuf, BufLen) (*UARTS_DMA_SendDataStart_MAP[uartnum])(SendBuf, BufLen)

#define UARTS_DMA_TxIsTransferDone(uartnum) (*UARTS_DMA_TxIsTransferDone_MAP[uartnum])()

#define UARTS_DMA_RecvDataStart(uartnum, RecvBuf, BufLen) (*UARTS_DMA_RecvDataStart_MAP[uartnum])(RecvBuf, BufLen)

#define UARTS_DMA_RxIsTransferDone(uartnum) (*UARTS_DMA_RxIsTransferDone_MAP[uartnum])();

#define UARTS_DMA_RxDataLen(uartnum) (*UARTS_DMA_RxDataLen_MAP[uartnum])()

#define UARTS_DMA_TxDataLen(uartnum) (*UARTS_DMA_TxDataLen_MAP[uartnum])()

/*Interrupt*/
#define UARTS_DMA_RxSetInterruptFun(uartnum, IntType, CallBack) (*UARTS_DMA_RxSetInterruptFun_MAP[uartnum])(IntType, CallBack)

#define UARTS_DMA_TxSetInterruptFun(uartnum, IntType, CallBack) (*UARTS_DMA_TxSetInterruptFun_MAP[uartnum])(IntType, CallBack)

#define UARTS_DMA_RxChannelEn(uartnum, enable) (*UARTS_DMA_RxChannelEn_MAP[uartnum])(enable)

#define UARTS_DMA_TxChannelEn(uartnum, enable) (*UARTS_DMA_TxChannelEn_MAP[uartnum])(enable)

#define UARTS_DMA_TxIntFlgClr(uartnum, IntType) (*UARTS_DMA_TxIntFlgClr_MAP[uartnum])(IntType)

#define UARTS_DMA_RxIntFlgClr(uartnum, IntType) (*UARTS_DMA_RxIntFlgClr_MAP[uartnum])(IntType)

#define UARTS_DMA_TxSetTransmitDoneFun(uartnum, CallBack) (*UARTS_DMA_TxSetTransmitDoneFun_MAP[uartnum])(CallBack)

#define UARTS_DMA_RxSetTransmitDoneFun(uartnum, CallBack) (*UARTS_DMA_RxSetTransmitDoneFun_MAP[uartnum])(CallBack)

#endif/*__UARTS_INTERFACE_H__*/
