#ifndef _PARTHUS_SYS_TYPES_
#define _PARTHUS_SYS_TYPES_

/******************************************************************************
 * MODULE NAME:    sys_types.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    All System Wide Type Definitions
 * MAINTAINER:     John Nelson, Gary Fleming, Conor Morris
 * DATE:           1 May 1999
 *
 * SOURCE CONTROL: $Id: sys_types.h,v 1.59 2014/08/08 15:51:59 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.f
 *
 * REVISION HISTORY:
 *    01 May 1999  -    jn      - Initial version 0.8
 *    01 June 1999 -    jn      - Update to V0.9
 *    22 July 1999 -    gf      - Updated to support higher layers (error code, 
 *                                event structure, srMode, spMode, 
 *    08 Mar 2001  -    js      - Moved intrinsic type use to hal_sys_types.h 
 *                                for portability
 *
 * NOTES TO USERS:
 *    Little endian format used, i.e. least significant byte in lowest address    
 ******************************************************************************/

/*
 * The following include file should be provided by any ANSI C compiler
 * and compiler definitions such as size_t etc.
 */

#include <stddef.h>

#include <limits.h> // CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs).

/*
 * Basic system types such as u_int8, u_int16 etc. are found in the following
 * platform specific file.
 */
#include "types.h"
#include "sys_const.h"

//move global definitions to here from make file
#define JAL_BASE_ADDR              0x00F76000
#define HAB_BASE_ADDR              0x00F78000
#define HW_DATA32
#define __BYTE_ORDER               __LITTLE_ENDIAN
#define __USE_INLINES__
#define BUILD_TYPE                 FLASH_BUILD
#include "sys_features.h"
#include "config.h"

typedef enum {JUNGO_HOST = 0, CEVA_HOST = 1, NONE_CONTROLLER = 2} HOST_MODE;
#define ADDR_ALIGNED(addr, align) ((addr)+(align)-1)/(align)*(align)
#if 0
typedef enum {
    FLASH_LINE_1=0,
    FLASH_LINE_2,
    FLASH_LINE_4
} FLASH_LINE_MODE;
#endif
#define FLASH_LINE_1    0
#define FLASH_LINE_2	1
#define FLASH_LINE_4 	2


#define FLASH_CLK_26mHz 8
#define FLASH_CLK_39mHz 5
#define FLASH_CLK_78mHz 4

#define CPU_DCO_CLK     120 // 96  /* CPU clk = 96MHz */
#define CPU_CLK_SEL     3

#ifdef BT_ONE_TO_MULTIPLE
#define CPU_CLK_DIV     1
#else
#define CPU_CLK_DIV     1
#endif
#define SNIFF_CPU_CLK_SEL 1
#define SNIFF_CPU_CLK_DIV 1
//#define BT_HOST_MODE    CEVA_HOST  /* for Charles BQB */
#define BT_HOST_MODE    JUNGO_HOST
#ifdef CONFIG_TWS
#define FLASH_CLK_SEL   FLASH_CLK_39mHz //FLASH_CLK_39mHz//FLASH_CLK_78mHz
#else
#define FLASH_CLK_SEL   FLASH_CLK_39mHz //FLASH_CLK_39mHz//FLASH_CLK_78mHz
#endif

#ifdef BT_ONE_TO_MULTIPLE
#ifdef BT_DUALMODE
#define DEFAULT_LINE_MODE FLASH_LINE_2
#else
#define DEFAULT_LINE_MODE FLASH_LINE_2
#endif
#else
	#ifdef CONFIG_TWS
#define DEFAULT_LINE_MODE FLASH_LINE_2 //FLASH_LINE_2 // FLASH_LINE_2
	#else
#define DEFAULT_LINE_MODE FLASH_LINE_2 //FLASH_LINE_2 // FLASH_LINE_2
	#endif
#endif

#define BEKEN_DEBUG/* */
/* #define HCI_DEBUG */
/* #define JMALLOC_STATISTICAL */

#ifdef BEKEN_DEBUG
void DEBUG_MSG(unsigned int val);
void HCI_MSG (unsigned char msg, unsigned int value);
#endif

/* 
 * CHAR_BIT - Defines the number of bits in a byte - typically 8, but can be 16 (DSPs). 
  * If so masking required on some char operations -
 */
#if (CHAR_BIT==8)
#define _SYS_GET_CHAR_8_BITS(x) (x)
#define _SYS_MASK_CHAR_TO_8_BITS(x)
#else
#define _SYS_GET_CHAR_8_BITS(x) ((x) & 0xFF)
#define _SYS_MASK_CHAR_TO_8_BITS(x) { x &= 0xFF; }
#endif

/* 
 * Baseband originated types                                               
 */
typedef t_u_int64 t_syncword;        /* Sync word                          */
typedef u_int32   t_uap_lap;         /* Uap [31:24], Lap [23:0             */

typedef enum
{
   TX_START     = 0,       /* Corresponds to Interrupt Tim0 */
   TX_MID       = 1,       /* Corresponds to Interrupt Tim1 */
   RX_START     = 2,       /* Corresponds to Interrupt Tim2 */
   RX_MID       = 3,       /* Corresponds to Interrupt Tim3 */
   NO_FRAME_POS = 5        /* No valid frame position */
} t_frame_pos;             /* The Tx/Rx positions in a 2-slot frame */


