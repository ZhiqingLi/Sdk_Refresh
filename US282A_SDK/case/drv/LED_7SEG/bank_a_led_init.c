

/*******************************************************************************
 *                              US282A
 *                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
 *                            All Rights Reserved.
 *
 *      <author>       <time>           
 *       Wekan       2015-3-26pm            
 *******************************************************************************/


#include "led_inner.h"


/******************************************************************************
 * \par  Description: LED初始化装载
 * \param[in]   none
 * \param[out]  none
 * \return      初始化是否成功
 * \retval      1: 初始化成功
 * \retval      0: 初始化成功  
 *   <author>    <time>     
 *    Wekan   2015-3-26pm  
 *******************************************************************************/

int led_init(void *null0, void *null1, void *null2)
{
    hdrv_hard_init();
    return 0;
}


/******************************************************************************
 * \par  Description:LED卸载
 * \param[in]   none
 * \param[out]  none
 * \return      卸载是否成功
 * \retval      1: 初始化成功
 * \retval      0: 初始化成功
 *   <author>    <time>     
 *    Wekan   2015-3-26pm  
 *******************************************************************************/
int led_exit(void *null0, void *null1, void *null2)
{
    return 0;
}

module_init(led_init)
module_exit(led_exit)

