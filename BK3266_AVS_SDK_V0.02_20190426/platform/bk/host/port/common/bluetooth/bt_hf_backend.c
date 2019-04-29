/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <string.h>
#include <stdlib.h>

#include <jos.h>
#include <bluetooth.h>
#include <bt_api.h>
#include <bt_hfp_hf.h>
#include "bt_app_internal.h"
#include "bt_rfcomm_channel.h"
#include "bt_sco_backend_utils.h"
#include "app_beken_includes.h"

#ifdef IPHONE_BAT_DISPLAY
uint8  set_iphone_bat_lever(void);
//ADC = (VBAT*1024)/4.6
const uint16 iphone_bat_lab[10]=
{
    712,		// 3.2V
    734,		// 3.3V
    755,		// 3.4V
    767,		// 3.45V
    779,		// 3.5V
    800,		// 3.6V
    822,		// 3.7V
    845,		// 3.8V	
    867,		// 3.9V
    890,		// 4.0V
};
#endif
#if(CONFIG_HFP17_MSBC_SUPPORTED == 1)
#define HF_SUPPORTED_FEATURES (                                    \
                               HF_CLI_PRESENTATION_CAPABILITY |    \
                               HF_REMOTE_VOLUME_CONTROL |        \
                               HF_VOICE_RECOGNITION_ACTIVATION|        \
                               HF_CODEC_NEGOTIATION|                \
                               HF_EC_NR_FUNCTION|   \
                               HF_CALL_WAITING_AND_3WAY_CALLING)
#else
#define HF_SUPPORTED_FEATURES (                                    \
                               HF_CLI_PRESENTATION_CAPABILITY |    \
                               HF_REMOTE_VOLUME_CONTROL |        \
                               HF_VOICE_RECOGNITION_ACTIVATION|        \
                               HF_CALL_WAITING_AND_3WAY_CALLING)
#endif

#define HFP_VOLUME_MAX        11

volatile uint8_t g_active_sco_and_incoming_flag = 0;


/* HF Service-level Notifications */
static void hf_at_result_print(hfp_hf_app_t *app_ptr, const at_result_t *at_result);
static void hfp_hf_connected(bt_hfp_hf_session_h session, bt_app_ctx_h app_ctx);
static void hfp_hf_disconnected(bt_hfp_hf_session_h session, result_t status,
                                bt_app_ctx_h app_ctx);

static bt_hfp_hf_conn_cbs_t hfp_hf_conn_cbs = {
    hfp_hf_connected,
    hfp_hf_disconnected
};

static void hfp_hf_newconn(bt_conn_req_h conn_req, const btaddr_t *laddr,
                           const btaddr_t *raddr, bt_link_modes_mask_t mode,
                           bt_app_ctx_h app_server_ctx);

void hfp_hf_input(bt_hfp_hf_session_h session, at_result_t *at_result,
                  bt_app_ctx_h app_ctx);

void hfp_get_features(bt_hfp_hf_session_h session_h,
                      uint16_t *supported_features, bt_app_ctx_h app_ctx);

static const bt_hfp_hf_cbs_t hfp_hf_cbs = {
    hfp_hf_input,
    hfp_get_features
};

result_t hf_cmd_disconnect(void);
result_t hf_cmd_accept_call(void);
result_t hf_cmd_hangup_call(int8_t reason);
result_t hf_cmd_place_call(char number[32]);
result_t hf_cmd_redial(void);
result_t hf_cmd_resp_and_hold(uint32_t command);
result_t hf_cmd_resp_and_hold_read(void);
result_t hf_cmd_set_call_wait_notif(uint32_t enable);
result_t hf_cmd_chld(uint32_t command, uint32_t call_id);
result_t hf_cmd_list_current_calls(void);
result_t hf_cmd_set_cli_notif(uint32_t enable);
result_t hf_cmd_get_operator(void);
result_t hf_cmd_get_subscriber(void);
result_t hf_cmd_set_iphone_batlevel(int8 level);

uint32_t a2dp_has_music(void);
btaddr_t *a2dp_get_current_app_remote_addr(void);

extern boolean bt_is_ag_support_feature(bt_hfp_hf_session_h session,uint8 feature);

static enum {
    UNINIT = 0,
    HF_REGISTER,
    SERVER_START,
    BACKEND_REGISTER,
    CONN_REGISTER
} hf_sample_init_stage = UNINIT;

void hf_backend_uninit(void);

/* XXX: Currently, only single service is supported by this sample */
static hfp_hf_app_t g_hfp_hf_app;
static hfp_hf_app_t *g_hf_current_app_ptr;

#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
static hfp_hf_app_t *g_hfp_app_array[2] = {0};
#else
static hfp_hf_app_t *g_hfp_app_array[BT_MAX_AG_COUNT] = {0};
#endif

void hfp_app_ptr_debug_printf(void)
{
    os_printf("----------------------------------------------------\r\n");
	if(g_hf_current_app_ptr == NULL)
		os_printf("|         current_hfp:NULL\r\n");
	else
	{
		os_printf("|           current_hfp:%x(%s)\r\n",g_hf_current_app_ptr,backend_unit_get_name(&(g_hf_current_app_ptr->raddr)));
		os_printf("|      current_hfp_flag:%x\r\n", g_hf_current_app_ptr->flag);
		os_printf("|    current_hfp_volume:%d\r\n", g_hf_current_app_ptr->volume);
		os_printf("|   current_hfp_session:%d\r\n", g_hf_current_app_ptr->session);
		os_printf("|   current_hfp_is_used:%d\r\n", g_hf_current_app_ptr->is_used);
	}
    os_printf("----------------------------------------------------\r\n");
}

hfp_hf_app_t *hfp_get_app_from_session(bt_hfp_hf_session_h session)
{
    uint32_t i;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) && (session == g_hfp_app_array[i]->session))
        {
            app_ptr = g_hfp_app_array[i];

            break;
        }
    }

#ifdef BEKEN_DEBUG
    if (!app_ptr)
        os_printf("hfp_get_app_from_session is NULL\r\n");
#endif
    return app_ptr;
}

#ifdef BT_ONE_TO_MULTIPLE


static uint32_t g_hfp_app_count = 0;
uint32_t hfp_all_apps_is_unused(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) &&
           (1 == g_hfp_app_array[i]->is_used))
        {
            return 0;
        }
    }

    return 1;
}

hfp_hf_app_t *hfp_get_app_from_priv(sco_utils_h priv)
{
    uint32_t i;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && (priv == g_hfp_app_array[i]->priv))
        {
            app_ptr = g_hfp_app_array[i];
            break;
        }
    }

#ifdef BEKEN_DEBUG
    if (!app_ptr)
        os_printf("hfp_get_app_from_priv is NULL\r\n");
#endif

    return app_ptr;
}

void* hfp_get_app_from_priv_ext(sco_utils_h priv)
{
   return (void*)hfp_get_app_from_priv(priv);
}

int hfp_current_is_calling(void)
{
    int ret = 0;

    if((g_hf_current_app_ptr)
       && (g_hf_current_app_ptr->flag_establish_call))
    {
        ret = 1;
    }

    return ret;
}

int hfp_check_establish_call_status(void *ctx)
{
    hfp_hf_app_t *app_ptr;

    app_ptr = hfp_get_app_from_priv(ctx);

    return app_ptr->flag_establish_call;
}

uint32_t hfp_app_register(hfp_hf_app_t *ptr)
{
    uint32_t i;

    if(g_hfp_app_count >= BT_MAX_AG_COUNT)
    {
        return UWE_FULL;
    }

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((hfp_hf_app_t *)0 == g_hfp_app_array[i])
        {
            g_hfp_app_array[i] = ptr;
            g_hfp_app_count ++;

            //os_printf("hfp_app_register:%x\r\n", ptr);

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

uint32_t hfp_app_unregister(hfp_hf_app_t *ptr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(ptr == g_hfp_app_array[i])
        {
            g_hfp_app_array[i] = 0;
            g_hfp_app_count --;

            return UWE_OK;
        }
    }

    return UWE_UNKNOWN;
}

hfp_hf_app_t *hfp_app_lookup_valid(const btaddr_t *raddr)
{
    uint32_t i;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if((app_ptr)
           && (btaddr_same(raddr, &app_ptr->raddr)))
        {
            return app_ptr;
        }
    }


    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if((app_ptr)
           && (!app_ptr->is_used))
        {
            app_ptr->flag = 0;
            break;
        }
    }

    return app_ptr;
}

hfp_hf_app_t *hfp_ex_app_alloc(void)
{
    hfp_hf_app_t *app_ptr;

    app_ptr = (hfp_hf_app_t *)jmalloc(sizeof(hfp_hf_app_t), M_ZERO);
    app_ptr->pending_wave_id = -1;
    return app_ptr;
}

void hfp_ex_app_free(hfp_hf_app_t *app_ptr)
{
    jfree(app_ptr);
    app_ptr = NULL; 
}

uint32_t hfp_ex_app_init(hfp_hf_app_t *app_ptr)
{
    result_t err;

    //os_printf("hfp_ex_app_init\r\n");
    err = bt_hfp_hf_conn_create(&app_ptr->session,
                                &hfp_hf_conn_cbs,
                                NULL);

    return err;
}

uint32_t hfp_ex_app_uninit(hfp_hf_app_t *app_ptr)
{
    bt_hfp_hf_conn_destroy(&app_ptr->session);

    return UWE_OK;
}

uint32_t hfp_app_get_count(void)
{
    return g_hfp_app_count;
}

uint32_t hfp_has_connection(void)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) &&
           (g_hfp_app_array[i]->flag & APP_FLAG_HFP_CONNECTION))
        {
            return 1;
        }
    }

    return 0;
}

uint32_t hfp_is_connection_based_on_raddr(btaddr_t *raddr)
{
    uint32_t i;
    uint32_t connected=0;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if((app_ptr)
           && (btaddr_same(raddr, &app_ptr->raddr))
           && (app_ptr->flag & APP_FLAG_HFP_CONNECTION))
        {
            connected = 1;
            break;
        }
    }

    return connected;
}

uint32_t hfp_is_connecting_based_on_raddr(btaddr_t *raddr)
{
    uint32_t i;
    uint32_t connecting=0;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if((app_ptr)
           && (btaddr_same(raddr, &app_ptr->raddr))
           && (app_ptr->is_used))
        {
            connecting = 1;
            break;
        }
    }

    return connecting;
}

uint32 hfp_check_autoconn_disconned_flag(btaddr_t *r_addr)
{
    hfp_hf_app_t *app_ptr=NULL;

    app_ptr = hfp_app_lookup_valid(r_addr);

    if (app_ptr)
        return app_ptr->flag & FLAG_HFP_AUTOCONN_DISCONNED;
    else
        // The app_ptr may be freed at this point. Then borrow the system flag
        return bt_flag1_is_set(FLAG_HFP_AUTOCONN_DISCONNED);
}
#else

