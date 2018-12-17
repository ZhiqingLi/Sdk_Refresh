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
#include "ap_manager_test.h"

uint32 g_upgrade_file_offset;
uint32 g_upg_file_pos;
uint32 g_upg_file_len;

static const uint8 lfi_name[] =
{ "FWIMAGE FW " };
static const uint8 nor_id_name[] =
{ "NOR_ID  BIN" };
static const uint8 brec_name[] =
{ "NORBREC BIN" };
static const uint8 mbrec_name[] =
{ "NORMBRECBIN" };
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
    UINT16 brec_backup_blk_addr; //brec备份块起始地址,扇区地址
    UINT8 reserved1[8]; //保留字节
} mbrec_head_info;

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

    //vfs_file_seek(_para.g_file_sys_id, _para.mbrec_offset, SEEK_SET, _para.file_handle);
    g_upg_file_pos = _para.mbrec_offset;

    _read_file(512);

    mbrec_head = (mbrec_head_info *) BUFFER_ADDR;

    if (mbrec_head->brec_backup_blk_addr != 0)
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

    print_log("start decrypt...");

    decrypt_init();

    print_log("decrypt init over");

    _read_file(MIN_ONCE_READ_LEN);


    if (base_upg_init(BUFFER_ADDR) != 0)
    {
        print_log("decrypt file err");

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
	

    return 0;
}

static void sys_reboot(void)
{    
    DEBUG_ATT_PRINT("reboot...", 0, 0);
    
    sys_local_irq_save();
    
    ENABLE_WATCH_DOG(1);
    
    while(1);
}


int32 att_fw_swtch_deal(void)
{
    int32 ret_val;
    att_swtich_fw_arg_t *att_switch_fw_arg;

    libc_memset((uint8 *)STUB_ATT_RW_TEMP_BUFFER, 0, 40);

    att_switch_fw_arg = (att_swtich_fw_arg_t *)STUB_ATT_RW_TEMP_BUFFER;
    
	//小机重启到ATT工具枚举的超时时间
    att_switch_fw_arg->reboot_timeout = 60;

    ret_val = att_write_data(STUB_CMD_ATT_REBOOT_TIMEOUT, 32, STUB_ATT_RW_TEMP_BUFFER);

    if(ret_val == 0)
    {
        ret_val = att_read_data(STUB_CMD_ATT_ACK, 0, STUB_ATT_RW_TEMP_BUFFER);

        return TRUE;
    }    
}


void reboot_to_card_product(void)
{
    uint8* buf = (uint8*) STUB_ATT_READ_BANK_BUFFER;

    sys_os_sched_lock();

    DEBUG_ATT_PRINT("reboot_to_card_product...", 0, 0);

    /* 擦除 MBREC
     */
    libc_memset(buf, 0, 512);

    att_fw_swtch_deal();

    // base_mbrec_write(0, buf, 1);
    {
        void (*PHY_CPU_Write)(uint32, uint8*, uint32) = (void*)0xbfc0dd95;
        PHY_CPU_Write(0x0, buf, 512);
    }

    /* 重启从 FLASH 引导失败后再从卡启动烧录固件
     */
    ENABLE_WATCH_DOG(0);

    while (1)
        ;
}

int32 check_upg_drv(void)
{
    sd_handle upg_handle;

    upg_handle = sys_sd_fopen(nor_drv_name);

    if (upg_handle != 0)
    {
        sys_sd_fclose(upg_handle);
        return TRUE;    
    }

    return FALSE;
}

int32 upgrade_deal(void)
{
    int retry_count = 0;
    int irq_flag;
    int fp;

    if ((g_att_version == 1) && (g_support_norflash_wp == TRUE))
    {
        if (check_upg_drv() == FALSE)
        {
            reboot_to_card_product();    
        }
        
        if (g_test_ap_info->stub_phy_type == STUB_PHY_USB)
        {
            irq_flag = act_readl(INTC_MSK);
            DEBUG_ATT_PRINT("intc msk:", irq_flag, 2);
            act_writel(act_readl(INTC_MSK) & (~(1 << INTC_MSK_USB_IM)), INTC_MSK);
        }
        else
        {
            irq_flag = sys_local_irq_save();
        }
    }
    else
    {
        reboot_to_card_product();
    }

    start: if (upgrade_fw_check() == FALSE)
        goto retry;

    if (search_content() != 0)
        goto retry;

    if (!upgrade_fw_write())
        goto retry;

    if (g_test_ap_info->stub_phy_type == STUB_PHY_USB)
    {
        act_writel(irq_flag, INTC_MSK);
    }
    else
    {
        sys_local_irq_restore(irq_flag);
    }

    return TRUE;

    retry: if (++retry_count < 3)
        goto start;

    return FALSE;
}

/*
 * 量产测试.
 */
test_result_e act_test_product_test(void *arg_buffer)
{
    int32 ret_val;
    return_result_t *return_data;

    //对于ATS2825可能重启之后要再次发START命令，需要区分之前有没有
    //量产过，如果已经量产过，直接上报结果并返回就OK了
    if (g_skip_product_test == FALSE)
    {    
        ret_val = upgrade_deal();
    }
    else
    {
        ret_val = TRUE;
    }

    if (ret_val == TRUE)
    {
        att_fw_swtch_deal();
       
        stub_close();

        sys_reboot();
    }
    else
    {
        /* 量产失败后不自动重启, 避免反复重启量产, 需要断电或强制重启.
         */
        return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

        return_data->test_id = TESTID_PRODUCT_TEST;
        return_data->test_result = 0;

        act_test_report_result(return_data, 4);

        while (1)
            ;
    }
}

test_result_e act_test_prepare_product(void *arg_buffer)
{
    return_result_t *return_data;

    if(g_support_norflash_wp == TRUE)
    {
        base_set_disable_write_protect();        
    }  

    //该测试项必须卡中量产固件才可以测试成功
    if (sys_detect_disk(DRV_GROUP_STG_CARD) != -1)
    {
        reboot_to_card_product(); 
    }      

    return_data = (return_result_t *) (STUB_ATT_RETURN_DATA_BUFFER);

    return_data->test_id = TESTID_PREPARE_PRODUCT_TEST;

    return_data->test_result = 1;

    act_test_report_result(return_data, 4);
}
