/*!
    \file  main.c
    \brief RTC timestamp demo
    
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

#define RTC_CLOCK_SOURCE_LXTAL 
#define BKP_VALUE    0x32F0

rtc_timestamp_struct rtc_timestamp;
rtc_parameter_struct rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;

void rtc_setup(void);
void rtc_show_time(void);
void rtc_show_timestamp(void);
uint8_t usart_input_threshold(uint32_t value);
void rtc_pre_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_com_init(EVAL_COM);
    printf("\n\r  ****************** RTC time-stamp demo ******************\n\r");
    gd_eval_key_init(KEY_TAMPER,KEY_MODE_GPIO);
    
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
  
    rtc_pre_config();

    /* check if RTC has aready been configured */
    if (BKP_VALUE != RTC_BKP0){    
        rtc_setup(); 
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
            printf("power on reset occurred....\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
            printf("external reset occurred....\n\r");
        }
        printf("no need to configure RTC....\n\r");
        
        rtc_show_time();
    } 
    
    rcu_all_reset_flag_clear();
    
    gd_eval_led_init(LED1);
    gd_eval_led_on(LED1);
       
    exti_flag_clear(EXTI_19); 
    exti_init(EXTI_19,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    nvic_irq_enable(RTC_IRQn,0);
    
    /* RTC timestamp configuration */
    rtc_timestamp_enable(RTC_TIMESTAMP_FALLING_EDGE);
    rtc_interrupt_enable(RTC_INT_TIMESTAMP); 
    rtc_flag_clear(RTC_STAT_TSF|RTC_STAT_TSOVRF);    
    
    while (1);
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC40K) 
          rcu_osci_on(RCU_IRC40K);
          rcu_osci_stab_wait(RCU_IRC40K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
  
          prescaler_s = 0x18F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC40K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    /* setup RTC time value */
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    rtc_initpara.rtc_factor_asyn = prescaler_a;
    rtc_initpara.rtc_factor_syn = prescaler_s;
    rtc_initpara.rtc_year = 0x16;
    rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
    rtc_initpara.rtc_month = RTC_APR;
    rtc_initpara.rtc_date = 0x30;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;

    /* current time input */
    printf("=======Configure RTC Time========\n\r");
    printf("  please input hour:\n\r");
    while (tmp_hh == 0xFF){    
        tmp_hh = usart_input_threshold(23);
        rtc_initpara.rtc_hour = tmp_hh;
    }
    printf("  %0.2x\n\r", tmp_hh);
    
    printf("  please input minute:\n\r");
    while (tmp_mm == 0xFF){    
        tmp_mm = usart_input_threshold(59);
        rtc_initpara.rtc_minute = tmp_mm;
    }
    printf("  %0.2x\n\r", tmp_mm);

    printf("  please input second:\n\r");
    while (tmp_ss == 0xFF){
        tmp_ss = usart_input_threshold(59);
        rtc_initpara.rtc_second = tmp_ss;
    }
    printf("  %0.2x\n\r", tmp_ss);

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){    
        printf("\n\r** RTC time configuration failed! **\n\r");
    }else{
        printf("\n\r** RTC time configuration success! **\n\r");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }   
}

/*!
    \brief      display the timestamp time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_timestamp(void)
{
    uint32_t ts_subsecond = 0;
    uint8_t ts_subsecond_ss,ts_subsecond_ts,ts_subsecond_hs ;
  
    rtc_timestamp_get(&rtc_timestamp);
    /* get the subsecond value of timestamp time, and convert it into fractional format */
    ts_subsecond = rtc_timestamp_subsecond_get();
    ts_subsecond_ss=(1000-(ts_subsecond*1000+1000)/400)/100;
    ts_subsecond_ts=(1000-(ts_subsecond*1000+1000)/400)%100/10;
    ts_subsecond_hs=(1000-(ts_subsecond*1000+1000)/400)%10;

    printf("Get the time-stamp time: %0.2x:%0.2x:%0.2x .%d%d%d \n\r", \
          rtc_timestamp.rtc_timestamp_hour, rtc_timestamp.rtc_timestamp_minute, rtc_timestamp.rtc_timestamp_second,\
          ts_subsecond_ss, ts_subsecond_ts, ts_subsecond_hs);
}

/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_time(void)
{
    uint32_t time_subsecond = 0;
    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;
  
    rtc_current_time_get(&rtc_initpara);
    /* get the subsecond value of current time, and convert it into fractional format */
    time_subsecond = rtc_subsecond_get();
    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

    printf("Current time: %0.2x:%0.2x:%0.2x .%d%d%d \n\r", \
          rtc_initpara.rtc_hour, rtc_initpara.rtc_minute, rtc_initpara.rtc_second,\
          subsecond_ss, subsecond_ts, subsecond_hs);
}

/*!
    \brief      get the input character string and check if it is valid
    \param[in]  none
    \param[out] none
    \retval     input value in BCD mode
*/
uint8_t usart_input_threshold(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2){
        while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_RBNE));
        tmp[index++] = usart_data_receive(EVAL_COM);
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){
            printf("\n\r please input a valid number between 0 and 9 \n\r");
            index--;
        }
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    if (index > value){
        printf("\n\r please input a valid number between 0 and %d \n\r", value);
        return 0xFF;
    }
    
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) <<4);
    return index;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM,USART_FLAG_TC));
    return ch;
}
