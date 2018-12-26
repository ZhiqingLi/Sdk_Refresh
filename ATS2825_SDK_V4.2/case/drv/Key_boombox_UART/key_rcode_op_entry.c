/*******************************************************************************
 *                              US280A
 *                            Module: Key driver
 *                 Copyright(c) 2003-2013 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       johnsen    2013-5-18 11:11    1.0             build this file
 *******************************************************************************/
/*!
 * \file     key_rcode_op_entry.c
 * \brief    key api entrance
 * \author   johnsen
 * \version 1.0
 * \date  2013-5-18 11:11
 *******************************************************************************/

#include "key_inner.h"

typedef void *(*key_op_i)(void *, void *, void *);
/*1.驱动内部实现的具体接口函数声明*/
typedef struct {
    key_op_i key_chargeget;
    key_op_i key_chargeset;
    key_op_i key_keytabaddr;
    key_op_i key_getholdstate;
    key_op_i key_speakcheck;
    key_op_i key_rcp_queue_init;
    key_op_i key_rcp_queue_init_heap;
    key_op_i key_rcp_queue_send;
    key_op_i key_rcp_queue_recv;
    key_op_i key_rcp_queue_flush;
    key_op_i key_rcp_queue_free;
    key_op_i key_rcp_queue_reply;
    key_op_i key_rcp_queue_is_empty;
    //for key tone
    key_op_i key_tone_init;
    key_op_i key_tone_exit;
    key_op_i key_tone_play;
} block_key_operations;

/*2.定义接口操作集结构体，并进行初始化*/
block_key_operations blk_op = {
    (key_op_i)key_inner_chargeget,
    (key_op_i)key_inner_chargeset,
    (key_op_i)key_inner_getkeytabaddress,
    (key_op_i)key_inner_holdcheck,
    (key_op_i)key_inner_speakcheck,
    (key_op_i)key_inner_rcp_queue_init,
    (key_op_i)key_inner_rcp_queue_init_heap,
    (key_op_i)key_inner_rcp_queue_send,
    (key_op_i)key_inner_rcp_queue_recv,
    (key_op_i)key_inner_rcp_queue_flush,
    (key_op_i)key_inner_rcp_queue_free,
    (key_op_i)key_inner_rcp_queue_reply,
    (key_op_i)key_inner_rcp_queue_is_empty,
    //for key tone
    (key_op_i)key_inner_keytone_play_init,
    (key_op_i)key_inner_keytone_play_exit,
    (key_op_i)key_inner_keytone_play_frame
};


