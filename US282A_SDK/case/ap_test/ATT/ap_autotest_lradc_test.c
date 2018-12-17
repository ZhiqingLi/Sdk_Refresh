/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     music_channel_test.c
 * \brief    通道测试实现模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"

/*
 * 根据不同转接板 ADC 不同电阻不同分压来获取不同设备号.
 */
uint32 get_adc_data(uint32 adc_num)
{
    uint32  adc_data;
    uint32 pmuadc_ctl_bak;
    uint32 gpiob0_ctl_bak;
    uint32 ad_select1_bak;

    pmuadc_ctl_bak = act_readl(PMUADC_CTL);
    gpiob0_ctl_bak = act_readl(GPIOB0_CTL);
    ad_select1_bak = act_readl(AD_Select1);

    //select PMUADC_CTL_LRADC_FS 250khz
    act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_BATADC_FS)), PMUADC_CTL); 

    if(adc_num == 1)
    {    
        //enable LRADC1
        act_writel((act_readl(PMUADC_CTL) | (1 << PMUADC_CTL_LRADC1_EN)), PMUADC_CTL);
        
        //mfp switch
        act_writel((act_readl(GPIOB0_CTL) & (~GPIOB0_CTL_GPIOB0_MFP_MASK)), GPIOB0_CTL);

         //rtc register
         act_writel(0xA596, RTC_REGUPDATA);
         while (act_readl(RTC_REGUPDATA) != 0x5A69)
         {
             ;//wait for register update
         }       

         //b0 as lradc1
        act_writel((act_readl(GPIOB0_CTL) | (1<<GPIOB0_CTL_GPIOB0_MFP_SHIFT)), GPIOB0_CTL); 

        //rtc register
        act_writel(0xA596, RTC_REGUPDATA);
        while (act_readl(RTC_REGUPDATA) != 0x5A69)
        {
            ;//wait for register update
        }   

        sys_mdelay(50);

        /* 获取 ADC 分压值 */
        adc_data = act_readl(LRADC1_DATA) & 0x7F;  

        act_writel(pmuadc_ctl_bak, PMUADC_CTL);

        act_writel(gpiob0_ctl_bak, GPIOB0_CTL);

        act_writel(ad_select1_bak, AD_Select1);
        
        //rtc register
        act_writel(0xA596, RTC_REGUPDATA);
        while (act_readl(RTC_REGUPDATA) != 0x5A69)
        {
            ;//wait for register update
        }          
    }
    else if(adc_num == 2)
    {
        act_writel(act_readl(PMUADC_CTL) | (1 << 5), PMUADC_CTL); // LRADC234_EN  

        act_writel((act_readl(AD_Select1) & (~AD_Select1_GPIOA22_MASK)), AD_Select1);
        
        act_writel((act_readl(AD_Select1) | (0x01 << AD_Select1_GPIOA22_SHIFT)), AD_Select1);

        sys_mdelay(50);

        /* 获取 ADC 分压值 */
        adc_data = act_readl(LRADC2_DATA) & 0x7F;   

        act_writel(pmuadc_ctl_bak, PMUADC_CTL);

        act_writel(gpiob0_ctl_bak, GPIOB0_CTL);

        act_writel(ad_select1_bak, AD_Select1);        
    }
    else if(adc_num == 4)
    {  
        act_writel(act_readl(PMUADC_CTL) | (1 << 5), PMUADC_CTL); // LRADC234_EN
           
        act_writel((act_readl(AD_Select1) & (~(1 << AD_Select1_GPIOA2))), AD_Select1);
        
        act_writel((act_readl(AD_Select1) | (0x01 << AD_Select1_GPIOA2)), AD_Select1); 

        sys_mdelay(50);

        /* 获取 ADC 分压值 */
        adc_data = act_readl(LRADC4_DATA) & 0x7F;   

        act_writel(pmuadc_ctl_bak, PMUADC_CTL);

        act_writel(gpiob0_ctl_bak, GPIOB0_CTL);

        act_writel(ad_select1_bak, AD_Select1);       
    }
    else
    {
        return 0;
    }

    //libc_print("adc data", adc_data, 2);   

    return adc_data;
}


void act_test_report_lradc_result(uint16 test_id, int32 ret_val)
{
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = test_id;

        return_data->test_result = ret_val;

        act_test_report_result(return_data, 4);
    }
    else
    {
        act_test_report_test_log(ret_val, test_id);
    }
}

test_result_e act_test_lradc_test(void *arg_buffer)
{
    int32 result;
    uint32 adc_data;
    
    lradc_test_arg_t *lradc_arg = (lradc_test_arg_t *)arg_buffer;

    result = TRUE;
    
    if(lradc_arg->lradc1_test == TRUE)
    {
        adc_data =  get_adc_data(1);

        if(g_test_mode == TEST_MODE_CARD)
        {
            att_write_test_info("lradc1 data: ", adc_data, 1);
        }
        else
        {
            print_log("lradc1 data: %d", adc_data);
        }
        
        if(adc_data >= lradc_arg->lradc1_thr_low 
            && adc_data <= lradc_arg->lradc1_thr_high)
        {
            result = TRUE;
        }
        else
        {
            result = FALSE;
            goto test_end;
        }
    }

    if(lradc_arg->lradc2_test == TRUE)
    {
        adc_data =  get_adc_data(2);

        if(g_test_mode == TEST_MODE_CARD)
        {
            att_write_test_info("lradc2 data: ", adc_data, 1);
        }
        else
        {
            print_log("lradc2 data: %d", adc_data);
        }


        if(adc_data >= lradc_arg->lradc2_thr_low 
            && adc_data <= lradc_arg->lradc2_thr_high)
        {
            result = TRUE;
        }
        else
        {
            result = FALSE;
            goto test_end;
        }
    }

    if(lradc_arg->lradc4_test == TRUE)
    {
        adc_data =  get_adc_data(4);

        if(g_test_mode == TEST_MODE_CARD)
        {
            att_write_test_info("lradc4 data: ", adc_data, 1);
        }
        else
        {
            print_log("lradc4 data: %d", adc_data);
        }


        if(adc_data >= lradc_arg->lradc4_thr_low 
            && adc_data <= lradc_arg->lradc4_thr_high)
        {
            result = TRUE;
        }
        else
        {
            result = FALSE;
            goto test_end;
        }
    }

test_end:    
    act_test_report_lradc_result(TESTID_LRADC_TEST, result);

    return result;
}

