#ifndef _PARTHUS_HW_LSLC_IMPL_
#define _PARTHUS_HW_LSLC_IMPL_

/*
 * MODULE NAME:    hw_lc_impl.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Hardware Access Functions
 * MAINTAINER:     Ivan Griffin
 * DATE:           1 Jun 1999
 *
 * SOURCE CONTROL: $Id: hw_lc_impl.h,v 1.141 2014/07/22 14:16:08 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    1 June 1999 -   jn       - Initial Version V0.9
 *    14 July 1999 -   ig       - Moved macros to inline functions
 *
 */

#include "sys_config.h"
#include "hw_memcpy.h"
#include "hw_lc.h"    
#include "lslc_clk.h"
#include "lslc_access.h"
#include "hw_delay.h"
#include "config.h"

#if (PRAGMA_INLINE==1)
#pragma inline(\
HW_set_bd_addr,\
HW_set_bd_addr_via_uap_lap_nap,\
HW_set_uap_lap,\
HW_set_nap,\
HW_get_uap_lap,\
HW_get_nap,\
HW_get_bd_addr_Ex,\
HW_set_sync,\
HW_get_sync_Ex,\
HW_set_intraslot_offset,\
HW_get_intraslot_offset,\
HW_Get_Intraslot_Avoid_Race,\
HW_set_enc_key,\
HW_get_enc_key_Ex,\
HW_set_bt_clk,\
HW_set_bt_clk_offset,\
HW_set_native_clk,\
HW_get_native_clk,\
HW_set_am_addr,\
HW_get_am_addr,\
HW_set_encrypt,\
HW_get_encrypt,\
HW_set_whiten,\
HW_get_whiten,\
HW_set_sleep_status,\
HW_get_sleep_status,\
HW_set_use_lf,\
HW_get_use_lf,\
HW_set_slave,\
HW_get_slave,\
HW_set_page,\
HW_get_page,\
HW_set_sco_cfg0,\
HW_get_sco_cfg0,\
HW_set_sco_cfg1,\
HW_get_sco_cfg1,\
HW_set_sco_cfg2,\
HW_get_sco_cfg2,\
HW_set_sco_fifo,\
HW_get_sco_fifo,\
HW_set_vci_clk_sel,\
HW_get_vci_clk_sel,\
HW_set_loop,\
HW_get_loop,\
HW_set_test_eco,\
HW_get_test_eco,\
HW_set_test_crc,\
HW_get_test_crc,\
HW_set_test_hec,\
HW_get_test_hec,\
HW_set_test_radio,\
HW_get_test_radio,\
HW_set_vci_clk_sel_map,\
HW_get_vci_clk_sel_map,\
HW_set_test_msg,\
HW_get_test_msg,\
HW_set_dwh_ini,\
HW_get_dwh_ini,\
HW_set_dwh2_ini,\
HW_get_dwh2_ini,\
HW_set_crc_ini,\
HW_get_crc_ini,\
HW_set_pkd_intr_mask,\
HW_get_pkd_intr_mask,\
HW_set_pkd_rx_hdr_intr_mask,\
HW_get_pkd_rx_hdr_intr_mask,\
HW_set_pka_intr_mask,\
HW_get_pka_intr_mask,\
HW_set_no_pkt_rcvd_intr_mask,\
HW_get_no_pkt_rcvd_intr_mask,\
HW_set_sync_det_intr_mask,\
HW_get_sync_det_intr_mask,\
HW_set_tim_intr_mask,\
HW_get_tim_intr_mask,\
HW_set_aux_tim_intr_mask,\
HW_get_aux_tim_intr_mask,\
HW_set_pkd_intr_clr,\
HW_set_pkd_rx_hdr_intr_clr,\
HW_set_pka_intr_clr,\
HW_set_no_pkt_rcvd_intr_clr,\
HW_set_sync_det_intr_clr,\
HW_set_tim_intr_clr,\
HW_set_aux_tim_intr_clr,\
HW_get_no_pkt_rcvd_intr_clr,\
HW_get_sync_det_intr_clr,\
HW_get_pkd_intr,\
HW_get_pkd_rx_hdr_intr,\
HW_get_pka_intr,\
HW_get_no_pkt_rcvd_intr,\
HW_get_sync_det_intr,\
HW_get_tim_intr,\
HW_get_aux_tim_intr,\
HW_set_tx_len,\
HW_get_tx_len,\
HW_set_tx_type,\
HW_get_tx_type,\
HW_set_tx_flow,\
HW_get_tx_flow,\
HW_set_tx_arqn,\
HW_get_tx_arqn,\
HW_set_tx_seqn,\
HW_get_tx_seqn,\
HW_set_tx_buf,\
HW_get_tx_buf,\
HW_set_transmit,\
HW_get_transmit,\
HW_set_tx_half,\
HW_get_tx_half,\
HW_set_tx_p_flow,\
HW_get_tx_p_flow,\
HW_set_tx_l_ch,\
HW_get_tx_l_ch,\
HW_get_tx0_over,\
HW_get_tx0_under,\
HW_get_tx1_over,\
HW_get_tx1_under,\
HW_get_tx2_over,\
HW_get_tx2_under,\
HW_set_rx_mode,\
HW_get_rx_mode,\
HW_set_tx_mode,\
HW_get_tx_mode,\
HW_set_abort_on_wrong_am_addr,\
HW_get_abort_on_wrong_am_addr,\
HW_set_rx_buf,\
HW_get_rx_buf,\
HW_set_err_sel,\
HW_get_err_sel,\
HW_set_win_ext,\
HW_get_win_ext,\
HW_set_freeze_bit_cnt,\
HW_get_freeze_bit_cnt,\
HW_get_rx_len,\
HW_get_rx_type,\
HW_get_rx_flow,\
HW_get_rx_arqn,\
HW_get_rx_seqn,\
HW_get_rx_am_addr,\
HW_get_rx_l_ch,\
HW_get_rx_p_flow,\
HW_get_rx_pkt,\
HW_get_hec_err,\
HW_get_crc_err,\
HW_get_rx_hdr,\
HW_get_rx0_over,\
HW_get_rx0_under,\
HW_get_rx1_over,\
HW_get_rx1_under,\
HW_get_rx2_over,\
HW_get_rx2_under,\
HW_get_err_cnt,\
HW_set_ser_cfg,\
HW_get_ser_cfg,\
HW_set_ser_data,\
HW_get_ser_data,\
HW_set_aux_timer,\
HW_get_aux_timer,\
HW_set_tx_delay,\
HW_get_tx_delay,\
HW_set_rx_delay,\
HW_get_rx_delay,\
HW_get_vci_tx_fifo_fill_level,\
HW_get_vci_rx_fifo_fill_level,\
HW_set_vci_tx_fifo_threshold,\
HW_set_vci_rx_fifo_threshold,\
HW_get_vci_tx_fill_status,\
HW_get_vci_rx_fill_status,\
HW_set_vci_rgf_fifo_reset,\
HW_set_vci_rgf_fifo_16bit_mode,\
HW_set_vci_rgf_mode_enable,\
HW_write_vci_rx_fifo_data,\
HW_read_vci_tx_fifo_data,\
HW_set_add_bt_clk_relative,\
HW_get_add_bt_clk_relative,\
HW_set_write_absolute_bt_clk,\
HW_set_delayed_bt_clk_update,\
HW_set_freeze_bt_clk,\
HW_get_freeze_bt_clk,\
HW_get_minor_revision,\
HW_get_major_revision,\
HWjal_Set_Rst_Code,\
HWjal_Get_Rst_Code,\
HW_toggle_tx_buf,\
HW_toggle_rx_buf)

