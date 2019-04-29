#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef BT_DUALMODE
extern volatile u_int8 le_mode; 
#endif

static app_env_t app_env;
static uint8 flag_auto_powerdown=0,app_env_cfg_lang_num = 0;
CONST char env_chip_magic[8] = {'B','K','3','2','6','6','0','1'};

app_env_t * app_env_get_handle( void )
{
    return &app_env;
}
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
extern void uart_send_ppp(unsigned char *buff, unsigned char fid,unsigned short len);
#endif


#ifdef BEKEN_DEBUG
void app_env_dump(void)
{
#if 0
    int i,j;
    char addr[14], linkkey[34];
    app_env_handle_t  env_h = app_env_get_handle();

    os_printf("env data:\r\n");
    for( i = 0; i < MAX_KEY_STORE; i++ )
    {
        for( j = 0; j < 6; j++ )
        {
            sprintf( &addr[j*2], "%02x", env_h->env_data.key_pair[i].btaddr.b[5-j] );
        }
        for( j=0; j < 16; j++)
            sprintf( &linkkey[j*2], "%02x", env_h->env_data.key_pair[i].linkkey[j] );
        addr[13] = 0;
        linkkey[33] = 0;
        //if( env_h->env_data.key_pair[i].used == 0x01 )
            os_printf("key_pair[%d] used: %02x, addr: 0x%s, linkkey: 0x%s\r\n",
                i, env_h->env_data.key_pair[i].used, addr, linkkey);
    }
    os_printf("key_index: %d, last btaddr: 0x%02x%02x%02x%02x%02x%02x\r\n",
        env_h->env_data.key_index, env_h->env_data.default_btaddr.b[5], env_h->env_data.default_btaddr.b[4],
        env_h->env_data.default_btaddr.b[3],env_h->env_data.default_btaddr.b[2],env_h->env_data.default_btaddr.b[1],
        env_h->env_data.default_btaddr.b[0] );
    os_printf("\r\nenv cfg:\r\n");
    os_printf("used: %d, bt flag: %x, system_flag: %x, disconn_action: %d,device_class, %d\r\n",
        env_h->env_cfg.used, env_h->env_cfg.bt_para.bt_flag, env_h->env_cfg.system_para.system_flag,
        env_h->env_cfg.bt_para.disconn_action, env_h->env_cfg.bt_para.device_class);
    os_printf("device_name: %s, device_pin: %s\r\n",
        env_h->env_cfg.bt_para.device_name, env_h->env_cfg.bt_para.device_pin);
    os_printf("device btaddr: 0x%02x%02x%02x%02x%02x%02x\r\n",
        env_h->env_cfg.bt_para.device_addr.b[5], env_h->env_cfg.bt_para.device_addr.b[4],
        env_h->env_cfg.bt_para.device_addr.b[3],env_h->env_cfg.bt_para.device_addr.b[2],
        env_h->env_cfg.bt_para.device_addr.b[1],env_h->env_cfg.bt_para.device_addr.b[0]);

    os_printf("auto connection interval: %d, auto connection count: %d\r\n",
        env_h->env_cfg.bt_para.auto_conn_interval,
        env_h->env_cfg.bt_para.auto_conn_count);
    os_printf("recovery start: %d, interval: %d, count: %d \r\n", env_h->env_cfg.bt_para.disconn_start,
        env_h->env_cfg.bt_para.disconn_retry_interval, env_h->env_cfg.bt_para.disconn_retry_count );
    os_printf("lowpower channel %d, threshold: %x, pd threshold: %x, interval: %d\r\n",
        env_h->env_cfg.system_para.lp_channel,
        env_h->env_cfg.system_para.lp_threshold,
        env_h->env_cfg.system_para.lp_pd_threshold,
        env_h->env_cfg.system_para.lp_interval);

    os_printf("a2dp vol: %d, mic vol: %d, hfp vol: %d, wave vol: %d, mute pin: %d, line in pin: %d\r\n",
        env_h->env_cfg.system_para.vol_a2dp, env_h->env_cfg.system_para.vol_mic,
        env_h->env_cfg.system_para.vol_hfp, env_h->env_cfg.system_para.vol_wave,
        env_h->env_cfg.system_para.pamute_pin, env_h->env_cfg.system_para.linein_pin);
    os_printf("sleep time out: %d, powerdown time out: %d\r\n",
        env_h->env_cfg.system_para.sleep_timeout,
        env_h->env_cfg.system_para.powerdown_timeout);
    os_printf("pd condition: %02x, match timeout: %d, action of disconn: %d\r\n",
        env_h->env_cfg.bt_para.pd_cond,
        env_h->env_cfg.bt_para.match_timeout, env_h->env_cfg.bt_para.action_disconn );

    os_printf("wave sel: %d\r\n", env_h->env_cfg.wave_lang_sel );
    for( i = 0; i < WAVE_EVENT; i++ )
    {
		os_printf("wave audio info[%d]: used: %x, page_index:%d\r\n",
			i, env_h->env_cfg.wave_info[i].used,
			env_h->env_cfg.wave_info[i].page_index );
    }

    for( i = 0; i < WAVE_EVENT; i++ )
    {
		os_printf("wave audio info1[%d]: used: %x, page_index:%d\r\n",
			i, env_h->env_cfg.wave_info1[i].used,
			env_h->env_cfg.wave_info1[i].page_index );
    }
    for( i = 0; i < WAVE_EVENT; i++ )
    {
		os_printf("wave audio info2[%d]: used: %x, page_index:%d\r\n",
			i, env_h->env_cfg.wave_info2[i].used,
			env_h->env_cfg.wave_info2[i].page_index );
    }
    for( i = 0; i < WAVE_EVENT; i++ )
    {
		os_printf("wave audio info3[%d]: used: %x, page_index:%d\r\n",
			i, env_h->env_cfg.wave_info3[i].used,
			env_h->env_cfg.wave_info3[i].page_index );
    }
    for( i = 0; i < WAVE_EVENT; i++ )
    {
    	os_printf("wave audio info4[%d]: used: %x, page_index:%d\r\n",
    		i, env_h->env_cfg.wave_info4[i].used,
    		env_h->env_cfg.wave_info4[i].page_index );
    }
    
    for( i = 0; i < LED_EVENT_END; i++ )
        os_printf("led info[%d]: index: %d, ontime: %d, offtime: %d, repeat:%d, num flash: %d, timeout: %d\r\n",
            i, env_h->env_cfg.led_info[i].index, env_h->env_cfg.led_info[i].led_ontime,
            env_h->env_cfg.led_info[i].led_offtime, env_h->env_cfg.led_info[i].repeat,
            env_h->env_cfg.led_info[i].number_flash, env_h->env_cfg.led_info[i].timeout );

    os_printf("button threshold: \r\npress: %d, repeat: %d, long: %d, very long: %d, double; %d\r\n",
        env_h->env_cfg.button_para.press, env_h->env_cfg.button_para.repeat,
        env_h->env_cfg.button_para.longp, env_h->env_cfg.button_para.vlongp,
        env_h->env_cfg.button_para.doublep);

    for( i = 0; i < BUTTON_BT_END; i++ )
        os_printf("button code[%d]: %x \r\n", i, env_h->env_cfg.button_code[i]);

    os_printf("wakup_pin:%d\r\n",env_h->env_cfg.system_para.wakup_pin);
		os_printf("pwrctrl_pin:%d\r\n",env_h->env_cfg.system_para.pwrctrl_pin);

    os_printf("mic_delay:%d fft_shift:%d decay_time:%d ear_gain:%d mic_gain:%d\r\n",
            env_h->env_cfg.env_aec_cfg.aec_mic_delay,
            env_h->env_cfg.env_aec_cfg.aec_fft_shift,
            env_h->env_cfg.env_aec_cfg.aec_decay_time,
            env_h->env_cfg.env_aec_cfg.aec_ear_gain,
            env_h->env_cfg.env_aec_cfg.aec_mic_gain);
    os_printf("feature_flag:0x%x BT_MODE: 0x%x,\r\ndig_buck: %d, ana_buck: %d, vol_mic_dig: %d, pa_mute_delay_time:%d,pa_unmute_delay_time:%d,vol_linein_ana=%d,vol_linein_dig=%d\r\n",
            env_h->env_cfg.feature.feature_flag,
            env_h->env_cfg.feature.bt_mode,
            env_h->env_cfg.feature.dig_buck,
            env_h->env_cfg.feature.ana_buck,
            env_h->env_cfg.feature.vol_mic_dig,
	env_h->env_cfg.feature.pa_mute_delay_time*10,
	env_h->env_cfg.feature.pa_unmute_delay_time*10,
	env_h->env_cfg.feature.vol_linein_ana,
	env_h->env_cfg.feature.vol_linein_dig);

    os_printf("a2dp:\r\n");
    for(i=0;i<17;i++)
        os_printf("vol[%d].ang:%d\t,dig:%d\r\n",i,env_h->env_cfg.feature.a2dp_vol.vol[i].ana_gain,env_h->env_cfg.feature.a2dp_vol.vol[i].dig_gain);
    os_printf("hfp_vol:\r\n");
    for(i=0;i<17;i++)
        os_printf("vol[%d].ang:%d\t,dig:%d\r\n",i,env_h->env_cfg.feature.hfp_vol.vol[i].ana_gain,env_h->env_cfg.feature.hfp_vol.vol[i].dig_gain);
    os_printf("linein_vol:\r\n");
    for(i=0;i<17;i++)
        os_printf("vol[%d].ang:%d\t,dig:%d\r\n",i,env_h->env_cfg.feature.linein_vol.vol[i].ana_gain,env_h->env_cfg.feature.linein_vol.vol[i].dig_gain);
#endif
}
#endif

