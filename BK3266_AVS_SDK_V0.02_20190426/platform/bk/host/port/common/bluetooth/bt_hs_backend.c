/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bluetooth.h>
#include <bt_api.h>
#include <bt_hsp_hs.h>
#include "bt_app_internal.h"
#include "bt_rfcomm_channel.h"
#include "bt_sco_backend_utils.h"
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#ifdef CONFIG_BLUETOOTH_HSP

static void at_result_print(const at_result_t *at_result);

/* HS Service-level Notifications */
void hsp_hs_connected(bt_hsp_hs_session_h session, bt_app_ctx_h app_ctx);
void hsp_hs_disconnected(bt_hsp_hs_session_h session, result_t status,
    bt_app_ctx_h app_ctx);

static bt_hsp_hs_conn_cbs_t hsp_hs_conn_cbs = {
    hsp_hs_connected,
    hsp_hs_disconnected,
};

void hsp_hs_newconn(bt_conn_req_h conn_req_h, const btaddr_t *laddr,
    const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_server_ctx);

void hsp_hs_input(bt_hsp_hs_session_h session, at_result_t *at_result,
    bt_app_ctx_h app_ctx);

void hsp_get_features(bt_hsp_hs_session_h session_h,
    bool_t *is_remote_volume_control_supported, bt_app_ctx_h app_ctx);

CONST static bt_hsp_hs_cbs_t hsp_hs_cbs = {
    hsp_hs_input,
    hsp_get_features
};


result_t hs_cmd_connect(char *params, unsigned int len);
result_t hs_cmd_disconnect(void);
result_t hs_cmd_button_pressed(void);

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
static result_t hs_cmd_accept_con_req(char *params, unsigned int len);
static result_t hs_cmd_reject_con_req(char *params, unsigned int len);


static bt_app_cmd_t hs_cmds[] = {
    {"connect",
        "connect (unit) (raddr) (channel)",
        "Connect to a remote HS device.\n"
		"(channel) - The RFCOMM channel is obtained in the SDAP "
                "search notification.\n"
		"(see 'sdap' help)\n"
		"Example: hs connect 0 11:22:33:44:55:66 13",
        hs_cmd_connect},
    {"disconnect",
        "disconnect (#service)",
        "Disconnect an active HS connection.\n"
		"(#service) is retrieved from a successful connection "
                "notification:\n"
		"[hs connected <#service>]\n"
		"Example: hs disconnect 1",
        hs_cmd_disconnect},
    {"accept_con_req",
        "accept_con_req (#service)",
        "Accept an HS connection request from a remote device.\n"
		"(#service) is retrieved from a successful connection "
                "notification:\n"
		"[hs connected <#service>]\n"
		"Example: hs accept_con_req 1",
        hs_cmd_accept_con_req},
    {"reject_con_req",
        "reject_con_req (#service)",
        "Reject HS connection attempt.\n"
		"(#service) is retrieved from a successful connection "
                "notification:\n"
		"[hs connected <#service>]\n"
		"Example: hs reject_con_req 1",
        hs_cmd_reject_con_req},
    {"button_pressed",
        "button_pressed (#service)",
        "Send a key-press event to the remote device (the only required "
            "HS user interface)\n"
            "Example: hs button_pressed 1",
        hs_cmd_button_pressed},
};

static bt_app_section_t hs_section = {
    "hs",
    "Headset backend",
    hs_cmds,
    UW_ARRAY_SIZE(hs_cmds)
};
#endif

typedef struct {
    uint32_t                    svc_id;
    uint32_t                    server_sdp;
    bt_hsp_hs_server_h          server;
    bt_hsp_hs_session_h         session;

    btaddr_t                    laddr;
    btaddr_t                    raddr;
    bt_conn_req_h               conn_req;
    bt_link_modes_mask_t        conn_req_mode;

    bool_t                      is_used;
    sco_utils_h                 priv;
} hsp_hs_app_t;

/* XXX: Currently, only single service is supported by this sample */
static hsp_hs_app_t g_hsp_hs_app;

void hsp_hs_connected(bt_hsp_hs_session_h session, bt_app_ctx_h app_ctx)
{
    result_t rc;
    rc = util_sco_open(&g_hsp_hs_app.laddr, 
                        &g_hsp_hs_app.raddr,
                        &g_hsp_hs_app.priv);
    if(rc)
    {
        g_hsp_hs_app.is_used = 0;
        bt_frontend_notification("sco failed to open %s", uwe_str(rc));
    }
    else
    {
        bt_frontend_notification("hs connected %lu", g_hsp_hs_app.svc_id);
    }

    os_printf(APP_RET_STATUS_CONNECTION,"HSP");

    bt_flag1_operate(APP_FLAG_HSP_CONNECTION, 1);
}

