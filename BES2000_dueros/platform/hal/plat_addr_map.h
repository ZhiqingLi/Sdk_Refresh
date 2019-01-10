#ifndef __PLAT_ADDR_MAP_H__
#define __PLAT_ADDR_MAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#elif defined(CHIP_BEST3001)
#define CHIP_ID_LITERAL                         best3001
#elif defined(CHIP_BEST2000)
#define CHIP_ID_LITERAL                         best2000
#elif defined(CHIP_BEST2300)
#define CHIP_ID_LITERAL                         best2300
#elif defined(CHIP_BEST1200)
#define CHIP_ID_LITERAL                         best1200
#else
#define CHIP_ID_LITERAL                         best1000
#endif

#define _TO_STRING_A(s)                         # s
#define _TO_STRING(s)                           _TO_STRING_A(s)

#define CONCAT_NAME_A(a, b)                     a ## b
#define CONCAT_NAME(a, b)                       CONCAT_NAME_A(a, b)
#define CONCAT_SUFFIX(a, b)                     a.b

#define CONSTRUCT_HDR_NAME_A(d, n)              d/n ## _ ## d
#define CONSTRUCT_HDR_NAME(d, n)                CONSTRUCT_HDR_NAME_A(d, n)
#define CHIP_SPECIFIC_HDR(n)                    _TO_STRING(CONSTRUCT_HDR_NAME(CHIP_ID_LITERAL, n).h)

#include CHIP_SPECIFIC_HDR(plat_addr_map)

#define RAM_TO_RAMX(d)                          ((d) - RAM_BASE + RAMX_BASE)
#define RAMX_TO_RAM(d)                          ((d) - RAMX_BASE + RAM_BASE)

#define FLASH_TO_FLASHX(d)                      ((d) - FLASH_BASE + FLASHX_BASE)
#define FLASHX_TO_FLASH(d)                      ((d) - FLASHX_BASE + FLASH_BASE)

#define FLASH_C_TO_NC(d)                        ((d) - FLASH_BASE + FLASH_NC_BASE)
#define FLASH_NC_TO_C(d)                        ((d) - FLASH_NC_BASE + FLASH_BASE)

#define PSRAM_TO_PSRAMX(d)                      ((d) - PSRAM_BASE + PSRAMX_BASE)
#define PSRAMX_TO_PSRAM(d)                      ((d) - PSRAMX_BASE + PSRAM_BASE)

#define PSRAM_C_TO_NC(d)                        ((d) - PSRAM_BASE + PSRAM_NC_BASE)
#define PSRAM_NC_TO_C(d)                        ((d) - PSRAM_NC_BASE + PSRAM_BASE)

#ifdef __cplusplus
}
#endif

#endif
