#include "applib.h"
#include "common_func.h"
#include "key_alexa_led.h"

#if defined(ENABLE_ALEXA_LED_NOTIFY)
typedef struct __alexa_led_status {
    uint32 led_notifaction_active: 1;
    uint32 led_alarm_active: 1;
    uint32 led_account_valid: 1;
    uint32 resverd: 29;
} ALEXA_LED_STATUS;

static ALEXA_LED_STATUS g_alexa_led_status;
#endif

#ifdef SMART_BOX_OPUS_MODE
void lp_update_alexa_state(uint8 e_type, uint8 e_state);

void __section__(".linkplay.playtone") lp_play_keytone(lp_keytone_mode_e e_mode)
{
    if(e_mode == LP_KEYTONE_MODE_START) {
        keytone_play(KEYTONE_TALK_START, KEYTONE_BLOCK);
    } else if(e_mode == LP_KEYTONE_MODE_APP_CON_LOST) {
        /*BLE未连上，提示：APP连接丢失，请启动并连接APP*/
        //PRINT_INFO("lion");
        lp_update_alexa_state(ALEXA_STATE_TYPE_NOT_CONNECT_APP, 0);
        com_tts_state_play((uint16)TTS_MODE_ONLYONE, (void *)TTS_APP_CON_LSOT);
        //com_tts_state_play((uint16)TTS_MODE_ONLYONE, (void *)TTS_LAUNCH_APP);
    } else if(e_mode == LP_KEYTONE_MODE_NETWORK_DISCONN) {
        lp_update_alexa_state(ALEXA_STATE_TYPE_NETWORK_DISCONNECT, 0);
    } else if(e_mode == LP_KEYTONE_MODE_ACCOUNT_INVALID) {
        lp_update_alexa_state(ALEXA_STATE_TYPE_ACCOUNT_INVALID, 0);
    } else {
        //do nothing
    }
}

#if defined(ENABLE_ALEXA_LED_NOTIFY)
void __section__(".linkplay.alexastate") lp_sdk_alexa_led_init(void)
{
    libc_memset(&g_alexa_led_status, 0, sizeof(g_alexa_led_status));
}

void __section__(".linkplay.alexastate") lp_alexa_led_init(void)
{
    //libc_print("init led", 0, 0);
    if(g_bt_stack_cur_info.ble_con_flag == 0 && g_bt_stack_cur_info.spp_con_flag == 0) {
        libc_memset(&g_alexa_led_status, 0, sizeof(g_alexa_led_status));
    }
}

void __section__(".linkplay.alexastate") lp_alexa_led_timer_deal(void)
{
    if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
        lp_update_alexa_state(ALEXA_STATE_TYPE_SPCH, ALEXA_SPEECH_STATE_IDLE);
    }
}

void __section__(".linkplay.alexastate") lp_alexa_start_led_timer(uint16 time)
{
    set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL, time, lp_alexa_led_timer_deal);
}
#endif

