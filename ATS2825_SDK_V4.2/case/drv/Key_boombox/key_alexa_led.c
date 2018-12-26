#include "key_inner.h"
#include "key_alexa_led.h"

#if defined(ENABLE_ALEXA_LED_NOTIFY)

typedef struct __led_op_status {
    uint32 led_breathing_timer: 8;
    uint32 led_breathing_value: 8;
    uint32 led_breathing_color_red: 8;
    uint32 led_breathing_color_green: 8;
    uint32 led_breathing_color_blue: 8;
    uint32 led_breathing_climb_up_time: 8;
    uint32 led_breathing_color_red_bak: 8;
    uint32 led_breathing_color_green_bak: 8;
    uint32 led_breathing_color_blue_bak: 8;
    uint32 led_breathing_climb_up_time_bak: 8;
    uint32 led_breathing_up_down_flag: 2;       //0:up, 1:high, 2: down, 3: low
    uint32 led_soft_breathing_running: 1;
    uint32 led_soft_blinking_running : 1;
    uint32 led_breathing_step_value : 6;
    uint32 led_breathing_step_value_bak: 6;
    uint32 led_mix_blinking_timer: 8;
    uint32 led_mix_blinking_color_chg_freq: 8;
    uint32 led_mix_blinking_value: 8;
    uint32 led_mix_blinking_running : 1;
    uint32 led_mix_blinking_color_flag : 1;
    uint32 led_mix_blinking_color_value : 4;
    uint32 led_mix_blinking_up_down_flag : 2;
} led_op_status;

static led_op_status g_led_op_status;

#define PWM_LED_RED             PWM1_CTL
#define PWM_LED_GREEN           PWM3_CTL
#define PWM_LED_BLUE            PWM2_CTL

void __section__(".bank0") key_inner_alexa_led_init(void)
{
    libc_memset(&g_led_op_status, 0, sizeof(g_led_op_status));

    //set the rgb to low in manager_main.c£¬ fix the rgb flash issue
    libc_print("o: ", act_readl(GPIOAOUTEN), 2);
    act_writel(act_readl(MRCR) & (~(1 << MRCR_PWM_LIGHT_Reset)), MRCR);
    act_writel(act_readl(MRCR) | (1 << MRCR_PWM_LIGHT_Reset), MRCR);
    //GPIOA22
    act_writel((1 << CMU_PWM1CLK_PWMCLKDIV_SHIFT), CMU_PWM1CLK);//clock: 80Hz
    //GPIOA8
    act_writel((1 << CMU_PWM3CLK_PWMCLKDIV_SHIFT), CMU_PWM3CLK);//clock: 80Hz
    //GPIOA20
    act_writel((1 << CMU_PWM2CLK_PWMCLKDIV_SHIFT), CMU_PWM2CLK);//clock: 80Hz

    act_writel(255, PWM_LED_RED);
    act_writel(255, PWM_LED_GREEN);
    act_writel(255, PWM_LED_BLUE);

    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_PWMCLKEN)), CMU_DEVCLKEN);         //ENABLE CLOCK

    sys_usleep(100000);
    //map GPIOA22/GPIOA8/GPIOA20 to pwm
    act_writel(((act_readl(MFP_CTL0) & (~(MFP_CTL0_GPIOA22_MASK | MFP_CTL0_GPIOA20_MASK))) |
                (7 << MFP_CTL0_GPIOA22_SHIFT) | (4 << MFP_CTL0_GPIOA20_SHIFT)), MFP_CTL0);
    act_writel(((act_readl(MFP_CTL1) & (~MFP_CTL1_GPIOA8_MASK)) | (5 << MFP_CTL1_GPIOA8_SHIFT)), MFP_CTL1);

    act_writel((act_readl(GPIOAOUTEN) & (~(1 << 22 | 1 << 20 | 1 << 8))), GPIOAOUTEN);
    act_writel((act_readl(GPIOAPDEN) & (~(1 << 22 | 1 << 20 | 1 << 8))), GPIOAPDEN);
}

void led_show_rgb(uint8 red, uint8 green, uint8 blue)
{
    act_writel((1 << CMU_PWM1CLK_PWMCLKDIV_SHIFT), CMU_PWM1CLK);//clock: 80Hz
    act_writel((1 << CMU_PWM3CLK_PWMCLKDIV_SHIFT), CMU_PWM3CLK);//clock: 80Hz
    act_writel((1 << CMU_PWM2CLK_PWMCLKDIV_SHIFT), CMU_PWM2CLK);//clock: 80Hz
    act_writel((255 - red), PWM_LED_RED);
    act_writel((255 - green), PWM_LED_GREEN);
    act_writel((255 - blue), PWM_LED_BLUE);
}

