#include <string.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "irqn.h"
#include "cache_tcm.h"
#include "timeout.h"
#include "debug.h"
#include "delay.h"
#include "timeout.h"
#include "watchdog.h"
#include "gpio.h"
#include "clk.h"
#include "uarts_interface.h"
#include "spi_flash.h"


#define UART_TX_IO	0
#define UART_RX_IO  0


void OutputExample(void)
{
	uint8_t					recvBuf;

	DBG("\nOutputExample\n");
	DBG("Input 'x' to exit this example\n");
	DBG("Input 'h' to output high \n");
	DBG("Input 'l' to output low \n");
	DBG("Input '0' to output Driving Strength Select 8mA (default value) \n");
	DBG("Input '1' to output Driving Strength Select 24mA \n");

	GPIO_RegOneBitClear(GPIO_A_IE, GPIOA2);

	GPIO_RegOneBitSet(GPIO_A_OE, GPIOA2);

	GPIO_RegOneBitSet(GPIO_A_OUT, GPIOA2);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == 'h')
		{
			DBG("A2 output High\n");
			GPIO_RegOneBitSet(GPIO_A_OUT, GPIOA2);
		}
		else if(recvBuf == 'l')
		{
			DBG("A2 output Low\n");
			GPIO_RegOneBitClear(GPIO_A_OUT, GPIOA2);
		}
		else if(recvBuf == '0')
		{
			DBG("Driving Strength Select 8mA\n");
			GPIO_RegOneBitClear(GPIO_A_OUTDS, GPIOA2);
		}
		else if(recvBuf == '1')
		{
			DBG("Driving Strength Select 24mA\n");
			GPIO_RegOneBitSet(GPIO_A_OUTDS, GPIOA2);
		}
	}
}

void InputExample(void)
{
	uint8_t					recvBuf;
	uint8_t					oldVal = 0;
	uint8_t					newVal = 0;

	DBG("\nInputExample\n");
	DBG("Input 'x' to exit this example\n");

	// A3 是 Input 脚
	GPIO_RegOneBitSet(GPIO_A_IE, GPIOA3);

	GPIO_RegOneBitClear(GPIO_A_OE, GPIOA3);

	// A2 是 Output 脚
	GPIO_RegOneBitClear(GPIO_A_IE, GPIOA2);

	GPIO_RegOneBitSet(GPIO_A_OE, GPIOA2);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		newVal = GPIO_RegOneBitGet(GPIO_A_IN, GPIOA3);
		if(newVal != oldVal)
		{
			if(newVal == 1)
			{
				DBG("A3 Get value is High\n");
			}
			else
			{
				DBG("A3 Get value is Low\n");
			}
			oldVal = newVal;
		}

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == 'h')
		{
			DBG("A2 High\n");
			GPIO_RegOneBitSet(GPIO_A_OUT, GPIOA2);
		}
		else if(recvBuf == 'l')
		{
			DBG("A2 Low\n");
			GPIO_RegOneBitClear(GPIO_A_OUT, GPIOA2);
		}
	}
}



void InterruptExample(void)
{

	uint8_t				recvBuf;
	uint8_t				triggerType = GPIO_NEG_EDGE_TRIGGER;

	DBG("\nInterruptExample\n");


	NVIC_EnableIRQ(GPIO_IRQn);

RESTART_I:

	// A4 为中断检测脚
	DBG("triggerType = %d\n", triggerType);
	GPIO_INTEnable(GPIO_A_INT, GPIOA4, triggerType);

	// A2 是 Output 脚
	GPIO_RegOneBitClear(GPIO_A_IE, GPIOA2);

	GPIO_RegOneBitSet(GPIO_A_OE, GPIOA2);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		if(recvBuf == 'x')
		{
			NVIC_DisableIRQ(GPIO_IRQn);
			break;
		}
		else if(recvBuf == 'h')
		{
			DBG("A2 High\n");
			GPIO_RegOneBitSet(GPIO_A_OUT, GPIOA2);
		}
		else if(recvBuf == 'l')
		{
			DBG("A2 Low\n");
			GPIO_RegOneBitClear(GPIO_A_OUT, GPIOA2);
		}
		else if(recvBuf == 'p')
		{
			// posedge trigger
			DBG("\nposedge trigger\n");
			triggerType = GPIO_POS_EDGE_TRIGGER;
			goto RESTART_I;
		}
		else if(recvBuf == 'n')
		{
			//negedge trigger
			DBG("\negedge trigger\n");
			triggerType = GPIO_NEG_EDGE_TRIGGER;
			goto RESTART_I;
		}
		else if(recvBuf == 'e')
		{
			//edge trigger
			DBG("\edge trigger\n");
			triggerType = GPIO_EDGE_TRIGGER;
			goto RESTART_I;
		}
	}
}

