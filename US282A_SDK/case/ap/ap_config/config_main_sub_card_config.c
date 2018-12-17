/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main_sub_card_config.c
 * \brief    开关机的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "ap_config.h"

#define CARD_FREQ_MAX           50
#define CARD_PADDRV_LEVEL_MAX   7
#define CARD_HOSC_SELECT        26

void card_set_drv_power(void)
{
    card_pm_cfg_t card_pm_param;

    card_pm_param.cmd_drv_level = (uint8)com_get_config_default(SETTING_CARD_CMD_PAD_LEVEL);

    if(card_pm_param.cmd_drv_level > CARD_PADDRV_LEVEL_MAX)
    {
        card_pm_param.cmd_drv_level = CARD_PADDRV_LEVEL_MAX;
    }

    card_pm_param.clk_drv_level = (uint8)com_get_config_default(SETTING_CARD_CLK_PAD_LEVEL);

    if(card_pm_param.clk_drv_level > CARD_PADDRV_LEVEL_MAX)
    {
        card_pm_param.clk_drv_level = CARD_PADDRV_LEVEL_MAX;
    }

    card_pm_param.data_drv_level = (uint8)com_get_config_default(SETTING_CARD_DAT_PAD_LEVEL);

    if(card_pm_param.data_drv_level > CARD_PADDRV_LEVEL_MAX)
    {
        card_pm_param.data_drv_level = CARD_PADDRV_LEVEL_MAX;
    }

    card_pm_param.sd_clk_init = (uint8)com_get_config_default(SETTING_CARD_INT_CLK);

    if(card_pm_param.sd_clk_init == 0)
    {
        card_pm_param.sd_clk_init = SDCLKCTL_HOSC_DIV2;
    }
    else
    {
        card_pm_param.sd_clk_init = SDCLKCTL_HOSC_DIV1;
    }

    card_pm_param.sd_clk_rw = (uint8)com_get_config_default(SETTING_CARD_RW_CLK);

    if(card_pm_param.sd_clk_rw == 0)
    {
        card_pm_param.sd_clk_rw = SDCLKCTL_HOSC_DIV2;
        card_pm_param.mcupll_use = 0;
    }
    else if(card_pm_param.sd_clk_rw == 1)
    {
        card_pm_param.sd_clk_rw = SDCLKCTL_HOSC_DIV2;
        card_pm_param.mcupll_use = 0;
    }
    else
    {
        //有可能出现卡驱动装载的时候,MCUPLL还未启动
        //此时需要设置一个有效的时钟设置，因此这里默认
        //设置一个有效的时钟值
        card_pm_param.sd_clk_rw = SDCLKCTL_HOSC_DIV2;
        //当设置mcupll_use为1时，卡驱动可根据系统mcupll
        //自适应到不大于mcupll_max_freq的频率
        card_pm_param.mcupll_use = 1;
    }

    card_pm_param.mcupll_max_freq = (uint8)com_get_config_default(SETTING_CARD_MAX_CLK);
    
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    card_pm_param.card_gpio_set = 1;
#else
    card_pm_param.card_gpio_set = 0;    
#endif        

//    //选择mcupll作为时钟源，频率范围为9M-42M
//    if(card_pm_param.mcupll_max_freq < CARD_HOSC_SELECT)
//    {
//        card_pm_param.mcupll_max_freq = CARD_HOSC_SELECT;
//    }
//    else if(card_pm_param.mcupll_max_freq > CARD_FREQ_MAX)
//    {
//        card_pm_param.mcupll_max_freq = CARD_FREQ_MAX;
//    }
//    else
//    {
//        ;//nothing for QAC
//    }

    //设置卡驱动相关配置参数
    sys_set_drv_setting(&card_pm_param);

//    PRINTD_INFO("set card pad");
//    PRINTD_DATA(&card_pm_param,sizeof(card_pm_cfg_t));

}
