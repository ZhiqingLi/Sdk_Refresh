#ifndef __HAL_CHIPID_H__
#define __HAL_CHIPID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

// BEST1000: A=0 C=1 D=2 F=3 G=4 I=5
// BEST2000: A=0 B=1 D=2

enum HAL_CHIP_METAL_ID_T {
    HAL_CHIP_METAL_ID_0,
    HAL_CHIP_METAL_ID_1,
    HAL_CHIP_METAL_ID_2,
    HAL_CHIP_METAL_ID_3,
    HAL_CHIP_METAL_ID_4,
    HAL_CHIP_METAL_ID_5,
    HAL_CHIP_METAL_ID_6,
    HAL_CHIP_METAL_ID_7,
    HAL_CHIP_METAL_ID_8,
    HAL_CHIP_METAL_ID_9,
    HAL_CHIP_METAL_ID_10,
    HAL_CHIP_METAL_ID_11,
    HAL_CHIP_METAL_ID_12,
    HAL_CHIP_METAL_ID_13,
    HAL_CHIP_METAL_ID_14,
    HAL_CHIP_METAL_ID_15,
};

// Invoked by hal_chipid_init() only
uint32_t read_hw_metal_id(void);

void hal_chipid_init(void);

enum HAL_CHIP_METAL_ID_T hal_get_chip_metal_id(void);

#ifdef __cplusplus
}
#endif

#endif
