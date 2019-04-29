/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bluetooth.h>
#include <bt_avrcp_ctrl.h>
#include <bt_api.h>
#include <bt_sdp.h>
#include <bt_sdp_types.h>
#include "bt_app_internal.h"
#include "app_beken_includes.h"
#include "avctp_internal.h"
#ifdef CONFIG_TWS
#include "beken_external.h"
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP_CT
static sdp_avrcp_profile_t avrcp_profile_ct = {
    BT_SVC_TYPE_AV_REMOTE_CONTROL,        /* Control or Target */
#ifdef CONFIG_BLUETOOTH_AVRCP_BROWSE
    SDP_AVRCP_SUPPORTS_BROWSING |         /* Supported features */
#endif
    SDP_AVRCP_PLAYER_RECORDER,
    "AVRCP-CT"          /* Service name */
};
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP_TG
static sdp_avrcp_profile_t avrcp_profile_tg = {
    BT_SVC_TYPE_AV_REMOTE_CONTROL_TARGET,  /*Target */
    SDP_AVRCP_MONITOR_AMPLIFIER,
    "AVRCP-TG"          /* Service name */
};
#endif

typedef struct {
    /* Only single service is supported by this sample */
    uint32_t                svc_id;
#ifdef CONFIG_BLUETOOTH_AVRCP_TG
    uint32_t                server_sdp[2];
#else
    uint32_t                server_sdp;
#endif
    uint32_t                flag;

    bt_avrcp_ctrl_server_h  server;
    bt_avrcp_ctrl_session_h session;

    uint32_t                attr_id;
    uint16_t                char_set_id;
    uint8_t                 last_tid;

    avrcp_op_t              continuation_tag;

    bt_conn_req_h           conn_req;

    uint16_t                is_used;
    bt_link_modes_mask_t    conn_req_mode;

    uint8_t                 last_cmd;
    jtask_h                 task;

    btaddr_t                raddr;
	uint8_t                 volume_tid;      //only for volume_change_reponse use who support vol sync
} avrcp_app_t;

static void avrcp_send_opcode(avrcp_app_t *app_ptr, void *arg);
static void avrcp_paththrough_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t rsp, bt_app_ctx_h app_ctx);
static void avrcp_getcaps_events_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t response, avrcp_event_id_t *events,
    uint8_t count, avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag, bt_app_ctx_h app_ctx);
static void avrcp_event_rsp(bt_avrcp_ctrl_session_h session, avrcp_tid_t tid,
    avc_response_t response, avrcp_event_rsp_t *ev,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx);
static void avrcp_get_nowplaying_attr_rsp(bt_avrcp_ctrl_session_h session_h,
    avrcp_tid_t tid, avc_response_t rsp, uint32_t attr_id, uint16_t char_set_id,
    uint16_t attr_total_len, char *attr_val, uint16_t attr_val_len,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx_h);
static void avrcp_get_play_status_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t rsp, avrcp_get_play_status_rsp_t *status,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx);
#ifdef CONFIG_TWS
static void avrcp_get_passthrough_req(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, uint8_t op_id, void *payload, uint8_t len);
#endif

result_t avrcp_cmd_connect(char *params, unsigned int len);
result_t avrcp_cmd_disconnect(void);
static result_t avrcp_cmd_accept(avrcp_app_t *app_ptr, char *params, unsigned int len);
result_t avrcp_cmd_register_notification(bt_avrcp_ctrl_session_h session,uint8_t event_id, uint32_t playback_pos_changed_interval);

/* Service-level notifications */
static void avrcp_svc_connected(bt_avrcp_ctrl_session_h session,
                                bt_app_ctx_h app_ctx);
static void avrcp_svc_disconnected(bt_avrcp_ctrl_session_h session,
                                result_t status, bt_app_ctx_h app_ctx);
static void avrcp_svc_newconn(bt_conn_req_h conn_req,
                                const btaddr_t *laddr,
                                const btaddr_t *raddr,
                                bt_link_modes_mask_t mode,
                                bt_app_ctx_h app_server_ctx);
static void avrcp_set_flag(avrcp_app_t *app_ptr, uint32 flag);
static void avrcp_clear_flag(avrcp_app_t *app_ptr, uint32 flag);
#ifdef CONFIG_TWS
static void avrcp_ct_connected(bt_avrcp_ctrl_session_h session,
    bt_app_ctx_h app_ctx);
static void avrcp_ct_disconnected(bt_avrcp_ctrl_session_h session,
    result_t status, bt_app_ctx_h app_ctx);
#endif

//extern void app_bt_sdp_service_connect(void);

static bt_avrcp_ctrl_cbs_t avrcp_responses = {
    avrcp_paththrough_rsp,
    avrcp_getcaps_events_rsp,
    avrcp_event_rsp,
    avrcp_get_nowplaying_attr_rsp,
    avrcp_get_play_status_rsp,
#ifdef CONFIG_TWS
	avrcp_get_passthrough_req,
#endif
};

static bt_avrcp_ctrl_conn_cbs_t svc_cbs = {
    avrcp_svc_connected,
    avrcp_svc_disconnected,
};
#ifdef CONFIG_TWS
static bt_avrcp_ctrl_conn_cbs_t svc_cbs_ct = {
    avrcp_ct_connected,
    avrcp_ct_disconnected,
};
static avrcp_app_t g_app_ct;
#endif

static avrcp_app_t g_avrcp_app;
static avrcp_app_t *g_avrcp_current_app;
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
static avrcp_app_t *g_avrcp_array[2] = {0};
#else
static avrcp_app_t *g_avrcp_array[BT_MAX_AG_COUNT] = {0};
#endif
avrcp_app_t *avrcp_get_app_from_session(bt_avrcp_ctrl_session_h session)
{
    uint32_t i;
    avrcp_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_avrcp_array[i] && (session == g_avrcp_array[i]->session))
        {
            app_ptr = g_avrcp_array[i];
            break;
        }
    }

#ifdef BEKEN_DEBUG
    if (!app_ptr)
        os_printf("avrcp_get_app_from_session is NULL\r\n");
#endif

    return app_ptr;
}

#ifdef BT_ONE_TO_MULTIPLE
static uint32_t g_avrcp_count = 0;
uint32_t avrcp_app_register(avrcp_app_t *ptr)
{
    uint32_t i;

    if(g_avrcp_count >= BT_MAX_AG_COUNT)
    {
        return UWE_FULL;
    }

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((avrcp_app_t *)0 == g_avrcp_array[i])
        {
            g_avrcp_array[i] = ptr;
            g_avrcp_count ++;

            //os_printf("avrcp_app_register:%x\r\n", ptr);

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

uint32_t avrcp_app_unregister(avrcp_app_t *ptr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(ptr == g_avrcp_array[i])
        {
            g_avrcp_array[i] = 0;
            g_avrcp_count --;

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

avrcp_app_t *avrcp_app_lookup_valid(const btaddr_t *raddr)
{
    uint32_t i;
    avrcp_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (btaddr_same(raddr, &app_ptr->raddr)))
        {
            return app_ptr;
        }
    }

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (!app_ptr->is_used))
        {
            break;
        }
    }

    return app_ptr;
}

avrcp_app_t *avrcp_ex_app_alloc(void)
{
    avrcp_app_t *app_ptr;

    app_ptr = (avrcp_app_t *)jmalloc(sizeof(avrcp_app_t), M_ZERO);

    return app_ptr;
}

