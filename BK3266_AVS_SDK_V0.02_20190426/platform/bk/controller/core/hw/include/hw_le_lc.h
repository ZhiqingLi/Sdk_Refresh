#ifndef _PARTHUS_HW_LE_LC_
#define _PARTHUS_HW_LE_LC_

#include "sys_types.h"
#include "sys_config.h"
#include "sys_features.h"
#include "hw_macro_defs.h"

#include "hw_jalapeno.h"                   /*Dedicated to hardware platform */

void HWlelc_Initialise(void);

/****************************************************************
*
*  Declare the functions used in accessing the Jalapeno
*  common control registers
*
****************************************************************/

__INLINE__ void HWle_set_le_mode(void);
__INLINE__ void HWle_clear_le_mode(void);

__INLINE__ u_int HWle_get_le_mode(void);


__INLINE__ void HWle_set_master_mode(void);
__INLINE__ void HWle_clear_master_mode(void);

__INLINE__ u_int HWle_get_master_mode(void);

__INLINE__ void HWle_set_adv_state(void);
__INLINE__ void HWle_clear_adv_state(void);

__INLINE__ u_int HWle_get_adv_state(void);

__INLINE__ void HWle_set_tx_enable(void);
__INLINE__ void HWle_clear_tx_enable(void);

__INLINE__ u_int HWle_get_tx_enable(void);

__INLINE__ void HWle_set_scan_state(const u_int);
__INLINE__ u_int HWle_get_scan_state(void);

__INLINE__ void HWle_set_tifs_default(void);
__INLINE__ void HWle_clear_tifs_default(void);
__INLINE__ u_int HWle_get_tifs_default(void);

__INLINE__ void HWle_set_tifs_abort(void);
__INLINE__ void HWle_clear_tifs_abort(void);

__INLINE__ u_int HWle_get_tifs_abort(void);


__INLINE__ void HWle_set_tifs_delay(const u_int);
__INLINE__ u_int HWle_get_tifs_delay(void);


__INLINE__ void HWle_set_tifs_default(void);
__INLINE__ void HWle_clear_tifs_default(void);
__INLINE__ u_int HWle_get_tifs_default(void);

__INLINE__ void HW_set_pd_addr(const t_bd_addr *);
__INLINE__ void HW_set_rd_addr(const t_bd_addr *);


__INLINE__ void HWle_set_acc_addr(const u_int);
__INLINE__ u_int HWle_get_acc_addr(void);



__INLINE__ void HWle_set_le_preamble(const u_int);
__INLINE__ u_int HWle_get_le_preamble(void);

__INLINE__ void HWle_set_crc_init(const u_int);
__INLINE__ u_int HWle_get_crc_init(void);

// Tranmist Control Functions

__INLINE__ void HWle_set_tx_pdu_type(const u_int);
__INLINE__ u_int HWle_get_tx_pdu_type(void);

__INLINE__ void HWle_set_tx_rfu1_adv(const u_int);
__INLINE__ u_int HWle_get_tx_rfu1_adv(void);

__INLINE__ void HWle_set_tx_tx_add(const u_int);
__INLINE__ u_int HWle_get_tx_tx_add(void);

__INLINE__ void HWle_set_tx_rx_add(const u_int);
__INLINE__ u_int HWle_get_tx_rx_add(void);

__INLINE__ void HWle_set_tx_length_adv(const u_int);
__INLINE__ u_int HWle_get_tx_length_adv(void);

__INLINE__ void HWle_set_tx_rfu2_adv(const u_int);
__INLINE__ u_int HWle_get_tx_rfu2_adv(void);

__INLINE__ void HWle_set_tx_nesn(const u_int);
__INLINE__ u_int HWle_get_tx_nesn(void);

__INLINE__ void HWle_set_tx_sn(const u_int);
__INLINE__ u_int HWle_get_tx_sn(void);

__INLINE__ void HWle_set_tx_md(void);
__INLINE__ void HWle_clear_tx_md(void);

__INLINE__ u_int HWle_get_tx_md(void);

__INLINE__ void HWle_set_tx_length_data(const u_int);
__INLINE__ u_int HWle_get_tx_length_data(void);


// Revice Control Functions

