/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief  btcall头文件
*      Cailizhen          2015-1          creat
*       Wekan        2015-3-30       review
*******************************************************************************/


#include "btcall_eg_stub_asqt.h"


//dsp默认参数
const int dsp_init_set[]={16,8000,  1,64,  1,6,15,  1,40,40,  1,15,  0,-60,  0,256,128,3,10,4,7, 0, 0, 3}; 

void show_dat(char*s,uint32 *pd, uint32 len)
{
    uint32 i;
    PRINT_INFO_INT("addr=", (uint32)pd);
    for(i=0;i<len;i++)
    {    
        libc_print(s, pd[i], 2);
        if((i & 0x07) == 0x07)
        {   
            sys_os_time_dly(1);
        }
    }
}

/******************************************************************************
 * \  par  Description:   初始化fifo数据区
 * \  param[in]     null0
 * \  param[out]   null
 * \  return           null
 * \  note
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
void init_asqt_fifo(Fifo_ctl_t *p_fifo, uint32 size, uint8*pdata)
{
    p_fifo->in_index = 0;
    p_fifo->out_index = 0;
    p_fifo->data_size = 0;
    p_fifo->total_size = size;
    p_fifo->data = pdata;
}

/******************************************************************************/
/*!
 * \par  Description:
 *  初始化m4k和dsp共享的数据块
 * \param[in]   en_ASQT_period 需要上传的时期的数据bit0表示时期1，类推
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
void init_m4k_dsp_ctl(uint8 en_ASQT_period)
{
    uint8 i;
    uint32 M4k_addr = 0;
    if(en_ASQT_period > 0)
    {
        M4k_addr = (uint32)g_pAsqtRam->share_buf;
    }
    for(i = 0; i < MAX_AEC_PERIOD; i++)
    {
        if((en_ASQT_period & (SET_ENABLE << i)) != 0)
        {
             g_asqt_parm.out_frame_cnt[i] = 0;
             g_p_share_mem->Asqt_ctl[i].frame_cnt = 0;
             g_asqt_parm.m4k_addr[i] = M4k_addr;
             g_p_share_mem->Asqt_ctl[i].block_num = ASQT_PERIOD_BLOCK;
             g_p_share_mem->Asqt_ctl[i].frame_size = 128; //uint16
             g_p_share_mem->Asqt_ctl[i].dsp_addr = (uint32*)ADDR_M4K2DSP(M4k_addr);//M4k_addr不能大于0x9fc40000;
             g_p_share_mem->Asqt_ctl[i].period_flag = i + 1;//需要该时期的ASQT数据
             M4k_addr += 256 * ASQT_PERIOD_BLOCK;
        }
        else
        {
            g_p_share_mem->Asqt_ctl[i].block_num = 0;
            g_p_share_mem->Asqt_ctl[i].frame_size = 0;
            g_p_share_mem->Asqt_ctl[i].period_flag = 0;
            g_p_share_mem->Asqt_ctl[i].dsp_addr = NULL;
        }
    } 
}



uint16 get_bt_air_len(void)
{
    return g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_attr.air_pkt_len;
}


/******************************************************************************/
/*!
 * \par  Description:设置为dsp默认的解码参数
 * \param[in]    
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 * \   真实打电话，启动mmm时调用
 *******************************************************************************/
