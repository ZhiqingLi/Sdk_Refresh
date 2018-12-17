/**
 **************************************************************************************
 * @file    spdif_interface.c
 * @brief   audio spdif interface
 *
 * @author  Jerry.rao
 * @version V1.0.0
 *
 * $Created: 2015-10-28 16:15:00$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 
#include "type.h"
//#include "app_config.h"
#include "uarts.h"
#include "gpio.h"
#include "dma.h"
#include "timeout.h"
#include "uarts_interface.h"


//串口驱动APIs的包装函数，用于OS加锁
bool UART0_InitWpr(uint32_t BaudRate, uint8_t DataBits, uint8_t Parity, uint8_t StopBits)
{
	return UART0_Init(BaudRate, DataBits, Parity, StopBits);
}

bool UART1_InitWpr(uint32_t BaudRate, uint8_t DataBits, uint8_t Parity, uint8_t StopBits)
{
	return UART1_Init(BaudRate, DataBits, Parity, StopBits);
}

bool UART2_InitWpr(uint32_t BaudRate, uint8_t DataBits, uint8_t Parity, uint8_t StopBits)
{
	return UART2_Init(BaudRate, DataBits, Parity, StopBits);
}

uint32_t UART0_RecvWpr(uint8_t* RecvBuf, uint32_t BufLen)
{
	return UART0_Recv(RecvBuf, BufLen);
}

uint32_t UART1_RecvWpr(uint8_t* RecvBuf, uint32_t BufLen)
{
	return UART1_Recv(RecvBuf, BufLen);
}

uint32_t UART2_RecvWpr(uint8_t* RecvBuf, uint32_t BufLen)
{
	return UART2_Recv(RecvBuf, BufLen);
}


bool UART0_RecvByteWpr(uint8_t* Val)
{
	return UART0_RecvByte(Val);
}

bool UART1_RecvByteWpr(uint8_t* Val)
{
	return UART1_RecvByte(Val);
}

bool UART2_RecvByteWpr(uint8_t* Val)
{
	return UART2_RecvByte(Val);
}

uint32_t UART0_SendWpr(uint8_t* SendBuf, uint32_t BufLen)
{
	return UART0_Send(SendBuf, BufLen);
}

uint32_t UART1_SendWpr(uint8_t* SendBuf, uint32_t BufLen)
{
	return UART1_Send(SendBuf, BufLen);
}

uint32_t UART2_SendWpr(uint8_t* SendBuf, uint32_t BufLen)
{
	return UART2_Send(SendBuf, BufLen);
}

void UART0_SendByteWpr(uint8_t SendByte)
{
	UART0_SendByte(SendByte);
}

void UART1_SendByteWpr(uint8_t SendByte)
{
	UART1_SendByte(SendByte);
}

void UART2_SendByteWpr(uint8_t SendByte)
{
	UART2_SendByte(SendByte);
}


int32_t UART0_IOCtlWpr(UART_IOCTL_CMD_T Cmd, uint32_t Arg)
{
	return UART0_IOCtl(Cmd, Arg);
}

int32_t UART1_IOCtlWpr(UART_IOCTL_CMD_T Cmd, uint32_t Arg)
{
	return UART1_IOCtl(Cmd, Arg);
}

int32_t UART2_IOCtlWpr(UART_IOCTL_CMD_T Cmd, uint32_t Arg)
{
	return UART2_IOCtl(Cmd, Arg);
}

//UART DMA方式的完成回调函数
//对于接收，数据的路径:UART---->UART Rx FIFO---->DMA Rx buffer 回调函数放在DMA接收缓冲端
//对于发送，数据的路径:DMA Tx buffer---->UART Tx FIFO---->UART 回调函数放在UART发送中断ISR端
static INT_FUNC DmaTxIntDoneFun[3] = {
	NULL,
	NULL,
	NULL,
};

static INT_FUNC DmaRxIntDoneFun[3] = {
	NULL,
	NULL,
	NULL,
};

//目前确认UART-DMA采用Circular方式发送和接收数据
//接收方式初始化
bool UART0_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART0_RX, 0, RxBufAddr, RxBufLen) != DMA_OK)
    {
        return FALSE;
    }
	
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART0_RX, ThresholdLen);
	DMA_InterruptFunSet(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT, CallBack);
	DMA_InterruptEnable(PERIPHERAL_ID_UART0_RX, DMA_THRESHOLD_INT, 1);
    DMA_ChannelEnable(PERIPHERAL_ID_UART0_RX, 1);
	UART0_IOCtl(UART_IOCTL_DMA_RX_EN, 1);
    return TRUE;
}

bool UART1_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART1_RX, 0, RxBufAddr, RxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART1_RX, ThresholdLen);
	DMA_InterruptFunSet(PERIPHERAL_ID_UART1_RX, DMA_THRESHOLD_INT, CallBack);
	DMA_InterruptEnable(PERIPHERAL_ID_UART1_RX, DMA_THRESHOLD_INT, 1);
    DMA_ChannelEnable(PERIPHERAL_ID_UART1_RX, 1);
    UART1_IOCtl(UART_IOCTL_DMA_RX_EN, 1);
    return TRUE;
}

bool UART2_DMA_RxInit(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART2_RX, 0, RxBufAddr, RxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART2_RX, ThresholdLen);
	DMA_InterruptFunSet(PERIPHERAL_ID_UART2_RX, DMA_THRESHOLD_INT, CallBack);
	DMA_InterruptEnable(PERIPHERAL_ID_UART2_RX, DMA_THRESHOLD_INT, 1);
    DMA_ChannelEnable(PERIPHERAL_ID_UART2_RX, 1);
    UART2_IOCtl(UART_IOCTL_DMA_RX_EN, 1);
    return TRUE;
}

//发送方式初始化
bool UART0_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART0_TX, 0, TxBufAddr, TxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART0_TX, ThresholdLen);
    DMA_ChannelEnable(PERIPHERAL_ID_UART0_TX, 1);
	DmaTxIntDoneFun[UART_PORT0] = CallBack;
    UART0_IOCtl(UART_IOCTL_DMA_TX_EN, 1);
    return TRUE;
}

bool UART1_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART1_TX, 0, TxBufAddr, TxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART1_TX, ThresholdLen);
    DMA_ChannelEnable(PERIPHERAL_ID_UART1_TX, 1);
	DmaTxIntDoneFun[UART_PORT1] = CallBack;
    UART1_IOCtl(UART_IOCTL_DMA_TX_EN, 1);
    return TRUE;
}

bool UART2_DMA_TxInit(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
    if(DMA_CircularConfig(PERIPHERAL_ID_UART2_TX, 0, TxBufAddr, TxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(PERIPHERAL_ID_UART2_TX, ThresholdLen);
    DMA_ChannelEnable(PERIPHERAL_ID_UART2_TX, 1);
	DmaTxIntDoneFun[UART_PORT2] = CallBack;
    UART2_IOCtl(UART_IOCTL_DMA_TX_EN, 1);
    return TRUE;
}

uint32_t UART0_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART0_TX);//如上次发送数据没有完成，会在此处被清掉
    DMA_CircularDataPut(PERIPHERAL_ID_UART0_TX, SendBuf, BufLen);
    while(DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_TX) && !IsTimeOut(&TimerRx));
    while(!UART0_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0));
    return BufLen;
}

uint32_t UART1_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART1_TX);
    DMA_CircularDataPut(PERIPHERAL_ID_UART1_TX, SendBuf, BufLen);
    while(DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_TX) && !IsTimeOut(&TimerRx) );
    while(!UART1_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0));
    return BufLen;
}

uint32_t UART2_DMA_Send(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART2_TX);
    DMA_CircularDataPut(PERIPHERAL_ID_UART2_TX, SendBuf, BufLen);
    while(DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_TX) && !IsTimeOut(&TimerRx));
    while(!UART2_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0));
    return BufLen;
}

uint32_t UART0_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    while(( DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_RX) < BufLen) && !IsTimeOut(&TimerRx));
    return DMA_CircularDataGet(PERIPHERAL_ID_UART0_RX, RecvBuf, BufLen);
}

uint32_t UART1_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    while(( DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_RX) < BufLen) && !IsTimeOut(&TimerRx));
    return DMA_CircularDataGet(PERIPHERAL_ID_UART1_RX, RecvBuf, BufLen);
}

uint32_t UART2_DMA_Recv(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	TimeOutSet(&TimerRx, TimeOut);
    while(( DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_RX) < BufLen) && !IsTimeOut(&TimerRx));
    return DMA_CircularDataGet(PERIPHERAL_ID_UART2_RX, RecvBuf, BufLen);
}

//非阻塞方式发送数据(查询是否完成的接口函数)
//准备数据，使能发送
void UART0_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen)
{
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART0_TX);//如上次发送数据没有完成，会在此处被清掉
    DMA_CircularDataPut(PERIPHERAL_ID_UART0_TX, SendBuf, BufLen);
}

void UART1_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen)
{
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART1_TX);//如上次发送数据没有完成，会在此处被清掉
    DMA_CircularDataPut(PERIPHERAL_ID_UART1_TX, SendBuf, BufLen);
}

void UART2_DMA_SendDataStart(uint8_t* SendBuf, uint16_t BufLen)
{
    DMA_CircularFIFOClear(PERIPHERAL_ID_UART2_TX);//如上次发送数据没有完成，会在此处被清掉
    DMA_CircularDataPut(PERIPHERAL_ID_UART2_TX, SendBuf, BufLen);
}

//判断是否传输完成
bool UART0_DMA_TxIsTransferDone(void)
{
	/*DMA BUFFER--->UART FIFO--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_TX) == 0 && UART0_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

bool UART1_DMA_TxIsTransferDone(void)
{
	/*DMA BUFFER--->UART FIFO--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_TX) == 0 && UART1_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

bool UART2_DMA_TxIsTransferDone(void)
{
	/*DMA BUFFER--->UART FIFO--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_TX) == 0 && UART2_IOCtl(UART_IOCTL_TX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

//非阻塞方式接收数据(查询是否完成的接口函数)
//准备缓冲区
int32_t UART0_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen)
{
	return DMA_CircularDataGet(PERIPHERAL_ID_UART0_RX, RecvBuf, BufLen);
}

int32_t UART1_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen)
{
	return DMA_CircularDataGet(PERIPHERAL_ID_UART1_RX, RecvBuf, BufLen);
}

int32_t UART2_DMA_RecvDataStart(uint8_t* RecvBuf, uint16_t BufLen)
{
	return DMA_CircularDataGet(PERIPHERAL_ID_UART2_RX, RecvBuf, BufLen);
}

//判断是否传输完成
bool UART0_DMA_RxIsTransferDone(void)
{
	/*UART FIFO--->DMA BUFFER--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_RX) == 0 && UART0_IOCtl(UART_IOCTL_RX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

bool UART1_DMA_RxIsTransferDone(void)
{
	/*UART FIFO--->DMA BUFFER--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_RX) == 0 && UART1_IOCtl(UART_IOCTL_RX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

bool UART2_DMA_RxIsTransferDone(void)
{
	/*UART FIFO--->DMA BUFFER--->*/
	if(DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_RX) == 0 && UART2_IOCtl(UART_IOCTL_RX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

//检查缓冲区数据长度
uint16_t UART0_DMA_RxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_RX);
}

