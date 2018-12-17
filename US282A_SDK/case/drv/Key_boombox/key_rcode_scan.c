/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                             All Rights Reserved.
 *      Description:查询按键状态和处理
 *      <author>       <time>
 *       Wekan       2015-3-26pm
 *******************************************************************************/

#include "key_inner.h"

//标识当前按键为红外遥控按键
uint8 g_ir_key = 0;
//当前遥控器用户码数据
uint8 g_current_usercode = IR_CODE_FF00;
uint8 g_ir_oldkey_val = NO_KEY;
uint8 g_ir_tmp_count;

//当前为触摸按键
uint8 g_tk_key = 0;
uint8 g_tk_oldkey_val = NO_KEY;
uint8 g_tk_tmp_count;

uint8 g_oldkey_val = NO_KEY;
uint8 g_tmpkey_val = NO_KEY;
uint8 g_tmp_count;
uint8 g_hold_count;
uint8 g_up_count;

#if (SUPPORT_ADC_KEY == 1)
#define ADC_KEY_NUM  (7)

const uint8 g_adc_data[ADC_KEY_NUM] =
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
{   0x3, 0x0e, 0x1c, 0x29, 0x38, 0x47, 0x59};
#else
{   0x08, 0x14, 0x22, 0x2F, 0x3E, 0x4D, 0x5F};
#endif
#endif

#if( SUPPORT_HPY_DRIVER  == 1)

uint32 in_reg_arrary[3][3] =
{
    {   KSIN0_EN_REG, KSIN0_DATA_REG, KSIN0_REG_BIT},
    {   KSIN1_EN_REG, KSIN1_DATA_REG, KSIN1_REG_BIT},
    {   KSIN2_EN_REG, KSIN2_DATA_REG, KSIN2_REG_BIT}
};

uint32 out_reg_arrary[3][4] =
{
    {   KSOUT0_EN_REG, KSOUT0_DATA_REG, KSOUT0_EN_REG_BIT,KSOUT0_DATA_REG_BIT},
    {   KSOUT1_EN_REG, KSOUT1_DATA_REG, KSOUT1_REG_BIT ,KSOUT1_REG_BIT},
    {   KSOUT2_EN_REG, KSOUT2_DATA_REG, KSOUT2_REG_BIT ,KSOUT2_REG_BIT}
};

#endif

int g_time_irq;

uint8 g_key_bat_charge_mode; //0表示内部充电，1表示外部充电
uint16 g_charge_tri_time_cnt; //涓流充电时间计数器关充电JL.Li

scan_isr_cbk g_key_scan_isr_cbk = NULL;

uint8 g_i2s_tk_enable = FALSE;

int key_inner_register_scan_isr_cbk(scan_isr_cbk cbk)
{
    g_key_scan_isr_cbk = cbk;

    return 0;
}

#ifdef SWITCH_KEY_POWER_OFF
/******************************************************************************/
/*!
 * \par  Description:
 * \ void shut_off_switch_detect(void)
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 软关机方案检测是否有关机操作(假硬开关？)
 *******************************************************************************/

void key_shut_off_switch_detect(void)
{

    uint16 msg;
    static uint8 count3 = 0;
    //input_gui_msg_t gui_msg;
    uint32 key_data;

    //dc5v exist, we wil' not send power off
    if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)
    {
        return;
    }
    act_writel((act_readl(SHUTOFF_DETECT_IO_OUT_EN_REG) & SHUTOFF_DETECT_CLR_BIT), SHUTOFF_DETECT_IO_OUT_EN_REG);
    act_writel((act_readl(SHUTOFF_DETECT_IO_IN_EN_REG) | SHUTOFF_DETECT_SET_BIT), SHUTOFF_DETECT_IO_IN_EN_REG);
    sys_udelay(100);
    if ((act_readl(SHUTOFF_DETECT_IO_DATA_REG) & SHUTOFF_DETECT_SET_BIT)==0)
    {
        count3++;
        if(count3 >= 20)
        {
            //发送拨动按键消息
        }
    }
}
#endif

