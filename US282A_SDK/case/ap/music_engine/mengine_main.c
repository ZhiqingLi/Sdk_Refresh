#include "app_mengine.h"
#include "audio_device.h"

//应用自己需要的软定时器个数
#define APP_TIMER_COUNT     3

#ifdef WAVES_ASET_TOOLS
waves_t  g_waves;
#endif
output_channel_e g_output_channel_config;

//globle variable
//保存中间件句柄
void *g_mp_handle = NULL;

//存放文件系统id
int32 g_file_sys_id = -1;

//进入模式
mengine_enter_mode_e g_mengine_enter_mode;

//引擎信息
mengine_info_t g_mengine_info[2];
mengine_info_t *g_mengine_info_p;
//中间件播放文件信息
mmm_mp_file_info_t g_eg_file_info;
//设置信息(保存配置)
mengine_config_t g_eg_config;

//初始化时的盘符
uint8 g_open_disk;

//文件名buffer
uint8 g_file_name[FILE_NAME_LEN];

//中间件状态检测定时器
int8 g_status_timer;

//中间件状态检测标志位
uint8 g_check_status_flag;

//music engine延时参数
uint8 g_delay_time;

//控件返回值
app_result_e g_mengine_result;

//引擎配置变量结构体指针
mengine_config_t *g_eg_cfg_p;

//引擎状态变量结构体指针
mengine_status_t *g_eg_status_p;

//引擎播放信息结构体指针
mengine_playinfo_t *g_eg_playinfo_p;

//eq data
mmm_mp_eq_info_t g_eq_info;
//private_msg_t g_cur_msg;

//for clk adjust
uint32 g_save_freq = 0;

#ifdef MENGINE_ID3_SUPPORT
//存放id3信息
id3_info_t g_id3_info;

//标题缓冲区
uint8 g_title_buf[Id3BufSIZE];

//歌手缓冲区
uint8 g_artist_buf[ArtBufSIZE];

//专辑缓冲区
uint8 g_album_buf[ArtBufSIZE];

#endif
#ifdef DIR_NAME_SUPPORT
uint8 g_dir_name_buf[32];
#endif
//访问列表开关
bool g_list_access_allow = FALSE;

//app_timer_vector：应用软定时器数组指针，指向应用空间的软定时器数组
app_timer_t mengine_app_timer_vector[APP_TIMER_COUNT];

//因TTS播报而暂停播放音乐，TTS播报结束后需要恢复播放音乐
bool g_stop_by_tts_flag;

uint8 g_shuffle_flag;
fsel_mode_e g_repeat_mode;
//是否支持卡
uint8 g_card_support;
//是否支持uhost
uint8 g_uhost_support;
fsel_type_e g_fsel_type;
//MMM_MP_OPEN指令是否发出
bool g_mmm_opened = FALSE;

//for dae adjust freq
dae_config_t *g_dae_cfg_shm;

bool g_music_replay_by_prev_key;

//for clr vram
uint8 g_need_clr_vm = 0;

void __section__(".text.BANK2") _clear_vm_deal(uint8 clr_sel)
{
    libc_memset(&g_eg_cfg_p->location.dirlocation, 0, sizeof(musfile_location_u));
    g_eg_cfg_p->location.dirlocation.disk = g_open_disk;
    libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));

    if ((clr_sel == 0) || (clr_sel == DISK_H))
    {
        sys_vm_write(g_eg_cfg_p, VM_CARD_BKINFO, sizeof(mengine_config_t));

#if (SUPPORT_MUSIC_FOLDER == 1)
        uint8 i;
        //清空所有特殊目录VRAM记忆
        for(i = 0; i < MUSIC_FOLDER_MAX; i++)
        {
            sys_vm_write(g_eg_cfg_p, g_music_folder_vm_addr[i], sizeof(mengine_config_t));
        }
#endif
    }

    if ((clr_sel == 0) || (clr_sel == DISK_U))
    {
        mengine_vram_write(DISK_U);
    }

}

/******************************************************************************/
/*!
 * \par  Description:
 * \void _load_cfg(void)
 * \读取配置信息
 * \param[in]    void  para1
 * \param[out]   none
 * \return       void the result
 * \retval
 * \retval
 * \ingroup      mengine_main.c
 * \note
 */
