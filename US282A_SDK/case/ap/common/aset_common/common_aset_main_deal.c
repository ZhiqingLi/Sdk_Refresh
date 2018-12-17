/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   主函数，音效调试
 *      <author>       <time>
 *       Wekan        2015-4-13       review
 *******************************************************************************/
#include "common_aset_new_func.h"
#include "common_aset_interface.h"

#define ASET_WORK_BUFFER_SIZE  (256)

dae_config_t * __section__(".gl_stub_data") g_p_dae_cfg;

uint32 __section__(".gl_stub_data") g_aset_base_timer;

aset_tools_run_state_t __section__(".gl_stub_data") g_aset_run_state;

bool __section__(".gl_stub_data") g_aset_sync_volume;
bool __section__(".gl_stub_data") g_aset_neednot_take_effect;
uint16 __section__(".gl_stub_data") g_aset_main_gain_value;
uint16 __section__(".gl_stub_data") g_aset_equivalent_gain;
bool __section__(".gl_stub_data") g_aset_main_switch_flag;
uint8 * __section__(".gl_stub_data") g_aset_rw_buffer; 

#ifdef SUPPORT_ASET_TEST

static  int8 aset_tools_open_ack(uint16 cmd)
{
    int ret_val;
    stub_ext_param_t stub_ext_param;

    uint8 cmd_data[8];

    stub_ext_param.opcode = cmd;
    stub_ext_param.payload_len = 0;
    stub_ext_param.rw_buffer = cmd_data;

    ret_val = stub_ext_write(&stub_ext_param);

    if (ret_val != 0)
    {
        PRINT_INFO("aset and small machine handshake failure");
    }

    return ret_val;
}

uint8 get_audiopp_type(void)
{
    uint32 audiopp_type;
    int ret_val;
    uint8 ret_audiopp_type; 
    uint8 recv_err_cnt = 0;
        
    retry:
        
    ret_val = aset_read_data(STUB_CMD_AUDIOPP_SELECT, &audiopp_type, 4);

    if (0 == ret_val)
    {
        ret_audiopp_type = (uint8)audiopp_type;
    }
    else
    {
        PRINT_INFO("get audiopp type fail!!");
            
        recv_err_cnt++;
        if (recv_err_cnt < 10)
        {
            goto retry;
        }
        else
        {
            ret_audiopp_type = SMART_MODE;
            recv_err_cnt = 0;
        }
    }
    
    PRINT_INFO_INT("audiopp_type",ret_audiopp_type);
    return ret_audiopp_type;
}

//更新应用的属性相关信息给工具
static void write_application_properties(void)
{
    int ret_val = 0;
    uint8 err_cnt = 0;
    
    application_properties_t* p_application_properties  = (application_properties_t *) ASET_RW_DATA_BUF;
    
    libc_memset(p_application_properties, 0x00, sizeof(application_properties_t));
    if ((APP_FUNC_PLAYLINEIN == g_app_info_state.cur_func_id) || (APP_FUNC_RADIO == g_app_info_state.cur_func_id))
    {
        p_application_properties->aux_mode = AUX_MODE;
    }
    else
    {
        p_application_properties->aux_mode = UNAUX_MODE;
    }

    retry:
    ret_val = aset_write_data(STUB_CMD_ASET_WRITE_APPLICATION_PROPERTIES, \
        p_application_properties, sizeof(application_properties_t));  

    //PC数据收不到STUB_CMD_ASET_WRITE_APPLICATION_PROPERTIES，最多try10次
    if (ret_val != 0)
    {
        err_cnt++;
        PRINT_INFO_INT("aset_write_app_pro_fail_cnt",err_cnt);
        if (err_cnt < 10)
        {
            goto retry;
        }
    }
    
    PRINT_INFO_INT("aux_mode",p_application_properties->aux_mode);
    PRINT_INFO_INT("cur_func_id",g_app_info_state.cur_func_id);
}

void aset_reconnect_deal(void)
{
    int8 ret_val = 0;
    
    if((g_aset_run_state.run_state == ASET_TOOLS_NOT_WORK)
            || (g_aset_run_state.run_state == ASET_TOOLS_WORK))
    {
        if((g_app_info_vector[APP_TYPE_GUI].app_id == APP_ID_RADIO)
                || (g_app_info_vector[APP_TYPE_GUI].app_id == APP_ID_LINEIN))
        {
            stub_ioctrl_set(SWITCH_URAM, STUB_USE_URAM0, 0);
        }
        else
        {
            stub_ioctrl_set(SWITCH_URAM, STUB_USE_URAM1, 0);
        }

        stub_ioctrl_set(SET_TIMEOUT, 50, 0);
        PRINT_INFO_INT("run_state",g_aset_run_state.run_state);
           
        if(g_aset_run_state.run_state == ASET_TOOLS_NOT_WORK)
        {
#ifdef WAVES_ASET_TOOLS
            ret_val = aset_tools_open_ack(STUB_CMD_WAVES_ASET_ACK);
#else 
            ret_val = aset_tools_open_ack(STUB_CMD_ASET_ACK);  
            g_p_dae_cfg->audiopp_type = get_audiopp_type();
#endif        
            if (0 == ret_val)
            {
                aset_write_case_info();

                g_aset_run_state.run_state = ASET_TOOLS_WORK;
            }
        }
        else
        {
                g_p_dae_cfg->audiopp_type = get_audiopp_type();
        }
        
#ifndef WAVES_ASET_TOOLS
        write_application_properties();
#endif

    }
    else
    {
        ;//nothing for QAC
    }
}

void aset_global_para_init(void)
{
    g_aset_run_state.err_cnt = 0;
    g_aset_run_state.run_state = 0; 
}