void avrcp_ex_app_free(avrcp_app_t *app_ptr)
{
    jfree(app_ptr);
    app_ptr = NULL;
}

uint32_t avrcp_ex_app_init(avrcp_app_t *app_ptr)
{
    result_t err;

    os_printf("avrcp_ex_app_init\r\n");

    err = jtask_init(&app_ptr->task, J_TASK_TIMEOUT);
    if(err)
    {
        goto Exit;
    }

    err = bt_avrcp_ctrl_conn_create(&app_ptr->session, &svc_cbs, NULL);

Exit:

    return err;
}

uint32_t avrcp_ex_app_uninit(avrcp_app_t *app_ptr)
{
    if(app_ptr->server)
    {
        bt_avrcp_ctrl_server_stop(&app_ptr->server);
    }
 #ifdef CONFIG_BLUETOOTH_AVRCP_TG
    if(app_ptr->server_sdp[0])
    {
        bt_sdp_service_unregister(app_ptr->server_sdp[0]);
    }
    if(app_ptr->server_sdp[1])
    {
        bt_sdp_service_unregister(app_ptr->server_sdp[1]);
    }
#else
    if(app_ptr->server_sdp)
    {
        bt_sdp_service_unregister(app_ptr->server_sdp);
    }
#endif
    if(app_ptr->conn_req)
    {
        bt_avrcp_ctrl_conn_reject(app_ptr->conn_req);
    }

    if(app_ptr->session)
    {       
        bt_avrcp_ctrl_conn_destroy(&app_ptr->session);
    }

    if(app_ptr->task)
    {
        jtask_uninit(app_ptr->task);
        app_ptr->task = NULL;
    }

    return UWE_OK;
}

uint32_t avrcp_app_get_count(void)
{
    return g_avrcp_count;
}

uint32 avrcp_is_connection(void)
{
    uint32_t i;
    uint32_t connected=0;
    avrcp_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if(app_ptr)
        {
            if(app_ptr->flag & APP_FLAG_AVCRP_CONNECTION)
            {
                connected = 1;
                break;
            }
        }
    }

    return connected;
}

uint32_t avrcp_has_the_connection(btaddr_t addr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_avrcp_array[i])
            && btaddr_same(&g_avrcp_array[i]->raddr,&addr)
            && (g_avrcp_array[i]->flag & APP_FLAG_AVCRP_CONNECTION))
        {
            return 1;
        }
    }

    return 0;
}
uint32 avrcp_is_cmd_processing(void)
{
    uint32_t i;
    uint32_t processing=0;
    avrcp_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if(app_ptr)
        {
            if(app_ptr->flag & APP_FLAG_AVCRP_PROCESSING)
            {
                processing = 1;
                break;
            }
        }
    }

    return processing;
}

#else
uint32 avrcp_is_connection(void)
{
    return (g_avrcp_app.flag & APP_FLAG_AVCRP_CONNECTION);
}

uint32 avrcp_is_cmd_processing(void)
{
    return (g_avrcp_app.flag & APP_FLAG_AVCRP_PROCESSING);
}
#endif

int avrcp_is_connected_based_on_raddr(const btaddr_t *raddr)
{
    uint32_t i;
    result_t ret;
    btaddr_t r_addr;
    uint32_t connected=0;
    avrcp_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (app_ptr->is_used)
            && (app_ptr->session))
        {
            ret = avrcp_peeraddr(app_ptr->session, &r_addr);
            if((UWE_OK == ret)
                && (btaddr_same(raddr, &r_addr))
                && (app_ptr->flag & APP_FLAG_AVCRP_CONNECTION))
            {
                connected = 1;
                break;
            }
        }
    }

    return connected;
}

uint32 avrcp_current_is_processing(void)
{
    return (g_avrcp_current_app->flag & APP_FLAG_AVCRP_PROCESSING);
}

uint32 avrcp_current_is_connected(void)
{
    return (g_avrcp_current_app->flag & APP_FLAG_AVCRP_CONNECTION);
}

result_t avrcp_backend_init(void)
{
    result_t err;
    os_printf("avrcp_backend_init\r\n");

    err = bt_avrcp_ctrl_register(&avrcp_responses);
    if(err)
    {
        return err;
    }

    j_memset(&g_avrcp_app, 0, sizeof(g_avrcp_app));
	#ifdef CONFIG_TWS
		j_memset(&g_app_ct, 0, sizeof(g_app_ct));
	#endif

    err = jtask_init(&g_avrcp_app.task, J_TASK_TIMEOUT);
    if(err)
    {
        goto Error;
    }
#ifdef CONFIG_TWS
	err = jtask_init(&g_app_ct.task, J_TASK_TIMEOUT);
	if (err)
	{
		DBG_E(DBT_APP, ("%s: %s\n", FNAME, uwe_str(err)));
		goto Error;
	}
#endif

    err = bt_avrcp_ctrl_conn_create(&g_avrcp_app.session, &svc_cbs, NULL);
    if(err)
    {
        goto Error;
    }
#ifdef CONFIG_TWS
	err = bt_avrcp_ctrl_conn_create(&g_app_ct.session, &svc_cbs_ct, NULL);
	if (err)
	{
		DBG_E(DBT_APP, ("%s: %s\n", FNAME, uwe_str(err)));
		goto Error;
	}
#endif

    err = bt_avrcp_ctrl_server_start(&g_avrcp_app.server,
                                        BTADDR_ANY,
                                        avrcp_svc_newconn,
                                        NULL);
    if(err)
    {
        goto Error;
    }

#if defined (CONFIG_BLUETOOTH_AVRCP_CT) && defined (CONFIG_BLUETOOTH_AVRCP_TG)
#ifdef CONFIG_BLUETOOTH_AVRCP_CT
    err = bt_sdp_service_register(BTADDR_ANY, BT_SVC_TYPE_AV_REMOTE_CONTROL,
        &avrcp_profile_ct, sizeof(avrcp_profile_ct), &g_avrcp_app.server_sdp[0]);
    if (err)
    {
        goto Error;
    }
#endif
#if 1/*(CONFIG_APP_TOOLKIT_5 == 1)*/||defined(CONFIG_BLUETOOTH_AVRCP_TG)
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
	extern int app_env_check_AVRCP_TG_profile_enable(void);
	if(app_env_check_AVRCP_TG_profile_enable())
    #endif
	{
	    err = bt_sdp_service_register(BTADDR_ANY, BT_SVC_TYPE_AV_REMOTE_CONTROL_TARGET,
	        &avrcp_profile_tg, sizeof(avrcp_profile_tg), &g_avrcp_app.server_sdp[1]);
	    if (err)
	    {
	        goto Error;
	    }
	}
#endif
#else
    err = bt_sdp_service_register(BTADDR_ANY, BT_SVC_TYPE_AV_REMOTE_CONTROL,
        &avrcp_profile_ct, sizeof(avrcp_profile_ct), &g_avrcp_app.server_sdp);
    if (err)
    {
        goto Error;
    }
#endif

    #ifdef BT_ONE_TO_MULTIPLE
    avrcp_app_register(&g_avrcp_app);
    #else
    g_avrcp_array[0] = &g_avrcp_app;
    #endif

    g_avrcp_current_app = &g_avrcp_app;

    return UWE_OK;

Error:
    bt_avrcp_ctrl_unregister();

    return err;
}