int hfp_current_is_calling(void)
{
    return g_hfp_hf_app.flag_establish_call;
}

int hfp_check_establish_call_status(void *ctx)
{
    return g_hfp_hf_app.flag_establish_call;
}

uint32_t hfp_is_connection_based_on_raddr(btaddr_t *raddr)
{
    result_t ret;
    btaddr_t r_addr;
    uint32_t connected=0;
    hfp_hf_app_t *app_ptr;

    app_ptr = &g_hfp_hf_app;

    if((app_ptr)
       && (app_ptr->session))
    {
        ret = bt_hfp_peeraddr((void *)app_ptr->session, &r_addr);

        if((UWE_OK == ret)
           && (btaddr_same(raddr, &r_addr))
           && (app_ptr->flag & APP_FLAG_HFP_CONNECTION))
        {
            connected = 1;
        }
    }

    return connected;
}
uint32_t hfp_has_connection(void)
{
    return g_hfp_hf_app.flag & APP_FLAG_HFP_CONNECTION;
}

uint32 hfp_check_autoconn_disconned_flag(btaddr_t *r_addr)
{
    hfp_hf_app_t *app_ptr;

    app_ptr = &g_hfp_hf_app;

    return app_ptr->flag & FLAG_HFP_AUTOCONN_DISCONNED;
}

int8_t hfp_is_used(void)
{
    return g_hfp_hf_app.is_used;
}
#endif

#ifdef IPHONE_BAT_DISPLAY
extern uint16 bat_now_value;
uint8 set_iphone_bat_lever(void)
{
    uint16 i = 0;
    uint16 temp = 0;
    app_env_handle_t env_h = app_env_get_handle();
    app_bat_display_t *bat_level_tab = &env_h->env_cfg.feature.bat_display_level;

    temp = saradc_get_value(); // bat_now_value

    for(i = 0; i < 10; i ++)
    {
    #if 1//(CONFIG_APP_TOOLKIT_5 == 1)
        if(temp <= bat_level_tab ->bat_level[i])
    #else
        if(temp <= iphone_bat_lab[i])
    #endif
    	{
    		if(i < 0)
    			i = 0;
    		if(i > 9)
    			i = 9;
    		return i;
    	}
    }
    return 9;
}
#endif

static void hfp_hf_connected(bt_hfp_hf_session_h session, bt_app_ctx_h app_ctx)
{
    //result_t rc;
    hfp_hf_app_t *app_ptr;
	app_env_handle_t env_h = app_env_get_handle();

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = hfp_get_app_from_session(session);
#else
    app_ptr = &g_hfp_hf_app;
#endif

    if(0 == app_ptr) {
        os_printf("hfp_hf_connected_null_app_ptr\r\n");
        return;
    }
    /*
    rc = util_sco_open(&app_ptr->laddr,
                       &app_ptr->raddr,
                       &app_ptr->priv);
    if(rc) {
        app_ptr->is_used = 0;
        os_printf("hfp_hf_connected-sco failed to open %s\r\n", uwe_str(rc));
    } else {
        os_printf("hfp_hf_connected-sco,app_ptr->svc_id=%lu, g_hf_current_app_ptr=0x%x\r\n", app_ptr->svc_id,g_hf_current_app_ptr);
    }
    */
    /*added by beken*/
    hf_set_flag(app_ptr, APP_FLAG_HFP_CONNECTION);
	
#ifdef BT_ONE_TO_MULTIPLE
   
    if(hf_check_flag(app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)) 
    {
        //first,if the newest connected ag is calling,we both need to (update the hf_current_app to the newest connected ag) and (enable hfp_button).
        g_hf_current_app_ptr = app_ptr;
        app_button_type_set(BUTTON_TYPE_HFP); 
        if(bt_flag2_is_set(APP_FLAG2_HFP_SECOND_CALLSETUP)) /* for twc button mode */
            app_button_type_set(BUTTON_TYPE_TWC); 
        
        //os_printf("<<>> hfp_hf_connected() 3, ag2 is calling,0x%x\r\n",g_hf_current_app_ptr);
    } 
    else if (!has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)) 
    {
        //second,if there is no call,we update the hf_current_app to the newest connected ag. 
	    g_hf_current_app_ptr = app_ptr;
        
        //os_printf("<<>> hfp_hf_connected() 2, updated to ag2,no calling,0x%x\r\n",g_hf_current_app_ptr);
    }
    
    if(NULL == g_hf_current_app_ptr) {
        //last,if there was no ag connected before, just update the hf_current_app to the newest connected ag.
        g_hf_current_app_ptr = app_ptr;
        //os_printf("<<>> hfp_hf_connected() 1, first ag connected,0x%x\r\n",g_hf_current_app_ptr);
    }
    
#else
    g_hf_current_app_ptr = app_ptr;
#endif

    app_ptr->volume = env_h->env_cfg.system_para.vol_hfp; //initial the hfp volume 

    if(hf_check_flag(app_ptr,APP_HFP_PRIVATE_FLAG_CODEC_NEGOTIATION)) 
    {
        app_ptr->freq = 16000;
    } 
    else 
    {
        app_ptr->freq = 8000;
    }
    os_printf("HFP_HF_CONNECTED\r\n");
    app_bt_profile_conn_wrap(PROFILE_ID_HFP,&app_ptr->raddr);

#ifdef IPHONE_BAT_DISPLAY
    if(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_APP_BAT_DISPLAY) {
    	 bt_hfp_hf_xtalapp(session);
         //hf_cmd_set_iphone_batlevel(set_iphone_bat_lever());
    }    
#endif

}

static void hfp_hf_disconnected(bt_hfp_hf_session_h session, result_t err,
                                bt_app_ctx_h app_ctx)
{
    hfp_hf_app_t *app_ptr;
    hci_link_t * tmp_link = NULL;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = hfp_get_app_from_session(session);
#else
    app_ptr = &g_hfp_hf_app;
#endif
    if(0 == app_ptr)
    {
        os_printf("hfp_hf_disconnected_null_app_ptr\r\n");

        return;
    }

    bt_frontend_notification("hf disconnected %lu", app_ptr->svc_id);
    if(app_ptr->is_used) //(hfp_has_connection())
        util_sco_close(app_ptr->priv);

    app_ptr->is_used = 0;
    app_ptr->priv    = NULL;

    hf_clear_flag(app_ptr, (APP_FLAG_HFP_CONNECTION|APP_FLAG_HFP_BAT_LEVEL));

    // If ACL link exists and peer does not explicitly disconnect it,
    // allow HFP to retry 3 times
    if ((err != UWE_CONNRESET) && (err != UWE_NOLINK) && (app_ptr->svc_id <= 3))
    {
        app_ptr->svc_id  ++;
        //os_printf("hf disconnected0: svc-%lu,link:%p,addr:"BTADDR_FORMAT"\r\n", app_ptr->svc_id,tmp_link,BTADDR(&(app_ptr->raddr)));
        app_bt_active_conn_profile(PROFILE_ID_HFP, &app_ptr->raddr);
        hf_set_flag(app_ptr, APP_FLAG_DISCONN_RECONN);
    }
    else
    {        
        //os_printf("hf disconnected2\r\n");

        // For Auto/Re-conn, disconn_wrap will finish the loop and exit appropriately
        // By setting APP_FLAG_HFP_CONNECTION flag, the loop will not be blocked by non-connected HFP        
        app_bt_profile_disconn_wrap(PROFILE_ID_HFP, app_ptr);

    #ifdef BT_ONE_TO_MULTIPLE
	
	    clear_hf_flag_1toN(&app_ptr->raddr, APP_HFP_AT_CMD_FLAG1_SET | APP_HFP_AT_CMD_FLAG2_SET);//for the situation:bt disconnect while is calling
	    clear_hf_flag_1toN(&app_ptr->raddr,APP_HFP_PRIVATE_FLAG_SET);
        //clear_hf_flag_1toN(&app_ptr->raddr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);

		if(hfp_has_connection())    //if there is another hfp_connection we need to change the hf_current_app to another one
	    	hfp_select_current_app_to_another(&app_ptr->raddr);
		else
		{
            //os_printf("====current_hfp_ptr is null=====\r\n");
        	g_hf_current_app_ptr = NULL;
		}
		
        if(!has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))   //if there is no call,we set BUTTON_TYPE_NON_HFP
	    	app_button_type_set(BUTTON_TYPE_NON_HFP);

        //os_printf("hfp_hf_disconnected-> unreg: 0x%x, 0x%x\r\n", app_ptr, &g_hfp_hf_app);
        if(app_ptr != &g_hfp_hf_app)
        {
            hfp_ex_app_uninit(app_ptr);
            hfp_app_unregister(app_ptr);            
            jfree(app_ptr);
            app_ptr = NULL; 
        }
        else
    #endif
        {
//            app_ptr->flag = 0;
            app_ptr->svc_id = 0;
        }
    }

    //bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);
}

static void hfp_hf_newconn(bt_conn_req_h conn_req,
                           const btaddr_t *laddr,
                           const btaddr_t *raddr,
                           bt_link_modes_mask_t mode,
                           bt_app_ctx_h app_server_ctx)
{
    hfp_hf_app_t *app_ptr;
/*
#ifdef CONN_WITH_MUSIC
	bt_flag2_operate(APP_FLAG2_SW_MUTE,0);
#endif
*/
#ifndef BT_ONE_TO_MULTIPLE
    bt_auto_connect_stop();
#endif
#ifdef BEKEN_SPP_OTA_3266
    if(flag_ota_enable)
    {
        os_printf("hfp reject\r\n");
        bt_hfp_hf_conn_reject(conn_req);
        return;
    }
#endif

#ifdef SUPPORT_SPP_IN_MP3PLAYER
    app_handle_t app_h = app_get_sys_handler();
    if(SYS_WM_BT_MODE !=app_h->sys_work_mode)
    {
        os_printf("hfp reject\r\n");
        bt_hfp_hf_conn_reject(conn_req);
        return;
    }
#endif

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = hfp_app_lookup_valid(raddr);

    if((0 == app_ptr) && (hfp_app_get_count() < BT_MAX_AG_COUNT))
    {
        //os_printf("hfp_hf_newconn\r\n");
        app_ptr = hfp_ex_app_alloc();
        if(app_ptr)
        {
            hfp_ex_app_init(app_ptr);
            hfp_app_register(app_ptr);
        }
    }
    else
    {
        //os_printf("hfp_hf_newconn:%x\r\n", app_ptr);
    }
#else
    app_ptr = &g_hfp_hf_app;
#endif

    if((0 == app_ptr)
       || (app_ptr->is_used)
       || (app_ptr->conn_req))
    {
        bt_hfp_hf_conn_reject(conn_req);
        return;
    }

    j_memcpy((void *)&app_ptr->laddr, (void *)laddr, sizeof(btaddr_t));
    j_memcpy((void *)&app_ptr->raddr, (void *)raddr, sizeof(btaddr_t));

    if(bt_hfp_hf_conn_accept(app_ptr->session, conn_req, mode))
    {
        //os_printf("hf_newconn %lu failed\r\n", app_ptr->svc_id);
    }
    else
    {
        app_ptr->is_used = 1;
        //os_printf("hf_newconn=%lu, %x\r\n", app_ptr->svc_id, app_ptr);
    }

    return;
}

