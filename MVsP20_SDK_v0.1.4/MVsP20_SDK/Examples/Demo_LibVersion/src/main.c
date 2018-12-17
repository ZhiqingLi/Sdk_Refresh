#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "rtc.h"
#include "rtc_interface.h"

#include "debug.h"
#include "irqn.h"
#include "uarts.h"
#include "uarts_interface.h"
#include "clk.h"
#include "gpio.h"
#include "watchdog.h"
#include "spi_flash.h"
#include "cache_tcm.h"



extern const unsigned char *GetLibVersionDriver(void);
extern const unsigned char *GetLibVersionFileSystem(void);
extern const unsigned char *GetLibVersionFlashFS(void);
extern const unsigned char *GetLibVersionMotor(void);
extern const unsigned char *GetLibVersionRtc(void);
extern const unsigned char *GetLibVersionWifi(void);


int main(void)
{

	WDG_Disable();
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
	GIE_ENABLE();


	GPIO_UartTxIoConfig(0, 1);//C2
	GPIO_UartRxIoConfig(0, 1);//C3
	UARTS_Init(0, 115200, 8, 0, 1);

	DBG("****************************************************************\n");
	DBG("*               MVsP2_Demo_LibVersion 2017 MVSilicon           *\n");
	DBG("****************************************************************\n");
	DBG("GetLibVersionDriver:        %s\n",GetLibVersionDriver());
	DBG("GetLibVersionFileSystem:    %s\n",GetLibVersionFileSystem());
	DBG("GetLibVersionFlashFS:       %s\n",GetLibVersionFlashFS());
	DBG("GetLibVersionMotor:         %s\n",GetLibVersionMotor());
	DBG("GetLibVersionRtc:           %s\n",GetLibVersionRtc());
	DBG("GetLibVersionWifi:          %s\n",GetLibVersionWifi());


	while(1);

}


