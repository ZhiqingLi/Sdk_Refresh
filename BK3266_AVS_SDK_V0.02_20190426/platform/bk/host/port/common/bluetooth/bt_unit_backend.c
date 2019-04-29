/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bluetooth.h>
#include <bt_security.h>
#include <bt_api.h>
#include <bt_unit.h>
#include "bt_app_internal.h"
#include "driver_beken_includes.h"
#include "app_debug.h"
#include "beken_external.h"
#include "app_beken_includes.h"
#ifdef CONFIG_TWS
#include <bluetooth\core\hci_internal.h>
#include "app_protocol.h"
#endif

#define SAMPLE_NAME             "BK3266"
#define SAMPLE_SERVICE          (BT_SERVICE_RENDERING | BT_SERVICE_AUDIO | \
                                BT_SERVICE_NETWORKING)
#define SAMPLE_DEVCLASS         BT_DEVCLASS_AUDIO_VIDEO
#ifdef CONFIG_TWS
#define SAMPLE_DEVSUBCLASS      BT_DEVSUBCLASS_WEARABLE_HEADSET
#else
#define SAMPLE_DEVSUBCLASS      BT_DEVSUBCLASS_PORTABLE_AUDIO
#endif

#define COMBO_SERVICE			0//BT_SERVICE_LIMITED_DISCOVERABLE
#define COMBO_DEVCLASS			BT_DEVCLASS_PERIPHERAL
#define COMBO_DEVSUBCLASS		BT_DEVSUBCLASS_KEYBOARD