void hfp_hf_input(bt_hfp_hf_session_h session, at_result_t *at_result,
                  bt_app_ctx_h app_ctx)
{
    hfp_hf_app_t *app_ptr;

    CLEAR_SLEEP_TICK;

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = hfp_get_app_from_session(session);
#else
    app_ptr = &g_hfp_hf_app;
#endif

    hf_at_result_print(app_ptr, at_result);
}

void hfp_get_features(bt_hfp_hf_session_h session_h,
                      uint16_t *supported_features, bt_app_ctx_h app_ctx)
{
    if(supported_features)
        *supported_features = HF_SUPPORTED_FEATURES;
}

uint16_t hfp_check_features(uint16_t flag)
{
	return HF_SUPPORTED_FEATURES&flag;
}

result_t hf_backend_init(void)
{
    result_t err;

    g_hf_current_app_ptr = &g_hfp_hf_app;

    j_memset(&g_hfp_hf_app, 0, sizeof(g_hfp_hf_app));

    g_hfp_hf_app.pending_wave_id = -1;

    err = bt_hfp_hf_register(&hfp_hf_cbs);
    if(err)
    {
        goto Exit;
    }

    hf_sample_init_stage = HF_REGISTER;

    err = bt_hfp_hf_server_start(&g_hfp_hf_app.server,
                                 BTADDR_ANY,
                                 RFCOMM_CHANNEL_HFP,
                                 hfp_hf_newconn,
                                 NULL);
    if(err)
    {
        goto Exit;
    }

    hf_sample_init_stage = SERVER_START;

    err = bt_hfp_hf_conn_create(&g_hfp_hf_app.session,
                                &hfp_hf_conn_cbs,
                                NULL);
    if(err)
    {
        goto Exit;
    }
    hf_sample_init_stage = CONN_REGISTER;

#ifdef BT_ONE_TO_MULTIPLE
    hfp_app_register(&g_hfp_hf_app);
#else
    g_hfp_app_array[0] = &g_hfp_hf_app;
#endif

    os_printf("hf_backend_init:%x\r\n", g_hfp_hf_app.svc_id);
 Exit:
    if(err)
    {
        hf_backend_uninit();
    }

    return err;
}

void hf_backend_uninit(void)
{
    if(hf_sample_init_stage == CONN_REGISTER)
    {
        if(g_hfp_hf_app.conn_req)
            (void)bt_hfp_hf_conn_reject(g_hfp_hf_app.conn_req);

        bt_hfp_hf_conn_destroy(&g_hfp_hf_app.session);
        hf_sample_init_stage = BACKEND_REGISTER;
    }

    if(hf_sample_init_stage == SERVER_START)
    {
        bt_hfp_hf_server_stop(&g_hfp_hf_app.server);
        hf_sample_init_stage = HF_REGISTER;
    }

    if(hf_sample_init_stage == HF_REGISTER)
    {
        bt_hfp_hf_unregister();
        hf_sample_init_stage = UNINIT;
    }

#ifdef BT_ONE_TO_MULTIPLE
    hfp_app_unregister(&g_hfp_hf_app);

    {
        uint32_t i;
        hfp_hf_app_t *app_ptr;

        for(i = 0; i < BT_MAX_AG_COUNT; i ++)
        {
            if((g_hfp_app_array[i])
               && (&g_hfp_hf_app != g_hfp_app_array[i]))
            {
                app_ptr = g_hfp_app_array[i];

                hfp_app_unregister(app_ptr);
                hfp_ex_app_uninit(app_ptr);
                hfp_ex_app_free(app_ptr);
            }
        }
    }
#endif
}


result_t hf_cmd_connect(char *params, unsigned int len)
{
    result_t err;
    btaddr_t laddr, raddr;
    int btaddr_tmp[6];
    uint32_t channel;
    uint32_t unit_id;
    hfp_hf_app_t *app_ptr;

    os_printf("hf_cmd_connect\r\n");

    if(j_snscanf(params,
                 NULL,
                 "%u " BTADDR_FORMAT " %u",
                 &unit_id,
                 BTADDR_TMP(btaddr_tmp),
                 &channel) != 8)
    {
        err = UWE_INVAL;
        goto Exit;
    }

    TMP_TO_BTADDR(btaddr_tmp, &raddr);

    if(hfp_is_connection_based_on_raddr(&raddr))
    {
        //os_printf("hf_connected_raddr, UWE_ALREADY!! \r\n");
        return UWE_ALREADY;
    }

#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = hfp_app_lookup_valid(&raddr);
    //os_printf("hf_app_ptr: 0x%x, %d \r\n", app_ptr, hfp_app_get_count());
    if((0 == app_ptr)
       && (hfp_app_get_count() < BT_MAX_AG_COUNT))
    {
        app_ptr = hfp_ex_app_alloc();
        if(app_ptr)
        {
            hfp_ex_app_init(app_ptr);
            hfp_app_register(app_ptr);
        }
    }
#else
    app_ptr = &g_hfp_hf_app;
#endif

    if((app_ptr) && (app_ptr->is_used))
    {
        err = UWE_ALREADY;
        //os_printf("exit1, %d \r\n", err);
        goto Exit;
    }

    err = backend_unit_get_addr(unit_id, &laddr);
    if (err)
    {
        //os_printf("exit2, %d \r\n", err);
        err = UWE_NODEV;
        goto Exit;
    }

    j_memcpy((void *)&app_ptr->laddr, (void *)&laddr, sizeof(btaddr_t));
    j_memcpy((void *)&app_ptr->raddr, (void *)&raddr, sizeof(btaddr_t));

    err = bt_hfp_hf_conn_connect(app_ptr->session,
                                 &laddr,
                                 &raddr,
                                 (uint8_t)channel);
    if (err)
    {
        //os_printf("exit3, %d \r\n", err);
        goto Exit;
    }

    app_ptr->is_used = 1;

    //os_printf("svc_id:%lu\r\n", app_ptr->svc_id);

 Exit:
    return err;
}

result_t hf_cmd_disconnect(void)
{
    if(!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;

    return bt_hfp_hf_conn_disconnect(g_hf_current_app_ptr->session);
}

result_t hf_cmd_accept_call(void)
{
    result_t ret = 0;

    if (!g_hf_current_app_ptr)
        return UWE_NODEV;

    g_hf_current_app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_ACK;
    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    ret = bt_hfp_hf_accept_call(g_hf_current_app_ptr->session);

    g_hf_current_app_ptr->flag_establish_call = 1;
    return ret;
}

result_t hf_cmd_hangup_call(int8_t reason) // 0-- cancel , 1-- reject
{
     if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
     
    //bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
    //bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);

    app_bt_sco_enable_wrap(0);
    //app_button_type_set(BUTTON_TYPE_NON_HFP);

    if(reason == 0)
    {
        g_hf_current_app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_CANCEL;
    }
    else if(reason == 1)
    {
        g_hf_current_app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_REJECT;
    }
    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    return bt_hfp_hf_hangup_call(g_hf_current_app_ptr->session);
}

result_t hf_cmd_place_call(char number[32])
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    return bt_hfp_hf_place_call(g_hf_current_app_ptr->session, number);
}

result_t hf_cmd_redial(void)
{
    result_t ret;
#if 0
#if defined (BT_ONE_TO_MULTIPLE) &&  !defined(BT_ONLY_ONE_BT)
    if(a2dp_has_music() && !has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED|APP_FLAG_SCO_CONNECTION))
        change_cur_hfp_based_on_addr(a2dp_get_current_app_remote_addr());
#endif
#endif

    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    g_hf_current_app_ptr->pending_wave_id = APP_WAVE_FILE_ID_REDIAL;

    ret = bt_hfp_hf_redial(g_hf_current_app_ptr->session);

    g_hf_current_app_ptr->flag_establish_call = 1;
    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    return ret;
}

result_t hf_cmd_resp_and_hold(uint32_t command)
{
    result_t rc = UWE_PARAM;

    switch (command)
    {
    case AT_BTRH_HOLD_INCOMING:
    case AT_BTRH_ACCEPT_HELD:
    case AT_BTRH_REJECT_HELD:
        rc = bt_hfp_hf_resp_and_hold_set(g_hfp_hf_app.session, command);
        break;

    default:
        /* TODO: send error notification */
        break;
    }

    return rc;
}

result_t hf_cmd_resp_and_hold_read(void)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;

    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    return bt_hfp_hf_resp_and_hold_read(g_hf_current_app_ptr->session);
}

result_t hf_cmd_set_call_wait_notif(uint32_t enable)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;  
    
    if(enable > 1)
        return UWE_PARAM;
    hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
    return bt_hfp_hf_set_call_wait_notif(g_hf_current_app_ptr->session, (uint8_t)enable);
}

result_t hf_cmd_chld(uint32_t command, uint32_t call_id)
{
    result_t rc = UWE_OK;

    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;

    switch (command)
    {
    case AT_CHLD_RELEASE_ALL_ACTIVE_ACCEPT_HELD:
    case AT_CHLD_HOLD_ALL_EXCEPT_INDICATED:
        /* Also catches the following cases */
        /* case AT_CHLD_RELEASE_INDICATED_ACTIVE: */
        /* case AT_CHLD_HOLD_ALL_ACTIVE_ACCEPT_HELD: */
        break;

    case AT_CHLD_RELEASE_ALL_HELD:
    case AT_CHLD_MULTIPARTY:
    case AT_CHLD_EXPLICIT_CALL_TRANSFER:
        break;

    default:
        rc = UWE_PARAM;
        /* TODO: send error notification */
        break;
    }

    if(!rc)
    {
        rc = bt_hfp_hf_chld(g_hf_current_app_ptr->session, (uint8_t)command,
                            (uint8_t)call_id);
    }
    return rc;
}

result_t hf_cmd_list_current_calls(void)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_list_current_calls(g_hf_current_app_ptr->session);
}

result_t hf_cmd_set_cli_notif(uint32_t enable)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_set_cli_notif(g_hf_current_app_ptr->session, enable);
}

boolean is_ag_support_feature(uint8 feature) {
    //os_printf("is_ag_support_feature, 0x%x, 0x%x\r\n",g_hf_current_app_ptr,feature);

    return g_hf_current_app_ptr 
        && bt_is_ag_support_feature(g_hf_current_app_ptr->session,feature);
}

