
////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2016, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:uart_example.c
//		Author：Messi
//
//		Description	:
//					create example to show how to use uart
//
//		Changelog	:
//					2016-11-28	create uart example
///////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "watchdog.h"
#include "timeout.h"
#include "clk.h"
#include "dma.h"
#include "timer.h"
#include "irqn.h"
#include "spi_flash.h"
typedef struct __BUFFER_INFO
{
	uint16_t	Read;		/* @field Current Read index. */
	uint16_t	Write;		/* @field Current Write index. */
	uint16_t	Length;		/* @field Length of buffer */
	uint8_t	*CharBuffer;		/* @field Start of buffer */
}BUFFER_INFO;

#define ResetFifo(Buffer)   		(Buffer.Write = Buffer.Read = 0)
#define BWrite(Buffer)	   		(Buffer->Write)
#define BRead(Buffer)		   	(Buffer->Read)
#define BLength(Buffer)	   		(Buffer->Length)
#define BStartAddr(Buffer)     (Buffer->CharBuffer)
#define BuffWrite(Buffer)   		(Buffer->CharBuffer+Buffer->Write)
#define BuffRead(Buffer)	   	(Buffer->CharBuffer+Buffer->Read)

#define BWrite_addr(Buffer)	   (Buffer.Write)
#define BRead_addr(Buffer)		   (Buffer.Read)
#define BLength_addr(Buffer)	   (Buffer.Length)
#define BuffWrite_addr(Buffer)   (Buffer.CharBuffer+Buffer.Write)
#define BuffRead_addr(Buffer)	   (Buffer.CharBuffer+Buffer.Read)
#define Buff_EndAddr(Buffer)     (Buffer.CharBuffer+Buffer.Length-1)
#define Buff_StartAddr(Buffer)     (Buffer.CharBuffer)

#define Buff_isEmpty	1
#define Buff_notEmpty	0
#define Buff_isFull	1
#define Buff_notFull	0
#define Buff_PushOK	0
#define Buff_PushErr	1
#define Buff_PopOK	0
#define Buff_PopErr	1

#define Buf_init(_Buffer,_Buffaddr,_uTotalSize) \
{\
   BUFFER_INFO *_Buf=_Buffer;\
   _Buf->Read = 0;\
	_Buf->Write = 0;\
	_Buf->Length = _uTotalSize;\
	_Buf->CharBuffer = _Buffaddr;\
}\

#define Buf_IsFull(_Buffer,_result)   \
{\
   BUFFER_INFO *_Buf=_Buffer;\
	uint16_t _tmp = BRead(_Buf);\
	if (_tmp == 0)\
		_tmp = BLength(_Buf);\
	if ( (_tmp-BWrite(_Buf)) == 1)\
	{\
		_result = Buff_isFull;\
	}\
	else\
	{\
		_result = Buff_notFull;\
	}\
}\

#define Buf_GetRoomLeft(_Buffer,_RoomLeft)   \
{\
   BUFFER_INFO *_Buf=_Buffer;\
   if ( BRead(_Buf) <= BWrite(_Buf) ) \
	{\
      _RoomLeft = BLength(_Buf) - BWrite(_Buf) + BRead(_Buf) - 1;\
	}\
	else\
	{\
		_RoomLeft = BRead(_Buf) - BWrite(_Buf) - 1;\
	}\
}\

#define Buf_Push_Multi(_Buffer,_pushDataPointer,_multi,offset) \
{\
   BUFFER_INFO *_Buf=_Buffer;\
   if( (BWrite(_Buf) + _multi) >= BLength(_Buf) ){\
      memcpy(BuffWrite(_Buf), _pushDataPointer + offset, (BLength(_Buf) - BWrite(_Buf))*sizeof(uint8_t));\
      memcpy(BStartAddr(_Buf) , _pushDataPointer + offset + (BLength(_Buf) - BWrite(_Buf)), (_multi- (BLength(_Buf) - BWrite(_Buf)))*sizeof(uint8_t));\
      BWrite(_Buf) = (BWrite(_Buf) + _multi) - BLength(_Buf);\
   }\
   else{\
   memcpy(BuffWrite(_Buf), _pushDataPointer + offset, _multi*sizeof(uint8_t));\
         BWrite(_Buf) += _multi;\
}\
}\

