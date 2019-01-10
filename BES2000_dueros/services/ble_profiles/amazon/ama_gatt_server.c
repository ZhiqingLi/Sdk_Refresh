/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_AMA_VOICE)
#include "gap.h"
#include "gattc_task.h"
#include "attm.h"
#include "gapc_task.h"
#include "ama_gatt_server.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "co_utils.h"

/*
 * AMA CMD PROFILE ATTRIBUTES
 ****************************************************************************************
 */

#define ama_service_uuid_128_content  		{0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x03,0xFE,0x00,0x00}
#define ama_rx_char_val_uuid_128_content  	{0x76,0x30,0xF8,0xDD,0x90,0xA3,0x61,0xAC,0xA7,0x43,0x05,0x30,0x77,0xB1,0x4E,0xF0}
#define ama_tx_char_val_uuid_128_content  	{0x0B,0x42,0x82,0x1F,0x64,0x72,0x2F,0x8A,0xB4,0x4B,0x79,0x18,0x5B,0xA0,0xEE,0x2B}	


#define ATT_DECL_PRIMARY_SERVICE_UUID		{ 0x00, 0x28 }
#define ATT_DECL_CHARACTERISTIC_UUID		{ 0x03, 0x28 }
#define ATT_DESC_CLIENT_CHAR_CFG_UUID		{ 0x02, 0x29 }

static const uint8_t AMA_SERVICE_UUID_128[ATT_UUID_128_LEN]	= ama_service_uuid_128_content;  

/// Full AMA SERVER Database Description - Used to add attributes into the database
const struct attm_desc_128 ama_att_db[AMA_IDX_NB] =
{
    // Service Declaration
    [AMA_IDX_SVC] 		=   {ATT_DECL_PRIMARY_SERVICE_UUID, PERM(RD, ENABLE), 0, 0},

    // Command RX Characteristic Declaration
    [AMA_IDX_RX_CHAR]    =   {ATT_DECL_CHARACTERISTIC_UUID, PERM(RD, ENABLE), 0, 0},
    // Command RX Characteristic Value
    [AMA_IDX_RX_VAL]     =   {ama_rx_char_val_uuid_128_content, 
    	PERM(WRITE_REQ, ENABLE), //| PERM(WRITE_COMMAND, ENABLE) | PERM(WP, UNAUTH), 
    	PERM(RI, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128), SMARTVOICE_MAX_LEN},

    // Command TX Characteristic Declaration
    [AMA_IDX_TX_CHAR] 	=   {ATT_DECL_CHARACTERISTIC_UUID, PERM(RD, ENABLE), 0, 0},
    // Command TX Characteristic Value
    [AMA_IDX_TX_VAL]     =   {ama_tx_char_val_uuid_128_content, PERM(NTF, ENABLE) | PERM(RD, ENABLE) ,//| PERM(RP, UNAUTH), 
    	PERM(RI, ENABLE) | PERM_VAL(UUID_LEN, PERM_UUID_128), SMARTVOICE_MAX_LEN},
    // Command TX Characteristic - Client Characteristic Configuration Descriptor
    [AMA_IDX_TX_NTF_CFG] =   {ATT_DESC_CLIENT_CHAR_CFG_UUID, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},

};

/**
 ****************************************************************************************
 * @brief Initialization of the SMARTVOICE module.
 * This function performs all the initializations of the Profile module.
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Collector or Service allocated environment data.
 * @param[in|out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @see enum attm_value_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeed or not.
 ****************************************************************************************
 */
static uint8_t ama_init(struct prf_task_env* env, uint16_t* start_hdl, 
	uint16_t app_task, uint8_t sec_lvl, void* params)
{
	uint8_t status;
		
    //Add Service Into Database
    status = attm_svc_create_db_128(start_hdl, AMA_SERVICE_UUID_128, NULL,
            AMA_IDX_NB, NULL, env->task, &ama_att_db[0],
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS))| PERM(SVC_MI, DISABLE)
            | PERM_VAL(SVC_UUID_LEN, PERM_UUID_128));


	BLE_GATT_DBG("attm_ama_create_db_128 returns %d start handle is %d", status, *start_hdl);

    //-------------------- allocate memory required for the profile  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {
        // Allocate SMARTVOICE required environment variable
        struct ama_env_tag* ama_env =
                (struct ama_env_tag* ) ke_malloc(sizeof(struct ama_env_tag), KE_MEM_ATT_DB);

        // Initialize AMA environment
        env->env           = (prf_env_t*) ama_env;
        ama_env->shdl     = *start_hdl;

        ama_env->prf_env.app_task    = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        // Mono Instantiated task
        ama_env->prf_env.prf_task    = env->task | PERM(PRF_MI, DISABLE);

        // initialize environment variable
        env->id                     = TASK_ID_AMA;
        ama_task_init(&(env->desc));

        /* Put HRS in Idle state */
        ke_state_set(env->task, AMA_IDLE);
    }

    return (status);
}

/**
 ****************************************************************************************
 * @brief Destruction of the SMARTVOICE module - due to a reset for instance.
 * This function clean-up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 ****************************************************************************************
 */
