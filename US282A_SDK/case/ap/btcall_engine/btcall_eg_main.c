/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *        brief   主函数，初始ap和参数
 *      <author>       <time>
 *      Cailizhen          2015-1          creat
 *       Wekan        2015-3-30       review
 *******************************************************************************/

#include  "btcall_engine.h"

/*全局变量定义*/
//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     2

app_timer_t btcall_timer_vector[APP_TIMER_COUNT];

uint32 g_dsp_m4k_share_mem[128/4];//dsp-m4k共享数据

uint32 g_agc_aec_shm_ptr;

uint8 g_stub_tools_type;

/*main中要填充这两个值，由系统人员分配*/
OS_STK *ptos = (OS_STK *) AP_BACK_LOW_STK_POS;
INT8U prio = AP_BACK_LOW_PRIO;

/******************************************************************************
 * \par  Description:   读取应用VM中的aec参数，该参数在config.txt中配置
 * \param[in]     null0
 * \param[out]   sv_prms 读取的参数存放的指针
 * \return           none
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

void actaec_init_sv_prms(sv_prms_t *sv_prms)
{
    int tmp_value;

    if(actaec_init_sv_prms_from_VM(sv_prms) == TRUE)
    {
        return;
    }
    sv_prms->frame_size = 16; //帧长时间，单位ms，目前仅支持 16ms 帧长
    sv_prms->sampling_rate = 8000; //采样率，单位Hz，目前仅支持 8000Hz 采样率

    // aec prms
    //aec模块开关，1：打开，0：关闭
    sv_prms->aec_prms.aec_enable = (uint8) com_get_config_default(BTCALL_AEC_MODULE_ENABLE);
    //回声尾长，单位 ms，可支持32ms，64ms，96ms，128ms
    sv_prms->aec_prms.tail_length = (uint8) com_get_config_default(BTCALL_AEC_TAIL_LENGTH);

    //非线性处理子模块开关，1：打开，0：关闭，为 1 时必须保证 aec_enable 为 1
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_NLP_ENABLE);

    sv_prms->aec_prms.nlp_enable = tmp_value; //默认为1
    //非线性处理程度，整数0~15,越大代表非线性处理越厉害
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_NLP_AGGRESSIVENESS);
    sv_prms->aec_prms.nlp_aggressiveness = tmp_value; //默认为10
    //双讲近端语音保留程度，整数0~15，越大表示双讲是近端语音保留越多，但是可能会带来单讲的回声，需找个折中值。
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_DT_THRESHOLD);
    sv_prms->aec_prms.dt_threshold = tmp_value;//默认为5
    //残留回声抑制子模块开关，1：打开，0：关闭，为 1 时必须保证 aec_enable 为1
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_AES_ENABLE);
    sv_prms->aec_prms.aes_enable = tmp_value;//默认为1
    //回声抑制子模块单讲回声抑制比，单位dB， 0~40
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_ECHO_SUPPRESS);
    sv_prms->aec_prms.echo_suppress = tmp_value;//默认为40
    //回声抑制子模块双讲回声抑制比，单位dB， 0~40
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_ECHO_SUPPRESS_ACTIVE);
    sv_prms->aec_prms.echo_suppress_avtive = tmp_value;//默认为40

    //ns prms
    //降噪开关，1：打开，0：关闭
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_ANS_ENABLE);
    sv_prms->ns_prms.ans_enable = tmp_value;//默认为1
    //噪声抑制比，单位dB， 0~25
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_NOISE_SUPPRESS);
    sv_prms->ns_prms.noise_suppress = tmp_value;//默认为25

    //cng prms
    //舒适噪声开关，1：打开，0：关闭
    sv_prms->cng_prms.cng_enable = (uint8) com_get_config_default(BTCALL_AEC_CNG_ENABLE);
    //舒适噪声水平，单位 dB Fs, 一般应小于-45 dB；
    tmp_value = (int) (uint32) com_get_config_default(BTCALL_AEC_NOISE_FLOOR);
    sv_prms->cng_prms.noise_floor = tmp_value - 5;//实际水平比该值大5db左右，所以这里-5；对应默认值为-45
    sv_prms->speaker_channel = (uint8) com_get_config_default(BTCALL_SPEAKER_CHANNEL_ENABLE);
    sv_prms->speaker_channel_weak_sel = (uint8) com_get_config_default(BTCALL_SPEAKER_CHANNEL_WEAK_SEL);
    sv_prms->speaker_channel_weak_val = (uint8) com_get_config_default(BTCALL_SPEAKER_CHANNEL_WEAK_VAL);
    sv_prms->hfp_mode = (uint8) com_get_config_default(BTCALL_AEC_HALF_FULL_MODE);//读取通话单双配置
    sv_prms->pre_agc_level = (uint8) com_get_config_default(BTCALL_AGC_PRETREAT_LEVEL);
    sv_prms->pos_agc_level = (uint8) com_get_config_default(BTCALL_AGC_POSTREAT_LEVEL);
	
	if((g_p_btcall_info != NULL) && (g_p_btcall_info->status == BTCALL_SIRI))
	{
		PRINT_INFO("SIRI aec param setting");
		sv_prms->aec_prms.aec_enable = 0;
		sv_prms->aec_prms.nlp_enable = 0;
		sv_prms->aec_prms.aes_enable = 0;
	}
}


/******************************************************************************
 * \par  Description:   读取应用VM中的aec参数，通过ASQT写入
 * \param[in]     null0
 * \param[out]   dsp_cfg 读取的参数存放的指针
 * \return          TRUE读取成功
 * \                   FALSE 没有保存参数
 * \note
 * \  <author>       <time>        <opt>
 * \   Wekan       2015-7-13     creat
 *******************************************************************************/