//speed: unit is Hz * 10
void led_show_blinking(uint8 red, uint8 green, uint8 blue, uint8 speed)
{
    act_writel((act_readl(CMU_DEVCLKEN) & ~(1 << CMU_DEVCLKEN_PWMCLKEN)), CMU_DEVCLKEN);         //DISABLE CLOCK
    act_writel(((1600 / speed - 1) << CMU_PWM1CLK_PWMCLKDIV_SHIFT), CMU_PWM1CLK);//clock: 80Hz
    act_writel(((1600 / speed - 1) << CMU_PWM3CLK_PWMCLKDIV_SHIFT), CMU_PWM3CLK);//clock: 80Hz
    act_writel(((1600 / speed - 1) << CMU_PWM2CLK_PWMCLKDIV_SHIFT), CMU_PWM2CLK);//clock: 80Hz

    if(red != 0) {
        act_writel((128 << PWM3_CTL_DUTY_SHIFT), PWM_LED_RED);
    } else {
        act_writel(0xFF, PWM_LED_RED);
    }

    if(green != 0) {
        act_writel((128 << PWM2_CTL_DUTY_SHIFT), PWM_LED_GREEN);
    } else {
        act_writel(0xFF, PWM_LED_GREEN);
    }

    if(blue != 0) {
        act_writel((128 << PWM1_CTL_DUTY_SHIFT), PWM_LED_BLUE);
    } else {
        act_writel(0xFF, PWM_LED_BLUE);
    }
    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_PWMCLKEN)), CMU_DEVCLKEN);         //ENABLE CLOCK
}

//upTIme: unit is ms
//highTime: uint is ms, max is 256 / freq
void led_show_breathing(uint8 red, uint8 green, uint8 blue, uint16 upTime, uint16 highTime, uint16 lowTime)
{
    uint16 freq = 4 * 32 * 1000 / upTime;
    uint8 val =  upTime / 100;
    uint8 low, high;

    act_writel((act_readl(CMU_DEVCLKEN) & ~(1 << CMU_DEVCLKEN_PWMCLKEN)), CMU_DEVCLKEN);         //DISABLE CLOCK
    act_writel(val - 1, CMU_PWM1CLK);//clock: 1280 / 5 = 255Hz
    act_writel(val - 1, CMU_PWM3CLK);//clock: 1280 / 5 = 255Hz
    act_writel(val - 1, CMU_PWM2CLK);//clock: 1280 / 5 = 255Hz

    high = highTime * freq / 1000;
    low = lowTime * freq / 1000;

    if(red != 0) {
        act_writel(7 << 24 | high << 16 | low << 8, PWM_LED_RED);
    } else {
        act_writel(0xFF, PWM_LED_RED);
    }

    if(green != 0) {
        act_writel(7 << 24 | high << 16 | low << 8, PWM_LED_GREEN);
    } else {
        act_writel(0xFF, PWM_LED_GREEN);
    }

    if(blue != 0) {
        act_writel(7 << 24 | high << 16 | low << 8, PWM_LED_BLUE);
    } else {
        act_writel(0xFF, PWM_LED_BLUE);
    }
    act_writel((act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_PWMCLKEN)), CMU_DEVCLKEN);         //ENABLE CLOCK
}

void led_set_default_clk(void)
{
    act_writel(0, CMU_PWM1CLK);//clock: 1280 / 5 = 255Hz
    act_writel(0, CMU_PWM3CLK);//clock: 1280 / 5 = 255Hz
    act_writel(0, CMU_PWM2CLK);//clock: 1280 / 5 = 255Hz
}

