#include "include.h"
#include "bsp_eq.h"

#define EQ_CRC_SEED                             0xffff
#define EQ_BAND_NUM                             12
#define CAL_FIX(x)		                        ((int)(x * (1 << 27)))

uint calc_crc(void *buf, uint len, uint seed);
int eq_huart_putcs(uint8_t *packet, uint16_t len);
void huart_eq_init(u8 *rx_buf, uint rx_len);
void huart_rx_kick(void);

extern const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN];

AT(.text.music)
void music_set_eq_by_num(u8 num)
{
    if (num > (MUSIC_EQ_TBL_LEN - 1)) {
        return;
    }
    if (music_set_eq_is_done()) {
        return;
    }
    sys_cb.eq_mode_set = 0;
    music_set_eq_by_res(music_eq_tbl[num].addr, music_eq_tbl[num].len);
}

u8 eq_rx_buf[EQ_BUFFER_LEN];
#if EQ_MODE_EN

AT(.text.music)
void sys_set_eq(void)
{
    sys_cb.eq_mode++;
    if (sys_cb.eq_mode > 5) {
        sys_cb.eq_mode = 0;
    }
    music_set_eq_by_num(sys_cb.eq_mode);
    gui_box_show_eq();
#if BT_APP_EN
    bsp_app_audio_ctl(CMD_MSC_EQ_TYPE, sys_cb.eq_mode);
#endif
}
#endif // EQ_MODE_EN

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
eq_dbg_cb_t eq_dbg_cb;
static u8 eq_tx_buf[12];
const char tbl_eq_header[4] = "EQ0#";
const char tbl_eq_version[10] = {'E', 'Q', '0', '*', 6, 0, 12, 4, 5, 0};

static void tx_ack(uint8_t *packet, uint16_t len)
{
    delay_5ms(1);   //延时一段时间再ack
    if (eq_dbg_cb.rx_type) {
#if EQ_DBG_IN_UART
        eq_huart_putcs(packet, len);
#endif // EQ_DBG_IN_UART
    } else {
#if EQ_DBG_IN_SPP && BT_SPP_EN
        if (xcfg_cb.eq_dgb_spp_en && (bt_get_status() >= BT_STA_CONNECTED)) {
            bt_spp_tx(packet, len);
        }
#endif // EQ_DBG_IN_SPP
    }
    eq_dbg_cb.rx_type = 0;
}

void eq_tx_ack(u8 bank_num, u8 ack)
{
    eq_tx_buf[0] = 'A';
    eq_tx_buf[1] = bank_num;
    eq_tx_buf[2] = ack;
    eq_tx_buf[3] = -(eq_tx_buf[0] + eq_tx_buf[1] + eq_tx_buf[2]);
    tx_ack(eq_tx_buf, 4);
}

void eq_tx_version(void)
{
//    eq_tx_buf[0] = 'E';
//    eq_tx_buf[1] = 'Q';
//    eq_tx_buf[2] = '0';
//    eq_tx_buf[3] = '*';
//    eq_tx_buf[4] = 6;     //size
//    eq_tx_buf[5] = 0;
//    eq_tx_buf[6] = 12;    //band_cnt
//    eq_tx_buf[7] = 4;     //param size
//    eq_tx_buf[8] = 5;     //param cnt
//    eq_tx_buf[9] = 0;     //rsvd
    memcpy(eq_tx_buf, tbl_eq_version, 10);
    u16 crc = calc_crc(eq_tx_buf, 10, EQ_CRC_SEED);
    eq_tx_buf[10] = crc;
    eq_tx_buf[11] = crc >> 8;
    tx_ack(eq_tx_buf, 12);
}

