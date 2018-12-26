#include "btcall_eg_stub_asqt.h"


sv_asqt_ram_16k_t *g_pAsqtRam_16k;

dsp_m4k_share_16k_t *g_p_share_mem_16k;

//sv_prms_t g_dsp_init_parm_16k;//存放dsp初始化参数
extern sv_prms_t g_dsp_init_parm;//存放dsp初始化参数

uint8 g_real_call_mode_16k = 0;

int8 g_stub_task_timer_id_16k = -1;//定时器id

asqt_parm_16k_t g_asqt_parm_16k;

extern asqt_peq_addr_t asqt_peq_addr;


PC_curStatus_e stub_asqt_tools_status_16k(void);

void stub_asqt_send_end_16k(uint8 cmd)
{
    stub_set_data(cmd << 8, g_pAsqtRam_16k->stub_buf, 0);
}

uint32 asqt_write_fifo_16k(Fifo_ctl_t *fifo, uint8 *buf, uint32 len)
{
    uint32 irq_save;
    uint32 ret;
    irq_save = sys_local_irq_save();
    for(ret = 0; ret < len; ret++) {
        if(fifo->data_size == fifo->total_size) { //写满
            sys_irq_print("asqt-full.&", (uint32)fifo, 2);
            break;
        }
        ((uint8 *)fifo->data)[fifo->in_index] = buf[ret];
        fifo->data_size++;
        fifo->in_index++;
        if(fifo->in_index >= fifo->total_size) {
            fifo->in_index  = 0;
        }
    }
    sys_local_irq_restore(irq_save);
    return ret;
}

uint32 asqt_read_fifo_16k(Fifo_ctl_t *fifo, uint8 *buf, uint32 len)
{
    uint32 irq_save;
    uint32 ret;
    irq_save = sys_local_irq_save();
    for(ret = 0; ret < len; ret++) {
        if(fifo->data_size == 0) { //没数据
            break;
        }
        buf[ret] = ((uint8 *)fifo->data)[fifo->out_index];
        fifo->data_size--;
        fifo->out_index++;
        if(fifo->out_index >= fifo->total_size) {
            fifo->out_index  = 0;
        }
    }
    sys_local_irq_restore(irq_save);
    return ret;
}

//bool push_to_fifo_16k(Fifo_ctl_t *fifo ,uint8 index)
//{
//
//    if(fifo->total_size >= (fifo->data_size + 256))
//    {
//        asqt_write_fifo_16k(fifo, (uint8*)get_asqt_frame_addr(index), 256);
//        return TRUE;
//    }
//    return FALSE;
//}

uint32 send_data_to_asrc_16k(Fifo_ctl_t *p_fifo)
{
    static bool asrc_dat_new = FALSE;
    static uint32 asrc_dat = 0;
    uint32 ret = 0;
    uint32 timeout = 0;
    while(ret < 640) { //10m = 80,1point = 2bytes   10ms,8K采样，8K*10/1000=80
        timeout++;

        if(asrc_dat_new == FALSE) {
            if(asqt_read_fifo_16k(p_fifo, (uint8 *)&asrc_dat, 2) == 0) {
                break;
            }
            asrc_dat_new = TRUE;
        }

        if((act_readl(ASRC_IN_IP) & (1 << ASRC_IN_IP_WFIFOEmpty)) != 0) {
            asrc_dat <<= 16;
            act_writel(asrc_dat, ASRC_IN_WFIFO);
            act_writel(asrc_dat, ASRC_IN_WFIFO);
            asrc_dat_new = FALSE;
            timeout = 0;
            ret++;
        } else {
            if(timeout > 1000) {
                break;
            }
        }
    }
    return ret;

}

