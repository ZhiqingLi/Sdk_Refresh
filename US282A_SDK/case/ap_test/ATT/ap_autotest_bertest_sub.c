/*******************************************************************************
 *                              US212A
 *                            Module: Picture
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-19 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     user1_main.c
 * \brief    picture主模块，负责进程初始化，退出处理，场景调度
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"
#include "ap_autotest_mptest.h"
#include "bt_controller_interface.h"
#include "ap_autotest_bertest.h"

static const uint8 bt_drv_name[] = "bt_ctrl.drv";

/** 
 功能: 上报测试结果给PC
 参数: 
 返回:
 说明:
*/
static void _ber_test_report_result(ber_test_arg_t *ber_arg, uint8 ret_val)
{
    return_result_t *return_data;
    uint16 trans_bytes = 0;

    return_data = (return_result_t *)(STUB_ATT_RETURN_DATA_BUFFER);

    if(g_ber_control->rssi_test == FALSE)
    {
        return_data->test_id = TESTID_BER_TEST;
    }
    else
    {
        return_data->test_id = TESTID_RSSI_TEST;
    }
    
    return_data->test_result = ret_val;

    /** 0x002c是参数分隔符
    */
    int32_to_unicode(ber_arg->ber_channel_low, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10); 
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->ber_channel_mid, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10); 
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->ber_channel_high, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10); 
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->ber_thr_low, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);  
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->ber_thr_high, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);  
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->rssi_thr_low, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);  
    return_data->return_arg[trans_bytes++] = 0x002c;
    int32_to_unicode(ber_arg->rssi_thr_high, &(return_data->return_arg[trans_bytes]), &trans_bytes, 10);      
    
    /** 添加结束符,如果参数未四字节对齐，要四字节对齐处理
    */
    return_data->return_arg[trans_bytes++] = 0x0000;
    if((trans_bytes % 2) != 0)
    {
        return_data->return_arg[trans_bytes++] = 0x0000;    
    }        

    act_test_report_result(return_data, trans_bytes*2 + 4);
}



 /** 
 功能: 通知PC工具停止测试
 参数: 
 返回:
 说明:
*/
static uint8  _ber_test_stop_att(ber_test_arg_t *ber_arg, uint32 test_result)
{
    uint32 ret_val;   
    uint32 read_len;
    uint8 *pdata;

    print_log("ber att stop!");

    /** 灵敏度的阈值
    */
    //g_ber_control->dut2pc_data.ber_thr = ber_arg->ber_thr;

    /** 把测试结果发给ATT工具并通知ATT工具stop
    */
    libc_memcpy(STUB_ATT_RW_TEMP_BUFFER, &(g_ber_control->dut2pc_data), sizeof(ber_update_arg_t));
    ret_val = att_write_data(STUB_CMD_ATT_BER_TX_STOP, sizeof(ber_update_arg_t), STUB_ATT_RW_TEMP_BUFFER);
    if(ret_val == 0)
    {       
        read_len = sizeof(ber_retrun_arg_t);
        ret_val = att_read_data(STUB_CMD_ATT_BER_TX_STOP, read_len, STUB_ATT_RW_TEMP_BUFFER); 
        if(ret_val == 0)
        {       
            pdata = (uint8 *)STUB_ATT_RW_TEMP_BUFFER;

            /** 从PC读回来的数据是否需要从bytes 6开始???
            */
            libc_memcpy(&(g_ber_control->pc2dut_data), &pdata[0], read_len);
        }
    }

    if(test_result == TRUE)
    {
        /** 配置RX stop
        */
        _ber_test_write_berdata(PKT_CFO_STOP_INDEX);
    }
    return TRUE; 
}



/** 
 功能: 读berdata.bin,用于BEGINE&STOP RX
 参数: 
 返回: 成功返回TRUE, 失败返回FALSE
 说明: 
*/
static uint8 _ber_test_read_berdata(ber_test_arg_t *ber_arg)
{
    uint32 i;
    atf_head_t *atf_head;
    uint8 *data_buffer = (uint8 *)ATT_MPDATA_TEMP_BUFFER;
    
    att_test_fread(data_buffer, 512, 0);
    
    atf_head = (atf_head_t *) data_buffer;
    for (i = 0; i < (512 / sizeof(atf_dir_t)); i++)
    {
        if (libc_memcmp(atf_head->atf_dir[i].filename, BER_HCI_CMD_BIN, 10) == 0)
        {
            break;
        }
    }  

    g_ber_control->berdata_len = atf_head->atf_dir[i].length;
    
    /** 读取HCI命令
    */
    if(i < (512 / sizeof(atf_dir_t)))
    {
        if(g_ber_control->berdata_len <= ATT_MPDATA_MEX_LENGTH)
        {
            att_test_fread(data_buffer, atf_head->atf_dir[i].length, atf_head->atf_dir[i].offset);
            return TRUE;
        }
    }

    return FALSE;
}