__INLINE__ void HWle_set_rx_le_preamble(const u_int);
__INLINE__ u_int HWle_get_rx_le_preamble(void);

__INLINE__ void HWle_set_rx_crc_init(const u_int);
__INLINE__ u_int HWle_get_rx_crc_init(void);

__INLINE__ void HWle_set_rx_pdu_type(const u_int);
__INLINE__ u_int HWle_get_rx_pdu_type(void);

__INLINE__ void HWle_set_rx_rfu1_adv(const u_int);
__INLINE__ u_int HWle_get_rx_rfu1_adv(void);

__INLINE__ void HWle_set_rx_tx_add(const u_int);
__INLINE__ u_int HWle_get_rx_tx_add(void);

__INLINE__ void HWle_set_rx_rx_add(const u_int);
__INLINE__ u_int HWle_get_rx_rx_add(void);

__INLINE__ void HWle_set_tx_length_adv(const u_int);
__INLINE__ u_int HWle_get_tx_length_adv(void);

__INLINE__ void HWle_set_rx_rfu2_adv(const u_int);
__INLINE__ u_int HWle_get_rx_rfu2_adv(void);

__INLINE__ void HWle_set_rx_nesn(const u_int);
__INLINE__ u_int HWle_get_rx_nesn(void);

__INLINE__ void HWle_set_rx_sn(const u_int);
__INLINE__ u_int HWle_get_rx_sn(void);

__INLINE__ void HWle_set_rx_md(const u_int);
__INLINE__ u_int HWle_get_rx_md(void);

__INLINE__ void HWle_set_tx_length_data(const u_int);
__INLINE__ u_int HWle_get_tx_length_data(void);


__INLINE__ void HWle_set_tx_llid(const u_int llid);
__INLINE__ u_int HWle_get_tx_llid(void);

//__INLINE__ void HWle_set_tx_llid(const u_int llid);
__INLINE__ u_int HWle_get_rx_llid(void);


__INLINE__ void HWle_set_rx_mic(const u_int);
__INLINE__ u_int HWle_get_rx_mic(void);

__INLINE__ void HWle_set_tx_mic(const u_int);
__INLINE__ u_int HWle_get_tx_mic(void);

__INLINE__ void HWle_set_delay_search_win(const u_int );
__INLINE__ u_int HWle_get_delay_search_win(void);

__INLINE__ void HWle_set_whitening_enable(void);
__INLINE__ void HWle_clear_whitening_enable(void);
__INLINE__ u_int HWle_get_whitening_enable(void);

__INLINE__ void HWle_set_whitening_init(const u_int );
__INLINE__ u_int HWle_get_whitening_init(void);

__INLINE__ u_int HWle_get_crc_err(void);

__INLINE__ void HWle_set_le_spi_only(void);
__INLINE__ void HWle_clear_le_spi_only(void);
__INLINE__ u_int HWle_get_le_spi_only(void);
__INLINE__ void HWle_set_win_offset_interval_in_tx_buffer(u_int16 offset,u_int16 interval);
__INLINE__ void HWle_set_advertiser_address_in_tx_buffer(const u_int8* pAddr);

__INLINE__ void HWle_set_advertiser_addr1_in_tx_buffer(u_int32 val);
__INLINE__ void HWle_set_advertiser_addr2_in_tx_buffer(u_int32 val);

__INLINE__ void HWle_set_aes_mode(const u_int aes_mode);
__INLINE__ void HWle_clear_aes_enable(void);
__INLINE__ void HWle_set_aes_enable(void);
__INLINE__ void HWle_set_aes_start(void);
__INLINE__ u_int HWle_get_aes_active(void);
__INLINE__ u_int HWle_get_aes_finished(void);
__INLINE__ void HWle_set_aes_llid(const u_int aes_llid);
__INLINE__ void HWle_set_aes_pkt_length(const u_int aes_pkt_length);
__INLINE__ void HWle_set_aes_data_ready(void);
__INLINE__ u_int HWle_get_aes_mic_status(void);

__INLINE__ void HWle_set_aes_pktcntr_byte0(const u_int byte0);
__INLINE__ void HWle_set_aes_pktcntr_byte1(const u_int byte1);
__INLINE__ void HWle_set_aes_pktcntr_byte2(const u_int byte2);
__INLINE__ void HWle_set_aes_pktcntr_byte3(const u_int byte3);
__INLINE__ void HWle_set_aes_pktcntr_byte4(const u_int byte4);