void hsp_hs_disconnected(bt_hsp_hs_session_h session, result_t status,
    bt_app_ctx_h app_ctx)
{
    bt_frontend_notification("hs disconnected %lu", g_hsp_hs_app.svc_id);
    util_sco_close(g_hsp_hs_app.priv);

    g_hsp_hs_app.svc_id++;
    g_hsp_hs_app.is_used = 0;

    bt_flag1_operate(APP_FLAG_HSP_CONNECTION, 0);
}

void hsp_hs_newconn(bt_conn_req_h conn_req, const btaddr_t *laddr,
    const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_server_ctx)
{
    DECLARE_FNAME("hsp_hs_newconn");

    if(g_hsp_hs_app.is_used || g_hsp_hs_app.conn_req)
    {
        DBG_E(DBT_HSP, ("%s: is_used %d, conn_req %p \n", FNAME,
            g_hsp_hs_app.is_used, g_hsp_hs_app.conn_req));
        bt_hsp_hs_conn_reject(conn_req);
        return;
    }

    #ifndef BT_ONE_TO_MULTIPLE
    bt_auto_connect_stop();
    #endif

    j_memcpy((void *)&g_hsp_hs_app.laddr, (void *)laddr, sizeof(btaddr_t));
    j_memcpy((void *)&g_hsp_hs_app.raddr, (void *)raddr, sizeof(btaddr_t));

    bt_frontend_notification("hs newconn %lu", g_hsp_hs_app.svc_id);

    if(bt_hsp_hs_conn_accept(g_hsp_hs_app.session, conn_req,
        mode))
    {
        goto Exit;
    }
    g_hsp_hs_app.is_used = 1;

Exit:
    bt_frontend_notification("hs connecting %lu %s", g_hsp_hs_app.svc_id,
        g_hsp_hs_app.is_used ? "done" : "failed");
}

void hsp_hs_input(bt_hsp_hs_session_h session, at_result_t *at_result,
    bt_app_ctx_h app_ctx)
{
    at_result_print(at_result);
}

void hsp_get_features(bt_hsp_hs_session_h session_h,
    bool_t *is_remote_volume_control_supported, bt_app_ctx_h app_ctx)
{
    if(is_remote_volume_control_supported)
        *is_remote_volume_control_supported = 1;
}

static enum {
    UNINIT = 0,
    HS_REGISTER,
    SERVER_START,
    BACKEND_REGISTER,
    CONN_REGISTER
} hs_sample_init_stage = UNINIT;

void hs_backend_uninit(void);
result_t hs_backend_init(void)
{
    result_t err;
    DECLARE_FNAME("hs_backend_init");

    j_memset(&g_hsp_hs_app, 0, sizeof(g_hsp_hs_app));

    err = bt_hsp_hs_register(&hsp_hs_cbs);
    if(err)
        goto Exit;
    hs_sample_init_stage = HS_REGISTER;

    err = bt_hsp_hs_server_start(&g_hsp_hs_app.server, BTADDR_ANY,
        RFCOMM_CHANNEL_HSP, hsp_hs_newconn, NULL);
    if(err)
        goto Exit;
    hs_sample_init_stage = SERVER_START;

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    err = backend_section_register(&hs_section);
    if(err)
        goto Exit;
    hs_sample_init_stage = BACKEND_REGISTER;
#endif

    err = bt_hsp_hs_conn_create(&g_hsp_hs_app.session, &hsp_hs_conn_cbs,
        NULL);
    if(err)
        goto Exit;
    hs_sample_init_stage = CONN_REGISTER;

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s, stage: %d\n", FNAME, uwe_str(err),
        hs_sample_init_stage));

    if(err)
        hs_backend_uninit();

    return err;
}

void hs_backend_uninit(void)
{
    if(hs_sample_init_stage == CONN_REGISTER)
    {
        if(g_hsp_hs_app.conn_req)
            (void)bt_hsp_hs_conn_reject(g_hsp_hs_app.conn_req);

        bt_hsp_hs_conn_destroy(&g_hsp_hs_app.session);
        hs_sample_init_stage = BACKEND_REGISTER;
    }

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    if(hs_sample_init_stage == BACKEND_REGISTER)
    {
        backend_section_unregister(&hs_section);
        hs_sample_init_stage = SERVER_START;
    }
#endif

    if(hs_sample_init_stage == SERVER_START)
    {
        bt_hsp_hs_server_stop(&g_hsp_hs_app.server);
        hs_sample_init_stage = HS_REGISTER;
    }

    if(hs_sample_init_stage == HS_REGISTER)
    {
        bt_hsp_hs_unregister();
        hs_sample_init_stage = CONN_REGISTER;
    }
}

