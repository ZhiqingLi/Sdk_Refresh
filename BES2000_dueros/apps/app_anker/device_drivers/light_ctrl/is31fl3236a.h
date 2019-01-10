#ifndef __BES_LIGHT_CTRL_IS31FL3236A_H__
#define __BES_LIGHT_CTRL_IS31FL3236A_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

typedef enum{
    IS31FL3236A_CMD_CHIP_INIT = 0,
    IS31FL3236A_CMD_OUT_CTRL_ALL,
    IS31FL3236A_CMD_OUT_CTRL,
    IS31FL3236A_CMD_SET_RGB,
    IS31FL3236A_CMD_SET_RGB_ALL,
    IS31FL3236A_CMD_SET_START_EFFECT_EN,
    IS31FL3236A_CMD_GET_START_EFFECT_EN,
    IS31FL3236A_CMD_SET_WORK_MODE,
    IS31FL3236A_CMD_CHIP_DEINIT,
    IS31FL3236A_CMD_MAX,
}IS31FL3236A_CMD_T;

#define RGB_DATA_NUM        (12)
struct rgb {
    uint8_t b_val;
    uint8_t g_val;
    uint8_t r_val;
};

struct light_data {
    uint8_t index;
    struct rgb data;
};

struct all_light_data {
    struct rgb data[RGB_DATA_NUM];
};

int32_t is31fl3236a_dev_ctrl(uint8_t cmd, void* arg);
#ifdef __cplusplus
}
#endif
#endif