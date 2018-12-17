/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙自动回连远程设备管理模块，回连策略通过该模块定制；该模块需要挂在2Hz的
 *       软定时器运行。
 * 作者：cailizhen
 ********************************************************************************/

#include  "common_func.h"

//回连不再需要使用USB检测定时器，我们会直接在前台的消息循环中获取btstack状态，并决定是否需要回连
//回连定时器可以采用0.5S定时器，先在btplay中挂0.5S定时器进行回连

//com_connect_tick_cnt计数的时间单位 ms
#define COM_AUTOCONNECT_TICK_INTEVAL  (500)

//启动连接调频后，30秒钟后就会将频率强制调到WORKING
#define LINKING_TO_WORKING_TIMER      (60)


void check_auto_con_flag()__FAR__;
//交替回连代码
#ifdef ENABLE_TRUE_WIRELESS_STEREO
void com_exchange_vale(auto_conn_dev_info_t *p_dev_info,uint8 i,uint8 j)__FAR__;

extern uint8 check_id1;
extern uint8 check_id2;
extern uint8 once_auto_flag;

uint8 __section__(".text.auto_connect_bank") com_get_dev_conn_init(void)
{
    msg_apps_t msg;
    msg_reply_t temp_reply;
    uint8 already_conn_init;
    temp_reply.content = &already_conn_init;
    
    msg.type = MSG_BTSTACK_GET_BTSTACK_CONN_INIT_SYNC;
    
    send_sync_msg_btmanager(g_bt_auto_connect_addr
        , &msg, &temp_reply, 0);
   
    return already_conn_init;
}

#endif

