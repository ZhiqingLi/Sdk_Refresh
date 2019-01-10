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

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include <string.h>

#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API

#include "app_bt_func.h"

#include "co_bt.h"                   // Common BT Definition
#include "co_math.h"                 // Common Maths Definition
#include "l2cc.h"
#include "l2cc_pdu.h"

#if (BLE_APP_SEC)
#include "app_sec.h"                 // Application security Definition
#endif // (BLE_APP_SEC)

#if (BLE_APP_HT)
#include "app_ht.h"                  // Health Thermometer Application Definitions
#endif //(BLE_APP_HT)

#if (BLE_APP_HR)
#include "app_hrps.h"
#endif

#if (BLE_APP_DATAPATH_SERVER)
#include "app_datapath_server.h"                  // Data Path Server Application Definitions
#endif //(BLE_APP_DATAPATH_SERVER)

#if (BLE_APP_DIS)
#include "app_dis.h"                 // Device Information Service Application Definitions
#endif //(BLE_APP_DIS)

#if (BLE_APP_BATT)
#include "app_batt.h"                // Battery Application Definitions
#endif //(BLE_APP_DIS)

#if (BLE_APP_HID)
#include "app_hid.h"                 // HID Application Definitions
#endif //(BLE_APP_HID)

#if (BLE_APP_SMARTVOICE)
#include "app_sv_ble.h"                  // Smart Voice Application Definitions
#endif //(BLE_APP_SMARTVOICE)

#if (BLE_APP_AMA_VOICE)
#include "app_ama_ble.h"                  // AMA Voice Application Definitions
#endif //(BLE_APP_AMA_VOICE)

#if (BLE_APP_DMA_VOICE)
#include "app_dma_ble.h"                  // DMA Voice Application Definitions
#endif //(BLE_APP_DMA_VOICE)

#if (BLE_APP_OTA)
#include "app_ota_ble.h"                  // OTA Application Definitions
#endif //(BLE_APP_OTA)


#if (DISPLAY_SUPPORT)
#include "app_display.h"             // Application Display Definition
#endif //(DISPLAY_SUPPORT)

#ifdef BLE_APP_AM0
#include "am0_app.h"                 // Audio Mode 0 Application
#endif //defined(BLE_APP_AM0)

#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif //(NVDS_SUPPORT)

#include "cmsis_os.h"
#include "ke_timer.h"
#include "nvrecord.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Default Device Name if no value can be found in NVDS
#if defined (__DMA_VOICE__)
#define APP_DFLT_DEVICE_NAME            ("BUMBLEBEE-LE")
#elif defined (__AMA_VOICE__)
#define APP_DFLT_DEVICE_NAME            ("BES_AMA_LE")
#else
#define APP_DFLT_DEVICE_NAME            ("BES_IOS_LE")

#endif
#define APP_DFLT_DEVICE_NAME_LEN        (sizeof(APP_DFLT_DEVICE_NAME))


#if (BLE_APP_HID)
// HID Mouse
#define DEVICE_NAME        "Hid Mouse"
#else
#define DEVICE_NAME        "RW DEVICE"
#endif

#define DEVICE_NAME_SIZE    sizeof(DEVICE_NAME)

/**
 * UUID List part of ADV Data
 * --------------------------------------------------------------------------------------
 * x03 - Length
 * x03 - Complete list of 16-bit UUIDs available
 * x09\x18 - Health Thermometer Service UUID
 *   or
 * x12\x18 - HID Service UUID
 * --------------------------------------------------------------------------------------
 */

#if (BLE_APP_HT)
#define APP_HT_ADV_DATA_UUID        "\x03\x03\x09\x18"
#define APP_HT_ADV_DATA_UUID_LEN    (4)
#endif //(BLE_APP_HT)


#if (BLE_APP_HID)
#define APP_HID_ADV_DATA_UUID       "\x03\x03\x12\x18"
#define APP_HID_ADV_DATA_UUID_LEN   (4)
#endif //(BLE_APP_HID)

