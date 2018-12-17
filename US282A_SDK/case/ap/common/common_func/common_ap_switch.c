/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：前台应用切换接口，前台应用退出前传递 app_result_e 进行前台应用切换。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_func.h"
#include "applib.h"
#include "case_independent.h"

const ap_switch_info_t ap_switch_info[] =
{
    { APP_ID_BTPLAY, 0, 1, RESULT_BLUETEETH_SOUND, APP_FUNC_BTPLAY },
    { APP_ID_MUSIC, PARAM_MUSIC_SDCARD, 1, RESULT_MUSIC_CPLAY, APP_FUNC_PLAYCARD },
    { APP_ID_MUSIC, PARAM_MUSIC_UHOST, 1, RESULT_MUSIC_UPLAY, APP_FUNC_PLAYUHOST },
    { APP_ID_LINEIN, 0, 1, RESULT_ENTER_LINEIN, APP_FUNC_PLAYLINEIN },
    { APP_ID_RADIO, 0, 1, RESULT_RADIO, APP_FUNC_RADIO },
    { APP_ID_USOUND, 0, 1, RESULT_USB_SOUND, APP_FUNC_USOUND },
    { APP_ID_OTA_TEST, 0, 1, RESULT_ENTER_OTA_SONG_TEST, APP_FUNC_OTA_TEST },
    
    /*!以下为快捷键进入的功能选项,它们不在功能循环内,需要返回到上一次的状态*/
    { APP_ID_CONFIG, 1, 1, RESULT_POWER_OFF, APP_FUNC_CONFIG },
    { APP_ID_CONFIG, 2, 1, RESULT_LOW_POWER, APP_FUNC_CONFIG },
    { APP_ID_CONFIG, 3, 1, RESULT_SYSTEM_ENTER_S3, APP_FUNC_CONFIG },
    { APP_ID_CONFIG, 4, 1, RESULT_IDLE_MODE, APP_FUNC_CONFIG },

    { APP_ID_UDISK, 0, 1, RESULT_USB_TRANS, APP_FUNC_UDISK },

// 固件太大，关闭闹钟AP
    { APP_ID_ALARM, 0, 1, RESULT_ENTER_ALARM, APP_FUNC_CLOCK },
    { APP_ID_ALARM, 1, 1, RESULT_ALARM_RING_FOLDER_SCAN, APP_FUNC_CLOCK },

    { APP_ID_BTCALL, 1, 1, RESULT_BLUETEETH_CALL_BACKGROUND, APP_FUNC_BTCALL },

    { APP_ID_UPGRADE, 0, 1, RESULT_ENTER_UPGRADE, APP_FUNC_UPGRADE },

    { APP_ID_RECORD, PARAM_FROM_LINEIN_TOC, 1, RESULT_RECORD_LINEIN_TOC, APP_FUNC_LINREC_TOC },
    { APP_ID_RECORD, PARAM_FROM_RADIO_TOC, 0, RESULT_RECORD_FM_TOC, APP_FUNC_FMREC_TOC },
    { APP_ID_RECORD, PARAM_FROM_MIC_TOC, 1, RESULT_RECORD_MIC_TOC, APP_FUNC_MICREC_TOC },
    { APP_ID_RECORD, PARAM_FROM_LINEIN_TOU, 1, RESULT_RECORD_LINEIN_TOU, APP_FUNC_LINREC_TOU },
    { APP_ID_RECORD, PARAM_FROM_RADIO_TOU, 0, RESULT_RECORD_FM_TOU, APP_FUNC_FMREC_TOU },
    { APP_ID_RECORD, PARAM_FROM_MIC_TOU, 1, RESULT_RECORD_MIC_TOU, APP_FUNC_MICREC_TOU },

    { APP_ID_MUSIC, PARAM_RECORD_CPLAY, 1, RESULT_RECORD_CPLAY, APP_FUNC_PLAYCRECORD },
    { APP_ID_MUSIC, PARAM_RECORD_UPLAY, 1, RESULT_RECORD_UPLAY, APP_FUNC_PLAYURECORD },

    { APP_ID_BTCALL, 0, 1, RESULT_ENTER_ASQT_HF_TEST, APP_FUNC_BTCALL },
    
    { APP_ID_OTA_UPGRADE, 0, 1, RESULT_ENTER_OTA_UPGRADE, APP_FUNC_UPGRADE },
    //{ APP_ID_USER1, 0, 1, RESULT_ENTER_USER1, APP_FUNC_USER1 },
};

