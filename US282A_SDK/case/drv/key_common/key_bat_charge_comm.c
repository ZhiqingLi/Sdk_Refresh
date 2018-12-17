
/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       Wekan   2015-3-26am         1.0             review this file
 *******************************************************************************/

#include "psp_includes.h"
#include "key_interface.h"

extern uint8 g_key_bat_charge_mode;
extern uint16 g_charge_tri_time_cnt;

void key_sampling_battery_voltage(void);
static uint8 battery_samp_filter(void);


void key_bat_charge_init(chg_current_e CurrentMA, chg_voltage_e VoltageMV);

const chg_FullVol_e __section__(".rodata_0") set_chg_full_vol_tab[8] = //充电结果电压
{
    Chg_FullVol_4160mV,
    Chg_FullVol_4160mV,
    Chg_FullVol_4180mV,
    Chg_FullVol_4180mV,
    Chg_FullVol_4180mV,
    Chg_FullVol_4320mV,
    Chg_FullVol_4320mV,
    Chg_FullVol_4340mV
};

battery_status_e g_bat_charge_status = BAT_NORMAL;

uint8 g_bat_adc_val[BAT_ADC_DATA_NUM] =
{ 0 };//记录电池电压的ADC值
uint8 g_bat_adc_val_index = 0;
uint8 cur_battery_val = 0;

uint32 g_chg_ctl = 0;
uint32 g_spd_ctl = 0;
//保存默认充电配置
uint32 g_def_chg_ctl = 0;
//保存前台应用id
uint8 g_app_id = 0;

uint16 g_check_end_cnt = 0;
//区分bat check的模式
uint8 g_bat_ext_check = 0;
//5分钟开关充电标志
uint8 g_charge_switch = 0;

uint8 g_no_ext_chg = 0;

//用大电池和稳压源为准结果1-30
//低电电压
#define BATLOW_VEL      BAT_ADC_3_40_V
//充满电电压
#define BATFUL_VEL      BAT_ADC_4_15_V  //标案满电为4.15  +- 0.05

//电池电量等级对应 ADC 参考值
const uint8 __section__(".rodata_1") battery_grade_vel[BATTERY_GRADE_MAX+1] =
{
    0,              //低电    返回 0 -- 低电关机
    BATLOW_VEL,     //空格电  返回 1 -- 低电提示，1分钟提示1次
    BAT_ADC_3_60_V, //一格电  返回 2
    BAT_ADC_3_65_V, //一格电  返回 3
    BAT_ADC_3_70_V, //两格电  返回 4
    BAT_ADC_3_75_V, //两格电  返回 5
    BAT_ADC_3_80_V, //三格电  返回 6
    BAT_ADC_3_90_V, //三格电  返回 7
    BAT_ADC_4_00_V, //四格电  返回 8
    BAT_ADC_4_10_V  //四格电  返回 9
};

//电压降低后需要限制音量，以限制输出功率，否则会被拉死
const uint8 __section__(".rodata_1") battery_voltage_volume_limit[] =
{
    BAT_ADC_3_75_V, //返回 0db
    BAT_ADC_3_70_V, //返回 -1db
    BAT_ADC_3_65_V, //返回 -2db
    BAT_ADC_3_60_V, //返回 -3db
    BAT_ADC_3_50_V, //返回 -4db
    BAT_ADC_3_45_V, //返回 -5db
    BAT_ADC_3_40_V, //返回 -6db
};

#if (SUPPORT_OUTER_CHARGE == 1)
//用大电池和稳压源为准结果1-30
//低电电压
#define BATLOW_VEL_TEMP      TEMP_ADC_1_62_V
//充满电电压
#define BATFUL_VEL_TEMP      TEMP_ADC_2_04_V

//电池电量等级对应 ADC 参考值
//单个电池 3.3 ~ 4.2，2个电池串联 6.6 ~ 8.4，减掉0.2二极管固定压降 6.4 ~ 8.2，然后1/4分压为 1.6 ~ 2.05
const uint8 __section__(".rodata_1") battery_grade_vel_temp[BATTERY_GRADE_MAX+1] =
{
    0,               //低电    返回 0 -- 低电关机
    BATLOW_VEL_TEMP, //空格电  返回 1 -- 低电提示，1分钟提示1次
    TEMP_ADC_1_68_V, //一格电  返回 2
    TEMP_ADC_1_72_V, //一格电  返回 3
    TEMP_ADC_1_77_V, //两格电  返回 4
    TEMP_ADC_1_83_V, //两格电  返回 5
    TEMP_ADC_1_86_V, //三格电  返回 6
    TEMP_ADC_1_89_V, //三格电  返回 7
    TEMP_ADC_1_94_V, //四格电  返回 8
    TEMP_ADC_1_99_V  //四格电  返回 9
};

