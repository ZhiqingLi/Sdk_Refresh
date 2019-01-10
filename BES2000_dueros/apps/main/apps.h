#ifndef __APPS_H__
#define __APPS_H__

#include "app_status_ind.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

int app_init(void);

int app_deinit(int deinit_case);

int app_shutdown(void);

int app_reset(void);

int app_status_battery_report(uint8_t level);
void app_10_second_timer_check(void);

int app_voice_report( APP_STATUS_INDICATION_T status,uint8_t device_id);

/*FixME*/
void app_status_set_num(const char* p);

////////////10 second tiemr///////////////
#define APP_PAIR_TIMER_ID    0
#define APP_POWEROFF_TIMER_ID 1
void app_stop_10_second_timer(uint8_t timer_id);
void app_start_10_second_timer(uint8_t timer_id);



#define CHIP_ID_C     1
#define CHIP_ID_D     2


////////////////////


#ifdef __cplusplus
}
#endif
#endif//__FMDEC_H__