#if (BLE_APP_DATAPATH_SERVER)
/*
 * x11 - Length
 * x07 - Complete list of 16-bit UUIDs available
 * .... the 128 bit UUIDs
 */
#define APP_DATAPATH_SERVER_ADV_DATA_UUID        "\x11\x07\x9e\x34\x9B\x5F\x80\x00\x00\x80\x00\x10\x00\x00\x00\x01\x00\x01"
#define APP_DATAPATH_SERVER_ADV_DATA_UUID_LEN    (18)

#endif //(BLE_APP_HT)


/**
 * Appearance part of ADV Data
 * --------------------------------------------------------------------------------------
 * x03 - Length
 * x19 - Appearance
 * x03\x00 - Generic Thermometer
 *   or
 * xC2\x04 - HID Mouse
 * --------------------------------------------------------------------------------------
 */

#if (BLE_APP_HT)
#define APP_HT_ADV_DATA_APPEARANCE    "\x03\x19\x00\x03"
#endif //(BLE_APP_HT)

#if (BLE_APP_HID)
#define APP_HID_ADV_DATA_APPEARANCE   "\x03\x19\xC2\x03"
#endif //(BLE_APP_HID)

#define APP_ADV_DATA_APPEARANCE_LEN  (4)

#if defined(__SW_IIR_EQ_PROCESS__) || defined(__HW_FIR_EQ_PROCESS__)
/**
 * Default Scan response data
 * --------------------------------------------------------------------------------------
 * x08                             - Length
 * xFF                             - Vendor specific advertising type
 * \xB0\x02\x45\x51\x5F\x54\x45\x53\x54 - "EQ_TEST"
 * --------------------------------------------------------------------------------------
 */
#define APP_SCNRSP_DATA         "\x0A\xFF\xB0\x02\x45\x51\x5F\x54\x45\x53\x54"
#define APP_SCNRSP_DATA_LEN     (11)
#else
#if defined(BAIDU_RFCOMM_DIRECT_CONN)
#define APP_SCNRSP_DATA           "\x03\x03\x04\xFE"
#define APP_SCNRSP_DATA_LEN     (4)
#else

#define APP_SCNRSP_DATA         "\x11\x07\xa2\xaf\x34\xd6\xba\x17\x53\x8b\xd4\x4d\x3c\x8a\xdc\x0c\x3e\x26"
#define APP_SCNRSP_DATA_LEN     (18)
#endif
#endif


#define APP_AMA_UUID            "\x11\x07\xfb\x34\x9b\x5f\x80\x00\x00\x80\x00\x10\x00\x00\x03\xFE\x00\x00"
#define APP_AMA_UUID_LEN        (18)

#if defined(BAIDU_RFCOMM_DIRECT_CONN)
#define APP_SV_BAIDU_VEND_ADV_DATA_HEAD "\x0b\xFF\xB0\x02" 
#define APP_SV_BAIDU_VEND_ADV_DATA_HEAD_LEN (4)

#define APP_SV_BAIDU_VEND_ADV_DATA_TAIL "\xFE\x04" 
#define APP_SV_BAIDU_VEND_ADV_DATA_TAIL_LEN (2)
#endif


/**
 * Advertising Parameters
 */
#if (BLE_APP_HID)
/// Default Advertising duration - 30s (in multiple of 10ms)
#define APP_DFLT_ADV_DURATION   (3000)
#endif //(BLE_APP_HID)
/// Advertising channel map - 37, 38, 39
#define APP_ADV_CHMAP           (0x07)

#define SLOW_ADVERTISING_TIMER          (60*1000) // 60s
#define FAST_ADVERTISING_TIMER          (30*1000) // 30s

#define FAST_ADVERTISING_INTERVAL_MIN          (96) // 60ms (96*0.625ms)
#define FAST_ADVERTISING_INTERVAL_MAX          (96) // 60ms (96*0.625ms)