#ifndef ENABLE_TRUE_WIRELESS_STEREO
uint8 __section__(".text.auto_connect_bank") auto_connect_deal(void)
{
    uint8 i;

    PRINT_DBG("20s");

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_auto_connect_ctrl.dev_info[i].conn_flag == 0)
        {
            continue;
        }

        PRINT_DBG_INT("i=", i);
        //回连次数限制
        //三种回连模式
        if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_TIMEOUT)
                && (g_bt_auto_connect_ctrl.auto_connect_count_timeout != 0))
        {
            if ((g_bt_auto_connect_ctrl.dev_info[i].connect_cnt >= g_bt_auto_connect_ctrl.auto_connect_count_timeout))
            {
                PRINT_DBG_INT("cnt:", g_bt_auto_connect_ctrl.dev_info[i].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_MODE)
                && (g_bt_auto_connect_ctrl.auto_connect_count_mode != 0))
        { //回连有限次
            if ((g_bt_auto_connect_ctrl.dev_info[i].connect_cnt >= g_bt_auto_connect_ctrl.auto_connect_count_mode))
            {
                PRINT_DBG_INT("mode:", g_bt_auto_connect_ctrl.dev_info[i].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_STARTUP)
                && (g_bt_auto_connect_ctrl.auto_connect_count_startup != 0))
        {
            if ((g_bt_auto_connect_ctrl.dev_info[i].connect_cnt >= g_bt_auto_connect_ctrl.auto_connect_count_startup))
            {
                PRINT_DBG_INT("start:", g_bt_auto_connect_ctrl.dev_info[i].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else
        {
            //do nothing for QAC
        }

        g_bt_auto_connect_ctrl.dev_info[i].connect_cnt++;

        if (((g_need_reset_controller_timeout == 1) && (g_bt_auto_connect_ctrl.dev_info[i].connect_cnt > 3))
                || (g_bt_auto_connect_ctrl.dev_info[i].connect_cnt >= 100))
        {
            //转发给COMMON，由COMMON重新装载BT STACK，且需要恢复回连
            msg_apps_t msg;

            if (g_this_app_info->app_id == APP_ID_BTPLAY)
            {
                msg.type = MSG_BTSTACK_ERR_HARDWARE_EXCEPTION;
            }
            else
            {
                msg.type = MSG_BTSTACK_ERR_HARDWARE;
            }        

            broadcast_msg(&msg, TRUE);

            goto NEED_TO_STOP_AUTOCONNECT;
        }

        libc_memcpy(g_bt_auto_connect_addr, g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes, BD_ADDR_LEN);

        if (((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) HFP_SUPPORTED) != 0)
        /*&& ((g_bt_stack_cur_info.serv_conn & HFP_CONNECTED) == 0)*/)
        {
            return AUTO_CONNECT_NEED_CONNECT_HFP; //每次只连一个设备
        }
    #ifdef __SUPPORT_HID_
        else if ((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) A2DP_SUPPORTED) != 0)
        {
            return AUTO_CONNECT_NEED_CONNECT_A2DP; //每次只连一个设备 
        }
        else //if((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) HID_SUPPORTED) != 0)
        {
            return AUTO_CONNECT_NEED_CONNECT_HID; //每次只连一个设备 
        }
    #else
        else //if ((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) A2DP_SUPPORTED) != 0)
        {
            return AUTO_CONNECT_NEED_CONNECT_A2DP; //每次只连一个设备 
        }
    #endif

        NEED_TO_STOP_AUTOCONNECT:
        //停止自动回连
        g_bt_auto_connect_ctrl.dev_info[i].connect_cnt = 0;
        g_bt_auto_connect_ctrl.dev_info[i].conn_flag = 0;
        return 0;
    }
}
#else

//交替回连修改代码
uint8 __section__(".text.auto_connect_bank") auto_connect_deal(void)
{
    uint8 i,j=0,ret=0;
    uint8 tmp_already_conn_init = 0;
    uint8 tmp_auto_connect_count_startup = g_bt_auto_connect_ctrl.auto_connect_count_startup;

  //  PRINT_DBG("20s");

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {

        if(check_id1 >= g_btmanager_gl_var.support_dev_num)
        {
            check_id1 = 0;
        }
                            
        if (g_bt_auto_connect_ctrl.dev_info[check_id1].conn_flag != 1)
        {
            j = j + 1;     
            if(j == g_btmanager_gl_var.support_dev_num)
            {
                g_bt_auto_connect_ctrl.need_auto_connect = 0;
            }
            
            if(++check_id1 >= g_btmanager_gl_var.support_dev_num)
            {
                check_id1 = 0;
            }
            continue;
        }
        libc_print("check_id1 =",check_id1,2);

       // PRINT_DBG_INT("i=", i);
        //回连次数限制
        //三种回连模式
        if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_TIMEOUT)
                && (g_bt_auto_connect_ctrl.auto_connect_count_timeout != 0))
        {
            if ((g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt >= g_bt_auto_connect_ctrl.auto_connect_count_timeout))
            {
                //PRINT_DBG_INT("cnt:", g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_MODE)
                && (g_bt_auto_connect_ctrl.auto_connect_count_mode != 0))
        { //回连有限次
            if ((g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt >= g_bt_auto_connect_ctrl.auto_connect_count_mode))
            {
              //  PRINT_DBG_INT("mode:", g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else if ((g_bt_auto_connect_ctrl.auto_connect_type == AUTO_CONNECT_TYPE_STARTUP)
                && (g_bt_auto_connect_ctrl.auto_connect_count_startup != 0))
        {
            if ((g_bt_auto_connect_ctrl.dev_info[check_id1].dev_type == TWS_DEV)
                  && (g_bt_auto_connect_ctrl.tws_prior_role == TWS_MASTER))
            {
                 tmp_auto_connect_count_startup = g_bt_auto_connect_ctrl.auto_connect_count_startup + 3;
            }    
            if ((g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt >= tmp_auto_connect_count_startup))
            {
             //   PRINT_DBG_INT("start:", g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt);
                goto NEED_TO_STOP_AUTOCONNECT;
            }
        }
        else
        {
            //do nothing for QAC
        }

        g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt++;

/*        if (((g_need_reset_controller_timeout == 1) && (g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt > 3))
                || (g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt >= 100))
        {
            //转发给COMMON，由COMMON重新装载BT STACK，且需要恢复回连
            msg_apps_t msg;

            msg.type = MSG_BTSTACK_ERR_HARDWARE_EXCEPTION;
            broadcast_msg(&msg, TRUE);

            goto NEED_TO_STOP_AUTOCONNECT;
        }
*/
        libc_memcpy(g_bt_auto_connect_addr, g_bt_auto_connect_ctrl.dev_info[check_id1].remote_addr.bytes, BD_ADDR_LEN);

    //确定一次回连没成功，那么就回连下一个设备  !!
        tmp_already_conn_init = com_get_dev_conn_init();
        if((tmp_already_conn_init == 1) && (g_bt_stack_cur_info.rmt_dev[check_id1].serv_conn == 0))
        {
            //解决当两箱都连接过手机出现一直check_id1 =0的问题
            //if(++check_id1 >= g_btmanager_gl_var.support_dev_num)
            if(check_id1 >= g_btmanager_gl_var.support_dev_num)
            {
                check_id1 = 0;
            }
            continue;
        }

        //if (((g_bt_auto_connect_ctrl.dev_info[check_id1].support_profile & (uint8) HFP_SUPPORTED) != 0)
        //if ((((g_bt_auto_connect_ctrl.dev_info[check_id1].support_profile & (uint8) HFP_SUPPORTED) != 0)&&(g_bt_auto_connect_ctrl.dev_info[check_id1].dev_type!=TWS_DEV))
        if (((g_bt_auto_connect_ctrl.dev_info[check_id1].support_profile & (uint8) HFP_SUPPORTED) != 0))
            // && (( g_bt_stack_cur_info.rmt_dev[check_id1].serv_conn & HFP_CONNECTED) == 0))
        {
            ret=AUTO_CONNECT_NEED_CONNECT_HFP; //每次只连一个设备
        }    
        else //if ((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) A2DP_SUPPORTED) != 0)
        {
            ret=AUTO_CONNECT_NEED_CONNECT_A2DP; //每次只连一个设备 
        }
        //else //if((g_bt_auto_connect_ctrl.dev_info[i].support_profile & (uint8) HID_SUPPORTED) != 0)
        //{
        //    ret=AUTO_CONNECT_NEED_CONNECT_HID; //每次只连一个设备 
        //}
        
        if(ret != 0)
        {
            if(++check_id1 >= g_btmanager_gl_var.support_dev_num)
            {
                check_id1 = 0;
            }
            return ret;
        }
        
        NEED_TO_STOP_AUTOCONNECT:
        //停止自动回连
        g_bt_auto_connect_ctrl.dev_info[check_id1].connect_cnt = 0;
        g_bt_auto_connect_ctrl.dev_info[check_id1].conn_flag = 0;
        if(++check_id1 >= g_btmanager_gl_var.support_dev_num)
        {
            check_id1 = 0;
        }
        return 0;

    }
}
#endif

//检测是否还有服务没有连上
uint8 __section__(".text.auto_connect_bank") connect_remain_profile_bank(auto_conn_dev_info_t *p_dev_info,
        uint8 need_connect)
{
    if (need_connect == AUTO_CONNECT_NEED_CONNECT_A2DP)
    {
        //try to connect again, up to the reconnect another limit
        if ((g_bt_auto_connect_ctrl.reconnect_another_limit == 0) ||
                (p_dev_info->reconnect_another_cnt < g_bt_auto_connect_ctrl.reconnect_another_limit))
        {
            /* if phone is ongoing, wait.  */
            if (g_bt_stack_cur_info.conn_status == CONN_STATUS_LINKED)
            {
                p_dev_info->onlyone_disconnect_cnt = 0;
                p_dev_info->reconnect_another_cnt++;

                return AUTO_CONNECT_NEED_CONNECT_A2DP;
            }
        }
    }
    else if(need_connect == AUTO_CONNECT_NEED_CONNECT_HID)
    {
        //
                //try to connect again, up to the reconnect another limit
        if ((g_bt_auto_connect_ctrl.reconnect_another_limit == 0) ||
                (p_dev_info->reconnect_another_cnt < g_bt_auto_connect_ctrl.reconnect_another_limit))
        {
            p_dev_info->onlyone_disconnect_cnt = 0;
            p_dev_info->reconnect_another_cnt++;

            return AUTO_CONNECT_NEED_CONNECT_HID;
        }
    }
    else 
    {
        //try to connect again, up to the reconnect another limit
        if ((g_bt_auto_connect_ctrl.reconnect_another_limit == 0) ||
                (p_dev_info->reconnect_another_cnt < g_bt_auto_connect_ctrl.reconnect_another_limit))
        {
            p_dev_info->onlyone_disconnect_cnt = 0;
            p_dev_info->reconnect_another_cnt++;

            return AUTO_CONNECT_NEED_CONNECT_HFP;
        }
    }

    return 0;
}



void __section__(".auto_connect_bank1") check_auto_con_flag()
{
    uint8 i,j=0;
    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (libc_memcmp(g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes,
                g_bt_stack_cur_info.rmt_dev[i].addr.bytes, 6) == 0)
        {
            if (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HFP_CONNECTED) != 0)
                    || ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) != 0))
            {
                //如果已经连上，标记一下，这样就不需要再回连了
                g_bt_auto_connect_ctrl.dev_info[i].connect_cnt = 0;
                g_bt_auto_connect_ctrl.dev_info[i].conn_flag = 0;
#ifdef ENABLE_TRUE_WIRELESS_STEREO
                if ((g_bt_auto_connect_ctrl.dev_info[1-i].conn_flag == 1)
                      && (g_bt_auto_connect_ctrl.need_auto_connect == 1)
                      && (once_auto_flag == 0))
                {
                    g_bt_auto_connect_ctrl.connect_tick_cnt = g_bt_auto_connect_ctrl.auto_connect_interval * 2;
                    once_auto_flag = 1;
                   // PRINT_INFO("hgg");
                }    
#endif    
            }
        }
    }  
}


