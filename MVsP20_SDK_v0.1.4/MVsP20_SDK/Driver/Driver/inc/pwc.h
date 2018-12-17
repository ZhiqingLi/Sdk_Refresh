/**
 **************************************************************************************
 * @file    pwc.h
 * @brief   Pulse Width Capture (Reuse with General Timer 3&4, Advanced Timer 5&6 ) API
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
 * @addtogroup PWC
 * @{
 * @defgroup pwc pwc.h
 * @{
 */

#ifndef __PWC_H__
#define __PWC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "timer.h"

#define   PWC_MAX_TIMESCALE   0xffff       /**< PWC 最大值量程 */
    
//PWC 输出通道GPIO复用关系
#define   TIMER3_PWC1_A2_B22_A30_C7    0   /**< TIMER3的PWC1引脚复用于GPIOA2,A30,B22或者C7 */

#define   TIMER4_PWC1_A3_B23_A31_C8    1   /**< TIMER4的PWC1引脚复用于GPIOA3,A31，B23或者C8 */
    
    
#define   TIMER5_PWC1_A16              2   /**< TIMER5的PWC1引脚复用与GPIOA16 */
    
#define   TIMER5_PWC2_A17              3   /**< TIMER5的PWC2引脚复用与GPIOA17 */
    
#define   TIMER5_PWC3_A18              4   /**< TIMER5的PWC3引脚复用与GPIOA18 */
    
#define   TIMER5_PWC4_A19              5   /**< TIMER5的PWC4引脚复用与GPIOA19 */
    
    
#define   TIMER6_PWC1_B6               6   /**< TIMER6的PWC1引脚复用与GPIOB6 */
    
#define   TIMER6_PWC2_B4               7   /**< TIMER6的PWC2引脚复用与GPIOB4 */
    
#define   TIMER6_PWC3_B2               8   /**< TIMER6的PWC3引脚复用与GPIOB2 */

#define   TIMER6_PWC4_B5               9   /**< TIMER6的PWC4引脚复用与GPIOB5 */


#define   TIMER7_PWC1_C4               10   /**< TIMER7的PWC1引脚复用与GPIOC4 */

#define   TIMER7_PWC2_C5               11   /**< TIMER7的PWC2引脚复用与GPIOC5 */

#define   TIMER7_PWC3_C6               12   /**< TIMER7的PWC3引脚复用与GPIOC6 */
    
#define   TIMER7_PWC4_C3               13   /**< TIMER7的PWC4引脚复用与GPIOC3 */


#define   TIMER8_PWC1_B9               14   /**< TIMER8的PWC1引脚复用与GPIOB9 */

#define   TIMER8_PWC2_B11              15   /**< TIMER8的PWC2引脚复用与GPIOB11 */

#define   TIMER8_PWC3_B13              16   /**< TIMER8的PWC3引脚复用与GPIOB13 */

#define   TIMER8_PWC4_B8               17   /**< TIMER8的PWC4引脚复用与GPIOB8 */


/**
 * @brief PWC IO Mode 选择
 */ 
typedef enum _PWC_IO_MODE
{
    PWC_IO_MODE_NONE = 0xFF,  /**< 还原为GPIO */
    PWC_IO_MODE_IN   = 1     /**< 直接输出 */
} PWC_IO_MODE;
    

/**
 * @brief   PWC通道定义
 * @note    TIMER1,TIMER2不支持PWC通道
 *          TIMER3,TIMER4只支持一个PWC通道：PWC1
 *          TIMER5,TIMER6支持4个PWC通道
 */
typedef enum __PWC_INDEX
{
    PWC1 = 0, /**< PWC通道1 */
    PWC2,/**< PWC通道2 */
    PWC3,/**< PWC通道3 */
    PWC4,/**< PWC通道4 */
    
} PWC_INDEX;

/**
 * @brief  PWC捕获模式
 */
typedef enum _PWC_CAPTURE_MODE
{
	PWC_CAPTURE_CONTINUES = 0,	/**< 连续捕获 */

	PWC_CAPTURE_ONCE = 1		/**< 只捕获一次 */

} PWC_CAPTURE_MODE;

/**
 * @brief  PWC1输入源选择
 */
typedef enum _PWC1_INPUT_SOURCE_SEL
{
	PWC1_INPUT_PWC1_ONLY = 0,		/**< PWC1直接输入 */
	PWC1_INPUT_PWC1_XOR_PWC2PWC3,   /**< PWC1与PWC2和PWC3异或后输入 */

} PWC1_INPUT_SOURCE_SEL;

