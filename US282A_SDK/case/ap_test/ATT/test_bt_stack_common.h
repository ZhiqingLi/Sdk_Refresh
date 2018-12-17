#ifndef _TEST_BTSTACK_COMMON_H
#define _TEST_BTSTACK_COMMON_H

#define HFP_SUPPORTED           0x01
#define A2DP_SUPPORTED          0x02
#define AVRCP_SUPPORTED         0x04
#define PROFILE_VALID           0x80
#define LINKKEY_VALID           0x40

#if 0
typedef struct
{
    uint8 bytes[BD_ADDR_LEN];
} t_bdaddr;

/*!
 * \brief
 *  bt_dev_info_t 提供给上层的远端蓝牙设备信息
 */
typedef struct
{
    /*! 已配对蓝牙设备地址 */
    t_bdaddr bd_addr;
    /*! 已配对蓝牙名称 */
    uint8 name[MGR_MAX_REMOTE_NAME_LEN];
    /*! 用于标识设备的使用频率，1表示最后一次使用
     * 然后依此类推*/
    uint8 mru_num; 
    uint8 support_profile;//增加连接过的profile供回连时使用
} bt_dev_info_t;

typedef struct
{
    uint32 enable_a2dp              : 1;
    uint32 enable_a2dp_cp           : 2;
    uint32 enable_avdtp_aac         : 1;
    uint32 enable_avrcp             : 1;
    uint32 enable_avrcp_14          : 1;
    uint32 enable_avrcp_volume_sync : 1;
    uint32 enable_hfp               : 1;
    uint32 enable_hfp_volume_sync   : 1;
    uint32 enable_hfp_bat_report    : 1;
    //HFP_PB_ALL,HFP_PB_DISABLE,HFP_PB_ONLY_NUM, 0:禁止，1:完整版本 2:只有来电号码播报
    uint32 enable_hfp_phonebook     : 2;
    uint32 enable_spp               : 1;
    uint32 enable_ble               : 1;
    uint32 enable_hid               : 1;
} btstack_config_t;

typedef struct
{
    uint8 work_mode : 2;
    uint8 need_patch_download : 1;
    uint8 save_addr : 1;
    uint8 save_name : 1;
    uint8 max_paired_num;
    uint8 volume;/*远程设备会来请求当前音量，所以将当前音量在BT STACK也维护好！！该音量值转为AVRCP VOLUME 0 ~ 127*/
    uint16 uart_idle_timer;
    uint32 random_lap;
    btstack_config_t cfg;

    t_bdaddr rmt_bd_addr;
    bt_dev_info_t *cfg_dev_info;
} bt_stack_arg_t;
#endif
typedef struct
{
	uint8    remote_addr[6];
	uint8    support_profile;		
} bt_paired_dev_info_t;

typedef struct
{
	uint8    remote_addr[6];
	uint8    support_profile;	
	uint8    linkkey_type;
	uint8    linkkey[16];
	uint8    local_addr[6];
    uint8    remote_name[16];
	uint8    reserved[6];
} bt_paired_dev_info2_t;

typedef enum
{
    TEST_CONN_STATUS_NONE,
    TEST_CONN_STATUS_WAIT_PAIR,
    TEST_CONN_STATUS_PAIRED,
    TEST_CONN_STATUS_LINKED,
    TEST_CONN_STATUS_ERROR		
} test_conn_status_e;

typedef enum
{
    TEST_BT_ERR_NONE,
    TEST_BT_ERR_HW,
    TEST_BT_ERR_PAGE_TIMEOUT,
    TEST_BT_ERR_CONNECTION_TIMEOUT,
    TEST_BT_ERR_LINKEY_MISS,  //对方取消配对
    TEST_BT_ERR_NOT_POPUP_WINDOW //对方不想弹出窗口 	
} test_bt_err_status_e;

typedef struct
{
    uint8 conn_status;
    uint8 err_status;
    uint8 a2dp_status;
    uint8 hfp_status;	
    uint8 num_connected; 
    uint8 support_profile;	       	
} test_btstack_status_t;

#endif