bool __section__(".text.auto_connect_profile_bank") stop_auto_con(uint8 i)
{
    uint8 h;
    bool ret=TRUE;
    if(g_btmanager_gl_var.support_dev_num==1)
    {
         //找到和和远端设备相同地址的回连设备id
        for(h=0;h<2;h++)
        {
            g_bt_auto_connect_ctrl.dev_info[h].connect_cnt = 0;
            g_bt_auto_connect_ctrl.dev_info[h].conn_flag = 0;
        }  
    }
    else
    {
        //找到和和远端设备相同地址的回连设备id
        for(h=0;h<2;h++)
        {
              if (libc_memcmp(g_bt_auto_connect_ctrl.dev_info[h].remote_addr.bytes,
                g_bt_stack_cur_info.rmt_dev[i].addr.bytes, 6) == 0)
              {
                 break;
              } 
        }
        if(h>=2)
        {
            ret=FALSE;
            return ret;
        }
        g_bt_auto_connect_ctrl.dev_info[h].connect_cnt = 0;
        g_bt_auto_connect_ctrl.dev_info[h].conn_flag = 0;
    }
    return ret;
}


#ifdef ENABLE_TRUE_WIRELESS_STEREO

//检测是否还有服务没有连上
void __section__(".auto_connect_bank1") com_exchange_vale(auto_conn_dev_info_t *p_dev_info,uint8 i,uint8 j)
{
    #if 1
     libc_print("\nexchange i=:",i,2);
    libc_memcpy(p_dev_info, &g_bt_auto_connect_ctrl.dev_info[i], sizeof(auto_conn_dev_info_t));
    libc_memcpy(p_dev_info->remote_addr.bytes,g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes, 6);

    libc_memcpy(&g_bt_auto_connect_ctrl.dev_info[i], &g_bt_auto_connect_ctrl.dev_info[j], sizeof(auto_conn_dev_info_t));
    libc_memcpy(g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes, g_bt_auto_connect_ctrl.dev_info[j].remote_addr.bytes, 6);

    libc_memcpy(&g_bt_auto_connect_ctrl.dev_info[j], p_dev_info, sizeof(auto_conn_dev_info_t));
    libc_memcpy(g_bt_auto_connect_ctrl.dev_info[j].remote_addr.bytes,  p_dev_info->remote_addr.bytes, 6);
    #endif
}