uint16_t UART1_DMA_RxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_RX);
}

uint16_t UART2_DMA_RxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_RX);
}

uint16_t UART0_DMA_TxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART0_TX);
}

uint16_t UART1_DMA_TxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART1_TX);
}

uint16_t UART2_DMA_TxDataLen(void)
{
	return DMA_CircularDataLenGet(PERIPHERAL_ID_UART2_TX);
}

//DMA中断方式回调注册及关闭
void UART0_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART0_RX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART0_RX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART0_RX, IntType, 0);
	}
}

void UART1_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART1_RX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART1_RX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART1_RX, IntType, 0);
	}
}

void UART2_DMA_RxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART2_RX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART2_RX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART2_RX, IntType, 0);
	}
}

void UART0_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART0_TX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART0_TX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART0_TX, IntType, 0);
	}
}

void UART1_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART1_TX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART1_TX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART1_TX, IntType, 0);
	}
}

void UART2_DMA_TxSetInterruptFun(DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	if(CallBack)
	{
		DMA_InterruptFunSet(PERIPHERAL_ID_UART2_TX, IntType, CallBack);
		DMA_InterruptEnable(PERIPHERAL_ID_UART2_TX, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(PERIPHERAL_ID_UART2_TX, IntType, 0);
	}
}

//完成函数回调注册
//Tx
void UART0_DMA_TxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaTxIntDoneFun[UART_PORT0] = CallBack;
}

