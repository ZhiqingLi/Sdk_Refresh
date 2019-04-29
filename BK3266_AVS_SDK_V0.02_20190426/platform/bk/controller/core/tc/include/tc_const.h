#ifndef _PARTHUS_TC_CONST_
#define _PARTHUS_TC_CONST_

/**********************************************************************
 *
 * MODULE NAME:    tc_const.h
 * PROJECT:       BlueStream
 * DESCRIPTION:   Constants used by the TCI (Command Opcodes etc.)
 * MAINTAINER:     Daire McNamara <Daire McNamara@sslinc.com>
 * CREATION DATE:  13 April 2000
 *
 * SOURCE CONTROL: $Id: tc_const.h,v 1.66 2014/05/01 13:52:45 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *
 **********************************************************************/

/*
 * TCI SYSTEM TEST OP CODES - OPCODE GROUP SUBFIELD = 0x3F
 */
#define ST_G ( 0x3F << 10)

#define TCI_ACTIVATE_REMOTE_DUT                         0x0002 + ST_G
#define TCI_TEST_CONTROL                                0x0003 + ST_G
#define TCI_INCREASE_REMOTE_POWER                       0x0004 + ST_G
#define TCI_WRITE_LOCAL_HOP_FREQUENCIES                 0x0005 + ST_G
#define TCI_READ_LOCAL_HARDWARE_VERSION                 0x0006 + ST_G
#define TCI_DECREASE_REMOTE_POWER                       0x0007 + ST_G
#define TCI_INCREASE_LOCAL_VOLUME                       0x0008 + ST_G
#define TCI_DECREASE_LOCAL_VOLUME                       0x0009 + ST_G
#define TCI_WRITE_LOCAL_NATIVE_CLOCK                    0x000A + ST_G
#define TCI_READ_LOCAL_NATIVE_CLOCK                     0x000B + ST_G
#define TCI_READ_LOCAL_HOST_CONTROLLER_RELATIVE_MIPS    0x000C + ST_G
#define TCI_TYPE_APPROVAL_TEST_CONTROL                  0x000D + ST_G
#define TCI_SET_LOCAL_FAILED_ATTEMPTS_COUNTER           0x000E + ST_G
#define TCI_CLEAR_LOCAL_FAILED_ATTEMPTS_COUNTER         0x000F + ST_G
#define TCI_READ_LOCAL_DEFAULT_PACKET_TYPE              0x0010 + ST_G
#define TCI_WRITE_LOCAL_DEFAULT_PACKET_TYPE             0x0011 + ST_G
#define TCI_WRITE_LOCAL_SYNCWORD                        0x0012 + ST_G
#define TCI_WRITE_LOCAL_HOPPING_MODE                    0x0013 + ST_G
#define TCI_READ_LOCAL_HOPPING_MODE                     0x0014 + ST_G
#define TCI_WRITE_LOCAL_WHITENING_ENABLE                0x0015 + ST_G
#define TCI_READ_LOCAL_WHITENING_ENABLE                 0x0016 + ST_G
#define TCI_WRITE_LOCAL_RADIO_POWER                     0x0017 + ST_G
#define TCI_READ_LOCAL_RADIO_POWER                      0x0018 + ST_G
#define TCI_SET_LOCAL_NEXT_AVAILABLE_AM_ADDR            0x0019 + ST_G
#define TCI_SET_LOCAL_BD_ADDR                           0x001A + ST_G
#define TCI_WRITE_LOCAL_LINK_KEY_TYPE                   0x001B + ST_G
#define TCI_READ_LOCAL_LINK_KEY_TYPE                    0x001C + ST_G
#define TCI_READ_LOCAL_EXTENDED_FEATURES                0x001D + ST_G
#define TCI_WRITE_LOCAL_FEATURES                        0x001E + ST_G
#define TCI_WRITE_LOCAL_EXTENDED_FEATURES               0x001F + ST_G
#define TCI_READ_LOCAL_TIMING_INFORMATION               0x002A + ST_G
#define TCI_WRITE_LOCAL_TIMING_INFORMATION              0x002B + ST_G
#define TCI_READ_REMOTE_TIMING_INFORMATION              0x002C + ST_G
#define TCI_WRITE_LOCAL_HARDWARE_REGISTER               0x002D + ST_G
#define TCI_RESET_LOCAL_BASEBAND_MONITORS               0x002E + ST_G

