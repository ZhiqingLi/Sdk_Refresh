#ifndef _BSP_CHARGE_H
#define _BSP_CHARGE_H

void charge_init(void);
void charge_off(void);
void charge_detect(void);
void charge_disp_sta(u8 sta);
u8 charge_dc_detect(void);
void charge_set_stop_time(u16 stop_time);
void internal_charge_disable(void);  //关闭内部充电,调用它关闭芯片内部充电.
#endif
