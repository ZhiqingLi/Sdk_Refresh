/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sbc编解码驱动，接口入口。
 * 作者：kehaitao
 ********************************************************************************/

#include "sbc_encode.h"

typedef void* (*sbc_op_i)(void*, void*, void*);
/*1.驱动内部实现的具体接口函数声明*/
typedef struct 
{
    sbc_op_i __sbc_init;
    sbc_op_i __sbc_finish;
    sbc_op_i __encode;
}block_sbc_operations;

/*2.定义接口操作集结构体，并进行初始化*/
block_sbc_operations blk_op =
{
    (sbc_op_i)sbc_init,
    (sbc_op_i)sbc_finish,
    (sbc_op_i)sbc_encode_entry,
};


