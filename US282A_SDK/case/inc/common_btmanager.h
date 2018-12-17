/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：蓝牙管理器相关的宏定义，枚举类型，结构体定义，外部接口声明等；蓝牙管理器
 *       充当 BT STACK 与 CASE 的中间人角色。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef __COMMON_BTMANAGER_H__
#define __COMMON_BTMANAGER_H__

#include "psp_includes.h"
#include "btstack_common.h"
#include "common_discrete_led.h"

#define SUPPORT_BT_COMMON_TTS   0 //TTS方式公共提示蓝牙状态
#define SUPPORT_BT_COMMON_KT    0 //按键音方式公共提示蓝牙状态

//下面几个回连计时都是以500ms为单位
#define A2DP_CONNECT_TIME_AFTER_HFP_CONNECT         16   /* wait remote initiate a2dp-connection.*/
#define HFP_CONNECT_TIME_AFTER_A2DP_CONNECT         16
#define HID_CONNECT_TIME_AFTER_OTHER_CONNECT        16

#define CHECK_AVRCP_CONNECT_TIME                    8
#define HFP_DISCONNECT_TIME_AFTER_A2DP_DISCONNECT   0     /* wait a2dp-disconnection.*/

#define AUTO_CONNECT_NEED_CONNECT_HFP     0x01
#define AUTO_CONNECT_NEED_CONNECT_A2DP    0x02
#define AUTO_CONNECT_NEED_CONNECT_AVRCP   0x04
#define AUTO_CONNECT_NEED_CONNECT_HID     0x20   //同btstack_common.h中定义serv_conn

typedef struct
{
    t_bdaddr remote_addr;
    uint8 support_profile;            //回连服务类型
    uint8 reconnect_another_cnt;      //在连接成功后，尝试回连另外一个服务的计数，次数限制由 reconnect_another_limit 决定
    uint16 avrcp_disconnect_cnt;      //倘若A2DP已连接且AVRCP未连接，定时发起连接AVRCP
    uint16 onlyone_disconnect_cnt;    //倘若尚有一个服务未连接，定时发起连接另一个服务，这里的服务仅指A2DP和HFP这两个
    uint16 connect_after_hfp_cnt;     //HFP连接OK后需要回连A2DP的计数
    uint8 connect_cnt;                //回连周期数
    uint8 conn_flag;                  //0:不需要回连, 1:需要回连
#ifdef ENABLE_TRUE_WIRELESS_STEREO    
    uint8 dev_type;                   //回连设备类型，NO_TWS_DEV/TWS_DEV
#endif
} auto_conn_dev_info_t;

typedef struct
{
    uint8 enable_auto_connect;        //是否开启回连功能
    uint8 need_auto_connect;          //是否需要回连手机
    int8 auto_connect_timer_id;       //回连COM软定时器
    uint8 enable_a2dp : 1;            //是否支持a2dp服务
    uint8 enable_hfp : 1;             //是否支持hfp服务
    uint8 enable_avrcp : 1;           //是否支持avrcp服务
    uint8 enable_hid : 1;               //是否支持hid服务
    uint8 reconnect_another_limit;    //在连接成功后，尝试回连另外一个服务的次数限制
    uint8 auto_connect_type;          //记录回连的类型:0开机回连，1切换模式后回连、2超时断开回连
    uint8 auto_connect_count_timeout; //超时断开后自动回连次数，0表示一直回连，1~15表示有限次
    uint8 auto_connect_count_startup; //开机后第一次进入自动回连次数，0表示一直回连，1~15表示有限次
    uint8 auto_connect_count_mode;    //切MODE键进入蓝牙自动回连次数，0表示一直回连，1~15表示有限次
#ifdef ENABLE_TRUE_WIRELESS_STEREO
    uint8 tws_prior_role;             //tws 之前充当的角色
#endif
    uint16 auto_connect_interval;     //自动回连时间间隔
    uint16 connect_tick_cnt;          //回连计时器计数
    auto_conn_dev_info_t dev_info[RMT_DEV_NUM];
} bt_auto_connect_ctrl_t;

typedef enum
{
    BTMANAGER_STATUS_IDLE = 0,
    BTMANAGER_STATUS_WORKING = 1,
    BTMANAGER_STATUS_LINKING = 2,
    BTMANAGER_STATUS_POWERON = 3,
} btmanager_status_e;

typedef struct
{
    hfp_status_e last_hfp_status[RMT_DEV_NUM];
    a2dp_status_e last_a2dp_status;
    conn_status_e last_conn_status;
    uint8 last_spp_ble_status;
    uint8 last_battery_value;
    uint8 last_num_connected;
    int8 a2dp_timer_id;
    int8 avrcp_timer_id;
    int8 sniff_timer_id;
    int8 bat_rpt_timer_id;
    uint8 support_dev_num;
    uint8 max_paired_num;
    uint8 send_vol_syn;
    uint8 discoverable_flag;
    btmanager_status_e btmanager_status;
    uint8 linking_timer;
    volatile bool btstack_standby;
    uint8 autoconnect_link_succ_flag; //回连成功标志
    uint8 enable_spp : 1; //是否支持SPP数据链路
    uint8 enable_ble : 1; //是否支持BLE数据链路
    uint8 enable_avrcp_volume_sync : 1; //是否支持AVRCP音量同步
	uint8 enable_siri : 1;		// 是否支持SIRI
#ifdef ENABLE_TRUE_WIRELESS_STEREO		
    uint8 last_role;//for tws use 
	uint8 discover_ctl;	
#endif //#ifdef ENABLE_TRUE_WIRELESS_STEREO		
    uint32 last_call_last_timestamp; //最近一次回拨时间，用于避免频繁发送回拨命令，导致响应不及时而堆积在BT STACK
} btmanager_global_variable_struct;

