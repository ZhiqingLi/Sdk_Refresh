/******************************************************************************
 * @file    chip_info.h
 * @author
 * @version V1.0.0
 * @date    19-07-2013
 * @brief   define all chip info
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */


/**
 * @addtogroup EFUSE
 * @{
 * @defgroup chip_info chip_info.h
 * @{
 */
 
#ifndef __CHIP_INFO_H__
#define __CHIP_INFO_H__


#define	SOC_TYPE_CHIP()		REG_MFV_VER

/**
* MPW有硬件bug，必须要通过外部时钟输入
* 配置LVD寄存器为1
* 然后切换芯片内部时钟
* 通过GPIO23或者GPIO12做为控制端口，GPIO23/12没有复用关系
*/


#endif /*__CHIP_INFO_H__*/

/**
 * @}
 * @}
 */