void __section__(".linkplay.alexastate") lp_update_alexa_state(uint8 e_type, uint8 e_state)
{
    libc_print("\tt: ", e_type, 2);
    libc_print("\ts: ", e_state, 2);
#if defined(ENABLE_ALEXA_LED_NOTIFY)
REDIRECT_LED_STATE:
    if(e_type == ALEXA_STATE_TYPE_SPCH) {
        if(ALEXA_SPEECH_STATE_IDLE == e_state) {
            if((g_bt_stack_cur_info.ble_con_flag || g_bt_stack_cur_info.spp_con_flag) && g_alexa_led_status.led_account_valid &&
                    g_alexa_led_status.led_alarm_active) {
                key_alexa_led_op(LED_OP_MIX_BLINKING, ((20 << 24) | 0x0000FF), ((2 << 24) | 0x00FFFF));
            } else if((g_bt_stack_cur_info.ble_con_flag || g_bt_stack_cur_info.spp_con_flag)
                      && g_alexa_led_status.led_account_valid &&
                      g_alexa_led_status.led_notifaction_active && sys_comval->mic_mute) {
                key_alexa_led_op(LED_OP_SOFT_BREATHING, ((10 << 24) | 0xFF0000),
                                 ((10 << 24) | 0xFFFF00)); //climb up time: 10 * 100ms, color:0xFF8000
            } else if((g_bt_stack_cur_info.ble_con_flag || g_bt_stack_cur_info.spp_con_flag)
                      && g_alexa_led_status.led_account_valid &&
                      g_alexa_led_status.led_notifaction_active) {
                key_alexa_led_op(LED_OP_SOFT_BREATHING, ((10 << 24) | 0xFFFF00), 0); //climb up time: 10 * 100ms, color:0xFF8000
            } else if(sys_comval->mic_mute) {
                key_alexa_led_op(LED_OP_ALWAYS, 0xFF0000, 0);//color:0xFF0000
            } else {
                key_alexa_led_op(LED_OP_ALWAYS, 0, 0);
            }
        } else if(ALEXA_SPEECH_STATE_LISTENING == e_state) {
            if(g_alexa_led_status.led_account_valid) {
                key_alexa_led_op(LED_OP_ALWAYS, 0x00FFFF, 0);   //show green blue
            }
        } else if(ALEXA_SPEECH_STATE_PROCESSING == e_state) {
            if(g_alexa_led_status.led_account_valid) {
                key_alexa_led_op(LED_OP_MIX_BLINKING, ((20 << 24) | 0x0000FF), ((1 << 24) | 0x00FFFF)); //
            }
        } else if(ALEXA_SPEECH_STATE_SPEAKING == e_state) {
            if(g_alexa_led_status.led_account_valid) {
                key_alexa_led_op(LED_OP_SOFT_BREATHING, ((10 << 24) | 0x00FFFF),
                                 ((10 << 24) | 0x0000FF));//climb up time: 10 * 100ms, color:0x00FFFF
            }
        } else if(ALEXA_SPEECH_STATE_ERROR == e_state) {
            lp_alexa_start_led_timer(3000); //3 seconds will clear
            key_alexa_led_op(LED_OP_BLINKING, ((20 << 24) | 0xFF0000), 0);//2Hz, color:0x00FFFF
        } else {
            //libc_print("speech_state: ", e_state, 2);
        }
    } else if(e_type == ALEXA_STATE_TYPE_NOTIFICATIONS) {
        if(e_state == ALEXA_NOTIFICATIONS_STATE_START) {
            g_alexa_led_status.led_notifaction_active = 1;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        } else if(e_state == ALEXA_NOTIFICATIONS_STATE_CLEAR
                  || e_state == ALEXA_NOTIFICATIONS_STATE_IDLE) {
            g_alexa_led_status.led_notifaction_active = 0;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        } else {
            //libc_print("notifications_state: ", e_state, 2);
        }
    } else if(e_type == ALEXA_STATE_TYPE_ACCOUNT_INVALID) {
        lp_alexa_start_led_timer(3000); //3 seconds will clear
        key_alexa_led_op(LED_OP_BLINKING, ((20 << 24) | 0xFF0000), 0);//2Hz
    } else if(e_type == ALEXA_STATE_TYPE_ACCOUNT) {
        if(e_state == ALEXA_ACCOUNT_STATE_VALID) {
            g_alexa_led_status.led_account_valid = 1;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        } else {
            g_alexa_led_status.led_account_valid = 0;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        }
    } else if(e_type == ALEXA_STATE_TYPE_ALARM) {
        if(e_state == ALEXA_ALARM_STATE_PLAYING) {
            g_alexa_led_status.led_alarm_active = 1;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        } else {
            g_alexa_led_status.led_alarm_active = 0;
            if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
                e_type = ALEXA_STATE_TYPE_SPCH;
                e_state = ALEXA_SPEECH_STATE_IDLE;
                goto REDIRECT_LED_STATE;
            }
        }
    } else if(e_type == ALEXA_STATE_TYPE_MIC_MUTE) {
        if(lp_get_alexa_spech_state() == ALEXA_SPEECH_STATE_IDLE) {
            e_type = ALEXA_STATE_TYPE_SPCH;
            e_state = ALEXA_SPEECH_STATE_IDLE;
            goto REDIRECT_LED_STATE;
        }
    } else if(e_type == ALEXA_STATE_TYPE_NOT_CONNECT_APP) {
        lp_alexa_start_led_timer(3000); //3 seconds will clear
        key_alexa_led_op(LED_OP_BLINKING, ((20 << 24) | 0xFF0000), 0);//2Hz, color:0x00FFFF
    } else if(e_type == ALEXA_STATE_TYPE_BLE_DISCONNECT || e_type == ALEXA_STATE_TYPE_SPP_DISCONNECT) {
        libc_memset(&g_alexa_led_status, 0, sizeof(g_alexa_led_status));
        e_type = ALEXA_STATE_TYPE_SPCH;
        e_state = ALEXA_SPEECH_STATE_IDLE;
        goto REDIRECT_LED_STATE;
    } else if(e_type == ALEXA_STATE_TYPE_MODE_CHANGE) {
        key_alexa_led_op(LED_OP_ALWAYS, 0x000000, 0);//all led off
    } else if(e_type == ALEXA_STATE_TYPE_NETWORK_DISCONNECT) {
        lp_alexa_start_led_timer(3000); //3 seconds will clear
        key_alexa_led_op(LED_OP_BLINKING, ((20 << 24) | 0xFF0000), 0);//2Hz, color:0x00FFFF
    } else {
        //libc_print("unknow: ", e_state, 2);
    }