void avrcp_backend_uninit(void)
{
    if(g_avrcp_app.server)
    {
        bt_avrcp_ctrl_server_stop(&g_avrcp_app.server);
    }
#ifdef CONFIG_BLUETOOTH_AVRCP_TG
     if(g_avrcp_app.server_sdp[0])
    {
        bt_sdp_service_unregister(g_avrcp_app.server_sdp[0]);
    }
    if(g_avrcp_app.server_sdp[1])
    {
        bt_sdp_service_unregister(g_avrcp_app.server_sdp[1]);
    }
#else
    if(g_avrcp_app.server_sdp)
    {
        bt_sdp_service_unregister(g_avrcp_app.server_sdp);
    }
#endif

    if(g_avrcp_app.conn_req)
    {
        bt_avrcp_ctrl_conn_reject(g_avrcp_app.conn_req);
    }

    if(g_avrcp_app.session)
    {
        bt_avrcp_ctrl_conn_destroy(&g_avrcp_app.session);
    }

    if(g_avrcp_app.task)
    {
        jtask_uninit(g_avrcp_app.task);
        g_avrcp_app.task = NULL;
    }
#ifdef CONFIG_TWS
	if(g_app_ct.task)
	{
		jtask_uninit(g_app_ct.task);
		g_app_ct.task = NULL;
	}
	if(g_app_ct.session)
		bt_avrcp_ctrl_conn_destroy(&g_app_ct.session);
#endif

    bt_avrcp_ctrl_unregister();

    #ifdef BT_ONE_TO_MULTIPLE
    avrcp_app_unregister(&g_avrcp_app);

    {
        uint32_t i;
        avrcp_app_t *app_ptr=NULL;

        for(i = 0; i < BT_MAX_AG_COUNT; i ++)
        {
            if((g_avrcp_array[i])
                && (&g_avrcp_app != g_avrcp_array[i]))
            {
                app_ptr = g_avrcp_array[i];

                avrcp_app_unregister(app_ptr);
                avrcp_ex_app_uninit(app_ptr);
                avrcp_ex_app_free(app_ptr);
            }
        }
    }
    #endif

}

static void current_send_pass_through_cmd(void * arg)
{
    result_t err;
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;

    g_avrcp_current_app->last_tid = (g_avrcp_current_app->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    err = bt_avrcp_send_pass_through_cmd(g_avrcp_current_app->session,
                                            g_avrcp_current_app->last_tid, avc_op,
                                            AVC_RELEASE);

    //if(err)
    //{
    //    os_printf("send_cmd_failed:%x:%x\r\n", avc_op, err);
    //}
}

static void avrcp_send_pass_through_cmd_by_app_ptr(avrcp_app_t *app_ptr,void * arg)
{
    result_t err;
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;

    app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    err = bt_avrcp_send_pass_through_cmd(app_ptr->session,
                                            app_ptr->last_tid, avc_op,
                                            AVC_RELEASE);

    //if(err)
    //{
    //    os_printf("send_cmd_failed:%x:%x\r\n", avc_op, err);
    //}
}

avrcp_app_t *avrcp_get_app_ptr_by_raddr(const btaddr_t *raddr)
{
    uint32_t i;
    btaddr_t *r_addr;
    avrcp_app_t *app_ptr=NULL;
    avrcp_app_t *return_app_ptr=NULL;

    //os_printf("sraddr "BTADDR_FORMAT"\r\n", BTADDR(raddr));
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (app_ptr->is_used)
            && (app_ptr->session))
        {
            r_addr = &app_ptr->raddr;
            //os_printf("r_addr "BTADDR_FORMAT"\r\n", BTADDR(r_addr));
            if(btaddr_same(raddr, r_addr))
            {
                return_app_ptr = app_ptr;

                //os_printf("get:%x\r\n", app_ptr);
                break;
            }
        }
    }

    return return_app_ptr;
}

void avrcp_send_opcode_by_raddr(const btaddr_t *raddr, void *arg)
{
    avrcp_app_t *app_avrcp_ptr;

    //os_printf("avrcp_send_opcode_by_raddr\r\n");
    app_avrcp_ptr = avrcp_get_app_ptr_by_raddr(raddr);
    if(app_avrcp_ptr)
    {
        avrcp_send_opcode(app_avrcp_ptr, arg);
    }
    //else
    //{
    //    os_printf("no_avrcp_ptr\r\n");
    //}
}

void avrcp_send_opcode(avrcp_app_t *app_ptr, void *arg)
{
    result_t err;
    int32_t duration = 500;
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;
    //app_env_handle_t env_h = app_env_get_handle();

    if(!app_ptr->is_used)
    {
        return;
    }

    app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;
    app_ptr->last_cmd = avc_op;
    err = bt_avrcp_send_pass_through_cmd(app_ptr->session,
                                            app_ptr->last_tid,
                                            avc_op,
                                            AVC_PRESS);
    //if(err)
    //{
    //    os_printf("tx_cmd_failed:%x\r\n", avc_op);
    //}

    //duration = env_h->env_cfg.button_para.repeat - 10;

    if((avc_op == AVC_OP_REWIND) || (avc_op == AVC_OP_FAST_FORWARD))
    {
        app_bt_shedule_task(current_send_pass_through_cmd, arg, duration);
    }
    else
    {
        //current_send_pass_through_cmd(arg);
        avrcp_send_pass_through_cmd_by_app_ptr(app_ptr,arg);
    }

    os_printf("send_opcode\r\n");
    avrcp_set_flag(app_ptr, APP_FLAG_AVCRP_PROCESSING);
}

void avrcp_current_send_opcode(void *arg)
{
    avrcp_send_opcode(g_avrcp_current_app, arg);
}

void send_get_play_status_cmd(avrcp_app_t *app_ptr)
{
	app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

	bt_avrcp_send_get_play_status_cmd(app_ptr->session, app_ptr->last_tid);
//	bt_avrcp_send_get_element_attr_cmd(g_app.session, g_app.last_tid,3);

}
result_t avrcp_cmd_getcapability(avrcp_app_t *app_ptr)
{

    app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

   return bt_avrcp_send_get_capabilities_cmd(app_ptr->session, app_ptr->last_tid,
        AVRCP_CAP_EVENTS_SUPPORTED);

}

static void avrcp_svc_connected(bt_avrcp_ctrl_session_h session,
    bt_app_ctx_h app_ctx)
{
    avrcp_app_t *app_ptr;
    app_handle_t sys_hdl = app_get_sys_handler();

    #ifdef A2DP_PATCH_FOR_AVRCP
    a2dp_terminate_try_avrcp();
    #endif

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif

    os_printf("avrcp_svc_connected\r\n");

    avrcp_set_flag(app_ptr, APP_FLAG_AVCRP_CONNECTION);
#ifdef BT_ONE_TO_MULTIPLE
	if(!a2dp_has_music()) //if there is no music,we update the current_avrcp_ptr to the new connection  
    	g_avrcp_current_app = app_ptr;  
#endif
#ifdef BT_DUALMODE
    app_bt_set_role_conning();
#endif

    app_bt_allpro_conn_start(AUTO_RE_CONNECT_SCHED_DELAY_AVRCP,&app_ptr->raddr);
    avrcp_cmd_getcapability(app_ptr);
#ifdef CONFIG_BLUETOOTH_AVRCP_TG
	
	jtask_stop(sys_hdl->app_save_volume_task);
	jtask_schedule(sys_hdl->app_save_volume_task,2000,avrcp_volume_sync,(void *)NULL);
#endif	
/*  // move to app_bt_profile_conn_wrap
    if(sys_hdl->unit)
    {
#ifdef BT_ONE_TO_MULTIPLE
        if(hci_get_acl_link_count(sys_hdl->unit) >= BT_MAX_AG_COUNT)
        {
            bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
        }
        else
        {
        	bt_unit_set_scan_enable(sys_hdl->unit, HCI_PAGE_SCAN_ENABLE);
        }
#else
        bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
#endif
    }*/

#if 0
	jtask_schedule(app_common_get_task_handle(), 2000, (jthread_func)app_bt_sdp_service_connect, NULL );
#endif

}

