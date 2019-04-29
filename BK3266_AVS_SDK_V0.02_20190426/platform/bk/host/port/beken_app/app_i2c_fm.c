#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "msg_pub.h"
#include "bt_mini_sched.h"
#include "app_i2c_fm.h"

#ifdef FM_ENABLE
#ifdef FM_BK1080H
#include "driver_bk1080.h"
#endif


#define FM_SCAN_STOP        0
#define FM_SCAN_PREV        1
#define FM_SCAN_NEXT		2
#define FM_SCAN_ALL         3

uint32 channel_tune_success = 0;

uint8 scan_mode = 0;
uint8 fm_init_flag = 0;
int16 sw_channel_index = 0;
uint16 sw_channel_index_back = 0;

uint8 fmPlay = 1;

//app_env_data_t * fmptr = NULL; 
jtask_h fm_task = 0;
//extern RAM_CODE void Beken_Uart_Rx(void);

//static void BK3252_FmEnMute(void );
//static void BK3252_FmDisMute(void );
extern RAM_CODE t_error BTms_Sched(u_int32 num_interations);

uint16 app_get_fm_station( void )
{
	app_env_handle_t env_h = app_env_get_handle();
	return env_h->env_data.valid_channel_count;
}
uint16 app_get_fm_channel( void )
{
	app_env_handle_t env_h = app_env_get_handle();
	return env_h->env_data.valid_channel_idx;
}

uint8 app_get_fm_vol(void)
{
		return player_vol_bt;
}

static void app_fm_info_save(uint8 write_now_Flag)
{
    app_handle_t app_h = app_get_sys_handler();
    
    if(write_now_Flag)
        app_env_fm_wr_flash(NULL);
    else
        {
        jtask_stop(app_h->app_save_volume_task);
        jtask_schedule(app_h->app_save_volume_task,3000,app_env_fm_wr_flash,NULL);
        }
}

static void app_fm_info_restore(void)
{
    app_env_handle_t env_h = app_env_get_handle();
    //if(fmptr == NULL)
    //	return;
    app_env_fm_restore(NULL);
    if(env_h->env_data.channel_valid_flag !=1)
    {
        env_h->env_data.valid_channel_idx = 0;
        env_h->env_data.valid_channel_count = 0;
    }
    //if(fmptr->fm_vol > AUDIO_VOLUME_MAX)
    //	fmptr->fm_vol = 8;
}
uint8 fm_ic_read_id(void)
{
#ifdef FM_BK1080H
    return BK1080_Read_ID();
#endif
}
void fm_ic_init(void)
{
#ifdef FM_BK1080H
    init_BK1080();
#endif

}
void fm_ic_set_vol(uint8 vol)
{
#ifdef FM_BK1080H
    BK1080_set_vol(vol);
#endif
}
uint8 fm_set_fre(uint16 Frequency)
{
#ifdef FM_BK1080H
    return set_fre_BK1080(Frequency);
#endif

}
void FM_IC_PowerDown(void)
{
#ifdef FM_BK1080H
    BK1080_PowerDown();
#endif

}
uint8 set_fre(uint8 mode)
{
    uint8 res = 0;
    if (mode == FM_SCAN_PREV)
    {
        sw_channel_index--;
    }
    else
    {
        sw_channel_index++;
    }

    if (sw_channel_index >(MAX_CHANNEL_COUNT) )//注意范围是 875-1081    1081为全自动搜台结束判断
    sw_channel_index = 0;

    if (sw_channel_index < 0)
    sw_channel_index = MAX_CHANNEL_COUNT;//注意范围是 875-1081

    FM_PRT("_________fre = %d\r\n",sw_channel_index+875);

    res = fm_set_fre(sw_channel_index+875 );
    return res;
}

//static 
    void BK3252_FmEnMute(void )
{	
#ifdef FM_BK1080H
    BK1080_mute(1);
#endif

}

//static 
    void BK3252_FmDisMute(void )
{
#ifdef FM_BK1080H
    BK1080_mute(0);
#endif

}

static uint32 BK3252_FMSeek_Software(uint16 Frequency)
{
#ifdef FM_BK1080H
    return set_fre_BK1080(Frequency);
#endif
}

