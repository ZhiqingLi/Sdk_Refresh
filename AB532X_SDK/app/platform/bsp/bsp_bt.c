#include "include.h"
#include "api.h"


#define BT_PROFILE          (PROF_A2DP*BT_A2DP_EN) | (PROF_HFP*BT_HFP_EN) | (PROF_SPP*BT_SPP_EN) | (PROF_HID*BT_HID_EN)
#define A2DP_FEATURE        (A2DP_VOL_CTRL*BT_A2DP_VOL_CTRL_EN) | (A2DP_RECON_CTRL*BT_A2DP_RECON_EN)
#define HFP_FEATURE         (HFP_BAT_REPORT*BT_HFP_BAT_REPORT_EN) | (HFP_IOS_RING*BT_HFP_PLAY_IOS_RING_EN) | (HFP_CALL_PRIVATE*BT_HFP_CALL_PRIVATE_FORCE_EN)

bt_cfg_t bt_cfg = {
    .pwrup_connect_times = BT_POWER_UP_RECONNECT_TIMES,
    .tout_connect_times = BT_TIME_OUT_RECONNECT_TIMES,
    .scan_timeout       = BT_DISCOVER_TIMEOUT,      //待实现
    .profile            = BT_PROFILE,
    .max_acl_link       = BT_2ACL_EN+1,
    .a2dp_feature       = A2DP_FEATURE,
    .hfp_feature        = HFP_FEATURE,
    .vx_set             = 2,    //0=hv3, 1=ev3, 2=2ev3

    .res[0]             = 0,
    .res[1]             = 0,
    .dual_mode          = BT_DUAL_MODE_EN,
    .tws_mode           = BT_TWS_EN,
    .fcc_test_mode      = BT_FCC_TEST_EN,
    .cbt_test_mode      = BT_CBT_TEST_EN,
    .simple_pair_mode   = BT_SIMPLE_PAIR_EN,
    .scan_ctrl          = BT_DISCOVER_CTRL_EN,

    .sco_plc_en         = BT_PLC_EN,
    .sco_msbc_en        = BT_HFP_MSBC_EN,
};

rf_cfg_t rf_cfg;

