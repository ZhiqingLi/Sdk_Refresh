#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "msg_pub.h"
#include "beken_external.h"
#include "tc_interface.h"
#include "tra_hcit.h"
#include "tc_const.h"
#include "app_debug.h"
#include "bt_at_types.h"
#include "le_config.h"
#include "lp_pdd_main.h"  // add begin by cai.zhong 20190221 for smartvoice

#if defined(CONFIG_TWS)&&defined(JUDGE_PIN_STEREO)
uint8 g_stereroleflag=0;
//uint8 g_stererolematch=0;
#endif
static void work_mode_switch_handle(uint32 mode_before,uint32 mode_now);
extern RAM_CODE t_error BTms_Sched(u_int32 num_interations);

#if (CONFIG_APP_MP3PLAYER == 1)
extern uint8 mp3_need_pause;
#endif
static btaddr_t btaddr_def = {{0x62, 0x32, 0x01, 0x66, 0x32, 0xa9}};

static APP_SYSTEM_T g_sys_app;
#ifdef IPHONE_BAT_DISPLAY
//uint16 bat_now_value = 0;
uint8 iphone_bat_lever=0;
uint8 iphone_bat_lever_bak=0;
uint8 iphone_bat_lever_bak_cnt = 0;
#endif

static int app_button_match_action(void);
static int app_button_volp_action( void );
static int app_button_volm_action( void );
static int app_button_playpause_action( void );
static int app_button_next_action( void );
static int app_button_prev_action( void );
//static int app_button_powerdown( void );
static int app_button_conn_disconn(void);
static int app_button_stop_action( void );

static  int app_button_rewind_action( void );
static  int app_button_forward_action( void );
static  int app_button_vol_mute_action( void );
static  int app_button_clear_memory( void );
static  int app_button_enter_dut_mode( void );
//static  int app_button_lang_change( void );
#ifdef CONFIG_BLUETOOTH_HFP
//static  int app_button_redial_last_number( void );
static  int app_button_micvolp_action( void );
static  int app_button_micvolm_action( void );
static  int app_button_micvol_mute_action( void );
static  int app_button_voice_dial_set( void );
//static  int app_button_hf_transfer_toggle( void );
static  int app_button_twc_hold_accept( void );
static  int app_button_twc_hung_accept( void );
static  int app_button_twc_reject_hold( void );
#ifdef CONFIG_TWS
static int app_button_stereo_action(void);
static int app_button_stereo_master_action(void);
static int app_button_stereo_slave_action(void);
#endif

#ifdef BT_ONE_TO_MULTIPLE
static int app_button_eSCO_A_B_swap(void);
static int app_button_eSCO_A_B_TWC_swap(void);
#endif
//static  int app_button_hf_transfer_toggle( void );
#endif
static int app_button_hold_swtch(void);
static uint8 app_set_led_low_battery_all_in(uint8 flag);

extern boolean is_ag_support_feature(uint8 feature);
extern boolean is_voice_recog_status_on(void);
extern void set_voice_recog_status(boolean status);

CONST static app_led_info_t default_led_info[LED_EVENT_END] =
{
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_AUTO_CONNECTION = 0,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_MATCH_STATE,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_NO_MATCH_NO_CONN_STATE,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_CONN,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_BT_PAUSE,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_BT_PLAY,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_INCOMING_RINGING,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_OUTGOING_RINGING,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_CALL_ESTABLISHMENT,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_LOW_BATTERY,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_CHARGING,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_BATTERY_FULL,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_POWER_ON,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_POWER_OFF,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_LINEIN_PLAY,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_LINEIN_PAUSE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_MUSIC_PLAY,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_MUSIC_PAUSE,
    { 0, 500, 500, 0, 1, -1},  // LED_EVENT_FM_PLAY,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_FM_PAUSE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_FM_SCAN,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_TEST_MODE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_STEREO_CONN_MODE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_STEREO_MATCH_MODE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_STEREO_RECON_MODE,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_RESERVED1,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_RESERVED2,
    { 0, 500, 500, 0, 1, -1},   // LED_EVENT_RESERVED3,
};
#if 0//(CONFIG_APP_MP3PLAYER == 0)
CONST static button_action_func bt_button_handler[BUTTON_BT_END] =
{
    NULL,
    app_button_playpause_action,
    app_button_next_action,
    app_button_prev_action,
    app_button_rewind_action,
    app_button_forward_action,
    app_button_volp_action,
    app_button_volm_action,
    app_button_vol_mute_action,
    app_button_match_action,
    app_button_powerdown,
#ifdef CONFIG_BLUETOOTH_HFP
    app_button_hfack_action,
    app_button_reject_action,
    app_button_redial_last_number,
    app_button_micvolp_action,
    app_button_micvolm_action,
    app_button_micvol_mute_action,
    app_button_voice_dial_set,
    app_button_hf_transfer_toggle,//app_button_hf_transfer_toggle,
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
    app_button_conn_disconn,
    app_button_stop_action,
    app_button_clear_memory,
    app_button_enter_dut_mode,
    app_button_lang_change,
#ifdef CONFIG_BLUETOOTH_HFP
    app_button_twc_hold_accept,
    app_button_twc_hung_accept,
    app_button_twc_reject_hold,
#else
    NULL,
    NULL,
    NULL,
#endif

#ifdef CONFIG_BLUETOOTH_HFP
#ifdef BT_ONE_TO_MULTIPLE
    app_button_eSCO_A_B_TWC_swap,	//BUTTON_BT_TWC_SWTCH_WAY,
    app_button_eSCO_A_B_swap,//BUTTON_BT_2PHONES_SWTCH,
#else
    NULL,
    NULL,
#endif
#endif

#ifdef CONFIG_TWS
    app_button_stereo_action,	//BUTTON_STEREO,
    app_button_stereo_master_action,	//BUTTON_RESERVED1,
    app_button_stereo_slave_action,	//BUTTON_RESERVED2,
#else
	NULL,
    app_button_hold_swtch,	//BUTTON_RESERVED1,
    NULL,	//BUTTON_RESERVED1,
    NULL,	//BUTTON_RESERVED2,
#endif
    NULL,	//BUTTON_RESERVED3,

};
#endif

static void app_clear_led_event_action( int index);
static void app_set_led_mode(int index, uint16 ontime, uint16 offtime, int initial);
static void app_config_led_mode( int index, uint8 repeat, uint8 num_flash, int16 timeout );
void app_bt_reset_policy_iocap( void );
static void app_set_powerdown_flag( void *arg);

__inline app_handle_t app_get_sys_handler(void)
{
    return &g_sys_app;
}

static __inline int app_led_onoff(int onoff)
{
    app_env_handle_t env_h = app_env_get_handle();
    int8 ledon = ((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_LED_REVERSE) >> 9);

    if(onoff)
        return 1 - ledon;
    else
        return ledon;
}

#define LED_ON_FUNC      app_led_onoff(1)
#define LED_OFF_FUNC     app_led_onoff(0)

static void app_set_chip_btaddr(btaddr_t *btaddr )
{
    uint8 buffer[HCI_COMMAND_HEAD_LENGTH+BLUETOOTH_BTADDR_SIZE];
    HCI_PACKET *pkt = (HCI_PACKET *)(&buffer[0]);

    pkt->code       = TRA_HCIT_COMMAND;
    pkt->opcode.ogf = VENDOR_SPECIFIC_DEBUG_OGF;
    pkt->opcode.ocf = 0X1A; // TCI_SET_LOCAL_BD_ADDR&0X3FF;
    pkt->total      = sizeof(btaddr_t);
    memcpy(pkt->param, btaddr->b, sizeof(btaddr_t));

    uart_send_poll(buffer, sizeof(buffer));
}

static void app_button_led_init( void )
{
    int i;
    app_handle_t app_h = app_get_sys_handler();
	
    buttonInit();

    for( i = 0; i < LED_NUM; i++)
    {
        gpio_config( app_h->led_map[i], 1 );
        app_set_led_mode(i, 0, 100, LED_OFF_FUNC);
    }
}

jtask_h app_get_audio_task_handle(void)
{
    return g_sys_app.app_audio_task;
}

// add begin by cai.zhong 20190215 for ble addr
static void change_ble_addr(btaddr_t *bt_addr)
{
	  // LEconfig_device_address 
       LEconfig_device_address[0] = bt_addr->b[0];
       LEconfig_device_address[1] = bt_addr->b[1];
       LEconfig_device_address[2] = bt_addr->b[2];
       LEconfig_device_address[3] = bt_addr->b[3];
       LEconfig_device_address[4] = bt_addr->b[4];
	   LEconfig_device_address[5] = bt_addr->b[5];

      
        if(LEconfig_device_address[0]<0xff)
       {
            LEconfig_device_address[0] +=1;  
       }
		else
		{
		     LEconfig_device_address[0]  =0x00; 
		}
		
		if(LEconfig_device_address[1] <0xff)
		{
		     LEconfig_device_address[1] +=1;  
		}
		else
		{
		     LEconfig_device_address[1]  =0x00; 
		}
		
		 if(LEconfig_device_address[2] <0xff)
		{
		     LEconfig_device_address[2] +=1;  
		}
		else
		{
		     LEconfig_device_address[2]  =0x00; 
		}

        if(LEconfig_device_address[3]<0xff)
       {
            LEconfig_device_address[3] +=1;  
       }
		else
		{
		     LEconfig_device_address[3]  =0x00; 
		}
		
		if(LEconfig_device_address[4] <0xff)
		{
		     LEconfig_device_address[4] +=1;  
		}
		else
		{
		     LEconfig_device_address[4]  =0x00; 
		}
		
		 if(LEconfig_device_address[5] <0xff)
		{
		     LEconfig_device_address[5] +=1;  
		}
		else
		{
		     LEconfig_device_address[5]  =0x00; 
		}
}
// add end by cai.zhong 20190215 for ble addr

void app_init(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();

    memset(app_h, 0, sizeof(APP_SYSTEM_T));

    app_wave_file_play_init();

    app_env_init();

    if( env_h->env_cfg.used == 0x01 )
    {
        memcpy( (char *)&btaddr_def, (char *)&env_h->env_cfg.bt_para.device_addr, 6 );
        memcpy((uint8 *)app_h->led_info, (uint8*)&env_h->env_cfg.led_info
               , sizeof(env_h->env_cfg.led_info));
        app_h->led_map[0] = env_h->env_cfg.led_map[0];
        app_h->led_map[1] = env_h->env_cfg.led_map[1];
        app_h->led_map[2] = env_h->env_cfg.led_map[2];

        change_ble_addr(&btaddr_def); // add by cai.zhong 20190215 for ble addr
    }
    else
    {
        memcpy((char *)&env_h->env_cfg.bt_para.device_addr,(char *)&btaddr_def, 6 );
        memcpy((uint8 *)app_h->led_info, (uint8*)&default_led_info, (sizeof(app_led_info_t)*LED_EVENT_END));
        app_h->led_map[0] = LED0_INDEX;
        app_h->led_map[1] = LED1_INDEX;
        app_h->led_map[2] = LED1_INDEX;
    }
    app_h->last_led_event = LED_EVENT_END;
    app_h->press_thre         = env_h->env_cfg.button_para.press;
    app_h->repeat_thre        = env_h->env_cfg.button_para.repeat;
    app_h->long_thre          = env_h->env_cfg.button_para.longp;
    app_h->vlong_thre         = env_h->env_cfg.button_para.vlongp;
    app_h->double_thre        = env_h->env_cfg.button_para.doublep;
    app_h->low_detect_count   = env_h->env_cfg.system_para.lp_interval;
    app_h->powerdown_count    = env_h->env_cfg.system_para.powerdown_timeout;
    app_h->low_detect_channel = env_h->env_cfg.system_para.lp_channel;
    app_h->volume_store       = 0;
    app_h->mic_volume_store   = env_h->env_cfg.system_para.vol_mic;
    app_h->linein_vol         = env_h->env_cfg.system_para.vol_a2dp;
    app_h->pause_powerdown_count = env_h->env_cfg.system_para.pause_powerdown_timeout;
    env_h->env_cfg.button_para.vlongp = env_h->env_cfg.button_para.vlongp+env_h->env_cfg.button_para.longp; // xugaojing 20180106
     #if (SYS_VOL_SAVE == 0)
    env_h->env_data.bt_vol = env_h->env_cfg.system_para.vol_a2dp;
     env_h->env_data.hfp_vol = env_h->env_cfg.system_para.vol_hfp;
     #endif
    //os_printf("system_flag: %x,%x\r\n",env_h->env_cfg.system_para.system_flag,env_h->env_cfg.used);
    //app_set_chip_btaddr(&btaddr_def);

    app_button_type_set( BUTTON_TYPE_NON_HFP );
    app_button_led_init();

    app_h->sys_work_mode = SYS_WM_BT_MODE;
    //app_h->bt_mode       = BT_DISCONNECTED;

    if( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_AUTO_CONN)
    {
        app_h->flag_sm1 |= APP_FLAG_AUTO_CONNECTION;
    }
}

void app_post_init( void )
{
    app_handle_t app_h = app_get_sys_handler();

    //os_printf("app_post_init\r\n");
#ifdef BT_ONE_TO_MULTIPLE
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    uint8_t idx = 0;
    for(idx = 0; idx<NEED_SNIFF_DEVICE_COUNT; idx++)
        bt_sniff_free_st(idx);
#endif
#endif
    jtask_init( &app_h->app_auto_con_task, J_TASK_TIMEOUT );
    jtask_init( &app_h->app_reset_task, J_TASK_TIMEOUT );
    jtask_init( &app_h->app_match_task, J_TASK_TIMEOUT );
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    jtask_init( &app_h->app_audio_task, J_TASK_TIMEOUT );
#endif
    jtask_init(&app_h->app_a2dp_task,J_TASK_TIMEOUT);
    //jtask_init( &app_h->app_common_task, J_TASK_TIMEOUT );
    jtask_init( &app_h->app_save_volume_task, J_TASK_TIMEOUT );
#ifdef CONFIG_TWS
    jtask_init( &app_h->app_stereo_task, J_TASK_TIMEOUT );
    jtask_init( &app_h->app_sync_task, J_TASK_TIMEOUT );
#endif
#ifdef CONFIG_BLUETOOTH_COEXIST
    jtask_init( &app_h->app_coexist_task, J_TASK_TIMEOUT );
#endif

    app_set_chip_btaddr(&btaddr_def);

    //app_set_led_event_action( LED_EVENT_POWER_ON );
}

void app_reset(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();

    bt_unit_enable( app_h->unit );

    if( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_AUTO_CONN)
        app_h->flag_sm1 |= APP_FLAG_AUTO_CONNECTION;

    app_set_chip_btaddr(&btaddr_def);

	#ifdef BEKEN_DEBUG
    os_printf("app_reset\r\n");
	#endif

    aud_PAmute_oper(1);
  //  app_button_led_init();

    if( env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PA_ENABLE )
        {
        gpio_config( env_h->env_cfg.system_para.pamute_pin, 1);
        if((env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PAMUTE_HIGH) >> 6 )
             gpio_output(env_h->env_cfg.system_para.pamute_pin,1);
        else
            gpio_output(env_h->env_cfg.system_para.pamute_pin,0);
        }
    if( env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_LINEIN_ENA )
    {
        gpio_config( env_h->env_cfg.system_para.linein_pin, 3 );
    }
#if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
    BK3000_set_clock(1, 0);
#endif

}

void transceiver_disable_role_switch(void)
{
    CONST static uint8 buffer[] = {0x01, 0xe0, 0xfc, 0x02, 0x34, 0x00};
    uart_send_poll((uint8 *)&buffer, sizeof(buffer));
    /* Delay(1000); */
}

#if 0
void app_bt_enable_dut_mode( int8 enable)
{
    extern void BK3000_RF_Initial(uint8 dut_mode);
    app_handle_t app_h = app_get_sys_handler();

    if(app_h->unit == NULL)
        return;

    app_sleep_func(0);
    if(enable)
    {
		if(app_h->flag_sm1 & APP_FLAG_DUT_MODE_ENABLE)
			return;

        BK3000_RF_Initial(1); // dut mode

        if(hci_get_acl_link_count(app_h->unit)
			&&( app_h->flag_sm1 & APP_AUDIO_FLAG_SET ))
		{
            app_button_match_action();
			app_h->flag_sm1 |= APP_FLAG_DUT_MODE_ENABLE;
		}
        else
		{
			result_t err;
            err = bt_unit_disable( app_h->unit );
			if(UWE_BUSY!=err)
				app_h->flag_sm1 |= APP_FLAG_DUT_MODE_ENABLE;

		}

    }
    else
    {
        BK3000_wdt_reset();
        #if 0
        {
		#ifdef BEKEN_DEBUG
        os_printf("Exit Dut mode, Reseting device...\r\n");
		#endif
        CONST static char bluecmdDut1[] = { 0x01, 0x05, 0x0c, 0x01, 0x00 };
        BK3000_RF_Initial(0); // not dut mode
        app_h->flag_sm1 |= APP_FLAG_DUT_MODE_ENABLE;
        uart_send_poll((uint8*)bluecmdDut1, sizeof(bluecmdDut1));
        bt_unit_enable(app_h->unit);
        }
        #endif
    }

    return;
}
void app_bt_enable_fcc_mode( int8 mode,uint8 chnl)
{
    u_int32 v;
    app_sleep_func(0);
    app_handle_t app_h = app_get_sys_handler();
    if(app_h->unit == NULL)
        return;
    if(mode > 0)
    {
        app_h->flag_sm1 |= APP_FLAG_FCC_MODE_ENABLE;
        bt_unit_set_scan_enable( app_h->unit, 0);
        bt_unit_disable( app_h->unit );
        SYSirq_Unmask_Interrupt(&v,1 << VIC_CEVA_ISR_INDEX);
        v=BK3000_XVR_REG_0x24;
        v=v&0xFFFFFF80;
        chnl=chnl+2;
        v=v|(chnl);
        BK3000_XVR_REG_0x24=v;
        BK3000_XVR_REG_0x25 = 0;
        os_delay_ms(1);
        switch(mode)
        {
            case 1: //PN9 GFSK
                    BK3000_XVR_REG_0x25 = 0x00003800;
                    break;
            case 2://PN9 EDR2
                    BK3000_XVR_REG_0x25 = 0x00007800;
                    break;
            case 3://PN9 EDR3
                    BK3000_XVR_REG_0x25 = 0x0000B800;
                    break;
            case 4://CW
                    BK3000_XVR_REG_0x25 = 0x00003000;
                    break;
            default:break;
        }
        #if 0
		os_delay_ms(1);
		if(enable > 1) /* PN9 test mode */
			BK3000_XVR_REG_0x25 = (0x1<<11)|(0x1<<12) | (0x1<<13);
		else		   /* single carrior test mode */
			BK3000_XVR_REG_0x25 = (0x1<<12) | (0x1<<13);
        #endif
            os_printf("Enter FCC testmode !!!\r\n");
		/* add other code */
    }
    else
    {
        BK3000_wdt_reset();
    }
}
#endif