#if (PRH_BS_CFG_SYS_SCO_REPEATER_SUPPORTED==1)
#pragma inline(\
HW_set_sco_repeater_bit,\
HW_get_sco_repeater_bit)
#endif


#if (BUILD_TYPE==UNIT_TEST_BUILD)
#pragma inline(HW_get_pkd_intr_clr,\
HW_get_pkd_rx_hdr_intr_clr,\
HW_get_pka_intr_clr,\
HW_get_tim_intr_clr,\
HW_get_aux_tim_intr_clr,\
HW_set_pkd_intr,\
HW_set_pkd_rx_hdr_intr,\
HW_set_pka_intr,\
HW_set_no_pkt_rcvd_intr,\
HW_set_sync_det_intr,\
HW_set_tim_intr,\
HW_set_aux_tim_intr,\
HW_set_tx0_over,\
HW_set_tx0_under,\
HW_set_tx1_over,\
HW_set_tx1_under,\
HW_set_tx2_over,\
HW_set_tx2_under,\
HW_set_rx_len,\
HW_set_rx_type,\
HW_set_rx_flow,\
HW_set_rx_arqn,\
HW_set_rx_seqn,\
HW_set_rx_am_addr,\
HW_set_rx_l_ch,\
HW_set_rx_p_flow,\
HW_set_rx_pkt,\
HW_set_hec_err,\
HW_set_crc_err,\
HW_set_rx_hdr,\
HW_set_rx0_over,\
HW_set_rx0_under,\
HW_set_rx1_over,\
HW_set_rx1_under,\
HW_set_rx2_over,\
HW_set_rx2_under,\
HW_set_rst_code,\
HW_set_err_cnt)
#endif /*#if (BUILD_TYPE==UNIT_TEST_BUILD)*/

#endif

/***********************************************************************
*
* This section define all the functions which write to and read
* the common control registers
*
************************************************************************/

/* Set Bluetooth Device Address */
__INLINE__ void HW_set_bd_addr(const t_bd_addr *p_bda)
{
#ifdef HW_DATA32
    /*
     * Write to hardware using only 32 bit words.
     */
    u_int32 bd_U32x2[2];
    BDADDR_Convert_to_U32x2(p_bda, bd_U32x2);
    mSetHWEntry64(JAL_BD_ADDR, bd_U32x2);
#else
    /*
     * Direct byte move
     */
    BDADDR_Get_Byte_Array_Ex(bda, (u_int8*)JAL_BD_ADDR_ADDR);
#endif

}

/*
 * Set BD_ADDR using uap_lap and nap fields (efficient).
 */
__INLINE__ void HW_set_bd_addr_via_uap_lap_nap(t_uap_lap uap_lap, t_nap nap)
{
    mSetHWEntry32(JAL_UAP_LAP, uap_lap);
    mSetHWEntry32(JAL_NAP, nap);
}

/*
 * Get/Set BD_ADDR using separate access to JAL_LAP_UAP JAL_NAP registers
 */
__INLINE__ void HW_set_uap_lap(t_uap_lap uap_lap)
                                        { mSetHWEntry32(JAL_UAP_LAP, uap_lap); }
__INLINE__ void HW_set_nap(t_nap nap)   { mSetHWEntry32(JAL_NAP, nap); }
__INLINE__ u_int32 HW_get_uap_lap(void) { return mGetHWEntry32(JAL_UAP_LAP); }
__INLINE__ u_int32 HW_get_nap(void)     { return mGetHWEntry32(JAL_NAP); }

/*
 * Get Bluetooth Device Address
 */
__INLINE__ void HW_get_bd_addr_Ex(t_bd_addr *p_bda)
{
#ifdef HW_DATA32
    /*
     * Write to hardware using only 32 bit words.
     */
    u_int32 bd_32[2];
    mGetHWEntry64_Ex(JAL_BD_ADDR, bd_32);
    BDADDR_Set_LAP_UAP_NAP(p_bda,
            bd_32[0]&0xFFFFFF, (t_uap) (bd_32[0]>>24), bd_32[1]);
#else
    /*
     * Direct byte read
     */
    BDADDR_Assign_from_Byte_Array(bda, (const u_int8*)JAL_BD_ADDR_ADDR);
#endif
}

#if (PRH_BS_CFG_SYS_SCO_REPEATER_SUPPORTED==1)
/*
 * SCO Repeater NT clock phase adjustment CONTROL REGISTER
 */
/* Set NT phase is adjusted by RX packet control*/
__INLINE__ void HW_set_sco_repeater_bit(const u_int value) { mSetHWEntry(JAL_SCO_REPEATER_BIT, value); }
/* Get state if NT phase is adjusted by RX packet control */
__INLINE__ u_int HW_get_sco_repeater_bit(void) { return mGetHWEntry(JAL_SCO_REPEATER_BIT); }
#endif


/* Set Syncword */
__INLINE__ void HW_set_sync(const u_int32 *sync) { mSetHWEntry64(JAL_SYNC, sync); }
__INLINE__ void HW_set_sync_U32(const u_int32 low_word, const u_int32 high_word)
{
    mSetHWEntry32(JAL_SYNC, low_word);    mSetHWEntry32(JAL_SYNC2, high_word);
}

/* Get Syncword */
__INLINE__ void HW_get_sync_Ex(u_int32 *sync) { mGetHWEntry64_Ex(JAL_SYNC, sync); }


/* Set Intraslot Offset */
__INLINE__ void HW_set_intraslot_offset(const u_int32 offset)
{


    mSetHWEntry32(JAL_INTRASLOT_OFFSET, offset);
}

/* Get Intraslot Offset */
__INLINE__ u_int32 HW_get_intraslot_offset(void) { return mGetHWEntry32(JAL_INTRASLOT_OFFSET); }

/* Get Intraslot Offset avoid race on packet header update */
__INLINE__ u_int32 HW_Get_Intraslot_Avoid_Race(void)
{
    u_int32 intraslot_offset = HW_get_intraslot_offset();


    if (HW_get_intraslot_offset() != intraslot_offset)
    {   /* transitioning => re-read */
        intraslot_offset = HW_get_intraslot_offset();
    }    
    return intraslot_offset;
}

/* Get Encryption Key */
__INLINE__ void HW_set_enc_key(const u_int32* enc_key)
{
    mSetHWEntry128( JAL_ENC_KEY, enc_key);
}

/* Get Encryption Key */
__INLINE__ void HW_get_enc_key_Ex(u_int32 *enc_key)
{
    mGetHWEntry128_Ex(JAL_ENC_KEY, enc_key);
}