#define SLOW_ADVERTISING_INTERVAL_MIN          (2048) // 1280 ms (2048*0.625ms)
#define SLOW_ADVERTISING_INTERVAL_MAX          (2048) // 1280 ms (2048*0.625ms)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef void (*appm_add_svc_func_t)(void);

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of service to add in the database
enum appm_svc_list
{
    #if (BLE_APP_HT)
    APPM_SVC_HTS,
    #endif //(BLE_APP_HT)	
    #if (BLE_APP_DIS)
    APPM_SVC_DIS,
    #endif //(BLE_APP_DIS)	
    #if (BLE_APP_BATT)
    APPM_SVC_BATT,
    #endif //(BLE_APP_BATT)	
	#if (BLE_APP_HID)
    APPM_SVC_HIDS,
    #endif //(BLE_APP_HID)
	#ifdef BLE_APP_AM0
    APPM_SVC_AM0_HAS,
    #endif //defined(BLE_APP_AM0)
    #if (BLE_APP_HR)
    APPM_SVC_HRP,
    #endif
	#if (BLE_APP_DATAPATH_SERVER)
    APPM_SVC_DATAPATH_SERVER,
	#endif //(BLE_APP_DATAPATH_SERVER)
	#if (BLE_APP_SMARTVOICE)
    APPM_SVC_SMARTVOICE,
	#endif //(BLE_APP_SMARTVOICE)
	#if (BLE_AMA_VOICE)
    APPM_AMA_SMARTVOICE,
	#endif //(BLE_APP_AMA)
	#if (BLE_DMA_VOICE)
    APPM_DMA_SMARTVOICE,
	#endif //(BLE_APP_DMA)
	#if (BLE_APP_OTA)
    APPM_SVC_OTA,
	#endif //(BLE_APP_OTA)

    APPM_SVC_LIST_STOP,
};

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */
//gattc_msg_handler_tab
//#define KE_MSG_HANDLER_TAB(task)   __STATIC const struct ke_msg_handler task##_msg_handler_tab[] =
/// Application Task Descriptor

//static const struct ke_task_desc TASK_DESC_APP = {&appm_default_handler,
//                                                  appm_state, APPM_STATE_MAX, APP_IDX_MAX};
extern const struct ke_task_desc TASK_DESC_APP;

/// List of functions used to create the database
static const appm_add_svc_func_t appm_add_svc_func_list[APPM_SVC_LIST_STOP] =
{
    #if (BLE_APP_HT)
    (appm_add_svc_func_t)app_ht_add_hts,
    #endif //(BLE_APP_HT)
    #if (BLE_APP_DIS)
    (appm_add_svc_func_t)app_dis_add_dis,
    #endif //(BLE_APP_DIS)
    #if (BLE_APP_BATT)
    (appm_add_svc_func_t)app_batt_add_bas,
    #endif //(BLE_APP_BATT)
    #if (BLE_APP_HID)
    (appm_add_svc_func_t)app_hid_add_hids,
    #endif //(BLE_APP_HID)
    #ifdef BLE_APP_AM0
    (appm_add_svc_func_t)am0_app_add_has,
    #endif //defined(BLE_APP_AM0)
    #if (BLE_APP_HR)
    (appm_add_svc_func_t)app_hrps_add_profile,
    #endif
	#if (BLE_APP_DATAPATH_SERVER)
    (appm_add_svc_func_t)app_datapath_add_datapathps,
    #endif //(BLE_APP_DATAPATH_SERVER)
	#if (BLE_APP_SMARTVOICE)
    (appm_add_svc_func_t)app_sv_add_sv,
    #endif //(BLE_APP_SMARTVOICE)
	#if (BLE_APP_AMA_VOICE)
    (appm_add_svc_func_t)app_ama_add_ama,
	#endif //(BLE_APP_AMA_VOICE)
	#if (BLE_APP_DMA_VOICE)
    (appm_add_svc_func_t)app_dma_add_dma,
	#endif //(BLE_APP_AMA_VOICE)
	#if (BLE_APP_OTA)
    (appm_add_svc_func_t)app_ota_add_ota,
	#endif //(BLE_APP_OTA)	

};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Environment Structure
struct app_env_tag app_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
 
