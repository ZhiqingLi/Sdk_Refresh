
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief   处理打电话的操作--铃声响未接通时
*      Cailizhen          2015-1          creat
*       Wekan        2015-3-30       review
*******************************************************************************/

#include "btcall_engine.h"

#define CALLRING_FADE_IN    1
#define CALLRING_FADE_OUT   2
#define CALLRING_FADE_OK    3


uint8 g_irq_cnt;
uint8 g_hf_flag;
uint8 g_ff_flag;
/******************************************************************************
* \par  Description:   把sco接收的数据写到缓存空间,单声道音频数据
*\                            拷贝为双声道音频
* \param[in]     tmp_sco_recv_cache接收到的sco数据的指针
* \param[in]     tmp_len  要写入缓存区的大小--单位是bytes
* \param[in]     fade 0表示没有淡入淡出，1表示淡入，2表示淡出
* \param[out]   tmp_sco_dac_buf存放数据的缓存空间指针
* \return           none
* \note
* \  <author>       <time>        <opt>
* \  Cailizhen          2015-1          creat
* \   Wekan       2015-3-30     reviwe
*******************************************************************************/

void memcpy_dual_callring(uint8 *tmp_sco_dac_buf, uint8 *tmp_sco_recv_cache, int32 tmp_len, uint8 fade)
{
    int32 i;
    int32 tmp_value;
    if(tmp_len == 0)
    {
        tmp_len = 1;
    }

    for (i = 0; i < tmp_len; i += 2)
    {
        tmp_value = *(int16*)(tmp_sco_recv_cache + i);
        if (fade == CALLRING_FADE_IN)
        {
            tmp_value = tmp_value * i / tmp_len;
        }
        else if (fade == CALLRING_FADE_OUT)
        {
            tmp_value = tmp_value * (tmp_len - i) / tmp_len;
        }
        else
        {
            ; //nothing
        }

        *(int16*)(tmp_sco_dac_buf + i*2) = (int16) tmp_value;
        *(int16*)(tmp_sco_dac_buf + i*2 + 2) = (int16) tmp_value;
    }
}

/******************************************************************************
* \par  Description:   加载callring数据
* \param[in]     tmp_sco_recv_cache接收到的sco数据的指针
* \param[in]     tmp_len  要写入缓存区的大小--单位是bytes
* \param[out]   tmp_sco_dac_buf存放数据的缓存空间指针
* \return           none
* \note
* \  <author>       <time>        <opt>
* \  Cailizhen          2015-1          creat
* \   Wekan       2015-3-30     reviwe
*******************************************************************************/

int32 callring_frame(uint8 *frame_buffer, int32 frame_size)
{
    int32 len;

    if (g_callring_data.callring_silent_flag == TRUE)
    {
        libc_memset(frame_buffer, 0x00, frame_size);
        g_callring_data.callring_silent_time--;
        if (g_callring_data.callring_silent_time == 0)
        {
            g_callring_data.callring_silent_flag = FALSE;
        }

        len = frame_size;
    }
    else
    {
        if (-1 == sys_sd_fseek(g_callring_data.callring_handle, 0, \
           ((uint32)g_callring_data.callring_read_ptr) & 0xFFFF) )
        {
            len = -1;
            goto jump_end;
        }

        if ((g_callring_data.callring_read_ptr + frame_size) > g_callring_data.callring_file_length)
        {
            len = sys_sd_fread(g_callring_data.callring_handle, frame_buffer, \
            g_callring_data.callring_file_length - g_callring_data.callring_read_ptr);
        }
        else
        {
            len = sys_sd_fread(g_callring_data.callring_handle, frame_buffer, frame_size);
        }
        if (len != -1)
        {
            if (g_callring_data.callring_read_ptr == 0)
            {//头4字节为文件长度，清为0
                frame_buffer[0] = 0;
                frame_buffer[1] = 0;
                frame_buffer[2] = 0;
                frame_buffer[3] = 0;
            }

            g_callring_data.callring_read_ptr += (uint16)len;
            if (g_callring_data.callring_read_ptr >= g_callring_data.callring_file_length)
            {
                g_callring_data.callring_read_ptr = 0;
                if (frame_size > 0)//for QAC
                {
                    //计算间隔帧数
                    g_callring_data.callring_silent_time = \
                    (uint16)((int32) btcall_gl_var.callin_ring_space_time * 1600 / frame_size); //100ms长度为1600字节
                }
                else
                {
                    g_callring_data.callring_silent_time = 6; //每帧32ms，6帧大约200ms
                }
                g_callring_data.callring_silent_flag = TRUE;
            }
            if (len < frame_size)
            {//填充静音
                libc_memset(frame_buffer + len, 0x00, frame_size - len);
            }
        }

    }
    jump_end:
    return len;
}

