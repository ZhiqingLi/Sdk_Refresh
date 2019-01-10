#ifndef __CONEXANT_CX20921_DEF_H__
#define __CONEXANT_CX20921_DEF_H__

#ifdef VOICE_INPUT_MODULE_CX20921
#ifdef __cplusplus
extern "C" {
#endif
#include "hal_gpio.h"
#include "bes_hal_board.h"
#include "bes_hal_uart.h"

#define CONEXANT_CX20921_DEBUG       BES_HAL_UART_DEBUG
#define CONEXANT_CX20921_INFO        BES_HAL_UART_INFO
#define CONEXANT_CX20921_WARNING     BES_HAL_UART_WARNING
#define CONEXANT_CX20921_ERROR       BES_HAL_UART_ERROR
#define CONEXANT_CX20921_FATAL       BES_HAL_UART_FATAL

/*************** Error code define ***************/
#define CXERR_NOERR                 0
#define CXERR_STATE_FATAL           -101
#define CXERR_LOAD_IMG_TIMEOUT      -102
#define CXERR_NULL_POINTER          -103
#define CXERR_I2C_ERROR             -104
#define CXERR_INVALID_PARAMETER     -105
#define CXERR_PARTITION_ERR         -106
#define CXERR_CRC_CHECK_ERROR       -107
#define CXERR_FAILED                -108
#define CXERR_WAITING_LOADER_TIMEOUT -109
#define CXERR_WAITING_RESET_TIMEOUT -110
#define CXERR_READ_FLASH_ERROR      -111
#define CXERR_MD5_CHECK_ERROR       -112
#define CXERR_FW_NO_RESPONSE        -113
#define CXERR_FW_CANNOT_BOOT        -114
#define CXERR_FW_VER_INCORRECT      -115

#define CX20921_I2C_SLAVE_ADDR  (0x41) // i2c slave addr
#define CX20921_I2C_REG_ADDR_LEN            (2)



#define RESET_INTERVAL_MS    200 // 200 ms



#define TIME_OUT_MS          100   //50 ms
#define MANUAL_TIME_OUT_MS   5000  //1000 ms
#define POLLING_INTERVAL_MS  1     // 1 ms






int32_t cx20921_hal_init(void);
void cx20921_hal_deinit(void);
void cx20921_power_onoff(uint8_t onoff);
void cx20921_reset(void);
void cx20921_wake_up_event_register(void (* cb)(void));


int32_t cx20921_i2c_transmit(const uint8_t *header, uint32_t num_header, const uint8_t *payload, uint32_t num_payload);


#ifdef __cplusplus
}
#endif
#endif
#endif
