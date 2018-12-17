#include  "ap_btcall.h"

//获取ASQT工具配置的音量
void get_eg_asqt_vol(void)
{
    static uint8 run_div = 0;
    static uint8 set_vol = 0xff;
    uint32 vol = 0;
    if (++run_div > 25)
    {
        run_div = 0;
        btcall_send_sync_msg(NULL, (void*) &vol, MSG_BTCALLEG_GET_EG_VOLUM_SYNC);
        if (set_vol == 0xff)
        {
            set_vol = vol & 0xff;//第一次参数不配置，兼容旧版本
        }
        if (set_vol != (vol & 0xff))
        {
            set_vol = vol & 0xff;
            com_set_sound_volume(set_vol, 0);
            libc_print("set_asqt_volum:", set_vol, 2);
        }
    }
}

//获取ASQT工具配置的PA音量
void get_eg_asqt_PA(void)
{
    static uint8 run_div = 0;
    static uint8 set_vol = 0xff;
    uint32 vol = 0;
    if (++run_div > 25)
    {
        run_div = 0;
        btcall_send_sync_msg(NULL, (void*) &vol, MSG_BTCALLEG_GET_EG_PA_VOLUM_SYNC);
        if (set_vol == 0xff)
        {
            set_vol = vol & 0xff;//第一次参数不配置，兼容旧版本
        }
        if (set_vol != (vol & 0xff))
        {
            set_vol = vol & 0xff;
            //            com_set_volume_gain(set_vol);
            libc_print("ASQT set_PA_volum:", set_vol, 2);
        }
    }
}

//btcall不是后台模式时，配置音量相关，从loop中移过来的

void volume_cfg(bool set)
{
    if (set == TRUE)
    {
        com_switch_volume_mode(VOLUME_MODE_CALL);
        com_set_volume_gain((uint8) com_get_config_default(SETTING_SOUND_CALL_VOLUME_GAIN));
        com_reset_sound_volume(SET_VOLUME_SYNC_TO_REMOTE);

        //打电话不能进行TTS播报
        com_tts_state_play_control(TTS_CTRL_IGNORE);
    }
    else
    {
        com_switch_volume_mode(VOLUME_MODE_NORMAL);
        com_set_volume_gain((uint8) com_get_config_default(SETTING_SOUND_MAIN_VOLUME_GAIN));
        com_reset_sound_volume(0);
        //退出打电话开启TTS播报
        com_tts_state_play_control(TTS_CTRL_CANCEL_IGNORE);
    }

}