#define Buf_Push(_Buffer,_pushData) \
{\
   BUFFER_INFO *_Buf=_Buffer;\
   *BuffWrite(_Buf) = _pushData;\
   if(BWrite(_Buf) >= (BLength(_Buf) - 1))\
   {\
   	BWrite(_Buf) = 0;\
   }\
   else\
   {\
	BWrite(_Buf)++;\
   }\
}\

#define Buf_GetBytesAvail(_Buffer,_BytesAvail) \
{\
   BUFFER_INFO *_Buf = _Buffer;\
	_BytesAvail = 0;\
	if (BWrite(_Buf) >= BRead(_Buf))\
		_BytesAvail = BWrite(_Buf) - BRead(_Buf);\
	else\
		_BytesAvail = BLength(_Buf) - BRead(_Buf) + BWrite(_Buf);	\
}\

#define Buf_Pop_Multi(_Buffer,_popDataPointer,_multi,offset) \
{\
   BUFFER_INFO *_Buf=_Buffer;\
   if( (BRead(_Buf) + _multi) >= BLength(_Buf) ){\
   	memcpy(_popDataPointer + offset, BuffRead(_Buf), (BLength(_Buf) - BRead(_Buf))*sizeof(uint8_t));\
   	memcpy(_popDataPointer + offset + (BLength(_Buf) - BRead(_Buf)), BStartAddr(_Buf), (_multi- (BLength(_Buf) - BRead(_Buf)))*sizeof(uint8_t));\
         BRead(_Buf) = BRead(_Buf) + _multi - BLength(_Buf) ;}\
   else{\
   	 memcpy(_popDataPointer + offset, BuffRead(_Buf), _multi);\
         BRead(_Buf) += _multi;}\
}\

#define Buf_Flush(_Buffer) \
{\
   BUFFER_INFO *_Buf = _Buffer;\
	_Buf->Write = _Buf->Read = 0;\
}




//#define       MASTER_DEVICE           //if master device, send data first, to trigger the slave device
#define TEST_UART_NUM 1
#define GPIO_MULTI_USE_TX 1
#define GPIO_MULTI_USE_RX 1
#define GPIO_MULTI_USE_RTS 1
#define GPIO_MULTI_USE_CTS 1

#define     MAX_ROUND_NUM 8
#define     DMA_TST_BUF_LEN 256 //eq to CRC_DATA_BLCOK_LEN

static uint8_t CurrTestUartNum = 1;

static uint8_t DmaTxBuf[512];//DmaTestBuf
static uint8_t DmaRxBuf[1024];//DmaTestBufRx
static uint8_t DmaTempBuf[512];//DmaTestBuf1
//static uint8_t DmaTxAuto[DMA_TST_BUF_LEN];
static uint8_t crc_check_buf[DMA_TST_BUF_LEN+1];

static uint16_t dLenInDma = 0;