/** 
 功能: 通知PC工具开始测试
 参数: 
 返回:
 说明:
*/
static uint8  _ber_test_start_att(ber_test_arg_t *ber_arg)
{
    uint32 ret_val;
    
    ber_param_t ber_param;


    /** 通知ATT工具START
    */
    libc_memset(&ber_param, 0, sizeof(ber_param_t));
    ber_param.ic_type = MP_ICTYPE;
    ber_param.channel = ber_arg->ber_channel_low;
    ber_param.tx_gain_idx = MP_TX_GAIN_IDX;
    ber_param.tx_gain_val = MP_TX_GAIN_VAL;    
    //ber_param.payload = PAYLOADTYPE_SET;
    ber_param.payload = BT_PAYLOAD_TYPE_1111_0000;
    ber_param.pkt_type = PKTTYPE_SET;  
    ber_param.tx_dac = MP_TX_DAC;
    ber_param.whitening_cv = WHITENCOEFF_SET;
    ber_param.pkt_header = PKTHEADER_SET;
    ber_param.tx_pkt_cnt = BER_TX_PKT_CNT;
    ber_param.hit_target_l = HIT_ADDRESS_SET_L;
    ber_param.hit_target_h = HIT_ADDRESS_SET_H;
    ber_param.sut_state = 0;
    ber_param.report_interval = BER_TX_REPORT_INTERVAL;
    ber_param.report_timeout = BER_TX_REPORT_TIMEOUT;

    libc_memcpy((uint8 *)(STUB_ATT_RW_TEMP_BUFFER + sizeof(stub_ext_cmd_t)), &ber_param, sizeof(ber_param_t));
    ret_val = att_write_data(STUB_CMD_ATT_BER_TX_BEGIN, sizeof(ber_param_t), STUB_ATT_RW_TEMP_BUFFER);
    
    if(ret_val == 0)
    {
        att_read_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);
    }

          
    return TRUE;
}

/** 
 功能: 模块测试初始化
 参数: ber_test_arg  编辑工具配置的参数信息 
 返回:
 说明:
*/
uint8 _ber_test_init(ber_test_arg_t *ber_arg)
{
    uint32 retry_cnt;
    uint32 ret_val;
    bt_drv_param_t tmp_bt_drv_param;
    btt_priv_data_t btt_priv_data;  

    /** 初始化全局控制结构
    */
    g_ber_control = (ber_control_t *)ATT_RINGBUFFER_ADDR;
    libc_memset(g_ber_control, 0, sizeof(ber_control_t));
    g_ber_control->get_pktinfo_timer = -1;
    g_ber_control->ber_test_state = BER_STATE_STOP;

    retry_cnt = 0;

retry:    
    if(sys_get_drv_install_info(DRV_GROUP_BT) > 0)
    {
        sys_drv_uninstall(DRV_GROUP_BT);
    }

    /** 重装载BT驱动
    */
    tmp_bt_drv_param.cbk = _ber_test_read_hci_data;
    tmp_bt_drv_param.bd_addr[0] = 0x00;
    tmp_bt_drv_param.bd_addr[1] = 0x01;
    tmp_bt_drv_param.bd_addr[2] = 0x02;   
    tmp_bt_drv_param.bd_addr[3] = 0xfd;
    tmp_bt_drv_param.bd_addr[4] = 0x4e;
    tmp_bt_drv_param.bd_addr[5] = 0xf4;
    tmp_bt_drv_param.uart_idle_timer = 5000;
    tmp_bt_drv_param.p_hci_buffer_get_len_cb = &(g_ber_control->g_hci_deal.get_data_len);
    tmp_bt_drv_param.p_hci_buffer_read_data_cb = &(g_ber_control->g_hci_deal.read_data);
    sys_drv_install(DRV_GROUP_BT, &tmp_bt_drv_param, bt_drv_name);

    bt_drv_controller_poweron();

    sys_mdelay(500);

    /** 加载PATCH CODE
    */
    btt_priv_data.download_patch = FALSE;
    btt_priv_data.reset_controller = TRUE;    
    bt_drv_down_patchcode(TRUE, BT_BTT_MODE, &btt_priv_data); 

    /** pkt type为1 bytes
    */
    g_ber_control->g_hci_deal.minReqLen = 3;
    g_ber_control->g_hci_deal.reqRxParseStatus = UART_HCI_STATUS_RX_TYPE;
    g_ber_control->cmd_state = BER_CMD_STATE_WRITE_PACKET_NUM;

    /** 加载berdata.bin
    */
    if(_ber_test_read_berdata(ber_arg) == FALSE)
    {
        return FALSE;
    }

    /** 配置RX begin
    */
    ret_val = _ber_test_write_berdata(PKT_CFO_BEGINE_INDEX);

    if(ret_val == FALSE)
    {
        retry_cnt++;

        if(retry_cnt < 3)
        { 
            print_log("retry start ber %d", retry_cnt);        
            goto retry;
        }
        else
        {
            return FALSE;
        }
    }
    g_ber_control->ber_test_state = BER_STATE_START;    

    return TRUE;
}


