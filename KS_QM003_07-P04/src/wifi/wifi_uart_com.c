#include "app_config.h"
#include "task_decoder.h"
#include "bt_app_func.h"
#include "bt_control_api.h"
#include "dev_detect_driver.h"
#include "sd_card.h"
#include "eq.h"
#include "eq_params.h"
#include "clk.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "sound_remind.h"
#include "mixer.h"
#include "breakpoint.h"
#include "recorder_control.h"
#include "nvm.h"
#include "sys_app.h"
#include "browser.h"
#include "lrc.h"
#include "eq.h"
#include "eq_params.h"
#include "timer_api.h"
#include "string_convert.h"
#include "player_control.h"
#include "type.h"
#include "gpio.h"
#include "uart.h"
#include "wifi_uart_com.h"
#include "wifi_control.h"
#include "wifi_init_setting.h"
#include "wifi_mcu_command.h"
#include "wifi_wifi_command.h"
#ifdef FUNC_DISP_EN
#include "led_display.h"
#include "seg_panel.h"
#include "seg_led_disp.h"
#endif

#ifdef FUNC_WIFI_EN

#define MAX_CMD_DATA_LEN	32			//命令数据域最大长度
#define RX_QUEUE_LENGTH 	1024		//串口中断接收缓冲区长度
#define RX_DATA_BUFFER_LENGTH 	(MAX_CMD_DATA_LEN + 2048)	//数据接收处理缓冲区长度(WiFi串口升级需求最大长度)

static const uint8_t Project_Name[] = {MCU_CAP_PRJ_PROJECT_NAME};   //Project  name
static const uint8_t Ssid_Name[] = {MCU_SSID_NAME};   //Device SSID  name
static const uint8_t Ap_PassWord[] = {MCU_CAP_PWD};   //WiFi AP Pass Word

#ifdef FUNC_WIFI_ALI_PROJECT_EN
static const uint8_t ALi_Prouduct_Id[] = {MCU_ALI_PID_PRODUCTID};   //Product id
#endif
#ifdef FUNC_WIFI_SPOTIFY_NEED_EN
static const uint8_t Spotify_ModeName[] = {MCU_SPY_NAM_MODE_NAME};   //Spotify mode  name
static const uint8_t Spotify_BrandName[] = {MCU_SPY_BRN_BRAND_NAME};   //Spotify brand name
static const uint8_t Spotify_ModeDispName[] = {MCU_SPY_MOD_DISP_NAME};  //Spotify mode display name
static const uint8_t Spotify_ClientId[] = {MCU_SPY_CID_CLIENTID_ID};   //Spotify brand display name
static const uint8_t Spotify_BrandDispName[] = {MCU_SPY_BRD_DISP_NAME};   //Spotify brand display name
#endif

uint8_t RxQueue[RX_QUEUE_LENGTH];
uint16_t RxQueueHead;
uint16_t RxQueueTail;	 
uint8_t gWiFiCmd[MAX_CMD_DATA_LEN];	//长度域、命令码域
uint8_t gWiFiData[RX_DATA_BUFFER_LENGTH];	//WiFi数据接收

int8_t SlaveRxIndex;
bool WiFiDataRcvStartFlag; 
uint16_t WiFiDataRcvLen; //WiFi数据有效长度
static TIMER RcvDataTimer;
bool PollingRevReadingFlag; //查询接收串口数据标志
bool RxDataEnableFlag;
extern TIMER WiFiLedBlinkTimer;

void WiFi_SendCmdToMcu(uint16_t WiFiCmd, uint8_t* CmdData);


void WiFi_UartInit(void)
{
	RxQueueHead = 0;
	RxQueueTail = 0;
	SlaveRxIndex = -3;
	WiFiDataRcvStartFlag = FALSE;
	WiFiDataRcvLen = 0;
	PollingRevReadingFlag = FALSE;
	RxDataEnableFlag = TRUE;
	memset(gWiFiCmd, 0, sizeof(gWiFiCmd));
	memset(gWiFiData, 0, sizeof(gWiFiData));
	
#ifdef WIFI_SELECT_BUART_COM
	GpioBuartRxIoConfig(WIFI_UART_RX_PORT);
	GpioBuartTxIoConfig(WIFI_UART_TX_PORT);
	
	//enable RTS flow control,GPIO B31
	//GpioBuartRtsIoConfig(1);
	//BuartIOctl(BUART_IOCTL_RXRTS_FLOWCTL_SET, 2);
	//use the PMEM as the buart external FIFO,you can uncomment the following line to use internal 4 byte fifo
	//BuartExFifoInit(ADC_PMEM_START_ADDR + ADC_PMEM_SIZE + 1/*offset from PMEM*/, 1024/*RX fifo 8KB*/, 1024/*TX fifo 2KB*/, 1/*interrupt trigger depth 1 byte*/);
	BuartExFifoInit(BUART_RX_TX_FIFO_ADDR - PMEM_ADDR, 1024, 1024, 1);
	BuartInit(57600, 8, 0, 1);
	//enblae buart interrupt
	BuartIOctl(UART_IOCTL_RXINT_SET, 1);
	
	//BuartSend((uint8_t *)"START...", 8);
#else
	GpioFuartRxIoConfig(WIFI_UART_RX_PORT);
	GpioFuartTxIoConfig(WIFI_UART_TX_PORT);
	OSDBGUartInit(57600, 8, 0, 1);
	//Enable buart Rx interrupt
	FuartIOctl(UART_IOCTL_RXINT_SET, 1);
	
	FuartSend((uint8_t *)"START...", 8);
#endif	
}

