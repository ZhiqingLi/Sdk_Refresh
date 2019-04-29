/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include <bt_api.h>
#include <bt_sdp.h>
#include <bt_sdp_types.h>
#include <bt_a2dp_sink.h>
#include <audio_out_interface.h>
#include "bt_app_internal.h"
#include "driver_beken_includes.h"
#include "driver_audio.h"
#include "beken_external.h"
#include "app_beken_includes.h"
#ifdef CONFIG_TWS

/* A2DP Notifications */
typedef struct {
    uint32_t               svc_id;
    bt_a2dp_sink_session_h svc;
    bt_a2dp_sink_session_h srv;
    uint32_t               sdp;
    audio_out_ctx_h        audio_out;

    uint32_t 		       freq;
    uint32_t		       channel;

    uint32_t               flag;
#ifdef CONFIG_TWS
    bt_a2dp_codec_t    codec;
#endif


    int8_t                 volume;

    int8_t                 is_used;
    int8_t                 auto_on;

    bt_link_modes_mask_t   conn_mode;
    bt_conn_req_h          conn_req;

    btaddr_t               raddr;
} a2dp_backend_t;
static void a2dp_clear_flag(a2dp_backend_t *app_ptr, uint32 flag);
#else
void a2dp_clear_flag(a2dp_backend_t *app_ptr, uint32 flag);
#endif
result_t a2dp_peer_addr(bt_a2dp_sink_session_h session, btaddr_t *raddr);
static void a2dp_connected(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx);
static void a2dp_disconnected(bt_a2dp_sink_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx);
static void a2dp_newconn(bt_conn_req_h conn_req, const btaddr_t *laddr,
    const btaddr_t *raddr, bt_link_modes_mask_t mode, bt_app_ctx_h app_ctx);

CONST static bt_a2dp_sink_conn_cbs_t conn_cbs = {
    a2dp_connected,
    a2dp_disconnected,
};

#ifdef CONFIG_TWS

static void a2dp_reconfiguration_cb(bt_a2dp_sink_session_h session_h,
    const bt_a2dp_codec_t *codec, int_t tid, int_t remote_ep_id,
    bt_app_ctx_h app_ctx_h);
	#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
	hci_link_t *select_curr_playing_link_handle_by_a2dp(a2dp_backend_t *app_ptr);
	#endif
#endif
static void a2dp_set_configuration_cb(bt_a2dp_sink_session_h session_h,
    const bt_a2dp_codec_t *codec, int_t local_ep_id, int_t remote_ep_id,
    bt_app_ctx_h app_ctx_h);
static void a2dp_stream_start_cb(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx);
static void a2dp_stream_suspend_cb(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx);
static void a2dp_stream_input_cb(bt_a2dp_sink_session_h session_h, void *buf,
    uint32_t len, bt_app_ctx_h app_ctx_h);
result_t a2dp_cmd_connect(char *params, unsigned int len);
result_t a2dp_cmd_disconnect(void);
void a2dp_volume_init(int8_t aud_volume);
void a2dp_codec_dump(const bt_a2dp_codec_t *codec);
#ifdef CONFIG_TWS
static void a2dp_set_flag(a2dp_backend_t *app_ptr, uint32 flag);
#else
void a2dp_set_flag(a2dp_backend_t *app_ptr, uint32 flag);
#endif
uint32_t a2dp_has_music(void);
uint32 a2dp_get_remote_addr(void *arg, btaddr_t *r_addr);
btaddr_t a2dp_get_addr_from_ptr(void *arg);
uint32_t a2dp_app_get_count(void);
uint32_t a2dp_all_is_not_music(void);

#ifdef CONFIG_BLUETOOTH_AVDTP_SCMS_T
void init_security_control_r_addr(btaddr_t * input_r_addr);
void uinit_security_control_r_addr(btaddr_t * input_r_addr);
#endif

#if UPGRADED_VERSION 
extern void set_delay_report_ms(uint16_t val_ms);
extern inline boolean isAvdtpSrcSupportDR(void);
#endif

static sdp_a2dp_profile_t a2dp_profile = {
    BT_SVC_TYPE_AUDIO_SINK,             /* Sink or Source */
    SDP_A2DP_SPEAKER|SDP_A2DP_HEADPHONE,/* Supported features */
    "Aud-Snk"                        /* Service name */
};

static const bt_a2dp_sink_cbs_t a2dp_cbs = {
    a2dp_set_configuration_cb,
#ifdef CONFIG_TWS
    a2dp_reconfiguration_cb,
#endif
    a2dp_stream_start_cb,
    a2dp_stream_suspend_cb,
    a2dp_stream_input_cb
};

static a2dp_backend_t g_a2dp;
static a2dp_backend_t *g_current_a2dp_ptr;
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
static a2dp_backend_t *g_a2dp_array[2] = {0};
#else
static a2dp_backend_t *g_a2dp_array[BT_MAX_AG_COUNT] = {0};
#endif

#ifdef BT_ONE_TO_MULTIPLE
a2dp_backend_t *a2dp_get_app_from_svc(bt_a2dp_sink_session_h svc)
{
    uint32_t i;
    a2dp_backend_t *app_ptr=NULL;

    for(i = 0; i<BT_MAX_AG_COUNT; i++)
    {
        if(g_a2dp_array[i] && (svc == g_a2dp_array[i]->svc))
        {
            app_ptr = g_a2dp_array[i];
            break;
        }
    }

#if 0//def BEKEN_DEBUG
    if (!app_ptr)
        os_printf("a2dp_get_app_from_svc is NULL\r\n");
#endif

    return app_ptr;
}
#endif

uint32_t a2dp_is_connection(void *arg)
{
    a2dp_backend_t *app_ptr;

    app_ptr = (a2dp_backend_t *)arg;

    if((app_ptr)
        && (app_ptr->flag & APP_FLAG_A2DP_CONNECTION))
    {
        return 1;
    }

    return 0;
}

#ifdef BT_ONE_TO_MULTIPLE
static uint32_t g_a2dp_count = 0;
uint32_t a2dp_all_apps_is_unused(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && (1 == g_a2dp_array[i]->is_used))
        {
            return 0;
        }
    }

    return 1;
}

uint32_t a2dp_app_register(a2dp_backend_t *ptr)
{
    uint32_t i;

    if(g_a2dp_count >= BT_MAX_AG_COUNT)
    {
        return UWE_FULL;
    }

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((a2dp_backend_t *)0 == g_a2dp_array[i])
        {
            g_a2dp_array[i] = ptr;
            g_a2dp_count ++;

            //os_printf("a2dp_app_register:%x\r\n", ptr);

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

uint32_t a2dp_app_unregister(a2dp_backend_t *ptr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(ptr == g_a2dp_array[i])
        {
            g_a2dp_array[i] = 0;
            g_a2dp_count --;

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

a2dp_backend_t *a2dp_app_lookup_valid(const btaddr_t *raddr)
{
    uint32_t i;
    a2dp_backend_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if((app_ptr)
            && (btaddr_same(raddr, &app_ptr->raddr)))
        {
            return app_ptr;
        }
    }

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if((app_ptr)
            && (!app_ptr->is_used))
        {
            break;
        }
    }

    return app_ptr;
}

a2dp_backend_t *a2dp_ex_app_alloc(void)
{
    a2dp_backend_t *app_ptr;

    app_ptr = (a2dp_backend_t *)jmalloc(sizeof(a2dp_backend_t), M_ZERO);

    return app_ptr;
}

void a2dp_ex_app_free(a2dp_backend_t *app_ptr)
{
    jfree(app_ptr);
    app_ptr = NULL;
}

uint32_t a2dp_ex_app_init(a2dp_backend_t *app_ptr)
{
    result_t err;

//    os_printf("a2dp_ex_app_init\r\n");
    err = bt_a2dp_sink_conn_create(&app_ptr->svc,
                                    &conn_cbs,
                                    NULL);
    app_ptr->auto_on = 1;
    app_ptr->srv = g_a2dp.srv;

    return err;
}

uint32_t a2dp_ex_app_uninit(a2dp_backend_t *app_ptr)
{
    bt_a2dp_sink_conn_destroy(&app_ptr->svc);

    return UWE_OK;
}

uint32_t a2dp_app_get_count(void)
{
    return g_a2dp_count;
}

uint32_t a2dp_all_is_not_music(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && (g_a2dp_array[i]->flag & APP_FLAG_MUSIC_PLAY))
        {
            return 0;
        }
    }

    return 1;
}

uint32_t a2dp_has_music(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && (g_a2dp_array[i]->flag & APP_FLAG_MUSIC_PLAY))
        {
            return 1;
        }
    }

    return 0;
}


uint32_t a2dp_has_the_music(btaddr_t addr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && (btaddr_same(&g_a2dp_array[i]->raddr,&addr))
            && (g_a2dp_array[i]->flag & APP_FLAG_MUSIC_PLAY) )
        {
            return 1;
        }
    }

    return 0;
}

uint32_t a2dp_has_the_connection(btaddr_t addr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && btaddr_same(&g_a2dp_array[i]->raddr,&addr)
            && (g_a2dp_array[i]->flag & APP_FLAG_A2DP_CONNECTION) )
        {
            return 1;
        }
    }

    return 0;
}

uint32_t a2dp_has_connection(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_a2dp_array[i])
            && (g_a2dp_array[i]->flag & APP_FLAG_A2DP_CONNECTION))
        {
            return 1;
        }
    }

    return 0;
}