/* Get Native Clock */
__INLINE__ t_clock HW_get_native_clk(void)
{
    return mGetHWEntry32(JAL_NATIVE_CLK);
}

/* Get Slave Mode */
__INLINE__ u_int HW_get_slave(void) { return mGetHWEntry(JAL_SLAVE); }

#if (BUILD_TYPE==UNIT_TEST_BUILD)
/* Set Bluetooth Clock: Currently only used by ATS. */
__INLINE__ void HW_set_bt_clk(t_clock bt_clk)
{
    /*
     * Write direct to BT_CLK_OFFSET register, no adjustments
     * required in ATS to emulate hardware.
     * Note if this is required on real hardware, would need to
     * calculate clk_offset to set requested bt_clk indirectly.
     */
    mSetHWEntry32(JAL_BT_CLK_OFFSET, bt_clk);
}
#endif /*(BUILD_TYPE==UNIT_TEST_BUILD)*/

/*
 * Set directly the BT clk offset.
 */
__INLINE__ void HW_set_bt_clk_offset(t_clock bt_clk_offset)
{
 	//extern void os_delay_ms();
#if (BUILD_TYPE==UNIT_TEST_BUILD)
    /*
     * Emulate hardware by adding current clock value to offset
     */
    bt_clk_offset += (HW_get_add_bt_clk_relative()?HW_get_bt_clk():HW_get_native_clk()) & 0x0FFFFFFC;
    bt_clk_offset &= 0x0FFFFFFC;
#endif

    mSetHWEntry32(JAL_BT_CLK_OFFSET, bt_clk_offset);
#ifdef BT_DUALMODE

	DEBUG_MSG(0x53);
	//HWdelay_Wait_For_ms(1,1);//charles
	LE_delay_time(1);
    LE_delay_time(1);
	LE_delay_time(1);
    LE_delay_time(1);
	DEBUG_MSG(0x54);
#else
	HWdelay_Wait_For_ms(1,1);

#endif
	//os_delay_ms(1);
}


/* Set Native Clock */
__INLINE__ void HW_set_native_clk(const t_clock clkn)
{
      mSetHWEntry32(JAL_NATIVE_CLK, clkn);
}


/* Set AM Address */
__INLINE__ void HW_set_am_addr(const u_int am_addr) { mSetHWEntry(JAL_AM_ADDR, am_addr); }
/* Get AM Address */
__INLINE__ u_int HW_get_am_addr(void) { return mGetHWEntry(JAL_AM_ADDR); }

/* Set Encrypt */
__INLINE__ void HW_set_encrypt(const u_int encrypt) { mSetHWEntry(JAL_ENCRYPT, encrypt); }
/* Get Encrypt */
__INLINE__ u_int HW_get_encrypt(void) { return mGetHWEntry(JAL_ENCRYPT); }

/* Set Whitening */
__INLINE__ void HW_set_whiten(const u_int whiten) { mSetHWEntry(JAL_WHITEN, whiten); }
/* Get Whitening */
__INLINE__ u_int HW_get_whiten(void) { return mGetHWEntry(JAL_WHITEN); }

/* Set Use LF */
__INLINE__ void HW_set_use_lf(const u_int use_lf) { mSetHWEntry(JAL_USE_LF, use_lf); }
/* Get Use LF */
__INLINE__ u_int HW_get_use_lf(void) { return mGetHWEntry(JAL_USE_LF); }

/* Set Slave Mode */
__INLINE__ void HW_set_slave(const u_int slave) { mSetHWEntry(JAL_SLAVE, slave); }

/* Set Page Bit */
__INLINE__ void HW_set_page(const u_int page) { mSetHWEntry(JAL_PAGE, page); }
/* Get Page Bit */
__INLINE__ u_int HW_get_page(void) { return mGetHWEntry(JAL_PAGE); }

/* Set Use LF */
__INLINE__ void HW_set_sleep_status(const u_int use_lf) { mSetHWEntry(JAL_SLEEP_STATUS, use_lf); }
/* Get Use LF */
__INLINE__ u_int HW_get_sleep_status(void) { return mGetHWEntry(JAL_SLEEP_STATUS); }


/* Set/Get SCO Configuration for Conversion for SCOs 0, 1 and 2 */
__INLINE__ void HW_set_sco_cfg0( const u_int cfg)   { mSetHWEntry(JAL_SCO_CFG0, cfg); }
__INLINE__ u_int HW_get_sco_cfg0(void)              { return mGetHWEntry(JAL_SCO_CFG0); }

__INLINE__ void HW_set_sco_cfg1( const u_int cfg)   { mSetHWEntry(JAL_SCO_CFG1, cfg); }
__INLINE__ u_int HW_get_sco_cfg1(void)              { return mGetHWEntry(JAL_SCO_CFG1); }

__INLINE__ void HW_set_sco_cfg2( const u_int cfg)   { mSetHWEntry(JAL_SCO_CFG2, cfg); }
__INLINE__ u_int HW_get_sco_cfg2(void)              { return mGetHWEntry(JAL_SCO_CFG2); }


/* Set SCO FIFO */
__INLINE__ void HW_set_sco_fifo( const u_int fifo ) { mSetHWEntry(JAL_SCO_FIFO, fifo ); }
/* Get SCO FIFO */
__INLINE__ u_int HW_get_sco_fifo(void)              { return mGetHWEntry(JAL_SCO_FIFO); }

/* Set SCO VCI Clock Select */
__INLINE__ void HW_set_vci_clk_sel( const u_int vci_clk ) { mSetHWEntry(JAL_VCI_CLK_SEL, vci_clk ); }
/* Set SCO VCI Clock Select */
__INLINE__ u_int HW_get_vci_clk_sel(void) { return mGetHWEntry(JAL_VCI_CLK_SEL); }

/* Set Loopback Bit */
__INLINE__ void HW_set_loop(const u_int loop) { mSetHWEntry(JAL_LOOP, loop); }
/* Get Loopback Bit */
__INLINE__ u_int HW_get_loop(void) { return mGetHWEntry(JAL_LOOP); }

/* Set Test ECO bit */
__INLINE__ void HW_set_test_eco(const u_int test_eco) { mSetHWEntry(JAL_TEST_ECO, test_eco); }
/* Get Test ECO bit */
__INLINE__ u_int HW_get_test_eco(void) { return mGetHWEntry(JAL_TEST_ECO); }

/* Set Test CRC bit */
__INLINE__ void HW_set_test_crc(const u_int test_crc) { mSetHWEntry(JAL_TEST_CRC, test_crc); }
/* Get Test CRC bit */
__INLINE__ u_int HW_get_test_crc(void) {return mGetHWEntry(JAL_TEST_CRC); }

/* Set Test HEC bit */
__INLINE__ void HW_set_test_hec(const u_int test_hec) { mSetHWEntry(JAL_TEST_HEC, test_hec); }
/* Get Test HEC bit */
__INLINE__ u_int HW_get_test_hec(void) { return mGetHWEntry( JAL_TEST_HEC); }

