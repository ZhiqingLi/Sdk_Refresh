/**
 **************************************************************************************
 * @file    pwm.h
 * @brief   Pulse Width Modulation (Reuse with General Timer 3&4, Advanced Timer 5&6 ) API
 *
 * @author  Cecilia Wang, Grayson Chen
 * @version V1.0.0
 *
 * $Created: 2016-04-29 13:25:30$
 *
 * @copyright Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

/**
 * @addtogroup PWM
 * @{
 * @defgroup pwm pwm.h
 * @{
 */

#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "timer.h"

#define   PWM_MAX_FREQ_DIV_VALUE   (65535 << 15)   /**< PWM最大分频比 */


//PWM 输出通道GPIO复用关系
#define   TIMER3_PWM1_A2_B22_A30_C7  0    /**< TIMER3的PWM引脚可复用在A2, A30, B22或者C7 */

#define   TIMER4_PWM1_A3_B23_A31_C8  1    /**< TIMER4的PWM引脚复用在A3, A31, B23或者C8 */
    

#define   TIMER5_PWM1_N_A19        2    /**< TIMER5的PWM1的负相输出端引脚可复用于A19 */
    
#define   TIMER5_PWM1_P_A16        3    /**< TIMER5的PWM1的正相输出端引脚复用于A16 */
    
#define   TIMER5_PWM2_N_A20        4    /**< TIMER5的PWM2的负相输出端引脚复用于A20 */
    
#define   TIMER5_PWM2_P_A17        5    /**< TIMER5的PWM2的正相输出端引脚复用于A17 */

#define   TIMER5_PWM3_N_A21        6    /**< TIMER5的PWM3的负相输出端引脚复用于A21 */

#define   TIMER5_PWM3_P_A18        7    /**< TIMER5的PWM3的正相输出端引脚复用于A18 */

#define   TIMER5_PWM4_P_A19        8    /**< TIMER5的PWM4的正相输出端引脚复用于A19 */


#define   TIMER6_PWM1_N_B5         9    /**< TIMER6的PWM1的负相输出端引脚复用于B5 */

#define   TIMER6_PWM1_P_B6         10   /**< TIMER6的PWM1的正相输出端引脚复用于B6 */

#define   TIMER6_PWM2_N_B3     	   11   /**< TIMER6的PWM2的负相输出端引脚复用于B3 */

#define   TIMER6_PWM2_P_B4     	   12   /**< TIMER6的PWM2的正相输出端引脚复用于B4 */

#define   TIMER6_PWM3_N_B1         13   /**< TIMER6的PWM3的负相输出端引脚复用于B1 */
    
#define   TIMER6_PWM3_P_B2         14   /**< TIMER6的PWM3的正相输出端引脚复用于B2 */
 
#define   TIMER6_PWM4_P_B5         15   /**< TIMER6的PWM4的正相输出端引脚复用于B5 */


#define   TIMER7_PWM1_N_C3         16    /**< TIMER7的PWM1的负相输出端引脚复用于C3 */
    
#define   TIMER7_PWM1_P_C4         17    /**< TIMER7的PWM1的正相输出端引脚复用于C4 */
    
#define   TIMER7_PWM2_N_C2     	   18    /**< TIMER7的PWM2的负相输出端引脚复用于C2 */
    
#define   TIMER7_PWM2_P_C5     	   19    /**< TIMER7的PWM2的正相输出端引脚复用于C5 */

#define   TIMER7_PWM3_N_C1         20    /**< TIMER7的PWM3的负相输出端引脚复用于C1 */
    
#define   TIMER7_PWM3_P_C6         21    /**< TIMER7的PWM3的正相输出端引脚复用于C6 */
 
#define   TIMER7_PWM4_P_C3         22    /**< TIMER7的PWM4的正相输出端引脚复用于C3 */


#define   TIMER8_PWM1_N_B8         23    /**< TIMER8的PWM1的负相输出端引脚复用于B8 */

#define   TIMER8_PWM1_P_B9         24    /**< TIMER8的PWM1的正相输出端引脚复用于B9 */

#define   TIMER8_PWM2_N_B10        25    /**< TIMER8的PWM2的负相输出端引脚复用于B10 */

