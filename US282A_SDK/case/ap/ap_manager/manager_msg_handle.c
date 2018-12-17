/*******************************************************************************
 *                              US212A
 *                            Module: Manager
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     manager_msg_handle.c
 * \brief    消息处理模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include  "manager.h"

extern void manager_msg_callback_sub(private_msg_t *pri_msg);

/******************************************************************************/
/*!
 * \par  Description:
 *    消息处理
 * \param[in]    私有消息指针
 * \param[out]   none
 * \return       none
 * \retval
 * \retval
 * \ingroup      module name
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 *******************************************************************************/
void manager_msg_callback(private_msg_t *pri_msg)
{
    uint8 ap_name[12];
    uint8 app_id;
    uint8 ap_param;

    if(g_app_info_state.inner_pa_inited == TRUE)
    {
        if(pa_thread_task_addr != 0)
        {
            sys_free_large_data(pa_thread_task_addr);

            pa_thread_task_addr = 0;
        }
    }

    switch (pri_msg->msg.type)
    {
        //创建前台应用
        case MSG_CREAT_APP:
        {
            adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO,FREQ_LEVEL7,FREQ_NULL);
            
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            
            wait_ap_exit();

            manager_mem_manager(prev_front_ap_id, FALSE);                
                
            sys_free_ap(FALSE);

            _get_app_name(ap_name, app_id, APP_TYPE_GUI);          

            manager_mem_manager(app_id, TRUE);

            prev_front_ap_id = app_id;
            
            sys_exece_ap(ap_name, 0, (int32) ap_param);
            
            adjust_freq_set_level(AP_PROCESS_MANAGER_PRIO,FREQ_LEVEL3,FREQ_NULL);
        }
        break;

        //创建后台应用
        case MSG_CREAT_APP_SYNC:
        {
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            
            //应用约定如果另一个后台存在，需要先把当前后台杀死，然后再创建需要的后台
            if (g_app_info_vector[APP_TYPE_CONSOLE].used == 0)
            {
                //wait for cur app exit
                libc_waitpid(0, 1);

                if(prev_back_ap_id != APP_ID_MAX)
                {
                    manager_mem_manager(prev_back_ap_id, FALSE);
                                        
                    sys_free_ap(TRUE);
                }
                else
                {
                    sys_free_ap(TRUE);
                }

                prev_back_ap_id = app_id;
                                              
                _get_app_name(ap_name, app_id, APP_TYPE_CONSOLE);

                manager_mem_manager(app_id, TRUE);
                
                if (sys_exece_ap(ap_name, 1, (int32) ap_param) >= 0)
                {
                    pri_msg->reply->type = MSG_REPLY_SUCCESS;
                }
                else
                {
                    pri_msg->reply->type = MSG_REPLY_FAILED;
                }
                libc_sem_post(pri_msg->sem);
            }
            else
            //如果后台存在才处理
            {
                pri_msg->reply->type = MSG_REPLY_SUCCESS;
                libc_sem_post(pri_msg->sem);
            }
        }
        break;

        case MSG_KILL_APP_SYNC:
        //如果后台存在才处理
        {
            app_id = pri_msg->msg.content.data[0];
            ap_param = pri_msg->msg.content.data[1];
            if (app_id == APP_ID_THEENGINE)
            {
                app_id = get_engine_appid_by_type(get_engine_type());
            }
            
            if (app_id == APP_ID_MAX)
            {
                goto kill_over;
            }
            
            if (((app_id == APP_ID_BTSTACK) && (g_app_info_vector[APP_TYPE_BTSTACK].used != 0))
             || ((app_id != APP_ID_BTSTACK) && (g_app_info_vector[APP_TYPE_CONSOLE].used != 0)))
            {
                //杀死后台
                kill_app(app_id, ap_param);
                if (app_id == APP_ID_BTSTACK)
                {
                    sys_free_ap(AP_BTSTACK_FILE);
                }
                else
                {
                    sys_free_ap(1);
                }
            }
            
            //如果应用已经不存在，直接返回成功，避免前台收不到响应而死等
            kill_over:
            pri_msg->reply->type = MSG_REPLY_SUCCESS;
            libc_sem_post(pri_msg->sem);
        }
        break;

        default:
        manager_msg_callback_sub(pri_msg);
        break;
    }

}
#if 1
//extern mem_use_info_t *g_mem_use_info;

void dump_cache_info(void)
{

    mem_pool_info_t *mem_pool_info;
    bank_info_t *bank_info;

    uint32 pool_size;
    uint32 pool_addr;

    uint32 cache_bank_num = 0;
        
    libc_print("free bdata: ", g_mem_use_info->free_bt_data, 2);
    
    libc_print("use bdata: ", g_mem_use_info->free_bt_data_peak, 2);

    libc_print("--------------------\n", 0, 0);

    mem_pool_info = *(g_mem_use_info->mem_pool);

    bank_info = *(g_mem_use_info->bank_info);

    while(mem_pool_info != NULL)
    {
        pool_size = (uint32)(((1 << mem_pool_info->blk_size) * mem_pool_info->total_index * 8));

        libc_print("mem pool addr: 0x", __addr32(mem_pool_info->addr), 2);
        libc_print("size: 0x", pool_size , 2);

        mem_pool_info = (mem_pool_info_t *)__addr32(mem_pool_info->next);

        libc_print("  ", 0, 0);
    }

    libc_print("--------------------\n", 0, 0);

    while(bank_info != NULL)
    {
        libc_print("cache VA: 0x", __addr32(bank_info->vaddr), 2);
        libc_print("cache PA: 0x", __addr32(bank_info->pmaddr), 2);
        libc_print("size: 0x", bank_info->size, 2);

        libc_print("  ", 0, 0);

        bank_info = (bank_info_t *)__addr32(bank_info->next);

        cache_bank_num++;
    } 

    libc_print("--------------------\n", 0, 0);
   
    libc_print("cache bank: ", cache_bank_num, 2);

}
#endif
