

#include "ambient_light_process.h"

#include "is31fl3236a.h"
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

#include "plat_types.h"



#define TITLE_STR(x)		#x
#define AmbientLightLog(title,value,...)	do{TRACE(title ":" value,##__VA_ARGS__);}while(0)
#define AmbientLightLogI(x,...)	AmbientLightLog(TITLE_STR(AmbientLightLogI),x,##__VA_ARGS__ )
#define AmbientLightLogD(x,...)	AmbientLightLog(TITLE_STR(AmbientLightLogD),x,##__VA_ARGS__ )
#define AmbientLightLogW(x,...)	AmbientLightLog(TITLE_STR(AmbientLightLogW),x,##__VA_ARGS__ )
#define AmbientLightLogE(x,...)	AmbientLightLog(TITLE_STR(AmbientLightLogE),x,##__VA_ARGS__ )



#define AMBIENT_LIGHT_PROCESS_THREAD_STACK_SIZE			(2*1024)
#define AMBIENT_LIGHT_PROCESS_THREAD_PRIORITY			(osPriorityAboveNormal)




typedef struct  _AMBIENT_LIGHT_DATA_{
	osThreadId ambient_light_thread_id;
	osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#ifdef CMSIS_OS_RTX
    uint32_t _semaphore_data[2];
#endif
	bool light_thread_runing;
	Led_Effect_Mode effect_mode;
}Ambient_Light_Data;

static Ambient_Light_Data ambient_light_data ;



void mic_mute_monitor(void){
	static bool pre_mic_mute = false;
	bool mic_mute = false;
	
	if(ambient_light_data.light_thread_runing == FALSE)return; // wait for light_thread running
		
	mic_mute = hal_mic_power_state_get()>0?true:false;

	if(mic_mute == pre_mic_mute)return;

	if(mic_mute){
		if(ambient_light_data.effect_mode >=EFFECT_MODE_START_LISTENING&&ambient_light_data.effect_mode<=EFFECT_MODE_SPEAKING){
			AmbientLightLogI("busy");
		}else{
			ambient_light_set_effect_mode(EFFECT_MODE_MIC_MUTE);
			pre_mic_mute = mic_mute;
			AmbientLightLogI("mic_mute : %d ",mic_mute);
		}
	}else{
		ambient_light_set_effect_mode(EFFECT_MODE_MIC_UNMUTE);
		AmbientLightLogI("mic_mute : %d ",mic_mute);
		pre_mic_mute = mic_mute;
	}
}

void bt_connect_state_monitor(void){
	static bool pre_state = TRUE;
	bool bt_connect_state = false;

	if(ambient_light_data.light_thread_runing == FALSE)return; // wait for light_thread running

	//bt_connect_state = app_bluetooth_is_connected();
	if(bt_connect_state == pre_state)return;

	if(bt_connect_state){
		ambient_light_set_effect_mode(EFFECT_MODE_IDEL);
	}else{
		ambient_light_set_effect_mode(EFFECT_MODE_BT_DISCONNECT);
	}
	pre_state = bt_connect_state;
}

static void listening_led_process(bool direction,bool *running_flag){ // 1:start direction 0:end direction 
		struct all_light_data all_data;
		int m=0;
		AmbientLightLogI("%s  dir:%d",__func__,direction);
		if(direction == 0)m = 5;
		while(*running_flag){
			memset(&all_data, 0, sizeof(struct all_light_data));
			all_data.data[m].r_val = 0x0;
        	all_data.data[m].g_val = 0x4f;
        	all_data.data[m].b_val = 0xf;
			for(int j=0;j<m;j++){
				all_data.data[j].r_val = 0x0;
	        	all_data.data[j].g_val = 0x0;
	        	all_data.data[j].b_val = 0x3f; 
			}
			all_data.data[11-m].r_val = 0x0;
        	all_data.data[11-m].g_val = 0x4f;
        	all_data.data[11-m].b_val = 0xf; 
			for(int k=11;k>(11-m);k--){
				all_data.data[k].r_val = 0x0;
	        	all_data.data[k].g_val = 0x0;
	        	all_data.data[k].b_val = 0x3f;
			}
		   is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
			osDelay(30);
			if(direction){ // begin dir
				if(++m>=6)break;
			}else{
				if(--m<0){  // end dir
					memset(&all_data, 0, sizeof(struct all_light_data));
					is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
					break;
				}
			}
		}
		AmbientLightLogI("%s exit dir:%d",__func__,direction);
}