void *a2dp_get_svc_based_on_raddr(const btaddr_t *raddr)
{
    uint32_t i;
    void *svc=NULL;
    result_t ret;
    btaddr_t r_addr;
    a2dp_backend_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if((app_ptr)
            && (app_ptr->svc))
        {
            ret = a2dp_peer_addr(app_ptr->svc, &r_addr);
            if((UWE_OK == ret)
                && (btaddr_same(raddr, &r_addr)))
            {
                svc = app_ptr ->svc;
                break;
            }
        }
    }

    return svc;
}

uint32_t a2dp_get_flag_on_raddr(const btaddr_t *raddr)
{
    a2dp_backend_t *app_ptr=NULL;
    int i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];

        if (app_ptr && btaddr_same(raddr, &(app_ptr->raddr)))
        {
            return app_ptr->flag;
        }
    }

    return 0;
}

#else
void *a2dp_get_svc_based_on_raddr(const btaddr_t *raddr)
{
    void *svc=NULL;
    result_t ret;
    btaddr_t r_addr;
    a2dp_backend_t *app_ptr = &g_a2dp;
    if((app_ptr)
        && (app_ptr->svc))
    {
        ret = a2dp_peer_addr(app_ptr->svc, &r_addr);
        //os_printf("=====svc:%x,%x,%x,%x,%x,%x\r\n",r_addr.b[0],r_addr.b[1],r_addr.b[2],r_addr.b[3],r_addr.b[4],r_addr.b[5]);
        //os_printf("=====svc:%x,%x,%x,%x,%x,%x\r\n",raddr->b[0],raddr->b[1],raddr->b[2],raddr->b[3],raddr->b[4],raddr->b[5]);
        if((UWE_OK == ret)
            && (btaddr_same(raddr, &r_addr)))
        {
            svc = app_ptr ->svc;
        }
    }
    return svc;
}

uint32_t a2dp_all_is_not_music(void)
{
    return (!(g_a2dp.flag & APP_FLAG_MUSIC_PLAY));
}

uint32_t a2dp_has_music(void)
{
    return (g_a2dp.flag & APP_FLAG_MUSIC_PLAY);
}

uint32_t a2dp_has_connection(void)
{
    return (g_a2dp.flag & APP_FLAG_A2DP_CONNECTION);
}

#endif

int a2dp_is_connected_based_on_raddr(const btaddr_t *raddr)
{
    result_t ret;
    btaddr_t r_addr;
    uint32_t connected=0;
    a2dp_backend_t *app_ptr=NULL;

#ifdef BT_ONE_TO_MULTIPLE
    uint32_t i;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if((app_ptr)
            && (app_ptr->svc))
        {
            ret = a2dp_peer_addr(app_ptr->svc, &r_addr);
            if((UWE_OK == ret)
                && (btaddr_same(raddr, &r_addr))
                && (app_ptr->flag & APP_FLAG_A2DP_CONNECTION))
            {
                connected = 1;
                break;
            }
        }
    }
#else
    app_ptr = &g_a2dp;
    if((app_ptr)
            && (app_ptr->svc))
    {
        ret = a2dp_peer_addr(app_ptr->svc, &r_addr);
        if((UWE_OK == ret)
            && (btaddr_same(raddr, &r_addr))
            && (app_ptr->flag & APP_FLAG_A2DP_CONNECTION))
        {
            connected = 1;
        }
    }
#endif

    return connected;
}

uint32_t a2dp_current_is_pause(void)
{
    return (!(g_current_a2dp_ptr->flag & APP_BUTTON_FLAG_PLAY_PAUSE));
}

uint32_t a2dp_current_is_music(void)
{
	if(g_current_a2dp_ptr == NULL)
		return 0;
	else
        return (g_current_a2dp_ptr->flag & APP_FLAG_MUSIC_PLAY);
}

static void a2dp_connected(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx)
{
#if UPGRADED_VERSION /* here is one choice place to set delay_report value. */
    if (isAvdtpSrcSupportDR()) {
        set_delay_report_ms(1500);
    }
#endif

    a2dp_backend_t *app_ptr;
//#if (defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)) && !defined(BT_ONLY_ONE_BT)
#if 1/*((CONFIG_APP_TOOLKIT_5 == 1))*/||(defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS))
    //app_handle_t sys_hdl = app_get_sys_handler();
    //extern int hci_get_acl_link_count(hci_unit_t *unit);
    //int acl_count = hci_get_acl_link_count(sys_hdl->unit);

    #if (BT_ONE2MULTIPLE_AS_SCATTERNET == 0)
	    //if (acl_count > 1)
		#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
		extern int app_env_check_Use_ext_PA(void);
			if(app_check_bt_mode(BT_MODE_1V2)
			||(app_check_bt_mode(BT_MODE_1V1)&&!app_env_check_Use_ext_PA())
			||(	app_check_bt_mode(BT_MODE_TWS|BT_MODE_DM_TWS)
				&&bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
				&&bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER)))
		#else
		    #ifdef CONFIG_TWS
		    if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE) && bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
			#endif
		#endif
	    {

	        //os_printf("acl_cnt: %d\r\n", acl_count);
	        if(Judge_role())
	        {
	    	    return;
	        }
	    }
    #endif
#endif
    lp_pdd_a2dp_connect_event(); // add by cai.zhong for a2dp event
    os_printf("A2DP CONNECTED\r\n");
#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif

    a2dp_set_flag(app_ptr, APP_FLAG_A2DP_CONNECTION);
#ifdef BT_ONE_TO_MULTIPLE
    if(!a2dp_has_music()) //if there is no music,we update the current_a2dp_ptr to the new connection 
		g_current_a2dp_ptr = app_ptr;
#endif

    //#ifdef BT_ONE_TO_MULTIPLE   //It is controlled by BT_ONE_TO_MULTIPLE,why ???
    #ifdef A2DP_PATCH_FOR_AVRCP
    {
        btaddr_t r_addr;
        if(UWE_OK == a2dp_peer_addr(app_ptr->svc, &r_addr))
        {
            btaddr_copy(&app_ptr->raddr, &r_addr);
            a2dp_patch_get_raddr(&app_ptr->raddr);
	       //a2dp_startup_try_avrcp();
        }
    }
    #endif
    //#endif
    
    app_bt_profile_conn_wrap(PROFILE_ID_A2DP,&app_ptr->raddr);
#ifdef CONFIG_BLUETOOTH_AVDTP_SCMS_T
    init_security_control_r_addr(&app_ptr->raddr); //initialize the content protection flag's raddr
#endif
}

#if UPGRADED_VERSION
extern result_t a2dp_send_delay_report(bt_a2dp_sink_session_h session, uint16_t dr_ms);

result_t a2dp_send_delayReport(uint16_t dr_ms)
{
    if (!g_current_a2dp_ptr) {
        os_printf("a2dp_send_delayReport, param error! 0x%x, %d \r\n", g_current_a2dp_ptr, dr_ms);
        return UWE_PARAM;
    }
    
    os_printf("a2dp_send_delayReport, 0x%x, %d \r\n", g_current_a2dp_ptr->svc, dr_ms);
    return a2dp_send_delay_report(g_current_a2dp_ptr->svc, dr_ms);
}
#endif

static void a2dp_disconnected(bt_a2dp_sink_session_h session_h,
    result_t status, bt_app_ctx_h app_ctx)
{
    a2dp_backend_t *app_ptr;

    #ifdef A2DP_PATCH_FOR_AVRCP
    a2dp_finish_try_avrcp();
    #endif

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif

    app_ptr->is_used = 0;
    if(a2dp_has_connection())
          audio_out_close(&app_ptr->audio_out);

#ifdef BT_ONE_TO_MULTIPLE
	//if we disconnect the connection during the music playing,we need to close audio particularly
    if(a2dp_has_the_music(app_ptr->raddr)&&(!get_other_a2dp_play_flag(&app_ptr->raddr)))//if we disconnect the connection during the music playing,we need to close audio particularly
#else
    if(a2dp_has_music())
#endif
    {
       if((!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))&&(!app_wave_playing()))
       {
        #ifdef CONFIG_BLUETOOTH_COEXIST
            if(app_is_bt_mode())
        #endif
            {
                aud_PAmute_oper(1);
                aud_close();
                aud_volume_mute(1);
            }
        }
       bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 0);
    }
    a2dp_clear_flag(app_ptr, APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE);
    a2dp_clear_flag(app_ptr, APP_FLAG_A2DP_CONNECTION);
    lp_pdd_a2dp_disconnect_event(); // add by cai.zhong for a2dp event
#ifdef CONFIG_BLUETOOTH_AVDTP_SCMS_T
    uinit_security_control_r_addr(&app_ptr->raddr);//uninitialize the content protection flag's raddr
#endif

    // If there is ACL link, and audio src does not explicitly terminate the A2DP connection,
    // allow a2dp to retry 3 times
#ifdef BT_ONLY_ONE_BT
    if ((status != UWE_CONNRESET) && (status != UWE_NOLINK) && (app_ptr->svc_id <= 15)) 
    {
        app_ptr->svc_id ++;
        os_printf("a2dp disconnected0: svc-%lu\r\n", app_ptr->svc_id);
        app_bt_allpro_conn_start(202,&app_ptr->raddr);
    }
#else
    if ((status != UWE_CONNRESET) && (status != UWE_NOLINK) && (app_ptr->svc_id <= 3))  
    {
         app_ptr->svc_id ++;

         os_printf("a2dp disconnected0: svc-%lu\r\n", app_ptr->svc_id);

         /*
            When a2dp fails for error situation
         */
         app_bt_active_conn_profile(PROFILE_ID_A2DP, &app_ptr->raddr);
    }