#define   TIMER8_PWM2_P_B11        26    /**< TIMER8的PWM2的正相输出端引脚复用于B11 */

#define   TIMER8_PWM3_N_B12        27    /**< TIMER8的PWM3的负相输出端引脚复用于B12 */

#define   TIMER8_PWM3_P_B13        28    /**< TIMER8的PWM3的正相输出端引脚复用于B13 */

#define   TIMER8_PWM4_P_B8         29    /**< TIMER8的PWM4的正相输出端引脚复用于B8 */

//Break in与GPIO复用关系
#define   TIMER5_BREAK_A15         0   /**< TIMER5的break in输入引脚 */

#define   TIMER6_BREAK_B0          1   /**< TIMER6的break in输入引脚  */

#define   TIMER7_BREAK_C0		   2   /**< TIMER7的break in输入引脚  */

#define   TIMER8_BREAK_B7          3   /**< TIMER8的break in输入引脚  */


/**
 * @brief   PWM通道定义
 * @note    TIMER1,TIMER2不支持PWM通道    
 * @note    TIMER3,TIMER4只支持一个PWM通道：PWM1
 * @note    TIMER5,TIMER6,TIMER7,TIMER8支持4个PWM通道
 */
typedef enum __PWM_INDEX
{
    PWM1 = 0,    /**< PWM通道1  */
    PWM2,        /**< PWM通道2  */
    PWM3,        /**< PWM通道3  */
    PWM4         /**< PWM通道4  */
    
} PWM_INDEX;

/**
 * @brief PWM IO Mode选择
 */ 
typedef enum _PWM_IO_MODE
{
    PWM_IO_MODE_NONE = 0xff,    /**< 还原为GPIO  */
    PWM_IO_MODE_OUT  = 1,       /**< 直接输出  */
    PWM_IO_MODE_PD1  = 2,       /**< PWM下拉1.7mA输出  */
    PWM_IO_MODE_PD2  = 3,       /**< PWM下拉2.4mA输出  */
    PWM_IO_MODE_PD12 = 4        /**< PWM下拉4.1mA输出  */
    
} PWM_IO_MODE;

/**
 * @brief  PWM计数模式
 */
typedef enum _PWM_COUNTER_MODE
{
    PWM_COUNTER_MODE_DOWN = 0,        /**< 从高到低计数  */
    PWM_COUNTER_MODE_UP,              /**< 从低到高计数  */
    PWM_COUNTER_MODE_CENTER_ALIGNED1, /**< 先从低加到高，再从高减至低，只在下溢出时产生中断  */
    PWM_COUNTER_MODE_CENTER_ALIGNED2, /**< 先从低加到高，再从高减至低，只在上溢出时产生中断  */
    PWM_COUNTER_MODE_CENTER_ALIGNED3  /**< 先从低加到高，再从高减至低，在上溢出和下溢出时都产生中断  */
} PWM_COUNTER_MODE;


/**
 * @brief  PWM输出类型定义  
 */
typedef enum _PWM_OUTPUT_TYPE
{
    PWM_OUTPUT_SINGLE_FORCE_NOACTIVE 	= 4,      /**< 强制输出无效电平  */
    PWM_OUTPUT_SINGLE_FORCE_ACTIVE 		= 5,      /**< 强制输出有效电平  */
    
    PWM_OUTPUT_SINGLE_1    				= 6,      /**< 标准输出：Duty为有效电平占空比  */
    PWM_OUTPUT_SINGLE_2     			= 7,      /**< 标准输出：Duty为无效电平占空比  */
    
    PWM_OUTPUT_COMBINED_PWM1_OR_PWM2 	= 8,	  /**< PWM1(产生行为同PWM_OUTPUT_SINGLE_1)与PWM2(产生行为同PWM_OUTPUT_SINGLE_2)或运算，运算结果通过PWM1输出  */
    PWM_OUTPUT_COMBINED_PWM1_AND_PWM2 	= 9,	  /**< PWM1与PWM2与运算输出  */
    PWM_OUTPUT_COMBINED_PWM3_OR_PWM4 	= 10,	  /**< PWM3(产生行为同PWM_OUTPUT_SINGLE_1)与PWM4(产生行为同PWM_OUTPUT_SINGLE_2)或运算，运算结果通过PWM3输出  */
    PWM_OUTPUT_COMBINED_PWM3_AND_PWM4 	= 11,	  /**< PWM3与PWM4与运算输出  */

    PWM_OUTPUT_ASYMMETRIC_1 			= 12,     /**< PWM1~4组合不对称输出并通过PWM1和PWM3输出，产生行为与PWM_OUTPUT_SINGLE_1相同  */
    PWM_OUTPUT_ASYMMETRIC_2 			= 13,     /**< PWM1~4组合不对称输出并通过PWM1和PWM3输出，产生行为与PWM_OUTPUT_SINGLE_2相同  */

    PWM_OUTPUT_ONE_PULSE				= 14	  /**< 输出一个脉冲后结束  */
} PWM_OUTPUT_TYPE;

