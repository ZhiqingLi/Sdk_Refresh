#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "type.h"
#include "dpa.h"
#include "dma.h"
#include "clk.h"

uint32_t dpa_buffer[1024 * 2] = {0};


uint8_t revert_one_byte(uint8_t inByte)
{
	int i = 0;
	uint8_t ret = 0x00;
	for(i = 0; i < 8; i++)
	{
		ret |= ((inByte & (0x01 << i)) >> i) << (8 - i - 1);
	}
	return ret;
}

void revert_every_bytes(uint8_t *data, uint16_t len)
{
	int i = 0;
	for(i = 0; i < len; i++)
		data[i] = revert_one_byte(data[i]);
}

void inversion_every_bit(uint8_t* input, uint32_t len)
{
	uint32_t i;
	for(i = 0; i < len; i++)
		input[i] = ~input[i];
}
/*
 * 将数据按位取反
 *
 */
void inversion_every_bit_dw(uint32_t* input, uint32_t len)
{
	uint32_t i;
	for(i = 0; i < len; i++)
		input[i] = ~input[i];
}

void revert_one_string(uint8_t *data, uint16_t len)
{
	int i = 0;
	uint8_t temp;
	for(i = 0; i < len / 2; i++)
	{
		temp = data[i];
		data[i] = data[len - i - 1];
		data[len - i - 1] = temp;
	}
}
/*
 * 将一端地址的数据以字节为单位，将存放地址倒序
 *
 */
void revert_one_string_dw(uint32_t *data, uint16_t len)
{
	int i = 0;
	uint32_t temp;
	for(i = 0; i < len / 2; i++)
	{
		temp = data[i];
		data[i] = data[len - i - 1];
		data[len - i - 1] = temp;
	}
}


bool extend_res_fr_s2p(uint8_t *src, uint16_t len, void *dst, uint8_t width, uint8_t flag)
{
	uint16_t i, j;
	uint8_t* ptr = (uint8_t*)dst;

	for(i = 0; i < len; i++)
	{
		for(j = 0; j < 8; j++)
		{
			uint32_t filter = 0x01 << j;
			if(width == 1)
				*(uint8_t*)ptr |= (((uint32_t)src[i] & filter) >> j) << flag;
			else if(width == 2)
				*(uint16_t*)ptr |= (((uint32_t)src[i] & filter) >> j) << flag;
			else if(width == 4)
				*(uint32_t*)ptr |= (((uint32_t)src[i] & filter) >> j) << flag;
			ptr += width;
		}
	}
	return TRUE;
}

/**
* extend serial data to multi-output serial data
* the width of src and dst must be the same
* width = 1,2,4
* intv = 2,3,4
* level = 0,1,2,3
* byte 0x01 0x02 0x03 0x04  to 	0x00
*								0x04
*								0x00
*								0x00
*								0x03
*								0x00
*								0x00
*								0x02
*								0x00
*								0x00 --->led_data_port2
*								0x01 --->led_data_port1
*								0x00 --->led_data_port0
*if width=1,intv=3,level=1
*/
bool extend_res_fr_s2ms(void *src, uint16_t len, void *dst, uint8_t width, uint8_t intv, uint8_t level)
{
	uint16_t i;
	uint8_t *ptr_s = (uint8_t*)src, *ptr_d = (uint8_t*)dst;

	for(i = 0; i < len; i++)
	{
		if(width == 1)
			*((uint8_t*)ptr_d + level) = *(uint8_t*)ptr_s;
		else if(width == 2)
			*((uint16_t*)ptr_d + level) = *(uint16_t*)ptr_s;
		else if(width == 4)
			*((uint32_t*)ptr_d + level) = *(uint32_t*)ptr_s;
		ptr_s += width;
		ptr_d += width * intv;
	}
	return TRUE;
}