uint8_t BuartRGBuf[1024];
BUFFER_INFO g_dma_ring_buf = {0};
static uint8_t DmaChannelMap[31] = {
    255,//PERIPHERAL_ID_SPIS_RX = 0,    //0
    255,//PERIPHERAL_ID_SPIS_TX,        //1
    255,//PERIPHERAL_ID_SD_RX,          //2
    255,//PERIPHERAL_ID_SD_TX,          //3
    255,//PERIPHERAL_ID_SPIM0_RX,       //4
    255,//PERIPHERAL_ID_SPIM0_TX,       //5
    255,//PERIPHERAL_ID_SPIM1_RX,       //6
    255,//PERIPHERAL_ID_SPIM1_TX,       //7
    5,//PERIPHERAL_ID_UART0_RX,         //8
    0,//PERIPHERAL_ID_UART1_RX,         //9
    255,//PERIPHERAL_ID_UART2_RX,       //10
    3,//PERIPHERAL_ID_TIMER3,           //11
    255,//PERIPHERAL_ID_TIMER4,         //12
    255,//PERIPHERAL_ID_I2S_TX,         //13
    255,//PERIPHERAL_ID_I2S_RX,         //14
    255,//PERIPHERAL_ID_SARADC,         //15
    4,//PERIPHERAL_ID_UART0_TX,       //16
    1,  //PERIPHERAL_ID_UART1_TX,       //17
    255,//PERIPHERAL_ID_UART2_TX,       //18
    255,//PERIPHERAL_ID_TIMER5,         //19
    255,//PERIPHERAL_ID_TIMER6,         //20
    255,//PERIPHERAL_ID_TIMER7,         //21
    255,//PERIPHERAL_ID_TIMER8,         //22
    2,//PERIPHERAL_ID_SOFTWARE,         //23
};

#define BEGIN_CASE_NUM 959
#define END_CASE_NUM 4096//2048
static const uint32_t At_Uart_BaudRate[] = {
    3000000,
    2764800,
    1843200,
    1382400,
    921600,
    460800,
    230400,
    115200,
    76800,
    57600,
    38400,
    19200,
    9600,
    4800,
    2400,
    1200,
};

static const uint8_t At_Uart_DatumBits[] = {
    8,// 8bits
    5,// 5bits
    6,// 6bits
    7,// 7bits
};

static const uint8_t At_Uart_Parity[] = {
    0,// 0 for NO
    1,// 1 for ODD
    2,// 2 for EVEN
    3,// need to skip
};

static const uint8_t At_Uart_StopBits[] = {
    1, // 1 bit length
    2, // 2 bits length
};
static const uint8_t At_Uart_UseInt[] = {
    0,//0, //POLL mode
    1,//1, //INT mode
};

static const uint8_t At_Uart_UseDma[] = {
    0,//0, //MCU mode
    1,//1, //DMA mode
};

static const uint8_t At_Uart_UseFlowCtrl[] = {
    0,//not use FlowCtrl
    1,//use FlowCtrl
};

static void DmaInterruptUart0Tx(void)//3
{
}
static void DmaInterruptUart1Tx(void)//3
{
}
static void DmaInterruptUart2Tx(void)//3
{
}

static void DmaInterruptUart0Rx(void)//3
{
    int32_t Echo, _result;
	//DBG("Go into DMA mode.\n");
    Echo = UARTS_DMA_RecvDataStart(UART_PORT0, DmaTempBuf, DMA_TST_BUF_LEN);
    Buf_GetRoomLeft(&g_dma_ring_buf, _result);
    if(_result > Echo) {
        //DBG("push to ring buffer : %d\n", Echo);
        Buf_Push_Multi(&g_dma_ring_buf, DmaTempBuf, Echo, 0);
		dLenInDma += Echo;
    } else {
        DBG("uart0 ring buffer overflow\n");
    }
    UARTS_DMA_RxIntFlgClr(UART_PORT0, DMA_DONE_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT0, DMA_THRESHOLD_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT0, DMA_ERROR_INT);
}

static void DmaInterruptUart1Rx(void)//3
{
    int32_t Echo, _result;
    Echo = UARTS_DMA_RecvDataStart(UART_PORT1, DmaTempBuf, DMA_TST_BUF_LEN);
    Buf_GetRoomLeft(&g_dma_ring_buf, _result);
    if(_result > Echo) {
        //DBG("push to ring buffer : %d\n", Echo);
        Buf_Push_Multi(&g_dma_ring_buf, DmaTempBuf, Echo, 0);
    } else {
        DBG("uart1 ring buffer overflow\n");
    }
    UARTS_DMA_RxIntFlgClr(UART_PORT1, DMA_DONE_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT1, DMA_THRESHOLD_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT1, DMA_ERROR_INT);
}

