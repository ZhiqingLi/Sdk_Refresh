#ifndef __APP_OVERLAY_H__
#define __APP_OVERLAY_H__

#include "hal_overlay.h"

#define app_overlay_load(id) hal_overlay_load((enum HAL_OVERLAY_ID_T)id)
#define app_overlay_unload(id) hal_overlay_unload((enum HAL_OVERLAY_ID_T)id)

enum APP_OVERLAY_ID_T {
    APP_OVERLAY_HFP = HAL_OVERLAY_ID_0,
    APP_OVERLAY_A2DP = HAL_OVERLAY_ID_1,
    APP_OVERLAY_FM = HAL_OVERLAY_ID_2,
    APP_OVERLAY_AAC = HAL_OVERLAY_ID_3,
    APP_OVERLAY_A2DP_AAC = HAL_OVERLAY_ID_4,
    APP_OVERLAY_MPA = HAL_OVERLAY_ID_5,
#ifdef	A2DP_SCALABLE_ON
    APP_OVERLAY_A2DP_SCALABLE= HAL_OVERLAY_ID_6,
#else
    APP_OVERLAY_WAV = HAL_OVERLAY_ID_6,
#endif
    APP_OVERLAY_FLAC = HAL_OVERLAY_ID_7,

    APP_OVERLAY_ID_QTY = HAL_OVERLAY_ID_QTY,
    APP_OVERLAY_ID_IN_CFG = HAL_OVERLAY_ID_IN_CFG,
};

void app_overlay_select(enum APP_OVERLAY_ID_T id);

void app_overlay_unloadall(void);

void app_overlay_open(void);

void app_overlay_close(void);

APP_OVERLAY_ID_T app_get_current_overlay(void);

#endif
