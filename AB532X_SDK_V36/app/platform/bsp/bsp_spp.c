#include "include.h"

extern u8 eq_rx_buf[EQ_BUFFER_LEN];

static const u16 bt_key_msg_tbl[] = {
    KU_PLAY,
    KU_PREV,
    KU_NEXT,
    KU_VOL_DOWN,
    KU_VOL_UP,
    KU_MODE,
};

void bt_app_cmd_process(u8 *ptr, u16 size)
{
#if EQ_DBG_IN_SPP
    if (xcfg_cb.eq_dgb_spp_en) {
        eq_spp_cb_t *p = &eq_dbg_cb.eq_spp_cb;
        if (ptr[0] == 'E' && ptr[1] == 'Q') {       //EQ消息
            if (ptr[2] == '?') {
                memcpy(eq_rx_buf, ptr, size);
                msg_enqueue(EVT_ONLINE_SET_EQ);
                return;
            }
            u32 rx_size = little_endian_read_16(ptr, 4) + 6;
            if (size < rx_size) {
                memcpy(eq_rx_buf, ptr, size);
                p->remain = 1;
                p->remian_ptr = size;
                p->rx_size = rx_size;
            }
            return;
        }
        if (p->remain) {
            memcpy(&eq_rx_buf[p->remian_ptr], ptr, size);
            p->remian_ptr += size;
            if (p->rx_size == p->remian_ptr) {
                msg_enqueue(EVT_ONLINE_SET_EQ);
                memset(p, 0, sizeof(eq_spp_cb_t));
            }
            return;
        }
    }
#endif
    printf("SPP RX:");
    print_r(ptr, size);
    switch (ptr[0]) {
        case 0x01:
            //system control
            if (size != 4) {
                return;     //2byte data + 2byte crc16
            }
            if ((ptr[1] > 0) && (ptr[1] <= sizeof(bt_key_msg_tbl))) {
                msg_enqueue(bt_key_msg_tbl[ptr[1] - 1]);
            }
            break;

        case 0x02:
            //rgb leds control
            if (size != 7) {
                return;     //5byte data + 2byte crc16
            }
#if PWM_RGB_EN
            if (ptr[1] > 0) {
                pwm_rgb_write(ptr[2], ptr[3], ptr[4]);
            } else {
                pwm_rgb_close();
            }
#endif // PWM_RGB_EN
            break;

        case 0x03:
            //ota control
            break;
    }
}

void spp_rx_callback(uint8_t *packet, uint16_t size)
{
#if BT_SPP_EN
    bt_app_cmd_process(packet, size);
#endif // BT_SPP_EN
}