static void DmaInterruptUart2Rx(void)//3
{
    int32_t Echo, _result;
    //DBG("GO INTO DmaInterruptUart2Rx\n");
    Echo = UARTS_DMA_RecvDataStart(UART_PORT2, DmaTempBuf, DMA_TST_BUF_LEN);
    Buf_GetRoomLeft(&g_dma_ring_buf, _result);
    if(_result > Echo) {
        //DBG("push to ring buffer : %d\n", Echo);
        Buf_Push_Multi(&g_dma_ring_buf, DmaTempBuf, Echo, 0);
    } else {
        DBG("uart2 ring buffer overflow\n");
    }

    UARTS_DMA_RxIntFlgClr(UART_PORT2, DMA_DONE_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT2, DMA_THRESHOLD_INT);
    UARTS_DMA_RxIntFlgClr(UART_PORT2, DMA_ERROR_INT);
}

INT_FUNC DmaIntTxFunArry[3] = {
    DmaInterruptUart0Tx,
    DmaInterruptUart1Tx,
    DmaInterruptUart2Tx
};
INT_FUNC DmaIntRxFunArry[3] = {
    DmaInterruptUart0Rx,
    DmaInterruptUart1Rx,
    DmaInterruptUart2Rx
};





typedef struct _ST_UART_TST_COUNTER {
    unsigned long BAUDRATE_T        :   4;
    unsigned long DATUMBITS_T       :   2;
    unsigned long PARITY_T          :   2;
    unsigned long STOPBITS_T        :   1;
    unsigned long USEINT_T          :   1;
    unsigned long USEDMA_T          :   1;
    unsigned long USEFLOWCTRL_T     :   1;
} ST_UART_TST_COUNTER;

static uint32_t uart_param_main = 0x00;//point to next
static uint32_t uart_param_curr = 0x00;//point to current
static ST_UART_TST_COUNTER *g_tst_counter;
#define OBTAIN_BAUDRATE At_Uart_BaudRate[g_tst_counter->BAUDRATE_T]
#define OBTAIN_DATUMBITS At_Uart_DatumBits[g_tst_counter->DATUMBITS_T]
#define OBTAIN_PARITY At_Uart_Parity[g_tst_counter->PARITY_T]
#define OBTAIN_STOPBITS At_Uart_StopBits[g_tst_counter->STOPBITS_T]
#define OBTAIN_USEINT At_Uart_UseInt[g_tst_counter->USEINT_T]
#define OBTAIN_USEDMA At_Uart_UseDma[g_tst_counter->USEDMA_T]
#define OBTAIN_USEFC At_Uart_UseFlowCtrl[g_tst_counter->USEFLOWCTRL_T]




