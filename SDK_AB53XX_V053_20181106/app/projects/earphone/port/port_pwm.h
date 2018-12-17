#ifndef __PORT_PWM_H__
#define __PORT_PWM_H__

void pwm_init(void);
void pwm_rgb_write(u8 red, u8 green, u8 blue);
void pwm_rgb_close(void);

#endif //__PORT_PWM_H__
