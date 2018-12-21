#include "include.h"
#include "func.h"
#include "func_bt.h"

#if FUNC_BT_EN

void sbc_decode_exit(void);
void mp3_res_play_kick(u32 addr, u32 len);

#if BT_HFP_RING_NUMBER_EN && !MUTIL_LANG_TBL_EN

const res_addr_t ring_num_tbl[10] = {
    {&RES_BUF_NUM_0_MP3, &RES_LEN_NUM_0_MP3},
    {&RES_BUF_NUM_1_MP3, &RES_LEN_NUM_1_MP3},
    {&RES_BUF_NUM_2_MP3, &RES_LEN_NUM_2_MP3},
    {&RES_BUF_NUM_3_MP3, &RES_LEN_NUM_3_MP3},
    {&RES_BUF_NUM_4_MP3, &RES_LEN_NUM_4_MP3},
    {&RES_BUF_NUM_5_MP3, &RES_LEN_NUM_5_MP3},
    {&RES_BUF_NUM_6_MP3, &RES_LEN_NUM_6_MP3},
    {&RES_BUF_NUM_7_MP3, &RES_LEN_NUM_7_MP3},
    {&RES_BUF_NUM_8_MP3, &RES_LEN_NUM_8_MP3},
    {&RES_BUF_NUM_9_MP3, &RES_LEN_NUM_9_MP3},
};

const res_addr_t *res_get_ring_num(u8 index)
{
    return &ring_num_tbl[index];
}
#endif // BT_HFP_RING_NUMBER_EN

#if WARNING_BT_INCALL
struct {
    u8 buf[25];
    u8 len;
    u8 cur;
    s8 gain_offset;
    u32 ring_tick;
} f_bt_ring;

AT(.text.func.btring)
void hfp_get_bnip_number(char *buf, u32 len)
{
//    printf("num:%s, len:%d\n",buf, len);
    if (f_bt_ring.len == 0) {
        u32 i;
        for(i = 0; i < len; i++){
            if (buf[i] >= '0' && buf[i] <= '9') {
                f_bt_ring.buf[i] = buf[i] - 0x30;
            } else {
                f_bt_ring.buf[i] = 0xff;
            }
        }
        f_bt_ring.cur = 0;
        f_bt_ring.len = len;
    }
}

AT(.text.func.btring)
static void sfunc_bt_ring_res_play(u32 addr, u32 len)
{
//    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0) {
        return;
    }
    if (sys_cb.mute) {
        bsp_loudspeaker_unmute();
        delay_5ms(6);
    }
    mp3_res_play_kick(addr, len);
}
#endif

#if BT_HSF_CALL_NUMBER_EN
AT(.text.func.btring)
bool hfp_is_list_curr_calls(void)
{
    return true;
}

AT(.text.func.btring)
void hfp_hf_emit_curr_calls(u8 idx, u8 type, const char * number, u32 len){
    printf("number: %s\n", number);
}
#endif

static void sfunc_bt_ring_process(void)
{
    func_process();

    func_bt_status();

#if WARNING_BT_INCALL
    if (bt_is_ring() && get_music_dec_sta() == MUSIC_STOP && f_bt.disp_status == BT_STA_INCOMING) {
        music_control(MUSIC_MSG_STOP);
        if (sys_cb.mute) {
            bsp_loudspeaker_mute();
        }
        if (bt_hfp_ring_number_en() && f_bt_ring.cur < f_bt_ring.len) {
#if BT_HFP_RING_NUMBER_EN
            if (f_bt_ring.buf[f_bt_ring.cur] != 0xff) {
                const res_addr_t *p = res_get_ring_num(f_bt_ring.buf[f_bt_ring.cur]);
                sfunc_bt_ring_res_play(*p->ptr, *p->len);
            }
            f_bt_ring.cur++;
            f_bt_ring.ring_tick = tick_get();
#endif // BT_HFP_RING_NUMBER_EN
        } else {
            if (tick_check_expire(f_bt_ring.ring_tick, 2500)) {
                sfunc_bt_ring_res_play(RES_BUF_RING_MP3, RES_LEN_RING_MP3);
                f_bt_ring.ring_tick = tick_get();
            }
        }
    }
#endif
}

static void sfunc_bt_ring_enter(void)
{
#if WARNING_BT_INCALL
    memset(&f_bt_ring, 0, sizeof(f_bt_ring));
    f_bt_ring.gain_offset = sys_cb.anl_gain_offset;
    dac_set_anl_offset(0);
    bt_audio_bypass();
    bsp_change_volume(WARNING_VOLUME);
#endif // WARNING_BT_INCALL
}

static void sfunc_bt_ring_exit(void)
{
#if WARNING_BT_INCALL
    music_control(MUSIC_MSG_STOP);
    sys_cb.anl_gain_offset = f_bt_ring.gain_offset;
    bsp_change_volume(sys_cb.vol);
    bt_audio_enable();
#endif
}

AT(.text.func.btring)
void sfunc_bt_ring(void)
{
    printf("%s\n", __func__);

    func_process();
    sfunc_bt_ring_enter();

    while ((f_bt.disp_status == BT_STA_INCOMING) && (func_cb.sta == FUNC_BT)) {
        sfunc_bt_ring_process();
        sfunc_bt_ring_message(msg_dequeue());
        func_bt_display();
    }

    sfunc_bt_ring_exit();
}

#endif //FUNC_BT_EN