sv_configs_t *asqt_set_dsp_init_parm(void)
{
    static bool read_sv_prms_flag = FALSE;
    sv_configs_t *asqt_parm = NULL;
    PRINT_INFO_INT("set dsp parm:", 0);
    if(btcall_gl_var.stub_asqt_en != 0)//ASQT调试模式
    {
        reset_aec_asqt();
        if(g_asqt_parm.AsqtCfg.analysis_mode == ASQT_CALL_MODE)
        {
            g_dsp_init_parm.air_pkt_len = get_bt_air_len();  //蓝牙空中报文包长度
        }
        else
        {
            g_dsp_init_parm.air_pkt_len = 0x3C;//调试模式下的模拟通话模式默认0x3C=60 
        }
        show_dat("Dsp:", (uint32*)&g_dsp_init_parm, sizeof(sv_prms_t)/4);
        asqt_parm = &g_asqt_parm.AsqtCfg;
    }
    else 
    {
        if(read_sv_prms_flag != TRUE)
        {
            read_sv_prms_flag = TRUE;
            //先设成默认的参数，再读config.txt的配置
            libc_memcpy((uint8*)&g_dsp_init_parm, (uint8*)dsp_init_set, sizeof(dsp_init_set));
            actaec_init_sv_prms(&g_dsp_init_parm);
            g_dsp_init_parm.pcm_mode = 0;//正常工作状态
            g_dsp_init_parm.air_pkt_len = get_bt_air_len();  //蓝牙空中报文包长度
            g_dsp_init_parm.hfp_connect_mod = 0;
            show_dat("dsp:", (uint32*)&g_dsp_init_parm, sizeof(sv_prms_t)/4);
        }
        
    }
    g_p_share_mem->pAEC_parm = (sv_prms_t*)ADDR_M4K2DSP(&g_dsp_init_parm);//转成dsp直接访问
    //兼容2015-5-25之前的库
    //要求init_m4k_dsp_ctl()先运行
    if(g_p_share_mem->Asqt_ctl[5].dsp_addr == NULL)//因为它的偏移在96
    {
        g_p_share_mem->Asqt_ctl[5].dsp_addr = (uint32*)ADDR_M4K2DSP(&g_dsp_init_parm);//转成dsp直接访问
    }
    else
    {
        libc_memcpy((uint8*)ADDR_DSP2M4K(g_p_share_mem->Asqt_ctl[5].dsp_addr), (uint8*)&g_dsp_init_parm, sizeof(sv_prms_t));
    }
    return asqt_parm;//return null表示用  config.txt的参数，否则用pc-tool的参数
}


/******************************************************************************/
/*!
 * \par  Description:复制aec参数，主要是复制从pc端拿到的参数
 * \param[in]      in_parm源参数
 * \param[out]   dsp_parm   dsp参数指针
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1     creat
 *******************************************************************************/
void copy_dsp_aec_parm(sv_prms_t *dsp_parm, sv_configs_t *in_parm)
{
    uint32 tmp_dac_weak;
    libc_memcpy((uint8*)dsp_parm, (uint8*)&in_parm->frame_size, (2 + 8 + 2 + 2)*4);
    
    dsp_parm->aec_prms.tail_length = (dsp_parm->aec_prms.tail_length + 8) & (~0x0F);//必须是16倍
    if(dsp_parm->aec_prms.tail_length == 0)
    {
        if(g_app_info_state_all.stub_phy_type == STUB_PHY_USB)
        {
            dsp_parm->aec_prms.tail_length = 128;
        }
        else
        {
            //耳机方案
            dsp_parm->aec_prms.tail_length = 64;
        }
    }
    if(in_parm->analysis_mode == 0)
    {
        dsp_parm->pcm_mode = 1;
    }
    else
    { 
        dsp_parm->pcm_mode = 0;   
    }
    dsp_parm->air_pkt_len = get_bt_air_len();  //蓝牙空中报文包长度

    dsp_parm->speaker_channel = in_parm->speaker_channel;
    if(in_parm->speaker_channel != 3)//双声道才衰减
    {
        tmp_dac_weak = 0;
    }
    else
    {
        tmp_dac_weak = in_parm->dac_weak;
    }
    if((tmp_dac_weak > 0) && (tmp_dac_weak < 19))
    {    
        dsp_parm->speaker_channel_weak_sel = 2;//左
        dsp_parm->speaker_channel_weak_val = in_parm->dac_weak;
    }
    else if((tmp_dac_weak > 32) && (tmp_dac_weak < 51))
    {
        dsp_parm->speaker_channel_weak_sel = 1;
        dsp_parm->speaker_channel_weak_val = in_parm->dac_weak - 33;
    }
    else
    {
        dsp_parm->speaker_channel_weak_sel = 0;
        dsp_parm->speaker_channel_weak_val = 0;
    }
    
    dsp_parm->hfp_mode = in_parm->hfp_mode;//0是双工1是单工
    if(in_parm->pre_agc_level < 0xFF)
    {
        dsp_parm->pre_agc_level = in_parm->pre_agc_level;
        dsp_parm->pos_agc_level = in_parm->pos_agc_level;
    }
    else
    {
        dsp_parm->pre_agc_level = in_parm->pre_agc_level + 30;
        dsp_parm->pos_agc_level = in_parm->pos_agc_level + 30; 
    }
    dsp_parm->agc_pre_en = in_parm->pre_agc_enable;
    dsp_parm->agc_back_en = in_parm->pos_agc_enable;
    
    dsp_parm->frame_size = 16;//固定
    dsp_parm->sampling_rate = 8000; //固定
}



