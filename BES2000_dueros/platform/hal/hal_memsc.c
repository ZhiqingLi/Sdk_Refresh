#include "plat_addr_map.h"
#include "hal_memsc.h"
#include "hal_cmu.h"

int hal_memsc_lock(enum HAL_MEMSC_ID_T id)
{
    if (id >= HAL_MEMSC_ID_QTY) {
        return 0;
    }

    return (hal_cmu_get_memsc_addr())[id];
}

void hal_memsc_unlock(enum HAL_MEMSC_ID_T id)
{
    if (id >= HAL_MEMSC_ID_QTY) {
        return;
    }

    (hal_cmu_get_memsc_addr())[id] = 1;
}

bool hal_memsc_avail(enum HAL_MEMSC_ID_T id)
{
    if (id >= HAL_MEMSC_ID_QTY) {
        return false;
    }

    return !!((hal_cmu_get_memsc_addr())[4] & (1 << id));
}