#define AP_SWITCH_INFO_COUNT    (ARR_COUNT(ap_switch_info))

static uint8 com_get_cfg_func_index(uint8 app_func_id);
static uint8 com_get_switch_seq_index(uint8 app_func_id);
static bool com_check_function_in(uint8 func_id);
static uint8 com_app_switch_next_function(uint8 last_index);
static uint8 com_app_switch_any_function(uint8 cur_index);
static void com_send_msg_to_manager(uint32 msg_type, uint8 param1, uint8 param2, bool synch_flag);
static uint8 com_ap_switch_next_index(app_result_e app_result);
static void restore_global_status(void);

//返回即将切换到哪个FUNCTION
uint8 com_ap_switch_ask_next_func(app_result_e app_result)
{
    uint8 next_index;
    uint8 default_index = 0; //对应到ap_func_table数组的下标

    //得到缺省APP_FUNC在ap_switch_info数组中的下标
    default_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);

    next_index = com_ap_switch_next_index(app_result);

    //for valid set
    if (next_index >= AP_SWITCH_INFO_COUNT)
    {
        //check index
        next_index = default_index;
    }

    return ap_switch_info[next_index].app_func_id;
}

/******************************************************************************/
/*!
 * \par  Description:
 *\ ap切换功能
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      common_func
 * \note
 *******************************************************************************/
