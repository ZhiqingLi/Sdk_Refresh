#ifndef _APP_BT_H_
#define _APP_BT_H_

typedef struct _bt_entity_s
{
    uint32 init_flag;

    btaddr_t btaddr_def;
}BT_ENTITY_T;
#if defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)
typedef struct _bt_sniff_s
{
    btaddr_t    remote_btaddr;
    uint16_t    link_handle;
    uint8_t     is_active;
    uint8_t     is_used;
    uint8_t     is_policy;
}BT_SNIFF_T, *bt_sniff_t;
#endif
typedef enum
{
	SCO_DISCONNECTED = 0,
	SCO_CONNECTED	 = 1,
	SCO_SWAP	 = 2
}t_sco_active_state;

#ifndef CONFIG_TWS
/* A2DP Notifications */
typedef struct {
    uint32_t               svc_id;
    bt_a2dp_sink_session_h svc;
    bt_a2dp_sink_session_h srv;
    uint32_t               sdp;
    audio_out_ctx_h        audio_out;

    uint32_t 		       freq;
    uint32_t		       channel;

    uint32_t               flag;
#if defined(CONFIG_TWS) || defined(A2DP_SBC_DUMP_SHOW)
	bt_a2dp_codec_t    codec;
#endif

    int8_t                 volume;

    int8_t                 is_used;
    int8_t                 auto_on;

    bt_link_modes_mask_t   conn_mode;
    bt_conn_req_h          conn_req;

    btaddr_t               raddr;
} a2dp_backend_t;
#endif
uint32 bt_is_install(void);
void bt_auto_connect_start(void);
btaddr_t *bt_get_default_btaddr_ptr(void);
void bt_create_conn_timeout_wrap(void *arg);
void bt_auto_connect_stop(void);
void bt_auto_connect_next(void);
int bt_is_reject_new_connect(void);
void bt_create_reconn_timeout_wrap(void *arg);
RAM_CODE uint32_t bt_flag1_is_set(uint32 flag);
RAM_CODE uint32_t bt_flag2_is_set(uint32 flag);
RAM_CODE void bt_flag1_operate(uint32 flag, uint8 oper);
RAM_CODE void bt_flag2_operate(uint32 flag, uint8 oper);
#if 0//def CONN_WITH_MUSIC
void bt_flag2_operate_0(uint32 flag);
#endif


#ifdef CONFIG_AUTO_CONN_AB
void bt_create_autoconn_timeout_AB_wrap(void *arg);
#endif

//void bt_select_other_a2dp_avrcp(uint8 addr_same);
void bt_select_current_a2dp_avrcp(void *arg);
void bt_select_current_hfp(void *arg);
//void bt_reconn_after_disconnect(void);
void bt_all_acl_disconnect(hci_unit_t *unit);
result_t bt_hfp_peeraddr(void *hf_session, btaddr_t *raddr);

uint8 Judge_role(void);

btaddr_t *hf_get_curhfp_raddr(void);

void get_cur_hfp_raddr(btaddr_t **raddr);
btaddr_t *get_pcb_raddr(struct sco_pcb *pcb);
result_t get_play_a2dp_avrcp_state(uint32 *index);
//void get_raddr_from_a2dp(btaddr_t**raddr0,btaddr_t**raddr1);
uint32_t get_sco_conn_status(btaddr_t *raddr);
void select_other_avrcp_app_pause_by_raddr(btaddr_t *raddr);
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
hci_link_t *select_other_playing_link_handle_by_raddr(btaddr_t *raddr);
#ifndef CONFIG_TWS
hci_link_t *select_curr_playing_link_handle_by_a2dp(a2dp_backend_t *app_ptr);
#endif
hci_link_t *select_flow_ctrl_link_handle_by_raddr(btaddr_t *raddr);
void bt_set_ag_flow_ctrl_by_handle(hci_link_t *link);
void bt_set_ag_flow_ctrl_timeout(void *arg);
#endif
uint32 avrcp_get_raddr(void *arg, btaddr_t *raddr);
uint32 avrcp_update_current_app(const btaddr_t *raddr);
void avrcp_select_current_app(void *arg);
uint32 avrcp_select_current_app_to_another(const btaddr_t *raddr);
uint32 avrcp_current_is_connected(void);
uint32 avrcp_current_is_processing(void);
uint32_t a2dp_current_is_pause(void);
void a2dp_current_set_flag(uint32 flag);
void a2dp_current_clear_flag(uint32 flag);
void avrcp_current_send_opcode(void *arg);
uint32 avrcp_is_connection(void);
uint32 avrcp_is_cmd_processing(void);
void avrcp_volume_sync(void);

void select_play_a2dp_avrcp(void* arg);
result_t select_a2dp_avrcp(btaddr_t *addr);

void app_bt_sco_enable_wrap(uint8 enable);
void app_bt_profile_disconn_wrap(uint8 profileId, void *app_ptr);
void app_bt_reset_policy_iocap(void);
void app_bt_active_allpro_conn(void *arg);
void app_bt_conn_compl_wrap(hci_link_t * link, const btaddr_t  *rbtaddr);
void app_bt_disable_complete_wrap( void );
int app_is_conn_be_accepted( void );
uint8 get_other_a2dp_play_flag(btaddr_t *btaddr);
btaddr_t *a2dp_get_current_app_remote_addr(void);
void a2dp_clear_current_app(void);
void *a2dp_select1_current_app(void *arg);
uint32 a2dp_check_flag(void *arg,uint32 flag);
uint32 get_a2dp_priv_flag(uint8 idx,uint32 flag);
result_t a2dp_select_current_app_to_another(btaddr_t *addr);
uint32_t a2dp_is_connection(void *arg);
int a2dp_is_connected_based_on_raddr(const btaddr_t *raddr);
uint32_t a2dp_all_apps_is_unused(void);
uint32_t a2dp_has_connection(void);
uint32_t a2dp_current_is_music(void);

