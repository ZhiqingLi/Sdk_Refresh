/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CMD_DEF_H_
#define _CMD_DEF_H_
#include "ssv_common.h"

/************************************************************************************************************************************************/
/*                                                                Extra Info                                                                    */
/************************************************************************************************************************************************/

enum ssv6xxx_tx_extra_type
{
    SSV6XXX_TX_FRAME_TIME,
    SSV6XXX_TX_DO_RTS_CTS,
    SSV6XXX_TX_ACK_POLICY,
    SSV6XXX_TX_TX_BRUST,
    SSV6XXX_TX_TX_REPORT,
    SSV6XXX_TX_RTS_CTS_NAV,
    SSV6XXX_TX_CRATE_IDX,
    SSV6XXX_TX_DRATE_IDX,
    SSV6XXX_TX_DL_LENGTH_IDX,

    SSV6XXX_SET_INIT_BEACON,
    SSV6XXX_SET_BEACON,
};


#define SSV_EXTRA_TOTAL_LEN_SIZE sizeof(u16)

struct cfg_bcn_info {
    u16 bcn_len;
    u16 tim_cnt_oft;		//include mac header
    //u8  hdr_len;
};


//SSV6XXX_SET_BEACON
struct cfg_bcn_param {
    u8 			 dtim_cnt;	//DTIM=4-->dtim_cnt=3;
    u16          bcn_itv;  	//beacon interval
};


//SSV6XXX_SET_INIT_BEACON
struct cfg_set_init_bcn {
    struct cfg_bcn_param  param;
    struct cfg_bcn_info   bcn_info;
};


/************************************************************************************************************************************************/
/*                                                                Host Command                                                                    */
/************************************************************************************************************************************************/


/**
 *  struct cfg_host_cmd - Host Command Header Format description
 *
 */
typedef struct cfg_host_cmd {
    u32 len:16;
    u32 c_type:3;
    u32 RSVD0:5;//It will be used as command index eg.  STA-WSID[0]-->RSVD0=0, STA-WSID[1]-->RSVD0=1
    u32 h_cmd:8;//------------------------->ssv_host_cmd/command id
    u32 cmd_seq_no;
    union { /*lint -save -e157 */
        u32 dummy; // Put a u32 dummy to make MSVC and GCC treat HDR_HostCmd as the same size.
#ifdef NOT_SUPPORT_ZERO_ARRAY
        u8  dat8[4];
        u16 dat16[2];
        u32 dat32[1];
#else
        u8  dat8[0];
        u16 dat16[0];
        u32 dat32[0];
#endif
    }un; /*lint -restore */
} HDR_HostCmd;
// Use 100 instead of 0 to get header size to avoid lint from reporting null pointer access.
#define HOST_CMD_HDR_LEN        ((size_t)(((HDR_HostCmd *)100)->un.dat8)-100U)

typedef struct cmd_seq_ctrl_st{
	u32 cmd_seq_no;
}CMD_SEQ_CTRL;


//Security entry
struct security_group_entry
{
    u8  index;
    u8  key[32];
    u64 tx_pn;
    u64 rx_pn;
};

struct security_pair_entry
{
    u8  index;
    u8  pair_key_idx;     //0-3
    u8  group_key_idx;    //0-3
    u8  reserve;
    u8  key[32];
    u64 tx_pn;
    u64 rx_pn;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------
//HOST_CMD_SET_EVT_CB



//-------------------------------------------------------------------------------------------------------------------------------------------------
//HOST_CMD_SET_DATA_CB


//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_WMM_PARAM

struct cfg_set_wmm_param {
    u32 aifsn:4;    //0~15
    u32 acm:1;
    u32 resv:3;
    u32 cwmin:4;//(1, 3, 7, 15, 31, 63, 127, 255, 511, 1023) millionsecond
    u32 cwmax:4;//(1, 3, 7, 15, 31, 63, 127, 255, 511, 1023)
    u32 txop:16;    //65536  milliseconds

