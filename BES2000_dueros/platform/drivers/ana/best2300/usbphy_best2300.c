#include "usbphy.h"
#include "reg_usbphy_best2300.h"
#include "cmsis_nvic.h"
#include "hal_cmu.h"
#include "hal_spi.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"

#ifdef CHIP_HAS_SPIPHY

#define USBPHY_READ_CMD(r)                  ((1 << 24) | (((r) & 0xFF) << 16))
#define USBPHY_WRITE_CMD(r, v)              ((((r) & 0xFF) << 16) | ((v) & 0xFFFF))
#define USBPHY_READ_VAL(v)                  ((v) & 0xFFFF)

static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = false,
    .cs = 0,
    .rate = 6500000,
    .tx_bits = 25,
    .rx_bits = 25,
    .rx_frame_bits = 0,
};

int usbphy_read(unsigned short reg, unsigned short *val)
{
    int ret;
    unsigned int data;
    unsigned int cmd;

    data = 0;
    cmd = USBPHY_READ_CMD(reg);
    ret = hal_spiphy_recv(&cmd, &data, 4);
    *val = USBPHY_READ_VAL(data);
    return ret;
}

int usbphy_write(unsigned short reg, unsigned short val)
{
    int ret;
    unsigned int cmd;

    cmd = USBPHY_WRITE_CMD(reg, val);
    ret = hal_spiphy_send(&cmd, 4);
    return ret;
}

void usbphy_ldo_config(int enable)
{
    int ret;

    hal_cmu_usbphy_clock_enable();
    ret = hal_spiphy_open(&spi_cfg);
    if (ret) {
        hal_cmu_usbphy_clock_disable();
        return;
    }

    if (enable) {
#ifdef USB_HIGH_SPEED
        // USB ldo voltage should match with vcore (now 0.9V)
        usbphy_write(0x61, 0x1118);
#else
        usbphy_write(0x61, 0x1116);
#endif
        usbphy_write(0x62, 0x2229);
        usbphy_write(0x63, 0x0D82);

        hal_sys_timer_delay(MS_TO_TICKS(1));
    } else {
        usbphy_write(0x61, 0x0000);
        usbphy_write(0x62, 0x0000);
        usbphy_write(0x63, 0x0000);

        hal_cmu_usbphy_clock_disable();
    }
}

void usbphy_open(void)
{
    unsigned short val_01, val;

#ifdef USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    // cdr_clk polariy=0 (stable for vcore 1.3V)
    val_01 &= ~((1 << 0) | (1 << 4) | (1 << 11));
    usbphy_write(0x01, val_01);
    val_01 |= (1 << 1) | (1 << 2) | (1 << 13);
    usbphy_write(0x01, val_01);

    usbphy_read(0x02, &val);
    val = (val & ~(0x1F << 10)) | (0xC << 10);
    usbphy_write(0x02, val);

    usbphy_read(0x05, &val);
    val |= (1 << 8);
    usbphy_write(0x05, val);

    usbphy_write(0x06, 0xCEE8);

    usbphy_write(0x07, 0x9813);

    usbphy_read(0x08, &val);
    usbphy_write(0x08, val);

    usbphy_read(0x09, &val);
    usbphy_write(0x09, val);

    usbphy_write(0x0D, 0x2A1E);

    // Disable force clocks, and disable hs son signal
    usbphy_write(0x22, 0x030F);

    // Ignore all UTMI errors
    usbphy_write(0x12, 0x0003);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    val_01 |= (1 << 0);
    usbphy_write(0x01, val_01);

#ifdef USB_HS_LOOPBACK_TEST
    usbphy_read(0x0A, &val);
    val |= (1 << 3) | (1 << 4) | (1 << 6);
    usbphy_write(0x0A, val);
    usbphy_read(0x0B, &val);
    val &= ~((1 << 3) | (1 << 4) | (3 << 6));
    usbphy_write(0x0B, val);
    usbphy_read(0x09, &val);
    usbphy_write(0x09, val);
    usbphy_read(0x06, &val);
    usbphy_write(0x06, val);

    hal_sys_timer_delay(MS_TO_TICKS(1));

#define LOOPBACK_PATTERN                (*(volatile uint8_t *)0x20000004)

    LOOPBACK_PATTERN = 0;
    uint8_t pattern = LOOPBACK_PATTERN;

    // Set tx pattern
    usbphy_read(0x03, &val);
    val = (val & ~(0xFF << 8)) | (pattern << 8);
    usbphy_write(0x03, val);

    // Enable force clocks
    usbphy_read(0x22, &val);
    val |= (1 << 4) | (1 << 5) | (1 << 6);
    usbphy_write(0x22, val);

    // Enable tx
    usbphy_read(0x04, &val);
    val |= (1 << 1);
    usbphy_write(0x04, val);

    // Update tx pattern
    while (1) {
        if (pattern != LOOPBACK_PATTERN) {
            pattern = LOOPBACK_PATTERN;
            usbphy_read(0x03, &val);
            val = (val & ~(0xFF << 8)) | (pattern << 8);
            usbphy_write(0x03, val);
        }
    }
#endif

#else // !USB_HIGH_SPEED

    usbphy_read(0x01, &val_01);
    val_01 &= ~(1 << 0);
    usbphy_write(0x01, val_01);

    val = 0;
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    usbphy_write(0x07, val);

    val = 0;
    usbphy_write(0x08, val);

    val = 0;
    usbphy_write(0x09, val);

    val_01 |= (1 << 0);
    usbphy_write(0x01, val_01);

#endif // USB_HIGH_SPEED

}