#define TCI_WRITE_LOCAL_RADIO_REGISTER                  0x0030 + ST_G
#define TCI_READ_LOCAL_RADIO_REGISTER                   0x0031 + ST_G
#define TCI_CHANGE_RADIO_MODULATION                     0x0032 + ST_G
#define TCI_READ_RADIO_MODULATION                       0x0033 + ST_G
#define TCI_SET_HCIT_UART_BAUD_RATE                     0x0034 + ST_G

#define TCI_GENERATE_LOCAL_AVRCP_L2CAP_CONTROL          0x0035 + ST_G

#define TCI_SEND_ENCRYPTION_KEY_SIZE_MASK_REQ           0x0039 + ST_G
#define TCI_RESET_LOCAL_PUMP_MONITORS                   0x003A + ST_G
#define TCI_READ_LOCAL_PUMP_MONITORS                    0x003B + ST_G
#define TCI_WRITE_LOCAL_ENCRYPTION_KEY_LENGTH           0x003C + ST_G
#define TCI_READ_LOCAL_ENCRYPTION_KEY_LENGTH            0x003D + ST_G
#define TCI_READ_LOCAL_HOP_FREQUENCIES                  0x003E + ST_G
#define TCI_READ_LOCAL_BASEBAND_MONITORS                0x003F + ST_G
#define TCI_SET_DISABLE_LOW_POWER_MODE                  0x0040 + ST_G
#define TCI_SET_ENABLE_LOW_POWER_MODE                   0x0041 + ST_G
#define TCI_READ_R2P_MIN_SEARCH_WINDOW                  0x0042 + ST_G
#define TCI_WRITE_R2P_MIN_SEARCH_WINDOW                 0x0043 + ST_G
#define TCI_SET_DISABLE_SCO_VIA_HCI                     0x0044 + ST_G
#define TCI_SET_ENABLE_SCO_VIA_HCI                      0x0045 + ST_G

#define TCI_WRITE_ESCO_RETRANSMISSION_MODE              0x0046 + ST_G
#define TCI_READ_ESCO_RETRANSMISSION_MODE               0x0047 + ST_G

#define TCI_WRITE_EPC_ENABLE                            0x0048 + ST_G
#define TCI_WRITE_PTA_ENABLE							0x0049 + ST_G

#define TCI_VCI_CLK_OVERRIDE                            0x004E + ST_G
#define TCI_SET_BROADCAST_SCAN_WINDOW                   0x004F + ST_G
#define TCI_WRITE_PARK_PARAMETERS                       0x0050 + ST_G
#define TCI_READ_UNUSED_STACK_SPACE                     0x0051 + ST_G

#define TCI_WRITE_AFH_CONTROL                           0x0060 + ST_G

#define TCI_READ_RAW_RSSI                               0x0061 + ST_G
#define TCI_READ_BER                                    0x0062 + ST_G
#define TCI_READ_PER                                    0x0063 + ST_G
#define TCI_READ_RAW_RSSI_PER_BER                       0x0064 + ST_G

#define TCI_WRITE_SECURITY_TIMEOUTS                     0x0066 + ST_G


#if(PRH_BS_CFG_SYS_BROADCAST_NULL_IN_INQ_PAGE_SUPPORTED==1)
#define TCI_SET_EMERGENCY_POLL_INTERVAL					0x0069 + ST_G
#endif

#define TCI_WRITE_FEATURES                              0x0067 + ST_G

#define TCI_LE_SET_TRANSMIT_WINDOW_PARAMS               0x006A + ST_G
#define TCI_LE_SET_DIRECT_ADVERTISING_TIMEOUT           0x006B + ST_G
#define TCI_LE_SET_TIFS_TX_ADJUSTMENT                   0x006C + ST_G
#define TCI_LE_SET_SEARCH_WINDOW_DELAY                  0x006D + ST_G

