/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include "bt_app_internal.h"
#include "app_beken_includes.h"


result_t bt_app_init(void)
{
    result_t err = UWE_OK;
    app_env_handle_t env_h = app_env_get_handle();
    DECLARE_FNAME("bt_app_init");

    LOCK_CHECK;

    /* Initialize frontend */
    /* TODO: Possible move this to be last if we give the frontend the ability
     * to scan for possible commands, eg. for completion */
    err = frontend_init();

    /* TODO: Call command backends to register parsers */
    err = unit_backend_init();

#ifdef CONFIG_BLUETOOTH_SDAP
    err = sdap_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_HSP
    if(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_HSP)
        err = hs_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_HFP
    if((env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_HFP)
        ||(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_APP_BAT_DISPLAY))
    {
        err = hf_backend_init();
    }
#endif

#ifdef CONFIG_BLUETOOTH_A2DP
    if(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_A2DP)
    {
        err = a2dp_backend_init();
	#ifdef CONFIG_TWS
        err |= a2dp_src_backend_init();
	#endif
    }
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP
    if(env_h->env_cfg.bt_para.bt_flag & APP_ENV_BT_FLAG_A2DP)
    {
        err = avrcp_backend_init();
    }
#endif

#ifdef CONFIG_BLUETOOTH_PAN
    err = pan_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_TESTS
    err = tests_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_PBAP
    err = pbap_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_OPUSH
    err = opush_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_MAP
    err = map_backend_init();
#endif

#if defined(CONFIG_BLUETOOTH_SPP)
#ifndef BT_DUALMODE
    if(app_env_check_SPP_profile_enable())
#endif
        err = spp_backend_init();
#endif

#if defined(CONFIG_BLUETOOTH_HID)
    if(app_env_check_HID_profile_enable())
        err = hid_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_DUN_DT
    err = dun_backend_init();
#endif

#ifdef CONFIG_BLUETOOTH_WIAP
    err = wiap_backend_init();
#endif

    return err;
}

void bt_app_uninit(void)
{
    LOCK_CHECK;

#ifdef CONFIG_BLUETOOTH_WIAP
    wiap_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_MAP
    map_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_OPUSH
    opush_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_PBAP
    pbap_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_TESTS
    tests_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_PAN
    pan_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP
    avrcp_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_HFP
    hf_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_HSP
    hs_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_A2DP
    a2dp_backend_uninit();
	#ifdef CONFIG_TWS
	a2dp_src_backend_uninit();
	#endif
#endif

#if defined(CONFIG_BLUETOOTH_SPP)
#ifndef BT_DUALMODE
    if(app_env_check_SPP_profile_enable())
#endif
        spp_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_DUN_DT
    dun_backend_uninit();
#endif

#ifdef CONFIG_BLUETOOTH_SDAP
    sdap_backend_uninit();
#endif

    /* must be last - removes all units */
    unit_backend_uninit();

    frontend_uninit();
}

#ifdef BEKEN_DEBUG
void bt_frontend_notification(const char *fmt, ...)
{
    char str[NOTIFY_BUFFER_SIZE];
    va_list args;

    va_start(args, fmt);
    j_vsnprintf(str, sizeof(str), fmt, args);
    va_end(args);
    str[sizeof(str) - 1] = 0;

    frontend_notify(str, j_strlen(str));
}
#endif
