#include "usbphy.h"
#include "hal_spi.h"
#include "hal_timer.h"
#include "hal_trace.h"

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

void usbphy_open(void)
{
    int ret;
    unsigned short val_01, val;

    ret = hal_spiphy_open(&spi_cfg);
    if (ret) {
        return;
    }

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

    // Tx off during rx
    usbphy_read(0x06, &val);
    val |= (1 << 13) | (1 << 14);
    // Set hs_ibias_trim to 7 (default is 4)
    //val = (val & ~(7 << 4)) | (7 << 4);
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~(1 << 8);
    usbphy_write(0x07, val);

    usbphy_read(0x08, &val);
    val = (val & ~((0xF << 0) | (0x3 << 4) | (0x3 << 6))) | (0x0 << 0) | (0x3 << 4) | (0x1 << 6);
    usbphy_write(0x08, val);

    usbphy_read(0x09, &val);
    val &= ~(1 << 0);
    val |= (1 << 3);
    usbphy_write(0x09, val);

    // Disable force clocks, and disable hs son signal
    usbphy_read(0x22, &val);
    val &= ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));
    usbphy_write(0x22, val);

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
    val |= (1 << 2);
    usbphy_write(0x09, val);
    usbphy_read(0x06, &val);
    val |= (1 << 12);
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

    val = (1 << 13);
    usbphy_write(0x06, val);

    usbphy_read(0x07, &val);
    val &= ~(1 << 8);
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
}

void usbphy_wakeup(void)
{
#ifdef USB_HIGH_SPEED
    uint16_t val;

    usbphy_read(0x01, &val);
    val |= (1 << 1) | (1 << 2) | (1 << 3);
    usbphy_write(0x01, val);
#endif
}

#endif

