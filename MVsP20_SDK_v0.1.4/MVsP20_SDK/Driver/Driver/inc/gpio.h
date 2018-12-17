/******************************************************************************
 * @file    gpio.h
 * @author  Cecilia Wang
 * @version V1.0.0
 * @date    4-May-2015
 * @brief   gpio register description and function declare
 * @TypicalAppication
 *   Gpio can be configured as general io pin also be reused for other pin.
 *   They are configured as general io pin using the following functions:
 *   1.Digital output:  
	--Disable input function: Clr GPIO_A_IE, GPIO_B_IE
		--Enable Output function: Set GPIO_A_OE, GPIO_B_OE
		--Strong or week output:  Set or clr GPIO_A_OUTDS, GPIO_B_OUTDS
		--Pull up or down function: Set or clr {PU,PD} 
		--strong or week pull: Set or clr GPIO_A_DS, GPIO_B_DS
		--Output High level or Low level: Set or clr GPIO_A_OUT, GPIO_B_OUT 
		
	 2.Digital input:
		--Disable output function: Clr GPIO_A_OE, GPIO_B_OE
		--Enable input function: Set GPIO_A_IE, GPIO_B_IE
		--Pull up or down function: Set or clr {PU,PD} 
		--strong or week pull: Set or clr GPIO_A_DS, GPIO_B_DS
		--Read input status
	 
	 3.Analog input:
		--Clr PU: Clr GPIO_A_PU, GPIO_B_PU
		--Set PD: Set GPIO_A_PD, GPIO_B_PD
		--Clr OE: Clr GPIO_A_OE, GPIO_B_OE
		--Clr IE: Clr GPIO_A_IE, GPIO_B_IE
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 * 
 */

/**
 * @addtogroup GPIO
 * @{
 * @defgroup gpio gpio.h
 * @{
 */
 
#ifndef __GPIO_H__
#define	__GPIO_H__

#include "type.h"


#define     RESTORE_TO_GENERAL_IO     			   			(0xFF)  /**<this mcaro will be used as a para value to restore the multiplex io to general io*/

// Bank A
#define		GPIO_A_IN							(0x00)	/**<GPIOA input data register macro*/
#define		GPIO_A_OUT							(0x01)	/**<GPIOA output data register macro*/
#define     GPIO_A_SET							(0x02)
#define     GPIO_A_CLR							(0x03)
#define     GPIO_A_TGL							(0x04)
#define		GPIO_A_IE							(0x05)	/**<GPIOA input enable register macro.0: disable, 1: enable*/
#define		GPIO_A_OE							(0x06)	/**<GPIOA output enable register macro.0: disable, 1: enable*/
#define		GPIO_A_DS							(0x07)	/**<GPIOA pull-up/pull-down drive strength(when GPIOA input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_A_OUTDS						(0x08)	/**<GPIOA digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_A_PU							(0x09)	/**<GPIOA pull up register macro*/
#define		GPIO_A_PD							(0x0a)	/**<GPIOA pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_A_PULLDOWN1					(0x0b)	/**<GPIOA pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_A_PULLDOWN2					(0x0c)	/**<GPIOA pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/