#endif
    else
    {
        os_printf("a2dp disconnected2 %lu\r\n", app_ptr->svc_id);
	#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
		hci_link_t *tmp_link;
		tmp_link = select_flow_ctrl_link_handle_by_raddr(&(app_ptr->raddr));
		if(tmp_link)
		{
            tmp_link->flow_ctrl = 0;
			bt_set_ag_flow_ctrl_by_handle(tmp_link);
		}
	#endif
        app_bt_profile_disconn_wrap(PROFILE_ID_A2DP, app_ptr);

        #ifdef BT_ONE_TO_MULTIPLE
        if(a2dp_select1_current_app(app_ptr) == app_ptr)//if there's anther MUSIC_PLAY on,we change g_current_a2dp_ptr to anther a2dp_ptr and do not come in
        {  
            if(a2dp_has_connection())  //if there is another a2dp_connection we need to change the current_a2dp_ptr  to anther one
                a2dp_select_current_app_to_another(&app_ptr->raddr);
            else
            {
    	    	//os_printf("====current_a2dp_ptr is null=====\r\n");
            	g_current_a2dp_ptr = NULL;
            }
        }
				
        if(app_ptr != &g_a2dp)
        {
            a2dp_app_unregister(app_ptr);
            a2dp_ex_app_uninit(app_ptr);
            os_printf("===a2dp destroy\r\n");
            jfree(app_ptr);
	        app_ptr = NULL;
        }
        else
        #endif
        {
            app_ptr->svc_id = 0;
	}
    }

    #ifdef OTT_STRETIGG_LINK_COEXIST
    if(NULL == g_current_a2dp_ptr)
    {
	//	select_play_a2dp_avrcp();
	/** comment by lianxue.liu,??????????????????? **/
	//	bt_select_other_a2dp_avrcp(btaddr_same(hf_get_curhfp_raddr(),&btaddr));
    }
 //  	bt_select_current_a2dp_avrcp(app_ptr);
    #endif
}

static void a2dp_newconn(bt_conn_req_h conn_req,
                            const btaddr_t *laddr,
                            const btaddr_t *raddr,
                            bt_link_modes_mask_t mode,
                            bt_app_ctx_h app_ctx)
{
    result_t err = UWE_OK;
    a2dp_backend_t *app_ptr;

    #ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_app_lookup_valid(raddr);
/*
	#ifdef CONN_WITH_MUSIC
	bt_flag2_operate(APP_FLAG2_SW_MUTE,0);
	#endif
*/
    if((0 == app_ptr)
        && (a2dp_app_get_count() < BT_MAX_AG_COUNT))
    {
        os_printf("a2dp_newconn\r\n");
        app_ptr = a2dp_ex_app_alloc();
        if(app_ptr)
        {
            a2dp_ex_app_init(app_ptr);
            a2dp_app_register(app_ptr);
        }
    }
    #else
    app_ptr = &g_a2dp;
    #endif

    if(0 == app_ptr)
    {
        os_printf("a2dp_unvalid_app\r\n");
        return;
    }
#ifdef BEKEN_SPP_OTA_3266
    if(flag_ota_enable)
    {
        os_printf("a2dp_reject\r\n");
        bt_a2dp_sink_conn_reject(conn_req);
        err = UWE_STATE;
        goto Exit;
    }
#endif
    
#ifdef SUPPORT_SPP_IN_MP3PLAYER
    app_handle_t app_h = app_get_sys_handler();
    if(SYS_WM_BT_MODE !=app_h->sys_work_mode)
    {
        os_printf("a2dp_reject\r\n");
        bt_a2dp_sink_conn_reject(conn_req);
        err = UWE_STATE;
        goto Exit;
    }
#endif

    if(app_ptr->auto_on && !app_ptr->is_used)
    {
        bt_a2dp_endpoint_desc_t ep;

        ep.codecs_count           = 1;
        ep.codecs[0].type         = A2DP_CODEC_SBC;
        bt_a2dp_sink_get_default_sbc(&ep.codecs[0].u.sbc);

        err = bt_a2dp_sink_conn_accept(app_ptr->svc, conn_req, mode, &ep);
        if(err)
        {
            goto Exit;
        }

        app_ptr->is_used = 1;
        //bt_frontend_notification("a2dp newconn request accepted (auto)");

        goto Exit;
    }

    if(app_ptr->is_used || app_ptr->conn_req)
    {
        bt_a2dp_sink_conn_reject(conn_req);
        err = UWE_STATE;
        goto Exit;
    }

    #ifndef BT_ONE_TO_MULTIPLE
    bt_auto_connect_stop();
    #endif

    //bt_frontend_notification("a2dp newconn request %lu", app_ptr->svc_id);

    app_ptr->conn_req      = conn_req;
    app_ptr->conn_mode     = mode;

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
}

static uint32_t a2dp_sbc_rate(const bt_a2dp_codec_t *codec)
{
    const bt_a2dp_sbc_t *sbc = &codec->u.sbc;

    if(sbc->octet0 & SBC_FREQ_48K)
        return 48000;
    else if(sbc->octet0 & SBC_FREQ_44K)
        return 44100;
    else if(sbc->octet0 & SBC_FREQ_32K)
        return 32000;
    else if(sbc->octet0 & SBC_FREQ_16K)
        return 16000;
    else
        return 0;
}

static uint32_t a2dp_sbc_bps(const bt_a2dp_codec_t *codec)
{
    const bt_a2dp_sbc_t *sbc = &codec->u.sbc;

    if(sbc->octet1 & SBC_BLOCK_LEN_4)
        return 4;
    else if(sbc->octet1 & SBC_BLOCK_LEN_8)
        return 8;
    else if(sbc->octet1 & SBC_BLOCK_LEN_12)
        return 12;
    else if(sbc->octet1 & SBC_BLOCK_LEN_16)
        return 16;
    else
        return 0;
}

static uint32_t a2dp_sbc_channels(const bt_a2dp_codec_t *codec)
{
    return (codec->u.sbc.octet0 & SBC_CHANNEL_MONO ? 1 : 2);
}
#ifdef A2DP_SBC_DUMP_SHOW
void a2dp_sbc_info_show(void)
{
	uint8 i;
    for(i=0;i<BT_MAX_AG_COUNT;i++)
        a2dp_codec_dump(&(g_a2dp_array[i]->codec));
}
#endif
static void a2dp_set_configuration_cb(bt_a2dp_sink_session_h session_h,
    const bt_a2dp_codec_t *codec,
    int_t local_ep_id,
    int_t remote_ep_id,
    bt_app_ctx_h app_ctx_h)
{
    result_t err;
    a2dp_backend_t *app_ptr;
    uint32_t rate, channels, bps;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif
    //a2dp_codec_dump(codec);

    rate     = a2dp_sbc_rate(codec);
    bps      = a2dp_sbc_bps(codec);
    channels = a2dp_sbc_channels(codec);

    app_ptr->freq    = rate;
    app_ptr->channel = channels;
    app_ptr->volume  = player_vol_bt;

#ifdef CONFIG_BLUETOOTH_AVRCP_TG
    //a2dp_volume_init_based_on_raddr(env_h->env_cfg.system_para.vol_a2dp, &app_ptr->raddr);
#endif
#ifdef CONFIG_TWS
	memcpy( &g_a2dp.codec, codec, sizeof(g_a2dp.codec) );
#endif
#if defined(A2DP_SBC_DUMP_SHOW) &&  !defined(CONFIG_TWS)
    memcpy( &app_ptr->codec, codec, sizeof(app_ptr->codec) );
#endif

    if(!rate || !bps || !channels)
    {
        err = UWE_INVAL;

        bt_a2dp_sink_set_configuration_rej(app_ptr->svc, local_ep_id,
            remote_ep_id);

        goto Exit;
    }

    os_printf("a2dp_cfg:%d:%x:%x\r\n", rate, channels, bps);

    if(!(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION) || app_wave_playing()))
    {
        audio_out_close(&app_ptr->audio_out);
        err = audio_out_open(rate, channels, bps, &app_ptr->audio_out);
        if(err)
        {
            bt_a2dp_sink_set_configuration_rej(app_ptr->svc, local_ep_id,
                remote_ep_id);
            goto Exit;
        }
    }

    err = bt_a2dp_sink_set_configuration_rsp(app_ptr->svc,
                                            A2DP_INPUT_METHOD_SBC_TO_PCM,
                                            local_ep_id,
                                            remote_ep_id);
	#ifdef CONFIG_TWS
		if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) &&
			bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
			a2dp_src_cmd_reconfig((bt_a2dp_codec_t *) codec );
	#endif

    if(err)
    {
        audio_out_close(&app_ptr->audio_out);
        goto Exit;
    }
Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
}

void a2dp_audio_restore(int mode)
{
    os_printf("a2dp_audio_restore(%d)\r\n", mode);
    if(1 == mode)
    {
    	 aud_close();
	 sbc_discard_uselist_node();  //discard all the old sbc note avoid noise
        sbc_init_adjust_param();
        aud_initial(g_current_a2dp_ptr->freq, g_current_a2dp_ptr->channel, 16);
        aud_volume_set(g_current_a2dp_ptr->volume);
        avrcp_volume_sync();
    }
    player_vol_bt = g_current_a2dp_ptr->volume;
    //os_printf("666666:%d\r\n",player_vol_bt);
    app_wave_file_aud_notify(g_current_a2dp_ptr->freq, g_current_a2dp_ptr->channel, g_current_a2dp_ptr->volume);
}

