/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _DEV_H_
#define _DEV_H_
#include <host_apis.h>
#include "cmd_def.h"
#include <rtos.h>

//#ifdef ARRAY_SIZE
#undef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
//#endif




typedef u32 tx_result;
#define TX_CONTINUE	((tx_result) 0u)
#define TX_DROP		((tx_result) 1u)
#define TX_QUEUED	((tx_result) 2u)


//Off->Init->Running->Pause->Off


enum TXHDR_ENCAP {
	TXREQ_ENCAP_ADDR4               =0,
	TXREQ_ENCAP_QOS                 ,
	TXREQ_ENCAP_HT                  ,
};


#define IS_TXREQ_WITH_ADDR4(x)          ((x)->txhdr_mask & (1<<TXREQ_ENCAP_ADDR4) )
#define IS_TXREQ_WITH_QOS(x)            ((x)->txhdr_mask & (1<<TXREQ_ENCAP_QOS)   )
#define IS_TXREQ_WITH_HT(x)             ((x)->txhdr_mask & (1<<TXREQ_ENCAP_HT)    )

#define SET_TXREQ_WITH_ADDR4(x, _tf)    (x)->txhdr_mask = (((x)->txhdr_mask & ~(1<<TXREQ_ENCAP_ADDR4))	| ((_tf)<<TXREQ_ENCAP_ADDR4) )
#define SET_TXREQ_WITH_QOS(x, _tf)      (x)->txhdr_mask = (((x)->txhdr_mask & ~(1<<TXREQ_ENCAP_QOS))	| ((_tf)<<TXREQ_ENCAP_QOS)   )
#define SET_TXREQ_WITH_HT(x, _tf)       (x)->txhdr_mask = (((x)->txhdr_mask & ~(1<<TXREQ_ENCAP_HT))		| ((_tf)<<TXREQ_ENCAP_HT)    )



typedef enum txreq_type_en {
	TX_TYPE_M0 = 0,
	TX_TYPE_M1,
	TX_TYPE_M2,
} txreq_type;


#define HOST_RECOVERY_CB_NUM  1
#define HOST_DATA_CB_NUM  2
#define HOST_EVT_CB_NUM  4
#define PRMOISCUOUS_CB_NUM  1

typedef enum {
	SSV6XXX_CB_ADD		,
	SSV6XXX_CB_REMOVE	,
	SSV6XXX_CB_MOD		,
} ssv6xxx_cb_action;

typedef struct DeviceInfo{

    OsMutex g_dev_info_mutex;
    OsMutex g_dev_bcn_mutex;
	ssv6xxx_hw_mode hw_mode;
	txreq_type tx_type;
	u32 txhdr_mask;
    u8 self_mac[ETH_ALEN];
	ETHER_ADDR addr4;
	u16 qos_ctrl;
	u32 ht_ctrl;

    //Set key base on VIF.
    u8 beacon_upd_need:1;
    u8 rsvd:7;
    struct ApInfo *APInfo; // AP mode used
    struct APStaInfo *StaConInfo; // AP mode used
    struct cfg_join_request *joincfg; // Station mode used
    struct cfg_join_request *joincfg_backup; // Station mode used
    #if(ENABLE_DYNAMIC_RX_SENSITIVE==1)
    u16 cci_current_level; //Station mode used
    u16 cci_current_gate; //Station mode used
    #endif
    enum conn_status    status;                 //Auth,Assoc,Eapol
    struct ssv6xxx_ieee80211_bss *ap_list; //station ap info list used (useing "g_dev_info_mutex" to protect it)
    u32 channel_edcca_count[MAX_CHANNEL_NUM];
    u32 channel_packet_count[MAX_CHANNEL_NUM];
//-----------------------------
//Data path handler
//-----------------------------
	data_handler data_cb[HOST_DATA_CB_NUM];

//-----------------------------
//Event path handler
//-----------------------------
	evt_handler evt_cb[HOST_EVT_CB_NUM];
//-----------------------------
// promiscuous call back function
//-----------------------------
    promiscuous_data_handler promiscuous_cb[PRMOISCUOUS_CB_NUM];

//-----------------------------
// promiscuous call back function
//-----------------------------
    recovery_handler recovery_cb[HOST_RECOVERY_CB_NUM];


//reload fw count
    s32 reload_fw_cnt;

//timer interrupt count
    u32 fw_timer_interrupt;

}DeviceInfo_st;

extern DeviceInfo_st *gDeviceInfo;

struct rcs_info
{
    u16 free_page;
    u16 free_id;
    //u16 max_frame[PRI_Q_NUM];
};
extern struct rcs_info tx_rcs;
extern OsMutex txsrcMutex;


struct Host_cfg {     
    //1st 
    u32 upgrade_per:8;
    u32 downgrade_per:8;
    u32 rate_mask:16;

    //2nd 
    u32 upper_fastestb:8;
    u32 resent_fail_report:1;
    u32 pre_alloc_prb_frm:1;
    u32 direct_rc_down:1;
    u32 tx_power_mode:2;
    u32 ampdu_tx_enable:1;
    u32 ampdu_rx_enable:1;
    u32 ampdu_rx_buf_size:8;
    u32 force_RTSCTS:1;
    u32 bcn_interval:8;

    //3rd 
    u32 pool_size:16;
    u32 recv_buf_size:16;

    //4th
    u32 trx_hdr_len:16;
    u32 tx_res_page:8;
    u32 rx_res_page:8;

    //5th
    u32 ap_rx_support_legacy_rate_msk:12; 
    u32 ap_rx_support_mcs_rate_msk:8; 
    u32 ap_rx_short_GI:1;
    u32 erp:1;
    u32 b_short_preamble:1;
    u32 tx_retry_cnt:8;
    u32 tx_sleep:1;

    //6th
    u32 rc_drate_endian:1;
    u32 RSVD0:31;    
};
//AMPDU RX

#define RX_AGG_RX_BA_MAX_STATION				1
#define RX_AGG_RX_BA_MAX_SESSIONS				8
#define HT_RX_REORDER_BUF_TIMEOUT               10//ms

/**
 * struct tid_ampdu_rx - TID aggregation information (Rx).
 *
 * @reorder_buf: buffer to reorder incoming aggregated MPDUs
 * @reorder_time: jiffies when skb was added
 * @session_timer: check if peer keeps Tx-ing on the TID (by timeout value)
 * @reorder_timer: releases expired frames from the reorder buffer.
 * @head_seq_num: head sequence number in reordering buffer.
 * @stored_mpdu_num: number of MPDUs in reordering buffer
 * @ssn: Starting Sequence Number expected to be aggregated.
 * @buf_size: buffer size for incoming A-MPDUs
 * @timeout: reset timer value (in TUs).
 * @dialog_token: dialog token for aggregation session
 * @rcu_head: RCU head used for freeing this struct
 * @reorder_lock: serializes access to reorder buffer, see below.
 *
 * This structure's lifetime is managed by RCU, assignments to
 * the array holding it must hold the aggregation mutex.
 *
 * The @reorder_lock is used to protect the members of this
 * struct, except for @timeout, @buf_size and @dialog_token,
 * which are constant across the lifetime of the struct (the
 * dialog token being used only for debugging).
 */
struct rx_ba_session_desc {

	OsMutex reorder_lock;
	void **reorder_buf;//[RX_AGG_RX_BA_MAX_BUF_SIZE];
	unsigned long *reorder_time;//[RX_AGG_RX_BA_MAX_BUF_SIZE];
	u16 head_seq_num;
	u16 stored_mpdu_num;
	u16 ssn;
	u16 buf_size;
};

#endif /* _HOST_GLOBAL_ */