#define     GPIOA0          					((uint32_t)1 << 0)	/**<GPIOA bit0 macro*/
#define     GPIOA1          					((uint32_t)1 << 1)	/**<GPIOA bit1 macro*/
#define     GPIOA2          					((uint32_t)1 << 2)	/**<GPIOA bit2 macro*/
#define     GPIOA3          					((uint32_t)1 << 3)	/**<GPIOA bit3 macro*/
#define     GPIOA4          					((uint32_t)1 << 4)	/**<GPIOA bit4 macro*/
#define     GPIOA5         	 					((uint32_t)1 << 5)	/**<GPIOA bit5 macro*/
#define     GPIOA6          					((uint32_t)1 << 6)	/**<GPIOA bit6 macro*/
#define     GPIOA7          					((uint32_t)1 << 7)	/**<GPIOA bit7 macro*/
#define     GPIOA8          					((uint32_t)1 << 8)	/**<GPIOA bit8 macro*/
#define     GPIOA9          					((uint32_t)1 << 9)	/**<GPIOA bit9 macro*/
#define     GPIOA10         					((uint32_t)1 << 10)	/**<GPIOA bit10 macro*/
#define     GPIOA11         					((uint32_t)1 << 11)	/**<GPIOA bit11 macro*/
#define     GPIOA12         					((uint32_t)1 << 12)	/**<GPIOA bit12 macro*/
#define     GPIOA13         					((uint32_t)1 << 13)	/**<GPIOA bit13 macro*/
#define     GPIOA14         					((uint32_t)1 << 14)	/**<GPIOA bit14 macro*/
#define     GPIOA15         					((uint32_t)1 << 15)	/**<GPIOA bit15 macro*/
#define     GPIOA16         					((uint32_t)1 << 16)	/**<GPIOA bit16 macro*/
#define     GPIOA17         					((uint32_t)1 << 17)	/**<GPIOA bit17 macro*/
#define     GPIOA18         					((uint32_t)1 << 18)	/**<GPIOA bit18 macro*/
#define     GPIOA19         					((uint32_t)1 << 19)	/**<GPIOA bit19 macro*/
#define     GPIOA20         					((uint32_t)1 << 20)	/**<GPIOA bit20 macro*/
#define     GPIOA21         					((uint32_t)1 << 21)	/**<GPIOA bit21 macro*/
#define     GPIOA22         					((uint32_t)1 << 22)	/**<GPIOA bit22 macro*/
#define     GPIOA23         					((uint32_t)1 << 23)	/**<GPIOA bit23 macro*/
#define     GPIOA24         					((uint32_t)1 << 24)	/**<GPIOA bit24 macro*/
#define     GPIOA25         					((uint32_t)1 << 25)	/**<GPIOA bit25 macro*/
#define     GPIOA26         					((uint32_t)1 << 26)	/**<GPIOA bit26 macro*/
#define     GPIOA27         					((uint32_t)1 << 27)	/**<GPIOA bit27 macro*/
#define     GPIOA28         					((uint32_t)1 << 28)	/**<GPIOA bit28 macro*/
#define     GPIOA29        						((uint32_t)1 << 29)	/**<GPIOA bit29 macro*/
#define     GPIOA30        						((uint32_t)1 << 30)	/**<GPIOA bit30 macro*/
#define     GPIOA31         					((uint32_t)1 << 31)	/**<GPIOA bit31 macro*/

// Bank B
#define		GPIO_B_IN							(0x00+0x0d)	/**<GPIOB input data register macro*/
#define		GPIO_B_OUT							(0x01+0x0d)	/**<GPIOB output data register macro*/
#define     GPIO_B_SET							(0x02+0x0d)         
#define     GPIO_B_CLR							(0x03+0x0d)         
#define     GPIO_B_TGL							(0x04+0x0d)         
#define		GPIO_B_IE							(0x05+0x0d)	/**<GPIOB input enable register macro.0: disable, 1: enable*/
#define		GPIO_B_OE							(0x06+0x0d)	/**<GPIOB output enable register macro.0: disable, 1: enable*/
#define		GPIO_B_DS							(0x07+0x0d)	/**<GPIOB pull-up/pull-down drive strength(when GPIOA input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_B_OUTDS						(0x08+0x0d)	/**<GPIOB digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_B_PU							(0x09+0x0d)	/**<GPIOB pull up register macro, NOTE: It's no use for GPIOB1~GPIOB6 & GPIO8~GPIO13*/
#define		GPIO_B_PD							(0x0a+0x0d)	/**<GPIOB pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_B_PULLDOWN1					(0x0b+0x0d)	/**<GPIOB pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_B_PULLDOWN2					(0x0c+0x0d)	/**<GPIOB pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/