static int32 fm_software_init(void)
{
//fmptr = (app_env_data_t*)jmalloc(sizeof(app_env_data_t),0);
//if(fmptr == NULL)
//{
//	os_printf("fm malloc fail\r\n");
//	return RET_FM_EXCEPTIONAL;
//}
    if(fm_init_flag)
    {
        FM_PRT("fm_install\r\n");
        return RET_FM_SUCCESS;
    }
    else
    {
        FM_PRT("fm_installing\r\n");
        fm_init_flag = 1;
    }

    app_fm_info_restore();
	
    if(0 != jtask_init(&fm_task, J_TASK_TIMEOUT ))
    {
        FM_PRT("fm_task_init_failed\r\n");
    }
    else
    {
        FM_PRT("fm_task:%p\r\n", fm_task);
    }
    return RET_FM_SUCCESS;
}

static void fm_channel_init(void)
{
    uint32 freq;
    app_env_handle_t env_h = app_env_get_handle();
	
    os_printf("valid_channel_idx:%d, valid_channel_count:%d, %d\r\n",
              env_h->env_data.valid_channel_idx,
              env_h->env_data.valid_channel_count,
              env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx]);

    if((env_h->env_data.valid_channel_idx >= env_h->env_data.valid_channel_count)
       || (env_h->env_data.valid_channel_idx > (MAX_CHANNEL_COUNT - 1)))
    {
        FM_PRT("restore_channel_exception\r\n");
        freq = MIN_FRE;
    }
    else
    {
        if(env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx] > (MAX_FRE_DEVIATION-1))//if there is no valid station, use the default station
        {
            FM_PRT("deviation_channel_exception\r\n");
            freq = MIN_FRE;
        }
        else
        {
            freq = MIN_FRE + env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx];
        }
    }
    sw_channel_index = freq - MIN_FRE;

    if(BK3252_FMSeek_Software(freq))
    {
        os_printf("fm_frequency_seek_ok:%d\r\n", freq);
    }
    else
    {
        os_printf("fm_frequency_seek_fail:%d\r\n", freq);
    }
}

static void fm_hardware_init(void)
{
    if(!fm_ic_read_id())
    {   
        msg_put(MSG_KEY_MODE);
        return;
    }
    fm_ic_init();
    fm_channel_init();

#ifdef FM_IN_LINEIN
    fm_ic_set_vol(30);
    aud_volume_set(player_vol_bt);
   linein_audio_open();
#else
    linein_audio_close();
    BK3000_Ana_Dac_enable(0);
    fm_ic_set_vol(player_vol_bt<<1);
#endif

    BK3252_FmDisMute();
    Delay(20);
    aud_PAmute_oper(0);
}
static void FM_Close(void)
{
    BK3000_set_clock(CPU_CLK_SEL,CPU_CLK_DIV);
#ifdef FM_IN_LINEIN
    aud_PAmute_oper(1);
    os_delay_ms(5);
    //BK3000_Ana_Line_enable(0);
    linein_audio_close();
#else
    aud_PAmute_oper(1);
    os_delay_ms(5);
    BK3252_FmEnMute();//播提示音 MUTE收音	
#endif
}
static void FM_Open(void)
{	
    BK3000_set_clock(1, CPU_CLK_DIV);
#ifdef FM_IN_LINEIN
    aud_PAmute_oper(1);
    os_delay_ms(5);
#ifndef CONFIG_AUDIO_DAC_ALWAYSOPEN
    BK3000_Ana_Dac_enable(1);
#endif
    //BK3000_Ana_Line_enable(1);
    linein_audio_open();
    aud_volume_set(player_vol_bt);
    if((player_vol_bt==0)||(fmPlay==0))
    {   
        fmPlay = 0;
        aud_PAmute_oper(1);
        BK3252_FmEnMute();
    }
    else
    {   
        BK3252_FmDisMute();
        aud_PAmute_oper(0);
    }
#else
    //没有进LINEIN
    if((player_vol_bt==0)||(fmPlay==0))
    {   
        fmPlay = 0;
        aud_PAmute_oper(1);
        BK3252_FmEnMute();
    }
    else
    {   
        BK3252_FmDisMute();
        aud_PAmute_oper(0);
    }
#endif
}
static void playwav_resume_fm(uint32 fieldID)
{
    if((!app_wave_playing()) && (0 == check_wave_file_correct(fieldID)))
    {
        FM_Close();
        start_wave_and_action(fieldID, FM_Open);
    }
    else  if((!app_wave_playing()) && (1 == check_wave_file_correct(fieldID)))      
    {   
    #ifndef FM_IN_LINEIN      //没有提示音(最小音量 低电)_最小音量的时候MUTE掉_进LINEIN的在设置音量时MUTE了
        if((player_vol_bt==0)||(fmPlay==1))
        {
            fmPlay = 0;
            aud_PAmute_oper(1);
            BK3252_FmEnMute();
        }
    #endif
    }
}