extern uint8 g_bt_auto_connect_addr[BD_ADDR_LEN];
extern bt_auto_connect_ctrl_t g_bt_auto_connect_ctrl;
extern btmanager_global_variable_struct g_btmanager_gl_var;
extern int8 g_fast_discoverable_timer;
extern uint16 g_fast_discoverable_cnt;

extern uint32 com_btmanager_gen_random_lap(void) __FAR__;
extern void __section__(".bank_4")com_btmanager_init(bool standby_exit) __FAR__;

#ifdef ENABLE_TRUE_WIRELESS_STEREO
extern uint8 g_tws_m_linein_flag;
extern uint8 g_tws_m_switchap_flag;
extern uint8 send_once_flag;
extern void com_btmanager_call_back_auto_connect(void)__FAR__;
extern void send_cmd_to_other(uint8 op)__FAR__;
#endif

extern void com_btmanager_exit(bool standby_enter, bool need_power_off) __FAR__;
extern void com_btmanager_conn_status_change_handle(bool init_flag) __FAR__;
extern void com_btstack_error_deal(void) __FAR__;

extern void get_bt_stack_info(void) __FAR__;
extern app_result_e com_btmanager_loop(bool tts_flag) __FAR__;
extern app_result_e com_btmanager_event_dispatch(btstack_event_t *p_event, bool tts_flag) __FAR__;
extern app_result_e com_btmanager_event_check_for_tts(btstack_event_t *p_event) __FAR__;

extern void com_restart_auto_connect_set(void) __FAR__;
extern void com_btmanager_autoconnect(void) __FAR__;
extern void com_btmanager_battery_handle(void) __FAR__;
extern void com_btmanager_sniff_handle(void) __FAR__;

extern void change_btmanager_status(btmanager_status_e status) __FAR__;

#define AVRCP_OP_REQ      0 //请求
#define AVRCP_OP_PRESS    1 //按下
#define AVRCP_OP_RELEASE  2 //释放

typedef enum
{
    BTPLAY_AVRCP_PAUSE,       /* 0 */
    BTPLAY_AVRCP_PLAY,        /* 1 */
    BTPLAY_AVRCP_NEXT,        /* 2 */
    BTPLAY_AVRCP_PREV,        /* 3 */
    BTPLAY_AVRCP_FFORWARD,    /* 4 */
    BTPLAY_AVRCP_FBACKWARD,   /* 5 */
    BTPLAY_AVRCP_RELEASEFF,   /* 6 */
    BTPLAY_AVRCP_RELEASEFB,   /* 7 */
    BTPLAY_AVRCP_MAXID,
} btplay_avrcp_opid_e;
extern int com_btmanager_avrcp_op(btplay_avrcp_opid_e opid) __FAR__;
extern app_result_e com_btmanager_avrcp_ind(uint8 op_id, bool tts_flag) __FAR__;
extern app_result_e com_btmanager_avrcp_set_volume_to_soundbox(uint8 vol, bool tts_flag) __FAR__;
extern uint8 com_btmanager_avrcp_volume2avrcp_volume(uint8 vol) __FAR__;
extern int   com_btmanager_avrcp_update_volume_to_phone(uint8 vol) __FAR__;

extern int com_btmanager_tws_send(uint8 data,uint8 data1, uint8 data2,uint8 data3,uint32 type)__FAR__;
#ifdef __BQB_TEST_PROFILE_
extern void  com_btmanager_delay_report(void) __FAR__;
#endif

typedef enum
{
    BTCALL_HFP_ANSWER_CALL,   /* 0 */
    BTCALL_HFP_REJECT_CALL,   /* 1 */
    BTCALL_HFP_HANGUP_CALL,   /* 2 */
    BTCALL_HFP_MAX_REQ,
} btcall_hfp_req_e;
extern int com_btmanager_hfp_call_req(btcall_hfp_req_e req) __FAR__;
extern int com_btmanager_hfp_call_last(void) __FAR__;
extern app_result_e com_btmanager_hfp_set_volume_to_soundbox(uint8 vol, bool tts_flag) __FAR__;
extern int com_btmanager_hfp_update_volume_to_phone(uint8 vol) __FAR__;
extern int com_btmanager_hfp_battery_report_enable(uint8 *bd) __FAR__;
extern void com_btmanager_hfp_battery_report_update_handle(void) __FAR__;
extern int com_btmanager_hfp_battery_report_update(uint8 bat_value) __FAR__;

extern int com_btmanager_force_unlink(void) __FAR__;
extern int com_btmanager_clearpairlist(void) __FAR__;

//for discoverable deal
extern void com_set_discoverable(uint8 mode) __FAR__;

extern void com_set_discover_ctl(uint8 mode) __FAR__;

extern void com_btmanager_power_off(void)__FAR__;
extern app_result_e com_key_deal_shutoff(void)__FAR__;



extern int com_btmanager_BQB_test(void) __FAR__;
extern int com_btmanager_rmt_dev_set(void) __FAR__;

extern int com_btmanager_sppble_send(void *data, uint16 data_len) __FAR__;
extern int com_btmanager_sppble_read(uint8 *rcp_buf, uint16 data_len) __FAR__;
extern void com_btmanager_apk_app_verification_handle(void) __FAR__;
extern int com_btmanager_hfp_siri_handle(void) __FAR__;
#endif
