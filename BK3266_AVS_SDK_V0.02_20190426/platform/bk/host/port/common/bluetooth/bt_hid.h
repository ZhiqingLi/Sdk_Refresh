/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef __BT_HID_H__
#define __BT_HID_H__

#include <bluetooth.h>
#include <bt_l2cap_proto.h> /* For L2CAP header size */
#include <bt_unit.h>        /* For ACL header size */


#define HID_HANDSHAKE           0
#define HID_CONTROL                1
#define HID_GET_REPORT          4
#define HID_SET_REPORT          5
#define HID_GET_PROTOCOL     6
#define HID_SET_PROTOCOL     7
#define HID_GET_IDLE              8
#define HID_SET_IDLE              9

#define HID_DATA                    10
#define HID_DATC                    11
#define KBD_NUMLOCK         (0x01 << 0x00)
#define KBD_CAPSLOCK        (0x01 << 0x01)
#define KBD_SCROLLLOCK      (0x01 << 0x02)

typedef enum
{
    hid_control_op_nop = 0,
    hid_control_op_hard_reset = 1,
    hid_control_op_soft_reset = 2,
    hid_control_op_suspend = 3,
    hid_control_op_exit_suspend = 4,
    hid_control_op_unplug = 5
} hid_control_op;
typedef enum
{
    hid_data_other = 0,
    hid_data_input = 1,
    hid_data_output = 2,
    hid_data_feature = 3
} hid_data_type;

typedef enum
{
    HID_INT_CHANNEL = 0,
    HID_CTRL_CHANNEL = 1
} hid_chan_type;

void hid_uninit(void);
result_t hid_init(void);
result_t send_hid_data(uint8_t *,int,uint8_t);
result_t sendHidKey_press(void);
result_t sendHidKey_releas(void);
void hid_con_connected(uint8 flag);
result_t hid_int_conn_disconnect(void);
result_t hid_ctrl_conn_disconnect(void);
void Set_photoKey(void);

#endif /* __BT_HID_H__ */