void app_playwav_resumeFM(uint32 fieldID)
{
    playwav_resume_fm(fieldID);
}

static void app_fm_set_vol_up( void )
{
    if(0 == fm_init_flag)
    {
        return;
    }

    player_vol_bt++;

    if(player_vol_bt > AUDIO_VOLUME_MAX)
    {
        player_vol_bt = AUDIO_VOLUME_MAX;
    }
    
    if(!app_wave_playing())
    {   
    #ifdef FM_IN_LINEIN
        aud_volume_set(player_vol_bt);
    #else
        fm_ic_set_vol(player_vol_bt<<1);                                
    #endif  
    }

    if(AUDIO_VOLUME_MAX ==player_vol_bt)
    {
        playwav_resume_fm(APP_WAVE_FILE_ID_VOL_MAX);
    }
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_FM_VOL_INF);
#endif
}

static void app_fm_set_vol_down( void )
{
    if(0 == fm_init_flag)
    {
        return;
    }
    if(player_vol_bt != AUDIO_VOLUME_MIN)
         player_vol_bt--;
    if(!app_wave_playing())
    {   
    #ifdef FM_IN_LINEIN
        aud_volume_set(player_vol_bt);
    #else
        fm_ic_set_vol(player_vol_bt<<1);                                
    #endif  
    }

    if(AUDIO_VOLUME_MIN == player_vol_bt)
    {
        playwav_resume_fm(APP_WAVE_FILE_ID_VOL_MIN);
    }
#if (SYS_VOL_SAVE == 1)  
    app_save_volume(ENV_FM_VOL_INF);
#endif
}

static void fm_seek_next_channel(void)
{
    uint16 freq;
    app_env_handle_t env_h = app_env_get_handle();
	
    if(0 == env_h->env_data.valid_channel_count)
		return;
    env_h->env_data.valid_channel_idx ++;
    if(env_h->env_data.valid_channel_idx >= env_h->env_data.valid_channel_count)
    {
       env_h->env_data.valid_channel_idx = 0;
    }

    if(env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx] > MAX_FRE_DEVIATION)
    {
        freq = MIN_FRE;
    }
    else
    {
        freq = MIN_FRE + env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx];
    }

    if(BK3252_FMSeek_Software(freq))
    {
        FM_PRT("find %d UPchannel:%d\r\n",env_h->env_data.valid_channel_idx,freq);
    }
    else
    {
        FM_PRT("failed %d UPchannel:%d\r\n",env_h->env_data.valid_channel_idx,freq);
    }

    sw_channel_index = freq - MIN_FRE;
    app_fm_info_save(1);
    BK3252_FmDisMute();
    aud_PAmute_oper(0);
}

static void fm_seek_previous_channel(void)
{
    uint16 freq;
    app_env_handle_t env_h = app_env_get_handle();
	
    if(0 == env_h->env_data.valid_channel_count)
        return;

    if(env_h->env_data.valid_channel_idx == 0)
    {
        env_h->env_data.valid_channel_idx = env_h->env_data.valid_channel_count - 1;
    }
    else
        env_h->env_data.valid_channel_idx--;
	

    if(env_h->env_data.valid_channel_idx >= 0)
    {
        if(env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx] > MAX_FRE_DEVIATION)
        {
            freq = MIN_FRE;
        }
        else
        {
            freq = MIN_FRE + env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_idx];
        }

        if(BK3252_FMSeek_Software(freq))
        {
            FM_PRT("find %d Dchannel:%d\r\n",env_h->env_data.valid_channel_idx,freq);
        }
        else
        {
            FM_PRT("failed %d Dchannel:%d\r\n",env_h->env_data.valid_channel_idx,freq);
        }
    }
    sw_channel_index = freq - MIN_FRE;
    app_fm_info_save(1);
    BK3252_FmDisMute();
    aud_PAmute_oper(0);
}