void app_print_linkkey(void)
{
    int i,j;
    char addr[14]={0}, linkkey[34]={0};
    app_env_handle_t  env_h = app_env_get_handle();

    for (i = 0; i < MAX_KEY_STORE; i++)
    {
        for (j = 0; j < 6; j++)
            sprintf( &addr[j*2], "%02x", env_h->env_data.key_pair[i].btaddr.b[5-j] );
        for( j=0; j < 16; j++)
            sprintf( &linkkey[j*2], "%02x", env_h->env_data.key_pair[i].linkkey[j] );
        if( env_h->env_data.key_pair[i].used != 0xff)
            os_printf("key_pair[%d] used: %02x, addr: 0x%s, linkkey: 0x%s\r\n",
                i, env_h->env_data.key_pair[i].used, addr, linkkey);
    }

}
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
void app_print_linkkey_to_uart(void)
{
    int i;
    app_env_handle_t  env_h = app_env_get_handle();
    for (i = 0; i < MAX_KEY_STORE; i++)
    {
        if((env_h->env_data.key_pair[i].used == 0x01) || (env_h->env_data.key_pair[i].used == 0x02))
        {
            uart_send_ppp(env_h->env_data.key_pair[i].btaddr.b,0x10,6);
            uart_send_ppp(env_h->env_data.key_pair[i].linkkey,0x11,16);
        }
    }
}
#endif

#if (CONFIG_CHARGE_EN == 1)
//extern uint16 adc_list[][2];
//extern uint16 th_low_trk,th_high_trk,th_lc,th_cv,th_cv_trk,th_end;
#endif

static env_voltage_cali_data_t s_env_voltage_cali_data;
static env_saradc_cali_data_t  s_env_saradc_cali_data;
static env_charge_cali_data_t  s_env_charge_cali_data;
static env_aud_dc_offset_data_t s_env_audio_dc_offset_0;
static env_aud_dc_offset_data_t s_env_audio_dc_offset_1;
static env_aud_dc_offset_data_t s_env_audio_dc_offset_2;
static env_aud_dc_offset_data_t s_env_audio_dc_offset_3;
//static env_aud_dc_offset_data_t    s_env_aud_dc_offset;

//__inline app_get_env_voltage_cali_data(void)
//{
//}
__inline env_saradc_cali_data_t *  app_get_env_saradc_cali_data(void)
{
    return &s_env_saradc_cali_data;
}
__inline env_charge_cali_data_t * app_get_env_charge_cali_data(void)
{
    return &s_env_charge_cali_data;
}
__inline env_aud_dc_offset_data_t* app_get_env_dc_offset_cali(void)
{
	app_env_handle_t  env_h = app_env_get_handle();
	return (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_DAC_DIFFER) ? &s_env_audio_dc_offset_0 : &s_env_audio_dc_offset_1;
}

