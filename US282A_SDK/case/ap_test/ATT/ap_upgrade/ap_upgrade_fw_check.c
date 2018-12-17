/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     mainmenu_main.c
 * \brief    MainMenuµÄmainº¯ÊýÄ£¿é
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"
#include "ap_upgrade.h"

const uint8 fw_ascfile_name[] = "UPGRADE.HEX";

bool upgrade_fw_check(void)
{
    uint32 i;
    atf_head_t *atf_head;
    uint8 *data_buffer = (uint8 *)ATT_MPDATA_TEMP_BUFFER;
    
    att_test_fread(data_buffer, 512, 0);

    atf_head = (atf_head_t *) data_buffer;

    for (i = 0; i < (512 / sizeof(atf_dir_t)); i++)
    {
        if (libc_memcmp(atf_head->atf_dir[i].filename, fw_ascfile_name, 10) == 0)
        {
            g_upgrade_file_offset = atf_head->atf_dir[i].offset;
            g_upg_file_len = atf_head->atf_dir[i].length;
            return TRUE;
        }
    }  

    return FALSE;
}