#define     GPIOB0          					((uint32_t)1 << 0)	/**<GPIOB bit0 macro*/
#define     GPIOB1          					((uint32_t)1 << 1)	/**<GPIOB bit1 macro*/
#define     GPIOB2          					((uint32_t)1 << 2)	/**<GPIOB bit2 macro*/
#define     GPIOB3          					((uint32_t)1 << 3)	/**<GPIOB bit3 macro*/
#define     GPIOB4          					((uint32_t)1 << 4)	/**<GPIOB bit4 macro*/
#define     GPIOB5         	 					((uint32_t)1 << 5)	/**<GPIOB bit5 macro*/
#define     GPIOB6          					((uint32_t)1 << 6)	/**<GPIOB bit6 macro*/
#define     GPIOB7          					((uint32_t)1 << 7)	/**<GPIOB bit7 macro*/
#define     GPIOB8          					((uint32_t)1 << 8)	/**<GPIOB bit8 macro*/
#define     GPIOB9          					((uint32_t)1 << 9)	/**<GPIOB bit9 macro*/
#define     GPIOB10         					((uint32_t)1 << 10)	/**<GPIOB bit10 macro*/
#define     GPIOB11         					((uint32_t)1 << 11)	/**<GPIOB bit11 macro*/
#define     GPIOB12         					((uint32_t)1 << 12)	/**<GPIOB bit12 macro*/
#define     GPIOB13         					((uint32_t)1 << 13)	/**<GPIOB bit13 macro*/
#define     GPIOB14         					((uint32_t)1 << 14)	/**<GPIOB bit14 macro*/
#define     GPIOB15         					((uint32_t)1 << 15)	/**<GPIOB bit15 macro*/
#define     GPIOB16         					((uint32_t)1 << 16)	/**<GPIOB bit16 macro*/
#define     GPIOB17         					((uint32_t)1 << 17)	/**<GPIOB bit16 macro*/
#define     GPIOB18         					((uint32_t)1 << 18)	/**<GPIOB bit16 macro*/
#define     GPIOB19         					((uint32_t)1 << 19)	/**<GPIOB bit16 macro*/
#define     GPIOB20         					((uint32_t)1 << 20)	/**<GPIOB bit16 macro*/
#define     GPIOB21         					((uint32_t)1 << 21)	/**<GPIOB bit21 macro*/
#define     GPIOB22         					((uint32_t)1 << 22)	/**<GPIOB bit22 macro*/
#define     GPIOB23         					((uint32_t)1 << 23)	/**<GPIOB bit23 macro*/
#define     GPIOB24         					((uint32_t)1 << 24)	/**<GPIOB bit24 macro*/
#define     GPIOB25         					((uint32_t)1 << 25)	/**<GPIOB bit25 macro*/
#define     GPIOB26         					((uint32_t)1 << 26)	/**<GPIOB bit26 macro*/
#define     GPIOB27         					((uint32_t)1 << 27)	/**<GPIOB bit27 macro*/
#define     GPIOB28         					((uint32_t)1 << 28)	/**<GPIOB bit28 macro*/
#define     GPIOB29        						((uint32_t)1 << 29)	/**<GPIOB bit29 macro*/
#define     GPIOB30        						((uint32_t)1 << 30)	/**<GPIOB bit30 macro*/
#define     GPIOB31         					((uint32_t)1 << 31)	/**<GPIOB bit31 macro*/

// Bank C
#define		GPIO_C_IN							(0x00+0x1a)	/**<GPIOC input data register macro*/
#define		GPIO_C_OUT							(0x01+0x1a)	/**<GPIOC output data register macro*/
#define     GPIO_C_SET							(0x02+0x1a)         
#define     GPIO_C_CLR							(0x03+0x1a)         
#define     GPIO_C_TGL							(0x04+0x1a)         
#define		GPIO_C_IE							(0x05+0x1a)	/**<GPIOC input enable register macro.0: disable, 1: enable*/
#define		GPIO_C_OE							(0x06+0x1a)	/**<GPIOC output enable register macro.0: disable, 1: enable*/
#define		GPIO_C_DS							(0x07+0x1a)	/**<GPIOC pull-up/pull-down drive strength(when GPIOA input or output) register macro. 0: weak pull(20uA), 1: strong pull(70uA)*/
#define		GPIO_C_OUTDS						(0x08+0x1a)	/**<GPIOC digital ouput driving strength select register macro.OUTDS=0 : 8mA,OUTDS=1 : 24mA*/
#define		GPIO_C_PU							(0x09+0x1a)	/**<GPIOC pull up register macro*/
#define		GPIO_C_PD							(0x0a+0x1a)	/**<GPIOC pull down register macro.{pu,pd} 2'b00: pull-up, 2'b01: AIN, 2'b10: no pull-up, no pull-down, 2'b11: pull-down*/
#define		GPIO_C_PULLDOWN1					(0x0b+0x1a)	/**<GPIOC pull down current register macro.0: 0mA pull-down current, 1: 1.7mA pull-down current*/
#define		GPIO_C_PULLDOWN2					(0x0c+0x1a)	/**<GPIOC pull down current register macro.0: 0mA pull-down current, 1: 2.4mA pull-down current*/

