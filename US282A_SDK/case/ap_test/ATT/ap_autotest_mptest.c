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

ringbuf_rw_t g_ringbuf_rw;
btt_hci_deal_t g_hci_deal;
uint16 g_mpdata_bin_len;
volatile uint8 g_hci_event_rev_flag;
uint32 g_update_timer_id;
uint8 g_SUT_state;
uint8 g_cfo_test_retry_num;
uint8 g_add_cfo_result_flag; //当cfo测出多组异常时，尽量多采集几组cfo值标志位
uint8 g_cfo_return_num;
#if 1
//DH1的update命令字
const uint8 update_cmd0[] = { 0x01, 0x4a, 0xfd, 0x04, 0x82, 0xf1, 0xff, 0x00 };
const uint8 update_cmd1[] = { 0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf5, 0x0f, 0x00 };
const uint8 update_cmd2[] = { 0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf4, 0x0f, 0x00 };

//3DH5的update命令字
//const uint8 update_cmd0[] = {0x01, 0x4a, 0xfd, 0x04, 0x82, 0xf1, 0xff, 0x00};
//const uint8 update_cmd1[] = {0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf9, 0x0f, 0x00};
//const uint8 update_cmd2[] = {0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf8, 0x0f, 0x00};

//3DH1的update命令字
//const uint8 update_cmd0[] = {0x01, 0x4a, 0xfd, 0x04, 0x82, 0xf1, 0xff, 0x00};
//const uint8 update_cmd1[] = {0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf9, 0x01, 0x00};
//const uint8 update_cmd2[] = {0x01, 0x4a, 0xfd, 0x04, 0x97, 0xf8, 0x01, 0x00};

void dut_re_trigger_tx(void)
{
    //libc_print("update", 0, 0);

    bt_drv_send_data(update_cmd0, sizeof(update_cmd0));

    bt_drv_send_data(update_cmd1, sizeof(update_cmd1));

    bt_drv_send_data(update_cmd2, sizeof(update_cmd2));
}
#else
void dut_re_trigger_tx(void)
{
    //libc_print("update", 0, 0);
    att_write_hci_pkt(PKT_CFO_UPDATE_INDEX);
}
#endif

//将数据从hci buffer读取到ringbuf中
void put_hci_data_to_ringbuf(uint16 read_num, uint8 hci_overflow)
{
    uint16 data_len = g_hci_deal.get_data_len();

    //if(data_len == 5)
    //{
    //    while(1);
    //}

    if (data_len > 0)
    {
        //libc_print("rev", data_len, 2);

        g_hci_event_rev_flag = TRUE;

#if 0
        if ((g_ringbuf_rw.write_ptr + data_len) > ATT_RINGBUFFER_LEN)
        {
            g_hci_deal.read_data(&g_ringbuf_rw.read_buffer[g_ringbuf_rw.write_ptr], ATT_RINGBUFFER_LEN
                    - g_ringbuf_rw.write_ptr);

            g_hci_deal.read_data(&g_ringbuf_rw.read_buffer[0], data_len - ATT_RINGBUFFER_LEN + g_ringbuf_rw.write_ptr);

            g_ringbuf_rw.write_ptr = data_len - ATT_RINGBUFFER_LEN + g_ringbuf_rw.write_ptr;
        }
        else
#endif
        {
            g_hci_deal.read_data(&g_ringbuf_rw.read_buffer[g_ringbuf_rw.write_ptr], data_len);
            //PRINT_DATA(&g_ringbuf_rw.read_buffer[g_ringbuf_rw.write_ptr], data_len);
            //g_ringbuf_rw.write_ptr += data_len;
        }

        //g_ringbuf_rw.readable_len += data_len;

        //if (g_ringbuf_rw.readable_len > ATT_RINGBUFFER_LEN)
        //{
        //    PRINT_ERR_STR("ATT buffer overflow!");
        //}
    }
}

