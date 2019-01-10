#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_norflaship.h"
#include "hal_norflash.h"
#include "hal_bootmode.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "pmu.h"
#include "string.h"

/* Demo:
 *
 *  uint8_t data[1024];
 *  hal_norflash_open(HAL_NORFLASH_ID_0, HAL_NORFLASH_SPEED_26M, 0);
 *  \/\/ hal_norflash_open(HAL_NORFLASH_ID_0, HAL_NORFLASH_SPEED_13M, DRV_NORFLASH_OP_MODE_QUAD);
 *  \/\/ hal_norflash_open(HAL_NORFLASH_ID_0, HAL_NORFLASH_SPEED_13M, DRV_NORFLASH_OP_MODE_QUAD|DRV_NORFLASH_OP_MODE_CONTINUOUS_READ);
 *  hal_norflash_erase(HAL_I2C_ID_0, 0, 4096);
 *  memset(data, 0xcc, 1024);
 *  hal_norflash_write(HAL_I2C_ID_0, 0, data, 1024);
 *  for (i = 0; i < 10; ++i) {
 *      TRACE("[0x%x] - 0x%x\n", 0x08000000 + i, *((volatile uint8_t *)(0x08000000 + i)));
 *  }
*/

#define HAL_NORFLASH_CP_ID_LEN              2  // add begin by cai.zhong 20190105 for unique id

#define HAL_NORFLASH_YES 1
#define HAL_NORFLASH_NO 0

/* control clk size in bytes each time to read data , it is max value of BLKSIZE */
/* see TX_BLKSIZE_MASK register bits width */
#define NORFLASHIP_ONCE_READ_SIZE (256)

/* read(rx) fifo size in bytes */
#define NORFLASHIP_RXFIFO_SIZE_MAX (8)

#define NORFLASH_DEVICE_ID_LEN_MAX 8

struct HAL_Norflash_Context {
    bool opened;
    uint8_t device_id[NORFLASH_DEVICE_ID_LEN_MAX];
// add begin by cai.zhong 20190105 for unique id
#ifdef FLASH_UNIQUE_ID
    uint8_t unique_id[HAL_NORFLASH_UNIQUE_ID_LEN + HAL_NORFLASH_CP_ID_LEN];
#endif
// add end by cai.zhong 20190105 for unique id
    uint32_t total_size;
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;
};

static struct HAL_Norflash_Context norflash_ctx[HAL_NORFLASH_ID_NUM];

static const char * const err_not_opened = "norflash not opened";

static const struct HAL_NORFLASH_CONFIG_T norflash_cfg = {
#ifdef FPGA
    .source_clk = HAL_NORFLASH_SPEED_13M * 2,
    .speed = HAL_NORFLASH_SPEED_13M,
#elif defined(FLASH_LOW_SPEED)
    .source_clk = HAL_NORFLASH_SPEED_26M * 2,
    .speed = HAL_NORFLASH_SPEED_26M,
#else
    .source_clk = HAL_NORFLASH_SPEED_104M * 2,
    .speed = HAL_NORFLASH_SPEED_104M,
#endif
    .mode  = (enum DRV_NORFLASH_OP_MODE)(DRV_NORFLASH_OP_MODE_QUAD | DRV_NORFLASH_OP_MODE_CONTINUOUS_READ),
    .override_config = 0,
};

static enum HAL_CMU_FREQ_T hal_norflash_clk_to_cmu_freq(uint32_t clk)
{
    if (clk >= HAL_NORFLASH_SPEED_208M) {
        return HAL_CMU_FREQ_208M;
    } else if (clk >= HAL_NORFLASH_SPEED_104M) {
        return HAL_CMU_FREQ_104M;
    } else if (clk >= HAL_NORFLASH_SPEED_78M) {
        return HAL_CMU_FREQ_78M;
    } else if (clk >= HAL_NORFLASH_SPEED_52M) {
        return HAL_CMU_FREQ_52M;
    } else {
        return HAL_CMU_FREQ_26M;
    }
}

uint8_t hal_norflash_get_size(enum HAL_NORFLASH_ID_T id, uint32_t *total_size,
        uint32_t *block_size, uint32_t *sector_size, uint32_t *page_size)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    if (total_size) {
        *total_size = norflash_ctx[id].total_size;
    }
    if (block_size) {
        *block_size = norflash_ctx[id].block_size;
    }
    if (sector_size) {
        *sector_size = norflash_ctx[id].sector_size;
    }
    if (page_size) {
        *page_size = norflash_ctx[id].page_size;
    }
    return 0;
}

