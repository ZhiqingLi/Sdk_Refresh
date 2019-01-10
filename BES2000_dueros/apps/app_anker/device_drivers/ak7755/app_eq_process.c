

#include "app_eq_process.h"
#include "ak7755.h"
#include "string.h"

//#include "app_custom_ctrl.h"
#include "hal_trace.h"

#define Ctrl_STR(x)		#x
#define EqCtrlLog(title,value,...)	do{TRACE(title ":-----------" value,##__VA_ARGS__);}while(0)
#define EqCtrlLogI(x,...)	EqCtrlLog(Ctrl_STR(EqCtrlLogI),x,##__VA_ARGS__ )
#define EqCtrlLogD(x,...)	EqCtrlLog(Ctrl_STR(EqCtrlLogD),x,##__VA_ARGS__ )
#define EqCtrlLogW(x,...)	EqCtrlLog(Ctrl_STR(EqCtrlLogW),x,##__VA_ARGS__ )
#define EqCtrlLogE(x,...)	EqCtrlLog(Ctrl_STR(EqCtrlLogE),x,##__VA_ARGS__ )


static Eq_Mode eq_mode = EQ_MODE_DEFAULT;
static uint8_t bass_state = false;

static uint8_t Eq_Frequency[9] = {0};


void app_set_custom_eq(int index,uint8_t db){
	Eq_Frequency[index] = db;
	EqCtrlLogI("%s: %d",__func__,Eq_Frequency[index]);
}

uint8_t app_get_custom_eq(int index){
	EqCtrlLogI("%s: %d",__func__,Eq_Frequency[index]);
	return Eq_Frequency[index];
}


void app_eq_mode_set(Eq_Mode mode){
	eq_mode = mode;
	EqCtrlLogI("%s: %d",__func__,eq_mode);
}

Eq_Mode app_eq_mode_get(void){
	EqCtrlLogI("%s: %d",__func__,eq_mode);
	return eq_mode;
}

void app_set_bass_state(uint8_t state){
	EqCtrlLogI("%s: %d",__func__,state);
	bass_state = state;
}
uint8_t  app_get_bass_state(void){
	EqCtrlLogI("%s: %d",__func__,bass_state);
	return bass_state;
}

#if 0
static int  app_custom_eq_process(uint16_t commond,uint8_t *data,uint32_t data_len,uint8_t *send_buf){
	
	uint8_t command_id = (uint8_t)(commond&0x00ff);
	int send_len = 0;
	EqCtrlLogI("%s: command: %d, datalen:%d",__func__,commond,data_len);
	switch(command_id){
		case CUSTOM_APP_CMD_EQ_GET_INDEX:{
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_GET_INDEX");
			uint8_t eq_index = (uint8_t)app_eq_mode_get();
			send_len = 1;
			memcpy(send_buf,&eq_index,send_len);}
			break;
		case CUSTOM_APP_CMD_EQ_SET_INDEX:
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_SET_INDEX");
			if(data_len == 1){
				app_eq_mode_set((Eq_Mode)(*data));
			}else
				send_len = -1;
			break;
		case CUSTOM_APP_CMD_EQ_GET_CUSTOM_EQ:
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_GET_CUSTOM_EQ");
			send_len = 9;
			for(int i=0;i<9;i++){
				send_buf[i] = app_get_custom_eq(i);
			}
			break;
		case CUSTOM_APP_CMD_EQ_SET_CUSTOM_EQ:
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_SET_CUSTOM_EQ");
			if(data_len == 9){
				for(int i=0;i<data_len;i++){
					app_set_custom_eq(i,data[i]);
				}
			}else{
			    EqCtrlLogE("data len error");
				send_len = -1;
			}
			break;
		case CUSTOM_APP_CMD_EQ_GET_ALL_EQ_INFO:
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_GET_ALL_EQ_INFO");
			send_buf[send_len++] = app_get_bass_state();
			send_buf[send_len++] = (uint8_t)app_eq_mode_get();
			for(int t=0;t<9;t++){
				send_buf[send_len++] = app_get_custom_eq(t);
			}
			break;
		case CUSTOM_APP_CMD_EQ_GET_EQ_LIST:
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_GET_EQ_LIST");
			send_len = -1;
			break;
		case CUSTOM_APP_CMD_EQ_GET_BASSUP_STATE:{
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_GET_BASSUP_STATE");
			uint8_t bass_state = app_get_bass_state();
			send_len = 1;
			memcpy(send_buf,&bass_state,send_len);
			}
			break;
		case CUSTOM_APP_CMD_EQ_SET_BASSUP_STATE:{
			EqCtrlLogI("CUSTOM_APP_CMD_EQ_SET_BASSUP_STATE");
			if(data_len == 1){
				app_set_bass_state(*data);
			}else{
				send_len = -1;
			}}
			break;
		default:
			EqCtrlLogE("%s : -- command not support command: %d",__func__,commond);
			send_len = -1; // response fail
			break;
	}
	return send_len;
}



void app_eq_init(void){
	EqCtrlLogI("%s",__func__);
#ifdef __CUSTOM_APP__
	register_custom_group_process((uint8_t)CUSTOM_APP_CMD_GROUP_EQ,app_custom_eq_process);
#endif
}
#endif