void com_ap_switch_deal(app_result_e app_result)
{
    uint8 deal_ap_id;
    uint8 ap_param;
    uint8 cur_index; //ap_func_table[cur_index]即要切入的APP_FUNC 的ID
    //(只在最后,cur_index变成了ap_switch_info的下标)
    uint8 last_index; //对应到ap_func_table的下标
    uint8 default_index = 0; //对应到ap_func_table数组的下标
    uint8 i;
    uint32 reg_val;

    if (app_result == RESULT_SYSTEM_EXIT_S3)
    {
        //恢复 app_info_state_t 和 app_last_state_t
        restore_global_status();

        deal_ap_id = ap_switch_info[g_app_info_state.cur_func_index].ap_id;
        ap_param = ap_switch_info[g_app_info_state.cur_func_index].param;
        ap_param |= (uint8) PARAM_S3_EXIT;
        goto ap_switch_create_app;
    }

    //得到缺省APP_FUNC在ap_switch_info数组中的下标
    default_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);

    //当前的index 作为上一次的保存
    last_index = get_cur_func_index();

    if (app_result == RESULT_ESD_RESTART)
    {
        //del  cur_index = (uint8) ((act_readl(RTC_BAK0) & (0xff << MY_RTC_FUNC_INDEX)) >> MY_RTC_FUNC_INDEX);
        cur_index = g_config_var.ap_id;
        if(ap_switch_info[cur_index].app_func_id == APP_FUNC_UDISK)
        {
            g_esd_cardreader_flag = 1;
        }
        goto ap_switch_create_app_esd_restart;
    }

    if (app_result == RESULT_BLUETEETH_CALL_BACKGROUND)
    {
        g_ap_switch_var.call_background_status = CALL_BACK_STATUS_IN;

#if 0        
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        g_bt_auto_connect_ctrl.need_auto_connect = 0;
        
        if(g_bt_stack_cur_info.dev_role == TWS_MASTER)
        {
            //当电话闹铃来，就去断开主副箱之间的连接
            msg_apps_t msg;
            msg.type =  MSG_BTSTACK_TWS_FORCE_UNLINK_SYNC;
            tws_auto_info_t *cur_auto_info;
            msg.content.addr = cur_auto_info;

            //cur_auto_info 要清0
            libc_memset(cur_auto_info, 0x00, sizeof(cur_auto_info));

            //发送同步消息
            send_sync_msg_btmanager(NULL, &msg, NULL, 0);  
            g_ap_switch_var.tws_phon_come_flag=1;

            for ( i= 0; i < g_btmanager_gl_var.support_dev_num; i++)
            {    
                PRINT_DATA(cur_auto_info->bd_addr.bytes,6);
                if (libc_memcmp(&g_bt_stack_cur_info.rmt_dev[i].addr.bytes[3],
                    &cur_auto_info->bd_addr.bytes[3], 3) == 0)        
                {
                    //PRINT_DATA(&g_bt_stack_cur_info.rmt_dev[i].addr.bytes[3],3);

                    libc_memcpy(g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes,
                                           cur_auto_info->bd_addr.bytes, 6);
                    g_bt_auto_connect_ctrl.dev_info[i].conn_flag=1;
                    g_bt_auto_connect_ctrl.dev_info[i].support_profile=cur_auto_info->support_profile;        
                    break;
                }                                                                                                              
            }    
        }
#endif
#endif
    }
    else
    {
        if (g_ap_switch_var.call_background_status == CALL_BACK_STATUS_IN)
        {
            g_ap_switch_var.call_background_status = CALL_BACK_STATUS_BACK;            
        }
        else
        {
            g_ap_switch_var.call_background_status = CALL_BACK_STATUS_NULL;
        }
    }

    //找一些检查，如果不行，cur_index就会变成下一个满足条件的，
    //但cur_index仍然是ap_switch_info数组里面的下标
    cur_index = com_ap_switch_next_index(app_result);

    //for valid set
    if (cur_index >= AP_SWITCH_INFO_COUNT)
    {
        //check index
        cur_index = default_index;
        last_index = default_index;
    }

    if (ap_switch_info[cur_index].engine_do == TRUE)
    {
        //check当前引擎，关闭
        if (get_engine_type() != ENGINE_NULL)
        {
            ap_param = 0;
            if (app_result == RESULT_SYSTEM_ENTER_S3)
            {
                ap_param = 1;
            }
            //杀死引擎
            com_send_msg_to_manager(MSG_KILL_APP_SYNC, APP_ID_THEENGINE, ap_param, TRUE);
        }
    }

    if (app_result == RESULT_SYSTEM_ENTER_S3)
    {
        sd_sec_param_t sd_sec_param;

        //备份 app_info_state_t 和 app_last_state_t
        libc_memset(SRAM_S3BT_BUFFER, 0x0, 0x200);

        //if (g_ap_switch_var.s3bt_nor_erase_flag == FALSE)
        //{
        //    sd_sec_param.file_offset = NOR_S3BT_ERASE_WRITE;
        //    sd_sec_param.sram_addr = SRAM_S3BT_BUFFER;
        //    sd_sec_param.sec_num = 1;
        //    base_ext_vram_write(&sd_sec_param);

        //    g_ap_switch_var.s3bt_nor_erase_flag = TRUE;
        //}

        libc_memcpy(SRAM_S3BT_BUFFER, &g_app_info_state, sizeof(app_info_state_t));
        libc_memcpy(SRAM_S3BT_BUFFER + sizeof(app_info_state_t), &g_app_last_state, sizeof(app_last_state_t));

        //sd_sec_param.file_offset = NOR_S3BT_APP_GLOBAL;
        //sd_sec_param.sram_addr = SRAM_S3BT_BUFFER;
        //sd_sec_param.sec_num = 1;
        //base_ext_vram_write(&sd_sec_param);

        sys_vm_write(SRAM_S3BT_BUFFER, VM_S3BT_APP_GLOBAL, sizeof(app_info_state_t) + sizeof(app_last_state_t));
    }

    ap_switch_create_app_esd_restart:

    //保存当前的index,作为上一次的序号
    set_last_func_index(last_index);

    //save cur function index
    set_cur_func_index(cur_index);
    
    //reg_val = g_config_var.ap_id;
    //del  reg_val = (act_readl(RTC_BAK0) & (~(0xff << MY_RTC_FUNC_INDEX)));
    
    if(g_app_info_state.stub_tools_type == 0)
    {
        //del  reg_val |= ((uint32)(g_app_info_state.cur_func_index) << MY_RTC_FUNC_INDEX);
        reg_val = g_app_info_state.cur_func_index;
    }
    else
    {
        //del  reg_val |= (APP_FUNC_INVALID << MY_RTC_FUNC_INDEX);    
        reg_val = APP_FUNC_INVALID;
    }
    g_config_var.ap_id = (uint8)reg_val;
    sys_vm_write(&g_config_var,VM_AP_CONFIG,sizeof(g_config_var_t));
    //del  act_writel(reg_val, RTC_BAK0); 

    //rtc register
    //del  act_writel(0xA596, RTC_REGUPDATA);
    //del  while (act_readl(RTC_REGUPDATA) != 0x5A69)
    {
        ;//wait for register update
    }      
    
    //save cur function id
    set_cur_func_id(ap_switch_info[cur_index].app_func_id);

    PRINT_INFO_INT("ap swi fuc:", ap_switch_info[cur_index].app_func_id);

    //向manager发消息创建前台应用

    deal_ap_id = ap_switch_info[cur_index].ap_id;

    ap_param = ap_switch_info[cur_index].param;

    //转发 创建应用 消息给manager
    ap_switch_create_app:
    g_ap_switch_var.apk_alarm_flag = 0; //非APK切换到闹钟设置界面
    com_send_msg_to_manager(MSG_CREAT_APP, deal_ap_id, ap_param, FALSE);

    g_ap_switch_var.wake_from_s2 = FALSE;
    g_ap_switch_var.wake_ret_from_s2 = 0;
}


