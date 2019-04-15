/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : GDPlatformconfig.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月15日
  最近修改   :
  功能描述   : GD32E230芯片平台配置文件
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
#include "app_config.h"
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
	/* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_14);

    /* connect port to USARTx_Rx */
    //gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_15);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_14);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_14);

    /* configure USART Rx as alternate function push-pull */
    //gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    //gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_15);
    
	/* USARTx configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- Stop Bit = 1 Stop Bit
	- Parity = No Parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, Baudrate);
    //usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

	/* Enable USART */
	usart_enable(USART1);
	
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
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    
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