result_t hs_cmd_connect(char *params, unsigned int len)
{
    result_t err;
    btaddr_t laddr, raddr;
    int btaddr_tmp[6];
    uint32_t channel;
    uint32_t unit_id;
    DECLARE_FNAME("hs_cmd_connect");

    if(g_hsp_hs_app.is_used)
    {
        err = UWE_ALREADY;
        goto Exit;
    }

    if(j_snscanf(params, NULL, "%u " BTADDR_FORMAT " %u", &unit_id,
        BTADDR_TMP(btaddr_tmp), &channel) != 8)
    {
        err = UWE_INVAL;
        goto Exit;
    }

    TMP_TO_BTADDR(btaddr_tmp, &raddr);

    err = backend_unit_get_addr(unit_id, &laddr);
    if(err)
    {
        err = UWE_NODEV;
        goto Exit;
    }

    j_memcpy((void *)&g_hsp_hs_app.laddr, (void *)&laddr, sizeof(btaddr_t));
    j_memcpy((void *)&g_hsp_hs_app.raddr, (void *)&raddr, sizeof(btaddr_t));

    err = bt_hsp_hs_conn_connect(g_hsp_hs_app.session, &laddr, &raddr, channel);
    if(err)
        goto Exit;

    g_hsp_hs_app.is_used = 1;
    bt_frontend_notification("hs connecting %lu", g_hsp_hs_app.svc_id);
    os_printf(APP_RET_STATUS_CONNECTING,"HSP");

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: Done (%s)\n", FNAME, uwe_str(err)));
    return err;
}

result_t hs_cmd_disconnect(void)
{
    if(!g_hsp_hs_app.session)
        return UWE_NODEV;

    return bt_hsp_hs_conn_disconnect(g_hsp_hs_app.session);
}

result_t hs_cmd_button_pressed(void)
{
    return bt_hsp_hs_button_pressed(g_hsp_hs_app.session);
}

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE

static result_t hs_cmd_accept_con_req(char *params, unsigned int len)
{
    result_t rc;
    uint32_t svc_id;

    if(j_snscanf(params, NULL, "%u", &svc_id) != 1)
        return UWE_INVAL;

    if(svc_id != g_hsp_hs_app.svc_id)
        return UWE_PARAM;

    if(g_hsp_hs_app.is_used)
        return UWE_ALREADY;

    if(!g_hsp_hs_app.conn_req)
        return UWE_STATE;

    rc = bt_hsp_hs_conn_accept(g_hsp_hs_app.session, g_hsp_hs_app.conn_req,
        g_hsp_hs_app.conn_req_mode);
    if(rc)
        goto Exit;

    g_hsp_hs_app.is_used = 1;
    bt_frontend_notification("hs connecting %lu", g_hsp_hs_app.svc_id);

Exit:
    g_hsp_hs_app.conn_req = NULL;
    return rc;
}

static result_t hs_cmd_reject_con_req(char *params, unsigned int len)
{
    uint32_t svc_id;

    if(j_snscanf(params, NULL, "%u", &svc_id) != 1)
        return UWE_INVAL;

    if(svc_id != g_hsp_hs_app.svc_id)
        return UWE_PARAM;

    if(g_hsp_hs_app.session || !g_hsp_hs_app.conn_req)
        return UWE_STATE;

    (void)bt_hsp_hs_conn_reject(g_hsp_hs_app.conn_req);

    g_hsp_hs_app.conn_req = NULL;
    return UWE_OK;
}

#endif

static void at_result_print(const at_result_t *at_result)
{
    switch (at_result->code)
    {
    case AT_RESULT_RING:
        bt_frontend_notification("AG ringing ...");
        break;

    case AT_RESULT_VGS:
        bt_frontend_notification("AG speaker volume changed %d",
            at_result->u.vgs);
        aud_volume_set(at_result->u.vgs);
        break;

    case AT_RESULT_VGM:
        bt_frontend_notification("AG microphone volume changed %d",
            at_result->u.vgm);
        aud_mic_volume_set(at_result->u.vgs);
        break;

    case AT_RESULT_BSIR:
        bt_frontend_notification("in-band ringtone is%s audible",
            at_result->u.bsir ? "" : " not");
        break;
        
    default:
        break;
    }
}
#endif

