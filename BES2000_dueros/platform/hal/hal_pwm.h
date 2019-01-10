#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "hal_cmu.h"

struct HAL_PWM_CFG_T {
    uint32_t freq;
    uint8_t ratio;
    bool inv;
    bool sleep_on;
};

int hal_pwm_enable(enum HAL_PWM_ID_T id, const struct HAL_PWM_CFG_T *cfg);

int hal_pwm_disable(enum HAL_PWM_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

