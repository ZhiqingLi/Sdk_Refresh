/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙管理模块，包括装载和卸载BT STACK。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"
#include "common_btmanager.h"
#include "config_id.h"
#include "sys_info.h"
#include "common_func.h"

//tws交替回连添加代码
#ifdef ENABLE_TRUE_WIRELESS_STEREO
uint8 __section__(".bt_globe_data")check_id1;
uint8 __section__(".bt_globe_data")check_id2;
uint8 __section__(".bt_globe_data")once_auto_flag;
uint8 __section__(".bt_globe_data")send_once_flag;
uint8 __section__(".bt_globe_data")g_tws_m_linein_flag;
uint8 __section__(".bt_globe_data")g_tws_m_switchap_flag;
#endif

bt_auto_connect_ctrl_t __section__(".bt_auto_connect") g_bt_auto_connect_ctrl;
uint8 __section__(".bt_auto_connect") g_bt_auto_connect_addr[BD_ADDR_LEN]; //自动回连设备地址
btmanager_global_variable_struct __section__(".bt_globe_data") g_btmanager_gl_var;
bt_stack_info_t __section__(".bt_globe_data") g_bt_stack_cur_info;
bt_stack_pipe_t * __section__(".bt_globe_data") g_p_rcp_in_data_ctl;
uint16 __section__(".bt_globe_data") g_fast_discoverable_cnt;
int8 __section__(".bt_globe_data") g_fast_discoverable_timer;

static void btmanager_global_var_init(bool standby_exit);
static void load_bt_stack_config(btstack_config_t *cfg);
static void install_bt_stack(bool standby_exit, bool need_patch_download, bool need_power_on);
static void uninstall_bt_stack(bool standby_enter, bool need_power_off);

extern void auto_connect_sync_rmt_device_list(void) __FAR__;
extern void print_btstack_ver(void) __FAR__;


#ifdef ENABLE_TRUE_WIRELESS_STEREO
//tws下退出打电话，主箱回连副箱
void __section__(".call_auto_conn") com_btmanager_call_back_auto_connect(void)
{
#if 0	
    uint8 i;

    for (i = 0; i < g_btmanager_gl_var.support_dev_num; i++)
    {
        if (libc_memcmp(&g_bt_stack_cur_info.rmt_dev[i].addr.bytes[3],
            &g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes[3], 3) == 0)
        {
            g_bt_auto_connect_ctrl.need_auto_connect = 0x01;
            g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_TIMEOUT;
            g_bt_auto_connect_ctrl.connect_tick_cnt=40;
            //PRINT_DATA(g_bt_auto_connect_ctrl.dev_info[i].remote_addr.bytes,6);
            PRINT_INFO("auto 1");
            g_bt_auto_connect_ctrl.dev_info[i].conn_flag = 1; 
        }	

    }
#endif
}

void __section__(".call_auto_conn") send_cmd_to_other(uint8 op)
{
     msg_apps_t msg;
     msg.type = MSG_BTSTACK_TWS_SUB_MODE_FLAG;
     msg.content.data[0] = op;
     send_sync_msg_btmanager(NULL, &msg, NULL, 0);
}

#endif


void __section__(".bank_4")com_btmanager_init(bool standby_exit)
{
    bool need_patch_download;
    bool need_power_on;
   
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 1)
    {
        return;
    }

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
    discrete_led_set(LED_ID_BT_LINK, LED_ALWAYS_ON, NULL);
