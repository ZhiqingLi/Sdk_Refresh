/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      fiona 2015-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_intern.h"

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
void usb_hid_exit(void)
{
    //close usb isr
    uint8 efuse_reg;
    //#endif
    sys_free_irq(IRQ_USB);

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

    sys_mdelay(2);

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

    //switch memory uram to MCU
    act_writel(act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_URAM0CLKSEL_MASK), CMU_MEMCLKSEL);
    act_writel(act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_URAM1CLKSEL_MASK), CMU_MEMCLKSEL);

    //disable usb phy pll USB_PHYCTRL
    act_writeb(0x3f & act_readb(USB_PHYCTRL), USB_PHYCTRL);
    //act_writel((act_readl(MULTI_USED) & 0xffffff7f), MULTI_USED); //disable uvdd

    //disable usb module clk
    //act_writel(act_readl(CMU_DEVCLKEN) & (~(1<<10)), CMU_DEVCLKEN);

}
