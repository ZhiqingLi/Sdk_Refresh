#include "hal_location.h"
#include "cmsis.h"
#include "task_schedule.h"

#ifdef SIMPLE_TASK_SWITCH

extern void task_run(uint32_t task_id);

#ifndef MAX_TASK_NUM
#define MAX_TASK_NUM                        2
#endif

uint32_t task_sp[MAX_TASK_NUM];

static enum TASK_STATE_T task_state[MAX_TASK_NUM] = { TASK_STATE_NONE, };

// CAUTION: Flash driver will access the variable during system initialization
uint32_t BOOT_DATA_LOC cur_task_id = MAX_TASK_NUM;

int task_setup(uint8_t id, uint32_t sp, enum TASK_STATE_T state, TASK_ENTRY_T entry)
{
    if (id >= MAX_TASK_NUM) {
        return 1;
    }
    if (sp & 0x7) {
        return 2;
    }
    if (task_state[id] != TASK_STATE_NONE) {
        return 3;
    }

    if (state == TASK_STATE_ACTIVE) {
        if (cur_task_id != MAX_TASK_NUM) {
            return 4;
        }
        cur_task_id = id;
    }

    if (sp) {
        // r4-r12, lr
        task_sp[id] = sp - (10 * 4);
        // lr is located at the stack top
        *(uint32_t *)(sp - 4) = (uint32_t)entry;
    } else {
        task_sp[id] = 0;
    }

    task_state[id] = state;

    return 0;
}

static bool task_schedule(enum TASK_STATE_T state)
{
    uint32_t next_task_id;
    uint32_t lock;

    next_task_id = cur_task_id;

    lock = int_lock();

    task_state[cur_task_id] = state;

    do {
        if (++next_task_id >= MAX_TASK_NUM) {
            next_task_id = 0;
        }
    } while (task_state[next_task_id] != TASK_STATE_PENDING);

    task_state[next_task_id] = TASK_STATE_ACTIVE;

    int_unlock(lock);

    if (next_task_id != cur_task_id) {
        task_run(next_task_id);
    }

    return true;
}

// CAUTION: Flash driver will access the function during system initialization
bool BOOT_TEXT_SRAM_LOC task_yield(void)
{
    //ASSERT(cur_task_id == MAX_TASK_NUM || task_state[cur_task_id] == TASK_STATE_ACTIVE,
    //    "task_yield: Invalid task state: task_state[%u]=%d", cur_task_id, task_state[cur_task_id]);

    if (cur_task_id == MAX_TASK_NUM) {
        return true;
    }

    return task_schedule(TASK_STATE_PENDING);
}

bool task_sleep(void)
{
    //ASSERT(cur_task_id == MAX_TASK_NUM || task_state[cur_task_id] == TASK_STATE_ACTIVE,
    //    "task_sleep: Invalid task state: task_state[%u]=%d", cur_task_id, task_state[cur_task_id]);

    if (cur_task_id == MAX_TASK_NUM) {
        return true;
    }

    return task_schedule(TASK_STATE_SLEEPING);
}

bool task_wakeup(uint8_t id)
{
    bool ret;
    uint32_t lock;

    if (id >= MAX_TASK_NUM) {
        return false;
    }

    lock = int_lock();
    if (task_state[id] == TASK_STATE_SLEEPING) {
        task_state[id] = TASK_STATE_PENDING;
        ret = true;
    } else {
        ret = false;
    }
    int_unlock(lock);

    return ret;
}

#endif

