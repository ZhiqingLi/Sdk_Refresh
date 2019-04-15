/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : AsmPlatformconfig.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月15日
  最近修改   :
  功能描述   : ASM32芯片平台配置文件
  函数列表   :
              fputc
              Usart_Debug_Init
  修改历史   :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include "stm32f0xx_conf.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
extern volatile uint32_t CORET_Systick_Count;			//CORET Systick count
/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*****************************************************************************
 函 数 名  : Usart_Debug_Init
 功能描述  : 串口打印初始化
 输入参数  : uint32_t Baudrate  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Usart_Debug_Init(uint32_t Baudrate){
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 

	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_1);

	/* Connect PXx to USARTx_Rx */
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_1);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	//GPIO_InitStructure.GPIO_Pin = GPIO_pin_15;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USARTx configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- Stop Bit = 1 Stop Bit
	- Parity = No Parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = Baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	//USART_DeInit(USART1);
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	printf("Usart debug init OK!\n");
}
/*****************************************************************************
 函 数 名  : fputc
 功能描述  : 打印输出重定向到USART TX
 输入参数  : int ch   
             FILE *f  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint16_t)ch);
    while(RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));

    return ch;
}
/*****************************************************************************
 函 数 名  : System_Systick_Init
 功能描述  : 系统滴答定时器初始化函数
 输入参数  : uint32_t ticks  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年4月15日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void System_Systick_Init(uint32_t ticks)
{
	if (SysTick_Config(SystemCoreClock / ticks)){
	/* capture error */
		printf("SysTick_Config ERRO!!!\n");
		return;
	}
	/* configure the systick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00U);
	printf("SysTick_Config %d uS OK.\n", ticks);
}


