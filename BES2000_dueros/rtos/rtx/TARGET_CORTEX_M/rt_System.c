/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_SYSTEM.C
 *      Purpose: System Task Manager
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Conf.h"
#include "rt_Task.h"
#include "rt_System.h"
#include "rt_Event.h"
#include "rt_List.h"
#include "rt_Mailbox.h"
#include "rt_Semaphore.h"
#include "rt_Time.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"

#include "hal_sleep.h"
#include "hal_timer.h"
#include "hal_trace.h"

extern void sysTimerTick(void);
extern uint32_t rt_timer_delay_count(void);

#if __RTX_CPU_STATISTICS__
U32 _os_stat_count = 0;
U32 os_stat_count = 0;
#if defined(_CPU_STATISTICS_PEROID_)
U32 os_stat_period = _CPU_STATISTICS_PEROID_;
#else
U32 os_stat_period = 0;
#endif
extern U32 os_peroid_start_time_tick;
#endif

U32 rt_get_now_tick(void);
/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

int os_tick_irqn;

/*----------------------------------------------------------------------------
 *      Local Variables
 *---------------------------------------------------------------------------*/

static volatile BIT os_lock;
static volatile BIT os_psh_flag;
static          U8  pend_flags;
static          U8  suspend_tick_wrap;
static          U32 systick_lock_tick;
static          U32 systick_lock_time;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

#if defined (__CC_ARM)
__asm void $$RTX$$version (void) {
   /* Export a version number symbol for a version control. */

                EXPORT  __RL_RTX_VER

__RL_RTX_VER    EQU     0x450
}
#endif

/*--------------------------- rt_suspend ------------------------------------*/
U32 rt_suspend (void) {
  /* Suspend OS scheduler */
  U32 delta = 0xFFFF;
  U32 start_tick;
  const U32 reload = (NVIC_ST_RELOAD + 1);

  suspend_tick_wrap = 0;

  __disable_irq();
  start_tick = NVIC_ST_CURRENT;
  rt_tsk_lock();
  if ((start_tick != 0 && start_tick < reload / 2) &&
        (systick_lock_tick == 0 || systick_lock_tick >= reload / 2)) {
#ifdef DEBUG_SLEEP
      TRACE("[%u / 0x%x / %2u / %u] rt_suspend corner case: %02u -> %02u",
        TICKS_TO_MS(hal_sys_timer_get()), hal_sys_timer_get(), NVIC_ST_CURRENT, os_time,
        start_tick, systick_lock_tick);
#endif
      pend_flags |= 1;
      suspend_tick_wrap = 1;
  }
  if (pend_flags) {
    delta = 0;
  }
  __enable_irq();

  if (delta == 0) {
    return 0;
  }

  if (os_dly.p_dlnk) {
    delta = os_dly.delta_time;
  }
#ifdef __CMSIS_RTOS
  {
    uint32_t tcnt = rt_timer_delay_count();
    if (tcnt && tcnt < delta) {
      delta = tcnt;
    }
  }
#else
  if (os_tmr.next) {
    if (os_tmr.tcnt < delta) delta = os_tmr.tcnt;
  }
#endif

  return (delta);
}

/*--------------------------- rt_resume -------------------------------------*/
void rt_resume (U32 sleep_time) {
  /* Resume OS scheduler after suspend */
  //P_TCB next;
  U32   delta;
  U32   systick_read;
  U32   systick_cal;
  U32   time;
  U32   sleep_ticks;
  const U32 reload = (NVIC_ST_RELOAD + 1);

  __disable_irq();
  systick_read = NVIC_ST_CURRENT;
  time = hal_sys_timer_get();
  __enable_irq();

  if (suspend_tick_wrap) {
    goto _task_unlock;
  }

  sleep_ticks = (uint64_t)(time - systick_lock_time) * OS_CLOCK / CONFIG_SYSTICK_HZ;
  systick_cal = (reload + systick_lock_tick - (sleep_ticks % reload)) % reload;
  if (systick_cal > systick_read) {
    if (systick_cal - systick_read > reload / 2) {
      delta = reload + systick_read - systick_cal;
      if (sleep_ticks > delta) {
        sleep_ticks -= delta;
      } else {
        sleep_ticks = 0;
      }
    } else {
      sleep_ticks += systick_cal - systick_read;
    }
  } else if (systick_cal < systick_read) {
    if (systick_read - systick_cal > reload / 2) {
      sleep_ticks += (reload + systick_cal - systick_read);
    } else {
      delta = systick_read - systick_cal;
      if (sleep_ticks > delta) {
        sleep_ticks -= delta;
      } else {
        sleep_ticks = 0;
      }
    }
  }
  if (systick_lock_tick == 0) {
    sleep_time = sleep_ticks / reload;
  } else if (sleep_ticks >= systick_lock_tick) {
    sleep_time = (sleep_ticks - systick_lock_tick) / reload + 1;
  } else {
    sleep_time = 0;
  }

  // Task switching is allowed in SVC/PENDSV/SYSTICK handlers only
#if 0
  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);