/**
 * @brief  PWC触发模式：中断或DMA
 */
typedef enum _PWC_DMA_INTERRUPT_MODE
{
	PWC_REQ_INTERRUPT_MODE = 0, /**< 中断模式 */

	PWC_REQ_DMA_MODE            /**< DMA模式 */

} PWC_DMA_INTERRUPT_MODE;

/**
 * @brief  PWC输入极性
 */
typedef enum _PWC_POLARITY
{
    PWC_POLARITY_BOTH     = 1,  /**< 双边沿捕获 */
    PWC_POLARITY_RAISING  = 2,  /**< 上升沿捕获 */
    PWC_POLARITY_FALLING  = 3   /**< 下降沿捕获 */

} PWC_POLARITY;

/**
 * @brief  PWC输入通道选择
 */
typedef enum _PWC_INPUT_CHANNEL_SEL
{
	PWC_INPUT_CHANNEL_PWC1 = 0,				/**< 只使用PWC1通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC2,			/**< 使用PWC1和PWC2通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC3,			/**< 使用PWC1和PWC3通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC4,			/**< 使用PWC1和PWC4通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC2_PWC3,		/**< 使用PWC1，PWC2和PWC3通道采集 */
	PWC_INPUT_CHANNLE_PWC1_PWC2_PWC4,		/**< 使用PWC1，PWC2和PWC4通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC3_PWC4,		/**< 使用PWC1，PWC3和PWC4通道采集 */
	PWC_INPUT_CHANNEL_PWC1_PWC2_PWC3_PWC4,	/**< 使用PWC1，PWC2，PWC3和PWC4通道采集 */

} PWC_INTPUT_CHANNEL_SEL;


/**
 * @brief  PWC输入滤波器采样时钟分频比
 */
typedef enum _PWC_FILTER_CLK_DIV
{   
    FILTER_CLK_DIV1 = 0,/**< 不分频,等于系统时钟，即Fsys */
    FILTER_CLK_DIV2,    /**< 系统时钟的2分频，即 Fsys / 2 */
    FILTER_CLK_DIV3,    /**< 系统时钟的4分频，即 Fsys / 4 */
    FILTER_CLK_DIV4,    /**< 系统时钟的8分频，即 Fsys / 8 */
    
} PWC_FILTER_CLK_DIV;

/**
 * @brief  DMA控制占空比的源选择
 */
typedef enum _PWC_DMA_SRC_SEL
{
	PWC_PWC1_TRIGGER_DMA_REG = 1,  /**< PWC1触发DMA */

	PWC_PWC2_TRIGGER_DMA_REG,      /**< PWC2触发DMA */

	PWC_PWC3_TRIGGER_DMA_REG,      /**< PWC3触发DMA */

	PWC_PWC4_TRIGGER_DMA_REG       /**< PWC4触发DMA */

} PWC_DMA_SRC_SEL;

/**
 * @brief  PWC错误号定义
 */
typedef enum _PWC_ERROR_CODE
{
    PWC_ERROR_INVALID_PWC_INDEX = -128,       /**< PWC定时器索引号错误 */
    PWC_ERROR_INVALID_PWC_POLARITY,           /**< PWC输入极性错误 */
    PWC_ERROR_INVALID_FILTER_CLK_DIV,         /**< PWC滤波器时钟分频比错误 */

    PWC_ERROR_INVALID_PWCx_INDEX,             /**< PWC通道索引号错误 */
    PWC_ERROR_INVALID_CHANNEL_INDEX,          /**< PWC输入通道选择错误 */

    PWC_ERROR_OK = 0                          /**< 无错误 */
} PWC_ERROR_CODE;


/** 
 * @brief  PWC初始化结构体定义
 */  