#define     GPIOC0          					((uint32_t)1 << 0)	/**<GPIOC bit0 macro*/
#define     GPIOC1          					((uint32_t)1 << 1)	/**<GPIOC bit1 macro*/
#define     GPIOC2          					((uint32_t)1 << 2)	/**<GPIOC bit2 macro*/
#define     GPIOC3          					((uint32_t)1 << 3)	/**<GPIOC bit3 macro*/
#define     GPIOC4          					((uint32_t)1 << 4)	/**<GPIOC bit4 macro*/
#define     GPIOC5         	 					((uint32_t)1 << 5)	/**<GPIOC bit5 macro*/
#define     GPIOC6          					((uint32_t)1 << 6)	/**<GPIOC bit6 macro*/
#define     GPIOC7          					((uint32_t)1 << 7)	/**<GPIOC bit7 macro*/
#define     GPIOC8          					((uint32_t)1 << 8)	/**<GPIOC bit8 macro*/
#define     GPIOC9          					((uint32_t)1 << 9)	/**<GPIOC bit9 macro*/
#define     GPIOC10         					((uint32_t)1 << 10)	/**<GPIOC bit10 macro*/
#define     GPIOC11         					((uint32_t)1 << 11)	/**<GPIOC bit11 macro*/

//INT
//#define 	GPIO_GLB_INTE		(0x27)

#define GPIO_A_SEP_INTE 		(0x28+0)/**<GPIOA interrupt inte register macro, using it with functions(GPIO_INTEnable, GPIO_INTDisable, GPIO_ClearINTFlag, GPIO_GetINTFlag)as the first para*/
#define GPIO_A_SEP_ETRIG		(0x28+1)
#define GPIO_A_SEP_PTRIG		(0x28+2)
#define GPIO_A_SEP_INTC			(0x28+3)
#define GPIO_A_SEP_INTS			(0x28+4)

#define	GPIO_B_SEP_INTE			(0x2d+0)	/**<GPIOB interrupt inte register macro, using it with functions(GPIO_INTEnable, GPIO_INTDisable, GPIO_ClearINTFlag, GPIO_GetINTFlag)as the first para*/
#define GPIO_B_SEP_ETRIG		(0x2d+1)
#define GPIO_B_SEP_PTRIG		(0x2d+2)
#define GPIO_B_SEP_INTC			(0x2d+3)
#define GPIO_B_SEP_INTS			(0x2d+4)

#define	GPIO_C_SEP_INTE			(0x32+0)	/**<GPIOC interrupt inte register macro, using it with functions(GPIO_INTEnable, GPIO_INTDisable, GPIO_ClearINTFlag, GPIO_GetINTFlag)as the first para*/
#define GPIO_C_SEP_ETRIG		(0x32+1)
#define GPIO_C_SEP_PTRIG		(0x32+2)
#define GPIO_C_SEP_INTC			(0x32+3)
#define GPIO_C_SEP_INTS			(0x32+4)

#define	GPIO_A_INT	GPIO_A_SEP_INTE
#define	GPIO_B_INT	GPIO_B_SEP_INTE
#define	GPIO_C_INT	GPIO_C_SEP_INTE


#define     GPIO_NEG_EDGE_TRIGGER				(0)	/**<GPIO interrupt mode:negedge trigger, you can use it with function GPIO_INTEnable as the third para*/
#define     GPIO_POS_EDGE_TRIGGER				(1)	/**<GPIO interrupt mode:posedge trigger, you can use it with function GPIO_INTEnable as the third para*/
#define     GPIO_EDGE_TRIGGER                   (2)/**<GPIO interrupt mode:edge trigger, you can use it with function GPIO_INTEnable as the third para*/

/**
 *  define A0 pull down current
 */
typedef enum _ICS_VAL
{
    ICS_0MA0 = 0,	/**<pulldown current:0.0mA*/
    ICS_1MA7,		/**<pulldown current:1.7mA*/
    ICS_2MA4,		/**<pulldown current:2.4mA*/
    ICS_4MA1		/**<pulldown current:4.1mA*/
} ICS_VAL;

