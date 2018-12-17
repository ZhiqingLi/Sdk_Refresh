/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：方案配置项管理子模块相关宏定义，结构体定义，外部接口声明等。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _common_config_H_
#define _common_config_H_

#include "psp_includes.h"
#include "case_independent.h"
#include "case_type.h"

/*!
 * \brief
 *  config_index_t 配置索引项数据结构
 */
typedef struct
{
    /*! 配置项数据相对文件头的偏移 */
    uint32 offset;
    /*! 配置项数据长度 */
    uint32 length;
} config_index_t;

/*!
 * \brief
 *  config_string_t 字符串配置项数据结构
 */
typedef struct
{
    /*! 字符串内容，可变长数组，内容与txt输入一致，以'\0'结束 */
    uint8 value[1];
} config_string_t;

/*!
 * \brief
 *  config_linear_t 线性数值配置项数据结构
 */
typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 取值区间的最小值 */
    uint16 min;
    /*! 取值区间的最大值 */
    uint16 max;
    /*! 步长 */
    uint16 step;
} config_linear_t;

/*!
 * \brief
 *  config_nonlinear_t 非线性数值配置项数据结构
 */
typedef struct
{
    /*! 默认数值 */
    uint16 default_value;
    /*! 有效值个数 */
    uint16 total;
    /*! 有效值数组，可变长数组 */
    uint16 value[1];
} config_nonlinear_t;

//配置项接口声明
extern bool com_open_config_file(const char* config_file) __FAR__;
extern bool com_close_config_file(void) __FAR__;
extern bool com_get_config_struct(uint16 config_id, uint8 *buf, uint16 buf_len) __FAR__;
extern uint16 com_get_config_default(uint16 config_id) __FAR__; //仅用于数值型参数

#endif