void UART1_DMA_TxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaTxIntDoneFun[UART_PORT1] = CallBack;
}

void UART2_DMA_TxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaTxIntDoneFun[UART_PORT2] = CallBack;
}

//Rx
void UART0_DMA_RxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaRxIntDoneFun[UART_PORT0] = CallBack;
}

void UART1_DMA_RxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaRxIntDoneFun[UART_PORT1] = CallBack;
}

void UART2_DMA_RxSetTransmitDoneFun(INT_FUNC CallBack)
{
	DmaRxIntDoneFun[UART_PORT2] = CallBack;
}


//DMA 通道使能，设置DMA开始/停止工作
void UART0_DMA_RxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_RX, enable);
}

void UART1_DMA_RxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART1_RX, enable);
}

void UART2_DMA_RxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART2_RX, enable);
}

void UART0_DMA_TxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART0_TX, enable);
}

void UART1_DMA_TxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART1_TX, enable);
}

void UART2_DMA_TxChannelEn(bool enable)
{
	DMA_ChannelEnable(PERIPHERAL_ID_UART2_TX, enable);
}

//DMA 中断标志位清除
//TX flag clear
int32_t UART0_DMA_TxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_TX, IntType);
}

int32_t UART1_DMA_TxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART1_TX, IntType);
}

