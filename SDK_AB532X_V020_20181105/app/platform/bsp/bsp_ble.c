#include "include.h"
#include "api.h"

#if LE_EN
const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,
    // Name
    0x0b, 0x09, 'L', 'E', ' ', 'C', 'o', 'u', 'n', 't', 'e', 'r',
};

const uint8_t profile_data[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18,
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a,
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'SPP+LE Counter'
    0x16, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x53, 0x50, 0x50, 0x2b, 0x4c, 0x45, 0x20, 0x43, 0x6f, 0x75, 0x6e, 0x74, 0x65, 0x72,

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18,
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x05, 0x2a,
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    0x08, 0x00, 0x02, 0x00, 0x06, 0x00, 0x05, 0x2a,

// Test Service
    // 0x0007 PRIMARY_SERVICE-FF10
    0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x10, 0xff,

// Test Characteristic
    // 0x0008 CHARACTERISTIC-FFF1-WRITE
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x1a, 0x09, 0x00, 0xf1, 0xff,
    // 0x0009 VALUE-FFF1 | WRITE  | DYNAMIC-''
    0x08, 0x00, 0x1a, 0x01, 0x09, 0x00, 0xf1, 0xff,
    // 0x000a CLINT_CONFIGURATION-FFF1 WRITE | DYNAMIC
    0x0a, 0x00, 0x0a, 0x01, 0x0a, 0x00, 0x02, 0x29, 0x00, 0x00,

    // END
    0x00, 0x00,
}; // total size 99 bytes

void ble_init_att_do(u8 index, u16 handle, u8 config , u8* buf, u8 len);
void bt_app_cmd_process(u8 *ptr, u16 size);

void ble_init_att(void)
{
    u8 buffer[4];
    memset(buffer, 0xaa, 4);
    ble_init_att_do(0, 0x0009, 1, buffer, 4);
}

u8 ble_att_write_callback(u16 handle, u8 *ptr, u8 len)
{
    printf("BLE_W[%04x]: ",handle);
    print_r(ptr, len);
    if (handle == 0x0009) {
        bt_app_cmd_process(ptr, len);
    }
    return true;
}

const u8 *ble_get_profile_data(void)
{
    return profile_data;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    int len;
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    //读取BLE配置的蓝牙名称
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        memcpy(&adv_buf[5], xcfg_cb.le_name, len);
        data_len = 5 + len;
        adv_buf[3] = len + 1;
    }

    return data_len;
}
#else
void ble_init_att(void) {}
const u8 *ble_get_profile_data(void) {return NULL;}
u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size) {return 0;}
u8 ble_att_write_callback(u16 handle, u8 *ptr, u8 len) {return 0;}
#endif
