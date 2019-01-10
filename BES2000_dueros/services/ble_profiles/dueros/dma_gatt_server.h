#ifndef _DMA_VOICE_H_
#define _DMA_VOICE_H_

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


#if (BLE_DMA_VOICE)
#include "prf_types.h"
#include "prf.h"
#include "attm.h"
#include "prf_utils.h"


/*
 * DEFINES
 ****************************************************************************************
 */
#define DMA_MAX_LEN            			(244)

/*
 * DEFINES
 ****************************************************************************************
 */
/// Possible states of the SMARTVOICE task
enum
{
    /// Idle state
    DMA_IDLE,
    /// Connected state
    DMA_BUSY,

    /// Number of defined states.
    DMA_STATE_MAX,
};

///Attributes State Machine
enum
{
    DMA_IDX_SVC,

    DMA_IDX_CMD_TX_CHAR,
    DMA_IDX_CMD_TX_VAL,
    DMA_IDX_CMD_TX_NTF_CFG,

    DMA_IDX_CMD_RX_CHAR,
    DMA_IDX_CMD_RX_VAL,

    DMA_IDX_DATA_TX_CHAR,
    DMA_IDX_DATA_TX_VAL,
    DMA_IDX_DATA_TX_NTF_CFG,

    DMA_IDX_DATA_RX_CHAR,
    DMA_IDX_DATA_RX_VAL,

    DMA_IDX_NB,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Datapath Profile Server environment variable
struct dma_env_tag
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


#define DMA_CONTENT_TYPE_COMMAND		0 
#define DMA_CONTENT_TYPE_DATA		1

/// Messages for Smart Voice Profile 
enum dma_msg_id
{
	DMA_CMD_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_DMA),

	DMA_TX_DATA_SENT,
	
	DMA_CMD_RECEIVED,

	DMA_SEND_CMD_VIA_NOTIFICATION,

	DMA_SEND_CMD_VIA_INDICATION,

	DMA_DATA_CCC_CHANGED,

	DMA_DATA_RECEIVED,

	DMA_SEND_DATA_VIA_NOTIFICATION,

	DMA_SEND_DATA_VIA_INDICATION,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct ble_dma_tx_notif_config_t
{
	bool 		isNotificationEnabled;
};

struct ble_dma_rx_data_ind_t
{
	uint16_t	length;
	uint8_t		data[0];
};

struct ble_dma_tx_sent_ind_t
{
	uint8_t 	status;
};

struct ble_dma_send_data_req_t
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
const struct prf_task_cbs* dma_prf_itf_get(void);


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
void dma_task_init(struct ke_task_desc *task_desc);



#endif /* #if (BLE_SMARTVOICE) */

/// @} DMA

#endif /* _SMARTVOICE_H_ */