/*******************************************************************************/
void _load_cfg(void)
{
    uint16 vram_magic;

    g_music_replay_by_prev_key = (bool) com_get_config_default(MUSIC_REPLAY_BY_PREV_KEY);

    if (g_open_disk == DISK_H)
    {
        vram_magic = VRAM_MAGIC(VM_CARD_BKINFO);
        //if ((g_app_info_state.need_update_playlist & CARD_NEED_SCAN) == 0)
        if (g_need_clr_vm == 0)
        {
            sys_vm_read(g_eg_cfg_p, VM_CARD_BKINFO, sizeof(mengine_config_t));
        }
        else
        {
            _clear_vm_deal(g_open_disk);
        }
    }
    else
    {
        vram_magic = VRAM_MAGIC(VM_UHOST_BKINFO);
        //if ((g_app_info_state.need_update_playlist & UHOST_NEED_SCAN) == 0)
        if (g_need_clr_vm == 0)
        {
            sys_vm_read(g_eg_cfg_p, VM_UHOST_BKINFO, sizeof(mengine_config_t));
        }
        else
        {
            _clear_vm_deal(g_open_disk);
        }
    }
    g_need_clr_vm = 0;

    get_bk_info();
    g_fsel_type = FSEL_TYPE_COMMONDIR;

    if (g_open_disk != DISK_SD)
    {
        //恢复断点
        g_eg_cfg_p->location.dirlocation.disk = g_open_disk;
    }

    g_card_support = (uint8) com_get_config_default(SETTING_APP_SUPPORT_CARDPLAY);
    g_uhost_support = (uint8) com_get_config_default(SETTING_APP_SUPPORT_UHOSTPLAY);

    if ((g_eg_cfg_p->magic != vram_magic) && (g_open_disk != DISK_SD))
    {
        g_eg_cfg_p->magic = vram_magic;
        //文件循环模式
        g_repeat_mode = FSEL_MODE_LOOPALL;
        //shuffle flag
        g_shuffle_flag = SHUFFLE_NONE;

        //(记录当前文件的路径信息/列表文件的层级关系)
        libc_memset(&g_eg_cfg_p->location.dirlocation, 0, sizeof(musfile_location_u));
        //续播默认为udisks
        g_eg_cfg_p->location.dirlocation.disk = g_open_disk;
        //断点信息
        libc_memset(&g_eg_cfg_p->bk_infor, 0, sizeof(mmm_mp_bp_info_t));
        mengine_vram_write(g_open_disk);
    }

    if (g_mengine_enter_mode == ENTER_ALARM)
    {
        if (g_open_disk == DISK_SD)
        {
            g_fsel_type = FSEL_TYPE_SDFILE;
        }
        //(记录当前文件的路径信息/列表文件的层级关系)
        libc_memset(&g_eg_cfg_p->location.dirlocation, 0, sizeof(musfile_location_u));
        g_eg_cfg_p->location.dirlocation.disk = (uint8) g_open_disk;
    }
    else
    {
        //检测文件有效性
        //需要主动获取文件信息
        g_eg_playinfo_p->cur_file_switch |= 0x01;
    }

	g_output_channel_config = (uint8)com_get_config_default(OUTPUT_CHANNEL_CONFIG);
}

//保存配置信息
void _save_cfg(void)
{
    /*保存VM变量*/
    //当前播放的是music歌曲
    if ((g_fsel_type < FSEL_TYPE_SDFILE) && (g_mengine_enter_mode != ENTER_RECOD_PLAY) && (g_mengine_enter_mode
            != ENTER_ALARM))
    {
        //只有music播放时才保存vram
        mengine_vram_write(g_eg_cfg_p->location.dirlocation.disk);
    }
}

#ifdef WAVES_ASET_TOOLS
void waves_init(void)
{
    if (STUB_PC_TOOL_WAVES_ASET_MODE == g_app_info_state_all.stub_pc_tools_type)
    {
        g_waves.tuning_status = TUNING;
    }
    else if (STUB_PC_TOOL_UNKOWN == g_app_info_state_all.stub_pc_tools_type)
    {
        g_waves.tuning_status = NO_TUNING;
    }
    g_waves.bin_number = g_app_info_state_all.bin_number;
    g_waves.input_para_enable = (uint8) com_get_config_default(SETTING_APP_SUPPORT_WAVES_INPUT_PARAM); 
}
#endif

//初始化引擎、读取配置信息、初始化文件选择器、中间件
int _app_init(void)
{
    int app_init_ret = 0;
    bool init_fsel_ret;
    char *mmm_name =
    { "mmm_mp.al" };

    //初始化applib库，后台AP
    applib_init(APP_ID_MENGINE, APP_TYPE_CONSOLE);

    //升频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    //读取配置信息 (包括vm信息)
    _load_cfg();

    //初始化 applib 消息模块
    applib_message_init();

    //初始化软定时器
    init_app_timers(mengine_app_timer_vector, APP_TIMER_COUNT);

    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL6, FREQ_NULL);

    //加载中间件驱动
    sys_load_mmm(mmm_name, TRUE);

    //初始化中间件库
    app_init_ret = mmm_mp_cmd(&g_mp_handle, MMM_MP_OPEN, (unsigned int) NULL);

    if (app_init_ret == 0)
    {
        if(1 == g_app_info_state_all.fix_sample_rate_flag)
        {
            mmm_mp_cmd(&g_mp_handle, MMM_MP_FIX_SAMPLE_RATE, NULL);
        }
        g_mmm_opened = TRUE;
        com_set_sound_out(SOUND_OUT_START);

        //for front ap check if eg init ok
        g_eg_status_p->ctl_status = EG_CTL_INITING;
        //update status
        me_update_share();

        sys_os_time_dly(2); //让前台处理一下用户交互

        if (g_fsel_type != FSEL_TYPE_SDFILE)
        {
            //初始化文件获取可播放文件/断点
            init_fsel_ret = mengine_file_init();
            if (init_fsel_ret == FALSE)
            {
                g_eg_playinfo_p->cur_file_switch &= (uint8) 0xfe;
            }
        }
        else
        {
            init_fsel_ret = TRUE;
        }

        g_eg_status_p->ctl_status = EG_CTL_NORMAL;
        //update status
        me_update_share();

        sys_os_time_dly(2); //让前台处理一下用户交互
    }

    g_delay_time = 2;
    //第一次进入立即查询状态，以更新mengine内容
    g_check_status_flag = TRUE;

    //降频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
    g_eg_playinfo_p->cur_time = (uint32)g_eg_cfg_p->bk_infor.bp_time_offset;