static void fm_seek_one_channel_start_over_nice(void *arg)
{
    if(scan_mode)
    {
        BK3252_FmEnMute();
        app_set_led_event_action(LED_EVENT_FM_SCAN);
        msg_put(MSG_FM_SEEK_CONTIUE);
    }
    else
    {
        msg_put(MSG_FM_AUTO_SEEK_OVER);
    }
}

static void fm_seek_channel_tune_continue_sw(void)
{
    uint32 msg_id;
    uint8 res = 0;

    res = set_fre(scan_mode);
    if (scan_mode == FM_SCAN_ALL)
    {
        if(sw_channel_index < MAX_FRE_DEVIATION)//sw_channel_index + 875   < 1081
        {
            if(res)
            {
                msg_id = MSG_FM_TUNE_SUCCESS_CONTINUE;
                channel_tune_success = sw_channel_index;
                FM_PRT("success______________fre = %d\r\n",sw_channel_index+875);
            }
            else
            {
                msg_id =MSG_FM_CHANNEL_TUNE_CONTINUE;
            }
        }
        else
        {
            msg_id = MSG_FM_AUTO_SEEK_OVER;
        }
    }
    else if (scan_mode == FM_SCAN_PREV)
    {
        if(res)
        {
            FM_PRT("success______________fre = %d\r\n",sw_channel_index+875);
            msg_put(MSG_FM_SET_FRE);
            return;
        }

        else if(sw_channel_index == sw_channel_index_back)
        {
            msg_put(MSG_FM_SET_FRE);
            return;
        }
        else
        {
            msg_id =MSG_FM_CHANNEL_TUNE_CONTINUE;
        }
    }
    else if (scan_mode == FM_SCAN_NEXT)
    {
        if(res)
        {
            FM_PRT("success______________fre = %d\r\n",sw_channel_index+875);
            msg_put(MSG_FM_SET_FRE);
            return;
        }
        else if(sw_channel_index == sw_channel_index_back)
        {
            msg_put(MSG_FM_SET_FRE);
            return;
        }
        else
        {
            msg_id =MSG_FM_CHANNEL_TUNE_CONTINUE;
        }

    }
    else
    {
        msg_id = MSG_FM_AUTO_SEEK_OVER;
    }
    msg_put(msg_id);
}

static void fm_seek_channel_success_continue(void)
{
    app_env_handle_t env_h = app_env_get_handle();
	
    BK3252_FmDisMute();
    aud_PAmute_oper(0);
    app_set_led_event_action(LED_EVENT_FM_PAUSE);

    FM_PRT("cnt:%d,readchan:%d\r\n", env_h->env_data.valid_channel_count,channel_tune_success + MIN_FRE);

    env_h->env_data.freq_deviation_array[env_h->env_data.valid_channel_count ++] = channel_tune_success;

    jtask_schedule(fm_task,
                   CHANNEL_INTERVAL_AT_SEEK_SUCCESS,
                   fm_seek_one_channel_start_over_nice,
                   0);
}

static void fm_auto_seek_over(void)
{
    app_env_handle_t env_h = app_env_get_handle();

    FM_PRT("auto_seek_over\r\n");

    app_set_led_event_action(LED_EVENT_FM_PLAY);
    scan_mode = FM_SCAN_STOP;
    if(env_h->env_data.valid_channel_count !=0)
    {
        env_h->env_data.channel_valid_flag = 1;// valid
        env_h->env_data.valid_channel_idx = env_h->env_data.valid_channel_count;			// xugaojing modified 
        msg_put(MSG_FM_SEEK_NEXT_CHANNEL);
        app_fm_info_save(1);
    }
    else
    {
        sw_channel_index = 0;
        env_h->env_data.channel_valid_flag = 0;// invalid
        BK3252_FMSeek_Software(MIN_FRE);
        BK3252_FmDisMute();
        aud_PAmute_oper(0);
    }
}
static void fm_seek_software(void)
{
    FM_PRT("BK3252_FMSeek_Software:%d,sw_channel_index\r\n");
    if(fm_task)
    {
        jtask_stop(fm_task);
    }

    BK3252_FmDisMute();
    aud_PAmute_oper(0);
    scan_mode = FM_SCAN_STOP;
    app_set_led_event_action(LED_EVENT_FM_PLAY);

}