const char bt_local_name[] = BT_NAME_DEFAULT;
const char bt_local_addr[6] = {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
const uint8_t bt_local_guid[16] = { 0x1f, 0xa5, 0xc8, 0xf8, 0x12, 0xef, 0x47, 0xb9,
                                    0x94, 0xfc, 0xbb, 0xd5, 0xc7, 0x19, 0xce, 0xfe };

#if 0
//是否支持android & ios播放暂停快速切换，需要时定义该函数
//注意：打开后ios播放微信小视频会无声，播放器听音乐不影响
//      部分android4.0及之前系统打开多个播放器可能有兼容问题
bool bt_force_super_fast_status(void)
{
    return true;
}
#endif

#if 0
//是否支持ios播放暂停快速切换，需要时定义该函数
//注意：打开后ios播放微信小视频会无声，播放器听音乐不影响
bool bt_support_ios_fast_status(void)
{
    return true;
}
#endif

////tws配对后，是否只允许固定的主副箱连接
////需要重新配对时，调用bt_tws_delete_link_info()删除配对信息
//bool bt_tws_is_chk_link(void)
//{
//    return true;
//}

//是否强制设为tws slave不允许被手机连接
//bool bt_tws_slave_dis_scan(void)
//{
//    return true;
//}

//u8 bt_tws_get_force_role(void)
//{
//    if(xcfg_cb.bt_tws_lr_mode == 3) {
//        return sys_cb.tws_left_channel | 0x80; //0x80=master, 0x81=slave
//    }
//    return 0;
//}

//设置search slave次数，每次9s（搜索5秒 + 空闲4秒）
//uint8_t bt_tws_get_search_slave_times(void)
//{
//    return 3;
//}

//自定义蓝牙图标，根据需要打开
//u32 bt_get_class_of_device(void)
//{
//    return 0x002540;    //键盘图标，Android带显示电量，IOS不行。全部IOS均可连接HID拍照。
//    return 0x240418;    //耳机图标，Android和IOS均带电量显示。IOS9之前的机型无法连接HID拍照。（默认使用）
//}

//HID兼容性选择，根据需要选择
//u8 bt_hid_get_compatibility(void)
//{
//    return 0;   //耳机图标（带电量显示），不影响键盘使用，使用V+拍照，影响部分Android拍照，体验较好（默认使用）
//    //return 1;   //耳机图标（带电量显示），影响ios键盘使用，使用V+和ENTER拍照，拍照兼容性好
//    //return 2;   //键盘图标（IOS不带电量显示），影响ios键盘使用，使用V+和ENTER拍照，拍照兼容性好
//}

//PIN配对码（最多16个字符），默认"0000"
//#if !BT_SIMPLE_PAIR_EN
//const char *bt_get_pin_code(void)
//{
//    return "0000";
//}
//#endif

//远距离断线回连是否自动恢复播放，根据需要选择（默认是true）
//bool bt_reconn_paly(void)
//{
//    return false;
//}

/************************使用示例************************/
////按键发起回拨号码
//bt_send_msg(BT_MSG_CALL_REDIAL_NUMBER);
//delay_5ms(10); //延迟一下，等它发送完毕
//
////此函数返回需要回拨的号码，例如“10086”
//char * hfp_get_outgoing_number(void) {
//    return "10086";
//}

////发送自定义ATCMD
//bt_send_msg(BT_MSG_HFP_AT_CMD);
//delay_5ms(10); //延迟一下，等它发送完毕
//
// //此函数返回需要发送的ATCMD
//char * hfp_get_at_cmd(void) {
//    return "AT+VTS=1;"; //例如，通话过程发起号码键"1"
//    return "ATD10086;"; //也可以，发起回拨号码"10086"
//}

////tws时，单独连接/断开手机
//    if(bt_nor_is_connected()) {
//        bt_nor_disconnect();
//    } else {
//        bt_nor_connect();
//    }
////tws时，单独连接/断开对箱
//    if(bt_tws_is_connected()) {
//        bt_tws_disconnect();
//    } else {
//        bt_tws_search_slave();
//    }
/********************************************************/

void bsp_bt_init(void)
{
    //更新配置工具的设置
    rf_cfg.txpwr   = xcfg_cb.bt_txpwr;
    rf_cfg.packet  = bt_get_packet(xcfg_cb.bt_rf_param);

    bt_cfg.profile = (PROF_A2DP*BT_A2DP_EN*xcfg_cb.bt_a2dp_en) | (PROF_HFP*BT_HFP_EN*xcfg_cb.bt_sco_en)\
                    | (PROF_SPP*BT_SPP_EN*(xcfg_cb.bt_spp_en||xcfg_cb.eq_dgb_spp_en)) | (PROF_HID*BT_HID_EN*xcfg_cb.bt_hid_en);
    bt_cfg.a2dp_feature = (A2DP_VOL_CTRL*BT_A2DP_VOL_CTRL_EN*xcfg_cb.bt_a2dp_vol_ctrl_en) | (A2DP_RECON_CTRL*BT_A2DP_RECON_EN);
    bt_cfg.dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
    bt_cfg.max_acl_link = BT_2ACL_EN * xcfg_cb.bt_2acl_en + 1;
#if BT_TWS_EN
    bt_cfg.tws_mode = BT_TWS_EN * xcfg_cb.bt_tws_en;
    if(xcfg_cb.bt_tws_pair_mode != 0) {
        bt_cfg.res[0] |= BT_FLAG0_TWS_SCAN;
        if(xcfg_cb.bt_tws_pair_mode == 3) {
            bt_cfg.res[0] |= BT_FLAG0_TWS_MENU_PAIR;
        }
    }
#if BT_TWS_M_RECON_EN
    bt_cfg.res[0] |= BT_FLAG0_TWS_M_RECON * xcfg_cb.bt_tws_recon_mode;
#endif
    if(bt_cfg.tws_mode && get_cur_sysclk() >= SYS_48M) {
        bt_cfg.res[0] |= BT_FLAG0_CLK_ADVANCE;
    }
    if(xcfg_cb.bt_tws_lr_mode == 3) {
        tws_lr_xcfg_sel();
    }
#endif

#if BT_FCC_TEST_EN
    bt_fcc_init();
#endif

#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        bt_cfg.profile = PROF_HID;
        bt_cfg.max_acl_link = 1;
        bt_cfg.dual_mode = 0;
        bt_cfg.tws_mode = 0;
    }
#endif // FUNC_BTHID_EN

    memset(&f_bt, 0, sizeof(func_bt_t));
    f_bt.disp_status = 0xfe;
    f_bt.need_pairing = 1;  //开机若回连不成功，需要播报pairing
    if (!is_bthid_mode()) {
        f_bt.hid_menu_flag = 1;
    }

    bt_setup();
}

