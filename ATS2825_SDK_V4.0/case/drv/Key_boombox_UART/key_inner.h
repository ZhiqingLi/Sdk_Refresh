/*
 ********************************************************************************
 *                       ACTOS
 *                  key board message define
 *
 *                (c) Copyright 2002-2013, Actions Co,Ld.
 *                        All Right Reserved
 *
 * File   : key.msa
 * By     : johnsen
 * Version: 1> v1.00     first version     2013-5-18 13:53
 ********************************************************************************
 ********************************************************************************
 */
#ifndef _KEY_INNER_H            //防止重定义
#define _KEY_INNER_H

//#include "psp_includes.h"
//#include "key_interface.h"
#include "key_uart.h"
//#include "gui_msg.h"
//#include "app_msg.h"


/*RCP堆空间分配结构体*/
typedef struct key_rcp_alloc_stru {
    void *address;
    uint32 size;
    struct key_rcp_alloc_stru *next;
} key_rcp_alloc_t;


/*! SPP命令队列 */
extern void *key_rcp_queue_p;

extern uint8 key_rcp_queue_readptr;

extern uint8 key_rcp_queue_writeptr;

extern uint8 key_rcp_queue_depth_empty;

extern uint8 key_rcp_queue_depth;

extern uint8 key_rcp_entity_head;

extern key_rcp_alloc_t head;

extern key_rcp_alloc_t tail;

//for scan data
extern int g_time_irq;

extern void status_scan(void);

extern bool key_inner_rcp_queue_init(uint8 queue_depth, uint8 head_size, void *null2);

//extern bool key_inner_rcp_queue_destroy(void* null0, void* null1, void* null2);

extern bool key_inner_rcp_queue_init_heap(void *heap_addr, uint32 heap_size, void *null2);

extern bool key_inner_rcp_queue_free(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2);

extern bool key_inner_rcp_queue_send(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2);

extern bool key_inner_rcp_queue_recv(rmt_ctrl_pkg_t *p_rcp, void *null1, void *null2);

extern bool key_inner_rcp_queue_flush(void *null0, void *null1, void *null2);

extern bool key_inner_rcp_queue_reply(uint8 mode, void *data, uint16 data_len);

extern bool key_inner_rcp_queue_is_empty(void *null0, void *null1, void *null2);

extern bool key_inner_rcp_queue_send_rcode(rmt_ctrl_pkg_t *p_rcp) __FAR__;

extern void *key_inner_rcp_malloc(uint32 num_bytes) __FAR__;

extern bool key_inner_rcp_free(void *alloc_addr) __FAR__;

extern uint8 key_inner_holdcheck(void *null0, void *null1, void *null2) __FAR__;

extern uint8 *key_inner_getkeytabaddress(void *null0, void *null1, void *null2)__FAR__;

extern uint8 *key_inner_speakcheck(uint8 onoff, void *null1, void *null2) __FAR__;

extern uint8 key_inner_chargeset(charge_control_e setting, charge_current_e current, battery_full_t param3) __FAR__;

extern charge_state_e key_inner_chargeget(void *null0, void *null1, void *null2) __FAR__;

extern int32 key_inner_keytone_play_init(void *null0, void *null1, void *null2) __FAR__;

extern int32 key_inner_keytone_play_exit(void *null0, void *null1, void *null2) __FAR__;

//play fram
extern int32 key_inner_keytone_play_frame(void *null0, void *null1, void *null2) __FAR__;

#endif/*_KEY_H*/