//电压降低后需要限制音量，以限制输出功率，否则会被拉死
const uint8 __section__(".rodata_1") battery_voltage_volume_limit_temp[] =
{
    TEMP_ADC_1_68_V,
    TEMP_ADC_1_66_V,
    TEMP_ADC_1_64_V,
    TEMP_ADC_1_62_V
};

#endif

uint8 g_charge_MA = 0;
uint8 g_charge_MV = 0;



/******************************************************************************/
/*!
 * \par  Description:
 *    获取电池电量，转换为电量等级。
 * \param[in]    batadc 电池电量ADC采样值，已由按键驱动处理过
 * \param[out]   none
 * \return       uint8 返回电池电量等级；如果大于等于充电满电压，返回 BATTERY_GRADE_MAX + 1
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
static uint32 __section__(".bank1") key_get_battery_grade(uint8 batadc)
{
    int32 i;

#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (batadc >= BATFUL_VEL_TEMP)
        {
            return 10;
        }
    }
    else
#endif
    {
        if (batadc >= BATFUL_VEL)
        {
            return 10;
        }
    }

    for (i = 9; i >= 0; i--)
    {
#if (SUPPORT_OUTER_CHARGE == 1)
        if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
        {
            if (batadc >= battery_grade_vel_temp[i])
            {
                return i;
            }
        }
        else
#endif
        {
            if (batadc >= battery_grade_vel[i])
            {
                return i;
            }
        }
    }

    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    获取当前电池电压需要降低音量多少db。
 * \param[in]    batadc 电池电量ADC采样值，已由按键驱动处理过
 * \param[out]   none
 * \return       int8 返回音量降低db
 * \ingroup      misc_func
 * \note
 *******************************************************************************/
static int8 __section__(".bank1") key_get_volume_limit(uint8 batadc)
{
    return 0;
#if 0    
    int8 i;   
    
    if (g_bat_charge_status != BAT_NORMAL)
    {
        return 0;
    }

#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        for (i = 0; i < sizeof(battery_voltage_volume_limit_temp); i++)
        {
            if (batadc >= battery_voltage_volume_limit_temp[i])
            {
                break;
            }
        }
    }
    else
#endif
    {
        for (i = 0; i < sizeof(battery_voltage_volume_limit); i++)
        {
            if (batadc >= battery_voltage_volume_limit[i])
            {
                break;
            }
        }
    }

    return -i;
#endif //if 0    
}

/******************************************************************************
 * \par  Description:   检测电源，检测电池，和开关充电
 * \                           电源插入会自动打开充电
 * \param[in]     null
 * \param[out]   none
 * \return           null
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-26am         1.0             review this file
 *******************************************************************************/
void ker_battery_charge_deal(void)
{
    key_sampling_battery_voltage();

#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        /*外挂充电检测*/
        if ((act_readl(EXTERN_CHARGE_DETECT_GPIODAT) & EXTERN_CHARGE_DETECT_PIN) != 0x0)
        {
            if ((act_readl(EXTERN_CHARGE_FULL_GPIODAT) & EXTERN_CHARGE_FULL_PIN) != 0x0)
            {
                g_bat_charge_status = BAT_FULL;
            }
            else
            {
                g_bat_charge_status = BAT_CHARGING;
            }
        }
        else
        {
            if (IS_EXTERN_BATTERY_IN() == 1) //带电池
            {
                g_bat_charge_status = BAT_NORMAL;
            }
            else
            {
                g_bat_charge_status = BAT_NO_EXIST;
            }
        }
    }
    else