/**
 *  define A0 work as funtion
 */
typedef enum _A0_MODE_AF
{
    A0_FOR_GPIO = 0,	/**<normal gpio*/
    A0_FOR_ICS,			/**<pulldown current*/
    A0_FOR_CHARGER,		/**<led charger*/
    A0_FOR_PWM			/**<pwm*/

} A0_MODE_AF;


/**************************************************************************************************
                                 通用GPIO设置或获取状态部分
**************************************************************************************************/

//************************* 1、 只设置或读取 1 bit  *****************************//
/**
 * @brief      GPIO 复位源选择
 *
 * @param[in]  source 
			@arg 0，表示GPIO状态会被系统复位而复位；
			@arg 1表示GPIO状态只受电源复位而复位。
 *
 * @return     无
 */
void GPIO_ResetControl(uint8_t rst_source);

/**
 * @brief      只置位 1bit
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 * @param[in]  GpioIndex：GPIO bit位，例如：GPIOA0
 *
 * @return     无
 */
void GPIO_RegOneBitSet(uint32_t	RegIndex, uint32_t GpioIndex);

/**
 * @brief      只清零 1bit
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 * @param[in]  GpioIndex：GPIO bit位，例如：GPIOA0
 *
 * @return     无
 */
void GPIO_RegOneBitClear(uint8_t	RegIndex, uint32_t GpioIndex);

/**
 * @brief      读取 1bit数据
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_IN
 * @param[in]  GpioIndex：GPIO bit位，例如：GPIOA0
 *
 * @return     1 or 0
 */
bool GPIO_RegOneBitGet(uint8_t RegIndex, uint32_t GpioIndex);


//************************* 2、 设置或读取多 bit  ***************************** //

/**
 * @brief      置位多 bits
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 * @param[in]  mask：GPIO bit位，例如：GPIOA0 | GPIOB1
 *
 * @return     无
 */
void GPIO_RegBitsSet(uint8_t RegIndex, uint32_t mask);

/**
 * @brief      清除多 bits
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 * @param[in]  mask：GPIO bit位，例如：GPIOA0 | GPIOA1
 *
 * @return     无
 */
void GPIO_RegBitsClear(uint8_t RegIndex, uint32_t mask);


//************************* 3、 设置或读取一组  ***************************** //

/**
 * @brief      设置一组GPIO数据
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 * @param[in]  mask：GPIO bit位，例如：0XF123
 *
 * @return     无
 */
void GPIO_RegSet(uint8_t RegIndex, uint32_t Val);

/**
 * @brief      读取一组GPIO数据
 *
 * @param[in]  RegIndex: GPIO地址索引，例如： GPIO_A_OUT
 *
 * @return     GPIO值
 */
uint32_t GPIO_RegGet(uint8_t RegIndex);


//*************************** 4、  GPIO中断  ******************************* //

/**
 * @brief      使能GPIO某一bit的中断功能
 *
 * @param[in]  RegIndex: GPIO中断地址索引，例如： GPIO_A_INT
 * @param[in]  GpioIndex: bit位，例如GPIOA10
 * @param[in]  EdgeSel: 触发极性，例如GPIO_NEG_EDGE_TRIGGER触发
 *
 * @return     无
 */
void GPIO_INTEnable(uint8_t RegIndex, uint32_t GpioIndex, uint8_t EdgeSel);

/**
 * @brief      禁能GPIO某一bit的中断功能
 *
 * @param[in]  RegIndex: GPIO中断地址索引，例如： GPIO_A_INT
 * @param[in]  GpioIndex: bit位，例如GPIOA10
 *
 * @return     无
 */
void GPIO_INTDisable(uint8_t RegIndex, uint32_t GpioIndex);

/**
 * @brief      获取GPIO的中断状态
 *
 * @param[in]  RegIndex: GPIO中断地址索引，例如： GPIO_A_INT
 *
 * @return     一组GPIO的中断状态
 */
uint32_t GPIO_INTFlagGet(uint8_t RegIndex);

/**
 * @brief      清除某一位GPIO的中断状态
 *
 * @param[in]  RegIndex: GPIO中断地址索引，例如： GPIO_A_INT
 * @param[in]  GpioIndex: GPIO bit位
 * @return     无
 */
void GPIO_INTFlagClear(uint8_t RegIndex, uint32_t GpioIndex);