void app_env_cali_data_init(void)
{
    TLV_TYPE tlv_cali_data;
    boolean tlv_loop = TRUE;
 #ifdef BEKEN_UP_OTA_3266
    uint8 *env_cali_ptr = (uint8 *)FLASH_ENVDATA2_DEF_ADDR; /* ATE calibration data address */
 #else
     uint8 *env_cali_ptr = NULL; /* ATE calibration data address */
 #endif
    uint8 chip_magic[8];
    /* set default calibration data */
    s_env_saradc_cali_data.sar_adc_dc = 512;
    s_env_saradc_cali_data.sar_adc_4p2 = 920;    // 4.2v
    s_env_charge_cali_data.charger_icp = 0x15;
    s_env_charge_cali_data.charger_vcv = 0x15;
    s_env_charge_cali_data.charger_vlcf = 0x1c;
    s_env_voltage_cali_data.cali_ana_1p8v = 2;   
    s_env_voltage_cali_data.cali_dig_1p2v = 1;
 #ifndef BEKEN_UP_OTA_3266
    if (flash_get_4M_flag())
        env_cali_ptr = (uint8 *)FLASH_ENVDATA2_DEF_ADDR_4M;
    else
        env_cali_ptr = (uint8 *)FLASH_ENVDATA2_DEF_ADDR_8M;		
#endif
    flash_read_data(chip_magic, (uint32)env_cali_ptr, sizeof(env_chip_magic));
    if(os_memcmp(chip_magic,env_chip_magic,sizeof(env_chip_magic))) /* has no ATE calibration data */
    {
        os_printf("===set default calibration data\r\n");
    }
    else
    {
        os_printf("===get calibration data\r\n");
        env_cali_ptr += sizeof(env_chip_magic);

        while(tlv_loop)
        {
            flash_read_data((uint8 *)&tlv_cali_data, (uint32)env_cali_ptr, sizeof(TLV_TYPE));
            env_cali_ptr += sizeof(TLV_TYPE);
            switch(tlv_cali_data.type)
            {
                case TLV_TYPE_CALI_END:
                    tlv_loop = FALSE;
                    break;
                case TLV_TYPE_CALI_DC_OFFSET_DIFF_DISPGA:
                    flash_read_data((uint8 *)&s_env_audio_dc_offset_0, (uint32)env_cali_ptr, tlv_cali_data.len);
                    break;
                case TLV_TYPE_CALI_DC_OFFSET_SNGL_DISPGA:
                    flash_read_data((uint8 *)&s_env_audio_dc_offset_1, (uint32)env_cali_ptr, tlv_cali_data.len);
                    break;
                case TLV_TYPE_CALI_DC_OFFSET_DIFF_ENPGA:
                    flash_read_data((uint8 *)&s_env_audio_dc_offset_2, (uint32)env_cali_ptr, tlv_cali_data.len);
                    break;
                case TLV_TYPE_CALI_DC_OFFSET_SNGL_ENPGA:
                    flash_read_data((uint8 *)&s_env_audio_dc_offset_3, (uint32)env_cali_ptr, tlv_cali_data.len);
                    break;
                case TLV_TYPE_CALI_CHARGE:
                    flash_read_data((uint8 *)&s_env_charge_cali_data, (uint32)env_cali_ptr, tlv_cali_data.len);
                    //os_printf("charger cali:%d,%d,%d\r\n",s_env_charge_cali_data.charger_vlcf,s_env_charge_cali_data.charger_icp,s_env_charge_cali_data.charger_vcv);
                    break;
                case TLV_TYPE_CALI_SARADC:
                    flash_read_data((uint8 *)&s_env_saradc_cali_data, (uint32)env_cali_ptr, tlv_cali_data.len);
                    //os_printf("saradc:%d,%d\r\n",s_env_saradc_cali_data.sar_adc_dc,s_env_saradc_cali_data.sar_adc_4p2);
                    break;
                case TLV_TYPE_CALI_VOLTAGE:
                    flash_read_data((uint8 *)&s_env_voltage_cali_data, (uint32)env_cali_ptr, tlv_cali_data.len);
                    //os_printf("====ana_dig_voltage:%d,%d\r\n",s_env_voltage_cali_data.cali_ana_1p8v,s_env_voltage_cali_data.cali_dig_1p2v);
                    break;
                
                default:break;
                
            }
            env_cali_ptr += tlv_cali_data.len;
        }
        
    }
}
void app_env_init( void )
{
    #define BT_ADDR_NAME_LEN  38
    //uint32 a7_val;
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 btaddr_name[BT_ADDR_NAME_LEN];
	uint32 file_size;
    uint8 i, j;

    memset( env_h, 0, sizeof( app_env_t ));
    //flash_read_data((uint8 *)&env_h->env_data, (uint32)FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    app_flash_read_env_data();
#ifdef BEKEN_UP_OTA_3266
    memcpy((uint8 *)&env_h->env_cfg, (uint8 *)FLASH_ENVCFG_DEF_ADDR, sizeof(app_env_cfg_t));
    flash_read_data((uint8*)&btaddr_name,(uint32)FLASH_CHANGED_BT_ADDR,BT_ADDR_NAME_LEN);
#else
    flash_read_data((uint8 *)&file_size, (uint32)FLASH_ENVSIZE_DEF_ADDR, sizeof(file_size));
    //os_printf("file_size 0x%x\r\n",file_size);

    if (file_size == FLASH_ENVSIZE_4M_FLAG)
    {
        memcpy((uint8 *)&env_h->env_cfg, (uint8 *)FLASH_ENVCFG_DEF_ADDR_4M, sizeof(app_env_cfg_t));
        flash_read_data((uint8*)&btaddr_name,(uint32)FLASH_CHANGED_BT_ADDR_4M,BT_ADDR_NAME_LEN);
    }
    else
    {
        memcpy((uint8 *)&env_h->env_cfg, (uint8 *)FLASH_ENVCFG_DEF_ADDR_8M, sizeof(app_env_cfg_t));
        flash_read_data((uint8*)&btaddr_name,(uint32)FLASH_CHANGED_BT_ADDR_8M,BT_ADDR_NAME_LEN);
    }
#endif	
    app_env_cali_data_init();

    j = 0;
    for(i = 0; i < 6; i ++)
    {
        if(btaddr_name[i]!=0xff)
            j++;
    }
    //suppose the number of !0xff in BTaddress is greater than 1
    if(j > 1)
    {
        memcpy((uint8*)&env_h->env_cfg.bt_para.device_addr,&btaddr_name[0],6);
    }
	
    j = 0;
    for(i = 6; i < BT_ADDR_NAME_LEN; i ++) // 32
    {
        if(btaddr_name[i] != 0xff)
            j ++;
    }
    if(j > 2) //suppose the valid character number in BTname is greater than 2
    {
        memset((uint8*)&env_h->env_cfg.bt_para.device_name[0],0,32);
        memcpy((uint8*)&env_h->env_cfg.bt_para.device_name[0],&btaddr_name[6],j);
    }
    if(env_h->env_cfg.used == 1)
    {
        if (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_UARTDBG)
        {
        #if (CONFIG_DEBUG_PCM_TO_UART == 1)
            uart_initialise(UART_BAUDRATE_460800);
        #else
        #ifdef CONFIG_APP_DATA_CAPTURE
            app_aec_set_data_capture_enable(0);
            if(app_aec_get_data_capture_mode())
            uart_initialise(UART_BAUDRATE_460800);
            else
            uart_initialise(UART_BAUDRATE_115200);
        #else
            uart_initialise(UART_BAUDRATE_115200);
        #endif
        #endif
        }
    }
    os_printf("env_cfg.used = %d\r\n", env_h->env_cfg.used);
    if( env_h->env_cfg.used != 0x01 )
    {
        env_h->env_cfg.bt_para.bt_flag = APP_BT_FLAG_DEFAULT;
        env_h->env_cfg.bt_para.disconn_action = ENV_ACTION_DISCONNECT_CONN;//ENV_ACTION_DISCONNECT_NONE;
        env_h->env_cfg.bt_para.disconn_retry_count = APP_AUTO_RECONN_RETRY_TIMES;
        env_h->env_cfg.bt_para.disconn_retry_interval = APP_DISCONN_ACTION_TIMEOUT;
        env_h->env_cfg.bt_para.disconn_start = APP_DISCONN_ACTION_FIRST_DELAY;
        env_h->env_cfg.bt_para.match_timeout = -1;    // always try
        env_h->env_cfg.bt_para.pd_cond = 0x3;
        env_h->env_cfg.bt_para.action_disconn = 0;

        env_h->env_cfg.system_para.system_flag = APP_SYSTEM_FLAG_DEFAULT;                       //default auto conn enable and a2dp enable
        env_h->env_cfg.bt_para.auto_conn_count = APP_AUTO_CONN_RETRY_TIMES;
        env_h->env_cfg.bt_para.auto_conn_interval = APP_DISCONN_ACTION_TIMEOUT;

        env_h->env_cfg.system_para.pamute_pin = PAMUTE_GPIO_PIN;

        env_h->env_cfg.system_para.linein_pin = LINEIN_DEFAULT_GPIO;

        env_h->env_cfg.system_para.sleep_timeout = SLEEP_TICK_CHECK;
        env_h->env_cfg.system_para.lp_channel = 0;
        env_h->env_cfg.system_para.lp_threshold = SARADC_BELOW_THRESHOLD;
        env_h->env_cfg.system_para.lp_pd_threshold = SARADC_BELOW_THRESHOLD - 100;
        env_h->env_cfg.system_para.lp_interval = APP_LOWPOWER_DETECT_INTERVAL;

        env_h->env_cfg.system_para.vol_a2dp  = 10;
        env_h->env_cfg.system_para.vol_hfp     = 10;
        env_h->env_cfg.system_para.vol_wave  = 10;
        env_h->env_cfg.system_para.vol_mic    = 12;
        env_h->env_cfg.system_para.powerdown_timeout = POWER_DOWN_CHECK;

        env_h->env_cfg.button_para.press = BUTTON_PRESS_COUNT_THRE;
        env_h->env_cfg.button_para.repeat = BUTTON_PRESS_REPEAT_THRE;
        env_h->env_cfg.button_para.longp = BUTTON_LONG_PRESS_COUNT_THRE;
        env_h->env_cfg.button_para.vlongp = BUTTON_LONG_PRESS_COUNT_THRE2;
        env_h->env_cfg.button_para.doublep = BUTTON_DOUBLE_CLICK_WAIT_COUNT_THRE;
        for(i=0;i<BUTTON_BT_END;i++) //BUTTON_END
            env_h->env_cfg.button_code[i] = 0;

        env_h->env_cfg.feature.bt_mode = BT_MODE;
        env_h->env_cfg.feature.vol_mic_dig = 48;
        env_h->env_cfg.feature.a2dp_rf_pwr.big_adj = 0xA;
        env_h->env_cfg.feature.a2dp_rf_pwr.small_adj1 = 0xf;
        env_h->env_cfg.feature.a2dp_rf_pwr.small_adj2 = 0x0;
        env_h->env_cfg.feature.hfp_rf_pwr.big_adj = 0xA;
        env_h->env_cfg.feature.hfp_rf_pwr.small_adj1 = 0xf;
        env_h->env_cfg.feature.hfp_rf_pwr.small_adj2 = 0x0;
        env_h->env_cfg.feature.charge_timeout = (3*60*60*100); // default:3 hours
	 env_h->env_cfg.system_para.frq_offset = 0x1f;
    }
    env_h->env_cfg.bt_para.bt_flag |= APP_ENV_BT_FLAG_ADDR_POLL;

    if(env_h->env_cfg.bt_para.auto_conn_interval < APP_DISCONN_ACTION_TIMEOUT)
    	env_h->env_cfg.bt_para.auto_conn_interval = APP_DISCONN_ACTION_TIMEOUT;
    if(env_h->env_cfg.bt_para.disconn_retry_interval < APP_DISCONN_ACTION_TIMEOUT)
    	env_h->env_cfg.bt_para.disconn_retry_interval = APP_DISCONN_ACTION_TIMEOUT;

    os_printf("===ENV BT_Flag:%08x\r\n",env_h->env_cfg.bt_para.bt_flag);
    if( env_h->env_data.key_index > MAX_KEY_STORE )
    {
        //os_printf("First env index:%d\r\n",env_h->env_data.key_index);
        env_h->env_data.key_index = 0;
    }

    //if (!(env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_UARTDBG))
    //{
    //    gpio_config(0,5);
    //    gpio_config(1,5);
    //}
    
    aud_mute_init();
    aud_PAmute_oper(1);

    saradc_set_lowpower_para(env_h->env_cfg.system_para.lp_interval,
                                env_h->env_cfg.system_para.lp_threshold,
                                env_h->env_cfg.system_para.lp_pd_threshold );

    aud_mic_volume_set(env_h->env_cfg.system_para.vol_mic);

    BK3000_AUD_ADC_CONF0  = (((env_h->env_cfg.feature.vol_mic_dig&0x3f)<<18)|(BK3000_AUD_ADC_CONF0&(~(0x3f<<18))));
    app_env_rf_pwr_set(0);

    BK3000_XVR_REG_0x0D =XVR_analog_reg_save[13] = (XVR_analog_reg_save[13] &~0x3f)|(env_h->env_cfg.system_para.frq_offset&0x3f);
    //os_printf("freq_offset:%d\r\n",env_h->env_cfg.system_para.frq_offset);
#if (CONFIG_PRE_EQ == 1)
    extern aud_pre_equ_t aud_pre_eqe;
    extern SAMPLE_ALIGN aud_pre_equ_para_t tbl_eq_coef[];
    aud_pre_eqe.globle_gain = env_h->env_cfg.pre_eq_gain;
    aud_pre_eqe.online_flag = env_h->env_cfg.pre_eq_flag;
    aud_pre_eqe.totle_EQ = env_h->env_cfg.pre_eq_cnt;
    os_printf("preEQ CNT:%d,flag:%d,gain:%d",aud_pre_eqe.totle_EQ,aud_pre_eqe.online_flag,aud_pre_eqe.globle_gain);
    for(i=0;i<aud_pre_eqe.totle_EQ;i++)
    {
        tbl_eq_coef[i].a[0] = env_h->env_cfg.pre_eq_para[i].a[0];
        tbl_eq_coef[i].a[1] = env_h->env_cfg.pre_eq_para[i].a[1];
        tbl_eq_coef[i].b[0] = env_h->env_cfg.pre_eq_para[i].b[0];
        tbl_eq_coef[i].b[1] = env_h->env_cfg.pre_eq_para[i].b[1];
        tbl_eq_coef[i].b[2] = env_h->env_cfg.pre_eq_para[i].b[2];
    }
    for(i=aud_pre_eqe.totle_EQ;i<10;i++)
    {
        tbl_eq_coef[i].a[0] = 0;
        tbl_eq_coef[i].a[1] = 0;
        tbl_eq_coef[i].b[0] = 0x100000;
        tbl_eq_coef[i].b[1] = 0;
        tbl_eq_coef[i].b[2] = 0;
    }
    //extern void app_show_pre_eq(void);
    //app_show_pre_eq();

#endif
}

