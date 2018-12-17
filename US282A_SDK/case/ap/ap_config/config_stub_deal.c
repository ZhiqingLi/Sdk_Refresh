/*******************************************************************************
 *                              US212A
 *                            Module: Config
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     config_main.c
 * \brief    开关机的处理
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_config.h"
#include "stub_interface.h"

//const uint8 aset_ack[] = {0xAE, 0x03, 0xfe, 0x00, 0x08, 0x00, 0x00, 0x00};

app_result_e open_stub(void)
{
    //bool bt_dt_debug_flag;
    uint8 stub_type;
    app_result_e result = RESULT_NULL;
    uint8 need_uninstall_led = FALSE;

    input_gui_msg_t gui_msg;
    int i = 0;

    if(g_app_info_state.stub_phy_type == STUB_PHY_USB)
    {
        /* UART stub 模式下不进 UDISK */
        //2秒钟以内按下按键就进入USB音箱，可以升级固件
        while (1)
        {
            if (get_gui_msg(&gui_msg) == TRUE)
            {
                result = RESULT_USB_TRANS;
                return result;
            }
            sys_os_time_dly(1);
            if (i > 200)
            {
                break;
            }
            i++;
        }
    }
    g_config_esd_restart = FALSE;
    //等待PA OK后再进行测试
    config_wait_pa_ok();
    
    result = com_sys_install_stub(0);

    return result;
}