void __section__(".text.ram") led_breathing_set_color(uint8 value)
{
    uint16 red = 0, green = 0, blue = 0;

    if(g_led_op_status.led_breathing_color_red != 0) {
        red = g_led_op_status.led_breathing_value * g_led_op_status.led_breathing_color_red / 255;
    }

    if(g_led_op_status.led_breathing_color_green != 0) {
        green = g_led_op_status.led_breathing_value * g_led_op_status.led_breathing_color_green / 255;
    }

    if(g_led_op_status.led_breathing_color_blue != 0) {
        blue = g_led_op_status.led_breathing_value * g_led_op_status.led_breathing_color_blue / 255;
    }

    act_writel((255 - red), PWM_LED_RED);
    act_writel((255 - green), PWM_LED_GREEN);
    act_writel((255 - blue), PWM_LED_BLUE);
}
#define __swap(a, b, mid) (mid = a, a = b, b = mid)
void __section__(".text.ram") key_inner_alexa_led_deal(void)
{
    uint32 val;
    if(g_led_op_status.led_soft_breathing_running) {
        switch(g_led_op_status.led_breathing_up_down_flag) {
            case 0: {
                val = g_led_op_status.led_breathing_climb_up_time + g_led_op_status.led_breathing_value;
                if(val >= 250) {
                    g_led_op_status.led_breathing_up_down_flag = 1;
                    g_led_op_status.led_breathing_timer = 25;        //high time hold 500ms
                    break;
                }

                g_led_op_status.led_breathing_value += g_led_op_status.led_breathing_climb_up_time;
                led_breathing_set_color(g_led_op_status.led_breathing_value);
            }
            break;

            case 1: {
                if(g_led_op_status.led_breathing_timer > 0) {
                    if((--g_led_op_status.led_breathing_timer) == 0) {
                        g_led_op_status.led_breathing_timer = 0;
                        g_led_op_status.led_breathing_up_down_flag = 2;
                        break;
                    }
                }
            }
            break;

            case 2: {
                val = (uint32)g_led_op_status.led_breathing_value - (uint32)g_led_op_status.led_breathing_climb_up_time;
                if(val > 0xFF) {
                    g_led_op_status.led_breathing_up_down_flag = 3;
                    g_led_op_status.led_breathing_timer = 25;        //low time hold 500ms
                    break;
                }

                g_led_op_status.led_breathing_value -= g_led_op_status.led_breathing_climb_up_time;
                led_breathing_set_color(g_led_op_status.led_breathing_value);
            }
            break;

            case 3: {
                if(g_led_op_status.led_breathing_timer > 0) {
                    if((--g_led_op_status.led_breathing_timer) == 0) {
                        g_led_op_status.led_breathing_timer = 0;
                        g_led_op_status.led_breathing_up_down_flag = 0;
                        g_led_op_status.led_breathing_value = 0;

                        if(g_led_op_status.led_breathing_climb_up_time_bak != 0) {
                            __swap(g_led_op_status.led_breathing_climb_up_time, g_led_op_status.led_breathing_climb_up_time_bak, val);
                            __swap(g_led_op_status.led_breathing_color_red, g_led_op_status.led_breathing_color_red_bak, val);
                            __swap(g_led_op_status.led_breathing_color_green, g_led_op_status.led_breathing_color_green_bak, val);
                            __swap(g_led_op_status.led_breathing_color_blue, g_led_op_status.led_breathing_color_blue_bak, val);
                        }
                    }
                }
            }
            break;
        }
    } else if(g_led_op_status.led_soft_blinking_running) {
        switch(g_led_op_status.led_breathing_up_down_flag) {
            case 0: {
                if((g_led_op_status.led_breathing_value++) == 0) {
                    //libc_print("H", 0, 0);
                    act_writel((255 - g_led_op_status.led_breathing_color_red), PWM_LED_RED);
                    act_writel((255 - g_led_op_status.led_breathing_color_green), PWM_LED_GREEN);
                    act_writel((255 - g_led_op_status.led_breathing_color_blue), PWM_LED_BLUE);
                }

                if(g_led_op_status.led_breathing_value >= g_led_op_status.led_breathing_timer) {
                    g_led_op_status.led_breathing_up_down_flag = 1;
                    g_led_op_status.led_breathing_value = 0;        //high time hold 500ms
                    break;
                }
            }
            break;

            case 1: {
                if((g_led_op_status.led_breathing_value++) == 0) {
                    act_writel(255, PWM_LED_RED);
                    act_writel(255, PWM_LED_GREEN);
                    act_writel(255, PWM_LED_BLUE);
                    //libc_print("L", 0, 0);
                }

                if(g_led_op_status.led_breathing_value >= g_led_op_status.led_breathing_timer) {
                    g_led_op_status.led_breathing_up_down_flag = 0;
                    g_led_op_status.led_breathing_value = 0;        //high time hold 500ms
                    break;
                }
            }
            break;
        }
    } else if(g_led_op_status.led_mix_blinking_running) {
        switch(g_led_op_status.led_mix_blinking_color_flag) {
            case 0: {
                switch(g_led_op_status.led_mix_blinking_up_down_flag) {
                    case 0: {
                        if((g_led_op_status.led_mix_blinking_value++) == 0) {
                            act_writel((255 - g_led_op_status.led_breathing_color_red), PWM_LED_RED);
                            act_writel((255 - g_led_op_status.led_breathing_color_green), PWM_LED_GREEN);
                            act_writel((255 - g_led_op_status.led_breathing_color_blue), PWM_LED_BLUE);
                        }

                        if(g_led_op_status.led_mix_blinking_value >= g_led_op_status.led_mix_blinking_timer) {
                            g_led_op_status.led_mix_blinking_up_down_flag = 1;
                            g_led_op_status.led_mix_blinking_value = 0;
                            break;
                        }
                    }
                    break;

                    case 1: {
                        if((g_led_op_status.led_mix_blinking_value++) == 0) {
                            act_writel(255, PWM_LED_RED);
                            act_writel(255, PWM_LED_GREEN);
                            act_writel(255, PWM_LED_BLUE);
                        }

                        if(g_led_op_status.led_mix_blinking_value >= g_led_op_status.led_mix_blinking_timer) {
                            g_led_op_status.led_mix_blinking_up_down_flag = 0;
                            g_led_op_status.led_mix_blinking_value = 0;
                            g_led_op_status.led_mix_blinking_color_value++;
                            if(g_led_op_status.led_mix_blinking_color_value == g_led_op_status.led_mix_blinking_color_chg_freq) {
                                g_led_op_status.led_mix_blinking_color_value = 0;
                                g_led_op_status.led_mix_blinking_color_flag = 1;
                                break;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;

            case 1: {
                switch(g_led_op_status.led_mix_blinking_up_down_flag) {
                    case 0: {
                        if((g_led_op_status.led_mix_blinking_value++) == 0) {
                            act_writel((255 - g_led_op_status.led_breathing_color_red_bak), PWM_LED_RED);
                            act_writel((255 - g_led_op_status.led_breathing_color_green_bak), PWM_LED_GREEN);
                            act_writel((255 - g_led_op_status.led_breathing_color_blue_bak), PWM_LED_BLUE);
                        }

                        if(g_led_op_status.led_mix_blinking_value >= g_led_op_status.led_mix_blinking_timer) {
                            g_led_op_status.led_mix_blinking_up_down_flag = 1;
                            g_led_op_status.led_mix_blinking_value = 0;
                            break;
                        }
                    }
                    break;

                    case 1: {
                        if((g_led_op_status.led_mix_blinking_value++) == 0) {
                            act_writel(255, PWM_LED_RED);
                            act_writel(255, PWM_LED_GREEN);
                            act_writel(255, PWM_LED_BLUE);
                        }

                        if(g_led_op_status.led_mix_blinking_value >= g_led_op_status.led_mix_blinking_timer) {
                            g_led_op_status.led_mix_blinking_up_down_flag = 0;
                            g_led_op_status.led_mix_blinking_value = 0;
                            g_led_op_status.led_mix_blinking_color_value++;
                            if(g_led_op_status.led_mix_blinking_color_value == g_led_op_status.led_mix_blinking_color_chg_freq) {
                                g_led_op_status.led_mix_blinking_color_value = 0;
                                g_led_op_status.led_mix_blinking_color_flag = 0;
                                break;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
}

int32 key_inner_alexa_led_show(uint32 cmd, void *arg0, void *arg1)
{
    uint8 red, green, blue, time;

    g_led_op_status.led_soft_breathing_running = 0;
    g_led_op_status.led_soft_blinking_running = 0;
    g_led_op_status.led_mix_blinking_running = 0;

    switch(cmd) {
        case LED_OP_ALWAYS: {
            red = (((uint32)arg0) >> 16) & 0xFF;
            green = (((uint32)arg0) >> 8) & 0xFF;
            blue = ((uint32)arg0) & 0xFF;
            led_show_rgb(red, green, blue);
        }
        break;

        case LED_OP_BREATHING: {
            time = (((uint32)arg0) >> 24) & 0xFF;
            red = (((uint32)arg0) >> 16) & 0xFF;
            green = (((uint32)arg0) >> 8) & 0xFF;
            blue = ((uint32)arg0) & 0xFF;
            led_show_breathing(red, green, blue, time * 100, 200, 200);
        }
        break;

        case LED_OP_BLINKING: {
            time = (((uint32)arg0) >> 24) & 0xFF;
            red = (((uint32)arg0) >> 16) & 0xFF;
            green = (((uint32)arg0) >> 8) & 0xFF;
            blue = ((uint32)arg0) & 0xFF;
            led_show_blinking(red, green, blue, time);
        }
        break;

        case LED_OP_SOFT_BREATHING: {
            led_show_rgb(0, 0, 0);
            g_led_op_status.led_soft_breathing_running = 1;
            g_led_op_status.led_breathing_up_down_flag = 0;
            g_led_op_status.led_breathing_value = 0;
            g_led_op_status.led_breathing_climb_up_time = 255 * 2 / (((((uint32)arg0) >> 24) & 0xFF) * 10);
            g_led_op_status.led_breathing_color_red = (((uint32)arg0) >> 16) & 0xFF;
            g_led_op_status.led_breathing_color_green = (((uint32)arg0) >> 8) & 0xFF;
            g_led_op_status.led_breathing_color_blue = ((uint32)arg0) & 0xFF;

            if(arg1 != NULL) {
                g_led_op_status.led_breathing_climb_up_time_bak = 255 * 2 / (((((uint32)arg1) >> 24) & 0xFF) * 10);
                g_led_op_status.led_breathing_color_red_bak = (((uint32)arg1) >> 16) & 0xFF;
                g_led_op_status.led_breathing_color_green_bak = (((uint32)arg1) >> 8) & 0xFF;
                g_led_op_status.led_breathing_color_blue_bak = ((uint32)arg1) & 0xFF;
            } else {
                g_led_op_status.led_breathing_climb_up_time_bak = 0;
                g_led_op_status.led_breathing_color_red_bak = 0;
                g_led_op_status.led_breathing_color_green_bak = 0;
                g_led_op_status.led_breathing_color_blue_bak = 0;
            }
        }
        break;

        case LED_OP_SOFT_BLINKING: {
            led_show_rgb(0, 0, 0);
            g_led_op_status.led_soft_blinking_running = 1;
            g_led_op_status.led_breathing_timer = 250 / ((((uint32)arg0) >> 24) & 0xFF);
            g_led_op_status.led_breathing_color_red = (((uint32)arg0) >> 16) & 0xFF;
            g_led_op_status.led_breathing_color_green = (((uint32)arg0) >> 8) & 0xFF;
            g_led_op_status.led_breathing_color_blue = ((uint32)arg0) & 0xFF;
            g_led_op_status.led_breathing_value = 0;
            g_led_op_status.led_breathing_up_down_flag = 0;
        }
        break;

        case LED_OP_MIX_BLINKING: {
            led_show_rgb(0, 0, 0);
            g_led_op_status.led_mix_blinking_running = 1;
            g_led_op_status.led_mix_blinking_up_down_flag = 0;
            g_led_op_status.led_mix_blinking_value = 0;
            g_led_op_status.led_mix_blinking_color_flag = 0;
            g_led_op_status.led_mix_blinking_timer = 250 / ((((uint32)arg0) >> 24) & 0xFF);
            g_led_op_status.led_breathing_color_red = (((uint32)arg0) >> 16) & 0xFF;
            g_led_op_status.led_breathing_color_green = (((uint32)arg0) >> 8) & 0xFF;
            g_led_op_status.led_breathing_color_blue = ((uint32)arg0) & 0xFF;

            if(arg1 != NULL) {
                g_led_op_status.led_mix_blinking_color_chg_freq = ((((uint32)arg1) >> 24) & 0xFF);
                g_led_op_status.led_breathing_color_red_bak = (((uint32)arg1) >> 16) & 0xFF;
                g_led_op_status.led_breathing_color_green_bak = (((uint32)arg1) >> 8) & 0xFF;
                g_led_op_status.led_breathing_color_blue_bak = ((uint32)arg1) & 0xFF;
            } else {
                g_led_op_status.led_mix_blinking_color_chg_freq = 0;
                g_led_op_status.led_breathing_color_red_bak = 0;
                g_led_op_status.led_breathing_color_green_bak = 0;
                g_led_op_status.led_breathing_color_blue_bak = 0;
            }
        }
        break;
    }

    return 0;
}
#endif