#endif
    {
        if((g_bat_charge_status == BAT_CHECKING) || (g_app_id == 1) || (g_charge_switch == 1))
        {
            return;
        }
        
        //充电保持DC5V存在
        if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)//电源接入
        {
            if(g_bat_charge_status != BAT_FULL)
            {
                if(g_bat_charge_status != BAT_CHARGING)     //开充电，默认配置
                {
                    g_chg_ctl |= (1 << CHG_CTL_CHGEN); 
                    act_writel(g_spd_ctl,SPD_CTL);
                    act_writel(g_chg_ctl,CHG_CTL);
                }
                //bat_charge_open();
            }
            else                                            //关充电
            {
                g_chg_ctl &= (~(1 << CHG_CTL_CHGEN)); 
                bat_charge_close();
            }
        }
        else                                                //关充电
        {
            if(g_bat_charge_status == BAT_CHARGING)
            {
                g_chg_ctl &= (~(1 << CHG_CTL_CHGEN)); 
                g_bat_charge_status = BAT_NORMAL;
                bat_charge_close();
            }
        }
    }
}

/******************************************************************************
 * \par  Description:   打开充电
 * \param[in]     CurrentMA 设置充电电流
 * \param[in]     VoltageMV 设置充电电压
 * \param[out]   none
 * \return           null
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-26am         1.0             review this file
 *******************************************************************************/
void __section__(".bank0") key_inner_battery_charge_open(chg_current_e CurrentMA, chg_voltage_e VoltageMV, void* null2)
{
#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        ; //nothing
    }
    else
#endif
    {
        key_bat_charge_init(CurrentMA, VoltageMV);
    }
}

/******************************************************************************
 * \par  Description:   关闭充电
 * \param[in]     null
 * \param[out]   none
 * \return           null
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-26am         1.0             review this
 *******************************************************************************/
void __section__(".bank0") key_inner_battery_charge_stop(void* null0, void* null1, void* null2)
{
#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        ; //nothing
    }
    else
#endif
    {
        bat_charge_close();
        //g_bat_charge_status = BAT_FULL;
        //PRINT_INFO_INT("charge-stop(adc=):", battery_samp_filter());
    }
}

#if 0
//电池采样滤波，过滤掉最小的10个值
static uint8  __section__(".bank1") battery_samp_filter(void)
{
    uint8 i, j, temp;
    uint8 temp_bat_adc_val[BAT_ADC_DATA_NUM];
    uint32 bat_val = 0; 
    
    for (i = 0; i < BAT_ADC_DATA_NUM; i++)
    {
        temp_bat_adc_val[i] = g_bat_adc_val[i];
    }
        
    for (j = 0; j < BAT_ADC_DATA_NUM - 1; j++)
    {
    	for (i = 0; i < BAT_ADC_DATA_NUM - 1 - j; i++)
        {
        	 if (temp_bat_adc_val[i] < temp_bat_adc_val[i + 1])
        	 {
        		temp = temp_bat_adc_val[i];
        		temp_bat_adc_val[i] = temp_bat_adc_val[i + 1];
        		temp_bat_adc_val[i + 1] = temp;
        	 }
         }
	 }  

    //去掉10个最小值
    for (i = 0; i < (BAT_ADC_DATA_NUM - 10); i++)
    {           
        bat_val += temp_bat_adc_val[i];
    }

    return (uint8)((bat_val / (BAT_ADC_DATA_NUM - 10)) & 0x7f);//本次ADC的值的计算结果     
}
#endif

/******************************************************************************
 * \par  Description:   获取电池充电状态
 * \param[in]     null
 * \param[out]   ad_val  本次读取的电池采样adc值
 * \param[out]   vol_limit  电压降低后需要限制音量，以限制输出功率，否则会被拉死，单位为 db
 * \return           null
 * \note
 *   <author>    <time>           <version >             <desc>
 *    Wekan   2015-3-26am         1.0             review this
 *******************************************************************************/
