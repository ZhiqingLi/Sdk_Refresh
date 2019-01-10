/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include <string.h>
#include "co_math.h"
#include "gapc_task.h"      // GAP Controller Task API Definition
#include "gap.h"            // GAP Definition
#include "gapc.h"           // GAPC Definition
#include "prf_types.h"

#include "app.h"            // Application API Definition
#include "app_sec.h"        // Application Security API Definition
#include "app_task.h"       // Application Manager API Definition

#if (DISPLAY_SUPPORT)
#include "app_display.h"    // Display Application Definitions
#endif //(DISPLAY_SUPPORT)

#if (NVDS_SUPPORT)
#include "nvds.h"           // NVDS API Definitions
#endif //(NVDS_SUPPORT)

#ifdef BLE_APP_AM0
#include "am0_app.h"
#endif // BLE_APP_AM0

#define _BLE_NVDS_

#ifdef _BLE_NVDS_
#define BLE_KEY_PRINT
#include "nvrecord_ble.h"
BleDeviceinfo ble_save_info;
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Security Environment Structure
struct app_sec_env_tag app_sec_env;

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_sec_init()
{
    /*------------------------------------------------------
     * RETRIEVE BOND STATUS
     *------------------------------------------------------*/
    #if (NVDS_SUPPORT)
    uint8_t length = NVDS_LEN_PERIPH_BONDED;

    // Get bond status from NVDS
    if (nvds_get(NVDS_TAG_PERIPH_BONDED, &length, (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
    {
        // If read value is invalid, set status to not bonded
        app_sec_env.bonded = false;
    }

    if ((app_sec_env.bonded != true) && (app_sec_env.bonded != false))
    {
        app_sec_env.bonded = false;
    }

    #if (DISPLAY_SUPPORT)
    // Update the bond status screen value
    app_display_set_bond(app_sec_env.bonded);
    #endif //(DISPLAY_SUPPORT)
    #endif //(NVDS_SUPPORT)
#ifdef _BLE_NVDS_
	if(nv_record_ble_record_Once_a_device_has_been_bonded()){
		app_sec_env.bonded = true;
	}else{
		app_sec_env.bonded = false;
	}
#endif	
}

bool app_sec_get_bond_status(void)
{
    return app_sec_env.bonded;
}

#if (NVDS_SUPPORT)
void app_sec_remove_bond(void)
{
    #if (BLE_APP_HID)
    uint16_t ntf_cfg = PRF_CLI_STOP_NTFIND;
    #endif //(BLE_APP_HID)

    // Check if we are well bonded
    if (app_sec_env.bonded == true)
    {
        // Update the environment variable
        app_sec_env.bonded = false;

        if (nvds_put(NVDS_TAG_PERIPH_BONDED, NVDS_LEN_PERIPH_BONDED,
                     (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }

        #if (BLE_APP_HT)
        if (nvds_del(NVDS_TAG_LTK) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }

        if (nvds_del(NVDS_TAG_PEER_BD_ADDRESS) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }
        #endif //(BLE_APP_HT)

        #if (BLE_APP_HID)
        if (nvds_put(NVDS_TAG_MOUSE_NTF_CFG, NVDS_LEN_MOUSE_NTF_CFG,
                     (uint8_t *)&ntf_cfg) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }
        #endif //(BLE_APP_HID)
    }
}
#endif //(NVDS_SUPPORT)

void app_sec_send_security_req(uint8_t conidx, enum gap_auth authority)
{
    // Send security request
    struct gapc_security_cmd *cmd = KE_MSG_ALLOC(GAPC_SECURITY_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
                                                 gapc_security_cmd);

    cmd->operation = GAPC_SECURITY_REQ;

	cmd->auth = authority;

    // Send the message
    ke_msg_send(cmd);
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

static int gapc_bond_req_ind_handler(ke_msg_id_t const msgid,
                                     struct gapc_bond_req_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // Prepare the GAPC_BOND_CFM message
    struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                             src_id, TASK_APP,
                                             gapc_bond_cfm);

	TRACE("%s param->request %d", __FUNCTION__, param->request);
    switch (param->request)
    {
        case (GAPC_PAIRING_REQ):
        {
            cfm->request = GAPC_PAIRING_RSP;

            #ifndef BLE_APP_AM0
            cfm->accept  = false;

            // Check if we are already bonded (Only one bonded connection is supported)
            //if (!app_sec_env.bonded)
            if(1)
            #endif // !BLE_APP_AM0
            {
                cfm->accept  = true;

				cfm->data.pairing_feat.auth      = BLE_AUTENTICATION_LEVEL;
			
                #if (BLE_APP_HT)
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_DISPLAY_ONLY;
                #else
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                #endif //(BLE_APP_HT)

                cfm->data.pairing_feat.key_size  = 16;
                cfm->data.pairing_feat.oob       = GAP_OOB_AUTH_DATA_NOT_PRESENT;
                cfm->data.pairing_feat.sec_req   = GAP_SEC1_NOAUTH_PAIR_ENC;
                #if (defined(BLE_APP_AM0))
                cfm->data.pairing_feat.ikey_dist = GAP_KDIST_IDKEY;
                cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY;
                #else
                cfm->data.pairing_feat.ikey_dist = GAP_KDIST_ENCKEY;
                cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY;
                #endif // (defined(BLE_APP_AM0))
            }
        } break;

        case (GAPC_LTK_EXCH):
        {
            // Counter
            uint8_t counter;

            cfm->accept  = true;
            cfm->request = GAPC_LTK_EXCH;

            // Generate all the values
            cfm->data.ltk.ediv = (uint16_t)co_rand_word();

            for (counter = 0; counter < RAND_NB_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)co_rand_word();
                cfm->data.ltk.randnb.nb[counter] = (uint8_t)co_rand_word();
            }

            for (counter = RAND_NB_LEN; counter < KEY_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)co_rand_word();
            }
#ifdef _BLE_NVDS_
#ifdef BLE_KEY_PRINT
			TRACE("<==============>LTK IS:");
			DUMP8("%02x ",(uint8_t *)&cfm->data.ltk,16);
			TRACE("<==============>EDIV IS: %04x:",cfm->data.ltk.ediv);
			TRACE("<==============>RANDOM IS:");
			DUMP8("%02x ",(uint8_t *)&cfm->data.ltk.randnb.nb,8);
#endif			
			memset(&ble_save_info,0,sizeof(BleDeviceinfo));
			memcpy(&ble_save_info.peer_bleAddr,(uint8_t *)gapc_get_bdaddr(0, SMPC_INFO_PEER),6);
			ble_save_info.EDIV=cfm->data.ltk.ediv;
			memcpy(&ble_save_info.RANDOM,(uint8_t *)&cfm->data.ltk.randnb.nb,8);
			memcpy(&ble_save_info.LTK,(uint8_t *)&cfm->data.ltk,16);
			ble_save_info.bonded=false;
#endif

            #if (NVDS_SUPPORT)
            // Store the generated value in NVDS
            if (nvds_put(NVDS_TAG_LTK, NVDS_LEN_LTK,
                         (uint8_t *)&cfm->data.ltk) != NVDS_OK)
            {
                ASSERT_ERR(0);
            }
            #endif // #if (NVDS_SUPPORT)
        } break;


        case (GAPC_IRK_EXCH):
        {
            #if (NVDS_SUPPORT)
            uint8_t addr_len = BD_ADDR_LEN;
            #endif //(NVDS_SUPPORT)

            cfm->accept  = true;
            cfm->request = GAPC_IRK_EXCH;

            // Load IRK
            memcpy(cfm->data.irk.irk.key, app_env.loc_irk, KEY_LEN);
            // load device address
            cfm->data.irk.addr.addr_type = ADDR_PUBLIC;
            #if (NVDS_SUPPORT)
            if (nvds_get(NVDS_TAG_BD_ADDRESS, &addr_len, cfm->data.irk.addr.addr.addr) != NVDS_OK)
            #endif //(NVDS_SUPPORT)
            {
                ASSERT_ERR(0);
            }
        } break;


        #if (BLE_APP_HT)
        case (GAPC_TK_EXCH):
        {
            // Generate a PIN Code- (Between 100000 and 999999)
            uint32_t pin_code = (100000 + (co_rand_word()%900000));

            #if DISPLAY_SUPPORT
            // Display the PIN Code
            app_display_pin_code(pin_code);
            #endif //DISPLAY_SUPPORT

            cfm->accept  = true;
            cfm->request = GAPC_TK_EXCH;

            // Set the TK value
            memset(cfm->data.tk.key, 0, KEY_LEN);

            cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
            cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
            cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
            cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
        } break;
        #endif //(BLE_APP_HT)

        default:
        {
            ASSERT_ERR(0);
        } break;
    }

    // Send the message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

static int gapc_bond_ind_handler(ke_msg_id_t const msgid,
                                 struct gapc_bond_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
	TRACE("%s param->info %d", __FUNCTION__, param->info);
    switch (param->info)
    {
        case (GAPC_PAIRING_SUCCEED):
        {
			TRACE("GAPC_PAIRING_SUCCEED");
            // Update the bonding status in the environment
            app_sec_env.bonded = true;
#ifdef _BLE_NVDS_			
			ble_save_info.bonded=true;
			if(!nv_record_blerec_add(&ble_save_info)){
				TRACE("BLE NVDS SETUP SUCCESS!!!,the key is the newest");
			}
#endif

            // Update the bonding status in the environment
            #if (PLF_NVDS)
            if (nvds_put(NVDS_TAG_PERIPH_BONDED, NVDS_LEN_PERIPH_BONDED,
                         (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
            {
                // An error has occurred during access to the NVDS
                ASSERT_ERR(0);
            }

            // Set the BD Address of the peer device in NVDS
            if (nvds_put(NVDS_TAG_PEER_BD_ADDRESS, NVDS_LEN_PEER_BD_ADDRESS,
                         (uint8_t *)gapc_get_bdaddr(0, SMPC_INFO_PEER)) != NVDS_OK)
            {
                // An error has occurred during access to the NVDS
                ASSERT_ERR(0);
            }

            #if (DISPLAY_SUPPORT)
            // Update the bond status screen value
            app_display_set_bond(app_sec_env.bonded);
            #endif //(DISPLAY_SUPPORT)
            #endif //(PLF_NVDS)

            #ifdef BLE_APP_AM0
            am0_app_send_audio_init(KE_IDX_GET(src_id));
            #endif // BLE_APP_AM0
        } break;

        case (GAPC_REPEATED_ATTEMPT):
        {
            appm_disconnect();
        } break;

        case (GAPC_IRK_EXCH):
        {
           #if (NVDS_SUPPORT)
           // Store peer identity in NVDS
           if (nvds_put(NVDS_TAG_PEER_IRK, NVDS_LEN_PEER_IRK, (uint8_t *)&param->data.irk) != NVDS_OK)
           {
               ASSERT_ERR(0);
           }
           #endif // (NVDS_SUPPORT)
        } break;

        case (GAPC_PAIRING_FAILED):
        {
			TRACE("GAPC_PAIRING_FAILED!!!");
#ifdef _BLE_NVDS_				
			app_sec_env.bonded = false;
#endif
            app_sec_send_security_req(0, BLE_AUTENTICATION_LEVEL);
        } break;

        default:
        {
            ASSERT_ERR(0);
        } break;
    }

    return (KE_MSG_CONSUMED);
}

static int gapc_encrypt_req_ind_handler(ke_msg_id_t const msgid,
                                        struct gapc_encrypt_req_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
	TRACE("gapc_encrypt_req_ind_handler,master ask for LTK TO encrypt!!!!");
    #if (NVDS_SUPPORT)
    // LTK value
    struct gapc_ltk ltk;
    // Length
    uint8_t length = NVDS_LEN_LTK;
    #endif // #if (NVDS_SUPPORT)

    // Prepare the GAPC_ENCRYPT_CFM message
    struct gapc_encrypt_cfm *cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                                src_id, TASK_APP,
                                                gapc_encrypt_cfm);

    cfm->found    = false;

    if (app_sec_env.bonded)
    {
        #if (NVDS_SUPPORT)
        // Retrieve the required informations from NVDS
        if (nvds_get(NVDS_TAG_LTK, &length, (uint8_t *)&ltk) == NVDS_OK)
        {
            // Check if the provided EDIV and Rand Nb values match with the stored values
            if ((param->ediv == ltk.ediv) &&
                !memcmp(&param->rand_nb.nb[0], &ltk.randnb.nb[0], sizeof(struct rand_nb)))
            {
                cfm->found    = true;
                cfm->key_size = 16;
                memcpy(&cfm->ltk, &ltk.ltk, sizeof(struct gap_sec_key));
            }
            /*
             * else we are bonded with another device, disconnect the link
             */
        }
        else
        {
            ASSERT_ERR(0);
        }
        #endif // #if (NVDS_SUPPORT)
		
#ifdef _BLE_NVDS_
	struct gapc_ltk ltk;

	int ret;
	ret=nv_record_ble_record_find_ltk_through_ediv_and_random(param->ediv,&param->rand_nb.nb[0],ltk.ltk.key);
	if(!ret){
		TRACE("FIND LTK SUCCESSED!!!");
#ifdef BLE_KEY_PRINT		
		DUMP8("%02x ",(uint8_t *)ltk.ltk.key,16);
#endif
		memset(&ble_save_info,0,sizeof(BleDeviceinfo));
		memcpy(&ble_save_info.peer_bleAddr,(uint8_t *)gapc_get_bdaddr(0, SMPC_INFO_PEER),6);
		ble_save_info.EDIV=param->ediv;
		memcpy(&ble_save_info.RANDOM,(uint8_t *)&param->rand_nb.nb[0],sizeof(struct rand_nb));
		memcpy(&ble_save_info.LTK,(uint8_t *)ltk.ltk.key,sizeof(struct gap_sec_key));
		ble_save_info.bonded=true;
		if(!nv_record_blerec_add(&ble_save_info)){
			TRACE("BLE NVDS RE_SETUP SUCCESS!!!");
		}
		cfm->found	  = true;
		cfm->key_size = 16;
		memcpy(&cfm->ltk, ltk.ltk.key, sizeof(struct gap_sec_key));
	}else{
		TRACE("FIND LTK failed!!!");
        ASSERT_ERR(0);
    }
#endif		
    }

	TRACE("%s app_sec_env.bonded %d", __FUNCTION__, app_sec_env.bonded);
    /*
     * else the peer device is not known, an error should trigger a new pairing procedure.
     */

    // Send the message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}


static int gapc_encrypt_ind_handler(ke_msg_id_t const msgid,
                                    struct gapc_encrypt_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
	TRACE("%s param->auth %d", __FUNCTION__, param->auth);
	
    // encryption/ re-encryption succeeded

    #ifdef BLE_APP_AM0
    am0_app_send_audio_init(KE_IDX_GET(src_id));
    #endif // BLE_APP_AM0

    return (KE_MSG_CONSUMED);
}

static int app_sec_msg_dflt_handler(ke_msg_id_t const msgid,
                                    void *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Drop the message

    return (KE_MSG_CONSUMED);
}

 /*
  * LOCAL VARIABLE DEFINITIONS
  ****************************************************************************************
  */

/// Default State handlers definition
const struct ke_msg_handler app_sec_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,  (ke_msg_func_t)app_sec_msg_dflt_handler},

    {GAPC_BOND_REQ_IND,       (ke_msg_func_t)gapc_bond_req_ind_handler},
    {GAPC_BOND_IND,           (ke_msg_func_t)gapc_bond_ind_handler},

    {GAPC_ENCRYPT_REQ_IND,    (ke_msg_func_t)gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (ke_msg_func_t)gapc_encrypt_ind_handler},
};

const struct ke_state_handler app_sec_table_handler =
    {&app_sec_msg_handler_list[0], (sizeof(app_sec_msg_handler_list)/sizeof(struct ke_msg_handler))};

#endif //(BLE_APP_SEC)

/// @} APP