static btpin_t default_pin  = {'0', '0', '0', '0', '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

#define SAMPLE_ENABLE_FLAG_SET_NAME       (1 << 0)
#define SAMPLE_ENABLE_FLAG_SET_CLASS      (1 << 1)
#define SAMPLE_ENABLE_FLAG_AUTH_ENABLE    (1 << 2)
#define SAMPLE_ENABLE_FLAG_SCAN_ENABLE    (1 << 3)
#define SAMPLE_ENABLE_FLAG_WRITE_STORED_LINKKEY (1 << 4)

#define SAMPLE_ENABLE_FLAG_COMPLETE   \
    (SAMPLE_ENABLE_FLAG_SET_NAME      \
    | SAMPLE_ENABLE_FLAG_SET_CLASS    \
    | SAMPLE_ENABLE_FLAG_AUTH_ENABLE  \
    | SAMPLE_ENABLE_FLAG_SCAN_ENABLE  \
    | SAMPLE_ENABLE_FLAG_WRITE_STORED_LINKKEY)

typedef struct bt_unit_app_s {
    bt_unit_h                  unit;
    app_unit_id_t              unit_id;
    btaddr_t                   unit_addr;
    uint8_t                    attach_flags;
#ifdef CONFIG_SINGLE_THREADED
    jtask_h                    enable_task;
#endif
    TAILQ_ENTRY(bt_unit_app_s) next;
} bt_unit_app_t;

static void unit_cmd_complete(bt_unit_app_t *unit, uint8_t flag,
    hci_error_t hci_err);
static void backend_unit_enable(void *arg);

/* TODO: Make this into a device address cache? */
/* TODO: bind to unit? */
typedef struct bt_remote_dev_s {
    btaddr_t                    addr;
    btclass_t                   uclass;
    char                        name[HCI_UNIT_NAME_SIZE];
    bool_t                      name_resolved;
    bool_t                      resolve_pending;
    LIST_ENTRY(bt_remote_dev_s) next;
} bt_remote_dev_t;

static app_unit_id_t g_unit_id = 0;

static TAILQ_HEAD(,bt_unit_app_s) g_unit_list =
    TAILQ_HEAD_INITIALIZER(g_unit_list);

/* Remote device list */
static LIST_HEAD(,bt_remote_dev_s) g_remote_dev_list =
    LIST_HEAD_INITIALIZER(g_remote_dev_list);

void unit_update_ext_inq_rsp(hci_unit_t *unit);
extern result_t bt_unit_set_page_activity(bt_unit_h unit, uint16_t interval_slot, uint16_t window_slot);

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
static result_t unit_enable(char *params, unsigned int len);
static result_t unit_disable(char *params, unsigned int len);
static result_t unit_inquiry(char *params, unsigned int len);
static result_t unit_stop_inquiry(char *params, unsigned int len);
static result_t unit_acl_connect(char *params, unsigned int len);
static result_t unit_acl_disconnect(char *params, unsigned int len);
static result_t unit_acl_accept(char *params, unsigned int len);
static result_t unit_acl_reject(char *params, unsigned int len);
static result_t unit_acl_echo(char *params, unsigned int len);
static result_t unit_del_key(char *params, unsigned int len);
static result_t unit_add_pin(char *params, unsigned int len);
static result_t unit_del_pin(char *params, unsigned int len);
static result_t unit_reject_pin(char *params, unsigned int len);
static result_t unit_add_svc_mode(char *params, unsigned int len);
static result_t unit_del_svc_mode(char *params, unsigned int len);
static result_t unit_set_scan_enable(char *params, unsigned int len);
#ifdef CONFIG_BLUETOOTH_SSP
static result_t unit_ssp_confirm(char *params, unsigned int len);
static result_t unit_ssp_passkey(char *params, unsigned int len);
#ifdef CONFIG_BLUETOOTH_SSP_DEBUG
static result_t unit_ssp_debug(char *params, unsigned int len);
#endif
static result_t unit_ssp_bonding_mode(char *params, unsigned int len);
#endif

static bt_app_cmd_t unit_cmds[] = {
    {"enable",
        "enable (unit)",
        "Enables the specified bluetooth (unit)\n"
            "Example: enable 0",
        unit_enable},
    {"disable",
        "disable (unit)",
        "Disables the specified bluetooth (unit)\n"
            "Example: disable 0",
        unit_disable},
    {"inquiry",
        "inquiry (unit) [(length)] [(max_responses)]",
        "Starts an Inquiry on (unit), lasting until (length) seconds "
            "have elapsed, or (max_responses) responses have been received\n"
            "If not specified, (length) defaults to 60 seconds, "
            "(max_responses) defaults to 0 (unlimited).\n"
            "Example: inquiry 0 20 30\n"
            "Starts an inquiry on unit 0 running for either 20 seconds or "
            "until 30 responses have been received",
        unit_inquiry},
    {"stop_inquiry",
        "stop_inquiry (unit)",
        "Cancel inquiry in progress on (unit)\n"
            "Example: stop_inquiry 0",
        unit_stop_inquiry},
    {"acl_connect",
        "acl_connect (unit) (raddr)",
        "Create an ACL link on (unit) to (raddr)\n"
            "Example: acl_connect 0 FE:ED:FA:CE:BE:EF",
        unit_acl_connect},
    {"acl_disconnect",
        "acl_disconnect (unit) (raddr)",
        "Disconnect an ACL link on (unit) to (raddr)\n"
            "Example: acl_disconnect 0 FE:ED:FA:CE:BE:EF",
        unit_acl_disconnect},
    {"acl_accept",
        "accept (unit) (raddr)",
        "Accept an ACL link from (raddr) on (unit)\n"
            "Example: acl_accept 0 FE:ED:FA:CE:BE:EF",
        unit_acl_accept},
    {"acl_reject",
        "acl_reject (unit) (raddr)",
        "Reject an ACL link from (raddr) on (unit)\n"
            "Example: acl_reject 0 FE:ED:FA:CE:BE:EF",
        unit_acl_reject},
    {"acl_echo",
        "acl_echo (unit) (raddr)",
        "Send an L2CAP Echo request on (unit) to (raddr)\n"
            "Example: acl_echo 0 FE:ED:FA:CE:BE:EF",
        unit_acl_echo},
    {"set_scan_enable",
        "set_scan_enable (unit) (discoverable) (connectable)",
        "Make (unit) (discoverable) and/or (connectable)\n"
            "Example: set_scan_enable 0 1 0",
        unit_set_scan_enable},
};

static bt_app_section_t unit_section = {
    "unit",
    "general commands for managing unit/inquiry/services",
    unit_cmds,
    UW_ARRAY_SIZE(unit_cmds)
};

static bt_app_cmd_t sec_cmds[] = {
    {"del_key",
        "del_key (unit) (raddr)",
        "Delete a link key on (unit) for (raddr)\n"
            "Example: del_key 0 FE:ED:FA:CE:BE:EF",
        unit_del_key},
    {"add_pin",
        "add_pin (raddr) (pin)",
        "Add a PIN code (pin) for (raddr)\n"
            "Example: add_pin FE:ED:FA:CE:BE:EF 1234",
        unit_add_pin},
    {"del_pin",
        "del_pin (raddr)",
        "Delete PIN code for (raddr)\n"
            "Example: del_pin FE:ED:FA:CE:BE:EF",
        unit_del_pin},
    {"reject_pin",
        "reject_pin (raddr)",
        "Reject a PIN code request from (raddr)\n"
            "Example: reject_pin FE:ED:FA:CE:BE:EF",
        unit_reject_pin},
#ifdef CONFIG_BLUETOOTH_SSP
    {"ssp_confirm",
        "ssp_confirm (raddr) (confirm)",
        "Confirm or deny an SSP confirmation request for (raddr), "
            "(confirm) set to 0 to deny, 1 to accept.\n"
            "Example: ssp_confirm FE:ED:FA:CE:BE:EF 1",
        unit_ssp_confirm},
    {"ssp_passkey",
        "ssp_passkey (raddr) (passkey)",
        "Respond to an SSP passkey request for (remote address) with "
            "(passkey)\n"
            "Example: ssp_passkey FE:ED:FA:CE:BE:EF 123456",
        unit_ssp_passkey},
#ifdef CONFIG_BLUETOOTH_SSP_DEBUG
    {"ssp_debug",
        "ssp_debug (unit) (enable)"
        "Enable/disable SSP debug mode on (unit). Set (enable) to 1 to enable\n"
        "Example: ssp_debug 0 1",
        unit_ssp_debug},
#endif
    {"ssp_bonding_mode",
        "ssp_bonding_mode (mode)",
        "Set default SSP bonding mode to (mode)\n"
            "(mode) may be 0 for non-bonding, 1 for general bonding\n"
            "Example: ssp_bonding_mode 0",
        unit_ssp_bonding_mode},
#endif
    {"add_svc_mode",
        "add_svc_mode (svc_type) (sec_level)",
        "Set security requirements for (svc_type) to (sec_level)\n"
            "(svc_type) may be one of sdp, rfcomm, sp, opp, pbap, hsag, "
            "a2src, a2snk, avrcp, hfag, wiap\n"
            "(sec_level) may be set to 0 for no security, 1 for low security, "
            "2 for medium security, 3 for high security.\n"
            "Example: add_svc_mode a2src 3",
        unit_add_svc_mode},
    {"del_svc_mode",
        "del_svc_mode (svc_type)",
        "delete security requirements for (svc_type)\n"
            "(svc_type) may be one of sdp, rfcomm, sp, opp, pbap, hsag, "
            "a2src, a2snk, avrcp, hfag, wiap\n"
            "Example: del_svc_mode a2src",
        unit_del_svc_mode},
};

static bt_app_section_t sec_section = {
    "sec",
    "commands for managing security",
    sec_cmds,
    UW_ARRAY_SIZE(sec_cmds)
};
#endif

void hci_security_pin_code_reply(const btaddr_t *raddr, const btpin_t *pin);

result_t unit_backend_init(void)
{
    result_t rc = 0;

    LIST_INIT(&g_remote_dev_list);

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    rc = backend_section_register(&unit_section);
    if(rc)
        return rc;

    rc = backend_section_register(&sec_section);
#endif

    return rc;
}

void unit_backend_uninit(void)
{
    bt_remote_dev_t *rdev;

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    backend_section_unregister(&sec_section);
    backend_section_unregister(&unit_section);
#endif

    /* Clear the remote device list */
    while ((rdev = LIST_FIRST(&g_remote_dev_list)) != NULL)
    {
        LIST_REMOVE(rdev, next);
        jfree(rdev);
        rdev = NULL;
    }
}

/***************************************/
/* Unit app registration and callbacks */
/***************************************/
/*
 * A PIN code is needed for establishing a connection.
 */
static void bt_pin_code_request(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;
#endif
    DECLARE_FNAME("bt_pin_code_request");

    LOCK_CHECK;

    DBG_I(DBT_APP, ("%s: raddr " BTADDR_FORMAT "\n", FNAME, BTADDR(raddr)));

#ifdef BEKEN_DEBUG
    len = j_snprintf(buf, sizeof(buf), "pin_req " BTADDR_FORMAT, BTADDR(raddr));
    frontend_notify(buf, len);
#endif

    /* TODO: remove */
    /* Application should call bt_sec_add_pin() or bt_sec_reject_pin() during
     * 30 seconds - the LMP response timeout.
     */
    //bt_sec_add_pin(raddr, default_pin);
    hci_security_pin_code_reply(raddr, (const btpin_t *)&default_pin);
}

void bt_auto_connect_stop(void);
#ifdef CONFIG_TWS
void app_bt_inquiry_active_conn( void *arg );
void *app_bt_get_handle( uint8_t handle_type );
#endif
static void bt_acl_con_request(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, btclass_t uclass)
{
    bt_unit_app_t *app = (bt_unit_app_t *)app_ctx_h;
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;
#endif
    result_t rc;

    LOCK_CHECK;

#ifdef BEKEN_DEBUG
    len = j_snprintf(buf, sizeof(buf), "acl_req " BTADDR_FORMAT " %02x%02x%02x",
        BTADDR(raddr), uclass[0], uclass[1], uclass[2]);
    frontend_notify(buf, len);
#endif

#if (CONFIG_CUSTOMER_1V2_CON_MODE == 1)
	if (app_customer_1v2_con_reject(raddr))
		return;
#endif

#ifdef CONFIG_TWS
		// classify stereo connection or normal connection
		//mobile: 0c025a //0c027a
		//itouch: 1c046a
		//slave rx master:040426
		//pc:04013e
    if(( uclass[1] == BT_DEVCLASS_AUDIO_VIDEO )&&( uclass[2] == 0x26))
    {
        os_memcpy( (void *)app_bt_get_handle(2), (uint8 *)raddr, 6);
        bt_flag2_operate(APP_FLAG2_STEREO_AUTOCONN_RETRY, 0 );
        if( bt_flag1_is_set( APP_FLAG_MUSIC_PLAY ))
        {
            unit_acl_reject_stereo(app->unit, raddr,
        	    HCI_ERR_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES);
        }
        if( bt_flag2_is_set( APP_FLAG2_MATCH_BUTTON)
            ||bt_flag1_is_set( APP_FLAG_A2DP_CONNECTION|APP_FLAG_ACL_CONNECTION ))
        {
            os_printf("stereo reject\r\n");
            unit_acl_reject_stereo(app->unit, raddr,
        	    HCI_ERR_CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS);
            bt_flag2_operate( APP_FLAG2_STEREO_INQUIRY_RES, 1 );
            bt_flag2_operate( APP_FLAG2_STEREO_ROLE_MASTER, 1 );
            app_env_update_stereo_role_info(1);
            app_bt_shedule_task( (jthread_func)app_bt_inquiry_active_conn, (void *)10000,4000);
            return;
        }
        else
        {
            os_printf("switch to slave\r\n");
            app_bt_inquiry( 0, 0, 1 );
            bt_flag2_operate( APP_FLAG2_STEREO_INQUIRY_RES, 0 );
            bt_flag2_operate( APP_FLAG2_STEREO_ROLE_MASTER, 0 );
            app_env_update_stereo_role_info(0);
            bt_flag2_operate( APP_FLAG2_STEREO_CONNECTTING,  1 );
        }
    }
    else if( bt_flag1_is_set( APP_FLAG_A2DP_CONNECTION ))
    {
    unit_acl_reject_stereo(app->unit, raddr,
    	    HCI_ERR_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES);
    }
    else
#endif
    {
        #if !defined(BT_ONE_TO_MULTIPLE) || (CONFIG_APP_RECONNECT_MATCH == 1)
        bt_auto_connect_stop();
        #endif

    }
    /* TODO: bt_unit_acl_accept/reject should be called by user */
    rc = bt_unit_acl_accept(app->unit, raddr);
    if (rc)
        DBG_E(DBT_APP, ("%s: bt_unit_acl_accept failed %d\n", FNAME, rc));
}


static void bt_remote_name_complete(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t status, const uint8_t *name,
    uint8_t name_len)
{
	u_int8 have_name_flag = 0;
    bt_remote_dev_t *rdev = NULL;
	
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;
#endif
    DECLARE_FNAME("bt_remote_name_complete");

    if (status)
    {
        /* Timeout (code 0x4) is a typical cause of failure */
        DBG_E(DBT_APP, ("%s: " BTADDR_FORMAT " completed with error 0x%x\n",
            FNAME, BTADDR(raddr), status));
    }
    else
    {
        DBG_I(DBT_APP, ("%s: " BTADDR_FORMAT "\n", FNAME, BTADDR(raddr)));
        DBG_HEX_DUMP(DBT_APP, DL_INFO, name, name_len, 0);
    }

#ifdef BEKEN_DEBUG
    len = j_snprintf(buf, sizeof(buf), "remote_name_complete " BTADDR_FORMAT
        " %02x %s", BTADDR(raddr), status, status ? "" : (char *)name);
    frontend_notify(buf, len);
#endif
#if 0
    LIST_FOREACH(rdev, &g_remote_dev_list, next)
    {
        if (!btaddr_same(raddr, &rdev->addr))
            continue;

        if (status)
        {
            /* In error, retry later */
            rdev->resolve_pending = 0;
            break;
        }

        rdev->name_resolved = 1;
        rdev->resolve_pending = 0;
        j_memcpy(&rdev->name, name, sizeof(rdev->name));
        break;
    }
#else
    LIST_FOREACH(rdev, &g_remote_dev_list, next)
    {
        if(btaddr_same(raddr, &rdev->addr))
        {
            have_name_flag = 1;
	    	break;
        }
    }

    if(!have_name_flag)
    {
        rdev = (bt_remote_dev_t *)jmalloc(sizeof(bt_remote_dev_t), M_ZERO); 
        btaddr_copy(&rdev->addr, raddr);
        //rdev->uclass = NULL;
        j_memcpy(&rdev->name, name, sizeof(rdev->name));
        rdev->name_resolved = 1;
        rdev->resolve_pending = 0;

        LIST_INSERT_HEAD(&g_remote_dev_list, rdev, next);
    }
#endif
}

#if defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)
hci_link_t *bt_get_link_from_raddr(bt_unit_h unit, const btaddr_t *raddr)
{
    hci_link_t *link;
    hci_link_t *my_link;

    my_link = 0;
    TAILQ_FOREACH(link, &unit->hci_links, hl_next)
    {
        if(btaddr_same(raddr, &(link->hl_btaddr)))
        {
            my_link = link;
            break;
        }
    }

    return my_link;
}

hci_link_t *bt_get_link_from_conn_handle(hci_unit_t *unit, uint16_t conn_handle)
{
    hci_link_t *link;
    hci_link_t *my_link;

    my_link = 0;
    TAILQ_FOREACH(link, &unit->hci_links, hl_next)
    {
        if(conn_handle == link->hl_handle)
        {
            my_link = link;
            break;
        }
    }

    return my_link;
}
#endif

static void bt_read_remote_features_complete(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t status, bt_lmp_features_t features)
{
#ifdef BT_ONE_TO_MULTIPLE
    bt_unit_h unit;
    hci_link_t *my_link;
    bt_unit_app_t *app;

    app = (bt_unit_app_t *)app_ctx_h;
    unit = app->unit;

    my_link = bt_get_link_from_raddr(unit, raddr);
    if(my_link)
    {
        if(features[0] & HCI_LMP_ROLE_SWITCH)
        {
            os_printf("remote:"BTADDR_FORMAT"-support-role_switch\r\n", BTADDR(raddr));
            my_link->support_role_switch = HCI_LINK_POLICY_ENABLE_ROLE_SWITCH;
        }
        else
        {
            os_printf("remote:BTADDR_FORMAT-unsupport-role_switch\r\n", BTADDR(raddr));
            my_link->support_role_switch = 0;
        }
    }
#endif
}

static void bt_sec_db_changed(bt_app_ctx_h app_ctx_h)
{
}

#ifdef CONFIG_BLUETOOTH_SSP
void bt_auto_connect_start(void);
//void bt_auto_connect_stop(void);

static void bt_simple_pairing_enable_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t status)
{
    unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_AUTH_ENABLE, status);
}
static void bt_user_confirmation_request(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint32_t value)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;

    len = j_snprintf(buf, sizeof(buf), "sec_ssp_conf_req " BTADDR_FORMAT
        " %06lu", BTADDR(raddr), value);
    frontend_notify(buf, len);