//app_func_id的值型如：APP_FUNC_xxx
//返回：app_func_id在ap_switch_info数组里面的下标
//      如果没找到，就返回0
static uint8 com_get_cfg_func_index(uint8 app_func_id)
{
    uint8 i;

    for (i = 0; i < AP_SWITCH_INFO_COUNT; i++)
    {
        if (ap_switch_info[i].app_func_id == app_func_id)
        {
            break;
        }
    }
    if (i >= AP_SWITCH_INFO_COUNT)
    {
        i = 0;
    }

    return i;
}

//返回：app_func_id是“config.txt的SETTING_APP_SWITCH_SEQUENCE项”中的第几个
static uint8 com_get_switch_seq_index(uint8 app_func_id)
{
    uint8 i;

    for (i = 0; i < MAX_FUNCTON_CYCLE; i++)
    {
        if (g_ap_switch_var.app_switch_seq[i] == app_func_id)
        {
            return i;
        }
    }

    return 0xff;
}

/******************************************************************************/
/*!
 * \par  Description:当前功能项硬件是否ok，能否进入
 *\ check cur app function hardware is ready and can realy enter
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      common_func
 * \note
 *******************************************************************************/

static bool com_check_function_in(uint8 func_id)
{
    //check if hard ware is not ready
    switch (func_id)
    {
        case APP_FUNC_PLAYUHOST:
        //u 盘不存在
        if ((int8) sys_detect_disk(DRV_GROUP_STG_UHOST) == -1)
        {
            return FALSE;
        }
        break;

        case APP_FUNC_PLAYCARD:
        //卡不存在
        if ((int8) sys_detect_disk(DRV_GROUP_STG_CARD) == -1)
        {
            return FALSE;
        }
        break;

        case APP_FUNC_PLAYLINEIN:
        //linein不在
#ifndef ENABLE_TRUE_WIRELESS_STEREO
        if (get_linein_state() != LINEIN_STATE_LINEIN_IN)
#else

#if (SUPPORT_AUX_DETECT == DETECT_NO_SUPPORT)
        g_tws_m_linein_flag = 1;
#endif
        if (g_bt_stack_cur_info.dev_role == NORMAL_DEV)
        {
            libc_print("c line flag",0,0);
            g_tws_m_linein_flag = 0;
        }
                        
        if ( ((get_linein_state() != LINEIN_STATE_LINEIN_IN) && (g_tws_m_linein_flag == 0))
            || ((get_linein_state() == LINEIN_STATE_LINEIN_IN)&& (g_tws_m_linein_flag == 0)
                &&(g_bt_stack_cur_info.dev_role == TWS_SLAVE)) )
#endif            
        {
            return FALSE;
        }
        break;

        case APP_FUNC_USOUND:
        //当前无USB线插入
        if (get_cable_state() != CABLE_STATE_CABLE_IN)
        {
            return FALSE;
        }

        if (g_app_info_state.stub_tools_type != 0)
        {
            return FALSE;
        }
        break;

        default:
        break;
    }

    return TRUE;
}

