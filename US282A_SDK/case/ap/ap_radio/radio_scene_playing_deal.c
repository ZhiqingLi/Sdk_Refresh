#include "app_radio.h"

//播放场景循环处理
app_result_e playing_loop_deal(void)
{
    app_result_e ret = RESULT_NULL;

    //radio 播放场景循环
    while (1)
    {
        ret = com_view_loop();
        //iOS APP只要点击菜单就会发一个停止搜台命令过来，不管我是不是正在搜台
        if ((ret > RESULT_COMMON_RESERVE) && (ret != RESULT_STOP_SEARCH))
        {
            break;
        }

#ifdef SUPPORT_ASET_TEST
        if ((g_app_info_state.stub_tools_type == STUB_PC_TOOL_WAVES_ASET_MODE) || (g_app_info_state.stub_tools_type == STUB_PC_TOOL_ASET_EQ_MODE))
        {
            aset_test_loop_deal();
        }
#endif 
        //挂起10ms 多任务交互
        sys_os_time_dly(1);
    }

    return ret;
}