#ifdef WAVES_ASET_TOOLS
    waves_init();
#endif   

    return app_init_ret;
}

//退出app的功能函数,保存配置信息
bool _app_deinit(void)
{
    //升频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL10, FREQ_NULL);

    //关闭文件选择器
    fsel_exit();

    //关闭声音设备
    //mmm_mp_cmd(g_mp_handle, MMM_MP_AOUT_CLOSE, (unsigned int) NULL);
    //关闭中间件设备
    if (g_mmm_opened == TRUE)
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_CLOSE, (unsigned int) NULL);
        g_mmm_opened = FALSE;
    }

    com_set_sound_out(SOUND_OUT_STOP);

    //save config 写vram
    _save_cfg();

    adjust_freq_set_level(AP_BACK_HIGH_PRIO, FREQ_NULL, FREQ_NULL);
    //卸载解码驱动
    sys_free_mmm(TRUE);

    //降频
    adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);

    //执行applib库的注销操作
    applib_quit();
    return TRUE;
}

//argc虽然是int型，但只有低8位有用(因为ap_switch_info_t.param是8位的)
//argv是没有用的(也是因为ap_switch_info_t的原因)
/*argc(低8位有效)参数的含义:
***********第0位和第1位表示播放模式*****************
*00  普通文件
*01  录音文件
*10  闹铃
*11  还没用
************第2、3、4位为扩展位*********************

************第5位和第6位表示存储介质****************
*00  还没用
*01  卡
*10  U盘
*11  还没用
************第7位表示断点***************************
*0   表示不需要断点信息
*1   表示需要断点信息
*/
int main(int argc, const char *argv[])
{
    g_mengine_enter_mode = (mengine_enter_mode_e) (argc & 0x1f);

    //保持前后台的盘符一致
    //g_open_disk = (uint8) (argc & 0x60);
    if ((argc & 0x20) == ENTER_MUSIC_DISK_H)
    {
        g_open_disk = DISK_H;
    }
    else if ((argc & 0x40) == ENTER_MUSIC_DISK_U)
    {
        g_open_disk = DISK_U;
    }
    else
    {
        g_open_disk = DISK_SD;
    }

    //当且仅当argc的第7位为1，并且引擎进入的模式不为闹铃时才清除断点信息
    if (((argc & MUSIC_NEED_BK) == 0)&&(g_mengine_enter_mode!=ENTER_ALARM))
    {
        g_need_clr_vm = 1;
    }
    else
    {
        g_need_clr_vm = 0;
    }

    //全局变量指针
    g_eg_cfg_p = &g_eg_config;

    g_mengine_info_p = &g_mengine_info[0]; //应用加载时初始化为0，不用再初始化了

    //创建共享查询，这时必须保证前台来获取状态时是可用的
    g_mengine_info_p = sys_share_query_creat(APP_ID_MENGINE, g_mengine_info_p, sizeof(mengine_info_t));
    if (g_mengine_info_p == NULL)
    {
        PRINT_ERR("music share query create fail!\n");
        while (1)
        {
            ; //QAC
        }
    }

    //get for dae freqency
    //更新DAE配置参数到共享内存
    g_dae_cfg_shm = (dae_config_t *) sys_shm_mount(SHARE_MEM_ID_DAECFG);
    if (g_dae_cfg_shm == NULL)
    {
        PRINT_ERR("dae_cfg shm not exist!!");
        while (1)
        {
            ; //nothing for QAC
        }
    }

    g_eg_status_p = &(g_mengine_info_p->eg_status);
    g_eg_playinfo_p = &(g_mengine_info_p->eg_playinfo);
    //初始化
    _app_init();
    set_esd_restart_tag();
    //循环
    g_mengine_result = mengine_control_block(); //app功能处理

    //退出
    _app_deinit();

    //销毁共享查询
    if (sys_share_query_destroy(APP_ID_MENGINE) == -1)
    {
        PRINT_ERR("music share query destroy fail!\n");
        while (1)
        {
            ; //QAC
        }
    }
    clear_esd_restart_tag();
    return g_mengine_result;
}
