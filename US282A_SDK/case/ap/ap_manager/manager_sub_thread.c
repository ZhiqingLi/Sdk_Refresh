/*******************************************************************************
 *                              US212A
 *                            Module: manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_main.c
 * \brief    manager的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"

void open_pa(void)
{
    g_app_info_state.aout_mode = (aout_type_e) com_get_config_default(SETTING_AUDIO_OUTPUT_CHANNAL);

#if (SPEAKER_HEADPHONE_FIX == 1)

    g_app_info_state.hp_state = HEADPHONE_STATE_IN;
    g_app_info_state.aout_mode = AO_SOURCE_DAC;

#else

#if (SUPPORT_HP_DETECT == DETECT_BY_GPIO)
#if (HP_GPIO_EN_LEVEL == ENABLE_LEVEL_LOW)
    if ((act_readl(HP_GPIO_DATA) & HP_GPIO_PIN) == 0)
    {
        g_app_info_state.hp_state = HEADPHONE_STATE_IN;
        g_app_info_state.aout_mode = AO_SOURCE_DAC;
    }
#else
    if ((act_readl(HP_GPIO_DATA) & HP_GPIO_PIN) != 0)
    {
        g_app_info_state.hp_state = HEADPHONE_STATE_IN;
        g_app_info_state.aout_mode = AO_SOURCE_DAC;
    }
#endif
#endif

#endif

    //安装audio驱动
    sys_drv_install(DRV_GROUP_AUDIO_DEVICE, 0, "aud_dev.drv");

    SPEAKER_CONTROL_ENABLE();

    //打开PA，PA开机后常开
    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        uint8 pa_mode = (uint8) com_get_config_default(SETTING_AUDIO_INNER_PA_SET_MODE);
        enable_pa(pa_mode, 0, AO_SOURCE_DAC);
    }
    else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
    {
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
        //DEMO IC:A0,A1,A2,VRO_S
        enable_pa(3, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
        //DVB:B7,B8,B9,A6
        enable_pa(4, 0, AO_SOURCE_I2S);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823)
        //DVB:AOUTL,AOUTR,VRO,VROS
        enable_pa(0, 0, AO_SOURCE_I2S);
#else
        //DEMO IC:A0,A1,A2,A3
        enable_pa(1, 0, AO_SOURCE_I2S);
#endif
    }
    else
    {
        ;//nothing
    }

    SPEAKER_ON();
    
#if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
    sys_avoid_linein_noise(1);
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823)
    sys_avoid_linein_noise(0);
#endif     

    g_app_info_state.inner_pa_inited = TRUE;

    /*注意：libc_pthread_exit无法返回*/
    libc_pthread_exit();
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    打开音频文件扫描,创建线程
 * \param[in]   param
 * \return       操作句柄
 * \retval           others sucess
 * \retval           NULL failed
 *******************************************************************************/
void * pa_thread_open(void* param)
{
    pthread_ext_param_t g_pa_thread;

    //初始化创建线程的参数
    g_pa_thread.pthread_param.start_rtn = (void *)open_pa;
    g_pa_thread.pthread_param.arg = param;
    //g_pa_thread.pthread_param.ptos = (void *) AP_FRONT_HIGH_STK_POS;
    pa_thread_task_addr = sys_malloc_large_data(AP_FRONT_HIGH_STK_SIZE);
    g_pa_thread.pthread_param.ptos = (void *)(pa_thread_task_addr + AP_FRONT_HIGH_STK_SIZE);
    g_pa_thread.stk_size = AP_FRONT_HIGH_STK_SIZE;

    libc_print("pa thread addr:", pa_thread_task_addr, 2);

    //禁止调度
    sys_os_sched_lock();

    //创建PA初始化线程
    if (libc_pthread_create(&g_pa_thread, AP_FRONT_HIGH_PRIO, CREATE_NOT_MAIN_THREAD) < 0)
    {
        sys_os_sched_unlock();
        return NULL;
    }
    sys_os_sched_unlock();

    return NULL;
}