//void bes_ble_stop_advertising(void);
void bes_ble_start_advertising(void);

static void appm_create_fast_adv_timer(void);
static void appm_stop_fast_adv_timer(void);
static void appm_create_slow_adv_timer(void);
static void appm_start_slow_adv_timer(void);
static void appm_stop_slow_adv_timer(void);

static void appm_fast_adv_timeout_handler(void const *param);
static void appm_slow_adv_timeout_handler(void const *param);

/**
 * Advertising timer: Fast ADV timer and Slow ADV timer
 */
osTimerDef(FAST_ADV, appm_fast_adv_timeout_handler);

static osTimerId fast_adv_timer = NULL;

static void appm_fast_adv_timeout_handler(void const *param)
{
    appm_start_slow_adv_timer();
    TRACE("appm_fast_adv_timeout_handler +++++++++");

    if (app_env.adv_mode == APP_FAST_ADV_MODE)
        bes_ble_stop_advertising();
}

osTimerDef(SLOW_ADV, appm_slow_adv_timeout_handler);

static osTimerId slow_adv_timer = NULL;

static void appm_slow_adv_timeout_handler(void const *param)
{
    TRACE("appm_slow_adv_timeout_handler +++++++++");
    if (app_env.adv_mode == APP_SLOW_ADV_MODE)
        bes_ble_stop_advertising();
}

static void appm_create_fast_adv_timer(void)
{
    if (fast_adv_timer == NULL)
        fast_adv_timer = osTimerCreate(osTimer(FAST_ADV), osTimerOnce, NULL);
}

static void appm_stop_fast_adv_timer(void)
{
    osTimerStop(fast_adv_timer);
}

static void appm_create_slow_adv_timer(void)
{
    if (slow_adv_timer == NULL)
        slow_adv_timer = osTimerCreate(osTimer(SLOW_ADV), osTimerOnce, NULL);
}

static void appm_start_slow_adv_timer(void)
{
    osTimerStart(slow_adv_timer, SLOW_ADVERTISING_TIMER);
}

static void appm_stop_slow_adv_timer(void)
{
    osTimerStop(slow_adv_timer);
}

