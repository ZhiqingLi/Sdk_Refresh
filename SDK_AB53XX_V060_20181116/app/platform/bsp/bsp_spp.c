#include "include.h"

extern u8 eq_rx_buf[EQ_BUFFER_LEN];
const u16 bt_key_msg_tbl[6] = {
    KU_PLAY,
    KU_PREV,
    KU_NEXT,
    KU_VOL_DOWN,
    KU_VOL_UP,
    KU_MODE,
};
#if BT_SCO_DBG_EN
void bsp_aec_ack_param(void)
{
    u8 buf[11];
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = f_bt.mic_anl_gain;
    buf[4] = f_bt.mic_dig_gain;
    buf[5] = (u8)f_bt.noise;
    buf[6] = (u8)(f_bt.noise >> 8);
    buf[7] = f_bt.aec_level;
    buf[8] = f_bt.aec_offset;
    bt_spp_tx(buf, 11);
}

void bsp_aec_ack(void)
{
    u8 buf[6];
    buf[0] = 'a';
    buf[1] = 'e';
    buf[2] = 'c';
    buf[3] = ' ';
    buf[4] = 'o';
    buf[5] = 'k';
    bt_spp_tx(buf, 6);
}
#endif

void bsp_app_0_process(u8 *ptr, u16 size)
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
#if BT_SCO_DBG_EN
    if (ptr[0] == 'a' && ptr[1] == 'e' && ptr[2] == 'c') {
        u8 cnt, i;
        if (ptr[3] == '?') {
            bsp_aec_ack_param();
            return;
        }
        cnt = 3;
        for (i = 0; i < 5; i++) {
            u16 value;
            if (i == 2) {   //第3个参数2Byte
                value = ptr[cnt++];
                value |= ptr[cnt++] << 8;
            } else {
                value = ptr[cnt++];
            }
            sco_audio_set_param(i, value);
        }
        bsp_aec_ack();
        return;
    }
#endif

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
    bsp_app_0_process(packet, size);
#endif // BT_SPP_EN
}
