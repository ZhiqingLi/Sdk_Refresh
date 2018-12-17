/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：外挂喇叭控制模块，包括使能GPIO OUT控制，打开喇叭，关闭喇叭，以及获取喇叭
 *       状态等。
 * 作者：cailizhen
 ********************************************************************************/

#include "case_independent.h"
#include "applib.h"
#include "config_id.h"
#include "ccd_interface.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    使能 SPEAKER 开关控制
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_CONTROL_ENABLE(void)
{
    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            act_writel(act_readl(SPEAKER_A_PA_EN_GPIOOUT) | SPEAKER_A_PA_EN_PIN, SPEAKER_A_PA_EN_GPIOOUT);
            act_writel(act_readl(SPEAKER_A_PA_BST_EN_GPIOOUT) | SPEAKER_A_PA_BST_EN_PIN, SPEAKER_A_PA_BST_EN_GPIOOUT);
            act_writel(act_readl(SPEAKER_A_PA_D_AB_GPIOOUT) | SPEAKER_A_PA_D_AB_PIN, SPEAKER_A_PA_D_AB_GPIOOUT);

            #if (SPEAKER_A_PA_BST_EN_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_BST_EN_GPIODATA) | SPEAKER_A_PA_BST_EN_PIN),
                    SPEAKER_A_PA_BST_EN_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_BST_EN_GPIODATA) & (~SPEAKER_A_PA_BST_EN_PIN)),
                    SPEAKER_A_PA_BST_EN_GPIODATA);
            #endif

            #if (SPEAKER_A_PA_D_AB_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) | SPEAKER_A_PA_D_AB_PIN), SPEAKER_A_PA_D_AB_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) & (~SPEAKER_A_PA_D_AB_PIN)), SPEAKER_A_PA_D_AB_GPIODATA);
            #endif
        }
    }
    else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
    {
        //install extern speaker driver 安装驱动时使能控制
        sys_drv_install(DRV_GROUP_CCD, 0, "ccd.drv");
    }
    else
    {
        ; //nothing
    }

    g_app_info_state.outer_pa_on = FALSE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    关闭 SPEAKER，断电
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_CONTROL_DISABLE(void)
{
    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            #if (SPEAKER_A_PA_EN_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) & (~SPEAKER_A_PA_EN_PIN)), SPEAKER_A_PA_EN_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) | SPEAKER_A_PA_EN_PIN), SPEAKER_A_PA_EN_GPIODATA);
            #endif

            sys_os_time_dly(5);

            #if (SPEAKER_A_PA_BST_EN_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_BST_EN_GPIODATA) & (~SPEAKER_A_PA_BST_EN_PIN)),
                    SPEAKER_A_PA_BST_EN_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_BST_EN_GPIODATA) | SPEAKER_A_PA_BST_EN_PIN),
                    SPEAKER_A_PA_BST_EN_GPIODATA);
            #endif

            act_writel(act_readl(SPEAKER_A_PA_EN_GPIOOUT) & (~SPEAKER_A_PA_EN_PIN), SPEAKER_A_PA_EN_GPIOOUT);
            act_writel(act_readl(SPEAKER_A_PA_BST_EN_GPIOOUT) & (~SPEAKER_A_PA_BST_EN_PIN),
                    SPEAKER_A_PA_BST_EN_GPIOOUT);
            act_writel(act_readl(SPEAKER_A_PA_D_AB_GPIOOUT) & (~SPEAKER_A_PA_D_AB_PIN), SPEAKER_A_PA_D_AB_GPIOOUT);
        }
    }
    else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
    {
        // unstall extern speaker driver 卸载驱动时断电
        sys_drv_uninstall(DRV_GROUP_CCD);
    }
    else
    {
        ; //nothing
    }

}

/******************************************************************************/
/*!
 * \par  Description:
 *    打开 SPEAKER
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_ON(void)
{
    if (g_app_info_state.outer_pa_on == TRUE)
    {
        return ;
    }

    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            #if (SPEAKER_A_PA_EN_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) | SPEAKER_A_PA_EN_PIN), SPEAKER_A_PA_EN_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) & (~SPEAKER_A_PA_EN_PIN)), SPEAKER_A_PA_EN_GPIODATA);
            #endif

            //外部PA上电BYPASS延时
            sys_os_time_dly(SPEAKER_A_PA_BYPASS_TIME);
        }
    }
    else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
    {
        ccd_i2s_pa_poweron();
    }
    else
    {
        ; //nothing
    }

    g_app_info_state.outer_pa_on = TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    关闭 SPEAKER
 * \param[in]    none
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_OFF(void)
{
    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            #if (SPEAKER_A_PA_EN_LEVEL == ENABLE_LEVEL_HIGH)
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) & (~SPEAKER_A_PA_EN_PIN)), SPEAKER_A_PA_EN_GPIODATA);
            #else
            act_writel((act_readl(SPEAKER_A_PA_EN_GPIODATA) | SPEAKER_A_PA_EN_PIN), SPEAKER_A_PA_EN_GPIODATA);
            #endif
        }
    }
    else if (g_app_info_state.aout_mode == AO_SOURCE_I2S)
    {
        if (g_app_info_state.outer_pa_on == TRUE)
        {
            ccd_i2s_pa_powerdown();
        }
    }
    else
    {
        ; //nothing
    }

    g_app_info_state.outer_pa_on = FALSE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    SPEAKER 是否开着
 * \param[in]    none
 * \param[out]   none
 * \return       bool    TRUE表示开，FALSE表示关
 *******************************************************************************/
bool IS_SPEAKER_ON(void)
{
    return g_app_info_state.outer_pa_on;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    SPEAKER D/AB模式切换
 * \param[in]    type 0表示D类功放，1表示AB类功放
 * \param[out]   none
 * \return       none
 *******************************************************************************/
void SPEAKER_D_AB_SWITCH(uint8 type)
{
    if (g_app_info_state.aout_mode == AO_SOURCE_DAC)
    {
        if (get_headphone_state() == HEADPHONE_STATE_NULL)
        {
            #if (SPEAKER_A_PA_D_AB_LEVEL == ENABLE_LEVEL_HIGH)
            if (type == SPEAKER_A_PA_TYPE_D)
            {
                act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) | SPEAKER_A_PA_D_AB_PIN), SPEAKER_A_PA_D_AB_GPIODATA);
            }
            else
            {
                act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) & (~SPEAKER_A_PA_D_AB_PIN)),
                        SPEAKER_A_PA_D_AB_GPIODATA);
            }
            #else
            if (type == SPEAKER_A_PA_TYPE_D)
            {
                act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) & (~SPEAKER_A_PA_D_AB_PIN)),
                        SPEAKER_A_PA_D_AB_GPIODATA);
            }
            else
            {
                act_writel((act_readl(SPEAKER_A_PA_D_AB_GPIODATA) | SPEAKER_A_PA_D_AB_PIN), SPEAKER_A_PA_D_AB_GPIODATA);
            }
            #endif
        }
    }
}