#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
void app_bt_write_sniff_link_policy(uint16_t link_handle,uint8 set_link_policy)
{
    uint8_t idx;
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t  env_h = app_env_get_handle();
    hci_write_link_policy_settings_cp  lp_cp;
    hci_exit_sniff_mode_cp exit_cp;
    if((!app_env_check_sniff_mode_Enable())
    #ifdef CONFIG_APP_USB
        ||(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
    #endif
        )
        return;
    
    if( app_h->unit == NULL )
        return;
    idx = bt_sniff_get_index_from_handle(link_handle);
    if(idx >= NEED_SNIFF_DEVICE_COUNT)
    {
        return;
    }
    bt_sniff_set_policy(idx,set_link_policy);
    if(set_link_policy)
    {
        os_printf("sniff enable:%d,%04x\r\n",idx,link_handle);
        lp_cp.con_handle = link_handle;
        lp_cp.settings = HCI_LINK_POLICY_ENABLE_SNIFF_MODE;
        hci_send_cmd( app_h->unit, HCI_CMD_WRITE_LINK_POLICY_SETTINGS, (void *)&lp_cp, sizeof(lp_cp));
    }
    else
    {
        if(!bt_sniff_is_active(idx))
        {
            os_printf("exit sniff:%d,%04x\r\n",idx,link_handle);
            exit_cp.con_handle = link_handle;
            hci_send_cmd( app_h->unit, HCI_CMD_EXIT_SNIFF_MODE, (void *)&exit_cp, sizeof(hci_exit_sniff_mode_cp));
            /* bt_sniff_set_active(idx,1);*/
        }
        /* else */
        {
            os_printf("sniff disable:%d,%04x\r\n",idx,link_handle);
            lp_cp.con_handle = link_handle;
            lp_cp.settings = 0;
            hci_send_cmd( app_h->unit, HCI_CMD_WRITE_LINK_POLICY_SETTINGS, (void *)&lp_cp, sizeof(lp_cp));
        }
    }

}
void app_bt_enter_sniff_mode(uint16_t link_handle,uint8_t enable)
{
    uint8_t idx;
    app_handle_t app_h = app_get_sys_handler();
    hci_sniff_mode_cp sniff_cp;
    hci_exit_sniff_mode_cp exit_cp;
    if( app_h->unit == NULL )
        return;
    idx = bt_sniff_get_index_from_handle(link_handle);
    if(idx >= NEED_SNIFF_DEVICE_COUNT)
    {
        return;
    }
    if(enable)
    {
        os_printf("enter sniff:%d,%04x\r\n",idx,link_handle);
        sniff_cp.con_handle = link_handle;
#if 0 //#ifdef LE_SLEEP_ENABLE
        sniff_cp.max_interval = 0x320; /* 0x320=500ms */
        sniff_cp.min_interval = 0x280; /* 0x200=320ms;0x280=400ms;0x2A0=420ms;0x2D0=450ms */
#else
        sniff_cp.max_interval = 0x280; //0x500;   /* 0x0006 ~ 0x0540 */
        sniff_cp.min_interval = 0x200; //0x400;   /* 0x0006 ~ 0x0540 */
#endif		
        sniff_cp.attempt = 4;
        sniff_cp.timeout = 4;
        hci_send_cmd( app_h->unit, HCI_CMD_SNIFF_MODE, (void *)&sniff_cp, sizeof(hci_sniff_mode_cp));
        /* bt_sniff_set_active(idx,0); */

    }
    else
    {
        os_printf("exit sniff:%d,%04x\r\n",idx,link_handle);
        exit_cp.con_handle = link_handle;
        hci_send_cmd( app_h->unit, HCI_CMD_EXIT_SNIFF_MODE, (void *)&exit_cp, sizeof(hci_exit_sniff_mode_cp));
        /* bt_sniff_set_active(idx,1); */
    }

}
#else
void app_bt_write_sniff_link_policy(uint16_t link_handle,uint8 set_link_policy)
{

}
#endif

void app_sleep_func( int enable)
{
    app_handle_t app_h = app_get_sys_handler();
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    uint8_t idx = 0;
#endif
    //os_printf("app_sleep_func(%d)\r\n",enable);

    CLEAR_SLEEP_TICK;

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if(bt_flag1_is_set(APP_FLAG_LINEIN)
        && bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
        && bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
    return;
#endif

#if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 0)
    if((!bt_flag1_is_set(APP_AUDIO_WORKING_FLAG_SET)))
    {
    #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
        aud_dma_stop();
    #else
        BK3000_AUD_FIFO_CONF &= ~((1 << sft_AUD_DACL_INT_EN)|(1 << sft_AUD_DACR_INT_EN ));  //disable intr
    #endif
        BK3000_Ana_Dac_enable(0);
    }
#endif

    if( enable
		&& (
			bt_flag1_is_set(APP_FLAG_DUT_MODE_ENABLE)
                     || app_wave_playing()
			||(SYS_WM_BT_MODE !=app_h->sys_work_mode)
			)
      )
    {
        return;
    }

#if defined(CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE)
    if(enable && (hfp_has_connection() || avrcp_is_connection() || a2dp_has_connection())
		&&(app_env_check_sniff_mode_Enable())
		&& (!bt_flag1_is_set(APP_FLAG_RECONNCTION|APP_FLAG_AUTO_CONNECTION)) )
    {

	#ifdef CONFIG_TWS
		if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)
			||a2dp_has_music())
			return;
		if(
			(
				(!bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE)||bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
					&&a2dp_has_connection()
					&&avrcp_is_connection()
					&&hfp_has_connection()
                     )
			||
			(
				(bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE)&&!bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
					&&a2dp_has_connection()
					&&avrcp_is_connection()
                    )
                )
        {
//	        app_sniff_debug();
            for(idx=0;idx<NEED_SNIFF_DEVICE_COUNT;idx++)
            {
                if(bt_sniff_is_used(idx))
                {
                    if(!bt_sniff_is_policy(idx))
                    {
                        app_bt_write_sniff_link_policy(bt_sniff_get_handle_from_idx(idx), 1);
                    }
                    else if(bt_sniff_is_active(idx))
                    {
                        app_bt_enter_sniff_mode(bt_sniff_get_handle_from_idx(idx), 1);
                    }
                }
            }
        }
	#endif
	#ifdef BT_ONE_TO_MULTIPLE
        for(idx=0;idx<NEED_SNIFF_DEVICE_COUNT;idx++)
        {
            if(bt_sniff_is_used(idx))
            {
                if(hfp_has_the_sco_conn(bt_sniff_get_rtaddr_from_idx(idx))
                    || a2dp_has_the_music(bt_sniff_get_rtaddr_from_idx(idx)) )
                {
                    continue;
                }
		if(a2dp_has_the_connection(bt_sniff_get_rtaddr_from_idx(idx))
			|| hfp_has_the_connection(bt_sniff_get_rtaddr_from_idx(idx)))
		{
	                if(!bt_sniff_is_policy(idx))
	                {
	                    app_bt_write_sniff_link_policy(bt_sniff_get_handle_from_idx(idx), 1);
	                }
	                else if(bt_sniff_is_active(idx))
	                {
	                    app_bt_enter_sniff_mode(bt_sniff_get_handle_from_idx(idx), 1);
	                }
		}
            }
        }
	#endif
    }
#endif
    if(enable)
    {
        if(app_wave_playing())
        {
            app_wave_file_play_stop();
        }
    }
}

uint8 app_led_is_pairing(void)
{
    app_handle_t app_h = app_get_sys_handler();
    if((app_h->led_event_save == LED_EVENT_MATCH_STATE) || (app_h->led_event_save == LED_EVENT_NO_MATCH_NO_CONN_STATE))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}
extern uint8 saradc_start;
int app_is_not_auto_reconnection(void)
{
    app_handle_t app_h = app_get_sys_handler();
    
    if(!app_env_check_sniff_mode_Enable())    
        return 0;
	//不连接时进SNIFF
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#ifdef CONFIG_APP_USB// USB enable, not support sniff mode.
    app_env_handle_t  env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
        return 0;
#endif

    return !(bt_flag1_is_set(APP_CPU_NOT_HALT_FLAG_SET)
		  ||bt_flag2_is_set(APP_FLAG2_WAKEUP_DLY)||bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)||saradc_start
		  ||get_Charge_state()
                ||app_led_is_pairing()
            #ifdef CONFIG_TWS
                ||bt_flag2_is_set(APP_FLAG2_STEREO_CONNECTTING)
            #endif
		  ||bt_flag1_is_set(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION)
            #ifdef BEKEN_UP_OTA_3266
                ||flag_ota_enable
            #endif
            #if (CONFIG_APP_MP3PLAYER == 1)
            #ifdef CONFIG_LINE_SD_SNIFF
                || (bt_flag2_is_set(APP_FLAG2_LINEIN_SD_PLAYING))
            #else
            #ifdef CONFIG_APP_SDCARD	
                ||(SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
            #endif
                ||(SYS_WM_LINEIN_MODE == app_h->sys_work_mode)
            #ifdef FM_ENABLE
                ||(SYS_WM_FM_MODE == app_h->sys_work_mode)
            #endif
            #endif
            #endif
            );
#else
	return 0;
#endif
}

void app_exit_sniff_mode(void)
{
    if(!app_env_check_sniff_mode_Enable())    
        return;
    
#if defined(CONFIG_LINE_SD_SNIFF)
#ifdef CONFIG_APP_USB// USB enable, not support sniff mode.
    app_env_handle_t  env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
        return ;
    else
#endif
#endif
    {
        Open_26M_Clock();
        sniffmode_wakeup_dly = 1000;
        bt_flag2_operate(APP_FLAG2_WAKEUP_DLY, 1);
    }
}

#ifdef BT_DUALMODE
RAM_CODE uint8 app_ble_sniff_is_enable(void) // 1:support ble enable    0:disable
{
    if(!app_env_check_sniff_mode_Enable())    
        return 0;

#ifdef LE_SLEEP_ENABLE
    return 1;
#else
    return 0;
#endif
}
#endif

extern t_error LC_Sleep(t_slots slots, t_deviceIndex device_index);
void app_linein_sd_set_sniff( void )
{
#if defined(CONFIG_LINE_SD_SNIFF)
    app_handle_t app_h = app_get_sys_handler();

    if(!app_env_check_sniff_mode_Enable())    
        return 0;
#ifdef CONFIG_APP_USB// USB enable, not support sniff mode.
    app_env_handle_t  env_h = app_env_get_handle();
    if(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_USB_ENA)
        return ;
    else
#endif
    {
        if((!bt_flag2_is_set(APP_FLAG2_LINEIN_SD_PLAYING)) 
    		&& ((SYS_WM_LINEIN_MODE == app_h->sys_work_mode)||(SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)))
        {
            LC_Sleep(1500, 0); // 1000*1slot:1000*625us 
        }
    }
#endif
}

int app_is_not_powerdown(void)
{
    return !(bt_flag1_is_set(APP_FLAG_POWERDOWN));
}
uint8 app_is_enable_msbc_resample(void)
{
#if (CONFIG_APP_MSBC_RESAMPLE == 1)
    return 1;
#else
    return 0;
#endif
}
uint8 app_is_enable_adc_vad(void)
{
#if (CONFIG_APP_ADC_VAD == 1)
    return 1;
#else
    return 0;
#endif
}
uint8 app_is_enable_dac_vad(void)
{
#if (CONFIG_APP_DAC_VAD == 1)
    return 1;
#else
    return 0;
#endif
}

#ifdef CONFIG_BLUETOOTH_SPP
extern __INLINE__ unsigned char spp_is_connected(void);
#endif
#ifdef BT_DUALMODE
extern u_int8 LEconnection_LE_Connected(void);
extern void LEadv_Advertise_Disable_Enable(unsigned char enable);
RAM_CODE int app_is_enable_set_LE_Advertise(void)
{// return 1:enable set_LE_Advertise           return 0:disable LE_Advertise
//底层会周期性调用该函数
//如果返回1，底层会调LEadv_Advertise_Disable_Enable(1)打开BLE广播
//如果返回0，底层不会主动开启或关闭BLE广播，完全应用层自己控制
#if 0
    if(1) // disable LE_Advertise
    {
    #ifdef BT_DUALMODE
        LEadv_Advertise_Disable_Enable(0);
    #endif
        return 0;
    }
    else
#endif
    {
        return (//(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION|APP_FLAG_HFP_CONNECTION))&&
                        (!(bt_flag1_is_set(/*APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION|*/APP_FLAG_SCO_CONNECTION)
                     #ifdef CONFIG_BLUETOOTH_SPP
                         ||spp_is_connected()
                     #endif
                    )));
    }
}
#if defined(CONFIG_BLUETOOTH_SPP)
uint8 app_mp3player_ble_spp_is_enable(void) // 1:support spp enable    0:disable
{
#ifdef SUPPORT_SPP_IN_MP3PLAYER
    if(/*app_env_check_SPP_profile_enable()&&*/(!spp_is_connected())&&(!LEconnection_LE_Connected()))
        return 1;
    else
#endif   
        return 0;
}
#endif
#endif

#ifdef OP_TIMEOUT_HANDLE
static void app_button_timeout_action( void *arg)
{
    app_handle_t app_h = app_get_sys_handler();

    if( app_h->flag_sm1 & APP_FLAG_AVCRP_PROCESSING )
    {
        app_h->flag_sm1 &= ~APP_FLAG_AVCRP_PROCESSING;
    }
}
#endif

void app_button_type_set( uint32 type )
{
    app_handle_t app_h = app_get_sys_handler();
    //os_printf("app_button_type_set(0x%x)\r\n",type);
    app_h->button_type = type;
    return;
}

static int app_button_match_action(void)
{
    app_handle_t app_h = app_get_sys_handler();
#ifdef BEKEN_DEBUG
    os_printf("match\r\n");
#endif
    app_sleep_func(0);
    aud_PAmute_oper(1);
#ifdef CONFIG_TWS
	if(!bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
        && bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
    	return 0;
//	bt_unit_write_inquiry_IAC( app_h->unit, 0 );
//	app_bt_inquiry( 0, 0, 1 );
	app_bt_stereo_pairing_exit();
	app_bt_stereo_auto_conn_stop();
	bt_flag2_operate(APP_FLAG2_STEREO_AUTOCONN, 0);
#endif

    if(app_h->flag_sm1 & (APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
    {
        jtask_stop(app_h->app_auto_con_task);
        app_h->reconn_num_flag = 0;
        app_h->flag_sm1 &= ~(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
    }

//    if(app_h->flag_sm1 & APP_MATCH_FLAG_SET)
//    {

#if 0
    #ifdef CONFIG_BLUETOOTH_AVRCP
        avrcp_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_A2DP
        a2dp_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_HFP
        hf_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_HSP
        hs_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_HID
        hid_cmd_disconnect();
    #endif
#endif
	//直接ACL断开，还有待评估20151116
    if(hci_get_acl_link_count(app_h->unit))
    {
        bt_flag2_operate( APP_FLAG2_MATCH_BUTTON, 1);
    #ifdef BT_ONE_TO_MULTIPLE
   	 	bt_all_acl_disconnect(app_h->unit);
    #else
        	bt_unit_acl_disconnect(app_h->unit, (const btaddr_t *)&app_h->remote_btaddr);
    #endif
     }
// }
    else
    {
        if(app_h->unit)
            bt_unit_set_scan_enable(app_h->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);

        app_h->flag_sm1 &= ~(APP_MATCH_FLAG_SET);
    }

    backend_unit_remove_name(); //remove all the remote name on list
    app_set_led_event_action( LED_EVENT_MATCH_STATE);
    app_button_type_set(BUTTON_TYPE_NON_HFP);
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||defined(BT_ONLY_ONE_BT)
    if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_1V2|BT_MODE_DM_1V1|BT_MODE_BLE))
        app_wave_file_play_start(APP_WAVE_FILE_ID_ENTER_PARING);
#endif
    return 0;
}

static int app_button_volp_action(void)
{
    app_handle_t app_h = app_get_sys_handler();
#if 0//def BEKEN_DEBUG
    os_printf("volp\r\n");
#endif
#ifdef CONFIG_BLUETOOTH_HFP
     if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
    {
        hf_cmd_set_vgs(1);
        player_vol_hfp = hf_get_volume();
    }
    else
#endif
    
    if(a2dp_has_connection()) // change by cai.zhong 20190424 for vol key sync
    //if(a2dp_has_music())  // change by cai.zhong 20190424 for vol key sync
    {
        a2dp_volume_adjust(1);
        player_vol_bt = a2dp_get_volume();
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||defined(CONFIG_BLUETOOTH_AVRCP_TG)
        if(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION) // APP_FLAG_MUSIC_PLAY //change by cai.zhong 20190424 for vol key sync
            &&app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_VOLUME_SYNC)
            && avrcp_current_support_vol_syn())
        {
            current_send_volume_change_response(get_tg_volume_value());
        }
#endif
    }
#ifdef CONFIG_TWS
    if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ))
    {
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
            send_ct_cmd_test((void *)AVC_OP_VOLUME_UP );
        else
            send_cmd_test((void *)AVC_OP_VOLUME_UP);
    }
#endif
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_BT_VOL_INFO);
#endif

    return 0;
}

static int app_button_volm_action(void)
{
    app_handle_t app_h = app_get_sys_handler();
#if 0//def BEKEN_DEBUG
    os_printf("volm\r\n");
#endif

#ifdef CONFIG_BLUETOOTH_HFP
    if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
    {
    	 hf_cmd_set_vgs(0);
        player_vol_hfp = hf_get_volume();
    }
    else
#endif

    if(a2dp_has_connection()) // change by cai.zhong 20190424 for vol key sync
    //if(a2dp_has_music()) // change by cai.zhong 20190424 for vol key sync
    {
        a2dp_volume_adjust(0);
        player_vol_bt = a2dp_get_volume();
    #if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||defined(CONFIG_BLUETOOTH_AVRCP_TG)
        if(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION) //  APP_FLAG_MUSIC_PLAY // change by cai.zhong 20190424 for vol key sync
            &&app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_VOLUME_SYNC)
            && avrcp_current_support_vol_syn())
        {
            current_send_volume_change_response(get_tg_volume_value());
        }
    #endif
    }

#ifdef CONFIG_TWS
    if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ))
    {
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
            send_ct_cmd_test((void *)AVC_OP_VOLUME_DOWN);
        else
            send_cmd_test((void *)AVC_OP_VOLUME_DOWN);
    }
#endif
#if (SYS_VOL_SAVE == 1)
    app_save_volume(ENV_BT_VOL_INFO);
#endif
    return 0;
}

static int app_button_playpause_action( void )
{
    app_handle_t app_h = app_get_sys_handler();
    uint32_t opcode;
    int ret=-1;
#ifdef BEKEN_DEBUG
    os_printf("playpause\r\n");
#endif

    if(is_voice_recog_status_on()) 
    {
        hf_cmd_set_voice_recog(0);
        os_printf("close voice_recog\r\n");
        return TRUE;        
    }
    
    if(avrcp_current_is_connected()
        && !avrcp_current_is_processing())
    {        

    #ifdef BT_ONE_TO_MULTIPLE
        if(a2dp_current_is_pause())
    #else      
        if(!( app_h->flag_sm1 & APP_BUTTON_FLAG_PLAY_PAUSE))//(	a2dp_current_is_pause()) // play
    #endif
        {
            opcode = AVC_OP_PLAY;
            os_printf("play\r\n");
        #ifdef BT_ONE_TO_MULTIPLE			
            a2dp_current_set_flag(APP_BUTTON_FLAG_PLAY_PAUSE);
        #endif
        }
        else
        {
            opcode = AVC_OP_PAUSE;
            os_printf("pause\r\n");
        #ifdef BT_ONE_TO_MULTIPLE
            a2dp_current_clear_flag(APP_BUTTON_FLAG_PLAY_PAUSE);
        #endif
        }
        app_h->flag_sm1 ^= APP_BUTTON_FLAG_PLAY_PAUSE;
    #ifdef CONFIG_BLUETOOTH_AVRCP
        //change begin by cai.zhong 20190419 for audio mix 
        if(lp_pdd_set_avrcp_event(opcode) == LP_ERR_SKIP_EVENT)
            avrcp_current_send_opcode((void *)opcode);
        //change end by cai.zhong 20190419 for audio mix 
    #endif
    #ifdef OP_TIMEOUT_HANDLE
        jtask_stop(app_h->app_auto_con_task);
        jtask_schedule(app_h->app_auto_con_task,
                        2000,
                        app_button_timeout_action,
                        NULL);
    #endif

        ret = 0;
    }
    (void)opcode;
    return ret;
}

#if 0
int app_button_playpause_action_caller( int play_pause )
{
    int i = 2;
    app_handle_t app_h = app_get_sys_handler();
    if(play_pause == 1)
    {
        app_h->flag_sm1 ^= APP_BUTTON_FLAG_PLAY_PAUSE;
    }
    else
        app_h->flag_sm1 |= APP_BUTTON_FLAG_PLAY_PAUSE;

    i = app_button_playpause_action();

    return i;
}

static void app_playpause_handle(void)
{
    app_handle_t app_h = app_get_sys_handler();
    if(app_h->button_type == BUTTON_TYPE_HFP)
    {
        app_button_hfack_action();
    }
    else
    {
        app_button_playpause_action();
    }
}

static int app_button_function(void)
{
    app_handle_t app_h = app_get_sys_handler();
    if(!( app_h->flag_sm1 & APP_FLAG_ACL_CONNECTION ))
    {
        app_button_match_action();
    }
    else if(app_h->button_type == BUTTON_TYPE_NON_HFP)
    {
        os_printf("app_button_playpause_action\r\n");
        app_button_playpause_action();
    }
}
#endif

static int app_button_common_action( uint32 opcode )
{
//    app_handle_t app_h = app_get_sys_handler();

    if(avrcp_is_connection()
        && !avrcp_is_cmd_processing())
    {
//        jtask_stop(app_h->app_auto_con_task);

        #ifdef CONFIG_BLUETOOTH_AVRCP
        avrcp_current_send_opcode((void *)opcode);
        #endif

        return 0;
    }
    else
        return -1;
}

static int app_button_next_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("app_button_next_action\r\n");
#endif
    return app_button_common_action(AVC_OP_NEXT);
}

static  int app_button_prev_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("prev\r\n");
#endif
    return app_button_common_action(AVC_OP_PREV);
}

static  int app_button_rewind_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("rewind\r\n");
#endif
return app_button_common_action(AVC_OP_REWIND);
}

static  int app_button_forward_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("forward\r\n");
#endif
    return app_button_common_action(AVC_OP_FAST_FORWARD);
}
static  int app_button_vol_mute_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("vol_mute\r\n");
#endif
	
#if (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)
	app_customer_hid_switch_mode();
#else	
    app_handle_t app_h = app_get_sys_handler();

    if( app_h->volume_store & 0x40 )
    {
        app_h->volume_store &= ~0x40;
        aud_volume_mute(0);
        aud_PAmute_oper(0);
    }
    else
    {
        app_h->volume_store |= 0x40;
        aud_PAmute_oper(1);
        app_wave_file_play_stop();
        aud_volume_mute(1);
    }