/**************************************************************************************************
                                 GPIO与其他功能的复用部分
**************************************************************************************************/
/**
 * @brief      GPIO复用为SPIM0引脚或还原为通用GPIO
 *
 * @param[in]  PortSel: 选择一组GPIO
 * <pre>
 *		---------------------------------------------------------
 *		|	ModeSel		|					|		GPIO		|
 *		|-------------------------------------------------------|
 *		|				|		miso		|		b[0]		|
 *		|		0		|		clk			|		b[1]		|
 *		|				|		mosi		|		b[2]		|
 *		|-------------------------------------------------------|
 *		|				|		miso		|		b[27]		|
 *		|		1		|		clk			|		b[28]		|
 *		|				|		mosi		|		b[29]		|
 *		|-------------------------------------------------------|
 *		|				|		miso		|		b[20]		|
 *		|		2		|		clk			|		b[21]		|
 *		|				|		mosi		|		b[22]		|
 *		|-------------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_Spim0IoConfig(uint8_t PortSel);


/**
 * @brief      使能禁止SPIMaster1
 *
 * @param[in]  PortSel: 从Which中选择一组GPIO
 * <pre>
 *		---------------------------------------------------------
 *		|	ModeSel		|	  				|		GPIO		|
 *		|-------------------------------------------------------|
 *		|				|		miso		|		c[0]		|
 *		|		1		|		clk			|		c[1]		|
 *		|				|		mosi		|		c[2]		|
 *		|-------------------------------------------------------|
 * </pre>
 *		0xff and 0: 禁止SPIMaster1
 * @return     无
 */
void GPIO_Spim1IoConfig(uint8_t PortSel);

/**
 * @brief      GPIO复用为SPIS引脚或还原为通用GPIO
 * @param[in]  ModeSel: 选择一组GPIO
 * <pre>
 *		|---------------------------------------------------|
 *		|	ModeSel		|					|		GPIO	|
 *		|---------------------------------------------------|
 *		|				|		miso		|		b[0]	|
 *		|		0		|		clk			|		b[1]	|
 *		|				|		mosi		|		b[2]	|
 *		|				|		cs			|		b[3]	|
 *		|---------------------------------------------------|
 *		|				|		miso		|		c[0]	|
 *		|		1		|		clk			|		c[1]	|
 *		|				|		mosi		|		c[2]	|
 *		|				|		cs			|		c[3]	|
 *		|---------------------------------------------------|
 *		|				|		miso		|		b[27]	|
 *		|		2		|		clk			|		b[28]	|
 *		|				|		mosi		|		b[29]	|
 *		|				|		cs			|		b[26]	|
 *		|---------------------------------------------------|
 *		|				|		miso		|		b[20]	|
 *		|		3		|		clk			|		b[21]	|
 *		|				|		mosi		|		b[22]	|
 *		|				|		cs			|		b[23]	|
 *		|---------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_SpisIoConfig(uint8_t ModeSel);

/**
 * @brief      GPIO复用为UART RX引脚或还原为通用GPIO
 *
 * @param[in]  Which: 选择一组端口
 * @param[in]  ModeSel: 从Which中选择一组GPIO
 * <pre>
 *		|----------------------------------------------------
 *		|	Which		|	ModeSel		|		GPIO		|
 *		|---------------------------------------------------|
 *		|		0		|		0		|		b[11]		|
 *		|				|		1		|		c[3]		|
 *		|---------------------------------------------------|
 *		|		1		|		0		|		a[1]		|
 *		|				|		1		|		a[29]		|
 *		|---------------------------------------------------|
 *		|		2		|		0		|		a[3]		|
 *		|				|		1		|		a[31]		|
 *		|---------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_UartRxIoConfig(int Which, uint8_t ModeSel);

/**
 * @brief      GPIO复用为UART TX引脚或还原为通用GPIO
 *
 * @param[in]  Which: 选择一组端口
 * @param[in]  ModeSel: 从Which中选择一组GPIO
 * <pre>
 *		|---------------------------------------------------|
 *		|	Which		|	ModeSel		|		GPIO		|
 *		|---------------------------------------------------|
 *		|		0		|		0		|		b[10]		|
 *		|				|		1		|		c[2]		|
 *		|---------------------------------------------------|
 *		|		1		|		0		|		a[0]		|
 *		|				|		1		|		a[28]		|
 *		|---------------------------------------------------|
 *		|		2		|		0		|		a[2]		|
 *		|				|		1		|		a[30]		|
 *		|---------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_UartTxIoConfig(int Which, uint8_t ModeSel);

/**
 * @brief      GPIO复用为UART CTS引脚或还原为通用GPIO
 *
 * @param[in]  Which: 选择一组端口
 * @param[in]  ModeSel: 从Which中选择一组GPIO
 * <pre> 
 *		|---------------------------------------------------|
 *		|	Which		|	ModeSel		|		GPIO		|
 *		|---------------------------------------------------|
 *		|		0		|		0		|		b[9]		|
 *		|				|		1		|		c[1]		|
 *		|---------------------------------------------------|
 *		|		1		|		0		|		a[3]		|
 *		|				|		1		|		a[27]		|
 *		|---------------------------------------------------|
 *		|		2		|		0		|		a[5]		|
 *		|				|		1		|		a[29]		|
 *		|---------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_UartCtsIoConfig(int Which, uint8_t ModeSel);


/**
 * @brief      GPIO复用为UART RTS引脚或还原为通用GPIO
 *
 * @param[in]  Which: 选择一组端口
 * @param[in]  ModeSel: 从Which中选择一组GPIO
 * <pre>
 *		|---------------------------------------------------|
 *		|	Which		|	ModeSel		|		GPIO		|
 *		|---------------------------------------------------|
 *		|		0		|		0		|		b[8]		|
 *		|				|		1		|		c[0]		|
 *		|---------------------------------------------------|
 *		|		1		|		0		|		a[2]		|
 *		|				|		1		|		a[26]		|
 *		|---------------------------------------------------|
 *		|		2		|		0		|		a[4]		|
 *		|				|		1		|		a[28]		|
 *		|---------------------------------------------------|
 * </pre>
 *      0xff: 还原为通用GPIO 
 * @return     无
 */