uint8_t hal_norflash_get_boundary(enum HAL_NORFLASH_ID_T id, uint32_t address, uint32_t *block_boundary, uint32_t *sector_boundary)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    static const uint32_t flash_base[] = { FLASH_BASE, FLASHX_BASE, FLASH_NC_BASE, FLASHX_NC_BASE, };
    int i;

    for (i = 0; i < ARRAY_SIZE(flash_base); i++) {
        if (flash_base[i] <= address && address < flash_base[i] + norflash_ctx[id].total_size) {
            address -= flash_base[i];
            if (block_boundary)
                *block_boundary  = flash_base[i] + (address/norflash_ctx[id].block_size)*norflash_ctx[id].block_size;
            if (sector_boundary)
                *sector_boundary = flash_base[i] + (address/norflash_ctx[id].sector_size)*norflash_ctx[id].sector_size;
            break;
        }
    }

    if (i == ARRAY_SIZE(flash_base)) {
        return 2;
    }

    return 0;
}

uint8_t hal_norflash_get_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    memcpy(value, norflash_ctx[id].device_id, MIN(len, sizeof(norflash_ctx[id].device_id)));
    return 0;
}

// add begin by cai.zhong 20190105 for unique id
#ifdef FLASH_UNIQUE_ID
uint8_t hal_norflash_get_unique_id(enum HAL_NORFLASH_ID_T id, uint8_t *value, uint32_t len)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);

    len = MIN(len, sizeof(norflash_ctx[id].unique_id));

    memcpy(value, norflash_ctx[id].unique_id, len);

    return 0;
}
#endif
// add end by cai.zhong 20190105 for unique id

uint8_t hal_norflash_open(enum HAL_NORFLASH_ID_T id, const struct HAL_NORFLASH_CONFIG_T *cfg)
{
    uint8_t div = 0;
    struct HAL_NORFLASH_CONFIG_T norcfg;

    // Place the config into ram
    memcpy(&norcfg, cfg, sizeof(norcfg));

    div = norcfg.source_clk/norcfg.speed;

    /* over write config */
    if (norcfg.override_config) {
        /* div */
        norflaship_div(norcfg.div);

        /* cmd quad */
        norflaship_cmdquad(norcfg.cmdquad?HAL_NORFLASH_YES:HAL_NORFLASH_NO);

        /* sample delay */
        norflaship_samdly(norcfg.samdly);

#if 0
        /* dummy clc */
        norflaship_dummyclc(norcfg.dummyclc);

        /* dummy clc en */
        norflaship_dummyclcen(norcfg.dummyclcen);

        /* 4 byte address */
        norflaship_addrbyte4(norcfg.byte4byteaddr);
#endif

        /* ru en */
        norflaship_ruen(norcfg.spiruen);

        /* rd en */
        norflaship_rden(norcfg.spirden);

        /* rd cmd */
        norflaship_rdcmd(norcfg.rdcmd);

        /* frd cmd */
        norflaship_frdcmd(norcfg.frdcmd);

        /* qrd cmd */
        norflaship_qrdcmd(norcfg.qrdcmd);
    }
    else {
        norflaship_div(div);
    }

#ifdef SIMU
#ifdef SIMU_FAST_FLASH
#define MAX_SIMU_FLASH_FREQ     HAL_CMU_FREQ_104M
#else
#define MAX_SIMU_FLASH_FREQ     HAL_CMU_FREQ_52M
#endif
    {
        enum HAL_CMU_FREQ_T source_clk;

        source_clk = hal_norflash_clk_to_cmu_freq(norcfg.source_clk);
        if (source_clk > MAX_SIMU_FLASH_FREQ) {
            source_clk = MAX_SIMU_FLASH_FREQ;
            hal_cmu_flash_set_freq(source_clk);
            norflaship_div(2);
        }
    }
#endif

    /* foreach driver in array, match chip and select drv_ops */
    norflash_ctx[id].opened = !!norflash_match_chip(cfg);

    if (!norflash_ctx[id].opened)
        return 1;

    norflash_get_size(&norflash_ctx[id].total_size, &norflash_ctx[id].block_size,
        &norflash_ctx[id].sector_size, &norflash_ctx[id].page_size);

    norflash_get_id(norflash_ctx[id].device_id, sizeof(norflash_ctx[id].device_id));

// add begin by cai.zhong 20190105 for unique id
#ifdef FLASH_UNIQUE_ID
#if (CHIP_FLASH_CTRL_VER == 1)
    // Slow down to avoid rx fifo overflow
    norflaship_div(8);
#endif
    norflash_get_unique_id(norflash_ctx[id].unique_id, sizeof(norflash_ctx[id].unique_id));
    norflaship_div(div);
#endif
// add end by cai.zhong 20190105 for unique id

#if (CHIP_FLASH_CTRL_VER == 1) && !defined(FLASH_LOW_SPEED) && !defined(OSC_26M_X4_AUD2BB)
    // 1) Flash controller V2 or later
    //    No requirement on system_freq
    // 2) Flash controller V1
    //    Reqirement on system_freq when running in quad mode (4-line mode):
    //      Byte Access:     flash_line_speed < 2 * system_freq
    //      Halfword Access: flash_line_speed < 4 * system_freq
    //      Word Access:     flash_line_speed < 8 * system_freq
    //    The maximum flash_line_speed is 120M in spec, and PLL_FREQ / 2 in our system.
    //    Normally it is 24.576M * 8 / 2 ~= 100M.
    //    So the safe system_freq should be larger than 50M/25M/12.5M for byte/halfword/word access.
    //    Cached access to flash is always safe, because it is always word-aligned (system_freq is never below 26M).
    //    However, uncached access (e.g., access to audio/user/factory data sections) is under risk.
    hal_sysfreq_set_min_freq(HAL_CMU_FREQ_52M);
#endif

#ifdef PROGRAMMER
    if (norcfg.speed >= HAL_NORFLASH_SPEED_104M) {
        norcfg.mode |= DRV_NORFLASH_OP_MODE_HIGH_PERFORMANCE;
    }
#endif

    /* quad mode, continuous read */
    norflash_set_mode(norcfg.mode, DRV_NORFLASH_OP_MODE_QUAD|DRV_NORFLASH_OP_MODE_CONTINUOUS_READ);

    return 0;
}