#endif
    return 0;
}
static  int app_button_clear_memory( void )
{
#ifdef BEKEN_DEBUG
    os_printf("clear_memory\r\n");
#endif
    uint8 cmd[24];
    app_handle_t app_h = app_get_sys_handler();

	app_set_led_event_action( LED_EVENT_FM_SCAN );	  // 增加清除配对列表灯 xian
    app_wave_file_play_start( APP_WAVE_FILE_ID_CLEAR_MEMORY );

	if(app_h->flag_sm1 & (APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
    {
        jtask_stop(app_h->app_auto_con_task);
        app_h->flag_sm1 &= ~(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
    }

    if(app_h->flag_sm1 & APP_MATCH_FLAG_SET)
    {
        bt_flag2_operate( APP_FLAG2_MATCH_BUTTON, 1);

    #ifdef CONFIG_BLUETOOTH_AVRCP
        avrcp_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_A2DP
        a2dp_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_HFP
        hf_cmd_disconnect();
    #endif

    #ifdef CONFIG_BLUETOOTH_HSP
        hs_cmd_disconnect();
    #endif

    #if defined(CONFIG_BLUETOOTH_HID)
        if(app_env_check_HID_profile_enable())
            hid_cmd_disconnect();
    #endif

	//直接ACL断开，还有待评估20151116
    #ifdef BT_ONE_TO_MULTIPLE
   	 bt_all_acl_disconnect(app_h->unit);
    #else
        bt_unit_acl_disconnect(app_h->unit,
            (const btaddr_t *)&app_h->remote_btaddr);
    #endif

    }

    memcpy( &cmd[0], (uint8 *)&app_h->remote_btaddr, sizeof(btaddr_t));
    cmd[6] = 0x01;
    hci_send_cmd( app_h->unit, HCI_CMD_DELETE_STORED_LINK_KEY,cmd, 7 );
    memset( (uint8 *)&app_h->remote_btaddr, 0, sizeof( btaddr_t ));

    app_env_clear_all_key_info();
    return 0;
}
static  int app_button_enter_dut_mode( void )
{
#if 0
#ifdef BEKEN_DEBUG
    os_printf("app_button_enter_dut_mode\r\n");
#endif
    if( bt_flag1_is_set( APP_FLAG_DUT_MODE_ENABLE ))
        app_bt_enable_dut_mode(0);
    else
        app_bt_enable_dut_mode(1);
#endif
    return 0;
}
int app_button_lang_change( void )
{
#ifdef BEKEN_DEBUG
    os_printf("lang\r\n");
#endif
    app_env_handle_t env_h = app_env_get_handle();

    env_h->env_cfg.wave_lang_sel++;
	if (env_h->env_cfg.wave_lang_sel >= app_env_get_wave_max_lang_num())
    	env_h->env_cfg.wave_lang_sel = 0;

    env_h->env_data.lang_sel = env_h->env_cfg.wave_lang_sel;

	app_wave_file_play_start( APP_WAVE_FILE_ID_RESERVED3 );

    app_env_write_action(&env_h->env_data.default_btaddr,0);

	return 0;
}
#ifdef CONFIG_BLUETOOTH_HFP
/*static*/  int app_button_redial_last_number( void )
{
    if( bt_flag1_is_set( APP_FLAG_HFP_CONNECTION ) 
        && (!bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP ))
        && (!get_current_hfp_flag(APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING))
        )
    {
        //app_wave_file_play_start( APP_WAVE_FILE_ID_REDIAL );
    	#ifdef BEKEN_DEBUG
    	os_printf("redial\r\n");
    	#endif

        hf_cmd_redial();

        return 0;
    }

    return -1;
}
static  int app_button_micvolp_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("micvolp\r\n");
#endif
    app_handle_t app_h = app_get_sys_handler();

    if( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED ) )
    {
        app_h->mic_volume_store++;

        if( app_h->mic_volume_store > ( SDADC_VOLUME_MAX + 0x6 ) )
            app_h->mic_volume_store = ( SDADC_VOLUME_MAX + 0x6 );

        if( !( app_h->mic_volume_store & 0x80 ) )  // not mute
            aud_mic_volume_set( app_h->mic_volume_store);
    }
    return 0;
}
static  int app_button_micvolm_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("micvolm\r\n");
#endif
    app_handle_t app_h = app_get_sys_handler();

    if( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED ) )
    {
        app_h->mic_volume_store--;
        if( app_h->mic_volume_store < 0 )
            app_h->mic_volume_store = 0;

        if( !( app_h->mic_volume_store & 0x80 ) )  // not mute
            aud_mic_volume_set( app_h->mic_volume_store);
    }
    return 0;
}

// add begin by cai.zhong 20190221 for smartvoice
static int app_button_smartvoice_action(void)
{
    os_printf("KEY smartvoice\r\n");
    lp_pdd_alexa_trigger_button();

}
// add end by cai.zhong 20190221 for smartvoice

static  int app_button_micvol_mute_action( void )
{
#ifdef BEKEN_DEBUG
    os_printf("micvol_mute\r\n");
#endif
    app_handle_t app_h = app_get_sys_handler();

    if( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED ) )
    {
        if( app_h->mic_volume_store & 0x80 )
        {
            app_h->mic_volume_store &= ~0x80;
        #ifdef CONFIG_DRIVER_ADC
			aud_mic_mute(0);
		#endif
		#ifdef CONFIG_DRIVER_I2S
			i2s_rx_volume_adjust( app_h->mic_volume_store );
		#endif
            app_wave_file_play_start(APP_WAVE_FILE_ID_UNMUTE_MIC);
        }
        else
        {
            app_wave_file_play_start(APP_WAVE_FILE_ID_MUTE_MIC);
            app_h->mic_volume_store |= 0x80;
        #ifdef CONFIG_DRIVER_ADC
            aud_mic_mute(1);
        #endif
        #ifdef CONFIG_DRIVER_I2S
            i2s_rx_volume_adjust(0);
        #endif
        }
    }

#ifdef CONFIG_BLUETOOTH_HID
    if(bt_flag1_is_set( APP_FLAG_HID_CONNECTION ))
    {
        os_printf("enter press\n");
        //send_Hid_key_press();

        //app_bt_shedule_task((jthread_func)send_Hid_key_releas, (void *)0, 200);
        photo_Key_Atvice();

    }
#endif
    return 0;
}

static  int app_button_voice_dial_set( void )
{
#ifdef BEKEN_DEBUG
    os_printf("voice_dial\r\n");
#endif
    
    if(!is_ag_support_feature(AG_VOICE_RECOGNITION_FUNCTION)) {
        //os_printf("ag does not support feature: AG_VOICE_RECOGNITION_FUNCTION\r\n");
        return FALSE;
    } else {
        //os_printf("ag supports feature: AG_VOICE_RECOGNITION_FUNCTION.\r\n");
    }
    
    if( bt_flag1_is_set(APP_FLAG_HFP_CONNECTION) ) 
    {
        if( bt_flag1_is_set(APP_FLAG_SCO_CONNECTION) ) 
        {
            hf_cmd_set_voice_recog(0);
            if( bt_flag1_is_set(APP_FLAG_MUSIC_PLAY_SCHEDULE) ) 
            {
                bt_flag1_operate(APP_FLAG_MUSIC_PLAY_SCHEDULE, 0);
                //app_button_playpause_action();
                app_button_type_set(APP_BUTTON_TYPE_A2DP);
            }
        } 
        else
        {
        
            if( bt_flag1_is_set(APP_FLAG_MUSIC_PLAY) ) 
            {
                //app_button_playpause_action();
                bt_flag1_operate(APP_FLAG_MUSIC_PLAY_SCHEDULE, 1);
                app_bt_shedule_task((jthread_func)hf_cmd_set_voice_recog, (void *)1, 1000);
            } 
			if (app_wave_file_play_start(APP_WAVE_FILE_ID_HFP_VOICE_DIAL) == WAV_NO_ERROR)
				app_bt_shedule_task((jthread_func)hf_cmd_set_voice_recog, (void *)1, 500);
            else 
            {
                hf_cmd_set_voice_recog(1);
            }
        }
    }

    return TRUE;
}

#ifdef BT_ONE_TO_MULTIPLE 
static int app_button_eSCO_A_B_swap(void)
{
#ifdef BEKEN_DEBUG
    os_printf("eSCO_A_B_swap\r\n");
#endif
    hfp_2eSCOs_A_B_SWAP();
    return 0;
}
static int app_button_eSCO_A_B_TWC_swap(void)
{
#ifdef BEKEN_DEBUG
    os_printf("eSCO_A_B_TWC_swap\r\n");
#endif
    app_button_eSCO_A_B_swap();
    if(get_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE))
    {
       app_button_type_set(APP_BUTTON_TYPE_TWC); 
    }
    else
    {
       app_button_type_set(APP_BUTTON_TYPE_HFP);  
    }
    return 0;
}
#endif
/*static*/ int app_button_hf_transfer_toggle( void )
{
#ifdef BEKEN_DEBUG
    os_printf("hf_transfer\r\n");
#endif
#ifdef BT_ONE_TO_MULTIPLE
    if( get_current_hfp_flag( APP_FLAG_CALL_ESTABLISHED ))//APP_FLAG_HFP_CALLSETUP
    {
        app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_TRANSFER );

        if(get_current_hfp_flag(APP_FLAG_SCO_CONNECTION))
            app_bt_shedule_task((jthread_func)hf_sco_handle_process, (void *)1, 1000);
        else
            app_bt_shedule_task((jthread_func)hf_sco_handle_process, (void *)0, 1000);
    }
#else
    if( bt_flag1_is_set( APP_FLAG_CALL_ESTABLISHED ))//APP_FLAG_HFP_CALLSETUP
    {
        app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_TRANSFER );

        if( bt_flag1_is_set(APP_FLAG_SCO_CONNECTION ))
            app_bt_shedule_task((jthread_func)hf_sco_handle_process, (void *)1, 1000);
        else
            app_bt_shedule_task((jthread_func)hf_sco_handle_process, (void *)0, 1000);
    }
#endif
	return 0;
}

static int app_button_hold_swtch(void)
{
	#ifdef BEKEN_DEBUG
	os_printf("app_button_hold_swtch\r\n");
	#endif
	
	if(get_current_hfp_flag(APP_FLAG_CALL_ESTABLISHED))
	{
		set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,1);
		hf_cmd_chld(2, -1);
	}

	return 0;
}

/***********************************************************************************************
AT+CHLD=<n>
0 = Releases all held calls or sets User Determined User Busy (UDUB) for a waiting call.
1 = Releases all active calls (if any exist) and accepts the other (held or waiting) call.
1x = Releases specified active call only (<idx>).
2 = Places all active calls (if any exist) on hold and accepts the other (held or waiting) call.
2x = Request private consultation mode with specified call (<idx>). (Place all calls on hold EXCEPT the call indicated by <idx>.)
3 = Adds a held call to the conversation.
4 = Connects the two calls and disconnects the subscriber from both calls (Explicit Call Transfer). Support for this value and its associated functionality is optional for the HF.
*************************************************************************************************/
static  int app_button_twc_hold_accept( void )
{
#ifdef BEKEN_DEBUG
    os_printf("twc_hold\r\n");
#endif
    if( bt_flag1_is_set( APP_FLAG_HFP_CONNECTION ) )
    {
        /* AG not support AT+CHLD=2x command */
        hf_cmd_chld(2, -1);
        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,1);
        //set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,0);
        app_button_type_set(APP_BUTTON_TYPE_TWC);
    }

    return 0;
}
static  int app_button_twc_hung_accept( void )
{
#ifdef BEKEN_DEBUG
    os_printf("twc_hung\r\n");
#endif
    if( bt_flag1_is_set( APP_FLAG_HFP_CONNECTION ) )
    {
        hf_cmd_chld(1, -1);
        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,0);
        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,0);
        app_button_type_set(APP_BUTTON_TYPE_HFP);
    }

    return 0;
}
static  int app_button_twc_reject_hold( void )
{
#ifdef BEKEN_DEBUG
    os_printf("twc_reject\n");
#endif
    if( bt_flag1_is_set( APP_FLAG_HFP_CONNECTION ) )
    {
        hf_cmd_chld(0, -1);
        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,0);
        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,0);
        app_button_type_set(APP_BUTTON_TYPE_HFP);
    }
    return 0;
}
#endif
#ifdef CONFIG_TWS
static int app_button_stereo_action(void)
{
    app_handle_t app_h = app_get_sys_handler();

    app_h->num_stereo_cnt = 0;
    app_button_stereo();
    return 0;
}

static int app_button_stereo_master_action(void)
{
#ifdef BEKEN_DEBUG
    os_printf("app_button_stereo_master_action\r\n");
#endif
    app_wave_file_play_start( APP_WAVE_FILE_ID_STEREO_MATCH );
    bt_flag2_operate(APP_FLAG2_STEREO_ROLE_MASTER, 1);
    app_button_stereo();
    return 0;
}

static int app_button_stereo_slave_action(void)
{
#ifdef BEKEN_DEBUG
    os_printf("app_button_stereo_slave_action\r\n");
#endif

    if(bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
        && bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION))
        return 0;

    app_wave_file_play_start( APP_WAVE_FILE_ID_STEREO_MATCH );
    bt_flag2_operate(APP_FLAG2_STEREO_ROLE_MASTER, 0);
    app_button_stereo();
    return 0;
}
#endif

static int app_button_stop_action( void )
{
#ifdef CONFIG_BLUETOOTH_HID
    if(bt_flag1_is_set( APP_FLAG_HID_CONNECTION ))
    {
        os_printf("enter press\n");
        //send_Hid_key_press();

        //app_bt_shedule_task((jthread_func)send_Hid_key_releas, (void *)0, 200);
        photo_Key_Atvice();

    }
#if (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)
	else
		app_button_playpause_action();
#endif

#endif

#ifdef CONFIG_TWS
	return 0;
#else
    return 0;//app_button_common_action(AVC_OP_STOP);
#endif
}

static int app_button_conn_disconn( void )
{
    app_handle_t app_h = app_get_sys_handler();
#ifdef BEKEN_DEBUG
    os_printf("conn_disconn\r\n");
#endif
    aud_PAmute_oper(1);

    if( bt_flag1_is_set(APP_FLAG_ACL_CONNECTION) )
    {
    #ifdef BT_ONE_TO_MULTIPLE
   	 	bt_all_acl_disconnect(app_h->unit);
    #else	
    	bt_unit_acl_disconnect(app_h->unit, &app_h->remote_btaddr);
    #endif
	}
    else if(app_get_env_key_num())
    {
        bt_flag1_operate(APP_FLAG_AUTO_CONNECTION, 1);
        //bt_unit_acl_connect(app_h->unit, &app_h->remote_btaddr);
	 bt_auto_connect_start();
    }

    return 0;
}
#if 0
//add by zjw for more memory
int app_button_conn_disconn_caller( void )
{
    app_button_conn_disconn();
    return 0;
}
#endif

static void app_set_powerdown_flag(void *arg)
{
    app_env_handle_t  env_h = app_env_get_handle();
    BK3000_icu_sw_powerdown(env_h->env_cfg.system_para.pwrctrl_pin+ GPIO0,POWERDOWN_SELECT);
}

void app_powerdown_action( void )
{
    app_handle_t app_h = app_get_sys_handler();
	app_env_handle_t  env_h = app_env_get_handle();
    /* int gpio = (app_h->button_code[BUTTON_POWERDOWN])& 0x0000ffff; */
#ifdef BEKEN_DEBUG
    os_printf("app_powerdown_action\r\n");
#endif
    app_wave_file_play_stop();
    //No need to clear the flag. Keep it so the protection will remain till end of the cycle
    //bt_flag1_operate(APP_FLAG_POWERDOWN, 0);
    app_h->button_mode = BUTTON_NONE;
    //app_h->button_commit = BUTTON_NONE;
    //app_h->button_press_count = 0;
    //app_h->button_long_press_count = 0;
    app_h->button_state = BUTTON_PRESS_NONE;
    usb_tf_aux_in_out_flag=0;
    j_stack_uninit();
    app_clear_led_event_action(0);    // LED off
    app_clear_led_event_action(1);    // LED off
    app_clear_led_event_action(2);
    app_led_action(1);
    aud_PAmute_oper(1);
    if(SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())
    {
        uint8 pwrctrl_pin = env_h->env_cfg.system_para.pwrctrl_pin;
        gpio_config( pwrctrl_pin, 1 );
        while (1)
        {
            if (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH)
                gpio_output(pwrctrl_pin, 0);
            else
                gpio_output(pwrctrl_pin, 1);
            Delay(1000);	
            CLEAR_WDT;
        }
    }
    app_set_powerdown_flag(NULL);
//    os_printf("ENTER_DEEP_SLEEP\r\n");
//	ENTER_DEEP_SLEEP;

}


#ifdef CONFIG_TWS
void app_powerdown( void )
#else
static void app_powerdown( void )
#endif
{
    app_handle_t app_h = app_get_sys_handler();

    if(bt_flag1_is_set(APP_FLAG_POWERDOWN))
        return;

    os_printf("app_powerdown\r\n");
    app_set_led_low_battery_all_in(1);
    if(!bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))
        app_set_led_event_action(LED_EVENT_POWER_OFF);

#if 1
    if (SYS_WM_BT_MODE !=app_h->sys_work_mode)
    {
        CLEAR_PWDOWN_TICK;
        bt_flag1_operate(APP_FLAG_POWERDOWN, 1);
        aud_PAmute_oper(1);
        os_delay_ms(20);
        if(SYS_WM_LINEIN_MODE == app_h->sys_work_mode)
        {
            //BK3000_Ana_Line_enable(0);
            linein_audio_close();
        }
    #ifdef FM_ENABLE
        else if(SYS_WM_FM_MODE == app_h->sys_work_mode)
        {
            fm_uninstall();
        }
    #endif
        app_powerdown_action();
        return;		
    }
#endif  

    app_sleep_func(0);

    CLEAR_PWDOWN_TICK;

    bt_flag1_operate(APP_FLAG_DUT_MODE_ENABLE, 0);
    bt_flag1_operate(APP_FLAG_POWERDOWN, 1);
    aud_PAmute_oper(1);
    os_delay_ms(20);
//    app_wave_file_play_start(APP_WAVE_FILE_ID_POWEROFF);
#ifdef CONFIG_TWS
    bt_unit_acl_disconnect(app_h->unit,(const btaddr_t *)&app_h->stereo_btaddr);
#endif
    if( app_h->flag_sm1 & APP_AUDIO_FLAG_SET )
    {
    #ifdef BT_ONE_TO_MULTIPLE
        bt_all_acl_disconnect(app_h->unit);
    #else
        bt_unit_acl_disconnect(app_h->unit,(const btaddr_t *)&app_h->remote_btaddr);
    #endif

        app_h->flag_sm1 &= ~(APP_AUDIO_FLAG_SET|APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
    #if 0
        app_bt_shedule_task((jthread_func)bt_unit_disable, (void *)app_h->unit, 0);

        jtask_schedule(app_h->app_reset_task,
                            1000,
                            (jthread_func)hci_disable_task,
                            (void *)app_h->unit);
    #endif
        //os_delay_ms(1000);
        //app_set_led_event_action(LED_EVENT_POWER_OFF);
        app_led_action(1);
        app_bt_shedule_task((jthread_func)app_powerdown_action, (void *)app_h->unit, 2000);
    }
    else
    {
        //os_printf("===have no BT \r\n");
        app_led_action(1);
        app_powerdown_action();
        //app_bt_shedule_task((jthread_func)app_powerdown_action, (void *)app_h->unit, 1000);

    }
    /***************
    else if(!(app_h->flag_sm1 & APP_BUTTON_FLAG_BT_ENABLE))
    {
        //app_set_led_event_action(LED_EVENT_POWER_OFF);
        app_led_action(1);
        app_bt_shedule_task((jthread_func)app_powerdown_action, (void *)app_h->unit, 1000);
    }
    else
    {
        //app_set_led_event_action(LED_EVENT_POWER_OFF);
        app_led_action(1);
        app_bt_shedule_task((jthread_func)bt_unit_disable, (void *)app_h->unit, 0);
        jtask_schedule(app_h->app_reset_task, 1000, (jthread_func)hci_disable_task, (void *)app_h->unit);
    }
    *************/
}

/*static*/ int app_button_powerdown(void)
{
    if(bt_flag1_is_set(APP_FLAG_POWERDOWN))
        return 0;
    app_set_led_low_battery_all_in(1);
    app_set_led_event_action(LED_EVENT_POWER_OFF);

#ifdef BEKEN_DEBUG
    os_printf("app_button_powerdown\r\n");
#endif
#ifdef CONFIG_TWS
    app_bt_sync_send_timer_stop();
    if (bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
    {
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
        send_ct_cmd_test((void *)AVC_OP_STOP );
        else
            send_cmd_test((void *)AVC_OP_STOP);
        Delay(2000);
    }
#endif

#if (CONFIG_CUSTOMER_ENV_SAVE_VOLUME == 1) 
    app_env_handle_t env_h = app_env_get_handle();

    env_h->env_data.volume = a2dp_get_volume();	
    app_env_write_action(&env_h->env_data.default_btaddr,0);
#endif

    start_wave_and_action(APP_WAVE_FILE_ID_POWEROFF, app_powerdown);
    return 0;
}

#ifdef CONFIG_BLUETOOTH_HFP
int app_button_reject_action(void)
{
#ifdef BEKEN_DEBUG
    os_printf("reject\r\n");
#endif
//#ifdef BT_ONE_TO_MULTIPLE
    if(get_current_hfp_flag(APP_FLAG_HFP_CONNECTION))
    {
        if(get_current_hfp_flag(APP_FLAG2_HFP_INCOMING))
        {
            //app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_REJECT );
            hf_cmd_hangup_call(1);
        }
        else if(get_current_hfp_flag(APP_FLAG_CALL_ESTABLISHED))
        {
            app_button_hf_transfer_toggle();
        }
        return 0;
    }
    return -1;
/*********************************************
#else
    app_handle_t app_h = app_get_sys_handler();
    if( app_h->flag_sm1 & APP_FLAG_HFP_CONNECTION )
    {
        if(bt_flag2_is_set(APP_FLAG2_HFP_INCOMING))
        {
            //app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_REJECT );
            hf_cmd_hangup_call(1);
        }
        return 0;
    }

    return -1;
*********************************************/
//#endif
}
int app_button_clear_at_pending(uint16 step)
{
    static uint16 at_pending_step = 0;
    if(get_current_hfp_flag(APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING))
    {
        at_pending_step += step;
        if(at_pending_step > 2000)
        {
            set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING,0);
            set_current_hfp_flag(APP_HFP_AT_CMD_FLAG1_SET | APP_HFP_AT_CMD_FLAG2_SET,0);
            app_button_type_set(BUTTON_TYPE_NON_HFP);
            at_pending_step = 0;
        }
    }
    else
    {
        at_pending_step = 0;
    }
    return 0;
}
int app_button_hfack_action( void )
{
    app_handle_t app_h = app_get_sys_handler();
    if(app_h->button_type != BUTTON_TYPE_HFP)
        return -1;
#if (BT_MODE==BT_MODE_1V2)
#if (CONFIG_CUSTOMER_2PHONES_HUNG_ACCETP==1) && defined(BT_ONE_TO_MULTIPLE)
    if (has_hfp_flag_1toN(APP_FLAG_CALL_ESTABLISHED)
    	&& get_current_hfp_flag(APP_FLAG2_HFP_INCOMING)
    	&& app_check_bt_mode(BT_MODE_1V2))
    {
    	app_set_2phones_hung_accetp(1);//挂断当前通话，接听来电
    }
#endif
#endif
    if(get_current_hfp_flag(APP_FLAG_HFP_CONNECTION)
        && (!get_current_hfp_flag(APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING))
    )
    {
    #ifdef BEKEN_DEBUG
        os_printf("hfack\r\n");
    #endif

        if(get_current_hfp_flag(APP_FLAG_CALL_ESTABLISHED|APP_FLAG_HFP_OUTGOING))
        {
            //app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_REJECT );
            os_printf("===hangup\r\n");
            hf_cmd_hangup_call(0);
            app_h->HF_establish_call = 0;
        }
        else
        {
        	//if((app_h->HF_establish_call == 0))
        #if 0
            if(get_current_hfp_flag(APP_FLAG_SCO_CONNECTION)
                &&get_current_hfp_flag(APP_FLAG_CALL_ESTABLISHED))
            {
                hf_cmd_hangup_call(0);
                app_h->HF_establish_call = 0;
            }
            else
        #endif
            if(get_current_hfp_flag(APP_FLAG_HFP_CALLSETUP))
            {
                app_h->HF_establish_call = 1;
                os_printf("===accept\r\n");
                hf_cmd_accept_call();
                //app_wave_file_play_start( APP_WAVE_FILE_ID_HFP_ACK );
            }
        }
        return 0;
    }

    return -1;
}
#if 0
int app_button_reject_action_match(void)
{
    app_handle_t app_h = app_get_sys_handler();

    if(get_current_hfp_flag(APP_FLAG_HFP_CONNECTION))
    {
        if(get_current_hfp_flag(APP_FLAG2_HFP_INCOMING))
        {
            hf_cmd_hangup_call(1);
        }
        else
        {
            app_button_match_action();
        }
        return 0;
    }

    return -1;
}
#endif
#endif

