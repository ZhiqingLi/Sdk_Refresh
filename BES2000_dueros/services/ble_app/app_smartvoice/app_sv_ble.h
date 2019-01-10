#ifndef APP_SMARTVOICE_BLE_H_
#define APP_SMARTVOICE_BLE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Smart Voice Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_SMARTVOICE)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

#define SV_BLE_CONNECTION_INTERVAL_MIN_IN_MS			20
#define SV_BLE_CONNECTION_INTERVAL_MAX_IN_MS			30
#define SV_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS		20000
#define SV_BLE_CONNECTION_SLAVELATENCY					0

extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_sv_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isCmdNotificationEnabled;
	uint8_t	isDataNotificationEnabled;
};

typedef void(*app_sv_tx_done_t)(void);

typedef void(*app_sv_activity_stopped_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_sv_env_tag app_sv_env;

/// Table of message handlers
extern const struct ke_state_handler app_sv_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_sv_init(void);

/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
void app_sv_add_sv(void);

void app_sv_connected_evt_handler(uint8_t conidx);

void app_sv_disconnected_evt_handler(void);

void app_sv_send_cmd_via_notification(uint8_t* ptrData, uint32_t length);

void app_sv_send_cmd_via_indication(uint8_t* ptrData, uint32_t length);

void app_sv_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_sv_send_data_via_indication(uint8_t* ptrData, uint32_t length);

void app_sv_register_tx_done(app_sv_tx_done_t callback);

void app_sv_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_sv_mtu_exchanged_handler(uint16_t MTU);

#ifdef __cplusplus
	}
#endif


#endif //(BLE_APP_SMARTVOICE)

/// @} APP

#endif // APP_SMARTVOICE_H_