//uint32 stub_asqt_write_data_16k(uint16 cmd, void* buf, uint16 len)
//{
//    uint8* buf_temp = NULL;
//    uint8  ret_val = 0;
//    uint8  err_cnt = 0;
//
//    buf_temp = (uint8*)buf;
//
//retry:
//
//    ret_val = stub_set_data(cmd, buf_temp, len-STUB_COMMAND_HEAD_SIZE);
//    if (0 != ret_val)
//    {
//        err_cnt++;
//        PRINT_ERR("---stub asqt write data error---");
//        if (err_cnt < 10)
//        {
//            goto retry;
//        }
//    }
//
//    return ret_val;
//}

uint32 stub_asqt_read_data_16k(uint16 cmd, void *buf, uint16 len)
{
    uint8 *buf_temp = NULL;

    buf_temp = buf;

    stub_get_data(cmd, buf_temp, len);

    return 0;
}

uint32 stub_asqt_get_volums_16k(uint32 *stub_buf)//0x0d
{
    stub_get_data(0x0d00, stub_buf, 4);
    return stub_buf[0];
}

uint32 stub_asqt_get_status_16k(uint32 *stub_buf)//0x01
{
    stub_get_data(0x0100, stub_buf, 4);
    return stub_buf[0];
}

int stub_asqt_get_parameter_16k(uint32 *stub_buf, sv_configs_t *parm)//0x02
{
    int ret = -2;
try_again:
    ret = stub_get_data(0x0200, stub_buf, sizeof(sv_configs_t));
    if(ret != -1) {
        if(parm != NULL) {
            libc_memcpy((uint8 *)parm, (uint8 *)stub_buf, sizeof(sv_configs_t));
        }
        g_real_call_mode_16k = ((sv_configs_t *)stub_buf)->analysis_mode;
        if(ret != sizeof(sv_configs_t)) { //兼容以前旧的pc-tool
            ((sv_configs_t *)stub_buf)->hfp_mode = 1; //默认是双工
        }
    } else {
        sys_os_time_dly(50);
        libc_print("asqt-get-parm-err", 0, 0);
        goto try_again;//读参数一般不会出错，如果出错一直死循
    }
    return ret;
}

bool stub_asqt_get_remote_voice_16k(uint32 *stub_buf)//0x03
{
    bool ret = FALSE;

    if((g_asqt_parm_16k.ppv_fifo.data_size + 640) <= g_asqt_parm_16k.ppv_fifo.total_size) {
        if(stub_get_data(0x0300, stub_buf, 640) > -1) {
            asqt_write_fifo_16k(&g_asqt_parm_16k.ppv_fifo, (uint8 *)stub_buf, 640);
            ret = TRUE;
        }
    }
    return ret;
}

//上传3Kbuffer数据
void stub_asqt_send_buffer_16k(uint8 buffer_index, uint32 frame_cnt)
{
    uint16 cmd_cnt = 0;

    cmd_cnt = (0x21 << 8) | (frame_cnt % 0xff);

    stub_set_data(cmd_cnt, (uint8 *)g_asqt_parm_16k.m4k_addr[buffer_index], 1024 * 3);
}

//上传AEC性能参数
void stub_asqt_send_aec_16k(void)
{
    uint8 *pbuf = (uint8 *)&g_pAsqtRam_16k->stub_buf[2];

    libc_memcpy(pbuf, (uint8 *)&g_asqt_parm_16k.AEC_info, sizeof(sv_aec_info_t));

    stub_set_data(0x2200, &g_asqt_parm_16k.AEC_info, sizeof(sv_aec_info_t));
}

