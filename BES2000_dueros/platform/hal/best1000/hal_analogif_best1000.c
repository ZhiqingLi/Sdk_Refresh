#include "plat_types.h"
#include "cmsis.h"
#include "hal_analogif.h"
#include "hal_location.h"
#include "hal_iomux.h"
#include "hal_spi.h"

#define ANA_REG_CHIP_ID                 0x5E

#define ANA_VAL_CHIP_ID                 0xC021

#define ANA_READ_CMD(r)                 ((1 << 31) | (((r) & 0x7F) << 24))
#define ANA_WRITE_CMD(r, v)             ((((r) & 0x7F) << 24) | (((v) & 0xFFFF) << 8))
#define ANA_READ_VAL(v)                 (((v) >> 8) & 0xFFFF);

static const struct HAL_SPI_CFG_T spi_cfg = {
    .clk_delay_half = false,
    .clk_polarity = false,
    .slave = false,
    .dma_rx = false,
    .dma_tx = false,
    .rx_sep_line = false,
    .cs = 0,
    .rate = 6500000,
    .tx_bits = 32,
    .rx_bits = 32,
    .rx_frame_bits = 0,
};

inline static int hal_analogif_rawread(unsigned int reg, unsigned short *val)
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

inline static int hal_analogif_rawwrite(unsigned int reg, unsigned short val)
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

    if (chip_id != ANA_VAL_CHIP_ID) {
        return -1;
    }

    return 0;
}

static uint32_t BOOT_BSS_LOC gSpiPage = 0;

#define PAGE0_CHANGETO_PAGE1() do{ \
								uint16_t readVal = 0; \
								hal_analogif_rawread(0x40, &readVal); \
								readVal &= ~(0x01); \
								readVal |= 0x0080; \
								hal_analogif_rawwrite(0x40, readVal); \
								gSpiPage = 1;\
							 }while(0)

#define PAGE0_CHANGETO_PAGE2() do{ \
								PAGE0_CHANGETO_PAGE1(); \
								PAGE1_CHANGETO_PAGE2(); \
								gSpiPage = 2;\
							 }while(0)

#define PAGE1_CHANGETO_PAGE0() do{ \
								hal_analogif_rawwrite(0xc0,0x0000); \
								gSpiPage = 0;\
							 }while(0)


#define PAGE1_CHANGETO_PAGE2() do{ \
								hal_analogif_rawwrite(0xc0, 0x8080); \
								gSpiPage = 2;\
							 }while(0)

#define PAGE2_CHANGETO_PAGE0() do{ \
								PAGE1_CHANGETO_PAGE0(); \
								gSpiPage = 0;\
							 }while(0)

#define PAGE2_CHANGETO_PAGE1() do{ \
								hal_analogif_rawwrite(0xc0, 0x0080); \
								gSpiPage = 1;\
							 }while(0)

inline static void hal_analogif_reg_pageselect(uint16_t reg)
{
	if (reg > 0xff)
	{
		if (gSpiPage == 0)
			PAGE0_CHANGETO_PAGE2();
		else if (gSpiPage == 1)
			PAGE1_CHANGETO_PAGE2();
	}
	else if (0x7f < reg && reg < 0x100)
	{
		if (gSpiPage == 0)
			PAGE0_CHANGETO_PAGE1();
		else if (gSpiPage == 2)
			PAGE2_CHANGETO_PAGE1();
	}
	else{
		if (gSpiPage == 1)
			PAGE1_CHANGETO_PAGE0();
		else if (gSpiPage == 2)
			PAGE2_CHANGETO_PAGE0();
	}
}

inline static void hal_analogif_reg_backtopage0(void)
{

	if (gSpiPage == 0){
	}else if (gSpiPage == 1){
		PAGE1_CHANGETO_PAGE0();
	}else if (gSpiPage == 2){
		PAGE2_CHANGETO_PAGE1();
		PAGE1_CHANGETO_PAGE0();
	}
	gSpiPage = 0;
}

static inline uint32_t hal_analogif_spi_access_lock(unsigned short reg)
{
    enum HAL_IOMUX_ISPI_ACCESS_T access;

    if (reg >= 0xA0 && reg < 0xC0) {
        access = HAL_IOMUX_ISPI_MCU_RF;
    } else if (reg >= 0xC0) {
        access = HAL_IOMUX_ISPI_MCU_ANA;
    } else {
        return 0;
    }

    return hal_iomux_ispi_access_disable(access);
}

static inline void hal_analogif_spi_access_unlock(uint32_t access)
{
    if (access) {
        hal_iomux_ispi_access_enable(access);
    }
}

int hal_analogif_reg_read(unsigned short reg, unsigned short *val)
{
    uint32_t lock = int_lock();
    uint32_t access = hal_analogif_spi_access_lock(reg);

	hal_analogif_reg_pageselect(reg);

	hal_analogif_rawread(reg, val);

	hal_analogif_reg_backtopage0();

    hal_analogif_spi_access_unlock(access);
    int_unlock(lock);
    return 0;
}

int hal_analogif_reg_write(unsigned short reg, unsigned short val)
{
    uint32_t lock = int_lock();
    uint32_t access = hal_analogif_spi_access_lock(reg);

	hal_analogif_reg_pageselect(reg);

	hal_analogif_rawwrite(reg, val);

	hal_analogif_reg_backtopage0();

    hal_analogif_spi_access_unlock(access);
    int_unlock(lock);
    return 0;
}