typedef u_int8  t_freq;        /* frequency type */
typedef u_int8  t_uap;         /* upper address part type */
typedef u_int32 t_lap;         /* lower address part type */
typedef u_int16 t_nap;         /* non-significant address part type */
typedef u_int32 t_clock;       /* clock type */
typedef u_int16 t_status;      /* status type */
typedef u_int8  t_am_addr;     /* AM address type */
typedef u_int8  t_lt_addr;     /* LT address type */

typedef u_int8  t_deviceIndex;          /* index to a device */
typedef u_int8  t_piconet_index;        /* index to piconet */
typedef u_int8* t_p_pdu;                /* pointer to a PDU */
typedef u_int16 t_connectionHandle;     /* connection handle */

#ifndef PRH_COMBINED_STACK_COMMON_TYPES
#define PRH_COMBINED_STACK_COMMON_TYPES 1
typedef u_int32 t_classDevice;          /* class of device */
#endif

typedef u_int8  t_sco_cfg;     /* sco codec configuration */
typedef u_int8  t_sco_fifo;    /* sco fifo */

/*
 * BD Address Type and associated operations
 */
typedef struct
{
   /*
    * u_int8 lap_byte0, lap_byte1, lap_byte2, uap, nap_byte0, nap_byte 1
    */
   u_int8 bytes[6];
} t_bd_addr;

#define BDADDR_Get_LAP(p_bd_addr)                                       \
            ( ((t_lap) (p_bd_addr)->bytes[2] << 16) +                   \
            (  (t_lap) (p_bd_addr)->bytes[1] << 8) + (p_bd_addr)->bytes[0] )

#define BDADDR_Get_UAP(p_bd_addr)                                       \
            ( (t_uap) (p_bd_addr)->bytes[3] )

#define BDADDR_Get_NAP(p_bd_addr)                                       \
            ( (t_nap) (((t_nap) (p_bd_addr)->bytes[5] << 8) +           \
              (p_bd_addr)->bytes[4]))

#define BDADDR_Get_UAP_LAP(p_bd_addr)                                   \
             ( (t_uap_lap) BDADDR_Get_UAP(p_bd_addr)<<24 |              \
               (t_uap_lap) BDADDR_Get_LAP(p_bd_addr) )

#define BDADDR_Get_Byte_Array_Ex(p_bd_addr, p_byte_array)               \
             { const u_int8 *p_bda = (p_bd_addr)->bytes;                \
               u_int8 *p_byte = p_byte_array;                           \
               *p_byte++ = *p_bda++;                                    \
               *p_byte++ = *p_bda++;                                    \
               *p_byte++ = *p_bda++;                                    \
               *p_byte++ = *p_bda++;                                    \
               *p_byte++ = *p_bda++;                                    \
               *p_byte   = *p_bda;                                      \
             }

#define BDADDR_Get_Byte_Array_Ref(p_bd_addr)                            \
            ( (p_bd_addr)->bytes )

#define BDADDR_Set_LAP(p_bd_addr, LAP)                                  \
            {  t_lap lap = (LAP);                                       \
               (p_bd_addr)->bytes[0] = (u_int8)(lap & 0xFF);            \
               (p_bd_addr)->bytes[1] = (u_int8)((lap>>8) & 0xFF);       \
               (p_bd_addr)->bytes[2] = (u_int8) (lap>>16); }

#define BDADDR_Set_UAP(p_bd_addr, UAP)                                  \
            ((p_bd_addr)->bytes[3] = (UAP))

#define BDADDR_Set_NAP(p_bd_addr, NAP)                                  \
            {  t_nap nap = (NAP);                                       \
               (p_bd_addr)->bytes[4] = (u_int8) (nap & 0xFF);           \
               (p_bd_addr)->bytes[5] = (u_int8) (nap>>8); }

#define BDADDR_Assign_from_Byte_Array(p_bd_addr, p_byte_array)          \
            {  u_int8 *p_bda = (p_bd_addr)->bytes;                      \
               const u_int8 *p_byte = (p_byte_array);                   \
               *p_bda++ = *p_byte++;                                    \
               *p_bda++ = *p_byte++;                                    \
               *p_bda++ = *p_byte++;                                    \
               *p_bda++ = *p_byte++;                                    \
               *p_bda++ = *p_byte++;                                    \
               *p_bda   = *p_byte;                                      \
            }

#define BDADDR_Set_to_Zero(p_bd_addr)                                   \
            {  u_int8 *p_bda = (p_bd_addr)->bytes;                      \
               u_int8 *p_bda_end = (p_bd_addr)->bytes+6;                \
               while (p_bda < p_bda_end) { *p_bda++ = 0; }              \
            }

#define BDADDR_Set_LAP_UAP_NAP(p_bd_addr, lap, uap, nap)                \
            {  BDADDR_Set_LAP(p_bd_addr, lap);                          \
               BDADDR_Set_UAP(p_bd_addr, uap);                          \
               BDADDR_Set_NAP(p_bd_addr, nap); }

