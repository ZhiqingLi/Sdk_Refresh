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

_para_t _para =
{ 0 };

typedef struct
{
    uint32 cache_addr;
    uint32 cache_len;
} decrypt_cache_t;

decrypt_cache_t g_decrypt_cache;

struct fw_decrypt_operations g_decrypt_ops;

uint32 g_firmware_offset;

#pragma __PRQA_IGNORE_START__

void fresh_display(void)
{
    int temp = (_para.g_write_sector * 100) / ((_para.brec_length + _para.lfi_length) >> 9);
    led_clear_screen();
    //led_display(NUMBER1, NUM_U, 1);
    led_display(NUMBER2, temp / 10, 1);
    led_display(NUMBER3, temp % 10, 1);
    //led_display(NUMBER4, NUM_E, 1);
    
    CLEAR_WATCH_DOG;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      计算两字节校验和
 * \param[in]    buf   data buffer address
 * \param[in]    len   data length, 2byte uint
 * \param[out]   none
 * \return       the checksum
 * \retval
 * \note
 *******************************************************************************/
uint16 check_sum16(uint16 *buf, UINT32 len)
{
    uint32 index;
    uint16 sum = 0;

    for (index = 0; index < len; index++)
    {
        sum += buf[index];
    }

    return sum;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      封装底层写逻辑扇区接口
 * \param[in]    pbuf           写入数据buffer
 * \param[in]    addr           写入逻辑区地址
 * \param[in]    len            写入长度
 * \return       the result
 * \retval       TRUE succeed  FALSE fail
 * \note
 *******************************************************************************/
int32 ud_sector_write(void *pbuf, uint32 addr, uint32 len)
{
    int32 ret_val;
    ud_sector_param_t ud_sec_param;

    if ((addr & 0x1ff) != 0)
    {
        //非512字节对齐
        return 0;
    }

    ud_sec_param.file_offset = (addr >> 9);
    ud_sec_param.sec_num = (len + 0x1ff) >> 9;
    ud_sec_param.sram_addr = (uint32) pbuf;

    ret_val = (int32) base_fw_write(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);
    //ret_val = (int32) base_fw_read(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);
    if (ret_val == -1)
    {
        return 0;
    }
    else
    {
        _para.g_write_sector += ud_sec_param.sec_num;
        fresh_display();
        return 1;
    }
}
/******************************************************************************/
/*!
 * \par  Description:
 *      封装底层读逻辑扇区接口
 * \param[in]    pbuf           读入数据buffer
 * \param[in]    addr           读入逻辑区地址
 * \param[in]    len            读入长度
 * \return       the result
 * \retval       0 succeed  -1 fail
 * \note
 *******************************************************************************/
int32 ud_sector_read(void *pbuf, uint32 addr, uint32 len)
{
    int32 ret_val;
    ud_sector_param_t ud_sec_param;

    if ((addr & 0x1ff) != 0)
    {
        //非512字节对齐
        return 0;
    }

    ud_sec_param.file_offset = addr >> 9;
    ud_sec_param.sec_num = (len + 0x1ff) >> 9;
    ud_sec_param.sram_addr = (uint32) pbuf;

    ret_val = (int32) base_fw_read(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);
    if (ret_val == -1)
    {
        return 0;
    }
    return 1;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      封装底层brec逻辑扇区接口
 * \param[in]    pbuf           读入数据buffer
 * \param[in]    addr           读入逻辑区地址
 * \param[in]    len            读入长度
 * \return       the result
 * \retval       0 succeed  -1 fail
 * \note
 *******************************************************************************/
bool brec_sector_write(void *pbuf, uint32 addr, uint32 len)
{
    int32 ret_val;
    ud_sector_param_t ud_sec_param;
    uint16 *p_lfi_length;
    uint32 lfi_sector_total;
    if ((addr & 0x1ff) != 0)
    {
        //非512字节对齐
        return FALSE;
    }

    if (addr == 0)
    {
        lfi_sector_total = (_para.lfi_length + 511) / 512;
        p_lfi_length = (uint16 *) (pbuf);
        p_lfi_length += 4;
        *p_lfi_length = (uint16)(lfi_sector_total);
    }

    ud_sec_param.file_offset = addr >> 0x9;
    ud_sec_param.sec_num = (len + 0x1ff) >> 9;
    ud_sec_param.sram_addr = (uint32) pbuf;

    if(_para.fw_mode == 1)
    {
        ud_sec_param.file_offset |= (1 << 31);    
    }

    ret_val = (int32) base_brec_write(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);
    //ret_val = (int32) base_fw_read(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);
    if (ret_val == -1)
    {
        return FALSE;
    }
    else
    {
        _para.g_write_sector += ud_sec_param.sec_num;
        return TRUE;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *      计算四字节校验和
 * \param[in]    buf   data buffer address
 * \param[in]    len   data length, 4byte uint
 * \param[out]   none
 * \return       the checksum
 * \retval
 * \note
 *******************************************************************************/
uint32 check_sum32(uint32 *buf, uint32 len)
{
    uint32 index;
    uint32 sum = 0;

    for (index = 0; index < len; index++)
    {
        sum += buf[index];
    }

    return sum;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      读出brec修改nor id后写入nor
 * \param[in]    None           文件句柄
 * \return       bool
 * \retval       写入操作时返回值
 * \note
 *******************************************************************************/
bool brec_write(void)
{
    int32 ret_val;
    uint16 sector_num;
    uint32 write_addr = 0;

    uint16 sector_total = (uint16)((_para.brec_length + 511) / 512);
    vfs_file_seek(_para.g_file_sys_id, _para.brec_offset, SEEK_SET, _para.file_handle);

    _para.progress = WRITE_BREC;
    while (sector_total > 0)
    {
        if (sector_total > 32)
        {
            sector_num = 32;
        }
        else
        {
            sector_num = sector_total;
        }

        ret_val = _read_file(16 * 1024);
        if (ret_val == 0)
        {
            //解密失败
            return FALSE;
        }
        ret_val = brec_sector_write((uint32 *) BUFFER_ADDR, write_addr, sector_num << 9);
        if (ret_val == 0)
        {
            return FALSE;
        }
        write_addr += sector_num << 9;
        sector_total -= sector_num;
    }

    PRINT_INFO("brec write over");

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      读出brec修改nor id后写入nor
 * \param[in]    None           文件句柄
 * \return       bool
 * \retval       写入操作时返回值
 * \note
 *******************************************************************************/
bool mbrec_write(void)
{
    int32 ret_val;
    ud_sector_param_t ud_sec_param;

    uint16 sector_total = (uint16)((_para.mbrec_length + 511) / 512);
    vfs_file_seek(_para.g_file_sys_id, _para.mbrec_offset, SEEK_SET, _para.file_handle);

    _para.progress = WRITE_MBREC;

    ret_val = _read_file(_para.mbrec_length);
    if (ret_val == 0)
    {
        //解密失败
        return FALSE;
    }
    
    ud_sec_param.file_offset = 0;
    ud_sec_param.sec_num = sector_total;
    ud_sec_param.sram_addr = (uint32) BUFFER_ADDR;

    ret_val = (int32) base_mbrec_write(ud_sec_param.file_offset, ud_sec_param.sram_addr, ud_sec_param.sec_num);

    if (ret_val == -1)
    {
        return FALSE;
    }
  
    PRINT_INFO("mbrec write over");

    return TRUE;
}


/******************************************************************************/
/*!
 * \par  Description:
 *      校验写入固件区的数据是否正确
 * \param[in]    none
 * \return       upgrade_status_e
 * \retval       校验结果返回值
 * \note
 *******************************************************************************/
bool lfi_check(void)
{
    LFIHead_t *lfi_head;
    LFIHead_dir_t *lfi_dir;
    uint32 check_sum;
    uint32 index;
    uint8 *buf;
    uint32 sector_num;
    uint32 sector_total;
    uint32 addr;

    lfi_head = (LFIHead_t *) BUFFER_ADDR;
    lfi_dir = (LFIHead_dir_t *) BUFFER_ADDR;

    //读入前8K
    ud_sector_read((uint32 *) BUFFER_ADDR, _para.lfi_start_addr * 512, 0x2000);

    //计算头部校验和
    check_sum = check_sum16((uint16*) BUFFER_ADDR, (sizeof(LFIHead_t) - 2) / 2);
    if (check_sum != lfi_head->Headchecksum)
    {
        return FALSE;
    }

    //SD区文件校验用另外8K空间
    buf = (uint8 *) (BUFFER_ADDR + 0x2000);

    for (index = 0; index < 240; index++)
    {
        if (0 == lfi_dir->diritem[index].filename[0])
        {
            break;
        }

        sector_total = (uint32)(lfi_dir->diritem[index].length / 0x200);
        addr = (lfi_dir->diritem[index].offset << 9);
        addr += _para.lfi_start_addr * 512;
        check_sum = 0;

        while (sector_total != 0)
        {
            if (sector_total > 16)
            {
                sector_num = 16;
            }
            else
            {
                sector_num = sector_total;
            }

            if (TRUE != ud_sector_read(buf, addr, sector_num << 9))
            {
                return FALSE;
            }

            check_sum = check_sum + check_sum32((uint32*) buf, sector_num * 512 / 4);

            addr += (sector_num << 9);
            sector_total -= sector_num;
        }

        if (lfi_dir->diritem[index].checksum != check_sum)
        {
            return FALSE;
        }

    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *      读取固件升级文件并写入固件区
 * \param[in]    None           文件句柄
 * \return       bool
 * \retval       写入操作时返回值
 * \note
 *******************************************************************************/
bool lfi_write(void)
{
    int32 ret_val;
    uint32 sector_num;
    LFIHead_t *lfi_head;
    uint16 check_sum;
    uint32 write_addr;

    uint32 sector_total = (uint32)((_para.lfi_length + 511) / 512);

    lfi_head = (LFIHead_t *) BUFFER_ADDR;

    //准备解析FW
    vfs_file_seek(_para.g_file_sys_id, _para.lfi_offset, SEEK_SET, _para.file_handle);

    //读取并解析FW前16K数据
    ret_val = _read_file(16 * 1024);
    if (ret_val == 0)
    {
        //解密失败
        return FALSE;
    }

    _para.progress = READ_LFI;

    libc_memcpy(&lfi_head->udisk_setting, &_para.udisk_setting, sizeof(_para.udisk_setting));

    libc_memcpy(&lfi_head->UsbSetupInfo, _para.UsbSetupInfo, sizeof(_para.UsbSetupInfo));

    libc_memcpy(&(lfi_head->CapInfo.vm_disk_cap), _para.CapInfo, sizeof(_para.CapInfo));

    //计算校验和
    check_sum = check_sum16((uint16*) BUFFER_ADDR, (sizeof(LFIHead_t) - 2) / 2);

    //写入头部校验和
    lfi_head->Headchecksum = check_sum;

    write_addr = _para.lfi_start_addr * 512;

    //写入16K数据
    ret_val = ud_sector_write((uint32 *) BUFFER_ADDR, write_addr, 16 * 1024);

    if (ret_val == 0)
    {
        return FALSE;
    }
    else
    {
        write_addr += 16 * 1024;
    }

    sector_total -= 32;

    while (sector_total > 0)
    {
        if (sector_total > 32)
        {
            sector_num = 32;
        }
        else
        {
            sector_num = sector_total;
        }

        ret_val = _read_file(16 * 1024);
        if (ret_val == 0)
        {
            //解密失败
            return FALSE;
        }

        ret_val = ud_sector_write((uint32 *) BUFFER_ADDR, write_addr, sector_num << 9);
        if (ret_val == 0)
        {
            return FALSE;
        }

        write_addr += sector_num << 9;
        sector_total -= sector_num;
    }

    _para.progress = WRITE_LFI;

    PRINT_INFO("lfi write over");

    return TRUE;
}

void remind_voice(uint32 interval)
{
    uint8 i;

    if (_para.upgrade_type == UPGRADE_TYPE_FACTORY)
    {
        while (1)
        {
            CLEAR_WATCH_DOG;
            upgrade_start_key_tone();
            sys_mdelay(interval);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            upgrade_start_key_tone();
            sys_mdelay(interval);
        }
    }
}

bool part_write(void)
{
    int32 ret_val;
    uint16 sector_num;
    uint32 write_addr = 0;

    uint16 sector_total = (uint16)((_para.part_tbl_length + 511) / 512);

    _para.progress = WRITE_PART_TABLE;
    
    vfs_file_seek(_para.g_file_sys_id, _para.part_tbl_offset, SEEK_SET, _para.file_handle);

    ret_val = _read_file(sector_total << 9);
      
    if (ret_val == 0)
    {
        return FALSE;
    }

    base_part_table_write((uint8 *) BUFFER_ADDR);
 
    sector_total = (uint16)((_para.part_length + 511) / 512);
    
    vfs_file_seek(_para.g_file_sys_id, _para.part_offset, SEEK_SET, _para.file_handle);

    _para.progress = WRITE_PART;
    
    while (sector_total > 0)
    {
        if (sector_total > 32)
        {
            sector_num = 32;
        }
        else
        {
            sector_num = sector_total;
        }

        ret_val = _read_file(16 * 1024);
        if (ret_val == 0)
        {
            //解密失败
            return FALSE;
        }
        ret_val = base_part_data_write((uint32 *)BUFFER_ADDR, write_addr, sector_num << 9);
        if (ret_val == 0)
        {
            return FALSE;
        }
        write_addr += sector_num << 9;
        sector_total -= sector_num;
    }

    PRINT_INFO("part write over");
  
    return TRUE;    
}
/******************************************************************************/
/*!
 * \par  Description:
 *  装载物理介质和文件系统，并检测是否存在升级文件
 * \param[in]    disk_type 磁盘类型
 * \param[out]   none
 * \return       none
 * \ingroup      upgrade
 * \note
 *******************************************************************************/
bool upgrade_fw_write(void)
{
    int32 ret_val;
    uint8 *p_temp_buffer = (uint8 *) BT_VRAM_BUFF;

    if(brec_write() != TRUE)
    {
        goto err_ret; 
    }

    if(lfi_write() != TRUE)
    {
        goto err_ret; 
    }

    if(_para.fw_mode == 1)  
    {
        if(part_write() != TRUE)
        {
            goto err_ret;    
        } 
    }  

    if(mbrec_write() != TRUE)
    {
        goto err_ret; 
    }

    if(lfi_check() != TRUE)
    {
        goto err_ret;    
    } 

    //立即退出，避免执行bank函数
    return TRUE;
    
err_ret: 
    return FALSE;
}
#pragma __PRQA_IGNORE_END__
