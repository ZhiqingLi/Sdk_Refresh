#ifndef _PARTHUS_BT_PUMP
#define _PARTHUS_BT_PUMP

/************************************************************************
 * MODULE NAME:    bt_pump.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:    Routines to support Internal Host Controller Data Pumping.
 * MAINTAINER:     John Nelson
 * CREATION DATE:  06 June 2001
 *
 * SOURCE CONTROL: $Id: bt_pump.h,v 1.17 2008/03/03 11:58:02 tomk Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2004 Ceva Inc.
 *     All rights reserved.
 *
 * Notes:
 *     V1 Extracted from bt_test.h 6 June 2001
 *************************************************************************/

/*
 * Note the current hybrid coupling for Connection Handle
 *      Valid connection handle values can be used for indicating Local Loopback
 *      and for Full Reset.
 */
#define PUMP_LOCAL_LOOPBACK__CONNECTION_HANDLE   0xEE
#define PUMP_RESET__CONNECTION_HANDLE            0xFF
#define PUMP_LOCAL_LOOPBACK__DEVICE              1

typedef enum {PUMP_OFF                            = 0x0000,
              PUMP_FIXED_PACKET_LENGTH            = 0x0001,
              PUMP_CYCLE_PACKET_LENGTH            = 0x0002,
              PUMP_CYCLE_PACKET_TYPES_AND_LENGTH  = 0x0004,
              PUMP_LOOPBACK                       = 0x0008,
              PUMP_RECEIVE                        = 0x0010,
              PUMP_LOCAL_LOOPBACK                 = 0x0020,
              PUMP_RESET_ALL                      = 0x0040, 
              PUMP_SCO_PACKET                     = 0x0080,
              PUMP_ERROR                          = 0x0100,
              PUMP_ESCO_PACKET                    = 0x0200,
              PUMP_ESCO_PACKET_COMPLETE           = 0x0400
             }  t_pump_type;

#define  PUMP_TYPES_TX  (PUMP_FIXED_PACKET_LENGTH | PUMP_CYCLE_PACKET_LENGTH | \
                PUMP_CYCLE_PACKET_TYPES_AND_LENGTH | PUMP_LOCAL_LOOPBACK  )

#define  PUMP_TYPES_RX  (PUMP_RECEIVE | PUMP_LOOPBACK | PUMP_LOCAL_LOOPBACK)

/*
 * Options active when pump is running
 */
typedef enum {
              L2CAP_DATA_PUMP_RX_TO_HCI = 1,
              L2CAP_DATA_PUMP_CHECK_LSLC_TX = 2,
              L2CAP_DATA_PUMP_CHECK_LSLC_RX = 4,
              L2CAP_DATA_PUMP_NO_RX_RESET_ON_PACKET_0 = 8
             }  t_pump_options;

#if 0
typedef enum {PUMP_SIG_REMOTE_LOOPBACK     = 0x01,
              PUMP_SIG_REMOTE_RX_START     = 0x02,
              PUMP_SIG_REMOTE_TX_START     = 0x03,
              PUMP_SIG_REMOTE_STOP         = 0x04,
              PUMP_SIG_REMOTE_PAUSE        = 0x05,
              PUMP_SIG_REMOTE_RESUME       = 0x06,
              PUMP_SIG_REMOTE_REQUEST_LOG  = 0x07,
              PUMP_SIG_REMOTE_SEND_LOG     = 0x08
             } t_pump_sig_message;
#endif

/*
 * All major fields are u_int32 for simple encoding/decoding
 */
