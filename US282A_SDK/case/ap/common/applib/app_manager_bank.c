/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：应用管理模块，包括应用初始化和应用注销接口，除了 ap_manager 外，其他应用
 *       都必须在应用进入时调用应用初始化，在应用退出时调用应用注销。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

static bool __app_running_set_app_info(uint8 app_id, const app_info_t *app_info);

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    应用在 applib 模块上的注册和初始化。
 * \param[in]    app_id 应用ID，如 APP_ID_MUSIC，APP_ID_MANAGER，APP_ID_MENGINE 等
 * \param[in]    type 应用类型，参见 app_type_e 定义
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 表示注册和初始化成功。
 * \retval           FALSE 表示注册和初始化失败。
 * \ingroup      app_manager
 * \par          exmaple code
 * \code
 例子1：前台应用 music AP 进行 applib 模块注册和初始化
 applib_init(APP_ID_MUSIC, APP_TYPE_GUI);

 例子2：后台引擎应用 music engine AP 进行 applib 模块注册和初始化
 applib_init(APP_ID_MENGINE, APP_TYPE_CONSOLE);

 例子3：进程管理应用 manager AP 进行 applib 模块注册和初始化
 applib_init(APP_ID_MANAGER, APP_TYPE_PMNG);
 * \endcode
 * \note
 * \li  在应用 main 函数入口处调用，在此之前，不许调用其他 applib 接口。
 *******************************************************************************/
bool applib_init(uint8 app_id, app_type_e type)
{
    app_info_t *cur_app_info;

    if (app_id >= APP_ID_MAX)
    {
        return FALSE;
    }

    //调节频率
    if (type == APP_TYPE_GUI)
    {
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
        g_app_info_state_all.ui_status = UI_STATUS_IDLE;
    }
    else if (type == APP_TYPE_CONSOLE)
    {
        adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_LEVEL2, FREQ_NULL);
    }
    else
    {
        //nothing
    }

    //获取 app_info 结构体指针
    cur_app_info = &g_app_info_vector[type];

    //上锁，禁止任务调度
    sys_os_sched_lock();

    //初始化应用信息
    libc_memset(cur_app_info, 0x0, sizeof(app_info_t));
    cur_app_info->used = 1; //标志已被分配使用
    cur_app_info->app_id = app_id;
    cur_app_info->app_type = (uint8) type;
    //cur_app_info->soft_watchdog_timeout_expires = 0;
    if (type == APP_TYPE_PMNG)
    {
        cur_app_info->mq_id = (uint8) MQ_ID_MNG;
    }
    else if (type == APP_TYPE_BTSTACK)
    {
        cur_app_info->mq_id = (uint8) MQ_ID_BT;
    }
    else if (type == APP_TYPE_GUI)
    {
        cur_app_info->mq_id = (uint8) MQ_ID_DESK;
        g_app_info_state_all.app_state = APP_STATE_NO_PLAY; //前台AP注册，初始化为 NO_PLAY 状态
    }
    else
    {
        //message queue
        cur_app_info->mq_id = (uint8) MQ_ID_EGN;

        g_app_info_state_all.engine_type = get_engine_type_by_appid(app_id);
    }

    //解锁，允许任务调度
    sys_os_sched_unlock();

    g_this_app_info = cur_app_info;

    //初始化线程互斥信号量
    libc_sem_init(&thread_mutex, 1);

    PRINT_INFO_INT("app init:", app_id);

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    应用在 applib 模块上的注销处理。
 * \param[in]    none
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 表示注销成功。
 * \retval           FALSE 表示注销失败。
 * \ingroup      app_manager
 * \par          exmaple code
 * \code
 例子1：应用进行 applib 模块注销
 applib_quit();
 * \endcode
 * \note
 * \li  在应用 main 函数退出处调用，在此之后，不许调用其他 applib 接口。
 *******************************************************************************/
bool applib_quit(void)
{
    app_info_t cur_app_info;

    PRINT_INFO_INT("app quit:", g_this_app_info->app_id);

    //销毁线程互斥信号量
    libc_sem_destroy(&thread_mutex);

    if (g_this_app_info->app_type == APP_TYPE_CONSOLE)
    {
        //修改后台引擎类型为 NONE
        g_app_info_state_all.engine_type = ENGINE_NULL;
        g_app_info_state_all.engine_state = ENGINE_STATE_NULL;
    }

    //修改 app_info 结构体为未被使用
    libc_memcpy(&cur_app_info, g_this_app_info, sizeof(app_info_t));
    cur_app_info.used = 0;
    cur_app_info.app_id = APP_ID_MAX;
    //cur_app_info.soft_watchdog_timeout_expires = 0;
    __app_running_set_app_info(g_this_app_info->app_id, &cur_app_info);

    //调节频率
    if (g_this_app_info->app_type == APP_TYPE_GUI)
    {
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_NULL, FREQ_NULL);
    }
    else if (g_this_app_info->app_type == APP_TYPE_CONSOLE)
    {
        adjust_freq_set_level(AP_BACK_LOW_PRIO, FREQ_NULL, FREQ_NULL);
    }
    else
    {
        //nothing
    }

    return TRUE;
}

/*! \endcond */

/*! \cond */

//根据应用ID设置应用信息
static bool __app_running_set_app_info(uint8 app_id, const app_info_t *app_info)
{
    app_info_t *app;
    uint8 i;

    if (app_id >= APP_ID_MAX)
    {
        return FALSE;
    }

    //搜索 app_id 对应 app_info_t
    app = NULL;
    for (i = 0; i < MAX_APP_COUNT; i++)
    {
        app = &g_app_info_vector[i];
        if ((app->used == 1) && (app->app_id == app_id))
        {
            break;
        }
    }

    if (NULL == app)
    {
        return FALSE;
    }

    sys_os_sched_lock();
    libc_memcpy(app, app_info, sizeof(app_info_t));
    sys_os_sched_unlock();

    return TRUE;
}

/*! \endcond */