static void thinking_led_process(bool *running_flag,Led_Effect_Mode *mode){
	struct all_light_data all_data;
	bool display = false;
	Led_Effect_Mode pre_mode = *mode;
	AmbientLightLogI("%s enter mode: %d ",*mode);
	while(*running_flag){
		memset(&all_data, 0, sizeof(struct all_light_data));
		for(int m=0;m<12;m++){
			if(display){
				if(m%2){
					all_data.data[m].r_val = 0x0;
					all_data.data[m].g_val = 0x0;
					all_data.data[m].b_val = 0x3f;
				}else{
					all_data.data[m].r_val = 0x0;
					all_data.data[m].g_val = 0x4f;
					all_data.data[m].b_val = 0xf;
				}
			}else{
					if(m%2){
						all_data.data[m].r_val = 0x0;
				    	all_data.data[m].g_val = 0x4f;
				    	all_data.data[m].b_val = 0xf;
					}else{
						all_data.data[m].r_val = 0x0;
			        	all_data.data[m].g_val = 0x0;
			        	all_data.data[m].b_val = 0x3f; 
					}
				}
		}
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		for(int delay=0;delay<10;delay++){
			osDelay(10);
			if((pre_mode != *mode) || (*running_flag== false))goto do_exit;
		}
		display = !display;
	}
do_exit:
	AmbientLightLogI("%s exit mode: %d ",*mode);
}

static void speaking_led_process(bool *running_flag,Led_Effect_Mode *mode){
	struct all_light_data all_data;
	Led_Effect_Mode pre_mode = *mode;
	for(int m=0;m<12;m++){
		all_data.data[m].r_val = 0x0;
		all_data.data[m].g_val = 0x4f;
		all_data.data[m].b_val = 0xf;
	}
	is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
	while(*running_flag && (pre_mode == *mode))osDelay(10);
	listening_led_process(0,running_flag);
}

static void start_listening_process(bool *running_flag){
	AmbientLightLogI("%s  running_flag: %d ",__func__,*running_flag);
	listening_led_process(1,running_flag);
}

static void end_listening_process(bool *running_flag){
	AmbientLightLogI("%s  running_flag: %d ",__func__,*running_flag);
	listening_led_process(0,running_flag);
}

static void idel_led_process(void){
	struct all_light_data all_data;
	memset(&all_data, 0, sizeof(struct all_light_data));
	is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
	AmbientLightLogI("%s",__func__);
}

static void mic_mute_led_process(void){
	struct all_light_data all_data;
	memset(&all_data, 0, sizeof(struct all_light_data));
	for(unsigned char i=0;i<0x3f;i+=2){
		for(int m=0;m<12;m++){
			all_data.data[m].r_val = i;
		}
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		osDelay(10);
	}
	AmbientLightLogI("%s",__func__);
}

static void mic_unmute_led_process(void){
	struct all_light_data all_data;
	memset(&all_data, 0, sizeof(struct all_light_data));
	for(char i=0x40;i>=0;i-=2){
		for(int m=0;m<12;m++){
			all_data.data[m].r_val = i;
		}
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		osDelay(10);
	}
	AmbientLightLogI("%s",__func__);
}
#if 0
static void bt_connected_or_disconnect_led_process(void){
	struct all_light_data all_data;
	memset(&all_data, 0, sizeof(struct all_light_data));
	for(int i=0;i<3;i++){
		for(int m=0;m<12;m++){
			all_data.data[m].b_val= 0x3f;
		}
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		osDelay(200);
		memset(&all_data, 0, sizeof(struct all_light_data));
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		osDelay(200);
	}
	AmbientLightLogI("%s",__func__);
}
#endif
static void bt_disconect_led_process(bool *running_flag,Led_Effect_Mode *mode){
	struct all_light_data all_data;
	Led_Effect_Mode pre_mode = *mode;
	AmbientLightLogI("%s enter mode: %d ",*mode);
	memset(&all_data, 0, sizeof(struct all_light_data));
	unsigned char scroll_index = 0;
	while(*running_flag){
		memset(&all_data, 0, sizeof(struct all_light_data));
		all_data.data[scroll_index].r_val = 0x1;
		all_data.data[scroll_index].g_val = 0x1;
		all_data.data[(scroll_index+1)%12].r_val = 0x1;
		all_data.data[(scroll_index+1)%12].g_val = 0x1;
		all_data.data[(scroll_index+2)%12].r_val = 0x1;
		all_data.data[(scroll_index+2)%12].g_val = 0x1;
		all_data.data[(scroll_index+3)%12].r_val = 0x1;
		all_data.data[(scroll_index+3)%12].g_val = 0x1;
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		if(++scroll_index>=12)scroll_index = 0;
		for(int j=0;j<6;j++){  // delay 120ms
			osDelay(20);
			if((pre_mode != *mode) || (*running_flag== false))goto do_exit;
		}
	}
do_exit:
	AmbientLightLogI("%s exit mode: %d ",*mode);
}