#endif
}

void __section__(".linkplay.dsp") lp_asr_dsp_reset(void)
{
#if defined(ASR_DSP_RESET_GPIO_OUTEN)
    act_writel(act_readl(ASR_DSP_RESET_GPIO_OUTEN) | ASR_DSP_RESET_GPIO_PIN, ASR_DSP_RESET_GPIO_OUTEN);
    act_writel(act_readl(ASR_DSP_RESET_GPIO_PUEN) | ASR_DSP_RESET_GPIO_PIN, ASR_DSP_RESET_GPIO_PUEN);
    act_writel(act_readl(ASR_DSP_RESET_GPIO_DATA) & (~ASR_DSP_RESET_GPIO_PIN), ASR_DSP_RESET_GPIO_DATA);
    sys_usleep(100000);         //maybe could decrease
    act_writel(act_readl(ASR_DSP_RESET_GPIO_DATA) | ASR_DSP_RESET_GPIO_PIN, ASR_DSP_RESET_GPIO_DATA);
#endif
}

uint32 __section__(".linkplay.alexaprofile") lp_get_alexa_profile(uint8 *profile, uint32 max_bytes)
{
    uint8 *pHead = profile;
    uint8 len1, len2, len3;
    uint8 alexa_product_id[32];
    uint8 alexa_client_id[64];
    uint8 alexa_clientsecret[80];

    libc_memset(alexa_product_id, 0, sizeof(alexa_product_id));
    libc_memset(alexa_client_id, 0, sizeof(alexa_client_id));
    libc_memset(alexa_clientsecret, 0, sizeof(alexa_clientsecret));

    if(com_get_config_default(ALEXA_PROFILE_CONFIG_TYPE) == 0)
        return 0;

    if(com_get_config_struct(ALEXA_PRODUCT_ID, alexa_product_id, sizeof(alexa_product_id)) == FALSE)
        return 0;

    if(com_get_config_struct(ALEXA_CLIENT_ID, alexa_client_id, sizeof(alexa_client_id)) == FALSE)
        return 0;

    if(com_get_config_struct(ALEXA_CLIENT_SECRET, alexa_clientsecret, sizeof(alexa_clientsecret)) == FALSE)
        return 0;

    len1 = libc_strlen(alexa_product_id);
    len2 = libc_strlen(alexa_client_id);
    len3 = libc_strlen(alexa_clientsecret);

    if((len1 + len2 + len3 + 41) > max_bytes)
        return 0;

    libc_memcpy(pHead, "\"productid\":\"", 13);
    pHead += 13;

    libc_memcpy(pHead, alexa_product_id, len1);
    pHead += len1;
    *(pHead++) = '\"';
    *(pHead++) = ',';

    libc_memcpy(pHead, "\"clientid\":\"", 12);
    pHead += 12;
    libc_memcpy(pHead, alexa_client_id, len2);
    pHead += len2;
    *(pHead++) = '\"';
    *(pHead++) = ',';

    libc_memcpy(pHead, "\"clientsecret\":\"", 16);
    pHead += 16;
    libc_memcpy(pHead, alexa_clientsecret, len3);
    pHead += len3;
    *(pHead++) = '\"';

    return (pHead - profile);
}

/*
 *       do somethine when customize ble received
 */
void __section__(".linkplay.ble") lp_receive_ble_data(uint8 *data, uint32 length)
{
    libc_print("receive bytes: ", length, 2);
}

const lp_cb_func_t __section__(".linkplay.const") g_lp_func_cb = {
    .pf_lp_play_keytone = lp_play_keytone,
    .pf_lp_update_alexa_state = lp_update_alexa_state,
    .pf_lp_dsp_reset = lp_asr_dsp_reset,
    .pf_lp_get_alexa_profile = lp_get_alexa_profile,
    .pf_lp_receive_ble_data = lp_receive_ble_data,
#if defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
    .nFuncBitMap = DSP_TYPE_C02,
#else
    .nFuncBitMap = DSP_TYPE_SA2,
#endif
};
#endif //#ifdef SMART_BOX_OPUS_MODE
