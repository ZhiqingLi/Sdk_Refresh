/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bluetooth.h>
#include <bt_api.h>
#include <bt_spp.h>
#include <bt_sdp.h>
#include "bt_app_internal.h"
#include "bt_rfcomm_channel.h"
#include "app_beken_includes.h"
#include "lp_pdd_main.h" // add by cai.zhong 20190220 for smartvoice
#include "hal_print.h"  // add by cai.zhong 20190220 for smartvoice debug

/* SPP Service-level Notifications */
static void spp_connected(bt_spp_session_h session, bt_app_ctx_h app_ctx);
static void spp_disconnected(bt_spp_session_h session, result_t status,
    bt_app_ctx_h app_ctx);

static bt_spp_conn_cbs_t spp_conn_cbs = {
    spp_connected,
    spp_disconnected
};

static void spp_newconn(bt_conn_req_h conn_req, const btaddr_t *laddr,
    const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_server_ctx, uint8_t channel);

void spp_result_input(bt_spp_session_h session, char *result,uint16_t length);

//void spp_result_input(bt_spp_session_h session, char *result,
//    bt_app_ctx_h app_ctx);

result_t spp_send( char *buff, uint8_t len );


static const bt_spp_cbs_t spp_cbs = {
    spp_result_input
};

typedef struct {
    uint32_t                    svc_id;
    uint32_t                    server_sdp;
    bt_spp_server_h             server;
    bt_spp_session_h            session;

    btaddr_t                    laddr;
    btaddr_t                    raddr;
    bt_conn_req_h               conn_req;
    bt_link_modes_mask_t        conn_req_mode;

    bool_t                      is_used;
} spp_app_t;

/* XXX: Currently, only single service is supported by this sample */
static spp_app_t g_spp_app;
#ifdef BT_DUALMODE
extern void LEadv_Advertise_Disable_Enable(unsigned char enable);
#endif
__INLINE__ unsigned char spp_is_connected(void)
{
    spp_app_t *app = &g_spp_app;
    return app->is_used;
}

static void spp_connected(bt_spp_session_h session, bt_app_ctx_h app_ctx)
{
    spp_app_t *app = &g_spp_app;
    
    os_printf("spp connected: %lu\r\n", app->svc_id);
    lp_pdd_connected(BLUETOOTH_SPP_CONNECT_EVENT); // add by cai.zhong 20190220 for smartvoice 
#ifdef BT_DUALMODE
    LEadv_Advertise_Disable_Enable(0);
#endif

#ifdef BEKEN_SPP_OTA_3266
    if(flag_ota_enable)
    {
        app_handle_t sys_hdl = app_get_sys_handler();
        bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
    }
#endif
}

static void spp_disconnected(bt_spp_session_h session, result_t err,
    bt_app_ctx_h app_ctx)
{
    spp_app_t *app = &g_spp_app;

    os_printf("spp disconnected: %lu\r\n", app->svc_id);
    lp_pdd_disconnected(BLUETOOTH_SPP_CONNECT_EVENT); // add by cai.zhong 20190220 for smartvoice 

    app->svc_id++;
    app->is_used = 0;
#ifdef BT_DUALMODE
    LEadv_Advertise_Disable_Enable(1);
#endif
}
#ifdef BT_DUALMODE
extern u_int8 LEconnection_LE_Connected(void);
#endif
static void spp_newconn(bt_conn_req_h conn_req, const btaddr_t *laddr,
    const btaddr_t *raddr, bt_link_modes_mask_t mode,
    bt_app_ctx_h app_server_ctx, uint8_t channel)
{
    spp_app_t *app = &g_spp_app;

    DECLARE_FNAME("spp_newconn");

    if(app->is_used || app->conn_req
    #ifdef BT_DUALMODE
        ||(LEconnection_LE_Connected())
    #endif
    #if 0//def BEKEN_SPP_OTA_3266
        ||(0 == flag_ota_enable)
    #endif
        )
    {
        DBG_E(DBT_HFP, ("%s: already connected - rejecting\n", FNAME));
        bt_spp_conn_reject(conn_req);
        return;
    }

    j_memcpy((void *)&app->laddr, (void *)laddr, sizeof(btaddr_t));
    j_memcpy((void *)&app->raddr, (void *)raddr, sizeof(btaddr_t));

    bt_frontend_notification("spp newconn %lu", app->svc_id);

    if(bt_spp_conn_accept(app->session, conn_req, mode))
    {
        bt_frontend_notification("spp connect %lu failed", app->svc_id);
        return;
    }

    ////app_set_led_event_action(LED_EVENT_HFP_CONNETCTING); 
    app->is_used = 1;
    bt_frontend_notification("spp connecting %lu", app->svc_id);

}

