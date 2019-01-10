#include "plat_types.h"
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_location.h"
#include "hal_spi.h"

#define ANA_REG_CHIP_ID                 0x00

#define ANA_CHIP_ID_SHIFT               (4)
#define ANA_CHIP_ID_MASK                (0xFFF << ANA_CHIP_ID_SHIFT)
#define ANA_CHIP_ID(n)                  BITFIELD_VAL(ANA_CHIP_ID, n)
#define ANA_VAL_CHIP_ID                 0x301

#define ANA_READ_CMD(r)                 ((1 << 24) | (((r) & 0xFF) << 16))
#define ANA_WRITE_CMD(r, v)             ((((r) & 0xFF) << 16) | ((v) & 0xFFFF))
#define ANA_READ_VAL(v)                 ((v) & 0xFFFF)

#define ANA_PAGE_NUM                    2
#define ANA_PAGE_0                      (0xA000)
#define ANA_PAGE_1                      (0xA010)

#define ISPI_REG_CS(r)                  ((r) >> 12)
#define ISPI_REG_PAGE(r)                (((r) >> 8) & 0xF)
#define ISPI_REG_OFFSET(r)              ((r) & 0xFF)

static uint8_t BOOT_BSS_LOC ana_cs;

static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = true,
    .cs = 0,
    .rate = 6500000,
    .tx_bits = 25,
    .rx_bits = 25,
    .rx_frame_bits = 0,
};

// Only PMU(CS=0) and ANA(CS=1) have multiple pages
static const uint16_t page_reg[] = { 0x00, 0x60, };
// Max page number is 2
static const uint16_t page_val[ANA_PAGE_NUM] = { ANA_PAGE_0, ANA_PAGE_1, };

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
    int ret;
    uint8_t cs;
    uint8_t page;

    cs = ISPI_REG_CS(reg);
    page = ISPI_REG_PAGE(reg);
    reg = ISPI_REG_OFFSET(reg);

    lock = int_lock();
    if (cs != ana_cs) {
        hal_ispi_activate_cs(cs);
        ana_cs = cs;
    }
    if (page) {
        hal_analogif_rawwrite(page_reg[cs & 1], page_val[page & 1]);
    }
    ret = hal_analogif_rawread(reg, val);
    if (page) {
        hal_analogif_rawwrite(page_reg[cs & 1], page_val[0]);
    }
    int_unlock(lock);

    return ret;
}

int hal_analogif_reg_write(unsigned short reg, unsigned short val)
{
    uint32_t lock;
    int ret;
    uint8_t cs;
    uint8_t page;

    cs = ISPI_REG_CS(reg);
    page = ISPI_REG_PAGE(reg);
    reg = ISPI_REG_OFFSET(reg);

    lock = int_lock();
    if (cs != ana_cs) {
        hal_ispi_activate_cs(cs);
        ana_cs = cs;
    }
    if (page) {
        hal_analogif_rawwrite(page_reg[cs & 1], page_val[page & 1]);
    }
    ret = hal_analogif_rawwrite(reg, val);
    if (page) {
        hal_analogif_rawwrite(page_reg[cs & 1], page_val[0]);
    }
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

    ana_cs = 0;
    hal_ispi_activate_cs(ana_cs);

    ret = hal_analogif_rawread(ANA_REG_CHIP_ID, &chip_id);
    if (ret) {
        return ret;
    }

    if (GET_BITFIELD(chip_id, ANA_CHIP_ID) != ANA_VAL_CHIP_ID) {
        return -1;
    }

    return 0;
}

