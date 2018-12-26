#include "btcall_eg_stub_asqt.h"

//extern uint32 stub_asqt_write_data_16k(uint16 cmd, void* buf, uint16 len);
extern int stub_asqt_get_parameter_16k(uint32 *stub_buf, sv_configs_t *parm);

//dsp默认参数
const int dsp_init_set_16k[] = {16, 16000,  1, 64,  1, 6, 15,  1, 40, 40,  1, 15,  0, -60,  0, 256, 128, 3, 10, 4, 7, 0, 0, 3};

void show_dat_16k(char *s, uint32 *pd, uint32 len)
{
    uint32 i;
    PRINT_INFO_INT("addr=", (uint32)pd);
    for(i = 0; i < len; i++) {
        libc_print(s, pd[i], 2);
        if((i & 0x07) == 0x07) {
            sys_os_time_dly(1);
        }
    }
}

void init_asqt_fifo_16k(Fifo_ctl_t *p_fifo, uint32 size, uint8 *pdata)
{
    p_fifo->in_index = 0;
    p_fifo->out_index = 0;
    p_fifo->data_size = 0;
    p_fifo->total_size = size;
    p_fifo->data = pdata;
}

void init_m4k_dsp_ctl_16k(uint8 flag)
{
    uint32 M4k_addr = 0;

    if(flag != 0) {
        M4k_addr = (uint32)g_pAsqtRam_16k->share_buf;

        g_asqt_parm_16k.m4k_addr[0] = M4k_addr;
        g_p_share_mem_16k->Asqt_ctl[0].ready_flag = 0;
        g_p_share_mem_16k->Asqt_ctl[0].frame_cnt = 0;
        g_p_share_mem_16k->Asqt_ctl[0].frame_size = 1024 * 3; //3K
        g_p_share_mem_16k->Asqt_ctl[0].dsp_addr = (uint32 *)ADDR_M4K2DSP(M4k_addr +
                8); //M4k_addr不能大于0x9fc40000; 加8是为了在后面用stub传输是填stub头

        M4k_addr += 1024 * 3 + 8;

        g_asqt_parm_16k.m4k_addr[1] = M4k_addr;
        g_p_share_mem_16k->Asqt_ctl[1].ready_flag = 0;
        g_p_share_mem_16k->Asqt_ctl[1].frame_cnt = 0;
        g_p_share_mem_16k->Asqt_ctl[1].frame_size = 1024 * 3; //3K
        g_p_share_mem_16k->Asqt_ctl[1].dsp_addr = (uint32 *)ADDR_M4K2DSP(M4k_addr + 8); //M4k_addr不能大于0x9fc40000;
    } else {
        g_p_share_mem_16k->Asqt_ctl[0].ready_flag = 0;
        g_p_share_mem_16k->Asqt_ctl[0].frame_cnt = 0;
        g_p_share_mem_16k->Asqt_ctl[0].frame_size = 0;
        g_p_share_mem_16k->Asqt_ctl[0].dsp_addr = NULL;

        g_p_share_mem_16k->Asqt_ctl[1].ready_flag = 0;
        g_p_share_mem_16k->Asqt_ctl[1].frame_cnt = 0;
        g_p_share_mem_16k->Asqt_ctl[1].frame_size = 0;
        g_p_share_mem_16k->Asqt_ctl[1].dsp_addr = NULL;
    }
}

uint16 get_bt_air_len_16k(void)
{
    return g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_attr.air_pkt_len;
}