/* Set Test Radio bit */
__INLINE__ void HW_set_test_radio(const u_int test_radio) { mSetHWEntry(JAL_TEST_RADIO, test_radio); }
/* Get Test Radio bit */
__INLINE__ u_int HW_get_test_radio(void) { return mGetHWEntry( JAL_TEST_RADIO ); }

/* Set vci clk sel map bit */
__INLINE__ void HW_set_vci_clk_sel_map(const u_int value) { mSetHWEntry(JAL_VCI_CLK_SEL_MAP, value); }
/* Get vci clk sel map bit */
__INLINE__ u_int HW_get_vci_clk_sel_map(void) { return mGetHWEntry(JAL_VCI_CLK_SEL_MAP); }

/* Set test msg bit for debugging pins */
__INLINE__ void HW_set_test_msg(const u_int value) {mSetHWEntry(JAL_TEST_MSG, value);}
__INLINE__ u_int HW_get_test_msg(void) { return mGetHWEntry(JAL_TEST_MSG); }

/* Set DWH INIT value */
__INLINE__ void HW_set_dwh_ini(const u_int dwh_init) { mSetHWEntry( JAL_DWH_INIT, dwh_init ); }
/* Get DWH INIT value */
__INLINE__ u_int HW_get_dwh_ini(void) { return mGetHWEntry( JAL_DWH_INIT ); }

/* Set DWH2 INIT value [2nd half for Inquiry FHS Response */
__INLINE__ void HW_set_dwh2_ini(const u_int dwh2_init) { mSetHWEntry( JAL_DWH2_INIT, dwh2_init ); }
/* Get DWH2 INIT value */
__INLINE__ u_int HW_get_dwh2_ini(void) { return mGetHWEntry( JAL_DWH2_INIT ); }

/* Set CRC Initial Value */
__INLINE__ void HW_set_crc_ini(const u_int crc_ini) { mSetHWEntry(JAL_CRC_INIT, crc_ini); }
/* Get CRC initial value */
__INLINE__ u_int HW_get_crc_ini(void)  { return mGetHWEntry(JAL_CRC_INIT);  } 

/* Set Timer interrupt mask */
__INLINE__ void HW_set_tim_intr_mask(const u_int value) { mSetHWEntry(JAL_TIM_INTR_MSK, value);
 }
/* Get Timer interrupt mask */
__INLINE__ u_int HW_get_tim_intr_mask(void) { return mGetHWEntry(JAL_TIM_INTR_MSK); }

/* Set Packet interrupt mask */
__INLINE__ void HW_set_pkd_intr_mask(const u_int value) { mSetHWEntry(JAL_PKD_INTR_MSK, value); }
/* Get Packet interrupt mask */
__INLINE__ u_int HW_get_pkd_intr_mask(void) { return mGetHWEntry(JAL_PKD_INTR_MSK); }

/* Set AUX Timer interrupt mask */
__INLINE__ void HW_set_aux_tim_intr_mask(const u_int value) { mSetHWEntry(JAL_AUX_TIM_INTR_MSK, value); }
/* Get AUX Timer interrupt mask */
__INLINE__ u_int HW_get_aux_tim_intr_mask(void) { return mGetHWEntry(JAL_AUX_TIM_INTR_MSK); }

/* Set PKA interrupt mask */
__INLINE__ void HW_set_pka_intr_mask(const u_int value) { mSetHWEntry(JAL_PKA_INTR_MSK, value); }
/* Get PKA Interrupt mask */
__INLINE__ u_int HW_get_pka_intr_mask(void) { return mGetHWEntry(JAL_PKA_INTR_MSK); }

__INLINE__ void  HW_set_pkd_rx_hdr_intr_mask(const u_int value) { mSetHWEntry(JAL_PKD_RX_HDR_INTR_MSK, value); }
__INLINE__ u_int HW_get_pkd_rx_hdr_intr_mask(void) { return mGetHWEntry(JAL_PKD_RX_HDR_INTR_MSK); }


/* Clear Timer interrupt Clear */
__INLINE__ void HW_set_tim_intr_clr(const u_int value) { mSetHWEntry(JAL_TIM_INTR_CLR, value); }

/* Set Pkd Interrupt Clear */
__INLINE__ void HW_set_pkd_intr_clr(const u_int value) { mSetHWEntry(JAL_PKD_INTR_CLR, value); }

/* Clear AUX Timer interrupt Clear */
__INLINE__ void HW_set_aux_tim_intr_clr(const u_int value) { mSetHWEntry(JAL_AUX_TIM_INTR_CLR, value); }

/* Set Pka Interrupt Clear */
__INLINE__ void HW_set_pka_intr_clr(const u_int value) { mSetHWEntry(JAL_PKA_INTR_CLR, value); }

/* Set Pkd RX HDR Interrupt Clear */
__INLINE__ void HW_set_pkd_rx_hdr_intr_clr(const u_int value) { mSetHWEntry(JAL_PKD_RX_HDR_INTR_CLR, value); }


/*
 * COMMON STATUS REGISTERS
 */
/* Get Timer Interrupt */
__INLINE__ u_int HW_get_tim_intr(void) { return mGetHWEntry(JAL_TIM_INTR); }

/* Get Pkd Interrupt */
__INLINE__ u_int HW_get_pkd_intr(void) { return mGetHWEntry(JAL_PKD_INTR); }

/* Get AUX Timer Interrupt */
__INLINE__ u_int HW_get_aux_tim_intr(void) { return mGetHWEntry(JAL_AUX_TIM_INTR); }

/* Get Pka Interrupt */
__INLINE__ u_int HW_get_pka_intr(void) { return mGetHWEntry(JAL_PKA_INTR); }

/* Get Pkd RX HDR Interrupt */
__INLINE__ u_int HW_get_pkd_rx_hdr_intr(void) { return mGetHWEntry(JAL_PKD_RX_HDR_INTR); }


/*
 * TRANSMIT CONTROL REGISTERS
 */

/* Set Transmit Length */
__INLINE__ void HW_set_tx_len(const u_int len) { mSetHWEntry(JAL_TX_LEN, len); }
/* Get Transmit Length */
__INLINE__ u_int HW_get_tx_len(void) { return mGetHWEntry(JAL_TX_LEN); }

/* Set Transmit Type */
__INLINE__ void HW_set_tx_type(const u_int type) { mSetHWEntry(JAL_TX_TYPE, type); }
/* Get Transmit Type */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
__INLINE__ u_int HW_get_tx_type(void) {
	return (mGetHWEntry(JAL_TX_TYPE)>DM1) ? ((mGetHWEntry(JAL_EDR_PTT_ACL) | mGetHWEntry(JAL_EDR_PTT_ESCO)) << 4) |
		mGetHWEntry(JAL_TX_TYPE): mGetHWEntry(JAL_TX_TYPE) ; }
#else
__INLINE__ u_int HW_get_tx_type(void) { return mGetHWEntry(JAL_TX_TYPE); }
#endif

/* Set TX Flow Bit */
__INLINE__ void HW_set_tx_flow(const u_int flow) { mSetHWEntry(JAL_TX_FLOW, flow); }
/* Get TX Flow Bit */
__INLINE__ u_int HW_get_tx_flow(void) { return mGetHWEntry(JAL_TX_FLOW); }

