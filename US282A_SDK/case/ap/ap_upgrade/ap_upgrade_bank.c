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
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_upgrade.h"

static const uint8 lfi_name[] = { "FWIMAGE FW " };
static const uint8 nor_id_name[] = { "NOR_ID  BIN" };
static const uint8 brec_name[] = { "NORBREC BIN" };
static const uint8 mbrec_name[] = {"NORMBRECBIN"};
static const uint8 part_table_name[] = {"PARTTBL BIN"};
static const uint8 part_data_name[] = {"PART    BIN"};
static const uint8 nor_drv_name[] = "nor_upg.drv";
/*!
 * \brief
 *      mbrec head info, 16bytes
 */
typedef struct
{
    UINT16 reserved0[3];
    UINT16 brec_backup_blk_addr;    //brec备份块起始地址,扇区地址
    UINT8 reserved1[8];             //保留字节
}mbrec_head_info;

/******************************************************************************/
/*!
 * \par  Description:
 *      获取LFI数据在文件中偏移
 * \param[in]    buf   data buffer address
 * \param[in]    buffer_len   buffer length
 * \param[in]    key_word 关键字字符串
 * \param[in]    key_len  关键字字符串长度
 * \param[in]    offset  偏移
 * \return       AFI_DIR_t* 目录项指针
 * \retval
 * \note
 *******************************************************************************/
static AFI_DIR_t *check_key_offset_exist(char *buffer, uint32 buffer_len, const uint8 *key_word, uint8 key_len,
        uint8 offset)
{
    uint32 str_len = 0;

    while (str_len < buffer_len)
    {
        if (libc_memcmp(&buffer[str_len + offset], key_word, (uint32) key_len) == 0)
        {
            return (AFI_DIR_t *) (&buffer[str_len]);
        }
        else
        {
            str_len += 32;
        }
    }

    return NULL;
}

void parse_firmware(void)
{
    mbrec_head_info *mbrec_head;
    
    vfs_file_seek(_para.g_file_sys_id, _para.mbrec_offset, SEEK_SET, _para.file_handle);

    _read_file(512);

    mbrec_head = (mbrec_head_info *)BUFFER_ADDR;

    if(mbrec_head->brec_backup_blk_addr != 0)
    {
        _para.lfi_start_addr = LFI_OFFSET;

        _para.fw_mode = 1;
    }
    else
    {
        _para.fw_mode = 0;

        _para.lfi_start_addr = ((_para.brec_length + 511) / 512) + 2;
    }

    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *  搜索目录项
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      search_content
 * \note
 *******************************************************************************/

int search_content(void)
{
    AFI_DIR_t *dir_p = NULL;

    PRINT_INFO("start decrypt...");

    decrypt_init();

    PRINT_INFO("decrypt init over");

    _read_file(MIN_ONCE_READ_LEN);
    
    if(base_upg_init(BUFFER_ADDR) != 0)
    {
        PRINT_INFO("decrypt file err");
        
        return -1;        
    }

    g_firmware_offset = (uint32)(DECRYPT_USR_BUF_LEN - g_decrypt.InOutLen);
    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, lfi_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.lfi_length = dir_p->length;
        _para.lfi_offset = dir_p->offset + g_firmware_offset;
    }
    else
    {
        return -1;
    }

    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, nor_id_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.nor_id_length = dir_p->length;
        _para.nor_id_offset = dir_p->offset + g_firmware_offset;
    }
    else
    {
        return -1;
    }

    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, brec_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.brec_length = dir_p->length;
        _para.brec_offset = dir_p->offset + g_firmware_offset;
    }
    else
    {
        return -1;
    }

    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, mbrec_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.mbrec_length = dir_p->length;
        _para.mbrec_offset = dir_p->offset + g_firmware_offset;
    }
    else
    {
        return -1;
    }

    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, part_table_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.part_tbl_length = dir_p->length;
        _para.part_tbl_offset = dir_p->offset + g_firmware_offset;
    }

    dir_p = check_key_offset_exist(BUFFER_ADDR, (uint32) g_decrypt.InOutLen, part_data_name, 11, 0);
    if (dir_p != NULL)
    {
        _para.part_length = dir_p->length;
        _para.part_offset = dir_p->offset + g_firmware_offset;
    }
    
    
    //get USB SN type
    sys_get_fw_info(&(_para.udisk_setting), FW_INFO_USB_SN_TYPE_ADDR, FW_INFO_USB_SN_TYPE_LEN);

    //获取USB序列号
    sys_get_fw_info(&(_para.UsbSetupInfo), FW_INFO_USB_ID_ADDR, FW_INFO_USB_ID_LEN);

    //获取相关盘符信息
    sys_get_fw_info(&(_para.CapInfo), FW_INFO_CAPINFO_ADDR, FW_INFO_CAPINFO_LEN);
    _para.progress = SEARCH_FMU;

    parse_firmware();

    if(_para.fw_mode == 1)
    {
        if(_para.part_tbl_length == 0)
        {
            return -1;
        }
    }    

    PRINT_INFO("search file over");

    //擦除VRAM
    base_clear_vram(); 

    //解除写保护
    base_set_disable_write_protect();

    sys_drv_uninstall(DRV_GROUP_STG_BASE);
    
    if (sys_drv_install(DRV_GROUP_STG_BASE, 0, nor_drv_name) != 0)
    {
        return -1;
    }     

    return 0;
}

static void upgread_result_display(uint8 mode)
{   
    if (mode == 0)
    {
#if(SUPPORT_LED_DRIVER == 1)        
        //量产失败
        led_display(NUMBER1, NUM_F, 1);
        led_display(NUMBER2, NUM_A, 1);
        led_display(NUMBER3, NUM_I, 1);
        led_display(NUMBER4, NUM_L, 1);
#endif        
        _para.upgrade_type = 1; //不重启
        remind_voice(500);
    }
    else
    {
#if(SUPPORT_LED_DRIVER == 1)
        led_display(NUMBER1, NUM_S, 1);
        led_display(NUMBER2, NUM_U, 1);
        led_display(NUMBER3, NUM_C, 1);
        led_display(NUMBER4, NUM_C, 1);
#endif        
        remind_voice(1000);
    }
}

app_result_e upgrade_deal(void)
{
    app_result_e result;

    if (upgrade_fw_check() == FALSE)
    {
        upgread_result_display(0);

        return RESULT_NEXT_FUNCTION;
    }

    if (search_content() != 0)
    {
        upgread_result_display(0);

        return RESULT_NEXT_FUNCTION;
    }

    if (upgrade_fw_write() == TRUE)
    {
        upgread_result_display(1);
    }
    else
    {
        upgread_result_display(0);
    }

    //量产成功或进行到写nor阶段后失败，都重启
    //act_writel(0x51, WD_CTL);
    CLEAR_WATCH_DOG;

    ENABLE_WATCH_DOG(3);
    while (1)
    {
        ;//nothing
    }
}