#define TCI_LE_AUTO_ADVERTISE_AFTER_SLAVE_DISCONNECT   0x70 + ST_G
#define TCI_LE_AUTO_INITIATE_AFTER_MASTER_DISCONNECT   0x71 + ST_G
#define TCI_LE_READ_ADV_PARAMS                         0x72 + ST_G
#define TCI_LE_WRITE_ADVERTISING_DELTA                 0x73 + ST_G
#define TCI_LE_WRITE_NUM_PACKETS_PER_CE                0x74 + ST_G
#define TCI_LE_ECHO_TRANSMIT_WINDOW_SIZE_AND_OFFSET    0x75 + ST_G
#define TCI_LE_GET_DEVICE_STATES                       0x76 + ST_G
#define TCI_LE_READ_SCAN_BACKOFF_INFO                  0x77 + ST_G
#define TCI_LE_WRITE_SCAN_FREQUENCIES                  0x78 + ST_G
#define TCI_LE_READ_SCAN_FREQUENCIES                   0x79 + ST_G
#define TCI_LE_WRITE_INITIATING_FREQUENCIES            0x7A + ST_G
#define TCI_LE_READ_INITIATING_FREQUENCIES             0x7B + ST_G
#define TCI_LE_NUM_TIMES_MASTER_DOESNT_TX_FIRST_WIN    0x7C + ST_G
#define TCI_LE_SLAVE_LISTEN_OUTSIDE_LATENCY            0x7D + ST_G
#define TCI_LE_READ_PEER_SCA                           0x7E + ST_G
#define TCI_LE_READ_SESSION_KEY                        0x7F + ST_G
#define TCI_LE_READ_HOP_INCREMENT                      0x80 + ST_G
#define TCI_LE_READ_ACCESS_CODE                        0x81 + ST_G
#define TCI_LE_WHITENING_ENABLE                        0x82 + ST_G
#define TCI_LE_ENABLE_SCAN_BACKOFF                     0x83 + ST_G
#define TCI_LE_PRECONFIGURE_HOP_INC					   0x84 + ST_G
#define TCI_LE_PRECONFIGURE_ACCESS_CODE				   0x85 + ST_G
#define TCI_LE_DIRECT_ADV_TIMEOUT                      0x86 + ST_G
#define TCI_LE_READ_SCAN_PARAMS                        0x87 + ST_G
#define TCI_LE_SET_TRACE_LEVEL                         0x88 + ST_G
#define TCI_LE_READ_LOCAL_POWER_LEVEL                  0x89 + ST_G
#define TCI_LE_INC_LOCAL_POWER_LEVEL                   0x8A + ST_G
#define TCI_LE_DEC_LOCAL_POWER_LEVEL                   0x8B + ST_G
/*
 * Pure Debug TCIs - only usefull to assist stress testing the system
 */
//#define TCI_LE_AUTO_REENABLE_ADVERTISING_IN_SLAVE_AFTER_DISCONNECT
//#define TCI_LE_AUTO_RESTART_INITIATING_IN_MASTER_AFTER_DISCONNECT
#define TCI_BEKEN_HARDWARE_TEST                        0x00e0 + ST_G
/* WARNING: uart download cmd {00 03 01 0e 0f 10 aa f4} */
enum
{
    BEKEN_UART_LINK_CHECK                       =0x00,  /*return 0x04 0x0e 0x04 0x01 0xe0 0xfc 0x00*/
    BEKEN_UART_REGISTER_WRITE_CMD               =0x01,  /*Write certain reg*/
    BEKEN_UART_REGISTER_CONTINUOUS_WRITE_CMD    =0x02,  /*write reg continuous*/
    BEKEN_UART_REGISTER_READ_CMD                =0x03,  /*read certain reg value*/
    
    BEKEN_UART_BT_START_CMD                     =0x04,  /*useless*/
    BEKEN_UART_BT_STOP_CMD                      =0x05,  /*useless*/
    BEKEN_UART_PATCH_CMD                        =0x06,  /*useless*/
    
    BEKEN_UART_SET_UART_PROTOCOL                =0x07,  /*set uart protocol, H4:00  BCSP:01*/
    
    /*
    01 e0 fc 09 08 aa aa aa aa bb cc dd ee
    aa: baudrate  
    bb: byte size 5,6,7,8
    cc: stop len  1 or 2
    dd: par_en    00:no par 01:par
    ee: par       00:odd 01:even
    */
    BEKEN_UART_SET_UART_CONFIG                  =0x08,  /*set uart config*/
    BEKEN_ENABLE_AFC                            =0x09,  /*enable afc*/
    
    /*
    01 e0 fc 06 0A aa bb cc dd ee
    aa: data_len        0:16bit, N(8,13,14,15): N bit
    bb: speed;          0:200K,1: 1M,2:64K,3:external,4:256K,5:512K,6:128K
    cc: is_msb          0:msb,1:lsb
    dd: role;           0:slave 1:master
    ee: sync_type;      0:short_sync, N(1-7): long_sync with length N
    */
    BEKEN_CONFIG_PCM                            =0x0A,  /*config PCM*/