void GPIO_UartRtsIoConfig(int Which, uint8_t ModeSel);

/**
 * @brief      GPIO复用为SD引脚或还原为通用GPIO
 *
 * @param[in]  ModeSel：复用模式
 * <pre>
 *		|---------------------------------------------------|
 *		|	ModeSel		|				|		GPIO		|
 *		|---------------------------------------------------|
 *		|				|		dat		|		b[0]		|
 *		|		0		|		clk		|		b[1]		|
 *		|				|		cmd		|		b[2]		|
 *		|---------------------------------------------------|
 *		|				|		dat		|		c[0]		|
 *		|		1		|		clk		|		c[1]		|
 *		|				|		cmd		|		c[2]		|
 *		|---------------------------------------------------|
 *		|				|		dat		|		b[27]		|
 *		|		2		|		clk		|		b[28]		|
 *		|				|		cmd		|		b[29]		|
 *		|---------------------------------------------------|
 *		|				|		dat		|		b[20]		|
 *		|		3		|		clk		|		b[21]		|
 *		|				|		cmd		|		b[22]		|
 *		|---------------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_SdIoConfig(uint8_t IOModeSel);

/**
 * @brief      GPIO复用为I2C引脚或还原为通用GPIO
 *
 * @param[in]  ModeSel：复用模式
 * <pre>
 *		|---------------------------------------------------|
 *		|	ModeSel		|				|		GPIO		|
 *		|---------------------------------------------------|
 *		|				|		scl		|		a[30]		|
 *		|		0		|		sda		|		a[31]		|
 *		|---------------------------------------------------|
 *		|				|		scl		|		b[30]		|
 *		|		1		|		sda		|		b[31]		|
 *		|---------------------------------------------------|
 *		|				|		scl		|		c[0]		|
 *		|		2		|		sda		|		c[1]		|
 *		|---------------------------------------------------|
 * </pre> 
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_I2cIoConfig(uint8_t ModeSel);

/**
 * @brief      GPIO复用为32k时钟输出引脚或还原为通用GPIO
 * @param[in]  ModeSel：复用模式
 *			@arg 0		clk_32k_out   a[3]
 *			@arg 0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_Clk32kOutIoConfig(uint8_t ModeSel);

/**
 * @brief      使能SW调试功能或禁能该功能
 * @param[in]  ModeSel：复用模式
 *		@arg 0   使能调试功能;sw_clk->a[0],sw_d->a[1]
 *		@arg 0xff 还原为通用GPIO
 * @return     无
 */
