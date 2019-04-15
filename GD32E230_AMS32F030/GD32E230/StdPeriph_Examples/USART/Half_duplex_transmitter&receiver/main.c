/*!
    \file  main.c
    \brief USART HalfDuplex transmitter and receiver
    
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

#include "gd32e230.h"
#include <stdio.h>
#include "gd32e230c_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE0   (ARRAYNUM(transmitter_buffer0) - 1)
#define TRANSMIT_SIZE1   (ARRAYNUM(transmitter_buffer1) - 1)

uint8_t transmitter_buffer0[] = "\n\ra usart half-duplex test example!\n\r";
uint8_t transmitter_buffer1[] = "\n\ra usart half-duplex test example!\n\r";
uint8_t receiver_buffer0[TRANSMIT_SIZE1];
uint8_t receiver_buffer1[TRANSMIT_SIZE0];
uint8_t transfersize0 = TRANSMIT_SIZE0;
uint8_t transfersize1 = TRANSMIT_SIZE1;
__IO uint8_t txcount0 = 0; 
__IO uint16_t rxcount0 = 0; 
__IO uint8_t txcount1 = 0; 
__IO uint16_t rxcount1 = 0; 
ErrStatus state1 = ERROR;
ErrStatus state2 = ERROR;

ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) ;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    /* enable USART and GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_USART1);
    
    /* configure the USART0 Tx pin and USART1 Tx pin */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    /* configure USART0 Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    /* configure USART1 Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    
    /* USART0 and USART1 baudrate configuration */
    usart_baudrate_set(USART0, 115200);
    usart_baudrate_set(USART1, 115200);

    /* Enable USART0 Half Duplex Mode*/
    usart_halfduplex_enable(USART0);
    /* Enable USART1 Half Duplex Mode*/
    usart_halfduplex_enable(USART1);
    /* configure USART transmitter and receiver*/
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    /* enable USART */
    usart_enable(USART0);
    usart_enable(USART1);
    
    /* USART0 transmit and USART1 receive*/
    usart_data_receive(USART1);
    while(transfersize0--)
    {
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE) );
        usart_data_transmit(USART0, transmitter_buffer0[txcount0++]);
 
        while(RESET == usart_flag_get(USART1, USART_FLAG_RBNE));
        /* store the received byte in the receiver_buffer1 */
        receiver_buffer1[rxcount0++] = usart_data_receive(USART1);
    }

    usart_data_receive(USART0);
    /* USART1 transmit and USART0 receive*/
    while(transfersize1--)
    { 
        /* wait until end of transmit */
        while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
        usart_data_transmit(USART1, transmitter_buffer1[txcount1++]);

        while(RESET == usart_flag_get(USART0, USART_FLAG_RBNE));
        /* store the received byte in the receiver_buffer0 */
        receiver_buffer0[rxcount1++] = usart_data_receive(USART0);
    }
  
    /* compare the received data with the send ones */
    state1 = memory_compare(transmitter_buffer0, receiver_buffer1, TRANSMIT_SIZE0);
    state2 = memory_compare(transmitter_buffer1, receiver_buffer0, TRANSMIT_SIZE1);
    if(SUCCESS == state1){
        /* if the data transmitted from USART0 and received by USART1 are the same */
        gd_eval_led_on(LED1);
    }else{
        /* if the data transmitted from USART0 and received by USART1 are not the same */
        gd_eval_led_off(LED1);
    }
    if(SUCCESS == state2){
        /* if the data transmitted from USART1 and received by USART0 are the same */
        gd_eval_led_on(LED2);
    }else{
        /* if the data transmitted from USART1 and received by USART0 are not the same */
        gd_eval_led_off(LED2); 
    }
    while (1)
    {
    }
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while(length--){
        if (*src++ != *dst++){
            return ERROR;
        }
    }
    return SUCCESS;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t) ch);
    while(RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}