/*********************************
* function:hf_cmd_set_voice_recog
* enable: 
* 1 ->  open voice_recog in ag
* 0 -> close voice_recog in ag
*********************************/
result_t hf_cmd_set_voice_recog(uint32_t enable) {
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV; 
    
    //if(enable) {
    //    app_wave_file_play_start(APP_WAVE_FILE_ID_HFP_VOICE_DIAL);
    //}
        
    return bt_hfp_hf_set_voice_recog(g_hf_current_app_ptr->session, enable);
}

result_t hf_cmd_trans_DTMF(char * code)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_trans_DTMF(g_hf_current_app_ptr->session, code);
}


result_t hf_cmd_get_operator(void)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_get_operator(g_hf_current_app_ptr->session);
}

result_t hf_cmd_get_subscriber(void)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_get_subscriber(g_hf_current_app_ptr->session);
}

result_t hf_cmd_set_iphone_batlevel(int8 level)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
#ifdef BT_ONE_TO_MULTIPLE
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i])
        	&&(g_hfp_app_array[i]->flag & APP_FLAG_HFP_CONNECTION)
        	&&(g_hfp_app_array[i]->flag & APP_FLAG_HFP_BAT_LEVEL)
        	)
        {
            bt_hfp_hf_iphoneaccev(g_hfp_app_array[i]->session, level);
        }
    }
    return UWE_OK;
#else
	if((g_hf_current_app_ptr->flag & APP_FLAG_HFP_CONNECTION)
		&& (g_hf_current_app_ptr->flag & APP_FLAG_HFP_BAT_LEVEL)
	  )
	{
    	bt_hfp_hf_iphoneaccev(g_hf_current_app_ptr->session, level);
    }
	return UWE_OK;
#endif
}

void hf_volume_init(int8_t aud_volume)
{
    if (!g_hf_current_app_ptr)
        return;
    
    g_hf_current_app_ptr->volume = aud_volume;

    if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
    {
        aud_volume_set(g_hf_current_app_ptr->volume);
    }
}

int8_t get_current_hfp_volume(void)
{
    if (!g_hf_current_app_ptr)
        return 0;
	return g_hf_current_app_ptr->volume;
}
uint32_t get_current_hfp_freq(void)
{
    if (!g_hf_current_app_ptr)
        return 0;
    return g_hf_current_app_ptr->freq;
}
void *get_current_hfp_ptr(void)
{
    return  (void *)g_hf_current_app_ptr;
}

int8_t hf_get_volume(void)
{
	 return g_hf_current_app_ptr->volume;
}

result_t hf_cmd_set_vgs(uint8_t oper)
{
    int8_t volume;

    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;

    if(oper == 0) //minus
    {
        g_hf_current_app_ptr->volume--;
        if(g_hf_current_app_ptr->volume <= AUDIO_VOLUME_MIN)
            g_hf_current_app_ptr->volume = AUDIO_VOLUME_MIN;
    }
    else  //plus
    {
        g_hf_current_app_ptr->volume++;
        if(g_hf_current_app_ptr->volume >= AUDIO_VOLUME_MAX)
            g_hf_current_app_ptr->volume = AUDIO_VOLUME_MAX;
    }

    volume = g_hf_current_app_ptr->volume;
    //os_printf("hf vol:%d\r\n",volume);

    if(volume <= AUDIO_VOLUME_MIN)// && (!app_wave_playing()))
    {
        bt_flag2_operate(APP_FLAG2_VOL_MUTE, 1);
        if(!app_wave_playing())
        {
            aud_PAmute_oper(1);
            aud_volume_mute(1);
        }
    }
    else
    {
        bt_flag2_operate(APP_FLAG2_VOL_MUTE, 0);
        aud_volume_mute(0);
        aud_PAmute_oper(0);

    }
    aud_volume_set(volume);

    app_wave_file_vol_s_init(volume);//for wave file play over to restore the changed volume

    if(volume == AUDIO_VOLUME_MAX) // max
    {
        //os_printf("max2\r\n");
        app_wave_file_play_start(APP_WAVE_FILE_ID_VOL_MAX);
    }
    else if(volume == AUDIO_VOLUME_MIN) // min
    {
        //os_printf("min2\r\n");
        app_wave_file_play_start(APP_WAVE_FILE_ID_VOL_MIN);
    }

//#ifdef HFP_VOL_SET_AG
    //hf_set_flag(g_hf_current_app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING); 
    /*peer only reponse AT_RESULT_OK with below AT_CMD*/
    if(volume <= 0)
        return bt_hfp_hf_vgs(g_hf_current_app_ptr->session, 0);
    else
        return bt_hfp_hf_vgs(g_hf_current_app_ptr->session, g_hf_current_app_ptr->volume-1);
//#endif

    return UWE_OK;
}

void hf_audio_conf_restore(int mode)
{
    if (!g_hf_current_app_ptr)
        return;
    if(BT_MAX_AG_COUNT > 1)
    {
        hfp_hf_app_t *app_ptr = NULL;
        app_ptr = (hfp_hf_app_t *)get_hfp_from_active_sco_flag();
        if(!app_ptr)
            return;
        if(mode == 1)
        {
    		aud_close();

    		hf_volume_init(app_ptr->volume);

#ifdef CONFIG_DRIVER_I2S
            aud_initial(48000, 1, 16);
#else
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            aud_initial(app_ptr->freq, 2, 16);
#else
            aud_initial(app_ptr->freq, 1, 16);
#endif
#endif
        }

#ifdef CONFIG_DRIVER_I2S
        app_wave_file_aud_notify(48000, 1, app_ptr->volume);
#else
        app_wave_file_aud_notify(app_ptr->freq, 1, app_ptr->volume);
#endif

    }
    else
    {
        if(mode == 1)
        {
            aud_close();

            hf_volume_init(g_hf_current_app_ptr->volume);

        #ifdef CONFIG_DRIVER_I2S
            aud_initial(48000, 1, 16);
        #else
        #if (CONFIG_AUDIO_TRANSFER_DMA == 1)
            aud_initial(g_hf_current_app_ptr->freq, 2, 16);
        #else
            aud_initial(g_hf_current_app_ptr->freq, 1, 16);
        #endif
        #endif
        }

    #ifdef CONFIG_DRIVER_I2S
        app_wave_file_aud_notify(48000, 1, g_hf_current_app_ptr->volume);
    #else
        app_wave_file_aud_notify(g_hf_current_app_ptr->freq, 1, g_hf_current_app_ptr->volume);
    #endif
    }
#if(CONFIG_AUD_FADE_IN_OUT == 1)
    set_aud_fade_in_out_state(AUD_FADE_IN);
#endif
}
#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
void app_bt_incoming_ring_ios( void *arg )
{
	if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
    app_wave_file_play_start(APP_WAVE_FILE_ID_HFP_RING);

    if(!bt_flag1_is_set(APP_FLAG_CALL_ESTABLISHED))
    {
        app_bt_shedule_task((jthread_func)app_bt_incoming_ring_ios,
    			(void *)arg,
    			(uint32)arg);
    }
}

#endif

static void hf_at_result_print(hfp_hf_app_t *app_ptr, const at_result_t *at_result)
{
    app_env_handle_t env_h = app_env_get_handle();
#ifdef BT_ONE_TO_MULTIPLE
    app_handle_t sys_hdl = app_get_sys_handler();
#endif
    if(bt_flag2_is_set(APP_FLAG2_CALL_PROCESS)&&(app_ptr != g_hf_current_app_ptr))
    {
        os_printf("=====not current hf:%p\r\n",app_ptr);
    }
    switch (at_result->code)
    {
    case AT_RESULT_RING:
        bt_frontend_notification("AG ringing ...");
        bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 1);
	 app_button_type_set(BUTTON_TYPE_HFP);
	 bt_flag1_operate(APP_FLAG_HFP_CALLSETUP, 1);
#if ( BT_MODE==BT_MODE_1V2)
        set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG2_HFP_INCOMING);
        set_hf_flag_call_seq(&app_ptr->raddr);  
        os_printf("~~~call num:%d,%d,current call seq:0x%x,ring count:%d\r\n",get_2hfps_sco_and_incoming_flag(),has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED),get_hfp_flag_1toN(&app_ptr->raddr,APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1 |APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2),app_ptr->ring_count);
#else
        hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG2_HFP_INCOMING);
#endif
        app_ptr->ring_count++; 
#if 1
#if ( BT_MODE==BT_MODE_1V2)
        if ((!get_2hfps_sco_and_incoming_flag()) &&
            (((has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)>1)
            && (get_hfp_flag_1toN(&app_ptr->raddr, APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2)))
            || (has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)<=1))
            )
#endif
        {
            if((app_ptr->ring_count >= 2) && ( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL ) )
            {
                if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
                {
                    if((app_ptr->vnum[0] != 0xFF) && (app_ptr->vnum[0] != 0x00))
                    {
                        int i = 0;

                        while(app_ptr->vnum[i] < '0' || app_ptr->vnum[i] > '9')
                        {
                            i++;
                        }

                        app_bt_shedule_task((jthread_func)app_wave_file_play_start
                                            , (void *)(APP_WAVE_FILE_ID_VOICE_NUM_0+(app_ptr->vnum[i]-'0'))
                                            , 200);
                        app_wave_file_voice_num_set((char *)&app_ptr->vnum[i+1]);                        
                    } 
                    //app_ptr->ring_count = 0;
                }
            }
            else
            {
              // if has sco, phone's ring will transfer to handset
                if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING) )
                {
				#if 1//(CONFIG_APP_TOOLKIT_5 == 1)
					if((app_check_bt_mode(BT_MODE_1V2)&&(!hfp_has_the_sco_conn(app_ptr->raddr)))
						||(!app_check_bt_mode(BT_MODE_1V2)&&!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
						)
					{
						app_bt_shedule_task((jthread_func)app_wave_file_play_start,
									(void *)APP_WAVE_FILE_ID_HFP_RING,
									200);
					}
					else if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_DISABLE_IOS_INCOMING_RING))
					{
						app_bt_shedule_task((jthread_func)app_bt_incoming_ring_ios,
											(void *)7000,
											200);
					}
				#else
					#if (BT_MODE == BT_MODE_1V2)
					if(!hfp_has_the_sco_conn(app_ptr->raddr))
					#else
					if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
					#endif
					{
						app_bt_shedule_task((jthread_func)app_wave_file_play_start,
									(void *)APP_WAVE_FILE_ID_HFP_RING,
									200);

                	}
				#endif
                }
            }
            
        }
        
