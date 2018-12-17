/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/



#ifndef _OS_PORTING_H_
#define _OS_PORTING_H_
#include <ssv_types.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>

/*
* For MV's EVB Board, Uart & Interrupt config as following:
* UART_TX  ----  GP_A0
* UART_RX  ----  GP_A1
* INT      ----  GP_C3
*/

#define LINUX_SIM   0xFFFF
#define HOST_PLATFORM_SEL LINUX_SIM

/*============Task Priority===================*/

#define RX_ISR_PRIORITY    3
#define CMD_ENG_PRIORITY        3
#define WIFI_TX_PRIORITY        3
#define WIFI_RX_PRIORITY        3
#define TCPIP_PRIORITY          2
#define NETAPP_PRIORITY         2
#define NETAPP_PRIORITY_1         2
#define NETAPP_PRIORITY_2         2
#define DHCPD_PRIORITY            2
#define NETMGR_PRIORITY        2

#define TMR_TASK_PRIORITY         1
#define MLME_TASK_PRIORITY      2
#define TASK_END_PRIO          2


/*============Console setting===================*/
#define PRINTF LOG_PRINTF
#define FFLUSH(x)

#define hal_print printf

extern int hal_putchar(int c);

extern u8 hal_getchar(void);

/*============Compiler setting===================*/
#define ARM_ADS
#define STRUCT_PACKED __attribute__ ((packed))
#define UNION_PACKED __attribute__ ((packed))
//LWIP PACK Definition
#define PACK_STRUCT_BEGIN   //#pragma pack(1)
#define PACK_STRUCT_END     //#pragma pack()
#define PACK_STRUCT_STRUCT     __attribute__ ((packed))
#define PACK(__Declaration__) __Declaration__ PACK_STRUCT_STRUCT;
#define PACK_STRUCT_FIELD(x)    x
#define inline __inline
#define ALIGN_ARRAY(a) __attribute__ ((aligned(a)))
#define MV_AP80A0
#define EXT_RX_INT
#define OS_NO_SUPPORT_PROFILING
#define NO_NEED_ALIGN_BYTE    


/* sdio rx int, use card int trigger, pulldown/pullup */
#define SDIO_CARD_INT_TRIGGER


/*============SSV-DRV setting===================*/
#define INTERFACE "sdio"
#define	CONFIG_RX_POLL      1
#define SDRV_INCLUDE_SDIO   1

/*============Stack Size (unint: 16bytes)===================*/
#define TMR_TASK_STACK_SIZE   8   //32
#define MLME_TASK_STACK_SIZE  0
#define CMD_ENG_STACK_SIZE    32       // 64
#define TCPIP_STACK_SIZE      64       //128
#define DHCPD_STACK_SIZE      32       //64
#define NETAPP1_STACK_SIZE    0        //32  // 128
#define NETAPP2_STACK_SIZE    0        //128
#define NETAPP3_STACK_SIZE    0        // 128
#define NETMGR_STACK_SIZE     32       //64
#define CLI_TASK_STACK_SIZE   32       //64
#define RX_ISR_STACK_SIZE     0        // 64
#define WIFI_RX_STACK_SIZE    32       //64
#define WIFI_TX_STACK_SIZE    32      //64
#define DEV_INIT_STACK_SIZE   16      //64

#define PING_THREAD_STACK_SIZE 0 //16 , ping thread doesn't enable now, I set staic size is zero to reduce data size.

#define TOTAL_STACK_SIZE (TMR_TASK_STACK_SIZE+ \
                          MLME_TASK_STACK_SIZE+ \
                          CMD_ENG_STACK_SIZE+ \
                          TCPIP_STACK_SIZE+ \
                          DHCPD_STACK_SIZE+ \
                          NETAPP1_STACK_SIZE+ \
						  NETAPP2_STACK_SIZE+ \
						  NETAPP3_STACK_SIZE+ \
                          NETMGR_STACK_SIZE+ \
                          CLI_TASK_STACK_SIZE+ \
						  RX_ISR_STACK_SIZE+ \
						  WIFI_RX_STACK_SIZE+ \
                          WIFI_TX_STACK_SIZE+ \
                          DEV_INIT_STACK_SIZE+ \
                          PING_THREAD_STACK_SIZE)
                          
/*============Memory========================*/
#ifndef __MTRACE_CHECK__
OS_APIs void *OS_MemAlloc( u32 size );
OS_APIs void __OS_MemFree( void *m );
#endif//#__MTRACE_CHECK__
OS_APIs void OS_MemSET(void *pdest, u8 byte, u32 size);
OS_APIs void OS_MemCPY(void *pdest, const void *psrc, u32 size);
/*=========================================*/
void platform_ldo_en_pin_init(void);
void platform_ldo_en(bool en);


/*wifi interface port
 *
 * sdio			pin
 * ldo_en		pin
 * interrupt	pin
 *
 * */
/*
#define SDIO_BUS_PORT			2
#define SDIO_PORT(X)			GPIO_B_##X
#define SDIO_PIN				(GPIOB27|GPIOB28|GPIOB29)

#define SDIO_INT_PORT(X)	GPIO_C_##X
#define SDIO_INT_PIN		GPIOC2

#define LDO_EN_PORT(X)	GPIO_C_##X
#define LDO_EN_PIN		GPIOC0
*/

#define SDIO_BUS_PORT			2  //0
#define SDIO_PORT(X)			GPIO_B_##X
#define SDIO_PIN				(GPIOB27|GPIOB28|GPIOB29)//(GPIOB0|GPIOB1|GPIOB2)

#define SDIO_INT_PORT(X)	GPIO_C_##X
#define SDIO_INT_PIN		GPIOC7

#define LDO_EN_PORT(X)	    GPIO_C_##X
#define LDO_EN_PIN		    GPIOC0


#endif