sv_configs_t *asqt_set_dsp_init_parm_16k(void)
{
    static bool read_sv_prms_flag = FALSE;
    sv_configs_t *asqt_parm = NULL;
    PRINT_INFO_INT("set dsp parm 16k:", 0);
    if(btcall_gl_var.stub_asqt_en != 0) { //ASQT调试模式
        reset_aec_asqt_16k();
        if(g_asqt_parm_16k.AsqtCfg.analysis_mode == ASQT_CALL_MODE) {
            g_dsp_init_parm.air_pkt_len = get_bt_air_len_16k();  //蓝牙空中报文包长度
        } else {
            g_dsp_init_parm.air_pkt_len = 0x3C;//调试模式下的模拟通话模式默认0x3C=60
        }
        show_dat_16k("Dsp:", (uint32 *)&g_dsp_init_parm, sizeof(sv_prms_t) / 4);
        asqt_parm = &g_asqt_parm_16k.AsqtCfg;
    } else {
        if(read_sv_prms_flag != TRUE) {
            read_sv_prms_flag = TRUE;
            //先设成默认的参数，再读config.txt的配置
            libc_memcpy((uint8 *)&g_dsp_init_parm, (uint8 *)dsp_init_set_16k, sizeof(dsp_init_set_16k));
            actaec_init_sv_prms(&g_dsp_init_parm);
            g_dsp_init_parm.pcm_mode = 0;//正常工作状态
            g_dsp_init_parm.air_pkt_len = get_bt_air_len_16k();  //蓝牙空中报文包长度
            g_dsp_init_parm.hfp_connect_mod = 0;
            show_dat_16k("dsp:", (uint32 *)&g_dsp_init_parm, sizeof(sv_prms_t) / 4);
        }

    }
    g_p_share_mem_16k->pAEC_parm = (sv_prms_t *)ADDR_M4K2DSP(&g_dsp_init_parm); //转成dsp直接访问
//  //兼容2015-5-25之前的库
//  //要求init_m4k_dsp_ctl()先运行
//  if(g_p_share_mem->Asqt_ctl[5].dsp_addr == NULL)//因为它的偏移在96
//  {
//      g_p_share_mem->Asqt_ctl[5].dsp_addr = (uint32*)ADDR_M4K2DSP(&g_dsp_init_parm);//转成dsp直接访问
//  }
//  else
//  {
//      libc_memcpy((uint8*)ADDR_DSP2M4K(g_p_share_mem->Asqt_ctl[5].dsp_addr), (uint8*)&g_dsp_init_parm, sizeof(sv_prms_t));
//  }

    return asqt_parm;//return null表示用  config.txt的参数，否则用pc-tool的参数
}

void copy_dsp_aec_parm_16k(sv_prms_t *dsp_parm, sv_configs_t *in_parm)
{
    uint32 tmp_dac_weak;
    libc_memcpy((uint8 *)dsp_parm, (uint8 *)&in_parm->frame_size, (2 + 8 + 2 + 2) * 4);

//  dsp_parm->aec_prms.tail_length = (dsp_parm->aec_prms.tail_length + 8) & (~0x0F);//必须是16倍
//  if(dsp_parm->aec_prms.tail_length == 0)
//  {
//      if(g_app_info_state_all.stub_phy_type == STUB_PHY_USB)
//      {
    dsp_parm->aec_prms.tail_length = 64;
//      }
//      else
//      {
//          //耳机方案
//          dsp_parm->aec_prms.tail_length = 64;
//      }
//  }
    if(in_parm->analysis_mode == 0) {
        dsp_parm->pcm_mode = 1;
    } else {
        dsp_parm->pcm_mode = 0;
    }
    dsp_parm->air_pkt_len = get_bt_air_len_16k();  //蓝牙空中报文包长度

    dsp_parm->speaker_channel = in_parm->speaker_channel;
    if(in_parm->speaker_channel != 3) { //双声道才衰减
        tmp_dac_weak = 0;
    } else {
        tmp_dac_weak = in_parm->dac_weak;
    }
    if((tmp_dac_weak > 0) && (tmp_dac_weak < 19)) {
        dsp_parm->speaker_channel_weak_sel = 2;//左
        dsp_parm->speaker_channel_weak_val = in_parm->dac_weak;
    } else if((tmp_dac_weak > 32) && (tmp_dac_weak < 51)) {
        dsp_parm->speaker_channel_weak_sel = 1;
        dsp_parm->speaker_channel_weak_val = in_parm->dac_weak - 33;
    } else {
        dsp_parm->speaker_channel_weak_sel = 0;
        dsp_parm->speaker_channel_weak_val = 0;
    }

    dsp_parm->hfp_mode = in_parm->hfp_mode;//0是双工1是单工
    if(in_parm->pre_agc_level < 0xFF) {
        dsp_parm->pre_agc_level = in_parm->pre_agc_level;
        dsp_parm->pos_agc_level = in_parm->pos_agc_level;
    } else {
        dsp_parm->pre_agc_level = in_parm->pre_agc_level + 30;
        dsp_parm->pos_agc_level = in_parm->pos_agc_level + 30;
    }
    dsp_parm->agc_pre_en = in_parm->pre_agc_enable;
    dsp_parm->agc_back_en = in_parm->pos_agc_enable;

    dsp_parm->frame_size = 16;//固定
    dsp_parm->sampling_rate = 16000; //固定
}