#endif
   /* if(bt_flag1_is_set(0x08|0x100))
        bt_auto_connect_stop();*/
    bt_sec_user_confirmation_reply(raddr, 1);
}

static void bt_user_passkey_request(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;

    len = j_snprintf(buf, sizeof(buf), "sec_ssp_passkey_req " BTADDR_FORMAT,
        BTADDR(raddr));
    frontend_notify(buf, len);
#endif
}

static void bt_user_passkey_notification(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint32_t passkey)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;

    len = j_snprintf(buf, sizeof(buf), "sec_ssp_passkey_notify " BTADDR_FORMAT
        " %06lu", BTADDR(raddr), passkey);
    frontend_notify(buf, len);
#endif
}

static void bt_keypress_notification(bt_app_ctx_h app_ctx_h,
    const btaddr_t *raddr, uint8_t notification_type)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;

    len = j_snprintf(buf, sizeof(buf), "sec_ssp_keypress_notify " BTADDR_FORMAT
        " %02x", BTADDR(raddr), notification_type);
    frontend_notify(buf, len);
#endif
}

extern void app_bt_reset_policy_iocap(void);

static void bt_simple_pairing_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t status, const btaddr_t *raddr)
{
#ifdef BEKEN_DEBUG
    char buf[NOTIFY_BUFFER_SIZE];
    int len;

    len = j_snprintf(buf, sizeof(buf), "sec_ssp_pairing_complete " BTADDR_FORMAT
        " %02x", BTADDR(raddr), status);
    frontend_notify(buf, len);
#endif

    //bt_flag1_operate(0x40000000, 0);
    app_bt_reset_policy_iocap();
}