#define BDADDR_Copy(p_bd_addr_dest, p_bd_addr_src)                      \
            ( *p_bd_addr_dest = *p_bd_addr_src )

#define BDADDR_Is_Equal(p_bd_addr_1, p_bd_addr_2)                       \
       ((BDADDR_Get_LAP(p_bd_addr_1) == BDADDR_Get_LAP(p_bd_addr_2)) && \
        (BDADDR_Get_UAP(p_bd_addr_1) == BDADDR_Get_UAP(p_bd_addr_2)) && \
        (BDADDR_Get_NAP(p_bd_addr_1) == BDADDR_Get_NAP(p_bd_addr_2)) )

#define BDADDR_Convert_to_U32x2(p_bd_addr, p_u32_words)                 \
        {  p_u32_words[0] = BDADDR_Get_UAP_LAP(p_bd_addr);              \
           p_u32_words[1] = BDADDR_Get_NAP(p_bd_addr);  }


typedef enum
{
    LCH_null        = 0,
    LCH_continue    = 1,
    LCH_start       = 2,
    LMP_msg         = 3,
    LE_LLC          = 3
}  t_LCHmessage;

typedef enum
{
    MASTER          = 0,
    SLAVE           = 1
} t_role;


typedef u_int8  t_piconetIndex;  /* an index into a piconet */
typedef u_int16 t_length;        /* maximum data packet length type */

/*
 * Note LM Air Mode is distinct from HCI Voice Settings
 */
typedef enum {
    LM_AIR_MODE_U_LAW,
    LM_AIR_MODE_A_LAW,
    LM_AIR_MODE_CVSD,
    LM_AIR_MODE_TRANSPARENT
} t_lm_air_mode;


typedef t_clock t_timer;         /* timer type */
typedef t_clock t_slots;         /* slots type */

/* 
 * Define the packet types
 *
 * t_packet         represents a single packet type e.g. DM1
 * t_packetTypes    represents a bit field representation of 
 *                  packets DM1_BIT|DM3_BIT
 */

#if 1//(PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
#define MAX_PACKET_TYPES 32
#else
#define MAX_PACKET_TYPES 24
#endif
typedef enum
{
    NULLpkt     =  0,   /* a NULL packet */
    POLLpkt     =  1,   /* a POLL packet */
    FHSpkt      =  2,   /* an FHS packet */
    DM1         =  3,   /* a DM1 packet */
    DH1         =  4,   /* a DH1 packet */
    HV1         =  5,   /* a HV1 packet */
    HV2         =  6,   /* a HV2 packet */
    HV3         =  7,   /* a HV3 packet */
    DV          =  8,   /* a DV packet */
    AUX1        =  9,   /* an AUX1 packet */
    DM3         = 10,   /* a DM3 packet */
    DH3         = 11,   /* a DH3 packet */
    EV4         = 12,   /* an EV4 packet */
    EV5         = 13,   /* an EV5 packet */
    DM5         = 14,   /* a DM5 packet */
    DH5         = 15,   /* a DH5 packet */
    IDpkt       = 16,   /* an ID packet */
    INVALIDpkt  = 17,   /* an unexpected packet type */
    // TK: always define EDR types - makes for cleaner code when applying bit masks

#if 1 // (PRH_BS_CFG_SYS_LMP_EDR_SUPPORTED==1)
    EDR_2DH1    = 20, // 0x14  
    EDR_2EV3    = 22, // 0x16
	EV3         = 21,//23 /* CM: 11 Sep 2007. To be set to 21 */   
	
 //    EV3         = 23,   
 /* an EV3 packet. N.B. EV3 packet shares 7 with HV3. Moved
                            to 7 | 0b10000. Ensure top bit is sheared off before
                            exporting outside controller to either host or peer! */
    EDR_3EV3    = 23,//0x17 /* CM: 11 Sep 2007. To be set to 23 */         
	EDR_3DH1    = 24, // 0x18
	EDR_AUX1    = 25, // 0x19
    EDR_2DH3    = 26, // 0x1A
    EDR_3DH3    = 27, // 0x1B  
    EDR_2EV5    = 28, // 0x1C
    EDR_3EV5    = 29, // 0x1D
    EDR_2DH5    = 30, // 0x1E  
    EDR_3DH5    = 31  // 0x1F
#else

	EV3         = 0x17
#endif

} t_packet;       

/*
 * Create type to support bit combination of supported packet types. This generates
 * a different bit mask to assigned over HCI for EDR to p_link->packet_types.
 */

#define DM1_BIT_MASK         (1UL<<DM1)
#define DH1_BIT_MASK         (1UL<<DH1)
#define HV1_BIT_MASK         (1UL<<HV1)
#define HV2_BIT_MASK         (1UL<<HV2)
#define HV3_BIT_MASK         (1UL<<HV3)
#define DV_BIT_MASK          (1UL<<DV)
#define AUX1_BIT_MASK        (1UL<<AUX1)
#define DM3_BIT_MASK         (1UL<<DM3)
#define DH3_BIT_MASK         (1UL<<DH3)
#define EV3_BIT_MASK         (1UL<<EV3)
#define EV4_BIT_MASK         (1UL<<EV4)
#define EV5_BIT_MASK         (1UL<<EV5)
#define DM5_BIT_MASK         (1UL<<DM5)
#define DH5_BIT_MASK         (1UL<<DH5)