void GPIO_SwIoConfig(uint8_t ModeSel);


 /**
 * @brief     配置GPIO为MCLK引脚或恢复成GPIO引脚
 *
 * @param[in]  ModeSel：复用模式
 * <pre>
 *		|-------------------------------------------|
 *		|	ModeSel		|		GPIO	|	Dir		|
 *		|-------------------------------------------|
 *		|		0		|		b[7]	|	in		|
 *		|-------------------------------------------|
 *		|		1		|		b[20]	|	in		|
 *		|-------------------------------------------|
 *		|		2		|		b[7]	|	out		|
 *		|-------------------------------------------|
 *		|		3		|		b[20]	|	out		|
 *		|-------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_MclkIoConfig(uint8_t ModeSel);

 /**
 * @brief      复用GPIO为I2S BCLK, LRCLK引脚或恢复为GPIO引脚
 *
 * @param[in]  ModeSel：复用模式
 * <pre>
 *		|---------------------------------------------------|
 *		|	ModeSel		|				|		GPIO		|
 *		|---------------------------------------------------|
 *		|				|		lrclk	|		b[8]		|
 *		|		0		|		bclk	|		b[9]		|
 *		|---------------------------------------------------|
 *		|				|		lrclk	|		b[12]		|
 *		|		1		|		bclk	|		b[13]		|
 *		|---------------------------------------------------|
 * </pre> 
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_I2sBclkLrclkConfig(uint8_t ModeSel);

 /**
 * @brief      复用GPIO引脚为I2S Dout引脚或将I2S Dout引脚恢复成普通GPIO引脚
 *
 * @param[in]  ModeSel：复用模式
 *<pre>
 *		|-----------------------------------|
 *		|	ModeSel		|		GPIO		|
 *		|-----------------------------------|
 *		|		0		|		b[10]		|
 *		|-----------------------------------|
 *		|		1		|		b[14]		|
 *		|-----------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_I2sDoutConfig(uint8_t ModeSel);
 
  /**
 * @brief      复用GPIO引脚为I2S Din引脚或将I2S Din引脚恢复成普通GPIO引脚
 *
 * @param[in]  ModeSel：复用模式
 *<pre>
 *		|-----------------------------------------------|
 *		|	ModeSel		|		I2S_LRCK	I2S_BCLK
 *		|-----------------------------------------------|
 *		|		0		|		b[8]		b[9]        |
 *		|-----------------------------------------------|
 *		|		1		|		b[12]		b[13]   	|
 *		|-----------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */
void GPIO_I2sDinConfig(uint8_t ModeSel);


 /**
 * @brief      sdio复用关系
 *
 * @param[in]  ModeSel: 复用引脚选择
 * <pre> 
 *		|-----------------------------------------------|
 *		|	ModeSel		|		SD_DAT0	SD_CLK SD_CMD|
 *		|-----------------------------------------------|
 *		|		0		|		b[0]	b[1]	b[2] 	|
 *		|-----------------------------------------------|
 *		|		1		|		c[0]	c[1]	c[2] 	|
 *		|-----------------------------------------------|
 *		|		2		|		b[27]	b[28]	b[29]|
 *		|-----------------------------------------------|
 *		|		3		|		b[20]	b[21]	b[22]	|
 *		|-----------------------------------------------|
 * </pre>
 *		0xff: 还原为通用GPIO
 * @return     无
 */

void GPIO_SDIOConfig(uint8_t ModeSel);
/***************************************************************************************
                                其他功能
****************************************************************************************/
/**
* @Brief	 GPIO复用关系检查
* @param[in] 传入32个字长度的数组指针，对应GPIOA口32bit的复用关系
* @param[in] 传入32个字长度的数组指针，对应GPIOB口32bit的复用关系
* @param[in] 传入32个字长度的数组指针，对应GPIOB口32bit的复用关系
* @return	无
*/
void GPIO_GpioReusedInfoGet(uint32_t*  GpioAReusedInfo,uint32_t*  GpioBReusedInfo,uint32_t*  GpioCReusedInfo);

#endif

/**
 * @}
 * @}
 */