void usbphy_sleep(void)
{
#ifdef USB_HIGH_SPEED
    uint16_t val;

    usbphy_read(0x01, &val);
    val &= ~((1 << 1) | (1 << 2) | (1 << 3));
    usbphy_write(0x01, val);
#endif

    usbphy_write(0x62, 0x0000);
    usbphy_write(0x63, 0x0000);
}

void usbphy_wakeup(void)
{
    usbphy_write(0x62, 0x2229);
    usbphy_write(0x63, 0x0D82);

#ifdef USB_HIGH_SPEED
    uint16_t val;

    usbphy_read(0x01, &val);
    val |= (1 << 1) | (1 << 2) | (1 << 3);
    usbphy_write(0x01, val);
#endif
}

//============================================================================================
// USB Pin Status Check
//============================================================================================

static enum PMU_USB_PIN_CHK_STATUS_T usb_pin_status;

static PMU_USB_PIN_CHK_CALLBACK usb_pin_callback;

static int pmu_usb_check_pin_status(enum PMU_USB_PIN_CHK_STATUS_T status)
{
    int dp, dm;

    pmu_usb_get_pin_status(&dp, &dm);

    //TRACE("[%X] %s: status=%d dp=%d dm=%d", hal_sys_timer_get(), __FUNCTION__, status, dp, dm);

    // HOST_RESUME: (resume) dp == 0 && dm == 1, (reset) dp == 0 && dm == 0

    if ( (status == PMU_USB_PIN_CHK_DEV_CONN && (dp == 1 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_DEV_DISCONN && (dp == 0 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_HOST_RESUME && dp == 0) ) {
        return 1;
    }

    return 0;
}

static void pmu_usb_pin_irq_handler(void)
{
    uint16_t val;
    uint32_t lock;

    //TRACE("[%X] %s", hal_sys_timer_get(), __FUNCTION__);

    lock = int_lock();
    usbphy_read(USBPHY_REG_1A, &val);
    val |= REG_1A_INTR_CLR;
    usbphy_write(USBPHY_REG_1A, val);
    int_unlock(lock);

    if (usb_pin_callback) {
        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
        }
    }
}

int pmu_usb_config_pin_status_check(enum PMU_USB_PIN_CHK_STATUS_T status, PMU_USB_PIN_CHK_CALLBACK callback, int enable)
{
    uint16_t val;
    uint32_t lock;

    //TRACE("[%X] %s: status=%d", hal_sys_timer_get(), __FUNCTION__, status);

    if (status >= PMU_USB_PIN_CHK_STATUS_QTY) {
        return 1;
    }

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    usb_pin_status = status;
    usb_pin_callback = callback;

    usbphy_read(USBPHY_REG_1A, &val);

    // Mask the irq
    val &= ~REG_1A_USB_INSERT_INTR_MSK;

    // Config pin check
    val |= REG_1A_DEBOUNCE_EN | REG_1A_NOLS_MODE | REG_1A_USBINSERT_DET_EN;

    val &= ~(REG_1A_POL_USB_RX_DP | REG_1A_POL_USB_RX_DM);
    if (status == PMU_USB_PIN_CHK_DEV_CONN) {
        // Check dp 0->1, dm x->0
    } else if (status == PMU_USB_PIN_CHK_DEV_DISCONN) {
        // Check dp 1->0, dm x->0
        val |= REG_1A_POL_USB_RX_DP;
    } else if (status == PMU_USB_PIN_CHK_HOST_RESUME) {
        // Check dp 1->0, dm 0->1 (resume) or dm 0->0 (reset)
        val |= REG_1A_POL_USB_RX_DP;
    }

    if (status != PMU_USB_PIN_CHK_NONE && callback) {
        val |= REG_1A_USBINSERT_INTR_EN | REG_1A_USB_INSERT_INTR_MSK | REG_1A_INTR_CLR;
    }

    usbphy_write(USBPHY_REG_1A, val);

    int_unlock(lock);

    if (enable) {
        // Wait at least 10 cycles of 32K clock for the new status when signal checking polarity is changed
        hal_sys_timer_delay(5);
        pmu_usb_enable_pin_status_check();
    }

    return 0;
}

void pmu_usb_enable_pin_status_check(void)
{
    uint16_t val;
    uint32_t lock;

    if (usb_pin_status != PMU_USB_PIN_CHK_NONE && usb_pin_callback) {
        lock = int_lock();
        usbphy_read(USBPHY_REG_1A, &val);
        val |= REG_1A_INTR_CLR;
        usbphy_write(USBPHY_REG_1A, val);
        int_unlock(lock);
        NVIC_ClearPendingIRQ(USB_PIN_IRQn);

        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
            return;
        }

        NVIC_SetVector(USB_PIN_IRQn, (uint32_t)pmu_usb_pin_irq_handler);
        NVIC_SetPriority(USB_PIN_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(USB_PIN_IRQn);
    }
}

void pmu_usb_disable_pin_status_check(void)
{
    uint16_t val;
    uint32_t lock;

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    usbphy_read(USBPHY_REG_1A, &val);
    val &= ~(REG_1A_USBINSERT_INTR_EN | REG_1A_USBINSERT_DET_EN);
    usbphy_write(USBPHY_REG_1A, val);

    int_unlock(lock);
}

void pmu_usb_get_pin_status(int *dp, int *dm)
{
    uint16_t pol, val;

    usbphy_read(USBPHY_REG_1A, &pol);
    usbphy_read(USBPHY_REG_25, &val);

    *dp = (!(pol & REG_1A_POL_USB_RX_DP)) ^ (!(val & REG_25_USB_STATUS_RX_DP));
    *dm = (!(pol & REG_1A_POL_USB_RX_DM)) ^ (!(val & REG_25_USB_STATUS_RX_DM));
}

#endif

