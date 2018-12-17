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
#include "ap_autotest_channel_test.h"
#include "fm_interface.h"
#include "ap_autotest_channel_test_SNR.h"

//16个点
const uint16 pcm_table[]=
{
    0x6ba,  0x371f, 0x5f22, 0x78a6, 0x7fce, 0x7328, 0x55c9, 0x2ab2,
    0xf945, 0xc8df, 0xa0de, 0x8757, 0x802f, 0x8c7f, 0xaa63, 0xd550
};



volatile uint8 g_adc_buffer_ptr _BANK_DATA_ATTR_;

uint8 g_sample_count _BANK_DATA_ATTR_;

uint32 g_power_val_left _BANK_DATA_ATTR_;

uint32 g_power_val_right _BANK_DATA_ATTR_;

uint32 power_val_array[MAX_POWER_SAMPLE_COUNT] _BANK_DATA_ATTR_;

//使用1K的缓存 16*16*2*2=1k
void init_dac_buffer(void)
{
    uint8 i, j;

    uint16 *p_dac_buffer = (uint16 *) DAC_BUFFER;

    for (j = 0; j < 16; j++)
    {
        for (i = 0; i < 16; i++)
        {
            //左右声道使用相同的数据
            *p_dac_buffer = pcm_table[i];
            p_dac_buffer++;
            *p_dac_buffer = pcm_table[i];
            p_dac_buffer++;
        }
    }

    return;
}

void sco_dac_dma_config(void)
{
    act_writel(0, DMA0CTL);

    sys_udelay(10);

    //separated stored in the memory, 0 for interleaved stored, 1 for separated stored
    act_writel(act_readl(DMA0CTL) & (~(1 << DMA0CTL_AUDIOTYPE)), DMA0CTL);

    //data width 16bit
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_DATAWIDTH_SHIFT), DMA0CTL);

    //source addr type:memory
    act_writel(act_readl(DMA0CTL) | (0 << DMA0CTL_SRCTYPE_SHIFT), DMA0CTL);

    //dest addr type:dac fifo
    act_writel(act_readl(DMA0CTL) | (0x0b << DMA0CTL_DSTTYPE_SHIFT), DMA0CTL);

    //reload enable
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_reload), DMA0CTL);

    //source address
    act_writel(DAC_BUFFER, DMA0SADDR0);

    //dma length
    act_writel(1024 / 2 / 2, DMA0FrameLen);

#if 0
    //enable dma0 half & full trans interupt
    //act_writel(act_readl(DMAIE)|0x00000011, DMAIE);

    //DAC FIFO Input Select:from Special DMA :DMA to DAC
    act_writel(act_readl(DAC_FIFOCTL) & ~(DAC_FIFOCTL_DAF0IS_MASK), DAC_FIFOCTL);
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0IS_SHIFT), DAC_FIFOCTL);

    //enalbe dac fifo
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0RT), DAC_FIFOCTL);

#else

    //DAC FIFO Input Select:from Special DMA :DMA to DAC
    act_writel(act_readl(DAC_FIFOCTL) & ~(DAC_FIFOCTL_DAF0IS_MASK), DAC_FIFOCTL);
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0IS_SHIFT), DAC_FIFOCTL);

    //both left&right channel
    act_writel((act_readl(DAC_ANACTL) | (1 << DAC_ANACTL_DAENL)), DAC_ANACTL);
    act_writel((act_readl(DAC_ANACTL) | (1 << DAC_ANACTL_DAENR)), DAC_ANACTL);

    //enalbe DAC FIFO Empty DRQ
    act_writel(act_readl(DAC_FIFOCTL) | (1 << DAC_FIFOCTL_DAF0EDE), DAC_FIFOCTL);

    //enable transfer
    act_writel(act_readl(DMA0CTL) | (1 << DMA0CTL_DMA0START), DMA0CTL);

    sys_udelay(10);
#endif
}

void print_channel_data(uint32 dst_addr)
{
    int i;

    DEBUG_ATT_PRINT("\n**************\n", 0, 0);

    for(i = 0; i < 128; i += 4)
    {
        DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i), 1);
        DEBUG_ATT_PRINT(0, *(uint16 *)(dst_addr + i + 2), 1);
    }
}