void reset_dsp_16k(uint8 call_mode)
{
    if(call_mode == ASQT_CALL_MODE) {
        if(g_p_btcall_info->status == BTCALL_HFP) { //正在通话才需要重启dsp
            btcalleg_free_hfp_mmm();
            reset_aec_asqt_16k();
            btcalleg_load_hfp_mmm();
        }
    } else {
        btcalleg_free_hfp_mmm();
        reset_aec_asqt_16k();
        deal_load_simu_mmm_16k();
    }
}

//void stub_asqt_send_end_16k(uint8 cmd)
//{
//    stub_set_data(cmd << 8, g_pAsqtRam->stub_buf, 0);
//}

int stub_asqt_down_param_16k(uint32 *stub_buf)
{
    int ret;
    uint32 i, sum;
    uint8 *p8;
    VM_SAVE_t *pParm = (VM_SAVE_t *)stub_buf;
    sv_configs_t *pc_parms = (sv_configs_t *)(stub_buf + (sizeof(VM_SAVE_t) / 4 + 1));
    ret = stub_get_data(0x0900, (uint32 *)pc_parms, sizeof(sv_configs_t));
    if(ret > -1) {
        copy_dsp_aec_parm_16k(&pParm->dsp_cfg, (sv_configs_t *)stub_buf);
        sys_vm_read(pParm, VM_ASQT_PARM_BASE, 8);
        if(pParm->magic != 0xAA55) {
            pParm->times = 1;
        } else {
            pParm->times++;
        }
        pParm->magic = 0xAA55;
        p8 = (uint8 *)&pParm->dsp_cfg;
        for(i = 0, sum = 0; i < sizeof(sv_prms_t); i++) {
            sum += p8[i];
        }
        pParm->cs = sum;
        sys_vm_write(pParm, VM_ASQT_PARM_BASE, sizeof(VM_SAVE_t));
        PRINT_INFO_INT("btcall-write-vm-times:", pParm->times);
    }
    return ret;
}

int reset_aec_asqt_16k(void)
{
    init_asqt_fifo_16k(&g_asqt_parm_16k.ppv_fifo, sizeof(g_pAsqtRam_16k->ppv_data), (uint8 *)g_pAsqtRam_16k->ppv_data);
    if(stub_asqt_get_parameter_16k(g_pAsqtRam_16k->rev_buf, &g_asqt_parm_16k.AsqtCfg) > -1) { //借用rev_buf
        libc_memcpy((uint8 *)&g_dsp_init_parm, (uint8 *)dsp_init_set_16k, sizeof(dsp_init_set_16k));
        copy_dsp_aec_parm_16k(&g_dsp_init_parm, &g_asqt_parm_16k.AsqtCfg);
    }
    init_m4k_dsp_ctl_16k(1);//
    return 0;
}

void set_asrc_cpu_clk_16k(void)
{
    act_writel(act_readl(PCM_CTL) & (~(3u << 4)), PCM_CTL); //关pcm
    act_writel(act_readl(ASRC_IN_CTL) & (~((1u << 1) | (3u << 11))), ASRC_IN_CTL);
    act_writel(act_readl(ASRC_OUT0_CTL) & (~((1u << 1) | (3u << 11))), ASRC_OUT0_CTL);
    set_asrc_in_wclk_cpu();
    set_asrc_out0_rclk_cpu();
    act_writel(act_readl(ASRC_IN_CTL) | (1u << 1) | (3u << 11), ASRC_IN_CTL);
    act_writel(act_readl(ASRC_OUT0_CTL) | (1u << 1) | (3u << 11), ASRC_OUT0_CTL);
}