#endif

    //BT MANAGER 全局变量初始化
    btmanager_global_var_init(standby_exit);

    //设置BT MANAGER状态为“上电初始化”
    change_btmanager_status(BTMANAGER_STATUS_POWERON);

    if (standby_exit == TRUE)
    {
        g_app_info_state.patch_download_flag = TRUE;
    }

    //判断是否是开机第一次进来
    if (g_app_info_state.patch_download_flag == FALSE)
    {
        g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_STARTUP;
        if (g_need_auto_connect_timeout == 1)
        {
            g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_TIMEOUT;
        }
        need_patch_download = TRUE;
    }
    else
    {
        g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_MODE;
        need_patch_download = FALSE;
    }

    //判断是否需要上电
    if (g_app_info_state.bt_power_on == FALSE)
    {
        need_power_on = TRUE;
    }
    else
    {
        need_power_on = FALSE;
    }

    //启动蓝牙协议栈，会更新 g_bt_auto_connect_ctrl.enable_a2dp/enable_avrcp/enable_hfp 变量
    if (standby_exit == TRUE)
    {
        sys_disable_mem_use(SRAM_S3BT_BUFFER, SRAM_S3BT_BUFFER + SRAM_S3BT_SIZE);
        
        g_btmanager_gl_var.btstack_standby = FALSE;

        sys_vm_read(SRAM_S3BT_BUFFER, VM_S3BT_BT_STACK, VM_S3BT_BT_STACK_LEN);

        base_clear_vram_index(VM_S3BT_BT_STACK); 

        sys_base_set_exit_S3bt_scene();
    }
    install_bt_stack(standby_exit, need_patch_download, need_power_on);
    g_app_info_state.bt_power_on = TRUE;
    if (standby_exit == TRUE)
    {
        while (g_btmanager_gl_var.btstack_standby == FALSE)
        {
            sys_udelay(1);
        }

        sys_enable_mem_use(SRAM_S3BT_BUFFER, SRAM_S3BT_BUFFER + SRAM_S3BT_SIZE);
    }

    //获取设备配对列表，以决定是否回连
    if (standby_exit == FALSE)
    {
        auto_connect_sync_rmt_device_list();

        //打印版本号
        //print_btstack_ver();
    }

    //初始化完成时更新一次BTSTACK状态
    get_bt_stack_info();

    //自动化测试，回连控制由测试代码控制，不直接通过common控制
    if(g_app_info_state.autotest_mode == FALSE)
    {
        //创建回连定时器 2Hz定时器
        g_bt_auto_connect_ctrl.auto_connect_timer_id =
            set_app_timer(APP_TIMER_ATTRB_COM_CONTROL, 500, com_btmanager_autoconnect);
    }

    //从S3BT回来需要恢复蓝牙指示灯等UI提示
    if (standby_exit == TRUE)
    {
        com_btmanager_conn_status_change_handle(TRUE);
    }

    if (sys_shm_creat(SHARE_MEM_ID_BTSTACK_STATUS, &g_bt_stack_cur_info, sizeof(bt_stack_info_t)) == -1)
    {
        PRINT_ERR("btstack status shm create fail!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

    if ((g_btmanager_gl_var.enable_spp == 1) || (g_btmanager_gl_var.enable_ble == 1))
    {
        g_p_rcp_in_data_ctl = (bt_stack_pipe_t *) sys_shm_mount(SHARE_MEM_ID_BTRCPPIPE);
        if (g_p_rcp_in_data_ctl == NULL)
        {
            PRINT_ERR("btstack rcp in shm not exist!!");
            while (1)
            {
                ; //nothing for QAC
            }
        }
    }
	
}

void __section__(".bank_3") com_btmanager_exit(bool standby_enter, bool need_power_off)
{
    uint8 *temp_buffer;
    
    if (g_app_info_vector[APP_TYPE_BTSTACK].used == 0)
    {
        return;
    }

    if (g_bt_auto_connect_ctrl.auto_connect_timer_id != -1)
    {
        kill_app_timer(g_bt_auto_connect_ctrl.auto_connect_timer_id);
        g_bt_auto_connect_ctrl.auto_connect_timer_id = -1;
    }

    if (g_btmanager_gl_var.avrcp_timer_id != -1)
    {
        kill_app_timer(g_btmanager_gl_var.avrcp_timer_id);
        g_btmanager_gl_var.avrcp_timer_id = -1;
    }

    if (g_btmanager_gl_var.a2dp_timer_id != -1)
    {
        kill_app_timer(g_btmanager_gl_var.a2dp_timer_id);
        g_btmanager_gl_var.a2dp_timer_id = -1;
    }

    if (g_btmanager_gl_var.sniff_timer_id != -1)
    {
        kill_app_timer(g_btmanager_gl_var.sniff_timer_id);
        g_btmanager_gl_var.sniff_timer_id = -1;
    }

    if (g_btmanager_gl_var.bat_rpt_timer_id != -1)
    {
        kill_app_timer(g_btmanager_gl_var.bat_rpt_timer_id);
        g_btmanager_gl_var.bat_rpt_timer_id = -1;
    }
        
    //for fast discover time kill 
    if (g_fast_discoverable_timer != -1)
    {
    	  libc_print("restart",0,0);
    	  kill_app_timer(g_fast_discoverable_timer);
        g_fast_discoverable_timer = -1;
    }   

    sys_shm_destroy(SHARE_MEM_ID_BTSTACK_STATUS);

    //关闭蓝牙指示灯
    discrete_led_set(LED_ID_BT_LINK, LED_ALWAYS_OFF, NULL);

    if (standby_enter == TRUE)
    {//备份全局变量
        sd_sec_param_t sd_sec_param;

        libc_memset(SRAM_S3BT_BUFFER + 0x6000, 0x0, 0x200);

        temp_buffer = sys_malloc(sizeof(g_btmanager_gl_var) + sizeof(g_bt_auto_connect_ctrl));
        
        libc_memcpy(temp_buffer, &g_btmanager_gl_var, sizeof(g_btmanager_gl_var));
        libc_memcpy(temp_buffer + sizeof(g_btmanager_gl_var), &g_bt_auto_connect_ctrl, sizeof(g_bt_auto_connect_ctrl));

        //sd_sec_param.file_offset = NOR_S3BT_BT_MANAGER;
        //sd_sec_param.sram_addr = SRAM_S3BT_BUFFER;
        //sd_sec_param.sec_num = 1;
        //base_ext_vram_write(&sd_sec_param);

    }

    //卸载蓝牙协议栈
    if (standby_enter == TRUE)
    {
        g_btmanager_gl_var.btstack_standby = FALSE;
        libc_memset(SRAM_S3BT_BUFFER, 0x0, 0x6000);
    }
    uninstall_bt_stack(standby_enter, need_power_off);
    if (need_power_off == TRUE)
    {
        g_app_info_state.patch_download_flag = FALSE;
        g_app_info_state.bt_power_on = FALSE;
    }
    if (standby_enter == TRUE)
    {
        sd_sec_param_t sd_sec_param;

        while (g_btmanager_gl_var.btstack_standby == FALSE)
        {
            sys_udelay(1);
        }

        //sd_sec_param.file_offset = NOR_S3BT_BT_STACK;
        //sd_sec_param.sram_addr = SRAM_S3BT_BUFFER;
        //sd_sec_param.sec_num = 0x6000/0x200;
        //base_ext_vram_write(&sd_sec_param);

        //这里写入比较多的VRAM数据，需要特殊处理
        *(uint16 *)(SRAM_S3BT_BUFFER - 2) = VM_LARGE_DATA_TAG;
        
        sys_vm_write((SRAM_S3BT_BUFFER - 2), VM_S3BT_BT_STACK, VM_S3BT_BT_STACK_LEN);  

        //放到最后面再写，防止建表误判
        sys_vm_write(temp_buffer, VM_S3BT_BT_MANAGER, sizeof(g_btmanager_gl_var) + sizeof(g_bt_auto_connect_ctrl));

        sys_free(temp_buffer);
    }

    //删除BT STACK调频点
    adjust_freq_set_level(AP_BT_HIGH_PRIO,FREQ_NULL,FREQ_NULL);
}

static void btmanager_global_var_init(bool standby_exit)
{
    //uint8 temp_data[64];
    
    if (standby_exit == TRUE)
    {
    	uint8 *temp_data;

        temp_data = sys_malloc(sizeof(g_btmanager_gl_var) + sizeof(g_bt_auto_connect_ctrl));

        sys_vm_read(temp_data, VM_S3BT_BT_MANAGER, sizeof(g_btmanager_gl_var) + sizeof(g_bt_auto_connect_ctrl));

        libc_memcpy(&g_btmanager_gl_var, temp_data, sizeof(g_btmanager_gl_var));
        libc_memcpy(&g_bt_auto_connect_ctrl, (temp_data + sizeof(g_btmanager_gl_var)), sizeof(g_bt_auto_connect_ctrl));
        sys_free(temp_data);
    }
    else
    {//重新加载并初始化全局变量
#ifdef ENABLE_TRUE_WIRELESS_STEREO
        send_once_flag=0;
        g_tws_m_linein_flag = 0;
#endif

        //初始化全局变量
        libc_memset(&g_btmanager_gl_var, 0x0, sizeof(g_btmanager_gl_var));

        //初始化非0变量
        g_btmanager_gl_var.avrcp_timer_id = -1;
        g_btmanager_gl_var.a2dp_timer_id = -1;
        g_btmanager_gl_var.sniff_timer_id = -1;
        g_btmanager_gl_var.bat_rpt_timer_id = -1;
        g_btmanager_gl_var.discoverable_flag = 0; //BT STACK默认不可见的
        g_btmanager_gl_var.discover_ctl = 0;//defualt 
        
        //for fast disoverable mode 
        g_fast_discoverable_timer = -1;
        g_fast_discoverable_cnt = 0;

        //支持多少个远程设备
        g_btmanager_gl_var.support_dev_num = (uint8) com_get_config_default(BTMANAGER_SUPPORT_DEVICE_NUM);
        g_btmanager_gl_var.max_paired_num = (uint8) com_get_config_default(BTMANAGER_MAX_PAIRED_LIST_MEMBER);

        //初始化回连控制结构体
        libc_memset(&g_bt_auto_connect_ctrl, 0x0, sizeof(g_bt_auto_connect_ctrl));

        g_bt_auto_connect_ctrl.enable_auto_connect = (uint8) com_get_config_default(BTMANAGER_ENABLE_AUTO_CONNECT);
        g_bt_auto_connect_ctrl.reconnect_another_limit =
                (uint8) com_get_config_default(BTMANAGER_RECONNECT_ANOTHER_TIMES);
        g_bt_auto_connect_ctrl.auto_connect_count_timeout =
                (uint8) com_get_config_default(BTMANAGER_AUTO_CONNECT_COUNT_TIMEOUT);
        g_bt_auto_connect_ctrl.auto_connect_count_startup =
                (uint8) com_get_config_default(BTMANAGER_AUTO_CONNECT_COUNT_STARTUP);
        g_bt_auto_connect_ctrl.auto_connect_count_mode =
                (uint8) com_get_config_default(BTMANAGER_AUTO_CONNECT_COUNT_MODE);
        g_bt_auto_connect_ctrl.auto_connect_interval =
                (uint8) com_get_config_default(BTMANAGER_AUTO_CONNECT_INTERVAL);

        //g_bt_auto_connect_ctrl.connect_tick_cnt = 0;
        g_bt_auto_connect_ctrl.connect_tick_cnt = g_bt_auto_connect_ctrl.auto_connect_interval * 2;
    }

    if(g_btmanager_gl_var.support_dev_num > 1)
    {
        //支持的设备个数大于1个，则需要使用send buffer
        //需要系统去掉这部分空间的管理
        sys_disable_use_send_buffer();
    }

    g_btmanager_gl_var.last_call_last_timestamp = sys_get_ab_timer();

}

static void load_bt_stack_config(btstack_config_t *cfg)
{
// 配置管理，OTA时只需要SPP、BLE
#ifndef OTA_VERSION
    cfg->enable_a2dp = com_get_config_default(SETTING_APP_SUPPORT_BTPLAY);
    cfg->enable_a2dp_cp = com_get_config_default(BTMANAGER_ENABLE_A2DP_CP);
    cfg->enable_avdtp_aac = com_get_config_default(BTMANAGER_ENABLE_AVDTP_AAC);
    cfg->enable_avrcp = com_get_config_default(BTMANAGER_ENABLE_AVRCP);
    cfg->enable_avrcp_14 = com_get_config_default(BTMANAGER_ENABLE_AVRCP_14);
    cfg->enable_avrcp_volume_sync = com_get_config_default(BTMANAGER_ENABLE_AVRCP_VOLUME_SYNC);
    cfg->enable_hfp = (com_get_config_default(BTMANAGER_ENABLE_HFP) == 1)
            && (com_get_config_default(SETTING_APP_SUPPORT_BTCALL) == 1); //必须使能BTCALL应用才使能HFP服务
    cfg->enable_hfp_volume_sync = (cfg->enable_hfp == 1)
            && (com_get_config_default(BTMANAGER_ENABLE_HFP_VOLUME_SYNC) == 1); //必须使能HFP服务才使能HFP音量同步服务
    cfg->enable_hfp_bat_report = (cfg->enable_hfp == 1)
            && (com_get_config_default(BTMANAGER_ENABLE_HFP_BAT_REPORT) == 1); //先使能HFP服务才使能HFP电池电量同步服务
    cfg->enable_hfp_phonebook = (cfg->enable_hfp == 1)
            && (com_get_config_default(BTMANAGER_ENABLE_HFP_PHONEBOOK) == 1); //必须使能HFP服务才使能电话本同步服务
    cfg->enable_spp = com_get_config_default(BTMANAGER_ENABLE_SPP);
    cfg->enable_ble = com_get_config_default(BTMANAGER_ENABLE_BLE);
    cfg->enable_hid = com_get_config_default(BTMANAGER_ENABLE_HID);
    cfg->enable_3way_call = com_get_config_default(BTMANAGER_ENABLE_THREEWAY_CALL);
    cfg->enable_ancs = com_get_config_default(BTMANAGER_ENABLE_ANCS);
    cfg->enable_tip = com_get_config_default(BTMANAGER_ENABLE_TIP);
    cfg->enable_siri = com_get_config_default(BTMANAGER_ENABLE_SIRI);
    cfg->enable_msbc = com_get_config_default(BTMANAGER_ENABLE_MSBC);
#else
    cfg->enable_a2dp = com_get_config_default(SETTING_APP_SUPPORT_BTPLAY);
    cfg->enable_a2dp_cp = 0;
    cfg->enable_avdtp_aac = 0;
    cfg->enable_avrcp = 0;
    cfg->enable_avrcp_14 = 0;
    cfg->enable_avrcp_volume_sync = 0;
    cfg->enable_hfp = 0; //必须使能BTCALL应用才使能HFP服务
    cfg->enable_hfp_volume_sync = 0; //必须使能HFP服务才使能HFP音量同步服务
    cfg->enable_hfp_bat_report = 0; //必须使能HFP服务才使能HFP电池电量同步服务
    cfg->enable_hfp_phonebook = 0; //必须使能HFP服务才使能电话本同步服务
    cfg->enable_spp = 1;//com_get_config_default(BTMANAGER_ENABLE_SPP);
    cfg->enable_ble = 1;//com_get_config_default(BTMANAGER_ENABLE_BLE);
    cfg->enable_hid = 0;
    cfg->enable_3way_call = 0;
    cfg->enable_ancs = 0;
    cfg->enable_tip = 0;
    cfg->enable_siri = 0;

#endif

    if(g_app_info_state.stub_tools_type == STUB_PC_TOOL_ATT_MODE)
    {
        cfg->enable_hfp = 1;
        cfg->enable_a2dp = 1;
        cfg->enable_hfp_volume_sync = 0;
        cfg->enable_avrcp_volume_sync = 0;
    }
}

static void install_bt_stack(bool standby_exit, bool need_patch_download, bool need_power_on)
{
    msg_apps_t msg;
    create_app_param_t param;
    bt_stack_arg_t bt_stack_arg;
    uint8 save_bt_data;

    if(g_app_info_state.stub_tools_type == 0)
    {
        bt_stack_arg.work_mode = WORK_NORNAL | WORK_MEM_NORMAL;
    }
    else
    {
        if (g_app_info_state.stub_tools_type != STUB_PC_TOOL_ATT_MODE)
        {
            bt_stack_arg.work_mode = WORK_NORNAL | WORK_MEM_SMALL;
        }
        else
        {
            bt_stack_arg.work_mode = WORK_AUTOTEST | WORK_MEM_SMALL;        
        }
    }

    
    bt_stack_arg.standby_exit = standby_exit;
    bt_stack_arg.need_patch_download = need_patch_download;
    bt_stack_arg.need_power_on = need_power_on;
    bt_stack_arg.uart_idle_timer = BT_UART_IDLE_TIMER;

    if (g_app_info_state.stub_tools_type == STUB_PC_TOOL_BTT_MODE)
    {
        bt_stack_arg.btt_test_mode = 1;
    }
    else
    {
        bt_stack_arg.btt_test_mode = 0;
    }
    load_bt_stack_config(&(bt_stack_arg.cfg));

    bt_stack_arg.volume = com_btmanager_avrcp_volume2avrcp_volume(sys_comval->volume[VOLUME_MODE_NORMAL]);

    sys_get_fw_info(&save_bt_data, FW_INFO_BT_SAVE_DATA, FW_INFO_BT_SAVE_DATA_LEN);

    if(save_bt_data == SAVE_BT_ALL)
    {
        bt_stack_arg.save_addr = 1;
        bt_stack_arg.save_name = 1;
    }
    else
    {
        if(save_bt_data == SAVE_BT_ADDR)
        {
            bt_stack_arg.save_addr = 1;
        }
        else
        {
            bt_stack_arg.save_addr = 0;
        }

        if(save_bt_data == SAVE_BT_NAME)
        {
            bt_stack_arg.save_name = 1;
        }
        else
        {
            bt_stack_arg.save_name = 0;
        }
    }

    bt_stack_arg.random_lap = com_btmanager_gen_random_lap();

    bt_stack_arg.max_paired_num = g_btmanager_gl_var.max_paired_num;
    bt_stack_arg.p_btstack_standby = &(g_btmanager_gl_var.btstack_standby);

    msg.type = MSG_CREAT_APP_EXT_SYNC;
    param.app_id = APP_ID_BTSTACK;
    param.data = &bt_stack_arg;
    msg.content.addr = &param;

    send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);

    //BT STACK 可以对 btstack_config_t cfg 进行修改，所以BT MANAGER 需要更新
    g_bt_auto_connect_ctrl.enable_a2dp = bt_stack_arg.cfg.enable_a2dp;
    g_bt_auto_connect_ctrl.enable_avrcp = bt_stack_arg.cfg.enable_avrcp;
    g_bt_auto_connect_ctrl.enable_hfp = bt_stack_arg.cfg.enable_hfp;
    g_bt_auto_connect_ctrl.enable_hid = bt_stack_arg.cfg.enable_hid;
    
    g_btmanager_gl_var.enable_spp = bt_stack_arg.cfg.enable_spp;
    g_btmanager_gl_var.enable_ble = bt_stack_arg.cfg.enable_ble;
    g_btmanager_gl_var.enable_avrcp_volume_sync = bt_stack_arg.cfg.enable_avrcp_volume_sync;
    g_btmanager_gl_var.enable_siri = bt_stack_arg.cfg.enable_siri;
}