/******************************************************************************/
/*!
 * \par  Description:
 * \配置ADC采样的参数以及DMA的配置
 * \param[in]    dst_addr 采集数据存放的缓冲区地址
 * \param[in]    block_length 采集的数据长度
 * \param[out]   none
 * \return       none
 * \retval
 * \ingroup      music_module_test.c
 * \note
 */
/*******************************************************************************/
void init_dma_to_adc(uint32 dst_addr, uint32 block_length)
{
    uint32 status;

    //左右声道均会采集数据
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADLEN), ADC_ANACTL);

    //左右声道均会采集数据
    act_writel(act_readl(ADC_ANACTL) | (1 << ADC_ANACTL_ADREN), ADC_ANACTL);

    //abort DMA stransfer and reset config
    *((REG32)(DMA1CTL)) = 0x00;

    //data width 16bit
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DATAWIDTH_SHIFT), DMA1CTL);

    //source addr type:adc fifo
    act_writel(act_readl(DMA1CTL) | (0x0b << DMA1CTL_SRCTYPE_SHIFT), DMA1CTL);

    //dest addr type:memory
    act_writel(act_readl(DMA1CTL) | (0 << DMA1CTL_DSTTYPE_SHIFT), DMA1CTL);

    //reload enable
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_reload), DMA1CTL);

    //dest address
    act_writel(dst_addr, DMA1DADDR0);

    //dma length
    act_writel(block_length / 2, DMA1FrameLen);

    //dma1 half & complete int disable
    act_writel(act_readl(DMAIE) & (~((1 << DMAIE_DMA1HFIE) | (1 << DMAIE_DMA1TCIE))), DMAIE);

    //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
    act_writel((1 << DMAIP_DMA1HFIP) | (1 << DMAIP_DMA1TCIP), DMAIP);    

    g_adc_buffer_ptr = 0;

    g_sample_count = 0;

    //挂接中断服务程序
    sys_request_irq(IRQ_DMA1, adc_data_deal);

    //dma1 half & complete int enable
    act_writel(act_readl(DMAIE) | (1 << DMAIE_DMA1HFIE) | (1 << DMAIE_DMA1TCIE), DMAIE);      

    //enable transfer
    act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);

  
#if 0
    while (1)
    {
        status = act_readl(DMA1CTL);

        if ((status & 0x01) == 0)
        {
            loop_cnt++;

            if (loop_cnt < 2)
            {
                act_writel(act_readl(DMA1CTL) | (1 << DMA1CTL_DMA1START), DMA1CTL);
            }
            else
            {
                //abort DMA stransfer and reset config
                *((REG32)(DMA1CTL)) = 0x00;
                break;
            }

        }
    }
#endif
}

void play_pcm_init(uint8 ain_type)
{
    DEBUG_ATT_PRINT("start channel test", ain_type, 2);

    set_dac_rate(ADC_SAMPLE_RATE, 0);

    enable_dac(DAFIS_DMA, DAF0_EN);

    //先使能模拟输入，再设置ADC采样率，最后使能ADC
    if (ain_type == AI_SOURCE_AUX1)
    {
        enable_ain(AI_SOURCE_AUX1, 1);
    }
    else if (ain_type == AI_SOURCE_AUX0)
    {
        enable_ain(AI_SOURCE_AUX0, 1);    
    }
    else if (ain_type == AI_SOURCE_ASEMIC)
    {
        //打开mic测试通道
        enable_ain(AI_SOURCE_ASEMIC, 0);
    }
    else
    {
        ;//FM测试暂不支持
    }

    set_adc_rate(ADC_SAMPLE_RATE);

    enable_adc(ADFOS_DMA, 0);

    init_dac_buffer();

    sco_dac_dma_config();

    if (ain_type == AI_SOURCE_ASEMIC)
    {
        //设置音量等级为5级音量，该音量可满足mic,linein的测试要求
        set_pa_volume(15, (0xbf));
    }
    else
    {
        set_pa_volume(30, (0xbf));
    }       
}

void play_pcm_exit(void)
{
    //停止数据发送
    act_writel(0, DMA0CTL);

    act_writel(0, DMA1CTL);

    disable_dac(DAF0_EN);

    disable_adc();
}

