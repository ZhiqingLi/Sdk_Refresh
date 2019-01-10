#include "sys_api_programmer.h"
#include "cmsis_nvic.h"
#include "export_fn_rom.h"
#include "flash_programmer.h"
#include "hal_bootmode.h"
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_iomux.h"
#include "hal_norflash.h"
#include "hal_sysfreq.h"
#include "hal_timer_raw.h"
#include "hal_uart.h"
#include "plat_addr_map.h"
#include "pmu.h"
#include "stdint.h"
#include "string.h"
#include "task_schedule.h"
#include "tool_msg.h"
#ifdef CHIP_HAS_USB
#include "usb_cdc.h"
#endif

#define TIMEOUT_INFINITE                ((uint32_t)-1)

enum UART_DMA_STATE {
    UART_DMA_IDLE,
    UART_DMA_START,
    UART_DMA_DONE,
    UART_DMA_ERROR,
};

static void setup_download_task(void);
static void setup_flash_task(void);

const unsigned int default_recv_timeout_short = MS_TO_TICKS(500);
const unsigned int default_recv_timeout_idle = TIMEOUT_INFINITE; //MS_TO_TICKS(10 * 60 * 1000);
const unsigned int default_recv_timeout_4k_data = MS_TO_TICKS(500);
const unsigned int default_send_timeout = MS_TO_TICKS(500);

static uint32_t send_timeout;
static uint32_t recv_timeout;

static const struct HAL_UART_CFG_T uart_cfg = {
    .parity = HAL_UART_PARITY_NONE,
    .stop = HAL_UART_STOP_BITS_1,
    .data = HAL_UART_DATA_BITS_8,
    .flow = HAL_UART_FLOW_CONTROL_NONE, //HAL_UART_FLOW_CONTROL_RTSCTS,
    .tx_level = HAL_UART_FIFO_LEVEL_1_2,
    .rx_level = HAL_UART_FIFO_LEVEL_1_2,
    .baud = 921600,
    .dma_rx = true,
    .dma_tx = false,
    .dma_rx_stop_on_err = false,
};
static const enum HAL_UART_ID_T dld_uart = HAL_UART_ID_0;
static const enum HAL_TRACE_TRANSPORT_T main_trace_transport =
#if (DEBUG_PORT == 2)
    HAL_TRACE_TRANSPORT_UART1
#else
    HAL_TRACE_TRANSPORT_UART0
#endif
    ;
#if (CHIP_HAS_UART > 1)
static const enum HAL_TRACE_TRANSPORT_T alt_trace_transport = HAL_TRACE_TRANSPORT_UART1;
#endif
static volatile enum UART_DMA_STATE uart_dma_rx_state = UART_DMA_IDLE;
static volatile uint32_t uart_dma_rx_size = 0;
static bool uart_opened = false;

#ifdef CHIP_HAS_USB
static bool usb_opened = false;
#endif

static volatile bool cancel_xfer = false;
static enum DOWNLOAD_TRANSPORT dld_transport;

static uint32_t xfer_err_time = 0;
static uint32_t xfer_err_cnt = 0;

static void uart_rx_dma_handler(uint32_t xfer_size, int dma_error, union HAL_UART_IRQ_T status)
{
    // The DMA transfer has been cancelled
    if (uart_dma_rx_state != UART_DMA_START) {
        return;
    }

    uart_dma_rx_size = xfer_size;
    if (dma_error || status.FE || status.OE || status.PE || status.RT || status.BE) {
        TRACE("UART-RX Error: dma_error=%d, status=0x%08x", dma_error, status.reg);
        uart_dma_rx_state = UART_DMA_ERROR;
    } else {
        uart_dma_rx_state = UART_DMA_DONE;
    }
}

static void uart_break_handler(void)
{
    TRACE("****** Handle break ******");

    cancel_xfer = true;
    hal_uart_stop_dma_recv(dld_uart);
    hal_uart_stop_dma_send(dld_uart);
}

#ifdef CHIP_HAS_USB
static void usb_serial_break_handler(void)
{
    TRACE("****** Handle break ******");

    cancel_xfer = true;
    hal_timer_stop();
    usb_serial_cancel_recv();
    usb_serial_cancel_send();
}

static void reopen_usb_serial(void)
{
    usb_serial_reopen(usb_serial_break_handler);
}
#endif