/******************************************************************************
* \par  Description:   启动铃声--未接通时
* \param[in]
* \param[out]
* \return           none
* \note
* \  <author>       <time>        <opt>
* \  Cailizhen          2015-1          creat
* \   Wekan       2015-3-30     reviwe
*******************************************************************************/
void callring_start(void)
{
    pthread_ext_param_t decode_thread_para;

    if (g_callring_data.callring_thread_exist != 0)
    {
        return ;
    }
    //    if (btcall_gl_var.g_a2dp_decode_deal_flag != 0)
    //    {//有些手机来电时不会停止播放音乐；这样会导致一些播放音乐时来电没有发pause ind的手机没有播放来电铃声
    //        return ;
    //    }
    btcall_gl_var.need_exit_thread = TRUE;

    if (btcall_gl_var.g_sco_deal_flag != 0)
    {//规避重复创建sco解码线程
        btcalleg_free_hfp_mmm();
    }

    g_callring_data.callring_handle = sys_sd_fopen("callring.pcm");
    if (g_callring_data.callring_handle == NULL)
    {
        PRINT_ERR("Cann't open file callring.pcm!");
        g_callring_data.callring_file_length = 0;
    }
    else
    {
        sys_sd_fseek(g_callring_data.callring_handle, 0, 0);
        sys_sd_fread(g_callring_data.callring_handle, &(g_callring_data.callring_file_length), 2);
    }
    g_callring_data.callring_read_ptr = 0;

    //该语句不能放在最开始的位置，从最开始的语句到这里仍然可能调度到前台
    g_callring_data.callring_timer_id = -1;//表示已经执行过该定时器
    btcall_gl_var.last_dma0_state = DMA_NOT_TRANS;

    //创建callring子线程
    decode_thread_para.pthread_param.start_rtn = callring_thread_handle;
    decode_thread_para.pthread_param.arg = NULL;
    decode_thread_para.pthread_param.ptos = (void *) AP_BACK_HIGH_STK_POS;//AP_BT_LOW_STK_POS
    decode_thread_para.stk_size = AP_BACK_HIGH_STK_SIZE;
    if(libc_pthread_create(&decode_thread_para, AP_BACK_HIGH_PRIO, CREATE_NOT_MAIN_THREAD) < 0)
    {
        while (1)
        {
            ;//创建callring子线程失败
        }
    }
}


/******************************************************************************
* \par  Description:   创建铃声线程
* \param[in]     param 暂时保留未用
* \param[out]
* \return           none
* \note
* \  <author>       <time>        <opt>
* \  Cailizhen          2015-1          creat
* \   Wekan       2015-3-30     reviwe
*******************************************************************************/

