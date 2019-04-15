/*!
    \file  main.h
    \brief the header file of main
    
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

#ifndef MAIN_H
#define MAIN_H

/* bit num chosen for this example */
#define WP_PAGES_BIT_1                  (8U)
#define WP_PAGES_BIT_2                  (9U)
#define WP_PAGES_BIT_3                  (10U)
#define WP_ALL_PAGES_BITMAP             (0xFFFFU)

/* construct the bitmap which pages need to be configured the write protection */
#define WRITE_PROTECT_PAGES(bit_num)    (BIT(bit_num))

/* every bit in OB_WP indicates 4 pages if they are in write protection */
#define PAGE_NUM_PER_WP_BIT             (4U)

/* address and data for fmc operation */
#define ERASE_PAGE                      (WP_PAGES_BIT_1 * PAGE_NUM_PER_WP_BIT)
#define ERASE_PAGE_START_ADDR           (0x08000000U + 0x400U * ERASE_PAGE)
#define PROGRAM_ADDRESS                 ERASE_PAGE_START_ADDR
#define PROGRAM_DATA                    (0x12345678U)

/* enable some pages' write protection function by configuring option byte */
void fmc_ob_write_protection_enable(uint16_t wp_pages_bitmap);
/* disable some pages' write protection function by configuring option byte */
void fmc_ob_write_protection_disable(uint16_t wp_pages_bitmap);
/* erase and program flash, meanwhile check the operation result */
void fmc_erase_and_program(void);

#endif

