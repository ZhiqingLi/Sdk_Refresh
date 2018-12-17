/**
 *******************************************************************************
 * @file    clk.h
 * @brief	Clock driver interface
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2015-11-05 10:46:11$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */
 
 
/**
 * @addtogroup CLOCK
 * @{
 * @defgroup clk clk.h
 * @{
 */
 
#ifndef __CLK_H__
#define __CLK_H__
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**
 * CLK module switch macro define
 */
typedef enum __CLOCK_MODULE_SWITCH
{
	FLASHC_CLK_EN = (1 << 0),           /**<FLASH control module clk switch */
	FLASHC_CLK_480M_EN = (1 << 1),			/**<FLASH control module clk 480M switch */
	USB_CLK_480M_EN = (1 << 2),         /**<USB module clk switch */
	FLASHC_HPM_CLK_480M_EN = (1 << 3),  /**<FLASH control HPM module clk 480M switch */
	EFUSE_CLK_EN = (1 << 4),            /**<AUDIO ADC1 module clk switch */
	I2S_CLK_EN = (1 << 5),             /**<I2S module clk switch */
	SARADC_CLK_EN = (1 << 6),           /**<SarADC module clk switch */
	I2C_CLK_EN = (1 << 7),              /**<I2C module clk switch */
	SD_CLK_EN = (1 << 8),               /**<SD module clk switch */
	SPIM0_CLK_EN = (1 << 9),           /**<SPIM0 module clk switch */
	SPIM1_CLK_EN = (1 << 10),           /**<SPIM1 module clk switch */
	UART0_CLK_EN = (1 << 11),           /**<UART0 module clk switch */
	UART1_CLK_EN = (1 << 12),           /**<UART1 module clk switch */
	UART2_CLK_EN = (1 << 13),           /**<UART2 module clk switch */
	USB_CLK_EN = (1 << 14),             /**<USB module clk switch */
	DMA_CLK_EN = (1 << 15),             /**<DMA module clk switch */
	FDMA_CLK_EN = (1 << 16),            /**<FDMA module clk switch */
	DPA_CLK_EN = (1 << 17),             /**<ACC module clk switch */
	LEDC_CLK_EN = (1 << 18),            /**<LED control module clk switch */
	TIMER1_CLK_EN = (1 << 19),          /**<TIMER1 module clk switch */
	TIMER2_CLK_EN = (1 << 20),          /**<TIMER2 module clk switch */
	TIMER3_CLK_EN = (1 << 21),          /**<TIMER3 module clk switch */
	TIMER4_CLK_EN = (1 << 22),          /**<TIMER4 module clk switch */
	TIMER5_CLK_EN = (1 << 23),          /**<TIMER5 module clk switch */
	TIMER6_CLK_EN = (1 << 24),          /**<TIMER6 module clk switch */
	TIMER7_CLK_EN = (1 << 25),          /**<TIMER7 module clk switch */
	TIMER8_CLK_EN = (1 << 26),          /**<TIMER8 module clk switch */
	ALL_MODULE_CLK_SWITCH = (0x7FFFFFF),/**<all module clk SWITCH*/
} CLOCK_MODULE_SWITCH;

/**
 * CLK module GATE switch macro define
 */