void eq_parse_cmd(void)
{
    if (eq_rx_buf[0] != 'E' || eq_rx_buf[1] != 'Q') {
        return;
    }
    if (eq_rx_buf[2] == '?' && eq_rx_buf[3] == '#') {
        eq_tx_version();
        return;
    }
//    print_r(eq_rx_buf, EQ_BUFFER_LEN);
    u8 band_num = eq_rx_buf[6];
    u32 size = little_endian_read_16(eq_rx_buf, 4);
    u32 crc = calc_crc(eq_rx_buf, size+4, EQ_CRC_SEED);
    if (crc == little_endian_read_16(eq_rx_buf, 4+size)) {
        eq_tx_ack(band_num, 0);
    } else {
        eq_tx_ack(band_num, 1);
        return;
    }

//    if (memcmp(tbl_eq_header, eq_rx_buf, sizeof(tbl_eq_header)) != 0) {
//        return;
//    }
    u8 band_cnt = eq_rx_buf[7];

    music_set_eq(band_cnt, (u32 *)&eq_rx_buf[14]);

#if (UART0_PRINTF_SEL != PRINTF_NONE)
    printf("%08x\n", little_endian_read_32(eq_rx_buf, 14));
    u8 k;
    u32 *ptr = (u32 *)&eq_rx_buf[18];
    for (k = 0; k < band_cnt*5; k++) {
        printf("%08x", *ptr++);
        if (k % 5 == 4) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
#endif

    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
}

#if EQ_DBG_IN_UART

AT(.com_text.eq)
void huart_rx_done(void)
{
    msg_enqueue(EVT_ONLINE_SET_EQ);
    eq_dbg_cb.rx_type = 1;
}

AT(.com_text.eq)
void huart_rx_set_dir(void)
{
    if (xcfg_cb.eq_uart_sel == 0) {
        GPIOADIR |= BIT(7);
    } else if (xcfg_cb.eq_uart_sel == 1) {
        GPIOBDIR |= BIT(2);
    } else if (xcfg_cb.eq_uart_sel == 2) {
        GPIOBDIR |= BIT(3);
    }
}

AT(.com_text.eq)
void huart_tx_set_dir(void)
{
    if (xcfg_cb.eq_uart_sel == 0) {
        GPIOADIR &= ~BIT(7);
    } else if (xcfg_cb.eq_uart_sel == 1) {
        GPIOBDIR &= ~BIT(2);
    } else if (xcfg_cb.eq_uart_sel == 2) {
        GPIOBDIR &= ~BIT(3);
    }
}

void eq_dbg_huart_init(void)
{
    FUNCMCON0 = 0x0f << 20 | 0x0f << 16;
    if (xcfg_cb.eq_uart_sel == 0) {
        if (UART0_PRINTF_SEL == PRINTF_PA7) {
            FUNCMCON0 = 0x0f << 8;
        }
        FUNCMCON0 = 0x01 << 20 | 0x01 << 16;
        GPIOADE |= BIT(7);
        GPIOAFEN |= BIT(7);
    } else if (xcfg_cb.eq_uart_sel == 1) {
        if (UART0_PRINTF_SEL == PRINTF_PB2) {
            FUNCMCON0 = 0x0f << 8;
        }
        FUNCMCON0 = 0x02 << 20 | 0x02 << 16;
        GPIOBDE |= BIT(2);
        GPIOBFEN |= BIT(2);
    } else if (xcfg_cb.eq_uart_sel == 2) {
        if (UART0_PRINTF_SEL == PRINTF_PB3) {
            FUNCMCON0 = 0x0f << 8;
        }
        FUNCMCON0 = 0x03 << 20 | 0x03 << 16;
        GPIOBDE |= BIT(3);
        GPIOBFEN |= BIT(3);
    }
    huart_eq_init(eq_rx_buf, EQ_BUFFER_LEN);
    huart_rx_set_dir();
    huart_rx_kick();
}
#endif

void eq_dbg_init(void)
{
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);
    memset(&eq_dbg_cb, 0, sizeof(eq_dbg_cb_t));
#if EQ_DBG_IN_UART
    if (xcfg_cb.eq_dgb_uart_en) {
        eq_dbg_huart_init();
    }
#endif // EQ_DBG_IN_UART

}
#endif

#if !EQ_DBG_IN_UART
void huart_rx_done(void) {}
void huart_rx_set_dir(void) {}
void huart_tx_set_dir(void) {}
#endif

