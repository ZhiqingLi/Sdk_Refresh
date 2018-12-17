/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：RCP 命令分发处理接口，需要进行物理包重组和拆分；检查RCP命令是否需要中断TTS。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"

//rcp dialog
uint8 __section__(".gl_rcp_data") g_dialog_state;
uint8 __section__(".gl_rcp_data") g_dialog_overtime_flag;
uint8 __section__(".gl_rcp_data") g_dialog_overtime;
uint8 __section__(".gl_rcp_data") g_dialog_choosed;
uint8 __section__(".gl_rcp_data") g_dialog_type;
uint8 __section__(".gl_rcp_data") g_dialog_button_type;
uint8 __section__(".gl_rcp_data") g_dialog_desc_id;
uint8 __section__(".gl_rcp_data") g_dialog_active_default;

uint8 __section__(".gl_rcp_data") g_rcp_cmd_flags[2]; //RCP命令标志字
uint8 __section__(".gl_rcp_data") g_rcp_cmd_id; //当前正在处理命令ID
uint8 __section__(".gl_rcp_data") g_rcp_next_seq; //下一次分段合法SEQ
bool __section__(".gl_rcp_data") g_rcp_seg_flag; //RCP分段标志
uint8 *__section__(".gl_rcp_data") g_rcp_long_buffer; //超长RCP缓冲区，超过256字节都属于超长RCP
uint16 __section__(".gl_rcp_data") g_rcp_long_length; //超长RCP长度
uint16 __section__(".gl_rcp_data") g_rcp_long_index; //超长RCP缓冲区INDEX
bool __section__(".gl_rcp_data") g_rcp_long_buffer_flag; //正在缓冲完整RCP标志
uint16 __section__(".gl_rcp_data") g_rcp_pkg_max_len; //RCP最大长度，受限于缓冲区大小

//用于rcp服务类型保存
uint8 __section__(".gl_rcp_data")g_rcp_service_type;
//用于rcp远端设备号保存
uint8 __section__(".gl_rcp_data")g_rcp_remote_number;

const rcp_cmd_cb_t * __section__(".gl_rcp_data") g_app_rcp_cmd_cb_tbl; //应用 RCP 命令映射表
rcp_get_func __section__(".gl_rcp_data") g_app_get_global_info; //应用获取全局状态命令响应函数

rcp_ext_serv_func __section__(".gl_rcp_data") g_app_ext_serv_cb[RCP_SERVICE_MAX]; //扩展服务响应函数

//uint8 __section__(".bss.rcp_default_buffer") g_rcp_default_long_buffer[RCP_LONG_COMMAND_BUFFER_LEN];

app_result_e com_rcp_dispatch_bank(rmt_ctrl_pkg_t *p_rcp) __FAR__;
//void com_rcp_dispatch_invalid(uint8 *rcp_data, uint16 rcp_len) __FAR__;
void com_rcp_dispatch_invalid(uint8 *rcp_data, uint16 rcp_len, uint8 rcp_serv) __FAR__;

