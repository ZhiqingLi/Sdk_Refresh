#ifndef __PORT_PWM_H__
#define __PORT_PWM_H__

void pwm_init(void);
void pwm_rgb_write(u8 red, u8 green, u8 blue);
void pwm_rgb_close(void);

void energy_led_init(void);
void energy_led_level_calc(void);
void energy_led_scan(void);

#if PWM_TIMER_EN
void timer_pwm_enable(uint8_t PwmChannel, uint8_t MapSel);
void timer_pwm_disable(uint8_t PwmChannel, uint8_t MapSel);
void timer_pwm_config(uint8_t PwmChannel, uint16_t Freq, uint16_t Duty, bool OutLevel);
#endif // PWM_TIMER_EN

#endif //__PORT_PWM_H__