static void avrcp_svc_disconnected(bt_avrcp_ctrl_session_h session,
                                    result_t status,
                                    bt_app_ctx_h app_ctx)
{
    avrcp_app_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif

    app_ptr->is_used = 0;
    jtask_stop(app_ptr->task);

    os_printf("avrcp_disconn src_id:%x\r\n", app_ptr->svc_id);

    avrcp_clear_flag(app_ptr, APP_FLAG_AVCRP_CONNECTION|APP_FLAG_AVCRP_PROCESSING|APP_FLAG2_VOL_SYNC|APP_FLAG2_VOL_SYNC_OK);

    #ifdef BT_ONE_TO_MULTIPLE
	if(avrcp_is_connection())
		avrcp_select_current_app_to_another(&app_ptr->raddr); //use for buttion_play_pause action       
	else	
    {
		g_avrcp_current_app = NULL;
	 //os_printf("====current_avrcp_ptr is null=====\r\n");
    }
	
    if(app_ptr != &g_avrcp_app)
    {		
        avrcp_ex_app_uninit(app_ptr);
        avrcp_app_unregister(app_ptr);
        jfree(app_ptr);
        app_ptr = NULL;
    }
    #endif
}

static void avrcp_svc_newconn(bt_conn_req_h conn_req,
                                const btaddr_t *laddr,
                                const btaddr_t *raddr,
                                bt_link_modes_mask_t mode,
                                bt_app_ctx_h app_server_ctx)
{
    uint32_t len;
    char strbuf[5];
    avrcp_app_t *app_ptr;

#ifdef SUPPORT_SPP_IN_MP3PLAYER
    app_handle_t app_h = app_get_sys_handler();
    if(SYS_WM_BT_MODE !=app_h->sys_work_mode)
    {
        os_printf("avrcp reject\r\n");
        bt_avrcp_ctrl_conn_reject(conn_req);
        return;
    }
#endif

    #ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_app_lookup_valid(raddr);

    if((0 == app_ptr) && (avrcp_app_get_count() < BT_MAX_AG_COUNT))
    {
        //os_printf("avrcp_svc_newconn\r\n");
        app_ptr = avrcp_ex_app_alloc();
        if(app_ptr)
        {
            avrcp_ex_app_init(app_ptr);
            avrcp_app_register(app_ptr);
        }
    }
    else
    {
        //os_printf("avrcp_svc_newconn:%x\r\n", app_ptr);
    }
    #else
    app_ptr = &g_avrcp_app;
    #endif

    if((0 == app_ptr)
            || app_ptr->is_used
            || app_ptr->conn_req)
    {
        bt_avrcp_ctrl_conn_reject(conn_req);
        return;
    }

    app_ptr->conn_req      = conn_req;
    app_ptr->conn_req_mode = mode;
    btaddr_copy(&app_ptr->raddr, raddr);

    //os_printf("avrcp-new-conn src-id:%x\r\n", app_ptr->svc_id);

    /* Automatic accept any incoming connection */
    len = j_snprintf(strbuf, sizeof(strbuf), "%ld", app_ptr->svc_id);
    avrcp_cmd_accept(app_ptr, strbuf, len);
}
#ifdef CONFIG_TWS
void app_bt_disconn_timerfunc( void *arg);
#endif
uint8_t avrc_last_tid_save_before_notif = AVRCP_MAX_TRANSACTIONS; //for the bug: button_playpause_action dosen't work sometime
static void avrcp_paththrough_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t rsp, bt_app_ctx_h app_ctx)
{
    avrcp_app_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif
	#ifdef CONFIG_TWS
		jtask_stop(app_ptr->task);
	#endif

    os_printf("avrcp-reponse:%p,tid:%d,last_tid:%d\r\n", app_ctx, tid,app_ptr->last_tid);

    if(tid == app_ptr->last_tid)
    {
        avrcp_clear_flag(app_ptr, APP_FLAG_AVCRP_PROCESSING);
	#ifdef CONFIG_TWS
			if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
					&& bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)
					&&(app_ptr->last_cmd == AVC_OP_PAUSE)
					&&bt_flag1_is_set( APP_FLAG_ROLE_SWITCH ))
			{
			//this is for piconet_conn with mac.
			//disconn after mac rsp the "pause" cmd.
				app_bt_shedule_task(app_bt_disconn_timerfunc, (void *)0,1000);
			}

	#endif
	}
	else if(tid == avrc_last_tid_save_before_notif) //for the bug: button_playpause_action dosen't work sometime
	{
	   	avrcp_clear_flag(app_ptr, APP_FLAG_AVCRP_PROCESSING);
		avrc_last_tid_save_before_notif = AVRCP_MAX_TRANSACTIONS;
        //os_printf("===!!!===notif before rsp===!!!===\r\n");
	}
}

static void avrcp_handle_continuation(avrcp_app_t *app_ptr,
    avrcp_pkt_type_t packet_type,
    avrcp_op_t continuation_tag,
    bool_t *continuation_required)
{
#ifdef BEKEN_DEBUG
    char strbuf[NOTIFY_BUFFER_SIZE];
    uint32_t len;

    /* During continuation we expect to receive AVRCP_PKT_CONT or AVRCP_PKT_END
     * packets with the same continuation tag received previously. Receiving of
     * any other continuation tag is considered by the application as an abort
     * from the target device */
    if(app_ptr->continuation_tag
        && app_ptr->continuation_tag != continuation_tag)
    {
        len = j_snprintf(strbuf, sizeof(strbuf),"Command in progress was "
            "aborted by the target device");

        frontend_notify(strbuf, len);
    }
#endif

    *continuation_required = FALSE;

    /* Update the global context's continuation_tag if we are in continuation or
     * zero it if not */
    switch (packet_type)
    {
    case AVRCP_PKT_START:
        app_ptr->continuation_tag = continuation_tag;
        *continuation_required = TRUE;
        break;
    case AVRCP_PKT_CONT:
        *continuation_required = TRUE;
        break;
    case AVRCP_PKT_END:
    case AVRCP_PKT_SINGLE:
        app_ptr->continuation_tag = 0;
        break;
    default:
        break;
    }

    /* Consider handling errors that can occur during continuation, such as
     * receiving AVRCP_PKT_CONT packet type before receiving AVRCP_PKT_START */
}
extern void a2dp_volume_init(int8_t aud_volume);
void avrcp_volume_sync(void)
{
#ifdef CONFIG_BLUETOOTH_AVRCP_TG
    uint8 volume;
    static uint8 vol_flag=0;
    app_env_handle_t  env_h = app_env_get_handle();
    if(g_avrcp_current_app && avrcp_current_support_vol_syn() && (!(g_avrcp_current_app->flag & APP_FLAG2_VOL_SYNC_OK) || (vol_flag==0)))
    {
        if (vol_flag == 0)
        {
            vol_flag = 1;
            a2dp_volume_init(env_h->env_cfg.system_para.vol_a2dp);	
            if(! app_wave_playing()
            #ifdef CONFIG_BLUETOOTH_COEXIST
                &&(app_is_bt_mode())
            #endif
            )
            {
                aud_volume_set(env_h->env_cfg.system_para.vol_a2dp);
            }
        }
        volume = get_tg_volume_value();
        os_printf("~~~avrcp_volume_sync:%x\r\n",volume);
        volume += 1;
        current_send_volume_change_response(volume); 
        volume -= 1;
        current_send_volume_change_response(volume);
        g_avrcp_current_app->flag |= APP_FLAG2_VOL_SYNC_OK;
    }
#endif
}

