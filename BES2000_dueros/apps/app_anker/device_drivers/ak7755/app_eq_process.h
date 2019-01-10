#ifndef __APP_EQ_PROCESS_H_
#define __APP_EQ_PROCESS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus 
    extern "C"  {
#endif 


typedef enum _EQ_MODE_{
	EQ_MODE_DEFAULT = 0,
	EQ_MODE_FLAT,
	EQ_MODE_CHILL,
	EQ_MODE_VOICE,
	EQ_MODE_CUSTOM = 0xFF
}Eq_Mode;

typedef enum _EQ_FREQUENCY_INDEX{
	FREQUENCY_1,
	FREQUENCY_2,
	FREQUENCY_3,
	FREQUENCY_4,
	FREQUENCY_5,
	FREQUENCY_6,
	FREQUENCY_7,
	FREQUENCY_8,
	FREQUENCY_9
}Eq_Frequency_Index;

void app_eq_init(void);


#ifdef __cplusplus 
}
#endif 



#endif
