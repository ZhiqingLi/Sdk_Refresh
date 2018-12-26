#ifndef __LP_MAIN_H__
#define __LP_MAIN_H__

#ifdef __LINKPLAY_MAIN_C__
#define __LP_DEC__
#else
#define __LP_DEC__ extern
#endif


#define SDK_VERSION         "4.2"
#define SDK_BUILD_DATE      "20181225"

typedef enum {
    LP_KEYTONE_MODE_START,
    LP_KEYTONE_MODE_STOP,
    LP_KEYTONE_MODE_APP_CON_LOST,
    LP_KEYTONE_MODE_NETWORK_DISCONN,
    LP_KEYTONE_MODE_ACCOUNT_INVALID,
} lp_keytone_mode_e;

typedef enum {
    ALEXA_STATE_TYPE_SPCH,
    ALEXA_STATE_TYPE_NOTIFICATIONS,
    ALEXA_STATE_TYPE_ACCOUNT,
    ALEXA_STATE_TYPE_ALARM,
    ALEXA_STATE_TYPE_NETWORK,
    ALEXA_STATE_TYPE_MIC_MUTE = 10,
    ALEXA_STATE_TYPE_NOT_CONNECT_APP,
    ALEXA_STATE_TYPE_BLE_DISCONNECT,
    ALEXA_STATE_TYPE_SPP_DISCONNECT,
    ALEXA_STATE_TYPE_MODE_CHANGE,
    ALEXA_STATE_TYPE_NETWORK_DISCONNECT,
    ALEXA_STATE_TYPE_ACCOUNT_INVALID,
} alexa_state_type_e;

typedef enum {
    ALEXA_SPEECH_STATE_IDLE,
    ALEXA_SPEECH_STATE_LISTENING,
    ALEXA_SPEECH_STATE_PROCESSING,
    ALEXA_SPEECH_STATE_SPEAKING,
    ALEXA_SPEECH_STATE_ERROR,
} alexa_spch_state_e;

typedef enum {
    ALEXA_NOTIFICATIONS_STATE_IDLE,
    ALEXA_NOTIFICATIONS_STATE_START,
    ALEXA_NOTIFICATIONS_STATE_CLEAR,
} alexa_notifications_state_e;

typedef enum {
    ALEXA_ACCOUNT_STATE_VALID,
    ALEXA_ACCOUNT_STATE_INVALID,
    ALEXA_ACCOUNT_STATE_NOTLOGIN,
} alexa_account_state_e;

typedef enum {
    ALEXA_ALARM_STATE_IDLE,
    ALEXA_ALARM_STATE_PLAYING,
    ALEXA_ALARM_STATE_STOPPED,
} alexa_alarm_state_e;

typedef enum {
    ALEXA_NETWORK_STATE_DISCONNECTED,
    ALEXA_NETWORK_STATE_CONNECTED,
} alexa_network_state_e;

typedef enum {
    LP_SDK_ERR_SUCCESS,
    LP_SDK_ERR_NOMEMEORY,
    LP_SDK_ERR_INVALID_PARAMETER,
    LP_SDK_ERR_UNKONW_CMD,
    LP_SDK_ERR_NEED_WAIT_DATA,
    LP_SDK_ERR_BLE_NOT_CONNECT,
    LP_SDK_ERR_I2C_ERROR,
    LP_SDK_ERR_KEY_NEED_SEND_TO_BT,
    LP_SDK_ERR_INVALID_APP,
} LP_SDK_ERR_CODE;

typedef enum {
    LP_ALEXA_PLAYER_PLAY        = 0,
    LP_ALEXA_PLAYER_PAUSE       = 1,
    LP_ALEXA_PLAYER_PREV        = 2,
    LP_ALEXA_PLAYER_NEXT        = 3,
    LP_ALEXA_PLAYER_ABORT       = 4,
} LP_SDK_PLAYER_OPERATION;

typedef enum {
    LP_TRIGGER_BY_KEY           = 2,
    LP_TRIGGER_BY_VOICE         = 6,
} LP_SDK_ALEXA_TRIGGER_MODE;

typedef void (*_lp_play_keytone)(lp_keytone_mode_e e_mode);
typedef void (*_lp_update_alexa_state)(uint8 e_type, uint8 e_state);
typedef void (*_lp_dsp_reset)(void);
typedef uint32(*_lp_get_alexa_profile)(uint8 *pProfile, uint32 max_bytes);
typedef void (*_lp_receive_ble_data)(uint8 *data, uint32 length);

#define DSP_TYPE_BIT_SHIFT      0                               //2bit
#define DSP_TYPE_BIT_MASK       (0x03 << DSP_TYPE_BIT_SHIFT)
#define DSP_TYPE_NONE           (0x00 << DSP_TYPE_BIT_SHIFT)
#define DSP_TYPE_C02            (0x01 << DSP_TYPE_BIT_SHIFT)
#define DSP_TYPE_SA2            (0x02 << DSP_TYPE_BIT_SHIFT)


typedef struct {
    _lp_play_keytone            pf_lp_play_keytone;
    _lp_update_alexa_state      pf_lp_update_alexa_state;
    _lp_dsp_reset               pf_lp_dsp_reset;                //if product is farfield, must fill this function
    _lp_get_alexa_profile       pf_lp_get_alexa_profile;
    _lp_receive_ble_data        pf_lp_receive_ble_data;
    uint8                       nFuncBitMap;
} lp_cb_func_t;

__LP_DEC__ int lp_sdk_init(void);
__LP_DEC__ int lp_init(lp_cb_func_t *t_func);

__LP_DEC__ int lp_key_record_triger(void);
__LP_DEC__ int lp_key_record_stop(void);
__LP_DEC__ void lp_set_data_encrypt_callback(void);

__LP_DEC__ void lp_set_alexa_player_status(uint8 status);

//key function, return LP_SDK_ERR_SUCCESS: no need send to bt, otherwise, send to bt
__LP_DEC__ int lp_alexa_key_play_deal(void);
__LP_DEC__ int lp_alexa_key_next_deal(void);
__LP_DEC__ int lp_alexa_key_prev_deal(void);

__LP_DEC__ void lp_set_vol_dev_to_phone(void);

__LP_DEC__ uint8 lp_deal_app_exit(void);
__LP_DEC__ uint8 lp_deal_ble_disconnect(void);
__LP_DEC__ uint8 lp_deal_spp_disconnect(void);
__LP_DEC__ int lp_alexa_voice_trigger(void *ev_param);

__LP_DEC__ void lp_stop_alexa_alarm(void);
__LP_DEC__ int lp_send_ble_data(uint8 *data, uint32 length);

__LP_DEC__ uint8 lp_get_alexa_spech_state(void);
__LP_DEC__ uint8 lp_record_stop_force(void);

#endif