bool actaec_init_sv_prms_from_VM(sv_prms_t *dsp_aec)
{
    uint8 pbuf[16];
    uint32 i,sum;
    uint8 *p8;
    VM_SAVE_t *parm_head = (VM_SAVE_t *)pbuf;
    sys_vm_read(parm_head, VM_ASQT_PARM_BASE, 8);
    if(parm_head->magic == 0xAA55)
    {
        sys_vm_read(dsp_aec, VM_ASQT_PARM_BASE + 8, sizeof(sv_prms_t));
        p8 = (uint8*)dsp_aec;
        for(i = 0, sum = 0; i < sizeof(sv_prms_t); i++)
        {
            sum += p8[i];
        }
        if(sum == parm_head->cs)
        {
            PRINT_INFO_INT("get AEC VM parms.", parm_head->times);
            return TRUE;
        }
        PRINT_INFO_INT("AEC VM params cs err.", parm_head->cs);
        PRINT_INFO_INT("AEC VM params cs=", sum);
    }
    return FALSE;
}



/******************************************************************************
 * \par  Description:   读取应用VM全局变量,初始化全局变量
 * \param[in]     null0
 * \param[out]   sv_prms 读取的参数存放的指针
 * \return           none
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

static void _read_var(void)
{
    btcall_gl_var.ignore_uncall_sco = (uint8) com_get_config_default(BTCALL_IGNORE_UNCALLSCO);

    btcall_gl_var.callin_ring_mode = (uint8) com_get_config_default(BTCALL_CALLIN_RING_MODE);
    btcall_gl_var.callin_ring_space_time = (uint8) com_get_config_default(BTCALL_CALLIN_RING_BUILDIN_SPACE_TIME);
    if (btcall_gl_var.callin_ring_space_time == 0)
    {
        btcall_gl_var.callin_ring_space_time = 1; //至少100ms
    }

    //全局变量非0无效值初始化
    g_callring_data.callring_timer_id = -1;

    g_p_bt_stack_cur_info = (bt_stack_info_t *) sys_shm_mount(SHARE_MEM_ID_BTSTACK_STATUS);
    if (g_p_bt_stack_cur_info == NULL)
    {
        PRINT_ERR("btstack status shm not exist!\n");
        while (1)
        {
            ;//nothing
        }
    }

    g_p_btcall_info = &g_btcall_info[0];

    switch (g_p_bt_stack_cur_info->rmt_dev[g_p_bt_stack_cur_info->hfp_active_id].hfp_status)
    {
        case HFP_STATUS_LINKED:
        g_p_btcall_info->status = BTCALL_STOP;
        break;

        case HFP_STATUS_INCOMING:
        g_p_btcall_info->status = BTCALL_CALLIN;
        break;

        case HFP_STATUS_OUTGOING:
        g_p_btcall_info->status = BTCALL_CALLOUT;
        break;

        case HFP_STATUS_CALLING:
        g_p_btcall_info->status = BTCALL_HFP;
        break;

        case HFP_STATUS_PHONE:
        g_p_btcall_info->status = BTCALL_PHONE;
        break;

		case HFP_STATUS_SIRI:
        g_p_btcall_info->status = BTCALL_SIRI;
        break;
		
        case HFP_STATUS_NONE:
        default:
        g_p_btcall_info->status = BTCALL_IDLE;
        break;
    }

    //创建共享查询，这时必须保证前台来获取状态时是可用的
    g_p_btcall_info = sys_share_query_creat(APP_ID_BTCALLEG, g_p_btcall_info, sizeof(btcall_info_t));
    if (g_p_btcall_info == NULL)
    {
        PRINT_ERR("btcall share query create fail!\n");
        while (1)
        {
            ;//nothing
        }
    }

    //创建共享内存
    g_agc_aec_shm_ptr = (uint32)&g_dsp_m4k_share_mem;

    if (sys_shm_creat(SHARE_MEM_ID_AGC_AEC, g_agc_aec_shm_ptr, sizeof(sv_prms_t)) == -1)
    {
        PRINT_ERR("agc aec shm create fail!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }
}

/******************************************************************************
 * \par  Description:   保存VRAM变量，目前只是接口，未有功能
 * \param[in]      null
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

static void _save_var(void)
{
    //;//nothing
    //销毁共享内存
    //g_agc_aec_shm_ptr = (uint32)&g_dsp_m4k_share_mem;

    if (sys_shm_destroy(SHARE_MEM_ID_AGC_AEC) == -1)
    {
        PRINT_ERR("agc aec shm destroy fail!!");
    }
}

/******************************************************************************
 * \par  Description:   app应用初始化， 包括读取初始化参数，
 *\                             app库，定时器，消息
 * \param[in]      null
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/
static void _app_init(void)
{
    /*读入VM变量*/
    _read_var();

    //初始化applib库，后台AP
    applib_init(APP_ID_BTCALLEG, APP_TYPE_CONSOLE);

    /*初始化软定时器*/
    init_app_timers(btcall_timer_vector, APP_TIMER_COUNT);

    /*初始化 applib 消息模块*/
    applib_message_init();

    change_engine_state(ENGINE_STATE_PAUSE);
}

/******************************************************************************
 * \par  Description:   app应用退出,
 * \param[in]      null
 * \param[out]   null
 * \return           null
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

static void _app_exit(void)
{
	sys_free_large_data(asqt_peq_addr.band_peq1_addr);
	sys_free_large_data(asqt_peq_addr.band_peq2_addr);
	
	/*执行applib库的注销操作*/
    applib_quit();

    change_engine_state(ENGINE_STATE_PAUSE);
    _save_var();
}

/******************************************************************************
 * \par  Description:   应用的入口函数和场景管理器模块
 * \param[in]      参数只为接口统一，没具体意义
 * \param[out]   none
 * \return       需app-manage响应的事件
 * \note
 * \  <author>       <time>        <opt>
 * \  Cailizhen          2015-1          creat
 * \   Wekan       2015-3-30     reviwe
 *******************************************************************************/

int main(int argc, const char *argv[])
{
    app_result_e result = RESULT_NULL;
    /*ap初始化*/
    _app_init();
    com_set_sound_out(SOUND_OUT_START);
    btcall_eg_asqt_deal((uint8) argc);
    result = get_message_loop();
    com_set_sound_out(SOUND_OUT_STOP);

    _app_exit();
    return result;
}

