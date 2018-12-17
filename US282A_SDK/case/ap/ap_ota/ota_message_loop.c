#include "ap_ota.h"

app_result_e get_message_loop(void)
{
    app_result_e result = RESULT_NULL;

    while (1)
    {
        result = com_view_loop();
		if (result == RESULT_APP_QUIT)
		{
			PRINT_INFO(" com_view_loop result: RESULT_APP_QUIT ");
			break;
		}
		
        if (g_need_draw == TRUE)
        {
            com_view_update(APP_VIEW_ID_MAIN);

            g_need_draw = FALSE;
        }
		
        //挂起10ms，多任务调度
        sys_os_time_dly(1);
	}
    return result;
}
