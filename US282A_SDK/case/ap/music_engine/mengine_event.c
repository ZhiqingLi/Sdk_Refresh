#include "app_mengine.h"

app_result_e mengine_get_filepath(void* msg_ptr)
{
    //uint8 location_size;
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    //返回路径的指针
    file_path_info_t* locat_info = (file_path_info_t*) (data_ptr->reply->content);

    //是目录模式
    //location_size = sizeof(file_location_t);

    libc_memcpy(&(locat_info->file_path), &(g_eg_cfg_p->location), sizeof(file_location_t));

    locat_info->file_source = g_fsel_type; //当前文件模式

    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

app_result_e mengine_get_playmode(void* msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    if (g_shuffle_flag == SHUFFLE)
    {
        *(uint8 *) (data_ptr->reply->content) = FSEL_MODE_RANOM;
    }
    else
    {
        *(uint8 *) (data_ptr->reply->content) = g_repeat_mode;
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