/**
 * @brief  PWM触发模式：中断或DMA
 */
typedef enum _PWM_DMA_INTERRUPT_MODE
{
	PWM_REQ_INTERRUPT_MODE = 0,  /**< 中断模式  */

	PWM_REQ_DMA_MODE             /**< DMA模式  */

} PWM_DMA_INTERRUPT_MODE;

/**
 * @brief  PWM的输出极性选择OutPolarity
 */
typedef enum _PWM_OUTPUT_POLARITY_MODE
{
	OUTP_ACTIVE_H_AND_OUTN_ACTIVE_H = 0, /**< 正向端和负向端均为有效电平为高电平  */

	OUTP_ACTIVE_L_AND_OUTN_ACTIVE_H = 1, /**< 正端为低电平有效，低电平为高电平有效  */

	OUTP_ACTIVE_H_AND_OUTN_ACTIVE_L = 2, /**< 正向端为高电平有效，负向端为低电平有效  */

	OUTP_ACTIVE_L_AND_OUTN_ACTIVE_L = 3, /**< 正向端和负向端均为有效电平为低电平  */

} PWM_OUTPUT_POLARITY_MODE;

/**
 * @brief  PWM的输出端口选择
 */
typedef enum _PWM_OUTPUT_ENABLE_MODE
{
	OUTP_AND_OUTN_DISABLE = 0,			    /**< N端和P端都禁能输出  */

	OUTP_ENABLE_AND_OUTN_DISABLE = 1,       /**< N端禁能输出，P端使能输出  */

	OUTP_DISABLE_AND_OUTN_ENABLE = 2,       /**< N端使能输出，P端禁能输出  */

	OUTP_AND_OUTN_ENABLE = 3,               /**< N端和P端都使能输出  */

} PWM_OUTPUT_ENABLE_MODE;

/**
 * @brief  PWM的idle状态电平选择
 */
typedef enum _PWM_IDLE_STATE_SEL
{
	IDLE_P_LOW_AND_N_LOW = 0,          /**< idle状态时，P端输出低电平，N端输出低电平  */

	IDLE_P_HIGH_AND_N_LOW,			   /**< idle状态时，P端输出高电平，N端输出低电平  */

	IDLE_P_LOW_AND_N_HIGH,			   /**< idle状态时，P端输出低电平，N端输出高电平  */

	IDLE_P_HIGH_AND_N_HIGH,			   /**< idle状态时，P端输出高电平，N端输出高电平  */

} PWM_IDLE_STATE_SEL;

/**
 * @brief  DMA控制占空比的源选择
 */
typedef enum _PWM_DMA_SRC_SEL
{
	PWM_PWM1_TRIGGER_DMA_REG = 1,  /**< DMA源选择PWM1 */

	PWM_PWM2_TRIGGER_DMA_REG,  /**< DMA源选择PWM2 */

	PWM_PWM3_TRIGGER_DMA_REG,  /**< DMA源选择PWM3 */

	PWM_PWM4_TRIGGER_DMA_REG   /**< DMA源选择PWM4 */

} PWM_DMA_SRC_SEL;

/**
 * @brief  断路插入的极性
 */
typedef enum _BREAK_IN_POLAR
{
    BREAK_POLARITY_LOW      = 0, /**< 低电平有效  */
    BREAK_POLARITY_HIGH     = 1  /**< 高电平有效  */
} BREAK_IN_POLAR;


