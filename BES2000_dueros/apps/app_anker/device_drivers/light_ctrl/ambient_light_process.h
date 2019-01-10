#ifndef _AMBIENT_LIGHT_PROCESS_H_
#define _AMBIENT_LIGHT_PROCESS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus 
    extern "C"  {
#endif 

typedef enum  _LED_EFFECT_MODE_{
	EFFECT_MODE_IDEL,
	EFFECT_MODE_START_LISTENING,
	EFFECT_MODE_ACTIVE_LISTENING,
	EFFECT_MODE_END_LISTENING,
	EFFECT_MODE_THINKING,
	EFFECT_MODE_SPEAKING,
	EFFECT_MODE_MIC_MUTE,
	EFFECT_MODE_MIC_UNMUTE,
	EFFECT_MODE_BT_DISCONNECT,
	EFFECT_MODE_BT_CONNECTED
}Led_Effect_Mode;

extern void ama_connect_state_set(bool state);
extern void ambient_light_set_effect_mode(Led_Effect_Mode mode);
extern void ambient_light_process_thread_create(void);
extern void mic_mute_monitor(void);
extern void bt_connect_state_monitor(void);


#ifdef __cplusplus 
}
#endif 



#endif

 
