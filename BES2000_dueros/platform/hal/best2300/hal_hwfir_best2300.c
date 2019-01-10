#include "cmsis.h"
#include "plat_addr_map.h"
#include "reg_hwfir_best2300.h"
#include "hal_hwfir.h"
#include "hal_codec.h"
#include "hal_cmu.h"
#include "hal_aud.h"
#include "hal_trace.h"
#include "analog.h"
#include "tgt_hardware.h"

static const char * const err_invalid_id = "Invalid HWFIR id: %d";

struct HWFIR_T *HWFIR = (struct HWFIR_T *)FFT_BASE;

static char chan_enabled[HWFIR_CHANNEL_ID_QTY];

static const uint16_t samp_coef_address_map[HWFIR_CHANNEL_ID_QTY*2] = {
    // samp, coef
    0x8000, 0x9000, // ch0
    0x8800, 0x9800, // ch1
    0xa000, 0xb000, // ch2
    0xa800, 0xb800, // ch3
    0xc000, 0xd000, // ch4
    0xc800, 0xd800, // ch5
    0xe000, 0xf000, // ch6
    0xe800, 0xf800, // ch7
};

HAL_HWFIR_CHANNEL_ID hal_hwfir_get_id(void)
{
    int i = 0;
    int lock  =0;
    lock = int_lock();
    for (i = 0; i < HWFIR_CHANNEL_ID_QTY; ++i) {
        if (chan_enabled[i] == 0) {
            chan_enabled[i] = 1;
            int_unlock(lock);
            return i;
        }
    }

    int_unlock(lock);
    return -1;
}
int hal_hwfir_acquire_id(HAL_HWFIR_CHANNEL_ID id)
{
    int lock  =0;
    ASSERT(id >= 0 && id < HWFIR_CHANNEL_ID_QTY, err_invalid_id, id);

    lock = int_lock();
    if (chan_enabled[id] == 0) {
        chan_enabled[id] = 1;
        int_unlock(lock);
        return id;
    }

    int_unlock(lock);
    return -1;
}
unsigned int hal_hwfir_get_data_memory_addr(HAL_HWFIR_CHANNEL_ID id)
{
    ASSERT(id >= 0 && id < HWFIR_CHANNEL_ID_QTY, err_invalid_id, id);

    //return (HWFIR_CH0_SAMP_MEMORY_BASE + ((id/2)*0x1000) + (0x800*(id%2)));
    return (FFT_BASE+samp_coef_address_map[id*2+0]);
}
unsigned int hal_hwfir_get_coef_memory_addr(HAL_HWFIR_CHANNEL_ID id)
{
    ASSERT(id >= 0 && id < HWFIR_CHANNEL_ID_QTY, err_invalid_id, id);
    //return (HWFIR_CH0_COEF_MEMORY_BASE + ((id/2)*0x1000) + (0x800*(id%2)));
    return (FFT_BASE+samp_coef_address_map[id*2+1]);
}
int hal_hwfir_free_id(HAL_HWFIR_CHANNEL_ID id)
{
    int lock  =0;
    ASSERT(id >= 0 && id < HWFIR_CHANNEL_ID_QTY, err_invalid_id, id);

    lock = int_lock();
    chan_enabled[id] = 0;
    int_unlock(lock);

    return 0;
}