static void avrcp_getcaps_events_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid,
    avc_response_t response,
    avrcp_event_id_t *events,
    uint8_t count,
    avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx)
{
#if 1
     uint8_t i;
     for (i = 0; i < count; i++)
    {

       if(events[i]==AVRCP_EVENT_PLAYBACK_STATUS_CHANGED)
       {
           avrcp_cmd_register_notification(session,AVRCP_EVENT_PLAYBACK_STATUS_CHANGED,0);

       }
//       else if(events[i]==AVRCP_EVENT_PLAYBACK_POS_CHANGED)
//       {
//            avrcp_cmd_register_notification(AVRCP_EVENT_PLAYBACK_POS_CHANGED,0x01);
//       }
    }
#else
	#ifdef BEKEN_DEBUG
	    char str[NOTIFY_BUFFER_SIZE];
	    uint32_t len;
	    int_t i;
	#endif
	    bool_t dummy;       /* No need to handle continuation in this scenario */
	    avrcp_app_t *app_ptr;

	#ifdef BT_ONE_TO_MULTIPLE
	    app_ptr = avrcp_get_app_from_session(session);
	#else
	    app_ptr = &g_avrcp_app;
	#endif

	    avrcp_handle_continuation(app_ptr, packet_type, bt_avrcp_continuation_tag, &dummy);

	#ifdef BEKEN_DEBUG
	    len = j_snprintf(str, sizeof(str), "avrcp getcaps eventids: count %d\n",
	        count);

	    for (i = 0; i < count; i++)
	    {
	        len += j_snprintf(&str[len], sizeof(str) - len, "  id=%d (%s)\n",
	            events[i], avrcp_event_str(events[i]));
	    }

	    frontend_notify(str, len);
	#endif
#endif
}

extern result_t  select_a2dp_avrcp(btaddr_t *addr);
#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
extern void g_a2dp_stream_start_cb(void);
extern uint32_t a2dp_get_flag_on_raddr(const btaddr_t *raddr);
#endif

static void avrcp_event_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid,
    avc_response_t response,
    avrcp_event_rsp_t *ev,
    avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx)
{
#ifdef BEKEN_DEBUG
    //char str[NOTIFY_BUFFER_SIZE];
    //uint32_t len;
#endif
    bool_t dummy;       /* No need to handle continuation in this scenario */
    avrcp_app_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif

    avrcp_handle_continuation(app_ptr, packet_type, bt_avrcp_continuation_tag, &dummy);

#ifdef BEKEN_DEBUG
    //len = j_snprintf(str, sizeof(str), "  id=%d (%s)", ev->event_id,
    //    avrcp_event_str(ev->event_id));
#endif

    switch (ev->event_id)
    {
        case AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
        {
        	uint8_t status = ev->params.playback_status_changed.play_status;
            /* STOPPED / PLAYING / PAUSED / FWD_SEEK / REV_SEEK */
        #ifdef BEKEN_DEBUG
            //len += j_snprintf(&str[len], sizeof(str) - len,
            //    ",  playback status=%s\n", avrcp_play_status_str(status));
        #endif
		if(response==AVC_RESPONSE_CHANGED)
		{
			switch(status)
			{
				case AVRCP_PLAY_STATUS_PLAYING:
					os_printf("%s:---PLAYING---\r\n", backend_unit_get_name(&(app_ptr->raddr)));
				    #ifdef CONN_WITH_MUSIC
                                    #if 0
					bt_flag2_operate(APP_FLAG2_SW_MUTE,1);
					jtask_stop(app_common_get_task_handle());
					jtask_schedule( app_common_get_task_handle(), 800, (jthread_func)bt_flag2_operate_0,
								(void *)APP_FLAG2_SW_MUTE );
                                    #endif
				    #endif
					#if (CONFIG_A2DP_PLAYING_SET_AG_FLOW == 0)
						//select_a2dp_avrcp(&(app_ptr->raddr));
						select_other_avrcp_app_pause_by_raddr(&(app_ptr->raddr));
						#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
						if(!(a2dp_get_flag_on_raddr(&(app_ptr->raddr)) & APP_A2DP_FLAG_STREAM_START))
						{
                                             select_a2dp_avrcp(&(app_ptr->raddr));
						   g_a2dp_stream_start_cb();
						}
						a2dp_current_clear_flag(APP_A2DP_FLAG_STREAM_START);
						#endif
					#endif
                                #ifdef CONFIG_BLUETOOTH_COEXIST
                                    if(!app_is_bt_mode()||app_change_mode_flow_ctrl_en())
                                    {
                                        app_not_bt_set_a2dp_flag(1);
                                    }
                                #endif
					break;
				case AVRCP_PLAY_STATUS_PAUSED:
				case AVRCP_PLAY_STATUS_STOPPED:
                                   {
                                      os_printf("%s:---PAUSED||STOPPED---\r\n", backend_unit_get_name(&(app_ptr->raddr)));
                                    #if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1 && CONFIG_SELECT_PREV_A2DP_PLAY == 0)
                                        hci_link_t *tmp_link;
                                        tmp_link = select_flow_ctrl_link_handle_by_raddr(&(app_ptr->raddr));
                                        if(tmp_link)
                                        {
                                            tmp_link->flow_ctrl = 0;
                                            bt_set_ag_flow_ctrl_by_handle(tmp_link);
                                        }
                                    #endif
                                    #ifdef CONFIG_BLUETOOTH_COEXIST
                                        if(!app_is_bt_mode()||app_change_mode_flow_ctrl_en())
                                        {
                                            app_not_bt_set_a2dp_flag(0);
                                        }
                                    #endif
                                   }
					break;

				case AVRCP_PLAY_STATUS_FWD_SEEK:
					os_printf("%s:---FWD_SEEK---\r\n", backend_unit_get_name(&(app_ptr->raddr)));
					break;
				case AVRCP_PLAY_STATUS_REV_SEEK:
					os_printf("%s:---REV_SEEK---\r\n", backend_unit_get_name(&(app_ptr->raddr)));
					break;

			}
			if(app_ptr->flag & APP_FLAG_AVCRP_PROCESSING)  //for the bug: button_playpause_action dosen't work sometime
			{
                            avrc_last_tid_save_before_notif = app_ptr->last_tid;
                            //os_printf("===avrcp last tid:%d\r\n",avrc_last_tid_save_before_notif);
                            avrcp_clear_flag(app_ptr, APP_FLAG_AVCRP_PROCESSING);
			}
			avrcp_cmd_register_notification(session,AVRCP_EVENT_PLAYBACK_STATUS_CHANGED,0);
		}
		break;
        }

        case AVRCP_EVENT_TRACK_CHANGED:
        {
#if 0//def BEKEN_DEBUG
            /*j_uint64_t val64 = {0, 0};
            uint8_t *val = ev->params.track_changed.cur_track_index;

            val64.high = ua_get_be32(val);
            val64.low = ua_get_be32(val + 4);

            len += j_snprintf(&str[len], sizeof(str) - len,
                ",  current track index=0x%08lx:%08lx\n", val64.high,
                val64.low);*/
#endif
            break;
        }

        default:                        /* TODO: parse other events */
        #if 0//def BEKEN_DEBUG
            //len += j_snprintf(&str[len], sizeof(str) - len, "\n");
        #endif
            break;
    }

#ifdef BEKEN_DEBUG
    //frontend_notify(str, len);
#endif

    /* TODO: re-register to this event again */
}

