#ifndef _PORT_LED_H
#define _PORT_LED_H

typedef struct {
    void (*port_init)(gpio_t *g);
    void (*set_on)(gpio_t *g);
    void (*set_off)(gpio_t *g);
} led_func_t;

extern led_func_t bled_func;
extern led_func_t rled_func;
extern gpio_t bled_gpio;
extern gpio_t rled_gpio;

void led_func_init(void);

void bled_fft_dac_pow(void);
void bled_fft_scan(void);

#endif // _PORT_LED_H
