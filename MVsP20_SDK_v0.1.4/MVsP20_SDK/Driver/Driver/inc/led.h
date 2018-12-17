/*
 ******************************************************************************
 * @file    led.h
 * @author  Castle
 * @version V1.0.0
 * @date    2016/04/20
 * @brief	LED controler
 *
 * Changelog:
 *
 *
 *
 ******************************************************************************* 
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2014 MVSilicon </center></h2>
 */
 
/**
 * @addtogroup LEDC_FDMA
 * @{
 * @defgroup led led.h
 * @{
 */

#ifndef __LED_H__
#define __LED_H__

#define LED_LINE_SEL_A (0)
#define LED_LINE_SEL_B (1)
#define LED_LINE_SEL_C (2)
#define LED_LINE_SEL_D (3)

#define LED_CLKOUT0_SEL (1<<0)//ledc_clkout0 signal a[16]
#define LED_CLKOUT1_SEL (1<<1)//ledc_clkout1 signala[17]
#define LED_CLKOUT2_SEL (1<<2)//ledc_clkout2 signala[18]
#define LED_CLKOUT3_SEL (1<<3)//ledc_clkout3 signala[2]
#define LED_CLKOUT4_SEL (1<<4)//ledc_clkout4 signala[3]
#define LED_CLKOUT5_SEL (1<<5)//ledc_clkout5 signala[4]
#define LED_CLKOUT6_SEL (1<<6)//ledc_clkout6 signala[5]
#define LED_CLKOUT7_SEL (1<<7)//ledc_clkout7 signala[6]

/**
 * LED FIFO control command
 */
typedef enum
{
	LED_IOCTL_FIFO_UNDER = 1,		/**< get fifo is under flow */ 
	LED_IOCTL_FIFO_EMPTY = 2,		/**< get fifo is empty flow */ 
	LED_IOCTL_FIFO_FULL = 3,		/**< get fifo is full flow */ 
	LED_IOCTL_FIFO_OVER = 4,		/**< get fifo is over flow */ 
	LED_IOCTL_FIFO_UNDER_CLR = 5,	/**< clear fifo under flow flag*/ 
	LED_IOCTL_FIFO_OVER_CLR = 6,	/**< clear fifo over flow flag*/ 
	LED_IOCTL_FIFO_CFGERR = 7,			/**< get fifo configuration is error */ 
	LED_IOCTL_FIFO_CLR = 8,			/**< fifo clear register, the fifo and fifo state signal will cleared*/ 

	LED_IOCTL_OEN_VALID = 9,
}LED_IOCTRL_CMD;

typedef enum
{
	FDMA_INT_TYPE_INVALID = -127,
	FDMA_MODE_INVALID,	
	FDMA_UNALIGNED_ADDRESS,
	FDMA_OK = -1
}FDMA_ERROR;

typedef enum
{
	FDMA_DONE_INT = 0,
	FDMA_THRESHOLD_INT,
	FDMA_ERROR_INT
}FDMA_INT_TYPE;

typedef enum
{
	FDMA_BLOCK_MODE = 0,
	FDMA_CIRCULAR_MODE
}FDMA_MODE;

