/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_AMA_VOICE)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_ama_ble.h"                  // ama Voice Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions         
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

#ifdef __AMA_VOICE__
#include "ama_gatt_server.h"
#include "app_ama_handle.h"
#endif
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
struct app_ama_env_tag app_ama_env;

static app_ama_tx_done_t tx_done_callback = NULL;


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_ama_connected_evt_handler(uint8_t conidx)
{
    app_ama_update_conn_parameter(AMA_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
        AMA_BLE_CONNECTION_INTERVAL_MAX_IN_MS, 
        AMA_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS,
        AMA_BLE_CONNECTION_SLAVELATENCY);
	
	app_ama_env.connectionIndex = conidx;	
}

void app_ama_disconnected_evt_handler(void)
{
	app_ama_env.connectionIndex = BLE_INVALID_CONNECTION_INDEX;
	app_ama_env.isCmdNotificationEnabled = false;
#ifdef __AMA_VOICE__
	app_ama_voice_disconnected(ama_ble_connect);
#endif

}

void app_ama_init(void)
{
    // Reset the environment
	app_ama_env.connectionIndex =  BLE_INVALID_CONNECTION_INDEX;
	app_ama_env.isCmdNotificationEnabled = false;
}

void app_ama_add_ama(void)
{
	BLE_APP_DBG("app_sv_add_sv");
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, 0);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, ENABLE);
    req->prf_task_id = TASK_ID_AMA;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_ama_send_via_notification(uint8_t* ptrData, uint32_t length)
{
	TRACE("ble ama data send!!len=%d",length);
	struct ble_ama_send_data_req_t * req = KE_MSG_ALLOC_DYN(AMA_SEND_VIA_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_AMA),
                                                TASK_APP,
                                                ble_ama_send_data_req_t,
                                                length);
	req->connecionIndex = app_ama_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_ama_send_via_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_ama_send_data_req_t * req = KE_MSG_ALLOC_DYN(AMA_SEND_VIA_INDICATION,
                                                prf_get_task_from_id(TASK_ID_AMA),
                                                TASK_APP,
                                                ble_ama_send_data_req_t,
                                                length);
	req->connecionIndex = app_ama_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

static int app_ama_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

static int app_ama_cmd_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_ama_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("ama cmd ccc changed to %d", param->isNotificationEnabled);
    app_ama_env.isCmdNotificationEnabled = param->isNotificationEnabled;
#ifdef __AMA_VOICE__
	app_ama_voice_connected(ama_ble_connect);
#endif	
    return (KE_MSG_CONSUMED);
}

static int app_ama_tx_data_sent_handler(ke_msg_id_t const msgid,
                              struct ble_ama_tx_sent_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    if (NULL != tx_done_callback)
    {
		tx_done_callback();
    }

    return (KE_MSG_CONSUMED);
}

static int app_ama_rx_received_handler(ke_msg_id_t const msgid,
                              struct ble_ama_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("ama ble receive======>");
	DUMP8("%02x ",param->data, param->length);
	app_ama_rcv_data(param->data, param->length);	
    return (KE_MSG_CONSUMED);
}

void app_ama_register_tx_done(app_ama_tx_done_t callback)
{
	tx_done_callback = callback;
}

void app_ama_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency)
{
	struct gapc_conn_param conn_param;

	conn_param.intv_min = (uint16_t)(min_interval_in_ms/1.25); 
	conn_param.intv_max = (uint16_t)(max_interval_in_ms/1.25); 
	conn_param.latency	= slaveLantency;
	conn_param.time_out = supervision_timeout_in_ms/10; 

	l2cap_update_param(&conn_param);
}

void app_ama_mtu_exchanged_handler(uint16_t MTU)
{
	app_ama_updata_mtu_size(MTU);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_ama_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)app_ama_msg_handler},
    {AMA_CCC_CHANGED,     		(ke_msg_func_t)app_ama_cmd_ccc_changed_handler},
    {AMA_TX_DATA_SENT,    		(ke_msg_func_t)app_ama_tx_data_sent_handler},//ama ble data sent callback
    {AMA_RECEIVED,   			(ke_msg_func_t)app_ama_rx_received_handler},
};

const struct ke_state_handler app_ama_table_handler =
    {&app_ama_msg_handler_list[0], (sizeof(app_ama_msg_handler_list)/sizeof(struct ke_msg_handler))};

#endif //BLE_APP_AMA_VOICE

/// @} APP