#endif

_inc_sleep_time:

  os_robin.task = NULL;

  /* Update delays. */
  if (os_dly.p_dlnk) {
    delta = sleep_time;
    if (delta >= os_dly.delta_time) {
      delta   -= os_dly.delta_time;
      os_time += os_dly.delta_time;
      os_dly.delta_time = 1;
      while (os_dly.p_dlnk) {
        rt_dec_dly();
        if (delta == 0) break;
        delta--;
        os_time++;
      }
      rt_psh_req();
    } else {
      os_time           += delta;
      os_dly.delta_time -= delta;
    }
  } else {
    os_time += sleep_time;
  }

#ifdef __CMSIS_RTOS
  delta = sleep_time;
  while (delta && rt_timer_delay_count()) {
    sysTimerTick();
    delta--;
  }
#else
  /* Check the user timers. */
  if (os_tmr.next) {
    delta = sleep_time;
    if (delta >= os_tmr.tcnt) {
      delta   -= os_tmr.tcnt;
      os_tmr.tcnt = 1;
      while (os_tmr.next) {
        rt_tmr_tick();
        if (delta == 0) break;
        delta--;
      }
    } else {
      os_tmr.tcnt -= delta;
    }
  }
#endif

  // Task switching is allowed in SVC/PENDSV/SYSTICK handlers only
#if 0
  /* Switch back to highest ready task */
  next = rt_get_first (&os_rdy);
  rt_switch_req (next);
#endif

#ifdef DEBUG_SLEEP
  if (sleep_time > 0) {
    TRACE("[%u / 0x%x / %2u / %u] rt_resume: run=%p, rdy=%p",
      TICKS_TO_MS(hal_sys_timer_get()), hal_sys_timer_get(), NVIC_ST_CURRENT, os_time,
      os_tsk.run->ptask, os_rdy.p_lnk ? os_rdy.p_lnk->ptask : NULL);
  }
#endif

_task_unlock:

  sleep_time = 0;

  __disable_irq();
  rt_tsk_unlock();
  // Handle corner case: systick counter wraps inside rt_resume() but just before rt_tsk_unlock()
  // Assumption: rt_resume can finish in 1 systick.
  systick_cal = NVIC_ST_CURRENT;
  if ((systick_read != 0 && systick_read < reload / 2) &&
        (systick_cal == 0 || systick_cal >= reload / 2) &&
        (OS_PENDING & 1) == 0) {
#ifdef DEBUG_SLEEP
    TRACE("[%u / 0x%x / %2u / %u] rt_resume corner case: %02u -> %02u -> %02u\n\t\t@ %u / 0x%x",
      TICKS_TO_MS(hal_sys_timer_get()), hal_sys_timer_get(), NVIC_ST_CURRENT, os_time,
      systick_lock_tick, systick_read, systick_cal, TICKS_TO_MS(time), time);
#endif
    rt_tsk_lock();
    systick_read = systick_cal;
    sleep_time = 1;
  }
  __enable_irq();

  if (sleep_time) {
    goto _inc_sleep_time;
  }
}


/*--------------------------- rt_tsk_lock -----------------------------------*/

void rt_tsk_lock (void) {
  /* Prevent task switching by locking out scheduler */
  if (os_tick_irqn < 0) {
    OS_LOCK();
    os_lock = __TRUE;
    OS_UNPEND (&pend_flags);
    systick_lock_tick = NVIC_ST_CURRENT;
    systick_lock_time = hal_sys_timer_get();
  } else {
    OS_X_LOCK(os_tick_irqn);
    os_lock = __TRUE;
    OS_X_UNPEND (&pend_flags);
  }
}


/*--------------------------- rt_tsk_unlock ---------------------------------*/