void app_env_post_init( void )
{
    app_env_handle_t  env_h = app_env_get_handle();


    os_printf("app_env_post_init\r\n");
#ifdef CONFIG_BLUETOOTH_A2DP
	#if (CONFIG_CUSTOMER_ENV_SAVE_VOLUME == 1)
	if((env_h->env_data.volume>AUDIO_VOLUME_MIN) && (env_h->env_data.volume<(AUDIO_VOLUME_MAX+1)))
		env_h->env_cfg.system_para.vol_a2dp = env_h->env_data.volume; 
	#endif		
    a2dp_volume_init( env_h->env_cfg.system_para.vol_a2dp );
#endif
#ifdef CONFIG_BLUETOOTH_HFP
    hf_volume_init( env_h->env_cfg.system_para.vol_hfp );
#ifdef CONFIG_APP_HALFDUPLEX
    app_hfp_agc_init( env_h->env_cfg.system_para.vol_mic );
    app_hfp_echo_cfg_ptr_set( &env_h->env_cfg.env_echo_cfg );
#endif
#endif
#ifdef CONFIG_APP_EQUANLIZER
    app_equalizer_a2dp_init( env_h->env_cfg.eq_para );
#endif
#if (CONFIG_DRC == 1)
	extern void app_set_drc_para(uint8 *para);
	app_set_drc_para(((uint8 *)&env_h->env_cfg.drc)-1);
#endif
    app_wave_file_volume_init( env_h->env_cfg.system_para.vol_wave);
    BK3000_set_ana_dig_voltage(s_env_voltage_cali_data.cali_ana_1p8v,s_env_voltage_cali_data.cali_dig_1p2v);
//#if (SYS_CFG_BUCK_ON == 0)
    if( env_h->env_cfg.used == 0x01 )
    {
        char enable;
		app_env_cfg_lang_num = env_h->env_cfg.wave_lang_sel; 
        enable = env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_BUCK_ENABLE ? 1:0; 
        if( env_h->env_data.lang_sel >= 0 && env_h->env_data.lang_sel <= 4 )
             env_h->env_cfg.wave_lang_sel = env_h->env_data.lang_sel;
        else
            env_h->env_data.lang_sel = env_h->env_cfg.wave_lang_sel;

    #ifdef BEKEN_DEBUG
        os_printf("buck:%d\r\n",enable);
    #endif
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        BK3000_Dig_Buck_open(!!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DIG_BUCK_ENABLE));
        BK3000_ana_buck_enable(!!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_ANA_BUCK_ENABLE));
    #elif (CHIP_PACKAGE_TSSOP_28 == 0)  // PACKAGE_TSSOP_28 has no buck;
        BK3000_Dig_Buck_open(enable);	//buck 1?±?μ?á÷?á′óê??àMA
        BK3000_ana_buck_enable(enable);
    #endif
    }
    else
    {
        // buck is determined by SYS_CFG_BUCK_ON,it has been setted at BK3000_analog_reg_init
        BK3000_Dig_Buck_open(SYS_CFG_BUCK_ON);        // buck 关闭电流会大十多MA
        BK3000_ana_buck_enable(SYS_CFG_BUCK_ON);      // BK3000_ana_buck_enable(1) 20151214
    }
//#endif
}

#if 0
int env_get_auto_conn_info_max_id(void)
{
    int i;
    uint32 id;
    app_env_handle_t env_h = app_env_get_handle();

    id = 0;
    for(i = 0; i < MAX_KEY_STORE; i++)
    {
        if((env_h->env_data.key_pair[i].used == 0x01)||(env_h->env_data.key_pair[i].used == 0x02))
        {
            id = i;
        }
    }

    return id;
}

int env_get_auto_conn_info(int start_index, int *id)
{
    int i;
    app_env_handle_t env_h = app_env_get_handle();

    if(start_index > MAX_KEY_STORE)
    {
        goto seek_over;
    }

    for(i = start_index; i < MAX_KEY_STORE; i++)
    {
        if((env_h->env_data.key_pair[i].used == 0x01) || (env_h->env_data.key_pair[i].used == 0x02))
        {
            *id = i;

            return 0;
        }
    }

seek_over:
    return -1;
}
#endif
int app_env_key_stored( btaddr_t *addr )
{
    int i;
    app_env_handle_t  env_h = app_env_get_handle();

    for( i = 0; i < MAX_KEY_STORE; i++ )
    {
        if((env_h->env_data.key_pair[i].used != 0xFF)
        && (btaddr_same( addr, &(env_h->env_data.key_pair[i].btaddr))) )
        return i+1;
    }
    return 0;
}
#ifdef CONFIG_ACTIVE_SSP
int app_env_key_delete( btaddr_t *addr )
{
    int i;    
    app_env_handle_t  env_h = app_env_get_handle();

    for( i = 0; i < MAX_KEY_STORE; i++ )
    {
        if((env_h->env_data.key_pair[i].used != 0xFF)
        && (btaddr_same( addr, &(env_h->env_data.key_pair[i].btaddr))) )
        {
            memset(env_h->env_data.key_pair[i].linkkey,0xff,HCI_KEY_SIZE);
            return 1;
        }
    }
    return 0;
}
#endif
int app_env_keypair_used_delete( btaddr_t *addr )
{
    int i;    
    app_env_handle_t  env_h = app_env_get_handle();

    for( i = 0; i < MAX_KEY_STORE; i++ )
    {
        if((env_h->env_data.key_pair[i].used != 0xFF)
        && (btaddr_same( addr, &(env_h->env_data.key_pair[i].btaddr))) )
        {
            env_h->env_data.key_pair[i].used = 0;
            return 1;
        }
    }
    return 0;
}