void hfpcall_select_a2dp(void *arg);
uint32_t hfp_is_connection_based_on_raddr(btaddr_t *raddr);
uint32 hfp_check_autoconn_disconned_flag(btaddr_t *r_addr);
uint32_t hfp_has_connection(void);
uint32 hf_clear_autoconn_diconned_flag(void *raddr);
void hf_set_flag(void *arg, uint32 flag);
uint32 hf_check_flag(void *arg, uint32 flag);
uint32 get_hf_priv_flag(uint8 idx,uint32 flag);
void hf_clear_flag(void *arg, uint32 flag);
result_t hf_cmd_connect(char *params, unsigned int len);
int avrcp_is_connected_based_on_raddr(const btaddr_t *raddr);

result_t hid_int_conn_disconnect(void);
result_t hid_cmd_connect(char *params, unsigned int len);

void Set_remoteDev_role_btaddr(btaddr_t *btaddr,uint32 role);
void bt_create_conn_status_wrap(uint8 status);

void otm_current_a2dp_stream_off_handler(void *arg);

int hci_get_acl_link_count(hci_unit_t *unit);
int hci_get_acl_link_addr(hci_unit_t *unit, btaddr_t** raddr);
int hci_check_acl_link(btaddr_t* raddr);
void a2dp_audio_restore(int mode);
int8 get_current_hfp_volume(void);
uint32_t get_current_hfp_freq(void);
void *get_current_hfp_ptr(void);

uint32 get_current_hfp_flag(uint32 flag);
void set_current_hfp_flag(uint32 flag,uint8 op);

#ifdef BT_ONE_TO_MULTIPLE
void app_bt_reconn_after_callEnd(void);
#ifndef  CONN_WITH_MUSIC
void app_bt_reconn_after_play(void);
#endif
void bt_exchange_hf_active_by_handle(uint16_t handle);
uint8 hf_exchange_sco_active_flag(btaddr_t *btaddr,t_sco_active_state state);
uint16  get_hf_active_sco_handle(void);
void hfp_2eSCOs_A_B_SWAP(void);
void *get_hfp_from_active_sco_flag(void);

__INLINE__ void set_2hfps_sco_and_incoming_flag(uint8_t val);
__INLINE__ uint8_t get_2hfps_sco_and_incoming_flag(void);
uint8_t check_2hfps_sco_and_incoming_status(void);


void* hfp_get_app_from_priv_ext(sco_utils_h priv);
void set_hf_flag_1toN(btaddr_t *btaddr,uint32 flag);
void set_hf_flag_call_seq(btaddr_t *btaddr);
void clear_hf_flag_1toN(btaddr_t *btaddr,uint32 flag);
uint32 get_hfp_flag_1toN(btaddr_t *btaddr,uint32 flag);
uint32 has_hfp_flag_1toN(uint32 flag);
int hf_sco_disconn_reconn(int oper);
void change_cur_hfp_based_on_addr(btaddr_t *btaddr);
void change_cur_hfp_to_another_call_conn(btaddr_t *btaddr);
void change_cur_hfp_to_another_incoming_call(btaddr_t *btaddr);
uint32_t change_cur_hfp_to_another_sco_conn(btaddr_t *btaddr);
uint32_t hfp_select_current_app_to_another(btaddr_t *btaddr);
uint32_t a2dp_has_the_music(btaddr_t addr);
uint32_t a2dp_has_the_connection(btaddr_t addr);
uint32_t hfp_has_the_sco_conn(btaddr_t addr);
uint32_t hfp_has_sco_conn(void);
uint32_t hfp_has_the_connection(btaddr_t addr);
uint32_t avrcp_has_the_connection(btaddr_t addr);
#else
int8_t hfp_is_used(void);
#endif
#ifdef CONFIG_BLUETOOTH_COEXIST
void app_not_bt_set_a2dp_flag(uint8 flag);
#endif

#if defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)
void bt_sniff_alloc_st(uint8_t idx,uint16_t handle,btaddr_t *rbtaddr);
void bt_sniff_free_st(uint8_t idx);
uint8_t bt_sniff_find_st_available(void);
uint16_t bt_sniff_get_handle_from_raddr(btaddr_t *rbtaddr);
uint8_t bt_sniff_get_index_from_handle(uint16_t handle);
uint8_t bt_sniff_get_index_from_raddr(btaddr_t *rbtaddr);
uint16_t bt_sniff_get_handle_from_idx(uint8_t idx);
btaddr_t bt_sniff_get_rtaddr_from_idx(uint8_t idx);
uint8_t bt_sniff_is_used(uint8_t idx);
uint8_t bt_sniff_is_active(uint8_t idx);
void bt_sniff_set_active(uint8_t idx,uint8_t enable);
uint8_t bt_sniff_is_policy(uint8_t idx);
void bt_sniff_set_policy(uint8_t idx,uint8_t enable);
#endif

#if defined(CONFIG_TWS) || (CONFIG_APP_RECONNECT_MATCH == 1)
void app_bt_auconn_stop( void );
#endif
#if(POWERKEY_5S_PARING == 1)
void power_on_auto_connect(void);
#endif

void app_bt_pinkey_missing(btaddr_t* addr);

#ifdef CONFIG_PRODUCT_TEST_INF
result_t hf_cmd_trans_buttoncode(uint32 value);
#endif

#endif
