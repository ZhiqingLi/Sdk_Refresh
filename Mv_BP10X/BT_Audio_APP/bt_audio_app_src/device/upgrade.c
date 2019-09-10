#include <stdio.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "type.h"
#include "watchdog.h"
#include "irqn.h"
#include "remap.h"
#include "core_d1088.h"
#include "flash_boot.h"
#include "mode_switch_api.h"
#include "sys.h"
#if FLASH_BOOT_EN



#define USER_CODE_RUN_START		0
#define	USER_CODE_RUN_RESTART	1
#define NO_DEVICE_LINK			2
#define NO_UDISK_LINK			3
#define NO_SDCARD_LINK			4
#define NO_PC_LINK				5
#define PC_MVA_ERROR			6
#define PC_UPGRADE_OK			7

#define MVA_CODE_LEN_ERROR		8
#define MVA_CODE_MAGIC_ERROR	9
#define MVA_BOOT_LEN_ERROR		10
#define MVA_BOOT_LEN_ERROR1		11
#define MVA_CODE_KEY_ERROR		12
#define MVA_CODE_CRC_ERROR		13
#define MVA_CONST_LEN_ERROR		14
#define MVA_CONST_OFFSET_ERROR	15
#define CODE_CONST_UPGADE_OK	16
#define CONST_UPGADE_ERROR		17
#define CODE_UPGADE_ERROR		18
#define CODE_CONST_UPGADE_ERROR	19
#define MVA_PAKET_ERROR			20
#define MVA_VERSION_ERROR		21
#define SDCARD_FS_ERROR			22
#define UDISK_FS_ERROR			23
#define FS_OPEN_MVA_ERROR		24

#define     ADR_FLASH_BOOT_FLAGE                                             (0x4000100C)
typedef struct _ST_FLASH_BOOT_FLAGE {
	volatile  unsigned long UDisk                      :  1; /**< enable U盘 mode  */
	volatile  unsigned long PC                         :  1; /**< enable PC升级 mode  */
	volatile  unsigned long sdcard                     :  2; /**< enable SD卡 mode  */
	volatile  unsigned long bt                         :  1; /**< bt  */
	volatile  unsigned long updata                     :  1; /**< 保留其他  */
	volatile  unsigned long flag                       :  2; /**< enable  */
	volatile  unsigned long RSV                        :  1; /**< 保留  */
	volatile  unsigned long ERROR_CODE                 :  8; /**< error code  */
	volatile  unsigned long POR_CODE                   :  8; /**< error code  */

} ST_FLASH_BOOT_FLAGE __ATTRIBUTE__(BITBAND);
#define SREG_FLASH_BOOT_FLAGE                    (*(volatile ST_FLASH_BOOT_FLAGE *) ADR_FLASH_BOOT_FLAGE)

//void report_up_grate()
//{
//	if(SREG_FLASH_BOOT_FLAGE.ERROR_CODE == USER_CODE_RUN_START)
//	{
//		DBG("正常启动\n");
//	}
//	else if(SREG_FLASH_BOOT_FLAGE.ERROR_CODE == USER_CODE_RUN_RESTART)
//	{
//		DBG("正常启动\n");
//	}
//	else if(SREG_FLASH_BOOT_FLAGE.ERROR_CODE == PC_UPGRADE_OK)
//	{
//		DBG("pc 升级OK\n");
//	}
//	else if(SREG_FLASH_BOOT_FLAGE.ERROR_CODE == CODE_CONST_UPGADE_OK)
//	{
//		DBG("U/sd升级OK\n");
//	}
//	else
//	{
//		DBG("error code %lu\n", (uint32_t)SREG_FLASH_BOOT_FLAGE.ERROR_CODE);
//	}
//}
//
//uint8_t Reset_FlagGet_Flash_Boot(void)
//{
//	return (uint8_t)SREG_FLASH_BOOT_FLAGE.POR_CODE;
//}

void DataCacheInvalidAll(void);//core_d1088.c
void start_up_grate(uint32_t UpdateResource)
{
	int i;
	uint32_t temp = 0;
	typedef void (*fun)();
	fun jump_fun;

	*(uint32_t *)ADR_FLASH_BOOT_FLAGE = 0;
	if(ResourceValue(AppResourceCard) && (UpdateResource == AppResourceCard))
	{
		//挂载检测指定的mva包存在，
		SREG_FLASH_BOOT_FLAGE.updata = 1;//升级使能位
		SREG_FLASH_BOOT_FLAGE.sdcard = 2;
	}
	else if(ResourceValue(AppResourceUDisk) && (UpdateResource == AppResourceUDisk))
	{
		//挂载检测指定的mva包存在，
		SREG_FLASH_BOOT_FLAGE.updata = 1;//升级使能位
		SREG_FLASH_BOOT_FLAGE.UDisk = 1;
	}
	else if(ResourceValue(AppResourceUsbDevice) && (UpdateResource == AppResourceUsbDevice))
	{
		//检测PC升级的连接有效性。
		SREG_FLASH_BOOT_FLAGE.PC = 1;//pc升级
		SREG_FLASH_BOOT_FLAGE.updata = 1;//升级使能位
	}

	if(SREG_FLASH_BOOT_FLAGE.updata)
	{
		temp = *(uint32_t *)ADR_FLASH_BOOT_FLAGE;
		DBG("start_up_grate0...................");
		jump_fun = (fun)0;
		WDG_Enable(WDG_STEP_1S);
		DisableIDCache();	//关闭cache
		DataCacheInvalidAll();//清除D cache
		SysTickDeInit();
		SysTimerIntFlagClear();
		GIE_DISABLE();
		
		//UART
		GPIO_PortAModeSet(GPIOA0, 0);
		GPIO_PortBModeSet(GPIOA1, 0);
		GPIO_PortAModeSet(GPIOA6, 0);
		GPIO_PortBModeSet(GPIOA19, 0);
		GPIO_PortBModeSet(GPIOA25, 0);
		//DMA
		*(uint32_t *)0x4000D100 = 0;
		for(i=0x4000D000;i<0x4000D104;)
		{
			*(uint32_t *)i = 0;
			i=i+4;
		}

		*(uint32_t *)0x40022000 &= ~0x77FFF8;//REG复位 flash aupll not reset
		*(uint32_t *)0x40022000 |= 0x7FFFF8;

		*(uint32_t *)0x40022004 &= ~0x7FFFF7FF;//fun reset  flash not reset
		*(uint32_t *)0x40022004 |= 0x7FFFF7FF;


		*(uint32_t *)ADR_FLASH_BOOT_FLAGE = temp;
		__nds32__mtsr(0, NDS32_SR_INT_MASK2);//中断使能位清零
		__nds32__mtsr(__nds32__mfsr(NDS32_SR_HSP_CTL) & 0, NDS32_SR_HSP_CTL);
		__asm("NOP");
		jump_fun();
		while(1);
	}
}

#endif