void stream_start_handler(void *arg)
{
    result_t ret;
    btaddr_t r_addr;
    a2dp_backend_t *app_ptr = (a2dp_backend_t *) arg;
    /*
	if(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
		return;
	*/
    os_printf("stream_start_handler:%d\r\n", app_ptr->freq);
    sbc_stream_start_init();
    //aud_PAmute_oper(0);
#ifdef CONFIG_BLUETOOTH_AVRCP_TG
    //if(bt_flag2_is_set(APP_FLAG2_VOL_SYNC))
    	//sync_ct_tg_volume();
#endif
    // change begin by cai.zhong 20190301 for sync code
    if((!app_wave_playing ()) && (!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
      ||(!app_get_mic_state()))
	// change end by cai.zhong 20190301 for sync code
    {
        a2dp_audio_restore(1);
    #ifdef CONFIG_TWS
        if(!bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ))
    #endif
            aud_open();
    }
    else
    {
        app_wave_file_aud_notify(app_ptr->freq, app_ptr->channel, app_ptr->volume);
        /*        
		if (!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
		{
		   app_wave_file_play_stop();
		   a2dp_audio_restore(1);
	       aud_open();
		}
        */
    }
    ret = a2dp_peer_addr(app_ptr->svc, &r_addr);
    if((UWE_OK == ret)
        && (!a2dp_is_connected_based_on_raddr(&r_addr))
#ifdef CONFIG_TWS
		&&(!bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ))
#endif
		)
    {
        app_bt_active_conn_profile(PROFILE_ID_AVRCP, &r_addr);
    }
    aud_PAmute_oper(0);	
}

void g_stream_start_handler(void) {
#ifndef BT_ONE_TO_MULTIPLE
    stream_start_handler(&g_a2dp);
#endif
}

#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
void g_a2dp_stream_start_cb(void)  
{
   a2dp_stream_start_cb(g_current_a2dp_ptr->svc, NULL);
}
#endif
#if (CONFIG_SELECT_PREV_A2DP_PLAY == 1)
static void a2dp_stream_start_cb(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx)
{
    a2dp_backend_t *app_ptr;
    btaddr_t *raddr;
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
    {
        app_not_bt_set_a2dp_flag(1);
        return ;
    }
#endif
#ifdef CONFIG_TWS
    if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
        app_wave_file_play_stop();
#endif
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
    app_handle_t sys_hdl = app_get_sys_handler();
#endif

    CLEAR_SLEEP_TICK;
   	app_ptr = a2dp_get_app_from_svc(session_h);
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    uint16_t handle;
    handle = bt_sniff_get_handle_from_raddr(&(app_ptr->raddr));
    app_bt_write_sniff_link_policy(handle, 0);
    #endif
	if( !bt_flag1_is_set( APP_FLAG_MUSIC_PLAY ))
	{
		get_cur_hfp_raddr(&raddr);

	    #ifndef OTT_STRETIGG_LINK_COEXIST
	    g_current_a2dp_ptr = app_ptr;
	    #endif
	    a2dp_set_flag(app_ptr, APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE);
#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
 		//for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
	    a2dp_set_flag(app_ptr, APP_A2DP_FLAG_STREAM_START);   
#endif
	    bt_select_current_a2dp_avrcp(app_ptr);
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
		hci_link_t *tmp_link;
    	tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_ACL);
		if(tmp_link)
		{
            tmp_link->flow_ctrl = 0x02;
			bt_set_ag_flow_ctrl_by_handle(tmp_link);
		}
#endif
#ifndef  CONN_WITH_MUSIC
	    if(bt_flag1_is_set(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
	        bt_flag2_operate(APP_FLAG2_RECONN_AFTER_PLAY,1);
#endif
	    stream_start_handler(app_ptr);
		#ifdef CONFIG_APP_EQUANLIZER
				app_equalizer_a2dp_apply();
		#endif
	    //for fixed bug#929
	    bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 1);
	    os_printf(APP_RET_STATUS_MUSIC_PLAY);
	    app_set_led_event_action( LED_EVENT_BT_PLAY );
		#ifdef CONFIG_TWS
		if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) )
		{
		    if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
			{
		        a2dp_src_cmd_stream_start();
			}
		    else
			{
		        bt_flag2_operate( APP_FLAG2_STEREO_STREAMING|APP_FLAG2_SYNC, 1 );

		        bt_flag2_operate( APP_FLAG2_STEREO_SLAVE_GET_TIME, 0 );
			}

		}
		else
		{
			bt_flag2_operate(APP_FLAG2_STEREO_PLAYING,0);
		}
		#endif
	}
    else if(btaddr_same(&(g_current_a2dp_ptr->raddr),&(app_ptr->raddr))) //For xiaomi stream_start twice!!!!
    {
        os_printf(APP_RET_STATUS_MUSIC_PLAY);
        return;
    }
	else /* Device has been playing music , and remote addr is not same.*/
	{
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
		hci_link_t *tmp_link;
		tmp_link = select_curr_playing_link_handle_by_a2dp(app_ptr);
		if(tmp_link)
		{
            tmp_link->flow_ctrl = 0x01;
			bt_set_ag_flow_ctrl_by_handle(tmp_link);
		}
#endif
		a2dp_set_flag(app_ptr, APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE);
#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
 		//for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
	    a2dp_set_flag(app_ptr, APP_A2DP_FLAG_STREAM_START);   
#endif	
		/* 
			This notify will modify app_aud_restore() parameters,when promt wave finished and stop.
			comment this app_wave_file_aud_notify(), a2dp_audio_restore() will do this operation.
		*/ 
		//app_wave_file_aud_notify(app_ptr->freq, app_ptr->channel, app_ptr->volume);
#ifndef  CONN_WITH_MUSIC
	    if(bt_flag1_is_set(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
	        bt_flag2_operate(APP_FLAG2_RECONN_AFTER_PLAY,1);
#endif
		os_printf(APP_RET_STATUS_MUSIC_PLAY);
	}
#ifdef NO_SCAN_WHEN_WORKING
	    app_handle_t sys_hdl = app_get_sys_handler();
	    if(hci_get_acl_link_count(sys_hdl->unit) < BT_MAX_AG_COUNT)
	        bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
#endif
#if (BT_MODE==BT_MODE_1V2)
#if defined(BT_ONE_TO_MULTIPLE) && !defined(CONN_WITH_MUSIC)
	if((hci_get_acl_link_count(sys_hdl->unit)<BT_MAX_AG_COUNT) && app_check_bt_mode(BT_MODE_1V2)) 
		bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
#endif
#endif
}

#else  /* original version */
static void a2dp_stream_start_cb(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx)
{
    a2dp_backend_t *app_ptr;
    btaddr_t *raddr;
#ifdef BT_ONE_TO_MULTIPLE
    app_handle_t sys_hdl = app_get_sys_handler();
#endif
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
    {
        app_not_bt_set_a2dp_flag(1);
        return ;
    }
#endif

#ifdef CONFIG_TWS
    if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE))
        app_wave_file_play_stop();
#endif

    CLEAR_SLEEP_TICK;

#ifdef CONFIG_APP_EQUANLIZER
	app_equalizer_a2dp_apply();
#endif
	get_cur_hfp_raddr(&raddr);

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif

    #ifndef OTT_STRETIGG_LINK_COEXIST
    g_current_a2dp_ptr = app_ptr;
    #endif
#ifndef  BT_ONE_TO_MULTIPLE
    if(bt_flag2_is_set(APP_FLAG2_CALL_PROCESS))
	{
		if (!btaddr_same(raddr, &(app_ptr->raddr)))
			return;
	}
#endif

    a2dp_set_flag(app_ptr, APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE);
#ifdef IPHONE_MUSIC_STATE_COME_LATE_REPAIR
    a2dp_set_flag(app_ptr, APP_A2DP_FLAG_STREAM_START);    //for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
#endif
	
    #ifdef BT_ONE_TO_MULTIPLE
//    bt_auto_connect_stop();

    bt_select_current_a2dp_avrcp(app_ptr);
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
	hci_link_t *tmp_link;
	tmp_link = select_other_playing_link_handle_by_raddr(&(app_ptr->raddr));
	if(tmp_link)
	{
        tmp_link->flow_ctrl = 1;
		bt_set_ag_flow_ctrl_by_handle(tmp_link);
	}
#endif
//	select_other_avrcp_app_pause_by_raddr(&(app_ptr->raddr));

    #endif
	
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
	#if defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)
            uint16_t handle;
            handle = bt_sniff_get_handle_from_raddr(&(app_ptr->raddr));
            app_bt_write_sniff_link_policy(handle, 0);
        #endif
    #endif
#ifdef BT_ONE_TO_MULTIPLE
	/*
	app_handle_t sys_hdl = app_get_sys_handler();
    jtask_schedule(sys_hdl->app_a2dp_task,
                        1000,
                        (jthread_func)stream_start_handler,
                        (void *)app_ptr);
    */
    stream_start_handler(app_ptr);
#ifndef  CONN_WITH_MUSIC
    if(bt_flag1_is_set(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
        bt_flag2_operate(APP_FLAG2_RECONN_AFTER_PLAY,1);
#endif
#else
    stream_start_handler(app_ptr);
#endif
    //for fixed bug#929
    bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 1);
    os_printf(APP_RET_STATUS_MUSIC_PLAY);
    app_set_led_event_action( LED_EVENT_BT_PLAY );

#ifdef NO_SCAN_WHEN_WORKING
    if(hci_get_acl_link_count(sys_hdl->unit) < BT_MAX_AG_COUNT)
        bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
#endif
	#ifdef CONFIG_TWS
	//    bt_unit_set_scan_enable( app_bt_get_handle(0), HCI_PAGE_SCAN_ENABLE );
	//    bt_unit_set_page_activity( app_bt_get_handle(0), 0x1000, 0x11 );

	if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) )
	{
	    if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
		{
	        a2dp_src_cmd_stream_start();
		}
	    else
		{
	        bt_flag2_operate( APP_FLAG2_STEREO_STREAMING|APP_FLAG2_SYNC, 1 );

	        bt_flag2_operate( APP_FLAG2_STEREO_SLAVE_GET_TIME, 0 );
		}

	}

	else
	{
		bt_flag2_operate(APP_FLAG2_STEREO_PLAYING,0);
	}
	#endif
