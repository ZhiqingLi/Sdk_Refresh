/**
 **************************************************************************************
 * @file    encoder_service.h
 * @brief   
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2019-2-28 13:06:47$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __ENCODER_SERVICE_H__
#define __ENCODER_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
#include "encoder_service.h"


MessageHandle GetEncoderServiceMessageHandle(void);

int32_t EncoderServiceCreate(MessageHandle parentMsgHandle);

void EncoderServiceStart(void);

void EncoderServiceStop(void);

void EncoderServiceKill(void);

void EncoderServicePlay(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif /* __ENCODER_SERVICE_H__ */

