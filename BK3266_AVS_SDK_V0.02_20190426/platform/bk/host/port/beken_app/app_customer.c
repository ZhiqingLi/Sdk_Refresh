#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "bt_init.h"

#if (CONFIG_CUSTOMER_2PHONES_HUNG_ACCETP == 1)\
	|| (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)\
	|| (CONFIG_CUSTOMER_1V2_CON_MODE == 1)\
	|| (CONFIG_CUSTOMER_MOTOR_CONTROL == 1)
static app_customer_t app_customer;
#endif

void app_customer_special_PA_open(void)
{
	app_env_handle_t  env_h = app_env_get_handle();
	uint32 clock_back;
	uint8 pa_mute_pin = env_h->env_cfg.system_para.pamute_pin;
	os_printf("pa_mute_pin:%d\r\n",pa_mute_pin);
		clock_back = BK3000_PMU_CONFIG;
		cpu_set_interrupts_enabled(0);
		BK3000_set_clock(3,1);
		os_delay_us(100);
		gpio_output(pa_mute_pin, 0);
		os_delay_ms(10);
		gpio_output(pa_mute_pin, 1);
		os_delay_ms(20);
		gpio_output(pa_mute_pin, 0);
		os_delay_us(70);
		gpio_output(pa_mute_pin, 1);
		os_delay_us(70);
		gpio_output(pa_mute_pin, 0);
		os_delay_us(70);
		gpio_output(pa_mute_pin, 1);
		os_delay_us(70);
		gpio_output(pa_mute_pin, 0);
		os_delay_us(70);
		gpio_output(pa_mute_pin, 1);
		os_delay_us(100);

		BK3000_PMU_CONFIG = clock_back;
		cpu_set_interrupts_enabled(1);

}

void app_customer_ADC_DC_filter(void)
{
	//ADC EQ:f0=60Hz,-20dB,Bw=80Hz,HPF
	BK3000_AUD_ADC_CONF0 |= (1<<sft_AUD_ADC_HPF2_BYPASS);
	BK3000_AUD_ADC_CONF0 = (BK3000_AUD_ADC_CONF0 &(~(0xffff)))|0x3e89;
	BK3000_AUD_ADC_CONF1 = 0x823b3f3c;
	BK3000_AUD_ADC_CONF2 = 0xc2367dc0;
	BK3000_AUD_ADC_CONF0 &= ~(1<<sft_AUD_ADC_HPF2_BYPASS);
	BK3000_AUD_ADC_CONF0 |= (1<<sft_AUD_ADC_HPF1_BYPASS);
}

/*****上升沿或下降沿触发KEY事件****/
#if (CONFIG_CUSTOMER_EDGE_KEY == 1)
void app_edge_key(void)
{
	uint8 keyvalue;
	static uint8 keybuf=0,cnt=0;

	gpio_config(PIN_EDGE_KEY, 0);
	keyvalue = gpio_input(PIN_EDGE_KEY);
	if (keyvalue != keybuf)
	{
		if (++cnt > 3)
		{
			cnt = 0;
			keybuf = keyvalue;
			if (keybuf)//上升沿
				app_button_redial_last_number();
			else//下降沿
				app_button_voice_dial_set();
		}
	}
	else
		cnt = 0;
}
#endif

/*****挂断当前通话，接听来电****/
#if (CONFIG_CUSTOMER_2PHONES_HUNG_ACCETP == 1)
void app_set_2phones_hung_accetp(uint8 flag)
{
	app_customer.phones_hung_accetp = flag;
}

uint8 app_get_2phones_hung_accetp(void)
{
	return app_customer.phones_hung_accetp;
}
#endif

/****有部分耳机要求开机或来电时电机要振动****/
#if (CONFIG_CUSTOMER_MOTOR_CONTROL == 1)
/*设置电机动作的时间 单位10ms*/
void app_M_cnt_set(uint16 cnt)
{
	app_customer.motor_cnt = cnt;
}

void app_M_control(void)
{
	if (get_current_hfp_flag(APP_FLAG2_HFP_INCOMING) || (app_customer.motor_cnt>0))
		M_EN;
	else
		M_OFF;

	if (app_customer.motor_cnt > 0)
		app_customer.motor_cnt --;
}
#endif

#if (CONFIG_CUSTOMER_BUTTON_HID_MODE == 1)
void app_customer_hid_set_cnt(uint8 file_id)
{
	if(app_env_check_HID_profile_enable())
	{
		if(file_id == APP_WAVE_FILE_ID_CONN)
	       app_customer.hid_cnt = 100;   // xian
	}
}

void app_customer_hid_disconnect(void)
{
	if(app_env_check_HID_profile_enable())
	{
		/*
		原理：A2DP连接并且HID已连接先断开HID，然后按键触发连接HID；
		*/
        if(app_customer.hid_cnt)
        {
        	app_customer.hid_cnt--;
        	if((app_customer.hid_cnt==0) && bt_flag1_is_set( APP_FLAG_HID_CONNECTION))
				hid_cmd_disconnect();
        }
	}
}

void app_customer_hid_switch_mode(void)
{
	if(app_env_check_HID_profile_enable())
	{
		if( bt_flag1_is_set(APP_FLAG_ACL_CONNECTION))
			app_wave_file_play_start(APP_WAVE_FILE_ID_VOL_MAX);//HID模式提示音
		
        if(bt_flag1_is_set( APP_FLAG_HID_CONNECTION ))
			hid_cmd_disconnect();
		else
			app_bt_active_conn_profile(PROFILE_ID_HID,NULL);
	}
}
#endif

#if (CONFIG_CUSTOMER_1V2_CON_MODE == 1)
void app_customer_1v2_con_set_cnt(uint8 file_id)
{
#if (BT_MODE==BT_MODE_1V2)
	if(app_check_bt_mode(BT_MODE_1V2))
	{
		if(file_id == APP_WAVE_FILE_ID_CONN)
			app_customer.con_1v2_cnt = 100; // xian		
	}
#endif
}

void app_customer_1v2_con_close(void)
{
#if (BT_MODE==BT_MODE_1V2)
	app_handle_t sys_hdl = app_get_sys_handler();

	if(app_check_bt_mode(BT_MODE_1V2))
	{
		if(app_customer.con_1v2_cnt)
		{
			app_customer.con_1v2_cnt--;
			if(app_customer.con_1v2_cnt == 0)
				bt_unit_set_scan_enable(sys_hdl->unit, HCI_PAGE_SCAN_ENABLE);
		}	
	}
#endif
}

uint8 app_customer_1v2_con_reject(const btaddr_t *raddr)
{
#if (BT_MODE==BT_MODE_1V2)
	btaddr_t addr; 
	app_handle_t sys_hdl = app_get_sys_handler();

	if(app_check_bt_mode(BT_MODE_1V2))
	{
		if(hci_get_acl_link_count(sys_hdl->unit) >= 1)	
		{
			memcpy(&addr, (uint8 *)raddr, sizeof(btaddr_t));
			if (app_env_key_stored(&addr) == 0)
			{
				unit_acl_reject(sys_hdl->unit, &addr);
				return 1;
			} 
		}
	}
#endif
	return 0;
}

#endif

//EOF