#if (BT_MODE==BT_MODE_1V2)	
#if defined(BT_ONE_TO_MULTIPLE) && !defined(CONN_WITH_MUSIC)
	if((hci_get_acl_link_count(sys_hdl->unit)<BT_MAX_AG_COUNT) && app_check_bt_mode(BT_MODE_1V2)) 
		bt_unit_set_scan_enable(sys_hdl->unit, HCI_NO_SCAN_ENABLE);
#endif
#endif

}
#endif

#ifdef CONFIG_BLUETOOTH_COEXIST
void app_not_bt_set_a2dp_flag(uint8 flag)
{
    uint8 i = 0;
    os_printf("app_not_bt_set_a2dp_flag(%d)\r\n",flag);

    if(flag)
    {
    #ifdef BT_ONE_TO_MULTIPLE
        for(i = 0; i < BT_MAX_AG_COUNT; i ++)
        {
            g_a2dp_array[i]->flag |= (APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY);
        }
    #endif
        bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 1);
    }
    else
    {
    #ifdef BT_ONE_TO_MULTIPLE
        for(i = 0; i < BT_MAX_AG_COUNT; i ++)
        {
            g_a2dp_array[i]->flag  &= ~ (APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY);
        }
    #endif
        bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 0);

    }
}
#endif

extern void app_bt_set_1v2_match_timeout(void);
static void a2dp_stream_suspend_cb(bt_a2dp_sink_session_h session_h,
    bt_app_ctx_h app_ctx)
{
    a2dp_backend_t *app_ptr;
    app_handle_t sys_hdl = app_get_sys_handler();

#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
	hci_link_t *tmp_link;
#endif

#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
    {
        app_not_bt_set_a2dp_flag(0);
        return ;
    }
#endif

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif
    a2dp_clear_flag(app_ptr, APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY);//
    if(!a2dp_has_music())
        bt_flag1_operate(APP_FLAG_MUSIC_PLAY|APP_BUTTON_FLAG_PLAY_PAUSE, 0);
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
    tmp_link = select_flow_ctrl_link_handle_by_raddr(&(app_ptr->raddr));
    if(tmp_link)
    {
        tmp_link->flow_ctrl &= ~(0x01);
        bt_set_ag_flow_ctrl_by_handle(tmp_link);
    }
    tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_ACL);
    if(tmp_link)
    {
        tmp_link->flow_ctrl &= ~(0x02);
        bt_set_ag_flow_ctrl_by_handle(tmp_link);
    }
#endif

/********************??????lianxue.liu
#ifdef BT_ONE_TO_MULTIPLE
    if(a2dp_all_apps_is_unused())
#endif
    {
        sbc_mem_pool_deinit();
        sbc_target_deinit_jfree_buff();
    }
****************************************/

// ......???????lianxue.liu 01/06/2017
#ifndef BT_ONE_TO_MULTIPLE
    if(bt_flag2_is_set(APP_FLAG2_CALL_PROCESS))
    {
        btaddr_t *raddr;
        get_cur_hfp_raddr(&raddr);
        if(!btaddr_same(raddr, &(app_ptr->raddr)))
            return;
    }
#endif

    // change begin by cai.zhong 20190301 for sync code
    if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION) || (!app_get_mic_state()))
    // change end by cai.zhong 20190301 for sync code
    {
        if((!app_wave_playing())
            && (!a2dp_has_music()))
        {
            aud_PAmute_oper(1);
            aud_close();
            aud_volume_mute(1);

            os_printf("stream suspen1\r\n");
        }
        else
        {
            os_printf("stream sus1\r\n");
        }
    }
    else
    {
        if(!app_wave_playing())
        {
            os_printf("stream suspended2\r\n");
            //aud_PAmute_oper(1);
            //hf_audio_conf_restore(1);
	        //aud_open();
        }
        else
        {
            os_printf("stream suspended3\r\n");
            //aud_PAmute_oper(0);
            //hf_audio_conf_restore(0);
        }
    }

    if( !bt_flag1_is_set( APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING )
		&& !a2dp_has_music())
    {
        app_set_led_event_action( LED_EVENT_BT_PAUSE );
    }


#if defined(BT_ONE_TO_MULTIPLE)||defined(CONFIG_TWS)
	#ifdef BT_ONE_TO_MULTIPLE
    if(a2dp_all_is_not_music())
	#endif
    {
        /***************??????????????????????????????????
        if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
            dac_init_buf();
        ???????????????????????????????????***************/
        sbc_discard_uselist_node();
        sbc_init_adjust_param();
        sbc_mem_free();
    }
#endif

#ifdef BT_ONE_TO_MULTIPLE
    #ifdef OTT_STRETIGG_LINK_COEXIST
	//if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
    {
        bt_select_current_a2dp_avrcp(app_ptr);
    #if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
        if(a2dp_check_flag(g_current_a2dp_ptr, APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY))
        {
            tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &g_current_a2dp_ptr->raddr, HCI_LINK_ACL);
            if(tmp_link)
            {
                tmp_link->flow_ctrl |= 0x02;
    		    bt_set_ag_flow_ctrl_by_handle(tmp_link);
            }
        }
    #endif
    }
#else
    otm_current_a2dp_stream_off_handler(app_ptr);
#endif
#endif
    os_printf(APP_RET_STATUS_MUSIC_PAUSE);

#ifdef BT_ONE_TO_MULTIPLE
#ifdef NO_SCAN_WHEN_WORKING
    app_handle_t sys_hdl = app_get_sys_handler();
    if(hci_get_acl_link_count(sys_hdl->unit) < BT_MAX_AG_COUNT)
    {
        if(app_env_check_inquiry_always())
            bt_unit_set_scan_enable(sys_hdl->unit,HCI_INQUIRY_SCAN_ENABLE | HCI_PAGE_SCAN_ENABLE);
        else
            bt_unit_set_scan_enable(sys_hdl->unit, HCI_PAGE_SCAN_ENABLE);
    }
#endif
#if (BT_MODE==BT_MODE_1V2)
#ifndef  CONN_WITH_MUSIC
    app_bt_reconn_after_play();
    if((hci_get_acl_link_count(sys_hdl->unit)<BT_MAX_AG_COUNT) 
        && app_check_bt_mode(BT_MODE_1V2)) 
    {
        if (app_env_check_inquiry_always())
            bt_unit_set_scan_enable(sys_hdl->unit, HCI_INQUIRY_SCAN_ENABLE|HCI_PAGE_SCAN_ENABLE);
        else	
            bt_unit_set_scan_enable(sys_hdl->unit, HCI_PAGE_SCAN_ENABLE);
        app_bt_set_1v2_match_timeout();
    }
#endif
#endif
#endif
#ifdef CONFIG_TWS
    if( bt_flag2_is_set( APP_FLAG2_STEREO_WORK_MODE ) )
    {
        if( bt_flag2_is_set( APP_FLAG2_STEREO_ROLE_MASTER ) )
        {
            //bt_unit_set_scan_enable( app_bt_get_handle(0), HCI_PAGE_SCAN_ENABLE );
            //bt_unit_set_page_activity( app_bt_get_handle(0), 0x400, 0x12);
            bt_flag2_operate(APP_FLAG2_STEREO_AUTOCONN_RETRY, 0 );
            a2dp_src_cmd_stream_suspend();
            app_bt_sync_send_timer_stop();
        }
        else
            bt_flag2_operate( APP_FLAG2_STEREO_STREAMING|APP_FLAG2_STEREO_PLAYING, 0 );

    }
