/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：sbc编解码驱动，初始化。
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
int sbc_encode_init(void)
{
    libc_print("@@@sbc_encode ok",0,0);
    return 0;
}

module_init(sbc_encode_init)
