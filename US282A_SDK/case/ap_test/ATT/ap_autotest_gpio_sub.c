/*******************************************************************************
 *                              US212A
 *                            Module: Picture
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-19 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     user1_main.c
 * \brief    picture主模块，负责进程初始化，退出处理，场景调度
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"
#include "ap_autotest_gpio.h"

void led_flash_ok(void)
{
    uint8 i;
    uint8 led_num = 8;
    uint32 loop_cnt = 8;

    DISABLE_WATCH_DOG;

    //A0默认输出高电平，IO口接下拉电阻
    act_writel(act_readl(GPIOAOUTEN) | (1 << 0), GPIOAOUTEN);

    act_writel(act_readl(GPIOADAT) | (1 << 0), GPIOADAT);

    for(i = 0; i < 8; i++)
    {
        act_writel(act_readl(GPIOAOUTEN) | (1 << led_num), GPIOAOUTEN);
        led_num++;
    }

    led_num = 8;

    // 2823 芯片使用 A21 / VRO_S
    act_writel(act_readl(GPIOAOUTEN) | (1 << 21), GPIOAOUTEN);
    act_writel(act_readl(SIO_OUTEN) | (1 << 9), SIO_OUTEN);    

    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            act_writel(act_readl(GPIOADAT) | (1 << led_num), GPIOADAT);

            sys_mdelay(20);

            led_num++;
        }

        act_writel(act_readl(GPIOADAT) | (1 << 21), GPIOADAT);
        act_writel(act_readl(SIO_DAT) | (1 << 9), SIO_DAT);        

        sys_mdelay(20);

        for(i = 0; i < 8; i++)
        {
            led_num--;

            act_writel(act_readl(GPIOADAT) & (~ (1 << led_num)), GPIOADAT);

            sys_mdelay(20);
        }

        act_writel(act_readl(GPIOADAT) & (~(1 << 21)), GPIOADAT);
        act_writel(act_readl(SIO_DAT) & (~(1 << 9)), SIO_DAT);        

        loop_cnt--;
    }
}

void led_flash_fail(void)
{
    uint8 i;
    uint8 led_num = 8;
    uint32 loop_cnt = 8;

    DISABLE_WATCH_DOG;

    //A0默认输出高电平，IO口接下拉电阻
    act_writel(act_readl(GPIOAOUTEN) | (1 << 0), GPIOAOUTEN);

    act_writel(act_readl(GPIOADAT) | (1 << 0), GPIOADAT);

    for(i = 0; i < 8; i++)
    {
        act_writel(act_readl(GPIOAOUTEN) | (1 << led_num), GPIOAOUTEN);
        led_num++;
    }

    // 2823 芯片使用 A21 / VRO_S
    act_writel(act_readl(GPIOAOUTEN) | (1 << 21), GPIOAOUTEN);
    act_writel(act_readl(SIO_OUTEN) | (1 << 9), SIO_OUTEN);

    led_num = 8;

    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            act_writel(act_readl(GPIOADAT) | (1 << led_num), GPIOADAT);

            led_num++;
        }
        act_writel(act_readl(GPIOADAT) | (1 << 21), GPIOADAT);
        act_writel(act_readl(SIO_DAT) | (1 << 9), SIO_DAT);

        sys_mdelay(100);

        for(i = 0; i < 8; i++)
        {
            led_num--;

            act_writel(act_readl(GPIOADAT) & (~ (1 << led_num)), GPIOADAT);

        }
        act_writel(act_readl(GPIOADAT) & (~(1 << 21)), GPIOADAT);
        act_writel(act_readl(SIO_DAT) & (~(1 << 9)), SIO_DAT);

        sys_mdelay(100);
    }
}

void act_test_report_gpio_result(gpio_test_arg_t *gpio_test_arg, test_result_e result, uint32 test_id)
{
    int ret_val;
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    DEBUG_ATT_PRINT("gpio test result", result, 2);

    if (result == TEST_PASS)
    {
        ret_val = 1;
        att_write_test_info("gpio test ok", 0, 0);
    }
    else
    {
        ret_val = 0;
        att_write_test_info("gpio test failed", 0, 0);
    }

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = test_id;

        return_data->test_result = ret_val;

        uint32_to_unicode(gpio_test_arg->gpioA_value, return_data->return_arg, &trans_bytes, 16);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        uint32_to_unicode(gpio_test_arg->gpioB_value, &(return_data->return_arg[trans_bytes]), &trans_bytes, 16);

        //添加参数分隔符','
        return_data->return_arg[trans_bytes++] = 0x002c;

        uint32_to_unicode(gpio_test_arg->gpioSIO_value, &(return_data->return_arg[trans_bytes]), &trans_bytes, 16);        

        //添加结束符
        return_data->return_arg[trans_bytes++] = 0x0000;

        //如果参数未四字节对齐，要四字节对齐处理
        if ((trans_bytes % 2) != 0)
        {
            return_data->return_arg[trans_bytes++] = 0x0000;
        }

        act_test_report_result(return_data, trans_bytes * 2 + 4);
    }
    else
    {
        act_test_report_test_log(ret_val, test_id);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *	  GPIO测试函数
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music_test
 * \note
 *******************************************************************************/