/*RCP 远程控制数据包解析处理*/
app_result_e __section__(".rcode") com_rcp_dispatch(rmt_ctrl_pkg_t *p_rcp)
{
#if (SUPPORT_RCP_FUNC == 1)

    rmt_ctrl_pkg_t dispatch_rcp;
    rcp_state_e cur_rcp_state;
    bool dispatch_rcp_flag = FALSE;
    app_result_e result = RESULT_NULL;

#if 0
    PRINT_INFO("com recv:");
    PRINT_DATA(p_rcp->rcp_data, p_rcp->rcp_len);
#endif

    //if((uint8) com_get_config_default(BTSTACK_ENABLE_WEIXIN) == 1)
    //{
        //音箱端验证APK/APP
        //g_app_info_state.verification_status = VERIFICATION_PASS;
    //}
    //libc_print("ver",g_app_info_state.verification_status,2);
#if 0    
    if((uint8)com_get_config_default(BTSTACK_ENABLE_WEIXIN)!=0)
    {
        g_app_info_state.verification_status = VERIFICATION_PASS; 
    } 
#endif
       
    if (g_app_info_state.verification_status == VERIFICATION_PASS)
    {
        if ((g_rcp_long_buffer_flag == FALSE) && (p_rcp->rcp_len < 4))
        {
            com_btmanager_sppble_read(g_rcp_long_buffer, p_rcp->rcp_len);
            //非法包，直接丢弃；直通模式需要在这里处理
            
            com_rcp_dispatch_invalid(g_rcp_long_buffer, p_rcp->rcp_len, p_rcp->rcp_flag);
            goto rcp_deal_ok;
        }
    }
    else
    {
        result = com_rcp_dispatch_bank(p_rcp);
        goto rcp_deal_ok;
    }

    if ((g_rcp_long_buffer_flag == FALSE) && (p_rcp->rcp_len == RCP_COMMAND_HEAD_SIZE)) //有可能是获取全局状态命令
    {
        //检验是否合法 RCP
        com_btmanager_sppble_read(g_rcp_long_buffer, RCP_COMMAND_HEAD_SIZE);

        cur_rcp_state = com_check_valid_rcp(g_rcp_long_buffer, RCP_COMMAND_HEAD_SIZE);
        if (cur_rcp_state == RCP_STATE_OK)
        {
            g_rcp_long_length = RCP_COMMAND_HEAD_SIZE;
            dispatch_rcp_flag = TRUE;
        }
        else
        {
            if (cur_rcp_state == RCP_STATE_HEAD)
            { //分段传输段首
                g_rcp_long_length = COMMAND_LENGTH(g_rcp_long_buffer);
                g_rcp_long_index = RCP_COMMAND_HEAD_SIZE;
                g_rcp_long_buffer_flag = TRUE;
            }
            else
            { //非法包，直接丢弃；直通模式需要在这里处理
                com_rcp_dispatch_invalid(g_rcp_long_buffer, RCP_COMMAND_HEAD_SIZE, p_rcp->rcp_flag);
            }
        }

        if (dispatch_rcp_flag == TRUE)
        {
            dispatch_rcp.rcp_data = g_rcp_long_buffer;
            dispatch_rcp.rcp_len = g_rcp_long_length;

            //分发给RCP命令解析
            result = com_rcp_cmd_dispatch(&dispatch_rcp);

            g_rcp_long_buffer_flag = FALSE;
            dispatch_rcp_flag = FALSE;
        }
    }
    else
    {
        result = com_rcp_dispatch_bank(p_rcp);
    }

    rcp_deal_ok:

    return result;

#else
    return RESULT_NULL;
#endif
}