int app_button_sw_action( uint8_t button_event )
{
    app_handle_t app_h = app_get_sys_handler();

    if((button_event >= BUTTON_BT_END) || (app_h->button_handler[button_event] == NULL))
    {
        return -1;
    }

    return app_h->button_handler[button_event]();
}

static uint8 app_get_led_low_battery_pin(void)
{
    app_handle_t app_h = app_get_sys_handler();

    if ((app_h->led_info[LED_EVENT_CONN].index==0)//以LED_EVENT_CONN为参照
		|| (app_h->led_info[LED_EVENT_CONN].index==5))
        return 0;
    else
        return 1;
}

static uint8 app_get_led_low_battery_index(void)
{
    app_handle_t app_h = app_get_sys_handler();

    if ((app_h->led_info[LED_EVENT_LOW_BATTERY].index>7)
		&& (app_h->led_info[LED_EVENT_LOW_BATTERY].index<11))
    {
        return (app_h->led_info[LED_EVENT_LOW_BATTERY].index-8);
    }

    return LED_EVENT_END;
}

static uint8 app_set_led_low_battery_all_in(uint8 flag)
{
    static uint8 led_map=0;
    static uint8 flag_led_low_battery_all_in=0;
    app_handle_t app_h = app_get_sys_handler();

    if (flag == 2)
    {
        if (flag_led_low_battery_all_in)
            return 1;
        else
            return 0;
    }

    if (app_get_led_low_battery_index() != LED_EVENT_END)
    {
        if (flag == 0)//改变LED灯PIN脚
        {
            flag_led_low_battery_all_in = 1;
            led_map = app_h->led_map[app_get_led_low_battery_pin()];//暂存当前PIN脚

            gpio_output(app_h->led_map[0],LED_OFF_FUNC);
            gpio_output(app_h->led_map[1],LED_OFF_FUNC);
            gpio_output(app_h->led_map[2],LED_OFF_FUNC);

            app_h->led_map[app_get_led_low_battery_pin()] = app_h->led_map[app_get_led_low_battery_index()];
            app_h->last_led_event = LED_EVENT_END;
            app_set_led_event_action(app_h->led_event_save);
        }
        else if (flag_led_low_battery_all_in)//恢复LED灯PIN脚
        {
            flag_led_low_battery_all_in = 0;
            gpio_output(app_h->led_map[app_get_led_low_battery_index()],LED_OFF_FUNC);
            app_h->led_map[app_get_led_low_battery_pin()] = led_map;
        }
    }
    return 0;
}

static __inline void app_set_led_blink_cross(int index1, int index2, uint16 ontime, uint16 offtime)
{
    app_set_led_mode(index1, ontime, offtime, LED_OFF_FUNC);
    app_set_led_mode(index2, ontime, offtime, LED_ON_FUNC);
    app_config_led_mode(index1, 0, 1, -1);
    app_config_led_mode(index2, 0, 1, -1);
}


static void app_set_led_mode(int index, uint16 ontime, uint16 offtime, int initial)
{
    app_handle_t app_h = app_get_sys_handler();
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
    if(index >= 5)
    {
        index -= 5;
    }
#endif
    if(index > 2 || initial > 1)
        return;
#ifdef BEKEN_DEBUG
//  os_printf("app_set_led_mode(index:%d,ontime:%d,offtime:%d,initial:%d)\r\n", index,ontime,offtime,initial);
#endif

    if((app_set_led_low_battery_all_in(2)&&(index!=app_get_led_low_battery_pin()))|| (ontime==0))
    {
        initial = LED_OFF_FUNC;
    }
    app_h->led_ontime[index] = ontime/10;
    app_h->led_offtime[index] = offtime/10;
    app_h->led_onoff[index] = initial;

    app_h->led_blink_count[index] = 0;
    gpio_output(app_h->led_map[index], initial);

    return;
}

static void app_config_led_mode(int index, uint8 repeat, uint8 num_flash, int16 timeout)
{
    app_handle_t app_h = app_get_sys_handler();
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
    if(index >= 5)
    {
        index -= 5;
    }
#endif
    if(index > 2)
        return;
#ifdef BEKEN_DEBUG
//os_printf("app_config_led_mode(index:%d,repeat:%d,num_flash:%d,timeout:%d)\r\n", index,repeat,num_flash,timeout);
#endif
    app_h->led_repeat[index] = repeat*100;
    app_h->led_num_flash[index] = num_flash;
    app_h->led_timeout[index] = timeout/10;

    app_h->led_repeat_count[index] = repeat*100;
    app_h->led_num_flash_count[index] = 0;
}

static void app_clear_led_event_action(int index)
{
    // reset led status to off
    app_set_led_mode(index , 0, 100, LED_OFF_FUNC);
    app_config_led_mode(index, 0, 1, -1);
}

static uint8 app_charge_led_another_on(void)
{
    app_handle_t app_h = app_get_sys_handler();

    if (app_h->led_info[LED_EVENT_CHARGING].index == 7)
        return 1;
    else
        return 0;	
}

static uint8 app_clear_led_event_detect(APP_LED_EVENT event)
{
    app_handle_t app_h = app_get_sys_handler();

    if(app_charge_led_another_on())
    {
        if(!get_Charge_state()
            && (app_h->led_info[event].index<5)
            && (!bt_flag1_is_set(APP_FLAG_LOWPOWER)
                ||(bt_flag1_is_set(APP_FLAG_LOWPOWER)
                &&(app_h->led_info[LED_EVENT_LOW_BATTERY].led_ontime==0) 
                &&(app_h->led_info[LED_EVENT_LOW_BATTERY].led_offtime==0)))
        )	
        return 1;
    }
    else if(app_h->led_info[event].index<5)
    {
        return 1;	
    }

    return 0;
}

#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
uint8 Last_Led_Index=0xFF;
#endif
//APP_LED_EVENT last_led_event;
//APP_LED_EVENT led_event_after_pwrON;
void app_set_led_event_action(APP_LED_EVENT event)
{
    app_handle_t app_h = app_get_sys_handler();
    int i,charge_pin=0,event_pin=0;
    static int charge_led=LED_EVENT_END;

#ifdef BEKEN_DEBUG
    //os_printf("app_set_led_event_action(%d)\r\n", event);
#endif

	//invalid case,return
    if((app_h->last_led_event == LED_EVENT_POWER_OFF)
		||(event >= LED_EVENT_END)
		||( app_h->led_info[event].led_ontime == 0 && app_h->led_info[event].led_offtime == 0 )
		||( app_h->led_info[event].index > 7 )
//		||( (event != LED_EVENT_LINEIN_PLAY) && app_bt_is_flag2_set( APP_FLAG2_LINEIN_SD_PLAYING ) )
		)
    {
        return;
    }

#if (BT_MODE==BT_MODE_1V2)
    if (app_check_bt_mode(BT_MODE_1V2)
        && a2dp_has_connection()
        && (event < LED_EVENT_CONN))
    {
        return;
    }
#endif

	//if the current led_event,will finish in limited time,
	//then waiting for the end
    if((!get_Charge_state())&&(app_h->led_info[event].timeout<=0))
    {
        for(i = 0; i < LED_NUM; i++)
        {
            if((int16)app_h->led_timeout[i]>0)
            {
                app_h->led_event_save = event;
                return;
            }
        }
    }

/*    	if((last_led_event == LED_EVENT_POWER_ON)
			&&(app_h->led_timeout[app_h->led_info[LED_EVENT_POWER_ON].index]>0)
			&&!get_Charge_state())
		{
			if(event != LED_EVENT_POWER_ON)
			{
				led_event_after_pwrON = event;
				return;
			}
		}

		if(event==LED_EVENT_POWER_ON)
			led_event_after_pwrON = LED_EVENT_END;
	last_led_event = event;
	if( event >= LED_EVENT_END )
		return;
*/

    if (get_Charge_state()&& (event!=LED_EVENT_CHARGING)&& (event!=LED_EVENT_BATTERY_FULL)&& bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))     
        return;

    if ((event==LED_EVENT_LOW_BATTERY) && get_Charge_state())	
        return;	

    if(!( (event == LED_EVENT_LOW_BATTERY)|| (event == LED_EVENT_CHARGING)||(event == LED_EVENT_BATTERY_FULL)))
    {
        if (bt_flag1_is_set(APP_FLAG_LINEIN)&& (event!=LED_EVENT_LINEIN_PLAY)
            && (event!=LED_EVENT_LINEIN_PAUSE)&& (event!=LED_EVENT_POWER_OFF))
            return;

        app_h->led_event_save = event;
        if (!app_charge_led_another_on())
        {
            if(get_Charge_state())
                return;
        }
    }

    if(app_h->last_led_event == event)
        return;
    app_h->last_led_event = event;

    if (app_clear_led_event_detect(event))
    {
        app_clear_led_event_action(0);
        app_clear_led_event_action(1);
        app_clear_led_event_action(2);
    }

    if(app_charge_led_another_on())
    {
        if(get_Charge_state())
        {
            if((event == LED_EVENT_BATTERY_FULL)|| (event == LED_EVENT_CHARGING))
            {
                charge_led = event; 
                if ((app_h->led_info[charge_led].index==2)//充电灯配置了LP_LED
                    || (app_h->led_info[charge_led].index==7))
                {
                    if (app_h->led_map[2] == app_h->led_map[0])	
                        app_clear_led_event_action(0);
                    else if (app_h->led_map[2] == app_h->led_map[1])
                        app_clear_led_event_action(1);
                }	
            }
            else if (charge_led != LED_EVENT_END)//有配置充电LED事件
            {
                if ((app_h->led_info[charge_led].index == 3)//交替闪
                    || (app_h->led_info[charge_led].index == 4))//同时闪
                {
                    return;//当前设置的LED灯事件不导入
                }

                charge_pin = app_h->led_info[charge_led].index;//获取充电PIN_LED脚号
                if (charge_pin > 4)
                    charge_pin -= 5;

                if ((charge_pin>1)	//配置了三个PIN_LED脚
                    && (app_h->led_map[charge_pin]!=app_h->led_map[0])
                    && (app_h->led_map[charge_pin]!=app_h->led_map[1]))
                {
                    //正常导入灯配置
                }
                else
                {
                    if ((app_h->led_info[event].index==3)|| (app_h->led_info[event].index==4))//当前设置的LED_event为交替或同时闪
                    {
                        if (app_h->led_map[charge_pin] == app_h->led_map[0])//导入另外一个PIN_LED
                        {
                            app_set_led_mode(1, app_h->led_info[event].led_ontime,
                            app_h->led_info[event].led_offtime, LED_ON_FUNC );
                            app_config_led_mode(1, app_h->led_info[event].repeat,
                            app_h->led_info[event].number_flash, app_h->led_info[event].timeout);
                        }
                        else
                        {
                            app_set_led_mode(0, app_h->led_info[event].led_ontime,
                            app_h->led_info[event].led_offtime, LED_ON_FUNC );
                            app_config_led_mode(0, app_h->led_info[event].repeat,
                            app_h->led_info[event].number_flash, app_h->led_info[event].timeout);
                        }
                        return;
                    }
                    else
                    {
                        event_pin = app_h->led_info[event].index;
                        if (event_pin > 4)
                            event_pin -= 5;		
                        if (app_h->led_map[charge_pin] == app_h->led_map[event_pin])
                        {
                            app_clear_led_event_action(charge_pin^0x01);
                            return;//充电灯事件优先，当前事件不予导入配置
                        }
                    }
                }
            }
        }
    }
	
    if( app_h->led_info[event].index == 3 )
    {
        app_set_led_blink_cross( 0, 1, app_h->led_info[event].led_ontime,app_h->led_info[event].led_offtime );
    }
    else if( app_h->led_info[event].index == 4 )
    {
        app_set_led_mode(0, app_h->led_info[event].led_ontime,app_h->led_info[event].led_offtime, LED_ON_FUNC );
        app_set_led_mode(1, app_h->led_info[event].led_ontime,app_h->led_info[event].led_offtime, LED_ON_FUNC);
        app_config_led_mode( 0, app_h->led_info[event].repeat,app_h->led_info[event].number_flash, app_h->led_info[event].timeout);
        app_config_led_mode( 1, app_h->led_info[event].repeat,app_h->led_info[event].number_flash, app_h->led_info[event].timeout);
    }
    else
    {
        app_set_led_mode(app_h->led_info[event].index, app_h->led_info[event].led_ontime,app_h->led_info[event].led_offtime, LED_ON_FUNC );
        app_config_led_mode( app_h->led_info[event].index, app_h->led_info[event].repeat,app_h->led_info[event].number_flash, app_h->led_info[event].timeout);
    }

    if(bt_flag1_is_set(APP_FLAG_LOWPOWER)&&bt_flag2_is_set(APP_FLAG2_LED_LOWPOWER))
        app_set_led_event_action(LED_EVENT_LOW_BATTERY);

//	if( app_bt_is_flag2_set(APP_FLAG2_LED_LOWPOWER) && (app_h->led_info[LED_EVENT_LOW_BATTERY].index == 2))
//	{
//		app_set_led_mode(app_h->led_info[LED_EVENT_LOW_BATTERY].index, app_h->led_info[LED_EVENT_LOW_BATTERY].led_ontime,
//				app_h->led_info[LED_EVENT_LOW_BATTERY].led_offtime, LED_ON_FUNC );
//		app_config_led_mode( app_h->led_info[LED_EVENT_LOW_BATTERY].index, app_h->led_info[LED_EVENT_LOW_BATTERY].repeat,
//			app_h->led_info[LED_EVENT_LOW_BATTERY].number_flash, app_h->led_info[LED_EVENT_LOW_BATTERY].timeout);
//	}

    return;
}

void app_led_action(uint32 step)
{
    int i;
    app_handle_t app_h = app_get_sys_handler();
    uint8 low_batt_led_index;
    low_batt_led_index = app_h->led_info[LED_EVENT_LOW_BATTERY].index;
    if(low_batt_led_index>=5)
        low_batt_led_index -= 5;

    for(i = 0; i < LED_NUM; i++)
    {
        if(app_h->led_map[i] == LED_INVALID_INDEX)
            continue;
        if(bt_flag2_is_set(APP_FLAG2_LED_LOWPOWER)&&(i!=low_batt_led_index)&&(low_batt_led_index<=2)
            &&(app_h->led_map[i]==app_h->led_map[low_batt_led_index]))
        {
            continue;
        }

        if (app_set_led_low_battery_all_in(2)&& (i!=app_get_led_low_battery_pin()))
            continue;

        if(app_h->led_num_flash_count[i] < app_h->led_num_flash[i])
        {
            app_h->led_blink_count[i] += (uint16)step;
            if( app_h->led_onoff[i] == LED_ON_FUNC )
            {
                if(app_h->led_offtime[i] != 0 && app_h->led_blink_count[i] >= app_h->led_ontime[i])
                {
                    gpio_output_reverse(app_h->led_map[i]);
                    app_h->led_blink_count[i] = 0;
                    app_h->led_onoff[i] = LED_OFF_FUNC;
                    app_h->led_num_flash_count[i]++;
                }
            }
            else
            {
                if(app_h->led_ontime[i] != 0 && app_h->led_blink_count[i] >= app_h->led_offtime[i])
                {
                    gpio_output_reverse(app_h->led_map[i]);
                    app_h->led_blink_count[i] = 0;
                    app_h->led_onoff[i] = LED_ON_FUNC;
                }
            }
        }

	 app_h->led_repeat_count[i] += (uint16)step;
        if(app_h->led_repeat_count[i] >= app_h->led_repeat[i])
        {
            app_h->led_repeat_count[i] = 0;
            app_h->led_num_flash_count[i] = 0;
        }

        if(app_h->led_timeout[i] > 0)
        {
            app_h->led_timeout[i]-=(uint16)step;
            if((int16)app_h->led_timeout[i] <= 0)
            {
                if(app_h->last_led_event==LED_EVENT_LOW_BATTERY)
                    bt_flag2_operate(APP_FLAG2_LED_LOWPOWER, 0);

                if(app_h->last_led_event == LED_EVENT_POWER_OFF)
                    app_clear_led_event_action(i);
                else
                    app_set_led_event_action(app_h->led_event_save);
            }
        }
    }
}

void app_led_dump(void)
{
#if 0
    int i;
    app_handle_t app_h = app_get_sys_handler();

    os_printf("\r\n  low_pwr_flag:%d,last:%d,saved:%d\r\n",!!bt_flag2_is_set(APP_FLAG2_LED_LOWPOWER),app_h->last_led_event,app_h->led_event_save);
    for(i = 0; i < LED_NUM; i++)
    {
        os_printf("LED_NUM%d:\r\n",app_h->led_map[i]);
	 os_printf("on:%d,off:%d,flash:%d,repeat:%d,tov:%d\r\n",
		app_h->led_ontime[i],app_h->led_offtime[i],app_h->led_num_flash[i],app_h->led_repeat[i],app_h->led_timeout[i]);
    }
#endif
}

#if (CONFIG_CHARGE_EN == 1)
//extern volatile uint8 det_charge_flag;
//extern volatile uint8 first_charge_flag ;
#endif
//VBAT 1023:4.6V     VADC:0-3V
//#if (CONFIG_CHARGE_EN == 1)
//adc_list在非CONFIG_CHARGE_EN时也有使用
//uint16 adc_list[20][2] =
//{{24,532},{25,555},{26,577},{27,600},{28,621},{29,644},{30,666},{31,689},{32,712},{33,733},{34,756},{35,778},{36,801},{37,821},{38,844},{39,866},{40,889},{41,912},{42,933},{43,945}};
#if (CONFIG_CHARGE_EN == 1)
//uint16 th_low_trk=0,th_lc=0,th_cv=0,th_high_trk=0,th_cv_trk=0,th_end=0;
CONST uint16 chg_current_idx[8] = {0x02,0x04,0x06,0x08,0x17,0x19,0x1b,0x1d}; /* 40 60 80 100 160 180 200 220 mA*/
//th_low_trk:绢流电压   th_high_trk:  :绢流电压+DELT(0.2V)    DELT:迟滞电压(0.2V)    th_lc:恒流       th_cv:恒压
#endif
//notes: 20161118
//当使用低电量电池进行充电时，考虑到电池内阻可能会比较大
//推荐充电电流设置为40MA,设置恒压模式为3.8V左右,充电结束为4.3V, 恒压模式下I(MAX)<20mA
//避免大电流充电时电压一下起来从而跳出充电状态,呈虚充的状态

/* Battery charge state machine for CHARGE_HARDWARE */
static t_battery_charge_state charge_state = BATTERY_CHARGE_INACTIVE;
t_battery_charge_state get_Charge_state(void)
{
    return charge_state;
}
#ifdef SBC_FIRST_DISCARD_ENABLE
extern uint8 sbc_first_discrad_flag;
extern uint8 sbc_first_discard_count;
extern uint8 sbc_first_discard_data;
#endif