//now we just extend on col
/* col_src = 4		col_des = 12
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0
* 1 1 1 1  to 1 1 1 1 0 0 0 0 0 0 0 0
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0
*/
bool merge_matrix_by_size(	uint8_t *src, uint16_t row_src, uint16_t col_src,
                            uint8_t *des, uint16_t row_des, uint16_t col_des)
{
	uint16_t i, j;

	if(col_des < col_src || row_des < row_src)
		return FALSE;

	for(i = 0; i < row_src; i++)
	{
		for(j = 0; j < col_src; j++)
		{
			des[i * col_des + j] |= src[i * col_src + j]; //do OR operation
		}
	}

	return TRUE;
}
/*
 * 将src地址数据复制到des里去
 *
 */
bool merge_matrix_by_size_dw(	uint32_t *src, uint16_t row_src, uint16_t col_src,
                                uint32_t *des, uint16_t row_des, uint16_t col_des)
{
	uint16_t i, j;

	if(col_des < col_src || row_des < row_src)
		return FALSE;

	for(i = 0; i < row_src; i++)
	{
		for(j = 0; j < col_src; j++)
		{
			des[i * col_des + j] |= src[i * col_src + j]; //do OR operation
		}
	}

	return TRUE;
}
/*
 * 将src地址数据复制到des里去，注意数据按照偏移地址和板子模型进行位置重组
 *
 */
bool merge_matrix_by_size_offset(	uint8_t *src, uint16_t row_src, uint16_t col_src,
                                    uint8_t *des, uint16_t row_des, uint16_t col_des,
                                    uint32_t offset)
{
	uint16_t i, j;

	if(col_des < col_src || row_des < row_src)
		return FALSE;

	for(i = 0; i < row_src; i++)
	{
		for(j = 0; j < col_src; j++)
		{
			des[i * col_des + j + offset] |= src[i * col_src + j]; //do OR operation
		}
	}

	return TRUE;
}

bool merge_matrix_by_size_offset_dw(	uint32_t *src, uint16_t row_src, uint16_t col_src,
                                        uint32_t *des, uint16_t row_des, uint16_t col_des,
                                        uint32_t offset)
{
	uint16_t i, j;

	if(col_des < col_src || row_des < row_src)
		return FALSE;

	for(i = 0; i < row_src; i++)
	{
		for(j = 0; j < col_src; j++)
		{
			des[i * col_des + j + offset] |= src[i * col_src + j]; //do OR operation
		}
	}

	return TRUE;
}


//FALSE:from right to left
//TRUE:from left to right
void move_line_l2r(uint8_t* data, uint16_t r_num, uint16_t c_num, bool direct)
{
	int i, j;
	uint8_t temp_bbit;
	uint8_t *pp = data;

	if(direct)
	{
		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = c_num - 1; i >= 0; i--)
			{
				if(i == c_num - 1)
				{
					temp_bbit = (*(pp + i) & 0x01) << 7;
					*(pp + i) = *(pp + i) >> 1;
				}
				else
				{
					*(pp + i + 1) |= (*(pp + i) & 0x01) << 7;
					*(pp + i) = *(pp + i) >> 1;
					if(i == 0)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}

			pp += c_num;
		}
	}
	else
	{
		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = 0; i < c_num; i++)
			{
				if(i == 0)
				{
					temp_bbit = (*(pp + i) & 0x80) >> 7;
					*(pp + i) = *(pp + i) << 1;
				}
				else
				{
					*(pp + i - 1) |= (*(pp + i) & 0x80) >> 7;
					*(pp + i) = *(pp + i) << 1;
					if(i == c_num - 1)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}
			pp += c_num;
		}
	}
}

//FALSE:from right to left
//TRUE:from left to right
void move_line_l2r_dw(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct)
{
	int i, j;
	uint32_t temp_bbit;
	uint32_t *pp = data;

	if(direct)
	{
		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = c_num - 1; i >= 0; i--)
			{
				if(i == c_num - 1)
				{
					temp_bbit = (*(pp + i) & 0x80000000) >> 31;
					*(pp + i) = *(pp + i) << 1;
				}
				else
				{
					*(pp + i + 1) |= (*(pp + i) & 0x80000000) >> 31;
					*(pp + i) = *(pp + i) << 1;
					if(i == 0)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}

			pp += c_num;
		}
	}
	else
	{
		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = 0; i < c_num; i++)
			{
				if(i == 0)
				{
					temp_bbit = (*(pp + i) & 0x00000001) << 31;
					*(pp + i) = *(pp + i) >> 1;
				}
				else
				{
					*(pp + i - 1) |= (*(pp + i) & 0x00000001) << 31;
					*(pp + i) = *(pp + i) >> 1;
					if(i == c_num - 1)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}
			pp += c_num;
		}
	}
}

