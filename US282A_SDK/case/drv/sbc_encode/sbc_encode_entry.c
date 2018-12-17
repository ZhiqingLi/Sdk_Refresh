/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sbc编解码驱动，解码入口。
 * 作者：kehaitao
 ********************************************************************************/
#include "sbc_encode_interface.h"
#include "sbc_encode.h"

int sbc_encode_entry(sbc_t *sbc,void *param,ssize_t *written)
{
	sbc_encode_param_t * sbc_encode_param;
	
	sbc_encode_param = (sbc_encode_param_t *) param;
/*	
	libc_print("b",sbc_encode_param->input_buffer,2);
	libc_print("b",sbc_encode_param->input_len,2);
	libc_print("b",sbc_encode_param->bit_depth,2);
	libc_print("b",sbc_encode_param->gain,2);
	libc_print("b",sbc_encode_param->output_buffer,2);
	libc_print("b",sbc_encode_param->output_len,2);
*/			
	return sbc_encode(sbc, sbc_encode_param->input_buffer, sbc_encode_param->input_len, sbc_encode_param->bit_depth, sbc_encode_param->gain,
			sbc_encode_param->output_buffer, sbc_encode_param->output_len, written);
//  return sbc_encode(sbc,0x9fc18580,512,16,4728462,0x9fc16400,77,written);
/*  
      p_temp.input_buffer = 0x9fc18580;
    p_temp.input_len = 512;
    p_temp.bit_depth = 16;
    p_temp.gain = 4728462;
    p_temp.output_buffer = 0x9fc16400;
    p_temp.output_len = 77;*/
}