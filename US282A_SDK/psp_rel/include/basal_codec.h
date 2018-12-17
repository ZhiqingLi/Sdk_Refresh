#ifndef __BASAL_CODEC_H__
#define __BASAL_CODEC_H__

typedef enum {
	MMM_OPEN = 0,
} mmm_cmd_e;

typedef enum {
	CODEC_OPEN = 0,
} codec_cmd_e;

extern void* mmm_front_op_entry(void *param1, void *param2, void *param3,
		mmm_cmd_e cmd_type);
extern void* mmm_back_op_entry(void *param1, void *param2, void *param3,
		mmm_cmd_e cmd_type);
extern void* codec_front_op_entry(void *param1, void *param2, void *param3,
		codec_cmd_e cmd_type);
extern void* codec_back_op_entry(void *param1, void *param2, void *param3,
		codec_cmd_e cmd_type);

#define mmm_front_open(a,b,c) mmm_front_op_entry((void*)(a), (void*)(b), (void*)(c), MMM_OPEN)
#define mmm_back_open(a,b,c) mmm_back_op_entry((void*)(a), (void*)(b), (void*)(c), MMM_OPEN)

#define codec_front_open(a,b,c) codec_front_op_entry((void*)(a), (void*)(b), (void*)(c), CODEC_OPEN)
#define codec_back_open(a,b,c) codec_back_op_entry((void*)(a), (void*)(b), (void*)(c), CODEC_OPEN)
#endif

