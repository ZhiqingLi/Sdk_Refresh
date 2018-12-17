/********************************************************************************
 *                            Module: music_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-08 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __LINEIN_COMMON_H__
#define __LINEIN_COMMON_H__

#include "psp_includes.h"
#include "enhanced.h"
#include "mmm_pp.h"

/*!
 * \brief
 *  当前播放状态
 */
typedef enum
{
    /*! 停止 */
    linein_play_status_stop = 0,
    /*! 播放 */
    linein_play_status_play = 2,
} linein_play_status_e;

/*!
 * \brief
 *  引擎状态
 */
typedef struct
{
    /*! 播放状态  */
    linein_play_status_e play_status;
    /*! 保留字节：4字节对齐要求 */
    uint8 reserve[3];
} linein_eg_status_t;

#endif //__LINEIN_COMMON_H__