//根据config.txt切换到下一个APP_FUNC
//返回：该APP_FUNC在ap_switch_info数组里面的下标
//last_index: 在ap_switch_info数组里面的下标
static uint8 com_app_switch_next_function(uint8 last_index)
{
    uint8 last_switch_seq_index;
    uint8 tmp_func_id;

    if (last_index >= MAX_FUNCTON_CYCLE)
    {
        last_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);
    }

    //是“config.txt的SETTING_APP_SWITCH_SEQUENCE项”中的第几个
    last_switch_seq_index = com_get_switch_seq_index(ap_switch_info[last_index].app_func_id);
    if (last_switch_seq_index == 0xff)
    {
        last_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);
        last_switch_seq_index = com_get_switch_seq_index(ap_switch_info[last_index].app_func_id);
        if (last_switch_seq_index == 0xff)
        {
            PRINT_ERR("ap swi er!");
            while (1)
            {
                ; //nothing for QAC
            }
        }
    }

    find_next_index:
    //按照config.txt切到下一个APP_FUNC
    last_switch_seq_index++;

    if (last_switch_seq_index >= MAX_FUNCTON_CYCLE)
    {
        last_switch_seq_index = 0;
    }

    tmp_func_id = g_ap_switch_var.app_switch_seq[last_switch_seq_index];
    if (((g_ap_switch_var.app_func_support & (uint32) (1 << tmp_func_id)) == 0)
            || (com_check_function_in(tmp_func_id) == FALSE))
    {
        goto find_next_index;
    }

    //返回该APP_FUNC在ap_switch_info数组里面的下标
    return com_get_cfg_func_index(tmp_func_id);
}

//cur_index是ap_switch_info数组里面的下标
//这里做了一些检查
//返回值仍然是ap_switch_info数组里面的下标
static uint8 com_app_switch_any_function(uint8 cur_index)
{
    uint8 cur_switch_seq_index;

    if (cur_index >= AP_SWITCH_INFO_COUNT)
    {
        cur_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);
    }

    if ((g_ap_switch_var.app_func_support & (uint32) (1 << ap_switch_info[cur_index].app_func_id)) == 0)
    {
        cur_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);
    }

    if (cur_index < MAX_FUNCTON_CYCLE)
    {
        //cur_switch_seq_index表示config.txt里面的第几个APP_FUNC
        cur_switch_seq_index = com_get_switch_seq_index(ap_switch_info[cur_index].app_func_id);
        if (cur_switch_seq_index == 0xff)
        {
            cur_switch_seq_index = com_get_switch_seq_index(g_ap_switch_var.app_default_func_id);
            cur_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);
        }

        if(cur_switch_seq_index >= MAX_FUNCTON_CYCLE)
        {
            goto index_err;
        }
        
        if (((g_ap_switch_var.app_func_support & (uint32) (1 << ap_switch_info[cur_index].app_func_id)) == 0)
                || (com_check_function_in(g_ap_switch_var.app_switch_seq[cur_switch_seq_index]) == FALSE))
        {
            cur_switch_seq_index = com_app_switch_next_function(cur_index);
        }
        
        if(cur_switch_seq_index >= MAX_FUNCTON_CYCLE)
        {
            goto index_err;
        }
        
        //cur_index是ap_switch_info数组里面的下标
        cur_index = com_get_cfg_func_index(g_ap_switch_var.app_switch_seq[cur_switch_seq_index]);
    }

    return cur_index;
    
    index_err:
    libc_print("index err:",cur_switch_seq_index,2);
    return cur_index;
}

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:发送消息给manager
 *\ check cur app function hardware is ready and can realy enter
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      common_func
 * \note
 *******************************************************************************/
static void com_send_msg_to_manager(uint32 msg_type, uint8 param1, uint8 param2, bool synch_flag)
{
    msg_apps_t msg;

    //message type
    msg.type = msg_type;
    //param
    msg.content.data[0] = param1;

    msg.content.data[1] = param2;
    //whether sych message
    if (synch_flag == TRUE)
    {
        //发送同步消息
        send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
    }
    else
    {
        //msg.content.data[1] = param2;
        //发送异步消息
        send_async_msg(APP_ID_MANAGER, &msg);
    }
}

