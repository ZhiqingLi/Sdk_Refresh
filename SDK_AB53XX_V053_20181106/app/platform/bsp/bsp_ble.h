#ifndef _BSP_BLE_H
#define _BSP_BLE_H

struct att_hdl_t {
    u16 hdl;    //att handle
    u8 cfg;     //client config enable
    u8 res;
};

enum {
    BLE_IDX_BATTERY,
    BLE_IDX_APP,
    BLE_IDX_END,
};

void ble_init_att_do(u8 index, u16 handle, u8 config , u8* buf, u8 len);
void bt_app_cmd_process(u8 *ptr, u16 size);
bool ble_send_packet(u8 *buf, u8 len);
#endif