void appm_init()
{
    #if (NVDS_SUPPORT)
    uint8_t key_len = KEY_LEN;
    #endif //(NVDS_SUPPORT)
    BLE_APP_FUNC_ENTER();

    // Reset the application manager environment
    memset(&app_env, 0, sizeof(app_env));

    // Create APP task
    ke_task_create(TASK_APP, &TASK_DESC_APP);

    // Initialize Task state
    ke_state_set(TASK_APP, APPM_INIT);

    // Load the device name from NVDS

    // Get the Device Name to add in the Advertising Data (Default one or NVDS one)
    #if (NVDS_SUPPORT)
    app_env.dev_name_len = APP_DEVICE_NAME_MAX_LEN;
    if (nvds_get(NVDS_TAG_DEVICE_NAME, &(app_env.dev_name_len), app_env.dev_name) != NVDS_OK)
    #endif //(NVDS_SUPPORT)
    {
		const char* ble_name_in_nv = nvrec_dev_get_ble_name();
		uint32_t nameLen = strlen(ble_name_in_nv);
		if (nameLen > APP_DEVICE_NAME_MAX_LEN)
		{
			nameLen = APP_DEVICE_NAME_MAX_LEN;
		}
        // Get default Device Name (No name if not enough space)
        memcpy(app_env.dev_name, ble_name_in_nv, nameLen);
		//app_env.dev_name[nameLen - 1] = '\0';
        app_env.dev_name_len = nameLen;

        // TODO update this value per profiles
    }

    #if (NVDS_SUPPORT)
    if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_env.loc_irk) != NVDS_OK)
    #endif //(NVDS_SUPPORT)
    {
        uint8_t counter;

        // generate a new IRK
        for (counter = 0; counter < KEY_LEN; counter++)
        {
            app_env.loc_irk[counter]    = (uint8_t)co_rand_word();
        }

        // Store it in NVDS
        #if (NVDS_SUPPORT)
        // Store the generated value in NVDS
        if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_env.loc_irk) != NVDS_OK)
        {
            ASSERT_INFO(0, NVDS_TAG_LOC_IRK, 0);
        }
        #endif // #if (NVDS_SUPPORT)
    }

    /*------------------------------------------------------
     * INITIALIZE ALL MODULES
     *------------------------------------------------------*/

    // load device information:

    #if (DISPLAY_SUPPORT)
    app_display_init();
    #endif //(DISPLAY_SUPPORT)

    #if (BLE_APP_SEC)
    // Security Module
    app_sec_init();
    #endif // (BLE_APP_SEC)

    #if (BLE_APP_HT)
    // Health Thermometer Module
    app_ht_init();
    #endif //(BLE_APP_HT)

    #if (BLE_APP_DIS)
    // Device Information Module
    app_dis_init();
    #endif //(BLE_APP_DIS)

    #if (BLE_APP_HID)
    // HID Module
    app_hid_init();
    #endif //(BLE_APP_HID)

    #if (BLE_APP_BATT)
    // Battery Module
    app_batt_init();
    #endif //(BLE_APP_BATT)

    #ifdef BLE_APP_AM0
    // Audio Mode 0 Module
    am0_app_init();
    #endif // defined(BLE_APP_AM0)

	#if (BLE_APP_DATAPATH_SERVER)
    // Data Path Server Module
    app_datapath_server_init();
	#endif //(BLE_APP_DATAPATH_SERVER)

	#if (BLE_APP_SMARTVOICE)
    // Smart Voice Module
    app_sv_init();
	#endif //(BLE_APP_SMARTVOICE)

	#if (BLE_APP_AMA_VOICE)
    // AMA Voice Module
    app_ama_init();
	#endif //(BLE_APP_AMA_VOICE)

	#if (BLE_APP_DMA_VOICE)
    // DMA Voice Module
    app_dma_init();
	#endif //(BLE_APP_DMA_VOICE)

	#if (BLE_APP_OTA)
    // OTA Module
    app_ota_init();
	#endif //(BLE_APP_OTA)

    app_env.adv_mode = APP_FAST_ADV_MODE;

    appm_create_fast_adv_timer();
    appm_create_slow_adv_timer();

    BLE_APP_FUNC_LEAVE();
}

bool appm_add_svc(void)
{
    // Indicate if more services need to be added in the database
    bool more_svc = false;

    // Check if another should be added in the database
    if (app_env.next_svc != APPM_SVC_LIST_STOP)
    {
        ASSERT_INFO(appm_add_svc_func_list[app_env.next_svc] != NULL, app_env.next_svc, 1);

		BLE_APP_DBG("appm_add_svc adds service");

        // Call the function used to add the required service
        appm_add_svc_func_list[app_env.next_svc]();

        // Select following service to add
        app_env.next_svc++;
        more_svc = true;
    }
	else
	{
		BLE_APP_DBG("appm_add_svc doesn't execute, next svc is %d", app_env.next_svc);
	}


    return more_svc;
}

void appm_disconnect(void)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, app_env.conidx), TASK_APP,
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = CO_ERROR_REMOTE_USER_TERM_CON;

    // Send the message
    ke_msg_send(cmd);
}

void bes_ble_start_advertising(void)
{
    appm_start_advertising();
    
    OS_NotifyEvm();
}

/**
 ****************************************************************************************
 * Advertising Functions
 ****************************************************************************************
 */
