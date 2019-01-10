#ifndef __HAL_TRACE_H__
#define __HAL_TRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"


//#define AUDIO_DEBUG
//#define HAL_TRACE_RX_ENABLE
uint8_t hal_trace_get_log_flag(void);
void hal_trace_log_flag(uint8_t flag);
#if defined(DEBUG) && !defined(AUDIO_DEBUG)
#define TRACE(str, ...)             hal_trace_printf(str, ##__VA_ARGS__)
#define TRACE_NOCRLF(str, ...)      hal_trace_printf_without_crlf(str, ##__VA_ARGS__)
#define TRACE_IMM(str, ...)         hal_trace_printf_imm(str, ##__VA_ARGS__)
#define TRACE_OUTPUT(str, len)      hal_trace_output(str, len)
#define FUNC_ENTRY_TRACE()          hal_trace_printf(__FUNCTION__)
#define DUMP8(str, buf, cnt)        hal_trace_dump(str, sizeof(uint8_t), cnt, buf)
#define DUMP16(str, buf, cnt)       hal_trace_dump(str, sizeof(uint16_t), cnt, buf)
#define DUMP32(str, buf, cnt)       hal_trace_dump(str, sizeof(uint32_t), cnt, buf)
#else
// To avoid warnings on unused variables
#define TRACE(str, ...)             hal_trace_dummy(str, ##__VA_ARGS__)
#define TRACE_NOCRLF(str, ...)      hal_trace_dummy(str, ##__VA_ARGS__)
#define TRACE_IMM(str, ...)         hal_trace_dummy(str, ##__VA_ARGS__)
#define TRACE_OUTPUT(str, len)      hal_trace_dummy(str, len)
#define FUNC_ENTRY_TRACE()          hal_trace_dummy(NULL)
#define DUMP8(str, buf, cnt)        hal_trace_dummy(str, buf, cnt)
#define DUMP16(str, buf, cnt)       hal_trace_dummy(str, buf, cnt)
#define DUMP32(str, buf, cnt)       hal_trace_dummy(str, buf, cnt)
#endif

#if defined(DEBUG) && defined(ASSERT_SHOW_FILE_FUNC)
#define ASSERT(cond, str, ...)      { if (!(cond)) { hal_trace_assert_dump(__FILE__, __FUNCTION__, __LINE__, str, ##__VA_ARGS__); } }
#elif defined(DEBUG) && defined(ASSERT_SHOW_FILE)
#define ASSERT(cond, str, ...)      { if (!(cond)) { hal_trace_assert_dump(__FILE__, __LINE__, str, ##__VA_ARGS__); } }
#elif defined(DEBUG) && defined(ASSERT_SHOW_FUNC)
#define ASSERT(cond, str, ...)      { if (!(cond)) { hal_trace_assert_dump(__FUNCTION__, __LINE__, str, ##__VA_ARGS__); } }
#else
#define ASSERT(cond, str, ...)      { if (!(cond)) { hal_trace_assert_dump(str, ##__VA_ARGS__); } }
#endif

#define ASSERT_NODUMP(cond)         { if (!(cond)) { SAFE_PROGRAM_STOP(); } }

#ifdef CHIP_BEST1000
// Avoid CPU instruction fetch blocking the system bus on BEST1000
#define SAFE_PROGRAM_STOP()         do { asm volatile("nop \n nop \n nop \n nop"); } while (1)
#else
#define SAFE_PROGRAM_STOP()         do { } while (1)
#endif

#ifdef DEBUG
#define TRACE_FUNC_DECLARE(d, r)    d;
#else
//change begin cai.zhong 20180516 for close debug 
//#define TRACE_FUNC_DECLARE(d, r)    static inline d { r; }
#define TRACE_FUNC_DECLARE(d, r)    d;
//change end cai.zhong 20180516 for close debug 
#endif

enum HAL_TRACE_TRANSPORT_T {
#ifdef CHIP_HAS_USB
    HAL_TRACE_TRANSPORT_USB,
#endif
    HAL_TRACE_TRANSPORT_UART0,
#if (CHIP_HAS_UART > 1)
    HAL_TRACE_TRANSPORT_UART1,
#endif

