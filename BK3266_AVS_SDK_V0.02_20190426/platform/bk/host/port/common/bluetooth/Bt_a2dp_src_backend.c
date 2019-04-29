/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bt_api.h>
#include <bt_a2dp_src.h>
#include <audio_out_interface.h>
#include "bt_app_internal.h"
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_TWS
/* A2DP Notifications */
result_t a2dp_stream_start(bt_a2dp_src_session_h  session);
result_t a2dp_stream_suspend(bt_a2dp_src_session_h session);
result_t a2dp_stream_send( bt_a2dp_src_session_h session, struct mbuf *m );

static void a2dp_connected(bt_a2dp_src_session_h session_h,
    bt_app_ctx_h app_ctx);
static void a2dp_disconnected(bt_a2dp_src_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx);

static const bt_a2dp_src_conn_cbs_t conn_cbs = {
    a2dp_connected,
    a2dp_disconnected,
};


typedef struct {
    uint32_t               svc_id;
    bt_a2dp_src_session_h svc;

    bool_t                 is_used;
    bool_t                 auto_on;

    bt_link_modes_mask_t   conn_mode;
} a2dp_backend_t;

static a2dp_backend_t g_a2dp;

void app_bt_stereo_profile_conn_wrap( void );
void app_bt_stereo_profile_disconn_wrap( void );


static void a2dp_connected(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx)
{
    bt_frontend_notification("a2dp src connected %lu", g_a2dp.svc_id);

    app_bt_stereo_profile_conn_wrap();
}

static void a2dp_disconnected(bt_a2dp_sink_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx)
{
    g_a2dp.is_used = 0;
    bt_frontend_notification("a2dp src disconnected %lu, status: %x", g_a2dp.svc_id, status);

    app_bt_stereo_profile_disconn_wrap();
}


result_t a2dp_src_backend_init(void)
{
    result_t err;
    DECLARE_FNAME("a2dp_backend_init");

    j_memset(&g_a2dp, 0, sizeof(g_a2dp));

    err = bt_a2dp_src_register();
    if (err)
        goto Exit;


    err = bt_a2dp_src_conn_create(&g_a2dp.svc, &conn_cbs, NULL);

Exit:
    if (err)
        a2dp_src_backend_uninit();

    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
    return err;
}

void a2dp_src_backend_uninit(void)
{
    if (g_a2dp.svc)
        bt_a2dp_src_conn_destroy(&g_a2dp.svc);

    bt_a2dp_src_unregister();
}

result_t a2dp_src_cmd_connect(char *params, unsigned int len)
{
    result_t err;
    btaddr_t laddr, raddr;
    int btaddr_tmp[6];
    uint32_t unit_id;
    bt_a2dp_endpoint_desc_t ep;
    DECLARE_FNAME("a2dp_cmd_connect");

    if (g_a2dp.is_used)
        return UWE_ALREADY;

    if (j_snscanf(params, NULL, "%u " BTADDR_FORMAT, &unit_id,
        BTADDR_TMP(btaddr_tmp)) != UW_ARRAY_SIZE(btaddr_tmp) + 1)
    {
        return UWE_INVAL;
    }

    TMP_TO_BTADDR(btaddr_tmp, &raddr);

    err = backend_unit_get_addr(unit_id, &laddr);
    if (err)
        goto Exit;

    ep.codecs_count = 1;
    ep.codecs[0].type = A2DP_CODEC_SBC;
    bt_a2dp_sink_get_default_sbc(&ep.codecs[0].u.sbc);

    err = bt_a2dp_src_conn_connect(g_a2dp.svc, &laddr, &raddr, &ep);
    if (err)
        goto Exit;

    g_a2dp.is_used = 1;
    bt_frontend_notification("a2dp connecting %lu", g_a2dp.svc_id);
    os_printf(APP_RET_STATUS_CONNECTING, "A2DP");

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
    return err;
}

result_t a2dp_src_cmd_disconnect(void)
{
    result_t err;

    if (!g_a2dp.svc)
        return UWE_NODEV;

    err = bt_a2dp_src_conn_disconnect(g_a2dp.svc);

    return err;
}



result_t a2dp_src_cmd_stream_start( void )
{
    result_t err;

    if (!g_a2dp.svc)
        return UWE_NODEV;

    err = a2dp_stream_start( g_a2dp.svc );

    return err;
}


result_t a2dp_src_cmd_stream_suspend( void )
{
    result_t err;

    if (!g_a2dp.svc)
        return UWE_NODEV;

    err = a2dp_stream_suspend( g_a2dp.svc );

    return err;
}

result_t a2dp_src_cmd_stream_send( struct mbuf *m )
{
//    if( sbc_src_num() )
//    {
//        sbc_src_num_oper( 1, 1 );
        return a2dp_stream_send( g_a2dp.svc, m );
//    }
//    else
//    {
//        m_freem(m);
//        return 0;
//    }
}

result_t a2dp_src_cmd_reconfig( bt_a2dp_codec_t *codec )
{
	if(g_a2dp.is_used)
	    return bt_a2dp_src_conn_reconfig(g_a2dp.svc, codec);
	else
		return UWE_OK;
}
#endif