typedef struct	_LED_CONFIG
{
	
/*	TRUE:serial out; FALSE: parallel out*/	
	bool isSerialOut;
	
/*
	When isSerialOut set 1:
		0: pop one fifo data serial out to led_data[0]
		1: pop two fifo data serial out to led_data[1:0]
		2: pop three fifo data serial out to led_data[2:0]//data clock
		3: pop four fifo data serial out to led_data[3:0]
*/
	uint8_t serialNum;

/*
	dataClkOutCycle to set led_data_clk period
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		31:	32 sys_clk cycles
	Note: outCycle must lager than highTime
*/

	uint8_t dataClkOutCycle;
/*
	dataClkHighTime	to set led_data_clk high time
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		31:	32 sys_clk cycles
*/
	uint8_t dataClkHighTime;

/*
	dataClkPolarity	data clock polarity
		0: At posedge of led_data_clk to store data
		1: At negedge of led_data_clk to store data
*/
	uint8_t dataClkPolarity;
/*
	clkOutmode set clkout mode
		0: nomal mode
		1: cascade 595 mode
*/
	uint8_t clkOutmode;

/*
	clkOutPolarity	clock out polarity
		0: At posedge of led_data_clk to store data 
		1: At negedge of led_data_clk to store data 
*/
	uint8_t clkOutPolarity;

/*
	clkOutNum		led_clk_out number
		0: 1 led_clk_out output
		1: 2 led_clk_out output
		......
		7: 8 led_clk_out output
*/
	uint8_t clkOutNum;

/*
	clkOutHighTime  Control the led_clk_out high time
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		31:	32 sys_clk cycles
*/
	uint8_t clkOutHighTime;

/*
	OENPolarity	clock out polarity
		0: At posedge of led_data_clk to store data 
		1: At negedge of led_data_clk to store data 
*/
	uint8_t OENPolarity;

/*
	OENToStbTime  Control the led_clk_out high time
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		31:	32 sys_clk cycles 
*/			
	uint8_t OENToStbTime;

/*
	OENCnt		Control how many data will output one led_oen signal, 
	one data means one led_clk_out.In other word, it is led line number
		0: 0 led data
		1: 1 led data
		......
		1048575: 1048575 led data	
*/			
	uint32_t OENCnt;

/*
	OENValidTime  led oen high level valid time
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		1048575: 1048576 sys_clk cycles
*/
	uint32_t OENValidTime;

/*
	STBPolarity	set STB polarity
		0: high level of led_stb is valid
		1: low level of led_stb is valid 
*/
	uint8_t STBPolarity;

/*
	STBStbTime  Control the led_stb valid time
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		255:256 sys_clk cycles	
*/
	uint8_t STBValidTime;

/*
	STBStbTime  Control how many sys_clk led_stb will valid after one line data transfered
		0:	1 sys_clk cycle
		1:	2 sys_clk cycles
		......
		255:256 sys_clk cycles	
*/
	uint8_t STBStbTime;

/*
	lnSelMode	Control LED led_abcd mode
		0: binary increase
		1: one signal control one line, 
*/
	uint8_t lnSelMode;

/*
	lnSel  	when mode = 0
		0: single line scan
		1: 1/2 line scan
		2: 1/3 line scan
		 ...
		15: 1/16 line scan
		when led_abcd_mode=1
		0: single line scan
		1: 1/2 line scan
		2: 1/3 line scan
		3: 1/4 line scan
		other: single line scan
*/
	uint8_t lnSel;

/*
	fifoLvl fifo full level control
		when out_width=0: fifo_level must less than 32
		when out_width=1: fifo_level must less than 32
		when out_width=2: fifo_level must less than 16
		when out_width=3: fifo_level must less than 8
*/
	uint8_t fifoLvl;

/*
	inWidth		The width of input data for fifo, frequently used for DMA as source
		0: 8bit valid
		1: 16bit valid
		2: 32bit valid
		3: reserved
*/
	uint8_t inWidth;

/*
	outWidth	The width of LED pop data from fifo, Note: when isSerialOut = 1 in
				LED_SerialData_Config, the out_width must lager than 0;
		0: 4bit valid
		1: 8bit valid
		2: 16bit valid
		3: 32bit valid
*/
	uint8_t outWidth;
} LED_CONFIG;



/**
 * @brief
 *   The LED_DataCLKIOConfig() function to Enable ledc_data_clk signal 
 * on GPIOA[18]
 *
 * @param	enable	0:disable
 					1:enable
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_DataCLKIOConfig(uint8_t enable);

/**
 * @brief
 *   The LED_STBIOConfig() function to Enable/Disable ledc_stb signal
 *  and select which gpio port for ledc_stb out.
 *
 * @param	PortSel	0:   	a[16]
 *					1:   	a[28]
 *					other: 	disable
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_STBIOConfig(uint8_t PortSel);

/**
 * @brief
 *   The LED_OENIOConfig() function to Enable/Disable ledc_oe signal 
 * and select which gpio port for ledc_oe out.
 *
 * @param	PortSel	0:   	a[17]
 *					1:   	a[29]
 *					other: 	disable
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_OENIOConfig(uint8_t PortSel);


/**
 * @brief
 *   The LED_LineSelIOConfig() function to Enable/Disable ledc_lineout_a/ledc_lineout_b
 * ledc_lineout_c/ledc_lineout_d signal and select which gpio port for each ledc_lineout out.
 *
 * @param	LineSel	is one of LED_LINE_SEL_A, LED_LINE_SEL_B, LED_LINE_SEL_C, LED_LINE_SEL_D, 
 *
 * @param	PortSel	
 *					for LED_LINE_SEL_A		0:   	a[24]
 *											1:   	b[8]
 *											2:		b[20]
 *											other: 	disable
 *					for LED_LINE_SEL_B		0:   	a[25]
 *											1:   	b[9]
 *											2:		b[21]
 *											other: 	disable
 *					for LED_LINE_SEL_C		0:   	a[26]
 *											1:   	b[10]
 *											2:		b[22]
 *											other: 	disable
 *					for LED_LINE_SEL_D		0:   	a[27]
 *											1:   	b[11]
 *											2:		b[23]
 *											other: 	disable
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_LineSelIOConfig(uint8_t LineSel, uint8_t PortSel);

/**
 * @brief
 *   The LED_CLKOutIOConfig() function to Enable/Disable ledc_clkoutX signal 
 *
 * @param	clkOutSel	refer to LED_CLKOUT0_SEL, LED_CLKOUT1_SEL, LED_CLKOUT2_SEL.....LED_CLKOUT7_SEL
 *
 * @param	enable		0:   	disable
 *						1:   	enable
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_CLKOutIOConfig(uint8_t clkOutSelMask);

/**
 * @brief
 *   The LED_DATAOutIOConfig() function to Enable/Disable ledc_datX signal
 *  We have totally 32 signal lines for output data begin from GPIOB[0], and every
 *	single bit on dataOutConfig show Enable/Disable of one signal line of output data.
 *
 * @param	dataOutConfig  	0x0001:	enable output data line 0
 *							0x0002: enable output data line 1
 *							.......
 *
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
void LED_DataOutIOConfig(uint32_t dataOutConfig);

/******LEDC Driver*****/

