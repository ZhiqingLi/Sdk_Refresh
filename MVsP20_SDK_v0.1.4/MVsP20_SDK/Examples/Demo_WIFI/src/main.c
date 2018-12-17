#include <stdio.h>
#include <nds32_intrinsic.h>
#include "cache_tcm.h"
#include "gpio.h"
#include "uarts.h"
#include "watchdog.h"
#include "uarts_interface.h"
#include "type.h"
#include "debug.h"
#include "irqn.h"
#include "spi_flash.h"
#include "timeout.h"
#include "clk.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "type.h"
#include "gpio.h"
#include "net_mgr.h"

#include "inc/wifi_api.h"

#define SET_LP_ROM_REG(x) \
do{		\
	REG_LP_ACCESS_PROTECT = 0xFF;	\
	REG_LP_USER_DEFINED_REG0 = x;	\
	REG_LP_ACCESS_PROTECT = 0x0;	\
}while(0);

extern int16_t APmode_config(void);

extern void DebugUartInit();
uint8_t SDIO_DmaChannelMap[27] =
{
		255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
		255,//PERIPHERAL_ID_SPIS_TX,		//1
		0,//PERIPHERAL_ID_SD_RX,			//2
		1,//PERIPHERAL_ID_SD_TX,			//3
		255,//PERIPHERAL_ID_SPIM0_RX,		//4
		255,//PERIPHERAL_ID_SPIM0_TX,		//5
		255,//PERIPHERAL_ID_SPIM1_RX,		//6
		255,//PERIPHERAL_ID_SPIM1_TX,		//7
		255,//PERIPHERAL_ID_UART0_RX,		//8
		255,//PERIPHERAL_ID_UART1_RX,		//9
		255,//PERIPHERAL_ID_UART2_RX,		//10
		255,//PERIPHERAL_ID_TIMER3,			//11
		255,//PERIPHERAL_ID_TIMER4,			//12
		255,//PERIPHERAL_ID_I2S_TX,			//13
		255,//PERIPHERAL_ID_I2S_RX,			//14
		255,//PERIPHERAL_ID_SARADC,			//15
		255,//PERIPHERAL_ID_UART0_TX,		//16
		255,//PERIPHERAL_ID_UART1_TX,		//17
		255,//PERIPHERAL_ID_UART2_TX,		//18
		255,//PERIPHERAL_ID_TIMER5,			//19
		255,//PERIPHERAL_ID_TIMER6,			//20
		255,//PERIPHERAL_ID_TIMER7,			//21
		255,//PERIPHERAL_ID_TIMER8,			//22
		255,//PERIPHERAL_ID_LACC_SRC,		//23
		255,//PERIPHERAL_ID_LACC_DST,		//24
		255,//PERIPHERAL_ID_LACC_DSTRD,     //25
		255,//PERIPHERAL_ID_SOFTWARE,		//26
};
void prvInitialiseHeap(void)
{
	extern char _end;
	extern void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );
	HeapRegion_t xHeapRegions[2];

	xHeapRegions[0].pucStartAddress = (uint8_t*)&_end;
	xHeapRegions[0].xSizeInBytes = 0x20030000+16*1024-(uint32_t)&_end;

	DBG("xHeapRegions[0].xSizeInBytes = %d\n",(int)xHeapRegions[0].xSizeInBytes);

	xHeapRegions[1].pucStartAddress = NULL;
	xHeapRegions[1].xSizeInBytes = 0;

	vPortDefineHeapRegions( (HeapRegion_t *)xHeapRegions );
}

void InitZiArea(void)
{
	/* Use compiler builtin and memset */
	#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char _end;
	extern char __bss_start;
	int size;

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
	return;
}



#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
void DispRunTimeStatesInf(void)
{
	char *buf= pvPortMalloc(4096);
	memset(buf,0,4096);
	vTaskGetRunTimeStats(buf);
	DBG("\nTask           \tAbs Time\tPercent\r\n****************************************\r\n");
	DBG("%s\n",buf);
	vPortFree(buf);
}
#endif

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
void DispTaskInf(void)
{
	char *buf= pvPortMalloc(4096);
	memset(buf,0,4096);
	vTaskList(buf);
	DBG("\nTask           State  Prio+3  FreeStack(word)  PID\n***************************************************\r\n");
	DBG("%s\n",buf);
	vPortFree(buf);
}
#endif

static xTaskHandle task_wifi_init;
xTaskHandle wifi_co_task=NULL;
extern int ssv6xxx_dev_init(int argc,char * argv [ ]);

extern uint8_t scan_falg;
extern bool g_cli_joining;
uint8_t wifi_link_flage = 0;
uint32_t wifi_leave=0;
uint32_t over_time_cnt = 0;

void clear_net_cfg(void)
{
	SpiFlashErase(SECTOR_ERASE,(0x200000 - 4096)/4096,0);
}


