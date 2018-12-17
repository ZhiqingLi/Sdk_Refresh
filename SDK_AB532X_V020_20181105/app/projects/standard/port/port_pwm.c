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


#if ENERGY_LED_EN    //能量灯软件PWM输出.

#define  ENERGY_LED_NUM 4         //灯的个数
#define  ENERGY_PWM_MAX_DUTY  8   //一个灯分8个亮度.

#define  ENERGY_LED_INIT()     {GPIOBDE |= 0x0F; GPIOBDIR &= ~0x0F;}
#define  ENERGY_LED_0_ON()     GPIOBSET = BIT(0)
#define  ENERGY_LED_0_OFF()    GPIOBCLR = BIT(0)
#define  ENERGY_LED_1_ON()     GPIOBSET = BIT(1)
#define  ENERGY_LED_1_OFF()    GPIOBCLR = BIT(2)
#define  ENERGY_LED_2_ON()     GPIOBSET = BIT(2)
#define  ENERGY_LED_2_OFF()    GPIOBCLR = BIT(2)
#define  ENERGY_LED_3_ON()     GPIOASET = BIT(3)
#define  ENERGY_LED_3_OFF()    GPIOACLR = BIT(3)

u8 pwm_duty_buf[ENERGY_LED_NUM];

AT(.com_text.rgb.tbl)   //非线性量化表.
u8 const energy_qtz_tbl[4 * 8] = {
     1,14,18,22,26,30,35,40,
     41,43,46,48,51,53,56,60,
     61,63,66,68,71,73,76,80,
     81,83,85,88,91,93,96,100,
};

void energy_led_init(void)
{
    ENERGY_LED_INIT();
}

AT(.com_text.rgb)
void energy_led_level_calc(void)    //约5ms调用一次.
{
    u8 level,i;
    u16 energy;
    static u8 disp_level,last_level;
    static u8 time_5ms_cnt = 0;

    time_5ms_cnt++;
    if(time_5ms_cnt >= 4){   //20ms计算一次能量
        time_5ms_cnt  = 0;

        energy = dac_pcm_pow_calc();
        //printf("[%X_%d]",rgb_led_en,energy);
        if (FUNC_MUSIC == func_cb.sta) {   //各模式能量效果可能不一样.
            energy = energy/450;
        } else if (FUNC_FMRX == func_cb.sta) {
            energy = energy/350;
        } else{
            energy = energy/300;
        }
        //非线性量化表
        for (i = 0, last_level = 0; i < 4*8; i++) {
           if (energy < energy_qtz_tbl[i]) {
               break;
           }
           last_level++;
        }
    }
    //能量相同, 不用更新
    if(disp_level == last_level){
        return;
    }

    //能量减少时,慢慢下降
    if (disp_level > last_level) {
        disp_level--;
    } else {
        disp_level = last_level;
    }

    //能量转为占空比,存放到pwm_duty_buf中.
    level = disp_level;
    for (i=0; i<ENERGY_LED_NUM; i++){
        if (level >= ENERGY_PWM_MAX_DUTY) {
            pwm_duty_buf[i] = 0;
            level -= ENERGY_PWM_MAX_DUTY;
        } else {
            pwm_duty_buf[i] = (1<<level) - 1;
            pwm_duty_buf[i] = ~pwm_duty_buf[i];
            level = 0;
        }
    }
}

AT(.com_text.rgb)
void energy_led_scan(void)  //建议1ms扫描一次.
{
    static u8 scan_bit = 0;
    if (pwm_duty_buf[0] & BIT(scan_bit)){
        ENERGY_LED_0_OFF();
    } else {
        ENERGY_LED_0_ON();
    }

    if (pwm_duty_buf[1] & BIT(scan_bit)) {
        ENERGY_LED_1_OFF();
    } else {
        ENERGY_LED_1_ON();
    }

    if (pwm_duty_buf[2] & BIT(scan_bit)) {
        ENERGY_LED_2_OFF();
    } else {
        ENERGY_LED_2_ON();
    }

    if (pwm_duty_buf[3] & BIT(scan_bit)) {
        ENERGY_LED_3_OFF();
    } else {
        ENERGY_LED_3_ON();
    }

    if (++scan_bit >= ENERGY_PWM_MAX_DUTY){ //ENERGY_PWM_MAX_DUTY= 8
        scan_bit = 0;
    }
}
#endif // ENERGY_LED_EN