/* Set TX Arqn Bit */
__INLINE__ void HW_set_tx_arqn(const u_int arqn) { mSetHWEntry(JAL_TX_ARQN, arqn); }
/* Get TX Arqn Bit */
__INLINE__ u_int HW_get_tx_arqn(void) { return mGetHWEntry(JAL_TX_ARQN); }

/* Set TX Seqn Bit */
__INLINE__ void HW_set_tx_seqn(const u_int seqn) { mSetHWEntry(JAL_TX_SEQN, seqn); }
/* Get TX Seqn Bit */
__INLINE__ u_int HW_get_tx_seqn(void) { return mGetHWEntry(JAL_TX_SEQN); }

/* Set TX Buffer */
__INLINE__ void HW_set_tx_buf(const u_int32 buf) { mSetHWEntry(JAL_TX_BUF, buf); }
/* Get TX Buffer */
__INLINE__ u_int HW_get_tx_buf(void) { return mGetHWEntry(JAL_TX_BUF); }

/* Set Transmit Enable Bit */
__INLINE__ void HW_set_transmit(const u_int tr) { mSetHWEntry(JAL_TRANSMIT, tr); }
/* Get Transmit Enable Bit */
__INLINE__ u_int HW_get_transmit(void) { return mGetHWEntry(JAL_TRANSMIT); }

/* Set tx half */
__INLINE__ void HW_set_tx_half(const u_int id)  { mSetHWEntry(JAL_TX_HALF, id); }
/* Get ID type */
__INLINE__ u_int HW_get_tx_half(void) { return mGetHWEntry(JAL_TX_HALF); }

/* Set TX L_CH */
__INLINE__ void HW_set_tx_l_ch(const u_int l_ch) { mSetHWEntry(JAL_TX_L_CH, l_ch); }
/* Get TX L_CH */
__INLINE__ u_int HW_get_tx_l_ch(void) { return mGetHWEntry(JAL_TX_L_CH); }

/* Set TX Payload Flow Bit */
__INLINE__ void HW_set_tx_p_flow(const u_int flow) { mSetHWEntry(JAL_TX_P_FLOW, flow); }
/* Get TX Payload Flow Bit */
__INLINE__ u_int HW_get_tx_p_flow(void) { return mGetHWEntry(JAL_TX_P_FLOW); }


/*
 * TRANSMIT STATUS REGISTERS - READ FUNCTIONS
 */
/* Get TX0 Overflow Bit */
__INLINE__ u_int HW_get_tx0_over(void)  { return mGetHWEntry(JAL_TX0_OVER); }

/* Get TX0 Underflow Bit */
__INLINE__ u_int HW_get_tx0_under(void) { return mGetHWEntry(JAL_TX0_UNDER); }

/* Get TX1 Overflow Bit */
__INLINE__ u_int HW_get_tx1_over(void)  { return mGetHWEntry(JAL_TX1_OVER); }

/* Get TX1 Underflow Bit */
__INLINE__ u_int HW_get_tx1_under(void) { return mGetHWEntry(JAL_TX1_UNDER); }

/* Get TX2 Overflow Bit */
__INLINE__ u_int HW_get_tx2_over(void)  { return mGetHWEntry(JAL_TX2_OVER); }

/* Get TX2 Underflow Bit*/
__INLINE__ u_int HW_get_tx2_under(void) { return mGetHWEntry(JAL_TX2_UNDER); }


/*
 * RECEIVE CONTROL REGISTERS
 */
/* Set RX Mode Bits */
__INLINE__ void HW_set_rx_mode(const u_int mode) { 
    mSetHWEntry(JAL_RX_MODE, mode); 
}
/* Get RX Mode Bits */
__INLINE__ u_int HW_get_rx_mode(void) { return mGetHWEntry(JAL_RX_MODE); }

/* Set TX Mode Bits */
__INLINE__ void HW_set_tx_mode(const u_int mode) { mSetHWEntry(JAL_TX_MODE, mode); }
/* Get TX Mode Bits */
__INLINE__ u_int HW_get_tx_mode(void) { return mGetHWEntry(JAL_TX_MODE); }

/* Set Early Abort */
__INLINE__ void HW_set_abort_on_wrong_am_addr(const u_int abort_on_wrong_am_addr) { mSetHWEntry( JAL_ABORT_ON_WRONG_AM_ADDR, abort_on_wrong_am_addr); }
/* Get Early Abort */
__INLINE__ u_int HW_get_abort_on_wrong_am_addr(void) { return mGetHWEntry( JAL_ABORT_ON_WRONG_AM_ADDR); }

/* Set RX Buffer */
__INLINE__ void HW_set_rx_buf(const u_int buf) { mSetHWEntry(JAL_RX_BUF, buf); }
/* Get RX Buffer */
__INLINE__ u_int HW_get_rx_buf(void) { return mGetHWEntry(JAL_RX_BUF); }

/* Set Error Sel */
__INLINE__ void   HW_set_err_sel(const u_int sel) { mSetHWEntry(JAL_ERR_SEL, sel); }
/* Get Error Sel */
__INLINE__ u_int32 HW_get_err_sel(void) { return mGetHWEntry(JAL_ERR_SEL); }

/* Set Window Extension */
__INLINE__ void   HW_set_win_ext(const u_int ext) { mSetHWEntry(JAL_WIN_EXT, ext); }
/* Get Window Extension */
__INLINE__ u_int32 HW_get_win_ext(void) { return mGetHWEntry(JAL_WIN_EXT); }

/* Set Freeze Bit Cnt */
__INLINE__ void   HW_set_freeze_bit_cnt(const u_int freeze) { mSetHWEntry(JAL_FREEZE_BIT_CNT, freeze); }
/* Get Freeze Bit Cnt */
__INLINE__ u_int  HW_get_freeze_bit_cnt(void) { return mGetHWEntry(JAL_FREEZE_BIT_CNT); }  

/*
 * RECEIVE STATUS REGISTERS - READ FUNCTIONS
 */

/* Get RX Length */
__INLINE__ u_int HW_get_rx_len(void) { return mGetHWEntry(JAL_RX_LEN); }

/* Get RX type */
#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define is_EDR_ACL_packet(pkt) (((1 <<  (pkt)) &  EDR_ACL_PACKET_MASK) !=0)
#define is_EDR_ESCO_packet(pkt) (((1 <<  (pkt)) &  EDR_ESCO_PACKET_MASK) !=0)
__INLINE__ u_int HW_get_rx_type(void) 
{
	u_int rx_type = mGetHWEntry(JAL_RX_TYPE);

	if ( (is_EDR_ESCO_packet((0x10|rx_type)) && mGetHWEntry(JAL_EDR_PTT_ESCO)) ||
         (is_EDR_ACL_packet((0x10|rx_type)) && mGetHWEntry(JAL_EDR_PTT_ACL)) )
	{
		rx_type |= 0x10;
	}

	return rx_type; 
}
#else
__INLINE__ u_int HW_get_rx_type(void) { return mGetHWEntry(JAL_RX_TYPE); }
#endif
/* Get RX flow */
__INLINE__ u_int HW_get_rx_flow(void) { return mGetHWEntry(JAL_RX_FLOW); }

