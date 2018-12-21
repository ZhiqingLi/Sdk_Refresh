#include "include.h"
#include "bsp_app.h"
#include "bsp_ble.h"
#include "func_fmrx.h"

#if BT_APP_EN

#define TRACE_EN                1
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FIL_NAME_LEN            150

struct app_cb_t app_cb;
char fname[FIL_NAME_LEN];

extern const u16 bt_key_msg_tbl[6];

const u8 app_func_tbl[4] = {
    FUNC_BT,
    FUNC_AUX,
    FUNC_FMRX,
    FUNC_MUSIC,
};

AT(.text.app.pkt)
u8 bsp_app_check_sum(u8 *buf, u8 len)
{
    u8 sum = 0;
    for (int i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

AT(.text.app.pkt)
bool bsp_app_send_packet(u8 cmd, u8 *buf, u8 len)
{
    if (buf == NULL || len > 16) {
        return false;
    }
    app_cb.tx_buf[0] = 0xAB;        //Header
    app_cb.tx_buf[1] = 1 + len;     //CMD + DAT
    app_cb.tx_buf[2] = cmd;
    if (len) {
        memcpy(&app_cb.tx_buf[3], buf, len);
    }
    app_cb.tx_buf[3 + len] = bsp_app_check_sum(app_cb.tx_buf, 3 + len);

    return ble_send_packet(app_cb.tx_buf, len + 4);
}

static u8 bsp_app_get_func(u8 func)
{
    u8 i;
    u8 res = 0xff;
    for (i = 0; i < sizeof(app_func_tbl); i++) {
        if (func == app_func_tbl[i]) {
            if (func == FUNC_MUSIC) {
                if (sys_cb.cur_dev <= DEV_SDCARD1) {
                    res = APP_FUNC_MSC_SD;
                } else if (sys_cb.cur_dev == DEV_UDISK) {
                    res = APP_FUNC_MSC_USB;
                }
            } else {
                res = i;
            }
            return res;
        }
    }
    return res;
}

struct sys_feature_t {
    u32 music_en    : 1;    //BIT(0)
    u32 aux_en      : 1;    //BIT(1)
    u32 fm_en       : 1;    //BIT(2)
    u32 clock_en    : 1;    //BIT(3)
    u32 light_en    : 1;    //BIT(4)
    u32 fm_rec_en   : 1;    //BIT(5)
    u32 aux_rec_en  : 1;    //BIT(6)
    u32 bt_rec_en   : 1;    //BIT(7)
    u32 vioce_en    : 1;    //BIT(8)
};

static struct sys_feature_t sys_feature = {
    .music_en   = FUNC_MUSIC_EN,
    .aux_en     = FUNC_AUX_EN,
    .fm_en      = FUNC_FMRX_EN,
    .clock_en   = FUNC_CLOCK_EN,
    .light_en   = LE_LIGHTING_EN,
    .fm_rec_en  = FMRX_REC_EN,
    .aux_rec_en = AUX_REC_EN,
    .bt_rec_en  = BT_REC_EN,
    .vioce_en   = 0,
};

AT(.text.app.sys)
void bsp_app_sys_init(void)
{
    u8 buf[7];
    memcpy(buf, &sys_feature, 4);
    buf[4] = bsp_app_get_func(func_cb.sta);
    buf[5] = VOL_MAX;
    buf[6] = sys_cb.vol;
    bsp_app_send_packet(APP_CMD_INIT_STA, buf, sizeof(buf));
}

AT(.text.app.sys)
void bsp_app_func_send(u8 func)
{
    u8 buf[2];
    if (func == FUNC_NULL) {
        return;
    }
    buf[0] = CMD_SYS_MODE;
    buf[1] = bsp_app_get_func(func);
    bsp_app_send_packet(APP_CMD_SYS_CTL, buf, sizeof(buf));
}

AT(.text.app.au)
void bsp_app_audio_ctl(u8 sub_cmd, u8 dat)
{
    u8 buf[2];
    buf[0] = sub_cmd;
    buf[1] = dat;
    bsp_app_send_packet(APP_CMD_AUDIO_CTL, buf, sizeof(buf));
}

AT(.text.app.fm)
void bsp_app_fm_send_ch(u8 sub_cmd, u8 ch)
{
    u8 buf[2];
    buf[0] = sub_cmd;
    buf[1] = ch;
    bsp_app_send_packet(APP_CMD_FM_CTL, buf, sizeof(buf));
}

AT(.text.app.fm)
void bsp_app_fm_send_freq(u16 freq)
{
    u8 buf[3];
    buf[0] = CMD_FM_CUR_FREQ;
    buf[1] = freq;
    buf[2] = freq >> 8;
    bsp_app_send_packet(APP_CMD_FM_CTL, buf, sizeof(buf));
}

AT(.text.app.fm)
void bsp_app_fm_seek(u8 en)
{
    u8 buf[2];
    buf[0] = CMD_FM_SEEK;
    buf[1] = en;
    bsp_app_send_packet(APP_CMD_FM_CTL, buf, sizeof(buf));
}

AT(.text.app.fm)
void bsp_app_fm_send_list(void)
{
    u8 buf[16], len = 0, i, ch = 1, cnt, ch_cnt = fmrx_cb.ch_cnt;
    u8 remain = 1, first = 1;
    if (fmrx_cb.ch_cnt == 0) {
        return;
    }
    buf[0] = CMD_FM_LIST;
    while (remain) {
        if (first) {
            first = 0;
            buf[1] = CMD_STA_TX_START;
            if (ch_cnt <= 6) {
                buf[1] |= CMD_STA_TX_END;   //一帧搞定
            }
        } else {
            if (ch_cnt <= 6) {
                buf[1] = CMD_STA_TX_END;
            } else {
                buf[1] = CMD_STA_TX_ING;
            }
        }
        if (ch_cnt > 6) {
            buf[2] = 6;
            len = 3 + 6 * 2;
            cnt = 6;
            remain = 1;
            ch_cnt -= 6;
        } else {
            buf[2] = ch_cnt;
            len = 3 + ch_cnt * 2;
            cnt = ch_cnt;
            remain = 0;
        }
        u16 *ptr = (u16 *)&buf[3];
        for (i = 0; i < cnt; i++, ch++) {
            ptr[i] = get_ch_freq(ch);
//            TRACE("get[%d]:%x\n", ch, ptr[i]);
        }
        bsp_app_send_packet(APP_CMD_FM_CTL, buf, len);
    }
}

AT(.text.app.fm)
void bsp_app_fm_send_info(void)
{
    u8 buf[7];
    memset(buf, 0, sizeof(buf));
    buf[0] = CMD_FM_INFO;
    buf[1] = fmrx_cb.ch_cnt;
    buf[2] = fmrx_cb.ch_cur;
    buf[3] = fmrx_cb.freq;
    buf[4] = fmrx_cb.freq >> 8;
    if (fmrx_cb.sta == FMRX_PAUSE) {
        buf[5] = 1;
    } else if (fmrx_cb.sta == FMRX_PLAY) {
        buf[5] = 2;
    } else if (fmrx_cb.sta == FMRX_SEEKING) {
        buf[5] = 3;
    }
#if FMRX_REC_EN
    if (sfunc_is_recording()) {
        buf[6] = 1;
    }
#endif
    bsp_app_send_packet(APP_CMD_FM_CTL, buf, sizeof(buf));
}

AT(.text.app.msc)
void bsp_app_msc_send_dev_info(void)
{
    u8 buf[4];
    buf[0] = CMD_MSC_DEV_STA;
    buf[1] = dev_is_online(DEV_UDISK);
    buf[2] = dev_is_online(DEV_SDCARD) | dev_is_online(DEV_SDCARD1);
    buf[3] = dev_is_online(DEV_LINEIN);
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

static u16 str_len_unicode(char *buf)
{
    u16 len = 0;
    while (1){
        if (((*buf) == 0) && ((*(buf + 1)) == 0)) {
            break;
        }
        buf += 2;
        len += 2;
    }
    return len;
}

AT(.text.app.msc)
void bsp_app_msc_get_name_for_num(u16 num)
{
    u8 buf[16], len = 0, str_len, ptr = 0;;
    u8 remain = 1, first = 1;
    if (num == 0) {
        return;
    }
    memset(fname, 0, FIL_NAME_LEN);
    if (!fs_get_filename_for_number(num, 1, fname, FIL_NAME_LEN)) {
        TRACE("get fname err\n");
        return;
    }
    str_len = str_len_unicode(fname);
#if 0//TRACE_EN
    TRACE("get fname[%d]:\n", num);
    print_r(fname, FIL_NAME_LEN);
#endif

    buf[0] = CMD_MSC_LIST;
    buf[2] = fname[FIL_NAME_LEN - 1] ? 0 : 1;
    buf[3] = num;
    buf[4] = num >> 8;
    while (remain) {
        if (first) {
            first = 0;
            buf[1] = CMD_STA_TX_START;
            if (str_len <= 11) {
                buf[1] |= CMD_STA_TX_END;   //一帧搞定
            }
        } else {
            if (str_len <= 11) {
                buf[1] = CMD_STA_TX_END;
            } else {
                buf[1] = CMD_STA_TX_ING;
            }
        }
        if (str_len > 11) {
            len = 11;
            remain = 1;
            str_len -= 11;
        } else {
            len = str_len;
            remain = 0;
        }
        memcpy(&buf[5], &fname[ptr], len);
        ptr += len;
        bsp_app_send_packet(APP_CMD_MSC_CTL, buf, len + 5);
    }
}

AT(.text.app.msc)
void bsp_app_msc_info(void)
{
    u8 buf[11];
    u16 sec;
    buf[0] = CMD_MSC_INFO;
    buf[1] = f_msc.file_num;
    buf[2] = f_msc.file_num >> 8;
    buf[3] = f_msc.file_total;
    buf[4] = f_msc.file_total >> 8;
    sec = f_msc.curtime.min * 60 + f_msc.curtime.sec;
    buf[5] = sec;
    buf[6] = sec >> 8;
    sec = f_msc.alltime.min * 60 + f_msc.alltime.sec;
    buf[7] = sec;
    buf[8] = sec >> 8;
    buf[9] = f_msc.pause ? 1 : 2;
    if (sys_cb.play_mode > SINGLE_MODE) {
        buf[10] = 2;
    } else {
        buf[10] = sys_cb.play_mode;
    }
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

AT(.text.app.msc)
void bsp_app_msc_cur_time(void)
{
    u8 buf[7];
    static u16 all_sec = 0;
    u16 sec = f_msc.curtime.min * 60 + f_msc.curtime.sec;
    buf[0] = CMD_MSC_CUR_TIME;
    buf[1] = sec;
    buf[2] = sec >> 8;
    if (f_msc.alltime.min != 0xff) {
        all_sec = f_msc.alltime.min * 60 + f_msc.alltime.sec;
    }
    buf[3] = all_sec;
    buf[4] = all_sec >> 8;
    buf[5] = f_msc.file_num;
    buf[6] = f_msc.file_num >> 8;
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

AT(.text.app.sys)
void bsp_app_msc_play_sta(u8 paly, u8 vol)
{
    u8 buf[4];
    memset(buf, 0, sizeof(buf));
    buf[0] = CMD_MSC_PLAY_STA;
    buf[1] = paly;
    if (paly == 0) {
        buf[2] = 1;
        buf[3] = vol;
    }
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

AT(.text.app.sys)
void bsp_app_msc_num(void)
{
    u8 buf[3];
    buf[0] = CMD_MSC_PLAY_NUM;
    buf[1] = f_msc.file_num;
    buf[2] = f_msc.file_num >> 8;
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

AT(.text.app.sys)
void bsp_app_msc_circle_mode(void)
{
    u8 buf[3];
    buf[0] = CMD_MSC_CIRCLE_MODE;
    buf[1] = sys_cb.play_mode;
    bsp_app_send_packet(APP_CMD_MSC_CTL, buf, sizeof(buf));
}

AT(.text.app.msc)
void bsp_app_msc_set_play_sta(u8 sta)
{
    u8 pause = sta - 1;
    if (func_cb.sta == FUNC_MUSIC) {
        f_msc.pause = pause;
    } else if (func_cb.sta == FUNC_AUX) {
        f_aux.pause = pause;
    }
    msg_enqueue(KU_PLAY);
}

AT(.text.app.aux)
void bsp_app_aux_play_info(void)
{
    u8 buf[3];
    buf[0] = CMD_AUX_PLAY_INFO;
    if (f_aux.pause) {
        buf[1] = 1;
    } else {
        buf[1] = 2;
    }
    buf[2] = 0;
#if AUX_REC_EN
    if (sfunc_is_recording()) {
        buf[2] = 1;
    }
#endif
    bsp_app_send_packet(APP_CMD_AUX_CTL, buf, sizeof(buf));
}

AT(.text.app.rec)
void bsp_app_rec_start(u8 rec, u8 start)
{
    u8 buf[2];
    if (rec) {
        buf[0] = CMD_REC_START;
    } else {
        buf[0] = CMD_REC_PLAY_START;
    }
    buf[1] = start;
    bsp_app_send_packet(APP_CMD_REC_CTL, buf, sizeof(buf));
}

AT(.text.app.rec)
void bsp_app_rec_play_time(u8 rec, u32 time)
{
    u8 buf[5];
    if (rec) {
        buf[0] = CMD_REC_TIME;
    } else {
        buf[0] = CMD_REC_PLAY_TIME;
    }
    buf[1] = time;
    buf[2] = time >> 8;
    buf[3] = time >> 16;
    buf[4] = time >> 24;
    bsp_app_send_packet(APP_CMD_REC_CTL, buf, sizeof(buf));
}

AT(.text.app.au)
void bsp_app_audio_proc(u8 *ptr, u8 len)
{
    if (ptr[3] == 0) {   //音乐
        switch (ptr[4]) {
           #if SYS_KARAOK_EN
            case CMD_MSC_VOL:
                sys_cb.music_vol = ptr[5];
                msg_enqueue(EVT_MUSIC_VOL);
                break;
            #endif

            #if SYS_EQ_FOR_IDX_EN
            case CMD_MSC_BASS_VOL:
                music_set_eq_for_index(8, ptr[5] - 12);
                break;
            #endif

            #if SYS_EQ_FOR_IDX_EN
            case CMD_MSC_TREB_VOL:
                music_set_eq_for_index(9, ptr[5] - 12);
                break;
            #endif

            #if EQ_MODE_EN
            case CMD_MSC_EQ_TYPE:
                sys_cb.eq_mode = ptr[5];
                music_set_eq_by_num(sys_cb.eq_mode);
                gui_box_show_eq();
                break;
            #endif

            #if SYS_EQ_FOR_IDX_EN
            case CMD_MSC_EQ:
                music_set_eq_for_gains((s8*)&ptr[5]);
                break;
            #endif

            default:
                break;
        }
    } else {        //MIC

    }
}

#if FUNC_FMRX_EN
AT(.text.app.fm)
void bsp_app_fm_proc(u8 *ptr, u8 len)
{
    switch (ptr[3]) {
        case CMD_FM_ALL_CH:
            bsp_app_fm_send_ch(CMD_FM_ALL_CH, fmrx_cb.fcnt);
            break;
        case CMD_FM_CUR_CH:
            if (fmrx_cb.ch_cur == ptr[4]) {
                break;
            }
            fmrx_cb.ch_cur = ptr[4];
            fmrx_set_curch_freq();
            break;
        case CMD_FM_CUR_FREQ:
            fmrx_cb.freq = ptr[5] << 8 | ptr[4];
            bsp_fmrx_set_freq(fmrx_cb.freq);
            dac_fade_in();
            break;
        case CMD_FM_SEEK:
            bsp_clr_mute_sta();
            if (fmrx_cb.sta == FMRX_PAUSE) {
                func_fmrx_pause_play();
            }
            if (fmrx_cb.sta == FMRX_PLAY) {
                fmrx_cb.sta = FMRX_SEEK_START;
            } else {
                fmrx_cb.sta = FMRX_SEEK_STOP;
            }
            break;
        case CMD_FM_LIST:
            bsp_app_fm_send_list();
            break;
        case CMD_FM_INFO:
            bsp_app_fm_send_info();
            break;
        default:
            break;
    }
}
#endif

AT(.text.app.sys)
void bsp_app_sys_proc(u8 *ptr, u8 len)
{
    switch (ptr[3]) {
        case CMD_SYS_MODE:
            if (ptr[4] < APP_FUNC_MSC_USB) {
                func_cb.sta = app_func_tbl[ptr[4]];
            } else {
                if (ptr[4] == APP_FUNC_MSC_SD) {
                    func_cb.sta = FUNC_MUSIC;
                    if (sys_cb.cur_dev == DEV_UDISK) {
                        sys_cb.cur_dev = DEV_SDCARD;
                        f_msc.dev_change = 1;
                    }
                } else if (ptr[4] == APP_FUNC_MSC_USB) {
                    func_cb.sta = FUNC_MUSIC;
                    if (sys_cb.cur_dev == DEV_SDCARD) {
                        sys_cb.cur_dev = DEV_UDISK;
                        f_msc.dev_change = 1;
                    }
                }
            }
            break;
        default:
            break;
    }
}

AT(.text.app.msc)
void bsp_app_msc_proc(u8 *ptr, u8 len)
{
    switch (ptr[3]) {
        case CMD_MSC_LIST:
            app_cb.msc_list_cnt = app_cb.msc_list_start = ptr[5] << 8 | ptr[4];
            app_cb.msc_list_end = ptr[7] << 8 | ptr[6];
            if ((app_cb.msc_list_cnt == 0) && (app_cb.msc_list_end == 0)) {
                app_cb.msc_list_sta = 0;
            } else {
                app_cb.msc_list_sta = 1;    //start
            }
            break;
        case CMD_MSC_DEV_STA:
            bsp_app_msc_send_dev_info();
            break;
        case CMD_MSC_INFO:
            bsp_app_msc_info();
            break;
        case CMD_MSC_PLAY_STA:
            if (ptr[4]) {
                bsp_app_msc_set_play_sta(ptr[4]);
            }
            if (ptr[5]) {
                bsp_set_volume(ptr[6]);
            }
            break;
        case CMD_MSC_CIRCLE_MODE:
            if (ptr[4] == 3) {
                sys_cb.play_mode = RANDOM_MODE;
            } else {
                sys_cb.play_mode = ptr[4];
            }
            break;
        case CMD_MSC_PLAY_NUM:
            f_msc.file_num = ptr[5] << 8 | ptr[4];
            f_msc.file_change = 1;
            break;
        default:
            break;
    }
}

AT(.text.app.aux)
void bsp_app_aux_proc(u8 *ptr, u8 len)
{
    switch (ptr[3]) {
        case CMD_AUX_PLAY_INFO:
            bsp_app_aux_play_info();
            break;
        default:
            break;
    }
}

AT(.text.app.rec)
void bsp_app_rec_proc(u8 *ptr, u8 len)
{
    if (ptr[3] == CMD_REC_START) {
        if ((ptr[4] && !sfunc_is_recording()) || (!ptr[4] && sfunc_is_recording())) {
            msg_enqueue(KU_REC);
        }
    }
}

AT(.text.app)
void bsp_app_process(u8 *ptr, u8 len)
{
    if (ptr[0] != 0xAB) {
        return;
    }
#if TRACE_EN
    printf("CMD[%d]: ", ptr[2]);
    print_r(&ptr[3], ptr[1]-1);
#endif
    switch (ptr[2]) {
        case APP_CMD_INIT_STA:
            bsp_app_sys_init();
            break;

        case APP_CMD_SYS_CTL:
            bsp_app_sys_proc(ptr, len);
            break;

        case APP_CMD_AUDIO_CTL:
            bsp_app_audio_proc(ptr, len);
            break;

        #if FUNC_FMRX_EN
        case APP_CMD_FM_CTL:
            bsp_app_fm_proc(ptr, len);
            break;
        #endif

        case APP_CMD_MSC_CTL:
            bsp_app_msc_proc(ptr, len);
            break;

        #if FUNC_REC_EN
        case APP_CMD_REC_CTL:
            bsp_app_rec_proc(ptr, len);
            break;
        #endif

        #if FUNC_AUX_EN
        case APP_CMD_AUX_CTL:
            bsp_app_aux_proc(ptr, len);
            break;
        #endif

        default:
            break;
    }
}

AT(.text.app.init)
void bsp_app_init(void)
{
    memset(&app_cb, 0, sizeof(struct app_cb_t));
}

#endif