int32_t UART2_DMA_TxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART2_TX, IntType);
}

//RX flag clear
int32_t UART0_DMA_RxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART0_RX, IntType);
}

int32_t UART1_DMA_RxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART1_RX, IntType);
}

int32_t UART2_DMA_RxIntFlgClr(DMA_INT_TYPE IntType)
{
	return DMA_InterruptFlagClear(PERIPHERAL_ID_UART2_RX, IntType);
}




/*******根据测试需要定义*************/
//uint8_t UART0_RcData[4]={0};
//uint8_t UART0_RcCount=0;
//uint8_t UART0_RcFlag=0;



/********************************/
//串口中断服务程序ISR，可以按照需求重新实现
//判断中断标志位之前，需要先判断中断使能，否则极易进入非预想的逻辑分支
extern void UART_RxCallback(uint8_t data);
__attribute__((section(".driver.isr")))
void UART0_Interrupt(void)
{
    //UART0
	uint8_t rxdata;
    if(UARTS_IOCTL(UART_PORT0, UART_IOCTL_DMA_RX_GET_EN, 0) != 1)
    {
		//callback
		if( UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXINT_GET, 0)
			&&(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x01))//RX INT
		{
			UART0_RecvByte(&rxdata);
			UART_RxCallback(rxdata);
			UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXINT_CLR, 1);
		}
    }
	
	if( UARTS_IOCTL(UART_PORT0, UART_IOCTL_RX_ERR_INT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x1C))//ERROR INT
	{
		//DBG("Uart0 RX error(0x%02X)\n", (uint32_t)(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x1C));
		UARTS_IOCTL(UART_PORT0, UART_IOCTL_RX_ERR_CLR, 1);
	}

	if( UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXINT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXSTAT_GET, 0)))//TX INT
	{
		if(DmaTxIntDoneFun[UART_PORT0])
			(*DmaTxIntDoneFun[UART_PORT0])();
		UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXINT_CLR, 1);
	}
	
    if(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x02) { //OVERTIME INT
        //DBG("Uart0 RX OverTime INT.\n");
        UARTS_IOCTL(UART_PORT0, UART_IOCTL_OVERTIME_CLR, 1);
    }
	
}

