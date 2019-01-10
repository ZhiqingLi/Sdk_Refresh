#ifndef _PORT_LINEIN_H
#define _PORT_LINEIN_H

bool is_linein_enter_enable(void);
void linein_detect_init(void);
bool linein_is_online(void);
bool is_detect_linein_busy(void);
bool is_sleep_dac_off_enable(void);
bool is_linein_det_mux_micl(void);
#endif // _PORT_LINEIN_H
