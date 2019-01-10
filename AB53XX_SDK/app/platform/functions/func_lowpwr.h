#ifndef __FUNC_LOWPWR_H
#define __FUNC_LOWPWR_H

typedef bool (*is_sleep_func)(void);

//
#define en_auto_pwroff()        sys_cb.pwroff_delay = sys_cb.pwroff_time;
#define dis_auto_pwroff()       sys_cb.pwroff_delay = -1L;
#define reset_pwroff_delay()    if(sys_cb.pwroff_delay != -1L) {sys_cb.pwroff_delay = sys_cb.pwroff_time;}
#define reset_sleep_delay()     sys_cb.sleep_delay = sys_cb.sleep_time;
bool sleep_process(is_sleep_func is_sleep);
void lowpwr_tout_ticks(void);

void sfunc_pwrdown(void);       //软开关方案，POWER键/低电时，进入关机状态
void func_pwroff(int pwroff_tone_en);         //进入低电或者关机状态
void func_sleepmode(void);      //sleepmode假关机，带红外唤醒
u8 charge_finish_into_lowpower_check(bool charge_stop);  //充电完成,进入低功耗
void sys_clk_restore(u32 *buf);
void sys_clk_seclet_RC(u32 *buf);
#endif // __FUNC_LOWPWR_H