#if SYS_BASS_TREBLE_EN || SYS_EQ_FOR_IDX_EN
const int tbl_coef_1[10] = {
	CAL_FIX(0.9999902462),    //Band:0(31Hz)
	CAL_FIX(0.9999597162),    //Band:1(63Hz)
	CAL_FIX(0.9997401793),    //Band:2(160Hz)
	CAL_FIX(0.9983764898),    //Band:3(400Hz)
	CAL_FIX(0.9898674722),    //Band:4(1000Hz)
	CAL_FIX(0.8419530754),    //Band:5(4000Hz)
	CAL_FIX(0.4177699622),    //Band:6(8000Hz)
	CAL_FIX(-0.6509365173),   //Band:7(16000Hz)

    CAL_FIX(0.9990866674),    //BASS:300Hz
    CAL_FIX(0.9100351062),    //TREB:3000Hz
};

//Q:0.750000
const int tbl_coef_0[10] = {
	CAL_FIX(0.0029444916),    //Band:0(31Hz)
	CAL_FIX(0.0059839058),    //Band:1(63Hz)
	CAL_FIX(0.0151961090),    //Band:2(160Hz)
	CAL_FIX(0.0379729998),    //Band:3(400Hz)
	CAL_FIX(0.0946628813),    //Band:4(1000Hz)
	CAL_FIX(0.3597005044),    //Band:5(4000Hz)
	CAL_FIX(0.6057018917),    //Band:6(8000Hz)
	CAL_FIX(0.5060881129),    //Band:7(16000Hz)

    CAL_FIX(0.0284864965),    //BASS:300Hz
    CAL_FIX(0.2763541250),    //TREB:3000Hz
};

struct eq_coef_tbl_t bass_treb_coef_tbl = {
    .coef_0 = &tbl_coef_0[8],
    .coef_1 = &tbl_coef_1[8],
};

void *bass_treble_get_coef_tbl(void)
{
    return &bass_treb_coef_tbl;
}
#endif

#if SYS_BASS_TREBLE_EN
int mic_coef[11];
int music_coef[11];

//参数默认44.1kHz
void bsp_bass_treble_init(void)
{
    mic_coef[0] = 0x800000;     //衰减0dB
    bass_treble_coef_cal(&mic_coef[1], 0, 0);
    bass_treble_coef_cal(&mic_coef[6], 0, 1);

    music_coef[0] = 0x800000;   //衰减0dB
    bass_treble_coef_cal(&music_coef[1], 0, 0);
    bass_treble_coef_cal(&music_coef[6], 0, 1);
}

//设置MIC高低音
void mic_bass_treble_set(int mode, int gain)
{
    if (mode == 0) {    //bass
        bass_treble_coef_cal(&mic_coef[1], gain, 0);
    } else if (mode == 1) {      //treble
        bass_treble_coef_cal(&mic_coef[6], gain, 1);
    }
    mic_set_eq(2, (u32 *)mic_coef);
}

//设置MUSIC高低音（会影响其他段EQ，不能边调EQ边调高低音）
void music_bass_treble_set(int mode, int gain)
{
    if (mode == 0) {    //bass
        bass_treble_coef_cal(&music_coef[1], gain, 0);
    } else if (mode == 1) {      //treble
        bass_treble_coef_cal(&music_coef[6], gain, 1);
    }
    music_set_eq(2, (u32 *)music_coef);

#if BT_APP_EN
    if (mode == 0) {
        bsp_app_audio_ctl(CMD_MSC_BASS_VOL, gain + 12);
    } else if (mode == 1) {
        bsp_app_audio_ctl(CMD_MSC_TREB_VOL, gain + 12);
    }
#endif
}

const u32 tbl_bass_coef[5] = {
    0x0082f2b6, 0x007bc6d1, 0x00feaf2f, 0xff0150d2, 0xff81467a,     //125Hz, 15dB
};

//库调用，把EQ的第9条Band当作低音，返回NULL，则采用资源的EQ表
const u32 *music_eq_get_bass_tbl(void)
{
    return NULL;
}

//库调用，把EQ的第10条Band当作高音，返回NULL，则采用资源的EQ表
const u32 *music_eq_get_treble_tbl(void)
{
    return NULL;
}

//库调用，把MIC EQ的第5条Band当作低音，返回NULL，则采用资源的EQ表
const u32 *mic_eq_get_bass_tbl(void)
{
    return NULL;
}