typedef struct __PWC_StructInit
{
	PWC_CAPTURE_MODE  		SingleGet;          /**< 捕获次数： 1 -- 只读取1次， 0 -- 连续读取 */

	PWC_DMA_INTERRUPT_MODE  DMAReqEnable;       /**< 是否使能DMA将读取的数据搬运MEM中，参数取值： 1 -- 使能DMA请求； 0 -- 禁能DMA请求 */
    
    uint16_t 				TimeScale;          /**< PWC测量的量程，取值范围[1,65535], 即可以测量的范围（1/Fsys * TimeScale  ~  65535/Fsys * TimeScale） */
    
    uint8_t  				FilterTime;         /**< 滤波时间，范围：1/Fpwc ~ 128/Fpwc */
    

	uint8_t  				InputPrescale[4];	/**< 只要有高级定时器才有此功能，取值范围[0, 7]，默认为0，输入信号不分频，InputPrescale[0]~InputPrescale[3]对应PWC1~PWC4的输入信号 */


    PWC_POLARITY  			Polarity[4];        /**< 捕获极性，参数取值范围：边缘到边缘， 上升沿到上升沿，下降沿到下降沿，详细请见 #PWC_POLARITY，Polarity[0]~Polarity[3]对应PWC1~PWC4 */


    PWC1_INPUT_SOURCE_SEL 	PWC1Source;	       /**< 只有高级定时器才有此功能，PWC1输入源选择 */
           
}PWC_StructInit;  

/**
 * @brief PWC IOCTROL 命令
 */
typedef enum _PWC_IOCTRL_CMD
{
	PWC_DATA_GET = 0,                    /**< PWC数据获取 */
	PWC_DONE_STATUS_GET,                 /**< PWC捕获完成状态获取 */
	PWC_OVER_CAPTURE_STATUS_GET,         /**< PWC重复捕获状态获取 */
	PWC_ERR_STATUS_GET,                  /**< PWC错误状态获取 */

	PWC_OVER_CAPTURE_STATUS_CLR,         /**< PWC重复捕获状态清除 */
	PWC_ERR_STATUS_CLR,                  /**< PWC错误状态清除 */

    PWC_DMA_SRC_SET,                     /**< PWC DMA触发源设置 */

    PWC_POLARITY_UPDATE,                 /**< PWC输入极性设置 */


} PWC_IOCTRL_CMD;

/**
 * @brief PWC IOCTROL 参数
 */
typedef struct _PWC_IOCTRL_ARG
{
	PWC_DMA_SRC_SEL PWCDmaSel;      /**< DMA控制源 */

	PWC_POLARITY    PWCPolarity;    /**< PWC输入极性 */

} PWC_IOCTRL_ARG;


/**
 * @brief      将GPIO复用为PWC引脚或还原为GPIO引脚.
 *
 * @param[in]  PWCChSel  PWC通道选择，详见 #PWC 输出通道GPIO复用关系 的宏定义
 * @param[in]  PWCIoSel  例如：当PWCChSel = TIMER3_PWC1_A2_B22_A30_C7，
 * 							  0：复用于A2，
 * 							  1：复用于B22，
 * 							  2：复用于A30，
 * 							  3：复用与C7
 * @param[in]  PWCMode   1 - 复用为PWC引脚， 0 - 还原为普通GPIO引脚，详见PWC_IO_MODE
 *
 * @return     无
 */
void PWC_GpioConfig(uint8_t PWCChSel, uint8_t PWCIoSel, PWC_IO_MODE PWCMode);

/**
 * @brief     配置一个定时器下的PWC输入通道
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 * @param[in] PWCParam  PWC初始化参数，详细参考 PWCInfo
 * @param[in] PWCChannelSel PWC采集通道选择，详见#PWC_INTPUT_CHANNEL_SEL
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWC_ERROR_CODE
 */
PWC_ERROR_CODE PWC_Config(TIMER_INDEX TimerIdx, PWC_StructInit *PWCParam, PWC_INTPUT_CHANNEL_SEL PWCChannelSel);

/**
 * @brief     使能TimerIdx下的PWC通道进行采集
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWC_ERROR_CODE
 */
PWC_ERROR_CODE PWC_Enable(TIMER_INDEX TimerIdx);

/**
 * @brief     关闭TimerIdx下的PWC通道进行采集
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 *
 * @return    错误号：0 - 正确，其他为错误，详细参考PWC_ERROR_CODE
 */
PWC_ERROR_CODE PWC_Disable(TIMER_INDEX TimerIdx);


/**
 * @brief     TimerIdx下的一路PWC进行操作
 *
 * @param[in] TimerIdx  定时器索引号，详见 # TIMER_INDEX
 * @param[in] PWCIdx    PWC索引号，详见 # PWCIdx
 *
 * @return    >=0: 正确数值， < 0: 错误返回值
 */
PWC_ERROR_CODE PWC_IOCTRL(TIMER_INDEX TimerIdx, PWC_INDEX PWCIdx, PWC_IOCTRL_CMD Cmd, PWC_IOCTRL_ARG *Arg);



#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__PWC_H__

/**
 * @}
 * @}
 */