void save_net_info(uint8_t *buffer)
{
	uint8_t *buf= pvPortMalloc(4096);
	if(SpiFlashRead(0x200000 - 4096, buf, 4096, 20) != FLASH_NONE_ERR)
	{
		vPortFree(buf);
		return ;
	}
	memcpy(buf+640,buffer,64);
	SpiFlashErase(SECTOR_ERASE,(0x200000 - 4096)/4096,0);
	if(SpiFlashWrite(0x200000 - 4096, buf, 4096, 20) != FLASH_NONE_ERR)
	{
		vPortFree(buf);
		return ;
	}
	vPortFree(buf);
	return ;
}

int get_net_info(uint8_t *buffer)
{
	if(SpiFlashRead(0x200000 - 4096 + 640, buffer, 64, 20) != FLASH_NONE_ERR)
	{
		return 0;
	}
	if( (buffer[0] == 'S') && (buffer[1] == 0x3A) && (buffer[32] == 'P') && (buffer[33] == 0x3A) )
	{
		return 1;
	}
	return 0;
}

uint32_t get_ipadress(void)
{
    ipinfo info;
    netmgr_ipinfo_get(WLAN_IFNAME, &info);
    return info.ipv4;
}

extern uint8_t sconfig_flag;
extern int16_t SCONFIGmode_default(void);
int smart_link_proess(void)
{
	vTaskDelay(1000);
	sconfig_flag = 0;
	vTaskDelay(100);
	if(SCONFIGmode_default() == 0)
	{
		DBG("SCONFIG mode success\n");
	}
	else
	{
		DBG("SCONFIG error\n");
		return 0;
	}
    while(1)
    {
    	if(sconfig_flag == 0)
		{
			vTaskDelay(500);
			DBG("Wait for smartlink\n");
		}
		else if(sconfig_flag == 1)
		{
			DBG("Smatlink success\n");
			return 1;
		}
		else if(sconfig_flag == 2)
		{
			DBG("Smartlink fail\n");
			return 0;
		}
    	vTaskDelay(100);
    }
}
extern int cmd_wifi_list(uint8_t *buffer);

#define READ_FLASH		1
#define WIFI_SCAN		2
#define AIRKISSS_START	3
#define WIFI_JOIN		4
#define WIFI_GET_IP		5
#define WIFI_LINK_OK	6
#define WIFI_INIT		7

uint32_t wifi_link_leave=1;
uint32_t wifi_link_ok=0;
uint32_t wifi_link_init=0;
#if 1
void wifi_connect_task()
{
	uint8_t *buffer = pvPortMalloc(64);
	uint8_t status = WIFI_INIT;
	uint32_t addr;
	uint32_t TryScanCount = 0;
	while(1)
	{
		switch(status)
		{
			case  WIFI_INIT:
				DBG("WIFI_INIT\n");
				if(wifi_link_init)
				{
					status = READ_FLASH;
					TryScanCount = 0;
					wifi_link_leave = 0;
				}
				else
				{
					DBG(".\n");
					vTaskDelay(500);
				}
				break;

			case READ_FLASH:
				DBG("READ_FLASH\n");
				if(get_net_info(buffer) == 1)
				{
					DBG("SSID: %s\n",buffer);
					DBG("PW  : %s\n",buffer+32);
					scan_falg = 0;
					cmd_wifi_scan();
					status = WIFI_SCAN;
				}
				else
				{
					status = AIRKISSS_START;
				}
				break;

			case WIFI_SCAN:
				DBG("WIFI_SCAN\n");
				if(scan_falg == 1)//scan done
				{
					if(cmd_wifi_list(buffer))
					{
						mv_wifi_join(buffer);
						g_cli_joining = 0;
						wifi_link_leave = 0;
						status = WIFI_JOIN;
					}
					else
					{
						if(TryScanCount == 5)
						{
							status = AIRKISSS_START;
						}
						else
						{
							TryScanCount++;
							status = READ_FLASH;
						}
					}
				}
				else if(scan_falg == 2)
				{
					DBG("scan error\n");
					status = READ_FLASH;
				}
				else
				{
					vTaskDelay(500);
					DBG("*");
				}
				break;

			case WIFI_JOIN:
				DBG("WIFI_JOIN\n");
				if(g_cli_joining == true)
				{
					status = WIFI_GET_IP;
					over_time_cnt = 0;
				}
				else
				{
					DBG("*\n");
					if(wifi_link_leave)
					{
						clear_net_cfg();//wifi password error
						status = AIRKISSS_START;
						over_time_cnt = 0;
					}
					vTaskDelay(100);
					if(over_time_cnt++ > 100)//加入路由器超时
					{
						over_time_cnt = 0;
						DBG("wifi_join timeout\n");
						status = AIRKISSS_START;
					}
				}
				break;

			case WIFI_GET_IP:
				DBG("WIFI_GET_IP\n");
				if(netif_status_get() == 0)
				{
					DBG(".\n");
					vTaskDelay(200);
				}
				else
				{
					wifi_link_leave = 0;
					DBG("Get ip success\n");
					addr=get_ipadress();
					DBG("IP=%d.%d.%d.%d\n",(int)(addr&0xFF),(int)(addr>>8)&0xFF,(int)(addr>>16)&0xFF,(int)(addr>>24)&0xFF);
					status = WIFI_LINK_OK;
				}
				break;

			case AIRKISSS_START:
				DBG("AIRKISSS_START\n");
				if(smart_link_proess() == 1)
				{
					while(1)
					{
						if(netif_status_get() == 0)
						{
							DBG(".\n");
							vTaskDelay(200);
						}
						else
						{
							DBG("Get ip success\n");
							addr=get_ipadress();
							DBG("IP=%d.%d.%d.%d\n",(int)(addr&0xFF),(int)(addr>>8)&0xFF,(int)(addr>>16)&0xFF,(int)(addr>>24)&0xFF);
							wifi_link_leave = 0;
							status = WIFI_LINK_OK;
							break;
						}
						if(g_cli_joining == 2)//join失败,如密码不正常
						{
							DBG("airkiss wifi_join failure\n");
							break;
						}
					}
				}

				break;

			case WIFI_LINK_OK:
				//DBG("WIFI_LINK_OK\n");
				if(wifi_link_leave)
				{
					DBG("wifi_leave\n");
					status = READ_FLASH;
				}
				else
				{
					//DBG(".\n");
					wifi_link_ok = 1;
				}
				vTaskDelay(200);
				break;
		}
	}
}
#endif