#else
        if((app_ptr->ring_count >= 4) /*&& ( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL )*/ )
        {
             if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
            {
                if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
                {
                    if((app_ptr->vnum[0] != 0xFF) && (app_ptr->vnum[0] != 0x00))
                    {
                        app_wave_file_voice_num_set((char *)app_ptr->vnum);
                    }

                    app_bt_shedule_task((jthread_func)app_wave_file_play_start,
                                        (void *)APP_WAVE_FILE_ID_HFP_RING,
                                        200);
                }
                else
                {
                    if((app_ptr->vnum[0] != 0xFF) && (app_ptr->vnum[0] != 0x00))
                    {
                        int i = 0;

                        while(app_ptr->vnum[i] < '0' || app_ptr->vnum[i] > '9')
                        {
                            i++;
                        }

                        if( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL )
                        {

                                app_bt_shedule_task((jthread_func)app_wave_file_play_start
                                            , (void *)(APP_WAVE_FILE_ID_VOICE_NUM_0+(app_ptr->vnum[i]-'0'))
                                            , 200);
                        }

                        app_wave_file_voice_num_set((char *)&app_ptr->vnum[i+1]);
                    }
                }
                app_ptr->ring_count = 0;
            }
        }
        else
        {
            if(!bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
            {
                if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
                {
//                    app_wave_file_voice_num_set(NULL);
                    app_bt_shedule_task((jthread_func)app_wave_file_play_start,
                                        (void *)APP_WAVE_FILE_ID_HFP_RING,
                                        200);
                }
            }
        }
#endif
        app_set_led_event_action( LED_EVENT_INCOMING_RINGING );
        break;
    case AT_RESULT_BCS:
        os_printf("===bcs:%d\r\n",at_result->u.bcs);
    #ifdef CONFIG_BLUETOOTH_COEXIST
        app_coexist_play_pause(0);
    #endif
        if(at_result->u.bcs == 2)
        {
            bt_hfp_hf_set_codec(app_ptr->session,2);
            app_ptr->freq = 16000;
        }
        else
        {
            //hf_clear_flag(app_ptr, APP_HFP_PRIVATE_FLAG_CODEC_NEGOTIATION);
            bt_hfp_hf_set_codec(app_ptr->session,1);
            app_ptr->freq = 8000;
        }
        break;
#ifdef CONFIG_PRODUCT_TEST_INF
    case AT_RESULT_BKCMD:
        if(at_result->u.bkcmd == 1)
        {
            uint32 val = 0;
            val = 0x40000000 | (uint32)( (aver_rssi&0xff)<<16) | ((uint32)aver_offset&0x1ff);
            hf_cmd_trans_buttoncode(val);
            os_printf("hf_cmd_trans_buttoncode:0x%x,aver_rssi:%x,offset:%x\r\n",val,aver_rssi,aver_offset);
        }
        break;
#endif
    case AT_RESULT_VGS:
        bt_frontend_notification("AG vol:%d",at_result->u.vgs);
        app_ptr->volume = at_result->u.vgs;
        aud_volume_set(app_ptr->volume);
		
		/*why do this?*/
        /*if(at_result->u.vgs >= 11)
            aud_volume_set(11);
        else
        {
            aud_volume_set(at_result->u.vgs);
            env_h->env_cfg.system_para.vol_hfp = at_result->u.vgs;
        }*/
        break;

    case AT_RESULT_VGM:
        bt_frontend_notification("AG microphone volume changed %d",at_result->u.vgm);
        aud_mic_volume_set(at_result->u.vgm);
        break;

    case AT_RESULT_BSIR:
        bt_frontend_notification("in-band ringtone is%s audible",at_result->u.bsir ? "" : " not");
        break;

    case AT_RESULT_BTRH:
        bt_frontend_notification("AG Response and Hold status - %s",
                                 at_result->u.btrh == AT_BTRH_HOLD_INCOMING ?  "call on hold" :
                                 at_result->u.btrh == AT_BTRH_ACCEPT_HELD ?  "held call accepted" :
                                 "held call rejected");
        break;

    case AT_RESULT_BVRA:
        {
            //os_printf("<<>> hf_at_result_print, u.bvra=%d\r\n",at_result->u.bvra);
            if(at_result->u.bvra == 0)
            {
                if(bt_flag1_is_set(APP_FLAG_MUSIC_PLAY_SCHEDULE))
                {
                    bt_flag1_operate(APP_FLAG_MUSIC_PLAY_SCHEDULE, 0);
                    //app_button_sw_action(BUTTON_BT_PLAY_PAUSE);
    		      app_button_type_set(BUTTON_TYPE_NON_HFP);
                }
            }
            break;
        }

    case AT_RESULT_CIEV:
        {
            const at_result_ciev_t *ciev = &(at_result->u.ciev);

            //bt_frontend_notification("AG indicator %s=%d",
            //    ag_cind_2_str(ciev->cind_code), ciev->val);

            if(ciev->cind_code == CIND_CALL)
            {
                if(ciev->val == 1)
                {
                    os_printf("ciev1\r\n");
                    app_bt_shedule_task(NULL,NULL,0);
                    app_wave_file_voice_num_set(NULL);
                    aud_PAmute_oper(0);
                    /*
                    bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
                    bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);
                    */
                    app_set_led_event_action( LED_EVENT_CALL_ESTABLISHMENT);
                    app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_ACK;
                    bt_flag1_operate(APP_FLAG_HFP_CALLSETUP, 1);
                    bt_flag1_operate(APP_FLAG_CALL_ESTABLISHED,1);
                #ifdef BT_ONE_TO_MULTIPLE
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED);
                    if(get_2hfps_sco_and_incoming_flag())
                    {
                        set_2hfps_sco_and_incoming_flag(0);
                        if(!get_hfp_flag_1toN(&app_ptr->raddr,APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE))
                        {
                            hfp_2eSCOs_A_B_SWAP();
                        }
                    }
                    //else
                    {
                        check_2hfps_sco_and_incoming_status();
                    }
                    //os_printf("===2hfps-2:%d\r\n",get_2hfps_sco_and_incoming_flag());
                    if(!has_hfp_flag_1toN(APP_FLAG_HFP_OUTGOING))
                    {
                         bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);    
                    }
                    if(!has_hfp_flag_1toN(APP_FLAG2_HFP_INCOMING))
                    {
                         bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);    
                    }
                    bt_select_current_hfp(app_ptr);
                #if (CONFIG_CUSTOMER_2PHONES_HUNG_ACCETP == 1)
                    if (app_get_2phones_hung_accetp())
                    {
                        app_set_2phones_hung_accetp(0);
                        hf_sco_disconn_reconn(1);
                    }
                #endif
                #else
                    hf_clear_flag(g_hf_current_app_ptr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED);
                    bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
                    bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);
                #endif
                    /* For some meizu/meilan/huawei mobile phones,which not support CIND_READ command */
                    app_button_type_set(BUTTON_TYPE_HFP);//re-enable hfp_button.
                    os_printf(APP_RET_STATUS_CALL_EST);
                }
                else
                {
                    os_printf("ciev0\r\n");
                    aud_PAmute_oper(1);
		      app_wave_file_voice_num_set(NULL);
		      //app_wave_file_play_stop();
                    bt_flag1_operate(APP_FLAG_HFP_CALLSETUP, 0);
                    bt_flag1_operate(APP_FLAG_CALL_ESTABLISHED,0);
                    app_ptr->ring_num=0;
                    /*
                    bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
                    bt_flag2_operate(APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS, 0);
                    */
                    bt_flag2_operate(APP_FLAG2_CALL_PROCESS, 0);
                    //stop_sco_test_call(app_ptr->priv);

                    app_ptr->vnum[0] = 0;
                    app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_CANCEL;
                    app_ptr->flag_establish_call = 0;

                #if ( BT_MODE==BT_MODE_1V2)
                    if(get_2hfps_sco_and_incoming_flag())
                    {
                        set_2hfps_sco_and_incoming_flag(0);
                        app_ptr->pending_wave_id = APP_WAVE_FILE_ID_NULL;
                    }
                    else
                    {
                        app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_CANCEL;
                    }
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED|APP_FLAG2_CALL_PROCESS);
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2);
                    if(!has_hfp_flag_1toN(APP_FLAG_HFP_OUTGOING))
                    {
                         bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);    
                    }
                    if(!has_hfp_flag_1toN(APP_FLAG2_HFP_INCOMING))
                    {
                         bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);    
                    }
                    // otm_current_hfp_call_over_handler(app_ptr);
                    /* select the device which is playing music,but not be flow controlled */
                    select_play_a2dp_avrcp((void *)app_ptr);
