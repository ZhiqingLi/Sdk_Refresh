/********************************************************************************
 *                              USDK(GL5110)
 *                            Module: KERNEL
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>      <time>           <version >             <desc>
 *      wuyueqian     2011-9-10 09:00     1.0             build this file
 ********************************************************************************/
/*!
 * \file     storage_device.h
 * \brief    ?ééè±??y?ˉ?ó?ú
 * \author   wuyueqian
 * \version  1.0
 * \date  2011/9/10
 *******************************************************************************/

#ifndef _CARD_DEVICE_H
#define _CARD_DEVICE_H

#ifndef _ASSEMBLER_

#include <storage_device.h>
#include <typeext.h>
#include <sysdef.h>

/*********************************************************/
//card 驱动能力管理
#define     SDCLKCTL_LOW_FREQUENCE              (0x01 << CMU_SDCLK_SDCLKSEL1)        // NOT divide 128
#define     SDCLKCTL_DIV_1                      (0x00 << CMU_SDCLK_SDCLKDIV_SHIFT)
#define     SDCLKCTL_DIV_2                      (0x01 << CMU_SDCLK_SDCLKDIV_SHIFT)
#define     SDCLKCTL_DIV_3                      (0x02 << CMU_SDCLK_SDCLKDIV_SHIFT)
#define     SDCLKCTL_DIV_4                      (0x03 << CMU_SDCLK_SDCLKDIV_SHIFT)
#define     SDCLKCTL_SEL_PLL                    (0x01 << CMU_SDCLK_SDCLKSEL0)        // 0: HOSC; 1: MCUPLL

#define     SDCLKCTL_LOW                        (SDCLKCTL_DIV_3 | SDCLKCTL_LOW_FREQUENCE)                            // (HOSC / 128) / 2
#define     SDCLKCTL_HOSC_DIV2                  (SDCLKCTL_DIV_2)  // (HOSC / 1) / 2
#define     SDCLKCTL_HOSC_DIV1                  (SDCLKCTL_DIV_1)  // (HOSC / 1) / 1
//#define     SDCLKCTL_MCUPLL                     (SDCLKCTL_DIV_1 | SDCLKCTL_SEL_PLL)// MCUPLL

typedef struct
{
    /*mcupll clk div*/  
    uint8 mcupll_max_freq:7;
    /*use mcupll or not*/
    uint8 mcupll_use:1; 
    /*card clk driver level*/
    uint8 clk_drv_level:4;
    /*card cmd driver level*/
    uint8 cmd_drv_level:4;
    /*card data driver level*/		
    uint8 data_drv_level:4;
    /*card gpio use 0:2825 1:2823*/    
    uint8 card_gpio_set:2;    
    /*reserved 2bit*/
    uint8 reserved0:2;
    uint8 reserved1;
    /*card init clk 0:hosc_div2 1:hosc_div1*/
    uint16 sd_clk_init;
    /*card read/write clk 0:hosc_div2 1:hosc_div1 2:MCUPLL*/
    uint16 sd_clk_rw;
}card_pm_cfg_t;

typedef struct
{
    /*mucpll clk div*/  
    uint8 mcupll_max_freq : 7;
    /*use mcupll or not*/
    uint8 mcupll_use : 1; 
    /*card clk driver level*/
    uint8 clk_drv_level:2;
    /*card cmd driver level*/
    uint8 cmd_drv_level:2;
    /*card data driver level*/		
    uint8 data_drv_level : 2;
    /*reserved 2bit*/
    uint8 reserved: 2;
    /*card init clk 0:hosc_div2 1:hosc_div1*/
    uint16 sd_clk_init;
    /*card read/write clk 0:hosc_div2 1:hosc_div1 2:MCUPLL*/
    uint16 sd_clk_rw;
}rom_card_pm_cfg_t;//rom

extern int device_card_read(unsigned int addr,unsigned char* buf,unsigned int sec_len) __FAR__;
extern int device_card_write(unsigned int addr,unsigned char* buf,unsigned int sec_len) __FAR__;
extern int device_card_update(void* null0, void* null1, void* null2);
extern int device_card_is_write_protect(void* null0, void* null1, void* null2);
extern uint32 device_card_get_cap(void* null0, void* null1, void* null2);
extern uint32 device_card_get_sector_size(void* null0, void* null1, void* null2);

#endif

#endif

