/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：line in tws推歌引擎应用解码线程加载和卸载接口实现。
 * 作者：
 ********************************************************************************/

#include "linein_engine.h"

#ifdef ENABLE_TRUE_WIRELESS_STEREO

void send_exit_mmm_to_other(uint8 op)
{
    msg_apps_t msg;
    msg.content.data[0]=op;
    msg.type = MSG_BTSTACK_DECODE_QUIT_SYNC;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);
}

void __section__(".text.bank2")send_pause_status(void)
{
	  //暂停状态下组队成功,主箱告知从箱更新状态为暂停
    if (tws_or_notws_flag != g_p_bt_stack_cur_info->dev_role)
    {
    	if ((tws_or_notws_flag == 0)&&(g_p_bt_stack_cur_info->dev_role == 1)
    		 /* &&(linein_tws_playpause_flag == 1)*/)
    	{
    		if (linein_tws_playpause_flag == 1)
    		{ //send pause
    		  tws_sync_cmd_send(2);
    		}	
    		else
    		{ //send play
    			tws_sync_cmd_send(1);
    		}	    		
    	}
    	
    	if((tws_or_notws_flag == TWS_SLAVE)&&(g_p_bt_stack_cur_info->dev_role == 0))	
    	{
    		 libc_print("fd3:",0,0);
    		 msg_apps_t msg;
         msg.content.data[0] = PIPE_OP_FILTER;
         //消息类型(即消息名称)
         msg.type = MSG_BTSTACK_PLAY_PIPE_CTRL_SYNC;

         //发送同步消息
         send_sync_msg_btmanager(NULL, &msg, NULL, 0); 
    		
    	}	        	
    }	
} 

void __section__(".text.bank2")send_mmm_exit_cmd(void)
{
        
    //主要管理从给主发卸载命令并保证接收到主卸载完中间件的命令才可以加载
    //副箱full
    if((g_p_bt_stack_cur_info->dev_role!=0)&&((g_p_tws_info->sub_full_exit_flag==1)||(g_p_tws_info->pri_full_exit_flag==1)
                    ||(g_p_tws_info->pri_empty_exit_mmm==1)))
    {
        //标志需要卸载自己的中间件命令
        second_empty_flag=1;
    }
    //给主箱发命令
    if((second_empty_flag==1)&&(send_once_cmd==0))
    {
        send_exit_mmm_to_other(1);
        send_once_cmd++;
        //libc_print("FF",0,0);
    }
    //保证每次只发生一个start命令给stack
    if(second_empty_flag==0)
    {
        send_once_cmd=0;
    }
    //去掉tts播报后主箱暂停通知副箱
    if(g_app_info_state_all.g_send_exit_mmm_tts==2)
     {
         g_app_info_state_all.g_send_exit_mmm_tts=0;
         send_mmm_exit_cmd2(1);
         send_once_cmd=0;
     }

}

void send_mmm_exit_cmd2(uint8 op)
{
    if(send_once_cmd==0)
    {
        send_exit_mmm_to_other(op);
        send_once_cmd++;
    }
}

void send_tts_stop_cmd(void)
{
    //tts播报结束 清除丢数标志
    if (g_app_info_state_all.g_send_exit_mmm_tts==1)
    {
        g_app_info_state_all.g_send_exit_mmm_tts= 0;
        libc_print("G5",0,0);
        send_mmm_exit_cmd2(3);
        send_once_cmd=0;
    }

}

void send_tts_start_cmd(void)
{
    if (g_app_info_state_all.g_send_exit_mmm_tts == 1)
    {
        //g_app_info_state_all.g_send_exit_mmm_tts= 0;
        libc_print("G2", 0, 0);
        send_mmm_exit_cmd2(2);
        send_once_cmd = 0;
    }

}

#endif
