/**
 ****************************************************************************************
 *
 * @file app_ota.c
 *
 * @brief OTA Application entry point
 *
 * Copyright (C) BES
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_OTA)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_ota_ble.h"                  // OTA Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "ota_gatt_server.h"
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"
#include "ota_handler.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
struct app_ota_env_tag app_ota_env;

static app_ota_tx_done_t ota_data_tx_done_callback = NULL;

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_ota_connected_evt_handler(uint8_t conidx)
{	
	app_ota_env.connectionIndex = conidx;
	ota_set_datapath_type(OTA_DATAPATH_BLE);
	ota_register_transmitter(app_ota_send_notification);
}

void app_ota_disconnected_evt_handler(void)
{
	app_ota_env.connectionIndex = BLE_INVALID_CONNECTION_INDEX;
	app_ota_env.isNotificationEnabled = false;
	ota_disconnection_handler();
}

void app_ota_init(void)
{
    // Reset the environment
	app_ota_env.connectionIndex =  BLE_INVALID_CONNECTION_INDEX;
	app_ota_env.isNotificationEnabled = false;
}

void app_ota_add_ota(void)
{
	BLE_APP_DBG("app_ota_add_ota");
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, 0);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, ENABLE);
    req->prf_task_id = TASK_ID_OTA;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_ota_send_notification(uint8_t* ptrData, uint32_t length)
{
	TRACE("Send ota rsp:");
	DUMP8("%02x ", ptrData, length);
	
	struct ble_ota_send_data_req_t * req = KE_MSG_ALLOC_DYN(OTA_SEND_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_OTA),
                                                TASK_APP,
                                                ble_ota_send_data_req_t,
                                                length);
	req->connecionIndex = app_ota_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_ota_send_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_ota_send_data_req_t * req = KE_MSG_ALLOC_DYN(OTA_SEND_INDICATION,
                                                prf_get_task_from_id(TASK_ID_OTA),
                                                TASK_APP,
                                                ble_ota_send_data_req_t,
                                                length);
	req->connecionIndex = app_ota_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

static int app_ota_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

static int app_ota_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_ota_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("ota data ccc changed to %d", param->isNotificationEnabled);
    app_ota_env.isNotificationEnabled = param->isNotificationEnabled;

	/*
	if (!app_ota_env.isNotificationEnabled)
	{
		// widen the connection interval to save the power consumption
		app_ota_update_conn_parameter(OTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
			OTA_LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS, OTA_LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS);
	}
	else
	{
		// narrow the connection interval to increase the speed
		app_ota_update_conn_parameter(OTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
			OTA_HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS, OTA_HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS);
	}
	*/
	
    return (KE_MSG_CONSUMED);
}

static int app_ota_tx_data_sent_handler(ke_msg_id_t const msgid,
                              struct ble_ota_tx_sent_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    if (NULL != ota_data_tx_done_callback)
    {
		ota_data_tx_done_callback();
    }

	ota_data_transmission_done_callback();

    return (KE_MSG_CONSUMED);
}

static int app_ota_data_received_handler(ke_msg_id_t const msgid,
                              struct ble_ota_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("receive ota data");
	ota_handle_received_data(param->data, param->length, true);	
    return (KE_MSG_CONSUMED);
}

void app_ota_register_tx_done(app_ota_tx_done_t callback)
{
	ota_data_tx_done_callback = callback;
}

void app_ota_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency)
{
	struct gapc_conn_param conn_param;

	conn_param.intv_min = (uint16_t)(min_interval_in_ms/1.25); 
	conn_param.intv_max = (uint16_t)(max_interval_in_ms/1.25); 
	conn_param.latency	= slaveLantency;
	conn_param.time_out = supervision_timeout_in_ms/10; 

	l2cap_update_param(&conn_param);
}

void app_ota_mtu_exchanged_handler(uint16_t MTU)
{
	ota_update_MTU(MTU);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_ota_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)app_ota_msg_handler},

    {OTA_CCC_CHANGED,     		(ke_msg_func_t)app_ota_ccc_changed_handler},
    {OTA_TX_DATA_SENT,    		(ke_msg_func_t)app_ota_tx_data_sent_handler},
    {OTA_DATA_RECEIVED, 		(ke_msg_func_t)app_ota_data_received_handler},
};

const struct ke_state_handler app_ota_table_handler =
    {&app_ota_msg_handler_list[0], (sizeof(app_ota_msg_handler_list)/sizeof(struct ke_msg_handler))};

#endif //BLE_APP_OTA

/// @} APP