void ClearRxQueue(void)
{	
	RxQueueHead = 0;
	RxQueueTail = 0;
	memset(RxQueue, 0, sizeof(RxQueue));
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

//MCU 定时查询接收WiFi串口数据 
void Mcu_TimerPollingRcvUartData(void)
{	
	uint8_t Temp;

	PollingRevReadingFlag = TRUE;
#ifdef WIFI_SELECT_BUART_COM
	if(BuartRecv(&Temp, 1, 0) == 1)
#else	
	if(FuartRecvByte(&Temp) != -1)
#endif	
	{
		if(RxDataEnableFlag == TRUE)
		{
			RxQueue[RxQueueTail++] = Temp;			//save data into queue
			if(RxQueueTail > (RX_QUEUE_LENGTH - 1))
			{
				RxQueueTail = 0;
			}
		}
	}
	PollingRevReadingFlag = FALSE;
}

//MCU 中断接收WiFi串口数据 
void Mcu_RcvUartData(void)
{
	uint8_t Temp;

	if(PollingRevReadingFlag == FALSE)
	{
#ifdef WIFI_SELECT_BUART_COM
		if(BuartRecv(&Temp, 1, 0) == 1)
#else	
		if(FuartRecvByte(&Temp) != -1)
#endif	
		{
			if(RxDataEnableFlag == TRUE)
			{
				RxQueue[RxQueueTail++] = Temp;			//save data into queue
				if(RxQueueTail > (RX_QUEUE_LENGTH - 1))
				{
					RxQueueTail = 0;
				}
			}
		}
	}
}

#ifdef WIFI_SELECT_BUART_COM
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

//MCU接送WiFi数据处理
void Mcu_RcvWiFiDataProcess(void)
{
	if(memcmp(gWiFiCmd, "+MAC+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MAC_XXX, NULL);
	}
	else if(memcmp(gWiFiCmd, "+POW+%03", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_POW_STA, NULL);
	}	
	else if(memcmp(gWiFiCmd, "+WAN+LST", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_WAN_LST, NULL);
	}
	else if(memcmp(gWiFiCmd, "+SET+RTC", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SET_RTC, NULL);	
	}	
	else if(memcmp(gWiFiCmd, "+SET+WEK", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SET_WEK, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+NXT+ALS", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_NXT_ALS, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+NXT+ALM", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_NXT_ALM, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+PAS+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_PAS_DATA, NULL);		
	}	
#ifdef FUNC_WIFI_UART_UPGRADE
	if(memcmp(gWiFiCmd, "+MCU+UPD", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_UPD, NULL); 	
	}
	else if(memcmp(gWiFiCmd, "+MCU+RUN", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_RUN, NULL); 	
	}
#endif
}

//MCU接送WiFi命令数据
bool Mcu_RcvWiFiCmd(void)
{
	uint8_t Temp;
	uint8_t CmdLen;

	if(SlaveRxIndex == -3)
	{
		TimeOutSet(&RcvDataTimer, 3000);
	}
	else
	{
		if(IsTimeOut(&RcvDataTimer))	//命令接收超时
		{
			SlaveRxIndex = -3;
		}	
	}
	
	while(GetRxQueueLen() > 0)
	{	
		if(IsTimeOut(&RcvDataTimer))
		{
			//DBG(("timeout\n"));
			SlaveRxIndex = -3;
			return FALSE;
		}	

		Temp = GetRxQueueData();	
		
		if(SlaveRxIndex == -3)
		{
			if(Temp == 'A')
			{
				SlaveRxIndex = -2;
			}
		}
		else if(SlaveRxIndex == -2)
		{
			if(Temp == 'X')
			{
				SlaveRxIndex = -1;
			}
			else
			{
				SlaveRxIndex = -3;
			}
		}
		else if(SlaveRxIndex == -1)
		{
			if(Temp == 'X')
			{
				SlaveRxIndex = 0;
			}
			else
			{
				SlaveRxIndex = -3;
			}
		}
		else 
		{
			if((SlaveRxIndex == 0) && (Temp != '+')) //不是WiFi命令(WiFi命令都是AXX+开头)
			{				
				SlaveRxIndex = -3;		
			}
			else
			{
				CmdLen = 8;	//WiFi标准命令长度统一为11
				gWiFiCmd[SlaveRxIndex++] = Temp;		//Cmd + Data[] + CS	

				//非标准命令的特殊处理，利用gWiFiData接收命令后面的数据
				if(memcmp(gWiFiCmd, "+UID+", 5) == 0)
				{
					CmdLen = 5;	
				}
				else if(memcmp(gWiFiCmd, "+MAC+", 5) == 0)
				{
					CmdLen = 5;	
				}				
				else if(memcmp(gWiFiCmd, "+PAS+", 5) == 0)
				{
					CmdLen = 5;	
				}
				else if(memcmp(gWiFiCmd, "+BT+EXDM", 8) == 0)
				{
					CmdLen = 8;	
				}	
				else if(memcmp(gWiFiCmd, "+BT+GPRL", 8) == 0)
				{
					CmdLen = 8;	
				}					
				else if(memcmp(gWiFiCmd, "+BT+PLAY", 8) == 0)
				{
					CmdLen = 8;	
				}					
				else if(memcmp(gWiFiCmd, "+BT+STOP", 8) == 0)
				{
					CmdLen = 8;	
				}
				else if(memcmp(gWiFiCmd, "+BT+PREV", 8) == 0)
				{
					CmdLen = 8;	
				}					
				else if(memcmp(gWiFiCmd, "+BT+NEXT", 8) == 0)
				{
					CmdLen = 8;	
				}				
				else if(memcmp(gWiFiCmd, "+BT+GETS", 8) == 0)
				{
					CmdLen = 8;	
				}					
				else if(memcmp(gWiFiCmd, "+BT+SACN", 8) == 0)
				{
					CmdLen = 8;	
				}
				else if(memcmp(gWiFiCmd, "+BT+NAME", 8) == 0)
				{
					CmdLen = 8;	
				}	
				else if(memcmp(gWiFiCmd, "+BT+", 4) == 0)
				{
					CmdLen = 50;	
				}			
				
				if((SlaveRxIndex >= CmdLen) || (gWiFiCmd[SlaveRxIndex-1]== '&'))	//CS接收完
				{
					gWiFiCmd[SlaveRxIndex] = '\0';
					SlaveRxIndex = -3;
					WiFiDataRcvStartFlag = FALSE;
					WiFiDataRcvLen = 0;		
#ifdef FUNC_SILENCE_TIMEOUT_POWEROFF_EN
					gSys.SilenceCounter = 0;
#endif
					return TRUE;  //如需要CheckSum则注释此行
				}
			}
		}

		//接收数据
		if(WiFiDataRcvStartFlag == TRUE)
		{
			if(WiFiDataRcvLen  < RX_DATA_BUFFER_LENGTH)
			{
				gWiFiData[WiFiDataRcvLen++] = Temp;	
				Mcu_RcvWiFiDataProcess();
			}			
		}
	}
	return FALSE;
}

//MCU发送给WiFi模组的命令数据
void Mcu_SendCmdToWiFi(uint16_t McuCmd, uint8_t* DataBuf)
{	
	uint16_t Temp;
	uint8_t Len;
	uint8_t CmdBuf[64];

	if(!gWiFi.InitState && (WiFiPowerOnInitStateGet() == FALSE))
	{
		return;
	}

	Temp = 0;
	switch(McuCmd)
	{
		case MCU_POW_SAV:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+POW+SAV", 11);
			Len = 11;
			break;			

		case MCU_POW_RES:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+POW+RES", 11);
			Len = 11;
			break;	

		case MCU_POW_STA:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+POW+STA", 11);
			Len = 11;
			break;	

		case MCU_BAT_ON:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+BAT++ON", 11);
			Len = 11;
			break;	
			
		case MCU_BAT_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+BAT+OFF", 11);
			Len = 11;
			break;	

		case MCU_BAT_VAL:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+BAT+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_BAT_LOW:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+BAT+LOW", 11);
			Len = 11;
			break;	
			
		case MCU_BOT_DON:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+BOT+DON", 11);
			Len = 11;
			break;	

		case MCU_GET_UID:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+GET+UID", 11);
			Len = 11;
			break;	
			
		case MCU_GET_MAC:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+GET+MAC", 11);
			Len = 11;
			break;	
			
		case MCU_DEV_RST:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+DEV+RST", 11);
			Len = 11;
			break;	

		case MCU_SND_RTC:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+SND+RTC", 11);
			Len = 11;
			break;					

		case MCU_PLY_PAU:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+PUS", 11);
			Len = 11;
			break;		

		case MCU_PLY_PLA:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY-PLA", 11);
			Len = 11;
			break;			

		case MCU_PLY_PUS:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY-PUS", 11);
			Len = 11;
			break;	

		case MCU_PLY_STP:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY-STP", 11);
			Len = 11;
			break;	

		case MCU_PLY_PUQ:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+PUQ", 11);
			Len = 11;
			break;	
			
		case MCU_PLY_NXT:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+NXT", 11);
			Len = 11;
			break;	

		case MCU_PLY_PRV:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+PRV", 11);
			Len = 11;
			break;			

		case MCU_VOL_ADD:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+VOL+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_CUR_VOL:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+VOL-", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;

		case MCU_VOL_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+VOL+GET", 11);
			Len = 11;
			break;				
			
		case MCU_MUT_SET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+MUT+", 8);
			CmdBuf[8] = 0x30;
			CmdBuf[9] = 0x30;
			CmdBuf[10] = DataBuf[0] + 0x30;
			Len = 11;
			break;		

		case MCU_MUT_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+MUT+GET", 11);
			Len = 11;
			break;	
			
		case MCU_CUR_MUT:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+MUT-", 8);
			CmdBuf[8] = 0x30;
			CmdBuf[9] = 0x30;
			CmdBuf[10] = DataBuf[0] + 0x30;
			Len = 11;
			break;	

		case MCU_WIF_WPS:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+WIF+WPS", 11);
			Len = 11;
			break;		

		case MCU_WIF_WSS:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+WIF+WSS", 11);
			Len = 11;
			break;		
			
		case MCU_WIF_EPS:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+WIF+EPS", 11);
			Len = 11;
			break;	

		case MCU_STA_ON:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+STA++ON", 11);
			Len = 11;
			break;	

		case MCU_STA_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+STA+OFF", 11);
			Len = 11;
			break;	

		case MCU_STA_DEL:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+STA+DEL", 11);
			Len = 11;
			break;	

		case MCU_RA0_ON:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+RA0++ON", 11);
			Len = 11;
			break;	

		case MCU_RA0_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+RA0+OFF", 11);
			Len = 11;
			break;	

		case MCU_STA_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+STA+GET", 11);
			Len = 11;
			break;	
			
		case MCU_RA0_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+RA0+GET", 11);
			Len = 11;
			break;	
			
		case MCU_ETH_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+ETH+GET", 11);
			Len = 11;
			break;
			
		case MCU_WWW_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+WWW+GET", 11);
			Len = 11;
			break;	
			
		case MCU_KEY_XNN:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+KEY+", 8);
			if(DataBuf[0] < 100)
			{	
				CmdBuf[8] = 1 + 0x30;
				CmdBuf[9] = (DataBuf[0] / 10) + 0x30;
				CmdBuf[10] = (DataBuf[0] % 10) + 0x30;			
			} 
			else
			{
				CmdBuf[8] = 2 + 0x30;
				CmdBuf[9] = ((DataBuf[0] % 100) / 10) + 0x30;
				CmdBuf[10] = (DataBuf[0] % 10) + 0x30;				   
			}				
			Len = 11;
			break;

		case MCU_KEY_NXT:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+KEY+NXT", 11);
			Len = 11;
			break;	

		case MCU_KEY_PRE:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+KEY+PRE", 11);
			Len = 11;
			break;
			
		case MCU_PRE_SET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PRE+SET", 11);
			Len = 11;
			break;	

		case MCU_PRE_NNN:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PRE+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;			
			
		case MCU_PLY_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+GET", 11);
			Len = 11;
			break;	
			
		case MCU_PLP_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLP+GET", 11);
			Len = 11;
			break;	

		case MCU_PLP_XXX:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLP+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_PAS_EQ_TREBLE:			
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PAS+EQ:treble:", 18);
			if(DataBuf[0] / 10)
			{
				CmdBuf[18] = (DataBuf[0] / 10) + 0x30;
				CmdBuf[19] = (DataBuf[0] % 10) + 0x30;				
				Temp = 20;
			}
			else
			{
				CmdBuf[18] = DataBuf[0] + 0x30;
				Temp = 19;
			}	
			CmdBuf[Temp++] = '&';
			Len = Temp;
			break;

		case MCU_PAS_EQ_BASS:			
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PAS+EQ:bass:", 16);
			if(DataBuf[0] / 10)
			{
				CmdBuf[16] = (DataBuf[0] / 10) + 0x30;
				CmdBuf[17] = (DataBuf[0] % 10) + 0x30;
				Temp = 18;
			}
			else
			{
				CmdBuf[16] = DataBuf[0] + 0x30;
				Temp = 17;
			}		
			CmdBuf[Temp++] = '&';
			Len = Temp;
			break;

		case MCU_MSX_NNN:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+MS", 6);
			CmdBuf[6] = DataBuf[0] + 0x30;
			CmdBuf[7] = '+';
			Temp = DataBuf[1] + (DataBuf[2] << 8);
			CmdBuf[8] = Temp / 100 + 0x30;
			CmdBuf[9] = (Temp % 100) / 10 + 0x30;
			CmdBuf[10] = Temp % 10 + 0x30;
			Len = 11;
			break;		

		case MCU_ALM_NXT:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+ALM+NXT", 11);
			Len = 11;
			break;		
			
		case MCU_FACTORY:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+FACTORY", 11);
			Len = 11;
			break;		

		case MCU_FCRYPOW:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+FCRYPOW", 11);
			Len = 11;
			break;		

		case MCU_M2S_NNN:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+M2S+", 8);
			Temp = DataBuf[0] + (DataBuf[1] << 8);
			CmdBuf[8] = Temp / 100 + 0x30;
			CmdBuf[9] = (Temp % 100) / 10 + 0x30;
			CmdBuf[10] = Temp % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_S2M_NNN:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+S2M+", 8);
			Temp = DataBuf[0] + (DataBuf[1] << 8);
			CmdBuf[8] = Temp / 100 + 0x30;
			CmdBuf[9] = (Temp % 100) / 10 + 0x30;
			CmdBuf[10] = Temp % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_SID_VAL:	
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'I';
			CmdBuf[Temp++] = 'D';
			CmdBuf[Temp++] = '+';
			memcpy((void*)&CmdBuf[Temp], Ssid_Name, sizeof(Ssid_Name));
			Temp += sizeof(Ssid_Name);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;		

		case MCU_TLK_ENA:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+TLK+ENA", 11);
			Len = 11;
			break;
		
		case MCU_TLK_DIS:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+TLK+DIS", 11);
			Len = 11;
			break;
		
		case MCU_TALK_ON:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+TLK++ON", 11);
			Len = 11;
			break;

		case MCU_TLK_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+TLK+OFF", 11);
			Len = 11;
			break;

		case MCU_TLK_TIM:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+TLK+TIM", 11);
			Len = 11;
			break;

		case MCU_WEATHER:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+WEATHER", 11);
			Len = 11;
			break;			

		case MCU_POW_OFF:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+POW+OFF", 11);
			Len = 11;
			break;		

		case MCU_PLM_SUB:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLM-", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;			

		case MCU_PLM_ADD:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLM+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;
			break;	

		case MCU_PLM_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLM+GET", 11);			
			Len = 11;
			break;	

		case MCU_USB_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+USB+GET", 11);			
			Len = 11;
			break;	

		case MCU_MMC_GET:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+MMC+GET", 11);			
			Len = 11;
			break;	
			
		case MCU_SLV_CHK:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+SLV+CHK", 11);
			Len = 11;
			break;

		case MCU_SLV_GET:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+SLV+GET", 11);
			Len = 11;
			break;
		case MCU_JNGROUP:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+JNGROUP", 11);
			Len = 11;
			break;

		case MCU_UNGROUP:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+UNGROUP", 11);
			Len = 11;
			break;
		
		//start AXX+CAP+GET
		case MCU_CAP_PRJ:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'A';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'R';
			CmdBuf[Temp++] = 'J';
			memcpy((void*)&CmdBuf[Temp], Project_Name, sizeof(Project_Name));
			Temp += sizeof(Project_Name);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	

		case MCU_PTV_XXX:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PTV+", 8);
			CmdBuf[8] = ((MCU_PTV_NUM / 100) % 10) + 0x30;
			CmdBuf[9] = ((MCU_PTV_NUM % 100) / 10) + 0x30;
			CmdBuf[10] = (MCU_PTV_NUM % 10) + 0x30;
			CmdBuf[11] = '&';
			Len = 12;
			break;


#ifdef FUNC_WIFI_ALI_PROJECT_EN
		case MCU_ALI_PID:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'A';
			CmdBuf[Temp++] = 'L';
			CmdBuf[Temp++] = 'I';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'I';
			CmdBuf[Temp++] = 'D';
			memcpy((void*)&CmdBuf[Temp], ALi_Prouduct_Id, sizeof(ALi_Prouduct_Id));
			Temp += sizeof(ALi_Prouduct_Id);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	
		
		case MCU_ALV_NUM:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+ALV+", 8);
			CmdBuf[8] = ((MCU_ALV_NUMBER / 100) % 10) + 0x30;
			CmdBuf[9] = ((MCU_ALV_NUMBER % 100) / 10) + 0x30;
			CmdBuf[10] = (MCU_ALV_NUMBER % 10) + 0x30;
			CmdBuf[11] = '&';
		  Len = 12;
			break;
#endif

#ifdef FUNC_WIFI_SPOTIFY_NEED_EN
		case MCU_SPY_NAM:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'Y';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'N';
			CmdBuf[Temp++] = 'A';
			CmdBuf[Temp++] = 'M';
			memcpy((void*)&CmdBuf[Temp], Spotify_ModeName, sizeof(Spotify_ModeName));
			Temp += sizeof(Spotify_ModeName);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	

		case MCU_SPY_BRN:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'Y';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'B';
			CmdBuf[Temp++] = 'R';
			CmdBuf[Temp++] = 'N';
			memcpy((void*)&CmdBuf[Temp], Spotify_BrandName, sizeof(Spotify_BrandName));
			Temp += sizeof(Spotify_BrandName);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	

		case MCU_SPY_MOD:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'Y';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'O';
			CmdBuf[Temp++] = 'D';
			memcpy((void*)&CmdBuf[Temp], Spotify_ModeDispName, sizeof(Spotify_ModeDispName));
			Temp += sizeof(Spotify_ModeDispName);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	

		case MCU_SPY_BRD:						
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'Y';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'B';
			CmdBuf[Temp++] = 'R';
			CmdBuf[Temp++] = 'D';
			memcpy((void*)&CmdBuf[Temp], Spotify_BrandDispName, sizeof(Spotify_BrandDispName));
			Temp += sizeof(Spotify_BrandDispName);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;	

		case MCU_SPY_CID:
			CmdBuf[Temp++] = 'M';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'U';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'S';
			CmdBuf[Temp++] = 'P';
			CmdBuf[Temp++] = 'Y';
			CmdBuf[Temp++] = '+';
			CmdBuf[Temp++] = 'C';
			CmdBuf[Temp++] = 'I';
			CmdBuf[Temp++] = 'D';
			memcpy((void*)&CmdBuf[Temp], Spotify_ClientId, sizeof(Spotify_ClientId));
			Temp += sizeof(Spotify_ClientId);
			CmdBuf[Temp - 1] = '&';
			Len = Temp;
			break;

		case MCU_SPY_TYP:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+SPY+TYP", 11);
			CmdBuf[11] = MCU_SPY_TYP_DEVICE_TYPE + 0x30;
			CmdBuf[12] = '&';
			Len = 13;	
			break;
#endif

		case MCU_CAP_001:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+001", 11);
			CmdBuf[11] = (MCU_CAP_001_BIT31 << 3) | (MCU_CAP_001_BIT30 << 2) | (MCU_CAP_001_BIT29 << 1) | MCU_CAP_001_BIT28;			
			if(CmdBuf[11] <= 9)
			{
				CmdBuf[11] += 0x30;
			}
			else
			{
				CmdBuf[11] += 0x37;
			}
			CmdBuf[12] = (MCU_CAP_001_BIT27 << 3) | (MCU_CAP_001_BIT26 << 2) | (MCU_CAP_001_BIT25 << 1) | MCU_CAP_001_BIT24;			
			if(CmdBuf[12] <= 9)
			{
				CmdBuf[12] += 0x30;
			}
			else
			{
				CmdBuf[12] += 0x37;
			}
			CmdBuf[13] = (MCU_CAP_001_BIT23 << 3) | (MCU_CAP_001_BIT22 << 2) | (MCU_CAP_001_BIT21 << 1) | MCU_CAP_001_BIT20;			
			if(CmdBuf[13] <= 9)
			{
				CmdBuf[13] += 0x30;
			}
			else
			{
				CmdBuf[13] += 0x37;
			}
			CmdBuf[14] = (MCU_CAP_001_BIT19 << 3) | (MCU_CAP_001_BIT18 << 2) | (MCU_CAP_001_BIT17 << 1) | MCU_CAP_001_BIT16;
			if(CmdBuf[14] <= 9)
			{
				CmdBuf[14] += 0x30;
			}
			else
			{
				CmdBuf[14] += 0x37;
			}
			CmdBuf[15] = (MCU_CAP_001_BIT15 << 3) | (MCU_CAP_001_BIT14 << 2) | (MCU_CAP_001_BIT13 << 1) | MCU_CAP_001_BIT12;
			if(CmdBuf[15] <= 9)
			{
				CmdBuf[15] += 0x30;
			}
			else
			{
				CmdBuf[15] += 0x37;
			}
			CmdBuf[16] = (MCU_CAP_001_BIT11 << 3) | (MCU_CAP_001_BIT10 << 2) | (MCU_CAP_001_BIT9 << 1) | MCU_CAP_001_BIT8;
			if(CmdBuf[16] <= 9)
			{
				CmdBuf[16] += 0x30;
			}
			else
			{
				CmdBuf[16] += 0x37;
			}
			CmdBuf[17] = (MCU_CAP_001_BIT7 << 3) | (MCU_CAP_001_BIT6 << 2) | (MCU_CAP_001_BIT5 << 1) | MCU_CAP_001_BIT4;
			if(CmdBuf[17] <= 9)
			{
				CmdBuf[17] += 0x30;
			}
			else
			{
				CmdBuf[17] += 0x37;
			}
			CmdBuf[18] = (MCU_CAP_001_BIT3 << 3) | (MCU_CAP_001_BIT2 << 2) | (MCU_CAP_001_BIT1 << 1) | MCU_CAP_001_BIT0;
			if(CmdBuf[18] <= 9)
			{
				CmdBuf[18] += 0x30;
			}
			else
			{
				CmdBuf[18] += 0x37;
			}
			CmdBuf[19] = '&';
			Len = 20;
			break;	

		case MCU_CAP_002:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+002", 11);
			CmdBuf[11] = (MCU_CAP_002_BIT31 << 3) | (MCU_CAP_002_BIT30 << 2) | (MCU_CAP_002_BIT29 << 1) | MCU_CAP_002_BIT28;	
			if(CmdBuf[11] <= 9)
			{
				CmdBuf[11] += 0x30;
			}
			else
			{
				CmdBuf[11] += 0x37;
			}
			CmdBuf[12] = (MCU_CAP_002_BIT27 << 3) | (MCU_CAP_002_BIT26 << 2) | (MCU_CAP_002_BIT25 << 1) | MCU_CAP_002_BIT24;	
			if(CmdBuf[12] <= 9)
			{
				CmdBuf[12] += 0x30;
			}
			else
			{
				CmdBuf[12] += 0x37;
			}
			CmdBuf[13] = (MCU_CAP_002_BIT23 << 3) | (MCU_CAP_002_BIT22 << 2) | (MCU_CAP_002_BIT21 << 1) | MCU_CAP_002_BIT20;	
			if(CmdBuf[13] <= 9)
			{
				CmdBuf[13] += 0x30;
			}
			else
			{
				CmdBuf[13] += 0x37;
			}
			CmdBuf[14] = (MCU_CAP_002_BIT19 << 3) | (MCU_CAP_002_BIT18 << 2) | (MCU_CAP_002_BIT17 << 1) | MCU_CAP_002_BIT16;
			if(CmdBuf[14] <= 9)
			{
				CmdBuf[14] += 0x30;
			}
			else
			{
				CmdBuf[14] += 0x37;
			}
			CmdBuf[15] = (MCU_CAP_002_BIT15 << 3) | (MCU_CAP_002_BIT14 << 2) | (MCU_CAP_002_BIT13 << 1) | MCU_CAP_002_BIT12;
			if(CmdBuf[15] <= 9)
			{
				CmdBuf[15] += 0x30;
			}
			else
			{
				CmdBuf[15] += 0x37;
			}
			CmdBuf[16] = (MCU_CAP_002_BIT11 << 3) | (MCU_CAP_002_BIT10 << 2) | (MCU_CAP_002_BIT9 << 1) | MCU_CAP_002_BIT8;
			if(CmdBuf[16] <= 9)
			{
				CmdBuf[16] += 0x30;
			}
			else
			{
				CmdBuf[16] += 0x37;
			}
			CmdBuf[17] = (MCU_CAP_002_BIT7 << 3) | (MCU_CAP_002_BIT6 << 2) | (MCU_CAP_002_BIT5 << 1) | MCU_CAP_002_BIT4;
			if(CmdBuf[17] <= 9)
			{
				CmdBuf[17] += 0x30;
			}
			else
			{
				CmdBuf[17] += 0x37;
			}
			CmdBuf[18] = (MCU_CAP_002_BIT3 << 3) | (MCU_CAP_002_BIT2 << 2) | (MCU_CAP_002_BIT1 << 1) | MCU_CAP_002_BIT0;
			if(CmdBuf[18] <= 9)
			{
				CmdBuf[18] += 0x30;
			}
			else
			{
				CmdBuf[18] += 0x37;
			}
			CmdBuf[19] = '&';
			Len = 20;
			break;	

		case MCU_CAP_LAU:
#if (MCU_CAP_LAU_ENGLISH == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUen_us&", 17);		
			Len = 17;
#elif (MCU_CAP_LAU_CHINESE == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUzh_cn&", 17);
			Len = 17;
#elif (MCU_CAP_LAU_FRENCH == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUfrench&", 18);
			Len = 18;
#elif (MCU_CAP_LAU_PORTUGUESE == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUportuguese&", 22);
			Len = 22;
#elif (MCU_CAP_LAU_ITALIAN == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUitalian&", 19);
			Len = 19;
#elif (MCU_CAP_LAU_GERMANY == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUGer_de&", 18);
			Len = 18;
#elif (MCU_CAP_LAU_SPANISH == 1)
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+LAUspanish&", 19);
			Len = 19;
#endif
			break;

		case MCU_LAULIST:
			break;

		case MCU_CAP_STM:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+STM", 11);			
			CmdBuf[11] = (MCU_CAP_STM_BIT31 << 3) | (MCU_CAP_STM_BIT30 << 2) | (MCU_CAP_STM_BIT29 << 1) | MCU_CAP_STM_BIT28;			
			if(CmdBuf[11] <= 9)
			{
				CmdBuf[11] += 0x30;
			}
			else
			{
				CmdBuf[11] += 0x37;
			}
			CmdBuf[12] = (MCU_CAP_STM_BIT27 << 3) | (MCU_CAP_STM_BIT26 << 2) | (MCU_CAP_STM_BIT25 << 1) | MCU_CAP_STM_BIT24;	
			if(CmdBuf[12] <= 9)
			{
				CmdBuf[12] += 0x30;
			}
			else
			{
				CmdBuf[12] += 0x37;
			}
			CmdBuf[13] = (MCU_CAP_STM_BIT23 << 3) | (MCU_CAP_STM_BIT22 << 2) | (MCU_CAP_STM_BIT21 << 1) | MCU_CAP_STM_BIT20;
			if(CmdBuf[13] <= 9)
			{
				CmdBuf[13] += 0x30;
			}
			else
			{
				CmdBuf[13] += 0x37;
			}
			CmdBuf[14] = (MCU_CAP_STM_BIT19 << 3) | (MCU_CAP_STM_BIT18 << 2) | (MCU_CAP_STM_BIT17 << 1) | MCU_CAP_STM_BIT16;
			if(CmdBuf[14] <= 9)
			{
				CmdBuf[14] += 0x30;
			}
			else
			{
				CmdBuf[14] += 0x37;
			}
			CmdBuf[15] = (MCU_CAP_STM_BIT15 << 3) | (MCU_CAP_STM_BIT14 << 2) | (MCU_CAP_STM_BIT13 << 1) | MCU_CAP_STM_BIT12;
			if(CmdBuf[15] <= 9)
			{
				CmdBuf[15] += 0x30;
			}
			else
			{
				CmdBuf[15] += 0x37;
			}
			CmdBuf[16] = (MCU_CAP_STM_BIT11 << 3) | (MCU_CAP_STM_BIT10 << 2) | (MCU_CAP_STM_BIT9 << 1) | MCU_CAP_STM_BIT8;
			if(CmdBuf[16] <= 9)
			{
				CmdBuf[16] += 0x30;
			}
			else
			{
				CmdBuf[16] += 0x37;
			}
			CmdBuf[17] = (MCU_CAP_STM_BIT7 << 3) | (MCU_CAP_STM_BIT6 << 2) | (MCU_CAP_STM_BIT5 << 1) | MCU_CAP_STM_BIT4;
			if(CmdBuf[17] <= 9)
			{
				CmdBuf[17] += 0x30;
			}
			else
			{
				CmdBuf[17] += 0x37;
			}
			CmdBuf[18] = (MCU_CAP_STM_BIT3 << 3) | (MCU_CAP_STM_BIT2 << 2) | (MCU_CAP_STM_BIT1 << 1) | MCU_CAP_STM_BIT0;
			if(CmdBuf[18] <= 9)
			{
				CmdBuf[18] += 0x30;
			}
			else
			{
				CmdBuf[18] += 0x37;
			}
			CmdBuf[19] = '&';
			Len = 20;
			break;	

		case MCU_CAP_PLM:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+PLM", 11);			
			CmdBuf[11] = (MCU_CAP_PLM_BIT31 << 3) | (MCU_CAP_PLM_BIT30 << 2) | (MCU_CAP_PLM_BIT29 << 1) | MCU_CAP_PLM_BIT28;			
			if(CmdBuf[11] <= 9)
			{
				CmdBuf[11] += 0x30;
			}
			else
			{
				CmdBuf[11] += 0x37;
			}
			CmdBuf[12] = (MCU_CAP_PLM_BIT27 << 3) | (MCU_CAP_PLM_BIT26 << 2) | (MCU_CAP_PLM_BIT25 << 1) | MCU_CAP_PLM_BIT24;	
			if(CmdBuf[12] <= 9)
			{
				CmdBuf[12] += 0x30;
			}
			else
			{
				CmdBuf[12] += 0x37;
			}
			CmdBuf[13] = (MCU_CAP_PLM_BIT23 << 3) | (MCU_CAP_PLM_BIT22 << 2) | (MCU_CAP_PLM_BIT21 << 1) | MCU_CAP_PLM_BIT20;
			if(CmdBuf[13] <= 9)
			{
				CmdBuf[13] += 0x30;
			}
			else
			{
				CmdBuf[13] += 0x37;
			}
			CmdBuf[14] = (MCU_CAP_PLM_BIT19 << 3) | (MCU_CAP_PLM_BIT18 << 2) | (MCU_CAP_PLM_BIT17 << 1) | MCU_CAP_PLM_BIT16;
			if(CmdBuf[14] <= 9)
			{
				CmdBuf[14] += 0x30;
			}
			else
			{
				CmdBuf[14] += 0x37;
			}
			CmdBuf[15] = (MCU_CAP_PLM_BIT15 << 3) | (MCU_CAP_PLM_BIT14 << 2) | (MCU_CAP_PLM_BIT13 << 1) | MCU_CAP_PLM_BIT12;
			if(CmdBuf[15] <= 9)
			{
				CmdBuf[15] += 0x30;
			}
			else
			{
				CmdBuf[15] += 0x37;
			}
			CmdBuf[16] = (MCU_CAP_PLM_BIT11 << 3) | (MCU_CAP_PLM_BIT10 << 2) | (MCU_CAP_PLM_BIT9 << 1) | MCU_CAP_PLM_BIT8;
			if(CmdBuf[16] <= 9)
			{
				CmdBuf[16] += 0x30;
			}
			else
			{
				CmdBuf[16] += 0x37;
			}
			CmdBuf[17] = (MCU_CAP_PLM_BIT7 << 3) | (MCU_CAP_PLM_BIT6 << 2) | (MCU_CAP_PLM_BIT5 << 1) | MCU_CAP_PLM_BIT4;
			if(CmdBuf[17] <= 9)
			{
				CmdBuf[17] += 0x30;
			}
			else
			{
				CmdBuf[17] += 0x37;
			}
			CmdBuf[18] = (MCU_CAP_PLM_BIT3 << 3) | (MCU_CAP_PLM_BIT2 << 2) | (MCU_CAP_PLM_BIT1 << 1) | MCU_CAP_PLM_BIT0;
			if(CmdBuf[18] <= 9)
			{
				CmdBuf[18] += 0x30;
			}
			else
			{
				CmdBuf[18] += 0x37;
			}
			CmdBuf[19] = '&';
			Len = 20;
			break;

		case MCU_CAP_EXT:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+EXT", 11);			
			CmdBuf[11] = (MCU_CAP_EXT_BIT31 << 3) | (MCU_CAP_EXT_BIT30 << 2) | (MCU_CAP_EXT_BIT29 << 1) | MCU_CAP_EXT_BIT28;			
			if(CmdBuf[11] <= 9)
			{
				CmdBuf[11] += 0x30;
			}
			else
			{
				CmdBuf[11] += 0x37;
			}
			CmdBuf[12] = (MCU_CAP_EXT_BIT27 << 3) | (MCU_CAP_EXT_BIT26 << 2) | (MCU_CAP_EXT_BIT25 << 1) | MCU_CAP_EXT_BIT24;	
			if(CmdBuf[12] <= 9)
			{
				CmdBuf[12] += 0x30;
			}
			else
			{
				CmdBuf[12] += 0x37;
			}
			CmdBuf[13] = (MCU_CAP_EXT_BIT23 << 3) | (MCU_CAP_EXT_BIT22 << 2) | (MCU_CAP_EXT_BIT21 << 1) | MCU_CAP_EXT_BIT20;
			if(CmdBuf[13] <= 9)
			{
				CmdBuf[13] += 0x30;
			}
			else
			{
				CmdBuf[13] += 0x37;
			}
			CmdBuf[14] = (MCU_CAP_EXT_BIT19 << 3) | (MCU_CAP_EXT_BIT18 << 2) | (MCU_CAP_EXT_BIT17 << 1) | MCU_CAP_EXT_BIT16;
			if(CmdBuf[14] <= 9)
			{
				CmdBuf[14] += 0x30;
			}
			else
			{
				CmdBuf[14] += 0x37;
			}
			CmdBuf[15] = (MCU_CAP_EXT_BIT15 << 3) | (MCU_CAP_EXT_BIT14 << 2) | (MCU_CAP_EXT_BIT13 << 1) | MCU_CAP_EXT_BIT12;
			if(CmdBuf[15] <= 9)
			{
				CmdBuf[15] += 0x30;
			}
			else
			{
				CmdBuf[15] += 0x37;
			}
			CmdBuf[16] = (MCU_CAP_EXT_BIT11 << 3) | (MCU_CAP_EXT_BIT10 << 2) | (MCU_CAP_EXT_BIT9 << 1) | MCU_CAP_EXT_BIT8;
			if(CmdBuf[16] <= 9)
			{
				CmdBuf[16] += 0x30;
			}
			else
			{
				CmdBuf[16] += 0x37;
			}
			CmdBuf[17] = (MCU_CAP_EXT_BIT7 << 3) | (MCU_CAP_EXT_BIT6 << 2) | (MCU_CAP_EXT_BIT5 << 1) | MCU_CAP_EXT_BIT4;
			if(CmdBuf[17] <= 9)
			{
				CmdBuf[17] += 0x30;
			}
			else
			{
				CmdBuf[17] += 0x37;
			}
			CmdBuf[18] = (MCU_CAP_EXT_BIT3 << 3) | (MCU_CAP_EXT_BIT2 << 2) | (MCU_CAP_EXT_BIT1 << 1) | MCU_CAP_EXT_BIT0;
			if(CmdBuf[18] <= 9)
			{
				CmdBuf[18] += 0x30;
			}
			else
			{
				CmdBuf[18] += 0x37;
			}
			CmdBuf[19] = '&';
			Len = 20;
			break;

		case MCU_PAR_XXX:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PARAMS+", 11);			
			CmdBuf[11] = MCU_PARAMS_FOR % 10 + 0x30;
			CmdBuf[12] = ':';
			CmdBuf[13] = (MCU_PARAMS_VAL / 1000) + 0x30;
			CmdBuf[14] = ((MCU_PARAMS_VAL / 100) % 10) + 0x30;
			CmdBuf[15] = ((MCU_PARAMS_VAL % 100) / 10) + 0x30;
			CmdBuf[16] = (MCU_PARAMS_VAL % 10) + 0x30;
			CmdBuf[17] = '&';
			Len = 18;
			break;
		case MCU_PRESETN:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PRESET+", 11);			
			CmdBuf[11] = MCU_PRESET_VAL / 10 + 0x30;
			CmdBuf[12] = MCU_PRESET_VAL % 10 + 0x30;
			CmdBuf[13] = '&';
			Len = 14;
			break;	

		case MCU_SET_PWD:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CAP+PWD", 11);			
			Temp = 11;
			memcpy((void*)&CmdBuf[Temp], Ap_PassWord, sizeof(Ap_PassWord));
			Temp += sizeof(Ap_PassWord);
			CmdBuf[Temp-1] += '&';
			Len = Temp;
			break;		

		case MCU_VMX_XXX:	
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+VMX+", 8);
			CmdBuf[8] = MCU_VMX_VAL / 100 + 0x30;
			CmdBuf[9] = (MCU_VMX_VAL % 100) / 10 + 0x30;
			CmdBuf[10] = MCU_VMX_VAL % 10 + 0x30;
			Len = 11;			
			break;
		
		case MCU_FMT_XXX:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+FMT+", 8);
			CmdBuf[8] = MCU_FMT_VAL / 100 + 0x30;
			CmdBuf[9] = (MCU_FMT_VAL % 100) / 10 + 0x30;
			CmdBuf[10] = MCU_FMT_VAL % 10 + 0x30;
			Len = 11;			
			break;
		//end AXX+CAP+GET;
		
		case MCU_PLY_LKE:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PLY+LKE", 11);
			Len = 11;
			break;			

		case MCU_CHL_NXT:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+CHL+NXT", 11);
			Len = 11;
			break;	

		case MCU_LAU_XXX:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+LAU+", 8);
			CmdBuf[8] = DataBuf[0] / 100 + 0x30;
			CmdBuf[9] = (DataBuf[0] % 100) / 10 + 0x30;
			CmdBuf[10] = DataBuf[0] % 10 + 0x30;
			Len = 11;		
			break;

		case MCU_LAU_GET:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+LAU+GET", 11);
			Len = 11;		
			break;

		case MCU_PMT_GET:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+PMT+GET", 11);
			Len = 11;		
			break;

		case MCU_VER_NUM:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+VER+", 8);
			CmdBuf[8] = (McuSoftSdkVerNum / 1000) + 0x30;
			CmdBuf[9] = ((McuSoftSdkVerNum / 100) % 10) + 0x30;
			CmdBuf[10] = ((McuSoftSdkVerNum % 100) / 10) + 0x30;
			CmdBuf[11] = (McuSoftSdkVerNum % 10) + 0x30;
			CmdBuf[12] = '&';
			Len = 13;
			break;

		case MCU_LGT__ON:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+LGT++ON", 11);
			CmdBuf[11] = '&';
			Len = 12;
			break;

		case MCU_RAD_XXX:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+RAD+", 8);
			CmdBuf[8] = (DataBuf[0] / 100) + 0x30;
			CmdBuf[9] = ((DataBuf[0] % 100) /10) + 0x30;
			CmdBuf[10] = (DataBuf[0] % 10) + 0x30;
			CmdBuf[11] = '&';
			Len = 12;
			break;

#ifdef FUNC_WIFI_UART_UPGRADE			
		case MCU_UPG_STA:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+UPGRADEstart&", 17);
			Len = 17;
			break;
			
		case MCU_UPG_RUN:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+UPGRADErun:", 15);
			Temp = 15;
			
			{
				extern uint16_t SoftBankIndex;

				if(SoftBankIndex == 0x0FFF) //第一包数据校验错误
				{
					CmdBuf[Temp++] = '-';
					CmdBuf[Temp++] = 0x31;
				}
				else
				{
					if(SoftBankIndex / 1000)
					{
						memcpy((void*)&CmdBuf[Temp], DataBuf, 4);
						Temp = 19;
					}
					else if(SoftBankIndex / 100)
					{	
						memcpy((void*)&CmdBuf[Temp], DataBuf, 3);
						Temp = 18;
					}
					else if(SoftBankIndex / 10)
					{	
						memcpy((void*)&CmdBuf[Temp], DataBuf, 2);
						Temp = 17;
					}
					else
					{
						memcpy((void*)&CmdBuf[Temp], DataBuf, 1);
						Temp = 16;
					}
				}
			}
			
			CmdBuf[Temp] = '&';
			Len = Temp + 1;
			break;

		case MCU_UPG_END:			
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+UPGRADEfinish&", 18);
			Len = 18;
			break;
		
		case MCU_UPG_FAL:
			memcpy((void*)&CmdBuf[Temp], (void*)"MCU+UPGRADEerror:", 17);
			Temp = 17;
			CmdBuf[Temp++] = DataBuf[0] + 0x30;
			CmdBuf[Temp++] = '&';
			Len = Temp;
			break;			
#endif
			
		default:
			break;
	}	
	

	CmdBuf[Len] = '\0';
	APP_DBG("Send gWiFiCmd is:%s\n", CmdBuf);

	if(memcmp(CmdBuf, "MCU+BAT+", 8) != 0)
	{
#ifdef FUNC_SLEEP_EN
		gSys.SleepTimeCnt = 0;
		gSys.SleepStartPowerOff = FALSE;
#endif

#ifdef FUNC_SLEEP_LEDOFF_EN
		gSys.SleepLedOffCnt = FALSE;
		gSys.SleepLedOffFlag = FALSE;
#endif	
	}
	
#ifdef WIFI_SELECT_BUART_COM
	BuartSend(CmdBuf, Len);
#else
	FuartSend(CmdBuf, Len);
#endif
}

//WiFi模组发送给MCU的命令功能处理
void WiFi_SendCmdToMcu(uint16_t WiFiCmd, uint8_t* CmdData)
{
	uint16_t Temp;
		
	switch(WiFiCmd)
	{		
		case AXX_CAP_GET:
			WiFiGetProjectParams();
			break;

		case AXX_GET_SID:
			Mcu_SendCmdToWiFi(MCU_SID_VAL, NULL);
			break;
			
		case AXX_BOT_DON:
			//升级重启后，初始化所有参数，为了显示LED灯
			memcpy((void*)&gWiFi, (void*)&InitgWiFi, sizeof(WIFI_WORK_STATE));
			break;
			
		case AXX_MCU_RDY:
			WiFiWorkStateSet(WIFI_STATUS_INIT_END);
			//通知凯叔app，小夜灯在开启状态
			Mcu_SendCmdToWiFi(MCU_LGT__ON, NULL);
			//凯叔故事机开机默认播放本地TF卡;
			if(MODULE_ID_PLAYER_WIFI_SD != gSys.CurModuleID)
			{
				gSys.NextModuleID = MODULE_ID_PLAYER_WIFI_SD;
				MsgSend(MSG_COMMON_CLOSE);
			}
			//Mcu_SendCmdToWiFi(MCU_MMC_GET, NULL);
			break;

		case AXX_MAC_XXX:
			break;
		
		case AXX_BOT_UP0:
		case AXX_DEV_RST:
			WiFiWorkStateSet(WIFI_STATUS_REBOOT_MCU);
			break;
			
		case AXX_POW_OFF:
			WiFiRequestMcuPowerOff();
			break;

		case AXX_POW_STA:
			WiFiPowerStateSet(&gWiFiData[0]);
			break;
		
		case AXX_FACTORY:
#ifdef FUNC_BT_EN
			{
				extern bool FlshBTInfoAreaErase(void);
				
				BTDisConnect();
				FlshBTInfoAreaErase();
			}
#endif
			WiFiFactoryStateSet(1);
			WiFiMicOn();
			gSys.Volume = DEFAULT_VOLUME;
			AudioSysInfoSetBreakPoint();
			if(gSys.CurModuleID != MODULE_ID_WIFI)
			{
				gSys.NextModuleID = MODULE_ID_WIFI;
				MsgSend(MSG_COMMON_CLOSE);
				APP_DBG("FACTORY Reset TO WiFi Module!!!\n");
			}
			break;

		case AXX_BURNING:
			WiFiWorkStateSet(WIFI_STATUS_INITING);
			gSys.Volume = DEFAULT_VOLUME;
			AudioSysInfoSetBreakPoint();
			WiFiFirmwareUpgradeStateSet(1);
			if(gSys.CurModuleID != MODULE_ID_WIFI)
			{
				gSys.NextModuleID = MODULE_ID_WIFI;
				MsgSend(MSG_COMMON_CLOSE);
			}
			break;

		case AXX_BURN002:
			WiFiFirmwareUpgradeStateSet(2);
			break;

		case AXX_BURN003:
			WiFiFirmwareUpgradeStateSet(3);
			break;

		case AXX_WPS_ON:	
			WiFiWpsStateSet(WIFI_STATUS_WPS_SCANNING);
			break;

		case AXX_LED_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiSetMcuLedState(Temp);
			break;

		case AXX_WPS_END:
			WiFiWpsStateSet(WIFI_STATUS_WPS_SCAN_STOP);
			break;

		case AXX_STA_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			
			if(Temp == 0x01)
			{
				WiFiStationStateSet(WIFI_STATUS_STATION_CONNECTED);
				WiFiWpsStateSet(WIFI_STATUS_WPS_SCAN_STOP);
			}
			else if(Temp == 0x02)
			{
				if(gWiFi.WPSState == WIFI_STATUS_WPS_SCANNING)
				{
					WiFiStationStateSet(WIFI_STATUS_STATION_CONNECTING);
				}
			}	
			else
			{
				WiFiStationStateSet(WIFI_STATUS_STATION_DISCONNECTED);
				if(gWiFi.WPSState == WIFI_STATUS_WPS_SCANNING)
				{
					Mcu_SendCmdToWiFi(MCU_WIF_EPS, NULL);
					WiFiWpsStateSet(WIFI_STATUS_WPS_SCAN_STOP);					
				}
			}			
			break;

		case AXX_ETH_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			if(Temp == 0x01)
			{
				WiFiEthStateSet(WIFI_STATUS_ETH_PLUGIN);
			}
			else if(Temp == 0x00)
			{
				WiFiEthStateSet(WIFI_STATUS_ETH_PLUGOUT);
			}
			else if(Temp == 0xFFF)
			{
				WiFiEthStateSet(WIFI_STATUS_ETH_PERMITED);
			}
			break;

		case AXX_WWW_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			if(Temp == 0x01)
			{
				if(gWiFi.WWWState != WIFI_STATUS_WWW_ENABLE)
				{
					WiFiStationStateSet(WIFI_STATUS_STATION_CONNECTED);
				}
				WiFiWwwStateSet(WIFI_STATUS_WWW_ENABLE);
			}
			else if(Temp == 0x00)
			{
				WiFiWwwStateSet(WIFI_STATUS_WWW_DISABLE);
			}
			break;

		case AXX_RA0_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			if(Temp == 0x00)
			{
				WiFiRa0StateSet(WIFI_STATUS_AP_NO_CONNECTED);
			}
			else if(Temp == 0x01)
			{
				WiFiRa0StateSet(WIFI_STATUS_AP_CONNECTED);
			}
			else if(Temp == 0x02)
			{
				WiFiRa0StateSet(WIFI_STATUS_AP_DISCONNECTED);
			}
			else if(Temp == 0xFFF)
			{
				WiFiRa0StateSet(WIFI_STATUS_AP_HIDDEN);
			}		 
			break;
			
		case AXX_MUT_NNN:
			if(CmdData[0] == '1')
			{
				WiFiMuteStateSet(1);
			}
			else if(CmdData[0] == '0')
			{
				WiFiMuteStateSet(0);
			}
			break;

		case AXX_VOL_GET:					
			WiFiGetMcuVolume();
			break;

		case AXX_VOL_NNN:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiSetMcuVolume(Temp);
			break;

		case AXX_KEY_RDY:
			break;

		case AXX_KEY_NIL:
			break;

		case AXX_PLP_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiSetRepeatMode(Temp);
			break;

		case AXX_CHN_XXX:
			//声道提示
			break;

		case AXX_SEL_PLY:
			break;

		case AXX_SEL_ERR:
			break;
		
		case AXX_MCU_VER:
			Mcu_SendCmdToWiFi(MCU_VER_NUM, NULL);
			break;	

		case AXX_PLY_XXX:
			if(CmdData[0] == '1')
			{
				WiFiWorkStateSet(WIFI_STATUS_PLAY_PLAYING);
				WiFiMuteStateSet(0);         //UnMUTE
			}
			else if(CmdData[0] == '0')
			{
				WiFiWorkStateSet(WIFI_STATUS_PLAY_PAUSE);
			}
			break;

		case AXX_PLM_GET:
			WiFiGetMcuCurPlayMode();
			break;

		case AXX_PLM_AVS:		
		case AXX_PLM_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiAppSetPlayMode(WiFiCmd,Temp);
			break;

		case AXX_USB_XXX:
			if(CmdData[0] == '1')
			{
				WiFiUSBStateSet(WIFI_STATUS_USB_PLUGIN);
			}
			else if(CmdData[0] == '0')
			{
				WiFiUSBStateSet(WIFI_STATUS_USB_PLUGOUT);
			}
			break;

		case AXX_MMC_XXX:
			if(CmdData[0] == '1')
			{
				WiFiCardStateSet(WIFI_STATUS_SD_PLUGIN);
			}
			else if(CmdData[0] == '0')
			{
				WiFiCardStateSet(WIFI_STATUS_SD_PLUGOUT);
			}
			break;

		case AXX_SLV_NOT:
			WiFiMutliRoomStateSet(0);
			break;

		case AXX_SLV_YES:
			WiFiMutliRoomStateSet(1);
			break;

		case AXX_SLV_XXX:	
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiSlaveSounBoxCntSet(Temp);
			break;

		case AXX_LAU_GET:
			WiFiGetMcuSoundRemindLanguage();
			break;

		case AXX_LAU_XXX:
			gSys.LanguageMode = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			break;

		case AXX_PMT_XXX:	
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiSoundRemindStateSet(Temp);
			break;

		case AXX_SET_RTC:
			WiFiSetMcuSystemTime(&gWiFiData[0]);
			break;

		case AXX_SET_WEK:
			WiFiSetMcuWeekDay(&gWiFiData[0]);
			break;

		case AXX_GET_RTC:
			break;

		case AXX_APP_RTC:
			break;

		case AXX_APP_WEK:
			break;

		case AXX_NXT_ALS:
			WiFiNoticeMcuNextAlarmTime(&gWiFiData[0]);
			break;

		case AXX_NXT_ALM:
			//WiFiSetMcuAlarmTime(&gWiFiData[0]);
			break;

		case AXX_LED_TES:
			WiFiTestModeStateSet();
			break;

		case AXX_TLK_ON:
			WiFiTalkStateSet(1);
			break;

		case AXX_TLK_OFF:
			WiFiTalkStateSet(0);
			break;

		case AXX_TLK_ENA:
			WiFiMicOn();
			WiFiSetMicState(WIFI_AVS_STATUS_IDLE);
			break;

		case AXX_TLK_DIS:
			WiFiMicOff();
			WiFiSetMicState(WIFI_AVS_STATUS_DIS);
			break;

		case AXX_WAN_LST:			
			break;

		case AXX_STA_INF:			
			break;

		case AXX_M2S_NNN:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			SlaveMcuRevPassThroughCmd(Temp);
			break;

		case AXX_S2M_NNN:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			MasterMcuRevPassThroughCmd(Temp);
			break;		
			
		case AXX_PAS_DATA:
			McuRevPassThroughData(&gWiFiData[0]);
			break;
			
		case AXX_RAD_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiKaiShuRadioSet(Temp);
			break;

		case AXX_LGT__ON:
		    gWiFi.KaiShuLightState = TRUE;
			SysBackLightBrightOnControl(TRUE);
			break;
			
		case AXX_LGT_OFF:
		    gWiFi.KaiShuLightState = FALSE;
			SysBackLightBrightOnControl(FALSE);
			break;

		case AXX_LCK__ON:
			WiFiKaiShuChildLockStateSet(TRUE);
			break;

		case AXX_LCK_OFF:
			WiFiKaiShuChildLockStateSet(FALSE);
			break;

		case AXX_VLM_XXX:
			Temp = (CmdData[0] - 0x30) * 100 + (CmdData[1] - 0x30) * 10 + (CmdData[2] - 0x30);
			WiFiKaiShuVolumeMaxSet(Temp);
			break;

		case AXX_SLP__ON:
			WiFiKaiShuSleepModeDeal(TRUE, FALSE);
			break;

		case AXX_SLP_OFF:
			WiFiKaiShuSleepModeDeal(FALSE, TRUE);
			break;
			
#ifdef	FUNC_WIFI_UART_UPGRADE
		case AXX_MCU_UPD:
			WiFiUpgradeMcuSoftMsg(&gWiFiData[0]);
			break;

		case AXX_MCU_RUN:
			WiFiUpgradeMcuSoftRunning(&gWiFiData[0]);
			break;
#endif

		default:
			break;
	}
}

//WiFi模组命令的接送解析
void WiFi_CmdProcess(void)
{
	if((WiFiPowerOnInitStateGet() == FALSE) || (Mcu_RcvWiFiCmd() == FALSE))
	{
		return;
	}

#ifdef FUNC_WIFI_BT_CONTROL_EN
	if(BtCtrl.IsScanEnableDone == TRUE)
	{
		BtCtrl.IsScanEnableDone = FALSE;
		if(BtCtrl.State == BT_CTRL_STATE_PAIR)
		{
			Mcu_SendBtCmdToWiFi(MCU_BT_PARF, NULL);
		}
		else if(BtCtrl.State == BT_CTRL_STATE_CTDI)
		{
			Mcu_SendBtCmdToWiFi(MCU_BT_CTIF, NULL);
		}
		else if(BtCtrl.State == BT_CTRL_STATE_CTPL)
		{
			Mcu_SendBtCmdToWiFi(MCU_BT_CTPF, NULL);
		}
		else
		{
			BTSetScanDisable();
		}
	}
#endif

	if(memcmp(gWiFiCmd, "+MUT+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_MUT_NNN, &gWiFiCmd[7]);
	}
	else if(memcmp(gWiFiCmd, "+VOL+GET", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_VOL_GET, NULL);
	}	
	else if(memcmp(gWiFiCmd, "+VOL+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_VOL_NNN, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+PLY+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_PLY_XXX, &gWiFiCmd[7]);
	}	
	else if(memcmp(gWiFiCmd, "+PLM+GET", 8) == 0)
	{	
		WiFi_SendCmdToMcu(AXX_PLM_GET, NULL);
	}
	else if(memcmp(gWiFiCmd, "+PLM+", 5) == 0)
	{    	
		WiFi_SendCmdToMcu(AXX_PLM_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+PLM-", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_PLM_AVS, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+SLV+NOT", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_SLV_NOT, NULL);
	}
	else if(memcmp(gWiFiCmd, "+SLV+YES", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SLV_YES, NULL);
	}
	else if(memcmp(gWiFiCmd, "+SLV+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SLV_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+PMT+", 5) == 0)
	{	
		WiFi_SendCmdToMcu(AXX_PMT_XXX, &gWiFiCmd[5]);
	}	
	else if(memcmp(gWiFiCmd, "+USB+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_USB_XXX, &gWiFiCmd[7]);
	}
	else if(memcmp(gWiFiCmd, "+MMC+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_MMC_XXX, &gWiFiCmd[7]);
	}
	else if(memcmp(gWiFiCmd, "+KEY+RDY", 8) == 0)
	{
    	WiFi_SendCmdToMcu(AXX_KEY_RDY, NULL);
	}
	else if(memcmp(gWiFiCmd, "+KEY+NIL", 8) == 0)
	{
    	WiFi_SendCmdToMcu(AXX_KEY_NIL, NULL);
	}	
	else if(memcmp(gWiFiCmd, "+PLP+", 5) == 0)
	{	
		WiFi_SendCmdToMcu(AXX_PLP_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+CHN+", 5) == 0)
	{	
		WiFi_SendCmdToMcu(AXX_CHN_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+CAP+GET", 8) == 0)
	{    	
		WiFi_SendCmdToMcu(AXX_CAP_GET, NULL);
	}
	else if(memcmp(gWiFiCmd, "+GET+SID", 8) == 0)
	{
		//WiFi_SendCmdToMcu(AXX_GET_SID, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+BOT+DON", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_BOT_DON, NULL);
	}
	else if(memcmp(gWiFiCmd, "+BOT+UP0", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_BOT_UP0, NULL);
	}	
	else if(memcmp(gWiFiCmd, "+MCU+RDY", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_RDY, NULL);
	}
	else if(memcmp(gWiFiCmd, "+UID+", 5) == 0)
	{		
		WiFiDataRcvStartFlag = TRUE;
	}
	else if(memcmp(gWiFiCmd, "+MAC+", 5) == 0)
	{		
		WiFiDataRcvStartFlag = TRUE;
	}
	else if(memcmp(gWiFiCmd, "+DEV+RST", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_DEV_RST, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+POW+OFF", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_POW_OFF, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+POW+%03", 8) == 0)
	{
		WiFiDataRcvStartFlag = TRUE;
	}
	else if(memcmp(gWiFiCmd, "+FACTORY", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_FACTORY, NULL);	
	}
	else if(memcmp(gWiFiCmd, "+BURNING", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_BURNING, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+BURN002", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_BURN002, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+BURN003", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_BURN003, NULL);		
	}		
	else if(memcmp(gWiFiCmd, "+SEL+PLY", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SEL_PLY, NULL);
	}
	else if(memcmp(gWiFiCmd, "+SEL+ERR", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SEL_ERR, NULL);
	}
	else if(memcmp(gWiFiCmd, "+WPS+END", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_WPS_END, NULL);
	}
	else if(memcmp(gWiFiCmd, "+WPS++ON", 8) == 0)
	{		
    //20160517 增加同步异步播放提示		
		WiFi_SendCmdToMcu(AXX_WPS_ON,  NULL);
	}		
	else if(memcmp(gWiFiCmd, "+LED+TES", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LED_TES, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+LED+", 5) == 0)
	{			
		WiFi_SendCmdToMcu(AXX_LED_XXX, &gWiFiCmd[5]);
	}	
	else if(memcmp(gWiFiCmd, "AXX+WAN+LST", 11) == 0)
	{		
		WiFiDataRcvStartFlag = TRUE;
	}
	else if(memcmp(gWiFiCmd, "+STA+INF", 8) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_STA_INF, NULL);
	}
	else if(memcmp(gWiFiCmd, "+STA+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_STA_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+ETH+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_ETH_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+WWW+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_WWW_XXX, &gWiFiCmd[5]);
	}	
	else if(memcmp(gWiFiCmd, "+RA0+", 5) == 0)
	{		
		WiFi_SendCmdToMcu(AXX_RA0_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+LAU+GET", 8) == 0)
	{		
		//WiFi_SendCmdToMcu(AXX_LAU_GET, NULL);
	}
	else if(memcmp(gWiFiCmd, "+LAU+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LAU_XXX, &gWiFiCmd[5]);		
	}				
	else if(memcmp(gWiFiCmd, "+SET+RTC", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SET_RTC, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+SET+WEK", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SET_WEK, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+NXT+ALS", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_NXT_ALS, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+NXT+ALM", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_NXT_ALM, NULL);		
	}	
	else if(memcmp(gWiFiCmd, "+M2S+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_M2S_NNN, &gWiFiCmd[5]);		
	}	
	else if(memcmp(gWiFiCmd, "+S2M+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_S2M_NNN, &gWiFiCmd[5]);		
	}		
	else if(memcmp(gWiFiCmd, "+PAS+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_PAS_DATA, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+TLK++ON", 8) == 0)
	{	
		WiFi_SendCmdToMcu(AXX_TLK_ON, NULL);
		if(gSys.CurModuleID == MODULE_ID_RTC)
		{
			gSys.NextModuleID = MODULE_ID_WIFI;
			MsgSend(MSG_MODE);
		}
	}
	else if(memcmp(gWiFiCmd, "+TLK+OFF", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_TLK_OFF, NULL);	
	}
	else if(memcmp(gWiFiCmd, "+TLK+ENA", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_TLK_ENA, NULL);
	}
	else if(memcmp(gWiFiCmd, "+TLK+DIS", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_TLK_DIS, NULL);
	}
	else if(memcmp(gWiFiCmd, "+VIS+IDL", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_IDLE);
	}	
	else if((memcmp(gWiFiCmd, "+VIS+LSN", 8) == 0)
	     || (memcmp(gWiFiCmd, "+WAK++UP", 8) == 0))
	{
		TimeOutSet(&WiFiLedBlinkTimer, 0);
		WiFiSetMicState(WIFI_AVS_STATUS_LSN);
	}	
	else if(memcmp(gWiFiCmd, "+VIS+THK", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_THK);
	}
	else if(memcmp(gWiFiCmd, "+VIS+TTS", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_TTS);
	}
	else if(memcmp(gWiFiCmd, "+VIS+ERR", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_ERR);
	}
	else if(memcmp(gWiFiCmd, "+VIS+INF", 8) == 0)
	{
		gWiFi.VisInfState = TRUE;
	}
	else if(memcmp(gWiFiCmd, "+VIS-INF", 8) == 0)
	{
		gWiFi.VisInfState = FALSE;
	}
	else if(memcmp(gWiFiCmd, "+VIS+CAO", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_CAO);
	}
	else if(memcmp(gWiFiCmd, "+VIS+CAC", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_CAC);
	}
	else if(memcmp(gWiFiCmd, "+VIS+CAI", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_CAI);
	}
	else if(memcmp(gWiFiCmd, "+VIS+CAA", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_CAA);
	}
	else if(memcmp(gWiFiCmd, "+VIS+CAN", 8) == 0)
	{
		WiFiSetMicState(WIFI_AVS_STATUS_CAN);
	}
	//KAISHU 故事机命令
	else if(memcmp(gWiFiCmd, "+LGT++ON", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LGT__ON, NULL);
	}
	else if(memcmp(gWiFiCmd, "+LGT+OFF", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LGT_OFF, NULL);
	}
	else if(memcmp(gWiFiCmd, "+LCK++ON", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LCK__ON, NULL);
	}
	else if(memcmp(gWiFiCmd, "+LCK+OFF", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_LCK_OFF, NULL);
	}
	else if(memcmp(gWiFiCmd, "+VLM+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_VLM_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+RAD+", 5) == 0)
	{
		WiFi_SendCmdToMcu(AXX_RAD_XXX, &gWiFiCmd[5]);
	}
	else if(memcmp(gWiFiCmd, "+SLP++ON", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SLP__ON, NULL);
	}
	else if(memcmp(gWiFiCmd, "+SLP+OFF", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_SLP_OFF, NULL);
	}
	//凯叔命令end
#ifdef FUNC_WIFI_UART_UPGRADE
	else if(memcmp(gWiFiCmd, "+MCU+VER", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_VER, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+MCU+UPD", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_UPD, NULL);		
	}
	else if(memcmp(gWiFiCmd, "+MCU+RUN", 8) == 0)
	{
		WiFi_SendCmdToMcu(AXX_MCU_RUN, NULL);		
	}
#endif
#ifdef FUNC_WIFI_BT_CONTROL_EN
	else
	{
		WiFi_CtrlBtCmdProcess(&gWiFiCmd[0]);
	}
#endif
	
#ifdef FUNC_SLEEP_EN
	gSys.SleepTimeCnt = 0;
	gSys.SleepStartPowerOff = FALSE;
#endif
	
#ifdef FUNC_SLEEP_LEDOFF_EN
	gSys.SleepLedOffCnt = FALSE;
	gSys.SleepLedOffFlag = FALSE;
#endif
	APP_DBG("Rcv gWiFiCmd is:%s\n", gWiFiCmd);

	if(WiFiDataRcvStartFlag != TRUE)
	{
		memset(gWiFiCmd, 0, sizeof(gWiFiCmd));
	}
}

#endif