//                    app_bt_reconn_after_callEnd();
                    if(has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))//if there is another call,we change the hf_curr_ptr to another one
                    {
                         change_cur_hfp_to_another_call_conn(&app_ptr->raddr);
		      }
                    else
                    {
                        app_button_type_set(BUTTON_TYPE_NON_HFP);
                    }
                    if (!has_hfp_flag_1toN(APP_FLAG_CALL_ESTABLISHED))
                    {
    				#ifdef CONFIG_BLUETOOTH_COEXIST
					   if(app_is_bt_mode())
					#endif
                        	app_set_led_event_action( LED_EVENT_CONN );
                    }
                #else
		      /*  comment by lianxue.liu ??????????????***/
		      /*
                    if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
                        g_stream_start_handler();
		      */
                    hf_clear_flag(g_hf_current_app_ptr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    hf_clear_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED|APP_FLAG2_CALL_PROCESS);
                    bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
                    bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);
                    app_button_type_set(BUTTON_TYPE_NON_HFP);
						
					#ifdef CONFIG_BLUETOOTH_COEXIST
					if(app_is_bt_mode())
					#endif
		      			app_set_led_event_action( LED_EVENT_CONN );
                #endif
                #ifdef CONFIG_BLUETOOTH_COEXIST
                    app_coexist_play_pause(1);
                #endif
                }
            }
            else if(ciev->cind_code == CIND_CALLSETUP)
            {
                if(ciev->val == 1)  // incoming call
                {
                #ifdef CONFIG_BLUETOOTH_COEXIST
                    app_coexist_play_pause(0);
                #endif
                    os_printf("===CALLSETUP=1\r\n");
                #ifdef BT_ONE_TO_MULTIPLE
                    if(!get_hfp_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
                #else   
                    if(!bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))
                #endif
                    {
                    #ifdef BT_ONE_TO_MULTIPLE
                        jtask_stop(sys_hdl->app_auto_con_task);
                        set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS);
                        check_2hfps_sco_and_incoming_status();
                        bt_select_current_hfp(app_ptr);
                        //hfpcall_select_a2dp(app_ptr);
                    #else

                        hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS);
                    #endif
                        app_ptr->ring_count = 0;
                        //app_button_type_set(BUTTON_TYPE_NON_HFP);
    		          app_button_type_set(BUTTON_TYPE_HFP);
                        app_ptr->dir = 0;
                        bt_flag1_operate(APP_FLAG_HFP_CALLSETUP, 1);
                    	   bt_flag2_operate(APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS, 1);
                        os_printf(APP_RET_STATUS_INCOMING);
                        hf_cmd_list_current_calls();
                    }
                    else
                    {
                        hf_cmd_list_current_calls();
                        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,1);
                        app_button_type_set(APP_BUTTON_TYPE_TWC);
                    }
                }
                else if(ciev->val == 2) //outgoing call
                {
                #ifdef CONFIG_BLUETOOTH_COEXIST
                    app_coexist_play_pause(0);
                #endif
                    os_printf("===CALLSETUP=2\r\n");
                    //app_button_type_set(BUTTON_TYPE_NON_HFP);
		      app_button_type_set(BUTTON_TYPE_HFP);
                    aud_PAmute_oper(0);
                    app_ptr->dir = 1;
                    os_printf(APP_RET_STATUS_OUTGOING);
                    set_2hfps_sco_and_incoming_flag(0);

                    //app_set_led_event_action( LED_EVENT_OUTGOING_RINGING );
		      bt_flag1_operate(APP_FLAG_HFP_CALLSETUP, 1);
					app_set_led_event_action( LED_EVENT_OUTGOING_RINGING );

                #if ( BT_MODE==BT_MODE_1V2)
                    jtask_stop(sys_hdl->app_auto_con_task);

                    // bt_deselect_other_hfp(app_ptr);
                    bt_select_current_hfp(app_ptr);
                    //hfpcall_select_a2dp(app_ptr);
                    bt_flag2_operate(APP_FLAG2_CALL_PROCESS, 1);
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING|APP_FLAG2_CALL_PROCESS);
                    set_hf_flag_call_seq(&app_ptr->raddr);  
                #else
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING|APP_FLAG2_CALL_PROCESS);
                #endif
                    bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 1);
                    //hf_cmd_list_current_calls();
                }
                else if(ciev->val == 0)  // call setup = 0
                {
                    bt_flag1_operate(APP_FLAG_HFP_CALLSETUP,0);
   		      //app_button_type_set(BUTTON_TYPE_HFP);
                #ifdef BT_ONE_TO_MULTIPLE
                    os_printf("===CALLSETUP=0\r\n");
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP);
                #else
                    hf_clear_flag(g_hf_current_app_ptr, APP_FLAG_HFP_CALLSETUP);          
                #endif                    

                #ifdef BT_ONE_TO_MULTIPLE
                    if(!get_hfp_flag_1toN(&app_ptr->raddr,APP_FLAG_CALL_ESTABLISHED))
                #else
                    if(!bt_flag1_is_set(APP_FLAG_CALL_ESTABLISHED))
                #endif
                    {
                        bt_flag1_operate(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED, 0);
                    #if ( BT_MODE==BT_MODE_1V2)
                        clear_hf_flag_1toN(&app_ptr->raddr,APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2);
                    #endif
                        aud_PAmute_oper(1);

                        if (!bt_flag1_is_set(APP_FLAG_AUTO_CONNECTION|APP_FLAG_RECONNCTION))
                            app_bt_shedule_task(NULL,NULL,0);

                        app_wave_file_voice_num_set(NULL);
                        //app_wave_file_play_stop();

                        app_ptr->vnum[0] = 0;
                        //app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_REJECT;

                    #ifdef BT_ONE_TO_MULTIPLE
                        if(get_hfp_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING))
                    #else
                        if(bt_flag1_is_set(APP_FLAG_HFP_OUTGOING)
                           || bt_flag2_is_set(APP_FLAG2_HFP_INCOMING))
                    #endif
                        {
                            //app_ptr->pending_wave_id = APP_WAVE_FILE_ID_HFP_REJECT;
                            set_2hfps_sco_and_incoming_flag(0);
                        }

                        //bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
                        bt_flag2_operate(APP_FLAG2_CALL_PROCESS, 0);


                    #ifdef BT_ONE_TO_MULTIPLE
                        clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS);
                        if(!has_hfp_flag_1toN(APP_FLAG_HFP_OUTGOING))
                        {
                             bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);    
                        }
                        if(!has_hfp_flag_1toN(APP_FLAG2_HFP_INCOMING))
                        {
                             bt_flag2_operate(APP_FLAG2_HFP_INCOMING, 0);    
                        }

                        // otm_current_hfp_call_over_handler(app_ptr);
						/* select the device which is playing music,but not be flow controlled */
                        select_play_a2dp_avrcp((void *)app_ptr);
//                        app_bt_reconn_after_callEnd();
                        if(has_hfp_flag_1toN(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED))//if there is another call,we change the hf_curr_ptr to another one
                        {
                           change_cur_hfp_to_another_call_conn(&app_ptr->raddr);
                        }
                        else
                        {
                           app_button_type_set(BUTTON_TYPE_NON_HFP);
                        }

                        if (!has_hfp_flag_1toN(APP_FLAG_CALL_ESTABLISHED))
                        {
                        	#ifdef CONFIG_BLUETOOTH_COEXIST
					   		if(app_is_bt_mode())
							#endif
                        		app_set_led_event_action( LED_EVENT_CONN );
                        }
                    #else
                        /*  comment by lianxue.liu ??????????????***/

                        /*
                        if(!bt_flag1_is_set(APP_FLAG_SCO_CONNECTION))
                            g_stream_start_handler();
                        */
                        hf_clear_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING|APP_FLAG2_CALL_PROCESS);
                        bt_flag1_operate(APP_FLAG_HFP_OUTGOING, 0);
			   app_button_type_set(BUTTON_TYPE_NON_HFP);
			#ifdef CONFIG_BLUETOOTH_COEXIST
				  if(app_is_bt_mode())
			#endif
			   		app_set_led_event_action( LED_EVENT_CONN );
                    #endif
                    #ifdef CONFIG_BLUETOOTH_COEXIST
                        app_coexist_play_pause(1);
                    #endif
                    }
                    else // current hfp has APP_FLAG_CALL_ESTABLISHED,Maybe this hfp is TWC state before callsetup = 0
                    {
                        os_printf("===clr twc state\r\n");
                        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,0);
                        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,0);
                        bt_flag2_operate(APP_FLAG2_HFP_SECOND_CALLSETUP, 0 );
                        app_button_type_set(BUTTON_TYPE_HFP);
                    }
                }
                else if(ciev->val == 3)
                {
                #ifdef CONFIG_BLUETOOTH_COEXIST
                    app_coexist_play_pause(0);
                #endif
                    os_printf("===CALLSETUP=3\r\n");
                    /* For some meizu/meilan/huawei mobile phones,which not support CIND_READ command */
                    bt_flag1_operate( APP_FLAG_HFP_CALLSETUP, 1 );
                #ifdef BT_ONE_TO_MULTIPLE
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING);
                #else
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP|APP_FLAG_HFP_OUTGOING);
                #endif
                    app_button_type_set(BUTTON_TYPE_HFP);//re-enable hfp_button.
                }
                
            }
            else if( ciev->cind_code == CIND_CALLHELD )
            {
/*******************************************************
                0= No calls held
                1= Call is placed on hold or active/held calls swapped (The AG has both an active AND a held call)
                2= Call on hold, no active call
*******************************************************/
                os_printf("===CIND CALLHELD:%d\r\n",ciev->val);
                if( ciev->val == 1 )
                {
                    app_button_type_set(BUTTON_TYPE_TWC);
                    set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,1);
                    set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,1);
                    bt_flag2_operate( APP_FLAG2_HFP_SECOND_CALLSETUP, 1 );
                }
                else if(ciev->val == 2)
                {
                    if(!get_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE))
                    {
                        os_printf("===AT chld:2\r\n");
                        //hf_cmd_chld(2, -1);
                    }
                    //app_button_type_set(BUTTON_TYPE_HFP);
                    //bt_flag2_operate( APP_FLAG2_HFP_SECOND_CALLSETUP, 0 );
                }
                else
                {
                    set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,0);
                    set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,0);
                    app_button_type_set(BUTTON_TYPE_HFP);
                    bt_flag2_operate( APP_FLAG2_HFP_SECOND_CALLSETUP, 0 );
                }
            }

            if(app_ptr->pending_wave_id >= 0)
            {
                //if(!a2dp_has_music())
                {
                    app_wave_file_play_start(app_ptr->pending_wave_id);
                }

                app_ptr->pending_wave_id = -1;
            }
            break;
        }

    case AT_RESULT_CLCC:
        {
            const at_result_clcc_t *clcc = &(at_result->u.clcc);

            bt_frontend_notification("AG current calls %d, %d, %d, %d, %d, %s,"
                                     " %d, %s, %d", clcc->call_id, clcc->dir, clcc->stat,
                                     clcc->mode, clcc->mpty, clcc->number, clcc->type, clcc->alpha,
                                     clcc->priority);

            if (bt_flag1_is_set(APP_FLAG_CALL_ESTABLISHED))
                break;
			
            if(app_ptr->dir)
                os_printf(APP_RET_STATUS_OCALLNUMBER, clcc->number);
            else if( env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_ADDR_AUDIO_DIAL )
            {
                os_printf(APP_RET_STATUS_ICALLNUMBER, clcc->number);
                memcpy(app_ptr->vnum, clcc->number, 32);

                app_ptr->ring_count = 0;
                app_ptr->ring_num   = 1;
                app_wave_file_voice_num_set((char *)clcc->number);
//                app_bt_shedule_task((jthread_func)app_wave_file_play_start
//                                    , (void *)APP_WAVE_FILE_ID_HFP_RING, 200);
            }
            break;
        }

    case AT_RESULT_CLIP:
        {
            const at_result_clip_t *clip = &at_result->u.clip;

            bt_frontend_notification("AG calling line identification %s, %d, "
                                     "%s, %d, %s, %d", clip->number, clip->type, clip->subaddr,
                                     clip->satype, clip->alpha, clip->cli_validity);

			if (bt_flag1_is_set(APP_FLAG_CALL_ESTABLISHED))
				break;
			
            //if(0)//(app_ptr->ring_num==0)
            if(app_ptr->vnum[0] == 0)
            {
                os_printf(APP_RET_STATUS_ICALLNUMBER, clip->number);

                memcpy(app_ptr->vnum, clip->number, 32);
                app_ptr->ring_count = 0;
                app_ptr->ring_num = 2;

                app_wave_file_voice_num_set((char *)clip->number);
//                app_bt_shedule_task((jthread_func)app_wave_file_play_start
//                                    , (void *)APP_WAVE_FILE_ID_HFP_RING
//                                    , 200);
            }
            break;
        }

    case AT_RESULT_COPS:
        {
            bt_frontend_notification("AG operator %s", at_result->u.cops.name);
            break;
        }

    case AT_RESULT_CIND_READ:
        {
            const int8_t *p = at_result->u.cind.data;
/****************
AT+CIND=? 
    call: Standard call status indicator, where: 
        <value>=0 means there are no calls in progress 
        <value>=1 means at least one call is in progress 

    callsetup: Bluetooth proprietary call set up status indicator. 
                Support for this indicator is optional for the HF. When supported, 
                this indicator shall be used in conjunction with, and as an extension of the standard call indicator. 
        Possible values are as follows:
        <value>=0 means not currently in call set up.  
        <value>=1 means an incoming call process ongoing. 
        <value>=2 means an outgoing call set up is ongoing. 
        <value>=3 means remote party being alerted in an outgoing call. 

    callheld: Bluetooth proprietary call hold status indicator. Support for this indicator is mandatory for the AG, 
              optional for the HF. Possible values are as follows:
         <value>=0: No calls held
         <value>=1: Call is placed on hold or active/held calls swapped
                    (The AG has both an active AND a held call)
         <value>=2: Call on hold, no active call
*****************/

            bt_frontend_notification("AG state {service %d, call %d, "
                                     "callsetup %d, callheld %d, signal %d, roam %d}",
                                     p[CIND_SERVICE], p[CIND_CALL], p[CIND_CALLSETUP],
                                     p[CIND_CALLHELD], p[CIND_SIGNAL], p[CIND_ROAM]);
            if( p[CIND_CALL] > 0 || p[CIND_CALLSETUP] > 0 )
            {
                os_printf("===AT+CIND+READ:%d,%d\r\n",p[CIND_CALL],p[CIND_CALLSETUP]);
                bt_flag1_operate( APP_FLAG_HFP_CALLSETUP, 1 );
            #ifdef BT_ONE_TO_MULTIPLE
                set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP);
            #else
                hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP);
            #endif
                app_button_type_set( BUTTON_TYPE_HFP );
                if(p[CIND_CALLSETUP] == 1) /* incoming call */
                {
                #ifdef BT_ONE_TO_MULTIPLE
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG2_HFP_INCOMING);
                #else
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG2_HFP_INCOMING);
                #endif
                }
                if(p[CIND_CALLSETUP] > 1) /* outgoing call */
                {
                #ifdef BT_ONE_TO_MULTIPLE
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_OUTGOING);
                #else
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_OUTGOING);
                #endif
                }
                if(p[CIND_CALL] > 0)
                {
                    bt_flag1_operate(APP_FLAG_CALL_ESTABLISHED,1);
                #ifdef BT_ONE_TO_MULTIPLE
                    clear_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_CALL_ESTABLISHED);
                    if(p[CIND_CALLSETUP] == 1)  /* 1 call and 1 incoming call, TWC MODE*/
                    {
                        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,1);
                        set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,1);
                        bt_flag2_operate( APP_FLAG2_HFP_SECOND_CALLSETUP, 1 );
                        app_button_type_set(APP_BUTTON_TYPE_TWC);
                    }
                #else
                    hf_clear_flag(g_hf_current_app_ptr,APP_FLAG_HFP_OUTGOING|APP_FLAG2_HFP_INCOMING);
                    hf_set_flag(g_hf_current_app_ptr,APP_FLAG_CALL_ESTABLISHED);
                #endif 
                }
            }
            //else if( p[CIND_CALLHELD] == 1 )
            if( p[CIND_CALLHELD] > 0 )
            {
                app_button_type_set(BUTTON_TYPE_TWC);
                bt_flag1_operate( APP_FLAG_HFP_CALLSETUP, 1 );
            #ifdef BT_ONE_TO_MULTIPLE
                set_hf_flag_1toN(&app_ptr->raddr,APP_FLAG_HFP_CALLSETUP);
            #else
                hf_set_flag(g_hf_current_app_ptr,APP_FLAG_HFP_CALLSETUP);
            #endif
                bt_flag2_operate( APP_FLAG2_HFP_SECOND_CALLSETUP, 1 );
            }
            break;
        }

    case AT_RESULT_CNUM:
        {
            bt_frontend_notification("AG subscriber %s, type %d",
                                     at_result->u.cnum.number, at_result->u.cnum.type);
            break;
        }

    case AT_RESULT_CCWA:
        {
            const at_result_ccwa_t *ccwa = &at_result->u.ccwa;

            bt_frontend_notification("AG waiting call notification "
                                     "%s, %d, %d, %s, %d, %s, %d, %d", ccwa->number, ccwa->type,
                                     ccwa->info_class, ccwa->alpha, ccwa->cli_validity,
                                     ccwa->subaddr, ccwa->satype, ccwa->priority);
            set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_MODE,1);
            set_current_hfp_flag(APP_HFP_PRIVATE_FLAG_TWC_WAIT_STATE,1);
            app_button_type_set(APP_BUTTON_TYPE_TWC);
            break;
        }

    default:
        break;
    }
    hf_clear_flag(app_ptr,APP_HFP_PRIVATE_FLAG_AT_CMD_PENDING);
}