static void uninstall_bt_stack(bool standby_enter, bool need_power_off)
{
    msg_apps_t msg;

    msg.type = MSG_KILL_APP_SYNC;
    msg.content.data[0] = APP_ID_BTSTACK;
    msg.content.data[1] = 0;
    if (standby_enter == TRUE)
    {
        msg.content.data[1] |= (1 << 0);
    }
    if (need_power_off == TRUE)
    {
        msg.content.data[1] |= (1 << 1);
    }

    //发送同步消息
    send_sync_msg(APP_ID_MANAGER, &msg, NULL, 0);
}

//获取设备配对列表，以决定是否回连
void __section__(".bank_2") auto_connect_sync_rmt_device_list(void)
{
    bt_paired_info_t *cur_list;

    msg_apps_t msg;
    uint8 i, j;
    uint8 auto_ptr = 0;
    uint8 support_dev_num_tmp;
    
    support_dev_num_tmp = g_btmanager_gl_var.support_dev_num;

    cur_list = sys_malloc(sizeof(bt_paired_info_t));

    if(NULL == cur_list)
    {
        libc_print("malloc pairlist fail", 0, 0);
        return;
    }

#ifdef ENABLE_TRUE_WIRELESS_STEREO
    libc_memset(cur_list, 0x0, sizeof(bt_paired_info_t));
    msg.type = MSG_BTSTACK_GET_TWS_PAIRED_LIST_SYNC;
    msg.content.addr = cur_list;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);

    PRINT_INFO_INT("tws_dev_num:", cur_list->dev_num);
    
    
    if ((cur_list->dev_num > 0) && (g_bt_auto_connect_ctrl.enable_auto_connect == 1))
    {
        if (g_ap_switch_var.forbid_auto_connect == FALSE)
        {
            g_bt_auto_connect_ctrl.need_auto_connect = 0x01;
        }

        PRINT_INFO_INT("tws_mru_num:", cur_list->dev[0].mru_num);

        libc_memcpy(g_bt_auto_connect_ctrl.dev_info[0].remote_addr.bytes,
                cur_list->dev[0].bd_addr.bytes, 6);
        g_bt_auto_connect_ctrl.dev_info[0].support_profile = cur_list->dev[0].support_profile;
        g_bt_auto_connect_ctrl.dev_info[0].conn_flag = 1;
        //for autoconn cnt;
        g_bt_auto_connect_ctrl.dev_info[0].connect_cnt = 0;

        g_bt_auto_connect_ctrl.dev_info[0].dev_type = cur_list->dev[0].dev_type;
        g_bt_auto_connect_ctrl.tws_prior_role = cur_list->dev_role;
        
        if (g_bt_auto_connect_ctrl.tws_prior_role == TWS_SLAVE)
        {
        	g_bt_auto_connect_ctrl.connect_tick_cnt = g_bt_auto_connect_ctrl.auto_connect_interval;
        }	

        auto_ptr = 1;
        PRINT_INFO_INT("tws_profile:", g_bt_auto_connect_ctrl.dev_info[0].support_profile);
          
        check_id1 = 0;	             	
    }
    
    if (support_dev_num_tmp > 1)
    {
        support_dev_num_tmp--; 
    }
    