uint8_t hal_norflash_erase_resume(enum HAL_NORFLASH_ID_T id)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);
    return 0;
}

uint8_t hal_norflash_erase_suspend(enum HAL_NORFLASH_ID_T id)
{
    ASSERT(norflash_ctx[id].opened, err_not_opened);
    return 0;
}

uint8_t hal_norflash_erase_chip(enum HAL_NORFLASH_ID_T id)
{
    uint32_t total_size = 0;

    ASSERT(norflash_ctx[id].opened, err_not_opened);

    total_size = norflash_ctx[id].total_size;

    hal_norflash_erase(id, 0, total_size);
    return 0;
}

uint8_t hal_norflash_erase(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t remain_len = 0, current_address = 0, total_size = 0, block_size = 0, sector_size = 0;

    ASSERT(norflash_ctx[id].opened, err_not_opened);
    //ASSERT((len%norflash_ctx[id].sector_size) == 0, "norflash erase len \% sector_size != 0!");
    //ASSERT((start_address%norflash_ctx[id].sector_size) == 0, "norflash erase len \% sector_size != 0!");

    total_size      = norflash_ctx[id].total_size;
    block_size      = norflash_ctx[id].block_size;
    sector_size     = norflash_ctx[id].sector_size;
    current_address = start_address;

    remain_len      = (len + sector_size - 1) / sector_size * sector_size;

    norflash_pre_operation();

    while (remain_len > 0) {
        /* erase whole chip */
        if (remain_len >= total_size) {
            norflash_erase(current_address, DRV_NORFLASH_ERASE_CHIP);
            remain_len      -= total_size;
            current_address += total_size;
        }
        /* if large enough to erase a block and current_address is block boundary - erase a block */
        else if (remain_len >= block_size && ((current_address % block_size) == 0)) {
            norflash_erase(current_address, DRV_NORFLASH_ERASE_BLOCK);
            remain_len      -= block_size;
            current_address += block_size;
        }
        /* erase a sector */
        else {
            norflash_erase(current_address, DRV_NORFLASH_ERASE_SECTOR);
            remain_len      -= sector_size;
            current_address += sector_size;
        }
    }

    norflash_post_operation();

    return 0;
}