__INLINE__ u_int HWle_get_rx_length_adv(void);
__INLINE__ void HWle_abort_tifs_count(void);
__INLINE__ u_int HWle_get_tx_data_header(void);
__INLINE__ u_int HWle_get_rx_length_data(void);
__INLINE__ u_int HWle_get_rx_data_header(void);

__INLINE__ void HWle_set_tx_test_pdu_rfu1(const u_int rfu1 );
__INLINE__ void HWle_set_tx_test_pdu_rfu2(const u_int rfu2 );
__INLINE__ void HWle_set_tx_test_length(const u_int length);
__INLINE__ void HWle_set_tx_test_pdu_type(const u_int pdu_type);

#if (PRH_SYS_CFG_HARDWARE_ADDRESS_FILTERING_SUPPORTED == 1)
__INLINE__ void HWle_set_immediate_filter_tx_addr_type(const u_int addr_type);
__INLINE__ u_int HWle_get_immediate_filter_tx_addr_type(void);

__INLINE__ void HWle_set_target_address_rx_addr_type(const u_int addr_type);
__INLINE__ u_int HWle_get_target_address__rx_addr_type(void);

__INLINE__ void HWle_set_rx_filters_active(u_int filters_active); 
__INLINE__ u_int HWle_get_rx_filters_active(void);
__INLINE__ u_int HWle_get_target_address_match(void);
__INLINE__ u_int HWle_get_target_last_rxed(void);

__INLINE__ void HWle_set_rx_mode(u_int rx_mode);
__INLINE__ u_int HWle_get_rx_mode(void);

__INLINE__ u_int HWle_get_rx_filtered_enum(void);

__INLINE__ void HWle_set_address_filtering(void);
__INLINE__ void HWle_clear_address_filtering(void);
__INLINE__ void HWle_set_CONNECT_REQ_filtering(void);
__INLINE__ void HWle_clear_CONNECT_REQ_filtering(void);
__INLINE__ void HWle_set_SCAN_REQ_filtering(void);
__INLINE__ void HWle_clear_SCAN_REQ_filtering(void);
__INLINE__ u_int HWle_get_Rx_Control_Byte2(void);

#else
__INLINE__ void HWle_set_rx_mode(u_int rx_mode);
__INLINE__ void HWle_clear_address_filtering(void);
#endif

#if defined(TEAKLITE4) && !defined(TEAKLITE4_IO_BB_PORT)
#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs).
#define ADDR_DIV	2 //(CHAR_BIT/8)
#else
#define ADDR_DIV	1
#endif

#if (PRH_BS_CFG_SYS_LE_DUAL_MODE == 1)
#ifndef JAL_LE_TX_ACL_BASE_ADDR
#define JAL_LE_TX_ACL_BASE_ADDR     (JAL_BASE_ADDR + 0x200/ADDR_DIV)//0x80006200 
#endif
#ifndef JAL_LE_RX_ACL_BASE_ADDR
#define JAL_LE_RX_ACL_BASE_ADDR     (JAL_BASE_ADDR + 0x400/ADDR_DIV) //0x80006400 
#endif
#define HWle_get_tx_acl_buf_addr()  JAL_LE_TX_ACL_BASE_ADDR
#define HWle_get_rx_acl_buf_addr()  JAL_LE_RX_ACL_BASE_ADDR
#else // SINGLE_MODE
#define HWle_get_tx_acl_buf_addr()  (0x000000A0 + 0x0)
#define HWle_get_rx_acl_buf_addr()  (0x000000D0 + 0x0)
#endif

#define HWle_get_aes_key_addr()     JAL_LE_AES_KEY_ADDR
#define HWle_get_aes_iv_addr()      JAL_LE_AES_IV_ADDR
#define HEle_get_aes_pkt_counter_addr() JAL_LE_AES_PKT_COUNTER_ADDR
#define HWle_get_aes_data_addr()    JAL_LE_AES_BUFFER_ADDR
#ifdef __USE_INLINES__
#include "hw_le_lc_impl.h"
#endif

#endif