void appm_start_advertising(void)
{
    BLE_APP_FUNC_ENTER();

    TRACE("%s state: %d", __func__, ke_state_get(TASK_APP));

    // Check if the advertising procedure is already is progress
    if (ke_state_get(TASK_APP) == APPM_READY)
    {
        // Prepare the GAPM_START_ADVERTISE_CMD message
        struct gapm_start_advertise_cmd *cmd = KE_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                            TASK_GAPM, TASK_APP,
                                                            gapm_start_advertise_cmd);

		cmd->op.addr_src    = GAPM_STATIC_ADDR;

		/* use following set-up for the random BD addr adv */
		/*
        cmd->op.addr_src    = GAPM_GEN_RSLV_ADDR;
		cmd->op.randomAddrRenewIntervalInSecond = 15;
		*/
		
        cmd->channel_map    = APP_ADV_CHMAP;
		
#if defined(BYPASS_SLOW_ADV_MODE)
		app_env.adv_mode = APP_FAST_ADV_MODE;
#endif

        if (app_env.adv_mode == APP_FAST_ADV_MODE)
        {
            cmd->intv_min = FAST_ADVERTISING_INTERVAL_MIN;
            cmd->intv_max = FAST_ADVERTISING_INTERVAL_MAX;
        }
        else if (app_env.adv_mode == APP_SLOW_ADV_MODE)
        {
            cmd->intv_min = SLOW_ADVERTISING_INTERVAL_MIN;
            cmd->intv_max = SLOW_ADVERTISING_INTERVAL_MAX;
        }
        
        {
            cmd->op.code        = GAPM_ADV_UNDIRECT;

            cmd->info.host.mode = GAP_GEN_DISCOVERABLE;

            /*-----------------------------------------------------------------------------------
             * Set the Advertising Data and the Scan Response Data
             *---------------------------------------------------------------------------------*/
            // Flag value is set by the GAP
            cmd->info.host.adv_data_len       = ADV_DATA_LEN - 3;
            cmd->info.host.scan_rsp_data_len  = SCAN_RSP_DATA_LEN;

            // Advertising Data
            {
                cmd->info.host.adv_data_len = 0;

                //Add list of UUID and appearance
                #if (BLE_APP_HT)
                memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len],
                       APP_HT_ADV_DATA_UUID, APP_HT_ADV_DATA_UUID_LEN);
                cmd->info.host.adv_data_len += APP_HT_ADV_DATA_UUID_LEN;
                memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len],
                       APP_HT_ADV_DATA_APPEARANCE, APP_ADV_DATA_APPEARANCE_LEN);
                cmd->info.host.adv_data_len += APP_ADV_DATA_APPEARANCE_LEN;
                #endif //(BLE_APP_HT)

                #if (BLE_APP_HID)
                memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len],
                       APP_HID_ADV_DATA_UUID, APP_HID_ADV_DATA_UUID_LEN);
                cmd->info.host.adv_data_len += APP_HID_ADV_DATA_UUID_LEN;
                memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len],
                       APP_HID_ADV_DATA_APPEARANCE, APP_ADV_DATA_APPEARANCE_LEN);
                cmd->info.host.adv_data_len += APP_ADV_DATA_APPEARANCE_LEN;
                #endif //(BLE_APP_HID)
            }

            // Scan Response Data
            {
#if 1
#if defined(BAIDU_RFCOMM_DIRECT_CONN)
				extern uint8_t bt_addr[];
				char _bt_addr[6];
				unsigned char temp = 0;
#endif
#endif
                cmd->info.host.scan_rsp_data_len = 0;

				
#ifndef __AMA_VOICE__
                memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len],
                       APP_SCNRSP_DATA, APP_SCNRSP_DATA_LEN);
                cmd->info.host.scan_rsp_data_len += APP_SCNRSP_DATA_LEN;
#else
				memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len],
					  APP_AMA_UUID, APP_AMA_UUID_LEN);
				cmd->info.host.scan_rsp_data_len += APP_AMA_UUID_LEN;