/**
 * @brief  死区和断路输入滤波器的时钟分频比
 */
typedef enum _DEAD_TIME_BREAK_FILTER_CLK_DIV
{
    SYS_CLK_DIV_1       = 0, /**< 死区控制时钟 = 系统时钟  */
    SYS_CLK_DIV_2       = 1, /**< 死区控制时钟 = 系统时钟/2  */
    SYS_CLK_DIV_4       = 2, /**< 死区控制时钟 = 系统时钟/4  */
    SYS_CLK_DIV_8       = 3  /**< 死区控制时钟 = 系统时钟/8  */
} DEAD_TIME_BREAK_FILTER_CLK_DIV;

/**
 * @brief  退出break in状态的两种触发方式
 */
typedef enum _BREAK_IN_EXIT_TRIGGER_MODE
{
	BREAK_IN_BY_SOFTWARE_WRITE_UPDATE = 0,			/**< 通过软件update退出break in状态  */
	BREAK_IN_BY_SOFRWARE_UPDATE_OR_HARDWARE_UPDATE, /**< 通过软件update或在下一次硬件update时退出break in状态  */

} BREAK_IN_EXIT_TRIGGER_MODE;

/**
 * @brief  进入break in后的状态
 */
typedef enum _BREAK_IN_STATE_MODE
{
	SWITCH_IDLE_STATE_DIRECTLY = 0,				 /**< 直接进入idle状态  */
	SWITCH_ACTIVE_STATE_BEFORE_SWITCH_IDLE_STATE /**< 先进入无效电平状态，然后在死区时间之后进入IDLE状态  */

} BREAK_IN_STATE_MODE;

/**
 * @brief  PWM配置错误类型定义  
 */
typedef enum _PWM_ERROR_CODE
{
    PWM_ERROR_INVALID_TIMER_INDEX = -128,/**< PWM定时器索引号错误  */
    PWM_ERROR_INVALID_PWM_TYPE,          /**< PWM输出类型错误  */
    PWM_ERROR_INVALID_PWM_COUNTER_MODE,  /**< PWM计数模式错误  */
    
    PWM_ERROR_DEAD_CLK_DIV,              /**< PWM死区时钟分频错误  */
    PWM_ERROR_BREAK_FILTER_CLK_DIV,      /**< PWM BREAK IN滤波时钟分频错误  */

    PWM_ERROR_DMA_TIGGER_SRC,            /**< PWM DMA源选择错误  */
    PWM_ERROR_OUTPUT_POLARITY_ACTIVE,    /**< PWM 输出极性错误  */
    PWM_ERROR_OUTPUT_ENABLE,             /**< PWM 输出使能错误  */

    PWM_ERROR_OK = 0                     /**< 无错误  */
} PWM_ERROR_CODE;

/**
 * @brief  PWM初始化结构体定义
 */
typedef struct __PWM_StructInit
{
	PWM_COUNTER_MODE     		CounterMode;        /**< PWM计数模式，参数取值范围 #PWM_COUNTER_MODE  */
    
	PWM_OUTPUT_TYPE      		OutputType;         /**< PWM输出类型,参数取值为#PWM_OUTPUT_TYPE  */

	PWM_DMA_INTERRUPT_MODE   	DMAReqEnable;       /**< 是否使能DMA从Mem搬运占空比到外设中，参数取值： 1 -- 使能DMA请求； 0 -- 禁能DMA请求  */
    
    uint32_t    		 		FreqDiv;            /**< PWM频率与系统时钟的分频比，取值范围[1, PWM_MAX_FREQ_DIV_VALUE]  */

    uint16_t    				Duty[4];            /**< 占空比,取值范围[0~100]对应占空比为[0%~100%],Duty[0]~Duty[3]对应PWM1~4的占空比  */

    PWM_OUTPUT_POLARITY_MODE    OutPolarity[4];	    /**< PWM的输出极性选择，OutPolarity[0]~OutPolarity[3]对应PWM1~4的极性，参数只针对TIMER5~8有效  */

    PWM_OUTPUT_ENABLE_MODE      OutEnable[4];	    /**< PWM的输出端口使能，OutEnable[0]~OutEnable[3]对应PWM1~4的端口使能 */

    PWM_IDLE_STATE_SEL	        OutIdleState[4];    /**< PWM的idle状态电平，OutIdleState[0]~OutIdleState[3]对应PWM1~4的端口idle状态 */
           
}PWM_StructInit;   



