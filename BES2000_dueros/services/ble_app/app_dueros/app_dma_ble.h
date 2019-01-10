#ifndef APP_DMA_BLE_H_
#define APP_DMA_BLE_H_

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

#if (BLE_APP_DMA_VOICE)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
#ifdef __cplusplus
extern "C" {
#endif

#define DMA_BLE_CONNECTION_INTERVAL_MIN_IN_MS			10
#define DMA_BLE_CONNECTION_INTERVAL_MAX_IN_MS			15
#define DMA_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS		20000
#define DMA_BLE_CONNECTION_SLAVELATENCY					0

extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_dma_env_tag
{
    uint8_t connectionIndex;
	uint8_t	isCmdNotificationEnabled;
	uint8_t	isDataNotificationEnabled;
};

typedef void(*app_dma_tx_done_t)(void);

typedef void(*app_dma_activity_stopped_t)(void);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_dma_env_tag app_dma_env;

/// Table of message handlers
extern const struct ke_state_handler app_dma_table_handler;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize DataPath Server Application
 ****************************************************************************************
 */
void app_dma_init(void);

/**
 ****************************************************************************************
 * @brief Add a DataPath Server instance in the DB
 ****************************************************************************************
 */
void app_dma_add_dma(void);

void app_dma_connected_evt_handler(uint8_t conidx);

void app_dma_disconnected_evt_handler(void);

void app_dma_send_cmd_via_notification(uint8_t* ptrData, uint32_t length);

void app_dma_send_cmd_via_indication(uint8_t* ptrData, uint32_t length);

void app_dma_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_dma_send_data_via_indication(uint8_t* ptrData, uint32_t length);

void app_dma_register_tx_done(app_dma_tx_done_t callback);

void app_dma_update_conn_parameter(uint32_t min_interval_in_ms, uint32_t max_interval_in_ms,
		uint32_t supervision_timeout_in_ms, uint8_t slaveLantency);

void app_dma_mtu_exchanged_handler(uint16_t MTU);

#ifdef __cplusplus
	}
#endif


#endif //(BLE_APP_DMA_VOICE)

/// @} APP

#endif // APP_DMA_VOICE_H_