//检测是否还有服务没有连上
uint8 connect_remain_profile(void)
{
    
    uint8 i;
    uint8 need_connect = 0;
    uint8 tmp_already_conn_init = 0;

    if(check_id2 >= g_btmanager_gl_var.support_dev_num)
    {
        check_id2 = 0;
    }

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        libc_memcpy(g_bt_auto_connect_addr, g_bt_stack_cur_info.rmt_dev[check_id2].addr.bytes, BD_ADDR_LEN);

    //确定本次回连没成功，回连下一个设备的服务
       // tmp_already_conn_init = com_get_dev_conn_init();
       // if((tmp_already_conn_init == 1) && (g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn == 0))
       if(g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn == 0)
        {
            if(++check_id2 >= g_btmanager_gl_var.support_dev_num)
            {
                check_id2 = 0;
            }
            continue;
        }
    //    libc_print("check_id2 =",check_id2,2);

        //主箱或者从箱不检测tws设备此项
        if (!( ((g_bt_stack_cur_info.dev_role == 1) || (g_bt_stack_cur_info.dev_role == 2))
              &&(g_bt_stack_cur_info.rmt_dev[check_id2].dev_type == TWS_DEV)))
        {
        if ((g_bt_auto_connect_ctrl.enable_a2dp == 1)
                && ((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & A2DP_CONNECTED) == 0)
                && ((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & HFP_CONNECTED) != 0))
        {
            if (g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt > A2DP_CONNECT_TIME_AFTER_HFP_CONNECT)
            {
                need_connect = connect_remain_profile_bank(&g_bt_auto_connect_ctrl.dev_info[check_id2],
                        AUTO_CONNECT_NEED_CONNECT_A2DP);
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt++;
            }
        }
        else if ((g_bt_auto_connect_ctrl.enable_hfp == 1)
                && ((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & HFP_CONNECTED) == 0)
                && ((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & A2DP_CONNECTED) != 0))
        {
            if (g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt > HFP_CONNECT_TIME_AFTER_A2DP_CONNECT)
            {
                need_connect = connect_remain_profile_bank(&g_bt_auto_connect_ctrl.dev_info[check_id2],
                        AUTO_CONNECT_NEED_CONNECT_HFP);
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt++;
            }
        }
        else
        {
            g_bt_auto_connect_ctrl.dev_info[check_id2].reconnect_another_cnt = 0;
            g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt = 0;
        }
        }
        else
        {
               g_bt_auto_connect_ctrl.dev_info[check_id2].reconnect_another_cnt = 0;
               g_bt_auto_connect_ctrl.dev_info[check_id2].onlyone_disconnect_cnt = 0;
        }

        //主箱不检测从箱此项
        if (!( (g_bt_stack_cur_info.dev_role == 1)
              &&(g_bt_stack_cur_info.rmt_dev[check_id2].dev_type == TWS_DEV) ))    
        {      
        //检测是否需要连接AVRCP
        if (((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & AVRCP_CONNECTED_MY) == 0)
                && ((g_bt_stack_cur_info.rmt_dev[check_id2].serv_conn & A2DP_CONNECTED) != 0))
        //倘若A2DP已连接且AVRCP未连接
        {
            if (g_bt_auto_connect_ctrl.dev_info[check_id2].avrcp_disconnect_cnt > CHECK_AVRCP_CONNECT_TIME)
            {
                need_connect = AUTO_CONNECT_NEED_CONNECT_AVRCP;

                g_bt_auto_connect_ctrl.dev_info[check_id2].avrcp_disconnect_cnt = 0;
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[check_id2].avrcp_disconnect_cnt++;
            }
        }
        else
        {
            g_bt_auto_connect_ctrl.dev_info[check_id2].avrcp_disconnect_cnt = 0;
        }

        if (need_connect != 0)
        {
            break; //每次只连一个设备
        }
        }
        else
        {
             g_bt_auto_connect_ctrl.dev_info[check_id2].avrcp_disconnect_cnt = 0;
        }    
        if(++check_id2 >= g_btmanager_gl_var.support_dev_num)
        {
            check_id2 = 0;
        }

    }
    return need_connect;
    
}
#else
//检测是否还有服务没有连上
uint8 connect_remain_profile(void)
{
    uint8 i;
    uint8 need_connect = 0;

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        libc_memcpy(g_bt_auto_connect_addr, g_bt_stack_cur_info.rmt_dev[i].addr.bytes, BD_ADDR_LEN);

        if ((g_bt_auto_connect_ctrl.enable_a2dp == 1)
                && ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) == 0)
                && (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HFP_CONNECTED) != 0)
                    ||((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HID_CONNECTED)!=0)))
        {
            if (g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt > A2DP_CONNECT_TIME_AFTER_HFP_CONNECT)
            {
                need_connect = connect_remain_profile_bank(&g_bt_auto_connect_ctrl.dev_info[i],
                        AUTO_CONNECT_NEED_CONNECT_A2DP);
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt++;
            }
        }
        else if ((g_bt_auto_connect_ctrl.enable_hfp == 1)
                && ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HFP_CONNECTED) == 0)
                && (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) != 0)
                     ||((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HID_CONNECTED)!=0)))
        {
            if (g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt > HFP_CONNECT_TIME_AFTER_A2DP_CONNECT)
            {
                need_connect = connect_remain_profile_bank(&g_bt_auto_connect_ctrl.dev_info[i],
                        AUTO_CONNECT_NEED_CONNECT_HFP);
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt++;
            }
        }
        else if ((g_bt_auto_connect_ctrl.enable_hid == 1)
                && ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HID_CONNECTED) == 0)
                && (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) != 0)
                     ||((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HFP_CONNECTED)!=0)))
        {
            if (g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt > HID_CONNECT_TIME_AFTER_OTHER_CONNECT)
            {
                need_connect = connect_remain_profile_bank(&g_bt_auto_connect_ctrl.dev_info[i],
                        AUTO_CONNECT_NEED_CONNECT_HID);
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt++;
            }
        }
        else
        {
            g_bt_auto_connect_ctrl.dev_info[i].reconnect_another_cnt = 0;
            g_bt_auto_connect_ctrl.dev_info[i].onlyone_disconnect_cnt = 0;
        }

        //检测是否需要连接AVRCP
        if (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & AVRCP_CONNECTED_MY) == 0)
                && ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) != 0))
        //倘若A2DP已连接且AVRCP未连接
        {
            if (g_bt_auto_connect_ctrl.dev_info[i].avrcp_disconnect_cnt > CHECK_AVRCP_CONNECT_TIME)
            {
                need_connect = AUTO_CONNECT_NEED_CONNECT_AVRCP;

                g_bt_auto_connect_ctrl.dev_info[i].avrcp_disconnect_cnt = 0;
            }
            else
            {
                g_bt_auto_connect_ctrl.dev_info[i].avrcp_disconnect_cnt++;
            }
        }
        else
        {
            g_bt_auto_connect_ctrl.dev_info[i].avrcp_disconnect_cnt = 0;
        }

        if (need_connect != 0)
        {
            break; //每次只连一个设备
        }
    }

    return need_connect;
}
#endif