#endif

int app_bt_get_disconn_event(void);

static void unit_cmd_complete(
						bt_unit_app_t *unit,
						uint8_t flag,
						hci_error_t hci_err)
{
    if(hci_err)
    {
        unit->attach_flags &= ~SAMPLE_ENABLE_FLAG_COMPLETE;
        goto Exit;
    }

    /* Ignore if flag already exists */
    if (unit->attach_flags & flag)
        return;

    /* update flags with completed command flag */
    unit->attach_flags |= flag;

    if(1 == app_bt_get_disconn_event())
        unit->attach_flags |= SAMPLE_ENABLE_FLAG_SCAN_ENABLE;

    /* Wait for more completed commands */
    if ((unit->attach_flags & SAMPLE_ENABLE_FLAG_COMPLETE) !=
        SAMPLE_ENABLE_FLAG_COMPLETE)
    {
        return;
    }

Exit:
#if ((CONFIG_CHARGE_EN == 1))
    if(!bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN)
		#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
		||!app_env_check_Charge_Mode_PwrDown()
		#endif
		)
#endif
    {
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
//	if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE))
//		app_set_led_event_action( LED_EVENT_MATCH_STATE );
//	else if(app_check_bt_mode(BT_MODE_1V2))
//		app_set_led_event_action( LED_EVENT_NO_MATCH_NO_CONN_STATE );
#else
/**** why ????
    #ifdef BT_ONLY_ONE_BT
        app_set_led_event_action( LED_EVENT_MATCH_STATE );
    #elif defined(BT_ONE_TO_MULTIPLE)
        app_set_led_event_action( LED_EVENT_NO_MATCH_NO_CONN_STATE );
    #endif
****/
#endif
    }
    os_printf("unit_cmd_complete: %lu %02x\r\n",unit->unit_id,hci_err);
