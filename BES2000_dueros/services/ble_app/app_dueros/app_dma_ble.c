/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_DMA_VOICE)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_dma_ble.h"                  // Smart Voice Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions         
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

#ifdef __DMA_VOICE__
#include "app_dma_handle.h"
#include "dma_gatt_server.h"
#endif


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
struct app_dma_env_tag app_dma_env;

static app_dma_tx_done_t tx_done_callback = NULL;


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void app_dma_connected_evt_handler(uint8_t conidx)
{
    app_dma_update_conn_parameter(DMA_BLE_CONNECTION_INTERVAL_MIN_IN_MS, 
        DMA_BLE_CONNECTION_INTERVAL_MAX_IN_MS, 
        DMA_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS,
        DMA_BLE_CONNECTION_SLAVELATENCY);
	
	app_dma_env.connectionIndex = conidx;
}

void app_dma_disconnected_evt_handler(void)
{
	app_dma_env.connectionIndex = BLE_INVALID_CONNECTION_INDEX;
	app_dma_env.isCmdNotificationEnabled = false;
	app_dma_env.isDataNotificationEnabled = false;

#ifdef __DMA_VOICE__
    dma_ble_disconnected();
#endif
}

void app_dma_init(void)
{
    // Reset the environment
	app_dma_env.connectionIndex =  BLE_INVALID_CONNECTION_INDEX;
	app_dma_env.isCmdNotificationEnabled = false;
	app_dma_env.isDataNotificationEnabled = false;
}

void app_dma_add_dma(void)
{
	BLE_APP_DBG("app_dma_add_dma");
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, 0);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, ENABLE);
    req->prf_task_id = TASK_ID_DMA;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_dma_send_cmd_via_notification(uint8_t* ptrData, uint32_t length)
{
	struct ble_dma_send_data_req_t * req = KE_MSG_ALLOC_DYN(DMA_SEND_CMD_VIA_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_DMA),
                                                TASK_APP,
                                                ble_dma_send_data_req_t,
                                                length);
	req->connecionIndex = app_dma_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_dmav_send_cmd_via_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_dma_send_data_req_t * req = KE_MSG_ALLOC_DYN(DMA_SEND_CMD_VIA_INDICATION,
                                                prf_get_task_from_id(TASK_ID_DMA),
                                                TASK_APP,
                                                ble_dma_send_data_req_t,
                                                length);
	req->connecionIndex = app_dma_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_dma_send_data_via_notification(uint8_t* ptrData, uint32_t length)
{
	struct ble_dma_send_data_req_t * req = KE_MSG_ALLOC_DYN(DMA_SEND_DATA_VIA_NOTIFICATION,
                                                prf_get_task_from_id(TASK_ID_DMA),
                                                TASK_APP,
                                                ble_dma_send_data_req_t,
                                                length);
	req->connecionIndex = app_dma_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

void app_dma_send_data_via_indication(uint8_t* ptrData, uint32_t length)
{
	struct ble_dma_send_data_req_t * req = KE_MSG_ALLOC_DYN(DMA_SEND_DATA_VIA_INDICATION,
                                                prf_get_task_from_id(TASK_ID_DMA),
                                                TASK_APP,
                                                ble_dma_send_data_req_t,
                                                length);
	req->connecionIndex = app_dma_env.connectionIndex;
	req->length = length;
	memcpy(req->value, ptrData, length);

	ke_msg_send(req);
}

static int app_dma_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    // Do nothing
    return (KE_MSG_CONSUMED);
}

static int app_dma_cmd_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_dma_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("dma cmd ccc changed to %d", param->isNotificationEnabled);
    app_dma_env.isCmdNotificationEnabled = param->isNotificationEnabled;
    return (KE_MSG_CONSUMED);
}

static int app_dma_data_ccc_changed_handler(ke_msg_id_t const msgid,
                              struct ble_dma_tx_notif_config_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	TRACE("dma data ccc changed to %d", param->isNotificationEnabled);
#ifdef __DMA_VOICE__
	dma_ble_connected();
	dma_ble_ccc_changed(param->isNotificationEnabled);

#endif
    app_dma_env.isDataNotificationEnabled = param->isNotificationEnabled;
	
    return (KE_MSG_CONSUMED);
}

static int app_dma_tx_data_sent_handler(ke_msg_id_t const msgid,
                              struct ble_dma_tx_sent_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
    if (NULL != tx_done_callback)
    {
		tx_done_callback();
    }

    return (KE_MSG_CONSUMED);
}

static int app_dma_rx_cmd_received_handler(ke_msg_id_t const msgid,
                              struct ble_dma_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{

    return (KE_MSG_CONSUMED);
}

static int app_dma_rx_data_received_handler(ke_msg_id_t const msgid,
                              struct ble_dma_rx_data_ind_t *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
	dma_data_received(param->data, param->length);

    return (KE_MSG_CONSUMED);
}

void app_dma_register_tx_done(app_dma_tx_done_t callback)
{
	tx_done_callback = callback;
}

void app_dma_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency)
{
	struct gapc_conn_param conn_param;

	conn_param.intv_min = (uint16_t)(min_interval_in_ms/1.25); 
	conn_param.intv_max = (uint16_t)(max_interval_in_ms/1.25); 
	conn_param.latency	= slaveLantency;
	conn_param.time_out = supervision_timeout_in_ms/10; 

	l2cap_update_param(&conn_param);
}

void app_dma_mtu_exchanged_handler(uint16_t MTU)
{
#ifdef __DMA_VOICE__
	dma_ble_mtu_changed(MTU);
#endif	
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_dma_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,    (ke_msg_func_t)app_dma_msg_handler},

    {DMA_CMD_CCC_CHANGED,     	(ke_msg_func_t)app_dma_cmd_ccc_changed_handler},
	{DMA_DATA_CCC_CHANGED,     	(ke_msg_func_t)app_dma_data_ccc_changed_handler},
    {DMA_TX_DATA_SENT,    		(ke_msg_func_t)app_dma_tx_data_sent_handler},
    {DMA_CMD_RECEIVED,   		(ke_msg_func_t)app_dma_rx_cmd_received_handler},
    {DMA_DATA_RECEIVED, 		(ke_msg_func_t)app_dma_rx_data_received_handler},
};

const struct ke_state_handler app_dma_table_handler =
    {&app_dma_msg_handler_list[0], (sizeof(app_dma_msg_handler_list)/sizeof(struct ke_msg_handler))};

#endif //BLE_APP_DMA

/// @} APP