bool aset_test_init(void)
{
    //更新DAE配置参数到 sys_comval->dae_cfg
    g_p_dae_cfg = &(sys_comval->dae_cfg);

    g_p_dae_cfg->peq_enable = 1;
    g_p_dae_cfg->spk_compensation_enable = 0;
    
#ifdef WAVES_ASET_TOOLS    
    g_p_dae_cfg->bypass = 1;
#else
    g_p_dae_cfg->bypass = 0;
#endif
 
    g_aset_rw_buffer = sys_malloc(ASET_WORK_BUFFER_SIZE);

    libc_print("aset malloc", g_aset_rw_buffer, 2);

    aset_reconnect_deal();

    return TRUE;
}

void aset_test_exit(void)
{
    libc_print("aset free", g_aset_rw_buffer, 2);
    sys_free(g_aset_rw_buffer);
}

void aset_test_set_dae_init(void)
{
    aset_set_dae_init();
}

uint8 reinstall_stub(void)
{
    uint8 stub_type;
    msg_apps_t msg;
    msg_reply_t msg_reply;
    bool need_uninstall_led;

    if (sys_comval->support_led_display == 1)
    {
        need_uninstall_led = TRUE;
    }
    else
    {
        need_uninstall_led = FALSE;
    }

    msg_reply.content = &stub_type;

    msg.type = MSG_STUB_INSTALL_SYNC;
    msg.content.data[0] = need_uninstall_led;

    send_sync_msg(APP_ID_MANAGER, &msg, &msg_reply, 0);

    g_aset_run_state.run_state = ASET_TOOLS_NOT_WORK;

    aset_reconnect_deal();

    return stub_type;
}

void aset_loop_deal(void)
{
    int ret_val = 0;

    aset_status_t aset_status;
    aset_waves_coeff_property_t    coeff_property;
    //libc_print("aset tools state:", g_aset_run_state.run_state, 2);

#if ((CASE_BOARD_TYPE != CASE_BOARD_ATS2823) && (CASE_BOARD_TYPE != CASE_BOARD_DVB_ATS2823))
    if(g_aset_run_state.run_state == ASET_TOOLS_DISCONNECT)
    {
        if (get_cable_state() == CABLE_STATE_CABLE_IN)
        {
            reinstall_stub();
        }
        else
        {
            return;
        }
    }

    if (get_cable_state() != CABLE_STATE_CABLE_IN)
    {
        //如果USB线断开连接，则卸载stub资源，等待重新建立连接
        g_aset_run_state.run_state = ASET_TOOLS_DISCONNECT;
        stub_close();
        sys_free_mmm(FALSE);
        return;
    }
#endif
#ifdef WAVES_ASET_TOOLS
   
   libc_memset(&coeff_property, 0, sizeof(aset_waves_coeff_property_t));

   aset_read_data(STUB_CMD_WAVES_ASET_READ_COEFF_PROPERTY, &coeff_property, sizeof(aset_waves_coeff_property_t));
       
   if (1 == coeff_property.update_flag)
   {
      coeff_property.update_flag = 0;
      //0x9fc3afe0之后0x1020数据用于缓存 WAVES工具和小机交互时，传输的数据信息 
      sys_disable_mem_use(0x3afe0,0x3afe0 + 0x1020);
      
      waves_set_effect_param(SET_WAVES_COEFF_PROPERTY,&coeff_property);   

      //类型改成void*
      ret_val = aset_read_data(STUB_CMD_WAVES_ASET_READ_COEFF_CONTENT, (void*)ASET_READ_DATA_BUF,coeff_property.length);       
        
      if (0 != ret_val)
      {

      }
      else
      { 
         waves_set_effect_param(SET_WAVES_EFFECT_PARAM,(uint8*)ASET_READ_DATA_BUF);  
      }
      
      //sys_enable_mem_use(0x3afe0,0x3afe0 + 0x1020); 
   }
  
 #else  
    ret_val = aset_read_data(STUB_CMD_ASET_READ_STATUS, &aset_status, sizeof(aset_status));

 #endif

    if (ret_val == 0)
    {
        if(g_aset_run_state.run_state == ASET_TOOLS_WORK)
        {
            aset_cmd_deal(&aset_status);
            g_aset_run_state.err_cnt = 0;
        }
        else if(g_aset_run_state.run_state == ASET_TOOLS_NOT_WORK)
        {          
#ifndef WAVES_ASET_TOOLS
            write_application_properties();//工具断开后，再重新连接，更新应用的属性相关信息给工具
            aset_update_audiopp();         //重新连接时要从工具端重新读取音效模型
            aset_write_case_info();
#endif            
            g_aset_run_state.run_state = ASET_TOOLS_WORK;
        }
        else
        {
            ;//nothing for QAC
        }
    }
    else
    {
        if(g_aset_run_state.err_cnt > 4)
        {
            if(g_aset_run_state.run_state == ASET_TOOLS_WORK)
            {
                g_aset_run_state.run_state = ASET_TOOLS_NOT_WORK;
            }
        }
        else
        {
            g_aset_run_state.err_cnt++;
        }
    }
}

void aset_loop_deal(void) __FAR__;

void __section__(".rcode") aset_test_loop_deal(void)
{
    uint32 tmp_ab_timer = sys_get_ab_timer();

#ifdef WAVES_ASET_TOOLS 

    aset_loop_deal();

#else
    //200ms执行一次任务
    if ((tmp_ab_timer - g_aset_base_timer) >= 200)
    {
        g_aset_base_timer = tmp_ab_timer;

        aset_loop_deal();

    }
#endif

}

#endif