int app_get_env_key_num( void )
{
    int i,n=0;
    app_env_handle_t  env_h = app_env_get_handle();

    for( i = 0; i < MAX_KEY_STORE; i++ )
    {
        if((env_h->env_data.key_pair[i].used == 0x01) || (env_h->env_data.key_pair[i].used == 0x02))
            n++;
    }
    return n;
}

int app_get_active_linkey( int Seq )
{
    int i,n=0;
    app_env_handle_t  env_h = app_env_get_handle();
    if(Seq == 0)  // First active linkey
    {
	 for( i = 0; i < MAX_KEY_STORE; i++ )
	 {
	        if(env_h->env_data.key_pair[i].used == 0x01)
	        {
	            n = i+1;
		    break;
	        }
	 }
    }
    else  // Second active linkey
    {
        for( i = 0; i < MAX_KEY_STORE; i++ )
	 {
            if(env_h->env_data.key_pair[i].used == 0x02)
	        {
	            n = i+1;
                    break;
	        }
	 }
    }
    return n;
}


/* 0x02 or 0x01 keys will be auto-connected. */
/* For connection */
/* The latest connection key->used is always set to 0x02. If 0x02 or 0x01 already exsits, decrement them by 1.*/
/* For disconnection, if key->used is 0x01, no change; if key->used is 0x02, and key->used 0x01 exists,swap. */
int app_env_write_action( btaddr_t *raddr,uint8 connected)
{
    app_env_handle_t  env_h = app_env_get_handle();
    int index = 0;
    app_env_key_pair_t *keytmp;
#ifdef BT_DUALMODE
    u_int32 cpu_flags, mask;
#endif
#ifdef CONFIG_PRODUCT_TEST_INF
#ifdef CONFIG_APP_TOOLKIT_5
    if(env_h->env_cfg.feature.feature_flag & APP_ENV_FEATURE_FLAG_AUTCONN_TESTER)
    {
        if(btaddr_same(raddr, &env_h->env_cfg.feature.tester_bt_addr))
            return 1;
    }
#else
    if(btaddr_same(raddr, &remote_btaddr_def))
        return 1;
#endif
#endif

    //os_printf("app_env_write_action(%d)\r\n", connected);

    if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE))
    {
        if(0x01 == connected)
        {
            index = app_env_key_stored(raddr);

            if(index > 0)
            {
                int i = 0;
                for(i=0;(env_h->env_data.key_pair[i].used != 0xFF && i<MAX_KEY_STORE);i++)
                {
                    keytmp = &env_h->env_data.key_pair[i];
                    keytmp->used = 0x00;
                }
                keytmp = &env_h->env_data.key_pair[index-1];
                keytmp->used = 0x01;
            }
        }
    }
    memcpy( (uint8 *)&env_h->env_data.default_btaddr, (uint8 *)raddr, 6);
    /*
    flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
    flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    */
#ifdef BT_DUALMODE
    SYSirq_Disable_Interrupts_Save_Flags(&cpu_flags, &mask);
    if(le_mode)   
    {
        /*if BLE mode, we delay flash action, avoid LE interrupt be cleared forcibly, Yangyang 2018/01/05*/
        MSG_T msg;
        msg.id = MSG_ENV_WRITE_ACTION;
        msg.arg = (uint32)connected;
        msg.hdl = 0;
        msg_lush_put(&msg);
        SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
        return 0;
    }
#endif

    app_flash_write_env_data();
#ifdef BT_DUALMODE
    SYSirq_Interrupts_Restore_Flags(cpu_flags, mask);
#endif
#if ((BT_MODE==BT_MODE_1V2)||(BT_MODE==BT_MODE_TWS)||(BT_MODE==BT_MODE_DM_TWS))
    if(app_check_bt_mode(BT_MODE_1V2|BT_MODE_TWS|BT_MODE_DM_TWS))
    {
        int index1, index2;
        index1= app_get_active_linkey(0);
        index2 = app_get_active_linkey(1);
        index = app_env_key_stored(raddr);
        os_printf("+++app_env_write_action(%d),%d,%d,%d\r\n",connected,index,index1,index2);
        //memcpy( (uint8 *)&env_h->env_data.default_btaddr, (uint8 *)raddr, 6);
        if (0x01 == connected)
        {
            // No need to update env data
            if (index == index2)
                return 0;

            if (index2)
            {
                env_h->env_data.key_pair[index2-1].used = 0x01;

                if (index1)
                {
                    env_h->env_data.key_pair[index1-1].used = 0x00;
                }
            }

            if(index > 0)
            {
    	        keytmp = &env_h->env_data.key_pair[index-1];
    	        keytmp->used = 0x02;//connected;
            }
            else if(index == 0)
            {
                 os_printf("++app_env_write_action,can't find the addr in key list\r\n");
            }
        }
        else if(0 == connected)
        {
            if ((index == index1) || !index1)
                return 0;

            if(index == index2)
            {
                if(hci_check_acl_link(&env_h->env_data.key_pair[index1-1].btaddr))
                {
                    env_h->env_data.key_pair[index2-1].used = 0x01;
                    env_h->env_data.key_pair[index1-1].used = 0x02;
                }
                else
                {
                    return 0;
                }
            }
            /*
            // one situation: new phone connect BK, get linkey,but disconnected. need store linkey
            else
            {
                return 0;
            }*/
        }
        else
            return 1;
    }
#endif
//    os_printf("+++app_env_write_action\r\n");
    //memcpy( (uint8 *)&env_h->env_data.default_btaddr, (uint8 *)raddr, 6);
#if 0
    flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
    flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));		
#endif
    return 1;
}
int app_env_write_flash(uint8 connected)
{
    //app_env_handle_t  env_h = app_env_get_handle();
    MSG_T msg;
    // No Flash Write if POWER DOWN is happening, otherwise the Flash Content may get corrupted
    if(bt_flag1_is_set(APP_FLAG_POWERDOWN))
        return 0;
    if(bt_flag1_is_set(APP_AUDIO_WORKING_FLAG_SET))
    {
        os_printf("222MSG_ENV_WRITE_ACTION\r\n");
        msg.id = MSG_ENV_WRITE_ACTION;
        msg.arg = connected;
        msg.hdl = 0;
        msg_lush_put(&msg);
        return 0;
    }
    os_printf("app_env_write_flash(%d)\r\n",connected);
    //flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
    //flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    app_flash_write_env_data();
    return 1;
}
/* 
 * env data sector len: 4096 = 16 * ENV_BLOCK_LEN,block len = 256 > sizeof(env_data) 
 * Tag:
 *    0x5a: this block is valid env data;
 *    0x00: the block is invalid env data;
 *    0xff: the block has not been used;
 */
void app_flash_read_env_data(void)
{
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 i;
    /* searching valid env_data and read out */
    for(i=0;i<8;i++) // 16
    {
        flash_read_data((uint8 *)&env_h->env_data, (uint32)FLASH_ENVDATA_DEF_ADDR + i*ENV_BLOCK_LEN, sizeof(app_env_data_t));
        if(env_h->env_data.env_tag == 0x5a || env_h->env_data.env_tag == 0xff)  // valid env data or have not stored evv_data
        {
            break;
        }
        else
        {
            memset((uint8 *)&env_h->env_data,0xff,sizeof(app_env_data_t));
        }
    }
}
void app_flash_write_env_data(void)
{
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 buff[32] = {0};
    uint8 i;
    /* searching valid space for save this env data */
    for(i=0; i<8; i++) // 16
    {
        flash_read_data(buff, (uint32)FLASH_ENVDATA_DEF_ADDR + i*ENV_BLOCK_LEN, 32);
        if(buff[0] == 0x5a)  // this block has been used 
        {
            buff[0] = 0;
            flash_write_data(buff,(uint32)FLASH_ENVDATA_DEF_ADDR + i*ENV_BLOCK_LEN,1); // write 1st byte '0' to override this tag
        }
        else if(buff[0] == 0xff) // this block has been erased and not been used;
        {
            break;
        }
    }
    if(i == 8) // 16 /* have not searched valid space,then erase this sector and write env data to 1st block; */
    {
    
        flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
        env_h->env_data.env_tag = 0x5a;
        flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    }
    else /* write env data to the i block */
    {
        env_h->env_data.env_tag = 0x5a;
        flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR + i*ENV_BLOCK_LEN, sizeof(app_env_data_t));
    }
}