void rt_tsk_unlock (void) {
  /* Unlock scheduler and re-enable task switching */
  if (os_tick_irqn < 0) {
    OS_UNLOCK();
    os_lock = __FALSE;
    OS_PEND (pend_flags, os_psh_flag);
    os_psh_flag = __FALSE;
  } else {
    OS_X_UNLOCK(os_tick_irqn);
    os_lock = __FALSE;
    OS_X_PEND (pend_flags, os_psh_flag);
    os_psh_flag = __FALSE;
  }
  // Allow cpu sleep
  hal_cpu_wake_unlock(HAL_CPU_WAKE_LOCK_USER_RTOS);
}


/*--------------------------- rt_psh_req ------------------------------------*/

void rt_psh_req (void) {
  /* Initiate a post service handling request if required. */
  if (os_lock == __FALSE) {
    OS_PEND_IRQ ();
  }
  else {
    os_psh_flag = __TRUE;
    // Prohibit cpu sleep when an OS service request is enqueued during os lock (rt_suspend)
    hal_cpu_wake_lock(HAL_CPU_WAKE_LOCK_USER_RTOS);
  }
}


/*--------------------------- rt_pop_req ------------------------------------*/

void rt_pop_req (void) {
  /* Process an ISR post service requests. */
  struct OS_XCB *p_CB;
  P_TCB next;
  U32  idx;

  os_tsk.run->state = READY;
  if (os_tsk.run == &os_idle_TCB) {
    rt_put_rdy_last (os_tsk.run);
  } else {
    rt_put_rdy_first (os_tsk.run);
  }

  idx = os_psq->last;
  while (os_psq->count) {
    p_CB = os_psq->q[idx].id;
    if (p_CB->cb_type == TCB) {
      /* Is of TCB type */
      rt_evt_psh ((P_TCB)p_CB, (U16)os_psq->q[idx].arg);
    }
    else if (p_CB->cb_type == MCB) {
      /* Is of MCB type */
      rt_mbx_psh ((P_MCB)p_CB, (void *)os_psq->q[idx].arg);
    }
    else {
      /* Must be of SCB type */
      rt_sem_psh ((P_SCB)p_CB);
    }
    if (++idx == os_psq->size) idx = 0;
    rt_dec (&os_psq->count);
  }
  os_psq->last = idx;

  next = rt_get_first (&os_rdy);
  rt_switch_req (next);
}


/*--------------------------- os_tick_init ----------------------------------*/

__weak int os_tick_init (void) {
  /* Initialize SysTick timer as system tick timer. */
  rt_systick_init ();
  return (-1);  /* Return IRQ number of SysTick timer */
}


/*--------------------------- os_tick_irqack --------------------------------*/

__weak void os_tick_irqack (void) {
  /* Acknowledge timer interrupt. */
}


/*--------------------------- rt_systick ------------------------------------*/
u32 _cnt = 0;
void rt_systick (void) {
  /* Check for system clock update, suspend running task. */
  P_TCB next;

#if __RTX_CPU_STATISTICS__
  if (os_stat_period == 0)
    os_stat_period = 3000;
  _os_stat_count = rt_get_now_tick()/MS_TO_TICKS(os_stat_period);
  if (_os_stat_count > os_stat_count) {
      os_stat_count = _os_stat_count;
      os_peroid_start_time_tick = rt_get_now_tick();
      for (_cnt = 0; _cnt < os_maxtaskrun; _cnt++) {
          P_TCB t = os_active_TCB[_cnt];
          if (t) {
              t->period_tick = 0;
          }
      }
      os_idle_TCB.period_tick = 0;
  }
#endif

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  /* Check Round Robin timeout. */
  rt_chk_robin ();

  /* Update delays. */
  os_time++;
  rt_dec_dly ();

  /* Check the user timers. */
#ifdef __CMSIS_RTOS
  sysTimerTick();
#else
  rt_tmr_tick ();
#endif

  /* Switch back to highest ready task */
  next = rt_get_first (&os_rdy);
  rt_switch_req (next);
}

/*--------------------------- rt_stk_check ----------------------------------*/
__weak void rt_stk_check (void) {
    /* Check for stack overflow. */
    if (os_tsk.run->task_id == 0x01) {
        // TODO: For the main thread the check should be done against the main heap pointer
    } else {
        if ((os_tsk.run->tsk_stack < (U32)os_tsk.run->stack) ||
            (os_tsk.run->stack[0] != MAGIC_WORD)) {
            os_error (OS_ERR_STK_OVF);
        }
    }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