uint32 att_write_hci_pkt(uint32 pkt_index)
{
    uint32 i;
    cmd_pkt_head_t *cmd_pkt_head;
    uint8 *data_buffer;
    uint8 cmd_len;
    uint32 old_time;

    cmd_pkt_head = (cmd_pkt_head_t *) (ATT_MPDATA_TEMP_BUFFER);

    data_buffer = (uint8 *) cmd_pkt_head;

    g_hci_event_rev_flag = FALSE;

    //write_time = sys_get_ab_timer();

    for (;;)
    {
        if (cmd_pkt_head->pkt_index == pkt_index)
        {
            break;
        }

        if (cmd_pkt_head->next_pkt_offset >= g_mpdata_bin_len)
        {
            return FALSE;
        }

        cmd_pkt_head = (cmd_pkt_head_t *) ((uint8 *) ATT_MPDATA_TEMP_BUFFER + cmd_pkt_head->next_pkt_offset);
    }

    data_buffer = (uint8 *) ((uint8 *) cmd_pkt_head + sizeof(cmd_pkt_head_t));

    for (i = 0; i < cmd_pkt_head->total_node; i++)
    {
        cmd_len = *data_buffer;

        data_buffer++;

        if (cmd_len != 0)
        {
            //PRINT_DATA(data_buffer, cmd_len);
            bt_drv_send_data(data_buffer, cmd_len);

            old_time = sys_get_ab_timer();

            //等待hci事件返回
            while (1)
            {
                if (g_hci_event_rev_flag == TRUE)
                {
                    g_hci_event_rev_flag = FALSE;
                    break;
                }

                if ((sys_get_ab_timer() - old_time) > 1000)
                {
                    libc_print("hci cmd timeout!", 0, 0);

                    PRINT_DATA(data_buffer, cmd_len);

                    while (1)
                        ;

                    //break;
                }
            }

            data_buffer += cmd_len;
        }
    }

    return TRUE;
}

static uint32 libc_abs(int32 value)
{
    if (value > 0)
    {
        return value;
    }
    else
    {
        return (0 - value);
    }
}

uint32 att_cfo_test_tx_begin(mp_test_arg_t *mp_arg)
{
    uint32 ret_val;

    cfo_param_t cfo_param;

    libc_memset(&cfo_param, 0, sizeof(cfo_param));

    cfo_param.ic_type = MP_ICTYPE;
    cfo_param.channel = mp_arg->cfo_channel_low;
    cfo_param.tx_gain_idx = MP_TX_GAIN_IDX;
    cfo_param.tx_gain_val = MP_TX_GAIN_VAL;
    cfo_param.payload = PAYLOADTYPE_SET;
    cfo_param.pkt_type = PKTTYPE_SET;
    cfo_param.tx_dac = MP_TX_DAC;
    cfo_param.whitening_cv = WHITENCOEFF_SET;
    cfo_param.pkt_header = PKTHEADER_SET;
    cfo_param.hit_target_l = HIT_ADDRESS_SET_L;
    cfo_param.hit_target_h = HIT_ADDRESS_SET_H;
    cfo_param.sut_state = g_SUT_state;
    cfo_param.report_interval = MP_REPORT_RX_INTERVAL;
    cfo_param.skip_report_count = MP_SKIP_PKT_NUM;
    cfo_param.once_report_pkts = MP_ONCE_REPORT_MIN_PKT_NUM;
    cfo_param.report_timeout = MP_REPORT_TIMEOUT;

    if (g_add_cfo_result_flag == TRUE)
    {
        if (g_cfo_return_num < 30)
        {
            //扩大样本点,增加测试的准确度
            g_cfo_return_num += 10;
        }

        g_add_cfo_result_flag = FALSE;
    }

    cfo_param.recv_cfo_count = g_cfo_return_num;
    //cfo_param.sut_download_patch = mp_arg->cfo_force_sut_init;

    libc_memcpy((uint8 *) (STUB_ATT_RW_TEMP_BUFFER + sizeof(stub_ext_cmd_t)), &cfo_param, sizeof(cfo_param));

    ret_val = att_write_data(STUB_CMD_ATT_CFO_TX_BEGIN, sizeof(cfo_param_t), STUB_ATT_RW_TEMP_BUFFER);

    if (g_SUT_state == 1)
    {
        att_write_hci_pkt(PKT_CFO_BEGINE_INDEX);

        //dut_re_trigger_tx();

        g_update_timer_id = sys_set_irq_timer1(dut_re_trigger_tx, 1000);
    }

    if (ret_val == 0)
    {
        att_read_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);
    }

    if (g_SUT_state == 0)
    {
        g_SUT_state = 1;
    }

    //libc_print("--BTTxbegin", 0, 0);

    return TRUE;
}