void asqt_get_result_and_send_16k(void)
{
    uint32 frame_cnt1, frame_cnt2;

    if(g_p_share_mem_16k->Asqt_ctl[0].ready_flag != 0) {
//      PRINT_DATA(g_asqt_parm_16k.m4k_addr[0], 1024 * 3);
        frame_cnt1 = (vint32)g_p_share_mem_16k->Asqt_ctl[0].frame_cnt;
        PRINT_INFO_INT("frame_cnt1", frame_cnt1);
        stub_asqt_send_buffer_16k(0, frame_cnt1);
        g_p_share_mem_16k->Asqt_ctl[0].ready_flag = 0;
        stub_asqt_send_aec_16k();
    }

    if(g_p_share_mem_16k->Asqt_ctl[1].ready_flag != 0) {
//      PRINT_DATA(g_asqt_parm_16k.m4k_addr[1], 1024 * 3);
        frame_cnt2 = (vint32)g_p_share_mem_16k->Asqt_ctl[1].frame_cnt;
        PRINT_INFO_INT("frame_cnt2", frame_cnt2);
        stub_asqt_send_buffer_16k(1, frame_cnt2);
        g_p_share_mem_16k->Asqt_ctl[1].ready_flag = 0;
        stub_asqt_send_aec_16k();
    }
}

void real_call_mode_deal_16k(PC_curStatus_e pc_tool_status, bool reset_dsp_falg)
{
    static btcall_status_e last_call_status = BTCALL_IDLE;
    static uint8 tool_step = 0;
    btcall_status_e call_sta = g_p_btcall_info->status;

    if(reset_dsp_falg == TRUE) {
        tool_step = 0;
    }
    if(tool_step == 0x05) {
        stub_asqt_send_end(0x07);//兼容旧版本
        tool_step = 0;
    }
    //铃声跟导数据空间冲突，必须在hfp才能导数据
    if(last_call_status != call_sta) {
        if((call_sta != BTCALL_HFP) && (last_call_status == BTCALL_HFP)) {
            stub_asqt_send_end(0x05);
            tool_step = 0x05;
        }
    }
    last_call_status = call_sta;//上一次状态

    if((pc_tool_status == sUserStart) && (call_sta == BTCALL_HFP)) {
        asqt_get_result_and_send_16k();
    }

}

void simu_call_mode_deal_16k(PC_curStatus_e pc_tool_status)
{
    static bool g_mmm_need_load = TRUE;
    if((pc_tool_status == sUserStop) && (g_mmm_need_load == FALSE)) {
        stub_asqt_send_end_16k(0x05);
        btcalleg_free_hfp_mmm();
        g_mmm_need_load = TRUE;
    }

    if(pc_tool_status != sUserStart) {
        return;
    }

    if(g_mmm_need_load == TRUE) {
        deal_load_simu_mmm_16k();
        g_mmm_need_load = FALSE;
    }

    while(stub_asqt_get_remote_voice_16k((uint32 *)g_pAsqtRam_16k->stub_buf) == TRUE) {
        ;//get远端数据
    }

    if(g_asqt_parm_16k.ppv_fifo.data_size > 1) {
//      PRINT_INFO_INT("e1:",g_asqt_parm_16k.ppv_fifo.data_size);
        send_data_to_asrc_16k(&g_asqt_parm_16k.ppv_fifo);
//      PRINT_INFO_INT("e:",g_asqt_parm_16k.ppv_fifo.data_size);
    } else {
        stub_asqt_send_end_16k(0x05);
    }
    asqt_get_result_and_send_16k();

}

void get_asqt_peq_para_from_pc_16k(asqt_peq_status_t *p_peq_status_t)
{
    stub_asqt_read_data_16k(STUB_CMD_ASQT_READ_EQ1_DATAS, asqt_peq_addr.band_peq1_addr, p_peq_status_t->package1_len);

    if(STUB_CMD_ASQT_MAQ_EQ_NUMS > STUB_CMD_ASQT_EQ_NUMS_EVERY_PACKAGE) {
        stub_asqt_read_data_16k(STUB_CMD_ASQT_READ_EQ2_DATAS, asqt_peq_addr.band_peq2_addr, p_peq_status_t->package2_len);
    }

    g_p_share_mem_16k->peq_info.band_peq1   = ADDR_M4K2DSP((uint32)asqt_peq_addr.band_peq1_addr);
    g_p_share_mem_16k->peq_info.band_peq2   = ADDR_M4K2DSP((uint32)asqt_peq_addr.band_peq2_addr);
    g_p_share_mem_16k->peq_info.change_flag = 1;

#if 0
    PRINT_INFO_INT("change_flag", g_p_share_mem->peq_info.change_flag);
    PRINT_DATA(asqt_peq_addr.band_peq1_addr, STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);
    PRINT_DATA(asqt_peq_addr.band_peq2_addr, STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);
#endif
}