#define BR_ACL_PACKET_MASK   ((1UL<<DH1)|(1UL<<DM1)|(1UL<<DH3)|(1UL<<DM3)|(1UL<<DH5)|(1UL<<DM5))

#define BR_SYN_PACKET_MASK   ((1UL<<EV3)|(1UL<<EV4)|(1UL<<EV5)|(1UL<<HV1)|(1UL<<HV2)|(1UL<<HV3))

#define BR_PACKET_MASK       (BR_ACL_PACKET_MASK|BR_SYN_PACKET_MASK|(1UL<<AUX1)|(1UL<<DV))


#define EDR_2DH1_BIT_MASK    (1UL<<EDR_2DH1)
#define EDR_2EV3_BIT_MASK    (1UL<<EDR_2EV3)
#define EDR_3EV3_BIT_MASK    (1UL<<EDR_3EV3)
#define EDR_3DH1_BIT_MASK    (1UL<<EDR_3DH1)
#define EDR_2DH3_BIT_MASK    (1UL<<EDR_2DH3)
#define EDR_3DH3_BIT_MASK    (1UL<<EDR_3DH3)
#define EDR_2EV5_BIT_MASK    (1UL<<EDR_2EV5)
#define EDR_3EV5_BIT_MASK    (1UL<<EDR_3EV5)
#define EDR_2DH5_BIT_MASK    (1UL<<EDR_2DH5)
#define EDR_3DH5_BIT_MASK    (1UL<<EDR_3DH5)
#define EDR_AUX1_BIT_MASK    (1UL<<EDR_AUX1)

#define EDR_PACKET_MASK      ( (1UL<<EDR_2DH1) | (1UL<<EDR_2EV3) | (1UL<<EDR_3EV3) | (1UL<<EDR_3DH1) | \
                               (1UL<<EDR_2DH3) | (1UL<<EDR_3DH3) | (1UL<<EDR_2EV5) | (1UL<<EDR_3EV5) | \
                               (1UL<<EDR_2DH5) | (1UL<<EDR_3DH5) | (1UL<<EDR_AUX1))

#define EDR_ACL_PACKET_MASK  ( (1UL<<EDR_2DH1) | (1UL<<EDR_3DH1) | (1UL<<EDR_2DH3) | \
                               (1UL<<EDR_3DH3) | (1UL<<EDR_2DH5) | (1UL<<EDR_3DH5) | (1UL<<EDR_AUX1) )

#define EDR_2MBITS_PACKET_MASK   ( (1UL<<EDR_2DH1) | (1UL<<EDR_2DH3) |  (1UL<<EDR_2DH5) |\
                                   (1UL<<EDR_2EV3) | (1UL<<EDR_2EV5) |  (1UL<<EDR_AUX1))

#define EDR_3MBITS_PACKET_MASK  ( (1UL<<EDR_3DH1) | (1UL<<EDR_3DH3) | (1UL<<EDR_3DH5) | \
	                              (1UL<<EDR_3EV3) | (1UL<<EDR_3EV5) | (1UL<<EDR_AUX1))

#define EDR_ACL_CRC_PACKET_MASK  (1UL<<EDR_2DH1) | (1UL<<EDR_3DH1) | (1UL<<EDR_2DH3) | \
                               (1UL<<EDR_3DH3) | (1UL<<EDR_2DH5) | (1UL<<EDR_3DH5)

#define EDR_ESCO_PACKET_MASK ( (1UL<<EDR_2EV3) | (1UL<<EDR_3EV3) | (1UL<<EDR_2EV5) | (1UL<<EDR_3EV5))

#define EDR_ESCO_SINGLE_SLOT_PACKET_MASK (1UL<<EDR_2EV3) | (1UL<<EDR_3EV3)

#define EDR_ESCO_THREE_SLOT_PACKET_MASK (1UL<<EDR_2EV5) | (1UL<<EDR_3EV5)

#define EDR_ACL_SINGLE_SLOT_PACKET_MASK  (1UL<<EDR_2DH1) | (1UL<<EDR_3DH1)

#define EDR_ACL_MULTI_SLOT_PACKET_MASK  (1UL<<EDR_2DH3) | (1UL<<EDR_3DH3) | (1UL<<EDR_2DH5) | (1UL<<EDR_3DH5) 

#define ACL_PACKET_MASK  (1UL<<DM1)|(1UL<<DM3)|(1UL<<DM5) | (1UL<<DV) |(1UL<<DH1)|(1UL<<DH3)| \
	                              (1UL<<DH5) | (1UL<<AUX1)

#define ACL_CRC_PACKET_MASK  (1UL<<DM1)|(1UL<<DM3)|(1UL<<DM5)|(1UL<<DV)|(1UL<<DH1)|(1UL<<DH3)| \
	                              (1UL<<DH5)

#define ACL_SINGLE_SLOT_PACKET_MASK  (1UL<<DM1)| (1UL<<DV) |(1UL<<DH1)|(1UL<<AUX1)

