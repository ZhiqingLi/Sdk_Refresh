#ifndef __AP_AUTOTEST_BERTEST_H
#define __AP_AUTOTEST_BERTEST_H

#define BER_HCI_CMD_BIN   "berdata.bin"
#define RX_DEBUG_BYTES   10

//以下两个值决定小机做一次ber分析的时间
//按间隔RECV_PKT_REPORT_INTERVAL取样，按PKT_INFO_COUNT获取采样个数
//然后根据采样得到的数据决定ber是否正确

//小机获取rx report间隔时间
#define RECV_PKT_REPORT_INTERVAL    50
//小机每次获取多少个report做一次ber分析
#define PKT_INFO_COUNT              10


#define INVALID_BER_VAL             0

//允许ber偏差最大值，超过该值，工具报错
#define MAX_BER_DIFF_VAL            (5)

//一次解析至少接收正确的BER数值个数
#define MIN_BER_RESULT_NUM          (8)

#define MAX_BER_RETRY_CNT           (1)

//计算ber参数时允许接收的最小包数
#define MIN_RECV_PKT_NUM            (0x40)

/** BEGIN命令时上传给PC工具的参数
*/
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
    uint16 tx_pkt_cnt;
    uint32 hit_target_l;
    uint32 hit_target_h;
    uint8 sut_state;            //0:默认状态 begin 1:continue 2:stop
    uint8 report_interval;      //以ms为单位
    uint8 skip_report_count;    //忽略的前n次report结果
    uint8 once_report_pkts;     //单次report cfo需要的包数
    uint8 report_timeout;       //接收rx report的超时时间，秒为单位
    uint8 recv_cfo_count;       //接收返回的cfo,pwr个数，stop返回
    uint8 reserved[6];
}ber_param_t;



/** PC工具通过STOP命令返回的参数
*/
typedef struct
{
    uint32 ber;
    uint8 reserve[28];
}ber_retrun_arg_t;


/** DUT在STOP的时候传给PC工具分析后的结果
    包括: pkg number,error bit
*/
typedef struct
{
	uint32	 pkts;     //DUT实际接收的包数
	uint32	 errpkts;  //DUT实际接收的错误包数，计算PER
	uint32   bits;	   //DUT实际接收的bits 数
	uint32	 errbits;  //DUT实际接收的错误bits 数，计算BER
	uint32   ber_thr; //BER门限，单位百万分之一
	uint8    reserve[12];
}ber_update_arg_t;


/** 灵敏度计算接收的包信息
*/
typedef struct
{
    uint32 time;
    uint16 result;
}ber_pktinfo_t;

/** 测试流程状态
*/
typedef enum
{
    BER_STATE_STOP = 0,
    BER_STATE_START,
    BER_STATE_FINISH
}ber_test_state_e;

typedef enum
{
    BER_CMD_STATE_WRITE_PACKET_NUM = 0,
    BER_CMD_STATE_READ_PACKET_NUM = 1,
    BER_CMD_STATE_WRITE_ERROR_BIT = 2,
    BER_CMD_STATE_READ_ERROR_BIT = 3,
    BER_CMD_STATE_WRITE_RSSI = 4,
    BER_CMD_STATE_READ_RSSI = 5,
    BER_CMD_STATE_WRITE_CLEAR_REPORT = 6,
    BER_CMD_STATE_DELAY,
}ber_cmd_state_e;


/** 全局控制结构
*/
typedef struct
{
    btt_hci_deal_t g_hci_deal;
    ber_update_arg_t dut2pc_data;  //stop时上传PC参数
    ber_retrun_arg_t pc2dut_data;  //stop时从PC获取的参数
    int32 get_pktinfo_timer; //RX 监控定时器
    uint16 berdata_len; //berdata.bin的数据长度
    uint8 rev_pkt_flag; //接收到数据包的标志位
    uint8 rev_valid_pkt_flag; //接收到有效数据包标志位
    uint8 rx_debug_buf[RX_DEBUG_BYTES]; 
    ber_pktinfo_t pkt_info;//缓存结果包信息
    uint8 pkt_info_count;
    uint8 ber_value[PKT_INFO_COUNT];
    uint8 rssi_value[PKT_INFO_COUNT];
    ber_test_state_e ber_test_state;//灵敏度测试状态 1表示stop, 2表示start, 3表示finish
    uint8 test_cnt;
    uint8 cmd_state; // 0 取packet num 1 取 error bit 2 取rssi
    uint16  rssi;
    uint8 tick_cnt;
    uint8 rssi_test; //是否只进行RSSI测试
}ber_control_t;

extern ber_control_t *g_ber_control;

extern void _ber_test_read_hci_data(uint16 read_num, uint8 hci_overflow) __FAR__;
#endif

