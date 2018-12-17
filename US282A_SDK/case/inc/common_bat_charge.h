/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：充电与电池电量相关的宏定义，枚举类型，结构体定义等；该头文件为KEY驱动和
 *       COMMON 共享。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __KEY_BAT_CHARGE__H
#define __KEY_BAT_CHARGE__H

#include  "psp_includes.h"

/*! 电池电压等级数，0 ~ 9，0表示显示低电关机，1表示低电需提示 */
#define BATTERY_GRADE_MAX      9

/* 充电电流 */
typedef enum
{
    Chg_current_25mA = 0,
    Chg_current_50mA,
    Chg_current_100mA,
    Chg_current_200mA,
    Chg_current_300mA,
    Chg_current_400mA,
    Chg_current_500mA,
    Chg_current_600mA,
} chg_current_e;

/* 恒压充电电压 */
typedef enum
{
    Chg_voltage_4200mV = 0,
    Chg_voltage_4230mV,
    Chg_voltage_4260mV,
    Chg_voltage_4290mV,
    Chg_voltage_4320mV,
    Chg_voltage_4350mV,
    Chg_voltage_4380mV,
    Chg_voltage_4410mV
} chg_voltage_e;

/* 充电结果电压 */
typedef enum
{
    Chg_FullVol_4160mV = 0,
    Chg_FullVol_4180mV,
    Chg_FullVol_4320mV,
    Chg_FullVol_4340mV
} chg_FullVol_e;

typedef enum
{
    BAT_NORMAL = 0, //电池供电且不在充电状态
    BAT_CHARGING, //正在充电
    BAT_FULL, //充满
    BAT_NO_EXIST, //没有电池
    BAT_CHECKING, //正在检测电池中
} battery_status_e;



#define BAT_LOW_WARNING_CYCLE  120  //约60秒,此值要大于10秒
#define BAT_FULL_WARNING_CYCLE  120  //约60秒，此值要大于10秒


/*! 电池电压采样数据记录个数 */
#define BAT_ADC_DATA_NUM    50

#define bat_check_open()       act_writel((act_readl(CHG_CTL) | (1 << CHG_CTL_ENBATDT)), CHG_CTL)
#define bat_check_close()      act_writel((act_readl(CHG_CTL) & (~(1 << CHG_CTL_ENBATDT))), CHG_CTL)

#define bat_check_end()        (act_readl(CHG_DET)&(1 << CHG_DET_DTOVER))
#define bat_check_status()     (act_readl(CHG_DET)&(1 << CHG_DET_BATEXT))

#define bat_charge_open()      act_writel(act_readl(CHG_CTL) | (1 << CHG_CTL_CHGEN), CHG_CTL)
#define bat_charge_close()     act_writel(act_readl(CHG_CTL) & (~(1 << CHG_CTL_CHGEN)), CHG_CTL)

#define bat_charge_trickle()   (act_readl(CHG_DET) & (1 << CHG_DET_CHG_STA0))//充电电流小于7.5%
#define bat_charge_full()      (act_readl(CHG_DET) & (1 << CHG_DET_CHGEND))//充满

#define get_bat_charge_open()  (act_readl(CHG_CTL) & (1 << CHG_CTL_CHGEN))

#endif