static void fm_seek_one_channel_start(void)
{
    msg_put(MSG_FM_CHANNEL_TUNE_CONTINUE);
}
static void fm_auto_sw_seek_start(void)
{
    uint8 res;
    app_env_handle_t env_h = app_env_get_handle();

    aud_PAmute_oper(1);
    BK3252_FmEnMute();
    	//清空
    memset(&(env_h->env_data.freq_deviation_array[0]), 0xFFFF, sizeof(env_h->env_data.freq_deviation_array));
    env_h->env_data.valid_channel_count = 0;
    sw_channel_index = -1;////第一个频点从875开始

    os_tick_delay(2);
    Delay(20);

    msg_put(MSG_FM_CHANNEL_TUNE_CONTINUE);
}
static void fm_auto_one_seek_start(void)
{
    aud_PAmute_oper(1);
    BK3252_FmEnMute();
    os_tick_delay(2);
    Delay(20);
    sw_channel_index_back = sw_channel_index;
    app_set_led_event_action(LED_EVENT_FM_SCAN);
}

static void app_fm_auto_seek_continue(void)
{
    if(fm_init_flag)
    {
        msg_put(MSG_FM_CHANNEL_SEEK_START);
    }
}

static void app_fm_seek_or_stop_seek(void)
{
    app_env_handle_t env_h = app_env_get_handle();

    if(scan_mode == FM_SCAN_STOP)//没有搜台 就开始搜台
    {
	 scan_mode = FM_SCAN_ALL;
        msg_put(MSG_FM_AUTO_SEEK_START);
        app_set_led_event_action(LED_EVENT_FM_SCAN);
    }
    else if(scan_mode == FM_SCAN_ALL)//全自动搜台
    {
    	 app_set_led_event_action(LED_EVENT_FM_PLAY);
        if(fm_task)
        {
            jtask_stop(fm_task);
        }

        if(env_h->env_data.valid_channel_count)
        {
            env_h->env_data.valid_channel_idx = env_h->env_data.valid_channel_count;      
            msg_put(MSG_FM_SEEK_NEXT_CHANNEL);		//全自动搜台 过程中 按键停止  已搜到电台播放下一频道
        }
        else
        {
        	BK3252_FMSeek_Software(MIN_FRE);//全自动搜台 过程中 按键停止  没有搜到电台播放下875
        	BK3252_FmDisMute();
        	aud_PAmute_oper(0);
        }
        scan_mode = FM_SCAN_STOP;
    }
    else//单步向上向下搜台  还没有搜到电台 停止到当前频点
    {
        app_set_led_event_action(LED_EVENT_FM_PLAY);
        if(fm_task)
        {
            jtask_stop(fm_task);
        }
        msg_put(MSG_FM_SET_FRE);	
    }
}

void fm_install(void *p)
{
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
    {
        app_h->flag_sm1 |= APP_FLAG_LINEIN;
        hf_audio_conf_restore(1);
        aud_mic_open(1);
    }
    else
#endif
    {
    #ifndef CONFIG_BLUETOOTH_COEXIST
    BK3000_set_clock(1, CPU_CLK_DIV);
    os_delay_ms(30);
    #endif
    aud_PAmute_oper(1);
    fm_hardware_init();
    }
}

void fm_uninstall(void)
{
    app_handle_t app_h = app_get_sys_handler();
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
    {
        app_h->flag_sm1 &= ~APP_FLAG_LINEIN;
    }
    else
#endif
    {
    BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
    os_delay_ms(30);
    app_fm_info_save(1);

	//if(fmptr)
	//{
	//	jfree(fmptr);
	//	fmptr = NULL;
	//}
	
    if(fm_init_flag)
    {
        scan_mode = FM_SCAN_STOP;

        BK3252_FmEnMute();
        
    #ifdef FM_IN_LINEIN
        //linein_audio_close();
        app_linein_exit();
    #endif
        FM_IC_PowerDown();
    
       //BK3000_Ana_FM_enable(0);
        
        FM_PRT("fm_uninstall\r\n");

        if(fm_task)
        {
            jtask_uninit(fm_task);
            fm_task = 0;
        }

        fm_init_flag = 0;
    }
        app_h->flag_sm1 &= ~APP_FLAG_LINEIN;
    }
}

