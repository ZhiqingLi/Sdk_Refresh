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
#include "stub_interface.h"
#include "stub_command.h"
#include "att_interface.h"

static uint8 att_temp_buffer[512] _BANK_DATA_ATTR_;

static void aset_tools_open_ack(uint16 cmd)
{
    stub_ext_param_t stub_ext_param;

    uint8 cmd_data[8];

    stub_ext_param.opcode = cmd;
    stub_ext_param.payload_len = 0;
    stub_ext_param.rw_buffer = &cmd_data;

    stub_ext_write(&stub_ext_param);

}

uint32 att_test_fread(uint8 *buffer, uint32 len, uint32 offset)
{
    int ret_val = 0;
    int read_len;
    uint8 cmd_data[16];
    uint32 loop_exit;
    att_fread_arg_t *att_fread_arg;
    stub_ext_param_t ext_param;
    uint32 true_offset = offset;

    while (len > 0)
    {
        if (len > 1024)
        {
            read_len = 1024;
        }
        else
        {
            //保证读取的数据是4字节对齐
            read_len = (((int)len + 3) / 4) * 4;
        }

        att_fread_arg = (att_fread_arg_t *) cmd_data;
        att_fread_arg->lengthl = (uint16)(read_len & 0xffff);
        att_fread_arg->lengthh = (uint16)(read_len >> 16);
        att_fread_arg->offsetl = (uint16)(true_offset & 0xffff);
        att_fread_arg->offseth = (uint16)(true_offset >> 16);

        ext_param.opcode = STUB_CMD_ATT_FREAD;
        ext_param.payload_len = 8;
        ext_param.rw_buffer = (uint8 *) &cmd_data;
        
        ret_val = stub_ext_write(&ext_param);

        if (ret_val == 0)
        {
            ext_param.payload_len = (uint16)read_len;
            ext_param.rw_buffer = (uint8 *) 0x3a100;
            ret_val = stub_ext_read(&ext_param);

            if (ret_val == 0)
            {
                if (read_len == 1024)
                {
                    libc_memcpy(buffer, &(ext_param.rw_buffer[6]), read_len);
                }
                else
                {
                    //拷贝实际长度
                    libc_memcpy(buffer, &(ext_param.rw_buffer[6]), len);
            
                    read_len = (int)len;
                }
                       
                //回复ACK
                ext_param.opcode = STUB_CMD_ATT_ACK;
                ext_param.payload_len = 0;
                ext_param.rw_buffer = (uint8 *) &cmd_data;
                ret_val = stub_ext_write(&ext_param);              
            }
        }

        buffer += read_len;
        len -= (uint32)read_len;
        true_offset += (uint32)read_len;
    }
    return ret_val;
}