#define ACL_MULTI_SLOT_PACKET_MASK  (1UL<<DM3)|(1UL<<DM5)|(1UL<<DH3)|(1UL<<DH5)

#define ESCO_PACKET_MASK  (1UL<<EV3) | (1UL<<EV4) | (1UL<<EV5)

#define ESCO_SINGLE_SLOT_PACKET_MASK  (1UL<<EV3)

#define ESCO_THREE_SLOT_PACKET_MASK  (1UL<<EV4) | (1UL<<EV5)

#ifdef _MSC_VER
#if (_MSC_VER==1200)
 typedef __int64 u_int64;
#define _LL(x) x
#endif
#else
 typedef unsigned long long int u_int64;  
#define _LL(x) x##ll
#endif

typedef u_int32 t_packetTypes;   /* packet-types based up | of above */

typedef enum
{
    NAK=0, 
    ACK=1
} t_arqn;

typedef enum
{
   STOP=0, 
   GO=1
} t_flow;

typedef enum
{
    TX_READY                =  0,       /* Ready to transmit */
    RX_READY                =  1,       /* Ready to receive */
    TX_OK                   =  2,       /* Transmission successful */
    TX_NONE                 =  3,       /* No Transmission occurred */
    RX_OK                   =  4,       /* Reception successful */
    RX_NONE                 =  5,       /* No Reception active */
    RX_DUPLICATE            =  6,       /* Received packet was interpreted as duplicate */
    RX_NO_PACKET            =  7,       /* Received no packet */
    RX_RESTRICTED_PACKET    =  8,       /* Received a restricted baseband packet */
    RX_AMADDR_ERROR         =  9,       /* Incorrect AM address in received packet */
    RX_HEC_ERROR            = 10,       /* HEC error in received packet */
    RX_CRC_ERROR            = 11,       /* CRC error in received packet */
    TX_2_IDS_IN_SLOT        = 12,       /* Indicates that 2_IDS were tx in this slot */
    TX_UNEXPECTED_ERROR     = 13,       /* Unexpected PKA */
    RX_UNEXPECTED_ERROR     = 14,       /* Unexpected PKD */
    RX_BROADCAST_PACKET     = 15,       /* Rx ACL Broadcast Packet */
    RX_BROADCAST_DUPLICATE  = 16,       /* Rx ACL Broadcast Duplicate, for L2CAP cont*/
    RX_NORMAL_HEADER        = 17,       /* Rx ACL packet header */
    RX_BROADCAST_HEADER     = 18,       /* Rx ACL Broadcast Packet Header */
    RX_MISSED_HEADER        = 19,        /* Header For This Packet Not Processed */
    RX_NORMAL_HEADER_PRIMARY_LT_ADDR = 20,
    RX_NORMAL_HEADER_SECONDARY_LT_ADDR    = 21
} t_TXRXstatus;                         /* Receive and Transmit Status Values */


typedef enum
{ 
     SCO_LINK=0x00,     /* SCO Link */
     ACL_LINK=0x01,      /* ACL Link */
     ESCO_LINK=0x02
} t_linkType; /* link type */


enum e_error_codes
{
#ifndef NO_ERROR
    NO_ERROR = 0x00,                         /* No Error */
#endif
    ILLEGAL_COMMAND  = 0x01,                 /* Illegal Command */
    NO_CONNECTION    = 0x02,                 /* No Connection   */
    HARDWARE_FAILURE = 0x03,                 /* Hardware Failure */
    PAGE_TIMEOUT     = 0x04,                 /* Page Timeout Occurred */
    AUTHENTICATION_FAILURE = 0x05,           /* Authentication Failed */
    PIN_MISSING      = 0x06,                 /* PIN missing */
    MEMORY_FULL      = 0x07,                 /* Out of memory */
    CONNECTION_TIMEOUT = 0x08,               /* Connection timed out */
    MAX_NUM_CONNECTIONS = 0x09,              /* Maximum number of connections exceeded */
    MAX_NUM_SCO      = 0x0A,                 /* Maximum number of SCO connections exceeded */
    ACL_ALREADY_EXISTS = 0x0B,               /* Maximum number of ACL connections exceeded */
    COMMAND_DISALLOWED = 0x0C,               /* Command was disallowed */
    HOST_REJECT_LIMITED_RESOURCES = 0x0D,    /* Host rejected, due to limited resources */
    HOST_REJECT_SECURITY = 0x0E,             /* Host rejected, due to security issues */
    HOST_REJECT_PERSONAL_DEVICE = 0x0F,      /* Host rejected, personal device */
    CONNECTION_ACCEPT_TIMEOUT = 0x10,        /* Host timed out */
    UNSUPPORTED_FEATURE  = 0x11,             /* Stack does not support this feature */
    INVALID_HCI_PARAMETERS = 0x12,           /* Illegal HCI parameter(s) in request */
    REMOTE_USER_TERMINATED_CONNECTION = 0x13, /* Remote user terminated the connection */
    REMOTE_RESOURCES_TERMINATED_CONNECTION = 0x14, /* Remote rejected, limited resources */
    REMOTE_POWER_TERMINATED_CONNECTION = 0x15,     /* Remote rejected, limited power */
    LOCAL_TERMINATED_CONNECTION = 0x16,      /* local device terminated connection */
    REPEATED_ATTEMPTS = 0x17,                /* max number of repeated attempts exceeded */
    PAIRING_NOT_ALLOWED = 0x18,              /* pairing not allowed */
    UNKNOWN_LMP_PDU = 0x19,                  /* unknown type of lmp pdu*/
    UNSUPPORTED_REMOTE_FEATURE = 0x1A,       /* unsupported lmp feature */
    SCO_OFFSET_REJECTED  = 0x1B,             /* rejected, sco offset unsuitable */
    SCO_INTERVAL_REJECTED = 0x1C,            /* rejected, sco interval unsuitable*/
    SCO_AIR_MODE_REJECTED = 0x1D,            /* rejected, sco air-mode unsuitable */
    INVALID_LMP_PARAMETERS = 0x1E,           /* invalid lmp parameters */
    UNSPECIFIED_ERROR = 0x1F,                /* unspecified error */
    UNSUPPORTED_PARAMETER_VALUE = 0x20,      /* unsupported parameter value */
    ROLE_CHANGE_NOT_ALLOWED = 0x21,
    LMP_RESPONSE_TIMEOUT = 0x22,
    LMP_ERROR_TRANSACTION_COLLISION = 0x23,  /* used by Master when both LMs initiate the same procedure simultaneously */
    LMP_PDU_NOT_ALLOWED = 0x24,
    ENCRYPTION_MODE_NOT_ACCEPTABLE = 0x25,
    UNIT_KEY_USED = 0x26,
    QOS_NOT_SUPPORTED = 0x27,
    INSTANT_PASSED = 0x28,
    PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED = 0x29,

