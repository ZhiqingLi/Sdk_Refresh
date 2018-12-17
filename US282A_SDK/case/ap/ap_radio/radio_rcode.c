#include "app_radio.h"

//用13M时，91和104是13的倍频，过滤
const uint32 freq_passed[] =
{ 76000, 85500, 91000, 96000, 104000, 108000 };

//是否应该被跳过的频点
bool should_be_passed(uint32 freq)
{
    //单位kHz(把各个波段都值都可以填到这里)
    //这里有些是方案产生的干扰频率，有些时FM芯片不能处理的频率
    uint8 n;

    for (n = 0; n < ARR_COUNT(freq_passed); n++)
    {
        if (freq_passed[n] == freq)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//判断指定频率是否在当前的保存列表中
//freq: 频率
//p_freq: 当前波段对应的预设电台的数组
//返回值: 当频率在数组中时，返回下标；否则返回0xff
uint8 whether_in_list(uint32 freq, uint32 *p_freq)
{
    uint8 ret;
    uint32 *ptr;
    uint8 num;
    uint32 tmp;

    /* 当前波段电台列表*/
    ptr = p_freq;

    for (num = 0; num < MAX_STATION_COUNT; num++)
    {
        tmp = *(ptr + num);
        if (tmp == freq)
        {
            //在列表中已找到
            break;
        }
    }

    //在列表中未找到
    if (num == MAX_STATION_COUNT)
    {
        ret = 0xff;
    }
    else
    {
        ret = num + 1;
    }
    return ret;
}

int radio_send_sync_msg(uint8 app_id, msg_apps_t *msg, msg_reply_t *reply, uint32 timeout)
{
    private_msg_t cur_send_pmsg;
    //msg_reply_t temp_reply;
    os_event_t *sem; //同步消息同步信号量指针
    int retvalue = ERR_NO_ERR;
    uint8 mq_id = MQ_ID_EGN;

    PRINT_DBG_INT("sync msg:", msg->type);

    //无限时等待信号量，同时只能有1个线程发送同步消息
    libc_sem_wait(thread_mutex, 0);

    //填充私有消息结构体
    libc_memcpy(&(cur_send_pmsg.msg), msg, sizeof(msg_apps_t));
    libc_sem_init(&sem, 0);
    cur_send_pmsg.sem = sem;
    cur_send_pmsg.reply = reply;

    //发送消息到队列中
    if (sys_mq_send((uint32) (mq_id), (void *) &cur_send_pmsg) < 0)
    {
        //PRINT_ERR("sync msg ERR_MSGQUEUE_FULL!!");
        retvalue = ERR_MSGQUEUE_FULL;
    }
    else
    {
        //等待同步消息回应
        if (libc_sem_wait(sem, timeout) < 0)
        {
            //PRINT_ERR("sync msg ERR_SYNC_TIMEOUT!!");
            retvalue = ERR_SYNC_TIMEOUT;
        }
    }

    libc_sem_destroy(&sem);
    libc_sem_post(thread_mutex);

    PRINT_DBG_INT("sync msg ret:", msg->type);

    return retvalue;
}

bool radio_send_msg(void *input_param, void *output_param, uint32 msg_type)
{
    bool bret = TRUE;
    msg_apps_t msg;
    //返回消息内容存放变量
    msg_reply_t reply;

    //存放输入参数的地址
    msg.content.addr = input_param;

    //存放获取参数的地址
    reply.content = output_param;

    //消息类型(即消息名称)
    msg.type = msg_type;

    //发送同步消息
    bret = (bool) (radio_send_sync_msg(APP_ID_FMENGINE, &msg, &reply, 0) == ERR_NO_ERR);

    if (reply.type == MSG_REPLY_FAILED)
    {
        bret = FALSE;
    }
    return bret;
}

//获取当前频率
bool radio_get_freq(void)
{
    return radio_send_msg(NULL, &g_engine_status.FM_CurrentFreq, MSG_FMENGINE_GET_FREQ);
}

//解除静音
void release_mute_timer(void)
{
    //解除静音
    radio_set_mute(releaseMUTE);
    g_mute_set_timer = -1;
}

