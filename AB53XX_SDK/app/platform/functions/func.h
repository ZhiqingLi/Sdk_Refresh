#ifndef _FUNC_H
#define _FUNC_H

#include "func_bt.h"
#include "func_bt_hid.h"
#include "func_clock.h"
#include "func_idle.h"
#include "func_music.h"
#include "func_usbdev.h"
#include "func_aux.h"
#include "func_speaker.h"
#include "func_spdif.h"
#include "sfunc_record.h"
#include "sfunc_record_play.h"
#include "func_lowpwr.h"
#include "func_fmrx.h"
#include "func_fmam_freq.h"
#include "func_update.h"

//task number
enum {
    FUNC_NULL = 0,
    FUNC_MUSIC,
    FUNC_FMRX,
    FUNC_CLOCK,
    FUNC_BT,
    FUNC_BTHID,
    FUNC_USBDEV,
    FUNC_AUX,
    FUNC_SPDIF,
    FUNC_FMAM_FREQ,
    FUNC_SPEAKER,
    FUNC_PWROFF,
    FUNC_SLEEPMODE,
    FUNC_IDLE,
    FUNC_EXSPIFLASH_MUSIC,
};

//task control block
typedef struct {
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
#if BT_BACKSTAGE_EN
    u8 sta_break;                                   //被中断的任务
#endif
    void (*mp3_res_play)(u32 addr, u32 len);        //各任务的语音播报函数接口
    void (*set_vol_callback)(u8 dir);               //设置音量的回调函数，用于各任务的音量事件处理。
} func_cb_t;
extern func_cb_t func_cb;

extern const u8 func_sort_table[];     //任务切换排序table

u8 get_funcs_total(void);
void func_process(void);
void func_message(u16 msg);

void func_run(void);
void func_music(void);
void func_idle(void);
void func_clock(void);
void func_fmrx(void);
void func_bt(void);
void func_bthid(void);
void func_usbdev(void);
void func_aux(void);
void func_exspifalsh_music(void);
#if FUNC_SPDIF_EN
void func_spdif(void);
#endif // FUNC_SPDIF_EN

#if FUNC_FMAM_FREQ_EN
void func_fmam_freq(void);
#endif // FUNC_FMAM_FREQ_EN

#if FUNC_SPEAKER_EN
void func_speaker(void);
#endif // FUNC_SPEAKER_EN

void func_bt_warning(void);
void func_bt_disp_status(void);

#endif // _FUNC_H
