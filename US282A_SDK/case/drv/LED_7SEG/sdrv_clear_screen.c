
/*******************************************************************************
*                              US282A
*                 Copyright(c) 2014-2015 Actions (Zhuhai) Microelectronics Co., Limited,
*                            All Rights Reserved.
*     brief    清除屏幕，供AP调用
*      <author>       <time>           
*       Wekan       2015-3-26pm            
*******************************************************************************/


#include "led_inner.h"


/******************************************************************************
* \par  Description: 灭掉所有segment
* \param[in]   none
* \param[out]  none
* \return      none
*   <author>    <time>     
*    Wekan   2015-3-26pm  
*******************************************************************************/

void sdrv_clear_screen(void *null1, void *null2, void *null3)
{
    uint8 i = 0;
    //clear data buffer can't use sys interface
    for(i = 0; i < sizeof(cur_disval); i++)
    {
        cur_disval[i] = 0;
    }
    //send data to led segment
    hdrv_data_send((uint32*) cur_disval);
}