#endif
}
static void a2dp_stream_input_cb(bt_a2dp_sink_session_h session_h, void *buf,
    uint32_t len, bt_app_ctx_h app_ctx_h)
{
    result_t err = UWE_OK;
    a2dp_backend_t *app_ptr;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = a2dp_get_app_from_svc(session_h);
#else
    app_ptr = &g_a2dp;
#endif
    DECLARE_FNAME("a2dp_stream_input_cb");

    err = audio_out_write(app_ptr->audio_out, buf, len);

    UNUSED_VAR(err);
    DBG_RC_X(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
}

static result_t a2dp_server_start(void)
{
    result_t err;
    bt_a2dp_sink_session_h srv = NULL;
    DECLARE_FNAME("a2dp_server_start");

    err = bt_a2dp_sink_server_start(&srv,
                                        BTADDR_ANY,
                                        a2dp_newconn,
                                        NULL);
    if(err)
        goto Exit;

    err = bt_sdp_service_register(BTADDR_ANY,
                                    BT_SVC_TYPE_AUDIO_SINK,
                                    &a2dp_profile,
                                    sizeof(a2dp_profile),
                                    &g_a2dp.sdp);
    if(err)
        goto Exit;

    g_a2dp.srv = srv;
    g_a2dp.auto_on = 1;

Exit:
    if(err && srv)
        bt_a2dp_sink_server_stop(&srv);

    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
    return err;
}

static void a2dp_server_stop(void)
{
    if(g_a2dp.sdp)
    {
        bt_sdp_service_unregister(g_a2dp.sdp);
        g_a2dp.sdp = 0;
    }

    if(g_a2dp.srv)
    {
        bt_a2dp_sink_server_stop(&g_a2dp.srv);
        g_a2dp.srv = NULL;
    }
}

result_t a2dp_backend_init(void)
{
    result_t err;

    g_current_a2dp_ptr = &g_a2dp;
    j_memset(&g_a2dp, 0, sizeof(g_a2dp));

    err = bt_a2dp_sink_register(&a2dp_cbs);
    if(err)
    {
        goto Exit;
    }

    err = a2dp_server_start();
    if(err)
    {
        goto Exit;
    }

    err = bt_a2dp_sink_conn_create(&g_a2dp.svc, &conn_cbs, NULL);

Exit:
    if(err)
    {
        a2dp_backend_uninit();
    }
    
    else
    {
    #ifdef BT_ONE_TO_MULTIPLE
        a2dp_app_register(&g_a2dp);
    #else
        g_a2dp_array[0] = &g_a2dp;
    #endif
    }
    os_printf("a2dp_backend_init:%x\r\n", err);

    return err;
}

void a2dp_backend_uninit(void)
{
    os_printf("a2dp_backend_uninit\r\n");

    audio_out_close(&g_a2dp.audio_out);

    if(g_a2dp.svc)
    {
        bt_a2dp_sink_conn_destroy(&g_a2dp.svc);
    }

    if(g_a2dp.conn_req)
    {
        bt_a2dp_sink_conn_reject(g_a2dp.conn_req);
    }

    a2dp_server_stop();

    bt_a2dp_sink_unregister();

#ifdef BT_ONE_TO_MULTIPLE
    a2dp_app_unregister(&g_a2dp);

    {
        uint32_t i;
        a2dp_backend_t *app_ptr;

        for(i = 0; i < BT_MAX_AG_COUNT; i ++)
        {
            if((g_a2dp_array[i])
                && (&g_a2dp != g_a2dp_array[i]))
            {
                app_ptr = g_a2dp_array[i];

                a2dp_app_unregister(app_ptr);
                a2dp_ex_app_uninit(app_ptr);
                a2dp_ex_app_free(app_ptr);
            }
        }
    }
#endif
}

result_t a2dp_cmd_connect(char *params, unsigned int len)
{
    result_t err;
    int btaddr_tmp[6];
    uint32_t unit_id;
    btaddr_t laddr, raddr;
    a2dp_backend_t *app_ptr;
    bt_a2dp_endpoint_desc_t ep;

    if(j_snscanf(params,
                    NULL,
                    "%u " BTADDR_FORMAT,
                    &unit_id,
                    BTADDR_TMP(btaddr_tmp)) != UW_ARRAY_SIZE(btaddr_tmp) + 1)
    {
        return UWE_INVAL;
    }

#ifndef BT_ONE_TO_MULTIPLE
    app_ptr = &g_a2dp;
    if(app_ptr->is_used)
    {
        return UWE_ALREADY;
    }
#else
    app_ptr = a2dp_app_lookup_valid(&raddr);
    if((app_ptr) && (app_ptr->is_used))
    {
        os_printf("a2dp_already\r\n");

        return UWE_ALREADY;
    }

    if((0 == app_ptr)
        && (a2dp_app_get_count() < BT_MAX_AG_COUNT))
    {
        os_printf("a2dpCmdConnect\r\n");
        app_ptr = a2dp_ex_app_alloc();
        if(app_ptr)
        {
            a2dp_ex_app_init(app_ptr);
            a2dp_app_register(app_ptr);
        }
    }
#endif

    TMP_TO_BTADDR(btaddr_tmp, &raddr);

    err = backend_unit_get_addr(unit_id, &laddr);
    if(err)
    {
        goto Exit;
    }

    ep.codecs_count = 1;
    ep.codecs[0].type = A2DP_CODEC_SBC;
    bt_a2dp_sink_get_default_sbc(&ep.codecs[0].u.sbc);

    err = bt_a2dp_sink_conn_connect(app_ptr->svc, &laddr, &raddr, &ep);
    if(err)
    {
        goto Exit;
    }

    btaddr_copy(&app_ptr->raddr, &raddr);

    app_ptr->is_used = 1;

    os_printf("a2dp_cmd_connect:%lu,app_ptr:0x%x\r\n", app_ptr->svc_id,app_ptr);

Exit:
    return err;
}

result_t a2dp_cmd_disconnect(void)
{
    if(!g_current_a2dp_ptr->svc)
    {
        return UWE_NODEV;
    }

    return bt_a2dp_sink_conn_disconnect(g_current_a2dp_ptr->svc);
}

void a2dp_volume_init(int8_t aud_volume)
{
    g_current_a2dp_ptr->volume = aud_volume;
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_is_bt_mode())
#endif
    player_vol_bt = aud_volume;
    //os_printf("777777:%d\r\n",player_vol_bt);
}

void a2dp_volume_init_based_on_raddr(int8_t aud_volume, const btaddr_t *raddr)
{
    a2dp_backend_t *app_ptr=NULL;
    int i;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if (app_ptr && btaddr_same(raddr, &(app_ptr->raddr)))
        {
            app_ptr->volume = aud_volume;
			break;
        }
    }
}
int8 a2dp_get_volume(void)
{
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_is_bt_mode())
#endif
    player_vol_bt = g_current_a2dp_ptr->volume;
    //os_printf("888888:%d\r\n",player_vol_bt);
    return g_current_a2dp_ptr->volume;
}
int8 a2dp_get_volume_based_on_raddr(const btaddr_t *raddr)
{
	a2dp_backend_t *app_ptr=NULL;
    int i;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if (app_ptr && btaddr_same(raddr, &(app_ptr->raddr)))
        {
            return app_ptr->volume;
        }
    }
    return AUDIO_VOLUME_MAX+1;
}

int a2dp_get_freq(void)
{
	return g_current_a2dp_ptr->freq;
}

result_t a2dp_volume_adjust(uint8_t oper)
{
    a2dp_backend_t *app_ptr;

    app_ptr = g_current_a2dp_ptr;
#ifdef CONFIG_BLUETOOTH_COEXIST
    if(app_coexist_is_not_bt_mode())
    {
        return ;
    }
#endif

    if(oper == 0) //minus
    {
        app_ptr->volume--;
        if(app_ptr->volume <= AUDIO_VOLUME_MIN)
        {
            app_ptr->volume = AUDIO_VOLUME_MIN;
        }
    }
    else  //plus
    {
        app_ptr->volume++;
        if(app_ptr->volume >= AUDIO_VOLUME_MAX)
        {
            app_ptr->volume = AUDIO_VOLUME_MAX;
        }
    }

    if(a2dp_has_music())
    {
        if(app_ptr->volume <= AUDIO_VOLUME_MIN)// && (!app_wave_playing()))
        {
            //os_printf("a2dp_volume_adjust1\r\n");
            if(!app_wave_playing())
            {
                aud_PAmute_oper(1);
                aud_volume_mute(1);
            }

			bt_flag2_operate(APP_FLAG2_VOL_MUTE, 1);
        }
        else
        {
            //os_printf("a2dp_volume_adjust2\r\n");
//            aud_open();
            aud_volume_mute(0);

            if(!bt_flag2_is_set(APP_FLAG2_MUTE_FUNC_MUTE))
            {
                aud_PAmute_oper(0);
            }

	     bt_flag2_operate(APP_FLAG2_VOL_MUTE, 0);
        }
    }

#if (CONFIG_CUSTOMER_ENV_SAVE_VOLUME == 1)
	a2dp_volume_init(app_ptr->volume);
#endif	

    if(!app_wave_playing())
    {
        aud_volume_set(app_ptr->volume);
    }

    app_wave_file_aud_notify(app_ptr->freq, app_ptr->channel, app_ptr->volume);

    if(app_ptr->volume == AUDIO_VOLUME_MAX) // max
    {
        //os_printf("max\r\n");
        app_wave_file_play_start(APP_WAVE_FILE_ID_VOL_MAX);
    }
    else if(app_ptr->volume == AUDIO_VOLUME_MIN) // min
    {
        //os_printf("min\r\n");
        app_wave_file_play_start(APP_WAVE_FILE_ID_VOL_MIN);
    }
    player_vol_bt = app_ptr->volume;

    return 0;
}