void update_asqt_para_16k(void)
{
    asqt_peq_status_t   peq_status;
    stub_asqt_read_data_16k(STUB_CMD_ASQT_READ_PEQ_STATUS, &peq_status, sizeof(asqt_peq_status_t));

    if(TRUE == peq_status.update_flag) {
        get_asqt_peq_para_from_pc_16k(&peq_status);
    }
}

void btcall_eg_stub_16k(PC_curStatus_e *last_pc_tool_status, PC_curStatus_e *pc_tool_status)
{
    bool reset_dsp_falg = FALSE;

    if((*pc_tool_status == sUserUpdate) && (*last_pc_tool_status == sUserStart)) {
        reset_dsp_16k(g_real_call_mode_16k);
        PRINT_INFO("reset dsp");
        reset_dsp_falg = TRUE;
    }
    *last_pc_tool_status = *pc_tool_status;
    if(g_real_call_mode_16k == ASQT_CALL_MODE) {
        real_call_mode_deal_16k(*pc_tool_status, reset_dsp_falg);
    } else {
        simu_call_mode_deal_16k(*pc_tool_status);
    }

    if(1 == btcall_gl_var.dsp_load_flag) {
        update_asqt_para_16k();
    }
}

void __section__(".rcode") task_timer10ms_16k(void)
{
    static  uint16 task_timer_dly = 0;
    static PC_curStatus_e last_pc_tool_status = sNotReady;
    PC_curStatus_e pc_tool_status;
//  bool reset_dsp_falg = FALSE;

    if(task_timer_dly  < 500) { //避免跟tts冲突
        task_timer_dly++;
        if(task_timer_dly == 500) {
            reset_aec_asqt_16k();
        }
        return;
    }

    if(g_stub_8k_16k == 1) {
//      PRINT_INFO("16k");
        pc_tool_status = stub_asqt_tools_status_16k();
        btcall_eg_stub_16k(&last_pc_tool_status, &pc_tool_status);
    } else {
//      PRINT_INFO("8k");
        pc_tool_status = stub_asqt_tools_status();
        btcall_eg_stub_8k(&last_pc_tool_status, &pc_tool_status);
    }
}

PC_curStatus_e __section__(".rcode") stub_asqt_tools_status_16k(void)
{
    static uint32 get_sta_timer_ab = 0;//获取两时间点，读pc-tool状态
    static PC_curStatus_e pc_tool_status = sNotReady;//pc-tool状态
    uint32 sys_time;
    PC_curStatus_e pc_status_new;
    uint32 pbuf[4];
    sys_time = sys_get_ab_timer();
    if(sys_time > (get_sta_timer_ab + 500)) { //500ms读一次
        pc_status_new = (PC_curStatus_e)stub_asqt_get_status_16k(pbuf);
        if((pc_status_new == sUserUpdate) ||
                ((pc_status_new == sUserStart) && (pc_tool_status != pc_status_new))
          ) {
            stub_asqt_get_parameter_16k(g_pAsqtRam_16k->rev_buf, &g_asqt_parm_16k.AsqtCfg);
            g_asqt_parm_16k.asqt_volume = stub_asqt_get_volums_16k(pbuf);
//          g_asqt_parm.asqt_volume = g_asqt_parm_16k.asqt_volume;
        }

        if(pc_status_new == sUserSetParam) {
            stub_asqt_down_param_16k(g_pAsqtRam_16k->rev_buf);
        }
        pc_tool_status = pc_status_new;
        get_sta_timer_ab = sys_time;
    }
    return pc_tool_status;
}