/******************************************************************************/
/*!
 * \par  Description:
 *  重启dsp，实现pc参数实时生效
 * \param[in]    call_mode 模拟或真实打电话
 * \param[out]   none
 * \return          none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-8-11      creat
 *******************************************************************************/

void reset_dsp(uint8 call_mode)
{
    if(call_mode == ASQT_CALL_MODE)
    {
        if(g_p_btcall_info->status == BTCALL_HFP)//正在通话才需要重启dsp
        {
            btcalleg_free_hfp_mmm();
            reset_aec_asqt();
            btcalleg_load_hfp_mmm();
        }
    }
    else
    {
        btcalleg_free_hfp_mmm();
        reset_aec_asqt();
        deal_load_simu_mmm();
    }
}


/******************************************************************************/
/*!
 * \par  Description:
 *  通知pc-tool，上传数据结束
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
void stub_asqt_send_end(uint8 cmd)
{ 
    stub_set_data(cmd << 8, g_pAsqtRam->stub_buf, 0);
}




/******************************************************************************/
/*!
 * \par  Description: 小机获取参数，保存到Vram
 * \param[in]    stub_buf缓存空间--至少512
 * \param[out]   none
 * \return       -1失败,0成功
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-7-13      creat
 *******************************************************************************/
int stub_asqt_down_param(uint32 *stub_buf)
{ 
    int ret;
    uint32 i, sum;
    uint8*p8;
    VM_SAVE_t *pParm = (VM_SAVE_t *)stub_buf;
    sv_configs_t *pc_parms = (sv_configs_t *)(stub_buf + (sizeof(VM_SAVE_t)/4 + 1));
    ret = stub_get_data(0x0900, (uint32*)pc_parms, sizeof(sv_configs_t));
    if(ret > -1)
    {
        copy_dsp_aec_parm(&pParm->dsp_cfg, (sv_configs_t*)stub_buf);
        sys_vm_read(pParm, VM_ASQT_PARM_BASE, 8);
        if(pParm->magic != 0xAA55)
        {
            pParm->times = 1;
        }
        else
        {
            pParm->times++;
        }
        pParm->magic = 0xAA55;
        p8 = (uint8*)&pParm->dsp_cfg;
        for(i = 0, sum = 0; i < sizeof(sv_prms_t); i++)
        {
            sum += p8[i];
        }
        pParm->cs = sum;
        sys_vm_write(pParm, VM_ASQT_PARM_BASE, sizeof(VM_SAVE_t));
        PRINT_INFO_INT("btcall-write-vm-times:", pParm->times);
    }
    return ret;
}