battery_status_e __section__(".bank1") key_inner_battery_get_status(uint32 *ad_val, int8* vol_limit, uint8* app_id)
{ 
    bool bat_val_full = FALSE;
    uint32 bat_val = 0;
    uint8 i = 0;
    
    //libc_print("retadc",bat_val,2);
    for (i = 0; i < BAT_ADC_DATA_NUM; i++)
    {
        bat_val += g_bat_adc_val[i];
    }
    cur_battery_val = (uint8)((bat_val / BAT_ADC_DATA_NUM) & 0x7f);

    //*ad_val = cur_battery_val;
    *ad_val = key_get_battery_grade(cur_battery_val);
    *vol_limit = key_get_volume_limit(cur_battery_val);

    //libc_print("tempadc",(act_readb(TEMPADC_DATA)),2);
    if (*ad_val > BATTERY_GRADE_MAX)
    { //仅对判断是否充电满有意义，判断OK后，即可改为 BATTERY_GRADE_MAX
        bat_val_full = TRUE;
        //*ad_val = BATTERY_GRADE_MAX;放到common去做
    }
    
#if 1    
    if (g_bat_charge_status == BAT_CHARGING)    //20ms定时器只有在status != CHARGING的时候才会修改充电配置
    {
        if (cur_battery_val < BAT_ADC_3_50_V)   //小于3.50V,开100mA充电
        {
            act_writel((act_readl(CHG_CTL) & (~0x3800)) | 0x1000,CHG_CTL);
        }
        if (cur_battery_val > BAT_ADC_3_55_V)    //大于3.55V,开默认电流充电
        {
            act_writel(g_chg_ctl,CHG_CTL);
        }
    }
    if(g_no_ext_chg == 1)
    {
        if (g_bat_charge_status == BAT_NO_EXIST)    //NO EXIST状态时20ms定时器会开充电
        {
            g_chg_ctl = ((g_chg_ctl & (~0x3800)) | (0x1000));
        }
        else
        {
            g_chg_ctl = (g_def_chg_ctl | (g_chg_ctl & (1 << CHG_CTL_CHGEN)));
        }
        g_no_ext_chg = 0;
    }
#endif    

#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        ; //nothing
    }
    else