    /*
     * V1.2 Error Codes
     */
	EC_DIFFERENT_TRANSACTION_COLLISION = 0x2A,
	EC_INSUFFICIENT_RESOURCES_FOR_SCATTER_MODE_REQUEST = 0x2B,
	EC_QOS_UNACCEPTABLE_PARAMETER = 0x2C,
	EC_QOS_REJECTED = 0x2D,
    EC_CHANNEL_CLASSIFICATION_NOT_SUPPORTED = 0x2E,
    EC_INSUFFICIENT_SECURITY = 0X2F,
    EC_PARAMETER_OUT_OF_MANDATORY_RANGE = 0X30,
	EC_SCATTER_MODE_NO_LONGER_REQUIRED = 0x31,
	EC_ROLE_SWITCH_PENDING = 0x32,
	EC_SCATTER_MODE_PARAMETER_CHANGE_PENDING = 0x33,
    EC_RESERVED_SLOT_VIOLATION = 0X34,
    EC_ROLE_SWITCH_FAILED = 0x35,
	/*
	 * V2.1 Error Codes
	 */

	 EC_EXTENDED_INQUIRY_RESPONSE_TOO_LARGE = 0x36,
	 EC_SSP_NOT_SUPPORTED_BY_HOST = 0x37,
	 EC_HOST_BUSY_PAIRING = 0x38,

	/*
	 * V4.0 Error Codes
	 */
	EC_CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL_FOUND = 0x39,
	EC_CONTROLLER_BUSY = 0x3A,
	EC_UNACCEPTABLE_CONNECTION_INTERVAL = 0x3B,
	EC_DIRECTED_ADVERTISING_TIMEOUT = 0x3C,
	EC_CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE = 0x3D,
	EC_CONNECTION_FAILED_TO_BE_ESTABLISHED = 0x3E,
	EC_MAC_CONNECTION_FAILED = 0x3F,

    /*
     * Extensions, used for Parthus Abstract Test Suites
     * Values never used and hence relocatable!
     */
    PRTH_TEST_PASSED_DEVIATED,
    PRTH_TEST_FAILED,
	

    /*
     * Test results that don't result in failure
     */
    PRTH_TEST_IS_INCOMPLETE,
    PRTH_TEST_DUPLICATE,
    PRTH_TEST_REQUIRES_HARDWARE,
    PRTH_TEST_IS_OBSOLETE,
    PRTH_TEST_IS_AMBIGUOUS,
    PRTH_TEST_NOT_SUPPORTED,
    PRTH_TEST_NOT_DEFINED,
    PRTH_TEST_LMP_FEATURE_IS_SUPPORTED,
    PRTH_TEST_LMP_FEATURE_IS_NOT_SUPPORTED,
    PRTH_TEST_SYS_FEATURE_IS_SUPPORTED,
    PRTH_TEST_SYS_FEATURE_IS_NOT_SUPPORTED,
    PRTH_TEST_SKIPPED,
	PRTH_TEST_IP_DEVELOPMENT_FAILURE

};  /*  Error Messages used by the HC, LMP, L2CAP */


/*
 * Error Messages used by the HC, LMP, L2CAP and ATS Tests
 */
typedef enum e_error_codes t_error;

#define PRTH_TEST_PASSED NO_ERROR



/*
 * The following structure represents the definitions for LMP encrypt_mode
 * Note hardware encrypt register is 0 off, 1 p2p, 2 broadcast, 3 p2p+broadcast
 */
