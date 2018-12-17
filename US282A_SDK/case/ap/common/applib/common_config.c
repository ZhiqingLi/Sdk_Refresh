/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：方案配置项管理子模块，包括打开配置脚本（SD区文件）、读取配置项等功能接口。
 * 作者：cailizhen
 ********************************************************************************/

#include "applib.h"

/*! \cond COMMON_API */

/******************************************************************************/
/*!
 * \par  Description:
 *    打开应用配置脚本文件，并读取解释配置文件头。
 * \param[in]    config_file 应用配置脚本文件名称
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 打开应用配置脚本成功。
 * \retval           FALSE 打开应用配置脚本失败
 * \ingroup      common_func
 * \note
 * \li  该接口只在 manager ap的 globe_data_init 中调用一次，也就是说在系统启动时
 *      进行一次打开初始化，之后任何地方读取应用配置项时都无须打开。
 *******************************************************************************/
bool com_open_config_file(const char* config_file)
{
    uint8 head[16];

    config_fp = sys_sd_fopen(config_file);
    if (config_fp == NULL)
    {
        return FALSE;
    }

    if (sys_sd_fread(config_fp, head, 16) == -1)
    {
        return FALSE;
    }

    if ((head[0] != 'c') || (head[1] != 'f') || (head[2] != 'g'))
    {
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    关闭应用配置脚本文件。
 * \param[in]    none
 * \param[out]   none
 * \return       bool
 * \retval           TRUE 关闭应用配置脚本成功。
 * \retval           FALSE 关闭应用配置脚本失败。
 * \ingroup      common_func
 * \note
 *******************************************************************************/
#if 0
bool com_close_config_file(void)
{
    if (sys_sd_fclose(config_fp) == -1)
    {
        return FALSE;
    }

    return TRUE;
}
#endif

/******************************************************************************/
/*!
 * \par  Description:
 *    读取应用配置项。
 * \param[in]    config_id 配置参数ID
 * \param[in]    buf_len 参数输出缓冲区大小
 * \param[out]   buf 参数输出缓冲区
 * \return       bool
 * \retval           TRUE 读取配置项成功。
 * \retval           FALSE 读取配置项失败。
 * \ingroup      common_func
 * \ingroup      common_func
 * \par          exmaple code
 * \code
 例子1：获取背光亮度时间参数结构体
 if(com_get_config_struct(CONFIG_ID_LIGHT_TIME, config_buf, CONFIG_BUF_LEN) == TRUE)
 {
 config_nonlinear_t *config_param;

 config_param = (config_nonlinear_t *)config_buf;
 然后就可以对 config_param 按 config_nonlinear_t 结构进行访问
 }
 else
 {
 没有找到该应用配置项
 }
 * \endcode
 * \note
 * \li  取可变长的 config_string_t 和 config_nonlinear_t，必须先根据使用场合申请最大
 *      的config参数的 uint8 缓冲区并读取数据，读取成功后把该缓冲区强制转换为
 *      config_string_t 或 config_nonlinear_t 类型指针，然后就可以按照 config_string_t
 *      和 config_nonlinear_t类型进行访问。
 *******************************************************************************/
bool com_get_config_struct(uint16 config_id, uint8 *buf, uint16 buf_len)
{
    config_index_t temp_index;

    //读取配置项索引结构体
    if (sys_sd_fseek(config_fp, SEEK_SET, 16 + config_id * sizeof(config_index_t)) == -1)
    {
        return FALSE;
    }
    if (sys_sd_fread(config_fp, &temp_index, sizeof(config_index_t)) == -1)
    {
        return FALSE;
    }

    //读取配置项内容
    if (sys_sd_fseek(config_fp, SEEK_SET, temp_index.offset) == -1)
    {
        return FALSE;
    }
    if (temp_index.length > buf_len)
    {
        if (sys_sd_fread(config_fp, buf, buf_len) == -1)
        {
            return FALSE;
        }
    }
    else
    {
        if (sys_sd_fread(config_fp, buf, temp_index.length) == -1)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    读取应用配置项默认值。
 * \param[in]    config_id 配置参数ID
 * \param[out]   none
 * \return       uint16
 * \retval           返回读取到的应用配置项默认值。
 * \ingroup      common_func
 * \ingroup      common_func
 * \par          exmaple code
 * \code
 例子1：获取背光亮度时间默认值
 light_time = com_get_config_default(CONFIG_ID_LIGHT_TIME);
 * \endcode
 * \note
 * \li  仅用于数值型参数，并由用户保证 config_id 正确，若找不到配置项则返回0。
 *******************************************************************************/
uint16 com_get_config_default(uint16 config_id)
{
    config_index_t temp_index;
    uint16 default_value;

    //读取配置项索引结构体
    if (sys_sd_fseek(config_fp, SEEK_SET, 16 + config_id * sizeof(config_index_t)) == -1)
    {
        return 0;
    }
    if (sys_sd_fread(config_fp, &temp_index, sizeof(config_index_t)) == -1)
    {
        return 0;
    }

    //读取配置项内容
    if (sys_sd_fseek(config_fp, SEEK_SET, temp_index.offset) == -1)
    {
        return 0;
    }
    if (sys_sd_fread(config_fp, &default_value, sizeof(uint16)) == -1)
    {
        return 0;
    }

    return default_value;
}

/*! \endcond */