/*
#ifdef CONFIG_BLUETOOTH_SSP
 #if(POWERKEY_5S_PARING == 1)
	power_on_auto_connect();
 #else
    bt_auto_connect_start();
 #endif
#endif
*/
}

static void bt_set_name_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err)
{
	os_printf("bt_set_name_complete\r\n");
    /* TODO: Notify frontend? */
    unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_SET_NAME, hci_err);
}

static void bt_set_class_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err)
{
    static uint8 flag_once_power_on = 0;
    app_handle_t sys_hdl = app_get_sys_handler();

    os_printf("bt_set_class_complete\r\n");
    /* TODO: Notify frontend? */
    unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_SET_CLASS, hci_err);
#ifdef CONFIG_BLUETOOTH_SSP
#ifdef CONFIG_BLUETOOTH_COEXIST
    flag_once_power_on = 1;
#endif

    if(flag_once_power_on)
    {// power on 第一次不启动 bt_auto_connect_start
        if(0 == app_bt_get_disconn_event())
        {
            bt_unit_set_scan_enable(sys_hdl->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);
        }
        bt_auto_connect_start();	
    }
    else
    {
        flag_once_power_on = 1;
        bt_flag1_operate(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION, 0);
    }
#endif
}

static void bt_authentication_enable_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err)
{
    /* TODO: Notify frontend? */
	os_printf("bt_authentication_enable_complete\r\n");
    unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_AUTH_ENABLE, hci_err);
}