//返回即将切换到哪个INDEX
static uint8 com_ap_switch_next_index(app_result_e app_result)
{
    uint8 cur_index; //ap_func_table[cur_index]即要切入的APP_FUNC 的ID
    //(只在最后,cur_index变成了ap_switch_info的下标)
    uint8 last_index; //对应到ap_func_table的下标
    uint8 default_index = 0; //对应到ap_func_table数组的下标

    //得到缺省APP_FUNC在ap_switch_info数组中的下标
    default_index = com_get_cfg_func_index(g_ap_switch_var.app_default_func_id);

    //当前的index 作为上一次的保存
    last_index = get_cur_func_index();

    if (app_result == RESULT_ESD_RESTART)
    {
        //del  cur_index = (uint8) ((act_readl(RTC_BAK0) & (0xff << MY_RTC_FUNC_INDEX)) >> MY_RTC_FUNC_INDEX);
        cur_index = g_config_var.ap_id;
        return cur_index;
    }

    if (app_result == RESULT_LASTPLAY)
    {
        //上一次的apindex
        cur_index = get_last_func_index();
        if (cur_index >= MAX_FUNCTON_CYCLE)
        {
            cur_index = com_get_switch_seq_index(g_ap_switch_var.app_default_func_id);
        }
    }
    else if (app_result == RESULT_NEXT_FUNCTION)
    {
        //进入下一个应用
        //cur_index是按照config.txt切换APP_FUNC后，这个APP_FUNC在ap_switch_info数组里面的下标
        cur_index = com_app_switch_next_function(last_index);
    }
    else
    {
        //根据返回值确定索引值
        for (cur_index = 0; cur_index < AP_SWITCH_INFO_COUNT; cur_index++)
        {
            //check enter which one
            if (ap_switch_info[cur_index].result_val == app_result)
            {
                break;
            }
        }

        if (cur_index >= AP_SWITCH_INFO_COUNT)
        {
            cur_index = default_index;
        }
    }

    //找一些检查，如果不行，cur_index就会变成下一个满足条件的，
    //但cur_index仍然是ap_switch_info数组里面的下标
    cur_index = com_app_switch_any_function(cur_index);

    return cur_index;
}

//恢复 app_info_state_t 和 app_last_state_t
static void restore_global_status(void)
{
    uint8 temp_data[512];
    
    app_info_state_t tmp_app_info_state;
    app_last_state_t tmp_app_last_state;

    sys_vm_read(temp_data, VM_S3BT_APP_GLOBAL, sizeof(app_info_state_t) + sizeof(app_last_state_t));

    libc_memcpy(&tmp_app_info_state, temp_data, sizeof(app_info_state_t));
    libc_memcpy(&tmp_app_last_state, temp_data + sizeof(app_info_state_t), sizeof(app_last_state_t));

    g_app_info_state.card_state = tmp_app_info_state.card_state;
    g_app_info_state.uhost_state = tmp_app_info_state.uhost_state;
    g_app_info_state.linein_state = tmp_app_info_state.linein_state;
    g_app_info_state.cable_state = tmp_app_info_state.cable_state;
    g_app_info_state.hp_state = tmp_app_info_state.hp_state;
    g_app_info_state.charge_state = tmp_app_info_state.charge_state;
    g_app_info_state.bat_value = tmp_app_info_state.bat_value;
    g_app_info_state.cur_func_index = tmp_app_info_state.cur_func_index;
    g_app_info_state.cur_func_id = tmp_app_info_state.cur_func_id;
    g_app_info_state.apk_status = tmp_app_info_state.apk_status;
    g_app_info_state.verification_status = tmp_app_info_state.verification_status;
    g_app_info_state.need_update_playlist = tmp_app_info_state.need_update_playlist;

    g_app_last_state.last_func_index = tmp_app_last_state.last_func_index;
}

uint32 com_check_ap_func_index_valid(void)
{
    uint32 cur_index;
    uint32 next_ap_id;
    
    //del  cur_index = ((act_readl(RTC_BAK0) & (0xff << MY_RTC_FUNC_INDEX)) >> MY_RTC_FUNC_INDEX);
    cur_index = g_config_var.ap_id;
    if(cur_index >= (sizeof(ap_switch_info) / sizeof(ap_switch_info_t)))
    {
        goto err_index;    
    }

    next_ap_id = ap_switch_info[cur_index].ap_id;

    libc_print("ESD next ap id: ", next_ap_id, 2);

    //过滤config ap的ESD处理流程
    if(next_ap_id == APP_ID_CONFIG)
    {
        goto err_index;
    }

    return TRUE;
    
    err_index:
    PRINT_INFO_INT("err index", cur_index);

    return FALSE;
}

