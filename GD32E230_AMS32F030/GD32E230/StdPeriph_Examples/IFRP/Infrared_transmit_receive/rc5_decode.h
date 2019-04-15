/*!
    \file  rc5_decode.h
    \brief the header file of rc5 infrared decoding
    
    \version 2018-06-19, V1.0.0, firmware for GD32E230
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef RC5_DECODE_H
#define RC5_DECODE_H

#include "gd32e230.h"

/* rc5 frame structure */
typedef struct
{   
    __IO uint8_t field_bit;       /*!< field bit */   
    __IO uint8_t toggle_bit;      /*!< toggle bit field */
    __IO uint8_t address;         /*!< address field */ 
    __IO uint8_t command;         /*!< command field */ 
} rc5_frame_struct;

/* rc5 packet structure */
typedef struct
{
    __IO uint16_t data;          /*!< rc5 data */ 
    __IO uint8_t status;         /*!< rc5 status */
    __IO uint8_t last_bit;       /*!< rc5 last bit */
    __IO uint8_t bit_count;      /*!< rc5 bit count */
} trc5_packet_struct;

/* rc5 last bit type enum */
enum rc5_last_bit_type
{
    RC5_ZER,
    RC5_ONE,
    RC5_NAN,
    RC5_INV
};

typedef enum rc5_last_bit_type trc5_last_bit_type;

#define RC5_1T_TIME                          0x00
#define RC5_2T_TIME                          0x01
#define RC5_WRONG_TIME                       0xFF
#define RC5_TIME_OUT_US                      3600

/* half bit period */
#define RC5_T_US                             900

/* tolerance time */
#define RC5_T_TOLERANCE_US                   270
#define RC5_NUMBER_OF_VALID_PULSE_LENGTH     2

/* total bits */
#define RC5_PACKET_BIT_COUNT                 13

/* packet struct for reception */
#define RC5_PACKET_STATUS_EMPTY              (uint16_t)(1<<0)

void menu_rc5decode_func(void);
void rc5_decode_deinit(void);
void rc5_decode_init(void);
void rc5_decode(rc5_frame_struct *rc5_frame);
void rc5_reset_packet(void);
void rc5_data_sampling(uint16_t raw_pulse_length, uint8_t edge);

#endif /* RC5_DECODE_H */
