#include "hal_overlay.h"
#include "hal_sysfreq.h"
#include "hal_cache.h"
#include "cmsis.h"

extern uint32_t __overlay_text_start__[];
extern uint32_t __overlay_text_exec_start__[];
extern uint32_t __load_start_overlay_text0[];
extern uint32_t __load_stop_overlay_text0[];
extern uint32_t __load_start_overlay_text1[];
extern uint32_t __load_stop_overlay_text1[];
extern uint32_t __load_start_overlay_text2[];
extern uint32_t __load_stop_overlay_text2[];
extern uint32_t __load_start_overlay_text3[];
extern uint32_t __load_stop_overlay_text3[];
extern uint32_t __load_start_overlay_text4[];
extern uint32_t __load_stop_overlay_text4[];
extern uint32_t __load_start_overlay_text5[];
extern uint32_t __load_stop_overlay_text5[];
extern uint32_t __load_start_overlay_text6[];
extern uint32_t __load_stop_overlay_text6[];
extern uint32_t __load_start_overlay_text7[];
extern uint32_t __load_stop_overlay_text7[];

extern uint32_t __overlay_data_start__[];
extern uint32_t __load_start_overlay_data0[];
extern uint32_t __load_stop_overlay_data0[];
extern uint32_t __load_start_overlay_data1[];
extern uint32_t __load_stop_overlay_data1[];
extern uint32_t __load_start_overlay_data2[];
extern uint32_t __load_stop_overlay_data2[];
extern uint32_t __load_start_overlay_data3[];
extern uint32_t __load_stop_overlay_data3[];
extern uint32_t __load_start_overlay_data4[];
extern uint32_t __load_stop_overlay_data4[];
extern uint32_t __load_start_overlay_data5[];
extern uint32_t __load_stop_overlay_data5[];
extern uint32_t __load_start_overlay_data6[];
extern uint32_t __load_stop_overlay_data6[];
extern uint32_t __load_start_overlay_data7[];
extern uint32_t __load_stop_overlay_data7[];

#ifndef NO_OVERLAY
static enum HAL_OVERLAY_ID_T cur_overlay_id = HAL_OVERLAY_ID_QTY;

static uint32_t * const text_load_start[HAL_OVERLAY_ID_QTY] = {
    __load_start_overlay_text0,
    __load_start_overlay_text1,
    __load_start_overlay_text2,
    __load_start_overlay_text3,
    __load_start_overlay_text4,
    __load_start_overlay_text5,
    __load_start_overlay_text6,
    __load_start_overlay_text7,
};

static uint32_t * const text_load_stop[HAL_OVERLAY_ID_QTY] = {
    __load_stop_overlay_text0,
    __load_stop_overlay_text1,
    __load_stop_overlay_text2,
    __load_stop_overlay_text3,
    __load_stop_overlay_text4,
    __load_stop_overlay_text5,
    __load_stop_overlay_text6,
    __load_stop_overlay_text7,
};

static uint32_t * const data_load_start[HAL_OVERLAY_ID_QTY] = {
    __load_start_overlay_data0,
    __load_start_overlay_data1,
    __load_start_overlay_data2,
    __load_start_overlay_data3,
    __load_start_overlay_data4,
    __load_start_overlay_data5,
    __load_start_overlay_data6,
    __load_start_overlay_data7,
};

static uint32_t * const data_load_stop[HAL_OVERLAY_ID_QTY] = {
    __load_stop_overlay_data0,
    __load_stop_overlay_data1,
    __load_stop_overlay_data2,
    __load_stop_overlay_data3,
    __load_stop_overlay_data4,
    __load_stop_overlay_data5,
    __load_stop_overlay_data6,
    __load_stop_overlay_data7,
};
#endif

enum HAL_OVERLAY_RET_T hal_overlay_load(enum HAL_OVERLAY_ID_T id)
{
    enum HAL_OVERLAY_RET_T ret;

    if (id >= HAL_OVERLAY_ID_QTY) {
        return HAL_OVERLAY_RET_BAD_ID;
    }

    ret = HAL_OVERLAY_RET_OK;

#ifndef NO_OVERLAY
    uint32_t lock;
    uint32_t *dst, *src;

    hal_sysfreq_req(HAL_SYSFREQ_USER_OVERLAY, HAL_CMU_FREQ_52M);
    lock = int_lock();

    if (cur_overlay_id == HAL_OVERLAY_ID_QTY) {
        cur_overlay_id = HAL_OVERLAY_ID_IN_CFG;
    } else if (cur_overlay_id == HAL_OVERLAY_ID_IN_CFG) {
        ret = HAL_OVERLAY_RET_IN_CFG;
    } else {
        ret = HAL_OVERLAY_RET_IN_USE;
    }

    if (ret != HAL_OVERLAY_RET_OK) {
        goto _exit;
    }

    for (dst = __overlay_text_start__, src = text_load_start[id];
            src < text_load_stop[id];
            dst++, src++) {
        *dst = *src;
    }

    for (dst = __overlay_data_start__, src = data_load_start[id];
            src < data_load_stop[id];
            dst++, src++) {
        *dst = *src;
    }

    hal_cache_invalidate(HAL_CACHE_ID_I_CACHE,
        (uint32_t)__overlay_text_exec_start__,
        text_load_stop[id] - text_load_start[id]);

    hal_cache_invalidate(HAL_CACHE_ID_D_CACHE,
        (uint32_t)__overlay_data_start__,
        data_load_stop[id] - data_load_start[id]);

    cur_overlay_id = id;

_exit:
    int_unlock(lock);
    hal_sysfreq_req(HAL_SYSFREQ_USER_OVERLAY, HAL_CMU_FREQ_32K);
#endif

    return ret;
}

enum HAL_OVERLAY_RET_T hal_overlay_unload(enum HAL_OVERLAY_ID_T id)
{
    enum HAL_OVERLAY_RET_T ret;

    if (id >= HAL_OVERLAY_ID_QTY) {
        return HAL_OVERLAY_RET_BAD_ID;
    }

    ret = HAL_OVERLAY_RET_OK;

#ifndef NO_OVERLAY
    uint32_t lock;

    lock = int_lock();
    if (cur_overlay_id == id) {
        cur_overlay_id = HAL_OVERLAY_ID_QTY;
    } else if (cur_overlay_id == HAL_OVERLAY_ID_IN_CFG) {
        ret = HAL_OVERLAY_RET_IN_CFG;
    } else {
        ret = HAL_OVERLAY_RET_IN_USE;
    }
    int_unlock(lock);
#endif

    return ret;
}

