/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：分立LED灯物理操作。
 * 作者：cailizhen
 ********************************************************************************/

#include "psp_includes.h"
#include "key_interface.h"

void _SetLEDOn_gpio(led_port_e port, uint8 pin);
void _SetLEDOff_gpio(led_port_e port, uint8 pin);
void _SetLEDOn_pwm(led_port_e port);
void _SetLEDOff_pwm(led_port_e port);
void __gpio_init(led_port_e port, uint8 pin);
void __pwm_init(led_port_e port);

void key_inner_discrete_led_init(led_port_e port, uint8 pin)
{
    if (port <= LED_PORT_GPIOB)
    {
        __gpio_init(port, pin);
    }
    else
    {
        __pwm_init(port);
    }
}

void __section__(".text.led_twinkle") key_inner_discrete_led_on(led_port_e port, uint8 pin)
{
    if (port <= LED_PORT_GPIOB)
    {
        _SetLEDOn_gpio(port, pin);
    }
    else
    {
        _SetLEDOn_pwm(port);
    }
}

void __section__(".text.led_twinkle") key_inner_discrete_led_off(led_port_e port, uint8 pin)
{
    if (port <= LED_PORT_GPIOB)
    {
        _SetLEDOff_gpio(port, pin);
    }
    else
    {
        _SetLEDOff_pwm(port);
    }
}

void __section__(".text.led_twinkle") key_inner_discrete_led_op(led_port_e port, uint8 pin, discrete_led_op_e op)
{
    switch (op)
    {
    case DISCRETE_LED_INIT:
        key_inner_discrete_led_init(port, pin);
        break;

    case DISCRETE_LED_ON:
        key_inner_discrete_led_on(port, pin);
        break;

    case DISCRETE_LED_OFF:
        key_inner_discrete_led_off(port, pin);
        break;

    default:
        break;
    }
}

void __section__(".text.led_twinkle") _SetLEDOn_gpio(led_port_e port, uint8 pin)
{
    if (LED_PORT_GPIOA == port)
    {
#if (LED_ON_GPIO_SEL == LED_ON_GPIO_HIGH)
        act_writel(act_readl(GPIOADAT) | (uint32) (1 << pin), GPIOADAT);
#else
        act_writel(act_readl(GPIOADAT) & (~(uint32) (1 << pin)), GPIOADAT);
#endif
    }
    else
    {
#if (LED_ON_GPIO_SEL == LED_ON_GPIO_HIGH)
        act_writel(act_readl(GPIOBDAT) | (uint32) (1 << pin), GPIOBDAT);
#else
        act_writel(act_readl(GPIOBDAT) & (~(uint32) (1 << pin)), GPIOBDAT);
#endif
    }
}

void __section__(".text.led_twinkle") _SetLEDOff_gpio(led_port_e port, uint8 pin)
{
    if (LED_PORT_GPIOA == port)
    {
#if (LED_ON_GPIO_SEL == LED_ON_GPIO_HIGH)
        act_writel(act_readl(GPIOADAT) & (~(uint32) (1 << pin)), GPIOADAT);
#else
        act_writel(act_readl(GPIOADAT) | (uint32) (1 << pin), GPIOADAT);
#endif
    }
    else
    {
#if (LED_ON_GPIO_SEL == LED_ON_GPIO_HIGH)
        act_writel(act_readl(GPIOBDAT) & (~(uint32) (1 << pin)), GPIOBDAT);
#else
        act_writel(act_readl(GPIOBDAT) | (uint32) (1 << pin), GPIOBDAT);
#endif
    }
}

void __section__(".text.led_twinkle") _SetLEDOn_pwm(led_port_e port)
{

}

void __section__(".text.led_twinkle") _SetLEDOff_pwm(led_port_e port)
{

}

void __gpio_init(led_port_e port, uint8 pin)
{
    _SetLEDOff_gpio(port, pin);
    if (port == LED_PORT_GPIOA)
    {
        act_writel(act_readl(GPIOAINEN) & (~(uint32) (1 << pin)), GPIOAINEN);
        act_writel(act_readl(GPIOAOUTEN) | (uint32) (1 << pin), GPIOAOUTEN);
    }
    else
    {
        act_writel(act_readl(GPIOBINEN) & (~(uint32) (1 << pin)), GPIOBINEN);
        act_writel(act_readl(GPIOBOUTEN) | (uint32) (1 << pin), GPIOBOUTEN);
    }
}

void __pwm_init(led_port_e port)
{
    //设置呼吸灯
    //呼吸灯的流程是：爬升->维持高->下降->维持低
    //PWM的频率不能少于100Hz,否则会看到闪烁(也就是说：Tpwm <= 0.01s)
    //爬升的时间 = (Q + 1) * 32 * Tpwm
#define Q       (4 - 1)
    //爬升后，最后维持亮的时间 = H * Tpwm
#define H       (255)
    //下降后，维持灭的时间 = L * Tpwm
#define L       (255)
    act_writel(0
            | (1 << PWM0_CTL_Mode_SEL) //设置为呼吸模式
            | (Q << PWM0_CTL_Q_SHIFT)
            | (H << PWM0_CTL_H_SHIFT)
            | (L << PWM0_CTL_L_SHIFT)
            ,
            PWM0_CTL);
    //设置PWM0的时钟
    act_writel(0
            | (7 << CMU_PWM0CLK_PWMCLKDIV_SHIFT)
            , CMU_PWM0CLK);//设置时钟源为PMU_32K, 8分频(125Hz)
    //也可以选择HOSC为时钟源
    //act_writel(0
    //    | (1 << CMU_PWM0CLK_PWMCLKSEL)
    //    | (255 << CMU_PWM0CLK_PWMCLKDIV_SHIFT)
    //    ,
    //    CMU_PWM0CLK);//设置时钟源为HOSC, 256分频
    act_writel(act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_PWMCLKEN), CMU_DEVCLKEN);//使能PWM CLK
    //把A7设置为PWM0
    act_writel(act_readl(MFP_CTL1) | (4 << MFP_CTL1_GPIOA7_SHIFT), MFP_CTL1);
    //PWM正常化
    act_writel(act_readl(MRCR) | (1 << MRCR_PWM_LIGHT_Reset), MRCR);
}

