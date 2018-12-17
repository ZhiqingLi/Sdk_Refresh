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
#include  <card.h>
#include  <sysdef.h>

//#define FT_MODE
OS_STK *ptos = (OS_STK *) AP_PROCESS_MANAGER_STK_POS;
INT8U prio = AP_PROCESS_MANAGER_PRIO;
test_share_info_t *g_p_test_share_info;

mem_use_info_t *g_mem_use_info;

uint8 prev_back_ap_id;

uint8 prev_front_ap_id;

uint8 *pa_thread_task_addr;

uint8 support_dev_num;

extern app_result_e manager_message_loop(void)__FAR__;
extern bool globe_data_init(void) __FAR__;

void system_config(void);

typedef void (*handler_ker)(void);

/******************************************************************************/
/*!
 * \par  Description:
 *  读取应用VM全局变量,初始化全局变量
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
static void _read_var(void)
{
    g_app_info_state_all.fix_sample_rate_flag = (uint8)com_get_config_default(SETTING_AUDIO_SAMPLE_RATE_FIXED);
    if(com_get_config_default(SETTING_AUDIO_POW_SAVING_MODE) != 0)
    {
        GLOBAL_KERNEL_BACKDOOR_FLAG |= BACKDOOR_AUDIO_POW_SAVING;
    }
    else
    {
        GLOBAL_KERNEL_BACKDOOR_FLAG &= (~BACKDOOR_AUDIO_POW_SAVING);
    }
}

#ifdef FT_MODE
void delay_ms(uint8 dly_ms)
{
    uint8 i, j;
    while (dly_ms != 0)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 255; j++)
            {
                ; //do nothing
            }
        }
        dly_ms--;
    }
}
#endif

/******************************************************************************/
/*!
 * \par  Description:
 *    应用的入口函数
 * \param[in]    未使用
 * \param[out]   none
 * \return       永远不会退出
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
int main(int argc, const char *argv[])
{
    uint8 dc5v_flag = 0;

    system_config();

    //初始化globe全局数据，并加载配置脚本 config.bin
    globe_data_init_applib();
    
    //初始化应用变量
    _read_var();
    
    ft_test_mode();

#ifdef FT_MODE
        //act_writel((act_readl(HOSC_CTL) & 0xffff0000), HOSC_CTL);        
        act_writel((act_readl(AD_Select1) & (~AD_Select1_GPIOA22_MASK)), AD_Select1);

        delay_ms(20);
        //act_writel((act_readl(HOSC_CTL) | 0x4646), HOSC_CTL);
        delay_ms(10);
        act_writel(act_readl(0xc01b0000) | 0x00000014, 0xc01b0000); //ft test mode
        delay_ms(20);  
        act_writel((act_readl(WD_CTL) & (~0x10)), WD_CTL);
        libc_print("FT MODE",0,0);
        while(1)
        {
          ; 
        }
#else 
  
    PRINT_INFO_INT("WAKE_TYPE:", argc);

    if ((act_readl(CHG_DET) & (1 << CHG_DET_UVLO)) != 0)
    {
        dc5v_flag = 1;
    }

    //config_hosc_freq();
    //config pcmram1 to cpu
    //reg_writel((act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_PCMRAM1CLKSEL_MASK)), CMU_MEMCLKSEL);
    //config asrc buffer 
    reg_writel((act_readl(CMU_MEMCLKEN) | (1<<CMU_MEMCLKEN_ASRCBUF1CLKEN)), CMU_MEMCLKEN);
    reg_writel((act_readl(CMU_MEMCLKSEL) & (~(1<<CMU_MEMCLKSEL_ASRCBUF1CLKSEL))), CMU_MEMCLKSEL);
   
    install_key_drv();

    adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO, FREQ_LEVEL8, FREQ_NULL);//加快开机速度

#ifdef WAVES_ASET_TOOLS
    //如果使用WAVES公司算法，DSP频率锁定为180Mhz，暂不考虑功耗

    //设置系统频率
    sys_adjust_clk(85 | (180 << 8), 0);
        
    //锁定频率为：MIPS -> 85M, DSP -> 180
    sys_lock_adjust_freq(85 | (180 << 8));
    
    g_app_info_state_all.stub_pc_tools_type = STUB_PC_TOOL_UNKOWN;
    g_app_info_state_all.bin_number = (uint8) com_get_config_default(SETTING_APP_WAVES_BIN_NUMBER);
    g_app_info_state_all.waves_dae_para_update_flag = 0;
        
#endif

    pa_thread_open(NULL);

    //初始化applib库，前台AP
    applib_init(APP_ID_MANAGER, APP_TYPE_PMNG);

    //初始化 applib 消息模块
    sys_mq_flush(MQ_ID_MNG);

    //sys_exec_ap为字符串参数接口，rodata数据要注意不被刷掉
    //所以要先锁掉度
    sys_os_sched_lock();
    
    //创建config应用
    if((argc != 0) || (dc5v_flag == 1))
    {
        sys_exece_ap("config.ap", 0, 0);    //软件开机、唤醒
    }
    else
    {
        sys_exece_ap("config.ap", 0, 5);    //上电开机
    }

    sys_os_sched_unlock();

    manager_message_loop();

    return 0;
#endif    
}

#if 0
/******************************************************************************/
/*!
 * \par  Description:
 *  设置NOR驱动能力
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
uint32 nor_set_drv_power(uint8 level)
{
    uint32 tmp_pad_drv;

    switch(level)
    {
        case 1:
        tmp_pad_drv = 0x00;
        break;

        case 2:
        tmp_pad_drv = 0x55;
        break;

        case 3:
        tmp_pad_drv = 0xaa;
        break;

        case 4:
        tmp_pad_drv = 0xff;
        break;

        //the default value
        default:
        tmp_pad_drv = 0x55;
        break;
    }

    act_writel(act_readl(PAD_DRV2) & (~0xff), PAD_DRV2);
    act_writel(act_readl(PAD_DRV2) | tmp_pad_drv, PAD_DRV2);
    //act_setl(tmp_pad_drv, PAD_DRV2);
    return 1;
}
#endif
/******************************************************************************/
/*!
 * \par  Description:
 *    system config
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
void system_config(void)
{
    /*for card configuration.  xiaomaky.  2013/2/19 13:49:32.*/
    /*
     bit0: 0--1_line, 1--4_line;
     bit1(sdvccout power): 0--direct, 1--GPIO;
     bit2(DAT3 pull high): 0--hardware pull, 1--GPIO pull;
     bit3(write_fast):0--not continuous, 1--continuous write;
     */


