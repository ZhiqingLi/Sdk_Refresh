#ifndef __LED_FUNC_H__
#define __LED_FUNC_H__

#include <stdio.h>
#include <stdint.h>
#include "type.h"

uint8_t revert_one_byte(uint8_t inByte);

void revert_every_bytes(uint8_t *data, uint16_t len);

void inversion_every_bit(uint8_t* input, uint32_t len);

void inversion_every_bit_dw(uint32_t* input, uint32_t len);

void revert_one_string(uint8_t *data, uint16_t len);

void revert_one_string_dw(uint32_t *data, uint16_t len);

/**
* extend one bit to (byte, half-word, word)
* width = 1 for 8bit, 2 for 16bit, 4 for 32bit
* flag = (0~31)
* 
* byte 0x55 to 	0x00000000
*				0x00000010
*				0x00000000
*				0x00000010
*				0x00000000
*				0x00000010
*				0x00000000
*				0x00000010
				  ||||||||
* if width=4,flag=1
*/
bool extend_res_fr_s2p(uint8_t *src, uint16_t len, void *dst, uint8_t width, uint8_t flag);

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
bool extend_res_fr_s2ms(void *src, uint16_t len, void *dst, uint8_t width, uint8_t intv, uint8_t level);

//now we just extend on col
/* col_src = 4		col_des = 12
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0      
* 1 1 1 1  to 1 1 1 1 0 0 0 0 0 0 0 0      
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0      
* 1 1 1 1     1 1 1 1 0 0 0 0 0 0 0 0      
*/
bool merge_matrix_by_size(	uint8_t *src, uint16_t row_src, uint16_t col_src,
							uint8_t *des, uint16_t row_des, uint16_t col_des);

bool merge_matrix_by_size_dw(	uint32_t *src, uint16_t row_src, uint16_t col_src,
								uint32_t *des, uint16_t row_des, uint16_t col_des);

bool merge_matrix_by_size_offset(	uint8_t *src, uint16_t row_src, uint16_t col_src,
									uint8_t *des, uint16_t row_des, uint16_t col_des,
									uint32_t offset);

bool merge_matrix_by_size_offset_dw(	uint32_t *src, uint16_t row_src, uint16_t col_src,
										uint32_t *des, uint16_t row_des, uint16_t col_des,
										uint32_t offset);

//FALSE:from right to left
//TRUE:from left to right
void move_line_l2r(uint8_t* data, uint16_t r_num, uint16_t c_num, bool direct);

//FALSE:from right to left
//TRUE:from left to right
void move_line_l2r_dw(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct);

void move_line_l2r_dw_step(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step);

//FALSE:from down to up
//TRUE:from up to down
//c_num and r_num is bytes
void move_line_u2d(uint8_t* data, uint8_t r_num, uint8_t c_num, bool direct);

void move_line_l2r_lacc_fullstep_dw_nobuf(uint32_t* data, uint16_t r_num, uint16_t c_num, bool direct, uint8_t step);

uint32_t fetch_one_serial_data(	uint8_t* data, //input data as material
									uint16_t r_intv, //74hc595 number in row
									uint16_t c_intv, //74hc595 number in col
									uint8_t p_scan, //scan gap,if 1/4 scan input 4
									bool entry_drct,//FALSE=from right, TRUE=from left
									bool entry_pst,	//FALSE=from down, TRUE=from up
									bool scan_drct,	//FALSE=from up, TRUE=from down
									uint8_t* output);//output data for display buffer


/*
* DWORD version
*
*/
uint32_t fetch_one_serial_data_dw(	uint32_t* data, //input data as material
									uint16_t r_intv, //74hc595 number in row
									uint16_t c_intv, //74hc595 number in col
									uint8_t p_scan, //scan gap,if 1/4 scan input 4
									bool entry_drct,//FALSE=from right, TRUE=from left
									bool entry_pst,	//FALSE=from down, TRUE=from up
									bool scan_drct,	//FALSE=from up, TRUE=from down
									uint8_t* output);//output data for display buffer

/*data external*/
extern uint8_t number0[];
extern uint8_t number1[];
extern uint8_t number2[];
extern uint8_t number3[];
extern uint8_t number4[];
extern uint8_t number5[];
extern uint8_t number6[];
extern uint8_t number7[];
extern uint8_t number8[];
extern uint8_t number9[];
extern uint8_t colon[];
extern uint32_t welcome_dw[64];
extern uint8_t welcome[];
extern uint8_t some_frame_4[];


#endif /*__LED_FUNC_H__*/