void switch_uart_config()
{
    //static bool clk_sel = FALSE;

    uart_param_curr = uart_param_main;
    DBG("switch_uart_config: %d,%d,%d,%d,[%d,%d,%d]--->%d\r\n",
            OBTAIN_BAUDRATE,
            OBTAIN_DATUMBITS,
            OBTAIN_PARITY,
            OBTAIN_STOPBITS,
            OBTAIN_USEINT,
            OBTAIN_USEDMA,
            OBTAIN_USEFC,
            uart_param_curr
           );
    GPIO_UartRxIoConfig(CurrTestUartNum, GPIO_MULTI_USE_RX);
    GPIO_UartTxIoConfig(CurrTestUartNum, GPIO_MULTI_USE_TX);
    GIE_ENABLE();
    UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_CONF_RESET, 1);
    UARTS_Init((UART_PORT_T)CurrTestUartNum, OBTAIN_BAUDRATE, OBTAIN_DATUMBITS, OBTAIN_PARITY, OBTAIN_STOPBITS);
    NVIC_EnableIRQ(UART1_IRQn);
    if(OBTAIN_USEFC == 1) {
        DBG("HWFC Set!!!\n");
        GPIO_UartCtsIoConfig(CurrTestUartNum, GPIO_MULTI_USE_CTS); //CTS-->b5
        GPIO_UartRtsIoConfig(CurrTestUartNum, GPIO_MULTI_USE_RTS); //RTS-->b6
        UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_RXRTS_FLOWCTL_SET, 2);
        UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_TXCTS_FLOWCTL_SET, 2);
    } else {
        GPIO_UartCtsIoConfig(CurrTestUartNum, 0xFF); //CTS-->b5
        GPIO_UartRtsIoConfig(CurrTestUartNum, 0xFF); //RTS-->b6
        UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_RXRTS_FLOWCTL_SET, 0);
        UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_TXCTS_FLOWCTL_SET, 0);
    }

    if(OBTAIN_USEDMA == 1) {
        UARTS_DMA_TxInit(CurrTestUartNum, (void*)DmaTxBuf, sizeof(DmaTxBuf), sizeof(DmaTxBuf)/8, DmaIntTxFunArry[CurrTestUartNum]);
        UARTS_DMA_RxInit(CurrTestUartNum, (void*)DmaRxBuf, sizeof(DmaRxBuf), sizeof(DmaRxBuf)/8, DmaIntRxFunArry[CurrTestUartNum]);

        if(OBTAIN_USEINT == 1) {
            UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_TXINT_SET, 1);//only need to enable TX INT
        } else {
            UARTS_DMA_RxSetInterruptFun(CurrTestUartNum, DMA_THRESHOLD_INT, NULL);
            UARTS_DMA_TxSetInterruptFun(CurrTestUartNum, DMA_THRESHOLD_INT, NULL);
            UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_TXINT_SET, 0);//only need to enable TX INT
        }

        UARTS_DMA_RxChannelEn(CurrTestUartNum, 1);
        UARTS_DMA_TxChannelEn(CurrTestUartNum, 1);
    } else {
        UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_DMA_TX_EN, 0);
        UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_DMA_RX_EN, 0);
        UARTS_DMA_RxChannelEn(CurrTestUartNum, 0);
        UARTS_DMA_TxChannelEn(CurrTestUartNum, 0);
        if(OBTAIN_USEINT == 1) {
            UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_RXINT_SET, 1);
        } else {
            UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_RXINT_SET, 0);
        }
      // SREG_UART1_RX_CTRL.RX_ERR_INT_EN = 0;
        UARTS_IOCTL(CurrTestUartNum,UART_IOCTL_RX_ERR_INT_EN, 0);
    }
	//UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_OVERTIME_SET, 1);//allow overtime int
	//UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_OVERTIME_NUM, OBTAIN_BAUDRATE/2);//set overtime number
}
uint32_t delay_by_baudrate(void)
{

    uint32_t delayt = 0;
    switch(OBTAIN_BAUDRATE) {
        case 3000000:
        case 2764800:
        case 1843200:
            delayt = 50000;
            break;
        case 1382400:
        case 921600:
        case 460800:
        case 230400:
            delayt =  150000;
            break;
        case 115200:
        case 76800:
        case 57600:
        case 38400:
            delayt = 200000;
            break;
        case 19200:
        case 9600:
            delayt = 400000;
            break;
        case 4800:
            delayt = 800000;
            break;
        case 2400:
            delayt = 2000000;
            break;
        case 1200:
            delayt = 4000000;
            break;
        default:
            delayt = 3500000;
            break;
    }

    //do not delay too much if MCU mode
    if(!OBTAIN_USEDMA)
        delayt/=4;

    return delayt;

}


static uint32_t WaitInputStartNum(void)
{
    uint8_t Input[8] = {0};
    uint8_t len, t_len=0;
    do {
        len = UARTS_Recv((UART_PORT_T)0, Input+t_len, 100);
        t_len += len;
        if(t_len>=4)
            break;
    } while(1);
    DBG("SEE : %s\n", Input);

    return strtoul((const char*)Input, 0, 10);
}
void clean_ring_buffer(BUFFER_INFO *buf)
{
    Buf_Flush(buf);
    memset(buf->CharBuffer, 0x00, buf->Length);
}


/******************************************CRC Begin******************************************************/
static unsigned long gseed = 1;