#endif
#if 1
#if defined(BAIDU_RFCOMM_DIRECT_CONN)
                if (nvrec_dev_get_btaddr(_bt_addr) == false) {
                    // if get bt addr from nv false, we use default bt addr var as addr
                    memcpy((void *)_bt_addr, (void *)bt_addr, 6);
                }
				TRACE("ble adv bt addr:");
				DUMP8("%02x ",_bt_addr,6);
                memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len],
                       APP_SV_BAIDU_VEND_ADV_DATA_HEAD, APP_SV_BAIDU_VEND_ADV_DATA_HEAD_LEN);
                cmd->info.host.scan_rsp_data_len += APP_SV_BAIDU_VEND_ADV_DATA_HEAD_LEN;
                memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len],
                       APP_SV_BAIDU_VEND_ADV_DATA_TAIL, APP_SV_BAIDU_VEND_ADV_DATA_TAIL_LEN);
                cmd->info.host.scan_rsp_data_len += APP_SV_BAIDU_VEND_ADV_DATA_TAIL_LEN;
                temp = _bt_addr[0];
                _bt_addr[0] = _bt_addr[5];
                _bt_addr[5] = temp;

                temp = _bt_addr[1];
                _bt_addr[1] = _bt_addr[4];
                _bt_addr[4] = temp;

                temp = _bt_addr[2];
                _bt_addr[2] = _bt_addr[3];
                _bt_addr[3] = temp;
                memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len],
                       _bt_addr, 6);
                cmd->info.host.scan_rsp_data_len += 6;
#endif
#endif				
            }

            //  Device Name Length
            uint8_t avail_space;

            // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
            avail_space = (ADV_DATA_LEN - 3) - cmd->info.host.adv_data_len - 2;

            // Check if data can be added to the Advertising data
            if (avail_space > 2)
            {
                avail_space = co_min(avail_space, app_env.dev_name_len);

                cmd->info.host.adv_data[cmd->info.host.adv_data_len]     = avail_space + 1;
                // Fill Device Name Flag
                cmd->info.host.adv_data[cmd->info.host.adv_data_len + 1] = (avail_space == app_env.dev_name_len) ? '\x08' : '\x09';
                // Copy device name
                memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len + 2], app_env.dev_name, avail_space);

                // Update Advertising Data Length
                cmd->info.host.adv_data_len += (avail_space + 2);
            }
        }

        // Send the message
        ke_msg_send(cmd);

        // Set the state of the task to APPM_ADVERTISING
        ke_state_set(TASK_APP, APPM_ADVERTISING);
    }
    // else ignore the request

    BLE_APP_FUNC_LEAVE();
}

void appm_restart_advertising(void)
{
    ke_state_set(TASK_APP, APPM_READY);

    appm_start_advertising();
}

void bes_ble_stop_advertising(void)
{
    appm_stop_advertising();
    
    OS_NotifyEvm();
}

void appm_stop_advertising(void)
{
    BLE_APP_FUNC_ENTER();

    if (ke_state_get(TASK_APP) == APPM_ADVERTISING)
    {
        #if (BLE_APP_HID)
        // Stop the advertising timer if needed
        if (ke_timer_active(APP_ADV_TIMEOUT_TIMER, TASK_APP))
        {
            ke_timer_clear(APP_ADV_TIMEOUT_TIMER, TASK_APP);
        }
        #endif //(BLE_APP_HID)

        // Go in ready state
        ke_state_set(TASK_APP, APPM_READY);

        // Prepare the GAPM_CANCEL_CMD message
        struct gapm_cancel_cmd *cmd = KE_MSG_ALLOC(GAPM_CANCEL_CMD,
                                                   TASK_GAPM, TASK_APP,
                                                   gapm_cancel_cmd);

        cmd->operation = GAPM_CANCEL;

        // Send the message
        ke_msg_send(cmd);
        
        #if (DISPLAY_SUPPORT)
        // Update advertising state screen
        app_display_set_adv(false);
        #endif //(DISPLAY_SUPPORT)
    }
    // else ignore the request

    BLE_APP_FUNC_LEAVE();
}

static void app_start_ble_advertising_handler(uint8_t advType, uint16_t adv_interval_in_ms)
{
    bes_ble_start_advertising();
}

void app_start_ble_advertising(void)
{
	app_bt_start_custom_function_in_bt_thread(0, 0, 
		(uint32_t)app_start_ble_advertising_handler);
}

static void app_stop_ble_advertising_handler(uint8_t advType, uint16_t adv_interval_in_ms)
{
    bes_ble_stop_advertising();
}