void a2dp_codec_dump(const bt_a2dp_codec_t *codec)
{
    /* Codec information element */
    switch (codec->type)
    {
    case A2DP_CODEC_SBC:
        {
            const bt_a2dp_sbc_t *p = &codec->u.sbc;

            bt_frontend_notification(" SBC [%02x %02x]", p->octet0, p->octet1);

            bt_frontend_notification(" bitpool %d/%d", p->min_bitpool,
                p->max_bitpool);

            bt_frontend_notification(" 16KHz %s, 32KHz %s, 44KHz %s, 48KHz %s",
                p->octet0 & SBC_FREQ_16K ? "yes" : "no",
                p->octet0 & SBC_FREQ_32K ? "yes" : "no",
                p->octet0 & SBC_FREQ_44K ? "yes" : "no",
                p->octet0 & SBC_FREQ_48K ? "yes" : "no");

            bt_frontend_notification(" MONO %s, DUAL %s, STEREO %s, "
                "JOINT STEREO %s",
                p->octet0 & SBC_CHANNEL_MONO ? "yes" : "no",
                p->octet0 & SBC_CHANNEL_DUAL ? "yes" : "no",
                p->octet0 & SBC_CHANNEL_STEREO ? "yes" : "no",
                p->octet0 & SBC_CHANNEL_JOINT_STEREO ?"yes":"no");

            bt_frontend_notification(" block 4 %s, 8 %s, 12 %s, 16 %s",
                p->octet1 & SBC_BLOCK_LEN_4 ? "yes" : "no",
                p->octet1 & SBC_BLOCK_LEN_8 ? "yes" : "no",
                p->octet1 & SBC_BLOCK_LEN_12 ? "yes" : "no",
                p->octet1 & SBC_BLOCK_LEN_16 ? "yes" : "no");

            bt_frontend_notification(" subbands 4 %s, 8 %s",
                p->octet1 & SBC_SUBBANDS_4 ? "yes" : "no",
                p->octet1 & SBC_SUBBANDS_8 ? "yes" : "no");

            bt_frontend_notification(" SNR %s, Loudness %s",
                p->octet1 & SBC_ALLOCATION_SNR ? "yes" : "no",
                p->octet1 & SBC_ALLOCATION_LOUDNESS ?"yes":"no");
            break;
        }

    case A2DP_CODEC_MPEG12:
        {
            const bt_a2dp_mpeg_1_2_audio_t *p = &codec->u.mpeg_1_2_audio;

            bt_frontend_notification(" MPEG-1,2 AUDIO [%02x %02x %02x %02x]",
                p->octet0, p->octet1, p->octet2, p->octet3);

            bt_frontend_notification(" I %s, II %s, III %s, CRC %s",
                p->octet0 & MPEG12_LAYER_I ? "yes" : "no",
                p->octet0 & MPEG12_LAYER_II ? "yes" : "no",
                p->octet0 & MPEG12_LAYER_III ? "yes" : "no",
                p->octet0 & MPEG12_CRC ? "yes" : "no");

            bt_frontend_notification(" MONO %s, DUAL %s, STEREO %s, "
                "JOINT STEREO %s",
                p->octet0 & MPEG12_CHANNEL_MONO ? "yes" : "no",
                p->octet0 & MPEG12_CHANNEL_DUAL ? "yes" : "no",
                p->octet0 & MPEG12_CHANNEL_STEREO ? "yes" : "no",
                p->octet0 & MPEG12_CHANNEL_JOINT_STEREO ?
                "yes" : "no");

            bt_frontend_notification(" 16K %s, 22K %s, 24K %s, 32K %s, 44K %s, "
                "48K %s",
                p->octet1 & MPEG12_FREQ_16K ? "yes" : "no",
                p->octet1 & MPEG12_FREQ_22K ? "yes" : "no",
                p->octet1 & MPEG12_FREQ_24K ? "yes" : "no",
                p->octet1 & MPEG12_FREQ_32K ? "yes" : "no",
                p->octet1 & MPEG12_FREQ_44K ? "yes" : "no",
                p->octet1 & MPEG12_FREQ_48K ? "yes" : "no");

            bt_frontend_notification(" MPF %s, VBR %s",
                p->octet1 & MPEG12_MPF ? "yes" : "no",
                p->octet2 & MPEG12_VBR ? "yes" : "no");

            bt_frontend_notification(" bit rate 0x%x, 0x%x",
                p->octet2 & MPEG12_OCTET2_BIT_RATE_MASK,
                p->octet3);
            break;
        }

    case A2DP_CODEC_MPEG_AAC:
        {
            const bt_a2dp_mpeg_aac_t *p = &codec->u.mpeg_aac;

            bt_frontend_notification(" MPEG-2,4 AAC "
                "[%02x %02x %02x %02x %02x %02x]",
                p->object_type, p->octet1, p->octet2, p->octet3, p->octet4,
                p->octet5);

            bt_frontend_notification(" MPEG2 LC %s, MPEG4 LC %s, LTP %s, "
                "SCALABLE %s",
                p->object_type & MPEG2_AAC_LC ? "yes" : "no",
                p->object_type & MPEG4_AAC_LC ? "yes" : "no",
                p->object_type & MPEG4_AAC_LTP ? "yes" : "no",
                p->object_type & MPEG4_AAC_SCALABLE ? "yes" :"no");

            bt_frontend_notification(" 8K %s, 11K %s, 12K %s, 16K %s, "
                "22K %s, 24K %s, 32K %s, 44K %s",
                p->octet1 & MPEG_AAC_FREQ_8K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_11K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_12K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_16K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_22K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_24K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_32K ? "yes" : "no",
                p->octet1 & MPEG_AAC_FREQ_44K ? "yes" : "no");

            bt_frontend_notification(" 48K %s, 64K %s, 88K %s, 96K %s",
                p->octet2 & MPEG_AAC_FREQ_48K ? "yes" : "no",
                p->octet2 & MPEG_AAC_FREQ_64K ? "yes" : "no",
                p->octet2 & MPEG_AAC_FREQ_88K ? "yes" : "no",
                p->octet2 & MPEG_AAC_FREQ_96K ? "yes" : "no");

            bt_frontend_notification(" MONO %s, STEREO %s, VBR %s, "
                "bit rate %ld",
                p->octet2 & MPEG_AAC_CHANNELS_1 ? "yes" : "no",
                p->octet2 & MPEG_AAC_CHANNELS_2 ? "yes" : "no",
                p->octet3 & MPEG_AAC_VBR ? "yes" : "no",
                MPEG_AAC_BIT_RATE(p));
            break;
        }

    case A2DP_CODEC_ATRAC:
        {
            const bt_a2dp_atrac_t *p = &codec->u.atrac;

            bt_frontend_notification(" ATRAC "
                "[%02x %02x %02x %02x %02x %02x %02x]",
                p->octet0, p->octet1, p->octet2, p->octet3, p->octet4,
                p->octet5, p->octet6);
            break;
        }

    default:
        break;
    }
}
#ifdef CONFIG_TWS
static void a2dp_set_flag(a2dp_backend_t *app_ptr, uint32 flag)
#else
void a2dp_set_flag(a2dp_backend_t *app_ptr, uint32 flag)
#endif
{
    app_ptr->flag |= flag;
}

#ifdef CONFIG_TWS
static void a2dp_clear_flag(a2dp_backend_t *app_ptr, uint32 flag)
#else
void a2dp_clear_flag(a2dp_backend_t *app_ptr, uint32 flag)
#endif
{
    app_ptr->flag &= ~flag;
}

void a2dp_current_set_flag(uint32 flag)
{
    g_current_a2dp_ptr->flag |= flag;
}

void a2dp_current_clear_flag(uint32 flag)
{
    g_current_a2dp_ptr->flag &= ~flag;
}

uint32_t a2dp_current_get_flag(void)
{	
	if(g_current_a2dp_ptr == NULL)
		return 0;
	else
    	return g_current_a2dp_ptr->flag;
}

void *a2dp_select1_current_app(void *arg)
{
    uint32 i;
    a2dp_backend_t *app_ptr = (a2dp_backend_t *)arg;

    if(app_ptr->flag & APP_BUTTON_FLAG_PLAY_PAUSE)
    {
        if((g_current_a2dp_ptr)
            && (app_ptr != g_current_a2dp_ptr))
       //     && (g_current_a2dp_ptr->flag & APP_BUTTON_FLAG_PLAY_PAUSE))
        {
            os_printf("start_a2dp\r\n");

     //       goto select_out;
        }

        g_current_a2dp_ptr = app_ptr;
    }
    else
    {
        if((g_current_a2dp_ptr)
            && (!(g_current_a2dp_ptr->flag & APP_BUTTON_FLAG_PLAY_PAUSE)))
        {
            for(i = 0; i < BT_MAX_AG_COUNT; i ++)
            {
                if((g_a2dp_array[i])
                    && (g_a2dp_array[i]->flag & APP_BUTTON_FLAG_PLAY_PAUSE)) //APP_FLAG_A2DP_CONNECTION
                {
                    os_printf("change_a2dp\r\n");

                    g_current_a2dp_ptr = g_a2dp_array[i];
                    app_wave_file_aud_notify(g_current_a2dp_ptr->freq, g_current_a2dp_ptr->channel, g_current_a2dp_ptr->volume);					
                    sbc_stream_start_init();
                    if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING|APP_FLAG_SCO_CONNECTION))
                    {
                        aud_PAmute_oper(0);
                        a2dp_audio_restore(1);
                        aud_open();
                    }
                    goto select_out;
                }
            }
        }
        else if(g_current_a2dp_ptr)
        {
            app_wave_file_aud_notify(g_current_a2dp_ptr->freq, g_current_a2dp_ptr->channel, g_current_a2dp_ptr->volume);
        }
    }

select_out:
    //os_printf("select1:%x\r\n", g_current_a2dp_ptr);
    return g_current_a2dp_ptr;
}

result_t a2dp_select_current_app_to_another(btaddr_t *addr)
{
    uint32 i;
    result_t err = UWE_INVAL;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if((g_a2dp_array[i])
              && (g_a2dp_array[i]->flag & APP_FLAG_A2DP_CONNECTION)
              && (!btaddr_same(addr,&g_a2dp_array[i]->raddr)))
        {
            g_current_a2dp_ptr = g_a2dp_array[i];
        	err = UWE_OK;
			break;
        }
    }
    return err;
}

