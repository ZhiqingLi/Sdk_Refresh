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
#include "common_ota.h"

uint8 g_temp_buffer[512];

void parse_ota_file_info(part_head_t *part_head,uint32 fp, uint32 file_sys_id)
{
    uint32 i;
        
    vfs_file_read(file_sys_id, g_temp_buffer, sizeof(g_temp_buffer), fp);

    libc_print("part total:", part_head->part_total, 2);

    for(i = 0; i < part_head->part_total; i++)
    {
        libc_print("part ID: ", part_head->part_item[i].part_id, 2);
        libc_print("part size: ", part_head->part_item[i].part_size, 2);
        libc_print("part offset: ", part_head->part_item[i].part_offset, 2);
    }
}

void write_part_data(ota_write_item_t *p_write_item, uint32 fp, uint32 file_sys_id)
{
    uint32 once_write_size;
    uint32 write_size = p_write_item->part_size;
    ota_write_param_t write_param;

    once_write_size = 1;

    write_param.data_buffer = g_temp_buffer;
    write_param.part_id = p_write_item->part_id;
    write_param.part_base_offset = p_write_item->part_offset;
    write_param.part_write_offset = 0;
    write_param.data_len = sizeof(g_temp_buffer)/512;

    while(write_size > 0)
    {
        if(write_size > once_write_size)
        {
            once_write_size = 1;
        }
        else
        {
            once_write_size = write_size;
        }

        vfs_file_read(file_sys_id, g_temp_buffer, sizeof(g_temp_buffer), fp);

        base_ota_part_write(&write_param);

        write_param.part_write_offset += (uint16)once_write_size;

        write_size -= once_write_size;
    }

    libc_print("part update over", 0, 0);

}

void sys_reboot(void)
{
    sys_local_irq_save();
    
    ENABLE_WATCH_DOG(1);

    while(1)
    {
        ;//nothing
    }  
}

void config_ota_write_tag(void)
{
    restore_breakpoint_t g_ota_bkinfo;

    g_ota_bkinfo.dowmload_statue = 2;
    g_ota_bkinfo.updata_statue = 0;
    g_ota_bkinfo.reboot_flg = 1;
    g_ota_bkinfo.ud_att = 1;
    sys_vm_write(&g_ota_bkinfo, VM_OTA, sizeof(g_ota_bkinfo));

}

void config_ota_test(uint32 fp, uint32 file_sys_id)
{    
    uint32 i;
    
    part_head_t *part_head;
    
    uint8 temp_buffer[128];

    ota_write_info_t *p_ota_write_info;

    sd_handle alarm_handle;

    sys_os_sched_lock();

    alarm_handle = sys_sd_fopen_ext("alarm1.mp3", 16);

    if(alarm_handle == NULL)
    {
        libc_print("ext file open err", 0, 0);
    }
    else
    {
        libc_print("ext file open ok", 0, 0);

        sys_sd_fread(alarm_handle, g_temp_buffer, 512);

        PRINT_DATA(g_temp_buffer, 512);

        sys_sd_fclose(alarm_handle);        
    }
    base_fw_read(0X1580,g_temp_buffer,1);
    PRINT_DATA(g_temp_buffer, 512);

    p_ota_write_info = (ota_write_info_t *)temp_buffer;

    part_head = (part_head_t *)g_temp_buffer;

    parse_ota_file_info(part_head, fp, file_sys_id);

    i = 0;

    p_ota_write_info->part_total = part_head->part_total;

    for(i = 0; i < p_ota_write_info->part_total; i++)
    {
        p_ota_write_info->part_info[i].part_id = (uint8)part_head->part_item[i].part_id;

        p_ota_write_info->part_info[i].part_size = (uint16)(part_head->part_item[i].part_size>>9);

        p_ota_write_info->part_info[i].write_mode = 0;

        p_ota_write_info->part_info[i].part_checksum = (uint16)part_head->part_item[i].check_sum;
    }

    p_ota_write_info = base_ota_part_write_query(p_ota_write_info);

    if(p_ota_write_info != NULL)
    {
        for(i = 0; i < p_ota_write_info->part_total; i++)
        {
            libc_print("part write enable", 0, 0);

            //找到当前升级数据区最开始的地方
            vfs_file_seek(file_sys_id, part_head->part_item[i].part_offset, SEEK_SET, fp);       

            write_part_data(&(p_ota_write_info->part_info[i]), fp, file_sys_id);
        }
        
        config_ota_write_tag();

        sys_reboot();
    }
    else
    {
        libc_print("part unable write", 0, 0);
    }

    while(1)
    {
        ;//nothing
    }
}