typedef enum __CLOCK_MODULE_GATE_SWITCH
{
	I2S_CLK_GATE_EN = (1 << 0),          				/**<I2S module clk gating switch */
	SARADC_CLK_GATE_EN = (1 << 1),              /**<SarADC module clk gating switch */
	FLASHC_CLK_GATE_EN = (1 << 2),              /**<FLASH control module clk gating switch */
	UART0_CLK_GATE_EN = (1 << 3),               /**<UART0 module clk gating switch */
	UART1_CLK_GATE_EN = (1 << 4),               /**<UART1 module clk gating switch */
	UART2_CLK_GATE_EN = (1 << 5),               /**<UART2 module clk gating switch */
	CORE_CLK_GATE_EN = (1 << 6),                /**<CORE module clk gating switch */
	GPIO_CLK_GATE_EN = (1 << 7),                /**<GPIO module clk gating switch */
	SRAM1_CLK_GATE_EN = (1 << 8),               /**<SRAM1 module clk gating switch */
	SRAM2_CLK_GATE_EN = (1 << 9),               /**<SRAM2 module clk gating switch */
	SRAM3_CLK_GATE_EN = (1 << 10),              /**<SRAM3 module clk gating switch */
	SRAM7_CLK_GATE_EN = (1 << 11),              /**<SRAM7 module clk gating switch */
	DMA_CLK_GATE_EN = (1 << 12),                /**<DMA module clk gating switch */
	SDIO_CLK_GATE_EN = (1 << 13),               /**<SDIO module clk gating switch */
	SPIM0_CLK_GATE_EN = (1 << 14),              /**<SPIM0 module clk gating switch */
	SPIM1_CLK_GATE_EN = (1 << 15),              /**<SPIM1 module clk gating switch */
	SPIS_CLK_GATE_EN = (1 << 16),               /**<SPIS module clk gating switch */
	ROM_CLK_GATE_EN = (1 << 17),                /**<ROM module clk gating switch */
	DPLL_CLK_GATE_EN = (1 << 18),               /**<DPLL module clk gating switch */
	RESET_CLK_GATE_EN = (1 << 19),              /**<RESET module clk gating switch */
	CLOCK_CLK_GATE_EN = (1 << 20),              /**<CLOCK module clk gating switch */
	TIMER1_CLK_GATE_EN = (1 << 21),             /**<TIMER1 module clk gating switch */
	TIMER2_CLK_GATE_EN = (1 << 22),             /**<TIMER1 module clk gating switch */
	TIMER3_CLK_GATE_EN = (1 << 23),             /**<TIMER1 module clk gating switch */
	TIMER4_CLK_GATE_EN = (1 << 24),             /**<TIMER1 module clk gating switch */
	TIMER5_CLK_GATE_EN = (1 << 25),             /**<TIMER1 module clk gating switch */
	TIMER6_CLK_GATE_EN = (1 << 26),             /**<TIMER1 module clk gating switch */
	TIMER7_CLK_GATE_EN = (1 << 27),             /**<TIMER1 module clk gating switch */
	TIMER8_CLK_GATE_EN = (1 << 28),             /**<TIMER1 module clk gating switch */
	LEDC_CLK_GATE_EN = (1 << 29),               /**<LED control module clk gating switch */
	FDMA_CLK_GATE_EN = (1 << 30),               /**<SPDIF module clk gating switch */
	ALL_MODULE_CLK_GATE_SWITCH = (0x7FFFFFFF), /**<all module clk gating SWITCH*/
} CLOCK_MODULE_GATE_SWITCH;


typedef enum __CLK_MODE{
	RC_CLK_MODE,
	PLL_CLK_MODE,
	SYSTEM_CLK_MODE /*Fpll / 4 / SYS DIV NUM*/
}CLK_MODE;


typedef enum __MCLK_CLK_SEL{
	PLL_GEN_12M288,		/**PLL自动生成12.288M*/
	PLL_GEN_11M2896,	/**PLL自动生成11.2896M*/
	PLL_GPIO_IN,		/**GPIO输入*/
	PLL_USER_CONFIG,	/**用户手动配置PLL分频比*/
}MCLK_CLK_SEL;

/**
 * @brief	系统参考时钟源配置选择
 * @param	IsOsc TURE：晶体时钟；FALSE：XIN端口外灌方波时钟
 * @param   Freq  系统参考时钟的工作频率：32.768K,1M,2M,...40M,单位HZ
 * @return	系统工作频率
 * @note	如果使用PLL时钟则必须要先调用该函数
 */
void Clock_Config(bool IsOsc, uint32_t Freq);

/**
 * @brief	获取系统工作的系统时钟频率
 * @param	无
 * @return	系统工作频率
 */