static void ama_destroy(struct prf_task_env* env)
{
    struct ama_env_tag* ama_env = (struct ama_env_tag*) env->env;

    // free profile environment variables
    env->env = NULL;
    ke_free(ama_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 ****************************************************************************************
 */
static void ama_create(struct prf_task_env* env, uint8_t conidx)
{
	struct ama_env_tag* ama_env = (struct ama_env_tag*) env->env;
	struct prf_svc ama_svc = {ama_env->shdl, ama_env->shdl + AMA_IDX_NB};
	prf_register_atthdl2gatt(env->env, conidx, &ama_svc);
}

/**
 ****************************************************************************************
 * @brief Handles Disconnection
 *
 * @param[in|out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 ****************************************************************************************
 */
static void ama_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    /* Nothing to do */
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// AMA Task interface required by profile manager
const struct prf_task_cbs ama_itf =
{
    (prf_init_fnct) ama_init,
    ama_destroy,
    ama_create,
    ama_cleanup,
};

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs* ama_prf_itf_get(void)
{
   return &ama_itf;
}


/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(ke_msg_id_t const msgid,
									  struct gapc_disconnect_ind const *param,
									  ke_task_id_t const dest_id,
									  ke_task_id_t const src_id)
{
	struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);
	ama_env->isCmdNotificationEnabled = false;
	return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GL2C_CODE_ATT_WR_CMD_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	
    // Get the address of the environment
    struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);
    uint8_t conidx = KE_IDX_GET(src_id);

    uint8_t status = GAP_ERR_NO_ERROR;

	BLE_GATT_DBG("gattc_write_req_ind_handler ama_env 0x%x write handle %d shdl %d", 
		ama_env, param->handle, ama_env->shdl);

    //Send write response
    struct gattc_write_cfm *cfm = KE_MSG_ALLOC(
            GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = param->handle;
    cfm->status = status;
    ke_msg_send(cfm);

    if (ama_env != NULL)
    {
		if (param->handle == (ama_env->shdl + AMA_IDX_TX_NTF_CFG))
		{
			uint16_t value = 0x0000;
			
            //Extract value before check
            memcpy(&value, &(param->value), sizeof(uint16_t));

            if (value == PRF_CLI_STOP_NTFIND)
            {
                ama_env->isCmdNotificationEnabled = false;
            }
			else if (value == PRF_CLI_START_NTF)
			{
				ama_env->isCmdNotificationEnabled = true;
			}
            else
            {
                status = PRF_APP_ERROR;
            }

            if (status == GAP_ERR_NO_ERROR)
            {
                //Inform APP of TX ccc change
                struct ble_ama_tx_notif_config_t * ind = KE_MSG_ALLOC(AMA_CCC_CHANGED,
                        prf_dst_task_get(&ama_env->prf_env, conidx),
                        prf_src_task_get(&ama_env->prf_env, conidx),
                        ble_ama_tx_notif_config_t);

                ind->isNotificationEnabled = ama_env->isCmdNotificationEnabled;

                ke_msg_send(ind);
            }
		}
		else if (param->handle == (ama_env->shdl + AMA_IDX_RX_VAL))
		{
			//inform APP of data
            struct ble_ama_rx_data_ind_t * ind = KE_MSG_ALLOC_DYN(AMA_RECEIVED,
                    prf_dst_task_get(&ama_env->prf_env, conidx),
                    prf_src_task_get(&ama_env->prf_env, conidx),
                    ble_ama_rx_data_ind_t,
                    sizeof(struct ble_ama_rx_data_ind_t) + param->length);

			ind->length = param->length;
			memcpy((uint8_t *)(ind->data), &(param->value), param->length);

            ke_msg_send(ind);	
		}
		else
		{
			status = PRF_APP_ERROR;
		}
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY message meaning that Measurement
 * notification has been correctly sent to peer device (but not confirmed by peer device).
 * *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
	// Get the address of the environment
    struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);

	uint8_t conidx = KE_IDX_GET(dest_id);
		
	// notification or write request has been sent out
    if ((GATTC_NOTIFY == param->operation) || (GATTC_INDICATE == param->operation))
    {
		
        struct ble_ama_tx_sent_ind_t * ind = KE_MSG_ALLOC(AMA_TX_DATA_SENT,
                prf_dst_task_get(&ama_env->prf_env, conidx),
                prf_src_task_get(&ama_env->prf_env, conidx),
                ble_ama_tx_sent_ind_t);

		ind->status = param->status;

        ke_msg_send(ind);

    }

	ke_state_set(dest_id, AMA_IDLE);
	
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the read request from peer device
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
__STATIC int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{	
    // Get the address of the environment
    struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);

	struct gattc_read_cfm* cfm;

	uint8_t status = GAP_ERR_NO_ERROR;

	BLE_GATT_DBG("gattc_read_req_ind_handler read handle %d shdl %d", param->handle, ama_env->shdl);

    if (param->handle == (ama_env->shdl + AMA_IDX_TX_NTF_CFG))
	{
		uint16_t notify_ccc;
		cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(notify_ccc));
		
		if (ama_env->isCmdNotificationEnabled)
		{
			notify_ccc = 1;
		}
		else
		{
			notify_ccc = 0;
		}
		cfm->length = sizeof(notify_ccc);
		memcpy(cfm->value, (uint8_t *)&notify_ccc, cfm->length);
	}
	else
	{
		cfm = KE_MSG_ALLOC(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm);
		cfm->length = 0;
		if (param->handle != (ama_env->shdl + AMA_IDX_TX_VAL))
		{
			status = ATT_ERR_REQUEST_NOT_SUPPORTED;
		}
	}

	cfm->handle = param->handle;

	cfm->status = status;

	ke_msg_send(cfm);

	ke_state_set(dest_id, AMA_IDLE);

    return (KE_MSG_CONSUMED);
}