#ifdef CONFIG_BLUETOOTH_COEXIST
static uint8 fm_player_status = 0; 
static uint8 flag_coexist_fm_player = 0;
void app_coexist_fm_play_pause(uint8 flag)
{
    app_handle_t app_h = app_get_sys_handler();
    os_printf("app_coexist_fm_play_pause(%d,%d)\r\n",flag,flag_coexist_fm_player);

    //if(app_is_linein_mode())
    {
        if(flag && (flag_coexist_fm_player != 1))
        {
            os_printf("coexist_line in playing...:%d\r\n",fm_player_status);
            app_button_type_set(BUTTON_TYPE_NON_HFP);
            app_wave_file_play_stop();
            sbc_mem_free();
            fmPlay = 1;
            aud_volume_set(player_vol_bt);
            jtask_stop(app_h->app_coexist_task);
            jtask_schedule(app_h->app_coexist_task,2000,fm_install,NULL);
            flag_coexist_fm_player = 1;
        }
        else if((0 == flag) && (flag_coexist_fm_player != 2))
        {
            os_printf("coexist_line in pause...\r\n");
            app_wave_file_play_stop();
            jtask_stop(app_h->app_coexist_task);
            fmPlay = 0;
            app_sleep_func(1);
            BK3252_FmEnMute();
            linein_audio_close();
            aud_volume_mute(1);
            flag_coexist_fm_player = 2;
        }
    }
}
#endif