void check_connect_status(void)
{
    uint8 i, j = 0;
    auto_conn_dev_info_t tmp_dev;
    //PRINT_DATA(g_bt_auto_connect_ctrl.dev_info[0].remote_addr.bytes,6);
    //PRINT_DATA(g_bt_auto_connect_ctrl.dev_info[1].remote_addr.bytes,6);
    if (g_bt_stack_cur_info.num_connected == g_btmanager_gl_var.support_dev_num)
    {
        for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
        {
            if (((g_bt_stack_cur_info.rmt_dev[i].serv_conn & HFP_CONNECTED) != 0)
                    || ((g_bt_stack_cur_info.rmt_dev[i].serv_conn & A2DP_CONNECTED) != 0))
            {

                //切bank
                stop_auto_con(i);
                j++;
                //PRINT_INFO("clear flag");            
                //g_bt_auto_connect_ctrl.dev_info[i].connect_cnt = 0;
                //g_bt_auto_connect_ctrl.dev_info[i].conn_flag = 0;

            }
        }

        if ((j == g_btmanager_gl_var.support_dev_num)
                && (g_bt_auto_connect_ctrl.need_auto_connect == 1))
        {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
           once_auto_flag = 0;
#endif
            g_bt_auto_connect_ctrl.need_auto_connect = 0x0;
            PRINT_INFO("stop auto-conn1");
        }
        return;
    }

    if (g_bt_stack_cur_info.num_connected != 0)
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        //添加条件 当副箱连接上了主箱的时候，那么就不再去回连手机
        if((g_bt_stack_cur_info.dev_role == TWS_SLAVE)&&( g_bt_auto_connect_ctrl.need_auto_connect!=0))
        {
             g_bt_auto_connect_ctrl.need_auto_connect = 0x0;
             PRINT_INFO("stop auto-conn2");
        }
        //调整设备回连顺序
        if(g_bt_stack_cur_info.dev_role==TWS_MASTER)
        {
            for (i = 0; i < g_bt_stack_cur_info.num_connected ; i++)
            {
                for(j = 0;j < g_btmanager_gl_var.support_dev_num;j++)
                {
                    //调整回连设备顺序
                    if ((libc_memcmp(g_bt_auto_connect_ctrl.dev_info[j].remote_addr.bytes,
                         g_bt_stack_cur_info.rmt_dev[i].addr.bytes, 6) == 0) && (i != j))
                    {
                        com_exchange_vale(&tmp_dev,i,j);
                        break;                        
                    }    
                }
             }
        }
#endif
        if(g_bt_auto_connect_ctrl.need_auto_connect!=0)
        {
            //切bank,该bank空间紧张 
            check_auto_con_flag(); 
        }      
    }

    //如果每个设备都不需要回连，则停止回连
    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (g_bt_auto_connect_ctrl.dev_info[i].conn_flag == 1)
        {
            break;
        }
    }
    if ((i == g_btmanager_gl_var.support_dev_num)
            && (g_bt_auto_connect_ctrl.need_auto_connect == 1))
    {
#ifdef ENABLE_TRUE_WIRELESS_STEREO
       once_auto_flag = 0;
#endif
        g_bt_auto_connect_ctrl.need_auto_connect = 0x0;
#ifdef __ESD_MODE_
        g_end_autoconnt = 1;
#endif
        PRINT_INFO("stop auto-conn");
    }
}

