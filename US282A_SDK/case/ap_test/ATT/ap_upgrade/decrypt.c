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

//解密数据结构体
PCFWDecrypt_struct g_decrypt;

typedef uint32 (*p_fw_decrypt_init)(PCFWDecrypt_struct *decrypt_rw);
typedef void (*p_fw_decrypt_run)(void *, void *, void *);

/******************************************************************************/
/*!
 * \par  Description:
 *      读取文件数据并解密
 * \param[in]    mode           0 初始化 1正常解密
 * \param[in]    read_len       读取的数据长度
 * \return       the read len
 * \note
 *******************************************************************************/
uint32 _read_file_translate(uint8 *read_buffer, uint32 read_len)
{
    uint32 read_file_offset;
    uint32 decrypt_mode;
    uint32 once_read_len;
    //uint32 true_read;
    uint32 need_read_len;
    uint32 true_read_offset;
    uint32 ret_val = 0;

    //vfs_file_tell(_para.g_file_sys_id, &read_file_offset, _para.file_handle);
    read_file_offset = g_upg_file_pos;

    if (read_file_offset < MIN_ONCE_READ_LEN)
    {
        decrypt_mode = 0;

        //前18k数据需要整体读取并解密
        if (read_file_offset != 0)
        {
            //vfs_file_seek(_para.g_file_sys_id, 0, SEEK_SET, _para.file_handle);
            g_upg_file_pos = 0;
        }
    } 
    else
    {
        decrypt_mode = 1;
    }

    //true_read = 1;

    need_read_len = read_len;

    //有可能出现需要多次读取数据，且跨越解密头的数据
    while (need_read_len != 0)
    {
        if (need_read_len < MIN_ONCE_READ_LEN)
        {
            once_read_len = need_read_len;
        }
        else
        {
            once_read_len = MIN_ONCE_READ_LEN;
        }

        //至少保证读取18k数据
        if ((once_read_len < MIN_ONCE_READ_LEN) && (decrypt_mode == 0))
        {
            once_read_len = MIN_ONCE_READ_LEN;
        }

        //if (true_read == 1)
        {
            //读取文件
            //vfs_file_read(_para.g_file_sys_id,  once_read_len, _para.file_handle);
            att_test_fread(g_decrypt.pInOutBuffer, once_read_len, g_upgrade_file_offset + g_upg_file_pos);
            
            g_upg_file_pos += once_read_len;

            if (decrypt_mode == 0)
            {
                g_decrypt.InOutLen = DECRYPT_USR_BUF_LEN;

                PRINT_INFO("firt decrypt init");
                PCFWDecryptInit(&g_decrypt);
                once_read_len = (uint32) g_decrypt.InOutLen;

                PRINT_INFO("firt decrypt over");
            }
            else
            {
                PCFWDecrypt(g_decrypt.pInOutBuffer, (void *) once_read_len, g_decrypt.pGLBuffer);
            }
        }

        if (once_read_len != 0)
        {
            if (decrypt_mode != 0)
            {
                libc_memcpy(read_buffer, g_decrypt.pInOutBuffer, once_read_len);

            }
            else
            {
                //判断解密头数据是否超过16k的长度
                if (once_read_len > read_file_offset)
                {
                    once_read_len = once_read_len - read_file_offset;
                }
                else
                {
                    //超过，则实际解密长度为18k减去应有的长度
                    once_read_len = MIN_ONCE_READ_LEN - read_file_offset;
                }

                if (read_file_offset != 0)
                {
                    true_read_offset = (uint32)(read_file_offset - (uint32) g_firmware_offset);
                }
                else
                {
                    true_read_offset = 0;
                }

                libc_memcpy(read_buffer, g_decrypt.pInOutBuffer + true_read_offset, once_read_len);
                decrypt_mode = 1;
            }

            need_read_len -= once_read_len;
            read_buffer += once_read_len;
        }
    }

    return read_len;
}

void decrypt_init(void)
{
    int32 file_len = 0;

    g_decrypt.initusebuffer = (uint8 *) DECRYPT_INIT_BUF_ADDR;
    g_decrypt.initusebufferlen = DECRYPT_INIT_BUF_LEN;
    g_decrypt.pInOutBuffer = (uint8 *) DECRYPT_USE_BUF_ADDR;
    g_decrypt.InOutLen = DECRYPT_USR_BUF_LEN;
    g_decrypt.pGLBuffer = (uint8 *) DECRYPT_GL_BUF_ADDR;

    //vfs_file_get_size(_para.g_file_sys_id, &file_len, _para.file_handle, 0);
    g_decrypt.FileLength = (long) g_upg_file_len;

    g_upg_file_pos = 0;

    g_write_percent = 0;

    libc_memset(&_para, 0, sizeof(_para));
} 

/******************************************************************************/
/*!
 * \par  Description:
 *	  读取文件数据并解密
 * \param[in]    read_len       读取的数据长度
 * \param[in]    fp             文件句柄
 * \return       the result
 * \note
 *******************************************************************************/
uint32 _read_file(uint32 read_len)
{
    uint32 read_bytes;

    read_bytes = _read_file_translate(BUFFER_ADDR, read_len);

    return read_bytes;
}