void app_battery_charge_process(void)
{
    uint8 cali_data;
    env_charge_cali_data_t *charge_cali;
    app_env_handle_t env_h = app_env_get_handle();
    app_handle_t app_h = app_get_sys_handler();
#if (CONFIG_CHARGE_MODE == CHARGE_HARDWARE)
    switch(charge_state)
    {
        case BATTERY_CHARGE_INACTIVE:
            if(BK3000_GPIO_DRB & (1<<1))                        /* USB cable plug in; */
            {
                charge_cali = app_get_env_charge_cali_data();
                //env_h = app_env_get_handle();
				//app_handle_t app_h = app_get_sys_handler();

                os_printf("===USB cable plug in\r\n");
                charge_state =  BATTERY_CHARGING;
                BK3000_A3_CONFIG |= (1<<16);
                BK3000_A3_CONFIG |= (1<<17);
                BK3000_A3_CONFIG |= (1<<18);
                BK3000_A4_CONFIG &= ~(1<<21);
                //BK3000_A3_CONFIG = ((BK3000_A3_CONFIG & ~0x3f) | 0x1c);                // cali_vlcf;
                cali_data = charge_cali->charger_vlcf+CINFIG_CHARGER_VLCF_OFFSET;

				if(cali_data > 0x3f)	
					cali_data = 0x3f;
						
                BK3000_A3_CONFIG = ((BK3000_A3_CONFIG & ~0x3f) | cali_data);

                //BK3000_A4_CONFIG = ((BK3000_A4_CONFIG & ~(0x1f<<27)) | (0x15 << 27)); // ical
                cali_data = charge_cali->charger_icp;
                BK3000_A4_CONFIG = ((BK3000_A4_CONFIG & ~(0x1f<<27)) | (cali_data << 27)); // ical

                //BK3000_A4_CONFIG = ((BK3000_A4_CONFIG & ~(0x1f<<22)) | (0x15 << 22)); // vcvcal
                cali_data = charge_cali->charger_vcv;
                BK3000_A4_CONFIG = ((BK3000_A4_CONFIG & ~(0x1f<<22)) | (cali_data << 22)); // vcvcal

                //BK3000_A3_CONFIG = ((BK3000_A3_CONFIG & ~(0x1f<<8)) | (0x04 << 8));   // charge current 60mA
                cali_data = chg_current_idx[env_h->env_cfg.system_para.charger_current & 0x07];
                BK3000_A3_CONFIG = ((BK3000_A3_CONFIG & ~(0x1f<<8)) | (cali_data << 8));   // charge current 60mA

                BK3000_A4_CONFIG |= (1<<12);

                app_set_led_low_battery_all_in(1);

                app_set_led_event_action(LED_EVENT_CHARGING);

		  if(app_env_check_Charge_Mode_PwrDown() || (flag_power_charge))
                {
                    bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,1);
                    if(app_h->flag_sm1 & APP_AUDIO_FLAG_SET )
                    {
                        aud_close();
                        BK3000_Ana_Dac_enable(0);
                        BK3000_dig_dac_close();
                        bt_all_acl_disconnect(app_h->unit);
                        app_h->flag_sm1 &= ~(APP_AUDIO_FLAG_SET|APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
                    #ifdef SBC_FIRST_DISCARD_ENABLE
                        sbc_first_discrad_flag = 1;
                        sbc_first_discard_count = 0;
                        sbc_first_discard_data = 1; //empirical value by measured when recover from prompt wav
                    #endif
                        /* Waiting for BT disconnect */
                        app_bt_shedule_task((jthread_func)app_charge_powerdown, (void *)app_h->unit, 2000);
                    }
                    else
                    {
                        //os_printf("===have no BT connected \r\n");
                        /* Fake power down at once */
                        app_charge_powerdown();
                    }
                }
            }
            break;
        case BATTERY_CHARGING:
            if(!(BK3000_GPIO_DRB & (1<<1)))                    /* USB cable pull out */
            {
                os_printf("===USB cable pull out\r\n");
                if(BK3000_AC_CONFIG & (1<<30))                  /* battery capacity full detect */
                    charge_state =  BATTERY_CHARGE_FINISHED;
                else                                            /* charge be aborted */
                    charge_state =  BATTERY_CHARGE_ABORTED;
            }
            else if(BK3000_AC_CONFIG & (1<<30))                  /* battery capacity full detect */
            {
                charge_state =	BATTERY_CHARGE_FULL;
            }
            break;
        case BATTERY_CHARGE_FULL:
			//os_printf("===charge full:%x\r\n",BK3000_GPIO_DRB);
            app_set_led_event_action(LED_EVENT_BATTERY_FULL);
            charge_state = BATTERY_CHARGE_FINISHED;
            break;
        case BATTERY_CHARGE_FINISHED:
            if(!(BK3000_GPIO_DRB & (1<<1)))                    /* USB cable pull out */
			{
				os_printf("===charge finished\r\n");
				BK3000_A4_CONFIG &= ~(1<<12);                       /* charge disable; */
	            charge_state = BATTERY_CHARGE_INACTIVE;
	            if(bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))
	            {
	                app_powerdown();
	            }
				else
				{
	            	app_h->last_led_event = LED_EVENT_END;
					app_set_led_event_action(app_h->led_event_save);
				}
    		}
            else
            {
                charge_state = BATTERY_CHARGING;   /* Battery has charged full,but USB not pull out */
			#if (CONFIG_CHG_DEEPSLEEP == 1)
				BK3000_A4_CONFIG &= ~(1<<12);                       /* charge disable; */
                BK3000_icu_sw_powerdown(env_h->env_cfg.system_para.wakup_pin+ GPIO0,POWERDOWN_CHG_DEEPSLEEP);
			#endif	
            }
            break;
        case BATTERY_CHARGE_ABORTED:
            os_printf("===charge aborted \r\n");
            BK3000_A4_CONFIG &= ~(1<<12);                       /* charge disable; */
            /* app_set_led_event_action(LED_EVENT_BATTERY_XXXX); */
            charge_state = BATTERY_CHARGE_INACTIVE;
            if( bt_flag2_is_set(APP_FLAG2_LED_LOWPOWER))
                app_clear_led_event_action(2);
            if (bt_flag1_is_set(APP_FLAG_LOWPOWER))
            	app_set_led_low_battery_all_in(0);
            if(bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))
            {
                app_powerdown();
            }
            else
            {
                app_h->last_led_event = LED_EVENT_END;
                app_set_led_event_action(app_h->led_event_save);
            }
            break;
            
        default:
            break;
    }
#endif
}

void app_poweroff_battery_charge_wakeup(void)
{
    static uint8 wakeup_cnt=0;

    if (flag_power_charge)
    {
        uint8 key_valid = 1;
        app_env_handle_t env_h = app_env_get_handle();
        uint8 high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH) ? 1:0 ;

        if(SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())
            key_valid = 0;

        if(((SW_PWR_KEY_SWITCH!=app_env_check_pwrCtrl_mode())
            &&( gpio_input(env_h->env_cfg.system_para.wakup_pin) == key_valid))
            ||((SW_PWR_KEY_SWITCH==app_env_check_pwrCtrl_mode())
            &&(gpio_input(env_h->env_cfg.system_para.pwrctrl_pin)== high_flag)))
        {
            if((++wakeup_cnt) > 100)
            {
                wakeup_cnt = 0;
                msg_put(MSG_POWER_UP);
            }
        }
        else
            wakeup_cnt = 0;
    }
}

void app_low_power_scanning(uint32 step )
{
    app_env_handle_t env_h = app_env_get_handle();
    app_handle_t app_h = app_get_sys_handler();

    saradc_chnl_scanning(step);   

    /**************** Battery level detect ******************/
    app_h->low_detect_count += step;
    if(( app_h->low_detect_count >=  200 ) && (!app_wave_playing ())) // 2S
    {
//        if(!bt_flag1_is_set(APP_FLAG_LOWPOWER)|| (app_h->low_detect_count>=env_h->env_cfg.system_para.lp_interval>>2))
        {
            app_h->low_detect_count = 0;
            msg_put(MSG_LOWPOWER_DETECT);
        }
    }

    /********************** detect end ********************************/

    /*************** Battery charge ************************/
#if (CONFIG_CHARGE_EN == 1)
    /* battery charge state process */
    app_battery_charge_process();  /* battery charge state */
    app_poweroff_battery_charge_wakeup();
#endif
    /*************** charge end ************************/
}

#ifdef IPHONE_BAT_DISPLAY
extern uint8  set_iphone_bat_lever(void);
void set_iphone_bat_level_result(void)
{
    iphone_bat_lever_bak = iphone_bat_lever;
}
#endif
void app_low_power_detect(void )
{
#if (LOWBAT_DET_ENABLE == 1)
    app_handle_t app_h = app_get_sys_handler();
    uint8 lowpower_flag = 0;
    static uint8 lowpower_pdCnt=0;
    //app_sleep_func(0);

#ifdef IPHONE_BAT_DISPLAY
    app_env_handle_t env_h = app_env_get_handle();
    if(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_APP_BAT_DISPLAY)
    {
        if(bt_flag1_is_set(APP_FLAG_HFP_CONNECTION))
        {
            iphone_bat_lever = set_iphone_bat_lever();
            if(iphone_bat_lever != iphone_bat_lever_bak)
            {
                // 如有变化连续发5次,确保手机能收到，不能一直发会导致某些手机进不了SNIFF
                if((++iphone_bat_lever_bak_cnt) >= 5)
                {
                    iphone_bat_lever_bak_cnt = 0;
                    iphone_bat_lever_bak = iphone_bat_lever;
                }
                os_printf("iphone_bat_lever:%d\r\n",iphone_bat_lever);
                hf_cmd_set_iphone_batlevel(iphone_bat_lever);
            }
        }
    }
#endif
    //saradc_init(SARADC_MODE_SINGLESTEP, app_h->low_detect_channel, 3);
    lowpower_flag = saradc_lowpower_status();
    //os_printf("lf:%d,adc:%d,mv:%d\r\n",lowpower_flag,saradc_get_value(),((saradc_get_value()*4600)>>10));
    if( !(app_h->flag_sm1 & APP_FLAG_LOWPOWER) && ((1 == lowpower_flag)||(2 == lowpower_flag)) )
    {
        app_h->flag_sm1 |= APP_FLAG_LOWPOWER;

        if (app_get_led_low_battery_index() == LED_EVENT_END)
        {
            app_set_led_event_action(LED_EVENT_LOW_BATTERY);
            bt_flag2_operate(APP_FLAG2_LED_LOWPOWER, 1);
        }
        else if (get_Charge_state()==0)
        {
            app_set_led_low_battery_all_in(0);
        }
    }
    else if( (app_h->flag_sm1 & APP_FLAG_LOWPOWER) && (0 == lowpower_flag) )
    {
        app_h->flag_sm1 &= ~APP_FLAG_LOWPOWER;
        bt_flag2_operate(APP_FLAG2_LED_LOWPOWER, 0);
        if (get_Charge_state() == 0)
            app_set_led_low_battery_all_in(1);
        app_h->last_led_event = LED_EVENT_END;
        app_set_led_event_action(app_h->led_event_save);
        lowpower_pdCnt=0;
    }

    if( app_h->flag_sm1 & APP_FLAG_LOWPOWER )
    {
        //app_h->flag_sm1 &= ~APP_FLAG_LOWPOWER;
        if((1 == lowpower_flag)||(2 == lowpower_flag))
        {
            os_printf("lowpower_flag=%d\r\n",lowpower_flag);
	     //saradc_reset();
            switch(app_h->sys_work_mode)
            {
            #if (CONFIG_APP_MP3PLAYER == 1)
                case SYS_WM_SD_MUSIC_MODE:
            #ifdef CONFIG_APP_USB     
		  case SYS_WM_UDISK_MUSIC_MODE:
            #endif
                    app_playwav_resumeMp3(APP_WAVE_FILE_ID_LOW_BATTERY);
                    break;
            #ifdef FM_ENABLE
                case SYS_WM_FM_MODE:
                    app_playwav_resumeFM(APP_WAVE_FILE_ID_LOW_BATTERY);
                    break;
            #endif
            #endif
                case SYS_WM_LINEIN_MODE:
                    app_playwav_resumelinein(APP_WAVE_FILE_ID_LOW_BATTERY);
                    break;
                case SYS_WM_BT_MODE:
                    app_wave_file_play_start(APP_WAVE_FILE_ID_LOW_BATTERY);
                    break;

                default:
                    break;
            }
            if(2 == lowpower_flag)
            {
                if((++lowpower_pdCnt) >1)
                {
                    os_printf("low power,power down\r\n");
                #ifdef FM_ENABLE
                    if(SYS_WM_FM_MODE == app_h->sys_work_mode)
                    FM_IC_PowerDown();
                #endif				

                    BK3000_PMU_PERI_PWDS |= bit_PMU_ADC_PWD;
                    start_wave_and_action(APP_WAVE_FILE_ID_POWEROFF, app_powerdown);
                }
                return;
            }
        }
    }
#endif
}
#if 1
 static int app_powerdown_condition_addtick( void )
 {
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();
    int result = 0;
    /* line in and dut mode don't power down */
    if (SYS_WM_BT_MODE == app_h->sys_work_mode)
    {
        if(env_h->env_cfg.bt_para.pd_cond & APP_ENV_BT_PD_FLAG_NOCONN)
        {
            if( !(bt_flag1_is_set(APP_FLAG_ACL_CONNECTION))
            #ifdef CONFIG_TWS
			&& !(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
	        #endif
			)
            {
                result = 1;
		  return result;				
            }
        }
        if(env_h->env_cfg.bt_para.pd_cond & APP_ENV_BT_PD_FLAG_PAUSE_TO)
        {
            if(bt_flag1_is_set(APP_FLAG_A2DP_CONNECTION))
            {
                if(bt_flag1_is_set(APP_FLAG_MUSIC_PLAY ))
                {
                    result = 0;
		      return result;	
                }
                else
                {
                    result = 1;
                }
            }
            if(bt_flag1_is_set(APP_FLAG_HFP_CONNECTION ))
            {
                if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_HFP_CALLSETUP))
                {
                    result = 0;
                }
                else
                {
                    result = 1;
                }
            }
        }
    }
    else if (SYS_WM_LINEIN_MODE == app_h->sys_work_mode)
    {
        result = 0;
    }
#if (CONFIG_APP_MP3PLAYER == 1)
    else if ((SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
    #ifdef CONFIG_APP_USB 
        ||(SYS_WM_UDISK_MUSIC_MODE == app_h->sys_work_mode)
    #endif
        )
    {
        result = 0;
    }
#ifdef FM_ENABLE
    else if (SYS_WM_FM_MODE == app_h->sys_work_mode)
    {
        result = 0;
    }
#endif	
#endif
    return result;
}
#endif

static int app_powerdown_condition_detect(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();
	
    if (((app_h->powerdown_count>0)
			&&(pwdown_tick>=app_h->powerdown_count)
			&&(env_h->env_cfg.bt_para.pd_cond&APP_ENV_BT_PD_FLAG_NOCONN)
			&&!bt_flag1_is_set(APP_FLAG_ACL_CONNECTION)
            )
            || ((app_h->pause_powerdown_count>0)
			&&(pwdown_tick>=app_h->pause_powerdown_count)
			&&bt_flag1_is_set(APP_FLAG_ACL_CONNECTION)
			&&(env_h->env_cfg.bt_para.pd_cond&APP_ENV_BT_PD_FLAG_PAUSE_TO))
            || (app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AUX_MODE_PWR_DOWN)
			&&bt_flag1_is_set(APP_FLAG_LINEIN))
        )	
    {
        return 1;
    }

    return 0;
}

#if (CONFIG_CHARGE_EN == 1)
void app_charge_powerdown( void )
{
    app_handle_t app_h = app_get_sys_handler();
    if((!bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN))
		||(!get_Charge_state())
		||app_wave_playing()
		||(SYS_WM_BT_MODE != app_h->sys_work_mode) //(app_h->flag_sm1 & APP_FLAG_LINEIN)
		||(!app_env_check_Charge_Mode_PwrDown()&&(0 == flag_power_charge))
		)
    return;

//    os_printf("app_charge_powerdown\r\n");
    CLEAR_PWDOWN_TICK;

    app_wave_file_play_stop();
    bt_unit_set_scan_enable( app_h->unit, 0);
    bt_unit_disable( app_h->unit );
    BK3000_set_clock(1, 0);
    set_flash_clk(FLASH_CLK_26mHz);      /**< 将FLASH时钟切换到26MHz */
    flash_set_line_mode(FLASH_LINE_2);
    //ENTER_DEEP_SLEEP;
}
#endif

#ifdef BEKEN_UP_OTA_3266
int app_button_ota_enable_action( void )
{
    app_button_match_action();
    os_printf("ota enable...\r\n");
    flag_ota_enable = 1;
    
    return 1;
}

void app_ota_time_out_action(void)// OTA升级超时复位
{
    static uint16 time_out_cnt=0;
    if(flag_ota_enable)
    {
        if(flag_ota_enable)
        {
            if(flag_ota_wdt_rst)
            {
                flag_ota_wdt_rst --;
                if(1==flag_ota_wdt_rst)
                {
                    flag_ota_wdt_rst = 0;
                    BK3000_wdt_reset();// OTA升级超时复位
                    while(1);
                }
            }

        }

        if(app_ota_is_updating()) // time out
        {
            if(++time_out_cnt>(1000*60)) // 10mins
            {	
                time_out_cnt=0;
                BK3000_wdt_reset();// OTA升级超时复位
                while(1);
            }
        }
        else 
        {
            time_out_cnt=0;
        }
    }
}
extern u_int8  ota_update_flag;
extern volatile uint8 flag_ota_beginning;
uint8 app_ota_is_updating(void)// OTA BLE 正在升级
{
#ifdef BEKEN_SPP_OTA_3266
    if(flag_ota_enable&&flag_ota_beginning&& spp_is_connected())
        return 1;
    else
#endif
#ifdef BEKEN_BLE_OTA_3266
    if(ota_update_flag) 
        return 1;
    else
#endif
        return 0;
}
#else
uint8  ota_update_flag = 0;
uint8 app_ota_is_updating(void)// OTA BLE 正在升级
{
    return 0;
}
#endif

void app_button_led_action( uint32 step )
{
	//app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();
#ifdef BEKEN_UP_OTA_3266
//    app_ota_time_out_action();// OTA升级超时复位
#endif
    app_led_action(step);
    //app_low_power_detect();

    if( app_powerdown_condition_addtick() )
        INC_PWDOWN_TICK(step);
    else
        CLEAR_PWDOWN_TICK;

    if (app_powerdown_condition_detect())	
    {
        CLEAR_PWDOWN_TICK;
//       	msg_put(MSG_POWER_DOWN);
        app_button_powerdown();
    }

    /* line in detect */
    if( env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_LINEIN_ENA )
        app_linein_scanning();

	/* button acton */
    if(!bt_flag1_is_set(APP_FLAG_POWERDOWN))
    {
        button_scanning();

    #ifdef CONFIG_BLUETOOTH_HFP
        app_button_clear_at_pending(step);
    #endif
    }
#ifdef CONFIG_PRODUCT_TEST_INF
    average_freqoffset_rssi();
#endif
    //app_button_photograph();

#if (CONFIG_CUSTOMER_EDGE_KEY == 1)
    app_edge_key();
#endif

#if (CONFIG_CUSTOMER_MOTOR_CONTROL == 1)
    app_M_control();
#endif
    app_env_pwrctrl_by_switch();
#if (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)
    app_customer_hid_disconnect();
#endif

#if (CONFIG_CUSTOMER_1V2_CON_MODE == 1)
	app_customer_1v2_con_close();
#endif
}

#ifdef SBC_FIRST_DISCARD_ENABLE 
extern uint8 sbc_first_discrad_flag;
extern uint8 sbc_first_discard_count;
extern uint8 sbc_first_discard_data;
#endif				
void app_audio_restore(void)
{
    uint32 freq;
    uint32 channel;
    uint32 vol_s;
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();

    if(bt_flag1_is_set(APP_FLAG_POWERDOWN|APP_FLAG_WAVE_PLAYING))
        return;
    app_get_wave_info(&freq, &channel, &vol_s);
    if(bt_flag1_is_set(APP_FLAG_LINEIN))
    {
        //app_handle_t app_h = app_get_sys_handler();
    #if defined(CONFIG_TWS) && !defined(TWS_CONFIG_LINEIN_BT_A2DP_SOURCE)
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER))
            bt_unit_set_scan_enable( app_h->unit, 0);
    #endif
    #ifdef CONFIG_BLUETOOTH_COEXIST
        if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
        {
            //aud_adc_initial(8000,1,16);
            hf_audio_conf_restore(1);
            aud_mic_open(1);
        }
        else
    #endif
        {
            linein_audio_open();
        }

        if(AUDIO_VOLUME_MIN == player_vol_bt)
            aud_volume_mute(1);
        else
            aud_PAmute_oper(0);
        return;
    }
    else if(app_is_bt_mode())
    {
    #ifdef BT_ONE_TO_MULTIPLE
    	if(!(a2dp_has_music()||(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))))
    #else
        if(!bt_flag1_is_set(  APP_FLAG_MUSIC_PLAY|APP_FLAG_SCO_CONNECTION))
    #endif
        {
        #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
            if( bt_flag1_is_set( APP_FLAG_LINEIN ))
                return ;
        #endif
            aud_PAmute_oper(1);
            aud_close();
            aud_volume_mute(1);
        #if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 1)
            //BK3000_Ana_Dac_enable(0);
            //BK3000_dig_dac_close();
        #endif
        }
        else
        {
        	/*
        	   Because both freq and chnanel are parameters of promt wave,this restore is wrong;
        	   and there are much delay in function aud_initial(),ADC isr can't be processed by CPU in time
        	   so MSG pool will be full,and printf("F").
        	*/
        	/*
        	  Fixed Bug#1385
        	  Both freq and chnanel have been saved in function a2dp_audio_restore,
        	  so these parameters are a2dp config param
        	*/
            if(bt_flag1_is_set(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE) && !bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
            {
            	/*avoid the "pop" due to unkown instant data flow overflow*/
            #ifdef SBC_FIRST_DISCARD_ENABLE 
                sbc_first_discrad_flag = 1;
                sbc_first_discard_count = 0;
                sbc_first_discard_data = 1; //empirical value by measured when recover from prompt wav
            #endif	
        	   //aud_close();
		  //aud_initial(freq, channel, 16 );
                a2dp_audio_restore(1);
            }
            aud_volume_set(vol_s );
            if(AUDIO_VOLUME_MIN == vol_s)
            {
                aud_volume_mute(1);
            }
            if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
	     {
	         aud_PAmute_oper(0);
                //aud_adc_initial(8000,1,16);
                hf_audio_conf_restore(1);
                aud_mic_open(1);
	     }

            aud_open();
            if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE))
            {
                if(env_h->env_cfg.feature.pa_unmute_delay_time )
                    os_delay_ms((10*env_h->env_cfg.feature.pa_unmute_delay_time));
                aud_PAmute_oper(0);
            }
            if((app_h->mic_volume_store&0x80)&&(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)))
            {
            #ifdef CONFIG_DRIVER_ADC
                aud_mic_mute(1);
            #endif
            }
            /*
            	Once APP_FLAG_SCO_CONNECTION is true,adc and mic should be initial and opened.
            */

            /*
            if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)) //( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP ) && !( app_h->mic_volume_store & 0x80))
	     {
			aud_adc_initial(8000,1,16);
			aud_mic_open(1);
	     }
		*/
        }
    }