    //0:normal mode; 1:only stop CPU when acl number = 0; 2:not stop CPU
    BEKEN_CPU_HALT_MODE                         =0x0B,  /*set CPU sleep mode*/
    BEKEN_ENABLE_32K_SLEEP                      =0x0C,  /*enable cpu enter 32K sleep*/
    BEKEN_ENABLE_ANALOG_POWERDOWN               =0x0D,  /*enable close CEVA clock*/
    #if 0 // for UART DOWNLOAD
    BEKEN_ENABLE_GPIO_EINT_WAKEUP               =0x0E,  /*enable GPIO wake up CPU*/
    BEKEN_ENABLE_UART_RX_WAKEUP                 =0x0F,  /*enable uart RX wake up BT chip*/
    BEKEN_SET_UART_RX_WAKEUP_COUNT              =0x10,  /*set UART RX wake up count*/
    #endif
    BEKEN_ENABLE_UART_TX_WAKEUP                 =0x11,  /*enable uart TX wake up HOST*/
    BEKEN_SET_UART_AFTER_WAKEUP_SIG_WAIT_COUNT  =0x12,  /*set UART TX wake up HOST count*/

    BEKEN_MAX_ACL_BUFF_SIZE                     =0x13,  /*useless*/
    BEKEN_ACL_ACTIVE_CHECK_WHEN_SLEEP           =0x14,
    BEKEN_DISABLE_SNIFFER_WHEN_OTHER_LINK_ACTIVE=0x15,
    BEKEN_LM_CONFIG_SLEEP_IN_STANDBY_MONITOR_PERIOD =0x16,
    BEKEN_LM_CONFIG_AWAKE_IN_STANDBY_MONITOR_PERIOD =0x17,
    BEKEN_DISABLE_ACL_ACCEPT_WHEN_ACL_EXIST     =0x18,
    BEKEN_ENABLE_TX_POWER_CONTROL               =0x19,    
    BEKEN_LM_CONFIG_AWAKE_KEEP_WHEN_UART_WAKEUP =0x1A,
    BEKEN_DISABLE_INQUIRY_WHEN_ACL_EXIST        =0x1B,
    BEKEN_DELAY_BETWEEN_EVERY_PACKET_UART_TX    =0x1C,
    BEKEN_SCATTER_LENGTH_FOR_PACKET_UART_TX     =0x1D,
    BEKEN_ENABLE_ACK_SEQ_CHECK                  =0x1E,
    BEKEN_RESET_ACK_SEQ_AFTER_TX                =0x1F,
    BEKEN_GET_FW_VERSION                        =0x20,
    BEKEN_UART_CLOCK_CONFIG_BEFORE_TX_LOW_LEVEL =0x21,
    BEKEN_UART_BAUD_RATE_FOR_TX_LOW_LEVEL       =0x22,
    BEKEN_DELAY1_AFTER_TX_LOW_LEVEL             =0x23,
    BEKEN_DELAY2_AFTER_TX_LOW_LEVEL             =0x24,
    BEKEN_DELAY_FOR_OBEX_PACKET_FINAL           =0x25,
    BEKEN_DISABLE_SPREADTRUM_HCI                =0x26,
    BEKEN_MAX_ACL_BUFF_NUMBER                   =0x27,
    BEKEN_WRITE_ADDR_AFTER_RESET                =0x28,
    
    BEKEN_ENABLE_MASTER_AFC                     =0x2C,
    BEKEN_ENABLE_VIMicro_ENCRYPTION_ISSUE       =0x2D,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC              =0x2E,
    BEKEN_CHANGE_CPU_CLK                        =0x2F,
    BEKEN_ENABLE_CSR_TX_CRC                     =0x30,
    