__attribute__((section(".driver.isr")))
void UART1_Interrupt(void)
{
    //UART1
	uint8_t rxdata;
    if(UARTS_IOCTL(UART_PORT1, UART_IOCTL_DMA_RX_GET_EN, 0) != 1)
    {
		if( UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXINT_GET, 0)
			&& (UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXSTAT_GET, 0) & 0x01))//RX INT
		{
			UART1_RecvByte(&rxdata);
			UART_RxCallback(rxdata);
			UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXINT_CLR, 1);
		}
    }
	
	if( UARTS_IOCTL(UART_PORT1, UART_IOCTL_RX_ERR_INT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXSTAT_GET, 0) & 0x1C))//ERROR INT
	{
		//DBG("Uart2 RX error(0x%02X)\n", (uint32_t)(UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXSTAT_GET, 0) & 0x1C));
		UARTS_IOCTL(UART_PORT1, UART_IOCTL_RX_ERR_CLR, 1);
	}

	if( UARTS_IOCTL(UART_PORT1, UART_IOCTL_TXINT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT1, UART_IOCTL_TXSTAT_GET, 0)))//TX INT
	{
		if(DmaTxIntDoneFun[UART_PORT1])
			(*DmaTxIntDoneFun[UART_PORT1])();
		UARTS_IOCTL(UART_PORT1, UART_IOCTL_TXINT_CLR, 1);
	}

    if(UARTS_IOCTL(UART_PORT1, UART_IOCTL_RXSTAT_GET, 0) & 0x02) { //OVERTIME INT
        //DBG("Uart1 RX OverTime INT.\n");
        UARTS_IOCTL(UART_PORT1, UART_IOCTL_OVERTIME_CLR, 1);
    }
}

__attribute__((section(".driver.isr")))
void UART2_Interrupt(void)
{
    //UART2
	uint8_t rxdata;
    if(UARTS_IOCTL(UART_PORT2, UART_IOCTL_DMA_RX_GET_EN, 0) != 1)
    {
		if( UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXINT_GET, 0)
			&& (UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXSTAT_GET, 0) & 0x01))//RX INT
		{
			UART2_RecvByte(&rxdata);
			UART_RxCallback(rxdata);
			UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXINT_CLR, 1);
		}
    }
	
	if( UARTS_IOCTL(UART_PORT2, UART_IOCTL_RX_ERR_INT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXSTAT_GET, 0) & 0x1C))//ERROR INT
	{
		//DBG("Uart2 RX error(0x%02X)\n", (uint32_t)(UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXSTAT_GET, 0) & 0x1C));
		UARTS_IOCTL(UART_PORT2, UART_IOCTL_RX_ERR_CLR, 1);
	}

	if( UARTS_IOCTL(UART_PORT2, UART_IOCTL_TXINT_GET, 0)
		&& (UARTS_IOCTL(UART_PORT2, UART_IOCTL_TXSTAT_GET, 0)))//TX INT
	{
		if(DmaTxIntDoneFun[UART_PORT2])
			(*DmaTxIntDoneFun[UART_PORT2])();
		UARTS_IOCTL(UART_PORT2, UART_IOCTL_TXINT_CLR, 1);
	}

    if(UARTS_IOCTL(UART_PORT2, UART_IOCTL_RXSTAT_GET, 0) & 0x02) { //OVERTIME INT
        //DBG("Uart2 RX OverTime INT.\n");
        UARTS_IOCTL(UART_PORT2, UART_IOCTL_OVERTIME_CLR, 1);
    }
}
/******************************************FUNCTION MAPS BEGIN*********************************************************************/
bool (*UARTS_Init_MAP[3])(uint32_t BaudRate, uint8_t DataBits, uint8_t Parity, uint8_t StopBits) = {
	UART0_InitWpr,
	UART1_InitWpr,
	UART2_InitWpr,
};

uint32_t (*UARTS_Recv_MAP[3])(uint8_t* RecvBuf, uint32_t BufLen) = {
	UART0_RecvWpr,
	UART1_RecvWpr,
	UART2_RecvWpr,
};

bool (*UARTS_RecvByte_MAP[3])(uint8_t* Val) = {
	UART0_RecvByteWpr,
	UART1_RecvByteWpr,
	UART2_RecvByteWpr,
};

uint32_t (*UARTS_Send_MAP[3])(uint8_t* SendBuf, uint32_t BufLen) = {
	UART0_SendWpr,
	UART1_SendWpr,
	UART2_SendWpr,
};

void (*UARTS_SendByte_MAP[3])(uint8_t SendByte) = {
	UART0_SendByteWpr,
	UART1_SendByteWpr,
	UART2_SendByteWpr,
};


int32_t (*UARTS_IOCTL_MAP[3])(UART_IOCTL_CMD_T Cmd, uint32_t Arg) = {
	UART0_IOCtlWpr,
	UART1_IOCtlWpr,
	UART2_IOCtlWpr,
};


/*UART DMA INTERFACE*/
bool (*UARTS_DMA_RxInit_MAP[3])(void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack) = {
	UART0_DMA_RxInit,
	UART1_DMA_RxInit,
	UART2_DMA_RxInit,
};