typedef enum
{ 
    ENCRYPT_NONE=0,                     /* No Encryption                     */
    ENCRYPT_POINT2POINT=1,              /* Non-Broadcast / P to P Encryption */
    ENCRYPT_POINT2POINT_BROADCAST=2,    /* Both Broadcast and PtoP Encrypted */
    ENCRYPT_HARDWARE_ONLY_ALL=3,        /* Remap of 2 for hardware           */
    ENCRYPT_MASTER_RX_ONLY=8+1,         /* Master Tx Unencrypt, Rx Encrypt   */
    ENCRYPT_MASTER_TX_ONLY=16+1         /* Master Tx Encrypt, Rx UnEncrypt   */

} t_encrypt_mode;                       /* Encryption Mode                   */


#define ENCRYPT_PAUSED_INITIATED_BY_PEER_DEVICE             0x01
#define ENCRYPT_PAUSED_INITIATED_BY_LOCAL_DEVICE            0x02
#define ENCRYPT_RESUME_PENDING_SWITCH_COMPLETE              0x04
#define ENCRYPT_RESUME_PENDING_LINK_KEY_CHANGE_EVENT        0x08


typedef struct t_hci_event 
{
    t_bd_addr bd_addr;
    t_connectionHandle handle; 
    t_error status;
    t_linkType linkType;
    t_encrypt_mode encrypt_mode;  /* Default Encryption Disabled */
    t_classDevice classDevice;
    t_error reason;
} t_hci_event;


typedef enum
{
    STANDARD_SCAN   = 0,
    INTERLACED_SCAN = 1,
    CONTINUOUS_SCAN = 2    
} t_scan_type;


typedef struct t_scanActivity
{
    u_int16     interval; /* scan interval */
    u_int16     window;   /* scan activity window */
    t_scan_type scan_type; /*Interlaced/Non interlaced, One_shot/Continuous*/
}t_scanActivity;   /* scan activity type */


typedef enum
{
    R0          = 0, 
    R1          = 1, 
    R2          = 2
} t_pageScanRepit; /* page scan repetitions type */

typedef enum
{
   P0           = 0, 
   P1           = 1, 
   P2           = 2
} t_pageScanPeriod; /* page scan period type */

typedef enum
{
    MANDATORY_PAGE_SCAN     = 0,        /* Mandatory Page Scan Mode */
    OPTIONAL_PAGE_SCAN_1    = 1,        /* Type 1 Optional Page Scan Mode */
    OPTIONAL_PAGE_SCAN_2    = 2,        /* Type 2 Optional Page Scan Mode */
    OPTIONAL_PAGE_SCAN_3    = 3         /* Type 3 Optional Page Scan Mode */
} t_pageScanMode;                       /* Types of page scan modes */

typedef enum t_scanEnable
{
    NO_SCANS_ENABLED            = 0x00, /* No scanning enabled */
    INQUIRY_SCAN_ONLY_ENABLED   = 0x01, /* Only inquiry scan enabled */
    PAGE_SCAN_ONLY_ENABLED      = 0x02, /* Only page scan enabled */
    BOTH_SCANS_ENABLED          = 0x03  /* Both page scan and inquiry scan enabled */
} t_scanEnable;                         /* Scan Enable type */



typedef struct t_pageParams
{  
    t_bd_addr       *p_bd_addr;         /* pointer to a bluetooth device address */
    u_int16          packet_types;      /* Requested packet types */
    t_pageScanRepit  srMode ;           /* page scan repetition mode */
    t_pageScanMode   pageScanMode;      /* page scan mode */
    u_int8           allow_role_switch; /* True if role switch allowed */
    t_clock          clockOffset;       /* clock offset */
} t_pageParams;                         /* collection of paging parameters */


typedef struct t_inquiryResult
{
    t_bd_addr bd_addr;           /* bluetooth device address */
    t_pageScanRepit srMode;      /* page scan repetition mode */
    t_pageScanPeriod spMode;     /* page scan period mode */
    t_pageScanMode pageScanMode; /* page scan mode */
    t_classDevice Cod;           /* class of device */
    u_int16 clkOffset;           /* clock offset */
    s_int8 rssi;                 /* RSSI */
#if (PRH_BS_CFG_SYS_LMP_EXTENDED_INQUIRY_RESPONSE_SUPPORTED==1)
    u_int8 extension_length;     /* extended inquiry response */
#endif
} t_inquiryResult;               /* container for an inquiry result */

/*
 * RF Hopping Selection Types [Values defined for LMP test mode commands]
 */
typedef enum
{
    SINGLE_FREQ         = 0,            /* Single Frequency Hopping */
    EUROPE_USA_FREQ     = 1,            /* Europe and the USA Frequency Hopping */
    FRANCE_FREQ         = 3,            /* French Frequency Hopping */
    REDUCED_FREQ        = 5             /* Reduced Frequency Hopping */
} t_rfSelection;        /* Select Type of Freqency Hopping */

/************************************
 *  Information to be stored here 
 *
 *  Version Information
 *  Features Supported
 *  Manufacturer Id
 *  Country Code
 *  HCI Buffer Size
 *
 **********************************/

typedef struct {
  u_int16 HCI_revision;
  u_int16 comp_id;
  u_int16 lmp_subversion;
  u_int8  lmp_version;
  u_int8  HCI_version;
} t_versionInfo;



