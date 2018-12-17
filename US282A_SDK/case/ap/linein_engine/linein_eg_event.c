#include "app_linein_eg.h"

app_result_e linein_eg_get_status(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //copy当前播放状态到消息指针指向的数据区
    libc_memcpy(data_ptr->reply->content, g_eg_status_p, sizeof(linein_eg_status_t));

    linein_eg_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
