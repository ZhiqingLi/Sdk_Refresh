/*
 * efuse.h
 *
 *  Created on: Jun 9, 2017
 *      Author: jerry_rao
 */

#ifndef EFUSE_H_
#define EFUSE_H_

/**
 * @addtogroup EFUSE
 * @{
 * @defgroup efuse efuse.h
 * @{
 */
 
#include "type.h"

/**
 * @brief  读取Efuse中指定地址的数据
 * @param  Addr为Efuse中指定的地址，十六进制表示
 *         可读的地址为0x00~0x3A，其中
 *             (1)客户自定义的地址：0x23~0x2A，共8个字节地址空间，默认为全0
 *             (2)芯片唯一的ID编号：0x0A~0x11，共8个字节地址空间，出厂已有值
 * @return 读取到指定地址的数据
 */
uint8_t Efuse_ReadData(uint8_t Addr);

#endif /* EFUSE_H_ */