static uint32_t gen_random_seed(void)
{
    gseed = gseed * 1103515245 + 12345;
    return gseed;
}

void misc_crc16(uint8_t aData[], uint8_t aDataout[], uint32_t aSize)
{
    uint8_t CRC16Lo, CRC16Hi;   // 8-bit unsigned entity.
    uint8_t CL, CH;   // 8-bit unsigned entity.
    uint8_t SaveHi, SaveLo;   // 8-bit unsigned entity.
    int32_t I, Flag;

    CRC16Lo=0x00;
    CRC16Hi=0x00;

    CL = 0x01;
    CH = 0xa0;

    for (I = 0 ; I<=aSize-1; I++ ) {
        CRC16Lo = CRC16Lo ^ aData[I] ;      //

        for (Flag = 0 ; Flag<= 7; Flag++ ) {
            SaveHi = CRC16Hi;
            SaveLo = CRC16Lo;

            CRC16Hi = CRC16Hi >> 1;           // '高位右移一位
            CRC16Lo = CRC16Lo >> 1;           // '低位右移一位

            if ((SaveHi & 0x01) == 0x01) {    //'如果高位字节最后一位为1
                CRC16Lo = CRC16Lo + 0x80;          //'则低位字节右移后前面补1
            } else {
                //'否则自动补0
            }
            if ((SaveLo & 0x01)  == 0x01) {  //'如果LSB为1，则与多项式码进行异或
                CRC16Hi = CRC16Hi ^ CH;
                CRC16Lo = CRC16Lo ^ CL;
            }
        }
    }

    aDataout[1]= CRC16Hi;   // 'CRC高位
    aDataout[0]= CRC16Lo;   // ' 'CRC低位
}
/*
* 1. sizeof(buf) sould larger than len+2
* 2. buf[len] and buf[len+1] is crc*/
void gen_crc16_string(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint32_t seed;
    static uint8_t hd_cont = 0x00;

    for(i=0; i<len; i+=4) {
        seed = gen_random_seed();
        buf[i] = seed & 0xFF;
        buf[i+1] = (seed>>8) & 0xFF;
        buf[i+2] = (seed>>16) & 0xFF;
        buf[i+3] = (seed>>24) & 0xFF;
    }
    DBG("\r\ngen_crc16_string1...");
    buf[0] = hd_cont++;
    misc_crc16(buf, &buf[len], len);
    DBG("\r\ngen_crc16_string2...");
}

static uint8_t hd_cont = 0;
void gen_crc16_string_mask(uint8_t *buf, uint16_t len, uint8_t mask)
{
    uint16_t i;
    uint8_t crc_ck[2] = {0};
    uint32_t seed = 0;

    for(i=0; i<len; i+=4) {
        seed = gen_random_seed();
        buf[i] = seed & mask;
        buf[i+1] = (seed>>8) & mask;
        buf[i+2] = (seed>>16) & mask;
        buf[i+3] = (seed>>24) & mask;
    }

    buf[0] = hd_cont++;
    misc_crc16(buf, crc_ck, len);
    buf[len] = (crc_ck[0]>>4)&0x0F;
    buf[len+1] = (crc_ck[0])&0x0F;
    buf[len+2] =(crc_ck[1]>>4)&0x0F;
    buf[len+3] = (crc_ck[1])&0x0F;
}

uint8_t mask_by_DatumBits(void)
{
    switch(OBTAIN_DATUMBITS) {
        case 5:
            return 0x1F;
        case 6:
            return 0x3F;
        case 7:
            return 0x7F;
        case 8:
            return 0xFF;
        default:
            return 0x1F;
    }
}



uint8_t dma_tst_buff[DMA_TST_BUF_LEN+1] = {0};
/*************************************************
 * 修改定义：#define TEST_UART_NUM			测试的串口编号
 * 			#define GPIO_MULTI_USE_TX 		选择串口TX引脚
 * 			#define GPIO_MULTI_USE_RX 		选择串口RX引脚
 * 			#define GPIO_MULTI_USE_RTS 		选择串口RTS引脚
 * 			#define GPIO_MULTI_USE_CTS 		选择串口CTS引脚
 * 注意：程序初始化UART0后会提示输入0000-4095范围内的字符，请输入四个字符
 ***************************************************/
