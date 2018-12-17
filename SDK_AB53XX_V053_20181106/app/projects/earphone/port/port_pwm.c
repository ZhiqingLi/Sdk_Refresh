#include "include.h"

#if PWM_RGB_EN

#define PWM_RGB_PR                  255*64

//LPWM0, LPWM2, LPW3对应R, G, B控制
void pwm_init(void)
{
    GPIOEDE |= (BIT(4) | BIT(6) | BIT(7));
    GPIOEDIR &= ~(BIT(4) | BIT(6) | BIT(7));
    GPIOE &= ~(BIT(4) | BIT(6) | BIT(7));
    FUNCMCON1 = LPWM0MAP_PE4 | LPWM2MAP_PE6 | LPWM3MAP_PE7;

    PWMPRCON = PWM_RGB_PR;
    PWMCON = 0;                     //初始化disable pwm
}

void pwm_rgb_write(u8 red, u8 green, u8 blue)
{
    PWM01DUTY = ((u32)red << 6);
    PWM23DUTY = ((u32)green << 6) | ((u32)blue << (6 + 16));
    if ((PWMCON & 0x0D) != 0x0D) {
        FUNCMCON1 = LPWM0MAP_PE4 | LPWM2MAP_PE6 | LPWM3MAP_PE7;
        PWMCON |= 0x0D;             //enbale LPWM0, LPWM2, LPWM3(R,G,B)
    }
}

void pwm_rgb_close(void)
{
    FUNCMCON1 = 0xff0f0000;
    PWMCON = 0;
}


/*
void pwm_test(void)
{
    printf("%s\n", __func__);
    GPIOADE |= 0x0f;
    GPIOADIR &= ~0x0f;              // PA0 ~ PA3 对应 lowpower PWM0~PWM3 Mapping G3
    GPIOA &= ~0x0f;
    FUNCMCON1 = (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28);

    PWMPRCON = 0x6590;              //PWM period = (PWMPR+1)，分辨率26MHz/4    0x6590->4ms
    PWM01DUTY = 0x32c84c2c;         //DUTY: PWMIVN=0:设置高电平时间； PWMIVN=1:设置低电平时间
    PWM23DUTY = 0x19641964;

    PWMSTEP = 0x68686868;           //PWMSTEP有效位7bit，最高位为符号位 （按STEP加减调节到DUTY或0） 0x68->16us
    PWMCYCNUM = 0x01010101;         //CYCNUM当前STEP的PWM输出次数 = PWMCYCNUM + 1

    PWMCON = 0x1f;
}
*/

#endif // PWM_RGB_EN