test_result_e test_gpio(gpio_test_arg_t *gpio_test_arg, int32 *index)
{
    test_result_e test_result;

    // uint32 gpioA_out_bak = act_readl(GPIOAOUTEN);
    // uint32 gpioB_out_bak = act_readl(GPIOBOUTEN);
    // uint32 gpioA_in_bak = act_readl(GPIOAINEN);
    // uint32 gpioB_in_bak = act_readl(GPIOBINEN);
    // uint32 gpioA_pu_bak = act_readl(GPIOAPUEN);
    // uint32 gpioB_pu_bak = act_readl(GPIOBPUEN);
    // uint32 gpioA_pd_bak = act_readl(GPIOAPDEN);
    // uint32 gpioB_pd_bak = act_readl(GPIOBPDEN);
    // uint32 gpioA_dat_bak = act_readl(GPIOADAT);
    // uint32 gpioB_dat_bak = act_readl(GPIOBDAT);

    uint32 gpio_bak_array[15];
    uint32 gpio_reg;
    uint8 i;

    gpio_reg = GPIOAOUTEN;
    for (i = 0; i < 15; i++, gpio_reg += 4)
    {
        gpio_bak_array[i] = act_readl(gpio_reg);
    }

    //MFP选择SIO7
    act_writel((act_readl(MFP_CTL3) & (~MFP_CTL3_VRO_MASK) | (2 << MFP_CTL3_VRO_SHIFT)), MFP_CTL3);

    act_writel((act_readl(MFP_CTL3) & (~MFP_CTL3_VROS_MASK) | (2 << MFP_CTL3_VROS_SHIFT)), MFP_CTL3);

    //VRO与SIO7复用，需关闭VRO
    act_writel((act_readl(DAC_ANACTL) & (~(1 << DAC_ANACTL_OPVROEN))), DAC_ANACTL);

    //disable JTAG
    act_writel(act_readl(MFP_CTL2) & (~0xc0000000), MFP_CTL2);

    //测试GPIO是否存在短路
    test_result = test_gpio_short(gpio_test_arg->gpioA_value, GPIO_REGA_BASE, gpio_test_arg->gpioB_value,
            GPIO_REGB_BASE, gpio_test_arg->gpioSIO_value, GPIO_REGSIO_BASE, index);

    if (test_result != TEST_PASS)
    {
        goto end_test;
    }

    //测试GPIO是否存在短路到GND
    test_result = test_gpio_short_gnd(gpio_test_arg->gpioA_value, GPIO_REGA_BASE, gpio_test_arg->gpioB_value,
            GPIO_REGB_BASE, gpio_test_arg->gpioSIO_value, GPIO_REGSIO_BASE, index);

    if (test_result != TEST_PASS)
    {
        goto end_test;
    }

    //测试GPIO是否存在断路
    test_result = test_gpio_open(gpio_test_arg->gpioA_value, GPIO_REGA_BASE, gpio_test_arg->gpioB_value,
            GPIO_REGB_BASE, gpio_test_arg->gpioSIO_value, GPIO_REGSIO_BASE, index);

    if (test_result != TEST_PASS)
    {
        goto end_test;
    }

    test_result = TEST_PASS;
    end_test:
    // act_writel(gpioA_out_bak, GPIOAOUTEN);
    // act_writel(gpioA_in_bak, GPIOAINEN);
    // act_writel(gpioA_pu_bak, GPIOAPUEN);
    // act_writel(gpioA_pd_bak, GPIOAPDEN);
    // act_writel(gpioA_dat_bak, GPIOADAT);
    // act_writel(gpioB_out_bak, GPIOBOUTEN);
    // act_writel(gpioB_in_bak, GPIOBINEN);
    // act_writel(gpioB_pu_bak, GPIOBPUEN);
    // act_writel(gpioB_pd_bak, GPIOBPDEN);
    // act_writel(gpioB_dat_bak, GPIOBDAT);

    gpio_reg = GPIOAOUTEN;
    for (i = 0; i < 15; i++, gpio_reg += 4)
    {
        act_writel(gpio_bak_array[i], gpio_reg);
    }

    if(test_result != TEST_PASS)
    {
        gpio_test_arg->gpioA_value = *index;
        gpio_test_arg->gpioB_value = test_result;
        gpio_test_arg->gpioSIO_value = 0;
    }

    return test_result;
}

test_result_e act_test_gpio_test(void *arg_buffer)
{
    int32 index;
    test_result_e ret_val;
    //DEBUG_ATT_PRINT("start gpio test", 0, 0);

    //DEBUG_ATT_PRINT("GPIO_A  :", *((uint32 *)arg_buffer), 2);
    //DEBUG_ATT_PRINT("GPIO_B  :", *(((uint32 *)arg_buffer)+1), 2);
    //DEBUG_ATT_PRINT("GPIO_SIO:", *(((uint32 *)arg_buffer)+2), 2);

    gpio_test_arg_t *gpio_test_arg = (gpio_test_arg_t *) arg_buffer;

    ret_val = test_gpio(gpio_test_arg, &index);

    act_test_report_gpio_result(gpio_test_arg, ret_val, TESTID_GPIO_TEST);

    return ret_val;
}

test_result_e act_test_gpio_test_ATS2823(void *arg_buffer)
{
    int32 index;
    test_result_e ret_val;
    //DEBUG_ATT_PRINT("start gpio test", 0, 0);

    //DEBUG_ATT_PRINT("GPIO_A  :", *((uint32 *)arg_buffer), 2);
    //DEBUG_ATT_PRINT("GPIO_B  :", *(((uint32 *)arg_buffer)+1), 2);
    //DEBUG_ATT_PRINT("GPIO_SIO:", *(((uint32 *)arg_buffer)+2), 2);

    gpio_test_arg_t *gpio_test_arg = (gpio_test_arg_t *) arg_buffer;

    ret_val = test_gpio(gpio_test_arg, &index);

    act_test_report_gpio_result(gpio_test_arg, ret_val, TESTID_GPIO_TEST_ATS2823);

    return ret_val;
}

