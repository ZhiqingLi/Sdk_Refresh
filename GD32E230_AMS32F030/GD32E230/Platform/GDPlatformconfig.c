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
#include <stdlib.h>
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
void Usart_Debug_Init(uint32_t Baudrate)	{
	/* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_15);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_15);
    
	/* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);
    usart_deinit(USART1);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_baudrate_set(USART1, Baudrate);
    //usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_invert_config(USART1, USART_SWAP_ENABLE);
    usart_interrupt_enable(USART1, USART_INT_TC);
	/* Enable USART */
	usart_enable(USART1);

	/*nvic interrupt enable*/
	nvic_irq_enable(USART1_IRQn, 3);
	
	printf("Usart debug init OK!\n");
}

bool Gd_Usart_Debug_Busy(int Word) {
	if ((SET == usart_flag_get(USART1, USART_FLAG_TBE)) && (FALSE == GetDebugDataLen())) {
    	usart_data_transmit(USART1, (uint32_t)Word);
    	return FALSE;
    }
	return TRUE;
}

/*****************************************************************************
 函 数 名  : System_DetectInit
 功能描述  : 系统按键及开机检测初始化
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月5日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void System_DetectInit(void) {
	printf("System is ");

	if (SET == rcu_flag_get(RCU_FLAG_LPRST)) {
		printf("LOW_POWER_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_PORRST)) {
		printf("POR_POWER_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_WWDGTRST)) {
		printf("WINDOWS_DOG_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_FWDGTRST)) {
		printf("FREE_DOG_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_EPRST)) {
		printf("EXTERNAL_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_SWRST)) {
		printf("SOFT_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_OBLRST)) {
		printf("OPTION_BYTE_RESET;\n");
	}
	else if (SET == rcu_flag_get(RCU_FLAG_V12RST)) {
		printf("VDD12_POWER_RESET;\n");
	}

	rcu_all_reset_flag_clear();
}
/*****************************************************************************
 函 数 名  : Bsp_SystemInit
 功能描述  : 芯片平台初始化
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月7日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Bsp_SystemInit(void) {
	//USART初始化，115200波特率
	Usart_Debug_Init(115200);
	//系统滴答定时器初始化，单位uS
	SysTick_Config(SystemCoreClock/1000);
	System_DetectInit();
}
/*****************************************************************************
 函 数 名  : Bsp_FuncRuning
 功能描述  : 具体功能逻辑处理，由主函数循环调用
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年5月7日
    作    者   : qing
    修改内容   : 新生成函数

*****************************************************************************/
void Bsp_FuncRuning(void) {
	static uint32_t MemSize = 0;
	uint8_t* MemPoint = NULL;

	MemSize++;
	
	//MemPoint = malloc(MemSize);
	if (NULL == MemPoint) {
		printf ("malloc failed %d;\n", MemSize);
		while (1);
	}

	//free(MemPoint);
}


