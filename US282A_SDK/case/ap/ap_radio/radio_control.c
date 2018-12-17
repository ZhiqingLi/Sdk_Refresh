#include "app_radio.h"

//进入Standby
bool radio_modual_standby(void)
{
    bool bret = TRUE;
    msg_apps_t msg;

    //FM 模组进入standby
    msg.type = MSG_FMENGINE_STANDBY;
    //发送异步消息
    bret = (bool) (send_async_msg(APP_ID_FMENGINE, &msg) == ERR_NO_ERR);
    return bret;
}