app_result_e common_load_att_code(int32 file_sys_id, uint8 stg_base)
{
    int i;
    atf_head_t *atf_head;
    ap_head_t *head_buf;
    uint32 test_file_offset;
    int (*start)(int arg0, task_attr_t *task_attr);
    test_ap_info_t test_ap_info;
    uint32 ap_fp;

    head_buf = (ap_head_t *) att_temp_buffer;

    libc_memset(&test_ap_info, 0, sizeof(test_ap_info_t));

    if (stg_base == DISK_S)
    {
        stub_ioctrl_set(SET_TIMEOUT, 20, 0);
        
        stub_ioctrl_set(SET_READ_MODE, 1, 0);

        sys_os_sched_lock();
        stub_set_debug_mode(0);

        aset_tools_open_ack(0x04fe);

        sys_mdelay(2);

        sys_disable_use_temp_pool();

        if(g_app_info_state.stub_phy_type == STUB_PHY_USB)
        {
            act_writeb(act_readb(Out2cs_hcin2cs)|0x02,Out2cs_hcin2cs);
        }
        //act_writeb(act_readb(In1cs_hcout1cs) | 0x02, In1cs_hcout1cs);

        retry:
        libc_print("  \r\nload acttest.ap", 0, 0);

        //直接读取文件
        att_test_fread(att_temp_buffer, sizeof(att_temp_buffer), 0);

        atf_head = (atf_head_t *)att_temp_buffer;

        for (i = 0; i < 15; i++)
        {
            if (libc_memcmp(atf_head->atf_dir[i].filename, "acttest.ap", 10) == 0)
            {
                test_ap_info.ap_file_offset = atf_head->atf_dir[i].offset;
                break;
            }
        }

        if (i >= 15)
        {
            goto retry;
        }

        att_test_fread(head_buf, sizeof(ap_head_t), atf_head->atf_dir[i].offset);

        if (libc_memcmp(head_buf->magic, "yqhx", 4) != 0)
        {
            goto retry;
        }
        if ((head_buf->text_length) != 0x00)
        {
            att_test_fread(head_buf->text_addr, head_buf->text_length, head_buf->text_offset + \
                test_ap_info.ap_file_offset);
        }

        stub_set_debug_mode(1);
        sys_os_sched_unlock();
    }
    else
    {
        return RESULT_NULL;
    }

    test_ap_info.info_size   = sizeof(test_ap_info_t);
    test_ap_info.test_stage  = 0;
    test_ap_info.base_type   = stg_base;
    test_ap_info.stub_phy_type = g_app_info_state.stub_phy_type;

    start = (int(*)(int arg0, void *arg1)) head_buf->entry;

    //该函数注意卡驱动和文件系统没有卸载
    //在测试ap中会进行卸载，务必确保测试ap卸载相关驱动
    start(&test_ap_info, &(head_buf->task_attr));

    g_app_info_state.autotest_mode = TRUE;

    sys_os_sched_unlock();

    do  // config 应用挂起等待直到测试阶段 1;
    {
        sys_os_time_dly(10);
    }
    while (test_ap_info.test_stage == 0);

    return RESULT_ENTER_ASQT_HF_TEST;
}

app_result_e com_sys_install_stub(void *ev_param)
{
    uint8 stub_type;
    msg_apps_t msg;
    msg_reply_t msg_reply;
    app_result_e result = RESULT_NULL;
    bool need_uninstall_led = FALSE;

#if(SUPPORT_LED_DRIVER == 1)
    if (sys_comval->support_led_display == 1)
    {
        //显示STUB
        led_display(NUMBER1, NUM_S, 1);
        led_display(NUMBER2, NUM_t, 1);
        led_display(NUMBER3, NUM_U, 1);
        led_display(NUMBER4, NUM_b, 1);

        need_uninstall_led = TRUE;
    }
#endif

    msg_reply.content = &stub_type;

    msg.type = MSG_STUB_INSTALL_SYNC;
    msg.content.data[0] = need_uninstall_led;

    send_sync_msg(APP_ID_MANAGER, &msg, &msg_reply, 0);
    
    g_app_info_state_all.stub_mode_flag = 1;

    if (stub_type == STUB_PC_TOOL_ASET_EQ_MODE)
    {
        g_aset_run_state.err_cnt = 0;
        g_aset_run_state.run_state = ASET_TOOLS_NOT_WORK;
    }
    else if (stub_type == STUB_PC_TOOL_WAVES_ASET_MODE)
    {
        g_aset_run_state.err_cnt = 0;
        g_aset_run_state.run_state = ASET_TOOLS_NOT_WORK;
    }
    else if (stub_type == STUB_PC_TOOL_BTT_MODE)
    {
        result = RESULT_BLUETEETH_SOUND;
    }
    else if (stub_type == STUB_PC_TOOL_ASQT_MODE)
    {
        result = RESULT_ENTER_ASQT_HF_TEST;
    }
    else if (stub_type == STUB_PC_TOOL_ATT_MODE)
    {
        sys_disable_mem_use(0x38000, 0x3c000);
        
        //不允许写VRAM
        //DISABLE_VRAM_WRITE();
        result = common_load_att_code(0, DISK_S);
    }
    else
    {
        ; //nothing
    }

    return result;
}

