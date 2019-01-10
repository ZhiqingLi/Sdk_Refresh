#ifndef __APP_UTILS_H__
#define __APP_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sysfreq.h"

enum APP_SYSFREQ_USER_T {
    APP_SYSFREQ_USER_APP_0 = HAL_SYSFREQ_USER_APP_0,
    APP_SYSFREQ_USER_APP_1 = HAL_SYSFREQ_USER_APP_1,
    APP_SYSFREQ_USER_APP_2 = HAL_SYSFREQ_USER_APP_2,
    APP_SYSFREQ_USER_APP_3 = HAL_SYSFREQ_USER_APP_3,
    APP_SYSFREQ_USER_APP_4 = HAL_SYSFREQ_USER_APP_4,
    APP_SYSFREQ_USER_APP_5 = HAL_SYSFREQ_USER_APP_5,

    APP_SYSFREQ_USER_QTY
};

enum APP_SYSFREQ_FREQ_T {
    APP_SYSFREQ_32K =  HAL_CMU_FREQ_32K,
    APP_SYSFREQ_26M =  HAL_CMU_FREQ_26M,
    APP_SYSFREQ_52M =  HAL_CMU_FREQ_52M,
    APP_SYSFREQ_78M =  HAL_CMU_FREQ_78M,
    APP_SYSFREQ_104M = HAL_CMU_FREQ_104M,
    APP_SYSFREQ_208M = HAL_CMU_FREQ_208M,

    APP_SYSFREQ_FREQ_QTY =  HAL_CMU_FREQ_QTY
};

int app_sysfreq_req(enum APP_SYSFREQ_USER_T user, enum APP_SYSFREQ_FREQ_T freq);

void app_wdt_ping(void);

int app_wdt_open(int seconds);

int app_wdt_close(void);

#ifdef __cplusplus
}
#endif

#endif//__FMDEC_H__