static void init_flash(void)
{
    int ret;

    static const struct HAL_NORFLASH_CONFIG_T cfg = {
#ifdef FPGA
        .source_clk = HAL_NORFLASH_SPEED_13M * 2,
        .speed = HAL_NORFLASH_SPEED_13M,
#else
        .source_clk = HAL_NORFLASH_SPEED_104M * 2,
        .speed = HAL_NORFLASH_SPEED_104M,
#endif
        .mode  = DRV_NORFLASH_OP_MODE_QUAD | DRV_NORFLASH_OP_MODE_CONTINUOUS_READ,
        .override_config = 0,
    };

    // Workaround for reboot: controller in SPI mode while FLASH in QUAD mode
    *(volatile uint32_t *)FLASH_NC_BASE;

    hal_cmu_pll_enable(HAL_CMU_PLL_AUD, HAL_CMU_PLL_USER_SYS);
    hal_cmu_flash_select_pll(HAL_CMU_PLL_AUD);
#if defined(CHIP_BEST1000) && defined(AUD_PLL_DOUBLE)
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_104M);
#else
    hal_cmu_flash_set_freq(HAL_CMU_FREQ_208M);
#endif

    ret = hal_norflash_open(HAL_NORFLASH_ID_0, &cfg);

    ASSERT(ret == 0, "Failed to open norflash: %d", ret);
}

void set_download_transport(enum DOWNLOAD_TRANSPORT transport)
{
    union HAL_UART_IRQ_T mask;

    dld_transport = transport;

    if (transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
        if (!usb_opened) {
            reopen_usb_serial();
            usb_opened = true;
        }
#endif
    } else if (transport == TRANSPORT_UART) {
        if (!uart_opened) {
            hal_uart_reopen(dld_uart, &uart_cfg);
            mask.reg = 0;
            mask.BE = 1;
            hal_uart_irq_set_dma_handler(dld_uart, uart_rx_dma_handler, NULL, uart_break_handler);
            hal_uart_irq_set_mask(dld_uart, mask);
            uart_opened = true;
        }
    }

    setup_download_task();
    setup_flash_task();
    init_flash();
}

static int uart_download_enabled(void)
{
    return !!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_DLD_TRANS_UART);
}

void init_download_transport(void)
{
    enum DOWNLOAD_TRANSPORT transport;

    transport = uart_download_enabled() ? TRANSPORT_UART : TRANSPORT_USB;
    set_download_transport(transport);
}

void init_cust_cmd(void)
{
    extern uint32_t __cust_cmd_init_tbl_start[];
    extern uint32_t __cust_cmd_init_tbl_end[];

    CUST_CMD_INIT_T *start = (CUST_CMD_INIT_T *)__cust_cmd_init_tbl_start;
    CUST_CMD_INIT_T *end = (CUST_CMD_INIT_T *)__cust_cmd_init_tbl_end;

    while (start < end) {
        (*start)();
        start++;
    }
}

enum ERR_CODE handle_cust_cmd(struct CUST_CMD_PARAM *param)
{
    enum ERR_CODE errcode = ERR_TYPE_INVALID;

    extern uint32_t __cust_cmd_hldr_tbl_start[];
    extern uint32_t __cust_cmd_hldr_tbl_end[];

    CUST_CMD_HANDLER_T *start = (CUST_CMD_HANDLER_T *)__cust_cmd_hldr_tbl_start;
    CUST_CMD_HANDLER_T *end = (CUST_CMD_HANDLER_T *)__cust_cmd_hldr_tbl_end;

    while (errcode == ERR_TYPE_INVALID && start < end) {
        errcode = (*start)(param);
        start++;
    }

    return errcode;
}

void reset_transport(void)
{
    cancel_xfer = false;

    if (dld_transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
        usb_serial_flush_recv_buffer();
        usb_serial_init_xfer();
#endif
    } else {
        hal_uart_flush(dld_uart, 0);
    }

    set_recv_timeout(default_recv_timeout_short);
    set_send_timeout(default_send_timeout);
}

void set_recv_timeout(unsigned int timeout)
{
    recv_timeout = timeout;
}

void set_send_timeout(unsigned int timeout)
{
    send_timeout = timeout;
}

int debug_read_enabled(void)
{
    return !!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_READ_ENABLED);
}

int debug_write_enabled(void)
{
    return !!(hal_sw_bootmode_get() & HAL_SW_BOOTMODE_WRITE_ENABLED);
}

#ifdef CHIP_HAS_USB
static void usb_send_timeout(uint32_t elapsed)
{
    usb_serial_cancel_send();
}

static void usb_send_timer_start(void)
{
    if (send_timeout == TIMEOUT_INFINITE) {
        return;
    }

    hal_timer_setup(HAL_TIMER_TYPE_ONESHOT, usb_send_timeout);
    hal_timer_start(send_timeout);
}