void PullUpDownExample(void)
{
	uint8_t					recvBuf;

	DBG("\nPullUpDownExample\n");
	DBG("Input 'x' to exit this example\n");
	DBG("Input '1' to Pull up 20uA\n");
	DBG("Input '2' to Pull down 20uA\n");
	DBG("Input '3' to Pull up 70uA\n");
	DBG("Input '4' to Pull down 70uA\n");

	// A3 是 Input 脚
	GPIO_RegOneBitSet(GPIO_A_IE, GPIOA3);
	GPIO_RegOneBitClear(GPIO_A_OE, GPIOA3);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == '1')
		{
			DBG("Pull up 20uA\n");
			GPIO_RegOneBitClear(GPIO_A_DS, GPIOA3);
			GPIO_RegOneBitClear(GPIO_A_PU, GPIOA3);
			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA3);
		}
		else if(recvBuf == '2')
		{
			DBG("Pull down 20uA\n");
			GPIO_RegOneBitClear(GPIO_A_DS, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PD, GPIOA3);
		}
		else if(recvBuf == '3')
		{
			DBG("Pull up 70uA\n");
			GPIO_RegOneBitSet(GPIO_A_DS, GPIOA3);
			GPIO_RegOneBitClear(GPIO_A_PU, GPIOA3);
			GPIO_RegOneBitClear(GPIO_A_PD, GPIOA3);
		}
		else if(recvBuf == '4')
		{
			DBG("Pull down 70uA\n");
			GPIO_RegOneBitSet(GPIO_A_DS, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PU, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PD, GPIOA3);
		}
	}
}

void PullDownCurrentExample(void)
{
	uint8_t					recvBuf;

	DBG("\nPullDownCurrentExample\n");
	DBG("Input 'x' to exit this example\n");
	DBG("Input '1' to Pull down current 1.7mA\n");
	DBG("Input '2' to Pull down current 2.4mA\n");
	DBG("Input '3' to Pull down current 4.1mA\n");


	GPIO_RegOneBitClear(GPIO_A_IE, GPIOA3);
	GPIO_RegOneBitClear(GPIO_A_OE, GPIOA3);
	GPIO_RegOneBitClear(GPIO_A_PU, GPIOA3);
	GPIO_RegOneBitClear(GPIO_A_PD, GPIOA3);

	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		if(recvBuf == 'x')
		{
			break;
		}
		else if(recvBuf == '1')
		{
			DBG("Pull down current 1.7mA\n");
			GPIO_RegOneBitSet(GPIO_A_PULLDOWN1, GPIOA3);
			GPIO_RegOneBitClear(GPIO_A_PULLDOWN2, GPIOA3);
		}
		else if(recvBuf == '2')
		{
			DBG("Pull down current 2.4mA\n");
			GPIO_RegOneBitClear(GPIO_A_PULLDOWN1, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PULLDOWN2, GPIOA3);
		}
		else if(recvBuf == '3')
		{
			DBG("Pull down current 4.1mA\n");
			GPIO_RegOneBitSet(GPIO_A_PULLDOWN1, GPIOA3);
			GPIO_RegOneBitSet(GPIO_A_PULLDOWN2, GPIOA3);
		}
	}
}


int main(void)
{
	uint8_t			recvBuf;

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
	 
	 
	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);
	 
	WDG_Disable();


RESTART:
	DBG("-------------------------------------------------------\n");
	DBG("GPIO Example:\n");
	DBG("o:  Output Example\n");
	DBG("i:  Input Example\n");
	DBG("t:  Interrupt Example\n");
	DBG("p:  Pull up , Pull down  Example\n");
	DBG("d:  Pull-down-current  Example\n");
	DBG("-------------------------------------------------------\n");





	while(1)
	{
		recvBuf = 0;
		UARTS_Recv(UART_RX_IO, &recvBuf, 1);

		if(recvBuf == 'o')
		{
			// output example
			OutputExample();
			goto RESTART;
		}
		else if(recvBuf == 'i')
		{
			// input example
			InputExample();
			goto RESTART;
		}
		else if(recvBuf == 't')
		{
			// interrupt example
			InterruptExample();
			goto RESTART;
		}
		else if(recvBuf == 'p')
		{
			// pull up / pu down example
			PullUpDownExample();
			goto RESTART;
		}
		else if(recvBuf == 'd')
		{
			// pull-down-current example
			PullDownCurrentExample();
			goto RESTART;
		}


	}

	return 0;
}

uint32_t gpioPinIndex[3] = {0,0,0};
__attribute__((section(".driver.isr"), weak)) void GpioInterrupt(void)
{

	gpioPinIndex[0] = GPIO_INTFlagGet(GPIO_A_INT);
	if(gpioPinIndex[0])
	{
		DBG("GpioA Int Index:0x%08x\n",gpioPinIndex[0]);
		GPIO_INTFlagClear(GPIO_A_INT,gpioPinIndex[0]);
	}

	gpioPinIndex[1] = GPIO_INTFlagGet(GPIO_B_INT);
	if(gpioPinIndex[1])
	{
		DBG("GpioB Int Index:0x%08x\n",gpioPinIndex[1]);
		GPIO_INTFlagClear(GPIO_B_INT,gpioPinIndex[1]);
	}


	gpioPinIndex[2] = GPIO_INTFlagGet(GPIO_C_INT);
	if(gpioPinIndex[2])
	{
		DBG("GpioC Int Index:0x%08x\n",gpioPinIndex[2]);
		GPIO_INTFlagClear(GPIO_C_INT,gpioPinIndex[2]);
	}
}
