/*!
    \file  readme.txt
    \brief description of the TIMERs parallel synchro demo for gd32e230
    
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

  This demo is based on the GD32E230C-EVAL-V1.0 board, it shows how to synchronize 
TIMER peripherals in parallel mode.
  
  In this example three timers are used:
  1/TIMER14 is configured as master timer:
  - PWM mode is used.
  - The TIMER14 update event is used as trigger output.

  2/TIMER2 is slave for TIMER14, 
  - PWM mode is used.
  - The ITR2(TIMER14) is used as input trigger.
  - external clock mode is used, the counter counts on the rising edges of
  the selected trigger.

  3/TIMER0 is slave for TIMER14, 
  - PWM mode is used.
  - The ITR0(TIMER14) is used as input trigger.
  - external clock mode is used, the counter counts on the rising edges of
  the selected trigger.
 
  The TIMERxCLK is fixed to 72 MHz, the TIMER14 counter clock is:
72MHz/3600= 20KHz.

  The master timer TIMER14 CH1(PA2) is running at TIMER14 frequency:
  TIMER14 frequency = (TIMER14 counter clock)/ (TIMER14 period + 1) = 5 Hz 
and the duty cycle = TIMER14_CH0CV/(TIMER14_CAR + 1) = 50%

  The TIMER2 CH0(PA6) is running:
  - At (TIMER14 frequency)/ (TIMER2 period + 1) = 5 Hz and a duty cycle
  equal to TIMER2_CH0CV/(TIMER2_CAR + 1) = 50%

  The TIMER0 CH0(PA8) is running:
  - At (TIMER14 frequency)/ (TIMER0 period + 1) = 5 Hz and a duty cycle
  equal to TIMER2_CH0CV/(TIMER2_CAR + 1) = 50%
  