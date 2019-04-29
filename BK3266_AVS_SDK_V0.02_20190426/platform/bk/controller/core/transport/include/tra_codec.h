#ifndef _CEVA_TRA_CODEC_H_
#define _CEVA_TRA_CODEC_H_
/*****************************************************************************
 * MODULE NAME:    tra_codec.h
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    Codec FIFO Transport Interface
 * MAINTAINER:     Tom Kerwick
 * CREATION DATE:  27 Feb 2013
 *
 * SOURCE CONTROL: $Id: tra_codec.h,v 1.1.2.1 2013/03/08 16:47:49 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2013 Ceva Inc
 *     All rights reserved.
 *
 *****************************************************************************/

#if (PRH_BS_CFG_SYS_ESCO_VIA_VCI_SUPPORTED==1)
#define TRA_CODEC_VCI_TX_FIFO_SIZE 40 /* 40 samples (8-bit or 16-bit) */
#define TRA_CODEC_VCI_RX_FIFO_SIZE 40 /* 40 samples (8-bit or 16-bit) */


#if (ESCO_LINK_COMPENSATION_AS_HFP170 == 1)
#define CVSD_FS 60 /* Frame Size 60 samples*/
#define CVSD_N 120 /* 15ms - Window Length for pattern matching */
#define CVSD_M 40 /* 5ms - Template for matching */
#define CVSD_LHIST (CVSD_N+CVSD_FS-1) /* Length of history buffer required */
#define CVSD_SBCRT 16 /* CVSD Reconvergence Time (samples) */
#define CVSD_OLAL 32 /* OverLap-Add Length (samples) */
/* PLC State Information */
typedef struct cvsd_plc_state
{
	int16_t hist[CVSD_LHIST+CVSD_FS+CVSD_SBCRT+CVSD_OLAL];
	int16_t bestlag;
	int16_t nbf;
    int16_t mute;
    uint16_t vad_cnt;
}t_cvsd_plc_state;

/* Prototypes */
RAM_CODE void cvsd_init_plc(struct cvsd_plc_state *plc_state);
RAM_CODE void cvsd_plc_bad_frame(struct cvsd_plc_state *plc_state, int16_t *ZIRbuf,int16_t *out);
#ifdef BT_DUALMODE
RAM_CODE void cvsd_plc_good_frame(struct cvsd_plc_state *plc_state, int16_t *in,int16_t *out);
#else
DRAM_CODE void cvsd_plc_good_frame(struct cvsd_plc_state *plc_state, int16_t *in,int16_t *out);
#endif
#define MSBC_FS 120 /* Frame Size 60 samples*/
#define MSBC_N 256 /* 15ms - Window Length for pattern matching */
#define MSBC_M 64 /* 5ms - Template for matching */
#define MSBC_LHIST (MSBC_N+MSBC_FS-1) /* Length of history buffer required */
#define MSBC_SBCRT 36 /* CVSD Reconvergence Time (samples) */
#define MSBC_OLAL 16 /* OverLap-Add Length (samples) */
/* PLC State Information */
typedef struct msbc_plc_state
{
	int16_t hist[MSBC_LHIST+MSBC_FS+MSBC_SBCRT+MSBC_OLAL];
	int16_t bestlag;
	int16_t nbf;
    int16_t mute;
    uint16_t vad_cnt;
}t_msbc_plc_state;

/* Prototypes */
RAM_CODE void msbc_init_plc(struct msbc_plc_state *plc_state);
#ifdef BT_DUALMODE
RAM_CODE void msbc_plc_bad_frame(struct msbc_plc_state *plc_state, int16_t *ZIRbuf,int16_t *out);
RAM_CODE void msbc_plc_good_frame(struct msbc_plc_state *plc_state, int16_t *in,int16_t *out);
#else
DRAM_CODE void msbc_plc_bad_frame(struct msbc_plc_state *plc_state, int16_t *ZIRbuf,int16_t *out);
DRAM_CODE void msbc_plc_good_frame(struct msbc_plc_state *plc_state, int16_t *in,int16_t *out);
#endif

#endif



t_error RAM_CODE TRAcodec_Dispatch_Pending_SCO(void);
#endif /* PRH_BS_CFG_SYS_ESCO_VIA_VCI_SUPPORTED */

#endif /* _CEVA_TRA_CODEC_H_ */
