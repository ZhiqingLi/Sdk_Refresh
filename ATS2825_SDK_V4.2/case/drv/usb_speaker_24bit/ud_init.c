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

//#ifndef FPGA_DEBUG
/******************************************************************************/
/*!
 * \par  Description:
 *   USB phy set.
 * \param[in]
 * \return
 * \retval
 * \ingroup ud_init.c
 * \par
 * \note
 *******************************************************************************/
void usb_phy_setting(uint8 bRegAdd, uint8 bValue)
{
    unsigned char bAddLow, bAddHigh;
    unsigned char bTemp;

    bAddLow = bRegAdd & 0x0f;
    bAddHigh = (bRegAdd >> 4) & 0x0f;

    bTemp = act_readb(VDCTRL);
    bTemp &= (uint8) 0x80;

    /* write vstatus: */
    act_writeb(bValue, VDSTAT);

    /* write vcontrol: */
    bAddLow |= 0x10;
    act_writeb((bAddLow | bTemp), VDCTRL);

    bAddLow &= 0x0f;
    act_writeb((bAddLow | bTemp), VDCTRL);

    bAddLow |= 0x10;
    act_writeb((bAddLow | bTemp), VDCTRL);

    bAddHigh |= 0x10;
    act_writeb((bAddHigh | bTemp), VDCTRL);

    bAddHigh &= 0x0f;
    act_writeb((bAddHigh | bTemp), VDCTRL);

    bAddHigh |= 0x10;
    act_writeb((bAddHigh | bTemp), VDCTRL);
}
//#endif

/******************************************************************************/
/*!
 * \par  Description:
 *   init usb device ,called by ap.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_init.c
 * \par
 * \note
 *******************************************************************************/
void usb_sound_init(isr_cbk *p_isr_cbk)
{
    //uint32 irq_save;
    //#ifndef FPGA_DEBUG

    uint8 efuse_reg;

    //sys_mdelay(3);
    //#endif

    act_writel((act_readl(MULTI_USED) & (~MULTI_USED_UVDD_V_MASK)) | 0x180,
               MULTI_USED); //vdd =1.2v vdd ldo enbale //从1.3v修改为1.2v更稳定

    //enable usb module clk CMU_DEVCLKEN
    act_writel(act_readl(CMU_DEVCLKEN) | (1 << CMU_DEVCLKEN_USBCLKEN), CMU_DEVCLKEN);

    //act_writel((act_readl(MULTI_USED) & 0xFFFFFF0F) | 0x1c0, MULTI_USED); //vdd =1.3v vdd ldo enbale

    //for detect cotec
    //irq_save = sys_local_irq_save();

    //reset2 usb moudule MRCR
    act_writel(act_readl(MRCR) | (1 << MRCR_USBReset2), MRCR);
    //system interface

    sys_mdelay(1);
    //enable usb phy pll USB_PHYCTRL
    act_writeb(((uint8) 0xc0 | (act_readb(USB_PHYCTRL))), USB_PHYCTRL);
    //system interface

    sys_mdelay(1);
    //set efuse
    act_writeb(0x00, USB_EFUSE_REF);
    efuse_reg = act_readb(USB_EFUSE_REF);
    act_writeb(0x80, USB_EFUSE_REF);
    act_writeb((act_readb(USB_EFUSE_REF) | efuse_reg), USB_EFUSE_REF);
    sys_mdelay(1);

    //system interface usbreset
    act_writel(act_readl(MRCR) | (1 << MRCR_USBReset), MRCR);

    sys_mdelay(1);

    //sys_local_irq_restore(irq_save);


    //wait reset complete
    while((act_readb(LINESTATUS) & (1 << LINESTATUS_otgreset)) != 0) {
        //clear watchdog
        ;//act_writel(0x00000001 | (act_readl(WD_CTL)), WD_CTL);
    }

    //enable dp,dm DPDMCTRL
    act_writeb(0x1f | (act_readb(DPDMCTRL)), DPDMCTRL);

    sys_mdelay(10);

    //enbale sofeware id
    act_writeb(0x0f | (act_readb(IDVBUSCTRL)), IDVBUSCTRL);

    sys_mdelay(5);

    //#ifndef FPGA_DEBUG
    //#endif

    //enable all memory clock
    act_writel(act_readl(CMU_MEMCLKEN) | 0x1ff, CMU_MEMCLKEN);

    //uram0
    act_writel((act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_URAM1CLKSEL_MASK)) | (1 << CMU_MEMCLKSEL_URAM1CLKSEL_SHIFT),
               CMU_MEMCLKSEL);

    //uspeaker 24bit会使用0x28000-0x2c000这一部分空间，因此要禁止这部分空间的使用
    sys_disable_mem_use(USPEAKER24_DATA_BUFFER_ADDR, USPEAKER24_DATA_BUFFER_ADDR + USPEAKER24_DATA_BUFFER_SIZE);

    // pcm ram1 as buffer so it need select cpu clk
    //act_writel((act_readl(CMU_MEMCLKSEL) & (~CMU_MEMCLKSEL_PCMRAM1CLKSEL_MASK)) | (0
    //<< CMU_MEMCLKSEL_PCMRAM1CLKSEL_SHIFT), CMU_MEMCLKSEL);//pcmram1
    //register irq and enable usb isr
    sys_request_irq(IRQ_USB, usound_isr);
    sys_request_irq(IRQ_DMA0, cap_dac_dma_isr);

    //#ifndef FPGA_DEBUG
    //usb_phy_setting(0xe0, 0xa3);
    //usb_phy_setting(0xe7, 0x0b);
    //usb_phy_setting(0xe7, 0x1f);
    //#endif
    usb_phy_setting(0x84, 0x1a);
    usb_phy_setting(0xe0, 0xa3);
    usb_phy_setting(0xe6, 0x8d);
    usb_phy_setting(0xe7, 0x2b);
    usb_phy_setting(0xe7, 0x3f);

    act_writeb(0x01 | act_readb(Usbirq_hcusbirq), Usbirq_hcusbirq);
    act_writeb(0x01 | act_readb(Usbien_hcusbien), Usbien_hcusbien);
    act_writeb(0x02 | act_readb(OUT03TOKIEN), OUT03TOKIEN);
    act_writeb(0x02 | act_readb(OUT03TOKIRQ), OUT03TOKIRQ);
    //act_writeb(0x02 | act_readb(IN03TOKIEN), IN03TOKIEN);
    act_writeb(0x06 | act_readb(IN03TOKIRQ), IN03TOKIRQ);
    act_writeb((uint8) 0x88 | act_readb(USBEIRQ), USBEIRQ);//enable external irq USBEIRQ

    act_writeb(0x00, UsbCTRL_STUS);
    act_writeb(0x80, BKDOOR); //disable high speed

    //set call back
    *p_isr_cbk = usound_isr_cbk;
    sys_set_sys_info(usound_isr_cbk, SYS_INFO_PAGEMISS_CALLBACK);

    descriptor_init();

}

