/*******************************************************************************
 *                              AS212A
 *                            Module: USB Host
 *                 Copyright(c) 2011-2016 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       gch      2011-12-19            1.0             build this file
*******************************************************************************/
/*!
 * \file     uhost_head.h
 * \brief    This file gathers the headfiles of USB Host driver.
 * \author   gch
 * \version  1.0
 * \date     2011-12-19
*******************************************************************************/
#ifndef _UHOST_H
#define _UHOST_H

#include <typeext.h>
#include <sysdef.h>



/* BULK IN and BULK OUT endpoint number
 * Note: endpoint number must be '1' or '2', and must not be other value or other format, because they
 *       are used by other macros.
 *       e.g. "0x01" or "0x02" is the error format.
 */
#define     BULK_IN_EP_NUM                                                  1 // BULK IN 端点号,
#define     BULK_OUT_EP_NUM                                                 2 // BULK OUT 端点号

 
//#define     GL_CONTROLLER_ENABLED          (0x01 << 0x01)
#define     FSM_A_HOST                     0x03 /* The USB FSM State: A_host */

/* function parameter for reset_usb_controller() */
#define     CONTROLLER_INIT             0x00
#define     CONTROLLER_ENABLE           0x01

    
#define     LINESTATUS_DP                                   0x08
    
#define     MULTI_USED_UVDD_EN_OR                           (0x01 << MULTI_USED_UVDD_EN)
#define     MULTI_USED_UVDD_V_1_3                           (0x06 << MULTI_USED_UVDD_V_SHIFT) // 1.3V
#define     MRCR_USBReset2_OR                               (0x01 << MRCR_USBReset2)
#define     MRCR_USBReset_OR                                (0x01 << MRCR_USBReset)
#define     CLKENCTL_USBCLKEN_OR                            (0x01 << 10)
#define     LINESTATUS_otgreset_OR                          (0x01 << LINESTATUS_otgreset)
#define     DPDMCTRL_LineDetEN_OR                           (0x01 << DPDMCTRL_LineDetEN)
#define     USB_PHYCTRL_Phy_PLLEN_OR                        (0x01 << USB_PHYCTRL_Phy_PLLEN)
#define     USB_PHYCTRL_Phy_DALLUALLEN_OR                   (0x01 << USB_PHYCTRL_Phy_DALLUALLEN)
#define     BKDOOR_hsdisable_OR                             (0x01 << BKDOOR_hsdisable)
//#define     USBCtrl_abusdrop_OR                             (0x01 << USBCtrl_abusdrop)
#define     USBCtrl_busreq_OR                               (0x01 << USBCtrl_busreq)
#define     USBien_locsofien_OR                             (0x01 << USBien_locsofien)
#define     USBEIRQ_usbien_OR                               (0x01 << USBEIRQ_usbien)


#define     MEMCLKCTL1_URAMCLKSEL_OR                        (0x01 << MEMCLKCTL1_URAMCLKSEL)
#define     MEMCLKCTL0_URAMCLKEN_OR                         (0x01 << MEMCLKCTL0_URAMCLKEN)

/*
 * Common Used macros
 */
#define     _CONCATENATE_2_MACRO(x, y)                      x ## y
#define     CONCATENATE_2_MACRO(x, y)                       _CONCATENATE_2_MACRO(x, y) // 连接2个宏定义
#define     CONCATENATE_3_MACRO(x, y, z)                    CONCATENATE_2_MACRO(CONCATENATE_2_MACRO(x, y), z)
#define     CONCATENATE_4_MACRO(w, x, y, z)                 CONCATENATE_2_MACRO(CONCATENATE_3_MACRO(w, x, y), z)
#define     CONCATENATE_5_MACRO(v, w, x, y, z)              CONCATENATE_2_MACRO(CONCATENATE_4_MACRO(v, w, x, y), z)


#define     HCxSTADDR_BULK_OUT          CONCATENATE_3_MACRO(IN, BULK_OUT_EP_NUM, STADDR) // INxSTADDR is OUT for host
#define     HCxSTADDR_BULK_IN           CONCATENATE_3_MACRO(OUT, BULK_IN_EP_NUM, STADDR) // OUTxSTADDR is IN for host
 
/* close USB clock (这样USB controller部分可以省功耗，DP&DM的时钟源应该是AHBCLK，所以关闭的clock跟DP&DM的时钟源无关).
 * 关闭PLLEN即可,其他像PLLEN, USBRESET 和 GoSuspend都不用管,另外Phy_DALLUALLEN是内部用的,也可关闭
 */
#define     CLOSE_USB_CONTROLLER_CLOCK() \
            act_clearb(USB_PHYCTRL_Phy_PLLEN_OR | USB_PHYCTRL_Phy_DALLUALLEN_OR, USB_PHYCTRL)
#define     OPEN_USB_CONTROLLER_CLOCK()  \
            act_setb(USB_PHYCTRL_Phy_PLLEN_OR | USB_PHYCTRL_Phy_DALLUALLEN_OR, USB_PHYCTRL)

/* enable Line status detect, enable 15Kohm DP/DM pull down, disable 500Kohm DP/DM pull up resistor */
#define     ENABLE_DEVICE_DETECT() \
            act_writeb((act_readb(DPDMCTRL) & (uint8)0xf0) | DPDMCTRL_LineDetEN_OR, DPDMCTRL)
#define     DISABLE_DEVICE_DETECT() // ??? 需要这个函数不? 还是只要上电后enable一次就可以了


//GL5116中UHOST电源使能不需要IO口控制，保留宏定义方便以后其能被IO口控制
#define     UHOST_POWER_EN_PIN               (21)
#define     UHOST_POWER_ENABLE()   \
			//act_writel((act_readl(GPIOAOUTEN) | (1 << UHOST_POWER_EN_PIN)), GPIOAOUTEN)

#define     UHOST_POWER_OPEN()		\
			//act_writel((act_readl(GPIOADAT) | (1 << UHOST_POWER_EN_PIN)), GPIOADAT)
			
#define     UHOST_POWER_CLOSE()		\
			//act_writel((act_readl(GPIOADAT) & (~(1 << UHOST_POWER_EN_PIN))), GPIOADAT)
			
#endif /* _UHOST_H */
