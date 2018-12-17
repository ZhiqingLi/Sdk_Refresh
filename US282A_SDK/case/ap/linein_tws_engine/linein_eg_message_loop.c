/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌引擎应用业务逻辑主循环实现。
 * 作者：
 ********************************************************************************/

#include  "linein_engine.h"

/******************************************************************************/
/*!
 * \par  Description:
 *      处理当前应用的私有消息
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
app_result_e btplay_deal_app_msg(void)
{
    private_msg_t cur_msg;
    app_result_e result = RESULT_NULL;

    //更新共享查询
    g_p_btplay_info = sys_share_query_update(APP_ID_LINEIN_EG);

    if (get_app_msg(&cur_msg) == TRUE)
    {
        result = app_message_deal(&cur_msg);
    }

    return result;
}

#ifdef ENABLE_TRUE_WIRELESS_STEREO
void Tws_clear_filter(void)
{
    if(btplay_gl_var.tts_play_flag == FALSE)
    {
        btplay_engine_pipe_control(PIPE_OP_UNFILTER );
        enable_unfilter_flag = 0;

    }
}

#endif

/******************************************************************************/
/*!
 * \par  Description:
 *      MainMenu 应用的入口函数和场景管理器模块
 * \param[in]    PARAM_FROM_XXX
 * \param[out]   none
 * \return       the result
 * \retval       0 sucess
 * \retval       1 failed
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
app_result_e get_message_loop(void)
{
    app_result_e result = RESULT_NULL;

    message_loop_init();
    while (1)
    {
                
        tws_sync_cmd_deal();
        if ((btplay_gl_var.codec_flag == FALSE) && (btplay_gl_var.tts_play_flag == FALSE)
                && (g_p_bt_stack_cur_info->conn_status >= CONN_STATUS_WAIT_PAIR)
                &&(g_need_adc_flag == 1))
        {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
            if (g_p_bt_stack_cur_info->dev_role != 2)
            {
                //libc_print("@11:",0,0);
                btplay_engine_pipe_control(PIPE_OP_FILTER);
            }
#endif

            //避免加载起来中间件后又会去卸载掉
            if ((g_p_tws_info->pri_exit_mmm == 1) || (g_p_tws_info->sub_exit_mmm == 1))
            {
                g_p_tws_info->pri_exit_mmm = 0;
                g_p_tws_info->sub_exit_mmm = 0;

            }

           a2dp_decode_init(A2DP_CODEC_SBC);
          
#ifdef ENABLE_TRUE_WIRELESS_STEREO

            if ((enable_unfilter_flag == 0)
            	  &&(linein_tws_playpause_flag == 0)
            	  &&(g_p_bt_stack_cur_info->dev_role != 2))
            {
                libc_print("@12:",0,0);
                btplay_engine_pipe_control(PIPE_OP_UNFILTER);
            }
#endif
        }

#ifdef ENABLE_TRUE_WIRELESS_STEREO

        send_mmm_exit_cmd();
        //卸载BTPLAY中间件和DSP解码库
        if ((btplay_gl_var.codec_flag == TRUE)
                && ((g_p_tws_info->pri_exit_mmm==1)
                        ||(g_p_tws_info->sub_exit_mmm==1)
                        ||(second_empty_flag!=0)
                        ||(g_p_tws_info->tts_start_flag==1)
                        ||(tws_or_notws_flag != g_p_bt_stack_cur_info->dev_role)))

        {
            //libc_print("phc:",g_p_bt_stack_cur_info->tws_master_phonecon,2);
            libc_print("fl:",second_empty_flag,2);
            btplay_engine_pipe_control(PIPE_OP_FILTER);
            a2dp_decode_quit();
            libc_print("G1",0,0);
            if((g_p_bt_stack_cur_info->dev_role==1)&&((g_p_tws_info->pri_exit_mmm==1)||(g_p_tws_info->pri_full_exit_flag)
                ||(g_p_tws_info->pri_empty_exit_mmm==1)
                ||(tws_or_notws_flag != g_p_bt_stack_cur_info->dev_role)))
            {                	
                //保证主箱加载前，副箱已经卸载完
                //1s后再去清除丢数标志
                if (linein_tws_playpause_flag == 0)
                {
                	 libc_print("EE",0,0);
//                	 clear_filter_timer_id = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,
//                        500, Tws_clear_filter);
                   deal_linein_clear_filter();
                   enable_unfilter_flag = 1;
                }	

                btplay_engine_pipe_control(PIPE_OP_FLUSH);
            }
            else if((g_p_bt_stack_cur_info->dev_role==1)&&(g_p_tws_info->tts_start_flag==1)) //对方tts播完才能清除丢数的标志
            {
                btplay_engine_pipe_control(PIPE_OP_FLUSH);
                g_p_tws_info->tts_start_flag=0;
            }
            else
            {
                libc_print("@e3:",0,0);
                //btplay_engine_pipe_control(PIPE_OP_UNFILTER | PIPE_OP_FLUSH);
                btplay_engine_pipe_control(PIPE_OP_FLUSH);
                //if (linein_tws_playpause_flag == 0)
                {
                	if (g_p_bt_stack_cur_info->dev_role == 2)
                	{
                	   libc_print("@e4:",0,0);
                	   btplay_engine_pipe_control(PIPE_OP_UNFILTER);
                	}	                	
                }	
                
                //btplay_engine_pipe_control(PIPE_OP_FLUSH);
                
            }
            //  second_empty_flag=0;
        }
        send_pause_status();        
        tws_or_notws_flag = g_p_bt_stack_cur_info->dev_role;
        //if no pair so 
        if(tws_or_notws_flag == 0)
        {
        	g_need_adc_flag = 1;
        }
        second_empty_flag=0;

        //对方tts播完清除丢数标志
        if((g_p_tws_info->tts_stop_flag==1)&&(g_p_tws_info->tts_start_flag==0))
        {
            //btplay_engine_pipe_control(PIPE_OP_UNFILTER);
            libc_print("@j:",0,0);
            if(g_p_bt_stack_cur_info->dev_role==1)
            {
//                 set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL,
//                        500, Tws_clear_filter);
                 deal_linein_clear_filter();
                 enable_unfilter_flag = 1;       
            }
            else
            {
                 btplay_engine_pipe_control(PIPE_OP_UNFILTER);
            }
            g_p_tws_info->tts_stop_flag=0;
        }

#endif

        //处理当前应用的私有消息
        result = btplay_deal_app_msg();
        if (result != RESULT_NULL)
        {
            break;
        }

        /*when wait_paired, and no connect_request, and not bt_front, release more os_time.*/
        if (g_p_btplay_info->status == BTPLAY_PLAY)
        {
            change_engine_state(ENGINE_STATE_PLAYING);
        }
        else
        {
            change_engine_state(ENGINE_STATE_PAUSE);
        }
        //check_linein_data();
        //deal_linein_usb_noise();
        //挂起10ms，多任务调度
        sys_os_time_dly(1);
    }

    message_loop_exit();

    return result;
}