/** 
 功能: 测试蓝牙的灵敏度
 参数: arg_buffer  编辑工具配置的参数信息 
 返回: 测试结果
 说明: 1. 使用Golden sample不能正确测试灵敏度;
       2. 这个函数测试的实际上不是灵敏度,而是灵敏度相关的可用于一致性参考的值;
*/
test_result_e act_test_ber_test(void *arg_buffer)
{
    ber_test_arg_t *ber_arg;
    
    int32 ret_val;

    int32 retry_cnt = 0;    
        
    print_log("enter ber_test !");

    ber_arg = (ber_test_arg_t *)arg_buffer;

retry:
    /** 启动SUT的tx pkt发送
    */
    if(_ber_test_start_att(ber_arg) == FALSE)
    {
        print_log("ber_test start fail !");
        ret_val = FALSE;
        goto ber_test_end;
    }

    sys_mdelay(20);

    /** 蓝牙和变量初始化
    */
    if(_ber_test_init(ber_arg) == FALSE)
    {
        print_log("ber_test init fail !");
        ret_val = FALSE;
        goto ber_test_end;
    }  

    print_log("ber att start!");

    sys_mdelay(20);
    
    ret_val = ber_test_loop_deal(ber_arg);

    retry_cnt++;

    ber_test_end:
    /** finish test
    */
    _ber_test_stop_att(ber_arg, ret_val);
    
    if(sys_get_drv_install_info(DRV_GROUP_BT) > 0)
    {        
        bt_drv_controller_poweroff();
        
        sys_drv_uninstall(DRV_GROUP_BT);
    }     

    if(retry_cnt < 3 && ret_val == FALSE)
    {               
        goto retry;
    } 
   
    _ber_test_report_result(ber_arg, ret_val);
    
    return TEST_PASS;
}


/** 
 功能: 测试蓝牙的灵敏度
 参数: arg_buffer  编辑工具配置的参数信息 
 返回: 测试结果
 说明: 1. 使用Golden sample不能正确测试灵敏度;
       2. 这个函数测试的实际上不是灵敏度,而是灵敏度相关的可用于一致性参考的值;
*/
test_result_e act_test_rssi_test(void *arg_buffer)
{
    ber_test_arg_t *ber_arg;
    
    int32 ret_val;

    int32 retry_cnt = 0;
        
    print_log("enter ber_test !");

    ber_arg = (ber_test_arg_t *)arg_buffer;

retry:
    /** 启动SUT的tx pkt发送
    */
    if(_ber_test_start_att(ber_arg) == FALSE)
    {
        print_log("ber_test start fail !");
        ret_val = FALSE;
        goto ber_test_end;
    }

    sys_mdelay(20);

    /** 蓝牙和变量初始化
    */
    if(_ber_test_init(ber_arg) == FALSE)
    {
        print_log("ber_test init fail !");
        ret_val = FALSE;
        goto ber_test_end;
    }   

    print_log("ber att start!");

    sys_mdelay(20);

    g_ber_control->rssi_test = TRUE;

    ret_val = ber_test_loop_deal(ber_arg);

    retry_cnt++;

    ber_test_end:
    /** finish test
    */
    _ber_test_stop_att(ber_arg, ret_val);
    
    if(sys_get_drv_install_info(DRV_GROUP_BT) > 0)
    {    
        bt_drv_controller_poweroff();
        
        sys_drv_uninstall(DRV_GROUP_BT);
    }     

    if(retry_cnt < 3 && ret_val == FALSE)
    {               
        goto retry;
    }  

    _ber_test_report_result(ber_arg, ret_val);
    
    return TEST_PASS;
}