uint8_t hal_norflash_write(enum HAL_NORFLASH_ID_T id, uint32_t start_address, const uint8_t *buffer, uint32_t len)
{
    const uint8_t *current_buffer = 0;
    uint32_t remain_len = 0, current_address = 0, page_size = 0, write_size = 0;
    uint32_t page_start;

    ASSERT(norflash_ctx[id].opened, err_not_opened);
    //ASSERT((len%norflash_ctx[id].page_size) == 0, "norflash write length \% pagesize != 0\n");
    //ASSERT((start_address%norflash_ctx[id].page_size) == 0, "norflash write address \% pagesize != 0\n");

    remain_len      = len;
    page_size       = norflash_ctx[id].page_size;
    current_address = start_address;
    current_buffer  = buffer;

    norflash_pre_operation();

    // Write unaligned first page
    page_start = current_address & ~(page_size - 1);
    if (current_address != page_start) {
        // Get write len in the first page
        write_size = page_start + page_size - current_address;
        if (write_size <= remain_len) {
            norflash_write(current_address, current_buffer, write_size);
            current_address += write_size;
            current_buffer  += write_size;
            remain_len      -= write_size;
        }
    }

    while (remain_len > 0) {
        if (remain_len >= page_size) {
            write_size = page_size;
        }
        else {
            write_size = remain_len;
        }

        norflash_write(current_address, current_buffer, write_size);

        current_address += write_size;
        current_buffer  += write_size;
        remain_len      -= write_size;
    }

    norflash_post_operation();

    return 0;
}

uint8_t hal_norflash_read(enum HAL_NORFLASH_ID_T id, uint32_t start_address, uint8_t *buffer, uint32_t len)
{
    uint8_t *current_buffer = 0;
    uint32_t remain_len = 0, current_address = 0, read_size = 0;

    ASSERT(norflash_ctx[id].opened, err_not_opened);
    //ASSERT((len%norflash_ctx[id].page_size) == 0, "norflash read length \% NORFLASHIP_ONCE_READ_SIZE != 0\n");

    /* do NOT use NORFLASHIP_ONCE_READ_SIZE, see comments at gd25lq32c_drv_norflash_read function */
    read_size       = NORFLASHIP_RXFIFO_SIZE_MAX;
    remain_len      = len;
    current_address = start_address;
    current_buffer  = buffer;

    norflash_pre_operation();

    while (remain_len > 0) {
        norflash_read(current_address, current_buffer, read_size);

        current_address += read_size;
        current_buffer  += read_size;
        remain_len      -= read_size;
    }

    norflash_post_operation();
    return 0;
}

uint8_t hal_norflash_close(enum HAL_NORFLASH_ID_T id)
{
    return 0;
}

void hal_norflash_sleep(enum HAL_NORFLASH_ID_T id)
{
    if (!norflash_ctx[id].opened) {
        return;
    }

#ifdef FLASH_DEEP_SLEEP
    norflash_sleep();
#else
    norflash_pre_operation();
#endif
}

void hal_norflash_wakeup(enum HAL_NORFLASH_ID_T id)
{
    if (!norflash_ctx[id].opened) {
        return;
    }

#ifdef FLASH_DEEP_SLEEP
    norflash_wakeup();
#else
    norflash_post_operation();
#endif

    // Flush prefetch buffer
    *(volatile uint32_t *)FLASH_BASE;
    *(volatile uint32_t *)(FLASH_BASE + 0x1000);
}

static void hal_norflash_prefetch_idle(void)
{
    hal_sys_timer_delay(4);
    if (norflaship_is_busy()) {
        hal_sys_timer_delay(4);
    }
}

uint8_t hal_norflash_init(void)
{
    int ret;

    // Pmu codes might be located in flash
    pmu_flash_freq_config(norflash_cfg.speed);

    // Avoid flash access from here
    hal_norflash_prefetch_idle();

    hal_cmu_flash_set_freq(hal_norflash_clk_to_cmu_freq(norflash_cfg.source_clk));

    ret = hal_norflash_open(HAL_NORFLASH_ID_0, &norflash_cfg);
    // Flash can be access again

    return ret;
}

uint32_t hal_norflash_get_flash_total_size(void)
{
    return norflash_ctx[0].total_size;
}