void deal_load_simu_mmm_16k(void)
{
    PRINT_INFO("SIMU");
    asqt_set_dsp_init_parm_16k();
//  g_p_bt_stack_cur_info->msbc_code_flag = 1;     //模拟打电话，如果能跑到这儿，肯定 com_get_config_default(BTMANAGER_ENABLE_MSBC) == 1
    btcalleg_load_hfp_mmm();
    btcall_gl_var.g_sco_deal_flag = 0;//避免主线程退出mmm
    set_asrc_cpu_clk_16k();
}

uint32 stub_asqt_write_data_16k(uint16 cmd, void *buf, uint16 len)
{
    uint8 *buf_temp = NULL;
    uint8  ret_val = 0;
    uint8  err_cnt = 0;

    buf_temp = (uint8 *)buf;

retry:

    ret_val = stub_set_data(cmd, buf_temp, len - STUB_COMMAND_HEAD_SIZE);
    if(0 != ret_val) {
        err_cnt++;
        PRINT_ERR("---16Kstub asqt write data error---");
        if(err_cnt < 10) {
            goto retry;
        }
    }

    return ret_val;
}

int8 asqt_init_16k(void)
{
    asqt_interface_config_t config_info;
    uint8 asqt_version[8];
    asqt_version[0] = 'U';
    asqt_version[1] = 'S';
    asqt_version[2] = '2';
    asqt_version[3] = '8';
    asqt_version[4] = '2';
    asqt_version[5] = 'A';
    asqt_version[6] = '1';
    asqt_version[7] = '2';

    libc_memset((uint8 *)&config_info, 0x00, sizeof(asqt_interface_config_t));
    config_info.eq_point_nums = STUB_CMD_ASQT_MAQ_EQ_NUMS;
    config_info.eq_version = 0;
    libc_memcpy(config_info.fw_version, asqt_version, sizeof(asqt_version));
    config_info.reserve[0] = 1;
    stub_asqt_write_data_16k(STUB_CMD_ASQT_WRITE_CONFIG_INFO, &config_info, sizeof(asqt_interface_config_t));

    return 0;
}

void btcall_eg_asqt_deal_16k(uint8 stub_tool_type)
{
    //g_agc_aec_shm_ptr指针在进入ap时已经传递给dsp
    g_p_share_mem_16k = (dsp_m4k_share_16k_t *)g_agc_aec_shm_ptr;
    g_stub_tools_type = stub_tool_type;
    libc_memset((uint8 *)&g_asqt_parm_16k, 0x00, sizeof(g_asqt_parm_16k));
    libc_memset((uint8 *)g_p_share_mem_16k, 0x00, sizeof(dsp_m4k_share_16k_t));

    asqt_peq_addr.band_peq1_addr = sys_malloc_large_data(STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);
    if(NULL == asqt_peq_addr.band_peq1_addr) {
//      PRINT_INFO("asqt peq1 malloc fail");
    }

    asqt_peq_addr.band_peq2_addr = sys_malloc_large_data(STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);
    if(NULL == asqt_peq_addr.band_peq2_addr) {
//      PRINT_INFO("asqt peq2 malloc fail");
    }

    init_m4k_dsp_ctl_16k(0);

    if(stub_tool_type == STUB_PC_TOOL_ASQT_MODE) {
        PRINT_INFO("---start stub asqt 16k---");
        asqt_init_16k();

        btcall_gl_var.stub_asqt_en = 1;
        g_pAsqtRam_16k = (sv_asqt_ram_16k_t *)&g_btcall_asqt_test_buf;
        if(g_stub_task_timer_id_16k == -1) { //定时器只初化一次
            g_stub_task_timer_id_16k = set_app_timer(APP_TIMER_ATTRB_CONTROL, 10, task_timer10ms_16k);
        }
//      PRINT_INFO_INT("asqt start.(0x2A00>)=",sizeof(sv_asqt_ram_t));
    }
}
