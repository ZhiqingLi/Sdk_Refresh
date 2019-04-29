#ifndef _PARTHUS_SEC_TYPES_
#define _PARTHUS_SEC_TYPES_

/******************************************************************************
 * MODULE NAME:    lmp_sec_types.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Basic LMP security types definitions
 * MAINTAINER:     Daire McNamara
 * CREATION DATE:  8 February 2000 
 * SOURCE CONTROL: $Id: lmp_sec_types.h,v 1.15 2009/09/17 12:13:01 garyf Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2000-2004 Ceva Inc.
 *   All rights reserved.
 *
 * REVISION HISTORY:
 *
 ******************************************************************************/
#include "sys_config.h"

#define MAX_LINK_KEYS 4


typedef enum {
    VARIABLE,
    FIXED 
} t_pin_type;


typedef enum {
    SEMI_PERMANENT,
    TEMPORARY
} t_link_key_life;


typedef u_int8 t_link_key_type;

#define COMBINATION_KEY                 0
#define LOCAL_UNIT_KEY                  1
#define REMOTE_UNIT_KEY                 2
#define DEBUG_COMBINATION_KEY           3
#define UNAUTHENTICATED_COMBINATION_KEY 4
#define AUTHENTICATED_COMBINATION_KEY   5
#define CHANGED_COMBINATION_KEY         6

#define UNIT_KEY_DEVICE        0
#define COMBINATION_KEY_DEVICE 1

#define HCI_SUPER_STATE(X) ( (((X)>>1) & 1) & (!((X) & 1)) )
#define ACL_SUPER_STATE(X) ( (((X)>>1) & 1) & ((X) & 1) )

#define SET_SUPER_STATE_PROC(X, Y) ((X) = (((X) & 3)|(Y)))

#define AUTHENTICATION_PROC         0x04
#define ORIG_PAIRING_PROC           0x08
#define TERM_PAIRING_PROC           0x10
#define SET_ENCR_ON_PROC            0x20
#define HCI_MASTER_LINKKEY_PROC     0x40
#define HCI_CHANGE_COMBKEY_PROC     0x80
#define INITIAL_SSP                 0x100                 

#define AUTHENTICATION_PROC_ON(X)     ((X) & AUTHENTICATION_PROC)
#define ORIG_PAIRING_PROC_ON(X)       ((X) & ORIG_PAIRING_PROC)
#define TERM_PAIRING_PROC_ON(X)       ((X) & TERM_PAIRING_PROC)
#define SET_ENCR_ON_PROC_ON(X)        ((X) & SET_ENCR_ON_PROC)

/*
 * In order to be a master link procedure
 * and not to be a term change comb key
 * procedure the top bit must be off.
 */
#define HCI_MASTER_LINKKEY_PROC_ON(X) ( (  (X) & HCI_MASTER_LINKKEY_PROC) \
                                        && (!((X) & 0x80)))
#define HCI_CHANGE_COMBKEY_PROC_ON(X) ((X) & HCI_CHANGE_COMBKEY_PROC)


typedef  u_int16 t_sec_super_state;

#define  SUPER_STATE_IDLE    0x00
#define  AUTHENTICATION      0x04
#define  HCI_AUTHENTICATION  0x06
#define  ACL_AUTHENTICATION  0x07
#define  ORIG_PAIRING        0x08
#define  HCI_ORIG_PAIRING    0x0A
#define  ACL_ORIG_PAIRING    0x0B
#define  TERM_PAIRING        0x10
#define  HCI_TERM_PAIRING    0x12
#define  ACL_TERM_PAIRING    0x13
#define  SET_ENCR_ON         0x20
#define  HCI_SET_ENCR_ON     0x22
#define  ACL_SET_ENCR_ON     0x23
#define  SET_ENCR_OFF        0x24
#define  HCI_SET_ENCR_OFF    0x26
#define  HCI_MASTER_LINKKEY  0x40
#define  ORIG_CHANGE_COMBKEY 0x80
#define  TERM_CHANGE_COMBKEY 0xc0


enum e_sec_sub_state {
  SUB_STATE_IDLE,
  W4_LINK_KEY_REQ_REPLY,
  W4_LINK_KEY_REQ_REPLY_CHALL_PENDING,
  W4_PIN_CODE_REQ_REPLY,
  W4_IN_RAND_RESPONSE,
  W4_SRES,
  W4_COMB_KEY_RESPONSE,
  W4_UNIT_KEY_RESPONSE,
  W4_ENCR_MODE_REPLY,
  W4_ENCR_KEY_SIZE_REQ,
  W4_ENCR_KEY_SIZE_REPLY, // 10
  W4_START_ENCR_REQ,
  W4_START_ENCR_REPLY,
  W4_STOP_ENCR_REPLY,
  W4_STOP_ENCR_REQ,
  W4_USE_SEMI_PERM_REPLY,
  SETUP_ENCR_KEY_SIZE_PENDING, // 0x10
  W4_MASTER_AUTH,
  W4_PEER_LINKKEY,
  W4_PEER_AUTH,
  PEER_AUTH_COMPLETE,
  W2_RESUME_ENC,
  W4_HCI_IO_CAP_REQUEST_REPLY,
  W4_LMP_IO_CAP_RESPONSE,
  W4_LMP_IO_CAP_REQUEST,
  W4_LMP_PUBLIC_KEY_HEADER,
  W4_LMP_PUBLIC_KEY_PAYLOAD,
  W4_LMP_ACK_PUBLIC_KEY_HEADER,
  W4_LMP_ACK_PUBLIC_KEY_PAYLOAD,
  W4_LMP_SIMPLE_PAIRING_CONFIRM,
  W4_LMP_SIMPLE_PAIRING_NUMBER,
  W4_LMP_ACK_SIMPLE_PAIRING_NUMBER,
  W4_HCI_USER_CONFIRMATION_REQUEST_REPLY, // 0x20
  W4_LMP_ACK_DHKEY_CHECK,
  W4_LMP_PEER_DHKEY_CHECK,
  W4_HCI_KEY_PRESS_NOTIFICATION,
  W4_HCI_REMOTE_OOB_DATA_REQUEST_REPLY
};

/*
 * Note there is a coupling between
 * the size of the enumerated type
 * above and the size of the typedef
 * (which contains them) below
 */
typedef u_int8 t_sec_sub_state;


typedef void t_sec_event_info;

typedef struct {
  t_pin_code pin_code;
  u_int8 pin_code_length;
}t_pin_code_info;


#endif

