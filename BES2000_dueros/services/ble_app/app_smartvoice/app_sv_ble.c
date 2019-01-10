/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_SMARTVOICE)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_sv_ble.h"                  // Smart Voice Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions         
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

#ifdef __SMARTVOICE__
#include "app_sv_handle.h"
#include "sv_gatt_server.h"

#endif
#include "app_sv_cmd_code.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
struct app_sv_env_tag app_sv_env;

static app_sv_tx_done_t tx_done_callback = NULL;

APP_SV_CMD_RET_STATUS_E app_sv_data_received(uint8_t* ptrData, uint32_t dataLength);
APP_SV_CMD_RET_STATUS_E app_sv_cmd_received(uint8_t* ptrData, uint32_t dataLength);
void app_sv_data_reset_env(void);

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_sv_connected_evt_handler(uint8_t conidx)
{
    app_sv_update_conn_parameter(SV_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
        SV_BLE_CONNECTION_INTERVAL_MAX_IN_MS, 
        SV_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS,
        SV_BLE_CONNECTION_SLAVELATENCY);
	
	app_sv_env.connectionIndex = conidx;
#ifdef __SMARTVOICE__
    app_smartvoice_connected(APP_SMARTVOICE_BLE_CONNECTED);
#endif
}

void app_sv_disconnected_evt_handler(void)
{
	app_sv_env.connectionIndex = BLE_INVALID_CONNECTION_INDEX;
	app_sv_env.isCmdNotificationEnabled = false;
	app_sv_env.isDataNotificationEnabled = false;

#ifdef __SMARTVOICE__
    app_smartvoice_disconnected(APP_SMARTVOICE_BLE_DISCONNECTED);
#endif
}

void app_sv_init(void)
{
    // Reset the environment
	app_sv_env.connectionIndex =  BLE_INVALID_CONNECTION_INDEX;
	app_sv_env.isCmdNotificationEnabled = false;
	app_sv_env.isDataNotificationEnabled = false;
}

void app_sv_add_sv(void)
{
	BLE_APP_DBG("app_sv_add_sv");
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, 0);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, ENABLE);
    req->prf_task_id = TASK_ID_SMARTVOICE;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_sv_send_cmd_via_notification(uint8_t* ptrData, uint32_t length)
{
	struct ble_sv_send_data_req_t * req = KE_MSG_ALLOC_DYN(SMARTVOICE_SEND_CMD_VIA_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_SMARTVOICE),
                                                TASK_APP,
                                                ble_sv_send_data_req_t,
                                                length);
	req->connecionIndex = app_sv_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_sv_send_cmd_via_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_sv_send_data_req_t * req = KE_MSG_ALLOC_DYN(SMARTVOICE_SEND_CMD_VIA_INDICATION,
                                                prf_get_task_from_id(TASK_ID_SMARTVOICE),
                                                TASK_APP,
                                                ble_sv_send_data_req_t,
                                                length);
	req->connecionIndex = app_sv_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_sv_send_data_via_notification(uint8_t* ptrData, uint32_t length)
{
	struct ble_sv_send_data_req_t * req = KE_MSG_ALLOC_DYN(SMARTVOICE_SEND_DATA_VIA_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_SMARTVOICE),
                                                TASK_APP,
                                                ble_sv_send_data_req_t,
                                                length);
	req->connecionIndex = app_sv_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_sv_send_data_via_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_sv_send_data_req_t * req = KE_MSG_ALLOC_DYN(SMARTVOICE_SEND_DATA_VIA_INDICATION,
                                                prf_get_task_from_id(TASK_ID_SMARTVOICE),
                                                TASK_APP,
                                                ble_sv_send_data_req_t,
                                                length);
	req->connecionIndex = app_sv_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

static int app_sv_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

static int app_sv_cmd_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_sv_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("sv cmd ccc changed to %d", param->isNotificationEnabled);
    app_sv_env.isCmdNotificationEnabled = param->isNotificationEnabled;
    return (KE_MSG_CONSUMED);
}

static int app_sv_data_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_sv_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("sv data ccc changed to %d", param->isNotificationEnabled);
    app_sv_env.isDataNotificationEnabled = param->isNotificationEnabled;

	/*
	if (!app_sv_env.isDataNotificationEnabled)
	{
		// widen the connection interval to save the power consumption
		app_sv_update_conn_parameter(LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
			LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS, LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS);
	}
	else
	{
		// narrow the connection interval to increase the speed
		app_sv_update_conn_parameter(HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
			HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS, HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS);
	}
	*/
	
    return (KE_MSG_CONSUMED);
}

static int app_sv_tx_data_sent_handler(ke_msg_id_t const msgid,
                              struct ble_sv_tx_sent_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    if (NULL != tx_done_callback)
    {
		tx_done_callback();
    }

    return (KE_MSG_CONSUMED);
}

static int app_sv_rx_cmd_received_handler(ke_msg_id_t const msgid,
                              struct ble_sv_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	app_sv_cmd_received(param->data, param->length);	
    return (KE_MSG_CONSUMED);
}

static int app_sv_rx_data_received_handler(ke_msg_id_t const msgid,
                              struct ble_sv_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	app_sv_data_received(param->data, param->length);
    return (KE_MSG_CONSUMED);
}

void app_sv_register_tx_done(app_sv_tx_done_t callback)
{
	tx_done_callback = callback;
}

void app_sv_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency)
{
	struct gapc_conn_param conn_param;

	conn_param.intv_min = (uint16_t)(min_interval_in_ms/1.25); 
	conn_param.intv_max = (uint16_t)(max_interval_in_ms/1.25); 
	conn_param.latency	= slaveLantency;
	conn_param.time_out = supervision_timeout_in_ms/10; 

	l2cap_update_param(&conn_param);
}

void app_sv_mtu_exchanged_handler(uint16_t MTU)
{
#ifdef __SMARTVOICE__
    app_smartvoice_config_mtu(MTU);
#endif	
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_sv_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        	(ke_msg_func_t)app_sv_msg_handler},

    {SMARTVOICE_CMD_CCC_CHANGED,     	(ke_msg_func_t)app_sv_cmd_ccc_changed_handler},
	{SMARTVOICE_DATA_CCC_CHANGED,     	(ke_msg_func_t)app_sv_data_ccc_changed_handler},
    {SMARTVOICE_TX_DATA_SENT,    		(ke_msg_func_t)app_sv_tx_data_sent_handler},
    {SMARTVOICE_CMD_RECEIVED,   		(ke_msg_func_t)app_sv_rx_cmd_received_handler},
    {SMARTVOICE_DATA_RECEIVED, 			(ke_msg_func_t)app_sv_rx_data_received_handler},
};

const struct ke_state_handler app_sv_table_handler =
    {&app_sv_msg_handler_list[0], (sizeof(app_sv_msg_handler_list)/sizeof(struct ke_msg_handler))};

#endif //BLE_APP_SMARTVOICE

/// @} APP
