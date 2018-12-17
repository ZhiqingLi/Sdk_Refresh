#ifndef _PORT_SD_H
#define _PORT_SD_H

void sdcard_detect_init(void);
bool sdcard_is_online(void);
bool is_det_sdcard_busy(void);
void sd_gpio_init(u8 type);
#endif // _PORT_SD_H