static void usb_send_timer_stop(void)
{
    hal_timer_stop();
}

static int usb_send_data(const unsigned char *buf, size_t len)
{
    int ret;

    usb_send_timer_start();
    ret = usb_serial_send(buf, len);
    usb_send_timer_stop();
    return ret;
}
#endif

static int uart_send_data(const unsigned char *buf, size_t len)
{
    uint32_t start;
    uint32_t sent = 0;

    start = hal_sys_timer_get();
    while (sent < len) {
        while (!cancel_xfer && !hal_uart_writable(dld_uart) &&
                (send_timeout == TIMEOUT_INFINITE || hal_sys_timer_get() - start < send_timeout) &&
                TASK_SCHEDULE);
        if (cancel_xfer) {
            break;
        }
        if (hal_uart_writable(dld_uart)) {
            hal_uart_putc(dld_uart, buf[sent++]);
        } else {
            break;
        }
    }

    if (sent != len) {
        return 1;
    }

    return 0;
}

int send_data(const unsigned char *buf, size_t len)
{
    if (cancel_xfer) {
        return -1;
    }

    if (dld_transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
        return usb_send_data(buf, len);
#else
        return 0;
#endif
    } else {
        return uart_send_data(buf, len);
    }
}

#ifdef CHIP_HAS_USB
static void usb_recv_timeout(uint32_t elapsed)
{
    usb_serial_cancel_recv();
}

static void usb_recv_timer_start(void)
{
    if (recv_timeout == TIMEOUT_INFINITE) {
        return;
    }

    hal_timer_setup(HAL_TIMER_TYPE_ONESHOT, usb_recv_timeout);
    hal_timer_start(recv_timeout);
}

static void usb_recv_timer_stop(void)
{
    hal_timer_stop();
}

static int usb_recv_data(unsigned char *buf, size_t len, size_t *rlen)
{
    int ret;

    usb_recv_timer_start();
    ret = usb_serial_recv(buf, len);
    usb_recv_timer_stop();
    if (ret == 0) {
        *rlen = len;
    }
    return ret;
}

static int usb_recv_data_dma(unsigned char *buf, size_t len, size_t expect, size_t *rlen)
{
    int ret;

    usb_recv_timer_start();
    ret = usb_serial_direct_recv(buf, len, expect, rlen, 0, NULL);
    usb_recv_timer_stop();
    return ret;
}
#endif

static int uart_recv_data_dma(unsigned char *buf, size_t expect, size_t *rlen)
{
    int ret;
    union HAL_UART_IRQ_T mask;
    uint32_t start;
    struct HAL_DMA_DESC_T dma_desc[17];
    uint32_t desc_cnt = ARRAY_SIZE(dma_desc);

    if (uart_dma_rx_state != UART_DMA_IDLE) {
        ret = -3;
        goto _no_state_exit;
    }

    uart_dma_rx_state = UART_DMA_START;
    uart_dma_rx_size = 0;

    ret = hal_uart_dma_recv(dld_uart, buf, expect, &dma_desc[0], &desc_cnt);
    if (ret) {
        goto _exit;
    }

    mask.reg = 0;
    mask.BE = 1;
    mask.FE = 1;
    mask.OE = 1;
    mask.PE = 1;
    //mask.RT = 1;
    hal_uart_irq_set_mask(dld_uart, mask);

    start = hal_sys_timer_get();
    while (uart_dma_rx_state == UART_DMA_START && !cancel_xfer &&
            (recv_timeout == TIMEOUT_INFINITE || hal_sys_timer_get() - start < recv_timeout) &&
            TASK_SCHEDULE);

    if (cancel_xfer) {
        ret = -14;
        goto _exit;
    }
    if (uart_dma_rx_state == UART_DMA_START) {
        hal_uart_stop_dma_recv(dld_uart);
        ret = -15;
        goto _exit;
    }
    if (uart_dma_rx_state != UART_DMA_DONE) {
        ret = -2;
        goto _exit;
    }

    ret = 0;
    *rlen = uart_dma_rx_size;

_exit:
    mask.reg = 0;
    mask.BE = 1;
    hal_uart_irq_set_mask(dld_uart, mask);

    uart_dma_rx_state = UART_DMA_IDLE;

_no_state_exit:
    return ret;
}

int recv_data_ex(unsigned char *buf, size_t len, size_t expect, size_t *rlen)
{
    if (cancel_xfer) {
        return -1;
    }

    if (dld_transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
        if (len == 0) {
            return usb_recv_data(buf, expect, rlen);
        } else {
            return usb_recv_data_dma(buf, len, expect, rlen);
        }
#else
        return 0;
#endif
    } else {
        return uart_recv_data_dma(buf, expect, rlen);
    }
}