void move_line_l2r_dw_step(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step)
{
	int i, j;
	uint32_t temp_bbit;
	uint32_t *pp = data;

	if(direct)
	{
		uint32_t maskm = 0x00;
		for(i = 0; i < step; i++)
			maskm |= (0x80000000 >> i);

		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = c_num - 1; i >= 0; i--)
			{
				if(i == c_num - 1)
				{
					temp_bbit = (*(pp + i) & maskm) >> (32 - step);
					*(pp + i) = *(pp + i) << step;
				}
				else
				{
					*(pp + i + 1) |= (*(pp + i) & maskm) >> (32 - step);
					*(pp + i) = *(pp + i) << step;
					if(i == 0)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}

			pp += c_num;
		}
	}
	else
	{
		uint32_t maskm = 0x00;
		for(i = 0; i < step; i++)
			maskm |= (0x00000001 << i);

		for(j = 0; j < r_num; j++)
		{
			temp_bbit = 0;
			for(i = 0; i < c_num; i++)
			{
				if(i == 0)
				{
					temp_bbit = (*(pp + i) & maskm) << (32 - step);
					*(pp + i) = *(pp + i) >> step;
				}
				else
				{
					*(pp + i - 1) |= (*(pp + i) & maskm) << (32 - step);
					*(pp + i) = *(pp + i) >> step;
					if(i == c_num - 1)
					{
						*(pp + i) |= temp_bbit;
					}
				}
			}
			pp += c_num;
		}
	}
}

//FALSE:from down to up
//TRUE:from up to down
//c_num and r_num is bytes
void move_line_u2d(uint8_t* data, uint8_t r_num, uint8_t c_num, bool direct)
{
	int i, j;
	uint8_t temp[64] = {0};
	if(direct)
	{
		for(j = 0; j < c_num; j++)
		{
			for(i = 0; i < r_num; i++)
			{
				temp[i] = data[i * c_num + j];
			}
			for(i = 0; i < r_num; i++)
			{
				if(i == r_num - 1)
					data[j] = temp[i];
				else
					data[(i + 1)*c_num + j] = temp[i];
			}
		}
	}
	else
	{
		for(j = 0; j < c_num; j++)
		{
			for(i = 0; i < r_num; i++)
			{
				temp[i] = data[i * c_num + j];
			}

			for(i = 0; i < r_num; i++)
			{
				if(i == r_num - 1)
					data[i * c_num + j] = temp[0];
				else
					data[i * c_num + j] = temp[i + 1];
			}
		}
	}
}

uint32_t fetch_one_serial_data(	uint8_t* data, //input data as material
                                uint16_t r_intv, //74hc595 number in row
                                uint16_t c_intv, //74hc595 number in col
                                uint8_t p_scan, //scan gap,if 1/4 scan input 4
                                bool entry_drct,//FALSE=from right, TRUE=from left
                                bool entry_pst,	//FALSE=from down, TRUE=from up
                                bool scan_drct,	//FALSE=from up, TRUE=from down
                                uint8_t* output)//output data for display buffer
{
	int i, j, c;
	uint32_t offset = 0;

	for((scan_drct ? (j = p_scan - 1) : (j = 0)); (scan_drct ? (j >= 0) : (j < p_scan)); (scan_drct ? (j--) : (j++)))
	{
		for((entry_drct ? (i = c_intv - 1) : (i = 0)); (entry_drct ? (i >= 0) : (i < c_intv)); (entry_drct ? (i--) : (i++)))
		{
			for((entry_pst ? (c = r_intv - 1) : (c = 0)); (entry_pst ? (c >= 0) : (c < r_intv)); (entry_pst ? (c--) : (c++)))
			{
				if(entry_drct)
					output[offset++] = data[c * p_scan * c_intv + j * c_intv + i];
				else
					output[offset++] = revert_one_byte(data[c * p_scan * c_intv + j * c_intv + i]);
			}
		}
	}
	return offset;
}