void __section__(".text.auto_connect_bank") auto_connect_profile(uint8 need_connect)
{
    msg_apps_t conn_msg;

    g_btmanager_gl_var.autoconnect_link_succ_flag = 1;

    if ((need_connect & AUTO_CONNECT_NEED_CONNECT_HFP) != 0)
    {
        conn_msg.type = MSG_BTSTACK_HFP_CONNECT_SYNC;
    }
    else if ((need_connect & AUTO_CONNECT_NEED_CONNECT_A2DP) != 0)
    {
        conn_msg.type = MSG_BTSTACK_A2DP_CONNECT_SYNC;
    }
    else if((need_connect & AUTO_CONNECT_NEED_CONNECT_HID) != 0)
    {
        conn_msg.type = MSG_BTSTACK_HID_CONNECT_SYNC;
    }
    else //if ((need_connect & AUTO_CONNECT_NEED_CONNECT_A2DP) != 0)
    {
        conn_msg.type = MSG_BTSTACK_AVRCP_CONNECT_SYNC;
    }

    //设置BT MANAGER状态为“连接”
    if (g_btmanager_gl_var.btmanager_status != BTMANAGER_STATUS_LINKING)
    {
        change_btmanager_status(BTMANAGER_STATUS_LINKING);
    }

    send_sync_msg_btmanager(g_bt_auto_connect_addr, &conn_msg, NULL, 0);
}

