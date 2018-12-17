#include "type.h"
#include "gpio.h"
#include "app_config.h"
#include "sys_app.h"
#include "uart.h"
#include "slave_uart_com.h"
#include "slave_mcu_command.h"
#include "sound_remind.h"
#include "dev_detect_driver.h"

#ifdef FUNC_SLAVE_UART_EN

#define MAX_CMD_DATA_LEN	32			//命令数据域最大长度
#define RX_QUEUE_LENGTH 	1024		//串口中断接收缓冲区长度

static uint8_t RxQueue[RX_QUEUE_LENGTH];
static uint16_t RxQueueHead;
static uint16_t RxQueueTail;	 
static uint8_t gSlaveCmd[MAX_CMD_DATA_LEN];	//长度域、命令码域

static int16_t SlaveRxIndex;
uint16_t MasterModeState = MAIN_SY_OFF;
uint16_t MasterPowerState = MAIN_SY_OFF;

void Slave_SendCmdToBt(uint8_t WiFiCmd);

void Slave_UartInit(void)
{
	RxQueueHead = 0;
	RxQueueTail = 0;
	SlaveRxIndex = 0;
	memset(gSlaveCmd, 0, sizeof(gSlaveCmd));
	memset(RxQueue, 0, sizeof(RxQueue));
	
#ifdef SLAVE_SELECT_BUART_COM
	GpioBuartRxIoConfig(SLAVE_UART_RX_PORT);
	GpioBuartTxIoConfig(SLAVE_UART_TX_PORT);
	
	//enable RTS flow control,GPIO B31
	//GpioBuartRtsIoConfig(1);
	//BuartIOctl(BUART_IOCTL_RXRTS_FLOWCTL_SET, 2);
	//use the PMEM as the buart external FIFO,you can uncomment the following line to use internal 4 byte fifo
	//BuartExFifoInit(ADC_PMEM_START_ADDR + ADC_PMEM_SIZE + 1/*offset from PMEM*/, 1024/*RX fifo 8KB*/, 1024/*TX fifo 2KB*/, 1/*interrupt trigger depth 1 byte*/);
	BuartExFifoInit(BUART_RX_TX_FIFO_ADDR - PMEM_ADDR, 1024, 1024, 1);
	BuartInit(9600, 8, 0, 1);
	//enblae buart interrupt
	BuartIOctl(UART_IOCTL_RXINT_SET, 1);
	
	BuartSend((uint8_t *)"START...", 8);
#else
	GpioFuartRxIoConfig(SLAVE_UART_RX_PORT);
	GpioFuartTxIoConfig(SLAVE_UART_TX_PORT);
	OSDBGUartInit(9600, 8, 0, 1);
	//Enable buart Rx interrupt
	FuartIOctl(UART_IOCTL_RXINT_SET, 1);
	
	FuartSend((uint8_t *)"START...", 8);
#endif	
}

static uint16_t GetRxQueueLen(void)
{
	return ((RxQueueTail + RX_QUEUE_LENGTH - RxQueueHead) % RX_QUEUE_LENGTH);
}

uint8_t GetRxQueueData(void)
{
	uint8_t val;

	val = RxQueue[RxQueueHead];
	RxQueueHead = ((RxQueueHead + 1) % RX_QUEUE_LENGTH);
	return val;
}

//MCU 中断接收WiFi串口数据 
static void Mcu_RcvUartData(void)
{
	  uint8_t Temp;

#ifdef SLAVE_SELECT_BUART_COM
		if(BuartRecv(&Temp, 1, 0) == 1)
#else	
		if(FuartRecvByte(&Temp) != -1)
#endif	
		{
			RxQueue[RxQueueTail++] = Temp;			//save data into queue
			if(RxQueueTail > (RX_QUEUE_LENGTH - 1))
			{
				RxQueueTail = 0;
			}
		}
}