/* Get RX arqn */
__INLINE__ u_int HW_get_rx_arqn(void) { return mGetHWEntry(JAL_RX_ARQN); }

/* Get RX seqn */
__INLINE__ u_int HW_get_rx_seqn(void) { return mGetHWEntry(JAL_RX_SEQN); }

/* Get RX AM Address */
__INLINE__ u_int HW_get_rx_am_addr(void) { return mGetHWEntry(JAL_RX_AM_ADDR); }

/* Get RX Link Controller Type */
__INLINE__ u_int HW_get_rx_l_ch(void) { return mGetHWEntry(JAL_RX_L_CH); }

/* Get RX p_flow bit */
__INLINE__ u_int HW_get_rx_p_flow(void) { return mGetHWEntry(JAL_RX_P_FLOW); }

/* Get RX packet */
__INLINE__ u_int HW_get_rx_pkt(void) { return mGetHWEntry(JAL_RX_PKT); }

/* Get HEC Error status */
__INLINE__ u_int HW_get_hec_err(void) { return mGetHWEntry(JAL_HEC_ERR); }

/* Get CRC Error status */
__INLINE__ u_int HW_get_crc_err(void) { return mGetHWEntry(JAL_CRC_ERR); }

/* Get RX_HDR status*/
__INLINE__ u_int HW_get_rx_hdr(void)  { return mGetHWEntry(JAL_RX_HDR); }

/* Get RX0 OVER */
__INLINE__ u_int HW_get_rx0_over(void) {return mGetHWEntry(JAL_RX0_OVER); }

/* Get RX0 UNDER */
__INLINE__ u_int HW_get_rx0_under(void) {return mGetHWEntry(JAL_RX0_UNDER); }

/* Get RX1 OVER */
__INLINE__ u_int HW_get_rx1_over(void) {return mGetHWEntry(JAL_RX1_OVER); }

/* Get RX1 UNDER */
__INLINE__ u_int HW_get_rx1_under(void) {return mGetHWEntry(JAL_RX1_UNDER); }

/* Get RX2 OVER */
__INLINE__ u_int HW_get_rx2_over(void) {return mGetHWEntry(JAL_RX2_OVER); }

/* Get RX2 UNDER */
__INLINE__ u_int HW_get_rx2_under(void) {return mGetHWEntry(JAL_RX2_UNDER); }

/* Get reset code */
__INLINE__ u_int HW_get_rst_code(void) { return mGetHWEntry(JAL_RST_CODE); }

/* Get RX error count */
__INLINE__ u_int32 HW_get_err_cnt(void) { return mGetHWEntry(JAL_ERR_CNT); }

/* Get RX AM_ADDR_ABORT  */
__INLINE__ u_int HW_get_am_addr_abort(void) { return mGetHWEntry(JAL_AM_ADDR_ABORT); }
                                                                 

/*
 * SERIAL REGISTERS
 */
/* Set Serial configuration */
__INLINE__ void HW_set_ser_cfg(const u_int ser_cfg) { mSetHWEntry(JAL_SER_CFG, ser_cfg); }
/* Get Serial Configuration */
__INLINE__ u_int HW_get_ser_cfg(void) { return mGetHWEntry(JAL_SER_CFG); }

/*
 * AUXILLIARY TIMER REGISTERS
 */
/* Set Aux Timer */
__INLINE__ void HW_set_aux_timer(const u_int aux_timer) { mSetHWEntry(JAL_AUX_TIMER, aux_timer); }
/* Get Aux Timer */
__INLINE__ u_int HW_get_aux_timer(void) { return mGetHWEntry(JAL_AUX_TIMER); }


/*
 * TRANSMIT and RECEIVE DELAY REGISTERS
 */
/* Set TX Delay*/
__INLINE__ void HW_set_tx_delay(const u_int value) { mSetHWEntry(JAL_TX_DELAY, value); }
/* Get TX Delay */
__INLINE__ u_int HW_get_tx_delay(void) { return mGetHWEntry(JAL_TX_DELAY); }

/* Set RX Delay */
__INLINE__ void HW_set_rx_delay(const u_int value) { mSetHWEntry(JAL_RX_DELAY, value); }
/* Get RX Delay */
__INLINE__ u_int HW_get_rx_delay(void) { return mGetHWEntry(JAL_RX_DELAY); }

/*
 * BT CLOCK CONTROL REGISTERS
 */
__INLINE__ void HW_set_add_bt_clk_relative(const u_int value) { mSetHWEntry(JAL_ADD_BT_CLK_RELATIVE, value); }
__INLINE__ u_int HW_get_add_bt_clk_relative(void) { return mGetHWEntry(JAL_ADD_BT_CLK_RELATIVE); }

__INLINE__ void HW_set_write_absolute_bt_clk(const u_int value) { mSetHWEntry(JAL_WRITE_ABSOLUTE_BT_CLK, value); }
__INLINE__ void HW_set_delayed_bt_clk_update(const u_int value) { mSetHWEntry(JAL_DELAYED_BT_CLK_UPDATE, value); }
__INLINE__ void HW_set_freeze_bt_clk(const u_int value) { mSetHWEntry(JAL_FREEZE_BT_CLK, value); }
__INLINE__ u_int HW_get_freeze_bt_clk(void) { return mGetHWEntry(JAL_FREEZE_BT_CLK); }

/*
 * REVISION CONTROL NUMBERS
 */
__INLINE__ u_int HW_get_minor_revision(void) { return mGetHWEntry(JAL_MINOR_REVISION); }
__INLINE__ u_int HW_get_major_revision(void) { return mGetHWEntry(JAL_MAJOR_REVISION); }

/*
 * RESET CONTROLS
 */
/* Set RST Code */
__INLINE__ void HWjal_Set_Rst_Code(const u_int rst_code) { mSetHWEntry(JAL_RST_CODE, rst_code); }


/*
 * AUXILLIARY FUNCTIONS
 */
__INLINE__ void HW_toggle_tx_buf(void) { mToggleHWBit(JAL_TX_BUF); }
__INLINE__ void HW_toggle_rx_buf(void) { mToggleHWBit(JAL_RX_BUF); }

/* 
 * eSCO Related
 */
/* Get eSCO LT_ADDR */
__INLINE__ u_int HW_get_esco_lt_addr(void) { return mGetHWEntry(JAL_ESCO_LT_ADDR); }

/* Get eSCO Tx Length */
__INLINE__ u_int HW_get_esco_tx_len(void) { return mGetHWEntry(JAL_ESCO_TX_LEN); }

/* Get eSCO Tx Length */
__INLINE__ u_int HW_get_esco_rx_len(void) { return mGetHWEntry(JAL_ESCO_RX_LEN); }

/* Get SCO_ROUTE */
__INLINE__ u_int HW_get_sco_route(void) { return mGetHWEntry(JAL_SCO_ROUTE); }