/*RCP 远程控制数据包解析处理*/
app_result_e com_rcp_dispatch_bank(rmt_ctrl_pkg_t *p_rcp)
{
    rmt_ctrl_pkg_t cur_rcp, dispatch_rcp;
    uint16 cur_len, head_len;
    rcp_state_e cur_rcp_state;
    bool dispatch_rcp_flag = FALSE;
    app_result_e result = RESULT_NULL;
    int cmp_ret;

    //音箱端验证APK/APP
    dispatch_again:
    if (g_app_info_state.verification_status == VERIFICATION_PASS)
    {
        if ((g_rcp_long_buffer_flag == FALSE) && (p_rcp->rcp_len < 4))//RCP_COMMAND_HEAD_SIZE
        {
            com_btmanager_sppble_read(g_rcp_long_buffer, p_rcp->rcp_len);
            //非法包，直接丢弃；直通模式需要在这里处理
            com_rcp_dispatch_invalid(g_rcp_long_buffer, p_rcp->rcp_len, p_rcp->rcp_flag);
            goto rcp_deal_ok;
        }
    }
    else
    {
        //#if 1
        //if ((g_bt_stack_cur_info.spp_con_flag != 0)
           // && ((uint8) com_get_config_default(BTMANAGER_VERIFICATION_APK_ENABLE) == 0))   
        if (((g_bt_stack_cur_info.spp_con_flag !=0)||(g_bt_stack_cur_info.ble_con_flag !=0))
            && ((uint8) com_get_config_default(BTMANAGER_VERIFICATION_APK_ENABLE) == 0))    
        //#else
        //if (((g_bt_stack_cur_info.rmt_dev[0].serv_conn & SPP_CONNECTED) != 0)
            //&& ((uint8) com_get_config_default(BTMANAGER_VERIFICATION_APK_ENABLE) == 0))                 
        //#endif
        {
            g_app_info_state.verification_status = VERIFICATION_PASS;
            PRINT_INFO("RCP VERIFICATION NEEDN'T");
            goto dispatch_again;
        }

        //如果验证失败，就忽略后续RCP，并且后台引擎会自动断开APK/APP
        if (g_app_info_state.verification_status == VERIFICATION_FAIL)
        {
            com_btmanager_sppble_read(g_rcp_long_buffer, p_rcp->rcp_len);
        }
        else //正在验证
        {
            if (p_rcp->rcp_len >= 8)
            {
                com_btmanager_sppble_read(g_rcp_long_buffer, 8);

                sys_os_sched_lock();
                cmp_ret = libc_memcmp(g_rcp_long_buffer, "01234567", 8);
                sys_os_sched_unlock();
                if (cmp_ret == 0)
                {
                    g_app_info_state.verification_status = VERIFICATION_PASS;
                    PRINT_INFO("RCP VERIFICATION PASS");

                    if (p_rcp->rcp_len > 8)
                    {
                        PRINT_INFO("RCP VERIFICATION AFTER");
                        p_rcp->rcp_data = (void *) ((uint8 *) p_rcp->rcp_data + 8);
                        p_rcp->rcp_len = p_rcp->rcp_len - 8;
                        goto dispatch_again;
                    }
                }
                else
                {
                    PRINT_INFO("RCP VERIFICATION INVALID"); //只要求在验证超时之前接收到验证包即可，允许收到非法包

                    if (p_rcp->rcp_len > 8)
                    {
                        com_btmanager_sppble_read(g_rcp_long_buffer + 8, p_rcp->rcp_len - 8);
                    }
                }
            }
            else
            {
                com_btmanager_sppble_read(g_rcp_long_buffer, p_rcp->rcp_len);
            }
        }

        //非法包，直接丢弃；直通模式需要在这里处理
        com_rcp_dispatch_invalid(g_rcp_long_buffer, p_rcp->rcp_len, p_rcp->rcp_flag);
        goto rcp_deal_ok;
    }

    cur_rcp.rcp_data = p_rcp->rcp_data;
    cur_rcp.rcp_len = p_rcp->rcp_len;
    //APK可能将多个命令合并为一个 RCP 发送过来
    while (cur_rcp.rcp_len > 0)
    {
        //正在缓冲RCP包
        if (g_rcp_long_buffer_flag == TRUE)
        {
            if ((g_rcp_long_index + cur_rcp.rcp_len) >= g_rcp_long_length)
            {
                cur_len = g_rcp_long_length - g_rcp_long_index;
                dispatch_rcp_flag = TRUE;
            }
            else
            {
                cur_len = cur_rcp.rcp_len;
            }

            com_btmanager_sppble_read(g_rcp_long_buffer + g_rcp_long_index, cur_len);
            //在缓存命令时，如果丢掉一包数据，会用有用命令填补，这里为了防止丢失有用命令
            if (cur_rcp.rcp_len == RCP_COMMAND_HEAD_SIZE)
            {             
                head_len = RCP_COMMAND_HEAD_SIZE;
                cur_rcp_state = com_check_valid_rcp(g_rcp_long_buffer + g_rcp_long_index, head_len);
                if (cur_rcp_state == RCP_STATE_OK)
                {
                    
                    libc_memcpy(g_rcp_long_buffer, g_rcp_long_buffer + g_rcp_long_index, head_len);
                    g_rcp_long_length = COMMAND_LENGTH(g_rcp_long_buffer);
                    dispatch_rcp_flag = TRUE;
                }              
            }     
            g_rcp_long_index += cur_len;
        }
        else
        {
            //检验是否合法 RCP  少于8字节的无用命令丢弃 
            if (cur_rcp.rcp_len >= RCP_COMMAND_HEAD_SIZE)
            {
                head_len = RCP_COMMAND_HEAD_SIZE;
            }
            else
            {
                head_len = cur_rcp.rcp_len;
            }
            com_btmanager_sppble_read(g_rcp_long_buffer, head_len);

            check_again:

            if (cur_rcp.rcp_len < 8)
            {
                cur_len = cur_rcp.rcp_len;
                goto drop_tail_package;
            }

            cur_rcp_state = com_check_valid_rcp(g_rcp_long_buffer, cur_rcp.rcp_len);
            if (cur_rcp_state == RCP_STATE_OK)
            {
                g_rcp_long_length = COMMAND_LENGTH(g_rcp_long_buffer);
                if (g_rcp_long_length > head_len)
                {
                    com_btmanager_sppble_read(g_rcp_long_buffer + head_len, g_rcp_long_length - head_len);
                }
                cur_len = g_rcp_long_length;
                dispatch_rcp_flag = TRUE;
            }
            else
            {
                if (cur_rcp_state == RCP_STATE_INVALID)
                {
                    //有可能是验证包+其他正常命令包，这时就需要将验证包过滤掉，继续处理正常包
                    sys_os_sched_lock();
                    cmp_ret = libc_memcmp(g_rcp_long_buffer, "01234567", 8);
                    sys_os_sched_unlock();
                    if (cmp_ret == 0)
                    {
                        PRINT_INFO("RCP VERIFICATION AFTER");
                        libc_memcpy(g_rcp_long_buffer, g_rcp_long_buffer + 8, 8);

                        cur_rcp.rcp_data = (void *) ((uint8 *) cur_rcp.rcp_data + 8);
                        cur_rcp.rcp_len = cur_rcp.rcp_len - 8;
                        head_len = RCP_COMMAND_HEAD_SIZE - 8;
                        goto check_again;
                    }
                }

                if (cur_rcp.rcp_len > head_len)
                {
                    com_btmanager_sppble_read(g_rcp_long_buffer + head_len, cur_rcp.rcp_len - head_len);
                }
                cur_len = cur_rcp.rcp_len;

                if (cur_rcp_state == RCP_STATE_HEAD)
                { //分段传输段首
                    g_rcp_long_length = COMMAND_LENGTH(g_rcp_long_buffer);
                    g_rcp_long_index = cur_len;
                    g_rcp_long_buffer_flag = TRUE;
                }
                else
                { //非法包，直接丢弃；直通模式需要在这里处理
                    drop_tail_package:
                    com_rcp_dispatch_invalid(g_rcp_long_buffer, p_rcp->rcp_len, p_rcp->rcp_flag);
                }
            }
        }

        if (dispatch_rcp_flag == TRUE)
        {
            dispatch_rcp.rcp_data = g_rcp_long_buffer;
            dispatch_rcp.rcp_len = g_rcp_long_length;

            //分发给RCP命令解析
            result = com_rcp_cmd_dispatch(&dispatch_rcp);

            g_rcp_long_buffer_flag = FALSE;
            dispatch_rcp_flag = FALSE;
        }

        //去掉当前命令，看看是否还有多余数据，如果有则接着处理
        cur_rcp.rcp_data = (void *) ((uint8 *) cur_rcp.rcp_data + cur_len);
        cur_rcp.rcp_len = cur_rcp.rcp_len - cur_len;
    }
    
    if (g_rcp_long_buffer_flag == 1)
    {
        result= RESULT_RCP_LC_WAIT;
    }

    rcp_deal_ok:

    return result;
}

