#include "include.h"
#include "func.h"
#include "func_bt.h"

void sbc_decode_exit(void);

AT(.text.func.bt.msg)
void func_bt_message(u16 msg)
{
    if (xcfg_cb.bt_test_redial_en) {
        if (bt_is_testmode() && ((msg & KEY_TYPE_MASK) == KEY_DOUBLE)) {   //测试模式下，所有双击都响应回拨
            if (bt_get_status() >= BT_STA_CONNECTED) {
                bt_call_redial_last_number();
                return;
            }
        }
    }
    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_POWER:
    case KU_PLAY_MODE:
        sys_cb.mute = 0;
        bt_music_play_pause();
        break;

    case KU_PREV_VOL_DOWN:
    case KL_VOL_DOWN_PREV:
    case KD_VOL_NEXT_PREV:
    case KU_PREV:
        sys_cb.mute = 0;
        bt_music_prev();
        break;

    case KU_NEXT_VOL_UP:
    case KL_VOL_UP_NEXT:
    case KL_VOL_NEXT_PREV:
    case KU_NEXT:
        sys_cb.mute = 0;
        bt_music_next();
        break;
    case KL_PREV:
        bt_music_rewind();
        break;
    case KLU_PREV:
        sys_cb.mute = 0;
        bt_music_rewind_end();
        break;
    case KL_NEXT:
        bt_music_fast_forward();
        break;
    case KLU_NEXT:
        sys_cb.mute = 0;
        bt_music_fast_forward_end();
        break;
//    case KL_PLAY:
//        sys_cb.mute = 0;
//        bt_disconnect();
//        break;

    case KD_HSF:
    case KD_PLAY:
    case KD_POWER:
    case KD_PLAY_POWER:
    case KD_PLAY_MODE:
        sys_cb.mute = 0;
        if (bt_get_status() >= BT_STA_CONNECTED) {
            bt_call_redial_last_number();           //回拨电话
            func_cb.mp3_res_play(RES_BUF_REDIALING_MP3, RES_LEN_REDIALING_MP3);
#if BT_TWS_EN
        } else if(xcfg_cb.bt_tws_en && xcfg_cb.bt_tws_pair_mode == 1) {
            wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV);  //TWS开始连接提示音
            bt_tws_search_slave();
#endif
        } else {
            func_bt_switch_voice_lang();            //切换提示音语言
        }
        break;

#if BT_TWS_EN
    case KL_MODE:
        if(xcfg_cb.bt_tws_en && xcfg_cb.bt_tws_pair_mode == 2) {
            if(bt_tws_is_connected()) {
                bt_tws_disconnect();
            } else if(!bt_is_connected()) {
                wav_res_play(RES_BUF_MAX_VOL_WAV, RES_LEN_MAX_VOL_WAV); //TWS开始连接提示音
                bt_tws_search_slave();
            }
        }
        break;
#endif

    case KD_VOL_DOWN:
        func_cb.sta = FUNC_BTHID;
        break;

    case KD_VOL_UP_NEXT:
    case KD_NEXT_VOL_UP:
#if BT_HID_MENU_EN
        bsp_bt_hid_tog_conn();
#endif
        break;

    case KD_PREV_VOL_DOWN:
    case KD_VOL_DOWN_PREV:
        sys_cb.mute = 0;
        bsp_bt_hid_photo();
        break;

    case MSG_SYS_1S:
        bt_send_msg(BT_MSG_HFP_REPORT_BAT);
        break;

    case EVT_A2DP_MUSIC_PLAY:
        dac_fade_in();
        break;

    case EVT_A2DP_MUSIC_STOP:
        dac_fade_out();
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.btring.msg)
void sfunc_bt_ring_message(u16 msg)
{
    switch (msg) {
//    case KU_PLAY:
    case KU_HSF:                //接听
    case KU_PLAY_POWER:
    case KU_PLAY_MODE:
        sys_cb.mute = 0;
        bt_call_answer_incoming();
        break;

    case KD_PLAY:
    case KL_HSF:
    case KD_HSF:
    case KL_PLAY_POWER:
    case KD_PLAY_POWER:
        sys_cb.mute = 0;
        //长按挂断电话，无论长按多久不响应关机功能。
        if (msg == KL_PLAY_POWER) {
            sys_cb.poweron_flag = 1;    //PWRKEY松开前不产生KHL_PLAY_POWER消息。按键松开自动清此标志。
        }
        bt_call_terminate();    //挂断
        func_cb.mp3_res_play(RES_BUF_CALL_REJECT_MP3, RES_LEN_CALL_REJECT_MP3);
        break;

    case MSG_SYS_1S:
        bt_send_msg(BT_MSG_HFP_REPORT_BAT);
        break;

    default:
        func_message(msg);
        break;
    }
}