/*
* 	将[r_intv*scan_drct]*[c_intv/4]的32位数据矩阵转化为[scan_drct]*[c_intv*4]8位数据矩阵
* 	* data		传入数据
* 	r_intv		行所占595个数
* 	c_intv		列所占595个数
* 	p_scan		扫描方式 单行扫描、1/2扫描、1/4扫描、1/8扫描、1/16扫描
*	entry_drct 	0左低右高 1左高右边低
*	entry_pst	0从下往上扫描 1从上往下扫描
*	* output	传出数据
*/
uint32_t fetch_one_serial_data_dw(	uint32_t* data, //input data as material
                                    uint16_t r_intv, //74hc595 number in row
                                    uint16_t c_intv, //74hc595 number in col
                                    uint8_t p_scan, //scan gap,if 1/4 scan input 4
                                    bool entry_drct,//FALSE=from right, TRUE=from left
                                    bool entry_pst,	//FALSE=from down, TRUE=from up
                                    bool scan_drct,	//FALSE=from up, TRUE=from down
                                    uint8_t* output)//output data for display buffer
{
	int i, j, c;
	uint32_t offset = 0;
	uint8_t *ptr8bit = (uint8_t*)data;
	for((scan_drct ? (j = 0) : (j = p_scan - 1)); (scan_drct ? (j < p_scan) : (j >= 0)); (scan_drct ? (j++) : (j--)))
	{
		for((entry_drct ? (i = 0) : (i = c_intv - 1)); (entry_drct ? (i < c_intv) : (i >= 0)); (entry_drct ? (i++) : (i--)))
		{
			for((entry_pst ? (c = 0) : (c = r_intv - 1)); (entry_pst ? (c < r_intv) : (c >= 0)); (entry_pst ? (c++) : (c--)))
			{
				if(entry_drct)
					output[offset++] = ptr8bit[c * p_scan * c_intv + j * c_intv + i];
				else
					output[offset++] = revert_one_byte(ptr8bit[c * p_scan * c_intv + j * c_intv + i]);
			}
		}
	}
	return offset;
}

void dpa_dma_cfg(uint32_t *SrcAddr, uint32_t *DstAddr, uint16_t SrcBufLen, uint16_t DstBufLen)
{
	Clock_ModuleEnable(DMA_CLK_EN);
	Clock_ModuleEnable(DPA_CLK_EN);

	DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 0);
	DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 0);
	
	DMA_BlockConfig(PERIPHERAL_ID_DPA_SRC);
	DMA_BlockConfig(PERIPHERAL_ID_DPA_DST);
	
	DMA_BlockBufSet(PERIPHERAL_ID_DPA_SRC, SrcAddr, SrcBufLen);
	DMA_BlockBufSet(PERIPHERAL_ID_DPA_DST, DstAddr, DstBufLen);

	DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 1);
	DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 1);
}


//lacc_buffer is word
void move_line_l2r_dpa(uint8_t* data, uint16_t r_num, uint16_t c_num, bool direct)
{
	int i;
	uint8_t *pp = data;

	uint8_t src_offset = 0, des_offset = 0;

	if(direct)
		des_offset = 1;
	else
		src_offset = 1;

	for(i = 0; i < r_num; i++)
	{
		DPA_Stop();
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 0);
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 0);

		revert_one_string(pp, c_num);

		//LedcAccDmaConfig((uint32_t)pp, 			DMA_DST_DWIDTH_WORD, c_num + src_offset * 4, 1);
		//LedcAccDmaConfig((uint32_t)lacc_buffer, DMA_DST_DWIDTH_WORD, c_num + des_offset * 4, 0);
		dpa_dma_cfg((uint32_t*)pp, (uint32_t*)dpa_buffer, c_num + src_offset * 4, c_num + des_offset * 4);
		
		DPA_Config(DPA_FUNC_MODE_BIT_SHIFT, DPA_REVERSE_MODE_CONTINOUS, src_offset, des_offset);
		DPA_Start();

		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT);
		DPA_Stop();

		if(direct)
			dpa_buffer[0] |= dpa_buffer[c_num] & 0x01;
		else
			dpa_buffer[c_num - 1] |= (pp[0] & 0x01) << 7;

		__builtin_memcpy(pp, dpa_buffer, c_num);
		revert_one_string(pp, c_num);

		pp += c_num;
	}
}