int app_env_store_autoconn_info( btaddr_t * remote_addr,  uint8 link_key[16] )
{
    int ret = 0,i;
    uint8 cmd[24];
    app_handle_t sys_hdl = app_get_sys_handler();     
    app_env_handle_t  env_h = app_env_get_handle();
    app_env_key_pair_t *key;
    app_env_key_pair_t *keytmp;

    if(env_h->env_data.key_index>MAX_KEY_STORE)  // if function app_env_clear_all_key_info is called
        env_h->env_data.key_index = 0;
    
    key =  &env_h->env_data.key_pair[env_h->env_data.key_index];

    i = app_env_key_stored( remote_addr );
    if( i > 0 )
    {
        key = &env_h->env_data.key_pair[i-1];
        if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE))
        {
            while(i--)
            {
                keytmp = &env_h->env_data.key_pair[i];
                keytmp->used = 0;
            }
        }
    }
    else
    {
        btaddr_t addr;
        memset((uint8 *)&addr,0xff,6);

        if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE))
        {
            i = env_h->env_data.key_index;
            while(i--)
            {
                keytmp = &env_h->env_data.key_pair[i];
                keytmp->used = 0;
            }
        }
                
        i = env_h->env_data.key_index;
        if(!btaddr_same( &addr, &(env_h->env_data.key_pair[i].btaddr)))
        {  
            os_printf("send delete cmd,index:%d\r\n",i);
            memcpy( &cmd[0], (uint8 *)&(env_h->env_data.key_pair[i].btaddr), sizeof(btaddr_t));
            cmd[6] = 0x00;
            hci_send_cmd( sys_hdl->unit, HCI_CMD_DELETE_STORED_LINK_KEY,cmd, 7 );
        }        
        env_h->env_data.key_index ++;
        if( env_h->env_data.key_index >= MAX_KEY_STORE )
        {
            env_h->env_data.key_index = 0;
        }
        
    }
    memcpy( (uint8 *)&key->linkkey, link_key, 16 );
    memcpy( (uint8 *)&key->btaddr, (uint8 *)remote_addr, 6 );
	key->crystal_cali_data = XVR_analog_reg_save[13] & 0x3f;
    
    cmd[0] = 1;
    memcpy(&cmd[1], (uint8 *)remote_addr, sizeof(btaddr_t));
    memcpy(&cmd[7], (uint8 *)link_key, 16);

    hci_send_cmd(sys_hdl->unit,
                    HCI_CMD_WRITE_STORED_LINK_KEY,
                    cmd,
                    23);


    if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE))
        key->used = 0x01;
    else
        key->used = 0;

    os_printf("key stored,env key index:%d\r\n",env_h->env_data.key_index);

#ifdef CONFIG_ACTIVE_SSP
    MSG_T msg;
    msg.id = MSG_ENV_WRITE_ACTION;
    msg.arg = 0x01;
    msg.hdl = 0;
    msg_lush_put(&msg);
#endif

    return ret;
}


btaddr_t* app_env_get_key_free( void )
{
    app_env_handle_t  env_h = app_env_get_handle();
    app_env_key_pair_t *key = &env_h->env_data.key_pair[env_h->env_data.key_index];

    if( key->used == 0x01 || key->used == 0x02)
        return &(key->btaddr);
    else
        return NULL;
}

void app_env_unit_info_init( char *name, uint8 *dev_class, char *pin )
{
    app_env_handle_t env_h = app_env_get_handle();

    if( env_h->env_cfg.used == 0x01 )
    {
        memcpy( name, (char *)env_h->env_cfg.bt_para.device_name, 32);
        memcpy( pin, (char *)env_h->env_cfg.bt_para.device_pin, 16 );
        *dev_class = env_h->env_cfg.bt_para.device_class;
    }

    return;

}

btkey_t * app_env_get_linkkey( btaddr_t *addr )
{
    app_env_handle_t env_h = app_env_get_handle();
    int i;
    btkey_t key;

    memset(key,0xff,HCI_KEY_SIZE);

    i = app_env_key_stored(addr);

    if( i != 0 )
    {
        if(memcmp(env_h->env_data.key_pair[i-1].linkkey,key,HCI_KEY_SIZE))
            return (btkey_t *)env_h->env_data.key_pair[i-1].linkkey;
        else
        {
            return NULL;
        }
    }
    else
       return NULL;
}

void app_env_clear_key_info( btaddr_t *addr )
{
    app_env_handle_t env_h = app_env_get_handle();
    int i;

    i = app_env_key_stored(addr);

    if( i != 0 )
    {
        env_h->env_data.key_pair[i-1].used = 0;
    }

    return;
}

void app_env_clear_all_key_info( void )
{
    app_env_handle_t env_h = app_env_get_handle();

    memset( (uint8 *)&env_h->env_data, 0xff, sizeof( env_h->env_data ));
	env_h->env_data.lang_sel = app_env_cfg_lang_num;

    /*
    flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
    flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    */
    app_flash_write_env_data();
    return;
}

static app_wave_info_t *app_env_get_wave_lang_handle( int lang, int wave_id )
{
    app_wave_info_t *wave_h;
    app_env_handle_t env_h = app_env_get_handle();

    switch( lang )
    {
        case 0:
            wave_h = &env_h->env_cfg.wave_info[wave_id];
            break;
        case 1:
            wave_h = &env_h->env_cfg.wave_info1[wave_id];
            break;
        case 2:
            wave_h = &env_h->env_cfg.wave_info2[wave_id];
            break;
        case 3:
            wave_h = &env_h->env_cfg.wave_info3[wave_id];
            break;
        case 4:
            wave_h = &env_h->env_cfg.wave_info4[wave_id];
            break;
        default:
            wave_h = NULL;
            break;
    }

    return wave_h;
}

int app_env_get_wave_page_index( int wave_id )
{
    app_env_handle_t env_h = app_env_get_handle();
    app_wave_info_t *wave_h;

    wave_h = app_env_get_wave_lang_handle( env_h->env_cfg.wave_lang_sel, wave_id );

    if( wave_h == NULL )
        return -1;
#ifdef CONFIG_SBC_PROMPT
    if((wave_h->used < INTER_WAV) ||(wave_h->used > EXT_SBC))
        return -1;
#else
    if(wave_h->used != 0x01)
        return -1;
#endif
    return wave_h->page_index;
}

int app_env_get_wave_type( int wave_id )
{
    app_env_handle_t env_h = app_env_get_handle();
    app_wave_info_t *wave_h;
//wave_h->type = wave_h->used + 1;
    wave_h = app_env_get_wave_lang_handle( env_h->env_cfg.wave_lang_sel, wave_id );

    if( wave_h == NULL )
        return -1;
#ifdef	CONFIG_SBC_PROMPT
    if((wave_h->used < INTER_WAV+1)
        ||(wave_h->used > EXT_SBC+1))
        return -1;
    return wave_h->used - 1;
#else
    if( (wave_h->used != 0x01) )
        return -1;

    return 0;
#endif

}

