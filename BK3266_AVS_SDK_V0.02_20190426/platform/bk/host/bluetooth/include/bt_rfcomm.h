/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*-
 * Copyright (c) 2006 Itronix Inc.
 * All rights reserved.
 *
 * Written by Iain Hibbert for Itronix Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Itronix Inc. may not be used to endorse
 *    or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ITRONIX INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ITRONIX INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*-
 * Copyright (c) 2001-2003 Maksim Yevmenkin <m_evmenkin@yahoo.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _BT_RFCOMM_H_
#define _BT_RFCOMM_H_

/*************************************************************************
 **      Contains RFCOMM specification definitions and structs          **
 *************************************************************************/

#include "bluetooth.h"
#include <include/uw_types.h>
#include <include/uw_compiler.h>

#define RFCOMM_MTU_MAX                  32767
#define RFCOMM_MTU_MIN                  23
#define RFCOMM_MTU_DEFAULT              127

#define RFCOMM_CREDITS_MAX              255     /* in any single packet */
#define RFCOMM_CREDITS_DEFAULT          7       /* default initial value */

#define RFCOMM_CHANNEL_ANY              0
#define RFCOMM_CHANNEL_MIN              1
#define RFCOMM_CHANNEL_MAX              30

/* RFCOMM frame types */
#define RFCOMM_FRAME_SABM               0x2f
#define RFCOMM_FRAME_DISC               0x43
#define RFCOMM_FRAME_UA                 0x63
#define RFCOMM_FRAME_DM                 0x0f
#define RFCOMM_FRAME_UIH                0xef

/* RFCOMM MCC commands */
#define RFCOMM_MCC_TEST                 0x08    /* Test */
#define RFCOMM_MCC_FCON                 0x28    /* Flow Control on */
#define RFCOMM_MCC_FCOFF                0x18    /* Flow Control off */
#define RFCOMM_MCC_MSC                  0x38    /* Modem Status Command */
#define RFCOMM_MCC_RPN                  0x24    /* Remote Port Negotiation */
#define RFCOMM_MCC_RLS                  0x14    /* Remote Line Status */
#define RFCOMM_MCC_PN                   0x20    /* Port Negotiation */
#define RFCOMM_MCC_NSC                  0x04    /* Non Supported Command */

/* RFCOMM modem signals */
#define RFCOMM_MSC_FC                   0x02    /* Flow Control asserted */
#define RFCOMM_MSC_RTC                  0x04    /* Ready To Communicate */
#define RFCOMM_MSC_RTR                  0x08    /* Ready To Receive */
#define RFCOMM_MSC_IC                   0x40    /* Incomming Call (RING) */
#define RFCOMM_MSC_DV                   0x80    /* Data Valid */

/* RPN parameters - baud rate */
#define RFCOMM_RPN_BR_2400              0x0
#define RFCOMM_RPN_BR_4800              0x1
#define RFCOMM_RPN_BR_7200              0x2
#define RFCOMM_RPN_BR_9600              0x3
#define RFCOMM_RPN_BR_19200             0x4
#define RFCOMM_RPN_BR_38400             0x5
#define RFCOMM_RPN_BR_57600             0x6
#define RFCOMM_RPN_BR_115200            0x7
#define RFCOMM_RPN_BR_230400            0x8

/* RPN parameters - data bits */
#define RFCOMM_RPN_DATA_5               0x0
#define RFCOMM_RPN_DATA_6               0x1
#define RFCOMM_RPN_DATA_7               0x2
#define RFCOMM_RPN_DATA_8               0x3

/* RPN parameters - stop bit */
#define RFCOMM_RPN_STOP_1               0
#define RFCOMM_RPN_STOP_15              1

/* RPN parameters - parity enable */
#define RFCOMM_RPN_PARITY_NONE          0x0

/* RPN parameters - parity type */
#define RFCOMM_RPN_PARITY_ODD           0x0
#define RFCOMM_RPN_PARITY_EVEN          0x1
#define RFCOMM_RPN_PARITY_MARK          0x2
#define RFCOMM_RPN_PARITY_SPACE         0x3

/* RPN parameters - default line_setting */
#define RFCOMM_RPN_8_N_1                0x03

/* RPN parameters - flow control */
#define RFCOMM_RPN_XON_CHAR             0x11
#define RFCOMM_RPN_XOFF_CHAR            0x13
#define RFCOMM_RPN_FLOW_NONE            0x00

/* RPN parameters - mask */
#define RFCOMM_RPN_PM_RATE              0x0001
#define RFCOMM_RPN_PM_DATA              0x0002
#define RFCOMM_RPN_PM_STOP              0x0004
#define RFCOMM_RPN_PM_PARITY            0x0008
#define RFCOMM_RPN_PM_PTYPE             0x0010
#define RFCOMM_RPN_PM_XON               0x0020
#define RFCOMM_RPN_PM_XOFF              0x0040