#endif
    {
        if(*app_id == 1)        //line in 充电特殊处理
        {
            if(g_app_id == 0)                                                   //进入line in
            {
                act_writel((act_readl(SPD_CTL) & (~0x180)),SPD_CTL);            //设置SYS_POWER为3.3
            }
            
            if(cur_battery_val <= BAT_ADC_3_70_V)
            {
                act_writel(g_def_chg_ctl,CHG_CTL);  //默认配置充电
                act_writel(act_readl(CHG_CTL) | 0x8000,CHG_CTL);                //开充电
                //libc_print("line in chg",cur_battery_val,2);
            }
            else if(cur_battery_val >= BAT_ADC_4_10_V)
            {
                act_writel(act_readl(CHG_CTL) & (~0x8000),CHG_CTL);             //关充电
                //libc_print("line in no chg",cur_battery_val,2);
            }
            else
            {
                ;//nothing
            }
        }
        else if(g_app_id == 1)                                                  //退出line in，恢复
        {
            act_writel(g_spd_ctl,SPD_CTL);
            act_writel(g_chg_ctl,CHG_CTL);
        }
        else
        {
            ;//nothing
        }
        
        g_app_id = *app_id;
        
        if (g_bat_charge_status == BAT_CHECKING)
        {
            if (g_bat_ext_check != 0)                //battery check
            {
                switch(g_check_end_cnt)
                {
                    case 0:
                    act_writel((act_readl(SPD_CTL) | 0x180),SPD_CTL);//SYSpower:4.4V
                    act_writel((act_readl(CHG_CTL) & 0xffffc7ff) | 0x9400,CHG_CTL);//开100mA充电
                    g_check_end_cnt ++;
                    break;
                    
                    case 3:
                    if((cur_battery_val > BAT_ADC_4_05_V) || (cur_battery_val < 0x03))
                    {
                        act_writel((act_readl(CHG_CTL) & (~0x8000)),CHG_CTL);//关充电
                        act_writel(act_readl(SPD_CTL) & (~0x180),SPD_CTL);//SYSpower:3.3V
                        g_check_end_cnt ++;
                    }
                    else
                    {
                        g_bat_charge_status = BAT_NORMAL;
                        act_writel(g_spd_ctl,SPD_CTL);
                        act_writel(g_chg_ctl,CHG_CTL);
                        g_bat_ext_check = 0;
                        g_check_end_cnt = 0;
                        g_charge_switch = 0;
                        g_no_ext_chg = 1;
                    }
                    break;

                    case 4:
                    if(cur_battery_val > BAT_ADC_3_50_V)
                    {
                        g_bat_charge_status = BAT_NORMAL;
                        g_charge_switch = 0;
                        g_no_ext_chg = 1;
                    }
                    else
                    {
                        g_bat_charge_status = BAT_NO_EXIST;
                    }
                    act_writel(g_spd_ctl,SPD_CTL);
                    act_writel(g_chg_ctl,CHG_CTL);
                    g_bat_ext_check = 0;
                    g_check_end_cnt = 0;
                    g_no_ext_chg = 1;
                    if(g_charge_switch == 1)
                    {
                        act_writel((act_readl(CHG_CTL) & (~0x8000)),CHG_CTL);//关充电
                    }
                    break;

                    default:
                    g_check_end_cnt ++;
                    break;
                }
            }
            else                                //charge check
            {
                if (g_check_end_cnt >= 10)      //关充电5s后认为确实充满
                {
                    g_bat_charge_status = BAT_FULL;
                    g_check_end_cnt = 0;
                }
                else if (bat_val_full == FALSE)
                {
                    g_bat_charge_status = BAT_NORMAL;
                    g_check_end_cnt = 0;
                }
                else
                {
                    g_check_end_cnt ++;
                }
            }
            //libc_print("g_bat_charge_status",g_bat_charge_status,2);
            return g_bat_charge_status;
        }
        
        if (g_bat_charge_status == BAT_CHARGING)    //检测到电池电压满则关闭充电再检测
        {
            if (bat_val_full == TRUE)
            {
                g_charge_tri_time_cnt++;
                if (g_charge_tri_time_cnt > (120 * 15)) //15min
                {
                    g_bat_charge_status = BAT_CHECKING;
                    bat_charge_close();
                    g_charge_tri_time_cnt = 0;
                    return g_bat_charge_status;
                }
            }
            else
            {
                g_charge_tri_time_cnt = 0;
            }
        }
        
        if (g_bat_charge_status == BAT_NO_EXIST)
        {
            g_check_end_cnt ++;
            if(g_check_end_cnt >= (60 * 2 * 5))             //充5分钟复查一次，再停5分钟
            {
                g_bat_ext_check = 1;                    //检测电池存在与否,硬件方法不可用
                g_check_end_cnt = 0;
                g_bat_charge_status = BAT_CHECKING;
                if(g_charge_switch == 0)
                {
                    g_charge_switch = 1;
                }
                else
                {
                    g_charge_switch = 0;
                }
            }
            return g_bat_charge_status;
        }
        
        if (g_bat_charge_status == BAT_FULL)
        {
            if(bat_val_full == FALSE)
            {
                g_bat_charge_status = BAT_NORMAL;
            }
            else
            {
                return g_bat_charge_status;
            }
        }
        else if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)//电源接入
        {
            if(get_bat_charge_open() != 0)
            {
                g_bat_charge_status = BAT_CHARGING;
            }
            else
            {
                g_bat_charge_status = BAT_NORMAL;
            }
        }
        else        //DC5V未接入，电池供电
        {
            g_bat_charge_status = BAT_NORMAL;
        }
    }
    return g_bat_charge_status;
}

/******************************************************************************
 * \par  Description:  初始化充电
 * \param[in]    CurrentMA 设置电电流
 * \param[in]    VoltageMV 设置电电压
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26am
 *******************************************************************************/
void __section__(".bank0") key_bat_charge_init(chg_current_e CurrentMA, chg_voltage_e VoltageMV)
{
    uint32 tmp;
#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        act_writel(act_readl(EXTERN_CHARGE_DETECT_GPIOIN) | EXTERN_CHARGE_DETECT_PIN, EXTERN_CHARGE_DETECT_GPIOIN);
        act_writel(act_readl(EXTERN_CHARGE_FULL_GPIOIN) | EXTERN_CHARGE_FULL_PIN, EXTERN_CHARGE_FULL_GPIOIN);

        act_writel(act_readl(EXTERN_BATTERY_DETECT_GPIOIN) | EXTERN_BATTER_DETECT_PIN, EXTERN_BATTERY_DETECT_GPIOIN);

        if (IS_EXTERN_BATTERY_IN() == 1) //带电池
        {
            g_bat_charge_status = BAT_NORMAL;
        }
        else //不带电池
        {
            g_bat_charge_status = BAT_NO_EXIST;
        }
        //for adc charge value use
        act_writel((act_readl(AD_Select) & EXTERN_CHARGE_VAL_MSK)|EXTERN_CHARGE_VAL_SEL, AD_Select);
		//act_writel(((act_readl(MFP_CTL0) & (~MFP_CTL0_GPIOA21_MASK)) | (0x7 << MFP_CTL0_GPIOA21_SHIFT)),MFP_CTL0);
        //wait for analog function stable
        sys_mdelay(10);
    }
    else
