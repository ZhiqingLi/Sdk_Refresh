#ifndef _AMA_GATT_SERVER_H_
#define _AMA_GATT_SERVER_H_

/**
 ****************************************************************************************
 * @addtogroup SMARTVOICE Datapath Profile Server
 * @ingroup SMARTVOICEP
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

#include <stdint.h>
#include "rwip_task.h" // Task definitions


#if (BLE_AMA_VOICE)
#include "prf_types.h"
#include "prf.h"
#include "attm.h"
#include "prf_utils.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define SMARTVOICE_MAX_LEN            			(509)

/*
 * DEFINES
 ****************************************************************************************
 */
/// Possible states of the SMARTVOICE task
enum
{
    /// Idle state
    AMA_IDLE,
    /// Connected state
    AMA_BUSY,

    /// Number of defined states.
    AMA_STATE_MAX,
};

///Attributes State Machine
enum
{
    AMA_IDX_SVC,
		
    AMA_IDX_TX_CHAR,
    AMA_IDX_TX_VAL,
    AMA_IDX_TX_NTF_CFG,
	AMA_IDX_RX_CHAR,
	AMA_IDX_RX_VAL,

    AMA_IDX_NB,

};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Datapath Profile Server environment variable
struct ama_env_tag
{
    /// profile environment
    prf_env_t 	prf_env;
	uint8_t		isCmdNotificationEnabled;
    /// Service Start Handle
    uint16_t 	shdl;
    /// State of different task instances
    ke_state_t 	state;
};


#define AMA_CONTENT_TYPE_COMMAND		0 
#define AMA_CONTENT_TYPE_DATA		1

/// Messages for Smart Voice Profile 
enum sv_msg_id
{
	AMA_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_AMA),

	AMA_TX_DATA_SENT,
	
	AMA_RECEIVED,

	AMA_SEND_VIA_NOTIFICATION,

	AMA_SEND_VIA_INDICATION,

};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct ble_ama_tx_notif_config_t
{
	bool 		isNotificationEnabled;
};

struct ble_ama_rx_data_ind_t
{
	uint16_t	length;
	uint8_t		data[0];
};

struct ble_ama_tx_sent_ind_t
{
	uint8_t 	status;
};

struct ble_ama_send_data_req_t
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
const struct prf_task_cbs* ama_prf_itf_get(void);


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
void ama_task_init(struct ke_task_desc *task_desc);



#endif /* #if (BLE_AMA_VOICE) */



#endif

