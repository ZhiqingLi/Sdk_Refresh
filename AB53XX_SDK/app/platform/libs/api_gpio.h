#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

enum {
    GPIOxSET    =   0,
    GPIOxCLR,
    GPIOx,
    GPIOxDIR,
    GPIOxDE,
    GPIOxFEN,
    GPIOxDRV,
    GPIOxPU,
    GPIOxPD,
    GPIOxPU200K,
    GPIOxPD200K,
    GPIOxPU300,
    GPIOxPD300,
};

typedef struct {
    psfr_t sfr;             //GPIO SFR ADDR
    u8 num;
    u8 type;                //type = 1,高压IO，没有300R的强上下拉电阻。 type = 0, 普通IO, 有内部300R上下拉电阻。
} gpio_t;

#define bsp_gpio_cfg_init(x, y)         gpio_cfg_init(x, y)

void gpio_cfg_init(gpio_t *g, u8 io_num);       //根据GPIO number初始化GPIO结构体
void wakeup_disable(void);
void adcch_io_wakeup_config(u8 adc_ch);         //ADCCH GPIO下降沿唤醒配置
void wakeup_gpio_config(u8 io_num, u8 edge);    //任意GPIO的边沿唤醒配置  0:内部下拉上升沿唤醒, 1:内部上拉下降沿唤醒, 2:内部上拉上升沿唤醒
void wakeup_wko_config(void);                   //配置WKO唤醒
void wakeup_udisk_config(u8 edge);              //配置U盘唤醒 0:上升沿唤醒, 1：下降沿唤醒
u32 wakeup_get_status(void);                    //获取唤醒状态，公共区函数
void adcch_io_pu10k_enable(u8 adc_ch);

#endif // _BSP_GPIO_H