    HAL_TRACE_TRANSPORT_QTY
};

enum HAL_TRACE_CRASH_STAGE_T {
    HAL_TRACE_CRASH_START,
    HAL_TRACE_CRASH_END,
};

typedef void (*HAL_TRACE_CRASH_DUMP_CB_T)(void);

typedef void (*HAL_TRACE_CRASH_APP_NOTIFY_T)(enum HAL_TRACE_CRASH_STAGE_T stage);

typedef void (*HAL_TRACE_CRASH_APP_OUTPUT_T)(const unsigned char *buf, unsigned int buf_len);

TRACE_FUNC_DECLARE(int hal_trace_switch(enum HAL_TRACE_TRANSPORT_T transport), return 0)

TRACE_FUNC_DECLARE(int hal_trace_open(enum HAL_TRACE_TRANSPORT_T transport), return 0)

TRACE_FUNC_DECLARE(int hal_trace_close(void), return 0)

TRACE_FUNC_DECLARE(int hal_trace_output(const unsigned char *buf, unsigned int buf_len), return 0)

TRACE_FUNC_DECLARE(int hal_trace_printf(const char *fmt, ...), return 0)

TRACE_FUNC_DECLARE(int hal_trace_printf_without_crlf(const char *fmt, ...), return 0)

TRACE_FUNC_DECLARE(void hal_trace_printf_imm(const char *fmt, ...), return)

TRACE_FUNC_DECLARE(int hal_trace_printf_without_crlf_fix_arg(const char *fmt), return 0)

TRACE_FUNC_DECLARE(int hal_trace_printf_with_tag(const char *tag, const char *fmt, ...), return 0)

TRACE_FUNC_DECLARE(int hal_trace_dump(const char *fmt, unsigned int size,  unsigned int count, const void *buffer), return 0)

TRACE_FUNC_DECLARE(int hal_trace_busy(void), return 0)

TRACE_FUNC_DECLARE(void hal_trace_idle_send(void), return)

TRACE_FUNC_DECLARE(int hal_trace_crash_dump_register(HAL_TRACE_CRASH_DUMP_CB_T cb), return 0)

TRACE_FUNC_DECLARE(void hal_trace_crash_app_register(HAL_TRACE_CRASH_APP_NOTIFY_T notify_cb, HAL_TRACE_CRASH_APP_OUTPUT_T output_cb), return)

static inline void hal_trace_dummy(const char *fmt, ...) { }

#if defined(DEBUG) && defined(ASSERT_SHOW_FILE_FUNC)
#define ASSERT_DUMP_ARGS    const char *file, const char *func, unsigned int line, const char *fmt, ...
#elif defined(DEBUG) && (defined(ASSERT_SHOW_FILE) || defined(ASSERT_SHOW_FUNC))
#define ASSERT_DUMP_ARGS    const char *scope, unsigned int line, const char *fmt, ...
#else
#define ASSERT_DUMP_ARGS    const char *fmt, ...
#endif
void NORETURN hal_trace_assert_dump(ASSERT_DUMP_ARGS);


//==============================================================================
// AUDIO_DEBUG
//==============================================================================

#ifdef AUDIO_DEBUG
#define AUDIO_DEBUG_TRACE(str, ...)         hal_trace_printf(str, ##__VA_ARGS__)
#define AUDIO_DEBUG_DUMP(buf, cnt)          hal_trace_output(buf, cnt)
#endif


//==============================================================================
// TRACE RX
//==============================================================================

#ifdef HAL_TRACE_RX_ENABLE

#include "stdio.h"

#define hal_trace_rx_parser(buf, str, ...)  sscanf(buf, str, ##__VA_ARGS__)

typedef unsigned int (*HAL_TRACE_RX_CALLBACK_T)(unsigned char *buf, unsigned int  len);

int hal_trace_rx_register(char *name, HAL_TRACE_RX_CALLBACK_T callback);
int hal_trace_rx_deregister(char *name);

#endif

#ifdef __cplusplus
}
#endif

#endif