void creat_wifi_connect_task()
{
	xTaskCreate (wifi_connect_task, "wifi_connect_task", 512, NULL, 2, &wifi_co_task );
}

void edp_task_start()
{
	extern void edp_test();
	xTaskCreate (edp_test, "edp_tset_task", 512, NULL, 2,NULL);
}



void wifi_init (void *args)
{
	uint8_t wifi_mode = SSV6XXX_HWM_AP ; //SSV6XXX_HWM_AP SSV6XXX_HWM_STA
	OsDebugInit();
	vTaskDelay(10);

    DBG("ssv6xxx_dev_init\n");
    wifi_link_init = 0;
    ssv6xxx_dev_init(wifi_mode,NULL);//wifi模块初始化
    wifi_link_init = 1;


    if(wifi_mode == SSV6XXX_HWM_STA)
    {
		creat_wifi_connect_task();//wifi联接任务
		edp_task_start();//EDP通讯任务
    }


    vTaskDelay(1000);

	while(1)
	{
		vTaskDelay(1000);
		if(wifi_mode == SSV6XXX_HWM_AP)
		{
			http_task();
		}

//		DispRunTimeStatesInf();
//		DispTaskInf();
	}
}

SPI_FLASH_INFO flashinfo;
SPI_FLASH_INFO SpiFlashInfo;
extern void mv_shell_task(void);




int main()
{
	WDG_Disable();
#if  1    //OSC_12M
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB30);	//PU=0,PD=1:模拟
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB30);
	GPIO_RegOneBitClear(GPIO_B_PU, GPIOB31);	//PU=0,PD=1:模拟
	GPIO_RegOneBitSet(GPIO_B_PD, GPIOB31);

	Clock_Config(1, 12000000);//用12M时钟需要将B30、B31改为模拟输入
#else//32K
	Clock_Config(1, 32768);
#endif
	Clock_PllLock(480000);
	Clock_SysClkSelect(PLL_CLK_MODE);
	Clock_UARTClkSelect(1);
	Clock_ModuleEnable(ALL_MODULE_CLK_SWITCH);

	CacheTcmDeInit();
	CacheTcmInitFast(PHYMEM_16KBPM0 /*cache*/, PHYMEM_NONE/*tcm r0*/, 0/*tcm r0 offset*/, 4/*tcm r0 size*/, PHYMEM_NONE/*tcm r1*/, 0/*tcm r1 offset*/, 0/*tcm r1 size*/);


	DebugUartInit();

	DBG("****************************************************************\n");
	DBG("               MVsP2_wifi_Demo 2017 MVSilicon                   \n");
	DBG("****************************************************************\n");

	InitZiArea();
	prvInitialiseHeap();

	SpiFlashInit(90000000, 2, 0, 1);
	memcpy(&flashinfo,SpiFlashInfoGet(),sizeof(SPI_FLASH_INFO));
	GIE_ENABLE();
	NVIC_EnableIRQ(SWI_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_EnableIRQ(GPIO_IRQn);

	NVIC_SetPriority(SWI_IRQn,3);
	NVIC_SetPriority(TMR1_IRQn,2);
	NVIC_SetPriority(GPIO_IRQn,1);
	NVIC_SetPriority(UART0_IRQn,0);

	DMA_ChannelAllocTableSet(SDIO_DmaChannelMap);

	xTaskCreate (wifi_init, "wifi_init", 1024, NULL, 2, &task_wifi_init );

	vTaskStartScheduler();
	while(1);
}
