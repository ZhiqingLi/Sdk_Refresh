/********************************************************************************
 *                            Module: music_engine
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *     fiona.yang   2011-09-08 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __USOUND_COMMON_H__
#define __USOUND_COMMON_H__

#include "psp_includes.h"
#include "enhanced.h"

typedef struct
{
    uint32 play_status;

    uint32 change_vol_val;

    uint16 line_status;
    
    uint16 switch_flag;

} uengine_status_t;

typedef struct
{
    uint32 vol_flag;

    uint32 vol_val;

} uengine_sync_volume_t;

/*!
 * \brief
 *  ÒýÇæ×´Ì¬
 */

#endif //__USOUND_COMMON_H__
