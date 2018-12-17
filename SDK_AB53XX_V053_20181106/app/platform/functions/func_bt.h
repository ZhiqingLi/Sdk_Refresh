#ifndef _FUNC_BT_H
#define _FUNC_BT_H

#define BT_WARN_CON             BIT(0)
#define BT_WARN_HID_CON         BIT(1)
#define BT_WARN_TWS_MCON        BIT(2)
#define BT_WARN_TWS_SCON        BIT(3)
#define BT_WARN_DISCON          BIT(4)
#define BT_WARN_HID_DISCON      BIT(5)
#define BT_WARN_PAIRING         BIT(6)
#define BT_WARN_TBOX_BUZZER     BIT(7)

enum {
    AEC_PARAM_MIC_ANL_GAIN,
    AEC_PARAM_MIC_DIG_GAIN,
    AEC_PARAM_NOISE,
    AEC_PARAM_LEVEL,
    AEC_PARAM_OFFSET,
};

typedef struct{
    u8 disp_status;
    u8 warning_status;
    u8 hid_menu_flag;
    u8 hid_discon_flag;
    u8 tws_status;
#if BT_SCO_DBG_EN
    u8 mic_anl_gain;
    u8 mic_dig_gain;
    u8 aec_level;
    u8 aec_offset;
    u16 noise;
#endif
    u8 rec_pause    : 1;
    u8 pp_2_unmute  : 1;
    u8 need_pairing : 1;

} func_bt_t;

extern func_bt_t f_bt;

void sfunc_bt_ring(void);
void sfunc_bt_ring_message(u16 msg);

void sfunc_bt_call(void);
void sfunc_bt_call_message(u16 msg);

void sfunc_bt_ota(void);

void func_bt_status(void);
void func_bt_message(u16 msg);
void func_bt_mp3_res_play(u32 addr, u32 len);
void func_bt_switch_voice_lang(void);
bool func_bt_chkclr_warning(u8 bits);

void bt_sco_rec_init(void);
void bt_music_rec_init(void);

void sco_audio_set_param(u8 type, u16 value);

#if (GUI_SELECT != GUI_NO)
void func_bt_display(void);
void func_bt_enter_display(void);
void func_bt_exit_display(void);
#else
#define func_bt_display()
#define func_bt_enter_display()
#define func_bt_exit_display()
#endif

#endif // _FUNC_BT_H