uint32 att_parse_cfo_result(mp_test_arg_t *mp_arg, cfo_return_arg_t *cfo_return_arg, uint32 result_num,
        int32 *data_buffer)
{
    int32 i;
    int32 cfo_val;
    int32 pwr_val;
    int32 diff_val;
    int32 cfo_avg_val;
    int32 pwr_avg_val;
    int8 div_val;
    int32 max_cfo_val;
    int32 min_cfo_val;
    int32 cfo_val_total;
    int32 ori_cfo_val;
    int32 max_diff_val;
    int32 max_diff_index;
    int32 invalid_data_flag;

    div_val = 0;

    cfo_avg_val = 0;

    pwr_avg_val = 0;

    max_cfo_val = 0;

    min_cfo_val = 0x7fffffff;

    cfo_val_total = 0;

    //先计算总的平均值
    for (i = 0; i < result_num; i++)
    {
        ori_cfo_val = (int32)(data_buffer[i << 1]);

        //-1认为是非法值
        if (ori_cfo_val == -1)
        {
            data_buffer[i << 1] = INVALID_CFO_VAL;
            ori_cfo_val = INVALID_CFO_VAL;
        }

        pwr_val = (int32)(data_buffer[(i << 1) + 1]);

        cfo_val = ori_cfo_val + mp_arg->cfo_upt_offset;

        //print_log("cfo result num[%d]: %d hz, pwr: %d\n", i, cfo_val, pwr_val);

        if (ori_cfo_val != INVALID_CFO_VAL)
        {
            cfo_val_total += cfo_val;
            pwr_avg_val += pwr_val;
            div_val++;
        }
        else
        {
            if (i == 0)
            {
                //cfo值非法，说明测试出现错误
                return FALSE;
            }
            else
            {
                continue;
            }
        }

        if (libc_abs(cfo_val) > max_cfo_val)
        {
            max_cfo_val = libc_abs(cfo_val);
        }

        if (libc_abs(cfo_val) < min_cfo_val)
        {
            min_cfo_val = libc_abs(cfo_val);
        }
    }

    //至少要有5组记录值
    if (div_val < MIN_RESULT_NUM)
    {
        //出现多组cfo异常值，需要扩大采集的样本点
        g_add_cfo_result_flag = TRUE;
        return FALSE;
    }

    cfo_avg_val = (cfo_val_total / div_val);
    pwr_avg_val = (pwr_avg_val / div_val);

    //计算得到的cfo平均值不能为0,必须找到一个合适的值
    if (cfo_avg_val == 0)
    {
        cfo_avg_val = (max_cfo_val + min_cfo_val) / 2;

        if ((cfo_avg_val == 0) && (max_cfo_val != 0))
        {
            cfo_avg_val = max_cfo_val;
        }
    }

    max_diff_val = 0;

    invalid_data_flag = FALSE;

    while (1)
    {
        //判断采样点离散程度,先找出离散程度很大的点
        for (i = 0; i < result_num; i++)
        {
            ori_cfo_val = (int32)(data_buffer[i << 1]);

            cfo_val = ori_cfo_val + mp_arg->cfo_upt_offset;

            if (ori_cfo_val != INVALID_CFO_VAL)
            {

                diff_val = libc_abs(cfo_avg_val - cfo_val);

                if (diff_val > max_diff_val)
                {
                    max_diff_index = i;
                    max_diff_val = diff_val;
                }
            }
        }

        //判断离散程度最大的点是否超过限制，如果超过限制，剔除该点，重复计算下一点
        if (max_diff_val > MAX_CFO_DIFF_VAL)
        {
            //print_log("CFO INVALID[%d]: %d hz\n", max_diff_index, data_buffer[max_diff_index << 1]
            //        + mp_arg->cfo_upt_offset);

            //标记该点为无效点
            data_buffer[max_diff_index << 1] = INVALID_CFO_VAL;

            invalid_data_flag = TRUE;

            max_diff_val = 0;
        }
        else
        {
            break;
        }
    }

    //存在无效点需要重新计算cfo平均值
    if (invalid_data_flag == TRUE)
    {
        cfo_val_total = 0;
        pwr_avg_val = 0;

        div_val = 0;

        for (i = 0; i < result_num; i++)
        {
            ori_cfo_val = (int32)(data_buffer[i << 1]);

            cfo_val = ori_cfo_val + mp_arg->cfo_upt_offset;

            if (ori_cfo_val != INVALID_CFO_VAL)
            {
                cfo_val_total += cfo_val;
                pwr_avg_val += pwr_val;
                div_val++;
            }
            else
            {
                continue;
            }

            if (libc_abs(cfo_val) > max_cfo_val)
            {
                max_cfo_val = libc_abs(cfo_val);
            }

            if (libc_abs(cfo_val) < min_cfo_val)
            {
                min_cfo_val = libc_abs(cfo_val);
            }
        }

        //至少要有5组记录值
        if (div_val < MIN_RESULT_NUM)
        {
            //出现多组cfo异常值，需要扩大采集的样本点
            g_add_cfo_result_flag = TRUE;
            return FALSE;
        }

        cfo_avg_val = (cfo_val_total / div_val);
        pwr_avg_val = (pwr_avg_val / div_val);

        //计算得到的cfo平均值不能为0,必须找到一个合适的值
        if (cfo_avg_val == 0)
        {
            cfo_avg_val = (max_cfo_val + min_cfo_val) / 2;

            if ((cfo_avg_val == 0) && (max_cfo_val != 0))
            {
                cfo_avg_val = max_cfo_val;
            }
        }
    }

    cfo_return_arg->cfo_val = cfo_avg_val;
    cfo_return_arg->pwr_val = pwr_avg_val;

    return TRUE;
}
uint32 att_cfo_test_tx_stop(mp_test_arg_t *mp_arg, cfo_return_arg_t *cfo_return_arg)
{
    uint32 ret_val;
    uint8 *pdata;
    uint32 read_len;
    int32 cfo_temp_buffer[30* 2 ]; //最多30组记录，每组记录8个字节

    ret_val = att_write_data(STUB_CMD_ATT_CFO_TX_STOP, 0, STUB_ATT_RW_TEMP_BUFFER);

    if(g_SUT_state == 1)
    {
        att_write_hci_pkt(PKT_CFO_STOP_INDEX);
    }

    if(ret_val == 0)
    {
        read_len = g_cfo_return_num * 2 * 4;

        ret_val = att_read_data(STUB_CMD_ATT_CFO_TX_STOP, read_len, STUB_ATT_RW_TEMP_BUFFER);

        if((ret_val == 0) && (g_SUT_state == 1))
        {
            pdata = (uint8 *)STUB_ATT_RW_TEMP_BUFFER;

            libc_memcpy(cfo_temp_buffer, &pdata[6], g_cfo_return_num * 8);

            ret_val = att_parse_cfo_result(mp_arg, cfo_return_arg, g_cfo_return_num, cfo_temp_buffer);
        }
    }

    if (g_update_timer_id != -1)
    {
        //libc_print("kill timer", 0, 0);
        sys_del_irq_timer1(g_update_timer_id);
        g_update_timer_id = -1;
    }

    return ret_val;
}

