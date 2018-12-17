
#include "type.h"
#include "debug.h"
#include "delay.h"
//#include "uart.h"
//#include "app_config.h"
#include "freeRTOS.h"
#include "bt_platform_interface.h"
//#include "app_msg.h"
#include "bt_config.h"
#include "uarts_interface.h"
#include "dma.h"

static bool BtPowerOn(void);
static bool BtUartInit(void);
static bool BtUartDeInit(void);
static bool BtUartChangeBaudRate(uint32_t baudrate);
static uint32_t BtUartSend(uint8_t * data, uint32_t dataLen, uint32_t timeout);
static uint32_t BtUartRecv(uint8_t * data, uint32_t dataLen, uint32_t timeout);
static void BtUartWaitMs(uint32_t ms);

#define BT_UART_PORT		1
#define BT_UART_RX_BUF_SIZE			(3*1024)
#define BT_UART_TX_BUF_SIZE			(1*1024)

PLATFORM_INTERFACE_BT_UART_T	pfiBtUart = {
		BtPowerOn,
		BtUartInit,
		BtUartDeInit,
		BtUartChangeBaudRate,
		BtUartSend,
		BtUartRecv,
		BtUartWaitMs,
};

uint8_t *uartRxBuffer;
uint8_t *uartTxBuffer;

static uint8_t dmaChannelMap[31] = {
    255,//PERIPHERAL_ID_SPIS_RX = 0,    //0
    255,//PERIPHERAL_ID_SPIS_TX,        //1
    255,//PERIPHERAL_ID_SD_RX,          //2
    255,//PERIPHERAL_ID_SD_TX,          //3
    255,//PERIPHERAL_ID_SPIM0_RX,       //4
    255,//PERIPHERAL_ID_SPIM0_TX,       //5
    255,//PERIPHERAL_ID_SPIM1_RX,       //6
    255,//PERIPHERAL_ID_SPIM1_TX,       //7
    255,//PERIPHERAL_ID_UART0_RX,         //8
    0,//PERIPHERAL_ID_UART1_RX,         //9
    255,//PERIPHERAL_ID_UART2_RX,       //10
    255,//PERIPHERAL_ID_TIMER3,           //11
    255,//PERIPHERAL_ID_TIMER4,         //12
    255,//PERIPHERAL_ID_I2S_TX,         //13
    255,//PERIPHERAL_ID_I2S_RX,         //14
    255,//PERIPHERAL_ID_SARADC,         //15
    255,//PERIPHERAL_ID_UART0_TX,       //16
    1,  //PERIPHERAL_ID_UART1_TX,       //17
    255,//PERIPHERAL_ID_UART2_TX,       //18
    255,//PERIPHERAL_ID_TIMER5,         //19
    255,//PERIPHERAL_ID_TIMER6,         //20
    255,//PERIPHERAL_ID_TIMER7,         //21
    255,//PERIPHERAL_ID_TIMER8,         //22
    255,//PERIPHERAL_ID_SOFTWARE,         //23
};

static bool BtPowerOn(void)
{
	BtLDOEn(FALSE);
	WaitMs(200);
	BtLDOEn(TRUE);
	WaitMs(200);
	return TRUE;
}

static bool BtUartInit(void)
{


	uartRxBuffer = pvPortMalloc(BT_UART_RX_BUF_SIZE);
	uartTxBuffer = pvPortMalloc(BT_UART_TX_BUF_SIZE);

	memset(uartRxBuffer, 0 , BT_UART_RX_BUF_SIZE);
	memset(uartTxBuffer, 0 , BT_UART_TX_BUF_SIZE);

	UARTS_Init(BT_UART_PORT, 115200, 8, 0, 1);

	DMA_ChannelAllocTableSet(dmaChannelMap);

	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_RXINT_SET, 0);
	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_TXINT_SET, 0);


	UARTS_DMA_RxInit(BT_UART_PORT, uartRxBuffer, BT_UART_RX_BUF_SIZE, 0, NULL);
	UARTS_DMA_TxInit(BT_UART_PORT, uartTxBuffer, BT_UART_TX_BUF_SIZE, 0, NULL);

#if FLOW_CTRL == ENABLE
	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_RXRTS_FLOWCTL_SET, 2);
#endif
	return TRUE;
}

static bool BtUartDeInit(void)
{

	return TRUE;
}

static bool BtUartChangeBaudRate(uint32_t baudrate)
{
//	//change uart baud rate
	DBG("change baudrate : %d\n", baudrate);
	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_DISENRX_SET,0);

	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_BAUDRATE_SET,baudrate);//different bt module may have the differnent bautrate!!

	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_DISENRX_SET,1);

	UARTS_IOCTL(BT_UART_PORT, UART_IOCTL_FUNC_RESET,1);

	return TRUE;
}

static uint32_t BtUartSend(uint8_t * data, uint32_t dataLen, uint32_t timeout)
{
	return UARTS_DMA_Send(BT_UART_PORT, data, dataLen, timeout);
}

static uint32_t BtUartRecv(uint8_t * data, uint32_t dataLen, uint32_t timeout)
{
	return UARTS_DMA_Recv(BT_UART_PORT, data, dataLen, timeout);
}

static void BtUartWaitMs(uint32_t ms)
{
	WaitMs(ms);
}

