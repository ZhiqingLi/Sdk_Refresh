
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*
*      <author>       <time>           
*       liminxian  2014-8-20    
*       Wekan   2015-3-27  review    
*******************************************************************************/

#ifndef _ADJUST_FREQ_INC_H_
#define _ADJUST_FREQ_INC_H_

#include "psp_includes.h"
#include "key_interface.h"


#define ADJUST_ID_MAX 12
#define ADJUST_RUN_TASK_MAX 8
#define OS_TASK_PRIO_MAX 14

typedef struct
{    
    uint8 freq_a_level:4;  //misp
    uint8 freq_b_level:4;  //dsp
    uint8 freq_a_level_sn:4;    //misp level_table    
    uint8 freq_b_level_sn:4;    //dsp level_table
}adjust_freq_t; 

#endif //_ADJUST_FREQ_INC_H_

