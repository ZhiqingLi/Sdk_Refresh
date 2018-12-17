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
 * \file     music_channel_test.c
 * \brief    通道测试实现模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_manager_test.h"
#include "ap_autotest_channel_test.h"
#include "fm_interface.h"
#include "ap_autotest_channel_test_SNR.h"

#ifdef DEBUG_WRITE_CHANNEL_DATA
static const uint8 att_mic_file[] = "MIC.PCM";
static const uint8 att_linein_file[] = "LINEIN.PCM";

static uint8 tmp_buffer[30] _BANK_DATA_ATTR_;

/*! \cond UI_DRIVER_API */
/******************************************************************************/
/*!
 * \par  Description:
 *    内码字符串转unicode字符串
 * \param[in]   src：源字符串指针
 * \param[in]   len：要转换的内码字符串长度
 * \param[out]  dest：目的字符串指针
 * \return      是否转换成功
 * \retval      TRUE 转换成功
 * \retval      FALSE 转换失败
 * \ingroup     string
 * \note        目的字符串缓冲区由用户保证不会溢出，缓冲区大小需要加上结束符。
 *******************************************************************************/
bool char_to_unicode(uint8 *dest, uint8 *src, uint16 len)
{
    int s_cnt = 0;
    int d_cnt = 0;
    uint16 font_code;

    while (1)
    {
        if ((s_cnt >= len) || (src[s_cnt] == 0))//到尾或者结束符

        {
            break;
        }

        if ((uint8) src[s_cnt] >= 0x80)
        {
            font_code = 0x3f;
        }
        else
        {
            font_code = src[s_cnt];
        }
        s_cnt++;

        dest[d_cnt] = *((uint8*) &font_code); //低字节
        d_cnt++;
        dest[d_cnt] = *((uint8*) &font_code + 1); //高字节
        d_cnt++;
    }
    dest[d_cnt] = 0;
    d_cnt++;
    dest[d_cnt] = 0;
    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    将内码编码文件名转换为Unicode编码文件名。
 * \param[in]    file_name 内码编码文件名
 * \param[out]   file_name 输出Unicode编码文件名
 * \return       none
 * \ingroup      common_func
 * \par          exmaple code
 * \code
 例子1：将内码编码文件名转换为Unicode编码文件名
 const char const_rec_name[] = "rec_001.wav";
 uint8 new_rec_name[26];

 libc_memcpy(new_rec_name, const_rec_name, 12);
 com_ansi_to_unicode(new_rec_name);
 //接着，就可以使用 new_rec_name 来创建名字为 rec_001.wav 的文件了
 * \endcode
 * \note
 * \li  用于创建文件前把文件名转换为Unicode编码，以适应 exFat 文件系统。
 * \li  用户需自己保证 file_name 够存放Unicode编码文件名，比如 英文内码文件名
 *      rec_001.wav，转成Unicode编码文件名，需要缓冲区大小为 26 个字节，即
 *      Unicode编码标志字符0xfffe 2字节 + 11个字符 2 * 11字节 + 结束符0x0 0x0 2字节。
 * \li  受限于辅助缓冲区大小（52字节），文件名不能超过 24 个字符长度。
 *******************************************************************************/
void com_ansi_to_unicode(uint8 *file_name)
{
    uint16 ansi_len = (uint16) libc_strlen(file_name);
    uint16 i;

    //往后移动2字节，前面添加 0xfffe
    for (i = ansi_len; i > 0; i--)
    {
        file_name[(i - 1) + 2] = file_name[i - 1];
    }
    file_name[0] = 0xff;
    file_name[1] = 0xfe;
    file_name[ansi_len + 2] = 0x0;

    //把文件名转换为Unicode编码 com_name_buffer
    char_to_unicode(tmp_buffer, file_name + 2, ansi_len);

    //拷贝Unicode编码
    libc_memcpy(file_name + 2, tmp_buffer, ansi_len * 2 + 2);
}

void write_temp_file(uint8 file_index, uint8 *write_buffer, uint32 write_len)
{
    int file_handle;

    int ret_val;

    uint8 filename[64];

    libc_memset(filename, 0, 64);

    if(file_index == 0)
    {
        libc_memcpy(filename, att_mic_file, sizeof(att_mic_file));
    }
    else
    {
        libc_memcpy(filename, att_linein_file, sizeof(att_linein_file));
    }

    com_ansi_to_unicode(filename);

    //定位到根目录
    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    ret_val = vfs_file_dir_exist(g_file_sys_id, filename, 1);

    if(ret_val == 0)
    {
        file_handle = vfs_file_create(g_file_sys_id, filename, write_len);

        if(file_handle == 0)
        {
            libc_print("create recrod file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, write_buffer, write_len, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }
    else
    {
        file_handle = vfs_file_open(g_file_sys_id, filename, OPEN_MODIFY);

        if(file_handle == 0)
        {
            libc_print("open file error!\n", 0, 0);

            while(1);
        }

        vfs_file_write(g_file_sys_id, write_buffer, write_len, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }

    return;

}

void read_temp_file(uint8 file_index, uint8 *read_buffer, uint32 read_len)
{
    int file_handle;

    int ret_val;

    uint8 filename[64];

    libc_memset(filename, 0, 64);

    if(file_index == 0)
    {
        libc_memcpy(filename, att_mic_file, sizeof(att_mic_file));
    }
    else
    {
        libc_memcpy(filename, att_linein_file, sizeof(att_linein_file));
    }

    com_ansi_to_unicode(filename);

    //定位到根目录
    vfs_cd(g_file_sys_id, CD_ROOT, 0);

    ret_val = vfs_file_dir_exist(g_file_sys_id, filename, 1);

    if(ret_val == 0)
    {
        return;
    }
    else
    {
        file_handle = vfs_file_open(g_file_sys_id, filename, R_NORMAL_SEEK);

        if(file_handle == 0)
        {
            libc_print("open file error!\n", 0, 0);

            while(1);
        }

        vfs_file_read(g_file_sys_id, read_buffer, read_len, file_handle);

        vfs_file_close(g_file_sys_id, file_handle);
    }

    return;

}

#endif