int hf_sco_handle_process(int oper)
{
    hfp_hf_app_t *app_ptr = NULL;
#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = g_hf_current_app_ptr;
#else
    app_ptr = &g_hfp_hf_app;
#endif
    os_printf("===sco toggle:%d\r\n",oper);
    if( oper == 0 )
        return util_sco_connect(app_ptr->priv);
    else if( oper == 1 )
        return util_sco_disconnect(app_ptr->priv);
    else
        return -1;
}

void hf_set_flag(void *arg, uint32 flag)
{
    hfp_hf_app_t *app_ptr = (hfp_hf_app_t *)arg;
    app_ptr->flag |= flag;
    //os_printf("----hf_set_flag:%x:%x\r\n", app_ptr, app_ptr->flag);

}

uint32 hf_check_flag(void *arg, uint32 flag)
{
    hfp_hf_app_t *app_ptr = (hfp_hf_app_t *)arg;
    return (app_ptr->flag & flag);
}
uint32 get_hf_priv_flag(uint8 idx,uint32 flag)
{
    if(g_hfp_app_array[idx & 0x01])
    return(g_hfp_app_array[idx & 0x01]->flag & flag);
    return 0;
}
uint32 hf_clear_autoconn_diconned_flag(void *raddr)
{
    uint32_t i;
    uint32_t executed=0;
    hfp_hf_app_t *app_ptr=NULL;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if((app_ptr)
           && (btaddr_same(raddr, &app_ptr->raddr)))
        {
            executed = 1;
            hf_clear_flag(app_ptr, FLAG_HFP_AUTOCONN_DISCONNED);
            break;
        }
    }

    bt_flag1_operate(FLAG_HFP_AUTOCONN_DISCONNED, 0);

    return executed;
}

void hf_clear_flag(void *arg, uint32 flag)
{
    hfp_hf_app_t *app_ptr = (hfp_hf_app_t *)arg;
    app_ptr->flag &= ~flag;
}

btaddr_t * hf_get_curhfp_raddr(void)
{
    if(g_hf_current_app_ptr)
    {
        return &(g_hf_current_app_ptr->raddr);
    }
    return NULL;
}

uint32 hfp_get_remote_addr(void *arg, btaddr_t *r_addr)
{
    hfp_hf_app_t *app_ptr = (hfp_hf_app_t *)arg;

    btaddr_copy(r_addr, &app_ptr->raddr);

    return UWE_OK;
}

void *hfp_select2_current_app(void *arg)
{
    hfp_hf_app_t *app_ptr = (hfp_hf_app_t *)arg;

    g_hf_current_app_ptr = app_ptr;
    //os_printf("selectHF:%x\r\n", g_hf_current_app_ptr);

    return g_hf_current_app_ptr;
}

// this function has not been called
void otm_current_hfp_call_over_handler(void *arg)
{
    app_handle_t app_h = app_get_sys_handler();

    bt_auto_connect_stop();

    if(arg == g_hf_current_app_ptr)
    {
        app_h->auto_conn_id = 0;
        bt_auto_connect_start();
    }
}
result_t curhfp_is_null(void)
{
    if(g_hf_current_app_ptr)
        return 0;
    else
        return 1;
}
uint32 get_current_hfp_flag(uint32 flag)
{
    if (!g_hf_current_app_ptr)
        return 0;
    
    return (g_hf_current_app_ptr->flag&flag);
}
void set_current_hfp_flag(uint32 flag,uint8 op)
{
    if (!g_hf_current_app_ptr)
        return;
    if(op)
        g_hf_current_app_ptr->flag |= flag;
    else
        g_hf_current_app_ptr->flag &= ~flag;
}
#ifdef BT_ONE_TO_MULTIPLE
void clr_hf_flags(btaddr_t *btaddr)
{
    uint32 i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
            hf_clear_flag(g_hfp_app_array[i],APP_FLAG_HFP_CONNECTION | APP_FLAG_SCO_CONNECTION|APP_FLAG_HFP_CALLSETUP |APP_FLAG_CALL_ESTABLISHED);
    }
}

void set_hf_flag_1toN(btaddr_t *btaddr,uint32 flag)
{
    uint32 i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
            hf_set_flag(g_hfp_app_array[i],flag);
    }
}

