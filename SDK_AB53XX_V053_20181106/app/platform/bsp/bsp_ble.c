#include "include.h"
#include "bsp_ble.h"

#if LE_EN

#define BLE_CMD_BUF_LEN     4
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)

struct ble_cmd_t{
    u8 len;
    u16 handle;
    u8 buf[20];
};

struct ble_cb_t {
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
}ble_cb;

const uint8_t adv_data_const[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x02,
    // Name, 0xff-企业标识符
    0x09, 0xff, 'B', 'l', 'u', 'e', 't', 'r', 'u', 'm',
};

const uint8_t scan_data_const[] = {
    // Name
    0x08, 0x09, 'B', 'L', 'E', '-', 'B', 'O', 'X',
};

const uint8_t profile_data[] =
{
    // 0x0001 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x0f, 0x18,
    // 0x0002 CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL-DYNAMIC | READ | NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x12, 0x03, 0x00, 0x19, 0x2a,
    // 0x0003 VALUE-ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL-DYNAMIC | READ | NOTIFY-''
    // READ_ANYBODY
    0x08, 0x00, 0x02, 0x01, 0x03, 0x00, 0x19, 0x2a,
    // 0x0004 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0a, 0x01, 0x04, 0x00, 0x02, 0x29, 0x00, 0x00,

// Test Service
    // 0x0005 PRIMARY_SERVICE-FF10
    0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x10, 0xff,
    // Test Characteristic, with read and notify
    // 0x0006 CHARACTERISTIC-FF11-READ | WRITE | NOTIFY | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x1a, 0x09, 0x00, 0xf1, 0xff,
    // 0x0007 VALUE-FF11-READ | WRITE | NOTIFY | DYNAMIC-''
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x09, 0x00, 0xf1, 0xff,
    // 0x0008 CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0a, 0x01, 0x0a, 0x00, 0x02, 0x29, 0x00, 0x00,

    // 0x0009 PRIMARY_SERVICE-FF12
    0x0a, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x00, 0x28, 0x12, 0xff,
    // 0x000a CHARACTERISTIC-FF13-NOTIFY | DYNAMIC
    0x0d, 0x00, 0x02, 0x00, 0x0c, 0x00, 0x03, 0x28, 0x10, 0x0d, 0x00, 0x13, 0xff,
    // 0x000b VALUE-FF13-NOTIFY | DYNAMIC-''
    0x08, 0x00, 0x00, 0x01, 0x0d, 0x00, 0x13, 0xff,
    // 0x000c CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0a, 0x01, 0x0e, 0x00, 0x02, 0x29, 0x00, 0x00,

    // END
    0x00, 0x00,
}; // total size 99 bytes

const struct att_hdl_t att_hdl_tbl[BLE_IDX_END] = {
    [BLE_IDX_BATTERY]   = {0x0003,  1},
    [BLE_IDX_APP]   	= {0x0009,  1},
};

const u8 *ble_get_profile_data(void)
{
    return profile_data;
}

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const);
    memcpy(scan_buf, scan_data_const, data_len);

    //读取BLE配置的蓝牙名称
    int len;
    len = strlen(xcfg_cb.le_name);
    if (len > 0) {
        memcpy(&scan_buf[2], xcfg_cb.le_name, len);
        data_len = 2 + len;
        scan_buf[0] = len + 1;
    }
    return data_len;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    memset(adv_buf, 0, buf_size);
    u32 data_len = sizeof(adv_data_const);
    memcpy(adv_buf, adv_data_const, data_len);

    //读取BLE配置的蓝牙名称
//    int len;
//    len = strlen(xcfg_cb.le_name);
//    if (len > 0) {
//        memcpy(&adv_buf[5], xcfg_cb.le_name, len);
//        data_len = 5 + len;
//        adv_buf[3] = len + 1;
//    }

    return data_len;
}

//level:0~100
uint ble_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 100)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }
    uint bat_level = (sys_cb.vbat - bat_off) / ((4200 - bat_off) / 100);
//    printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
    return bat_level;
#else
    return 100;
#endif
}

void ble_init_att(void)
{
    u8 buffer[4];
//    buffer[0] = ble_get_bat_level();
//    ble_init_att_do(BLE_IDX_BATTERY, att_hdl_tbl[BLE_IDX_BATTERY].hdl, att_hdl_tbl[BLE_IDX_BATTERY].cfg, buffer, 1);   //初始化电量
    memset(&ble_cb, 0, sizeof(struct ble_cb_t));
#if BT_APP_EN
    bsp_app_init();
#endif
    memset(buffer, 0, 4);
    for (int i = 0; i < BLE_IDX_END; i++) {
        ble_init_att_do(i, att_hdl_tbl[i].hdl, att_hdl_tbl[i].cfg, buffer, 4);
    }
}

u8 ble_att_write_callback(u16 handle, u8 *ptr, u8 len)
{
    printf("BLE RX:");
    print_r(ptr, len);

    u8 wptr = ble_cb.cmd_wptr & BLE_CMD_BUF_MASK;
    ble_cb.cmd_wptr++;
    memcpy(ble_cb.cmd[wptr].buf, ptr, len);
    ble_cb.cmd[wptr].len = len;
    ble_cb.cmd[wptr].handle = handle;
    return true;
}

bool ble_send_packet(u8 *buf, u8 len)
{
    if (buf == NULL || len > 20) {
        return false;
    }
    static u32 ble_tick = 0;
    while (!tick_check_expire(ble_tick, 30)) {     //延时30ms再发
        delay_5ms(2);
    }
    ble_tick = tick_get();
    printf("BLE TX:");
    print_r(buf, len);
    return le_tx_notify(BLE_IDX_APP, buf, len);
}

void bsp_ble_process(void)
{
    if (ble_cb.cmd_rptr == ble_cb.cmd_wptr) {
        return;
    }
    u8 rptr = ble_cb.cmd_rptr & BLE_CMD_BUF_MASK;
    ble_cb.cmd_rptr++;
    u8 *ptr = ble_cb.cmd[rptr].buf;
    u8 len = ble_cb.cmd[rptr].len;
    u16 handle = ble_cb.cmd[rptr].handle;
    if (handle == 0x0009) {             //兼容旧版APP
        bt_app_cmd_process(ptr, len);
    }
}
#endif
