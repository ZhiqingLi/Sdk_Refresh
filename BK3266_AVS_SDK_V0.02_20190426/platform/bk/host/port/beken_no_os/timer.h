
#ifndef _TIMER_H_
#define _TIMER_H_

#include <uw_types.h>

#define PWM1_PT2_COUNT         320     // 32KHz clock 10ms timer

extern void timer_pwm1_pt2_init(void);
extern void RAM_CODE timer_pwm1_pt2_swi(void);
extern uint64_t os_get_tick_counter(void);
void os_tick_delay(uint32 count);
void compute_cpu_speed(void);
void timer_polling_handler(uint32 step);
void timer_clear_watch_dog(void);
int os_timer_is_pending(os_timer_h timer_h);
#ifdef CONFIG_TWS
void pwm1_pt1_isr(void);
void timer_pt1_start(uint32_t val);
#endif
void sniff_enable_pwm1_pt2(void);
void sniff_tick_counter(void);
#if (CONFIG_APP_MP3PLAYER == 1)
void enable_timeout_timer(uint8 flag);
uint8 timeout_handle(void);
void disable_timeout_timer(void);
#endif
#endif // _TIMER_H_

//EOF