#endif    

    libc_memset(cur_list, 0x0, sizeof(bt_paired_info_t));
    msg.type = MSG_BTSTACK_GET_PAIRED_LIST_SYNC;
    msg.content.addr = cur_list;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);
    
    PRINT_INFO_INT("dev_num:", cur_list->dev_num);

    if ((cur_list->dev_num > 0) && (g_bt_auto_connect_ctrl.enable_auto_connect == 1))
    {
        if (g_ap_switch_var.forbid_auto_connect == FALSE)
        {
            g_bt_auto_connect_ctrl.need_auto_connect = 0x01;
        }

        for (i = 0; i < cur_list->dev_num; i++)
        {
            PRINT_INFO_INT("mru_num:", cur_list->dev[i].mru_num);

            for (j = 0; j < support_dev_num_tmp; j++)
            {
                if ((cur_list->dev[i].mru_num == (j + 1))&&((auto_ptr+j)<RMT_DEV_NUM))
                {
                    libc_memcpy(g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].remote_addr.bytes,
                            cur_list->dev[i].bd_addr.bytes, 6);
                    g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].support_profile = cur_list->dev[i].support_profile;
                    g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].conn_flag = 1;                   
                    
                    //for autoconn cnt;
                    g_bt_auto_connect_ctrl.dev_info[0].connect_cnt = 0;
                    
                    //PRINT_INFO_INT("disctl:", g_btmanager_gl_var.discover_ctl);
                    
                    if(g_btmanager_gl_var.discover_ctl == 1)
                    {
                    	g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].conn_flag = 0;
                    }                    

