/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _TXRX_HDL_H
#define _TXRX_HDL_H
#include <dev.h>
#include "host_apis.h"

struct wifi_flt
{
    u8 fc_b7b2;
    u8 b7b2mask;
    data_handler cb_fn;
};

struct eth_flt
{
    u16 ethtype;
    data_handler cb_fn;    
};

bool TxHdl_prepare_wifi_txreq(void *frame, u32 len, bool f80211, u32 priority, u8 tx_dscrp_flag);
bool TxHdl_FrameProc(void *frame, bool apFrame, u32 priority, u32 flags);
s32 TxHdl_FlushFrame(void);

s32 TxRxHdl_Init(void);
s32 RxHdl_FrameProc(void* frame);
s32 RxHdl_SetWifiRxFlt(struct wifi_flt *flt, ssv6xxx_cb_action act);
s32 RxHdl_SetEthRxFlt(struct eth_flt *flt, ssv6xxx_cb_action act);
void ieee80211_release_reorder_frames(struct rx_ba_session_desc *tid_agg_rx,
	u16 head_seq_num);
void ieee80211_delete_ampdu_rx(u8 wsid);
void ieee80211_delete_all_ampdu_rx(void);
void ieee80211_addba_handler(void *data);


#endif //_TXRX_HDL_H