__INLINE__ void HW_set_use_hab_crl1(const u_int value) { mSetHWEntry(JAL_USE_HAB_CTRL,value); }

/* Test Functions for Software Simulation */
#if (BUILD_TYPE==UNIT_TEST_BUILD)
__INLINE__ u_int HW_get_pkd_intr_clr(void) { return mGetHWEntry(JAL_PKD_INTR_CLR); }
__INLINE__ u_int HW_get_pkd_rx_hdr_intr_clr(void) { return mGetHWEntry(JAL_PKD_RX_HDR_INTR_CLR); }
__INLINE__ u_int HW_get_pka_intr_clr(void) { return mGetHWEntry(JAL_PKA_INTR_CLR); }
__INLINE__ u_int HW_get_tim_intr_clr(void) { return mGetHWEntry(JAL_TIM_INTR_CLR); }
__INLINE__ u_int HW_get_aux_tim_intr_clr(void) { return mGetHWEntry(JAL_AUX_TIM_INTR_CLR); }

__INLINE__ void HW_set_pkd_intr(const u_int pkd_intr) {mSetHWEntry(JAL_PKD_INTR, pkd_intr); }
__INLINE__ void HW_set_pkd_rx_hdr_intr(const u_int pkd_rx_hdr_intr) {mSetHWEntry(JAL_PKD_RX_HDR_INTR, pkd_rx_hdr_intr); }
__INLINE__ void HW_set_pka_intr(const u_int pka_intr) {mSetHWEntry(JAL_PKA_INTR, pka_intr); }
__INLINE__ void HW_set_no_pkt_rcvd_intr(const u_int no_pkt_rcvd_intr) {mSetHWEntry(JAL_NO_PKT_RCVD_INTR, no_pkt_rcvd_intr); }
__INLINE__ void HW_set_sync_det_intr(const u_int sync_det_intr) {mSetHWEntry(JAL_SYNC_DET_INTR, sync_det_intr); }
__INLINE__ void HW_set_tim_intr(const u_int tim_intr) {mSetHWEntry(JAL_TIM_INTR, tim_intr); }
__INLINE__ void HW_set_aux_tim_intr(const u_int aux_tim_intr) {mSetHWEntry(JAL_AUX_TIM_INTR, aux_tim_intr); }

__INLINE__ void HW_set_tx0_over(u_int  tx0_over)  {mSetHWEntry(JAL_TX0_OVER, tx0_over); }
__INLINE__ void HW_set_tx0_under(u_int tx0_under) {mSetHWEntry(JAL_TX0_UNDER, tx0_under); }
__INLINE__ void HW_set_tx1_over(u_int  tx1_over)  {mSetHWEntry(JAL_TX1_OVER, tx1_over); }
__INLINE__ void HW_set_tx1_under(u_int tx1_under) {mSetHWEntry(JAL_TX1_UNDER, tx1_under); }
__INLINE__ void HW_set_tx2_over(u_int  tx2_over)  {mSetHWEntry(JAL_TX2_OVER, tx2_over); }
__INLINE__ void HW_set_tx2_under(u_int tx2_under) {mSetHWEntry(JAL_TX2_UNDER, tx2_under); }

__INLINE__ void HW_set_rx_len(u_int rx_len )   {mSetHWEntry(JAL_RX_LEN, rx_len); }
__INLINE__ void HW_set_rx_type(u_int rx_type ) {mSetHWEntry(JAL_RX_TYPE, rx_type); }
__INLINE__ void HW_set_rx_flow(u_int rx_flow ) {mSetHWEntry(JAL_RX_FLOW, rx_flow); }
__INLINE__ void HW_set_rx_arqn(u_int rx_arqn ) {mSetHWEntry(JAL_RX_ARQN, rx_arqn); }
__INLINE__ void HW_set_rx_seqn(u_int rx_seqn ) {mSetHWEntry(JAL_RX_SEQN, rx_seqn); }
__INLINE__ void HW_set_rx_am_addr(u_int rx_am_addr ) {mSetHWEntry(JAL_RX_AM_ADDR, rx_am_addr); }
__INLINE__ void HW_set_rx_l_ch(u_int rx_l_ch ) {mSetHWEntry(JAL_RX_L_CH, rx_l_ch); }
__INLINE__ void HW_set_rx_p_flow(u_int rx_p_flow )   {mSetHWEntry(JAL_RX_P_FLOW, rx_p_flow); }
__INLINE__ void HW_set_rx_pkt(u_int rx_pkt )   {mSetHWEntry(JAL_RX_PKT, rx_pkt); }
__INLINE__ void HW_set_hec_err(u_int hec_err ) {mSetHWEntry(JAL_HEC_ERR, hec_err); }
__INLINE__ void HW_set_crc_err(u_int crc_err ) {mSetHWEntry(JAL_CRC_ERR, crc_err); }
__INLINE__ void HW_set_rx_hdr(u_int rx_hdr )   {mSetHWEntry(JAL_RX_HDR, rx_hdr); }
__INLINE__ void HW_set_rx0_over(u_int rx0_over )     {mSetHWEntry(JAL_RX0_OVER, rx0_over); }
__INLINE__ void HW_set_rx0_under(u_int rx0_under )   {mSetHWEntry(JAL_RX0_UNDER, rx0_under); }
__INLINE__ void HW_set_rx1_over(u_int rx1_over )     {mSetHWEntry(JAL_RX1_OVER, rx1_over); }
__INLINE__ void HW_set_rx1_under(u_int rx1_under )   {mSetHWEntry(JAL_RX1_UNDER, rx1_under); }
__INLINE__ void HW_set_rx2_over(u_int rx2_over )     {mSetHWEntry(JAL_RX2_OVER, rx2_over); }
__INLINE__ void HW_set_rx2_under(u_int rx2_under)    {mSetHWEntry(JAL_RX2_UNDER, rx2_under); }
__INLINE__ void HW_set_rst_code(u_int rst_code )     {mSetHWEntry(JAL_RST_CODE, rst_code); }
__INLINE__ void HW_set_err_cnt(u_int err_cnt )       {mSetHWEntry(JAL_ERR_CNT, err_cnt); }

/*
 * eSCO related.
 */
__INLINE__ void HW_set_esco_lt_addr(u_int esco_lt_addr) {mSetHWEntry(JAL_ESCO_LT_ADDR, esco_lt_addr); }
__INLINE__ void HW_set_esco_rx_len(u_int esco_rx_len) {mSetHWEntry(JAL_ESCO_RX_LEN, esco_rx_len); }

#endif

__INLINE__ u_int HW_get_spi_now_conflict(void) { return mGetHWEntry(JAL_SPI_NOW_CONFLICT); }
__INLINE__ void HW_set_spi_now_conflict_clr(const u_int value) {mSetHWEntry(JAL_SPI_NOW_CONFLICT_CLR, value); }
__INLINE__ void HW_set_ser0_wr_clr(const u_int value) {mSetHWEntry(JAL_SER0_WR_CLR, value); }

__INLINE__ u_int HW_get_no_pkt_rcvd_intr_clr(void) { return mGetHWEntry(JAL_NO_PKT_RCVD_INTR_CLR); }