//    handler_ker detect_card_isr_ker = (handler_ker) GLOBAL_DETECT_CARD_ISR_ADDR;//指向kernel固化接口 detect_card_isr

    /* bit1----if is 0(default), if exception, will reset; if 1, not reset.  */
    /* bit2----if is 0(default), card-gpio-detec; if 1, card-cmd-detect for sys_detect_disk().
            must not to request timer irq to detect card.  */
    /* bit3----only for US280A, if is 0(default), disable isr_card-cmd-detec; if 1, enable isr_card-cmd-detec.*/
    // GLOBAL_KERNEL_BACKDOOR_FLAG |= 0x06;

    // GLOBAL_KERNEL_DETECT_CARD_IN_HANDER = GLOBAL_KSD_DETECT_CARD_ISR_ADDR;

    g_card_para = 0x00;

    g_card_type = 0;

    /* for GPIOA19 card-detect.  need to enable PU as soon for be stable.   */
    //act_writel((~(0x01 << 19)) & act_readl(GPIO_AOUTEN), GPIO_AOUTEN);  /* disable output.  */
    //act_writel((0x01 << 19) | act_readl(GPIO_AINEN), GPIO_AINEN);       /* enable input.  */
    //act_writel((~(0x01 << 19)) & act_readl(GPIO_APDEN), GPIO_APDEN);    /* disable 50K PD.  */
    //act_writel((0x01 << 19) | act_readl(GPIO_AOUTEN), GPIO_APUEN);      /* 50K PU Enable, 这样没有卡时默认为高.  */

    // sys_set_irq_timer1(detect_card_isr_ker, 10);

    //因中断检测时间间隔太长，config需要过滤卡插入消息，主动调一次卡检测函数
    //  detect_card_isr_ker();

    //屏蔽rtc中断检测卡插入
    //GLOBAL_KERNEL_BACKDOOR_FLAG |= 0x08;

    //默认设置检测usb线是否插入
    //sys_set_usb_detect_mode(1);
    DISABLE_UHOST_DETECT();
    ENABLE_USB_DETECT();

    /* for uart-gpio, 0x9fc19a88--uart_tx_gpio_cfg_mask,  0x9fc19a8c--uart_tx_gpio_cfg_value.  */
    //GLOBAL_UART_TX_GPIO_CFG_MASK = 0xFFFF1FFF;//GPIO A21
    //GLOBAL_UART_TX_GPIO_CFG_VALUE = 0x00006000; //GPIO A21
    //GLOBAL_UART_TX_GPIO_CFG_MASK = 0xFFFFFFC7;//GPIO A17
    //GLOBAL_UART_TX_GPIO_CFG_VALUE = 0x00000020; //GPIO A17

    /*系统调频时，不再调整VCC电压，系统初始化为3.1V，应用可以根据需求，
     将其调整到合适档位，比如，为了EJTAG调试，设为3.3V.*/
    //根据哈曼案子需求，将VDD电压调整到3.2V
    act_writel((0xFFFFFF8F & act_readl(VOUT_CTL)) | 0x50, VOUT_CTL);

    /*for EMI setting.  */
    //    act_writel(0x5c92 | (0xffff0000 & act_readl(MCUPLL_DEBUG)), MCUPLL_DEBUG);   /* 0x5c12\0x5c92\0x5d12\0x5d92.*/
    //    /*disable MCUPLL-EMI setting.  */
    //    act_writel(act_readl(MCUPLL_DEBUG)&0xffffbfff, MCUPLL_DEBUG);
    //    /* bit0----if is 1, for pass EMI, not change nand_pad_drv according to frequency;   */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x01;
    //    /* bit4----if is 1, for pass EMI, Nand/Card clock is fixed HOSC, not according to MCUPLL;    */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x10;
    //    /* bit5----if is 1, for pass EMI, UDisk AP set clock and USB LIB not;  */
    //    GLOBAL_KERNEL_BACKDOOR_FLAG = GLOBAL_KERNEL_BACKDOOR_FLAG | 0x20;
    //    设置NOR驱动能力
    //    nor_set_drv_power(3);
}

