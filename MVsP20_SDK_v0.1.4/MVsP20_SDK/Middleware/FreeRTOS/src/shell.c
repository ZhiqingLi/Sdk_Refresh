#include <stdio.h>
#include <nds32_intrinsic.h>
#include <string.h>
#include "type.h"
#include "uarts_interface.h"
#include "debug.h"
#include "irqn.h"
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"

#define BUFSIZE		32
uint8_t	UARTBuffer[BUFSIZE];
uint32_t	gRxQueueHead = 0;
uint32_t	gRxQueueTail = 0;

#define CMD_ARG_MAX_COUNT	3
#define ONE_ARG_MAX_LEN		10
char cmdbuffer[CMD_ARG_MAX_COUNT][ONE_ARG_MAX_LEN];

//多出ONE_ARG_MAX_LEN,目的有参数之间的间隔' '，还有cmd长度可以大于ONE_ARG_MAX_LEN,因此多出ONE_ARG_MAX_LEN长度预留
uint8_t shellbuffer[(CMD_ARG_MAX_COUNT+1)*ONE_ARG_MAX_LEN];

const int num_tab[10] = {	1000000000,
							100000000,
							10000000,
							1000000,
							100000,
							10000,
							1000,
							100,
							10,
							1};



int char_to_num(char *buffer)
{
    int i;
    int templen;
    int sum = 0;
    templen = strlen(buffer);
    if(templen > 10)
    {
        DBG("CMD ERROR\n");
        return -1;
    }
    for(i=0;i<templen;i++)
    {
        buffer[i] -= 0x30;
        if((buffer[i]>=0)&&(buffer[i]<=9))
        {
        	//DBG("%x",buffer[i]);
        }
        else
        {
        	DBG("ERROR");
            return -1;
        }
    }
    int j = 0;
    for(i=0;i<templen;i++)
    {
        sum += buffer[i]*num_tab[10 - templen + j];
        j++;
    }
    return sum;
}

uint8_t shell_getchar(void)
{
	if(gRxQueueHead != gRxQueueTail)//非空
	{
		unsigned char	val;
		val = UARTBuffer[gRxQueueHead];
		gRxQueueHead = ((gRxQueueHead + 1) % BUFSIZE);
		return val;
	}
	return 0;
}