void bsp_bt_close(void)
{
}

#if BT_PWRKEY_5S_DISCOVER_EN
bool bsp_bt_w4_connect(void)
{
    if (xcfg_cb.bt_pwrkey_nsec_discover) {
        while (sys_cb.pwrkey_5s_check) {            //等待检测结束
            WDT_CLR();
            delay_5ms(2);
        }

        //已检测到长按5S，需要直接进入配对状态。播放PAIRING提示音。
        if (sys_cb.pwrkey_5s_flag) {
            return false;
        }
    }
    return true;
}

bool bsp_bt_pwrkey5s_check(void)
{
    bool res = !bsp_bt_w4_connect();
    delay_5ms(2);
    return res;
}

void bsp_bt_pwrkey5s_clr(void)
{
    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
        return;
    }
    sys_cb.pwrkey_5s_flag = 0;
    bt_cfg.scan_ctrl = 0;
}
#endif // BT_PWRKEY_5S_DISCOVER_EN

void bsp_bt_vol_change(void)
{
#if BT_A2DP_VOL_CTRL_EN || BT_TWS_EN
    if((xcfg_cb.bt_a2dp_vol_ctrl_en && (bt_get_status() >= BT_STA_CONNECTED)) || bt_tws_is_connected()) {
        bt_send_msg(BT_MSG_VOL_CHANGE);     //通知手机音量已调整
    }
#endif
}

#if BT_HID_MENU_EN
void bsp_bt_hid_tog_conn(void)
{
    if (xcfg_cb.bt_hid_menu_en) {
        func_bt_chkclr_warning(BT_WARN_HID_CON|BT_WARN_HID_DISCON);
        f_bt.hid_menu_flag = 2;     //按键连接/断开HID，置上标志，用于播放提示音
        if (bt_hid_is_connected()) {
            bt_hid_disconnect();
        } else {
            bt_hid_connect();
        }
    }
}
#endif

void bsp_bt_hid_photo(void)
{
    bt_hid_key(HID_KEY_ENTER);              //enter key, android 4.0以上
    delay_5ms(10);
    bt_hid_consumer(HID_KEY_VOL_UP);        //consumer key vol_up, ios
}

////点击左边屏幕中心位置
//void bsp_bt_hid_screen_left(void)
//{
//    bt_hid_touch_screen(0);
//    delay_5ms(10);
//    bt_hid_touch_screen(1);
//}
//
////点击右边屏幕中心位置
//void bsp_bt_hid_screen_right(void)
//{
//    bt_hid_touch_screen(0x11);
//    delay_5ms(10);
//    bt_hid_touch_screen(0x10);
//}

/*****************************************************************************
 * 回调函数
 *****************************************************************************/
uint a2dp_get_vol(void)
{
    uint vol;

    vol = ((u32)sys_cb.vol * 1280L /VOL_MAX) /10;
    if(vol > 0x7f) {
        vol = 0x7f;
    }

    return vol;
}

uint hfp_get_mic_gain(void)
{
    return 9;
}

uint hfp_get_spk_gain(void)
{
    return sys_cb.hfp_vol;
}

uint hfp_get_bat_level(void)
{
#if VBAT_DETECT_EN
    //计算方法：level = (实测电压 - 关机电压) / ((满电电压 - 关机电压) / 10)
    u16 bat_off = LPWR_OFF_VBAT * 100 + 2700;
    if (bat_off > sys_cb.vbat) {
        return 0;
    }
    uint bat_level = (sys_cb.vbat - bat_off) / ((4200 - bat_off) / 10);
    if (bat_level) {
        bat_level--;
    }
//    printf("bat level: %d %d\n", sys_cb.vbat, bat_level);
    return bat_level;
#else
    return 9;
#endif
}