#if 0
void deal_linein_usb_noise(void)
{
		g_check_count++;

    //if vbat is higher than 3.90 we need to stop charging
    if(g_check_count >= COUNT_BOUND)
    {
      deal_linein_usb_noise_sub();
    }	
}
#endif

void tws_sync_cmd_deal(void)
{
	
  if (g_p_tws_info->tws_sync_cmd != 0)
  {
  	tws_sync_cmd_deal_sub();
  }	
  	
}


void pcm_data_deal(void)
{ 
   check_linein_data();   	
}


void move_data(uint32 addr,uint32 len)
{
//	 libc_print("addr:",addr,2);
//	 libc_print("len:",len,2);
//	 libc_print("mov_ptr:",mov_ptr,2);
	 libc_memcpy((uint8*)addr,SBC_ENC_BUFFER+mov_ptr,len);
	 mov_ptr += len;
	 mov_ptr %= (SBC_FRAME_LENGTH*SBC_ENCODE_FRAME_CNT);
//	 libc_print("f_len:",len,2);
}

void encode_sbc_data(uint32 buffer,uint8 len)
{
    //lineintws节点1 ADCbuf半满 A22
//    act_writel(act_readl(GPIOAOUTEN) | (1<<22), GPIOAOUTEN);
//    act_writel(act_readl(GPIOADAT) | (1<<22), GPIOADAT);
//    act_writel(act_readl(GPIOADAT) & (~(1<<22)), GPIOADAT);
    sbc_encode_param_t p_temp;
    uint32 wr_en,temp_flag;
    uint8 i;
    
    //p_temp.input_buffer = buffer;
    p_temp.input_len = 512;
    p_temp.bit_depth = 16;
    p_temp.gain = (1<<24);//4728462;
    p_temp.output_buffer = SBC_ENC_BUFFER;
    p_temp.output_len = SBC_FRAME_LENGTH;
    
    for (i=0;i<SBC_ENCODE_FRAME_CNT;i++)
    {
#if 1
    	p_temp.input_buffer = (buffer + 512*i);
    	p_temp.output_buffer = (SBC_ENC_BUFFER + SBC_FRAME_LENGTH*i);
    	bt_drv_sbc_encode(&sbc_obj,&p_temp,&wr_en);
#endif          	    	    	
    }

    //lineintws节点2 SBC编码 B7
//    act_writel(act_readl(GPIOBOUTEN) | (1<<7), GPIOBOUTEN);
//    act_writel(act_readl(GPIOBDAT) | (1<<7), GPIOBDAT);
//    act_writel(act_readl(GPIOBDAT) & (~(1<<7)), GPIOBDAT);


#if 1    
    temp_flag = sys_local_irq_save();
    update_pipe(&move_data,SBC_FRAME_LENGTH*SBC_ENCODE_FRAME_CNT);
    sys_local_irq_restore(temp_flag); 
#endif         	
}

void check_linein_data(void)
{
	  if ((act_readl(DMAIP) & 0x00000800) != 0)
    {
        //libc_print("half",0,0);
        //clear half IRQ Pending
        while ((act_readl(DMAIP) & 0x00000800) != 0)
        {
            act_writel(0x00000800, DMAIP);
        }
        encode_sbc_data(ADC_SAMPLE_BUFFER,SBC_ENCODE_FRAME_CNT);
    }
    else if ((act_readl(DMAIP) & 0x00000008) != 0)
    {
        //libc_print("Complete",0,0);
        //clear Complete IRQ Pending
        while ((act_readl(DMAIP) & 0x00000008) != 0)
        {
            act_writel(0x00000008, DMAIP);
        }
        encode_sbc_data(ADC_SAMPLE_BUFFER+ADC_SAMPLE_BUFFER_HALF,SBC_ENCODE_FRAME_CNT);
    }
    else
    {
        ;//qac
    }	
}