/******************************************************************************
 * \par  Description:   检测ADC 按键是否按下
 * \param[in]     none
 * \param[out]   adc_val 按按的adc采样值
 * \return           按键结果
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
#if( SUPPORT_HPY_DRIVER  == 1)
uint8 key_gpio_status_check(void)
{
    uint8 gpio_key_val = NO_KEY;

    uint8 i,j, n = 0;
    for (i = 0; i < 3; i++)
    {
        if(i != 0)
        {
            act_writel((act_readl(out_reg_arrary[i][1]) | out_reg_arrary[i][3]), out_reg_arrary[i][1]);
            for (j = 0; j < 3; j++)
            {
                //input value read
                if ((act_readl(in_reg_arrary[j][1]) & in_reg_arrary[j][2]) != 0)
                {
                    gpio_key_val = i;
                    break;
                }
                n++;
            }
        }
        //out put low
        if(i> 0)
        {
            act_writel((act_readl(out_reg_arrary[i][1]) & (~out_reg_arrary[i][3])), out_reg_arrary[i][1]);
        }
        if (gpio_key_val != NO_KEY)
        {
            break;
        }
    }

    return gpio_key_val;
}
#endif

/******************************************************************************
 * \par  Description:   检测ADC 按键是否按下
 * \param[in]     none
 * \param[out]   adc_val 按按的adc采样值
 * \return           按键结果
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
#if (SUPPORT_ADC_KEY == 1)
uint8 key_lradc_status_check(uint8* adc_val)
{
    //check key val
    uint8 lradc_key_val = NO_KEY;
    uint8 i;
    uint8 adc_data;

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    adc_data = act_readb(LRADC2_DATA) & 0x7F;//[0~6bit]
#else
    adc_data = act_readb(LRADC1_DATA) & 0x7F;//[0~6bit]
#endif
    for (i = 0; i < ADC_KEY_NUM; i++)
    {
        if (adc_data < g_adc_data[i])
        {
            lradc_key_val = i;
            break;
        }
    }

    //check on off key
    if ((act_readl(ONOFF_KEY) & (1<<ONOFF_KEY_ONOFF_PRESS_0)) != 0)
    {
        lradc_key_val = KEY_PHY_PLAY;//play
        adc_data = 0;//skip hold state
    }

    *adc_val = adc_data;
    return lradc_key_val;
}
#endif

/******************************************************************************
 * \par  Description:   按键抬起对扫描按键使用的变量初始化
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/

void key_init_key_vars(void)
{
    //清扫描次数
    g_tmp_count = 0;
    //hold key count
    g_hold_count = 0;
    //up key count
    g_up_count = 0;
    //last key value
    g_oldkey_val = NO_KEY;

    g_tmpkey_val = NO_KEY;
}

/******************************************************************************
 * \par  Description:  处理按键抬起
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
void key_deal_key_up(void)
{
#if (SUPPORT_IR_DRIVER == 1)
    if (g_ir_key != 0)
    {
        //是否为红外按键
        g_ir_tmp_count++;
        if (g_ir_tmp_count >= 7)
        {
            //发keyup消息
            post_key_msg(KEY_TYPE_IR, KEY_PHY_UP, g_ir_oldkey_val);
            g_ir_key = 0;
        }
    }
#endif

#if (SUPPORT_TK_DRIVER == TK_DRIVER_INNER)
    if (g_tk_key != 0)
    {
        //是否为触摸按键
        if (key_tk_up_status_check() == TRUE)
        {
            //whether up
            //发keyup消息
            post_key_msg(KEY_TYPE_TK, KEY_PHY_UP, g_tk_oldkey_val);
            //clear tk flag
            g_tk_key = 0;
            g_tk_oldkey_val = NO_KEY;
        }
        else
        {
            //long or hold
            g_tk_tmp_count++;
            if (g_tk_tmp_count >= HOLD_KEY_TIMER)
            {
                //发键值消息
                post_key_msg(KEY_TYPE_TK, KEY_PHY_CONT, g_tk_oldkey_val);
                g_tk_tmp_count = 0;
            }
        }
    }
#endif
}

/******************************************************************************
 * \par  Description:  按键扫描以及充电轮询，
 每隔20ms扫描一次，按键已经含有去抖处理
 * \param[in]     none
 * \param[out]   none
 * \return           none
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
void key_scan_int_process(void)
{
    unsigned char key_val = NO_KEY;
#if (SUPPORT_ADC_KEY == 1)
    unsigned char adcdat;
#endif
    key_phy_type_e key_type = KEY_TYPE_KEY;

    ker_battery_charge_deal();

#if (SUPPORT_IR_DRIVER == 1)
    key_ir_status_check();
#endif

    key_deal_key_up();

#if (SUPPORT_ADC_KEY == 1)
    //check if lradc key press
    key_val = key_lradc_status_check(&adcdat);
    if (key_val == NO_KEY)
    {
#ifdef SWITCH_KEY_POWER_OFF
        key_shut_off_switch_detect();
#endif
    }
#endif

#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
    key_type = KEY_TYPE_TK;
    key_val = key_i2c_tk_check();
#endif
#ifdef SUPPORT_UART_SIM_KEY
   uart1_process();

   key_val = record_key_play(key_val);
#endif
    /*没有按键*/
    if (g_oldkey_val == NO_KEY)
    {
        /*没有检测到按键动作*/
        if ((g_tmpkey_val == NO_KEY) && (key_val == NO_KEY))
        {
            return;
        }

        /*去除抖动*/
        if ((g_tmpkey_val != NO_KEY) && (key_val != g_tmpkey_val))
        {
            key_init_key_vars();
            return;
        }
    }

    /*有按键时，去除抖动，等同于按键抬起*/
    if ((g_oldkey_val != NO_KEY) && ((key_val != NO_KEY) && (key_val != g_oldkey_val)))
    {
        //发keyup消息
        post_key_msg(key_type, KEY_PHY_UP, g_oldkey_val);
        key_init_key_vars();
    }

    if (g_oldkey_val == NO_KEY)
    {
        if (g_tmp_count == 0)
        {
            g_tmpkey_val = key_val;
        }

        if (key_val == g_tmpkey_val)
        {
            //累加扫描次数
            g_tmp_count++;
            //扫描三次，是同一键值，确定有按键按下
            if (g_tmp_count >= DOWN_KEY_TIMER)
            {
                //如果本次扫描捕获键值不为空
                //发送按键消息
                g_oldkey_val = g_tmpkey_val;
                //save old key as cur value
                post_key_msg(key_type, KEY_PHY_DOWN, g_oldkey_val);
                g_tmp_count = 0;
#ifdef SUPPORT_UART_SIM_KEY
			    get_keyval(g_oldkey_val);
#endif
            }
        }
    }
    else
    {
        //如果本次键值等于上次按键值,累计8次(160ms)，发一次按键消息
        if (key_val != NO_KEY)
        {
            //长按消息
            g_hold_count++;
            g_up_count = 0; //按住按键中间突然抖动一下，产生1次错误采样值，需要清除掉

            if (g_hold_count == HOLD_KEY_TIMER)
            {
                g_hold_count = 0;
                //发键值消息
                post_key_msg(key_type, KEY_PHY_CONT, g_oldkey_val);
            }
#ifdef SUPPORT_UART_SIM_KEY
			get_keytime();
#endif
        }
        else
        {
            g_up_count++;
            if (g_up_count >= UP_KEY_TIMER)
            {
                //发keyup消息
                post_key_msg(key_type, KEY_PHY_UP, g_oldkey_val);
                key_init_key_vars();
#ifdef SUPPORT_UART_SIM_KEY
				reocrdkey_print();
#endif
            }
        }
    }
}