void sfunc_bt_call_message(u16 msg)
{
    u8 call_status;
    switch (msg) {
    case KU_PLAY:
    case KU_HSF:
    case KU_PLAY_POWER:
    case KU_PLAY_MODE:
        sys_cb.mute = 0;
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_answer_incoming();  //接听第2路通话
        } else {
            bt_call_terminate();        //挂断当前通话
            func_cb.mp3_res_play(RES_BUF_CALL_HANGUP_MP3, RES_LEN_CALL_HANGUP_MP3);
        }
        break;

    case KL_PLAY:
        sys_cb.mute = 0;
#if BT_HFP_CALL_PRIVATE_EN
        if (xcfg_cb.bt_hfp_private_en) {
            bt_call_private_switch();
        }
#endif // BT_HFP_CALL_PRIVATE_EN
        break;

    case KL_HSF:
        sys_cb.mute = 0;
        call_status = bt_get_call_indicate();
        if(call_status == BT_CALL_INCOMING) {
            bt_call_terminate();        //拒接第2路通话
        } else if(call_status == BT_CALL_3WAY_CALL) {
            bt_call_swap();             //切换两路通话
        }
        break;

    case KL_NEXT_VOL_UP:
    case KH_NEXT_VOL_UP:
    case KU_VOL_UP_NEXT:
    case KU_VOL_UP:
    case KL_VOL_UP:
    case KH_VOL_UP:
        if (sys_cb.hfp_vol < 15) {
            sys_cb.hfp_vol++;
            bt_send_msg(BT_MSG_HFP_SPK_GAIN);
            bsp_change_volume(sys_cb.hfp_vol * sys_cb.hfp2sys_mul);
        }
        gui_box_show_vol();
        break;

    case KL_PREV_VOL_DOWN:
    case KH_PREV_VOL_DOWN:
    case KU_VOL_DOWN_PREV:
    case KU_VOL_DOWN:
    case KL_VOL_DOWN:
    case KH_VOL_DOWN:
        if (sys_cb.hfp_vol > 0) {
            sys_cb.hfp_vol--;
            bt_send_msg(BT_MSG_HFP_SPK_GAIN);
            bsp_change_volume(sys_cb.hfp_vol * sys_cb.hfp2sys_mul);
        }
        gui_box_show_vol();
        break;

    case EVT_HFP_SET_VOL:
        bsp_change_volume(sys_cb.hfp_vol * sys_cb.hfp2sys_mul);
        gui_box_show_vol();
        break;

    case MSG_SYS_1S:
        bt_send_msg(BT_MSG_HFP_REPORT_BAT);
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.bt.msg)
void func_bthid_message(u16 msg)
{
    switch (msg) {
    case KU_PLAY:
    case KU_PLAY_POWER:
    case KU_PLAY_MODE:
        if (bt_get_status() < BT_STA_DISCONNECTING) {
            bt_connect();
        } else {
            bsp_bt_hid_photo();
            mp3_res_play(RES_BUF_TAKE_PHOTO_MP3, RES_LEN_TAKE_PHOTO_MP3);
        }
        break;

    case KL_PLAY:
        //bt_disconnect();
        break;

    case KD_VOL_DOWN:
        func_cb.sta = FUNC_BT;
        break;

    default:
        func_message(msg);
        break;
    }
}