static void avrcp_get_nowplaying_attr_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t rsp, uint32_t attr_id, uint16_t char_set_id,
    uint16_t attr_total_len, char *attr_val, uint16_t attr_val_len,
    avrcp_pkt_type_t packet_type, avrcp_op_t bt_avrcp_continuation_tag,
    bt_app_ctx_h app_ctx_h)
{
#ifdef BEKEN_DEBUG
    char strbuf[NOTIFY_BUFFER_SIZE];
    uint32_t len;
#endif
    bool_t continuation_required;
    avrcp_app_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif

    DECLARE_FNAME("avrcp_get_nowplaying_attr_rsp");

    avrcp_handle_continuation(app_ptr,
                                packet_type,
                                bt_avrcp_continuation_tag,
                                &continuation_required);

   /* Update the global context continuation parameters that are specific to
    * this command. */
    switch (packet_type)
    {
    case AVRCP_PKT_START:
        app_ptr->attr_id = attr_id;
        app_ptr->char_set_id = char_set_id;
        break;

    case AVRCP_PKT_CONT:
        attr_id = app_ptr->attr_id;
        char_set_id = app_ptr->char_set_id;
        break;

    case AVRCP_PKT_END:
        attr_id = app_ptr->attr_id;
        char_set_id = app_ptr->char_set_id;
        app_ptr->attr_id = 0;
        app_ptr->char_set_id = 0;
        break;

    default:
        break;
    }

#ifdef BEKEN_DEBUG
    len = j_snprintf(strbuf, sizeof(strbuf),"    attr_id %s, char_set_id %d, "
        "attr_len 0x%x attr_val%s: %.*s\n", avrcp_media_attr_id_str(attr_id),
        char_set_id, attr_val_len, continuation_required ? "[cont']" : "",
        attr_val_len, attr_val);

    frontend_notify(strbuf, len);
#endif

    if(continuation_required)
    {
        /* XXX: Spec doesn't define behaviour for continuation command Tid */
        app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

        /* Call for continuation */
        bt_avrcp_send_request_continuing_response_cmd(session, app_ptr->last_tid,
            bt_avrcp_continuation_tag);
    }
}

static void avrcp_get_play_status_rsp(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, avc_response_t rsp,
    avrcp_get_play_status_rsp_t *play_status, avrcp_pkt_type_t packet_type,
    avrcp_op_t bt_avrcp_continuation_tag, bt_app_ctx_h app_ctx)
{
#ifdef BEKEN_DEBUG
    char str[NOTIFY_BUFFER_SIZE];
    uint32_t len;
#endif
    bool_t dummy;       /* No need to handle continuation in this scenario */
    avrcp_app_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = avrcp_get_app_from_session(session);
#else
    app_ptr = &g_avrcp_app;
#endif
    avrcp_handle_continuation(app_ptr, packet_type, bt_avrcp_continuation_tag, &dummy);

#ifdef BEKEN_DEBUG
    len = j_snprintf(str, sizeof(str), "avrcp play status: \n  "
        "song length %ld sec\n  song position %ld sec\n  play status %s\n",
        ua_get_be32(play_status->song_length),
        ua_get_be32(play_status->song_position),
        avrcp_play_status_str(play_status->play_status));

    frontend_notify(str, len);
#endif
}

result_t avrcp_cmd_connect(char *params, unsigned int len)
{
    result_t err;
    avrcp_app_t *app_ptr;
    btaddr_t laddr, raddr;
    unsigned int i, unit_id, tmp[6];

    if(j_snscanf(params,
                    NULL,
                    "%u " BTADDR_FORMAT,
                    &unit_id,
                    &tmp[5],
                    &tmp[4],
                    &tmp[3],
                    &tmp[2],
                    &tmp[1],
                    &tmp[0]) != 7)
    {
        return UWE_PARAM;
    }

    for (i = 0; i < 6; i++)
    {
        raddr.b[i] = (uint8_t)tmp[i];
    }

    #ifndef BT_ONE_TO_MULTIPLE
    app_ptr = &g_avrcp_app;
    if(app_ptr->is_used)
    {
        return UWE_ALREADY;
    }
    #else
    app_ptr = avrcp_app_lookup_valid(&raddr);
    if((app_ptr) && (app_ptr->is_used))
    {
        return UWE_ALREADY;
    }

    if((0 == app_ptr)
        && (avrcp_app_get_count() < BT_MAX_AG_COUNT))
    {
        os_printf("avrcp_cmd_connect\r\n");
        app_ptr = avrcp_ex_app_alloc();
        if(app_ptr)
        {
            avrcp_ex_app_init(app_ptr);
            avrcp_app_register(app_ptr);
        }
    }
    #endif

    err = backend_unit_get_addr(unit_id, &laddr);
    if(err)
    {
        goto Exit;
    }

    app_ptr->svc_id++;

    err = bt_avrcp_ctrl_conn_connect(app_ptr->session, &laddr, &raddr);
    if(err)
    {
        goto Exit;
    }

    btaddr_copy(&app_ptr->raddr, &raddr);
    app_ptr->is_used = 1;

    //os_printf("avrcp_cmd_connect svc-id:%x\r\n", app_ptr->svc_id);

Exit:
    return err;
}

result_t avrcp_cmd_disconnect(void)
{
    bt_avrcp_ctrl_conn_disconnect(g_avrcp_current_app->session);

    return UWE_OK;
}