void set_hf_flag_call_seq(btaddr_t *btaddr)
{
#if ( BT_MODE==BT_MODE_1V2)
    uint8 i;
    uint8 index = 0;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if(g_hfp_app_array[i] && btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
    	{
            index = i;
            break;
    	}
    }
    if((index == BT_MAX_AG_COUNT) || hf_check_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2))
    {
        os_printf("No change,index:%d,:0x%x, addr:"BTADDR_FORMAT"\r\n",index,hf_check_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2),BTADDR(btaddr));
        return;
    }
    else  // 
    {
        if(g_hfp_app_array[1-index] && (hf_check_flag(g_hfp_app_array[1-index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1)))
        {
            hf_set_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2);
        }
        else if(g_hfp_app_array[1-index] && hf_check_flag(g_hfp_app_array[1-index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2))
        {
            hf_set_flag(g_hfp_app_array[1-index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1);
            hf_clear_flag(g_hfp_app_array[1-index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2);
            hf_set_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2);
        }
        else
        {
            hf_set_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1);
        }
    }
    os_printf("set seq:0x%x, addr:"BTADDR_FORMAT"\r\n",hf_check_flag(g_hfp_app_array[index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2),BTADDR(btaddr));
    if(g_hfp_app_array[1-index])
        os_printf("set seq:0x%x, addr:"BTADDR_FORMAT"\r\n",hf_check_flag(g_hfp_app_array[1-index],APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE1|APP_HFP_PRIVATE_FLAG_CALL_SEQUENCE2),BTADDR(&g_hfp_app_array[1-index]->raddr));
#else
    return;
#endif
}

void clear_hf_flag_1toN(btaddr_t *btaddr,uint32 flag)
{
    uint32 i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
        {
            hf_clear_flag(g_hfp_app_array[i],flag);
        }
    }
}
uint8 hf_exchange_sco_active_flag(btaddr_t *btaddr,t_sco_active_state state)
{
    uint32 i;
    uint8 sco_cnt = 0;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
    	if(g_hfp_app_array[i]&& (g_hfp_app_array[i]->flag & APP_FLAG_SCO_CONNECTION))
    	{
    		sco_cnt++;
    		if(state == SCO_DISCONNECTED)
    		{
	    		if(btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
    				//hf_clear_flag(g_hfp_app_array[i],APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE);
    				g_hfp_app_array[i]->flag &= ~APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE;
    			else
    				//hf_set_flag(g_hfp_app_array[i],APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE);
    				g_hfp_app_array[i]->flag |= APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE;
    		}else if(state == SCO_CONNECTED)
    		{
    			if(btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
    				//hf_set_flag(g_hfp_app_array[i],APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE);
    				g_hfp_app_array[i]->flag |= APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE;
    			else
    				//hf_clear_flag(g_hfp_app_array[i],APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE);
    				g_hfp_app_array[i]->flag &= ~APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE;
    		}
    	}
    }
    if((state == SCO_SWAP) && (sco_cnt > 1))
    {
        for(i = 0; i < BT_MAX_AG_COUNT; i++)
        {
            g_hfp_app_array[i]->flag ^= APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE;
            if(g_hfp_app_array[i]->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE)
            {
                g_hf_current_app_ptr = g_hfp_app_array[i];
            }
        }
    }
    return sco_cnt;
}
uint16 get_hf_active_sco_handle(void)
{
    uint8 i;
    hfp_hf_app_t *app_ptr = NULL;
    hci_link_t * tmp_link = NULL;
    app_handle_t sys_hdl;
    sys_hdl = app_get_sys_handler();
    //os_printf("===sco select other raddr: "BTADDR_FORMAT"\r\n", BTADDR(raddr));
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if (app_ptr && (app_ptr->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE))
        {
            tmp_link = hci_sco_link_lookup_btaddr(sys_hdl->unit, &app_ptr->raddr, HCI_LINK_eSCO);
            if(tmp_link)
            {
                return tmp_link->hl_handle;
            }
        }
    }
    return 0;
}

void hfp_2eSCOs_A_B_SWAP(void)
{
    u_int16 sco_handle;
    u_int8 sco_cnt;
    sco_cnt = hf_exchange_sco_active_flag(NULL,SCO_SWAP);
    if(sco_cnt > 1)
    {
        sco_handle = get_hf_active_sco_handle();
        if(sco_handle > 0)
        {
            bt_exchange_hf_active_by_handle(sco_handle);
            if(bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
                hf_audio_conf_restore(0);
            else
                hf_audio_conf_restore(1);
            aud_mic_open(1);
        #ifndef ADJUST_AEC_DELAY
            aud_open();
        #endif
        }
    }
}
void *get_hfp_from_active_sco_flag(void)
{
    uint8 i;
    hfp_hf_app_t *app_ptr = NULL;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if (app_ptr && (app_ptr->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE))
        {
            return app_ptr;
        }
    }
    return NULL;
}
__INLINE__ void set_2hfps_sco_and_incoming_flag(uint8_t val)
{
    g_active_sco_and_incoming_flag = val;
}
__INLINE__ uint8_t get_2hfps_sco_and_incoming_flag(void)
{
    return g_active_sco_and_incoming_flag;
}
uint8_t check_2hfps_sco_and_incoming_status(void)
{
    uint8_t ret,ret1 = 0,ret2 = 0;
    uint8 i;
    hfp_hf_app_t *app_ptr = NULL;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        app_ptr = g_hfp_app_array[i];
        if(app_ptr && (app_ptr->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE)&& (app_ptr->flag & APP_FLAG_CALL_ESTABLISHED))
        {
            ret1++;
            continue;
        }
        if(app_ptr && (!(app_ptr->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE)) && (app_ptr->flag & APP_FLAG2_HFP_INCOMING))
        {
            ret2++;
        }
    }
    ret = ((ret1 * ret2) > 0);
    set_2hfps_sco_and_incoming_flag(ret);
    return (ret);

}
static hfp_hf_app_t *get_inactive_sco_hfp_ptr(void)
{
    uint8 i;
    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i]
            && (g_hfp_app_array[i]->flag & APP_SCO_PRIVATE_FLAG_CONNECTION)
            && !(g_hfp_app_array[i]->flag & APP_SCO_PRIVATE_FLAG_CONNECTION_ACTIVE)
            )
        {
            return  g_hfp_app_array[i];
        }

    }
    return NULL;
}

int hf_sco_disconn_reconn(int oper)
{
    hfp_hf_app_t *app_ptr = NULL;
    app_ptr = get_inactive_sco_hfp_ptr();
    if(app_ptr == NULL)
        return -1;
    if( oper == 0 )
        return util_sco_connect(app_ptr->priv);
    else if( oper == 1 )
    	return bt_hfp_hf_hangup_call(app_ptr->session);
        //return util_sco_disconnect(app_ptr->priv);
    else
        return -1;
}

uint32 get_hfp_flag_1toN(btaddr_t *btaddr,uint32 flag)
{
    uint32 i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
        {
            return (g_hfp_app_array[i]->flag&flag);
        }
    }
    //os_printf("ERROR:g_hfp_app_array addr not match!\r\n");
    return 0;
}

uint32 has_hfp_flag_1toN(uint32 flag)
{
    uint32 i;
    uint32 num = 0;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && (g_hfp_app_array[i]->flag&flag))
        {
            num++;
        }
    }
    return num;
}

void change_cur_hfp_based_on_addr(btaddr_t *btaddr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if(g_hfp_app_array[i] && (btaddr_same(&g_hfp_app_array[i]->raddr, btaddr)))
        {
            g_hf_current_app_ptr = g_hfp_app_array[i];
            break;
        }
    }
}
void change_cur_hfp_to_another_call_conn(btaddr_t *btaddr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) && (!btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
			&&(g_hfp_app_array[i]->flag&(APP_FLAG_HFP_CALLSETUP|APP_FLAG_CALL_ESTABLISHED)))
        {
            g_hf_current_app_ptr = g_hfp_app_array[i];
            break;
        }
    }
}

void change_cur_hfp_to_another_incoming_call(btaddr_t *btaddr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) && (!btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
			&&(g_hfp_app_array[i]->flag&(APP_FLAG_HFP_CALLSETUP|APP_FLAG2_HFP_INCOMING))
			&&(!(g_hfp_app_array[i]->flag&APP_FLAG_CALL_ESTABLISHED)))
        {
            g_hf_current_app_ptr = g_hfp_app_array[i];
            break;
        }
    }
}

uint32_t change_cur_hfp_to_another_sco_conn(btaddr_t *btaddr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) && (!btaddr_same(&g_hfp_app_array[i]->raddr,btaddr))
			&&(g_hfp_app_array[i]->flag&APP_FLAG_SCO_CONNECTION))
        {
            g_hf_current_app_ptr = g_hfp_app_array[i];
            return 1;
        }
    }
    return 0;
}

uint32_t hfp_select_current_app_to_another(btaddr_t *btaddr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i]) 
			 && (g_hfp_app_array[i]->flag&APP_FLAG_HFP_CONNECTION)
			 && (!btaddr_same(&g_hfp_app_array[i]->raddr,btaddr)))
        {
            g_hf_current_app_ptr = g_hfp_app_array[i];
            return 1;
        }
    }
	
    return 0;
}


uint32_t hfp_has_the_sco_conn(btaddr_t addr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i])
            && btaddr_same(&g_hfp_app_array[i]->raddr,&addr)
            && (g_hfp_app_array[i]->flag & APP_FLAG_SCO_CONNECTION) )
        {
            return 1;
        }
    }

    return 0;
}

uint32_t hfp_has_the_connection(btaddr_t addr)
{
    uint32_t i;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i])
            && btaddr_same(&g_hfp_app_array[i]->raddr,&addr)
            && (g_hfp_app_array[i]->flag & APP_FLAG_HFP_CONNECTION) )
        {
            return 1;
        }
    }

    return 0;
}



uint32_t hfp_has_sco_conn(void)
{
    uint32_t i;
    uint32_t num = 0;

    for(i = 0; i < BT_MAX_AG_COUNT; i ++)
    {
        if((g_hfp_app_array[i])
            && (g_hfp_app_array[i]->flag & APP_FLAG_SCO_CONNECTION) )
        {
            num++;
        }
    }
    return num;
}
#endif

#ifdef CONFIG_PRODUCT_TEST_INF
result_t hf_cmd_trans_buttoncode(uint32 value)
{
    if (!g_hf_current_app_ptr || !g_hf_current_app_ptr->session)
        return UWE_NODEV;
    
    return bt_hfp_hf_trans_buttoncode(g_hf_current_app_ptr->session, value);
}
#endif

#if PTS_TESTING
/* create a sco connection from IUT to PTS. */
void pts_util_sco_connect(void)
{
    result_t err = UWE_OK;
    hfp_hf_app_t *app_ptr = NULL;
#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = g_hf_current_app_ptr;
#else
    app_ptr = &g_hfp_hf_app;
#endif
    err = util_sco_connect_pts(app_ptr->priv);
    os_printf("pts_util_sco_connect, 0x%x, %d \r\n", app_ptr, err);
}

void pts_util_sco_disconnect(void)
{
    result_t err = UWE_OK;
    hfp_hf_app_t *app_ptr = NULL;
#ifdef BT_ONE_TO_MULTIPLE
    app_ptr = g_hf_current_app_ptr;
#else
    app_ptr = &g_hfp_hf_app;
#endif
    err = util_sco_disconnect(app_ptr->priv);
    os_printf("pts_util_sco_disconnect, 0x%x, %d \r\n", app_ptr, err);
}

/* initiate a service level connection from IUT to PTS. */
void pts_util_initiate_slc(void)
{
    result_t err = UWE_OK;
    app_env_handle_t env_h = app_env_get_handle();
    btaddr_t *remote_btaddr = &env_h->env_data.default_btaddr;
    btaddr_t *local_btaddr = &env_h->env_cfg.bt_para.device_addr;
    
    os_printf("pts_util_initiate_slc, local = " BTADDR_FORMAT ", remote = " BTADDR_FORMAT "\r\n", BTADDR(local_btaddr), BTADDR(remote_btaddr));
    
    err = app_bt_active_conn_profile(PROFILE_ID_HFP, (void *)remote_btaddr);

    os_printf("pts_util_initiate_slc, Exit, 0x%x, %d \r\n", env_h, err);
}
#endif

//  EOF