#endif
    {
        if(CurrentMA == 0)
        {
            CurrentMA = g_charge_MA;
        }
        if(VoltageMV == 0)
        {
            VoltageMV = g_charge_MV;
        }
        g_charge_MA = CurrentMA;
        g_charge_MV = VoltageMV;
        if(CurrentMA >= Chg_current_600mA)
        {
            CurrentMA = Chg_current_600mA;
        }
        if(VoltageMV >= Chg_voltage_4410mV)
        {
            VoltageMV = Chg_voltage_4410mV;
        }
        ;//延时
        tmp = act_readl(CHG_CTL);
        tmp &= (~CHG_CTL_CHG_CURRENT_MASK)&(~CHG_CTL_STOPV_MASK)&(~CHG_CTL_ENFASTCHG_MASK)&(~(1 << CHG_CTL_ENTKLE));

        tmp |= (uint32) ((0 << CHG_CTL_ENTKLE) //不开涓流(使能涓流时为充电电流0.1)
                | (CurrentMA << CHG_CTL_CHG_CURRENT_SHIFT)//设置充电电流
                | (VoltageMV << CHG_CTL_STOPV_SHIFT) //设置充满电压4.16  4.18  4.32 4.34
                | (set_chg_full_vol_tab[(uint8)VoltageMV] << CHG_CTL_ENFASTCHG_SHIFT)//设置恒压充电电压
                | (1 << CHG_CTL_CHGOP_SEL));//新方法充电

        act_writel(tmp, CHG_CTL);
        
        g_chg_ctl = act_readl(CHG_CTL);
        g_spd_ctl = act_readl(SPD_CTL);
        g_def_chg_ctl = g_chg_ctl;
        
        if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)
        {
            g_bat_ext_check = 1;                    //检测电池存在与否,硬件方法不可用
            g_check_end_cnt = 0;
            g_bat_charge_status = BAT_CHECKING;
        }
        else
        {
            g_bat_charge_status = BAT_NORMAL;       //无DC5V接入则为电池供电
        }
    }

    g_charge_tri_time_cnt = 0;
}

/******************************************************************************
 * \par  Description:  电池电压采样
 * \param[in]    void
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26am
 *******************************************************************************/
void key_sampling_battery_voltage(void)
{
    uint8 bat_adc;
#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_BATTERY_IN() == 1)
        {
            bat_adc = (act_readb(TEMPADC_DATA))& 0x7F;
        }
        else
        {
            bat_adc = 0x74;
        }
    }
    else
#endif
    {
        bat_adc = (act_readb(BATADC_DATA)) & 0x7f;//0-6bit有效
    }

    if(g_bat_adc_val_index >= BAT_ADC_DATA_NUM)
    {
        g_bat_adc_val_index = 0;
    }   
    g_bat_adc_val[g_bat_adc_val_index] = bat_adc;
    g_bat_adc_val_index++;

}

/******************************************************************************
 * \par  Description:  电池电压采样初始化
 * \param[in]    void
 * \param[out]   null
 * \return           null
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26am
 *******************************************************************************/

void __section__(".bank0") key_sampling_battery_voltage_init(void)
{
    uint8 bat_adc;
    uint8 i;

    //初始化电池电量采样计算模块
#if (SUPPORT_OUTER_CHARGE == 1)
    if (g_key_bat_charge_mode == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_BATTERY_IN() == 1)
        {
            bat_adc = (act_readb(TEMPADC_DATA))& 0x7F;
        }
        else
        {
            bat_adc = 0x74;
        }
    }
    else
#endif
    {
        bat_adc = (act_readb(BATADC_DATA)) & 0x7F;
    }

    for (i = 0; i < BAT_ADC_DATA_NUM; i++)
    {
        g_bat_adc_val[i] = bat_adc;//记录电池电压的ADC值
    }
    g_bat_adc_val_index = 0;
    cur_battery_val = bat_adc;
}