//库调用，把MIC EQ的第6条Band当作高音，返回NULL，则采用资源的EQ表
const u32 *mic_eq_get_treble_tbl(void)
{
    return NULL;
}

////设置MUSIC高低音（通过设置系统EQ来设置BASS和TREB，系统EQ不能为空
////不会影响其他段EQ，适合又调EQ，又调高低音的用法）
//void music_bass_treble_set(int mode, int gain)
//{
//    if (mode == 0) {    //bass
//        bass_treble_coef_cal(&music_coef[1], gain, 0);
//    } else if (mode == 1) {      //treble
//        bass_treble_coef_cal(&music_coef[6], gain, 1);
//    }
//    music_set_eq_by_num(sys_cb.eq_mode);
//}
//
////库调用，把EQ的第9条Band当作低音，返回NULL，则采用资源的EQ表
//const u32 *music_eq_get_bass_tbl(void)
//{
//    return (u32 *)&music_coef[1];
//}
//
////库调用，把EQ的第10条Band当作高音，返回NULL，则采用资源的EQ表
//const u32 *music_eq_get_treble_tbl(void)
//{
//    return (u32 *)&music_coef[6];
//}
#endif

//10条EQ独立调节
#if SYS_EQ_FOR_IDX_EN
int eq_coef[50+1];

struct eq_coef_tbl_t eq_coef_tbl = {
    .coef_0 = tbl_coef_0,
    .coef_1 = tbl_coef_1,
};

//根据EQ号来设置EQ[9:0]
void music_set_eq_for_index(u8 index, int gain)
{
    if (index > 9 || gain < -12 || gain > 12) {
        return;
    }
    if (music_set_eq_is_done()) {
        return;
    }
    if (index == 8) {           //bass
        bass_treble_coef_cal(&eq_coef[40 + 1], gain, 0);
    } else if (index == 9) {    //treble
        bass_treble_coef_cal(&eq_coef[45 + 1], gain, 1);
    } else {                    //eq
        eq_coef_cal(index, &eq_coef[index * 5 + 1], gain, &eq_coef_tbl);
    }
    sys_cb.eq_mode_set = 1;
    music_set_eq(10, (u32 *)eq_coef);

#if BT_APP_EN
    if (index == 8) {
        bsp_app_audio_ctl(CMD_MSC_BASS_VOL, gain + 12);
    } else if (index == 9) {
        bsp_app_audio_ctl(CMD_MSC_TREB_VOL, gain + 12);
    } else {

    }
#endif
}

//一次性设置8条EQ[7:0]
void music_set_eq_for_gains(s8 gain[8])
{
    if (music_set_eq_is_done()) {
        return;
    }
    for (int i = 0; i < 8; i++) {
        eq_coef_cal(i, &eq_coef[i * 5 + 1], gain[i], &eq_coef_tbl);
    }
    sys_cb.eq_mode_set = 1;
    music_set_eq(10, (u32 *)eq_coef);
}

//库调用，EQ[0~7]条band独立调试
const u32 *music_eq_get_tbl(u8 index)
{
    if (sys_cb.eq_mode_set && index < 8) {
        return (u32 *)&eq_coef[index * 5 + 1];
    } else {
        return NULL;
    }
}

//库调用，把EQ的第9条Band当作低音
const u32 *music_eq_get_bass_tbl(void)
{
    return (u32 *)&eq_coef[40 + 1];
}

//库调用，把EQ的第10条Band当作高音
const u32 *music_eq_get_treble_tbl(void)
{
    return (u32 *)&eq_coef[45 + 1];
}
#endif

void bsp_eq_init(void)
{
#if SYS_BASS_TREBLE_EN
    bsp_bass_treble_init();
#endif

#if SYS_EQ_FOR_IDX_EN
    eq_coef[0] = 0x800000;          //衰减0dB
    for (int i = 0; i < 10; i++) {
        if (i == 8) {           //bass
            bass_treble_coef_cal(&eq_coef[40 + 1], 0, 0);
        } else if (i == 9) {    //treble
            bass_treble_coef_cal(&eq_coef[45 + 1], 0, 1);
        } else {                    //eq
            eq_coef_cal(i, &eq_coef[i * 5 + 1], 0, &eq_coef_tbl);
        }
    }
#endif
}
