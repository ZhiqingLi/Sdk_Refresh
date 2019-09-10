/**
 **************************************************************************************
 * @file    standby_play.h
 * @brief   standby
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-10-22 11:40:00$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __STANDBY_MODE_H__
#define __STANDBY_MODE_H__



#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 



MessageHandle GetStandbyPlayMessageHandle(void);

bool StandbyPlayCreate(MessageHandle parentMsgHandle);

bool StandbyPlayKill(void);

bool StandbyPlayStart(void);

bool StandbyPlayPause(void);

bool StandbyPlayResume(void);

bool StandbyPlayStop(void);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif // __STANDBY_MODE_H__