result_t avrcp_cmd_register_notification(bt_avrcp_ctrl_session_h session,avrcp_event_id_t event_id,
       uint32_t playback_pos_changed_interval)
{
    result_t err;
		avrcp_app_t *app_ptr;
	#ifdef BT_ONE_TO_MULTIPLE
		app_ptr = avrcp_get_app_from_session(session);
	#else
		app_ptr = &g_avrcp_app;
	#endif

    app_ptr->last_tid = (app_ptr->last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    err = bt_avrcp_send_register_notification_cmd(app_ptr->session, app_ptr->last_tid,
        event_id, playback_pos_changed_interval);
    if(err)
        goto Exit;

    //os_printf("avrcp-transaction-label:%x\r\n", app_ptr->last_tid);

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
    return err;
}

static result_t avrcp_cmd_accept(avrcp_app_t *app_ptr, char *params, unsigned int len)
{
    result_t err;
    uint32_t svc_id;

    if(j_snscanf(params, NULL, "%u", &svc_id) != 1)
        return UWE_INVAL;

    if(svc_id != app_ptr->svc_id)
        return UWE_PARAM;

    if(app_ptr->is_used)
        return UWE_ISCONN;

    if(!app_ptr->conn_req)
        return UWE_STATE;

    app_ptr->svc_id++;

    err = bt_avrcp_ctrl_conn_accept(app_ptr->session,
                                        app_ptr->conn_req,
                                        app_ptr->conn_req_mode);
    if(err)
        goto Exit;

    app_ptr->is_used = 1;

    //os_printf("avrcp-connecting, svc id:%x\r\n", app_ptr->svc_id);

Exit:
    app_ptr->conn_req = NULL;
    return err;
}

static void avrcp_set_flag(avrcp_app_t *app_ptr, uint32 flag)
{
    app_ptr->flag |= flag;
}

static void avrcp_clear_flag(avrcp_app_t *app_ptr, uint32 flag)
{
    app_ptr->flag &= ~flag;
}

uint32 avrcp_get_raddr(void *arg, btaddr_t *raddr)
{
    avrcp_app_t *app_ptr;

    app_ptr = (avrcp_app_t *)arg;
    if(0 == app_ptr)
    {
        return UWE_INVAL;
    }

    btaddr_copy(raddr, &(app_ptr->raddr));

    return UWE_OK;
}

void avrcp_select_current_app(void *arg)
{
    g_avrcp_current_app = (avrcp_app_t *)arg;
}

uint32 avrcp_select_current_app_to_another(const btaddr_t *raddr)
{
    uint32_t i;
    avrcp_app_t *app_ptr=NULL;
    uint32_t update_fail;

    update_fail = 1;
	
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (app_ptr->is_used)
            && (app_ptr->session)
            && (app_ptr->flag & APP_FLAG_AVCRP_CONNECTION)
            && (!btaddr_same(&app_ptr->raddr, raddr)))
        {
            g_avrcp_current_app = app_ptr;
			update_fail = 0;
            //os_printf("select_avrcp_current_app:%x\r\n", app_ptr);
            break;
        }
    }

    return update_fail;
}

uint32 avrcp_update_current_app(const btaddr_t *raddr)
{
    uint32_t i;
    btaddr_t *r_addr;
    avrcp_app_t *app_ptr=NULL;
    uint32_t update_fail;

    update_fail = 1;

    //os_printf("sraddr "BTADDR_FORMAT"\r\n", BTADDR(raddr));
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_avrcp_array[i];
        if((app_ptr)
            && (app_ptr->is_used)
            && (app_ptr->session))
        {
            r_addr = &app_ptr->raddr;
            //os_printf("r_addr "BTADDR_FORMAT"\r\n", BTADDR(r_addr));
            if(btaddr_same(raddr, r_addr))
            {
                g_avrcp_current_app = app_ptr;

                //os_printf("update:%x\r\n", app_ptr);
                update_fail = 0;
                break;
            }
        }
    }

    return update_fail;
}

void select_other_avrcp_app_pause_by_raddr(btaddr_t *raddr)
{
    uint32_t i;
    avrcp_app_t *app_ptr=NULL;
    bool_t   avrcp_find = FALSE;
    //os_printf("avrcp===select other raddr: "BTADDR_FORMAT"\r\n", BTADDR(raddr));
    app_ptr = avrcp_get_app_ptr_by_raddr(raddr);
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if( (g_avrcp_array[i])
            && (g_avrcp_array[i]->is_used)
            && (g_avrcp_array[i]->session)
            && (app_ptr != g_avrcp_array[i]
            && get_other_a2dp_play_flag(raddr))
        )
        {
            avrcp_find = TRUE;
            app_ptr = g_avrcp_array[i];
            break;
        }
    }
    if(avrcp_find)
    {
    	 //os_printf("===other avrcp send pause\r\n");
        avrcp_send_opcode(app_ptr,(void *)AVC_OP_PAUSE);
    }
}

void clr_avrcp_flags(btaddr_t *btaddr)
{
    int i;

    for(i=0;i<BT_MAX_AG_COUNT;i++)
    {
        if(g_avrcp_array[i] && btaddr_same(btaddr, &(g_avrcp_array[i]->raddr)))
            avrcp_clear_flag(g_avrcp_array[i], APP_FLAG_AVCRP_CONNECTION|APP_FLAG_AVCRP_PROCESSING|APP_FLAG2_VOL_SYNC|APP_FLAG2_VOL_SYNC_OK);
    }
}
btaddr_t *avrcp_get_raddr_from_session(bt_avrcp_ctrl_session_h session)
{
    return &avrcp_get_app_from_session(session)->raddr;
}

#ifdef CONFIG_BLUETOOTH_AVRCP_TG
bool_t avrcp_update_vol_tid_based_on_session(bt_avrcp_ctrl_session_h session, uint8_t volume_tid)
{
    avrcp_app_t *app_ptr =NULL;

    app_ptr = avrcp_get_app_from_session(session);

    if(app_ptr) 
    {
        app_ptr->volume_tid = volume_tid ; 
		return TRUE;
    }

    //os_printf("Error:avrcp_update_vol_tid_based_on_session failed!!\r\n");
    return FALSE;
}

uint32 avrcp_current_support_vol_syn(void)
{
    return (g_avrcp_current_app->flag & APP_FLAG2_VOL_SYNC);
}

bool_t avrcp_set_vol_syn_flag_based_on_session(bt_avrcp_ctrl_session_h session)
{
    avrcp_app_t *app_ptr =NULL;

    app_ptr = avrcp_get_app_from_session(session);

    if(app_ptr) 
    {
        avrcp_set_flag(app_ptr,APP_FLAG2_VOL_SYNC); 
		return TRUE;
    }

    //os_printf("Error:avrcp_set_vol_syn_flag_based_on_session failed!!\r\n");
	return FALSE;
}

bool_t avrcp_set_vol_syn_ok_flag_based_on_session(bt_avrcp_ctrl_session_h session)
{
    avrcp_app_t *app_ptr =NULL;

    app_ptr = avrcp_get_app_from_session(session);

    if(app_ptr) 
    {
        avrcp_set_flag(app_ptr,APP_FLAG2_VOL_SYNC|APP_FLAG2_VOL_SYNC_OK); 
	return TRUE;
    }

    //os_printf("Error:avrcp_set_vol_syn_flag_based_on_session failed!!\r\n");
	return FALSE;
}

extern result_t avrcp_send_vendor_dependent_volume_change_response(bt_avrcp_ctrl_session_h session, uint8_t tid, uint8_t volume_value);
void current_send_volume_change_response(uint8_t volume_value)
{
    result_t err;

    err = avrcp_send_vendor_dependent_volume_change_response(g_avrcp_current_app->session,
		                                                     g_avrcp_current_app->volume_tid,
		                                                     volume_value);
	
    if(err)
       os_printf("ERROR:current_send_volume_change_response_failed!!\r\n");
}
#else
bool_t avrcp_update_vol_tid_based_on_session(bt_avrcp_ctrl_session_h session, uint8_t volume_tid)
{
	return FALSE;
}
bool_t avrcp_set_vol_syn_flag_based_on_session(bt_avrcp_ctrl_session_h session)
{
    return FALSE;
}
bool_t avrcp_set_vol_syn_ok_flag_based_on_session(bt_avrcp_ctrl_session_h session)
{
    return FALSE;
}
#endif

#ifdef CONFIG_TWS
#ifdef DEBUG_BEKEN
static void avrcp_notify_num(char *desc, uint32_t num)
{
    char str[NOTIFY_BUFFER_SIZE];
    uint32_t len;

    len = j_snprintf(str, sizeof(str), "avrcp %s: %lu", desc, num);

    frontend_notify(str, len);
}
#endif