#ifdef SLAVE_SELECT_BUART_COM
__attribute__((section(".driver.isr"))) void BuartInterrupt(void)
{
	long Status;
	/*
	* take datum reception proirity over transmittion
	*/
	if((Status = BuartIOctl(UART_IOCTL_RXSTAT_GET,0)) & 0x1)
	{
		/*
		* clear interrupt status firstly
		*/
		BuartIOctl(UART_IOCTL_RXINT_CLR,0);
		//or you can receive data in interrupt context
#if 0
		if(BuartRxDatLen < sizeof(BuartRxBuf))
		BuartRxBuf[BuartRxDatLen ++] = BuartRecv(BuartRxBuf,sizeof(BuartRxBuf) - BuartRxDatLen,0);
		else
		DBG("Buart Rx Buffer overflow\n");
#else	
		Mcu_RcvUartData();
#endif
	}

	if(Status & 0x1E)
	{
		/*
		* clear FIFO before clear other flags rudely
		*/
		BuartIOctl(UART_IOCTL_RXFIFO_CLR,0);
		BuartIOctl(BUART_IOCTL_RXFIFO_CLR,0);		
		/*
		* clear other error flags
		*/        
		BuartIOctl(UART_IOCTL_RXINT_CLR,0);
	}
	if(BuartIOctl(UART_IOCTL_TXSTAT_GET,0) & 0x1)
	{
		/*
		* clear interrupt status
		*/
		BuartIOctl(UART_IOCTL_TXINT_CLR,0);		
#ifdef FREERTOS_VERSION
		/*
		* notify the task that BUART data is transferring done
		*/
		OSQueueMsgSend(MSGDEV_BUART_DATDON, NULL, 0, MSGPRIO_LEVEL_LO, 0);
#endif //FREERTOS_VERSION
	}
}

#else

__attribute__((section(".driver.isr"))) void FuartInterrupt(void)
{
	//If data received
	if(IsFuartRxIntSrc())
	{		
		//Get received byte
		Mcu_RcvUartData();		
		//clear interrupt
		FuartClrRxInt(); 		
	}
	//If data sent
	if(IsFuartTxIntSrc())
	{
		//clear interrupt
		FuartClrTxInt();
	}
}
#endif

//MCU接送WiFi命令数据
bool Mcu_RcvBtCmd(void)
{
	uint8_t Temp;
	
	while(GetRxQueueLen() > 0)
	{	
		Temp = GetRxQueueData();			
		
		if(Temp != '\n')	//所有串口命令都是以‘\n’结束的
		{
		  gSlaveCmd[SlaveRxIndex++] = Temp;		//Cmd + Data[] + CS	
		}
		else
		{
			gSlaveCmd[SlaveRxIndex++] = '\0';
			SlaveRxIndex = 0;	
			return TRUE;  //如需要CheckSum则注释此行
		}		
	}
	return FALSE;
}

//MCU发送给WiFi模组的命令数据
void Slave_SendCmdToBt(uint8_t McuCmd)
{	
	uint16_t Temp = 0;
	uint8_t Len;
	uint8_t CmdBuf[32];

	if(MasterPowerState == MAIN_SY_OFF)
	{
		return;
	}
	
	switch(McuCmd)
	{
		case COM_MBT:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+MBT\n", 8);
			Len = 8;
			break;			

		case COM_MAX:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+MAX\n", 8);
			Len = 8;
			break;	

		case COM_REC_OK:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+REC+OK\n", 11);
			Len = 11;
			break;	

		case COM_REC_ERR:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+REC+ERR\n", 12);
			Len = 12;
			break;	
			
		case COM_REC_END:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+REC+END\n", 12);
			Len = 12;
			break;	

		case COM_PA_OK:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+REC+PA+OK\n", 14);
			Len = 14;
			break;	
		
		case COM_PA_ERR:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+REC+PA+ERR\n", 15);
			Len = 15;
			break;
	
    case COM_SD_PN:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+SD+PLUGIN\n", 14);
			Len = 14;
			break;
		
		case COM_SD_PO:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+SD+PLUGOUT\n", 15);
			Len = 15;
			break;
		
		case COM_SD_ON:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+SD+ON\n", 10);
			Len = 10;
			break;
		
		case COM_SD_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"COM+SD+OFF\n", 11);
			Len = 11;
			break;
		
		default:
			break;
	}

#ifdef SLAVE_SELECT_BUART_COM
	BuartSend(CmdBuf, Len);
#else
	FuartSend(CmdBuf, Len);
#endif
	
	CmdBuf[Len] = '\0';
	APP_DBG("Send bt is:%s\n", CmdBuf);
}