#ifdef ENABLE_TRUE_WIRELESS_STEREO
                    g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].dev_type = cur_list->dev[i].dev_type;
                    g_bt_auto_connect_ctrl.tws_prior_role = cur_list->dev_role;
                    if (g_bt_auto_connect_ctrl.tws_prior_role == TWS_SLAVE)
                    {
                    	g_bt_auto_connect_ctrl.connect_tick_cnt = g_bt_auto_connect_ctrl.auto_connect_interval;
                    	
                    	//从箱如果连接过手机,也不进行手机回连
                    	if (g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].dev_type == NO_TWS_DEV)
                    	{
                    		g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].conn_flag = 0;
                    		PRINT_INFO("not auto phone");
                    	}	
                    }
                    once_auto_flag = 0;
                    check_id1 = 0;	
#endif
                    PRINT_INFO_INT("profile:", g_bt_auto_connect_ctrl.dev_info[auto_ptr+j].support_profile);
                }
            }
        }
                	
    }

    sys_free(cur_list);
    g_ap_switch_var.forbid_auto_connect = FALSE;
}

//打印BT STACK版本号
void __section__(".bank_3") print_btstack_ver(void)
{
    msg_apps_t msg;
    uint8 ver_buf[16];

    msg.type = MSG_BTSTACK_GET_BTSTACK_VER_SYNC;
    msg.content.addr = ver_buf;
    send_sync_msg_btmanager(NULL, &msg, NULL, 0);

    PRINT_INFO("BT STACK VER: ");
    PRINT_INFO_STR(ver_buf);

    PRINT_INFO("BT CONTROLLER VER: ");
    PRINT_INFO_STR(ver_buf + 8);
}

void __section__(".bank_2") com_restart_auto_connect_set(void)
{
    g_bt_auto_connect_ctrl.auto_connect_type = AUTO_CONNECT_TYPE_MODE;
     
    auto_connect_sync_rmt_device_list();     
    
    g_bt_auto_connect_ctrl.connect_tick_cnt = (g_bt_auto_connect_ctrl.auto_connect_interval*2);   

}

void __section__(".bank_3") com_set_discover_ctl(uint8 mode)
{	
	g_btmanager_gl_var.discover_ctl = mode;
	auto_connect_sync_rmt_device_list();
}

