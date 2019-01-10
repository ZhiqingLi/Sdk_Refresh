/**
 ****************************************************************************************
 *
 * @file ota.h
 *
 * @brief Header file - Smart Voice Profile Server.
 *
 * Copyright (C) BES
 *
 *
 ****************************************************************************************
 */

#ifndef _OTA_GATT_SERVER_H_
#define _OTA_GATT_SERVER_H_

/**
 ****************************************************************************************
 * @addtogroup OTA Datapath Profile Server
 * @ingroup OTAP
 * @brief Datapath Profile Server
 *
 * Datapath Profile Server provides functionalities to upper layer module
 * application. The device using this profile takes the role of Datapath Server.
 *
 * The interface of this role to the Application is:
 *  - Enable the profile role (from APP)
 *  - Disable the profile role (from APP)
 *  - Send data to peer device via notifications  (from APP)
 *  - Receive data from peer device via write no response (from APP)
 *
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_OTA)
#include "prf_types.h"
#include "prf.h"
#include "attm.h"
#include "prf_utils.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define OTA_MAX_LEN            			(509)	

/*
 * DEFINES
 ****************************************************************************************
 */
/// Possible states of the OTA task
enum
{
    /// Idle state
    OTA_IDLE,
    /// Connected state
    OTA_BUSY,

    /// Number of defined states.
    OTA_STATE_MAX,
};

///Attributes State Machine
enum
{
    OTA_IDX_SVC,

    OTA_IDX_CHAR,
    OTA_IDX_VAL,
    OTA_IDX_NTF_CFG,

    OTA_IDX_NB,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Datapath Profile Server environment variable
struct ota_env_tag
{
    /// profile environment
    prf_env_t 	prf_env;
	uint8_t		isNotificationEnabled;
    /// Service Start Handle
    uint16_t 	shdl;
    /// State of different task instances
    ke_state_t 	state;
};

#define OTA_CONTENT_TYPE_COMMAND		0 
#define OTA_CONTENT_TYPE_DATA		1

/// Messages for OTA Profile 
enum ota_msg_id
{
	OTA_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_OTA),

	OTA_TX_DATA_SENT,
	
	OTA_DATA_RECEIVED,

	OTA_SEND_NOTIFICATION,

	OTA_SEND_INDICATION,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct ble_ota_tx_notif_config_t
{
	bool 		isNotificationEnabled;
};

struct ble_ota_rx_data_ind_t
{
	uint16_t	length;
	uint8_t		data[0];
};

struct ble_ota_tx_sent_ind_t
{
	uint8_t 	status;
};

struct ble_ota_send_data_req_t
{
	uint8_t 	connecionIndex;
	uint32_t 	length;
	uint8_t  	value[__ARRAY_EMPTY];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve HRP service profile interface
 *
 * @return HRP service profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* ota_prf_itf_get(void);


/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * Initialize task handler
 *
 * @param task_desc Task descriptor to fill
 ****************************************************************************************
 */
void ota_task_init(struct ke_task_desc *task_desc);



#endif /* #if (BLE_OTA) */

/// @} OTA

#endif /* _OTA_H_ */

