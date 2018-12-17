/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2008 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen     2013-5-18 10:29    1.0             build this file
 *******************************************************************************/
/*!
 * \file     key_banka0_charge.c
 * \brief    Get and set charge state
 * \author   johnsen
 * \version 1.0
 * \date  2013-5-18 10:29
 *******************************************************************************/
#include "key_inner.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    Get charge state
 * \param[in]      none reserve 0
 * \param[in]      none reserve 0
 * \param[in]      none reserve 0
 * \param[out]   charge state
 * \return       the result
 * \retval           0 sucess
 * \retval           1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note    :充电是否满的指示，充满时关闭充电。在应用中每10秒钟调用一次
 *******************************************************************************/
charge_state_e key_inner_chargeget(void *null0, void *null1, void *null2)
{
    charge_state_e ret = CHARGE_CHARGING;
    /*if(g_the_charge==0)
     {
     ret = CHARGE_FULL;
     }
     else
     {
     ret = CHARGE_CHARGING;
     }*/
    //if charging
//    if ((act_readl(CHG_DET) & (1 << CHG_DET_CHG_STA0)) == 0)
//    {
//        ret = CHARGE_FULL;
//    }
//    else
//    {
//        ret = CHARGE_CHARGING;
//    }

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    Set charge
 * \param[in]      setting CHARGE_START:start charging CHARGE_STOP:stop charging
 * \param[in]      current  choose from charge_current_e
 * \param[in]      param3   choose from battery_full_t
 * \param[out]   charge state
 * \return       the result
 * \retval           0 sucess
 * \retval           1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
uint8 key_inner_chargeset(charge_control_e setting, charge_current_e current, battery_full_t param3)
{
    uint8 ret = 0;

//    if (setting == CHARGE_START)
//    {
//        //check batter exist
//        if (key_bat_exist_check() == TRUE)
//        {
//            //设置用户充电电流
//            //act_writeb(((act_readb(CHG_CTL) & (uint8)0xf0)|ChargeCurrent), CHG_CTL);
//            //open charge circute
//            key_open_charge_init();
//        }
//
//    }
//    else //CHARGE_STOP
//    {
//        //stop charge, clr charge flag
//        //g_the_charge = 0;
//        key_stop_charge_exit();
//    }

    return ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    return key map address to application
 * \param[in]    none
 * \param[out]   unsigned int
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * speak 设置开关配置
 *******************************************************************************/
uint8 *key_inner_speakcheck(uint8 onoff, void *null1, void *null2)
{
//    g_speakonoff = onoff;
//    if (g_speakonoff == 0)
//    {
//        act_writel(act_readl(GPIOADAT) & 0xfffffffd, GPIOADAT);//enable speak control mute
//    }
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    return key map address to application
 * \param[in]    none
 * \param[out]   unsigned int
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 返回按键影射地址给应用
 *******************************************************************************/
uint8 *key_inner_getkeytabaddress(void *null0, void *null1, void *null2)
{
//    return g_key_map_tab;
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    charge flow deal
 * \param[in]    none
 * \param[in]    none
 * \param[in]    none
 * \param[out]   uint8
 * \return       the result
 * \retval           0 no lock
 * \retval           1 lock
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 负责充电处理，这个会被定时器轮询的。
 *******************************************************************************/
uint8 key_inner_holdcheck(void *null0, void *null1, void *null2)
{
//    unsigned char key_val;
//    unsigned char ret;
//
//    key_val = act_readb(LRADC1_DATA) & 0x3f;
//    if ((key_val < MIN_HOLD_ADC) || (key_val > MAX_HOLD_ADC))
//    {
//        ret = 0; //unhold
//    }
//    else
//    {
//        ret = 1; //hold
//    }

//    return ret;
    return 0;
}


int32 key_inner_keytone_play_init(void *null0, void *null1, void *null2)
{
    return 0;
}


int32 key_inner_keytone_play_exit(void *null0, void *null1, void *null2)
{
    return 0;
}

int32 key_inner_keytone_play_frame(void *null0, void *null1, void *null2)
{
    return 0;
}