    //u32   cw:4;
    u32 backoffvalue:16;
    u32 enable_backoffvalue:1;
    u32 RESV:15;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_TX_PACKET_CNT
//dat->u32



//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_TX_QUEUE_HALT
//dat->u16


//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_INIT_BCN_PARAM



//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_STA


//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_MIB
enum cfg_mib_type
{
    CFG_MIB_RTS
};

struct cfg_set_mib{
    u32 mib_type;
    u32 mib_value;
};


//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_STA
//******************************************************
//				=======IMPORTANT=======
//Info flag is driect mapping to hw reg. DO NOT CHANGE IT.
//******************************************************
//HT_MODE(2)+OP_MODE(2)+QoS(1)+valid(1)
//
#define SET_STA_INFO_VALID(s, v)       (s)->info = (((s)->info & ~(1<<0)) | ((v)<<0))
#define SET_STA_INFO_QOS_EN(s, v)      (s)->info = (((s)->info & ~(1<<1)) | ((v)<<1))
#define SET_STA_INFO_OP_MODE(s, v)     (s)->info = (((s)->info & ~(3<<2)) | ((v)<<2))
#define SET_STA_INFO_HT_MODE(s, v)     (s)->info = (((s)->info & ~(3<<4)) | ((v)<<4))

#define IS_STA_INFO_VALID(s)               !!((s)->info & (1<<0))

//#define GET_STA_INFO_OPER(s)       ((s)->info & (3))//bit 0 1

enum cfg_sta_oper
{
    CFG_STA_DEL,
    CFG_STA_ADD,
    CFG_STA_DEL_ALL,
    CFG_STA_MODIFY,
};


//-------------------------------------
enum cfg_qos_type
{
    CFG_QOS_NONE,
    CFG_QOS_ENABLE,
};


enum cfg_op_mode_type
{
    CFG_OP_MODE_STA,
    CFG_OP_MODE_AP,
    CFG_OP_MODE_AD_HOC,
    CFG_OP_MODE_WDS,
};


enum cfg_ht_type
{
    CFG_HT_NONE,
    CFG_HT_MF,
    CFG_HT_GF,
};


struct cfg_wsid_info {
    //******************************************************
    //              =======IMPORTANT=======
    //Info flag is driect mapping to hw reg. DO NOT CHANGE IT.
    //******************************************************
    //HT_MODE(2)+OP_MODE(2)+QoS(1)+valid(1)
    u32 info;
    struct ETHER_ADDR_st addr;
    s32 wsid_idx;

};
struct cfg_ht_capabilities {
	u16 ht_capabilities_info;
	u8 supported_mcs_set[16];
};

struct cfg_dq_status {
    s32 wsid;
};

struct cfg_dq_lock {
    s32 wsid;
    bool lock;
};

struct cfg_set_sta {
    enum cfg_sta_oper sta_oper;
    struct cfg_wsid_info wsid_info;//info set to reg
    u16 aid;
    u16 non_ht_supp_rates;
    bool short_preamble;
    struct cfg_ht_capabilities ht_capabilities;
    
};


//-------------------------------------------------------------------------------------------------------------------------------------------------
//SSV6XXX_HOST_CMD_SET_AP_CFG
struct cfg_set_ap_cfg {
    struct cfg_80211_ssid ssid;
    ssv6xxx_sec_type sec_type;
    u8 password[MAX_PASSWD_LEN+1];
};
struct ap_calc_pmk_cfg {
    u8 ssid[MAX_SSID_LEN];
    size_t ssid_len;
    u8 password[MAX_PASSWD_LEN+1];
};

struct ap_calc_ptk_cfg {
    u8 pmk[32];
    u8 AAddr[6];
    u8 SAddr[6];
    u8 ANonce[32];
    u8 SNonce[32];
};

enum wpa_host_cipher {
    CIPHER_HOST_NONE,
    CIPHER_HOST_WEP40,
    CIPHER_HOST_TKIP,
    CIPHER_HOST_CCMP,
    CIPHER_HOST_WEP104
};

//Security
struct securityEntry {
    u8 cipher;
    u8 wpaUnicast;
    u8 keyLen;
    u8 keyIndex;
    u8 key[32];
    u8 bssid[6];
};

enum cmdtype_rcinfo {
    RC_RATEMASK,
    RC_RESENT,
    RC_PER,
    RC_PREPRBFRM,
    RC_UPPERFASTESTB,
    DIRECT_RATE_DW,
    FORCE_RTSCTS,
    RC_DRATE_ENDIAN, //The default data rate is from the lowest index of rate masK to high, or highest index
};

typedef enum{
//===========================================================================
    //Public command
    SSV6XXX_HOST_CMD_START              = 0                                                         ,
    SSV6XXX_HOST_CMD_SCAN               = SSV6XXX_HOST_CMD_START                                    ,   //Station
    SSV6XXX_HOST_CMD_JOIN                                                                           ,   //Station
    SSV6XXX_HOST_CMD_LEAVE                                                                          ,   //Station

    SSV6XXX_HOST_CMD_SET_HW_MODE                                                                    ,
    SSV6XXX_HOST_CMD_SET_CONFIG                                                                     ,
    SSV6XXX_HOST_CMD_GET_CONFIG                                                                     ,
    SSV6XXX_HOST_CMD_SET_REG                                                                        ,
    SSV6XXX_HOST_CMD_GET_REG                                                                        ,
    SSV6XXX_HOST_CMD_SET_STA_MAC                                                                    ,
    SSV6XXX_HOST_CMD_SET_BSSID                                                                      ,
    SSV6XXX_HOST_CMD_SET_DECITBL                                                                    ,
    SSV6XXX_HOST_CMD_SET_WSIDTBL                                                                    ,
    SSV6XXX_HOST_CMD_SET_TX_ETHTRAP                                                                 ,
    SSV6XXX_HOST_CMD_SET_RX_ETHTRAP                                                                 ,
    SSV6XXX_HOST_CMD_SET_FCMD_TXDATA                                                                ,
    SSV6XXX_HOST_CMD_SET_FCMD_TXMGMT                                                                ,
    SSV6XXX_HOST_CMD_SET_FCMD_TXCTRL                                                                ,
    SSV6XXX_HOST_CMD_SET_FCMD_RXDATA                                                                ,
    SSV6XXX_HOST_CMD_SET_FCMD_RXMGMT                                                                ,
    SSV6XXX_HOST_CMD_SET_FCMD_RXCTRL                                                                ,
    //SSV6XXX_HOST_CMD_SET_TXF_ID                                                                  ,
    SSV6XXX_HOST_CMD_SET_SIFS                                                                       ,
    SSV6XXX_HOST_CMD_SET_DIFS                                                                       ,
    SSV6XXX_HOST_CMD_SET_EIFS                                                                       ,
    //Security
    SSV6XXX_HOST_CMD_SET_PAIR_SECURITY                                                              ,
    SSV6XXX_HOST_CMD_SET_GROUP_SECURITY                                                             ,
    SSV6XXX_HOST_CMD_SET_PAIR_ENTRY                                                                 ,
    SSV6XXX_HOST_CMD_SET_GROUP_ENTRY																,
    SSV6XXX_HOST_CMD_SET_SECURITY_ENTRY                                                             ,
    SSV6XXX_HOST_CMD_SET_TX_INFO_SECURITY                                                           ,

    SSV6XXX_HOST_CMD_SET_WMM_PARAM                                                                  ,
    SSV6XXX_HOST_CMD_SET_WMM_RANDOM                                                                 ,
    SSV6XXX_HOST_CMD_SET_TX_PACKET_CNT                                                              ,
    SSV6XXX_HOST_CMD_SET_TX_QUEUE_HALT                                                              ,
    SSV6XXX_HOST_CMD_SET_TXOP_SUB_FRM_TIME                                                          ,
    SSV6XXX_HOST_CMD_SET_OPMODE                                                                     ,
    SSV6XXX_HOST_CMD_SET_NAV                                                                        ,
    SSV6XXX_HOST_CMD_SET_RX2HOST                                                                    ,
    SSV6XXX_HOST_CMD_SET_RXSNIFFER																,
    SSV6XXX_HOST_CMD_STRIP_OFF                                                                      ,
    SSV6XXX_HOST_CMD_SET_TRAP_MASK                                                                  ,
    SSV6XXX_HOST_CMD_SET_GLOBAL_SEQCTRL                                                             ,
    SSV6XXX_HOST_CMD_SET_AUTO_SEQNO                                                                 ,
    SSV6XXX_HOST_CMD_SET_RX_NULL_DATA_TRAP															,
    SSV6XXX_HOST_CMD_SET_M2_RX2HOST																,
    SSV6XXX_HOST_CMD_SET_RX_INFO_SIZE															,
    SSV6XXX_HOST_CMD_SET_ERP_PROTECT                                                                ,
    SSV6XXX_HOST_CMD_SET_PBUF_OFFSET                                                                ,
    SSV6XXX_HOST_CMD_SET_DUP_FLT                                                                    ,
    SSV6XXX_HOST_CMD_SET_HT_MODE                                                                    ,
    SSV6XXX_HOST_CMD_SET_PROMIS_MODE                                                                ,
    SSV6XXX_HOST_CMD_SET_QOS_CAP                                                                    ,
    SSV6XXX_HOST_CMD_SET_GMFLT                                                                      ,
    SSV6XXX_HOST_CMD_SET_MGMT_TXQID                                                                 ,
    SSV6XXX_HOST_CMD_SET_NONQOS_TXQID                                                               ,
    SSV6XXX_HOST_CMD_SET_MAC_MODE                                                                   ,

    SSV6XXX_HOST_CMD_SET_STOP_BCN                                                                  ,
    SSV6XXX_HOST_CMD_SET_STA                                                                        ,
    SSV6XXX_HOST_CMD_GET_STA_MAC                                                                    ,
    SSV6XXX_HOST_CMD_GET_BSSID                                                                      ,
    SSV6XXX_HOST_CMD_GET_DECI_TBL                                                                   ,
    SSV6XXX_HOST_CMD_GET_WSID_TBL                                                                   ,
    SSV6XXX_HOST_CMD_GET_ETHER_TRAP                                                                 ,
    SSV6XXX_HOST_CMD_GET_FCMDS                                                                      ,
    //SSV6XXX_HOST_CMD_GET_TX_ETH_TRAP                                                                ,
    //SSV6XXX_HOST_CMD_GET_RX_ETH_TRAP                                                                ,
    //SSV6XXX_HOST_CMD_GET_FCMD_TX_DATA                                                               ,
    //SSV6XXX_HOST_CMD_GET_FCMD_TX_MGMT                                                               ,
    //SSV6XXX_HOST_CMD_GET_FCMD_TX_CTRL                                                               ,
    //SSV6XXX_HOST_CMD_GET_FCMD_RX_DATA                                                               ,
    //SSV6XXX_HOST_CMD_GET_FCMD_RX_MGMT                                                               ,
    //SSV6XXX_HOST_CMD_GET_FCMD_RX_CTRL                                                               ,

    SSV6XXX_HOST_CMD_SET_MIB																		,
    //SSV6XXX_HOST_CMD_INIT_PHY_TABLE																,
    SSV6XXX_HOST_CMD_SET_PHY_INFO_TBL                                                               ,
    SSV6XXX_HOST_CMD_GET_PHY_INFO_TBL                                                               ,

    SSV6XXX_HOST_CMD_SET_RX_CHECK	                                                                ,
    SSV6XXX_HOST_CMD_SHOW_ALL																		,

 /*
    SSV6XXX_HOST_CMD_GET_SIFS                                                                       ,
    SSV6XXX_HOST_CMD_GET_DIFS                                                                       ,
    SSV6XXX_HOST_CMD_GET_EIFS                                                                       ,
    */
    SSV6XXX_HOST_CMD_ADDBA_RESP											,
    SSV6XXX_HOST_CMD_DELBA                                               ,

    SSV6XXX_HOST_CMD_LOG                                                                            ,
    SSV6XXX_HOST_CMD_PHY_ON                                                                         ,
    SSV6XXX_HOST_CMD_CAL											   								,
    SSV6XXX_HOST_CMD_SET_BCN_TIMER_EN                                                               ,
    SSV6XXX_HOST_CMD_GET_CHANNEL                                                                    ,

#if defined(BEACON_DBG) && (BEACON_DBG == 1)
    SSV6XXX_HOST_CMD_BEACON_RELEASE_TEST															,

#endif
    SSV6XXX_HOST_CMD_SET_MULTI_MAC_MODE															,
    SSV6XXX_HOST_CMD_SET_RX_TODS_MASK															,
    SSV6XXX_HOST_CMD_GET_SOC_STATUS                                                                 ,
    SSV6XXX_HOST_CMD_BUS_THROUGHPUT_TEST                                                            ,
    SSV6XXX_HOST_CMD_INIT_CALI                                                                      ,
    SSV6XXX_HOST_CMD_IPD                                                                        ,
    SSV6XXX_HOST_CMD_SMART_CONFIG                                                                   ,
    SSV6XXX_HOST_CMD_DQ_STATUS                                                                      ,
    SSV6XXX_HOST_CMD_DQ_LOCK                                                                        ,
    SSV6XXX_HOST_CMD_BUS_LOOPBACK_TEST                                                              ,
    SSV6XXX_HOST_CMD_SET_PWR_SAVING                                                                 ,
	SSV6XXX_HOST_CMD_GET_PMK                                                                        ,
    SSV6XXX_HOST_CMD_CALC_PTK                                                   ,      
    SSV6XXX_HOST_CMD_RECOVER                                                   ,  
    SSV6XXX_HOST_CMD_SELF_BCN_ENABLE                                                                ,    
	SSV6XXX_HOST_CMD_SET_HT_PROTECT                                                                 , 
    SSV6XXX_HOST_CMD_SET_AMPDU_PARAM,   
    SSV6XXX_HOST_CMD_SET_RC_VALUES,   
	SSV6XXX_HOST_SOC_CMD_MAXID                                                                      ,

//===========================================================================

    SSV6XXX_HOST_CMD_SET_EVT_CB     = SSV6XXX_HOST_SOC_CMD_MAXID,
    SSV6XXX_HOST_CMD_SET_DATA_CB                                                                    ,
    SSV6XXX_HOST_CMD_UNSET_EVT_CB                                                                   ,
    SSV6XXX_HOST_CMD_UNSET_DATA_CB                                                                  ,

    SSV6XXX_HOST_CMD_SET_BSS_PARAM                                                                  ,
    SSV6XXX_HOST_CMD_SET_ADDR4                                                                      ,
    SSV6XXX_HOST_CMD_SET_HT                                                                         ,
    SSV6XXX_HOST_CMD_SET_SECURITY                                                                   ,
    SSV6XXX_HOST_CMD_SET_QOS                                                                        ,
    SSV6XXX_HOST_CMD_SET_AP_CFG																		,
    SSV6XXX_HOST_CMD_UPDATE_BEACON                                                                  ,
    SSV6XXX_HOST_CMD_SET_STA_CFG                                                                    ,

    SSV6XXX_HOST_CMD_MAXID                                                                          ,
}ssv6xxx_host_cmd_id;

//-------------------------------------------------------------------------------------------------------------------------------------------


/************************************************************************************************************************************************/
/*                                                                Host Event                                                                        */
/************************************************************************************************************************************************/


/**
 *  struct cfg_host_event - Host Event Header Format description
 *
 */
typedef struct cfg_host_event {
    u32 len:16;
    u32 c_type:3;
    u32 RSVD0:5;
    u32 h_event:8;//------------------>ssv_host_evt
    u32 evt_seq_no;
    u8  dat[0];
} HDR_HostEvent;

//=========================================
//                Public Event Data
//=========================================

#if 0
struct resp_evt_result {
    union{
        bool is_sucessful;  //Scan, SetHwMode
        s32  status_code;   //Join
    }u;
    s32 aid;
} ;//__attribute__ ((packed));
#endif

#define  CMD_SUCESS 0
#define CMD_PASS_FRAME 1

struct resp_evt_result {
#ifdef USE_CMD_RESP
    u8  cmd;
    u8  result;
    u32 cmd_seq_no;
#endif // USE_CMD_RESP
    union {
        PACK( struct {
            u8                              result_code;//SUCCESS,PASS_FRAME
            struct ssv6xxx_ieee80211_bss    bss_info;
            u16                             dat_size;
            u8                              dat[0];//for saving probe resp
        } scan)
        struct {
   	 u32                        channel_edcca_count[MAX_CHANNEL_NUM];
         u32                        channel_packet_count[MAX_CHANNEL_NUM];
   		 u8                   		result_code;//0: Success, 1:fail
        } scan_done;
        struct {
        u8                          channel;
        u8                   	    ssid[MAX_SSID_LEN];//ssid[] didnot include '\0' at end of matrix
        u8                          ssid_len;
        u8                   	    pwd[MAX_PASSWD_LEN+1];
        u8                          rand;
        u8                   	    result_code;//0: Success, 1:fail
        } sconfig_done;
        struct {
            s32 status_code;
            s32 aid;
        } join;
        struct {
            s16 reason_code;
        } leave;
        struct {
         s16            wsid;
         s16		    len;
        } dq_status;
        struct {
            u8 policy;
            u8 dialog_token;
            u16 tid;
            u16 agg_size;
            u16 timeout;
            u16 start_seq_num;
        } addba_req;
        struct {
            u16 initiator;
            u16 tid;
            u16 reason_code;
        } delba_req;
#ifdef NOT_SUPPORT_ZERO_ARRAY
        u8 dat[1];
#else
        u8 dat[0];
#endif
        u8 pmk[32];
        u8 wsid;
    } u;
};

enum ap_act
{
	AP_ACT_REMOVE_AP,
	AP_ACT_ADD_AP,
	AP_ACT_MODIFY_AP,
};

typedef struct {
    u16 index;
	enum ap_act act;
    struct ssv6xxx_ieee80211_bss *apInfo;
}ap_info_state;

#define RESP_EVT_HEADER_SIZE        ((size_t)(&((struct resp_evt_result *)100)->u.dat[0]) - 100U)
#define CMD_RESPONSE_BASE_SIZE      (sizeof(HDR_HostEvent) + RESP_EVT_HEADER_SIZE)
#define SCAN_RESP_EVT_HEADER_SIZE   ((size_t)(&((struct resp_evt_result *)100)->u.scan.dat[0]) - 100U)
#define SCAN_DONE_EVT_HEADER_sIZE   (1+(MAX_CHANNEL_NUM)*sizeof(u32)+(MAX_CHANNEL_NUM)*sizeof(u32))
#define LEAVE_RESP_EVT_HEADER_SIZE   sizeof(s16) // leave result only has a s16 member

//=========================================
//                Private Event Data
//=========================================

//-------------------------------------------------------------------------------------------------------------------------------------------------
//HOST_EVENT_PS_POLL
struct cfg_ps_poll_info {
    //This field must be in the beginning.
    u16 status;     // 0: q unlock, 1, q lock
    s32 q_len;      // <0 need data framw, >=0 no need data
    ETHER_ADDR      SAAddr;
} ;

//-------------------------------------------------------------------------------------------------------------------------------------------------
//HOST_EVENT_NULL_DATA
//
//Bit info stores in Flags of cfg_null_data_info
#define HOST_EVT_NULL_DATA_PM   BIT(0)
#define HOST_EVT_NULL_DATA_QOS  BIT(1)

struct cfg_null_data_info {
    //This field must be in the beginning.
    ETHER_ADDR    SAAddr;

