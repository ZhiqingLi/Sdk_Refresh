/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : gpio_key.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年4月20日
  最近修改   :
  功能描述   : 
  函数列表   :
              GpioKeyInit
              GpioKeyScan
  修改历史   :
  1.日    期   : 2019年4月20日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

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

/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : power_key.c
  版 本 号   : 初稿
  作    者   : 李治清
  生成日期   : 2018年9月13日
  最近修改   :
  功能描述   : GPIO电源控制功能
  函数列表   :
              PowerKeyScan
              SysPowerOnControl
              SystemGotoDeepSleepPowerDown
              SystemPowerControlInit
              SystemPowerOnDetect
  修改历史   :
  1.日    期   : 2018年9月13日
    作    者   : 李治清
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "app_config.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

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


#define 	POWER_KEY_JITTER_TIME		20			//消抖时间，该时间和软开关开关机硬件时间有关
#define 	POWER_KEY_CP_JITTER_TIME	500	
#define 	POWER_KEY_CP_HOLD_TIME		1000			

typedef enum _POWER_KEY_STATE
{
	POWER_KEY_STATE_IDLE,
	POWER_KEY_STATE_JITTER,
	POWER_KEY_STATE_PRESS_DOWN,
	POWER_KEY_STATE_CP,
	POWER_KEY_STATE_CP_UP,
} POWER_KEY_STATE;

static TIMER	PowerKeyWaitTimer;
POWER_KEY_STATE	PowerKeyState;

										/*按键开始*/			/*短按松开*/			/*长按开始*/		/*长按保持*/		/*长按松开*/
static const uint16_t PowerKeyEvent[5] = {MSG_NONE, 		MSG_NEXT, 			MSG_NONE,		MSG_NONE,		MSG_NONE,};

// POWER_KEY与普通的按键不同，连接按钮开关（软开关）时的主要作用还是系统开关机，当然，也允许复用短按功能。
// 短按产生时，推送短按消息。超过短按区间，此处不做任何处理，由系统硬件检测关机。
uint16_t GpioKeyScan(void)							
{
	switch(PowerKeyState)
	{
		case POWER_KEY_STATE_IDLE:
			if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				return MSG_NONE;
			}
			else
			{	
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_JITTER_TIME);
				PowerKeyState = POWER_KEY_STATE_JITTER;
			}
			break;
            
		case POWER_KEY_STATE_JITTER:
			if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_PRESS_DOWN;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_JITTER_TIME);
				return PowerKeyEvent[0];			//return key sp value
			}
			break;
			
		case POWER_KEY_STATE_PRESS_DOWN:
			if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
                /* 产生短按消息 */
				return PowerKeyEvent[1];
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				PowerKeyState = POWER_KEY_STATE_CP;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_HOLD_TIME);
				return PowerKeyEvent[2];
			}
			break;
			
		case POWER_KEY_STATE_CP:
			//此处仅保证一次按键不会响应多次短按
			if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
			{
				PowerKeyState = POWER_KEY_STATE_IDLE;
				return PowerKeyEvent[4];//return MSG_NONE;				
			}
			else if(IsTimeOut(&PowerKeyWaitTimer))
			{
				/* 检测到关机按键,系统进入关机，不会返回*/
				PowerKeyState = POWER_KEY_STATE_CP;
				TimeOutSet(&PowerKeyWaitTimer, POWER_KEY_CP_HOLD_TIME);
       			return PowerKeyEvent[3];//return MSG_NONE;
			}
			break;              
			
		default:
			PowerKeyState = POWER_KEY_STATE_IDLE;
			break;
	}
	return MSG_NONE;
}
/*****************************************************************************
 函 数 名  : SystemPowerControlInit
 功能描述  : 系统电源控制初始化，
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年9月13日
    作    者   : 李治清
    修改内容   : 新生成函数

*****************************************************************************/
void GpioKeyInit(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