void app_stop_ble_advertising(void)
{
	app_bt_start_custom_function_in_bt_thread(0, 0, 
		(uint32_t)app_stop_ble_advertising_handler);
}

volatile char gapc_conn_param_valid = 0;
volatile struct gapc_conn_param cent_comm_param;

static void appm_update_param_handler(uint8_t p1, uint16_t p2)
{
    struct gapc_conn_param conn_param;

    if (gapc_conn_param_valid) {
        memcpy(&conn_param, &cent_comm_param, sizeof(cent_comm_param));
        conn_param.intv_min = 10;
        conn_param.intv_max = 10;
    }
    else {
        conn_param.intv_min = 10;
        conn_param.intv_max = 10;
        conn_param.latency  = 0;
        conn_param.time_out = 20000;
    }

    appm_update_param(&conn_param);
}

void app_update_conn_param(void)
{
	app_bt_start_custom_function_in_bt_thread(0, 0, 
		(uint32_t)appm_update_param_handler);
}



void appm_update_param(struct gapc_conn_param *conn_param)
{
    // Prepare the GAPC_PARAM_UPDATE_CMD message
    struct gapc_param_update_cmd *cmd = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, app_env.conidx), TASK_APP,
                                                     gapc_param_update_cmd);

    cmd->operation  = GAPC_UPDATE_PARAMS;
    cmd->intv_min   = conn_param->intv_min;
    cmd->intv_max   = conn_param->intv_max;
    cmd->latency    = conn_param->latency;
    cmd->time_out   = conn_param->time_out;

    // not used by a slave device
    cmd->ce_len_min = 0xFFFF;
    cmd->ce_len_max = 0xFFFF;

    // Send the message
    ke_msg_send(cmd);
}

void l2cap_update_param(struct gapc_conn_param *conn_param)
{
    struct l2cc_update_param_req *req = L2CC_SIG_PDU_ALLOC(app_env.conidx, L2C_CODE_CONN_PARAM_UPD_REQ,
                                                   KE_BUILD_ID(TASK_GAPC, app_env.conidx), l2cc_update_param_req);

    // generate packet identifier
    uint8_t pkt_id = co_rand_word() & 0xFF;
    if	(pkt_id == 0)
    {
        pkt_id = 1;
    }

    /* fill up the parameters */
    req->intv_max = conn_param->intv_max;
    req->intv_min = conn_param->intv_min;
    req->latency  = conn_param->latency;
    req->timeout  = conn_param->time_out;
    req->pkt_id   = pkt_id;

    l2cc_pdu_send(req);
}

//add begin by cai.zhong 20181101 for set ble name   
void appm_set_dev_name(uint8_t *name,int len)
{
    TRACE("LE name: %s %d", name, len);
    
	memcpy(app_env.dev_name, name, (len > APP_DEVICE_NAME_MAX_LEN) ? APP_DEVICE_NAME_MAX_LEN : len);
	app_env.dev_name[len] = 0;
    app_env.dev_name_len = len; // change by cai.zhong 20180928 for ble name
    TRACE("%s LE name: %s", __func__, app_env.dev_name);
	return;
}
//add end by cai.zhong 20181101 for set ble name   

uint8_t appm_get_dev_name(uint8_t* name)
{
    // copy name to provided pointer
    memcpy(name, app_env.dev_name, app_env.dev_name_len);
    // return name length
    return app_env.dev_name_len;
}

void app_connected_evt_handler(void)
{
    BLE_APP_FUNC_ENTER();
    
    appm_stop_fast_adv_timer();
    appm_stop_slow_adv_timer();
    
    app_env.adv_mode = APP_FAST_ADV_MODE;
    
    BLE_APP_FUNC_LEAVE();
}
	
void app_disconnected_evt_handler(void)
{
    BLE_APP_FUNC_ENTER();

    app_env.adv_mode = APP_FAST_ADV_MODE;

    BLE_APP_FUNC_LEAVE();	
}

#endif //(BLE_APP_PRESENT)

/// @} APP