void move_line_l2r_dpa_fullstep(uint8_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step)
{
	int i, j;
	uint32_t *pp = (uint32_t*)data;

	uint8_t src_offset = 0, des_offset = 0;

	if(direct)
		des_offset = step;
	else
		src_offset = step;

	for(i = 0; i < r_num; i++)
	{
		DPA_Stop();
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 0);
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 0);

		revert_one_string((uint8_t*)pp, c_num);

		//LedcAccDmaConfig((uint32_t)pp, 			DMA_DST_DWIDTH_WORD, c_num + (src_offset ? 4 : 0), 1);
		//LedcAccDmaConfig((uint32_t)lacc_buffer, DMA_DST_DWIDTH_WORD, c_num + (des_offset ? 4 : 0), 0);
		dpa_dma_cfg((uint32_t*)pp, (uint32_t*)dpa_buffer, c_num + (src_offset ? 4 : 0), c_num + (des_offset ? 4 : 0));

		DPA_Config(DPA_FUNC_MODE_BIT_SHIFT, DPA_REVERSE_MODE_CONTINOUS, src_offset, des_offset);
		DPA_Start();

		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT);
		DPA_Stop();

		if(direct)
		{
			uint32_t maskm = 0x00;
			for(j = 0; j < des_offset; j++)
				maskm |= ((uint32_t)0x01) << j;
			dpa_buffer[0] |= dpa_buffer[c_num / 4] & maskm;
		}
		else
		{
			uint32_t maskm = 0x00;
			for(j = 0; j < src_offset; j++)
				maskm |= ((uint32_t)0x01) << j;
			dpa_buffer[c_num / 4 - 1] |= (pp[0] & maskm) << (32 - src_offset);
		}

		__builtin_memcpy(pp, dpa_buffer, c_num);
		revert_one_string((uint8_t*)pp, c_num);

		pp += c_num / 4;
	}
}

void move_line_l2r_dpa_fullstep_dw(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step)
{
	int i, j;
	uint32_t *pp = data;

	uint8_t src_offset = 0, des_offset = 0;

	if(direct)
		des_offset = step;
	else
		src_offset = step;

	for(i = 0; i < r_num; i++)
	{
		uint32_t maskm = 0x00;

		memset(dpa_buffer, 0x00, sizeof(dpa_buffer));

		DPA_Stop();
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 0);
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 0);

		//LedcAccDmaConfig((uint32_t)pp, 			DMA_DST_DWIDTH_WORD, c_num + (src_offset ? 4 : 0), 1);
		//LedcAccDmaConfig((uint32_t)lacc_buffer, DMA_DST_DWIDTH_WORD, c_num + (des_offset ? 4 : 0), 0);
		dpa_dma_cfg((uint32_t*)pp, (uint32_t*)dpa_buffer, c_num + (src_offset ? 4 : 0), c_num + (des_offset ? 4 : 0));

		DPA_Config(DPA_FUNC_MODE_BIT_SHIFT, DPA_REVERSE_MODE_CONTINOUS, src_offset, des_offset);
		DPA_Start();

		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT);
		DPA_Stop();

		if(direct)
		{
			for(j = 0, maskm = 0; j < des_offset; j++)
				maskm |= ((uint32_t)0x01) << j;
			dpa_buffer[0] |= dpa_buffer[c_num / 4] & maskm;
		}
		else
		{
			for(j = 0, maskm = 0; j < src_offset; j++)
				maskm |= ((uint32_t)0x80000000) >> j;
			dpa_buffer[c_num / 4 - 1] &= ~maskm;
			for(j = 0, maskm = 0; j < src_offset; j++)
				maskm |= ((uint32_t)0x00000001) << j;
			dpa_buffer[c_num / 4 - 1] |= (pp[0] & maskm) << (32 - src_offset);
		}

		__builtin_memcpy(pp, dpa_buffer, c_num);

		pp += c_num / 4;
	}
}

