#ifndef _BSP_CHARGE_H
#define _BSP_CHARGE_H

void charge_init(void);
void charge_off(void);
void charge_detect(u8 mode);
void charge_disp_sta(u8 sta);
u8 charge_dc_in(void);
void charge_set_stop_time(u16 stop_time);

#endif