/**
 * @brief 死区和断路插入初始化结构体
 */
typedef struct _BDTInfo
{    
	DEAD_TIME_BREAK_FILTER_CLK_DIV     DTFreqDiv;          /**< 死区时钟分频比  */

    uint16_t   						   DTCycle;            /**< 死区占用的Cycle,范围：1~256  */

    uint8_t     					   FilterTime;         /**< 断路插入滤波器的滤波时间，范围：1 ~ 128  */
    
    BREAK_IN_POLAR     				   BreakPolar;         /**< Break in极性设置  */

    BREAK_IN_EXIT_TRIGGER_MODE		   AutoExitIdleEnable; /**< break in状态退出方式  */

    BREAK_IN_STATE_MODE			       BreakOffStateSel;   /**< 进入break in后的状态选择  */
    
} BDTInfo;
 

/**
 * @brief PWM IOCTRL 命令
 */
typedef enum _PWM_IOCTRL_CMD
{
	//update and Select cmd
	OUTPUT_SOFTWARE_UPDATE 		= 0x1,	    /**< TIMERx软件update寄存器  */

	OUTPUT_TYPE_UPDATE 			= 0x2,		/**< #PWM_OUTPUT_TYPE  */

	OUTPUT_ENABLE_UPDATE	 	= 0x4, 		/**< 同PWM_StructInit中的OutEnable  */

	OUTPUT_POLARITY_UPDATE 		= 0x8,		/**< 极性更新  */

	OUTPUT_FREQUENCY_UPDATE 	= 0x10,	    /**< 频率更新  */

	OUTPUT_DUTY_UPDATE 			= 0x20,		/**< 占空比更新  */

    DMA_SRC_PWM_SEL 			= 0x40,		/**< DMA控制源选择  */

	BREAKIN_FLAG_CLR 			= 0x80,		/**< break in标志清除  */

	SOFT_BREAKIN_SET 			= 0x100, 	/**< software break 设置，0或1  */

    //以上控制命令支持多条命令同时输入，例如cmd = OUTPUT_FREQUENCY_UPDATE | OUTPUT_DUTY_UPDATE

    //以下控制命令不支持命令同时输入
	//Get cmd
	OUTPUT_STATE_GET = 0x1000,    			/**< PWM输出状态获取，1 -- 运行，0 -- IDLE  */

	BREAKIN_FLAG_GET = 0x2000,	 			/**< Break in中断使能标志获取，1 -- 使能break in； 0 -- 禁能break in； -128 -- 错误的定时器索引  */

	BREAKIN_STATUS_GET = 0x4000,		    /**< 获取break in的状态  */


} PWM_IOCTRL_CMD;


/**
 * @brief PWM IOCTRL 参数
 */
typedef struct _PWM_IOCTRL_ARG
{
	PWM_OUTPUT_TYPE     	   OutputType;         /**< PWM输出类型  */

	PWM_OUTPUT_ENABLE_MODE     OutEnable;          /**< PWM的输出端口使能  */

	PWM_OUTPUT_POLARITY_MODE   OutPolarity;		   /**< PWM的输出极性选择  */

	uint32_t    			   FreqDiv;            /**< PWM频率与系统时钟的分频比，取值范围[1, PWM_MAX_FREQ_DIV_VALUE]  */

	uint16_t				   Duty;			   /**< 占空比  */

	PWM_DMA_SRC_SEL			   DMATiggerSel;	   /**< DMA源选择  */

	bool					   SoftBreakSet;       /**< 软件break in状态设置  */

} PWM_IOCTRL_ARG;

/**
 * @brief     设置一路PWM的GPIO复用
 *
 * @param[in] PWMChSel  PWM通道选择，参数可见#PWM 通道GPIO复用关系宏定义
 * @param[in] PwmIoSel  例如：当选择TIMER3_PWM1_A2_B22_A30_C7，当PwmIoSel = 0XFF，则PWM复用与A2，
 *                          当PwmIoSel = 1，则复用于B22口, PwmIoSel = 2,复用于 A30， PwmIoSel = 3，复用于C7。
 * @param[in] PWMMode   PWM复用关系，详细参考 #PWM_IO_MODE
 *
 * @return    无
 */
