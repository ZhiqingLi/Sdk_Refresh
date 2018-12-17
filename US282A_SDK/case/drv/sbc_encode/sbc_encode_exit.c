/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sbc编解码驱动，
 * 作者：kehaitao
 ********************************************************************************/
#include "sbc_encode_interface.h"

/******************************************************************************/
/*!
 * \par  Description:
 *	  key initialize
 * \param[in]    none
 * \param[in]    none
 * \param[in]    none
 * \param[out]   none
 * \ingroup      key drvier
 * \par          exmaple code
 * \code
 *               list example code in here
 * \endcode
 * \note
 * 按键初始化，负责中断向量安装等
 *******************************************************************************/
int sbc_encode_exit(void)
{
		return 0;   
}

module_exit(sbc_encode_exit)