//WiFi模组命令的接送解析
void Slave_CmdProcess(void)
{
	if(Mcu_RcvBtCmd() == FALSE)
	{
		return;
	}

	if(memcmp(gSlaveCmd, "REC+START", 9) == 0)
	{	
    if((gSys.CurModuleID == MODULE_ID_PLAYER_SD) || (gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY))
		{
			Slave_SendCmdToBt(COM_MBT);
		}
    else if(gSys.CurModuleID == MODULE_ID_RECORDER)
		{
			Slave_SendCmdToBt(COM_REC_END);
		}			
		MsgSend(MSG_REC);
	}
	else if(memcmp(gSlaveCmd, "REC+PA", 6) == 0)
	{	
    if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		{
			MsgSend(MSG_COMMON_CLOSE);
		}
    else
    {	
		  MsgSend(MSG_REC_PLAYBACK);
    }
	}	
	else if(memcmp(gSlaveCmd, "BT_AC", 5) == 0
		   || memcmp(gSlaveCmd, "BT_WP", 5) == 0
	     || memcmp(gSlaveCmd, "BT_WC", 5) == 0)
	{
    MasterModeState = MAIN_BT_DN; 
	}
	else if(memcmp(gSlaveCmd, "BT_CN", 5) == 0)
	{
		MasterModeState = MAIN_BT_CN;
	}
	else if(memcmp(gSlaveCmd, "BT_PA", 5) == 0)
	{
		MasterModeState = MAIN_BT_PA;
	}
	else if(memcmp(gSlaveCmd, "BT_PU", 5) == 0)
	{
		MasterModeState = MAIN_BT_PU;
	}
	else if(memcmp(gSlaveCmd, "AX_PA", 5) == 0)
	{
		MasterModeState = MAIN_AX_PA;
	}
	else if(memcmp(gSlaveCmd, "AX_PU", 5) == 0)
	{
		MasterModeState = MAIN_AX_PU;
	}
	else if(memcmp(gSlaveCmd, "SY_PO", 5) == 0)
	{			
		MasterPowerState = MAIN_SY_ON;
		if(IsCardLink())
		{
			Slave_SendCmdToBt(COM_SD_ON);
		}	
		else
		{
			Slave_SendCmdToBt(COM_SD_OFF);
		}
	}
	else if(memcmp(gSlaveCmd, "SY_PF", 5) == 0)
	{		
		MasterModeState = MAIN_SY_OFF;
	}	
	else if(memcmp(gSlaveCmd, "COM+PP", 6) == 0)
	{	
		MsgSend(MSG_PLAY_PAUSE);
	}
	else if(memcmp(gSlaveCmd, "COM+PV", 6) == 0)
	{    	
		MsgSend(MSG_PRE);
	}
	else if(memcmp(gSlaveCmd, "COM+PN", 6) == 0)
	{		
		MsgSend(MSG_NEXT);
	}
	else if(memcmp(gSlaveCmd, "COM+VD", 6) == 0)
	{
		MsgSend(MSG_VOL_DW);
	}
  else if(memcmp(gSlaveCmd, "COM+VP", 6) == 0)
	{
		MsgSend(MSG_VOL_UP);
	}	
	else if(memcmp(gSlaveCmd, "COM+MSD", 7) == 0)
	{
		if(gSys.CurModuleID != MODULE_ID_PLAYER_SD)
		{
		  gSys.NextModuleID = MODULE_ID_PLAYER_SD;
		  MsgSend(MSG_COMMON_CLOSE);
		}
	}
	else if(memcmp(gSlaveCmd, "COM+MBT", 7) == 0
	     || memcmp(gSlaveCmd, "COM+MAX", 7) == 0)
	{
		if(gSys.CurModuleID != MODULE_ID_LINEIN)
		{
		  gSys.NextModuleID = MODULE_ID_LINEIN;
		  MsgSend(MSG_COMMON_CLOSE);
		}
	}
	else if(memcmp(gSlaveCmd, "COM+REC+DEL", 11) == 0)
	{
		if(gSys.CurModuleID == MODULE_ID_REC_BACK_PLAY)
		{
			MsgSend(MSG_REC_FILE_DEL);
		}
	}

	APP_DBG("Rcv gSlaveCmd is:%s\n", gSlaveCmd);
}

#endif

