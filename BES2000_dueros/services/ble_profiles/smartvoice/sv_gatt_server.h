#ifndef _SMARTVOICE_H_
#define _SMARTVOICE_H_

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


#if (BLE_SMARTVOICE)
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
    SMARTVOICE_IDLE,
    /// Connected state
    SMARTVOICE_BUSY,

    /// Number of defined states.
    SMARTVOICE_STATE_MAX,
};

///Attributes State Machine
enum
{
    SMARTVOICE_IDX_SVC,

    SMARTVOICE_IDX_CMD_TX_CHAR,
    SMARTVOICE_IDX_CMD_TX_VAL,
    SMARTVOICE_IDX_CMD_TX_NTF_CFG,

    SMARTVOICE_IDX_CMD_RX_CHAR,
    SMARTVOICE_IDX_CMD_RX_VAL,

    SMARTVOICE_IDX_DATA_TX_CHAR,
    SMARTVOICE_IDX_DATA_TX_VAL,
    SMARTVOICE_IDX_DATA_TX_NTF_CFG,

    SMARTVOICE_IDX_DATA_RX_CHAR,
    SMARTVOICE_IDX_DATA_RX_VAL,

    SMARTVOICE_IDX_NB,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Datapath Profile Server environment variable
struct sv_env_tag
{
    /// profile environment
    prf_env_t 	prf_env;
	uint8_t		isCmdNotificationEnabled;
	uint8_t		isDataNotificationEnabled;
    /// Service Start Handle
    uint16_t 	shdl;
    /// State of different task instances
    ke_state_t 	state;
};


#define SMARTVOICE_CONTENT_TYPE_COMMAND		0 
#define SMARTVOICE_CONTENT_TYPE_DATA		1

/// Messages for Smart Voice Profile 
enum sv_msg_id
{
	SMARTVOICE_CMD_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_SMARTVOICE),

	SMARTVOICE_TX_DATA_SENT,
	
	SMARTVOICE_CMD_RECEIVED,

	SMARTVOICE_SEND_CMD_VIA_NOTIFICATION,

	SMARTVOICE_SEND_CMD_VIA_INDICATION,

	SMARTVOICE_DATA_CCC_CHANGED,

	SMARTVOICE_DATA_RECEIVED,

	SMARTVOICE_SEND_DATA_VIA_NOTIFICATION,

	SMARTVOICE_SEND_DATA_VIA_INDICATION,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct ble_sv_tx_notif_config_t
{
	bool 		isNotificationEnabled;
};

struct ble_sv_rx_data_ind_t
{
	uint16_t	length;
	uint8_t		data[0];
};

struct ble_sv_tx_sent_ind_t
{
	uint8_t 	status;
};

struct ble_sv_send_data_req_t
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
const struct prf_task_cbs* sv_prf_itf_get(void);


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
void sv_task_init(struct ke_task_desc *task_desc);



#endif /* #if (BLE_SMARTVOICE) */

/// @} SMARTVOICE

#endif /* _SMARTVOICE_H_ */