static void ama_disconnected_led_process(bool *running_flag,Led_Effect_Mode *mode){
	struct all_light_data all_data;
	Led_Effect_Mode pre_mode = *mode;
	int breathe_count = 0;
	bool direction = true;
	AmbientLightLogI("%s enter mode: %d ",*mode);
	while(*running_flag){
		memset(&all_data, 0, sizeof(struct all_light_data));
		for(int m=0;m<12;m++){
			all_data.data[m].r_val = breathe_count;
		}
		is31fl3236a_dev_ctrl(IS31FL3236A_CMD_SET_RGB_ALL,&all_data);
		osDelay(30);
		if(direction){
			breathe_count ++;
			if(breathe_count>50)direction = false;
		}else{
			breathe_count --;
			if(breathe_count <= 0)direction = true;
		}
		if((pre_mode != *mode) || (*running_flag== false))goto do_exit;
	}
do_exit:
	AmbientLightLogI("%s exit mode: %d ",*mode);
	
}

void ambient_light_set_effect_mode(Led_Effect_Mode mode){
	if(ambient_light_data.light_thread_runing == FALSE){
		AmbientLightLogE("ambient_light_thread not running");
		return;
	}
	ambient_light_data.effect_mode = mode;
	AmbientLightLogI("%s : mode : %d",__func__,mode);
	osSemaphoreRelease(ambient_light_data._osSemaphoreId);
	osDelay(1);
}

static void ambient_light_thread(void *arg){
	Ambient_Light_Data *light_data = &ambient_light_data;//(Ambient_Light_Data)arg;
	static Led_Effect_Mode pre_mode = EFFECT_MODE_IDEL;
	
	AmbientLightLogI("%s enter",__func__);
	is31fl3236a_dev_ctrl(IS31FL3236A_CMD_CHIP_INIT,NULL);
	light_data->light_thread_runing = true;
	while(light_data->light_thread_runing){

		if(pre_mode != light_data->effect_mode){
			pre_mode = light_data->effect_mode;
		}else{
			osSemaphoreWait(ambient_light_data._osSemaphoreId, 1000);
			continue;
		}
		AmbientLightLogI("effect mode : %d ",light_data->effect_mode);
		switch(light_data->effect_mode){
			case EFFECT_MODE_IDEL:
				idel_led_process();
				break;
			case EFFECT_MODE_START_LISTENING:
				start_listening_process(&light_data->light_thread_runing);
				break;
			case EFFECT_MODE_ACTIVE_LISTENING:
				break;
			case EFFECT_MODE_END_LISTENING:
				end_listening_process(&light_data->light_thread_runing);
				break;
			case EFFECT_MODE_THINKING:
				thinking_led_process(&light_data->light_thread_runing,&light_data->effect_mode);
				break;
			case EFFECT_MODE_SPEAKING:
				speaking_led_process(&light_data->light_thread_runing,&light_data->effect_mode);
				break;
			case EFFECT_MODE_MIC_MUTE:
				mic_mute_led_process();
				break;
			case EFFECT_MODE_MIC_UNMUTE:
				mic_unmute_led_process();
				break;
			case EFFECT_MODE_BT_DISCONNECT:
				bt_disconect_led_process(&light_data->light_thread_runing,&light_data->effect_mode);
				break;
			default:
				AmbientLightLogE("unknow effect mode");
				break;
		}

	}
	is31fl3236a_dev_ctrl(IS31FL3236A_CMD_CHIP_DEINIT,NULL);
    light_data->ambient_light_thread_id = NULL;
	AmbientLightLogI("%s destroy",__func__);
}

osThreadDef(ambient_light_thread, AMBIENT_LIGHT_PROCESS_THREAD_PRIORITY, AMBIENT_LIGHT_PROCESS_THREAD_STACK_SIZE);


void ambient_light_process_thread_create(void)
{
	if(ambient_light_data._osSemaphoreId == NULL){
		ambient_light_data._osSemaphoreDef.semaphore = ambient_light_data._semaphore_data;
		ambient_light_data._osSemaphoreId = osSemaphoreCreate(&(ambient_light_data._osSemaphoreDef), 0);
		AmbientLightLogI("_osSemaphoreId create");
	}

    if(ambient_light_data.ambient_light_thread_id == NULL)
    {
    	AmbientLightLogI("%s",__func__);
		ambient_light_data.ambient_light_thread_id = osThreadCreate(osThread(ambient_light_thread),&ambient_light_data);
    }else{
		AmbientLightLogW("%s already create",__func__);
	}
	
}

static void ambient_light_process_thread_destroy(void)
{
	ambient_light_data.light_thread_runing = false;
	AmbientLightLogI("%s",__func__);
}