/*COMMON 后台蓝牙超时断开后回连统一处理*/
void com_btmanager_autoconnect(void)
{
    uint8 need_connect = 0;

    //uint8 weixin_flag;

    //btstack没有初始化完成的话,此时发起回连,不成功,需要间隔
    //20s才发起下一次回连.避免浪费20s,加快回连成功时间
    if (g_bt_stack_cur_info.conn_status == CONN_STATUS_NONE)
    {
        return;
    }

    check_connect_status();

    if (g_bt_auto_connect_ctrl.need_auto_connect == 1)
    {
        g_bt_auto_connect_ctrl.connect_tick_cnt++;
        if (g_bt_auto_connect_ctrl.connect_tick_cnt >= (g_bt_auto_connect_ctrl.auto_connect_interval * 2))
        {
            need_connect = auto_connect_deal();
            g_bt_auto_connect_ctrl.connect_tick_cnt = 0;
            g_ap_switch_var.tws_phon_come_flag=0;
        }
    }
    else
    {
        need_connect = connect_remain_profile();
        g_bt_auto_connect_ctrl.connect_tick_cnt = 0;
    }

    if (need_connect != 0)
    { //有且只有一个服务要发起连接
        auto_connect_profile(need_connect);
    }

    if ((g_btmanager_gl_var.btmanager_status == BTMANAGER_STATUS_POWERON)   
      ||(g_btmanager_gl_var.btmanager_status == BTMANAGER_STATUS_LINKING))
    {
        g_btmanager_gl_var.linking_timer++;
        if (g_btmanager_gl_var.linking_timer > LINKING_TO_WORKING_TIMER)
        {
            change_btmanager_status(BTMANAGER_STATUS_WORKING);
        }
    }

    //APK/APP校验
#ifndef SUPPORT_SPPBLE_TEST
    //weixin_flag = (uint8) com_get_config_default(BTSTACK_ENABLE_WEIXIN);
    //#if 0
    //if (weixin_flag == 0)
    //{
    com_btmanager_apk_app_verification_handle();
    //}
#endif
}