//tws一些同步信息，例如EQ, 语言等，param最大是12byte
bool bt_get_tws_info(uint8_t *param)
{
#if LANG_SELECT == LANG_EN_ZH
    param[0] = sys_cb.lang_id & 0x0f;  //bit0~3
#endif
#if EQ_MODE_EN
    param[0] |= (u8)sys_cb.eq_mode<<4;
#endif
    return true;
}

void bt_set_tws_info(uint8_t *param)
{
    uint8_t tmp = 0;
#if LANG_SELECT == LANG_EN_ZH
    tmp = (param[0] & 0x0f);
    if(xcfg_cb.lang_id >= 2 && tmp < 2 && tmp != sys_cb.lang_id) {  //bit0~3
        sys_cb.lang_id = tmp;
        msg_enqueue(EVT_BT_SET_LANG_ID);
    }
#endif
#if EQ_MODE_EN
    tmp = (param[0] & 0xf0)>>4;
    if(tmp < 6 && tmp != sys_cb.eq_mode) {  //bit4~7
        sys_cb.eq_mode = tmp;
        msg_enqueue(EVT_BT_SET_EQ);
    }
#endif
    tmp = tmp;      //避免编译警告
}

void tbox_notice(u8 *param);
void bt_emit_notice(uint evt, u32 param)
{
    switch(evt) {
    case BT_NOTICE_INIT_FINISH:
#if BT_PWRKEY_5S_DISCOVER_EN
        if(bsp_bt_w4_connect()) {
            bt_connect();
        } else {
            bt_cfg.scan_ctrl = 1;
            bt_set_scan(true);          //长按5S开机，直接进bt scanning
        }
#else
        bt_connect();
#endif
        break;
    case BT_NOTICE_DISCONNECT:
#if BT_TWS_EN
        if(bt_cfg.tws_mode) {
            if(param & 0xc0) {
                f_bt.tws_status &= ~0xc0;
                break;                      //tws断线不播报提示音
            } else {
                f_bt.tws_status &= ~0x01;
            }
        }
#endif
        f_bt.warning_status |= BT_WARN_DISCON;
        delay_5ms(5);
        break;
    case BT_NOTICE_CONNECTED:
#if BT_TWS_EN
        if(bt_cfg.tws_mode) {
            if(param & 0x80) {
                f_bt.tws_status |= 0x80;
                f_bt.warning_status |= BT_WARN_TWS_SCON;
            } else if(param & 0x40) {
                f_bt.tws_status |= 0x40;
                f_bt.warning_status |= BT_WARN_TWS_MCON;
            } else {
                f_bt.tws_status |= 0x01;
                f_bt.warning_status |= BT_WARN_CON;
            }
        } else
#endif
        {
            f_bt.warning_status |= BT_WARN_CON;
        }
#if BT_PWRKEY_5S_DISCOVER_EN
        bsp_bt_pwrkey5s_clr();
#endif // BT_PWRKEY_5S_DISCOVER_EN
        delay_5ms(5);
        break;
    case BT_NOTICE_LOSTCONNECT:
    case BT_NOTICE_INCOMING:
    case BT_NOTICE_RING:
    case BT_NOTICE_OUTGOING:
    case BT_NOTICE_CALL:
        break;
    case BT_NOTICE_SET_SPK_GAIN:
        if(param != sys_cb.hfp_vol) {
            sys_cb.hfp_vol = param;
            msg_enqueue(EVT_HFP_SET_VOL);
        }
        break;

    case BT_NOTICE_MUSIC_PLAY:
        msg_enqueue(EVT_A2DP_MUSIC_PLAY);
        break;

    case BT_NOTICE_MUSIC_STOP:
        if (bt_get_disp_status() > BT_STA_PLAYING) {
            break;
        }
        msg_enqueue(EVT_A2DP_MUSIC_STOP);
        break;

    case BT_NOTICE_MUSIC_CHANGE_VOL:
        if(param == 0) {
            msg_enqueue(KU_VOL_DOWN);
        } else {
            msg_enqueue(KU_VOL_UP);
        }
        break;

    case BT_NOTICE_TWS_HID_SHUTTER:
        msg_enqueue(EVT_TWS_HID_SHUTTER);
        break;

    case BT_NOTICE_TWS_INFO:
        bt_set_tws_info((u8 *)param);
        break;

    case BT_NOTICE_TWS_SET_VOL:
        param = (param+1) * VOL_MAX / 128;
        sys_cb.vol = param;
        if (sys_cb.vol > VOL_MAX) {
            sys_cb.vol = VOL_MAX;
        }
        msg_enqueue(EVT_TWS_SET_VOL);
        break;

    case BT_NOTICE_MUSIC_SET_VOL:
        param = (param+1) * VOL_MAX / 128;
        if(param != sys_cb.vol) {
            sys_cb.vol = param;
            if (sys_cb.vol > VOL_MAX) {
                sys_cb.vol = VOL_MAX;
            }
            msg_enqueue(EVT_A2DP_SET_VOL);
        }
        break;
    case BT_NOTICE_HID_CONN_EVT:
#if BT_HID_MENU_EN
        if (xcfg_cb.bt_hid_menu_en) {
            if (f_bt.hid_menu_flag == 2) {
                //按键连接/断开HID Profile完成
                if (param) {
                    f_bt.warning_status |= BT_WARN_HID_CON;
                } else {
                    f_bt.warning_status |= BT_WARN_HID_DISCON;
                }
                f_bt.hid_menu_flag = 1;
            }
    #if BT_HID_DISCON_DEFAULT_EN
            else if (f_bt.hid_menu_flag == 1) {
                if ((param) & (xcfg_cb.bt_hid_discon_default_en)) {
                    f_bt.hid_discon_flag = 1;
                }
            }
    #endif // BT_HID_DISCON_DEFAULT_EN
        }
#endif // BT_HID_MENU_EN
        break;
    }
}