typedef struct s_BTpump
{
    u_int32 pump_type;
    u_int32 pump_options;
    t_packet packet_type;
    u_int32 packet_length;
    t_deviceIndex target_device_index;
    u_int32 handle;
    u_int32 rx_expected_packet_num;
    u_int32 tx_max_packets;

    u_int32 TX_PDUS_ENQUEUED;
    u_int32 TX_ENQUEUE_FULL_DETECTED;
    /*
     *  Tx Throughput monitoring  (last_time, rx_bytes_per_sec for easy debug)
     */
    u_int32 tx_start_time;
    u_int32 tx_last_time;
    u_int32 tx_bytes;
    u_int32 tx_bytes_per_sec;
    u_int32 tx_bits_per_sec;


    u_int32 RX_PDUS_DEQUEUED;
    u_int32 RX_CRC_ERRS_ON_DEQUEUE;
    u_int32 RX_SEQ_ERRS_ON_DEQUEUE;
    u_int32 RX_LAST_ERROR_PACKET_NUM_ON_DEQUEUE;

    /*
     *  Rx Throughput monitoring  (last_time, rx_bytes_per_sec for easy debug)
     */
    u_int32 rx_start_time;
    u_int32 rx_last_time;
    u_int32 rx_bytes;
    u_int32 rx_bytes_per_sec;
    u_int32 rx_bits_per_sec;
    u_int32 rx_hw_crc_errors, rx_hw_hec_errors, rx_duplicates, rx_no_pkt_count;
    
    /*
     * LSLC Monitors for Before Tx and After BB Rx Checks
     */
    u_int32 LSLCacc_NUM_PUMP_PDUS_TX;
    u_int32 LSLCacc_NUM_PUMP_CRC_ERRS_BEFORE_TX;

    u_int32 LSLCacc_NUM_PUMP_PDUS_RX;
    u_int32 LSLCacc_NUM_PUMP_CRC_ERRS_AFTER_RX;
    u_int32 LSLCacc_NUM_PUMP_MSGTYPE_ERRS_AFTER_RX;
    u_int16 LSLCacc_MIN_PUMP_LEN;
    u_int16 LSLCacc_MAX_PUMP_LEN;
    u_int32 LSLCacc_PUMP_LAST_ERROR_PACKET_NUM;


} t_BTpump_Log;


/*
 * Initialisation function
 */
#if (PRH_BS_CFG_SYS_DATA_PUMP_SUPPORTED==1)
void BTpump_Initialise(void);
void BTpump_Initialise_Device_By_Handle(u_int16 handle);
t_error BTpump_Request_Data_Pump(t_pump_type pump_type, u_int32 pump_options, 
        t_connectionHandle handle, t_packetTypes packet_types,
        t_length packet_length, u_int32 num_packets);

void BTpump_Auto_Actiavtions(void);
u_int8 BTpump_Is_Tx_Data_Pump_Active(void);
u_int8 BTpump_Is_Rx_Data_Pump_Active(void);

u_int8* BTpump_Build_Tx_Payload(boolean l2cap_header,
     u_int8 *pkt_buffer, u_int32 pkt_length, u_int32 pkt_num);

t_error BTpump_Generate_Data_Pump_Tx_Packet(void);
t_error BTpump_Process_Data_Pump_Rx_Packet(void);



#elif (PRH_BS_CFG_SYS_DATA_PUMP_SUPPORTED==0)
/*
 * No pump support, just ignore any calls
 */
#define BTpump_Initialise()
#define BTpump_Request_Data_Pump(pump_type, pump_options, \
        handle, packet_types, packet_length, num_packets)

#define BTpump_Is_Tx_Data_Pump_Active(void)		0
#define BTpump_Is_Rx_Data_Pump_Active(void)		0
#define BTpump_Auto_Actiavtions(void)           0

#define BTpump_Build_Tx_Payload(l2cap_header, pkt_buffer, pkt_length, pkt_num)

#define BTpump_Generate_Data_Pump_Tx_Packet()
#define BTpump_Process_Data_Pump_Rx_Packet()

#endif

#if (PRH_BS_DBG_PUMP_VERIFY_HARDWARE_BEFORE_TX_AFTER_RX==1)
t_TXRXstatus BTpump_Verify_Data_Pump_Before_Tx(t_deviceIndex device,
             const u_int8 *p_payload, t_length tx_length, t_LCHmessage msg_type);
t_TXRXstatus BTpump_Verify_Data_Pump_After_Rx(t_deviceIndex device,
             const u_int8 *p_payload, t_length rx_length, t_LCHmessage msg_type);
#else
#define BTpump_Verify_Data_Pump_Before_Tx(device, p_payload, tx_length, msg_type)
#define BTpump_Verify_Data_Pump_After_Rx(device, p_payload, rx_length, msg_type)
#endif


#endif

