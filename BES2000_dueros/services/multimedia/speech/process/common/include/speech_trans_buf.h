#ifndef __SPEECH_TRANS_BUF_H__
#define __SPEECH_TRANS_BUF_H__

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
int speech_trans_buf_init(void);
int speech_trans_buf_deinit(void);

int speech_trans_buf_input_save_data(short *buf, int len);
int16_t *speech_trans_buf_input_get_data(int len);

int speech_trans_buf_output_save_data(short *buf, int len);
int speech_trans_buf_output_get_data(short **buf);


int speech_echo_buffer_init(void);
int speech_echo_buffer_save_data(short *buf, uint32_t len);
// int speech_echo_buffer_get_data(short **buf, int len);
short *speech_echo_buffer_get_data(int len);
#endif
    
#ifdef __cplusplus
}
#endif

#endif
