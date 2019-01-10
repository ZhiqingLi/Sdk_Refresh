
#include "led_process.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "app_pwl.h"
#include "app_battery.h"
#include "app_bt_stream.h"
#include "nvrecord.h"

#include "bes_hal_board.h"
#include "app_bt.h"
#include "app_status_ind.h"
#include "apps.h"
#include "ambient_light_process.h"

#if 1
#ifdef __cplusplus
extern "C" {
#endif
extern bool is_speech_type_tap(void);
#ifdef __cplusplus
	}
#endif
#endif
#define LED_TIMER_INTERVAL	100 // 100ms

#define LED_TIMER_200MS   (200/LED_TIMER_INTERVAL)
#define LED_TIMER_500MS   (500/LED_TIMER_INTERVAL)
#define LED_TIMER_1S   (1000/LED_TIMER_INTERVAL)
#define LED_TIMER_10S   (10000/LED_TIMER_INTERVAL)



#define TITLE_STR(x)		#x
#define LedPrcessLog(title,value,...)	do{TRACE(title ":" value,##__VA_ARGS__);}while(0)
#define LedPrcessLogI(x,...)	//LedPrcessLog(TITLE_STR(LedPrcessLogI),x,##__VA_ARGS__ )
#define LedPrcessLogD(x,...)	LedPrcessLog(TITLE_STR(LedPrcessLogD),x,##__VA_ARGS__ )
#define LedPrcessLogW(x,...)	LedPrcessLog(TITLE_STR(LedPrcessLogW),x,##__VA_ARGS__ )
#define LedPrcessLogE(x,...)	LedPrcessLog(TITLE_STR(LedPrcessLogE),x,##__VA_ARGS__ )


typedef enum{
	BLINK_STATE_NORMAL=0,
	BLINK_STATE_VOL,
	BLINK_STATE_CHARGING,
	BLINK_STATE_BATTERY_LOW
}Blink_State;

typedef enum{
	BT_LED_STATE_DISCONNECT=0,
	BT_LED_STATE_PAIRING,
	BLINK_STATE_CONNECTED
}BT_LED_State;

static void power_led_process(void);
static void bt_led_process(void);





static bool get_eq_reset_to_default_flag(void){
	static int eq_blink_count = 0;
	static bool pre_eq_reset_state = false;
	bool eq_reset_flag = false;;

	if(eq_reset_flag == pre_eq_reset_state && eq_blink_count == 0)return false;
	if(eq_reset_flag == false){
		pre_eq_reset_state = eq_reset_flag;
		eq_blink_count = 0;
		return false;
	}
	if(eq_reset_flag != pre_eq_reset_state){
		eq_blink_count = 4;
		return true;
	}else if(eq_blink_count>0){
		return true;
	}
	return false;
}



static void bass_led_process(void){
	static unsigned int power_blink_count = 0;
	bool bass_up_mode_state = false;
	return;
	if(get_eq_reset_to_default_flag()){
		if((++power_blink_count%LED_TIMER_200MS) == 0)
			switch_led_bass_state();
	}else{
		power_blink_count = 0;
		bes_anker_3163_led_ctrl(LED_BASS_STATE,bass_up_mode_state);
	}
}

static void light_led_process(void){
	bool ambient_pattern_light_state = false;
	
	bes_anker_3163_led_ctrl(LED_ENV_STATE,ambient_pattern_light_state);
}

static void mic_led_process(void){
#if 0 //def	BOARD_HW_MIC_CTRL_MODULE
	hal_mic_power_ctrl(is_speech_type_tap());
#endif
}


static void play_pause_led_process(void){

}
static void timer_10s_handler(void){
	static unsigned int timer_count = 0;
	
	if(++timer_count%LED_TIMER_10S == 0){
		LedPrcessLogI("timer_10s_handler");
		app_10_second_timer_check();
	}

}
static void led_process_timeout_handler(void const *param){
	power_led_process();
	bt_led_process();
	bass_led_process();
	light_led_process();
	play_pause_led_process();
	mic_led_process();
	timer_10s_handler();
	mic_mute_monitor();
}

static osTimerId led_process_timer = NULL;
osTimerDef(led_timer, led_process_timeout_handler);

void led_pin_init(void){
	int i;
	for (i=0;i<2;i++){
        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_pinmux_pwl[i], 1);
        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[i].pin, HAL_GPIO_DIR_OUT, 1);
    }
}
static bool power_red_led_state = false;
static bool power_white_led_state = false;

void power_red_led_set(bool state){
	if(state)
		hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	else
		hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	power_red_led_state = state;
}

void power_red_led_switch_state(void){

	if(power_red_led_state){
		power_red_led_set(false);
	}else{
		power_red_led_set(true);
	}
}

void power_white_led_set(bool state){
	if(state)
		hal_gpio_pin_clr((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_0].pin);
	else
		hal_gpio_pin_set((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_0].pin);
	power_white_led_state = state;
}

void power_white_led_switch_state(void){
	uint8_t ret = hal_gpio_pin_get_val((enum HAL_GPIO_PIN_T)cfg_hw_pinmux_pwl[APP_PWL_ID_1].pin);
	if(power_white_led_state){
		power_white_led_set(false);
	}else{
		power_white_led_set(true);
	}
}


