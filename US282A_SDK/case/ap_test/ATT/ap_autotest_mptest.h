#ifndef __AP_AUTOTEST_MPTEST_H
#define __AP_AUTOTEST_MPTEST_H

#pragma __PRQA_IGNORE_START__

//接收CONTROLLER数据的缓冲区
#define ATT_RINGBUFFER_ADDR  0x30000
#define ATT_RINGBUFFER_LEN   16 * 1024

#define INVALID_CFO_VAL      (0x7fffffff)
#define INVALID_PWR_VAL      (0x7fffffff)

//允许cfo偏差最大值，超过该值，工具报错
#define MAX_CFO_DIFF_VAL     (20000)

#define CFO_TEST_RETRY_NUM   (3)

#define MIN_RESULT_NUM       (5)

typedef enum
{
    BT_PAYLOAD_TYPE_ALL0 = 0,
    BT_PAYLOAD_TYPE_ALL1 = 1,
    BT_PAYLOAD_TYPE_0101 = 2,
    BT_PAYLOAD_TYPE_1010 = 3,
    BT_PAYLOAD_TYPE_0x0_0xF = 4,
    BT_PAYLOAD_TYPE_0000_1111 = 5,
    BT_PAYLOAD_TYPE_1111_0000 = 6,
    BT_PAYLOAD_TYPE_PRBS9 = 7,
    //////////////////////////////////
    BT_PAYLOAD_TYPE_NUM = 8
} BT_PAYLOAD_TYPE;

typedef enum
{
    BT_PKT_1DH1 = 0,
    BT_PKT_1DH3,
    BT_PKT_1DH5,
    BT_PKT_2DH1,
    BT_PKT_2DH3,
    BT_PKT_2DH5,
    BT_PKT_3DH1,
    BT_PKT_3DH3,
    BT_PKT_3DH5,
    BT_PKT_LE,
    //////////////////////////////////////////////////
    BT_PKT_TYPE_NULL,
    BT_PKT_TYPE_NUM
} BT_PKT_TYPE;

typedef struct
{
    uint16 read_ptr;
    uint16 write_ptr;
    uint16 readable_len;
    uint8 *read_buffer;
} ringbuf_rw_t;

/** 分析接收包的状态
*/
typedef enum
{
    UART_HCI_STATUS_RX_NULL = 0,
    UART_HCI_STATUS_RX_TYPE, 
    UART_HCI_STATUS_RX_HEADER, 
    UART_HCI_STATUS_RX_DATA
}rev_pkt_state_e;


typedef struct
{
    uint16 (*get_data_len)(void);
    void (*read_data)(uint32 buf_addr, uint32 data_len);
    uint16 minReqLen;
    uint16 pktHdrLen;
    uint8 pktType;
    rev_pkt_state_e reqRxParseStatus;
    uint8 headerBuffer[32];    
} btt_hci_deal_t;

typedef struct
{
    uint8 pkt_index;
    uint8 total_node;
    uint16 next_pkt_offset;
} cmd_pkt_head_t;

typedef struct
{
    uint8 cmd_len;
    uint8 cmd_data[0];
} cmd_node_t;

typedef struct
{
    cmd_pkt_head_t pkt_head;
    cmd_node_t cmd_node[0];
} cmd_pkt_t;

typedef enum
{
    PKT_CFO_BEGINE_INDEX = 0, PKT_CFO_STOP_INDEX = 1, PKT_CFO_UPDATE_INDEX = 2,
} pkt_index_e;

//cfo参数 32字节
typedef struct
{
    uint8 ic_type;
    uint8 channel;
    uint8 tx_gain_idx;
    uint8 tx_gain_val;
    uint8 payload;
    uint8 pkt_type;
    uint8 tx_dac;
    uint8 whitening_cv;
    uint16 pkt_header;
    //uint16 tx_pkt_cnt;
    uint8 reserved0[2];
    uint32 hit_target_l;
    uint32 hit_target_h;
    uint8 sut_state; //0:默认状态 begin 1:continue 2:stop
    uint8 report_interval; //以ms为单位
    uint8 skip_report_count; //忽略的前n次report结果
    uint8 once_report_pkts; //单次report cfo需要的包数
    uint8 report_timeout; //接收rx report的超时时间，秒为单位
    uint8 recv_cfo_count; //接收返回的cfo,pwr个数，stop返回
    //uint8 sut_download_patch;   //SUT是否下载patch
    uint8 reserved[6];
} cfo_param_t;

typedef struct
{
    stub_ext_cmd_t ext_cmd;
    cfo_param_t cfo_param;
} cfo_test_param_t;

typedef struct
{
    int8 cfo_index_low;
    int8 cfo_index_high;
    uint16 cfo_index;
    int32 cfo_val;
    int32 pwr_val;
    int32 ber_val;
} cfo_return_arg_t;

typedef uint32 (*modify_cb_func_t)(uint8 *hci_data, mp_test_arg_t *mp_arg);

#define MP_ICTYPE                           0
#define MP_TX_GAIN_IDX                      7
//#define MP_TX_GAIN_VAL                      0xcc
#define MP_TX_GAIN_VAL                      0xce
#define MP_TX_DAC                           0x13
#define HIT_ADDRESS_SET_L                   0x009e8b33
#define HIT_ADDRESS_SET_H                   0
#define PKTTYPE_SET		                    BT_PKT_1DH1
#define PKTHEADER_SET                       0x1234
#define WHITENCOEFF_SET	                    0x7F                    //(TX)   --->FF (RX)
#define PAYLOADTYPE_SET                     BT_PAYLOAD_TYPE_PRBS9
#define MP_REPORT_RX_INTERVAL               1
#define MP_SKIP_PKT_NUM                     0
#define MP_ONCE_REPORT_MIN_PKT_NUM          8
#define MP_RETURN_RESULT_NUM                10
#define MP_REPORT_TIMEOUT                   2
#define BER_TX_PKT_CNT                      0xffff
#define BER_TX_REPORT_INTERVAL              1
#define BER_TX_REPORT_TIMEOUT               4

typedef enum
{
    CAP_INDEX_0 = 0, //电容值范围0pf-3.0pf
    CAP_INDEX_1 = 1, //电容值范围3pf-6pf
    CAP_INDEX_2 = 2, //电容值范围6pf-9pf
    CAP_INDEX_3 = 3, //电容值范围9pf-12pf
    CAP_INDEX_4 = 4, //电容值范围12pf-15pf
    CAP_INDEX_5 = 5,
//电容值范围15pf-18pf
} cap_index_value_e;

#define CFO_THRESHOLD_LEFT  0
#define CFO_THRESHOLD_RIGHT (CAP_INDEX_5 * 32)
#define CFO_THRESHOLD_ADJUST_VAL  5

#define SEARCH_LEFT_FLAG    0x01
#define SEARCH_RIGHT_FLAG   0x02

extern ringbuf_rw_t g_ringbuf_rw;
extern btt_hci_deal_t g_hci_deal;
extern uint16 g_mpdata_bin_len;
extern volatile uint8 g_hci_event_rev_flag;
extern uint32 g_update_timer_id;
extern uint8 g_SUT_state;

extern void put_hci_data_to_ringbuf(uint16 read_num, uint8 hci_overflow);

#pragma __PRQA_IGNORE_END__
#endif

