
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*        brief   全局变量
*      Cailizhen          2015-1          creat
*       Wekan        2015-3-30       review
*******************************************************************************/

#include  "btcall_engine.h"

btcall_global_variable_struct btcall_gl_var =  { 0 };

void *mp_handle = NULL;

callring_data_struct g_callring_data;

bt_stack_info_t *g_p_bt_stack_cur_info;

btcall_info_t g_btcall_info[2];
btcall_info_t *g_p_btcall_info;

//请不要扩大callring-buff的大小(最多4k)，空间与asqt共用
//不然会导致calling模式asqt不能工作或死机，其他模式没影响
uint8 __section__(".bss.callring_dac") g_btcall_callring_dac_buf[CALLRING_DAC_BUFFER_LENGTH];
uint8 __section__(".bss.callring_read") g_btcall_callring_read_buf[CALLRING_READ_BUFFER_LENGTH];
uint8 __section__(".bss.asqt_test_mem") g_btcall_asqt_test_buf[ASQT_TEST_BUFFER_LENGTH];