    SLEEP_FOR_ATE_POWER_TEST                    =0x32,
    BEKEN_DISALBE_HAREWARE_ERROR_LOG            =0x33,
    BEKEN_ENABLE_ROLE_SWITCH                    =0x34,
    BEKEN_SET_LMP_FEATURES                      =0x35,
    BEKEN_SET_LMP_EXT_FEATURES                  =0x36,
    BEKEN_DISALBE_EDR3                          =0x37,
    BEKEN_DISALBE_2DH1_WHEN_AUTORATE            =0x38,    
    BEKEN_FORCE_DM1_WHEN_LITTLE_PACKET          =0x39,
    BEKEN_ENABLE_QOS                            =0x3A,
//    BEKEN_DISABLE_ESCO                          =0x3A,    
    BEKEN_DELAY_PTT_SET                         =0x3B,    
    BEKEN_SET_32K_WAKUP_TIME                    =0x3C,
    BEKEN_CFG_MIN_SLOTS_FOR_SLEEP_PROCEDURE     =0x3D,
    BEKEN_SET_HOST_WAKEUP_TIME                  =0x3E,
    BEKEN_BT_ACTIVE_PIN_SEL                     =0x3F,
    BEKEN_BT_PRIORITY_PIN_SEL                   =0x40,
    BEKEN_WLAN_ACTIVE_PIN_SEL                   =0x41,
    BEKEN_WLAN_ACTIVE_PIN_POL                   =0x42,
    BEKEN_ENABLE_SOFTIRQ_FOR_UART               =0x43,
    BEKEN_SET_T_SNIFF_MIN                       =0x44,
    BEKEN_SET_T_SNIFF_MAX                       =0x45,
    BEKEN_CHANGE_SNIFF_ATTEMP_WHEN_SNIFF_MIN_IS_SMALL   =0x46,
    BEKEN_CHANGE_SNIFF_TIMEOUT_WHEN_SNIFF_MIN_IS_SMALL  =0x47,
    BEKEN_PTA_TX_DELAY_AFTER_PRIORITY           =0x48,
    BEKEN_PTA_TX_DELAY_AFTER_FREQ_OVERLAP       =0x49,
    BEKEN_PTA_RX_DELAY_AFTER_PRIORITY           =0x4A,
    BEKEN_ENABLE_PTA                            =0x4B,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC2             =0x4C,
    BEKEN_SPEED_UART_CRC                        =0x4D,
    BEKEN_UART_MODULE_TEST_CMD                  =0x50,
    BEKEN_UART_MODULE_SUB_TEST_CMD              =0x51,
    BEKEN_UART_MODULE_GENERAL_CMD               =0x52,
    BEKEN_ENABLE_AUTO_CHANGE_CPU_CLK            =0x53,

    BEKEN_SHOW_SYSTEM_INFO                      =0x60,
    
    BEKEN_SET_SCO_USE_HCI                       =0x82,
#if (CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
   	BEKEN_SET_L2CAP_FLOW						=0x83,
#endif
	BEKEN_EXCHANGE_ACTIVE_ESCO					=0x84,
	
#if (DEBUG_BASEBAND_MONITORS == 1)
    BEKEN_READ_BASEBAND_MONITORS                =0x90,
    BEKEN_RESET_BASEBAND_MONITORS               =0x91,
#endif
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
    BEKEN_AGC_MODE_PARAM                        =0x92,
#endif
#if (DEBUG_SCATTERNET_MONITORS == 1)
    BEKEN_READ_SCATTERNET_MONITORS              =0x94,
#endif
	BEKEN_CLEAR_LINKKEY_CMD						=0x95,

    BEKEN_FLASH_READ_CMD                        =0xA0,
    BEKEN_FLASH_WRITE_CMD                       =0xA1,
    BEKEN_FLASH_ERASE_CMD                       =0xA2,
    BEKEN_SHOW_STACK_CMD                        =0xA3, // 0xAA for uart download
    BEKEN_DUMP_ENV_CMD                          =0xAB,
    BEKEN_SHOW_BT_STATUS                        =0xAC,
    BEKEN_SHOW_BT_DEBUG                         =0xAD,
    BEKEN_PRINT_LINK_KEY                        =0XAE,
    BEKEN_ENTRY_DUT_MODE                        =0XAF,
#ifdef CONFIG_TWS
	BEKEN_TWS									=0xB1,
#endif
	BEKEN_LED_EQ_BUTTON							=0xB2,
	BEKEN_SET_AEC_PARA							=0xB3,
	BEKEN_ENTRY_FCC_TESTMODE					=0xFC,
    LOOP_MODE_CMD                               =0XCC,
    BEKEN_TEMP_CMD                              =0XDD,

#ifdef CONFIG_PRODUCT_TEST_INF
    BEKEN_RSSI_CMD                              =0XE0,
#endif

    /* begin: the following cmds just for testing. */
    BEKEN_CMD_SDP_CONNECT                       =0XE1,
    BEKEN_CMD_SERVICE_SEARCH_REQUEST            =0XE2,
    BEKEN_CMD_SERVICE_ATTRIBUTE_REQUEST         =0XE3,
    BEKEN_CMD_A2DP_SSA_REQUEST                  =0XE4,
    BEKEN_CMD_HFP_SSA_REQUEST                   =0XE5,
#if defined(BT_ONE_TO_MULTIPLE) && !defined(CONN_WITH_MUSIC)
    BEKEN_CMD_1V2_POLL_PKT_TYPE     			=0XE8,
#endif	
    BEKEN_CMD_SYS_RESET                         =0xFE
    /* end: the cmds just for testing. */

};

#endif
