#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_cache.h"
#include "hal_location.h"

/* cache controller */

/* reg value */
#define CACHE_ENABLE_REG_OFFSET             0x00
#define CACHE_INI_CMD_REG_OFFSET            0x04
#define WRITEBUFFER_ENABLE_REG_OFFSET       0x08
#define WRITEBUFFER_FLUSH_REG_OFFSET        0x0C
#define LOCK_UNCACHEABLE_REG_OFFSET         0x10
#define INVALIDATE_ADDRESS_REG_OFFSET       0x14
#define INVALIDATE_SET_CMD_REG_OFFSET       0x18
// Since best2300
#define MONITOR_ENABLE_REG_OFFSET           0x1C
#define MONITOR_CNT_READ_HIT0_REG_OFFSET    0x20
#define MONITOR_CNT_READ_HIT1_REG_OFFSET    0x24
#define MONITOR_CNT_READ_MISS0_REG_OFFSET   0x28
#define MONITOR_CNT_READ_MISS1_REG_OFFSET   0x2C

#define CACHE_EN                            (1 << 0)
// Since best2300
#define WRAP_EN                             (1 << 1)

#define WRITEBUFFER_EN                      (1 << 0)
// Since best2300
#define WRITE_BACK_EN                       (1 << 1)

#define LOCK_UNCACHEABLE                    (1 << 0)

// Since best2300
#define MONITOR_EN                          (1 << 0)

#define CNT_READ_HIT_31_0_SHIFT             (0)
#define CNT_READ_HIT_31_0_MASK              (0xFFFFFFFF << CNT_READ_HIT_31_0_SHIFT)
#define CNT_READ_HIT_31_0(n)                BITFIELD_VAL(CNT_READ_HIT_31_0, n)

#define CNT_READ_HIT_39_32_SHIFT            (0)
#define CNT_READ_HIT_39_32_MASK             (0xFF << CNT_READ_HIT_39_32_SHIFT)
#define CNT_READ_HIT_39_32(n)               BITFIELD_VAL(CNT_READ_HIT_39_32, n)

/* read write */
#define cacheip_write32(v,b,a) \
    (*((volatile uint32_t *)(b+a)) = v)
#define cacheip_read32(b,a) \
    (*((volatile uint32_t *)(b+a)))

static inline void cacheip_enable_cache(uint32_t reg_base, uint32_t v)
{
    if (v)
        cacheip_write32((CACHE_EN | WRAP_EN), reg_base, CACHE_ENABLE_REG_OFFSET);
    else
        cacheip_write32(0, reg_base, CACHE_ENABLE_REG_OFFSET);
}
static inline void cacheip_init_cache(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, CACHE_INI_CMD_REG_OFFSET);
}
static inline void cacheip_enable_writebuffer(uint32_t reg_base, uint32_t v)
{
    if (v)
        cacheip_write32(WRITEBUFFER_EN, reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
    else
        cacheip_write32(0, reg_base, WRITEBUFFER_ENABLE_REG_OFFSET);
}
static inline void cacheip_flush_writebuffer(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, WRITEBUFFER_FLUSH_REG_OFFSET);
}
static inline void cacheip_set_invalidate_address(uint32_t reg_base, uint32_t v)
{
    cacheip_write32(v, reg_base, INVALIDATE_ADDRESS_REG_OFFSET);
}
static inline void cacheip_trigger_invalidate(uint32_t reg_base)
{
    cacheip_write32(1, reg_base, INVALIDATE_SET_CMD_REG_OFFSET);
}
/* cache controller end */

/* hal api */
static inline uint32_t _cache_get_reg_base(enum HAL_CACHE_ID_T id)
{
    switch (id) {
        default:
        case HAL_CACHE_ID_I_CACHE:
            return ICACHE_CTRL_BASE;
        break;
#ifdef DCACHE_CTRL_BASE
        case HAL_CACHE_ID_D_CACHE:
            return DCACHE_CTRL_BASE;
        break;
#endif
    }

    return 0;
}
uint8_t BOOT_TEXT_FLASH_LOC hal_cache_enable(enum HAL_CACHE_ID_T id, uint32_t val)
{
    uint32_t reg_base = 0;
    reg_base = _cache_get_reg_base(id);

    if (reg_base == 0) {
        return 0;
    }

    if (val)
        cacheip_init_cache(reg_base);

    cacheip_enable_cache(reg_base, val);
    return 0;
}
uint8_t BOOT_TEXT_FLASH_LOC hal_cache_writebuffer_enable(enum HAL_CACHE_ID_T id, uint32_t val)
{
    uint32_t reg_base = 0;
    reg_base = _cache_get_reg_base(id);

    if (reg_base == 0) {
        return 0;
    }

    cacheip_enable_writebuffer(reg_base, val);
    return 0;
}
uint8_t hal_cache_writebuffer_flush(enum HAL_CACHE_ID_T id)
{
    uint32_t reg_base = 0;
    reg_base = _cache_get_reg_base(id);

    if (reg_base == 0) {
        return 0;
    }

    cacheip_flush_writebuffer(reg_base);
    return 0;
}
uint8_t hal_cache_invalidate(enum HAL_CACHE_ID_T id, uint32_t start_address, uint32_t len)
{
    uint32_t temp = 0;
    uint32_t reg_base = 0;
    reg_base = _cache_get_reg_base(id);

    if (reg_base == 0) {
        return 0;
    }

    cacheip_set_invalidate_address(reg_base, start_address);
    temp = *((volatile uint8_t *)start_address);
    temp = temp;
    cacheip_trigger_invalidate(reg_base);

    return 0;
}
