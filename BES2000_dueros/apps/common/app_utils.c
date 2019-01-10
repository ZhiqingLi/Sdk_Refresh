#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "hal_wdt.h"
#include "hal_sleep.h"
#include "app_thread.h"
#include "app_utils.h"

static uint32_t app_wdt_ping_time = 0;

int app_sysfreq_req(enum APP_SYSFREQ_USER_T user, enum APP_SYSFREQ_FREQ_T freq)
{
    int ret;

#ifdef ULTRA_LOW_POWER
    uint32_t lock = int_lock();

    enum HAL_CMU_FREQ_T old_sysfreq = hal_sysfreq_get();

    // Enable PLL if required
    if (old_sysfreq <= HAL_CMU_FREQ_52M && freq > APP_SYSFREQ_52M) {
        hal_cmu_low_freq_mode_disable();
    }
#endif

    ret = hal_sysfreq_req((enum HAL_SYSFREQ_USER_T)user, (enum HAL_CMU_FREQ_T)freq);

#ifdef ULTRA_LOW_POWER
    // Disable PLL if capable
    if (old_sysfreq > HAL_CMU_FREQ_52M && hal_sysfreq_get() <= HAL_CMU_FREQ_52M) {
        hal_cmu_low_freq_mode_enable();
    }

    int_unlock(lock);
#endif

    return ret;
}

static void app_wdt_kick(void)
{
    hal_wdt_ping(HAL_WDT_ID_0);
}

static int app_wdt_handle_process(APP_MESSAGE_BODY *msg_body)
{
    app_wdt_kick();

    return 0;
}

static void app_wdt_irq_handle(enum HAL_WDT_ID_T id, uint32_t status)
{
    TRACE("%s id:%d status:%d",__func__, id, status);
}

void app_wdt_ping(void)
{
    APP_MESSAGE_BLOCK msg;
    uint32_t time = hal_sys_timer_get();
    if  ((time - app_wdt_ping_time)>MS_TO_TICKS(3000)){
        app_wdt_ping_time = time;
        msg.mod_id = APP_MODUAL_WATCHDOG;
        app_mailbox_put(&msg);
    }
}

int app_wdt_open(int seconds)
{
    hal_wdt_set_irq_callback(HAL_WDT_ID_0, app_wdt_irq_handle);
    hal_wdt_set_timeout(HAL_WDT_ID_0, seconds);
    hal_wdt_start(HAL_WDT_ID_0);
    hal_sleep_set_lowpower_hook(HAL_SLEEP_HOOK_USER_0, app_wdt_kick);
    app_set_threadhandle(APP_MODUAL_WATCHDOG, app_wdt_handle_process);

    return 0;
}

int app_wdt_close(void)
{
    hal_wdt_stop(HAL_WDT_ID_0);
    return 0;
}