bool (*UARTS_DMA_TxInit_MAP[3])(void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack) = {
	UART0_DMA_TxInit,
	UART1_DMA_TxInit,
	UART2_DMA_TxInit
};

uint32_t (*UARTS_DMA_Send_MAP[3])(uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut) = {
	UART0_DMA_Send,
	UART1_DMA_Send,
	UART2_DMA_Send
};

uint32_t (*UARTS_DMA_Recv_MAP[3])(uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut) = {
	UART0_DMA_Recv,
	UART1_DMA_Recv,
	UART2_DMA_Recv,
};

void (*UARTS_DMA_SendDataStart_MAP[3])(uint8_t* SendBuf, uint16_t BufLen) = {
	UART0_DMA_SendDataStart,
	UART1_DMA_SendDataStart,
	UART2_DMA_SendDataStart,
};

bool (*UARTS_DMA_TxIsTransferDone_MAP[3])(void) = {
	UART0_DMA_TxIsTransferDone,
	UART1_DMA_TxIsTransferDone,
	UART2_DMA_TxIsTransferDone,
};

int32_t (*UARTS_DMA_RecvDataStart_MAP[3])(uint8_t* RecvBuf, uint16_t BufLen) = {
	UART0_DMA_RecvDataStart,
	UART1_DMA_RecvDataStart,
	UART2_DMA_RecvDataStart,
};

bool (*UARTS_DMA_RxIsTransferDone_MAP[3])(void) = {
	UART0_DMA_RxIsTransferDone,
	UART1_DMA_RxIsTransferDone,
	UART2_DMA_RxIsTransferDone,
};

uint16_t (*UARTS_DMA_RxDataLen_MAP[3])(void) = {
	UART0_DMA_RxDataLen,
	UART1_DMA_RxDataLen,
	UART2_DMA_RxDataLen,
};

uint16_t (*UARTS_DMA_TxDataLen_MAP[3])(void) = {
	UART0_DMA_TxDataLen,
	UART1_DMA_TxDataLen,
	UART2_DMA_TxDataLen,
};

void (*UARTS_DMA_RxSetInterruptFun_MAP[3])(DMA_INT_TYPE IntType, INT_FUNC CallBack) = {
	UART0_DMA_RxSetInterruptFun,
	UART1_DMA_RxSetInterruptFun,
	UART2_DMA_RxSetInterruptFun,
};

void (*UARTS_DMA_TxSetInterruptFun_MAP[3])(DMA_INT_TYPE IntType, INT_FUNC CallBack) = {
	UART0_DMA_TxSetInterruptFun,
	UART1_DMA_TxSetInterruptFun,
	UART2_DMA_TxSetInterruptFun,
};

void (*UARTS_DMA_RxChannelEn_MAP[3])(bool enable) = {
	UART0_DMA_RxChannelEn,
	UART1_DMA_RxChannelEn,
	UART2_DMA_RxChannelEn,
};

void (*UARTS_DMA_TxChannelEn_MAP[3])(bool enable) = {
	UART0_DMA_TxChannelEn,
	UART1_DMA_TxChannelEn,
	UART2_DMA_TxChannelEn,
};

int32_t (*UARTS_DMA_TxIntFlgClr_MAP[3])(DMA_INT_TYPE IntType) = {
	UART0_DMA_TxIntFlgClr,
	UART1_DMA_TxIntFlgClr,
	UART2_DMA_TxIntFlgClr,
};

int32_t (*UARTS_DMA_RxIntFlgClr_MAP[3])(DMA_INT_TYPE IntType) = {
	UART0_DMA_RxIntFlgClr,
	UART1_DMA_RxIntFlgClr,
	UART2_DMA_RxIntFlgClr,
};

void (*UARTS_DMA_TxSetTransmitDoneFun_MAP[3])(INT_FUNC CallBack) = {
	UART0_DMA_TxSetTransmitDoneFun,
	UART1_DMA_TxSetTransmitDoneFun,
	UART2_DMA_TxSetTransmitDoneFun,
};

void (*UARTS_DMA_RxSetTransmitDoneFun_MAP[3])(INT_FUNC CallBack) = {
	UART0_DMA_RxSetTransmitDoneFun,
	UART1_DMA_RxSetTransmitDoneFun,
	UART2_DMA_RxSetTransmitDoneFun,
};

/******************************************FUNCTION MAPS ENDS****************************************************************/