/*
* src_addr and dst_addr are the same ADDRESS
* 将每一行数据按位向左或向右移动
* 	* data	需移动的数据矩阵
* 	r_num	行
* 	c_num	列
* 	direct	移动方向，0向右移动数据，1向左移动数据
* 	step	数据移动的bit量
* uint32_t* data
*/
void move_line_l2r_dpa_fullstep_dw_nobuf(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step)
{
	int i, j;
	uint32_t *pp = data, temp1 = 0, temp2 = 0;

	uint8_t src_offset = 0, dst_offset = 0;

	if(step > 31)
		return;

	if(direct)
		dst_offset = step;
	else
		src_offset = step;

	for(i = 0; i < r_num; i++)
	{
		//LACC will overwirte pp[c_num/4]
		temp1 = pp[c_num / 4];
		temp2 = pp[0];

		DPA_Stop();
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_SRC, 0);
		DMA_ChannelEnable(PERIPHERAL_ID_DPA_DST, 0);

		//LedcAccDmaConfig((uint32_t)pp, DMA_DST_DWIDTH_WORD, c_num + (src_offset ? 4 : 0), 1);
		//LedcAccDmaConfig((uint32_t)pp, DMA_DST_DWIDTH_WORD, c_num + (dst_offset ? 4 : 0), 0);
		//设置LACC源地址目标地址
		dpa_dma_cfg((uint32_t*)pp, (uint32_t*)pp, c_num + (src_offset ? 4 : 0), c_num + (dst_offset ? 4 : 0));
		
		DPA_Config(DPA_FUNC_MODE_BIT_SHIFT, DPA_REVERSE_MODE_CONTINOUS, src_offset, dst_offset);
		DPA_Start();

		while(!DMA_InterruptFlagGet(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT));
		DMA_InterruptFlagClear(PERIPHERAL_ID_DPA_DST, DMA_DONE_INT);
		DPA_Stop();

		if(direct)
		{
			uint32_t maskm = 0x00;
			for(j = 0, maskm = 0; j < dst_offset; j++)
				maskm |= ((uint32_t)0x01) << j;
			pp[0] &= ~maskm;
			pp[0] |= pp[c_num / 4] & maskm;
			pp[c_num / 4] = temp1;
		}
		else
		{
			uint32_t maskm = 0x00;
			for(j = 0, maskm = 0; j < src_offset; j++)
				maskm |= ((uint32_t)0x80000000) >> j;
			pp[c_num / 4 - 1] &= ~maskm;
			for(j = 0, maskm = 0; j < src_offset; j++)
				maskm |= ((uint32_t)0x00000001) << j;
			pp[c_num / 4 - 1] |= (temp2 & maskm) << (32 - src_offset);
			pp[c_num / 4] = temp1;
		}

		pp += c_num / 4;
	}
}

#define     REG_RNG_CTRL                   (*(volatile unsigned long *) 0x40021044)
#define     ADR_RNG_CTRL                                               (0x40021044)
#define     MASK_RNG_START                 (0x1)
#define     MASK_RNG_READY                 (0x2)
#define     MASK_RNG_CLR                   (0x4)
#define     BIAS_RNG_START                 (0)
#define     BIAS_RNG_READY                 (1)
#define     BIAS_RNG_CLR                   (2)
#define     REG_RNG_D                      (*(volatile unsigned long *) 0x40021048)
#define     ADR_RNG_D                                                  (0x40021048)
uint32_t DPARandom(uint32_t max)
{
	REG_RNG_CTRL |= MASK_RNG_CLR;
	REG_RNG_CTRL |= MASK_RNG_START;
	while(!(REG_RNG_CTRL & MASK_RNG_READY));

	return REG_RNG_D;
}




