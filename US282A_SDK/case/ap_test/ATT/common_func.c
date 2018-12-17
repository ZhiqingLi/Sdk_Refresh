/*******************************************************************************
 *                              US212A
 *                            Module: Picture
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-19 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     user1_main.c
 * \brief    picture主模块，负责进程初始化，退出处理，场景调度
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"

//卡驱动文件名
static const uint8 card_drv_name[] = "card.drv";
static const uint8 uhost_drv_name[] = "uhost.drv";

extern void tick_ISR(void)
__FAR__;

/******************************************************************************/
/*!
 * \par  Description:
 *  装载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
int32 _config_fs_init(uint8 disk_type)
{
    uint8 drv_type;
    uint8 *drv_name;
    int32 file_sys_id;

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8 *) card_drv_name;
    }
    else
    {
        drv_type = DRV_GROUP_STG_CARD;
        drv_name = (uint8 *) card_drv_name;
    }

    //装载物理介质
    if (sys_drv_install(drv_type, 0, drv_name) != 0)
    {
        return -2;
    }

    //装载文件系统
    file_sys_id = sys_mount_fs(drv_type, disk_type, 0);

    if (file_sys_id == -1)
    {
        //卸载驱动程序
        sys_drv_uninstall(drv_type);
        return -3;
    }

    return file_sys_id;
}
/******************************************************************************/
/*!
 * \par  Description:
 *  卸载物理介质和文件系统
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      config
 * \note
 *******************************************************************************/
bool _config_fs_deinit(uint8 disk_type, int32 *file_sys_id)
{
    int32 ret_val;
    uint8 drv_type;

    if (*file_sys_id != -1)
    {
        ret_val = sys_unmount_fs(*file_sys_id);
        *file_sys_id = -1;
    }
    else
    {
        return TRUE;
    }

    if (0 > ret_val)
    {
        return FALSE;
    }

    if (disk_type == DISK_H)
    {
        drv_type = DRV_GROUP_STG_CARD;
    }
    else if (disk_type == DISK_U)
    {
        drv_type = DRV_GROUP_STG_UHOST;
    }
    else
    {
        return FALSE;
    }

    ret_val = sys_drv_uninstall(drv_type);

    if (0 > ret_val)
    {
        return FALSE;
    }

    return TRUE;
}

uint32 tick_ISR_install(void)
{
    uint32 timer_id;
    timer_id = sys_set_irq_timer1(tick_ISR, 100);
    tick_cnt = 0;
    return timer_id;
}

void tick_ISR_uninstall(uint32 timer_id)
{
    sys_del_irq_timer1((void *) timer_id);
}

static void sys_reboot(void)
{       
    DEBUG_ATT_PRINT("reboot...", 0, 0);
    
    sys_local_irq_save();
    
    ENABLE_WATCH_DOG(1);
    
    while(1);
}


int32 act_test_report_test_log(uint32 ret_val, uint32 test_id)
{
    if (ret_val == FALSE)
    {
        att_write_test_info("test fail: ", test_id, 1);
        write_log_file(FALSE);
        //sys_mdelay(5000);
        //sys_reboot(); 
        led_flash_fail();
    }  
    else
    {
        att_write_test_info("test ok: ", test_id, 1);        
    }  
    
    att_write_test_info("   ", 0, 0);   
}