//非法包，直接丢弃；直通模式需要在这里处理
//void com_rcp_dispatch_invalid(uint8 *rcp_data, uint16 rcp_len)
void com_rcp_dispatch_invalid(uint8 *rcp_data, uint16 rcp_len, uint8 rcp_serv)
{
#if 0
    PRINT_INFO_INT("invalid rcp : ", rcp_serv);
    PRINT_DATA(rcp_data, rcp_len);
#endif
    if ( (rcp_serv < RCP_SERVICE_MAX) &&(rcp_serv!=RCP_ACT_SERV_FLAG)&& (g_app_ext_serv_cb[rcp_serv] != NULL) )
    {
        g_app_ext_serv_cb[rcp_serv](rcp_data, rcp_len, rcp_serv);
    }
}

//检查RCP命令是否需要中断TTS
//NOTE!! 对于多个命令放到同一个RCP包的情况，有一定的风险漏掉了关键命令？该怎么办？ //TODO
bool __section__(".bank.check_for_tts") com_rcp_check_for_tts(rmt_ctrl_pkg_t *p_rcp)
{
    uint8 cmd_type;
    uint8 cmd_id;
    uint8 check_valid;
    //还没验证或者验证失败，RCP包都会丢弃，所以就返回FALSE，在TTS中直接处理掉
    if (g_app_info_state.verification_status != VERIFICATION_PASS)
    {
        return FALSE;
    }

    sys_os_sched_lock();

    //TODO 需要增加一个接口从堆中获取当前命令类型和ID
    cmd_type = g_p_rcp_in_data_ctl->raw_data[(g_p_rcp_in_data_ctl->read_ptr + 4) % g_p_rcp_in_data_ctl->capacity];
    cmd_id = g_p_rcp_in_data_ctl->raw_data[(g_p_rcp_in_data_ctl->read_ptr + 5) % g_p_rcp_in_data_ctl->capacity];

    sys_os_sched_unlock();

    if (cmd_type == RCP_CMD_APSWITCH)
    {
        check_valid = com_check_valid_rcp(&g_p_rcp_in_data_ctl->raw_data[g_p_rcp_in_data_ctl->read_ptr \
            % g_p_rcp_in_data_ctl->capacity], RCP_COMMAND_HEAD_SIZE);
        if(check_valid == RCP_STATE_OK)
        {
            return TRUE;
        }
        libc_print("cmd_id_1",cmd_id,2);
        PRINTD_DATA(&g_p_rcp_in_data_ctl->raw_data[g_p_rcp_in_data_ctl->read_ptr  % g_p_rcp_in_data_ctl->capacity],16);
        //return TRUE;
    }

    if (com_rcp_cmd_find_cb(cmd_type, cmd_id) == NULL)
    {
        return FALSE;
    }

    if (cmd_type == RCP_CMD_CONTROL)
    {
        return TRUE;
    }
    else if ((cmd_type == RCP_CMD_QUERY) || (cmd_type == RCP_CMD_SIMKEY))
    {
        return FALSE;
    }
    else if (cmd_type == RCP_CMD_SETTING)
    {
        if ((cmd_id == SETTING_ID_SYSTEM_SETTING)
                || (cmd_id == SETTING_ID_SYSTEM_STATUS)
                || (cmd_id == SETTING_ID_DEVICE_NAME)
                || (cmd_id == SETTING_ID_MUSIC_SETTING)
                || (cmd_id == SETTING_ID_OTA_CONNECT_SETTING)
                || (cmd_id == SETTING_ID_OTA_RECIEVE_DATA)
                || (cmd_id == SETTING_ID_OTA_RESTAT_UD)
                || (cmd_id == SETTING_ID_OTA_RESTAT_CD)
                || (cmd_id == SETTING_ID_SEND_COMPLETE)
                || (cmd_id == SETTING_ID_OTA_RECIEVE_VRAM_DATA))
                
        {
            return FALSE;
        }
    }
    else //非法RCP命令包，可能是验证命令包
    {
        return FALSE;
    }

    return TRUE;
}

int com_rcp_ble_serv_send(void *data, uint16 data_len, uint8 serv_flag)
{
#if (SUPPORT_RCP_FUNC == 1)
    msg_apps_t rcp_msg;
    rmt_ctrl_pkg_t tmp_rcp;

    tmp_rcp.rcp_data = data;
    tmp_rcp.rcp_len = data_len;
    
    tmp_rcp.rcp_flag = serv_flag;
    
    tmp_rcp.rcp_remote = (uint8)(g_rcp_remote_number | 0x80);

    rcp_msg.type = MSG_BTSTACK_RCP_SEND_SYNC;
    rcp_msg.content.addr = &tmp_rcp;
    if (send_sync_msg_btmanager(NULL, &rcp_msg, NULL, 0) == ERR_NO_ERR)
    {
        return 0;
    }
    else
    {
        return -1;
    }
#else
    return 0;
#endif
}
