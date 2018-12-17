/********************************************************************************
 *        Copyright(c) 2015-2016 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sbc encode api,结构体定义等。
 * 
 * 作者：kehaitao
 ********************************************************************************/
#ifndef _SBC_ENCODE_INTERFACE_H            //防止重定义
#define _SBC_ENCODE_INTERFACE_H

#include "psp_includes.h"
#include "case_independent.h"




typedef struct
{
    int input_buffer;
    int input_len;
    int bit_depth;
    int gain;
    int output_buffer;
    int output_len;
} sbc_encode_param_t;


/*1.定义提供的具体接口命令，这里是按键驱动统一提供的接口*/

typedef enum
{
    SBC_INIT = 0,
    SBC_FINISH,
    SBC_ENCODE,
    CMD_MAX
} sbc_encode_cmd_e;



typedef struct
{
	unsigned long flags;

	uint8 frequency;
	uint8 blocks;
	uint8 subbands;
	uint8 mode;
	uint8 allocation;
	uint8 bitpool;
	uint8 endian;

	void *priv;
	void *priv_alloc_base;
} sbc_obj_t;

extern void *sbc_encode_op_entry(void *param1, void *param2, void *param3, sbc_encode_cmd_e cmd)__FAR__;

/******************************************************************************/
/*!
 * \par  Description:
 *    send data from buffer to uart tx.
 * \param[in]    buf        data-buffer.
 * \param[in]    count      length of data to be sent.
 * \param[out]   none
 * \return       length of data to be sent.
 *****************************************************************************/
#define sbc_drv_encode_init(sbc_obj, flag)    (int)sbc_encode_op_entry((void*)(sbc_obj),(void*)(flag),(void*)0,SBC_INIT)

/******************************************************************************/
/*!
 *将bt controller唤醒
 * mode 0 :controller 根据自己实际情况是否进lps
 * mode 1 :controller 从lps退出
 *****************************************************************************/
#define bt_drv_encode_finish(sbc_obj)       (int)sbc_encode_op_entry((void*)(sbc_obj),(void*)0,(void*)0,SBC_FINISH)

/******************************************************************************/
/*!
 *初始化uart模块,下载bt controller工作所需要的patchcode.
 * mode            是否需要下载patchcode        1:需要下载;0:不需要下载
 * bt_work_mode    bt controller 后续的工作模式,BT_NORMAL_MODE;BQB_TEST_MODE
 ******************************************************************************/
#define bt_drv_sbc_encode(sbc_obj,encode_param,written) (int) sbc_encode_op_entry((void*)(sbc_obj),\
(void*)(encode_param),(void*)(written), SBC_ENCODE)


#endif

