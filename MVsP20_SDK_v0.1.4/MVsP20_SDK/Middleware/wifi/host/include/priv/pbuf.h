/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _PBUF_H_
#define _PBUF_H_
#include <porting.h>
#include <ssv_pktdef.h>
#include <host_config.h>

#define SSV_PBUF_DBG_FLAG_RESET      (0)
#define SSV_PBUF_DBG_FLAG_TCP        (1)
#define SSV_PBUF_DBG_FLAG_UDP        (2)
#define SSV_PBUF_DBG_FLAG_IP         (3)
#define SSV_PBUF_DBG_FLAG_ARP                   (4)
#define SSV_PBUF_DBG_FLAG_L2                    (5)
#define SSV_PBUF_DBG_FLAG_L2_TX_DRIVER          (6)
#define SSV_PBUF_DBG_FLAG_L2_CMDENG          	(7)


void        *PBUF_MAlloc_Raw(u32 size, u32 need_header, PBuf_Type_E buf_type);
void		_PBUF_MFree(void *PKTMSG);

#define PBUF_MAlloc(size, type)  PBUF_MAlloc_Raw(size, 1, type);
#define PBUF_MFree(PKTMSG) _PBUF_MFree(PKTMSG)
#define PBUF_isPkt(addr)    (1)

s32 PBUF_Init(u32 pbuf_no);


#endif /* _PBUF_H_ */