int32 att_get_cfo_val(uint32 index, mp_test_arg_t *mp_arg, cfo_return_arg_t *cfo_return_arg, uint32 test_mode)
{
    uint8 bc_val;
    uint8 tc_val;
    uint16 cap_val;
    int32 ret_val;

    bc_val = (index >> 5);

    switch (bc_val)
    {
        case CAP_INDEX_0:
        bc_val = 0;
        break;

        case CAP_INDEX_2:
        bc_val = 2;
        break;

        case CAP_INDEX_3:
        bc_val = 3;
        break;

        case CAP_INDEX_4:
        bc_val = 6;
        break;

        case CAP_INDEX_5:
        bc_val = 7;
        break;

        default:
        bc_val = 1;
        break;
    }

    tc_val = (index & 0x1f);

    cap_val = ((bc_val << 5) | tc_val);

    cap_val = (cap_val | (cap_val << 8));

    // if(test_mode == 0)
    {
        sys_set_hosc_param(cap_val);
    }
    retry: att_cfo_test_tx_begin(mp_arg);

    ret_val = att_cfo_test_tx_stop(mp_arg, cfo_return_arg);

    /** 判断获取的RSSI是否在正常范围内
    */
    if(mp_arg->pwr_test == 1)
    {
        if((cfo_return_arg->pwr_val < mp_arg->pwr_threshold_low) ||
            (cfo_return_arg->pwr_val > mp_arg->pwr_threshold_high))
        {
            print_log("RSSI is wrong : %d", cfo_return_arg->pwr_val);
    		ret_val = FALSE;
        }
    }
    

    if (ret_val == FALSE)
    {
        if (g_cfo_test_retry_num < CFO_TEST_RETRY_NUM)
        {
            print_log("retry index %d...", index);

            //先stop
            g_SUT_state = 2;

            att_cfo_test_tx_begin(mp_arg);

            att_cfo_test_tx_stop(mp_arg, cfo_return_arg);

            g_SUT_state = 0;

            //强制SUT重新初始化
            att_cfo_test_tx_begin(mp_arg);

            g_cfo_test_retry_num++;

            goto retry;
        }
        else
        {
            //测试失败，重新插拔SUT
            print_log("Please re plug in SUT..\n");
        }
    }
    else
    {
        print_log("bc:%d, tc:%d, cap:0x%x, cfo:%d, pwr:%d\n", bc_val, tc_val, cap_val, cfo_return_arg->cfo_val,
                cfo_return_arg->pwr_val);
        g_cfo_test_retry_num = 0;
        g_add_cfo_result_flag = FALSE;
        g_cfo_return_num = MP_RETURN_RESULT_NUM;
    }

    return ret_val;
}