void *callring_thread_handle(void *param)
{
    uint8 fade_falg = 0;
    g_callring_data.callring_thread_exist = 1;
    btcall_gl_var.need_exit_thread = FALSE;
    while(1)
    {
        if (btcall_gl_var.need_exit_thread == TRUE)
        {
            if (fade_falg == 0)
            {
                fade_falg = CALLRING_FADE_OUT;
            }
            if (fade_falg == CALLRING_FADE_OK)
            {
                if (btcall_gl_var.last_dma0_state == DMA_HALF_TRANSED)
                {
                    libc_memset(g_btcall_callring_dac_buf + CALLRING_DAC_BUFFER_LENGTH/2, 0x00, 256);
                    while ((g_hf_flag == 0))//half transed，表示淡出帧播放结束
                    {
                        sys_mdelay(1);
                    }
                }
                else
                {
                    libc_memset(g_btcall_callring_dac_buf, 0x00, 256);
                    while (g_ff_flag == 0)//trans complete，表示淡出帧播放结束
                    {
                        sys_mdelay(1);
                    }
                }
                break;
            }
        }

        if (btcall_gl_var.last_dma0_state == DMA_NOT_TRANS)
        {
            callring_frame(g_btcall_callring_read_buf, CALLRING_READ_BUFFER_LENGTH);
            memcpy_dual_callring(g_btcall_callring_dac_buf, g_btcall_callring_read_buf, \
                    CALLRING_READ_BUFFER_LENGTH, 0);

            if(1 == g_app_info_state_all.fix_sample_rate_flag)
            {
                config_asrc(K_OUT0_U0, 3);
                set_dac_rate(48,0);
                set_asrc_rate(8,0,1000);
                //act_writel(act_readl(ASRC_OUT0_CTL) | (0x01),ASRC_OUT0_CTL);
                enable_dac(DAFIS_ASRC, DAF0_EN);
            }
            else
            {
                set_dac_rate(8,0);
                enable_dac(DAFIS_DMA, DAF0_EN);
            }

            callring_dac_dma_config();
            dma_dac_start_transfer();

            btcall_gl_var.last_dma0_state = DMA_TRANS_COMPLECT;//表示将更新buffer上半half
        }
        #if 1
        else if (btcall_gl_var.last_dma0_state == DMA_TRANS_COMPLECT)
        {
            if (g_hf_flag == 1)//((act_readl(DMAIP) & 0x00000800) != 0)//half transed
            {
                //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
                //act_writel(0x00000800, DMAIP);
                g_hf_flag = 0;
                
                callring_frame(g_btcall_callring_read_buf, CALLRING_READ_BUFFER_LENGTH/2);
                memcpy_dual_callring(g_btcall_callring_dac_buf, g_btcall_callring_read_buf, \
                        CALLRING_READ_BUFFER_LENGTH/2, fade_falg);
                btcall_gl_var.last_dma0_state = DMA_HALF_TRANSED;
                if ((g_irq_cnt == 0) && (1 == g_app_info_state_all.fix_sample_rate_flag))
                {
                    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_reload), DMA3CTL);
                    act_writel(act_readl(DMA3CTL) & (~(1 << DMA3CTL_DMA3START)), DMA3CTL);
                    act_writel(act_readl(DMA3CTL) | (1 << DMA3CTL_DMA3START), DMA3CTL);
                }
                
                if (fade_falg == CALLRING_FADE_OUT)
                {
                    fade_falg = CALLRING_FADE_OK;
                }
            }
        }
        else //btcall_gl_var.last_dma0_state == DMA_HALF_TRANSED
        {
            if (g_ff_flag == 1)//((act_readl(DMAIP) & 0x00000008) != 0)//trans complete
            {
                //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
                //act_writel(0x00000008, DMAIP);
                g_ff_flag = 0;
                
                callring_frame(g_btcall_callring_read_buf, CALLRING_READ_BUFFER_LENGTH/2);
                memcpy_dual_callring(g_btcall_callring_dac_buf + CALLRING_DAC_BUFFER_LENGTH/2, \
                        g_btcall_callring_read_buf, CALLRING_READ_BUFFER_LENGTH/2, fade_falg);
                btcall_gl_var.last_dma0_state = DMA_TRANS_COMPLECT;
                
                if (fade_falg == CALLRING_FADE_OUT)
                {
                    fade_falg = CALLRING_FADE_OK;
                }
                g_irq_cnt ++;
            }
        }
        #endif
        sys_os_time_dly(1);
    }
    g_irq_cnt = 0;

    act_writel(act_readl(DMA3CTL) & (~(1 << DMA3CTL_DMA3START)), DMA3CTL);
    
    sys_free_irq(IRQ_DMA3);
    
    disable_dac(DAF0_EN);
    
    if(1 == g_app_info_state_all.fix_sample_rate_flag)
    {
        close_asrc(K_OUT0_U0);
    }
    
    sys_sd_fclose(g_callring_data.callring_handle);
    btcall_gl_var.last_dma0_state = DMA_NOT_TRANS;
    g_callring_data.callring_thread_exist = 0;
    g_callring_data.callring_silent_flag = FALSE;
    libc_pthread_exit();

    return (void *) NULL;
}

__section__ (".text.irq")int ring_dma_isr(void)
{
    uint8 tempdata = 0;
    if ((act_readl(DMAIP) & 0x00000008) != 0)//trans complete
    {
        //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
        act_writel(0x00000008, DMAIP);
        
        g_ff_flag = 1;
    }
    else if ((act_readl(DMAIP) & 0x00000800) != 0)//half transed
    {
        //clear pending，注意不能使用读-或-写这种方式，避免清掉其他pending位
        act_writel(0x00000800, DMAIP);

        g_hf_flag = 1;
    }
    else
    {
        ;//nothing
    }
    return tempdata;
}
