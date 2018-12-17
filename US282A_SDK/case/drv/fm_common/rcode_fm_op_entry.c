#include "fm_interface.h"
#include "fm_drv.h"

gpio_init_cfg_t gpio_twi[];

/* TWI 配置指针*/
gpio_init_cfg_t *twi_gpio = (gpio_init_cfg_t *) gpio_twi;

/* TWI 所用模拟GPIO 信息结构*/
gpio_init_cfg_t gpio_twi[2] =
{
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SCL_BIT },
    { GPIOAINEN, GPIOAOUTEN, GPIOADAT, GPIO_SDA_BIT }
};

/* TWI 操作延时*/
uint8 delay_twi = PARAM_DELAY_TWI;

/*FM 驱动对外接口函数*/
fm_driver_operations fm_drv_op =
{
    (fm_op_func) sFM_Init, //初始化
    (fm_op_func) sFM_Standby, //standby
    (fm_op_func) sFM_SetFreq, //设置频率
    (fm_op_func) sFM_GetStatus, //获取状态
    (fm_op_func) sFM_Mute, //静音
    (fm_op_func) sFM_Search, //软件搜台
    (fm_op_func) sFM_HardSeek, //硬件搜台
    (fm_op_func) sFM_SetBand, //设置波段
    (fm_op_func) sFM_SetThrod, //设置搜台门限
    (fm_op_func) sFM_BreakSeek, //中断搜索
    (fm_op_func) sFM_GetHardSeekflag, //获取硬件搜台的是否结束
    (fm_op_func) sFM_GetBand, //获取当前波段
    (fm_op_func) sFM_GetFreq, //获取当前播放频率
    (fm_op_func) sFM_GetIntsity, //获取当前信号强度
    (fm_op_func) sFM_GetAnten, //是否有天线
    (fm_op_func) sFM_GetStereo, //是否立体音
    (fm_op_func) sFM_Debug, //调试接口， 便于方案公司调试用
};