void main1(void)
{
	uint32_t i=0;
	uint32_t good_num=0;
	uint32_t bad_num=0;
	//Disable watchDog
	WDG_Disable();

	//CacheTcmDeInit();
	CacheTcmInitFast(PHYMEM_16KBPM0 + PHYMEM_16KBPM1/*cache*/, PHYMEM_64KBPM2/*tcm r0*/, 0/*tcm r0 offset*/, 64/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);
	Clock_Config(1, 32768);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	GPIO_UartTxIoConfig(0, 1);//GPIO_C2
	GPIO_UartRxIoConfig(0, 1);//GPIO_C3

	UARTS_Init(0,  115200, 8,  0,  1);
	DBG("*********************************************\n");
	DBG("---------P20 uart example test begin-----------\n");
    DBG("*********************************************\n");
	DBG("Input begin number: (0000~%d)\n", END_CASE_NUM);
	DMA_ChannelAllocTableSet(DmaChannelMap);
	CurrTestUartNum = TEST_UART_NUM;
	g_tst_counter = (ST_UART_TST_COUNTER *)&uart_param_curr;
	while(1){
	    uart_param_main = WaitInputStartNum();
		//uart_param_main =0X0400;
	    switch_uart_config();
	    Buf_init(&g_dma_ring_buf, BuartRGBuf, sizeof(BuartRGBuf));
	    clean_ring_buffer(&g_dma_ring_buf);
	    for(i=0; i<6000000; i++);
	    while(1)
	    {
	    	int echo, _result;
	     	static uint32_t counterr = 0, round_counter = 0;//, need_switch = 0;
	        if(++counterr > delay_by_baudrate()/4) { //100000 ok, 10000 ok, 5000 ok, 1000 is testing
	                gen_crc16_string_mask(dma_tst_buff, DMA_TST_BUF_LEN-4, mask_by_DatumBits());//
	                if(OBTAIN_USEDMA)
	                    UARTS_DMA_SendDataStart(CurrTestUartNum, dma_tst_buff, DMA_TST_BUF_LEN);
	                else
	                    UARTS_Send((UART_PORT_T)CurrTestUartNum, dma_tst_buff, DMA_TST_BUF_LEN);
	                DBG("\r\nSend_num:%d", round_counter);
	                round_counter ++;
	               /* if(dma_tst_buff[0] == MAX_ROUND_NUM-1) {
	                    need_switch = 1;
	                }
	            }*/
	            counterr = 0;
	        }
	        if(!OBTAIN_USEINT) {
	             echo = 0;
	             if(OBTAIN_USEDMA) { //dma poll
	                 if(0 < UARTS_DMA_RxDataLen(CurrTestUartNum)) {
	                     echo = UARTS_DMA_RecvDataStart(CurrTestUartNum, DmaTempBuf, DMA_TST_BUF_LEN);
	                 }
	             } else {//此处即使用查询法接收收据，数据量大频率高肯定会丢失数据
	                 if(UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_RXSTAT_GET, 1) & 0x01) { //mcu poll
	                     echo = UARTS_Recv((UART_PORT_T)CurrTestUartNum, DmaTempBuf, DMA_TST_BUF_LEN);
	                     UARTS_IOCTL(CurrTestUartNum, UART_IOCTL_RXINT_CLR, 1);
	                 }
	             }
	             Buf_GetRoomLeft(&g_dma_ring_buf, _result);
	             if(_result > echo) {
	                 Buf_Push_Multi(&g_dma_ring_buf, DmaTempBuf, echo, 0);
	             } else {
	                 DBG("uart ring buffer overflow\n");
	             }
	         }
	        Buf_GetBytesAvail(&g_dma_ring_buf, echo);
	        if(echo >= DMA_TST_BUF_LEN) { //castle added
	            static uint8_t crc_v[2];
	            Buf_Pop_Multi(&g_dma_ring_buf, crc_check_buf, DMA_TST_BUF_LEN, 0);
	            crc_check_buf[DMA_TST_BUF_LEN-4] = (crc_check_buf[DMA_TST_BUF_LEN-4]<<4)|(crc_check_buf[DMA_TST_BUF_LEN-3]&0x0F);
	            crc_check_buf[DMA_TST_BUF_LEN-3] = (crc_check_buf[DMA_TST_BUF_LEN-2]<<4)|(crc_check_buf[DMA_TST_BUF_LEN-1]&0x0F);
	            misc_crc16(crc_check_buf, crc_v, DMA_TST_BUF_LEN-2);
	            if(crc_v[0]==0x00 && crc_v[1]==0x00) {
	            	good_num++;
	                DBG("\r\nRECV ONE Good CRC package:%d",good_num);
	            } else {
	            	bad_num++;
	                DBG("\r\nRECV one BAD package :%d",bad_num);
	            }
	        }

	    }
	}

}




