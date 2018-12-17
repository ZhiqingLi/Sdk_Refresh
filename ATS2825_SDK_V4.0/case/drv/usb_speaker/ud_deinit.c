/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_intern.h"

/*void sound_deinit(void)
 {
 disable_dac(1);
 #ifdef _USB_RECORD_
 disable_ain(g_mic_config_val); //mic
 disable_adc();
 #endif
 }*/
extern int g_pagemiss_cb_bak;
/******************************************************************************/
/*!
 * \par  Description:
 *   USB device app exit.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_deinit.c
 * \par
 * \note
 *******************************************************************************/
void usb_device_exit(void)
{
    //close usb isr
    //#ifndef FPGA_DEBUG
    uint8 efuse_reg;
    //sys_mdelay(40);
    //#endif
    sys_set_sys_info(g_pagemiss_cb_bak, SYS_INFO_PAGEMISS_CALLBACK);

    sys_free_irq(IRQ_USB);

    //sound_deinit();

    //reset usb moudule
    act_writel((~(1 << MRCR_USBReset2)) & act_readl(MRCR), MRCR);
    sys_mdelay(1);

    act_writel((1 << MRCR_USBReset2) | act_readl(MRCR), MRCR);

    sys_mdelay(2);

    act_writel((~(1 << MRCR_USBReset)) & act_readl(MRCR), MRCR);
    //system interface
    sys_mdelay(1);

    //set usb moudule to normal state
    act_writel((1 << MRCR_USBReset) | act_readl(MRCR), MRCR);

    //enable usb phy pll USB_PHYCTRL
    act_writeb(((uint8) 0xc0 | (act_readb(USB_PHYCTRL))), USB_PHYCTRL);
    sys_mdelay(1);

    //#ifndef FPGA_DEBUG
    //set efuse
    act_writeb(0x00, USB_EFUSE_REF);
    efuse_reg = act_readb(USB_EFUSE_REF);
    act_writeb(0x80, USB_EFUSE_REF);
    act_writeb((act_readb(USB_EFUSE_REF) | efuse_reg), USB_EFUSE_REF);
    //#endif

    sys_mdelay(1);

    //wait reset complete
    while(0 != ((act_readb(LINESTATUS)) & 0x01)) {
        //clear watchdog
        ;//act_writel(0x00000001 | (act_readl(WD_CTL)), WD_CTL);
    }

    //disable dp,dm DPDMCTRL
    //act_writeb(0xe0 & (act_readb(DPDMCTRL)), DPDMCTRL);

    sys_mdelay(10);

    //disable sofeware id
    act_writeb((uint8) 0xfb & (act_readb(IDVBUSCTRL)), IDVBUSCTRL);

    sys_mdelay(5);

    //usb disconnect
    act_writeb(0x40, UsbCTRL_STUS);

    //switch memory to MCU
    act_writel(act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_URAM0CLKSEL_MASK), CMU_MEMCLKSEL);
    act_writel(act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_URAM1CLKSEL_MASK), CMU_MEMCLKSEL);

    //disable usb phy pll USB_PHYCTRL
    act_writeb(0x3f & act_readb(USB_PHYCTRL), USB_PHYCTRL);

}
