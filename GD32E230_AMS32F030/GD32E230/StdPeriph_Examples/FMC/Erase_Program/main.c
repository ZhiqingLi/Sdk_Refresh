/*!
    \file  main.c
    \brief main flash program, erase and reprogram
    
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
#include "main.h"
#include "gd32e230c_eval.h"

#define FMC_PAGE_SIZE           ((uint16_t)0x400U)
#define FMC_WRITE_START_ADDR    ((uint32_t)0x08004000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x08004800U)

uint32_t *ptrd;
uint32_t address = 0x00U;
uint32_t data0   = 0x01234567U;
uint64_t data1   = 0x0123456789abcdefU;
led_typedef_enum lednum = LED2;

/* calculate the number of page to be programmed/erased */
uint32_t page_num = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t word_num0 = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);
/* calculate the number of page to be programmed/erased by double word */
uint32_t word_num1 = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 3);

/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(void)
{
    uint32_t erase_counter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    
    /* erase the flash pages */
    for(erase_counter = 0U; erase_counter < page_num; erase_counter++){
        fmc_page_erase(FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * erase_counter));
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_WRITE_START_ADDR;
    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_word_program(address, data0);
        address += 4U;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      program fmc double word by double word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_doubleword(void)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_doubleword_program(address, data1);
        address += 8U;
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0U; i < word_num0; i++){
        if(0xFFFFFFFFU != (*ptrd)){
            lednum = LED1;
            gd_eval_led_on(lednum);
            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0U; i < word_num0; i++){
        if((*ptrd) != data0){
            lednum = LED1;
            gd_eval_led_on(lednum);
            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_doubleword_check(void)
{
    uint32_t i;
    uint32_t data_0, data_1;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0U; i < word_num1; i++){
        data_0 = *ptrd;
        ptrd++;
        data_1 = *ptrd;
        if((data_0 | ((uint64_t)data_1<<32U)) != data1){
            lednum = LED1;
            gd_eval_led_on(lednum);
            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize led on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* step1: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step2: word program and check if it is successful. If not, light the LED2. */
    fmc_program();
    fmc_program_check();
    
    /* step3: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step4: double word program and check if it is successful. If not, light the LED3. */
    fmc_program_doubleword();
    fmc_program_doubleword_check();

    /* if all the operations are successful, light the LED2. */
    if(LED2 == lednum){
        gd_eval_led_on(LED2);
    }

    while(1){
    }
}