#define RFCOMM_RPN_PM_FLOW              0x3f00

#define RFCOMM_RPN_PM_ALL \
    (RFCOMM_RPN_PM_RATE | RFCOMM_RPN_PM_DATA | RFCOMM_RPN_PM_STOP | \
    RFCOMM_RPN_PM_PARITY | RFCOMM_RPN_PM_PTYPE | RFCOMM_RPN_PM_XON | \
    RFCOMM_RPN_PM_XOFF | RFCOMM_RPN_PM_FLOW)

/* RFCOMM command frame header */
  struct rfcomm_cmd_hdr
{
    uint8_t             address;
    uint8_t             control;
    uint8_t             length;
    uint8_t             fcs;
} __PACKED_POST__;

/* RFCOMM MSC command */
  struct rfcomm_mcc_msc
{
    uint8_t             address;
    uint8_t             modem;
    uint8_t             brk;
} __PACKED_POST__;

/* RFCOMM RPN command */
  struct rfcomm_mcc_rpn
{
    uint8_t             dlci;
    uint8_t             bit_rate;
    uint8_t             line_settings;
    uint8_t             flow_control;
    uint8_t             xon_char;
    uint8_t             xoff_char;
    uint16_t            param_mask;
} __PACKED_POST__;

/* RFCOMM RLS command */
  struct rfcomm_mcc_rls
{
    uint8_t             address;
    uint8_t             status;
} __PACKED_POST__;

/* RFCOMM PN command */
  struct rfcomm_mcc_pn
{
    uint8_t             dlci;
    uint8_t             flow_control;
    uint8_t             priority;
    uint8_t             ack_timer;
    uint16_t            mtu;
    uint8_t             max_retrans;
    uint8_t             credits;
} __PACKED_POST__;

/* RFCOMM frame parsing macros */
#define RFCOMM_DLCI(b)                  (((b) & 0xfc) >> 2)
#define RFCOMM_TYPE(b)                  (((b) & 0xef))

#define RFCOMM_EA(b)                    (((b) & 0x01))
#define RFCOMM_CR(b)                    (((b) & 0x02) >> 1)
#define RFCOMM_PF(b)                    (((b) & 0x10) >> 4)

#define RFCOMM_CHANNEL(dlci)            (((dlci) >> 1) & 0x2f)
#define RFCOMM_DIRECTION(dlci)          ((dlci) & 0x1)

#define RFCOMM_MKADDRESS(cr, dlci) \
    ((((dlci) & 0x3f) << 2) | ((cr) << 1) | 0x01)

#define RFCOMM_MKCONTROL(type, pf)      ((((type) & 0xef) | ((pf) << 4)))
#define RFCOMM_MKDLCI(dir, channel)     ((((channel) & 0x1f) << 1) | (dir))

/* RFCOMM MCC macros */
#define RFCOMM_MCC_TYPE(b)              (((b) & 0xfc) >> 2)
#define RFCOMM_MCC_LENGTH(b)            (((b) & 0xfe) >> 1)
#define RFCOMM_MKMCC_TYPE(cr, type)     ((((type) << 2) | ((cr) << 1) | 0x01))

/* RPN macros */
#define RFCOMM_RPN_DATA_BITS(line)      ((line) & 0x3)
#define RFCOMM_RPN_STOP_BITS(line)      (((line) >> 2) & 0x1)
#define RFCOMM_RPN_PARITY(line)         (((line) >> 3) & 0x1)
#define RFCOMM_RPN_PTYPE(line)          (((line) >> 4) & 0x3)

/* MSC "break" macros */
#define RFCOMM_MSC_BREAK(msc_brk)       (((msc_brk) >> 4) & 0xf)
#define RFCOMM_MSC_MKBREAK(brk)         (((brk) << 4) & 0xf0)

/* XXX: consider using BT_LINK_MODE_xxx directly for all profiles */
/* RFCOMM link mode flags */
#define RFCOMM_LM_NONE          BT_LINK_MODE_NONE      /* unsecure */
#define RFCOMM_LM_AUTH          BT_LINK_MODE_AUTH      /* want authentication */
#define RFCOMM_LM_ENCRYPT       BT_LINK_MODE_ENCRYPT   /* want encryption */
#define RFCOMM_LM_SECURE        BT_LINK_MODE_SECURE    /* want secured link */

/*
 * Upper layers custom commands
 */

/* Modem Status Command - called when Modem Status Command is received */
#define BT_RFCOMM_CMD_RECV_MSC     1

typedef struct {
    uint8_t modem;
    int8_t brk;
} bt_rfcomm_cmd_recv_msc_t;

/* PN definitions */
#define RFCOMM_PN_REQ_CFC_SUPPORTED     0xf0
#define RFCOMM_PN_RSP_CFC_SUPPORTED     0xe0

#endif /* _BT_RFCOMM_H_ */