#ifdef CHIP_HAS_USB
static int usb_handle_error(void)
{
    int ret;

    TRACE("****** Send break ******");

    // Send break signal, to tell the peer to reset the connection
    ret = usb_serial_send_break();
    if (ret) {
        TRACE("Sending break failed: %d", ret);
    }
    return ret;
}
#endif

static int uart_handle_error(void)
{
    TRACE("****** Send break ******");

    // Send break signal, to tell the peer to reset the connection
    hal_uart_break_set(dld_uart);
    hal_sys_timer_delay(MS_TO_TICKS(50));
    hal_uart_break_clear(dld_uart);
    hal_uart_flush(dld_uart, 0);

    return 0;
}

int handle_error(void)
{
    int ret = 0;
    uint32_t err_time;

    hal_sys_timer_delay(MS_TO_TICKS(50));

    if (!cancel_xfer) {
        if (dld_transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
            ret = usb_handle_error();
#endif
        } else {
            ret = uart_handle_error();
        }
    }

    err_time = hal_sys_timer_get();
    if (xfer_err_cnt == 0 || err_time - xfer_err_time > MS_TO_TICKS(5000)) {
        xfer_err_cnt = 0;
        xfer_err_time = err_time;
    }
    xfer_err_cnt++;
    if (xfer_err_cnt < 3) {
        hal_sys_timer_delay(MS_TO_TICKS(100));
    } else if (xfer_err_cnt < 5) {
        hal_sys_timer_delay(MS_TO_TICKS(500));
    } else {
        hal_sys_timer_delay(MS_TO_TICKS(2000));
    }

    return ret;
}

#ifdef CHIP_HAS_USB
static int usb_cancel_input(void)
{
    return usb_serial_flush_recv_buffer();
}
#endif

static int uart_cancel_input(void)
{
    hal_uart_flush(dld_uart, 0);
    return 0;
}

int cancel_input(void)
{
    if (dld_transport == TRANSPORT_USB) {
#ifdef CHIP_HAS_USB
        return usb_cancel_input();
#else
        return 0;
#endif
    } else {
        return uart_cancel_input();
    }
}

void system_reboot(void)
{
    hal_sys_timer_delay(MS_TO_TICKS(10));
    hal_cmu_sys_reboot();
}

void system_shutdown(void)
{
#if 0
    if (dld_transport == TRANSPORT_USB) {
        // Avoid PC usb serial driver hanging
        usb_serial_close();
    }
#endif
    hal_sys_timer_delay(MS_TO_TICKS(10));
    pmu_shutdown();
}

void system_flash_boot(void)
{
    __export_fn_rom.boot_from_flash_reent();
}

void system_set_bootmode(unsigned int bootmode)
{
    bootmode &= ~(HAL_SW_BOOTMODE_READ_ENABLED | HAL_SW_BOOTMODE_WRITE_ENABLED);
    hal_sw_bootmode_set(bootmode);
}

void system_clear_bootmode(unsigned int bootmode)
{
    bootmode &= ~(HAL_SW_BOOTMODE_READ_ENABLED | HAL_SW_BOOTMODE_WRITE_ENABLED);
    hal_sw_bootmode_clear(bootmode);
}

unsigned int system_get_bootmode(void)
{
    return hal_sw_bootmode_get();
}

int get_sector_info(unsigned int addr, unsigned int *sector_addr, unsigned int *sector_len)
{
    int ret;

    ret = hal_norflash_get_boundary(HAL_NORFLASH_ID_0, addr, NULL, sector_addr);
    if (ret) {
        return ret;
    }

    ret = hal_norflash_get_size(HAL_NORFLASH_ID_0, NULL, NULL, sector_len, NULL);

    return ret;
}

int erase_sector(unsigned int sector_addr, unsigned int sector_len)
{
    return hal_norflash_erase(HAL_NORFLASH_ID_0, sector_addr, sector_len);
}

int erase_chip(void)
{
    return hal_norflash_erase_chip(HAL_NORFLASH_ID_0);
}

int burn_data(unsigned int addr, const unsigned char *data, unsigned int len)
{
    return hal_norflash_write(HAL_NORFLASH_ID_0, addr, data, len);
}