uint8 app_env_get_wave_max_lang_num(void)
{
	uint8 i,j,lang,num;
	static uint8 get_lang=0;
	static uint8 max_lang=0;
	app_env_handle_t env_h = app_env_get_handle();

	if (get_lang == 0)
	{
		get_lang = 1;
		lang = env_h->env_cfg.wave_lang_sel; 
		for (i=0; i<4; i++)
		{
			num = 0;
			for (j=0; j<INVALID_WAVE_EVENT; j++)
			{
				env_h->env_cfg.wave_lang_sel  = i;
				if ((app_env_get_wave_page_index(j)>0)
					&& (app_env_get_wave_type(j) != -1)) 
					num ++;	
			}
			if (num)
				max_lang ++;
		}
		env_h->env_cfg.wave_lang_sel = lang;
	}

	return max_lang;
}

void app_set_auto_powerdown(void)
{
	flag_auto_powerdown = 1;
}

uint8 app_get_auto_powerdown(void)
{
	return flag_auto_powerdown;
}

void app_env_power_on_check(void)
{
    app_env_handle_t  env_h = app_env_get_handle();
    if ( env_h->env_cfg.used == 0x01)
    {
/*
		if (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_UARTDBG)
		{
	#if (CONFIG_DEBUG_PCM_TO_UART == 1)
			uart_initialise(UART_BAUDRATE_460800);
	#else
		#ifdef CONFIG_APP_DATA_CAPTURE
			app_aec_set_data_capture_enable(0);
			if(app_aec_get_data_capture_mode())
				uart_initialise(UART_BAUDRATE_460800);
			else
				uart_initialise(UART_BAUDRATE_115200);
		#else
			uart_initialise(UART_BAUDRATE_115200);
		#endif
	#endif
		}
*/
//	app_env_dump();
	uint16 volt = sdradc_transf_adc_to_vol(saradc_get_value());
	//uint16 lp_volt = env_h->env_cfg.system_para.lp_threshold;
	uint16 pd_volt = env_h->env_cfg.system_para.lp_pd_threshold;
		if((volt<pd_volt)
			|| (app_linein_powerup_check()
			&&app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_AUX_MODE_PWR_DOWN)))
		{
		    app_powerdown_action();
              }

		if((SW_PWR_KEY==app_env_check_pwrCtrl_mode())
			||(SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())
			||(SW_PWR_KEY_SWITCH==app_env_check_pwrCtrl_mode())
			)
        {
            uint8 wakup_pin = env_h->env_cfg.system_para.wakup_pin + GPIO0;

            Delay(100);
        #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
            uint8 key_valid=1;
            uint8 pwrctrl_pin = env_h->env_cfg.system_para.pwrctrl_pin + GPIO0;
            uint8 high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH) ? 1:0 ;

            //if(!app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_PAIRING_AFTER_PWR_ON))
			{
				switch(app_env_check_pwrCtrl_mode())
				{
					case SW_PWR_KEY:
						if(DEFAULT_WAKEUP_PIN != wakup_pin)
						    key_valid = 0; //only gpio22 default = 0,active = 1;
						break;
					case SW_PWR_KEY_SWITCH:
						if(high_flag)
						    gpio_config( pwrctrl_pin, 0 );
						else
						    gpio_config( pwrctrl_pin, 3 );
						break;
					case SW_PWR_KEY_MOS_CTRL:
						//if(DEFAULT_WAKEUP_PIN != wakup_pin)
						//	key_valid = 0;
						break;
					case HW_PWR_KEY:
						default:
						break;

                }

                uint32 t0,t1;
                t0 = os_get_tick_counter();
                while(1)
                {
                    t1 = os_get_tick_counter();
                    if( (t1 - t0) <=  env_h->env_cfg.feature.sw_pwr_on_key_time)// 1s开机
                    {

						if(((SW_PWR_KEY==app_env_check_pwrCtrl_mode())&&( gpio_input(wakup_pin) != key_valid))
							||((SW_PWR_KEY_SWITCH==app_env_check_pwrCtrl_mode())&&(gpio_input(pwrctrl_pin)!= high_flag))
							//||((SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())&&(gpio_input(wakup_pin)!= high_flag))			
							||(get_Charge_state()&&app_env_check_Charge_Mode_PwrDown())
							)
						{
						//os_printf("pwrMode:%d,gpio(%d)=%d,key_valid:%d..gpio(%d)=%d,high_flag:%d\r\n",app_env_check_pwrCtrl_mode(),
						//	wakup_pin,gpio_input(wakup_pin),key_valid,pwrctrl_pin,gpio_input(pwrctrl_pin),high_flag);
						#if ((CONFIG_CHARGE_EN == 1))
							if ((GPIO_CHARGER_FLAG))
							{
								bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,1);
								if(!app_env_check_Charge_Mode_PwrDown())
									flag_power_charge = 1;
								break;
								#if 0
                                if(app_env_check_Charge_Mode_PwrDown())
                                {
                                    bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,1);
                                    break;
                                }
                                else
                                {
                                    extern void app_battery_charge_process(void);
                                    app_battery_charge_process();
                                    CLEAR_WDT;
                                    Delay(5000);
                                    os_printf("dummy sleep in charge mode\r\n");
                                    t0 = os_get_tick_counter();
                                }
								#endif
                            }
                            else
                        #endif
                            {
                                os_printf("ENTER_DEEP_SLEEP\r\n");
                                app_powerdown_action();
                            }
                        }
                        else
                        {
                            Delay(1000);
                            BK3000_start_wdt(0xFFFF);
                        }
                    }
                    else
                    {
                        break;
                    }
				}
				if(SW_PWR_KEY_MOS_CTRL==app_env_check_pwrCtrl_mode())
				{
					uint8 pwrctrl_pin = env_h->env_cfg.system_para.pwrctrl_pin;
					gpio_config( pwrctrl_pin, 1 );
					if (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH)
						gpio_output(pwrctrl_pin, 1);
					else
						gpio_output(pwrctrl_pin, 0);
				}

			}
#endif

#if 0
		gpio_config( pwrctrl_pin, 1 );
		if (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH)
			gpio_output(pwrctrl_pin, 1);
		else
			gpio_output(pwrctrl_pin, 0);
#endif
		}
	}
	else
	{
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
		uart_initialise(UART_BAUDRATE_460800);//UART_BAUDRATE_460800
#else
		uart_initialise(UART_BAUDRATE_115200);
#endif
	}

}

#ifdef CONFIG_TWS
int app_get_env_stereo_key_used(void)
{
	app_env_handle_t  env_h = app_env_get_handle();

	if(env_h->env_data.stereo_key.used == 0x01)
		return 1;
	else
		return 0;
}

void app_env_set_stereo_info( btaddr_t *saddr, uint8 link_key[16] )
{
    app_env_handle_t env_h = app_env_get_handle();
    app_env_key_pair_t *key = &env_h->env_data.stereo_key;

    if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ))
        env_h->env_data.stereo_role = 1;
    else
        env_h->env_data.stereo_role = 0;

    key->used = 1;
    memcpy( (uint8 *)&key->linkkey, link_key, 16 );
    memcpy( (uint8 *)&key->btaddr, (uint8 *)saddr, 6 );

    return;
}

void app_env_update_stereo_role_info( uint8 role )
{
    app_env_handle_t env_h = app_env_get_handle();

    env_h->env_data.stereo_role = role;
}
#endif

#if 1//(CONFIG_APP_MP3PLAYER == 1)
#ifdef FM_ENABLE 		
void app_env_fm_wr_flash(void *arg)
{
    app_env_handle_t  env_h = app_env_get_handle();
    //flash_erase_sector( FLASH_ENVDATA_DEF_ADDR );
    //flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
    app_flash_write_env_data();
}
void app_env_fm_restore(void* buf)
{
    //app_env_handle_t env_h = app_env_get_handle();
    //flash_read_data((uint8 *)&env_h->env_data, (uint32)FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));
}
#endif

void app_evn_wr_flash(void* arg)
{
    /*app_env_handle_t  env_h = app_env_get_handle();
    flash_erase_sector(FLASH_ENVDATA_DEF_ADDR);
    flash_write_data( (uint8 *)&(env_h->env_data), FLASH_ENVDATA_DEF_ADDR, sizeof(app_env_data_t));	
    */
    app_flash_write_env_data();
}