void UART0_Interrupt(void)
{
	if(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 1) & 0x01) { //接收中断
		int i;
		uint8_t Temp[0];
		for(i=0;i<4;i++)
		{
			if(UARTS_RecvByte(UART_PORT0,Temp) == 0)
			{
				break;
			}
			else
			{
				UARTBuffer[gRxQueueTail] = Temp[0];
				if (((gRxQueueTail + 1) % BUFSIZE) != gRxQueueHead)		//queue is not full
				{
					gRxQueueTail = ((gRxQueueTail + 1) % BUFSIZE);			//queue tail++
				}
			}
		}
		UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXINT_CLR, 1);
	}
    if(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x3C) { //错误中断
        //kprintf("Uart0 RX error(0x%02X)\n", (uint32_t)(UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXSTAT_GET, 0) & 0x3C));
        UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXINT_CLR, 1);
    }

    if(UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXSTAT_GET, 0)) { //发送中断
        UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXINT_CLR, 1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cmd_test(int argc, char *argv[]);
static void DisplayMemInf(void);
static void DisplayRunTimeStatesInf(void);
static void DisplayTaskInf(void);
static void Displayhelp(void);
extern void clear_net_cfg(void);
extern void creat_wifi_connect_task(void);
extern void edp_task_start();
extern void mqtt_run();
shell_cmds mv_shell[] =
{
    { "test",   				cmd_test,       			"for test"},
    { "displaytaskinfo",   		DisplayTaskInf,       		"show task info"},
    { "displayruntimestats",   	DisplayRunTimeStatesInf,	"show run time states"},
    { "displaymeminfo",   		DisplayMemInf,				"show heap memory size"},
    { "clearnetcfg",   			clear_net_cfg,				"clear net config"},
    { "connectwifi",   			creat_wifi_connect_task,			"link wifi"},
    { "edprun",   				edp_task_start,				"edp_test"},
    { "mqttrun",   				mqtt_run,				"mqtt_test"},
    { "help",   				Displayhelp,				"help"},
    { NULL, NULL, NULL },
};

void cmd_test(int argc, char *argv[])
{
	int i;
	for(i=0;i<argc;i++)
	{
		if(i==0)
		{
			DBG("%s\n",argv[i]);
		}
		else
		{
			DBG("%d\n",char_to_num(argv[i]));
		}
	}
}


static void DisplayMemInf()
{
	DBG("\nRemainingMemoryInBytes: %d\r\n",xPortGetFreeHeapSize());
}

static void DisplayRunTimeStatesInf(void)
{
	uint8_t *buf= pvPortMalloc(4096);
	memset(buf,0,4096);
	vTaskGetRunTimeStats(buf);
	vPortFree(buf);
	DBG("\nTask           \tAbs Time\tPercent\r\n****************************************\r\n");
	DBG("%s\n",buf);
}

static void DisplayTaskInf(void)
{
	uint8_t *buf= pvPortMalloc(4096);
	memset(buf,0,4096);
	vTaskList(buf);
	vPortFree(buf);
	DBG("\nTask           State  Prio+3  FreeStack(word)  PID\n***************************************************\r\n");
	DBG("%s\n",buf);
}


static void Displayhelp(void)
{
	int i=0;
	DBG("\n***************************************************\nmv shell cmd list:\n");
	DBG("\tcmd\t\thelpinfo\t\n");
	while(1)
	{
		if(shellbuffer,mv_shell[i].Cmd == NULL)
		{
			break;
		}
		DBG("\t%s\t\t%s\t\n",mv_shell[i].Cmd,mv_shell[i].CmdUsage);
		i++;
	}
	DBG("cmd example:test 123 456\n");
	DBG("***************************************************\n");
}


void cmd_proess()
{
	int i = 0;
	int j = 0;
	int k = 0;
	int ii = 0;
	int argc=0;
	char *argv[CMD_ARG_MAX_COUNT];
	char temp[ONE_ARG_MAX_LEN];
	while(1)
	{
		if(mv_shell[i].Cmd == NULL)
		{
			if(j==0)
			{
				DBG("未知命令\n");
			}
			return;
		}
		if(memcmp(shellbuffer,mv_shell[i].Cmd,strlen(mv_shell[i].Cmd)) == 0)
		{
			argc=0;
			ii = 0;
			memset(temp,0,sizeof(temp));
			for(k=0;k<strlen(shellbuffer);k++)
			{
				if((shellbuffer[k] == 0x20)||(shellbuffer[k] == 0x0D))
				{
					memcpy(cmdbuffer+argc,temp,sizeof(temp));
					memset(temp,0,sizeof(temp));
					argv[argc] = (char*)(cmdbuffer+argc);
					ii = 0;
					argc++;
					if(argc == CMD_ARG_MAX_COUNT)
					{
						break;
					}
				}
				else
				{
					temp[ii] = shellbuffer[k];
					ii++;
					if(argc >= ONE_ARG_MAX_LEN)
					{
						ii=10;
					}
				}
			}
			mv_shell[i].CmdHandler(argc,argv);
			j++;
		}
		i++;
	}
}


void shell_init(void)
{
    UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXFIFO_CLR, 1);
    UARTS_IOCTL(UART_PORT0, UART_IOCTL_TXFIFO_CLR, 1);
    UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXINT_SET, 1);
    UARTS_IOCTL(UART_PORT0, UART_IOCTL_RXFIFO_TRGRDEP_SET, 0);
    memset(UARTBuffer,0,sizeof(UARTBuffer));
    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn,0);
}


void mv_shell_task(void)
{
	uint8_t count = 0;
	uint8_t cmdstr;
	DBG("mv_shell_task STSRT\n");
	while(1)
	{
		cmdstr = shell_getchar();
		if(cmdstr)
		{
			DBG("%c",cmdstr);
			shellbuffer[count] = cmdstr;
			if((cmdstr == 0x0A)||(cmdstr == 0x0D))
			{
				DBG("\n");
				cmd_proess();
				count = 0;
				memset(shellbuffer,0,100);
			}
			else
			{
				count++;
			}
		}
		vTaskDelay(30);
	}
}
