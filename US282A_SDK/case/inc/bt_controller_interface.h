#ifndef _BT_CONTROLLER_INTERFACE_H            //防止重定义
#define _BT_CONTROLLER_INTERFACE_H

#include "psp_includes.h"
#include "case_independent.h"

typedef enum
{
    HW_RESULT_NULL = 0x00,
    HW_RESULT_H5LINK_ERROR,
    HW_RESULT_PATCHDOWNLOAD_ERROR,
} bt_controller_result_e;

typedef uint16 (*bt_drv_hci_buffer_get_len)(void);
typedef void (*bt_drv_hci_buffer_read_data)(uint32 buf_addr, uint32 read_len);
typedef void (*bt_stack_hci_pkg_dispatch)(uint16 read_num, uint8 hci_overflow);

typedef struct
{
    bt_stack_hci_pkg_dispatch cbk;
    uint8 bd_addr[6];
    uint16 uart_idle_timer; //进入IDLE状态的定时器，单位为1ms
    bt_drv_hci_buffer_get_len *p_hci_buffer_get_len_cb; //返回参数，直接修改gBtDevDesc.hciCtrl
    bt_drv_hci_buffer_read_data *p_hci_buffer_read_data_cb; //返回参数，直接修改gBtDevDesc.hciCtrl
} bt_drv_param_t;

typedef enum
{
    BT_CONTROLLER_STATUS_NORMAL = 0,
    BT_CONTROLLER_STATUS_IDLE = 1,
    BT_CONTROLLER_STATUS_LPS = 2,
} bt_controller_status_e;

/*1.定义提供的具体接口命令，这里是按键驱动统一提供的接口*/

typedef enum
{
    BT_SEND_DATA = 0,
    BT_WAKE,
    BT_DOWN_PATCHCOD,
    BT_GET_STATUS,
    BT_POWERON,
    BT_POWEROFF,
    BT_DATA_SAVE,
    BT_DATA_RESTORE,
    BT_GET_VER,
    BT_CMD_MAX
} bt_drv_cmd_e;

typedef enum
{
    BT_NORMAL_MODE = 0,
    BQB_TEST_MODE = 1,
    S3BT_START_MODE = 2,
    BT_BTT_MODE = 3,
    BT_NORMAL_NOPATCH_MODE = 4,
} bt_controller_workmode_e;

typedef struct
{
    uint32 file_len;
    uint8 download_patch;
    uint8 reset_controller;
    uint8 reserved[2];
    //变长数组
    uint8 patch_buffer[0];
} btt_priv_data_t;

extern void *bt_op_entry(void *param1, void *param2, void *param3, bt_drv_cmd_e cmd)__FAR__;

/******************************************************************************/
/*!
 * \par  Description:
 *    send data from buffer to uart tx.
 * \param[in]    buf        data-buffer.
 * \param[in]    count      length of data to be sent.
 * \param[out]   none
 * \return       length of data to be sent.
 *****************************************************************************/
#define bt_drv_send_data(addr, len)    bt_op_entry((void*)(uint32)(addr),(void*)(uint32)(len),(void*)0,BT_SEND_DATA)

/******************************************************************************/
/*!
 *将bt controller唤醒
 * mode 0 :controller 根据自己实际情况是否进lps
 * mode 1 :controller 从lps退出
 *****************************************************************************/
#define bt_drv_controller_wake(mode)       bt_op_entry((void*)(uint32)(mode),(void*)0,(void*)0,BT_WAKE)

/******************************************************************************/
/*!
 *初始化uart模块,下载bt controller工作所需要的patchcode.
 * mode            是否需要下载patchcode        1:需要下载;0:不需要下载
 * bt_work_mode    bt controller 后续的工作模式,BT_NORMAL_MODE;BQB_TEST_MODE
 ******************************************************************************/
#define bt_drv_down_patchcode(mode,bt_work_mode,priv_data) (bt_controller_result_e) bt_op_entry((void*)(uint32)(mode),\
(void*)(uint32)(bt_work_mode),(void*)(priv_data), BT_DOWN_PATCHCOD)

/******************************************************************************/
/*获取bt controller工作状态
 ******************************************************************************/
#define bt_drv_get_status()            (bt_controller_status_e) bt_op_entry((void*)0,(void*)0,(void*)0,BT_GET_STATUS)

/******************************************************************************/
/*bt controller poweron
 ******************************************************************************/
#define bt_drv_controller_poweron()            bt_op_entry((void*)0,(void*)0,(void*)0,BT_POWERON)

/******************************************************************************/
/*bt controller poweroff
 ******************************************************************************/
#define bt_drv_controller_poweroff()           bt_op_entry((void*)0,(void*)0,(void*)0,BT_POWEROFF)

/******************************************************************************/
/*bt controller data save
 *暂时用于s3bt退出;bt controller data保存
 ******************************************************************************/
#define bt_drv_controller_data_save(addr)           bt_op_entry((void*)(uint32)(addr),(void*)0,(void*)0,BT_DATA_SAVE)

/******************************************************************************/
/*bt controller data save
 *暂时用于s3bt退出;bt controller data保存
 ******************************************************************************/
#define bt_drv_controller_data_restore(addr)   bt_op_entry((void*)(uint32)(addr),(void*)0,(void*)0,BT_DATA_RESTORE)

/******************************************************************************/
/*bt controller get version
 *读取蓝牙控制器版本号
 ******************************************************************************/
#define bt_drv_controller_get_ver(ver, len)    bt_op_entry((void*)(ver),(void*)(uint32)(len),(void*)0,BT_GET_VER)


#endif