uint32 read_cfo_index(void)
{
    uint32 index;
    uint32 cap_val;
    uint32 bc_val;
    uint32 tc_val;

    if (base_trim_cap_read(&cap_val, RW_TRIM_CAP_SNOR) != TRIM_CAP_READ_NO_ERROR)
    {
        //print_log("Error: not trim cap!");
        //return -1;
        cap_val = sys_read_hosc_param();
    }

    cap_val &= 0xff;

    bc_val = (cap_val >> 5);

    tc_val = (cap_val & 0x1f);

    if (bc_val >= 4)
    {
        bc_val -= 2;
    }

    index = (bc_val << 5) | tc_val;

    return index;
}

//随着电容值的增大，频偏会逐渐从正偏转向负偏
uint32 att_cfo_test(mp_test_arg_t *mp_arg, cfo_return_arg_t *cfo_return_arg, uint32 test_mode)
{
    int32 cfo_val;
    int32 left;
    int32 right;
    uint32 mid;
    int32 cfo_threshold_low;
    int32 cfo_threshold_high;
    uint32 cfo_right_value_flag;
    int32 min_cfo;
    uint32 min_cfo_index;
    uint32 retry_flag;
    uint32 start_left;
    uint32 start_right;
    uint32 abs_cfo_val;
    uint32 adjust_step_changed;
    int32 ret_val;
    int32 min_positive_cfo;
    uint32 min_positive_cfo_index;
    uint32 adjust_direction; //0:往正方向调 1:往负方向调
    int32 last_cfo_val;

    cfo_threshold_low = mp_arg->cfo_threshold_low * 1000;
    cfo_threshold_high = mp_arg->cfo_threshold_high * 1000;

    left = mp_arg->cfo_index_low;
    right = mp_arg->cfo_index_high;

    start_left = left;

    start_right = right;

    //是否搜到符合阈值的cfo值
    cfo_right_value_flag = FALSE;

    min_cfo = 0x7fffffff;

    min_cfo_index = 0;

    min_positive_cfo = 0x7fffffff;

    min_positive_cfo_index = 0;

    cfo_val = 0;

    retry_flag = 0;

    adjust_step_changed = 0;

    last_cfo_val = 0;

    adjust_direction = 0;

    g_cfo_test_retry_num = 0;

    g_add_cfo_result_flag = FALSE;

    g_cfo_return_num = MP_RETURN_RESULT_NUM;    

    retry: while (left <= right)
    {
        mid = (left + right) / 2;

        if (test_mode == 1)
        {
            if ((mid = read_cfo_index()) == -1)
                goto test_fail;
        }

        print_log("left:%d right:%d index:%d", left, right, mid);

        ret_val = att_get_cfo_val(mid, mp_arg, cfo_return_arg, test_mode);

        //如果测试过程中发现测试数据错误，则返回测试失败
        if (ret_val == FALSE)
        {
            goto test_fail;
        }

        cfo_val = cfo_return_arg->cfo_val;

        if (cfo_val > 0)
        {
            //记录最小的正数值
            if (cfo_val < min_positive_cfo)
            {
                min_positive_cfo = cfo_val;
                min_positive_cfo_index = mid;
            }
        }

        abs_cfo_val = libc_abs(cfo_val);

        if (abs_cfo_val < libc_abs(min_cfo))
        {
            //记录cfo最小的index及cfo的值
            min_cfo = cfo_val;

            min_cfo_index = mid;
        }

        if (test_mode == 1)
        {
            break;
        }

        //保证调整方向的单调性
        if (last_cfo_val != 0)
        {
            //判断调整方向是否和预期相符
            if (adjust_direction == 0)
            {
                //往正方向调，新的值应该大于上一个值
                //如果出现新的值小于上一个值，则重新根据index读取cfo值
                if (cfo_val < last_cfo_val)
                {
                    left = mp_arg->cfo_index_low;
                    right = mp_arg->cfo_index_high;

                    print_log("cfo adjust error, retry");

                    last_cfo_val = 0;

                    continue;
                }
            }
            else
            {
                //往负方向调整，新的值应该小于上一个值
                //如果出现新的值大于上一个值，则重新根据index获取cfo值
                if (cfo_val > last_cfo_val)
                {
                    left = mp_arg->cfo_index_low;
                    right = mp_arg->cfo_index_high;

                    print_log("cfo adjust error, retry");

                    last_cfo_val = 0;

                    continue;
                }
            }
        }

        //频偏正偏
        if (cfo_val > 0)
        {
            left = mid + 1;

            adjust_direction = 1;
        }
        else if (cfo_val < 0)
        {
            right = mid - 1;

            adjust_direction = 0;
        }
        else
        {
            ;//nothing for QAC
        }

        last_cfo_val = cfo_val;

        //异常分支处理
        if ((left < 0) || (right < 0))
        {
            goto test_fail;
        }
    }

    if (test_mode == 0)
    {
        if (min_positive_cfo != min_cfo)
        {
            if (min_positive_cfo >= cfo_threshold_low && min_positive_cfo <= cfo_threshold_high)
            {
                min_cfo = min_positive_cfo;

                min_cfo_index = min_positive_cfo_index;
            }
        }
    }

    //判断是否找到最优值
    if (min_cfo >= cfo_threshold_low && min_cfo <= cfo_threshold_high)
    {
        cfo_right_value_flag = TRUE;
    }

    if (cfo_right_value_flag == FALSE && test_mode == 0)
    {
        //最小的频偏仍然为负偏，需要在最左区间搜索
        if ((min_cfo < 0) && (start_left > CFO_THRESHOLD_LEFT))
        {
            if ((retry_flag & SEARCH_LEFT_FLAG) == 0)
            {
                right = left;
                left = CFO_THRESHOLD_LEFT;
                retry_flag |= SEARCH_LEFT_FLAG;

                goto retry;
            }
        }

        //最小的频偏仍然正偏，需要在最右区间搜索
        if ((min_cfo > 0) && (start_right < CFO_THRESHOLD_RIGHT))
        {
            if ((retry_flag & SEARCH_RIGHT_FLAG) == 0)
            {
                left = right;
                right = CFO_THRESHOLD_RIGHT;
                retry_flag |= SEARCH_RIGHT_FLAG;

                goto retry;
            }
        }
    }

    test_fail:

    //停止测试
    if (g_SUT_state == 1)
    {
        g_SUT_state = 2;

        //print_log("test fail stop\n");

        att_cfo_test_tx_begin(mp_arg);

        att_cfo_test_tx_stop(mp_arg, cfo_return_arg);
    }

    cfo_return_arg->cfo_val = min_cfo;

    cfo_return_arg->cfo_index = min_cfo_index;

    return cfo_right_value_flag;
}

