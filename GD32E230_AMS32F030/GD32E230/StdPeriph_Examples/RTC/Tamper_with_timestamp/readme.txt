/*!
    \file  readme.txt
    \brief description of the tamper with timestamp example
    
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

  This example is based on the GD32E230C-EVAL-V1.0 board, it explains how 
to configure the RTC module. In this demo, RTC peripheral is configured to keep
time and generate the tamper interrupt.
  
  The RTC clock source can be chose to LXTAL or IRC40K by uncomment the corresponding 
define in main file. And the HyperTerminal should be connected to the evaluation 
board via USART0/USART1. The vbat bin on the board should connect to the CR1220 battery.
The tamper key is the tamper0 pin. The tamper0 is configured to used for timestamp function. 
  
  After start-up, the program check if the BKP data register is written to a key 
value. If the value is not correct, the program will ask to set the time value on the 
HyperTerminal. If the key value is correct, the current time and subsecond will be 
displayed on the HyperTerminal.
  
  The RTC module is in the Backup Domain, and is not reset by the system reset or 
power reset. 
  
  When press the tamper key, a tamper0 interrupt will be generated, and the tamper0 event 
will reset the BKP0 register, thus if you press the reset key, on the HyperTerminal it asks
you to set the time again. LED1, LED2 toggles. If LED2 always turn on, there may be other write
operations to BKP0 register has occured. In this example it's configured to enable timestamp 
function when the tamper0 event occurs, thus the timestamp value is then be print on the 
HyperTerminal to tell the time that tamper0 event occured.