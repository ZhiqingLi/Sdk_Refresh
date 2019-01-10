#ifndef __APP_BT_H__
#define __APP_BT_H__

#ifdef __cplusplus
extern "C" {
#endif

enum APP_BT_REQ_T {
    APP_BT_REQ_ACCESS_MODE_SET,
    APP_BT_REQ_NUM
};

enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T {
    APP_BT_GOLBAL_HANDLE_HOOK_USER_0 = 0,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_1,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY
};

typedef void (*APP_BT_REQ_CONNECT_PROFILE_FN_T)(void *, void *);
typedef BtStatus (*APP_BT_REQ_HF_OP_FN_T)(void *);
typedef void (*APP_BT_GOLBAL_HANDLE_HOOK_HANDLER)(const BtEvent *Event);

#define app_bt_accessmode_set_req(accmode) do{app_bt_send_request(APP_BT_REQ_ACCESS_MODE_SET, accmode, 0, 0);}while(0)

void PairingTransferToConnectable(void);

void app_bt_golbal_handle_init(void);

void app_bt_opening_reconnect(void);

void app_bt_accessmode_set(BtAccessibleMode mode);

void app_bt_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t ptr);

void app_bt_init(void);

int app_bt_state_checker(void);

void *app_bt_profile_active_store_ptr_get(uint8_t *bdAddr);

void app_bt_profile_connect_manager_open(void);

void app_bt_profile_connect_manager_opening_reconnect(void);

BOOL app_bt_profile_connect_openreconnecting(void *ptr);

int app_bt_golbal_handle_hook_set(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user, APP_BT_GOLBAL_HANDLE_HOOK_HANDLER handler);

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_golbal_handle_hook_get(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user);

bool app_is_hfp_service_connected(void);
bool a2dp_service_is_connected(void);

bool app_bluetooth_is_connected(void);
bool app_is_link_connected(void);


void app_bt_stay_active(uint8_t deviceId);

void app_bt_allow_sniff(uint8_t deviceId);

void app_bt_stop_sniff(uint8_t deviceId);

void app_check_pending_stop_sniff_op(void);


int app_hfp_dial_number(const char *phone_number, unsigned int len);
int app_hfp_answer_call(void);
int app_hfp_hangup_call(void);
int app_hfp_send_at_command(const char *cmd);
int app_hfp_redial(void);


#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