/****************************************************************
 * 修改定义：#define USE_UART_INT			是否使用中断，可注释掉
 * 			#define TEST_UART_NUM			测试的串口编号
 * 			#define GPIO_MULTI_USE_TX 		选择串口TX引脚
 * 			#define GPIO_MULTI_USE_RX 		选择串口RX引脚
 ******************************************************************/
//#define USE_UART_INT
#define BUFF_LEN 128
uint8_t TEST_BUFF[BUFF_LEN]={0};
uint8_t RX_NUM=0;
void UART_RxCallback(uint8_t data)
{
	TEST_BUFF[RX_NUM]=data;
	RX_NUM++;
	if(RX_NUM>=BUFF_LEN)
	{
		RX_NUM=0;
		printf("RX BUFF IS OVERRUN!!");

	}
}
int main(void)
{

	(*(volatile unsigned long *) 0x40020004) = 1;
	(*(volatile unsigned long *) 0x4002000C) = 1;

	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_16KBPM1/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);

	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);
	Clock_Config(1, 12000000);
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);

	SpiFlashInit(80000000, MODE_4BIT, 0, 1);

	GPIO_UartTxIoConfig(0, 1);//GPIO_C2
	GPIO_UartRxIoConfig(0, 1);//GPIO_C3
	UARTS_Init(0,  115200, 8,  0,  1);

	DBG("*********************************************\n");
	DBG("---------P20 uart example test begin-----------\n");
    DBG("*********************************************\n");
	GPIO_UartTxIoConfig(CurrTestUartNum, GPIO_MULTI_USE_TX);
	GPIO_UartRxIoConfig(CurrTestUartNum, GPIO_MULTI_USE_TX);

	UARTS_Init(CurrTestUartNum,  115200, 8,  0,  1);
#ifdef USE_UART_INT
    UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_RXFIFO_CLR, 1);
    UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_TXFIFO_CLR, 1);
    UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_RXINT_SET, 1);
    UARTS_IOCTL((UART_PORT_T)CurrTestUartNum, UART_IOCTL_RXINT_SET, 1);
    if(CurrTestUartNum==0)
    NVIC_EnableIRQ(UART0_IRQn);
    else if(CurrTestUartNum==1)
    NVIC_EnableIRQ(UART1_IRQn);
    else if(CurrTestUartNum==2)
    NVIC_EnableIRQ(UART2_IRQn);
    GIE_ENABLE();
#endif
	while(1)
	{
#ifdef USE_UART_INT
		if(RX_NUM>0)
		{
		UARTS_Send((UART_PORT_T)CurrTestUartNum,TEST_BUFF,RX_NUM);
		RX_NUM=0;
		}
#else
		UARTS_Send((UART_PORT_T)CurrTestUartNum,TEST_BUFF,UARTS_Recv((UART_PORT_T)CurrTestUartNum, TEST_BUFF,BUFF_LEN));
#endif
	}

	return 0;
}