uint32_t Clock_SysClockFreqGet(void);
/**
 * @brief  设置系统时钟分频系数
 * @param  DivVal [0-256]，0关闭分频功能, 1不分频
 * @return 无
 */
void Clock_SysClkDivSet(uint32_t DivVal);
    
/**
 * @brief  获取系统分频系数
 * @param  NONE
 * @return  系统分频系数[1-256]
 */
uint32_t Clock_SysClkDivGet(void);

/**
 * @brief  设置APB总线分频系数，基于系统总线分频
 * @param  DivVal [2-16]
 * @return 无
 */
void Clock_ApbClkDivSet(uint32_t DivVal);

/**
 * @brief  获取APB总线分频系数，基于系统总线分频
 * @param  无
 * @return  APB总线分频系数[2-12]
 */
uint32_t Clock_ApbClkDivGet(void);
    
/**
 * @brief	获取rc频率
 * @param	IsReCount 是否再次获取硬件计数器值。TRUE：再次启动硬件计数器。FALSE：获取上次记录值。
 * @return  rc频率
 */	
uint32_t Clock_RcFreqGet(bool IsReCount);

/**
 * @brief	设置pll工作频率,等待pll lock
 * @param	PllFreq pll频率,单位KHz[240000K-480000K]
 * @return  PLL锁定情况  TRUE:按设定目标频率锁定
 */
bool Clock_PllLock(uint32_t PllFreq);

/**
 * @brief	设置pll工作频率,快速锁定模式，等待pll lock
 * @param	PllFreq pll频率,单位KHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @param	Slope [0-16777216]
 * @return  PLL锁定情况  TRUE:按设定目标频率锁定
 */
bool Clock_PllQuicklock(uint32_t PllFreq, uint8_t K1, uint8_t OS, uint32_t NDAC, uint32_t Slope);

/**
 * @brief	设置pll工作频率,开环锁定模式(不需要晶体，无自校准)
 * @param	PllFreq pll频率,单位KHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @return  无
 */
void Clock_PllFreeRun(uint32_t PllFreq, uint32_t K1, uint32_t OS, uint32_t NDAC);

/**
 * @brief	设置pll工作频率,开环锁定模式(不需要晶体，无自校准)，参数来源于Efuse
 * @param	无
 * @return  Efuse值配置有效，返回TRUE
 */
bool Clock_PllFreeRunEfuse(void);

/**
 * @brief	获取pll频率
 * @param	无
 * @return  pll频率
 * @note    该函数调用需要应用确保pll已经lock，否则读数不准
 */
uint32_t Clock_PllFreqGet(void);

/**
 * @brief	pll模块关闭
 * @param	无
 * @return  无
 */
void Clock_PllClose(void);

/**
 * @brief	选择系统工作时钟
 * @param	ClkMode, 0: RC12M时钟; 1:pll时钟; 
 * @return  是否成功切换系统时钟，TRUE：工程切换；FALSE：切换时钟失败。
 * @note    系统时钟工作于pll时钟时，为pll时钟的4分频
 */
bool Clock_SysClkSelect(CLK_MODE ClkMode);

/**
 * @brief	Uart模块时钟选择pll时钟还是RC时钟
 * @param	ClkMode 0: RC时钟; 1:pll时钟
 * @return  无
 */
void Clock_UARTClkSelect(CLK_MODE ClkMode);

/**
 * @brief	USB模块时钟选择,需要提供60M时钟。基于PLL时钟7分频还是8分频
 * @param	ClkMode 0: pll8分频（pll锁定480M）; 1:pll时钟7分频（pll锁定420M）
 * @return  无
 */
void Clock_USBClkSelect(uint32_t ClkMode);

/**
 * @brief	Timer3模块时钟选择,系统时钟时钟还是RC时钟
 * @param	ClkMode 0: 系统时钟; 1:RC 12M时钟
 * @return  无
 */
void Clock_Timer3ClkSelect(uint32_t ClkMode);

/**
 * @brief	Timer4模块时钟选择,系统时钟时钟还是RC时钟
 * @param	ClkMode 0: 系统时钟; 1:RC 12M时钟
 * @return  无
 */
