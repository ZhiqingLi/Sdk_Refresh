#include  "manager.h"

void install_key_drv(void)
{
    uint32 key_para;

    key_para = (uint32) com_get_config_default(SETTING_BAT_CHARGE_VOLTAGE);
    key_para |= ((uint32) com_get_config_default(SETTING_BAT_CHARGE_CURRENT) << 8);
    key_para |= ((uint32) com_get_config_default(SETTING_INPUT_IR_CODE) << 16);
    key_para |= ((uint32) com_get_config_default(SETTING_BAT_CHARGE_MODE) << 24);
    sys_drv_install(DRV_GROUP_KEY, key_para, "key.drv");

#if (SUPPORT_OUTER_CHARGE == 1)
    if ((uint8) com_get_config_default(SETTING_BAT_CHARGE_MODE) == BAT_CHARGE_MODE_OUTER)
    {
        if (IS_EXTERN_BATTERY_IN() == 1)
        {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
            key_extern_tk_op(TK_OP_ON);
#endif
            g_app_info_state.extern_bat_in = 1;
        }
    }
    else
#endif
    {
#if (SUPPORT_TK_DRIVER == TK_DRIVER_OUTER)
        key_extern_tk_op(TK_OP_ON);
#endif
        g_app_info_state.extern_bat_in = 1;
    }
    
}