result_t get_play_a2dp_avrcp_state(uint32 *index)
{
	uint32 i;
	result_t err = UWE_INVAL;
	*index = 0;
	for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if((g_a2dp_array[i])
                 && (g_a2dp_array[i]->flag & (APP_BUTTON_FLAG_PLAY_PAUSE | APP_FLAG_MUSIC_PLAY))
                 && (!(g_a2dp_array[i]->flag & APP_A2DP_PRIVATE_FLAG_FLOW_CTRL))
            )  //APP_FLAG_A2DP_CONNECTION
        {
        	err = UWE_OK;
        	*index = i;
			break;
        }
	}
	return err;
}
result_t  select_a2dp_avrcp(btaddr_t *addr)
{
	uint32 i;
	result_t err = UWE_INVAL;
	for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if((g_a2dp_array[i])
                 && (g_a2dp_array[i]->flag &  APP_FLAG_A2DP_CONNECTION))
        {
        	if(btaddr_same(addr,&(g_a2dp_array[i]->raddr)))
        	{
        		err = UWE_OK;
				g_current_a2dp_ptr = g_a2dp_array[i];
				break;
        	}
        }
	}
	if(err)
		return err;
	return avrcp_update_current_app(addr);

}
void select_play_a2dp_avrcp(void *arg)
{
	uint32 index;
	if(UWE_OK == get_play_a2dp_avrcp_state(&index))
	{
         sbc_stream_start_init();
         aud_PAmute_oper(0);

		 g_current_a2dp_ptr = g_a2dp_array[index];
         //a2dp_audio_restore(1);
		 //aud_open();
		 app_set_led_event_action(LED_EVENT_BT_PLAY);
	}
}
void *a2dp_get_current_app(void)
{
    a2dp_backend_t *app_ptr;

    app_ptr = g_current_a2dp_ptr;
    if(!(app_ptr && app_ptr->is_used))
    {
        app_ptr = 0;
    }

    return (void *)app_ptr;
}

btaddr_t *a2dp_get_current_app_remote_addr(void)
{
   return &(g_current_a2dp_ptr->raddr);
}

void a2dp_clear_current_app(void)
{
    g_current_a2dp_ptr = 0;
}

uint32 a2dp_get_remote_addr(void *arg, btaddr_t *r_addr)
{
    a2dp_backend_t *app_ptr = (a2dp_backend_t *)arg;

    if (UWE_OK != a2dp_peer_addr(app_ptr->svc, r_addr))
    {
        btaddr_copy(r_addr, &app_ptr->raddr); // If session not available, use the address stored locally
    }

    return UWE_OK;
}

btaddr_t a2dp_get_addr_from_ptr(void *arg)
{
	a2dp_backend_t *app_ptr = (a2dp_backend_t *)arg;

    return (app_ptr->raddr);
}

void *a2dp_get_current_session(void)
{
    bt_a2dp_sink_session_h session_h;

    session_h = 0;
    if(g_current_a2dp_ptr)
    {
        session_h = g_current_a2dp_ptr->svc;
    }

    return session_h;
}

#if 0
void get_raddr_from_a2dp(btaddr_t**raddr0,btaddr_t**raddr1)
{
	if(g_a2dp_array[0]->flag & APP_FLAG_A2DP_CONNECTION)
	    *raddr0 = &(g_a2dp_array[0]->raddr);
	if(g_a2dp_array[1]->flag & APP_FLAG_A2DP_CONNECTION)
	    *raddr1 = &(g_a2dp_array[1]->raddr);
}
#endif

void clr_a2dp_flags(btaddr_t *btaddr)
{
	uint32 i;

	for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if(g_a2dp_array[i] && btaddr_same(&g_a2dp_array[i]->raddr,btaddr))
			a2dp_clear_flag(g_a2dp_array[i], APP_BUTTON_FLAG_PLAY_PAUSE|APP_FLAG_MUSIC_PLAY|APP_FLAG_A2DP_CONNECTION);
	}


}
uint32 a2dp_check_flag(void *arg,uint32 flag)
{
    a2dp_backend_t *app_ptr=( a2dp_backend_t *)arg;
    return(app_ptr->flag & flag);  
}

uint32 get_a2dp_priv_flag(uint8 idx,uint32 flag)
{
    if(g_a2dp_array[idx & 0x01])
    return(g_a2dp_array[idx & 0x01]->flag & flag);
    return 0;
}

uint8 get_other_a2dp_play_flag(btaddr_t *btaddr)
{
    uint32 i;
    uint8 rtn = 0;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if( g_a2dp_array[i]
			&& (!btaddr_same(&g_a2dp_array[i]->raddr,btaddr))
			&& (g_a2dp_array[i]->flag & APP_FLAG_MUSIC_PLAY) )
    	{
            rtn = 1;
            break;
    	}
    }
    return rtn;
}
#ifdef CONFIG_TWS
#ifdef CONFIG_BLUETOOTH_AVDTP_SCMS_T
int8_t check_need_enable_scms_t(struct mbuf *m)
{
    result_t err;
	int8_t ret = 0;
	uint8_t m_val_tmp[2];
	if(bt_flag2_is_set(APP_FLAG2_STEREO_WORK_MODE)
		&&!bt_flag2_is_set(APP_FLAG2_STEREO_ROLE_MASTER))
	{
//		os_printf("111");
		err = m_trim(m, sizeof(m_val_tmp), &m_val_tmp);
		if(err)
			return 0;
//		os_printf("m:%x,%x\r\n",m_val_tmp[0],m_val_tmp[1]);
		if((m_val_tmp[0]!=0x9c)&&(m_val_tmp[1]==0x9c))
			ret = 1;
	}
	return ret;
}
#endif
static void a2dp_reconfiguration_cb(bt_a2dp_sink_session_h session_h,
    const bt_a2dp_codec_t *codec, int_t tid, int_t remote_ep_id,
    bt_app_ctx_h app_ctx_h)
{
    result_t err;
    uint32_t rate, channels, bps;
    app_env_handle_t env_h = app_env_get_handle();

    os_printf("reconfig cb\r\n");

    a2dp_codec_dump(codec);

    rate = a2dp_sbc_rate(codec);
    bps = a2dp_sbc_bps(codec);
    channels = a2dp_sbc_channels(codec);

    if (!rate || !bps || !channels)
    {
        err = UWE_INVAL;
        bt_a2dp_sink_reconfig_res(g_a2dp.svc, tid, remote_ep_id, 1);
        goto Exit;
    }

    g_a2dp.freq = rate;
    g_a2dp.channel = channels;
   // a2dp_volume_init( env_h->env_cfg.system_para.vol_a2dp );

    if( !(bt_flag1_is_set(APP_FLAG_SCO_CONNECTION) || app_wave_playing()))
    {
        audio_out_close(&g_a2dp.audio_out);
        err = audio_out_open(rate, channels, bps, &g_a2dp.audio_out);
        if (err)
        {
            bt_a2dp_sink_reconfig_res(g_a2dp.svc, tid, remote_ep_id, 1);
            goto Exit;
        }
    }

    err = bt_a2dp_sink_reconfig_res(g_a2dp.svc, tid, remote_ep_id, 0);
    if (err)
    {
        audio_out_close(&g_a2dp.audio_out);
        goto Exit;
    }

Exit:
    DBG_RC_I(err, DBT_APP, ("%s: done, %s\n", FNAME, uwe_str(err)));
}

bt_a2dp_codec_t *a2dp_get_codec( void )
{
    return &g_a2dp.codec;
}

#endif
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
a2dp_backend_t *get_other_a2dp_by_raddr(btaddr_t *btaddr)
{
    uint32 i;
	a2dp_backend_t *a2dp_ptr = NULL;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if( g_a2dp_array[i]
			&& (!btaddr_same(&g_a2dp_array[i]->raddr,btaddr))
			&& (g_a2dp_array[i]->flag & APP_FLAG_MUSIC_PLAY) )
    	{
	    	a2dp_ptr = g_a2dp_array[i];
            break;
    	}
    }
    return a2dp_ptr;
}
#if(CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
hci_link_t *select_other_playing_link_handle_by_raddr(btaddr_t *raddr)
{
	a2dp_backend_t *app_ptr=NULL;
    hci_link_t * tmp_link = NULL;
    app_handle_t sys_hdl;
    sys_hdl = app_get_sys_handler();
    os_printf("a2dp===select other raddr: "BTADDR_FORMAT"\r\n", BTADDR(raddr));
    app_ptr = get_other_a2dp_by_raddr(raddr);
    if(app_ptr)
    {
        os_printf("===other link handle\r\n");
        a2dp_set_flag(app_ptr,APP_A2DP_PRIVATE_FLAG_FLOW_CTRL);
        tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_ACL);
        return tmp_link;
    }
    return NULL;
}
#endif
hci_link_t *select_curr_playing_link_handle_by_a2dp(a2dp_backend_t *app_ptr)
{
    hci_link_t * tmp_link = NULL;
    app_handle_t sys_hdl;
    sys_hdl = app_get_sys_handler();
    if(app_ptr)
    {
    	a2dp_set_flag(app_ptr,APP_A2DP_PRIVATE_FLAG_FLOW_CTRL);
    	tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_ACL);
		return tmp_link;
    }
	return NULL;
}

#ifdef SBC_FIRST_DISCARD_ENABLE 
extern uint8 sbc_first_discrad_flag;
extern uint8 sbc_first_discard_count;
extern uint8 sbc_first_discard_data;
#endif
hci_link_t *select_flow_ctrl_link_handle_by_raddr(btaddr_t *raddr)
{
    int i;
	a2dp_backend_t *app_ptr=NULL;
    hci_link_t * tmp_link = NULL;
    app_handle_t sys_hdl;
    sys_hdl = app_get_sys_handler();
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_a2dp_array[i];
        if (app_ptr && (app_ptr->flag & APP_A2DP_PRIVATE_FLAG_FLOW_CTRL))
        {
            /*avoid the "pop" due to the flow ctrl recover(2DH-1->2DH-5) */
			#ifdef SBC_FIRST_DISCARD_ENABLE 
			sbc_first_discrad_flag = 1;
			sbc_first_discard_count = 0;
			sbc_first_discard_data = 3; //empirical value by measured
			#endif
       		a2dp_clear_flag(app_ptr,APP_A2DP_PRIVATE_FLAG_FLOW_CTRL);
	    	tmp_link = hci_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_ACL);
            //os_printf("===Flow ctrl link handle:%04x\r\n",tmp_link->hl_handle);
			return tmp_link;
        }
    }
	return NULL;
}

#endif

// EOF
