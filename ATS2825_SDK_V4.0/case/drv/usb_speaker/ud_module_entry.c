/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_intern.h"

/******************************************************************************/
/*!
 * \par  Description:
 *   init usb
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_init.c
 * \par
 * \note
 *******************************************************************************/
int usb_init(void *null1, void *null2, void *null3)
{
    sys_set_mem_use_info(MEM_USE_USOUND);
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   init usb
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_init.c
 * \par
 * \note
 *******************************************************************************/
int usb_exit(void *null1, void *null2, void *null3)
{
    sys_set_mem_use_info(MEM_USE_USOUND);
    return 0;
}

module_init(usb_init)
module_exit(usb_exit)