/******************************************************************************/
/*!
 * \par  Description:复位aec调试内存值
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
int reset_aec_asqt(void)
{
    init_asqt_fifo(&g_asqt_parm.ppv_fifo, sizeof(g_pAsqtRam->ppv_data), (uint8*)g_pAsqtRam->ppv_data);
    init_asqt_fifo(&g_asqt_parm.mic_fifo, sizeof(g_pAsqtRam->mic_data), (uint8*)g_pAsqtRam->mic_data);
    init_asqt_fifo(&g_asqt_parm.agc_fifo, sizeof(g_pAsqtRam->agc_data), (uint8*)g_pAsqtRam->agc_data);
    init_asqt_fifo(&g_asqt_parm.plc_fifo, sizeof(g_pAsqtRam->plc_data), (uint8*)g_pAsqtRam->plc_data);
    if(stub_asqt_get_parameter(g_pAsqtRam->rev_buf, &g_asqt_parm.AsqtCfg) > -1)//借用rev_buf
    {
        libc_memcpy((uint8*)&g_dsp_init_parm, (uint8*)dsp_init_set, sizeof(dsp_init_set));
        copy_dsp_aec_parm(&g_dsp_init_parm, &g_asqt_parm.AsqtCfg);
    }
    init_m4k_dsp_ctl(0x3F);// 1111111     
    return 0;
}


/******************************************************************************/
/*!
 * \par  Description: 设置asrc-in为cpu读写
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
void set_asrc_cpu_clk(void)
{
    act_writel(act_readl(PCM_CTL) & (~(3u << 4)),PCM_CTL);//关pcm
    act_writel(act_readl(ASRC_IN_CTL) & (~((1u << 1)|(3u<<11))), ASRC_IN_CTL);
    act_writel(act_readl(ASRC_OUT0_CTL) & (~((1u << 1)|(3u<<11))), ASRC_OUT0_CTL);
    set_asrc_in_wclk_cpu();
    set_asrc_out0_rclk_cpu();
    act_writel(act_readl(ASRC_IN_CTL) | (1u << 1)|(3u<<11), ASRC_IN_CTL);
    act_writel(act_readl(ASRC_OUT0_CTL) | (1u << 1)|(3u<<11), ASRC_OUT0_CTL);
}



/******************************************************************************/
/*!
 * \par  Description:加载模拟打电话的中间件
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
void deal_load_simu_mmm(void)
{
    asqt_set_dsp_init_parm();
    btcalleg_load_hfp_mmm();
    btcall_gl_var.g_sco_deal_flag = 0;//避免主线程退出mmm
    set_asrc_cpu_clk();
}

int8 asqt_init(void)
{
	asqt_interface_config_t config_info; 
	uint8 asqt_version[10];
	asqt_version[0] = 'U';
	asqt_version[1] = 'S';
	asqt_version[2] = '2';
	asqt_version[3] = '8';
	asqt_version[4] = '2';
	asqt_version[5] = 'A';
	asqt_version[6] = '1';
	asqt_version[7] = '2';
	asqt_version[8] = '5';
	asqt_version[9] = '\0';
	
	libc_memset((uint8*)&config_info, 0x00, sizeof(asqt_interface_config_t));
	config_info.eq_point_nums = STUB_CMD_ASQT_MAQ_EQ_NUMS;
	config_info.eq_version = 0;
	libc_memcpy(config_info.fw_version, asqt_version, sizeof(asqt_version));
	stub_asqt_write_data(STUB_CMD_ASQT_WRITE_CONFIG_INFO,&config_info,sizeof(asqt_interface_config_t));
	
	return 0;
}

/******************************************************************************
 * \par  Description:   处理asqt功能，如果config.txt配置使能，
                                 则发异步消息给引擎，否则初始化为默认参数
 * \param[in]     null0
 * \param[out]   none
 * \return           none
 g_p_share_mem->Asqt_ctl,  g_p_share_mem->pAEC_parm  ,g_dsp_parm  ,g_pAsqtRam必须在开始前初始化好地址
 *   <author>    <time> 
 *    Wekan   2015-4-01  正常通话不会用到0x16c00的空间
 *******************************************************************************/

void btcall_eg_asqt_deal(uint8 stub_tool_type)
{
    //g_agc_aec_shm_ptr指针在进入ap时已经传递给dsp
    g_p_share_mem = (dsp_m4k_share_t *)g_agc_aec_shm_ptr;
    g_stub_tools_type = stub_tool_type;
    libc_memset((uint8*)&g_asqt_parm, 0x00, sizeof(g_asqt_parm));
    libc_memset((uint8*)g_p_share_mem, 0x00, sizeof(dsp_m4k_share_t));

	asqt_peq_addr.band_peq1_addr = sys_malloc_large_data(STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);
	if (NULL == asqt_peq_addr.band_peq1_addr)
	{
		PRINT_INFO("asqt peq1 malloc fail");
	}
	
	asqt_peq_addr.band_peq2_addr = sys_malloc_large_data(STUB_CMD_ASQT_MAX_BYTES_EVERY_PACKAGE);	
	if (NULL == asqt_peq_addr.band_peq2_addr)
	{
		PRINT_INFO("asqt peq2 malloc fail");
	}
		
    init_m4k_dsp_ctl(0);

    if(stub_tool_type == STUB_PC_TOOL_ASQT_MODE)
    {
		PRINT_INFO("---start stub asqt---");
		asqt_init();
		
		btcall_gl_var.stub_asqt_en = 1;
        g_pAsqtRam = (sv_asqt_ram_t *)&g_btcall_asqt_test_buf;
        if(g_stub_task_timer_id == -1)//定时器只初化一次
        {
            g_stub_task_timer_id = set_app_timer(APP_TIMER_ATTRB_CONTROL, 10, task_timer10ms);
        }
        PRINT_INFO_INT("asqt start.(0x2A00>)=",sizeof(sv_asqt_ram_t));
    }
}   	

/******************************************************************************/
/*!
 * \par  Description: 获取stub调试的类型，asqt为01
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-4-1      creat
 *******************************************************************************/
uint32 get_stub_type(void)
{
    uint32 type;
    stub_get_data(0xFF00, &type, 4);
    return type;
}


