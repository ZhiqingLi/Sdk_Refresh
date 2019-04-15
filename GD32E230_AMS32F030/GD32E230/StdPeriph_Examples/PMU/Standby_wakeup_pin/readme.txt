/*!
    \file    readme.txt
    \brief   description of the standby mode wakeup through wakeup pin demo
    
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

  This example is based on the GD32E230C-EVAL-V1.0 board, it shows pmu how to enter standby mode 
and wakeup it. 

  Connect the CK_OUT pin (PA8) to an oscilloscope to monitor the CK_SYS frequency.

  Step 1: When the board is start up, all of the LEDs are on and the oscilloscope shows that 
the frequency of CK_SYS is 72MHz.

  Step 2: Press tamper key to enter standby mode. All of the LEDs are off and there is no waveform 
on the oscilloscope.

  Step 3: Press wakeup key. Mcu will be wakeuped from standby mode and LEDs will be turn on again. 
When exit from the standby mode, a power-on reset occurs and the mcu will execute instruction code 
from the 0x00000000 address. So the SystemInit() function will be reconfigured. The oscilloscope 
shows that the frequency of CK_SYS is 72MHz.
