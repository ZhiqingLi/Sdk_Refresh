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
void KeyScan(void);

#ifdef FUNC_KEY_CPH_EN
#ifdef FUNC_ADC_KEY_EN
extern uint16_t GetAdcKeyState(void);
#endif
#ifdef FUNC_IR_KEY_EN
extern uint16_t GetIrKeyState(void);
#endif	
#ifdef FUNC_IIC_KEY_EN 
extern uint16_t GetI2cKeyState(void);
#endif
#endif

#ifdef FUNC_POWERON_USB_UPDATA_EN
extern bool IsHardwareUpdataLink(void);
#ifdef FUNC_ADC_KEY_EN
extern uint8_t GetAdcKeyIndex(void);
#endif
#ifdef FUNC_IR_KEY_EN
extern uint8_t GetIrKeyIndex(void);
#endif	
#ifdef FUNC_IIC_KEY_EN 
extern uint8_t GetI2cKeyIndex(void);
#endif
#ifdef FUNC_GPIO_KEY_EN 
extern uint8_t GetGpioKeyIndex(void);
#endif

#endif

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
