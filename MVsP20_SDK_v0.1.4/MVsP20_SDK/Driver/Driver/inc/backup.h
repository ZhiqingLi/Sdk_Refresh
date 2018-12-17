/**
 *******************************************************************************
 * @file    backup.h
 * @brief	backup module driver interface
 
 * @author  Peter Zhai
 * @version V1.0.0
 
 * $Created: 2016-12-26 13:25:07$
 * @Copyright (C) 2014, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */
 
 /**
 * @addtogroup BACKUP
 * @{
 * @defgroup backup backup.h
 * @{
 */

#ifndef __BACKUP_H__
#define __BACKUP_H__

typedef enum
{
	REG0 = 0,
	REG1,
	REG2,
	REG3,
	REG4,
}USER_DEFINED_REG_NUM;

/**
 * @brief	Set UserDefinedReg
 *
 * @param
 *   		which : Set which register value
 *   		val
 * @return
 *   		NONE
 * @note
 */
void 	Backup_UserDefinedRegSet(USER_DEFINED_REG_NUM which, uint32_t val);

/**
 * @brief	Get UserDefinedReg
 *
 * @param
 *   		which : Get which register value
 * @return
 *   		NONE
 * @note
 */
uint32_t Backup_UserDefinedRegGet(USER_DEFINED_REG_NUM which);

#endif

/**
 * @}
 * @}
 */