/******************************************************************************
 * \par  Description:    封装按键信息，并发送给消息队列
 * \param[in]     key_type 按键类型:物理、红外、tk
 * \param[in]    key_status 按键状态:刚按下、继续按下、抬起
 * \param[in]    key_id 按键值
 * \param[out]   none
 * \return           成功发送返回true
 * \note
 *   <author>    <time>
 *    Wekan   2015-3-26pm
 *******************************************************************************/
bool post_key_msg(key_phy_type_e key_type, key_phy_status_e key_status, uint8 key_id)
{
    key_phy_msg_t key_msg;
    static int32 last_key_val = -1;
    key_msg.key_type = key_type;
    key_msg.key_status = key_status;
    if (key_type == KEY_TYPE_IR)
    {
        key_msg.key_ir_code = g_current_usercode;
    }
    key_msg.key_id = key_id;
    key_msg.key_timestamp = (uint16) sys_get_ab_timer();

    post_again: 
    if(last_key_val != ((key_type<<16)+(key_status<<8)+key_id))
    {
        last_key_val = (key_type << 16) +(key_status << 8) + key_id;
        sys_irq_print("key_val:", last_key_val, 2);
    }
    if ((int) sys_mq_send(MQ_ID_GUI, (void *) &key_msg) == 0)
    {
        return TRUE;
    }
    else
    {
        if (key_status == KEY_PHY_UP)
        {
            key_phy_msg_t drop_key_msg;

            if ((int) sys_mq_receive(MQ_ID_GUI, (void *) &drop_key_msg) == 0)
            {
                goto post_again;
            }
        }
    }

    //消息队列已满，返回false
    return FALSE;
}