void app_env_restore(void *arg,uint8 item)
{
    app_env_handle_t env_h = app_env_get_handle();
    app_mp3_backup_t *tmp;
    os_printf("app_env_restore---  item:%d\r\n",item);

    switch(item)
    {
    #if (CONFIG_APP_MP3PLAYER == 1)
        case ENV_SD_INFO:
    //			os_printf("testbuf[0]=%x,testbuf[end]=%x\r\n",testbuf[0],testbuf[3*1024-1]);
            tmp = &(env_h->env_data.sd_card);
            memcpy(arg,(void*)tmp,sizeof(app_mp3_backup_t));
            break;
        case ENV_UDISK_INFO:
            tmp = &(env_h->env_data.udisk);
            memcpy(arg,(void*)tmp,sizeof(app_mp3_backup_t));
            break;
    #endif    				
    	 case ENV_BT_VOL_INFO:
            player_vol_bt = env_h->env_data.bt_vol;
            //os_printf("111111:%d\r\n",player_vol_bt);
            player_vol_hfp = env_h->env_data.hfp_vol;
            os_printf("env_bt  vol:%x,%x\r\n",player_vol_bt,env_h->env_data.bt_vol);
            if(player_vol_bt >AUDIO_VOLUME_MAX)
                player_vol_bt= env_h->env_cfg.system_para.vol_a2dp;//PLAYER_VOL_MEDIUM;
            if(player_vol_hfp >AUDIO_VOLUME_MAX)
                player_vol_hfp= env_h->env_cfg.system_para.vol_hfp;//PLAYER_VOL_MEDIUM_HFP;			
            break;	
    #if (CONFIG_APP_MP3PLAYER == 1)
        case ENV_MUSIC_VOL_INFO:
    #ifdef FM_ENABLE 		
    	 case ENV_FM_VOL_INF:
    #endif
    #endif
        case ENV_LINE_VOL_INFO:
            player_vol_bt = env_h->env_data.bt_vol;
            //os_printf("222222:%d\r\n",player_vol_bt);
            os_printf("env_line/music vol:%x,%x\r\n",player_vol_bt,env_h->env_data.hfp_vol);
            if(player_vol_bt >AUDIO_VOLUME_MAX)
                player_vol_bt= env_h->env_cfg.system_para.vol_a2dp;//PLAYER_VOL_MEDIUM;
            if(player_vol_hfp >AUDIO_VOLUME_MAX)
                player_vol_hfp= env_h->env_cfg.system_para.vol_hfp;//PLAYER_VOL_MEDIUM_HFP;	
            break;
    /*			
    	case ENV_FM_VOL_INF:
    		player_vol_fm = env_h->env_data.fm_vol;
    		if(player_vol_fm > AUDIO_VOLUME_MAX)
    			player_vol_fm=PLAYER_VOL_MEDIUM;
    		break;*/
        default:
            break;
    }
}

void app_env_backup(void *arg,uint8 item)
{
    app_env_handle_t env_h = app_env_get_handle();
    app_mp3_backup_t *tmp;

    switch(item)
    {
    #if (CONFIG_APP_MP3PLAYER == 1)
        case ENV_SD_INFO:
            //os_printf("333333:%d\r\n",player_vol_bt);
            env_h->env_data.bt_vol=player_vol_bt;
            tmp = &(env_h->env_data.sd_card);
            memcpy((void*)tmp,arg,sizeof(app_mp3_backup_t));
            break;
        case ENV_UDISK_INFO:
            //os_printf("444444:%d\r\n",player_vol_bt);
            env_h->env_data.bt_vol=player_vol_bt;
            tmp = &(env_h->env_data.udisk);
            memcpy((void*)tmp,arg,sizeof(app_mp3_backup_t));
            break;
    #endif
        case ENV_BT_VOL_INFO:
            if(player_vol_bt<= AUDIO_VOLUME_MAX)
                env_h->env_data.bt_vol=player_vol_bt;
            if(player_vol_hfp <= AUDIO_VOLUME_MAX)
                env_h->env_data.hfp_vol=player_vol_hfp;
            break;	
    #if (CONFIG_APP_MP3PLAYER == 1)        
        case ENV_MUSIC_VOL_INFO:
    #ifdef FM_ENABLE 	
        case ENV_FM_VOL_INF:
    #endif
    #endif     
        case ENV_LINE_VOL_INFO:
            //os_printf("555555:%d\r\n",player_vol_bt);
            os_printf("save vol:%x\r\n",player_vol_bt);
            if(player_vol_bt <= AUDIO_VOLUME_MAX)
                env_h->env_data.bt_vol=player_vol_bt;
            break;

        default:
            break;
    }
}

#endif

RAM_CODE int app_env_check_feature_flag(uint32 flag)
{
    app_env_handle_t env_h = app_env_get_handle();
    return (env_h->env_cfg.feature.feature_flag & flag);
}
RAM_CODE int app_env_check_inquiry_always(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_INQUIRY_ALWAYS);
}
RAM_CODE int app_env_check_SPP_profile_enable(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_SPP_PROFILE);
}
RAM_CODE int app_env_check_HID_profile_enable(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_HID_PROFILE);
}
RAM_CODE int app_env_check_AVRCP_TG_profile_enable(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_VOLUME_SYNC);
}
RAM_CODE int app_env_check_DAC_POP_handle_enable(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DAC_DC_COMPENSATION);
}
RAM_CODE int app_env_check_Charge_Mode_PwrDown(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_CHARGE_MODE_PWR_DOWN);
}
RAM_CODE int app_env_check_Charge_Det_use_gpio(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_CHARGE_DET_USE_GPIO);
}
RAM_CODE int app_env_check_Charge_Time_Out_Enable(void)
{
	return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_CHARGE_TIMEOUT_ENABLE);
}
RAM_CODE int app_env_check_Use_ext_PA(void)
{
    app_env_handle_t env_h = app_env_get_handle();
	return env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PA_ENABLE;
}
RAM_CODE int app_env_check_bat_display(void)
{
    app_env_handle_t env_h = app_env_get_handle();
    return (env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_APP_BAT_DISPLAY);
}

RAM_CODE int app_env_check_pwrCtrl_mode(void)
{
//return: 0=拨码开关; 1=按键软开关;
// 2=拨码软开关; 3=按键软开关加gpio控制mos
//
    app_env_handle_t env_h = app_env_get_handle();
    if(env_h->env_cfg.used)
        return env_h->env_cfg.system_para.mos_soft_power_flag;
    else
        return 0;
}
RAM_CODE int app_env_check_sniff_mode_Enable(void)
{
    return app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_SNIFF_ENABLE);
}
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/&&(CONFIG_SW_SWITCH_KEY_POWER == 1)
void app_env_pwrctrl_by_switch(void)
{
    if(SW_PWR_KEY_SWITCH==app_env_check_pwrCtrl_mode())
    {
        static uint8 sw_io_cnt = 0;
        app_env_handle_t env_h = app_env_get_handle();
        uint8 pwrctrl_pin = env_h->env_cfg.system_para.pwrctrl_pin + GPIO0;
        uint8 high_flag = (env_h->env_cfg.system_para.system_flag & APP_ENV_SYS_FLAG_PWRCTRL_HIGH) ? 1:0 ;
        //			gpio_config( pwrctrl_pin , 0 );

        if((high_flag == gpio_input(pwrctrl_pin))|| (bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN)&&get_Charge_state()))
        {
            sw_io_cnt = 0;
        }
        else
        {
            if (sw_io_cnt < 200)
                sw_io_cnt++;

            if (sw_io_cnt == 5)
            {
		  app_set_auto_powerdown();
		  //app_button_sw_action(BUTTON_BT_POWERDOWN);
                app_button_powerdown();
            }
        }
    }
}
#endif


extern u_int32 XVR_analog_reg_save[];
extern u_int32 XVR_reg_0x24_save;
void app_env_rf_pwr_set(uint8 type)
{
    //type: 0=a2dp mode;  1=hfp mode;
    app_env_handle_t  env_h = app_env_get_handle();
    app_rfpwr_t *rf_pwr;

    if(type==0)
    {
        rf_pwr = &env_h->env_cfg.feature.a2dp_rf_pwr;
    }
    else if(type == 1)
    {
        rf_pwr = &env_h->env_cfg.feature.hfp_rf_pwr;
    }
    else
        return;

    XVR_reg_0x24_save = (XVR_reg_0x24_save&~(0x1f<<8))|(((rf_pwr->big_adj)&0x1f)<<8);
}