typedef struct t_bufferSize {
        u_int16  aclDataPacketLength;
        u_int8   scoDataPacketLength;
        u_int16  numAclDataPackets;
        u_int16  numScoDataPackets;
        } t_bufferSize;


typedef struct
{
    u_int8  length;  /* String length */
    u_int8* p_utf_char; /* C String */
} t_string;          /* UTF String */


/* 
 * Security related
 */
#define LINK_KEY_SIZE 16
#define RANDOM_NUM_SIZE 16
#define PIN_CODE_SIZE 16
#define BYTE_BD_ADDR_SIZE 6
#define SRES_SIZE 4
#define ACO_SIZE 12
#define COF_SIZE 12

typedef u_int8 t_encrypt_enable;
typedef u_int8 t_pin_code[PIN_CODE_SIZE]; 
typedef u_int8 t_link_key[LINK_KEY_SIZE];
typedef u_int8 t_rand[RANDOM_NUM_SIZE];
typedef u_int8 t_byte_bd_addr[BYTE_BD_ADDR_SIZE];
typedef u_int8 t_sres[SRES_SIZE];
typedef u_int8 t_aco[ACO_SIZE];
typedef u_int8 t_cof[COF_SIZE];

/* Flush Related 
 */

#define NON_AUTO_FLUSHABLE  0x00
#define AUTO_FLUSHABLE      0x01
#define NON_AUTO_FLUSHABLE_MARK 0xFF

/*
 * Park Related
 */ 
typedef t_deviceIndex t_parkDeviceIndex; /* index to a parked device */

typedef enum
{
    UNPARK_NONE,
    UNPARK_SLAVE_INIT,
    UNPARK_MASTER_INIT
} t_unpark_type;

typedef u_int8 t_ar_addr;

typedef u_int8 t_pm_addr;

typedef struct t_parkModeInfo
{
    t_slots d_access,
            t_access,
            m_access,
            n_b,
            delta_b,
            n_acc_slot, 
            n_poll;

    t_ar_addr ar_addr;
} t_parkModeInfo;


typedef void (*t_SYS_Fn_Ptr)(void);

/*
 * Some useful macros associated with types
 */
#define mNum_Elements(array)             ( sizeof(array)/sizeof(array[0]) )


#define SYNC_DATA_GOOD           0x00 // No HEC or CRC Error
#define SYNC_DATA_WITH_ERRORS    0x01 // CRC Error
#define SYNC_DATA_LOST           0x02 // HEC Error
#define SYNC_DATA_ERR2GOOD       0x04 //=====
#define SYNC_DATA_LOST2GOOD      0x08 //=====

#define SYNC_DATA_PARTIALLY_LOST 0x03  // Not usable in our implementation as we dont aggregate 

#if (PRH_BS_CFG_SYS_ENHANCED_POWER_CONTROL_SUPPORTED==1)
typedef enum
{
    DECR_POWER_ONE_STEP=0, 
	INCR_POWER_ONE_STEP=1, 
	INCR_MAX_POWER=2
} t_power_adjustment_req;

typedef enum
{
    NOT_SUPPORTED=0, 
	CHANGED_ONE_STEP, 
	AT_MAX_POWER,
	AT_MIN_POWER
} t_power_adjustment_status;

typedef struct t_powerAdjustmendResp
{
    t_power_adjustment_status Mbits_1, Mbits_2, Mbits_3;
} t_powerAdjustmentResp;

typedef enum
{
    CURRENT_TRANSMIT_POWER_LEVEL=0, 
	MAX_TRANSMIT_POWER_LEVEL
} t_requested_transmit_power_level;

typedef struct t_transmitPowerLevel
{
    s_int8 Mbits_1, Mbits_2, Mbits_3; 
} t_transmitPowerLevel;

#endif

#define PKA_IRQ     0x01
#define PKD_IRQ     0x02
#define NO_PKD_IRQ  0x03

#if (AGC_MODE_CHANNEL_ASSESSMENT == 1)

#define AGC_MODE_SOLT_NUMS     16

#define FLT_RSSI_THD_LOW        -74
#define FLT_RSSI_THD_HIGH       -68

#define FLT_RSSI_THD_LOW2        -80
#define FLT_RSSI_THD_HIGH2       -62

typedef struct t_agc_mode_param
{
    u_int8 chnlwptr;
    u_int8 avg_chnl_cls;
    s_int8 avg_chnl_rssi;
    u_int8 reserved;
    #if (AGC_MODE_HEC_ERR_CFG == 1)
    u_int8 hec_err[AGC_MODE_SOLT_NUMS];
    u_int8 crc_err[AGC_MODE_SOLT_NUMS];
    #endif
    s_int8 chnl_rssi[AGC_MODE_SOLT_NUMS];
    u_int8 chnl_cls[AGC_MODE_SOLT_NUMS];
}t_agc_mode_param; 

enum
{
    CHNL_ASSESSMENT_GOOD = 0,
    CHNL_ASSESSMENT_FAIR,
    CHNL_ASSESSMENT_POOR,
    CHNL_ASSESSMENT_BAD,
    CHNL_ASSESSMENT_END
};

#endif

#endif