int verify_flash_data(unsigned int addr, const unsigned char *data, unsigned int len)
{
    const unsigned char *fdata;
    const unsigned char *mdata;
    int i;

    fdata = (unsigned char *)addr;
    mdata = data;
    for (i = 0; i < len; i++) {
        if (*fdata++ != *mdata++) {
            --fdata;
            --mdata;
            TRACE("****** Verify flash data failed: 0x%02X @ %p != 0x%02X @ %p", *fdata, fdata, *mdata, mdata);
            return *fdata - *mdata;
        }
    }

    return 0;
}

int wait_data_buf_finished(void)
{
    while (free_data_buf()) {
        task_yield();
    }

    return 0;
}

int wait_all_data_buf_finished(void)
{
    while (data_buf_in_use()) {
        while (free_data_buf() == 0);
        task_yield();
    }

    return 0;
}

// ========================================================================
// Programmer Task

extern uint32_t __StackTop[];
extern uint32_t __StackLimit[];
extern uint32_t __bss_start__[];
extern uint32_t __bss_end__[];

#define EXEC_STRUCT_LOC                 __attribute__((section(".exec_struct")))

void programmer_main(void)
{
    uint32_t *dst;

    for (dst = __bss_start__; dst < __bss_end__; dst++) {
        *dst = 0;
    }

#ifdef PROGRAMMER_LOAD_SIMU

    hal_cmu_simu_pass();

    // Avoid compiler warnings
    if (main_trace_transport == alt_trace_transport) {
    }

#else // !PROGRAMMER_LOAD_SIMU

#ifdef DEBUG
    for (dst = (uint32_t *)(__get_MSP() - 4); dst >= __StackLimit; dst--) {
        *dst = 0xCCCCCCCC;
    }
#endif

    NVIC_InitVectors();
#ifdef UNALIGNED_ACCESS
    SystemInit();
#endif
    hal_sys_timer_open();
    hal_gpdma_open();

    enum HAL_TRACE_TRANSPORT_T transport;

    if (uart_download_enabled()) {
#if (CHIP_HAS_UART > 1)
        transport = alt_trace_transport;
#else
        transport = HAL_TRACE_TRANSPORT_QTY;
#endif
    } else {
        transport = main_trace_transport;
    }
#ifndef NO_UART_IOMUX
    if (transport == HAL_TRACE_TRANSPORT_UART0) {
        hal_iomux_set_uart0();
#if (CHIP_HAS_UART > 1)
    } else if (transport == HAL_TRACE_TRANSPORT_UART1) {
        hal_iomux_set_uart1();
#endif
    }
#endif
    hal_trace_open(transport);

#ifndef FPGA
    int ret;
    ret = hal_analogif_open();
    ASSERT(ret == 0, "Failed to open analogif");
    hal_chipid_init();
#endif

    hal_sysfreq_req(HAL_SYSFREQ_USER_APP_0, HAL_CMU_FREQ_52M);

    TRACE("------ Enter %s ------", __FUNCTION__);

    programmer_loop();

    TRACE("------ Exit %s ------", __FUNCTION__);

#endif // !PROGRAMMER_LOAD_SIMU

    SAFE_PROGRAM_STOP();
}

void programmer_start(void)
{
    __set_MSP((uint32_t)__StackTop);
    programmer_main();
}

const struct exec_struct_t EXEC_STRUCT_LOC exec_struct = {
    .entry = (uint32_t)programmer_start,
    .param = 0,
    .sp = 0,
};


// ========================================================================
// Flash Task

#define STACK_ALIGN                     ALIGNED(8)
#define FLASH_TASK_STACK_SIZE           (2048)

enum TASK_ID_T {
    TASK_ID_MAIN = 0,
    TASK_ID_FLASH,
};

static unsigned char STACK_ALIGN flash_task_stack[FLASH_TASK_STACK_SIZE + 16];

static void setup_download_task(void)
{
    int ret;

    // Fake entry for main task
    ret = task_setup(TASK_ID_MAIN, 0, TASK_STATE_ACTIVE, NULL);

    ASSERT(ret == 0, "Failed to setup download task: %d", ret);
}

static void flash_task(void)
{
    unsigned int index;

    while (1) {
        while (get_burn_data_buf(&index)) {
            task_yield();
        }

        handle_data_buf(index);
    }
}

static void setup_flash_task(void)
{
    int ret;

#ifdef DEBUG
    memset(&flash_task_stack[0], 0xcc, sizeof(flash_task_stack));
#endif

    ret = task_setup(TASK_ID_FLASH, (uint32_t)flash_task_stack + FLASH_TASK_STACK_SIZE, TASK_STATE_PENDING, flash_task);

    ASSERT(ret == 0, "Failed to setup flash task: %d", ret);
}

