#ifndef APP_AMA_BLE_H_
#define APP_AMA_BLE_H_

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

#if (BLE_APP_AMA_VOICE)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

#define AMA_BLE_CONNECTION_INTERVAL_MIN_IN_MS			10
#define AMA_BLE_CONNECTION_INTERVAL_MAX_IN_MS			15
#define AMA_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS		20000
#define AMA_BLE_CONNECTION_SLAVELATENCY					0

extern struct app_env_tag app_env;

/// ama application environment structure
struct app_ama_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isCmdNotificationEnabled;
};

typedef void(*app_ama_tx_done_t)(void);

typedef void(*app_ama_activity_stopped_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_sv_env_tag app_sv_env;

/// Table of message handlers
extern const struct ke_state_handler app_ama_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_ama_init(void);

/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
void app_ama_add_ama(void);

void app_ama_connected_evt_handler(uint8_t conidx);
void app_ama_disconnected_evt_handler(void);

void app_ama_send_via_notification(uint8_t* ptrData, uint32_t length);
void app_ama_send_via_indication(uint8_t* ptrData, uint32_t length);


void app_ama_register_tx_done(app_ama_tx_done_t callback);

void app_ama_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_ama_mtu_exchanged_handler(uint16_t MTU);

#ifdef __cplusplus
	}
#endif


#endif //(BLE_APP_AMA_VOICE)

/// @} APP

#endif // APP_AMA_VOICE_H_