#if (CONFIG_APP_MP3PLAYER == 1)
    else if(app_is_mp3_mode())
    {
    #ifdef CONFIG_BLUETOOTH_COEXIST
        if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
        {
            //aud_adc_initial(8000,1,16);
            hf_audio_conf_restore(1);
            aud_mic_open(1);
        }
        else
    #endif
        {
            if(mp3_need_pause)
            {
            app_player_button_play_pause();
            }
        }
        mp3_need_pause = 0;
    }
#endif


}

void app_bt_status_show( void )
{
    app_handle_t app_h = app_get_sys_handler();
    
    os_printf("BEKEN SW Compliled at %s, %s\r\n", __TIME__, __DATE__ );
#ifdef BT_ONE_TO_MULTIPLE
{
    uint32 flag;
    uint8 i;
    os_printf("------------------------------------------\r\n");
    for(i=0;i<BT_MAX_AG_COUNT;i++)
    {
        flag = get_hf_priv_flag(i,0xffffffff);
        os_printf("|    hfp-private flag %d:%08x\r\n",i,flag);
        flag = get_a2dp_priv_flag(i,0xffffffff);
        os_printf("|   a2dp-private flag %d:%08x\r\n",i,flag);
    }
    os_printf("------------------------------------------\r\n");
}
#endif
#ifdef BEKEN_DEBUG
    if( app_h->unit == NULL )
    {
        os_printf("Bluetooth device not initialized yet.\r\n");
        return;
    }
    os_printf("     HFP flag:%08x\r\n",get_hf_priv_flag(0,0xffffffff));
    os_printf("    a2dp flag:%08x\r\n",get_a2dp_priv_flag(0,0xffffffff));
    os_printf("Global Flag-1:%08x\r\n",app_h->flag_sm1);
    os_printf("Global Flag-2:%08x\r\n",app_h->flag_sm2);
#if 0
    extern void aud_fade_status_debug(void);
    aud_fade_status_debug();
#endif

#if 0
    extern u_int8 syspwr_cpu_halt;
    extern int SYSpwr_Is_Available_Sleep_System(void);
    extern boolean USLCsleep_Is_Tabasco_Asleep(void);
    os_printf("Sleep is allowed:%d,%d,%d,%d,%d\r\n",app_is_not_auto_reconnection(),saradc_start,syspwr_cpu_halt,SYSpwr_Is_Available_Sleep_System(),USLCsleep_Is_Tabasco_Asleep());
    os_printf("CPU irq mask:%08x\r\n",get_spr(SPR_VICMR(0)));
#endif
    os_printf("Device addr: "BTADDR_FORMAT"\r\n", BTADDR(&(app_h->unit->hci_btaddr)) );
    if( app_h->flag_sm1 & APP_FLAG_ACL_CONNECTION )
        os_printf("Remote device addr: "BTADDR_FORMAT"\r\n", BTADDR(&(app_h->remote_btaddr)) );
    else
    {
        os_printf("Not connect to other device. 0x%x \r\n", app_h->flag_sm1);
        return;
    }

    os_printf("A2DP status: %s\r\n",(app_h->flag_sm1 & APP_FLAG_A2DP_CONNECTION )?"connected":"disconnected");

    if( app_h->flag_sm1 & APP_FLAG_MUSIC_PLAY )
        os_printf("Audio stream started now.\r\n");

    os_printf("AVRCP status: %s\r\n",(app_h->flag_sm1 & APP_FLAG_AVCRP_CONNECTION )?"connected":"disconnected");

    os_printf("HFP status: %s\r\n",(app_h->flag_sm1 & APP_FLAG_HFP_CONNECTION )?"connected":"disconnected");
#else
    os_printf(APP_RET_STATUS_DEVICE_INFO, BTADDR(&(app_h->unit->hci_btaddr)));
#endif

}

#ifdef BEKEN_DEBUG
#ifdef A2DP_SBC_DUMP_SHOW
extern void a2dp_sbc_info_show(void);
extern void sbc_encode_frame_info(void);
#endif

void app_bt_debug_show( void )
{
#if 0
    extern int encode_pkts;
    extern int decode_pkts;
    extern int encode_buffer_full;
    extern int encode_buffer_empty;
   
#ifdef A2DP_SBC_DUMP_SHOW
    a2dp_sbc_info_show();
    sbc_encode_frame_info();
#endif

    os_printf("--------sbc decode statistic--------------\r\n");

    os_printf("| encode pkts: %d\r\n",encode_pkts);
    os_printf("| decode pkts: %d\r\n",decode_pkts);
    os_printf("| encode full: %d\r\n",encode_buffer_full);
    os_printf("|encode empty: %d\r\n",encode_buffer_empty);
    os_printf("| encode node: %d\r\n",sbc_buf_get_node_count());
    os_printf("--------sbc decode statistic--------------\r\n");

    os_printf("memory status: \r\n");
    memory_usage_show();
    os_printf("--------tick status----------------------\r\n");
    os_printf("|           sleep tick: %d\r\n", sleep_tick);
    os_printf("|       powerdown tick: %d\r\n", pwdown_tick);
    os_printf("| sniffmode_wakeup_dly: %d\r\n",sniffmode_wakeup_dly);
    os_printf("--------tick status----------------------\r\n");
#endif


    return;
}

void app_bt_debug_info_clear(void)
{
    extern int encode_pkts;
    extern int decode_pkts;
    extern int encode_buffer_full;
    extern int encode_buffer_empty;

    encode_pkts = 0;
    decode_pkts = 0;
    encode_buffer_full = 0;
    encode_buffer_empty = 0;
}
#endif

#if (CONFIG_APP_MP3PLAYER == 1)
static void media_start_task(void* arg)
{
    uint8 type;
    uint32 cur_mode =get_app_mode();
    type = (uint32) arg;

    if( ((cur_mode == SYS_WM_SD_MUSIC_MODE)
        #ifdef CONFIG_APP_USB 
        ||(cur_mode == SYS_WM_UDISK_MUSIC_MODE)
        #endif
        )
   #ifndef CONFIG_BLUETOOTH_COEXIST
       && (!bt_flag1_is_set(APP_AUDIO_FLAG_SET))
   #endif
       )
    {
        os_printf("BT is inactive!!!\r\n");
    #ifdef CONFIG_BLUETOOTH_COEXIST
        if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
        {
            //aud_adc_initial(8000,1,16);
            hf_audio_conf_restore(1);
            aud_mic_open(1);
        }
        else
    #endif
        {
            sbc_mem_free();
            app_player_init();

            if(Media_Fs_Init(type))
            {
                os_printf("media Init Err!!!\r\n");
                msg_put(MSG_CHANGE_MODE);
                return;
            }
        }
    }
    else
    {
        msg_put(MSG_CHANGE_MODE);
        os_printf("BT is still active,flag1=%x\r\n",bt_flag1_is_set(APP_AUDIO_FLAG_SET));
    }
    (void)type;
    (void)cur_mode;
}
#endif

uint8 system_mode_shift_for_linein_change(uint8 attatch_flag)
{
    uint32 new_mode;
    app_handle_t app_h = app_get_sys_handler();
    uint32 old_mode = app_h->sys_work_mode;

    //jtask_stop(app_h->app_common_task);
    if(attatch_flag == 1)
    {
        if(old_mode != SYS_WM_LINEIN_MODE)
            new_mode = SYS_WM_LINEIN_MODE;
        else
            return 0;
    }
    else
    {
        if(SYS_WM_LINEIN_MODE == old_mode)
            new_mode = SYS_WM_BT_MODE;
        else
            return 0;
    }
    work_mode_switch_handle(old_mode, new_mode);
    return 1;
}

#ifdef CONFIG_APP_SDCARD
uint8 system_mode_shift_for_sd_change(uint8 attatch_flag)
{
    uint32 new_mode;
    app_handle_t app_h = app_get_sys_handler();
    uint32 old_mode = app_h->sys_work_mode;
    //jtask_stop(app_h->app_common_task);

    if(attatch_flag)	
    {
        if(SYS_WM_SD_MUSIC_MODE  != old_mode)
            new_mode = SYS_WM_SD_MUSIC_MODE;
        else
            return 0;
    }
    else
    {
        if( SYS_WM_SD_MUSIC_MODE == old_mode)
        {
        #ifdef CONFIG_APP_USB
            if(udisk_is_attached())
                new_mode = SYS_WM_UDISK_MUSIC_MODE;
            else 
        #endif
            if(linein_is_attached())
                new_mode = SYS_WM_LINEIN_MODE;
            else
                new_mode = SYS_WM_BT_MODE;
        }
        else
            return 0;
    }
    work_mode_switch_handle(old_mode,new_mode);
    return 1;
}
#endif

#ifdef CONFIG_APP_USB
uint8 system_mode_shift_for_Udisk_change(uint8 attatch_flag)
{
    uint32 new_mode;
    app_handle_t app_h = app_get_sys_handler();
    uint32 old_mode=app_h->sys_work_mode;
    //jtask_stop(app_h->app_common_task);

    if(attatch_flag)
    {
        app_sleep_func(0);
        if(SYS_WM_UDISK_MUSIC_MODE != old_mode )
            new_mode = SYS_WM_UDISK_MUSIC_MODE;
        else
            return 0;
    }
    else
    {
        if(SYS_WM_UDISK_MUSIC_MODE == old_mode)
        {
        #if defined(CONFIG_APP_SDCARD)
            if(sd_is_attached())
                new_mode = SYS_WM_SD_MUSIC_MODE;
            else
        #endif
            if(linein_is_attached())
                new_mode = SYS_WM_LINEIN_MODE;
            else
                new_mode = SYS_WM_BT_MODE;
        }
        else
            return 0;
    }

    work_mode_switch_handle(old_mode, new_mode);
    return 1;
}
#endif

int system_work_mode_change_button(void)
{
    uint8 new_mode = 0,old_mode = 0;
    uint8 loop = 1;
    app_handle_t app_h = app_get_sys_handler();
    
    //jtask_stop(app_h->app_common_task);
    old_mode = app_h->sys_work_mode;
    new_mode = old_mode;
    do
    {
        new_mode++;
        switch(new_mode)
        {
            case SYS_WM_NULL:
                new_mode = SYS_WM_BT_MODE;
                loop = 0;
                break;
        #if (CONFIG_APP_MP3PLAYER == 1)        
        #if defined(CONFIG_APP_SDCARD)
            case SYS_WM_SD_MUSIC_MODE:
                if(sd_is_attached())
                {
                #if defined(CONFIG_LINE_SD_SNIFF)
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                    app_exit_sniff_mode();
                #endif
                    loop = 0;
                }
                break;
        #endif
        #ifdef CONFIG_APP_USB
            case SYS_WM_UDISK_MUSIC_MODE:
                if(udisk_is_attached())
                {
                #if defined(CONFIG_LINE_SD_SNIFF)
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                    app_exit_sniff_mode();
                #endif
                    loop = 0;
                }
                break;
        #endif
        #ifdef FM_ENABLE
            case SYS_WM_FM_MODE:
                loop = 0;
                break;
        #endif
        #endif

            case SYS_WM_LINEIN_MODE:
                if(linein_is_attached())
                {
                #if defined(CONFIG_LINE_SD_SNIFF)
                    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,1);
                    app_exit_sniff_mode();
                #endif
                    loop = 0;
                }
                break;

            default:
                loop = 1;
                break;
        }
    }while(loop);
    
    if(new_mode == old_mode)
    {
        return 0;
    }

    work_mode_switch_handle(old_mode,new_mode);
    return 1;
}
uint8 system_work_mode_direct_change(uint32 new_mode)
{
    app_handle_t app_h = app_get_sys_handler();
    uint32 old_mode=app_h->sys_work_mode;
    uint8 change=0;
    //jtask_stop(app_h->app_common_task);
    if(new_mode == old_mode)
        return 0;
    switch(new_mode)
    {
        case SYS_WM_BT_MODE:
            change = 1;
            break;
    #if (CONFIG_APP_MP3PLAYER == 1)
    #ifdef CONFIG_APP_USB
        case SYS_WM_UDISK_MUSIC_MODE:
            if(udisk_is_attached())
                change = 1;
            break;
    #endif
    #if defined(CONFIG_APP_SDCARD)
        case SYS_WM_SD_MUSIC_MODE:
            if(sd_is_attached())
            change = 1;
            break;
    #endif
    #ifdef FM_ENABLE
        case SYS_WM_FM_MODE:
            change = 1;
            break;
    #endif 
    #endif
        case SYS_WM_LINEIN_MODE:
            if(linein_is_attached())
                change = 1;
            break;
        default:
            change = 0;	
            break;
    }
    if(change)
    {
        work_mode_switch_handle(old_mode, new_mode);
        return 1;
    }
    return 0;
}
void exit_work_mode(uint32 mode)
{
    app_handle_t app_h = app_get_sys_handler();
    os_printf("exit work mode :%x\r\n",mode);
    aud_PAmute_oper(1);
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#if defined(CONFIG_LINE_SD_SNIFF)
    bt_flag2_operate(APP_FLAG2_LINEIN_SD_PLAYING,0);
#endif
    app_exit_sniff_mode();
#endif
   
    switch(mode)
    {
    #if (CONFIG_APP_MP3PLAYER == 1)
        case SYS_WM_SD_MUSIC_MODE:
    #ifdef CONFIG_APP_USB 
        case SYS_WM_UDISK_MUSIC_MODE:
    #endif
        #ifdef CONFIG_BLUETOOTH_COEXIST
            if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
            {

            }
            else
        #endif
            {
            #ifdef CONFIG_BLUETOOTH_COEXIST
                app_change_mode_flag_set();
            #endif
                app_player_play_pause_caller(0);
                Delay(1000);
                app_player_unload(mode);
            #ifdef CONFIG_BLUETOOTH_COEXIST
                app_change_mode_flag_clear();
            #endif
            }
            break;
    #ifdef FM_ENABLE
        case SYS_WM_FM_MODE:
            fm_uninstall();
            break;
    #endif		
    #endif	
        case SYS_WM_LINEIN_MODE:
            app_linein_exit();
            break;
            
        case SYS_WM_BT_MODE:
            if(0)// (app_h->flag_sm1 & APP_FLAG_AVCRP_PROCESSING) // AVRCP is processing
            {
                os_printf("Can not disconnect BT!\r\n");
            }
            else
            {
            #ifdef CONFIG_BLUETOOTH_COEXIST
                app_change_mode_flag_set();
            #else
                app_h->flag_sm1 &= ~(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
                bt_auto_connect_stop();
                if(app_h->flag_sm1 & APP_AUDIO_FLAG_SET) // (bt_flag1_is_set(APP_FLAG_ACL_CONNECTION))
                {
                    aud_close(); // to void (BT mode is playing music now,then turn to SD mode will cause a noise.) // xugaojing 20180116
                    bt_unit_acl_disconnect(app_h->unit, (const btaddr_t *)&app_h->remote_btaddr);
                    os_delay_ms(1000);
                    app_h->flag_sm1 &= ~(APP_AUDIO_FLAG_SET|APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
                }
                else
                {
               
                }
                 // Disable BT scan
                bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
                jtask_stop(app_h->app_auto_con_task);
                jtask_stop(app_h->app_reset_task);
            #endif
            }
            break;

        default:
            break;
    }
}

void enter_work_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t env_h = app_env_get_handle();
    os_printf("enter work mode:%x\r\n",app_h->sys_work_mode);
    switch(app_h->sys_work_mode)
    {
        case SYS_WM_BT_MODE:
        #ifndef CONFIG_BLUETOOTH_COEXIST
            app_bt_acl_time_out_wrap();
        #endif
            if( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_AUTO_CONN)
            {
                app_h->flag_sm1 |= APP_FLAG_AUTO_CONNECTION;
            }
        #ifdef CONFIG_BLUETOOTH_COEXIST
            if(!hci_get_acl_link_count(app_h->unit))
                bt_auto_connect_start();
            else
            {
                if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED) )
                {

                }
                else
                {
                    bt_flag1_operate(APP_FLAG_AUTO_CONNECTION, 0);
                    bt_flag1_operate(APP_FLAG_RECONNCTION, 0);
                    // Make sure all reconnection flags are cleared
                    bt_flag2_operate(APP_FLAG2_RECONN_AFTER_CALL,0);
                    bt_flag2_operate(APP_FLAG2_RECONN_AFTER_PLAY,0);
                    app_wave_file_play_stop();
                }
            }
        #endif
            break;
    #if (CONFIG_APP_MP3PLAYER == 1)
    #if defined(CONFIG_APP_SDCARD)
        case SYS_WM_SD_MUSIC_MODE:
            media_start_task((void*)0);
            break;
    #endif
    #ifdef CONFIG_APP_USB
        case SYS_WM_UDISK_MUSIC_MODE:
            media_start_task((void*)1);
            break;
    #endif
    #ifdef FM_ENABLE
        case SYS_WM_FM_MODE:
            fm_install(NULL);
            break;
    #endif
    #endif
        case SYS_WM_LINEIN_MODE:
            app_linein_enter(NULL);
            break;
        default:
            break;
    }
}

static void work_mode_switch_handle(uint32 mode_before,uint32 mode_now)
{
    app_handle_t app_h = app_get_sys_handler();
    app_h->sys_work_mode = mode_now;
    exit_work_mode(mode_before);
}

uint32 get_app_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return (app_h->sys_work_mode);
}
/*
unsigned int get_bt_mode()
{
    app_handle_t app_h = app_get_sys_handler();
    return (app_h->bt_mode);
}
*/
/*void set_bt_mode(unsigned int mode)
{
    app_handle_t app_h = app_get_sys_handler();
    app_h->bt_mode = mode;

    os_printf("set bt_mode to %d\r\n", mode);
}*/

#if (CONFIG_APP_MP3PLAYER == 1)
uint32 app_is_mp3_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return ((SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
            #ifdef CONFIG_APP_USB
                ||(SYS_WM_UDISK_MUSIC_MODE == app_h->sys_work_mode)
            #endif
        );
}
#endif

uint32 app_is_bt_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return (SYS_WM_BT_MODE == app_h->sys_work_mode);
}
uint32 app_is_linein_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return (SYS_WM_LINEIN_MODE == app_h->sys_work_mode);
}
#ifdef FM_ENABLE
uint32 app_is_fm_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return (SYS_WM_FM_MODE == app_h->sys_work_mode);
}
#endif
uint8 app_get_bt_vol(void)
{
    if( bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING)
        || bt_flag2_is_set(APP_FLAG2_HFP_INCOMING)
        || bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
    {
        return player_vol_hfp;
    }
    else
    {
        return player_vol_bt;
    }
}

void app_env_save_task(void *arg,uint8 type)
{
    app_handle_t app_h = app_get_sys_handler();
    jtask_stop(app_h->app_save_env_task);
    app_env_backup(arg,type);

    switch(type)
    {
    #if (CONFIG_APP_MP3PLAYER == 1)
        case ENV_SD_INFO:
        case ENV_UDISK_INFO:
            os_printf("*****save break point\r\n");
            app_evn_wr_flash(NULL);
            break;
    #endif
    
    #if (CONFIG_APP_MP3PLAYER == 1)
        case ENV_MUSIC_VOL_INFO:
    #ifdef FM_ENABLE
        case ENV_FM_VOL_INF:
    #endif
    #endif       
        case ENV_BT_VOL_INFO:
        case ENV_LINE_VOL_INFO:
        #if (SYS_VOL_SAVE == 1)
            os_printf("*****save vol  3 Second later\r\n");
            jtask_schedule(app_h->app_save_env_task,3000,app_evn_wr_flash,NULL);
        #endif
            break;

        default:
            break;
    }
}

void app_restore_volume(uint8 voltype)
{
    app_env_restore(NULL,voltype);
    hf_volume_init(player_vol_hfp);
}
#if (SYS_VOL_SAVE == 1)
void app_save_volume(uint8 voltype)
{
    app_env_save_task(NULL,voltype);
}
#endif
/***********************************************************
 *
 * bt protocol stack wrap API
 *
 ***************************************************************/

#ifdef A2DP_PATCH_FOR_AVRCP
/* when bluetooth connects, some mobile phones do not launch avrcp-request*/

btaddr_t *a2dp_raddr_ptr = 0;
jtask_h a2dp_try_avrcp_connect_timer;

#define A2DP_TRY_AVRCP_PERIOD               (1000)

void a2dp_patch_get_raddr(btaddr_t *addr)
{
    a2dp_raddr_ptr = addr;
}

void a2dp_try_avrcp_timer_handler(void *arg)
{
    if(0 == a2dp_raddr_ptr)
    {
        return;
    }

    if(!avrcp_is_connected_based_on_raddr(a2dp_raddr_ptr))
    {
        app_bt_active_conn_profile(PROFILE_ID_AVRCP, (void *)a2dp_raddr_ptr);
    }

    a2dp_raddr_ptr = 0;
}

uint32 a2dp_startup_try_avrcp(void)
{
    if(0 == a2dp_try_avrcp_connect_timer)
    {
        jtask_init(&a2dp_try_avrcp_connect_timer, J_TASK_TIMEOUT);
    }

    if(a2dp_try_avrcp_connect_timer)
    {
        jtask_schedule(a2dp_try_avrcp_connect_timer,
                        A2DP_TRY_AVRCP_PERIOD,
                        a2dp_try_avrcp_timer_handler,
                        (void *)0);
    }

    return UWE_OK;
}

