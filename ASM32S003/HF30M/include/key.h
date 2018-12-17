///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: key_event_convert.h
//  maintainer: Halley
///////////////////////////////////////////////////////////////////////////////

#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


void KeyInit(void);
uint16_t KeyScan(void);
#ifdef FUNC_GPIO_POWER_CONTROL_EN
void SystemGotoDeepSleepPowerDown(void);
#endif
#ifdef __cplusplus
}
#endif//__cplusplus

#endif