static void bt_scan_enable_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err)
{
	os_printf("scan_enable_complete\r\n");

    if(0 == app_bt_get_disconn_event())
    {
        unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_SCAN_ENABLE, hci_err);
    }
}

static void bt_write_linkkey_complete(bt_app_ctx_h app_ctx_h,
    hci_error_t hci_err)
{
	os_printf("bt_write_linkkey_complete\r\n");

    /* TODO: Notify frontend? */
    unit_cmd_complete(app_ctx_h, SAMPLE_ENABLE_FLAG_WRITE_STORED_LINKKEY, hci_err);
}

void app_env_unit_info_init(char *name, uint8_t *dev_class, char *pin);
static void bt_enable_complete(bt_app_ctx_h app_ctx_h,
                                hci_error_t hci_err,
                                result_t err)
{
    char unit_name[32];
    bt_unit_app_t *app = (bt_unit_app_t *)app_ctx_h;
    
    uint8_t major_device_class = SAMPLE_DEVCLASS;
    uint8_t   dev_class = SAMPLE_DEVSUBCLASS;

#if defined(CONFIG_BLUETOOTH_HID)
    if(app_env_check_HID_profile_enable())
    {
        dev_class = COMBO_DEVSUBCLASS;
    }
    else
#endif
    {
        dev_class = SAMPLE_DEVSUBCLASS;
    }


    DECLARE_FNAME("bt_enable_complete");

    if (hci_err || err)
    {
        goto Error;
    }

    bt_unit_get_addr(app->unit, &app->unit_addr);
    j_snprintf(unit_name,
                    sizeof(unit_name),
                    "%s-%02x%02x%02x%02x%02x%02x",
                    SAMPLE_NAME,
                    BTADDR(&app->unit_addr));

    app_env_unit_info_init(unit_name, &dev_class, (char *)default_pin);

    err = bt_unit_set_name(app->unit, unit_name);
#ifdef BEKEN_DEBUG
    jprintf("-----------------------------------\r\n");
    jprintf("| Bluetooth name: %s\r\n",app->unit->local_name);
    jprintf("| Bluetooth addr: " BTADDR_FORMAT "\r\n",
        BTADDR(&app->unit->hci_btaddr));
    jprintf("-----------------------------------\r\n");
#endif

    if (err)
        goto Error;

#if defined(CONFIG_BLUETOOTH_HID)
    if(app_env_check_HID_profile_enable())
        err = bt_unit_set_class(app->unit, COMBO_SERVICE,COMBO_DEVCLASS,dev_class);
    else
#endif
        err = bt_unit_set_class(app->unit, SAMPLE_SERVICE, major_device_class,dev_class);

    if (err)
    {
        goto Error;
    }

#ifdef CONFIG_BLUETOOTH_SSP
    err = bt_unit_simple_pairing_enable(app->unit);
#else
    err = bt_unit_authentication_enable(app->unit, TRUE);
#endif
    if (err)
    {
        goto Error;
    }

    unit_update_ext_inq_rsp(app->unit);
#ifdef CONFIG_TWS
//	bt_unit_write_inquiry_IAC( app->unit, 0 );
    app_bt_stereo_pairing_exit();
#endif

#ifdef CONFIG_BLUETOOTH_SSP
    os_printf("bt_sec_set_io_caps\r\n");
    bt_sec_set_io_caps(HCI_IO_CAPABILITY_NO_IO);
#endif

    bt_unit_set_page_scan_type(app->unit,STANDARD_SCAN);//STANDARD_SCAN
    bt_unit_set_inquiry_scan_type(app->unit,INTERLACED_SCAN);

    os_printf("%s\r\n", __func__);
    if(app_check_bt_mode(BT_MODE_1V1|BT_MODE_DM_1V1|BT_MODE_BLE)) // 0x800:1.28s  page scan
        err = bt_unit_set_page_activity(app->unit, 0x400, 0x12);
    else
        err = bt_unit_set_page_activity(app->unit, 0x800, 0x12);

    if(err)
    {
        goto Error;
    }

    return;

Error:
    DBG_E(DBT_APP, ("%s: error, %s\n", FNAME, uwe_str(err)));
}