/**
 * @brief
 *   The LED_Init() function to initialization the LED control module
 *
 * @param	NONE
 *
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
bool LED_Init(void);


/**
 * @brief
 *   The LED_Config() function to Config all parameters for LED display
 *
 * @param	cfg refer to LED_CONFIG
 *
 * @return
 *   TRUE for success, FALSE for failed.
 *
 * ERRORS
 *   none
 * @note
 */
bool LED_Config(LED_CONFIG *cfg);


/**
 * @brief
 *   The LED_Ctrl_Enable() function to Enable/Disable LED controler
 *
 * @param	Enable	0:   	Disable LED controler
 *					1:   	Enable LED controler
 *
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
bool LED_Ctrl_Enable(uint8_t Enable);


/**
 * @brief
 *   The LED_FIFO_DMA_Enable() function to Enable/Disable interface between LEDC and FDMA
 *
 * @param	Enable	0:   	Disable 
 *					1:   	Enable 
 *
 * @return
 *   none
 *
 * ERRORS
 *   none
 * @note
 */
bool LED_FIFO_DMA_Enable(uint8_t Enable);


/**
 * @brief
 *   The LED_OutPutData() function to output data to LED fifo
 *
 * @param	data
 *
 * @return
 *	TRUE:	output data successfully
 *	FALSE:	FIFO is full, over full, output data failed		
 *
 * ERRORS
 *   none
 * @note
 */
bool LED_OutPutData(uint32_t data);


/**
 * @brief
 *   The LED_IOCtl() function control LED action including but not limited to configration,
 * interrupt and other misc settings by the command of Cmd with parameter referred by arg.
 *
 * @param	cmd	I/O control command
 * @param	arg	the control command argument if the command require
 *
 * @return
 *   Upon successful completion, LED_IOCtl() function shall return the result like
 * LED_IOCTL_XXXX_GET(such as LED_IOCTL_FIFO_ERR_GET) or return zero like
 * LED_IOCTL_XXXX_SET(such as LED_IOCTL_FIFO_CLR_SET).
 * Otherwise,a negative error code will be returned.
 *
 * ERRORS
 *   ENOSYS The command is not available currently.
 *   EINVAL Invalid parameter,especially for LED_IOCTL_XXXX_SET.
 * @note
 */
int32_t LED_IOCtl(LED_IOCTRL_CMD cmd, uint32_t arg);

/********************************************FDMA Drivers*****************************************************/

/**
 * @brief	Enable/Disable current FDMA interrupt
 * @param	IntType				type of interrupt, refer to FDMA_INT_TYPE
 * @param	En					1:enable    0:disable
 *								 
 * @return
 *   		FDMA_ERROR
 * @note
 */
FDMA_ERROR FDMA_InterruptEn(FDMA_INT_TYPE IntType, bool En);

/**
 * @brief	Get the current FDMA interrupt enable/disable
 * @param	IntType				type of interrupt, refer to FDMA_INT_TYPE
 *								 
 * @return
 *   		return the interrupt enable(1 for enable, 0 for disable), or FDMA_ERROR
 * @note
 */
int32_t FDMA_GetInterruptEn(FDMA_INT_TYPE IntType);