static void send_cmd_test_release( void * arg )
{
#ifdef DEBUG_BEKEN
    result_t err;
#endif
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;

    g_avrcp_app.last_tid = (g_avrcp_app.last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

#ifdef DEBUG_BEKEN
    err = bt_avrcp_send_pass_through_cmd(g_avrcp_app.session, g_avrcp_app.last_tid, avc_op,
        AVC_RELEASE);
    if (err)
        avrcp_notify_num("release failed", avc_op);
#else
	bt_avrcp_send_pass_through_cmd(g_avrcp_app.session, g_avrcp_app.last_tid, avc_op,
	AVC_RELEASE);
#endif
}

void send_cmd_test(void *arg)
{
	#ifdef DEBUG_BEKEN
    result_t err;
	#endif
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;
    app_env_handle_t env_h = app_env_get_handle();
    int duration = 200;

    if (!g_avrcp_app.is_used)
        return;


    g_avrcp_app.last_tid = (g_avrcp_app.last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    g_avrcp_app.last_cmd = avc_op;

#ifdef DEBUG_BEKEN
    err = bt_avrcp_send_pass_through_cmd(g_avrcp_app.session, g_avrcp_app.last_tid, avc_op,
        AVC_PRESS);
    if (err)
        avrcp_notify_num("press failed", avc_op);
#else
	bt_avrcp_send_pass_through_cmd(g_avrcp_app.session, g_avrcp_app.last_tid, avc_op,
	AVC_PRESS);
#endif

    duration = env_h->env_cfg.button_para.repeat - 10;

    if( avc_op == AVC_OP_REWIND || avc_op == AVC_OP_FAST_FORWARD )
        app_bt_shedule_task(send_cmd_test_release, arg, duration);
    else
        send_cmd_test_release( arg );

    bt_flag1_operate( APP_FLAG_AVCRP_PROCESSING, 1);

    //jtask_schedule(g_avrcp_app.task, 20000, send_cmd_test, (void *)((uint32_t)next));
}




result_t avrcp_cmd_ct_connect(char *params, unsigned int len)
{
    result_t err;
    btaddr_t laddr, raddr;
    unsigned int i, unit_id, tmp[6];

    if (g_app_ct.is_used)
        return UWE_ALREADY;

    if (j_snscanf(params, NULL, "%u " BTADDR_FORMAT, &unit_id, &tmp[5], &tmp[4
],
        &tmp[3], &tmp[2], &tmp[1], &tmp[0]) != 7)
    {
        return UWE_PARAM;
    }

    for (i = 0; i < 6; i++)
        raddr.b[i] = (uint8_t)tmp[i];

    err = backend_unit_get_addr(unit_id, &laddr);
    if (err)
        return err;

    g_app_ct.svc_id++;

    err = bt_avrcp_ctrl_conn_connect(g_app_ct.session, &laddr, &raddr);
    if (err)
        goto Exit;

    g_app_ct.is_used = 1;

    os_printf(APP_RET_STATUS_CONNECTING, "AVRCP");

#ifdef DEBUG_BEKEN
    avrcp_notify_num("connecting, svc id", g_app_ct.svc_id);
#endif

Exit:
    return err;
}

result_t avrcp_cmd_ct_disconnect(void)
{
    bt_avrcp_ctrl_conn_disconnect(g_app_ct.session);

    return UWE_OK;
}


static void avrcp_ct_connected(bt_avrcp_ctrl_session_h session,
    bt_app_ctx_h app_ctx)
{
#ifdef DEBUG_BEKEN
    avrcp_notify_num("ct connected, svc id", g_app_ct.svc_id);
#endif
}

static void avrcp_ct_disconnected(bt_avrcp_ctrl_session_h session,
    result_t status, bt_app_ctx_h app_ctx)
{
#ifdef DEBUG_BEKEN
    avrcp_notify_num("ct disconnected, svc id", g_app_ct.svc_id);
#endif
    g_app_ct.is_used = 0;
    jtask_stop(g_app_ct.task);

}

extern void app_powerdown(void);
void avrcp_get_passthrough_req(bt_avrcp_ctrl_session_h session,
    avrcp_tid_t tid, uint8_t op_id, void *payload, uint8_t len )
{
//    uint16_t *seq;

    //os_printf("avrcp cmd recv tid: %d, op_id: %x\r\n", tid, op_id);

    switch(op_id)
    {
        case AVC_OP_PLAY:
        case AVC_OP_PAUSE:
        case AVC_OP_NEXT:
        case AVC_OP_PREV:
        case AVC_OP_REWIND:
        case AVC_OP_FAST_FORWARD:
            if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) &&
                (op_id & 0x80) == 0 ) // we only process "press"
                send_cmd_test((void *)(op_id&0x7F));
            break;
        case AVC_OP_VOLUME_DOWN:
            if( bt_flag1_is_set( APP_FLAG_A2DP_CONNECTION ) )
                a2dp_volume_adjust(0);
            break;
        case AVC_OP_VOLUME_UP:
            if( bt_flag1_is_set( APP_FLAG_A2DP_CONNECTION ) )
                a2dp_volume_adjust(1);
            break;
        case AVC_OP_MATCH:
            os_printf("Match message\r\n");
            bt_flag2_operate( APP_FLAG2_STEREO_MATCH_BUTTON, 1 );
            app_bt_auconn_stop();
            break;
		case AVC_OP_STOP:
			start_wave_and_action(APP_WAVE_FILE_ID_POWEROFF, app_powerdown);
			break;
        default:
            break;
    }
}

void send_ct_cmd_test(void *arg)
{
//    result_t err;
    uint32_t tmp = (uint32_t)arg;
    avc_op_t avc_op = (avc_op_t)tmp;
//    app_env_handle_t env_h = app_env_get_handle();
//    int duration = 200;

    if (!g_app_ct.is_used)
        return;

    if( (avc_op != AVC_OP_VOLUME_DOWN)
		&& (avc_op != AVC_OP_VOLUME_UP)
		&& (avc_op != AVC_OP_STOP))
        return;

    g_app_ct.last_tid = (g_app_ct.last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    bt_avrcp_send_pass_through_cmd(g_app_ct.session, g_app_ct.last_tid, avc_op,
        AVC_PRESS);

    g_app_ct.last_tid = (g_app_ct.last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

    bt_avrcp_send_pass_through_cmd(g_app_ct.session, g_app_ct.last_tid, avc_op,
        AVC_RELEASE);

}

void send_ct_cmd_test1(void *arg,uint8 len)
{
//	sync_data_TypeDef *sync_data_tmp;
//	sync_data_tmp = (sync_data_TypeDef *)arg;
//	os_printf("tx:%x,%x\r\n",sync_data_tmp->u.sync_send.bt_clk,sync_data_tmp->u.sync_send.aud_num);

//	os_printf("tx:%4x,%4x,%d\r\n",*((uint32_t *)arg),*((uint32_t *)arg+1),len);
    if (!g_app_ct.is_used)
        return;

    g_app_ct.last_tid = (g_app_ct.last_tid + 1) % AVRCP_MAX_TRANSACTIONS;

//    avrcp_send_vendor_cmd(g_app_ct.session, g_app_ct.last_tid,AVC_CTYPE_CONTROL,
//        AVRCP_OP_GetCapabilities, arg, len);
	avrcp_send_bk_data(g_app_ct.session,g_app_ct.last_tid,arg, len);
}


#endif

// EOF