static void send_notifiction(uint8_t conidx,const uint8_t* ptrData, uint32_t length)
{
	uint16_t handle_offset = 0xFFFF;
	struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);


	if(ama_env->isCmdNotificationEnabled)
	{
		handle_offset = AMA_IDX_TX_VAL;
	}


	if (0xFFFF != handle_offset)
	{
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *report_ntf = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(&ama_env->prf_env, conidx),
                gattc_send_evt_cmd, length);

        // Fill in the parameter structure
        report_ntf->operation = GATTC_NOTIFY;
        report_ntf->handle    = ama_env->shdl + handle_offset;
        // pack measured value in database
        report_ntf->length    = length;
        memcpy(report_ntf->value, ptrData, length);
        // send notification to peer device
        ke_msg_send(report_ntf);
	}
}

static void send_indication(uint8_t conidx, const uint8_t* ptrData, uint32_t length)
{
	uint16_t handle_offset = 0xFFFF;
	struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);

	if(ama_env->isCmdNotificationEnabled)
	{
		handle_offset = AMA_IDX_TX_VAL;
	}


	if (0xFFFF != handle_offset)
	{
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *report_ntf = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(&ama_env->prf_env, conidx),
                gattc_send_evt_cmd, length);

        // Fill in the parameter structure
        report_ntf->operation = GATTC_INDICATE;
        report_ntf->handle    = ama_env->shdl + handle_offset;
        // pack measured value in database
        report_ntf->length    = length;
        memcpy(report_ntf->value, ptrData, length);
        // send notification to peer device
        ke_msg_send(report_ntf);
	}
}

__STATIC int ama_send_via_notification_handler(ke_msg_id_t const msgid,
                                      struct ble_ama_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	send_notifiction(param->connecionIndex,param->value, param->length);
	return (KE_MSG_CONSUMED);
}

__STATIC int ama_send_via_indication_handler(ke_msg_id_t const msgid,
                                      struct ble_ama_send_data_req_t const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	send_indication(param->connecionIndex,param->value, param->length);
	return (KE_MSG_CONSUMED);
}




/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
        struct gattc_att_info_req_ind *param,
        ke_task_id_t const dest_id,
        ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct gattc_att_info_cfm * cfm;

    //Send write response
    cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id, dest_id, gattc_att_info_cfm);
    cfm->handle = param->handle;

	struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);

	if (param->handle == (ama_env->shdl + AMA_IDX_TX_NTF_CFG))
	{
		// CCC attribute length = 2
        cfm->length = 2;
        cfm->status = GAP_ERR_NO_ERROR;
	}
	else if (param->handle == (ama_env->shdl + AMA_IDX_RX_VAL))
	{
        // force length to zero to reject any write starting from something != 0
        cfm->length = 0;
        cfm->status = GAP_ERR_NO_ERROR;			
	}
	else
	{
		cfm->length = 0;
    	cfm->status = ATT_ERR_WRITE_NOT_PERMITTED;
	}

    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/* Default State handlers definition. */
KE_MSG_HANDLER_TAB(ama)
{
	{GAPC_DISCONNECT_IND,		(ke_msg_func_t) gapc_disconnect_ind_handler},
    {GATTC_WRITE_REQ_IND,    	(ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_CMP_EVT,          	(ke_msg_func_t) gattc_cmp_evt_handler},
    {GATTC_READ_REQ_IND,     	(ke_msg_func_t) gattc_read_req_ind_handler},
    {AMA_SEND_VIA_NOTIFICATION, (ke_msg_func_t) ama_send_via_notification_handler},
	{AMA_SEND_VIA_INDICATION,   (ke_msg_func_t) ama_send_via_indication_handler},		
	{GATTC_ATT_INFO_REQ_IND,    (ke_msg_func_t) gattc_att_info_req_ind_handler },
};

void ama_task_init(struct ke_task_desc *task_desc)
{
    // Get the address of the environment
    struct ama_env_tag *ama_env = PRF_ENV_GET(AMA, ama);

    task_desc->msg_handler_tab = ama_msg_handler_tab;
    task_desc->msg_cnt         = ARRAY_LEN(ama_msg_handler_tab);
    task_desc->state           = &(ama_env->state);
    task_desc->idx_max         = 1;
}


#endif /* BLE_AMA */