static void bt_disable_complete(bt_app_ctx_h app_ctx_h,
                                    hci_error_t hci_err,
                                    result_t err)
{
    bt_unit_app_t *app = (bt_unit_app_t *)app_ctx_h;

    os_printf("disable_complete %lu %02x\r\n", app->unit_id, hci_err);

    app->attach_flags = 0;
    app_bt_disable_complete_wrap();
}

/* A new HCI unit attached*/
result_t bt_app_unit_attach(bt_unit_h unit_h,
                            bt_app_ctx_h *app_ctx_h,
                            bt_unit_app_cbs_t *app_cbs)
{
    result_t rc;
    bt_unit_app_t *app;

    LOCK_CHECK;

    app = (bt_unit_app_t *)jmalloc(sizeof(bt_unit_app_t), M_ZERO);
    app->unit = unit_h;
    app->unit_id = g_unit_id++;

    TAILQ_INSERT_TAIL(&g_unit_list, app, next);

    app_cbs->pin_code_request_cb = bt_pin_code_request;
    app_cbs->acl_con_request_cb = bt_acl_con_request;
    app_cbs->inq_result_cb = NULL; //bt_inq_result;
    app_cbs->ext_inq_result_cb = NULL; //bt_ext_inq_result;
    app_cbs->inq_complete_cb = NULL; //bt_inq_complete;
    app_cbs->remote_name_complete_cb = bt_remote_name_complete;
    app_cbs->read_remote_features_complete_cb =
        bt_read_remote_features_complete;
    app_cbs->sec_db_changed_cb = bt_sec_db_changed;
    app_cbs->set_name_complete_cb = bt_set_name_complete;
    app_cbs->set_class_complete_cb = bt_set_class_complete;
    app_cbs->auth_enable_complete_cb = bt_authentication_enable_complete;
    app_cbs->scan_enable_complete_cb = bt_scan_enable_complete;
    app_cbs->enable_complete_cb = bt_enable_complete;
    app_cbs->disable_complete_cb = bt_disable_complete;
    app_cbs->write_linkkey_complete_cb = bt_write_linkkey_complete;

#ifdef CONFIG_BLUETOOTH_SSP
    app_cbs->simple_pairing_enable_complete_cb =
        bt_simple_pairing_enable_complete;
    app_cbs->user_confirmation_request_cb = bt_user_confirmation_request;
    app_cbs->user_passkey_request_cb = bt_user_passkey_request;
    app_cbs->user_passkey_notification_cb = bt_user_passkey_notification;
    app_cbs->keypress_notification_cb = bt_keypress_notification;
    app_cbs->simple_pairing_complete_cb = bt_simple_pairing_complete;
#endif

    *app_ctx_h = app;

#ifdef CONFIG_SINGLE_THREADED
    rc = jtask_init(&app->enable_task, 0);
    if(rc)
    {
        goto Error;
    }

    rc = jtask_schedule(app->enable_task, 100, backend_unit_enable, app);
    if(rc)
    {
        goto Error;
    }
#endif

    os_printf("bt_app_unit_attach id:%x\r\n", app->unit_id);

    return UWE_OK;

Error:
    if(app)
    {
        TAILQ_REMOVE(&g_unit_list, app, next);

    #ifdef CONFIG_SINGLE_THREADED
        if(app->enable_task)
        {
            jtask_uninit(app->enable_task);
            app->enable_task = NULL;
        }
    #endif

        jfree(app);
        app = NULL;
    }

    *app_ctx_h = NULL;

    return rc;
}