#ifdef BEKEN_SPP_OTA_3266
extern void oad_spp_pdu_reframe(uint8 *pValue, uint16_t length);
#endif
extern void uart_send (unsigned char *buff, unsigned int len) ;
void spp_result_input(bt_spp_session_h session, char *result,uint16_t length)
{
   // change begin by cai.zhong 20190220 for smartvoice
    #if 0
    os_printf("Receive data:%d\r\n",length);
    DUMP8("0x%02x ", result, length);
    #endif
    lp_pdd_receive_cmd_data(result,length);
    // change begin by cai.zhong 20190220 for smartvoice
}

static enum {
    UNINIT = 0,
    SPP_REGISTER,
    SERVER_START,
    BACKEND_REGISTER,
    CONN_REGISTER
} spp_sample_init_stage = UNINIT;

void spp_backend_uninit(void);
result_t spp_backend_init(void)
{
    result_t err;
    
    os_printf("spp_backend_init\r\n");

    j_memset(&g_spp_app, 0, sizeof(g_spp_app));
    
    err = bt_spp_register(&spp_cbs);
    if(err)
        goto Exit;
    spp_sample_init_stage = SPP_REGISTER;

    err = bt_spp_server_start(&g_spp_app.server, BTADDR_ANY,
        RFCOMM_CHANNEL_SPP, spp_newconn, NULL);
    if(err)
        goto Exit;
    spp_sample_init_stage = SERVER_START;

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    err = backend_section_register(&hf_section);
    if(err)
        goto Exit;
    spp_sample_init_stage = BACKEND_REGISTER;
#endif

    err = bt_spp_conn_create(&g_spp_app.session, &spp_conn_cbs,
        NULL);
    if(err)
        goto Exit;
    spp_sample_init_stage = CONN_REGISTER;

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s, stage: %d\n", FNAME, uwe_str(err),
        spp_sample_init_stage));

    if(err)
        spp_backend_uninit();
    
    return err;
}

void spp_backend_uninit(void)
{
    if(spp_sample_init_stage == CONN_REGISTER)
    {
        if(g_spp_app.conn_req)
            (void)bt_spp_conn_reject(g_spp_app.conn_req);
        
        bt_spp_conn_destroy(&g_spp_app.session);
        spp_sample_init_stage = BACKEND_REGISTER;
    }

#ifdef CONFIG_BLUETOOTH_COMM_INTERFACE
    if(spp_sample_init_stage == BACKEND_REGISTER)
    {
        backend_section_unregister(&hf_section);
        spp_sample_init_stage = SERVER_START;
    }
#endif

    if(spp_sample_init_stage == SERVER_START)
    {
        bt_spp_server_stop(&g_spp_app.server);
        spp_sample_init_stage = SPP_REGISTER;
    }

    if(spp_sample_init_stage == SPP_REGISTER)
    {
        bt_spp_unregister();
        spp_sample_init_stage = UNINIT;
    }
}

result_t spp_connect_rfcomm(btaddr_t raddr, uint8_t channel)
{
	result_t err;
	btaddr_t laddr;
	uint32_t unit_id = 0;
	spp_app_t *app = &g_spp_app;
	
	err = backend_unit_get_addr(unit_id, &laddr);
	if(err)
	{
		err = UWE_NODEV;
		goto Exit;
	}
	
	app->laddr = laddr;
	app->raddr = raddr;
	err = bt_spp_conn_connect(app->session, &laddr, &raddr, channel);
	if(err)
		goto Exit;
	
	bt_frontend_notification("spp connecting %lu", app->svc_id);
	
Exit:
	DBG_RC_I(err, DBT_APP, ("%s: Done (%s)\n", FNAME, uwe_str(err)));
	return err;
}

result_t spp_send(char *buff, uint8_t len)
{
    spp_app_t *app = &g_spp_app;


    return bt_spp_conn_send(app->session, buff, len);
}
