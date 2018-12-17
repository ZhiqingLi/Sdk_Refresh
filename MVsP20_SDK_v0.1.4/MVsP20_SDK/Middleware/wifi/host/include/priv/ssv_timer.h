/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SSV_TIMER_H_
#define _SSV_TIMER_H_
#include "ssv_lib.h"

typedef void (*timer_handler)(void *, void *);


typedef enum
{
    EN_TMR_FREE,
    EN_TMR_CNT_DWN,
    EN_TMR_TICK_POST,
    EN_TMR_CANCEL,      
}EN_TMR_STATE;


typedef struct os_timer
{
    struct list_q tmr_list;
    u32             nMsgType;
    timer_handler   handler;
    void*           infombx;
    u32             nMTData0;
    u32             nMTData1;
    u32             msTimeout;
    u32             msRemian;
    EN_TMR_STATE    en_state;
} os_timer_st;

enum msgtype_tmr
{
    TMR_EVT_CREATE=0,
    TMR_EVT_CANCEL,
    TMR_EVT_FREE,
    TMR_EVT_FLUSH,
};

//Timer related
s32 os_create_timer(u32 ms, timer_handler handler, void *data1, void *data2, void* mbx);
s32 os_cancel_timer(timer_handler handler, u32 data1, u32 data2);
s32 os_free_timer(struct os_timer* free_tmr);
void os_timer_expired(void* evt);

#endif