__INLINE__ u_int HW_get_no_pkt_rcvd_intr(void) { return mGetHWEntry(JAL_NO_PKT_RCVD_INTR); }
__INLINE__ void HW_set_no_pkt_rcvd_intr_clr(const u_int value) {mSetHWEntry(JAL_NO_PKT_RCVD_INTR_CLR, value); }

__INLINE__ void HW_set_no_pkt_rcvd_intr_mask(const u_int value) {mSetHWEntry(JAL_NO_PKT_RCVD_INTR_MSK, value); }
__INLINE__ u_int HW_get_no_pkt_rcvd_intr_mask(void) { return mGetHWEntry(JAL_NO_PKT_RCVD_INTR_MSK); }
 
__INLINE__ u_int HW_get_sync_det_intr_clr(void) { return mGetHWEntry(JAL_SYNC_DET_INTR_CLR); }

__INLINE__ u_int HW_get_sync_det_intr(void) { return mGetHWEntry(JAL_SYNC_DET_INTR); }
__INLINE__ void HW_set_sync_det_intr_clr(const u_int value) {mSetHWEntry(JAL_SYNC_DET_INTR_CLR, value); }

__INLINE__ void HW_set_sync_det_intr_mask(const u_int value) {mSetHWEntry(JAL_SYNC_DET_INTR_MSK, value); }
__INLINE__ u_int HW_get_sync_det_intr_mask(void) { return mGetHWEntry(JAL_SYNC_DET_INTR_MSK); }

#if (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
/****************************************************************
 *   Declare the functions used in accessing the tx and rx delays
 ****************************************************************/
__INLINE__ void HW_set_tx_edr_delay(const u_int value) {mSetHWEntry(JAL_EDR_TX_EDR_DELAY, value); }
__INLINE__ u_int HW_get_tx_edr_delay(void) {return mGetHWEntry(JAL_EDR_TX_EDR_DELAY);}

__INLINE__ void HW_set_rx_edr_delay(const u_int value)  {mSetHWEntry(JAL_EDR_RX_EDR_DELAY, value); }
__INLINE__ u_int HW_get_rx_edr_delay(void) {return mGetHWEntry(JAL_EDR_RX_EDR_DELAY);}

/*
 *  EDR_Sync_Err
 */
__INLINE__ void HW_set_edr_sync_err(const u_int value)  {mSetHWEntry(JAL_EDR_SYNC_ERROR, value); }
__INLINE__ u_int HW_get_edr_sync_err(void) { return mGetHWEntry(JAL_EDR_SYNC_ERROR);}
#endif

#if (PRH_BS_CFG_SYS_ESCO_VIA_VCI_SUPPORTED==1)

/****************************************************************
* Register definitions for FIFO interface to CODEC (eSCO via VCI)
****************************************************************/

__INLINE__ u_int HW_get_vci_tx_fifo_fill_level(void) { return mGetHWEntry(JAL_VCI_TX_FIFO_FILL_LEVEL);}
__INLINE__ u_int HW_get_vci_rx_fifo_fill_level(void) { return mGetHWEntry(JAL_VCI_RX_FIFO_FILL_LEVEL);}

__INLINE__ void HW_set_vci_tx_fifo_threshold(const u_int value) { mSetHWEntry(JAL_VCI_TX_FIFO_FILL_THRESHOLD, value);}
__INLINE__ void HW_set_vci_rx_fifo_threshold(const u_int value) { mSetHWEntry(JAL_VCI_RX_FIFO_FILL_THRESHOLD, value);}

__INLINE__ u_int HW_get_vci_tx_fill_status(void) { return mGetHWEntry(JAL_VCI_VTFH_TX_LEVEL_STATUS);}
__INLINE__ u_int HW_get_vci_rx_fill_status(void) { return mGetHWEntry(JAL_VCI_VRFL_RX_LEVEL_STATUS);}

__INLINE__ void HW_set_vci_rgf_fifo_reset(const u_int enable) { mSetHWEntry(JAL_VCI_VFIR_FIFO_RESET, enable);}
__INLINE__ void HW_set_vci_rgf_fifo_16bit_mode(const u_int enable) { mSetHWEntry(JAL_VCI_VFI16_FIFO_16BIT_MODE, enable);}
__INLINE__ void HW_set_vci_rgf_mode_enable(const u_int enable) { mSetHWEntry(JAL_VCI_VFIM_FIFO_RGF_MODE, enable);}

__INLINE__ void HW_write_vci_rx_fifo_data(const u_int32 data) { mSetHWEntry32(JAL_VCI_WR_FIFO_DATA, data);}
__INLINE__ u_int32 HW_read_vci_tx_fifo_data(void) { return mGetHWEntry32(JAL_VCI_RD_FIFO_DATA);}

#endif

__INLINE__ u_int HW_get_pta_grant_rx_denied(void) { return mGetHWEntry(JAL_PTA_GRANT_RX_DENIED);}
__INLINE__ u_int HW_get_pta_grant_tx_denied(void) { return mGetHWEntry(JAL_PTA_GRANT_TX_DENIED);}

__INLINE__ void HW_set_pta_mode_enable(const u_int mode) { mSetHWEntry(JAL_PTA_MODE_ENABLE, mode);}
__INLINE__ void HW_set_pta_grant_test_enable(const u_int mode) { mSetHWEntry(JAL_PTA_GRANT_TEST_ENABLE, mode);}
__INLINE__ void HW_set_pta_tx_active_gio_sel(const u_int gio_n) { mSetHWEntry(JAL_PTA_TX_ACTIVE_GIO_SEL, gio_n);}
__INLINE__ void HW_set_pta_grant_inverse(const u_int enable) { mSetHWEntry(JAL_PTA_GRANT_INVERSE, enable);}
__INLINE__ void HW_set_pta_grant_rx_override(const u_int enable) { mSetHWEntry(JAL_PTA_GRANT_RX_OVERRIDE, enable);}
__INLINE__ void HW_set_pta_bpktctl_grant(const u_int enable) { mSetHWEntry(JAL_PTA_BPKTCTL_GRANT, enable);}
#ifdef BT_DUALMODE

/*------------------charles modify, DualMode use only--------------------*/
__INLINE__ void HW_set_XVR_SW_reset(const u_int value) {mSetHWEntry(BEKEN_xver_soft_reset, value); }
__INLINE__ u_int HW_get_XVR_SW_reset(void) {return mGetHWEntry(BEKEN_xver_soft_reset);}

__INLINE__ u_int8 HW_Get_Native_Clk_Avoid_Race_last2bits(void)
{
    t_clock native_clk;
    native_clk = HW_Get_Native_Clk_Avoid_Race();
    return (native_clk&0x00000003);
}
__INLINE__ u_int8 HW_Get_BT_Clk_Avoid_Race_last2bits(void)
{
    t_clock native_clk;
    native_clk = HW_Get_Bt_Clk_Avoid_Race();
    return (native_clk&0x00000003);
}
#endif
/*------------------charles modify, DualMode use only--------------------*/

#endif