void led_process_init(void){
	
	led_pin_init();
	
	if (led_process_timer == NULL)
        led_process_timer = osTimerCreate(osTimer(led_timer), osTimerPeriodic, NULL);
	else{
		LedPrcessLogW("led_process_timer already create");
		return;
	}
	
	if(led_process_timer == NULL){
		LedPrcessLogE("led_process_timer create fail");
		return;
	}
	osTimerStart(led_process_timer, LED_TIMER_INTERVAL);// 100ms
}


static void bt_led_process(void){
	static unsigned int pre_bt_state = BT_LED_STATE_DISCONNECT;
	static unsigned int bt_blink_count= 0;
	BT_LED_State bt_state = BT_LED_STATE_DISCONNECT;

	APP_STATUS_INDICATION_T state = app_status_indication_get();
	if(app_bluetooth_is_connected()){
		bt_state = BLINK_STATE_CONNECTED;
	}else if(state == APP_STATUS_INDICATION_BOTHSCAN || state == APP_STATUS_INDICATION_CONNECTING){
		bt_state = BT_LED_STATE_PAIRING;
	}else {
		bt_state = BT_LED_STATE_DISCONNECT;
	}
	
	if(pre_bt_state != bt_state){
		pre_bt_state = bt_state;
		bt_blink_count = 0;
	}
	
	bes_anker_3163_led_ctrl(LED_BT_WHITE,0);
	switch(bt_state){
	case BT_LED_STATE_DISCONNECT:
		bes_anker_3163_led_ctrl(LED_BT_BLUE,0);
		break;
	case BT_LED_STATE_PAIRING:
		if(++bt_blink_count%LED_TIMER_500MS == 0){
			switch_bt_blue_led_state();
		}
		break;
	case BLINK_STATE_CONNECTED:
		bes_anker_3163_led_ctrl(LED_BT_BLUE,1);
		break;
	default:break;
	}
}

static void get_charging_status(bool *low_battery_flag,bool *charging_flag){
	uint8_t s_battery_level;
	enum APP_BATTERY_STATUS_T s_battery_status;
	uint16_t vol=0;
	app_battery_get_info(&vol, &s_battery_level, &s_battery_status);
	//LedPrcessLogI("vol: %d  level: %d,status: %d",vol,s_battery_level,s_battery_status);
	if(s_battery_level<=2){
		*low_battery_flag = true;
	}
	if(s_battery_status== APP_BATTERY_STATUS_CHARGING){
		*charging_flag = true;
	}
}

#define VOL_LED_BLINK_TIMES    4

/*******
return true:  blink, false: not blink 

******/
static bool vol_led_blink_judge(unsigned int *times){
	static unsigned int pre_vol = (unsigned int)TGT_VOLUME_LEVEL_6;
	
	btdevice_volume *bt_volume = app_bt_stream_volume_get_ptr();
	
	unsigned int vol = bt_volume->a2dp_vol;
	
	if(vol == pre_vol && *times == 0)return false;
	
	if(vol == (unsigned int)TGT_VOLUME_LEVEL_15 || vol == (unsigned int)TGT_VOLUME_LEVEL_MUTE){
		if(vol != pre_vol){
			*times = VOL_LED_BLINK_TIMES;
			pre_vol = vol;
			LedPrcessLogI("vol_blink_times = %d",VOL_LED_BLINK_TIMES);
			return true;
		}else if(*times>0){
			return true;
		}
	}else{
		pre_vol = vol;
		*times = 0;
	}	
	return false;
}

static void power_led_process(void){
	static unsigned int power_blink_ticks= 0;
	static unsigned int blink_times = 0;
	static Blink_State blink_state = BLINK_STATE_NORMAL;
	static Blink_State pre_blink_state = BLINK_STATE_NORMAL;
	unsigned int blink_speed = 0;
	
	bool battery_low_flag = false;
	bool charging_flag = false;
	
	get_charging_status(&battery_low_flag,&charging_flag);
	
	if(vol_led_blink_judge(&blink_times)){ // vol max or vol min will blink  power led
		blink_state = BLINK_STATE_VOL;
		blink_speed= LED_TIMER_200MS;
		if(blink_times == VOL_LED_BLINK_TIMES){  // first time init blink color
			power_red_led_set(false);
			power_white_led_set(true);
		}
	}else if(charging_flag){  // charging state 
		blink_state = BLINK_STATE_CHARGING;
		blink_speed= 0;
		blink_times = 0;
	}else if(battery_low_flag){  // low battery state
		blink_state = BLINK_STATE_BATTERY_LOW;
		blink_speed = LED_TIMER_500MS;
		blink_times = 0;
	}else{  // normal state
		blink_state = BLINK_STATE_NORMAL;
		blink_speed = 0; // 
		blink_times = 0;
	}

	if(blink_state !=pre_blink_state){
		LedPrcessLogI("switch %d to %d",pre_blink_state,blink_state);
		power_blink_ticks = 0;  // refresh blink_count,when blink state change
		pre_blink_state = blink_state;
	}
	
	if((blink_speed != 0)&& (++power_blink_ticks%blink_speed != 0))return;
	
	if(blink_times && --blink_times == 0)return;
	
	
	switch(blink_state){
		case BLINK_STATE_NORMAL:
			power_white_led_set(true);
			power_red_led_set(false);
			break;
		case BLINK_STATE_VOL:
			power_white_led_switch_state();
			power_red_led_set(false);
			break;
		case BLINK_STATE_CHARGING:
			power_white_led_set(false);
			power_red_led_set(true);
			break;
		case BLINK_STATE_BATTERY_LOW:
			power_red_led_switch_state();
			power_white_led_set(false);
			break;
		default:break;
	}
}


