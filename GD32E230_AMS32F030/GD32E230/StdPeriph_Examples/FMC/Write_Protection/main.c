/*!
    \file  main.c
    \brief main flash pages write protection
    
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
#include "gd32e230c_eval.h"
#include "main.h"

/*!
    \brief      enable some pages' write protection function by configuring option byte
    \param[in]  wp_pages_bitmap: bitmap of pages which need to be enabled write protection function
    \param[out] none
    \retval     none
*/
void fmc_ob_write_protection_enable(uint16_t wp_pages_bitmap)
{
    uint8_t ob_user;
    uint16_t ob_data;
    uint16_t old_wp, new_wp;

    /* unlock the main flash and option byte */
    fmc_unlock();
    ob_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* backup the old OB_USER, OB_DATA and OB_WP */
    ob_user = ob_user_get();
    ob_data = ob_data_get();
    old_wp  = ob_write_protection_get();

    /* it's need to do operation just when the pages indicated by wp_pages_bitmap have not been enabled */
    if(0 != (old_wp & wp_pages_bitmap)){
        /* caculate the new write protectiom bitmap */
        new_wp = ((~old_wp) | wp_pages_bitmap);

        /* erase the option byte before modify the content */
        ob_erase();

        /* restore the OB_USER and OB_DATA */
        ob_user_write(ob_user);
        ob_data_program(ob_data);

        /* enable the new write protection in option byte */
        ob_write_protection_enable(new_wp);

        /* lock the option byte firstly and then lock the main flash after operation */
        ob_lock();
        fmc_lock();

        /* reload the option byte and generate a system reset */ 
        ob_reset();
    }
}

/*!
    \brief      disable some pages' write protection function by configuring option byte
    \param[in]  wp_pages_bitmap: bitmap of pages which need to be disabled write protection function
    \param[out] none
    \retval     none
*/
void fmc_ob_write_protection_disable(uint16_t wp_pages_bitmap)
{
    uint8_t ob_user;
    uint16_t ob_data;
    uint16_t old_wp, new_wp;

    /* unlock the main flash and option byte */
    fmc_unlock();
    ob_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* backup the old OB_USER, OB_DATA and OB_WP */
    ob_user = ob_user_get();
    ob_data = ob_data_get();
    old_wp  = ob_write_protection_get();

    /* it's need to do operation just when the pages indicated by wp_pages_bitmap have been enabled */
    if((old_wp & wp_pages_bitmap) != wp_pages_bitmap){
        /* caculate the new write protectiom bitmap */
        new_wp = ~(old_wp | wp_pages_bitmap);

        /* erase the option byte before modify the content */
        ob_erase();

        /* restore the OB_USER and OB_DATA */
        ob_user_write(ob_user);
        ob_data_program(ob_data);

        /* enable the new write protection in option byte */
        ob_write_protection_enable(new_wp);

        /* lock the option byte firstly and then lock the main flash after operation */
        ob_lock();
        fmc_lock();

        /* reload the option byte and generate a system reset */ 
        ob_reset();
    }
}

/*!
    \brief      erase and program flash, meanwhile check the operation result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_and_program(void)
{
    uint32_t *ptr = (uint32_t *)ERASE_PAGE_START_ADDR;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* erase target page */
    fmc_page_erase(ERASE_PAGE_START_ADDR);
    /* check the erase result, light the LED2 if the result is failed */
    if(0xFFFFFFFF != (*ptr)){
        gd_eval_led_on(LED2);

        return;
    }

    /* program target address */
    fmc_word_program(PROGRAM_ADDRESS, PROGRAM_DATA);
    /* check the program result, light the LED2 if the result is failed */
    if(PROGRAM_DATA != (*ptr)){
        gd_eval_led_on(LED2);

        return;
    }

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* lock the main FMC after the operation */
    fmc_lock();
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize the LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* configure the keys */
    gd_eval_key_init(KEY_WAKEUP, KEY_MODE_EXTI);
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_EXTI);

    /* check the write protection result and light corresponding LEDs */
    if(WP_ALL_PAGES_BITMAP == ob_write_protection_get()){
        gd_eval_led_on(LED1);
    }
    /* erase and program flash, 
       failure (LED2 on) indicates the page is in write protection, 
       success (LED2 off) indicates the page is not in write protection */
    fmc_erase_and_program();

    while(1);
}