/**
 * @brief	Get the current FDMA interrupt flag
 * @param	IntType				type of interrupt, refer to FDMA_INT_TYPE
 *								 
 * @return
 *   		return the interrupt flag, or FDMA_ERROR
 * @note
 */
int32_t FDMA_GetInterruptFlag(FDMA_INT_TYPE IntType);

/**
 * @brief	Clear the current FDMA interrupt flag
 * @param	IntType				type of interrupt, refer to FDMA_INT_TYPE
 *								 
 * @return
 *   		FDMA_ERROR
 * @note
 */
FDMA_ERROR FDMA_InterruptFlagClr(FDMA_INT_TYPE IntType);

/**
 * @brief	Configurate the FDMA with block mode
 * @param	srcDwidth		source data width for each read from memory
 *							0: byte, 
 * 							1: half word, 
 *							2: word
 * @param	dstDwidth		destination data width for each write to LED controler
 *							0: byte, 
 * 							1: half word, 
 *							2: word
 * @param	Ptr				the address for memory buffer
 * @param	Len				the length for memory buffer
 
 * @return
 *   		FDMA_ERROR
 * @note	Ptr must be aligned with srcDwidth, and Len must be aligned with srcDwidth or dstDwidth,
 *			otherwise will encounter an FDMA_UNALIGNED_ADDRESS
 */
FDMA_ERROR FDMA_BlockConfig(uint8_t srcDwidth, uint8_t dstDwidth, void* Ptr, uint16_t Len);

/**
 * @brief	Get the done lenght with block mode
 *
 * @return
 *   		Done length or FDMA_MODE_INVALID if current FDMA is not block mode
 * @note	
 *			
 */
uint16_t FDMA_BlockGetDoneLen(void);

/**
 * @brief	Configurate the FDMA with circular mode
 * @param	srcDwidth		source data width for each read from memory
 *							0: byte, 
 * 							1: half word, 
 *							2: word
 * @param	dstDwidth		destination data width for each write to LED controler
 *							0: byte, 
 * 							1: half word, 
 *							2: word
 * @param	Threshold		circular mode threshold length in byte, when the empty data space 
 *							in the memory is larger 'Threshold', it will report tint
 * @param	Ptr				the address for memory buffer
 * @param	Len				the length for memory buffer
 
 * @return
 *   		FDMA_ERROR
 * @note	Ptr must be aligned with srcDwidth, and Len must be aligned with srcDwidth or dstDwidth,
 *			otherwise will encounter an FDMA_UNALIGNED_ADDRESS
 */
FDMA_ERROR FDMA_CircularConfig(uint8_t srcDwidth, uint8_t dstDwidth, uint16_t Threshold, void* Ptr, uint16_t Len);

/**
 * @brief	Set circular mode threshold length in byte, when the empty data space 
 *			in the memory is larger 'Threshold', it will report tint
 * @param	Threshold		circular mode threshold length in byte
 *
 * @return
 *   		FDMA_OK or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note	
 */
FDMA_ERROR FDMA_CircularSetThresholdLen(uint16_t Threshold);

/**
 * @brief	Get circular mode threshold length in byte.
 * @param	void
 *
 * @return
 *   		Threshold, or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note	
 */
int32_t FDMA_CircularGetThresholdLen(void);

/**
 * @brief	Get data length in the circular fifo
 * @param	void
 *
 * @return
 *   		data length, or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note	
 */
int32_t FDMA_CircularGetDataLen(void);

/**
 * @brief	Get space length in the circular fifo
 * @param	void
 *
 * @return
 *   		space length, or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note	
 */
int32_t FDMA_CircularGetSpaceLen(void);

/**
 * @brief	Clear the circular FIFO
 * @param	void
 *
 * @return
 *   		FDMA_OK or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note	
 */
FDMA_ERROR FDMA_CircularClrFIFO(void);

/**
 * @brief	Get the write pointer with circular FIFO in byte
 * @param	void
 *
 * @return
 *    		the write pointer or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note
 */
int32_t FDMA_CircularGetWritePtr(void);

/**
 * @brief	Set the write pointer with circular FIFO in byte
 * @param	void
 *
 * @return
 *    		the write pointer or FDMA_MODE_INVALID if current FDMA is not circular mode
 * @note
 */
FDMA_ERROR FDMA_CircularSetWritePtr(uint16_t Ptr);

/**
 * @brief	Start or stop FDMA. When after configuration, the FDMA will not work, only 
 			if we enable it.
 * @param	void
 *
 * @return
 *    		always FDMA_OK
 * @note
 */
FDMA_ERROR FDMA_StartStop(bool start);


#endif/*__LED_H__*/

/**
 * @}
 * @}
 */