void fm_msg_handler(void)
{
    int ret;
    uint8 exit_flag = 0;
    MSG_T msg;
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();
    
    app_set_led_event_action(LED_EVENT_FM_PLAY);
#ifdef CONFIG_BLUETOOTH_COEXIST
    flag_coexist_fm_player = 0;
    if(!hci_get_acl_link_count(app_h->unit))
        bt_unit_set_scan_enable(app_h->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);
    else
        bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
#else
		bt_unit_set_scan_enable(app_h->unit,HCI_NO_SCAN_ENABLE);
#endif
    fmPlay = 0;
    if(pre_enter_mode_handle(app_h->sys_work_mode))
        return;

    if(fm_software_init() != RET_FM_SUCCESS)
        return;
    app_restore_volume(ENV_FM_VOL_INF);
    enter_work_mode();
#if defined(CONFIG_BLUETOOTH_SPP)&& defined(SUPPORT_SPP_IN_MP3PLAYER)
    if(app_mp3player_ble_spp_is_enable()) 
    {
        bt_unit_set_scan_enable( app_h->unit, HCI_PAGE_SCAN_ENABLE); 
        //bt_unit_set_scan_enable( app_h->unit, HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE );
    }
#endif
    fmPlay = 1;
#ifdef CONFIG_BLUETOOTH_COEXIST
    flag_coexist_fm_player = 1;
#endif
    while(!exit_flag)
    {
        ret = msg_get(&msg);
        if(MSG_FAILURE_RET == ret)
            msg.id = MSG_NULL;
        else
        {

            switch(msg.id)// 任意按键中断自动搜台
            {
            case MSG_KEY_VOL_DOWN:
            case MSG_KEY_VOL_UP:
            case MSG_KEY_NEXT:
            case MSG_KEY_PREV:
            case MSG_FM_SEEK_PREV_CHANNEL:
            case MSG_FM_SEEK_NEXT_CHANNEL:
            case MSG_KEY_PLAY:
            if(scan_mode!= FM_SCAN_STOP)//  任意按键中断自动搜台
            {
            app_fm_seek_or_stop_seek();
            msg.id = MSG_NULL;
            }

            break;

            case MSG_KEY_POWER_DOWN:
            if(scan_mode != FM_SCAN_STOP)//  关机键不要清消息
            {
            app_fm_seek_or_stop_seek();
            }

            break;
            }

            switch(msg.id)
            {
                case MSG_KEY_PLAY:
                //case MSG_KEY_MODE:
                case MSG_KEY_POWER_DOWN :
                    if(fmPlay==0)
                    {
                        fmPlay=1;
                        app_set_led_event_action(LED_EVENT_FM_PLAY); 
                        BK3252_FmDisMute();
                        aud_PAmute_oper(0);
                    }
                    else
                    {
                        fmPlay=0;
                        app_set_led_event_action(LED_EVENT_FM_PAUSE);
                        aud_PAmute_oper(1);
                        BK3252_FmEnMute();
                    }
                    break;

                case MSG_KEY_VOL_DOWN:
                    app_fm_set_vol_down();
                    if((fmPlay==0)&&(player_vol_bt != AUDIO_VOLUME_MIN))
                    {
                        fmPlay=1;
                        app_set_led_event_action(LED_EVENT_FM_PLAY); 
                        BK3252_FmDisMute();
                        aud_PAmute_oper(0);
                    }
                    break;

                case MSG_KEY_VOL_UP:
                    app_fm_set_vol_up();
                    if((fmPlay==0)&&(player_vol_bt != AUDIO_VOLUME_MIN))
                    {
                        fmPlay=1;
                        app_set_led_event_action(LED_EVENT_FM_PLAY); 
                        BK3252_FmDisMute();
                        aud_PAmute_oper(0);
                    }
                    break;

                //case MSG_KEY_NEXT:
                //	fm_seek_next_channel();
                //	break;

                //case MSG_KEY_PREV:
                //	fm_seek_previous_channel();
                //	break;

                case MSG_KEY_NEXT:
                    fm_auto_one_seek_start();
                    scan_mode = FM_SCAN_NEXT;
                    msg_put(MSG_FM_CHANNEL_TUNE_CONTINUE);
                    break;

                case MSG_KEY_PREV:
                    fm_auto_one_seek_start();
                    scan_mode = FM_SCAN_PREV;
                    msg_put(MSG_FM_CHANNEL_TUNE_CONTINUE);
                    break;

                //case MSG_KEY_PLAY :
                case MSG_KEY_MODE :
                    app_fm_seek_or_stop_seek();
                    break;

                case MSG_FM_SEEK_CONTIUE:
                    app_fm_auto_seek_continue();
                    break;

                case MSG_FM_SEEK_PREV_CHANNEL:
                    fm_seek_previous_channel();
                    break;

                case MSG_FM_SEEK_NEXT_CHANNEL:
                    fm_seek_next_channel();
                    break;

                case MSG_FM_AUTO_SEEK_START:
                    fm_auto_sw_seek_start();
                    break;

                case MSG_FM_CHANNEL_SEEK_START:
                    fm_seek_one_channel_start();
                    break;

                case MSG_FM_CHANNEL_TUNE_CONTINUE:
                    fm_seek_channel_tune_continue_sw();
                    break;

                case MSG_FM_CHANNEL_TUNE_OVER:
                    break;

                case MSG_FM_TUNE_SUCCESS_CONTINUE:
                    fm_seek_channel_success_continue();
                    break;

                case MSG_FM_TUNE_FAILURE_CONTINUE:
                    break;

                case MSG_FM_AUTO_SEEK_OVER:
                    fm_auto_seek_over();
                    break;

                case MSG_FM_SET_FRE:
                    fm_seek_software();
                    break;

                case MSG_HALF_SECOND:
                    break;

                case MSG_INPUT_TIMEOUT:
                    break;
            #ifdef CONFIG_BLUETOOTH_COEXIST
                case MSG_KEY_HFP_ACK:
                    app_button_hfack_action();
                    break;
                case MSG_KEY_HFP_REJECT:
                    app_button_reject_action();
                    break;
                case MSG_KEY_NUM_REDIAL:
                	app_button_redial_last_number();
                	break;
                case MSG_KEY_TRANSFER_TOGGLE:
                    app_button_hf_transfer_toggle();
                    break;
            #endif       

                default:
                    exit_flag = common_msg_handler(&msg);
                    break;
            }
        }
        if(app_wave_playing())
            app_wave_file_play();
    #ifdef CONFIG_BLUETOOTH_COEXIST
    #ifdef CONFIG_APP_AEC
	 if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AEC_ENABLE))
            app_aec_swi();
    #endif
        BTms_Sched(2);//controller的处理
    #else
        BTms_Sched(1);//controller的处理
    #endif
        //	    Beken_Uart_Rx();
        timer_clear_watch_dog();
    #ifdef CONFIG_BLUETOOTH_COEXIST
        BK3000_set_clock(CPU_CLK_SEL, CPU_CLK_DIV);
    #endif
    }
}
#endif

