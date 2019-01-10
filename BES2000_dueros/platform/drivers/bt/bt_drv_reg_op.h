#ifndef __BT_DRV_REG_OP_H__
#define  __BT_DRV_REG_OP_H__

#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "plat_types.h"


#ifdef __cplusplus
extern "C" {
#endif

void bt_drv_reg_op_rssi_set(uint16_t rssi);
void bt_drv_reg_op_scan_intv_set(uint32_t scan_intv);
void bt_drv_reg_op_encryptchange_errcode_reset(uint16_t hci_handle);
void bt_drv_reg_op_sco_checker(void);
void bt_drv_reg_op_trigger_time_checker(void);
void bt_drv_reg_op_tws_output_power_fix_separate(uint16_t hci_handle, uint16_t pwr);
bool bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_get(void);
void bt_drv_reg_op_ld_sniffer_env_monitored_dev_state_set(bool state);
int bt_drv_reg_op_currentfreeaclbuf_get(void);
void bt_drv_reg_op_save_mobile_airpath_info(uint16_t hciHandle);
void bt_drv_reg_op_block_xfer_with_mobile(uint16_t hciHandle);
void bt_drv_reg_op_resume_xfer_with_mobile(uint16_t hciHandle);
int bt_drv_reg_op_packet_type_checker(uint16_t hciHandle);
void bt_drv_reg_op_max_slot_setting_checker(uint16_t hciHandle);
void bt_drv_reg_op_force_task_dbg_idle(void);
void bt_drv_reg_op_afh_follow_mobile_mobileidx_set(uint16_t hciHandle);
void bt_drv_reg_op_afh_follow_mobile_twsidx_set(uint16_t hciHandle);
void bt_drv_reg_op_connection_checker(void);
void bt_drv_reg_op_bt_info_checker(void);
void bt_drv_reg_op_ble_buffer_cleanup(void);

void bt_drv_reg_op_crash_dump(void);

#ifdef __cplusplus
}
#endif

#endif

