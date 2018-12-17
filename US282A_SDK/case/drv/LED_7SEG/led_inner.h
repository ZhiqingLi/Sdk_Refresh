
/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *
 *      <author>       <time>           
 *       Wekan       2015-3-26pm            
 *******************************************************************************/

#ifndef _LCD_INNER_H
#define _LCD_INNER_H

#include "psp_includes.h"
#include "led_driver.h"

extern uint8 cur_disval[8];
extern bool Led_sleep_falg;


extern void wait_for_trans(void);


extern void hdrv_hard_init(void) __FAR__;

extern void hdrv_data_send(uint32* dis_val_ptr) __FAR__;



#endif //_LCD_INNER_H