    u32            Flags:28;
    u32            Priority:4;
    s32 q_len;      // <0 need data framw, >=0 no need data
} ;

//HOST_EVENT_TXLOOPBACK
struct cfg_tx_loopback_info {
    u32 reg;            // PMU3 reg
    bool result;        // loopback result
} ;

struct cfg_addba_info {
    u32 wsid;
    u32 tid;
    u16 ssn;
    u16 buf_size;
} ;

typedef enum _CmdResult_E {
    CMD_OK,             // Command executed successfully. Check corresponding returned values for status.
    CMD_INVALID,        // Invalid command.
    CMD_STATE_ERROR,    // Not executable in current firmware state.
    CMD_TIMEOUT,        // Peer does not response in time. Command expired.
    CMD_BUSY,           // Too busy to accept new command.
    CMD_NOMEM,          // Out of resource to execute this command
} CmdResult_E;

typedef enum{
//===========================================================================
    //Public event
#ifdef USE_CMD_RESP
    SOC_EVT_CMD_RESP                        , // Response of a host command.
    SOC_EVT_SCAN_RESULT                     , // Scan result from probe response or beacon
    SOC_EVT_DEAUTH                          , // Deauthentication received but not for leave command
#else
    SOC_EVT_HW_MODE_RESP                    ,
    SOC_EVT_SCAN_RESULT                     ,
    SOC_EVT_JOIN_RESULT                     ,
    SOC_EVT_LEAVE_RESULT                    ,
    SOC_EVT_GET_REG_RESP                    ,
    SOC_EVT_GET_STA_MAC_RESP                ,
    SOC_EVT_GET_BSSID_RESP                  ,
    SOC_EVT_GET_DECI_TABLE_RESP             ,
    SOC_EVT_GET_WSID_TABLE_RESP             ,
    //SOC_EVT_GET_TX_ETH_TRAP_RESP                  ,
    //SOC_EVT_GET_RX_ETH_TRAP_RESP                  ,
    SOC_EVT_GET_ETHER_TRAP_RESP             ,
    //SOC_EVT_GET_FCMD_TX_DATA_RESP             ,
    //SOC_EVT_GET_FCMD_TX_MGMT_RESP             ,
    //SOC_EVT_GET_FCMD_TX_CTRL_RESP                 ,
    //SOC_EVT_GET_FCMD_RX_DATA_RESP                 ,
    //SOC_EVT_GET_FCMD_RX_MGMT_RESP                 ,
    //SOC_EVT_GET_FCMD_RX_CTRL_RESP                     ,
    SOC_EVT_GET_FCMDS_RESP                  ,
    SOC_EVT_GET_PHY_INFO_TBL_RESP           ,
    SOC_EVT_GET_SIFS_RESP                   ,
    SOC_EVT_GET_DIFS_RESP                   ,
    SOC_EVT_GET_EIFS_RESP                   ,
#endif // USE_CMD_RESP
    SOC_EVT_LOG                             ,			// ssv log module soc event
    SOC_EVT_ACK                             ,
    //SOC_EVT_SDIO_TEST_RX_RESULT,
    SOC_EVT_TX_ALL_DONE                     ,
    SOC_EVT_RCV_ADDBA_REQ                   ,
    SOC_EVT_RCV_DELBA                       ,
    SOC_EVT_MLME_CMD_DONE                   ,
    SOC_EVT_GET_SOC_STATUS                  ,
    SOC_EVT_BUS_THROUTHPUT_TEST             ,
    SOC_EVT_POLL_STATION                     ,
    SOC_EVT_SCAN_DONE                     ,
    SOC_EVT_SCONFIG_SCAN_DONE                   ,
    SOC_EVT_DATA_QUEUE_STATUS              ,
    SOC_EVT_BUS_LOOPBACK                    ,
    SOC_EVT_PS_SETUP_OK                     ,
    SOC_EVT_PS_WAKENED                      ,
    SOC_EVT_GET_PMK                         ,
    SOC_EVT_ADD_STA_DONE                    ,
    SOC_EVT_STA_STATUS                      ,
    SOC_EVT_TX_LOOPBACK_DONE                ,
    SOC_EVT_USER_END                        ,

//===========================================================================
    //Private    event
    SOC_EVT_PRIVE_CMD_START                 = SOC_EVT_USER_END,
    SOC_EVT_CONFIG_HW_RESP                  = SOC_EVT_PRIVE_CMD_START,
    SOC_EVT_SET_BSS_PARAM_RESP              ,
    SOC_EVT_PS_POLL                         ,
    SOC_EVT_NULL_DATA                       ,
    SOC_EVT_REG_RESULT                      ,
    SOC_EVT_ADD_BA                          ,

    //SOC_EVT_DTIM_EXPIRED                    ,
    //    SOC_EVT_GET_REG_RESP                          ,

    SOC_EVT_MAXID                           ,
} ssv6xxx_soc_event;

struct MsgEvent_st;
//typedef struct MsgEvent_st MsgEvent;

struct MsgEvent_st *HostEventAlloc(ssv6xxx_soc_event hEvtID, u32 size);

struct MsgEvent_st *HostCmdRespEventAlloc(const struct cfg_host_cmd *host_cmd, CmdResult_E cmd_result, u32 resp_size, void **p_resp_data);

/**
 * Define Macros for host event manipulation:
 *
 * @ HOST_EVENT_ALLOC():    Allocate a host event structure from the system.
 * @ HOST_EVENT_SET_LEN(): Set the host event length. The length excludes
 *                                             the event header length.
 * @ HOST_EVENT_SEND():      Send the event to the host.
 */
#define HOST_EVENT_SET_LEN(ev, l)                       \
{                                                       \
    ((HDR_HostEvent *)((ev)->MsgData))->len =          \
    (l) + sizeof(HDR_HostEvent);                        \
}
#define HOST_EVENT_ALLOC_RET(ev, evid, l, ret)          \
{                                                       \
    (ev) = HostEventAlloc(evid, l);                     \
    ASSERT_RET(ev, ret);                                \
}
#define HOST_EVENT_ALLOC(ev, evid, l)                   \
{                                                       \
    (ev) = HostEventAlloc(evid, l);                     \
    ASSERT_RET(ev, EMPTY);                              \
}
#define HOST_EVENT_DATA_PTR(ev)                         \
    ((HDR_HostEvent *)((ev)->MsgData))->dat
#define HOST_EVENT_ASSIGN_EVT_NO(ev,no)			\
{														\
    (((HDR_HostEvent *)((ev)->MsgData))->evt_seq_no)=no;	\
}

#define HOST_EVENT_ASSIGN_EVT(ev, evt_id)                   \
    do {                                                   \
        ((HDR_HostEvent *)((ev)->MsgData))->h_event = evt_id;\
    } while (0)

#define CMD_RESP_ALLOC(evt_msg, host_cmd, cmd_result, resp_size, p_resp_data) \
    do { \
        evt_msg = host_cmd_resp_alloc(host_cmd, cmd_result, resp_size, p_resp_data); \
        if (evt_msg == NULL) \
            return; \
    } while (0)

#define CMD_RESP_ALLOC_RET(evt_msg, host_cmd, cmd_result, resp_size, p_resp_data, fail_ret) \
    do { \
        evt_msg = host_cmd_resp_alloc(host_cmd, cmd_result, resp_size, p_resp_data); \
        if (evt_msg == NULL) \
            return fail_ret; \
    } while (0)

#endif//_CMD_DEF_H_