void a2dp_terminate_try_avrcp(void)
{
    if(a2dp_try_avrcp_connect_timer)
    {
        jtask_stop(a2dp_try_avrcp_connect_timer);
    }
}

void a2dp_finish_try_avrcp(void)
{
    if(a2dp_try_avrcp_connect_timer)
    {
        jtask_stop(a2dp_try_avrcp_connect_timer);
        jtask_uninit(a2dp_try_avrcp_connect_timer);

        a2dp_try_avrcp_connect_timer = 0;
        a2dp_raddr_ptr = 0;
    }
}
#endif

#ifdef BT_ONLY_ONE_BT
void app_bt_role_switch(uint8 role )
{
    app_handle_t sys_hdl = app_get_sys_handler();
    hci_switch_role_cp  sw_cp;
    hci_write_link_policy_settings_cp  lp_cp;

    if( sys_hdl->unit == NULL )
        return;

    if(1)
    {
        lp_cp.con_handle = sys_hdl->link_handle;
        lp_cp.settings = HCI_LINK_POLICY_ENABLE_ROLE_SWITCH;
        hci_send_cmd( sys_hdl->unit, HCI_CMD_WRITE_LINK_POLICY_SETTINGS, (void *)&lp_cp, sizeof(lp_cp));
    }

    os_printf("role switch: %d\r\n", role );
    if( sys_hdl->unit == NULL )
        return;

    memcpy( (void *)&sw_cp.btaddr, (void *)&sys_hdl->remote_btaddr, sizeof(btaddr_t) );
    sw_cp.role = role;

    hci_send_cmd( sys_hdl->unit, HCI_CMD_SWITCH_ROLE, (void *)&sw_cp, sizeof(hci_switch_role_cp));

    return;
}

void app_bt_set_role_conning(void)
{
    app_bt_role_switch(1); // 0:Master   1:Slave
}
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP_TG
/*解决Sony xperia音量调节bug,底层lib会调用此函数*/
void AVRCP_TG_VOL_adjust_callback(uint8 opera)
{
    if(opera == 1)
        a2dp_volume_adjust(1);
    else
        a2dp_volume_adjust(0);
}

//const uint8_t volume_table[16+1]={0,0x08,0x11,0x19,0x22,0x2a,0x33,0x3b,0x44,0x4c,0x55,0x5d,0x66,0x6e,0x77,0x7e,0x7f};
//const uint8_t volume_table[16+1]={0,0x07,0xf,0x17,0x1f,0x27,0x2f,0x37,0x3f,0x47,0x4f,0x57,0x5f,0x67,0x6f,0x77,0x7f};

extern btaddr_t *a2dp_get_current_app_remote_addr(void);
extern int8 a2dp_get_volume_based_on_raddr(const btaddr_t *raddr);
extern void a2dp_volume_init_based_on_raddr(int8_t aud_volume, const btaddr_t *raddr);

uint8_t get_tg_volume_value(void)
{
    uint32_t vol = a2dp_get_volume();
    return (int8)vol <= 0 ? 0
		: (vol >= 16) ? 0x7f:(vol * 8 + 6);
}

void sync_ct_tg_volume(uint8_t volume_dat, btaddr_t *raddr)
{
    uint8_t vol_idx = ((uint32_t)volume_dat + 1) / 8;
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
    {
        return ;
    }
#endif

    a2dp_volume_init_based_on_raddr(vol_idx, raddr);
    //os_printf("volume_index=%d\r\n",i);
    if(btaddr_same(raddr,a2dp_get_current_app_remote_addr()) && (!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)) && (!app_wave_playing ()))
    {
        aud_volume_set(vol_idx);
        player_vol_bt = vol_idx;
    #if (SYS_VOL_SAVE == 1)
        app_save_volume(ENV_BT_VOL_INFO);
    #endif
        if(vol_idx==0)
        {
            aud_PAmute_oper(1);
            aud_volume_mute(1);
            bt_flag2_operate(APP_FLAG2_VOL_MUTE, 1);
        }
    }
}

void AVRCP_TG_VOL_set_callback(uint8_t volume_dat, btaddr_t *raddr)
{
    if(btaddr_same(raddr,a2dp_get_current_app_remote_addr()) && (!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)) && (!app_wave_playing ()))
    {
        uint32_t vol_idx      = ((uint32_t)volume_dat + 1) / 8;
        uint32_t a2dp_vol_idx = a2dp_get_volume();

        if(vol_idx > a2dp_vol_idx)
        {
            a2dp_volume_adjust(1);
        }
        else if(vol_idx < a2dp_vol_idx)
        {
            a2dp_volume_adjust(0);
        }

        sync_ct_tg_volume(volume_dat, raddr);
    }
    else
    {
        sync_ct_tg_volume(volume_dat, raddr);
    }
    //os_printf("iphone_volume_set=%x\r\n",volume_dat);
}

uint8_t AVRCP_TG_VOL_get_callback(btaddr_t *raddr)
{
    uint8_t volume_index;
    volume_index = a2dp_get_volume_based_on_raddr(raddr);
    if(volume_index == AUDIO_VOLUME_MAX+1)
        return 0x60;
    else
        return (int8)volume_index <= 0 ? 0
			: (volume_index >= 16) ? 0x7f
			:(volume_index * 8 + 6);
	//return volume_index > 0 ? ((volume_index+1) * 8 - 1) : 0;
}

#else
void AVRCP_TG_VOL_adjust_callback(uint8 opera)
{

}
void AVRCP_TG_VOL_set_callback(uint8_t volume_dat, btaddr_t *raddr)
{

}
uint8_t AVRCP_TG_VOL_get_callback(btaddr_t *raddr)
{
    return 0;
}
#endif

#ifdef CONFIG_TWS
void send_ct_cmd_test1(void *arg,uint8 len);

void app_bt_role_switch(uint8 role )
{
    app_handle_t app_h = app_get_sys_handler();
    hci_switch_role_cp  sw_cp;
    hci_write_link_policy_settings_cp  lp_cp;

    os_printf("enable role switch: %d\r\n", role );

    if( app_h->unit == NULL )
        return;

    if(1)
    {
        os_printf("role switch enable\n");
        lp_cp.con_handle = app_h->link_handle;
        lp_cp.settings = HCI_LINK_POLICY_ENABLE_ROLE_SWITCH;
        hci_send_cmd( app_h->unit, HCI_CMD_WRITE_LINK_POLICY_SETTINGS, (void *)&lp_cp, sizeof(lp_cp));
    }

    os_printf("role switch: %d\r\n", role );

    if( app_h->unit == NULL )
        return;
    bt_flag1_operate(APP_FLAG_ROLE_SWITCH,0);

    memcpy( (void *)&sw_cp.btaddr, (void *)&app_h->remote_btaddr, sizeof(btaddr_t) );
    sw_cp.role = role;

    hci_send_cmd( app_h->unit, HCI_CMD_SWITCH_ROLE, (void *)&sw_cp, sizeof(hci_switch_role_cp));

    return;
}


void app_bt_role_discovery( void )
{
    app_handle_t app_h = app_get_sys_handler();
    hci_role_discovery_cp  sw_cp;

    //os_printf("role switch: %d\r\n", role );

    if( app_h->unit == NULL )
        return;

    memcpy( (void *)&sw_cp.con_handle, (void *)&app_h->link_handle, 2 );

    hci_send_cmd( app_h->unit, HCI_CMD_ROLE_DISCOVERY, (void *)&sw_cp, sizeof(hci_role_discovery_cp));

    return;
}