void Clock_Timer4ClkSelect(uint32_t ClkMode);

/**
 * @brief	SarADC模块时钟选择
 * @param	ClkMode 0: MCLK时钟; 1:SarADC时钟
 * @return  无
 * @note    Mclk时钟和I2s共有,一般用于MIC采样时；
 * 			SarADC时钟为独用时钟，最高可到30M，仅用于DC采样情况下。请配合Clock_SarADCClkSelect()函数
 */
void Clock_SarADCClkSelect(uint32_t ClkMode);

/**
 * @brief	SarADC模块时钟分频比配置，即时钟频率选择
 * @param	DivNum 分频比选择[7-63]
 * @return  分频配置是否成功。TRUE：配置成功；FALSE：配置失败
 * @note    配置目标的频率为：PLL_frequency/2(DivNum+1)；PLL_frequency为当前PLL频率
 */
bool Clock_SarADCClkDivSet(uint32_t DivNum);

/**
 * @brief	模块时钟使能
 * @param	ClkSel 模块源，根据CLOCK_MODULE_SWITCH选择
 * @return  无
 */
void Clock_ModuleEnable(CLOCK_MODULE_SWITCH ClkSel);

/**
 * @brief	模块时钟禁能
 * @param	ClkSel 模块源，根据CLOCK_MODULE_SWITCH选择
 * @return  无
 */
void Clock_ModuleDisable(CLOCK_MODULE_SWITCH ClkSel);

/**
 * @brief	子模块时钟动态自动关闭功能使能，将降低动态功耗
 * @param	ClkGateSel 模块源，根据CLOCK_MODULE_GATE_SWITCH选择
 * @return  无
 */
void Clock_ModuleGateEnable(CLOCK_MODULE_GATE_SWITCH ClkGateSel);

/**
 * @brief	子模块时钟动态自动关闭功能禁能，将降低动态功耗
 * @param	ClkGateSel 模块源，根据CLOCK_MODULE_GATE_SWITCH选择
 * @return  无
 */
void Clock_ModuleGateDisable(CLOCK_MODULE_GATE_SWITCH ClkGateSel);

/**
 * @brief	I2S&Mic, Master Clock选择配置
 * @param	ClkSel 时钟选择类型
 * @param	DivNum PLL分频比，只在ClkSel为3时有效，高8bit为整数部分，低8bit为小数部分
 * @return  无
 */
void Clock_MclkConfig(MCLK_CLK_SEL ClkSel, uint16_t DivNum);

/**
 * @brief	I2S&Mic, 采样率微调
 * @param	Sign  0：调慢；1：调快
 * @param	Ppm 时钟微调参数，为0时硬件时钟无微调功能。
 * @return  无
 */
void Clock_MclkAdjust(uint8_t Sign, uint8_t Ppm);

/**
 * @brief	RCCnt模块时钟源选择
 * @param	ClkSel  0：32K时钟；1：xMHz时钟
 * @return  无
 * @note	谨慎使用，如有疑问请咨询MV工程师
 */
void Clock_RCCntClockSelect(uint32_t ClkSel);

/**
 * @brief	RCCnt模块时钟源选择
 * @param	ClkSel  0：32K时钟；1：RC32k时钟; 2：xMHz时钟
 * @return  无
 * @note	谨慎使用，如有疑问请咨询MV工程师
 */
void Clock_LPClockSelect(uint32_t ClkSel);

/**
 * @brief	 开启检测OSC32K是否停振功能
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void Clock_DetectOSC32KEnable(void);

/**
 * @brief	 关闭检测OSC32K是否停振功能
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void Clock_DetectOSC32KDisable(void);

/**
 * @brief	 关闭 OSC12M晶振.
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void Clock_OSC12MDisable(void);

/**
 * @brief	 使能动态控制clock的gate功能，以节省功耗
 *
 * @param
 *   		NONE
 * @return
 *   		NONE
 * @note
 */
void Clock_DynamicClockGateEnable(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
 
