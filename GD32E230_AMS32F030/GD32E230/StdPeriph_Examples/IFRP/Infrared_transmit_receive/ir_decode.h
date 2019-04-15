/*!
    \file  main.c
    \brief keyboard polling mode
    
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

#ifndef IR_DECODE_H
#define IR_DECODE_H

#include "gd32e230.h"

typedef enum {NO = 0, YES = !NO} status_yes_or_no;

#if defined TIMER_CHANNEL1
  #define IR_TIMER                 TIMER2                   /*!< timer used for IR decoding */
  #define TIMER_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIMER_CLK             RCU_TIMER2               /*!< clock of the used timer */
  #define IR_TIMER_IRQn            TIMER2_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIMER_Channel         TIMER_CH_1               /*!< IR TIMER channel */

  #define IR_GPIO_PORT             GPIOB                    /*!< port which IR output is connected */
  #define IR_GPIO_PORT_CLK         RCU_GPIOB                /*!< IR pin GPIO clock port */
  #define IR_GPIO_PIN              GPIO_PIN_5               /*!< pin which IR is connected */
  #define IR_GPIO_SOURCE           GPIO_PINSOURCE5
#else
  #define IR_TIMER                 TIMER2                   /*!< timer used for IR decoding */
  #define TIMER_PRESCALER          71                       /*!< TIMER prescaler */
  #define IR_TIMER_CLK             RCU_TIMER2               /*!< clock of the used timer */
  #define IR_TIMER_IRQn            TIMER2_IRQn              /*!< IR TIMER IRQ */
  #define IR_TIMER_Channel         TIMER_CH_0               /*!< IR TIMER channel */

  #define IR_GPIO_PORT             GPIOB                    /*!< port which IR output is connected */
  #define IR_GPIO_PORT_CLK         RCU_GPIOB                /*!< IR pin GPIO clock port */
  #define IR_GPIO_PIN              GPIO_PIN_4               /*!< pin which IR is connected */
  #define IR_GPIO_SOURCE           GPIO_PINSOURCE5
#endif
#endif /* IR_DECODE_H */
