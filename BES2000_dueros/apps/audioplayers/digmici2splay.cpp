#ifdef MBED
#include "mbed.h"
#endif
// Standard C Included Files
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtos.h"
#ifdef MBED
#include "SDFileSystem.h"
#endif
#include "cqueue.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_trace.h"

uint8_t digmic_buf[100*1024];
uint32_t digmic_buf_len = 0;

uint32_t dig_mic_audio_more_data(uint8_t *buf, uint32_t len)
{
    TRACE("%s:%d\n", __func__, __LINE__);
    memcpy(buf, digmic_buf, len);

    return len;
}
uint32_t dig_mic_audio_data_come(uint8_t *buf, uint32_t len)
{
    TRACE("%s:%d\n", __func__, __LINE__);

    memcpy(digmic_buf + digmic_buf_len, buf, len);

#if 1
    digmic_buf_len = (digmic_buf_len + len)%(100*1024);
#endif
    return len;
}