void app_tws_set_stereo_role(void)
{
#ifdef JUDGE_PIN_STEREO
//		STEREO2_DEBUG_INIT;
	gpio_config(PIN_STEREO_ROLE, 0);
	Delay(20);
	if(gpio_input(PIN_STEREO_ROLE)||gpio_input(7))
//		if(gpio_input(PIN_STEREO_ROLE))
		g_stereroleflag = 1;
	else
		g_stereroleflag = 0;
	os_printf("stereo_flag:%d---------------\r\n",g_stereroleflag);
	//g_stereroleflag = 1;
#endif
}
int app_button_stereo( void )
{
    if(bt_flag1_is_set(APP_FLAG_MUSIC_PLAY
                        | APP_FLAG_HFP_CALLSETUP
                        | APP_FLAG_CALL_ESTABLISHED
                        | APP_FLAG_SCO_CONNECTION))
    {
        os_printf("%s, plz (suspend music/hang up call) first then to start TWS M/S matching! return!\r\n", __func__);
        return 1;
    }
    app_handle_t app_h = app_get_sys_handler();
    char cmd[7];
    app_h->inquiry_ticks = 0;
    app_bt_stereo_auto_conn_stop();
#ifdef JUDGE_PIN_STEREO
    bt_flag2_operate(APP_FLAG2_STEREO_ROLE_MASTER, g_stereroleflag);
#endif
#if 0
    if (app_h->num_stereo_cnt > MAX_STEREO_CNT)
    {
        os_printf("Stereo timeout\r\n");
        jtask_stop(app_h->app_auto_con_task);

        // Get prepared for new stereo connection? xhe
        app_bt_inquiry( 0, 0, 1 );
        #if 0
        memcpy( &cmd[0], (uint8 *)&app_h->stereo_btaddr, sizeof(btaddr_t));
        cmd[6] = 0x00;
        hci_send_cmd( app_h->unit, HCI_CMD_DELETE_STORED_LINK_KEY,cmd, 7 );

        bt_flag2_operate( APP_STEREO_FLAG_SET, 0 );
        bt_unit_write_inquiry_IAC( app_h->unit, 1 );
        #endif
        bt_unit_write_inquiry_IAC( app_h->unit, 0 );
        bt_unit_set_scan_enable( app_h->unit, HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE );
        return 0;
    }
    else
    {
        os_printf("Stereo count %d\r\n", app_h->num_stereo_cnt);
        app_h->num_stereo_cnt++;
    }
#endif

    if( app_h->flag_sm1 & (APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
    {
        jtask_stop( app_h->app_auto_con_task );
        app_h->flag_sm1 &= ~(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION);
    }

//    if( bt_flag1_is_set( APP_FLAG_AUTO_CONNECTION ) )
//    {
//        bt_flag2_operate( APP_FLAG2_STEREO_BUTTON_PRESS, 1 );
//        return 0;
//    }
//    jtask_stop( app_h->app_auto_con_task );
//    app_bt_sleep_enable(0);

    // Disconnect if already connected???? xhe
    if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) )
    {
        os_printf("stereo conn\r\n");
        bt_flag2_operate( APP_FLAG2_STEREO_BUTTON_PRESS, 1 );
		avrcp_cmd_ct_disconnect();
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
        {
			a2dp_src_cmd_disconnect();
			//avrcp_cmd_disconnect();
			//a2dp_cmd_disconnect();
        }
        else
        {
            a2dp_cmd_disconnect();
        }

        return 0;
    }

    //os_printf("button press\r\n");
    app_set_led_event_action( LED_EVENT_STEREO_MATCH_MODE);
//	app_wave_file_play_start( APP_WAVE_FILE_ID_STEREO_MATCH );

    // Get prepared for new stereo connection? xhe
    memcpy( &cmd[0], (uint8 *)&app_h->stereo_btaddr, sizeof(btaddr_t));
    cmd[6] = 0x00;
    hci_send_cmd( app_h->unit, HCI_CMD_DELETE_STORED_LINK_KEY,cmd, 7 );

    bt_flag2_operate( APP_STEREO_FLAG_SET, 0 );
//    app_bt_inquiry( 0, 0, 1 );
    app_bt_stereo_pairing_exit();

//    app_bt_inquiry( APP_BT_INQUIRY_DURATION, 1, 0);
//    if( 1 == app_bt_get_disconn_event() )
//        bt_unit_set_scan_enable( app_h->unit, HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE );
#ifndef JUDGE_PIN_STEREO
    if(!app_get_env_stereo_key_used())
        app_bt_stereo_judge_role_again();
    else
#endif
    {
        if( bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
            app_bt_stereo_pairing_master();
        else
            app_bt_stereo_pairing_slave();
    }
    return 0;
}


#define con_audio_interval_xms(val) (val*32/10)

void app_bt_sync_send_timer( void *arg )
{
    if(bt_flag2_is_set(APP_FLAG2_STEREO_PLAYING))
    {
    #ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
        if( bt_flag1_is_set( APP_FLAG_LINEIN ))
        {
            sync_data.u.sync_send.bt_clk = app_bt_read_CLKN();
            //sync_data.u.sync_send.bt_clk += (44.1*40);
            sync_data.u.sync_send.clk_mode = 9;
        }
        else
    #endif
        {
            app_sbc_dac_clk_adjust();
            sync_data.u.sync_send.clk_mode = get_sbc_clk_mode();
            sync_data.u.sync_send.bt_clk = app_bt_read_CLKN();
        }

        if(sbc_check_src_num())
            send_ct_cmd_test1(&sync_data,sizeof(struct sync_send_s));
        sbc_show_all_status();
    }

//	tmp *= 2;
//	if(tmp > 10000)
//		tmp = 10000;
//	arg = tmp;
//	#ifndef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    app_handle_t app_h = app_get_sys_handler();
    jtask_schedule( app_h->app_sync_task, (uint32)arg, (jthread_func)app_bt_sync_send_timer,(void *)arg );
//	#endif
}
#define make_dac_num_to_time_44100(x) ((x)*10/441) //ms
#define make_dac_num_to_time_48000(x) ((x)/48) //ms


#define make_clkn_to_dac_num_44100(x) (((x) * 3125*441/100000))
#define make_clkn_to_dac_num_48000(x) (((x) * 3125*48/10000))
//clkn: 1=312.5us
//fs=44.1K时, 1= (1000/44.1) us
//所以, rzl = x *312.5/((1000/44.1)) = x * 3125*441/100000
#define con_sync_start_flag 7
void app_bt_store_audio_sync( void * arg ,uint8_t len)
{
//    app_handle_t app_h = app_get_sys_handler();
	sync_data_TypeDef *sync_data_rx;
	sync_data_rx = (sync_data_TypeDef *)arg;

	uint32_t now;
	int32_t target_sync,latency;
//	rx[0] = *((uint32_t *)arg);
//	rx[1] = *((uint32_t *)arg+1);
//	if((rx[0]&0xffffff00)==0x5a5a5a00)
//	{
//		aux_volue_bak=(rx[0]&0x0f);
//		aux_volue_bak--;
//		app_button_volp_action();
//	}
//	else
//	os_printf("len:%d\r\n",len);
	if((!bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
	||(bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)))
	{
	os_printf("not_stereo_slave\r\n");
	return;
	}


	if(!bt_flag2_is_set(APP_FLAG2_STEREO_SLAVE_GET_TIME)
		&&(sync_data_rx->u.sync_start.flag==con_sync_start_flag))
	{
	//音乐播放后,第一次的同步处理
		uint32_t rx_start_time,dac_clk;
		rx_start_time = (uint32_t)sync_data_rx->u.sync_start.start_time;
		//sync_data.u.sync_send.bt_clk = sync_data_rx->u.sync_send.bt_clk;
		//audio_sync[0] = rx[0];
		bt_flag2_operate( APP_FLAG2_STEREO_SLAVE_GET_TIME, 1 );
		//app_bt_trig_dac(audio_sync[0]);
		dac_clk = (uint32_t)sync_data_rx->u.sync_start.clk_val;
		dac_set_clk(dac_clk);

		app_bt_trig_dac((uint32_t)rx_start_time);
		os_printf("dac_clk:0x%x,vol:%d\r\n",dac_clk,(uint32_t)sync_data_rx->u.sync_start.vol);

	}
	else
	{
	//音乐播放中的同步处理
	//根据master发送过来的 时刻 和 音频播放点数,结合本地读取的
	//	   时刻 和 音频播放点数,计算得出延时情况
		uint32_t rx_bt_clk,rx_aud_num;
		rx_bt_clk = (uint32_t)sync_data_rx->u.sync_send.bt_clk;
		rx_aud_num = (uint32_t)sync_data_rx->u.sync_send.aud_num;
		now = app_bt_read_CLKN();
		if(now<rx_bt_clk) now += 1<<28;
		if((BK3000_AUD_AUDIO_CONF & 0x3 ) == 2) //44.1K
			target_sync = make_clkn_to_dac_num_44100((now - rx_bt_clk));
		else //48K
			target_sync = make_clkn_to_dac_num_48000((now - rx_bt_clk));
		target_sync += rx_aud_num;
//			latency =audio_sync[1]-target_sync;
		latency =sync_data.u.sync_send.aud_num-target_sync;
//		os_printf("%d,%x,%x,%x,%x\r\n",latency,rx_bt_clk,rx_aud_num,now,sync_data.u.sync_send.aud_num);

		if((BK3000_AUD_AUDIO_CONF & 0x3 ) == 2) //44.1K
			target_sync = make_dac_num_to_time_44100(latency);
		else //48K
			target_sync = make_dac_num_to_time_48000(latency);

		dac_clk_adjust_tws(( int)sync_data_rx->u.sync_send.clk_mode);

		os_printf("%dms,%x,%d,0x%x\r\n",target_sync,latency,sync_data_rx->u.sync_send.clk_mode,BK3000_AUD_DAC_FRACCOEF);
	{
		static int8 sync_trace = 0;
		if(ABS(target_sync)==0)
		{
		#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
		if(sync_data_rx->u.sync_send.clk_mode == 9)
			dac_clk_adjust_tws(0);
		#endif
		}
		else if(ABS(target_sync)<=2)
		{
			if(target_sync>0)
			{
				dac_clk_adjust_tws(2);
				show_dac_clk(2);
			}
			else
			{
				dac_clk_adjust_tws(1);
				show_dac_clk(1);
			}
		}
		else if(ABS(target_sync)<=4)
		{
			if(target_sync>0)
			{
				dac_clk_adjust_tws(4);
				show_dac_clk(4);
			}
			else
			{
				dac_clk_adjust_tws(3);
				show_dac_clk(3);
			}
		}
		else if(ABS(target_sync)<=10)
		{
			if(target_sync>0)
			{
				dac_clk_adjust_tws(4);
				dac_clk_adjust_tws(4);
				show_dac_clk(4);
			}
			else
			{
				dac_clk_adjust_tws(3);
				dac_clk_adjust_tws(3);
				show_dac_clk(3);
			}
		}
		else if(ABS(target_sync)<=100)
		{
			stereo_mode_slave_latency = (int)latency;
			os_printf("big_step_adjust1\r\n");
		}
		else// if(ABS(target_sync)<=200)
		{
		       #define sync_trace_200 9
			if(sync_trace == (sync_trace_200-1))
			{
				stereo_mode_slave_latency = (int)latency;
				os_printf("big_step_adjust2\r\n");
			}
			else
			{
				os_printf("%d,%x,%x,%x,%x\r\n",latency,rx_bt_clk,rx_aud_num,now,sync_data.u.sync_send.aud_num);
			}
			sync_trace = sync_trace_200;
		}
//		else
//		{
//			os_printf("%d,%x,%x,%x,%x\r\n",latency,rx_bt_clk,rx_aud_num,now,sync_data.u.sync_send.aud_num);
//		}
		sync_trace -- ;
	}
//		if(target_sync>0)
//			os_printf("sF:%dms\r\n",target_sync);
//		else
//			os_printf("sS:%dms\r\n",target_sync);

//			stereo_mode_slave_latency = (int)latency;
//			bt_flag2_operate(APP_FLAG2_STEREO_SLAVE_ADJ_CLK,1);


	}
}


void app_bt_sync_send_timer_stop( void )
{
    app_handle_t app_h = app_get_sys_handler();
    jtask_stop( app_h->app_sync_task );
    os_printf("app_bt_sync_send_timer_stop\r\n");

}
t_clock HW_get_bt_clk(void);

uint32_t app_bt_read_CLKN( void )
{
//	uint32_t timer_val1,timer_val2,timer_delta;
//	do{
//		timer_val1 = HW_get_bt_clk();
//		timer_val2 = HW_get_bt_clk();
//	}while(timer_val2!=(timer_val1+1));
//	return timer_val2;
	return HW_get_bt_clk();
}
int get_sbc_mem_pool_Nodes(void);

void app_bt_trig_dac(uint32_t timer)
{
    uint32_t interval,now;

    //	STEREO_DEBUG_HIGH;
    now = app_bt_read_CLKN();
    if(timer < now)
    timer += 1<<28;
    interval = timer - now;
    timer_pt1_start(interval*10);
    //	aud_close();
    //	sbc_mem_pool_deinit();
    memset((void *)&sync_data, 0, sizeof( sync_data));
    os_printf("now:%.8x,target:%.8x,interval:%.8x\r\n",now,timer,interval);
//os_printf("dac:%.8x\n",BK3000_AUD_FIFO_CONF);
}

void app_stereo_master_start_play(void)
{
    bt_flag2_operate(APP_FLAG2_STEREO_PLAYING,0);
//		memset( audio_sync, 0, sizeof( audio_sync));
//		audio_sync[0] = con_audio_interval_xms(100) + app_bt_read_CLKN();
#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    if( bt_flag1_is_set( APP_FLAG_LINEIN ))
        sync_data.u.sync_start.start_time = con_audio_interval_xms(60) + app_bt_read_CLKN();
    else
#endif
        sync_data.u.sync_start.start_time = con_audio_interval_xms(100) + app_bt_read_CLKN();

    sync_data.u.sync_start.flag = con_sync_start_flag;
    sync_data.u.sync_start.clk_val = BK3000_AUD_DAC_FRACCOEF;
    sync_data.u.sync_start.vol = a2dp_get_volume();
//		show_dac_clk(2);

//		os_printf("app_stereo_master_start_play:%x\r\n",sync_data.u.sync_start.start_time);
    send_ct_cmd_test1(&sync_data,sizeof(struct sync_start_s));

//		send_ct_cmd_test1(&audio_sync[0],4);
//os_printf("now:%.8x,%.8x\n",pre_now,now);
//		app_bt_trig_dac(audio_sync[0]);
//	app_bt_trig_dac(audio_sync[0]);
    app_bt_trig_dac((uint32_t)sync_data.u.sync_start.start_time);

    if( bt_flag1_is_set( APP_FLAG_LINEIN ))
        bt_flag2_operate(APP_FLAG2_STEREO_LINEIN_TX, 1);
#ifdef CONFIG_LINE_IN_TX_LOOP
    else
#endif
        app_bt_sync_send_timer((void *)AUDIO_SYNC_INTVAL);
    bt_flag2_operate(APP_FLAG2_SYNC,0);

}


void hci_err_page_timeout_callback(void)
{
};

void app_bt_disconn_timerfunc( void *arg)
{
    app_handle_t app_h = app_get_sys_handler();
    int handle_type;
    handle_type = (int)arg;
    switch(handle_type)
    {
        case 0:
        #ifdef CONFIG_BLUETOOTH_AVRCP
            avrcp_cmd_disconnect();
        #endif
        #ifdef CONFIG_BLUETOOTH_A2DP
            a2dp_cmd_disconnect();
        #endif

        #ifdef CONFIG_BLUETOOTH_HFP
            hf_cmd_disconnect();
        #endif
            bt_unit_acl_disconnect(app_h->unit, &app_h->remote_btaddr);
            break;
            
        default:
            break;
    }

}
uint8 debug_trace;
void debug_set_trace_val(uint8 val)
{
	debug_trace = val;
}

int8 debug_show_trace(uint8 val)
{
    static uint8 show_cnt=0,last_val;
    if(debug_trace==val)
    {
        if(last_val!=val)
        {
            last_val = val;
            show_cnt = 0;
        }

        if(show_cnt<=10)
        {
            show_cnt++;
            os_printf("%d",val);
        }
        return 1;
    }
    else
    {
        show_cnt = 0;
        return 0;
    }
//		os_printf("trace:%d\r\n",val);
}
void debug_show_flag(void)
{
	app_handle_t sys_hdl = app_get_sys_handler();
	os_printf("flag1:0x%4x,flag2:0x%4x\r\n",sys_hdl->flag_sm1,sys_hdl->flag_sm2);
}

void app_tws_debug(uint8 *para)
{
	os_printf("tws_para:%d\r\n",para[0]);
	switch(para[0])
	{
		case 0:
			bt_flag2_operate(APP_FLAG2_STEREO_ROLE_MASTER, 0);
			app_button_stereo();
			break;
		case 1:
			bt_flag2_operate(APP_FLAG2_STEREO_ROLE_MASTER, 1);
			app_button_stereo();
			break;
		case 2:{//scan_enable
			app_handle_t sys_hdl = app_get_sys_handler();
                     bt_unit_set_scan_enable(sys_hdl->unit,para[1]);
			break;
			}
		case 3:{//show_sniff_status
		#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
			app_sniff_debug();
              #endif
			break;
			}
		case 4:{//set_gpio_status
			gpio_config(para[1],para[2]);
			break;
			}
		case 5:{//get_gpio_status
			int i;
			uint32 gpio_map=0;
			for(i=2;i<GPIO_NUM;i++)
			{
				if(gpio_input(i))
				{
				gpio_map |= 1<<i;
				}
			}
			os_printf("gpio:%0.4x\r\n",gpio_map);
			break;
			}
		case 6:{//get DL_amaddr_pool
			extern uint8 DL_amaddr_pool;
			os_printf("DL_amaddr_pool:%x\r\n",DL_amaddr_pool);
			break;
			}

		case 8:{//set trace val
			debug_set_trace_val(para[1]);
			break;
			}

		case 9:{//output flag
			debug_show_flag();
			break;
			}
	}
}
#endif

#if(POWERKEY_5S_PARING == 1)
void enter_match_state(void)
{
	app_button_match_action();
}
#endif

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
void app_start_encode(void)
{
    app_handle_t sys_hdl = app_get_sys_handler();
    bt_a2dp_codec_t codec;
    LTX_PRT("app_start_encode\r\n");
    //写入LINK KEY
//    app_env_write_action(&app_h->remote_btaddr);

    BK3000_set_clock(3, 1);
//	STEREO2_DEBUG(1);
//	BK3000_AUD_DAC_FRACCOEF = AUDIO_DIV_441K_SLOW2;


    linein_sbc_encode_init();
//	if( app_bt_is_flag_set( APP_FLAG_A2DP_CONNECTION ) )
    {
    	codec.type = A2DP_CODEC_SBC;
    	codec.u.sbc.octet0 = 0x21;
    	codec.u.sbc.octet1 = 0x15;
    	codec.u.sbc.min_bitpool = 2;
    	codec.u.sbc.max_bitpool = 32;//48;
    //		bt_a2dp_sink_get_default_sbc(&codec.u.sbc);
    	a2dp_src_cmd_reconfig(&codec);
    }

    jtask_schedule( sys_hdl->app_auto_con_task, 500, (jthread_func)a2dp_src_cmd_stream_start, NULL);
    jtask_schedule( sys_hdl->app_audio_task, 1000, (jthread_func)app_stereo_master_start_play, NULL);
//app_reset_task
//app_audio_task

}

void app_start_aux_tx(void *arg)
{
    app_handle_t sys_hdl = app_get_sys_handler();
        
    if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
        &&bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
    {
        app_start_encode();
        //BK3000_Ana_Line_enable(1);
        LTX_PRT("app_start_aux_tx 1\r\n");
    #ifdef CONFIG_LINE_IN_TX_LOOP
        BK3000_AUD_FIFO_CONF |=  ( 1 << sft_AUD_LOOP_ADC2DAC);
    #endif

        sdadc_enable(1);
        sbc_src_num_set(0);
    }
    else
    {
        LTX_PRT("try start again\n");
        jtask_schedule( sys_hdl->app_reset_task, (uint32_t)arg, (jthread_func)app_start_aux_tx,( void *)arg);
    }
}
#endif

uint8_t app_set_role_accept_con_req(hci_unit_t *unit, const btaddr_t *btaddr)
{
    if(app_check_bt_mode(BT_MODE_TWS|BT_MODE_DM_TWS))
    {
        if( btaddr_same(btaddr, app_bt_get_handle(2)) )
        {
            return HCI_ROLE_SLAVE;
        }
        else
            return HCI_ROLE_MASTER;
    }
    #if (BT_MODE==BT_MODE_1V2)
    ///else if(app_check_bt_mode(BT_MODE_1V2|BT_MODE_1V1)&&(BT_ONE2MULTIPLE_AS_SCATTERNET == 0))
	 else if(app_check_bt_mode(BT_MODE_1V2))
        {
            return HCI_ROLE_MASTER; // HCI_ROLE_MASTER;
        }
    #endif
        else if(app_check_bt_mode(BT_MODE_1V1))
		{
            if(app_env_check_Use_ext_PA())
            {
                if(unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)
    	    		return HCI_ROLE_MASTER;
    		    else
    	    		return HCI_ROLE_SLAVE;
            }
            else
            {
                #if (BT_ONE2MULTIPLE_AS_SCATTERNET == 0)
    			    return HCI_ROLE_MASTER; // HCI_ROLE_MASTER;
    			#else
    			    if(unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)
    	    			return HCI_ROLE_MASTER;
    				else
    	    			return HCI_ROLE_SLAVE;
    			#endif
            }
    }
    else
    {
        if(unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)
            return HCI_ROLE_MASTER;
        else
            return HCI_ROLE_SLAVE;
    }

}
// 1=accept_role_switch;
uint8_t app_set_role_creat_con_req(hci_unit_t *unit, const btaddr_t *btaddr)
{
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
#if (BT_MODE==BT_MODE_1V2)
    if(app_check_bt_mode(BT_MODE_1V2))
    {
        return 0; 
    }
    else 
#endif
    if(app_check_bt_mode(BT_MODE_1V1))
    {
        if(app_env_check_Use_ext_PA())
        {
            return (unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)?1:0;
        }
        else
        {
        #if (BT_ONE2MULTIPLE_AS_SCATTERNET == 0)
            return 0; 
        #else
            return (unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)?1:0;
        #endif
        }
    }
    else
        return (unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)?1:0;

#elif (BT_ONE2MULTIPLE_AS_SCATTERNET == 0)
#if defined(BT_ONE_TO_MULTIPLE)
    return 0;
#else
    return (unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)?1:0;
#endif
#else
    return (unit->hci_link_policy & HCI_LINK_POLICY_ENABLE_ROLE_SWITCH)?1:0;
#endif

}
uint8_t app_check_bt_mode(uint8 mode)
{
#if 0//(CONFIG_APP_TOOLKIT_5 == 1)
	app_env_handle_t  env_h = app_env_get_handle();
	return (env_h->env_cfg.feature.bt_mode)&mode;
#else
	return BT_MODE&mode;
#endif
}
uint8 get_bt_dev_priv_work_flag(void)
{
    uint32 hf_flag=0,a2dp_flag=0;
    uint8 i;
    for(i=0; i<BT_MAX_AG_COUNT; i++)
    {
        hf_flag = get_hf_priv_flag(i,APP_FLAG2_HFP_INCOMING|APP_FLAG_HFP_OUTGOING);
        a2dp_flag = get_a2dp_priv_flag(i,APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY);
        if(hf_flag && a2dp_flag)
            return 1;
    }
    return 0;
}

// change begin by cai.zhong 20190228 for audio data
int app_get_flow_ctrl_en(void)
{
#if 0
    if(app_get_mic_state()){
        return 1;
    }else{
	    return 0;
	}
#else
    return 0;
#endif
}
// change end by cai.zhong 20190228 for audio data

void show_bt_stack_status(void)
{
    os_printf("%s\r\n", __func__);

    os_printf("sizeof(struct m_hdr):%d, sizeof(struct mbuf):%d\r\n", sizeof(struct m_hdr), sizeof(struct mbuf));

    os_printf("BEKEN SW Compliled at %s, %s\r\n", __TIME__, __DATE__);

    app_handle_t app_h = app_get_sys_handler();

#ifdef BT_ONE_TO_MULTIPLE
    uint32 flag;
    uint8 i;
    os_printf("------------------------------------------\r\n");
    for(i=0; i<BT_MAX_AG_COUNT; i++)
    {
        flag = get_hf_priv_flag(i, 0xffffffff);
        os_printf("*    hfp-private flag: %d - %08x\r\n", i, flag);
        flag = get_a2dp_priv_flag(i, 0xffffffff);
        os_printf("*   a2dp-private flag: %d - %08x\r\n", i, flag);
    }
    os_printf("------------------------------------------\r\n");
#endif

    if(NULL == app_h->unit)
    {
        os_printf("Bluetooth device not initialized yet.\r\n");
        return;
    }
    os_printf("     HFP flag: %08x\r\n", get_hf_priv_flag(0,0xffffffff));
    os_printf("    a2dp flag: %08x\r\n", get_a2dp_priv_flag(0,0xffffffff));
    os_printf("Global Flag-1: %08x\r\n", app_h->flag_sm1);
    os_printf("Global Flag-2: %08x\r\n", app_h->flag_sm2);

#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    extern u_int8 syspwr_cpu_halt;
    extern int SYSpwr_Is_Available_Sleep_System(void);
    extern boolean USLCsleep_Is_Tabasco_Asleep(void);
    os_printf("Sleep is allowed:%d,%d,%d,%d,%d\r\n", app_is_not_auto_reconnection(),saradc_start,syspwr_cpu_halt,SYSpwr_Is_Available_Sleep_System(),USLCsleep_Is_Tabasco_Asleep());
    os_printf("CPU irq mask:%08x\r\n", get_spr(SPR_VICMR(0)));
#endif

    os_printf("Device addr: "BTADDR_FORMAT"\r\n", BTADDR(&(app_h->unit->hci_btaddr)) );

    if( app_h->flag_sm1 & APP_FLAG_ACL_CONNECTION )
        os_printf("Remote device addr: "BTADDR_FORMAT"\r\n", BTADDR(&(app_h->remote_btaddr)) );
    else
    {
        os_printf("Not connect to other device. 0x%x \r\n", app_h->flag_sm1);
        return;
    }

    os_printf("A2DP status: %s\r\n",
              (app_h->flag_sm1 & APP_FLAG_A2DP_CONNECTION )?"connected":"disconnected");

    if( app_h->flag_sm1 & APP_FLAG_MUSIC_PLAY )
        os_printf("Audio stream started now.\r\n");

    os_printf("AVRCP status: %s\r\n",
              (app_h->flag_sm1 & APP_FLAG_AVCRP_CONNECTION )?"connected":"disconnected");

    os_printf("HFP status: %s\r\n",
              (app_h->flag_sm1 & APP_FLAG_HFP_CONNECTION )?"connected":"disconnected");
}

#ifdef CONFIG_BLUETOOTH_COEXIST
void app_change_mode_flag_set(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_h->flag_exit_bt = 1;
}
void app_change_mode_flag_clear(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_h->flag_exit_bt = 0;
}
uint8 app_change_mode_flow_ctrl_en(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return app_h->flag_exit_bt;
}

uint8 app_coexist_is_not_bt_mode(void)
{
    app_handle_t app_h = app_get_sys_handler();
    return (app_change_mode_flow_ctrl_en()
                ||(SYS_WM_SD_MUSIC_MODE == app_h->sys_work_mode)
            #ifdef CONFIG_APP_USB
                ||(SYS_WM_UDISK_MUSIC_MODE == app_h->sys_work_mode)
            #endif
                ||(SYS_WM_LINEIN_MODE == app_h->sys_work_mode)
            #ifdef FM_ENABLE
                ||(SYS_WM_FM_MODE == app_h->sys_work_mode)
            #endif          
        );
}

void app_coexist_play_pause(uint8 flag) // 1:recover music play    0:stop music
{
    if(app_is_mp3_mode())
        app_coexist_mp3_player_play_pause(flag);
    else if(app_is_linein_mode())
        app_coexist_linein_play_pause(flag);
#ifdef FM_ENABLE
    else if(app_is_fm_mode())
        app_coexist_fm_play_pause(flag);
#endif
}
#else
uint8 app_coexist_is_not_bt_mode(void)
{
    return 0;
}
#endif

#ifdef BEKEN_SPP_OTA_3266
extern RAM_CODE void BK3266_ota_updating_spp_ms_Sched(void);  //SPP
#endif
void bt_mode_msg_handler(void)
{
    int ret;
    uint8 exit_flag = 0;
    MSG_T msg;
    app_handle_t app_h = app_get_sys_handler();
    os_printf("----enter bt mode-----\r\n");
    app_restore_volume(ENV_BT_VOL_INFO);
#ifdef CONFIG_BLUETOOTH_COEXIST
    app_change_mode_flag_set();
    if(!hci_get_acl_link_count(app_h->unit))
        bt_unit_set_scan_enable(app_h->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);
    else
        bt_unit_set_scan_enable( app_h->unit, HCI_NO_SCAN_ENABLE);
#endif

    if(pre_enter_mode_handle(app_h->sys_work_mode))
        return;
#ifdef CONFIG_BLUETOOTH_COEXIST
    app_change_mode_flag_clear();
#endif
    enter_work_mode();
    while(!exit_flag)
    {
        ret = msg_get(&msg);
        if(MSG_FAILURE_RET == ret)
            msg.id = MSG_NULL;
        else
        {
            switch(msg.id)
            {
                //////////////////////蓝牙按键短按////////////////////
                case MSG_KEY_PLAY:
                    app_button_playpause_action();
                    break;
                case MSG_KEY_MATCH:
                    app_button_match_action();
                    break;
                case MSG_KEY_NUM_REDIAL:
                	app_button_redial_last_number();
                	break;
                case MSG_KEY_VOICE_DIAL:
                    app_button_voice_dial_set();
                    break;
                case MSG_KEY_HFP_ACK:
                    app_button_hfack_action();
                    break;
                case MSG_KEY_HFP_REJECT:
                    app_button_reject_action();
                    break;
                case MSG_KEY_TRANSFER_TOGGLE:
                    app_button_hf_transfer_toggle();
                    break;
                case MSG_KEY_TWC_HOLD_ACCEPT:
                    app_button_twc_hold_accept();
                    break;
                case MSG_KEY_TWC_HUNG_ACCEPT:
                    app_button_twc_hung_accept();
                    break;
                case MSG_KEY_TWC_REJECT_HOLD:
                    app_button_twc_reject_hold();
                    break;
                case MSG_KEY_TWC_SWITCH_WAY:
                    #ifdef BT_ONE_TO_MULTIPLE 
                    app_button_eSCO_A_B_TWC_swap();
                    #endif
                    break;
                case MSG_KEY_2PHONE_SWITCH:
                    #ifdef BT_ONE_TO_MULTIPLE 
                    app_button_eSCO_A_B_swap();
                    #endif
                    break;
                case MSG_KEY_STEREO:
                    #ifdef CONFIG_TWS
                    app_button_stereo_action();
                    #endif
                    break;
                case MSG_KEY_CONN_DISCONN:
                    app_button_conn_disconn();
                    break;
                case MSG_KEY_NEXT:
                    //change begin by cai.zhong 20190419 for audio mix 
                    if(lp_pdd_set_avrcp_event(AVC_OP_NEXT) == LP_ERR_SKIP_EVENT)
                        app_button_next_action();
                    //change end by cai.zhong 20190419 for audio mix 
                    break;
                case MSG_KEY_PREV:
                    //change begin by cai.zhong 20190419 for audio mix 
                    if(lp_pdd_set_avrcp_event(AVC_OP_PREV) == LP_ERR_SKIP_EVENT)
                        app_button_prev_action();
                    //change end by cai.zhong 20190419 for audio mix 
                    break;
                case MSG_KEY_REWIND:
                    app_button_rewind_action();
                    break;
                case MSG_KEY_FORWARD:
                    app_button_forward_action();
                    break;
                case MSG_KEY_VOL_DOWN:
                    app_button_volm_action();
                    break;
                case MSG_KEY_VOL_UP:
                    app_button_volp_action();
                    break;
                case MSG_KEY_MIC_DOWN:
                    app_button_micvolm_action();
                    break;
                case MSG_KEY_MIC_UP:
                    app_button_micvolp_action();
                    break;
                case MSG_KEY_MIC_MUTE:
                    app_button_smartvoice_action();  // add by cai.zhong 20190221 for smartvoice
                    //app_button_micvol_mute_action(); // add by cai.zhong 20190221 for smartvoice
                    break;
                case MSG_KEY_STOP:
                    app_button_stop_action();
                    break;
                case MSG_CLEAR_BT_MEM:
                    app_button_clear_memory();
                    break;
                case MSG_KEY_TEST_MODE:
                    app_button_enter_dut_mode();
                    break;
                case MSG_HALF_SECOND:
                    break;
                default:
                    exit_flag = common_msg_handler(&msg);
                    break;
            }
        }
    #ifdef CONFIG_APP_AEC
	 if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AEC_ENABLE))
            app_aec_swi();
    #endif

        if(app_wave_playing())
            app_wave_file_play();
        else
            sbc_do_decode();
    #if 1
    //for customers  who want BAT_DISPLAY,without HFP
    	app_env_handle_t	env_h = app_env_get_handle();
    	if((!(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_HFP))
            &&(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_APP_BAT_DISPLAY)
            )
    	{
    		if((get_current_hfp_flag(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
    			&&(get_current_hfp_flag(APP_FLAG_SCO_CONNECTION)))
    		{
    			hf_sco_handle_process(1);
    		}
    	}
    #endif

        BTms_Sched(2);//controller的处理
        //Beken_Uart_Rx();//调试串口命令处理
        /*
        * if don't care current in idle, then modify to close analog DAC before sleep mode.
        * this may avoid 'pop',when voice/audio source switched.
        */
    // add begin by cai.zhong 20190321 for smartvoice
    #ifdef LP_MDD_PDD_SUPPORT
        LPHANDLE lpHandle = lp_get_mdd_handle();
        lp_mdd_event_process(lpHandle); // process mdd event list
        lp_pdd_event_process();  // process pdd event list
        lp_pdd_check_mic_data(); // check mic data
    #endif
    // add end by cai.zhong 20190321 for smartvoice
        
    #ifdef BEKEN_SPP_OTA_3266
        BK3266_ota_updating_spp_ms_Sched() ;
    #endif

    #if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 1)
        aud_ana_dac_close_in_idle();
    #endif

    #if (CONFIG_CPU_CLK_OPTIMIZATION == 1)
        if( !bt_flag1_is_set( APP_FLAG_WAVE_PLAYING ) )
            BK3000_set_clock(1, 0);
    #endif
        timer_clear_watch_dog();
    }
//wait WAV and BT disconnect end	
    uint64_t tmp =os_get_tick_counter();
    os_printf("====waiting...===\r\n");	
    while(1)
    {
    #ifdef CONFIG_BLUETOOTH_COEXIST
        BTms_Sched(2);//controller的处理
    #endif
        BT_msg_handler();
        app_wave_file_play();   
        if(app_wave_playing()
        #ifndef CONFIG_BLUETOOTH_COEXIST
            ||bt_flag1_is_set(APP_AUDIO_FLAG_SET)
        #endif
            )
        {
            if((os_get_tick_counter() - tmp) > 400)
            {
                break;
            }
        }
        else
        {
            if((os_get_tick_counter() - tmp) > 100)
            {
                break;
            }
        }
    }
#ifdef CONFIG_BLUETOOTH_COEXIST
    app_change_mode_flag_clear();
#endif
    memory_usage_show();
    os_printf("====exit BT mode===\r\n");
}

// EOF