/*
 * An existing HCI unit detached
 */
result_t bt_app_unit_detach(bt_app_ctx_h app_ctx_h)
{
    bt_unit_app_t *ctx = (bt_unit_app_t *)app_ctx_h;
    DECLARE_FNAME("bt_app_unit_detach");

    LOCK_CHECK;

    DBG_I(DBT_APP, ("%s: entered. app %p\n", FNAME, ctx));

    /* TODO: disconnect from services */
    /* TODO: Notify frontend */
    TAILQ_REMOVE(&g_unit_list, ctx, next);

#ifdef CONFIG_SINGLE_THREADED
    if(ctx->enable_task)
    {
        jtask_uninit(ctx->enable_task);
        ctx->enable_task = NULL;
    }
#endif

    jfree(ctx);
    ctx = NULL;

    return UWE_OK;
}

static void backend_unit_enable(void *arg)
{
    bt_unit_app_t *app = (bt_unit_app_t *)arg;
    result_t err;
    DECLARE_FNAME("backend_unit_enable");

    LOCK_CHECK;

    /* Power on the radio. When done, btaddr, local features, buffer sizes
     * become available. */
    err = bt_unit_enable(app->unit);

    UNUSED_VAR(err);
    DBG_RC_I(err, DBT_APP, ("%s: done, (%s)\n", FNAME, uwe_str(err)));
}

result_t backend_unit_lookup_id(app_unit_id_t id, bt_unit_h *unit_h)
{
    bt_unit_app_t *ctx;
    result_t rc = UWE_NODEV;

    if(!unit_h)
    {
        rc = UWE_PARAM;
        goto Exit;
    }

    TAILQ_FOREACH(ctx, &g_unit_list, next)
    {
        if(ctx->unit_id != id)
            continue;
        *unit_h = ctx->unit;
        rc = UWE_OK;
        break;
    }

Exit:
    return rc;
}

result_t backend_unit_get_addr(app_unit_id_t id, btaddr_t *addr)
{
    bt_unit_app_t *ctx;
    result_t rc = UWE_NODEV;

    if(!addr)
    {
        rc = UWE_PARAM;
        goto Exit;
    }

    TAILQ_FOREACH(ctx, &g_unit_list, next)
    {
        if(ctx->unit_id != id)
            continue;
        j_memcpy((void *)addr, (void *)&ctx->unit_addr, sizeof(*addr));
        rc = UWE_OK;
        break;
    }

Exit:
    return rc;
}

char *backend_unit_get_name(const btaddr_t *raddr)
{
    bt_remote_dev_t *rdev;

    LIST_FOREACH(rdev, &g_remote_dev_list, next)
    {
        if(!btaddr_same(raddr, &rdev->addr))
            continue;
        if(!rdev->name_resolved)
            break;
        return rdev->name;
    }

    return "Unknown";
}

result_t backend_unit_remove_the_name(const btaddr_t *raddr)
{
    bt_remote_dev_t *rdev;

    LIST_FOREACH(rdev, &g_remote_dev_list, next)
    {
        if(btaddr_same(raddr, &rdev->addr))
        {
           LIST_REMOVE(rdev, next);
           jfree(rdev);
           rdev = NULL;
           return UWE_OK;
        }
    }

    return UWE_NODEV;
}

void backend_unit_remove_name(void)
{
    bt_remote_dev_t *rdev;

    // Clear all the remote device list 
    while ((rdev = LIST_FIRST(&g_remote_dev_list)) != NULL)
    {
        LIST_REMOVE(rdev, next);
        jfree(rdev);
        rdev = NULL;
    }
}