uint8_t bt_get_func_sta(void)
{
    if(func_cb.sta == FUNC_PWROFF) {
        return FUNC_PWROFF;
    }
    return 0xff;
}

bool bt_set_func_sta(uint8_t func_sta)
{
    if(func_sta != 0xff) {
        sys_cb.pwrdwn_tone_en = 1;
        func_cb.sta = FUNC_PWROFF;
    }
    return true;
}

const char *bt_get_local_name(void)
{
#if 1
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        return xcfg_cb.bthid_name;
    }
#endif // FUNC_BTHID_EN
    return xcfg_cb.bt_name;
#else
    return bt_local_name;
#endif // 1

}

void bt_get_local_bd_addr(u8 *addr)
{
#if BT_LOCAL_ADDR
    param_random_key_read(&addr[2]);
    addr[0] = 0x41;
    addr[1] = 0x42;
#else
    memcpy(addr, xcfg_cb.bt_addr, 6);
#endif
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        addr[5] ^= BIT(0);
    }
#endif // FUNC_BTHID_EN
}

u32 bt_get_rand_seed(void)
{
    return sys_cb.rand_seed;
}

WEAK const uint8_t *bt_get_rf_param(void)
{
    return NULL;
}

void bt_get_guid(uint8_t *buf, uint8_t buf_len)
{
    const char *ptr = bt_get_local_name();
    u16 name_len = strlen(ptr);
    if(name_len > 10) {
        name_len = 10;
    }
    if(name_len > buf_len) {
        name_len = buf_len;
    }
    memcpy(buf, bt_local_guid, buf_len);
    memcpy(buf, ptr, name_len);
}

void bt_get_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_read(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        cm_read(buf, BT_CM_PAGE(addr), size);
    }
    //print_r(buf, size);
}

void bt_put_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    //print_r(buf, size);
#if FUNC_BTHID_EN
    if (is_bthid_mode()) {
        cm_write(buf, BTHID_CM_PAGE(addr), size);
    } else
#endif // FUNC_BTHID_EN
    {
        cm_write(buf, BT_CM_PAGE(addr), size);
    }
}

void bt_get_ext_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    cm_read(buf, EXT_CM_PAGE(addr), size);
    //print_r(buf, size);
}

void bt_put_ext_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    //print_r(buf, size);
    cm_write(buf, EXT_CM_PAGE(addr), size);
}

void bt_sync_link_info(void)
{
    cm_sync();
}

#if BT_FCC_TEST_EN
ALIGNED(4)
u8 huart_buffer[512];

void huart_init(void)
{
    huart_init_do(HUART_TR_PB3, HUART_TR_PB3, 1500000, huart_buffer, 512);
}
#endif
