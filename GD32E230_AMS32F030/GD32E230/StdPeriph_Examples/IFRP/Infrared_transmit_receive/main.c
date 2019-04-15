/*!
    \file  main.c
    \brief Infrared_tramsmit_receive
    
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
#include "rc5_decode.h"
#include "rc5_encode.h"
#include "ir_decode.h"
#include "systick.h"
#include "gd32e230c_eval.h"

rc5_frame_struct rc5_frame;

extern uint8_t address_index;
extern uint8_t instruction_index;
extern rc5_ctrl_enum rc5_ctrl1;

/* rc5 frame state */ 
extern __IO status_yes_or_no rc5_frame_received; 
extern __IO uint8_t send_operation_completed;

void test_status_led_init(void);
void flash_led(int times);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* init test status LED */ 
    test_status_led_init();

    /* flash LED for test */
    flash_led(1);

    /* enable the GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* initialize the infrared application: rc5 encode*/
    rc5_encode_init();

    /* initialize the infrared application: rc5 decode*/
    rc5_decode_init();

    delay_1ms(200);

    /* test */
    {
        address_index     = 0x06 ;
        instruction_index = 15 ;

        /*send IR frame (address, command)*/
        rc5_encode_send_frame(address_index , instruction_index , rc5_ctrl1);

        /* if data do not send compelet */
        while(0x00 == send_operation_completed);

        /* if data do not received */
        while(0x00 == rc5_frame_received);

        /* decode the rc5 frame */
        rc5_decode(&rc5_frame);
    }

    /* transferstatus ? passed : failed */
    while(1){
        if(15 == rc5_frame.command){
            /* turn on LED1 */
            gd_eval_led_on(LED1);
            /* insert 200 ms delay */
            delay_1ms(200);

            /* turn on LED2 */
            gd_eval_led_on(LED2);
            /* insert 200 ms delay */
            delay_1ms(200);

            /* turn on LED3 */
            gd_eval_led_on(LED3);
            /* insert 200 ms delay */
            delay_1ms(200);

            /* turn on LED4 */
            gd_eval_led_on(LED4);
            /* insert 200 ms delay */
            delay_1ms(200);

            /* turn off LEDs */
            gd_eval_led_off(LED1);
            gd_eval_led_off(LED2);
            gd_eval_led_off(LED3);
            gd_eval_led_off(LED4);

            /* insert 200 ms delay */
            delay_1ms(200);
        }else{
            /* flash LED for status error */
            flash_led(1);
        }
    }
}

/*!
    \brief      test status led init
    \param[in]  none
    \param[out] none
    \retval     none
*/
void test_status_led_init(void)
{
    /* initialize the leds */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    /* close all of leds */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);

    /* setup SysTick timer for 1 msec interrupts  */
    if(SysTick_Config(SystemCoreClock / 1000)){ 
        /* capture error */ 
        while(1);
    }
}

/*!
    \brief      test status leds
    \param[in]  times: leds blink times
    \param[out] none
    \retval     none
*/
void flash_led(int times)
{
    int i;

    for(i = 0; i < times; i++){
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on leds */
        gd_eval_led_on(LED1);
        gd_eval_led_on(LED2);
        gd_eval_led_on(LED3);
        gd_eval_led_on(LED4);

        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off LEDs */
        gd_eval_led_off(LED1);
        gd_eval_led_off(LED2);
        gd_eval_led_off(LED3);
        gd_eval_led_off(LED4);
    }
}