void PWM_GpioConfig(uint8_t PWMChSel, uint8_t PwmIoSel, PWM_IO_MODE PWMMode);


/**
 * @brief     设置一路PWM break in的GPIO复用
 *
 * @param[in] BreakSel  break in的引脚选择
 * @param[in] PWMMode   1 -- 复用为break in； 0xFF -- 还原为gpio
 *
 * @return    无
 */
void PWM_BreakGpioConfig(uint8_t BreakSel, uint8_t PWMMode);


/**
 * @brief     选择一个定时器下的PWM通道并配置其参数
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 * @param[in] PWMIdx    PWM索引号，详见 # PWMIdx
 * @param[in] PWMParam  PWM初始化参数，详细参考 PWMInfo
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE PWM_Config(TIMER_INDEX TimerIdx, PWM_INDEX PWMIdx, PWM_StructInit *PWMParam);

/**
 * @brief     开启定时器TimerIdx的一个通道PWM
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE PWM_Enable(TIMER_INDEX TimerIdx);

/**
 * @brief     关闭定时器TimerIdx的一个通道PWM
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE PWM_Disable(TIMER_INDEX TimerIdx);

/**
 * @brief     暂停定时器TimerIdx的一个通道PWM
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 * 			  IsPause   1: PWM暂停，保持在最后输出电平  0：PWM正常输出
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE PWM_Pause(TIMER_INDEX TimerIdx, bool IsPause);

/**
 * @brief     TimerIdx下的一路PWM进行操作
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 * @param[in] PWMIdx    PWM索引号，详见 # PWMIdx
 * @param[in] IOCtrl    IOCtrl类型
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE PWM_IOCTRl(TIMER_INDEX TimerIdx, PWM_INDEX PWMIdx, PWM_IOCTRL_CMD Cmd, PWM_IOCTRL_ARG *Arg);

/**
 * @brief      Dead时间配置
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 * @param[in]  BDTParam  DeadTime相关参数配置
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_DeadTimeConfig(TIMER_INDEX TimerIdx, BDTInfo *BDTParam);

/**
 * @brief      Dead功能使能
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_DeadTimeEnable(TIMER_INDEX TimerIdx);

/**
 * @brief      Dead功能禁能
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_DeadTimeDisable(TIMER_INDEX TimerIdx);


/**
 * @brief      Break in功能配置
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 * @param[in]  BDTParam  break in相关参数配置
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_BreakInConfig(TIMER_INDEX TimerIdx, BDTInfo *BDTParam);

/**
 * @brief      break in功能使能
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_BreakInEnable(TIMER_INDEX TimerIdx);

/**
 * @brief      break in功能禁能
 *
 * @param[in]  TimerIdx  定时器索引号，只有TIMER5和TIMER6支持Break in功能
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
PWM_ERROR_CODE  PWM_BreakInDisable(TIMER_INDEX TimerIdx);

/**
 * @brief      占空比动态配置,用于高精度占空比设置
 *
 * @param[in]  TimerIdx 定时器索引号
 * @param[in]  PWMIdx   PWM通道的索引号
 * @param[in]  Duty     0~0xFFFF对应0~100%
 *
 * @return     错误号：0 - 正确，其他为错误，详细参考PWM_ERROR_CODE
 */
int32_t PWM_DutyConfigIQ16(TIMER_INDEX TimerIdx, PWM_INDEX PWMIdx, uint16_t Duty);


/**
 * @brief      将定时器TimerIdx占空比参数进行转换，将百分比数据转成十六进制数据
 *
 * @param[in]  TimerIdx 定时器索引号
 * @param[in]  BufIn    占空比输入参数地址，单位％
 * @param[out] BufOut   占空比输出参数地址，十六进制
 * @param[in]  Len      BufIn的长度,单位：字节
 *
 * @return     BufOut   长度
 */
uint32_t PWM_DutyPercentoHex(TIMER_INDEX TimerIdx, uint8_t *BufIn, uint16_t *BufOut, uint16_t Len);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__PWM_H__

/**
 * @}
 * @}
 */

