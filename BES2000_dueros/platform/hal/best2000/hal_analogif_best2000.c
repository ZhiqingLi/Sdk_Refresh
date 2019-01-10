#include "plat_types.h"
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_location.h"
#include "hal_spi.h"

#define ANA_REG_CHIP_ID                 0x00

#define ANA_CHIP_ID_SHIFT               (4)
#define ANA_CHIP_ID_MASK                (0xFFF << ANA_CHIP_ID_SHIFT)
#define ANA_CHIP_ID(n)                  BITFIELD_VAL(ANA_CHIP_ID, n)
#define ANA_VAL_CHIP_ID                 0x20E

#define ANA_READ_CMD(r)                 ((1 << 24) | (((r) & 0xFF) << 16))
#define ANA_WRITE_CMD(r, v)             ((((r) & 0xFF) << 16) | ((v) & 0xFFFF))
#define ANA_READ_VAL(v)                 ((v) & 0xFFFF)

struct PAGE_REG_VAL_T {
    uint8_t reg;
    uint16_t pagex;
    uint16_t page0;
};

static const struct PAGE_REG_VAL_T page_info[3] = {
    { 0x60, 0xA010, 0xA000, },
    { 0xC0, 0x8000, 0x0000, },
    { 0xC0, 0x4000, 0x0000, },
};

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

static int hal_analogif_rawread(unsigned short reg, unsigned short *val)
{
    int ret;
    unsigned int data;
    unsigned int cmd;

    data = 0;
    cmd = ANA_READ_CMD(reg);
    ret = hal_ispi_recv(&cmd, &data, 4);
    if (ret) {
        return ret;
    }
    *val = ANA_READ_VAL(data);
    return 0;
}

static int hal_analogif_rawwrite(unsigned short reg, unsigned short val)
{
    int ret;
    unsigned int cmd;

    cmd = ANA_WRITE_CMD(reg, val);
    ret = hal_ispi_send(&cmd, 4);
    if (ret) {
        return ret;
    }
    return 0;
}

int hal_analogif_reg_read(unsigned short reg, unsigned short *val)
{
    uint32_t lock;
    uint32_t idx;
    int ret;

    if (reg < 0x100) {
        lock = int_lock();
        ret = hal_analogif_rawread(reg, val);
        int_unlock(lock);
        return ret;
    } else if (reg >= 0x160 && reg <= 0x16A) {
        idx = 0;
    } else if (reg >= 0x180 && reg <= 0x1FF) {
        idx = 1;
    } else if (reg >= 0x280 && reg <= 0x2FF) {
        idx = 2;
    } else {
        return -1;
    }

    reg &= 0xFF;

    lock = int_lock();
    hal_analogif_rawwrite(page_info[idx].reg, page_info[idx].pagex);
    ret = hal_analogif_rawread(reg, val);
    hal_analogif_rawwrite(page_info[idx].reg, page_info[idx].page0);
    int_unlock(lock);

    return ret;
}

int hal_analogif_reg_write(unsigned short reg, unsigned short val)
{
    uint32_t lock;
    uint32_t idx;
    int ret;

    if (reg < 0x100) {
        lock = int_lock();
        ret = hal_analogif_rawwrite(reg, val);
        int_unlock(lock);
        return ret;
    } else if (reg >= 0x160 && reg <= 0x16A) {
        idx = 0;
    } else if (reg >= 0x180 && reg <= 0x1FF) {
        idx = 1;
    } else if (reg >= 0x280 && reg <= 0x2FF) {
        idx = 2;
    } else {
        return -1;
    }

    reg &= 0xFF;

    lock = int_lock();
    hal_analogif_rawwrite(page_info[idx].reg, page_info[idx].pagex);
    ret = hal_analogif_rawwrite(reg, val);
    hal_analogif_rawwrite(page_info[idx].reg, page_info[idx].page0);
    int_unlock(lock);

    return ret;
}

int BOOT_TEXT_FLASH_LOC hal_analogif_open(void)
{
    int ret;
    unsigned short chip_id;

    ret = hal_ispi_open(&spi_cfg);
    if (ret) {
        return ret;
    }

    ret = hal_analogif_rawread(ANA_REG_CHIP_ID, &chip_id);
    if (ret) {
        return ret;
    }

    if (GET_BITFIELD(chip_id, ANA_CHIP_ID) != ANA_VAL_CHIP_ID) {
        return -1;
    }

    return 0;
}