static int32 caculate_power_value(uint32 dac_buffer_addr, uint32 data_length, uint32 *power_val_array, uint32 index)
{
    uint32 i;
    uint32 power_value = 0;
    uint32 power_sample_value = 0;

    int16 *dac_buffer = (int16 *)dac_buffer_addr;

    //计算单个声道的样本点
    for (i = 0; i < data_length; i += 2)
    {
        if (dac_buffer[i] >= 0)
        {
            power_sample_value = (uint32)(dac_buffer[i]);
        }
        else
        {
            power_sample_value = (uint32)(-dac_buffer[i]);
        }

        power_value += power_sample_value;
    }

    power_value = power_value / (data_length / 2);

    power_val_array[index] = power_value;

    att_write_test_info("power value:", power_value, 1);

    return 0;
}

static uint32 libc_abs(int32 value)
{
    if (value > 0)
    {
        return value;
    }
    else
    {
        return (0 - value);
    }
}

int32 analyse_power_val_valid(uint32 sample_cnt, uint32 *power_val_array, uint32 *p_power_val_left, uint32 *p_power_val_right)
{
    uint32 i;
    uint32 power_val;
    uint32 power_val_total;
    uint32 div_val;

    int32 diff_val;

    int32 max_diff_val;
    int32 max_diff_index;
    int32 invalid_data_flag;

    int32 analyse_flag = 0;
    
    uint32 sample_count = (sample_cnt >> 1);

    retry:
    power_val_total = 0;
    
    for(i = 0; i < sample_count; i++)
    {
        power_val_total += power_val_array[i*2 + analyse_flag];
    }

    power_val = power_val_total / i;

    invalid_data_flag = FALSE;

    max_diff_val = 0;

    while (1)
    {
        //判断采样点离散程度,先找出离散程度很大的点
        for (i = 0; i < sample_count; i++)
        {
            if (power_val_array[i*2 + analyse_flag] != INVALID_POWER_VAL)
            {
                diff_val = libc_abs(power_val_array[i*2 + analyse_flag] - power_val);

                if (diff_val > max_diff_val)
                {
                    max_diff_index = i;
                    max_diff_val = diff_val;
                }
            }
        }

        //判断离散程度最大的点是否超过限制，如果超过限制，剔除该点，重复计算下一点
        if (max_diff_val > MAX_POWER_DIFF_VAL)
        {   
            libc_print("invalid power", power_val_array[max_diff_index*2 + analyse_flag], 2);

            libc_print("index ", max_diff_index, 2);
            
            //标记该点为无效点
            power_val_array[max_diff_index*2 + analyse_flag] = INVALID_POWER_VAL;

            invalid_data_flag = TRUE;

            max_diff_val = 0;

            //重新计算一次平均值
            power_val_total = 0;

            div_val = 0;

            for (i = 0; i < sample_count; i++)
            {     
                if (power_val_array[i*2 + analyse_flag] != INVALID_POWER_VAL)
                {
                    power_val_total += power_val_array[i*2 + analyse_flag];
                                 
                    div_val++;
                }
            }  

            power_val = (power_val_total / div_val); 

            if(analyse_flag == 0)
            {
                libc_print("cal left power", power_val, 2);
            }
            else
            {
                libc_print("cal right power", power_val, 2);    
            }
        }
        else
        {
            break;
        }
    }

    //存在无效点需要重新计算cfo平均值
    if (invalid_data_flag == TRUE)
    {
        power_val_total = 0;

        div_val = 0;

        for (i = 0; i < sample_count; i++)
        {
            if (power_val_array[i*2 + analyse_flag] != INVALID_POWER_VAL)
            {
                power_val_total += power_val_array[i*2 + analyse_flag];
                div_val++;
            }
            else
            {
                continue;
            }
        }

        //至少要有2组记录值
        if (div_val < 2)
        {
            return FALSE;
        }

        power_val = (power_val_total / div_val);
    }

    if(analyse_flag == 0)
    {
        *p_power_val_left = power_val;

        analyse_flag = 1;

        goto retry;
    }
    else
    {
        *p_power_val_right = power_val;
    }

    if((libc_abs(*p_power_val_left - power_val_array[sample_cnt - 2]) <= MAX_POWER_DIFF_VAL)
       && (libc_abs(*p_power_val_right - power_val_array[sample_cnt - 1]) <= MAX_POWER_DIFF_VAL))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void adc_data_deal(void)
{
    int32 ret_val;
    uint8 *src_buffer_addr = (uint8 *)SOUND_DATA_ADDR;
    
	if ((act_readl(DMAIP) & (1 << DMAIP_DMA1HFIP)) != 0)
    {
        libc_print("half",0,0);
        while ((act_readl(DMAIP) & (1 << DMAIP_DMA1HFIP)) != 0)
        {
            act_writel((1 << DMAIP_DMA1HFIP), DMAIP);
        }   
    }
    else if ((act_readl(DMAIP) & (1 << DMAIP_DMA1TCIP)) != 0)
    {
        libc_print("Complete", g_adc_buffer_ptr, 2);
        while ((act_readl(DMAIP) & (1 << DMAIP_DMA1TCIP)) != 0)
        {
            act_writel((1 << DMAIP_DMA1TCIP), DMAIP);
        }

        //print_channel_data(SAVE_SOUND_DATA_ADDR + i * SOUND_DATA_LEN);

        //统计左声道能量值
        caculate_power_value(src_buffer_addr, SOUND_DATA_LEN / 2, power_val_array, g_sample_count++);
    
        //统计右声道能量值
        caculate_power_value(src_buffer_addr + 2, SOUND_DATA_LEN / 2, power_val_array, g_sample_count++);
 
        //超过最小采样次数,开始进行数据分析
        if(g_sample_count >= MIN_POWER_SAMPLE_COUNT)
        {
            //比较采用得到的能量平均值，如果平均值在阈值范围内，则认为采样数据已经稳定
            ret_val = analyse_power_val_valid(g_sample_count, power_val_array, &g_power_val_left, &g_power_val_right);

            //如果数据稳定，结束数据采样
            if(ret_val == TRUE)
            {
                act_writel(0, DMA1CTL);               
            }
            else
            {
                g_sample_count = 0;
            }
        }
    }
    else
    {
        ;//qac
    }	    
}

int32 channel_test(void *arg_buffer, uint32 test_id)
{
    int32 i;
    uint32 ret_val = TRUE;
    channel_test_arg_t *channel_test_arg = (channel_test_arg_t *)arg_buffer;
    
    libc_memset(power_val_array, 0, sizeof(power_val_array));

    //开始通过linein通道采集数据
    init_dma_to_adc(SOUND_DATA_ADDR, SOUND_DATA_LEN);  

    while(1)
    {
        if(act_readl(DMA1CTL) == 0)
        {
            sys_free_irq(IRQ_DMA1);

            if(test_id == TESTID_MIC_CH_TEST)
            {
                //采集结束，关闭通道
                disable_ain(AI_SOURCE_ASEMIC);                    
            }
            else
            {
                //采集结束，关闭通道
                disable_ain(AI_SOURCE_AUX1);   
                disable_ain(AI_SOURCE_AUX0); 
            }
            break;
        }
    }
    
    DEBUG_ATT_PRINT("return power val left", g_power_val_left, 2);

    DEBUG_ATT_PRINT("return power val right", g_power_val_right, 2);

    if(channel_test_arg->test_left_ch == TRUE)
    {
        if(g_power_val_left < channel_test_arg->left_ch_power_threadshold)
        {
            ret_val = FALSE;
        }
    }

    if(channel_test_arg->test_right_ch == TRUE)
    {
        if(g_power_val_right < channel_test_arg->right_ch_power_threadshold)
        {
            ret_val = FALSE;
        }
    }  

    if(ret_val == FALSE)
    {
        channel_test_arg->left_ch_power_threadshold = g_power_val_left;
        channel_test_arg->right_ch_power_threadshold = g_power_val_right;
    }
    
    if(test_id == TESTID_LINEIN_CH_TEST_ATS2825 || test_id == TESTID_LINEIN_CH_TEST_ATS2823)
    {
#ifdef DEBUG_WRITE_CHANNEL_DATA
        write_temp_file(1, SOUND_DATA_ADDR, SOUND_DATA_LEN);
#endif  
    }
    else if(test_id == TESTID_MIC_CH_TEST)
    {
#ifdef DEBUG_WRITE_CHANNEL_DATA
        write_temp_file(0, SOUND_DATA_ADDR, SOUND_DATA_LEN);
#endif                     
    }      
    else
    {
        ;//nothing for QAC
    }

    if(ret_val == TRUE)
    {
        ret_val = thd_test(SOUND_DATA_ADDR, channel_test_arg);
    }
    else
    {
        channel_test_arg->left_ch_SNR_threadshold = 0;
        channel_test_arg->left_ch_max_sig_point = 0;
        channel_test_arg->right_ch_SNR_threadshold = 0;
        channel_test_arg->right_ch_max_sig_point = 0;
    }
    
    return ret_val;
}

void act_test_report_channel_result(uint16 test_id, int32 ret_val, void *arg_buffer)
{
    return_result_t *return_data;
    uint16 trans_bytes = 0;
    channel_test_arg_t *channel_test_arg = (channel_test_arg_t *)arg_buffer;

    if (g_test_mode != TEST_MODE_CARD)
    {
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = test_id;

        return_data->test_result = ret_val;

        int32_to_unicode(channel_test_arg->left_ch_power_threadshold, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);
        
        return_data->return_arg[trans_bytes++] = 0x002c;
        
        int32_to_unicode(channel_test_arg->right_ch_power_threadshold, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);
        
        return_data->return_arg[trans_bytes++] = 0x002c;
        
        int32_to_unicode(channel_test_arg->left_ch_SNR_threadshold, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);
        
        return_data->return_arg[trans_bytes++] = 0x002c;
        
        int32_to_unicode(channel_test_arg->right_ch_SNR_threadshold, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);

        return_data->return_arg[trans_bytes++] = 0x002c;

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

test_result_e act_test_linein_channel_test(void *arg_buffer, uint32 ic_type)
{
    int32 result = 0;

    if(ic_type == 1)
    {
        //初始化ADC,DAC,初始化相关DMA
        play_pcm_init(AI_SOURCE_AUX1);

        result = channel_test(arg_buffer, TESTID_LINEIN_CH_TEST_ATS2825);

        play_pcm_exit();

        if(result == TRUE)
        {
            //初始化ADC,DAC,初始化相关DMA
            play_pcm_init(AI_SOURCE_AUX0);

            result = channel_test(arg_buffer, TESTID_LINEIN_CH_TEST_ATS2825);

            play_pcm_exit();         
        }
    }
    else
    {
        //初始化ADC,DAC,初始化相关DMA
        play_pcm_init(AI_SOURCE_AUX0);

        result = channel_test(arg_buffer, TESTID_LINEIN_CH_TEST_ATS2823);

        play_pcm_exit();    
    }
    
    if (result == FALSE)
    {
        att_write_test_info("linein channel test failed", 0, 0);
    }
    else
    {
        att_write_test_info("linein channel test ok", 0, 0);
    }

    if(ic_type == 1)
    {
        act_test_report_channel_result(TESTID_LINEIN_CH_TEST_ATS2825, result, arg_buffer);
    }
    else
    {
        act_test_report_channel_result(TESTID_LINEIN_CH_TEST_ATS2823, result, arg_buffer);    
    }
    
    return result;
}

test_result_e act_test_linein_channel_test_ATS2825(void *arg_buffer)
{
    return act_test_linein_channel_test(arg_buffer, 1);
}

test_result_e act_test_linein_channel_test_ATS2823(void *arg_buffer)
{
    return act_test_linein_channel_test(arg_buffer, 0);
}

test_result_e act_test_mic_channel_test(void *arg_buffer)
{
    int32 result;

    //初始化ADC,DAC,初始化相关DMA
    play_pcm_init(AI_SOURCE_ASEMIC);

    result = channel_test(arg_buffer, TESTID_MIC_CH_TEST);

    play_pcm_exit();

    if (result == FALSE)
    {
        att_write_test_info("mic channel test failed", 0, 0);
    }
    else
    {
        att_write_test_info("mic channel test ok", 0, 0);
    }

    act_test_report_channel_result(TESTID_MIC_CH_TEST, result, arg_buffer);

    return result;
}

