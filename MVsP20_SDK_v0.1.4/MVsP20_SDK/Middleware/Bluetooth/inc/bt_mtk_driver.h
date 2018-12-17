
/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_mtk_driver bt_mtk_driver.h
* @{
*/

#ifndef __BT_MTK_DRIVER_H__
#define __BT_MTK_DRIVER_H__

#include "type.h"

/***************************************************************
 *
 * UART releated defines
 * 
 * ATTENTION:
 *	Only 460800, 921600, 3200000 bps valid in this version
 */

typedef uint8_t		UartBaudrate;

#define UART_BAUDRATE_USER		0	/*!< user defined */
#define UART_BAUDRATE_9P6K		1	/*!< 9.6K bps     */
#define UART_BAUDRATE_14P4K		2	/*!< 14.4K bps    */
#define UART_BAUDRATE_19P2K		3	/*!< 19.2K bps    */
#define UART_BAUDRATE_28P8K		4	/*!< 28.8K bps    */
#define UART_BAUDRATE_33P9K		5	/*!< 33.9K bps    */
#define UART_BAUDRATE_38P4K		6	/*!< 38.4K bps    */
#define UART_BAUDRATE_57P6K		7	/*!< 57.6K bps    */
#define UART_BAUDRATE_115P2K	8	/*!< 115.2K bps   */
#define UART_BAUDRATE_230P4K	9	/*!< 230.4K bps   */
#define UART_BAUDRATE_460P8K	10	/*!< 460.8K bps   */
#define UART_BAUDRATE_921P6K	11	/*!< 921.6K bps   */
#define UART_BAUDRATE_3P2M		12	/*!< 3.2M bps     */

/*
 * Uart data bits
 */
typedef uint8_t		UartDataBits;

#define UART_DATA_BITS_8		0	/*!< 8 bits*/
#define UART_DATA_BITS_7		1	/*!< 7 bits*/
#define UART_DATA_BITS_6		2	/*!< 6 bits*/
#define UART_DATA_BITS_5		3	/*!< 5 bits*/

/*
 * Uart stop bits
 */
typedef uint8_t		UartStopBits;

#define UART_STOP_BITS_1		0	/*!< 1 stop bit   */
#define UART_STOP_BITS_2		1	/*!< 2 stop bits  */
#define UART_STOP_BITS_1P5		2	/*!< 1.5 stop bits*/


/*
 * Uart parity check
 */
typedef uint8_t		UartParityCheck;

#define UART_PARITY_CHECK_NONE	0	/*!< none parity check*/
#define UART_PARITY_CHECK_ODD	1	/*!< odd parity check*/
#define UART_PARITY_CHECK_EVEN	2	/*!< even parity check*/
#define UART_PARITY_CHECK_SPACE	3	/*!< space parity check*/

/*
 * Uart flow control
 */
typedef uint8_t		UartFlowControl;

#define UART_FLOW_CONTROL_NONE	0	/*!< no flow control*/
#define UART_FLOW_CONTROL_HW	1	/*!< hardware flow control*/
#define UART_FLOW_CONTROL_X		2	/*!< Xon/Xoff flow control*/


typedef struct _MtkUartSetting
{
	UartBaudrate		baudrate;
	UartDataBits		dataBits;
	UartStopBits		stopBits;
	UartParityCheck		parityChk;
	UartFlowControl		flowCtrl;
}MtkUartSettings;

/***************************************************************
 *
 * PCM releated defines
 */



/*
 * PCM mode. Master or Slave mode
 */
typedef uint8_t 	PcmMode;

#define PCM_MODE_SLAVE		0
#define PCM_MODE_MASTER		1


/**
 * Define PCM output clock 
 * Only vaild in Master mode
 */
typedef uint8_t		PcmClock;

#define PCM_CLOCK_128K		0	/*!<PCM output clock 128KHz */
#define PCM_CLOCK_1024K		1	/*!<PCM output clock 1024KHz*/
#define PCM_CLOCK_64K		2	/*!<PCM output clock 64KHz  */
#define PCM_CLOCK_256K		4	/*!<PCM output clock 256KHz */
#define PCM_CLOCK_512K		5	/*!<PCM output clock 512KHz */

/**
 * PCM input clock frequence ability
 * Less than 1MHz or Larger than 1MHz
 * Valid in Slave mode
 */
typedef uint8_t		PcmInClockAbility;

#define PCM_IN_CLOCK_LESS_1M	0		/*!< PCM clock input < 1MHz*/

#define PCM_IN_CLOCK_LARGER_1M	1		/*!< PCM clock input >= 1MHz*/

/**
 * PCM frame sync format
 * Long frame sync or Short frame sync
 */
typedef uint8_t		PcmFrameSyncFormat;

#define PCM_FRAME_SYNC_FORMAT_LONG	0	/*!< Long frame sync format*/

#define PCM_FRAME_SYNC_FORMAT_SHORT	1	/*!< Short frame sync format*/



/**
 * PCM frame endian format
 * Long frame sync or Short frame sync
 */
typedef uint8_t		PcmEndianFormat;

#define PCM_ENDIAN_MSB			0		/*!< endian in MSB*/

#define PCM_ENDIAN_LSB			1		/*!< endian in LSB*/


/**
 * PCM Sign Extension
 * Zero padding or Sign Extension
 * Only valid when the linear PCM length is less than 16bits
 */
typedef uint8_t		PcmSignExtension;

#define PCM_ZERO_PADDING		0		/*!< Zero padding.Only applies to LSB first data formats  */

#define PCM_SIGN_EXTENSION		1		/*!< Sign Extension.Only applies to MSB first data formats */


/*
 * PCM Settings
 */
typedef struct _MtkPcmSettings
{
	PcmMode				pcmMode;		/*!< mode : maser or slave*/
	PcmClock			pcmClock;		/*!< PCM output clock, valid in Master mode*/
	PcmInClockAbility	pcmInClkAbi;	/*!< PCM input clock frequence ability*/
	PcmFrameSyncFormat	pcmSyncFormat;	/*!< PCM frame sync format*/
	PcmEndianFormat		pcmEndianFormat;/*!< PCM frame endian format*/
	PcmSignExtension	pcmSignExt;		/*!< PCM Sign Extension */
}MtkPcmSettings;

/***************************************************************
 *
 * Low power sleep mode defines
 */

typedef struct _MtkSleepMode
{
	bool		sleepModeEnable;		/*!< Enable/Disable sleep mode*/
	uint16_t	durationToSleep;		/*!< The duration of MT6622 go to sleep mode when there is no activity*/
}MtkSleepMode;

/***************************************************************
 *
 * Local features defines
 */

typedef struct _MtkLocalFeature
{
	bool		enableAutoSniff;		/*!< TRUE for enable auto sniff*/
	bool		enableEV3;				/*!< TRUE for enable EV3*/
	bool		enable3EV3;				/*!< TRUE for enable 3EV3*/
	bool		enableSimplePairing;	/*!< TRUE for enable simple pairing*/
}MtkLocalFeature;


/***************************************************************
 *
 * MTK Config parameters
 */

typedef struct _MtkConfigParam
{
	uint8_t				bdAddr[6];
	MtkUartSettings		uartSettings;
	MtkPcmSettings		pcmSettings;
	MtkLocalFeature		localFeatures;
	uint8_t				txPower;		/*!<TX power. Max is 0x07*/
	uint8_t				trimValue;		/*!< XO trim. 0x00 ~ 0x7F*/
}MtkConfigParam;


bool BTDeviceInit_MTK662x(MtkConfigParam * configParams);

bool BTDeviceDeInit_MTK662x(void);

/** No used */
bool MtkSetSleepMode(MtkSleepMode *sleepMode);

#endif /*__BT_MTK_DRIVER_H__*/
