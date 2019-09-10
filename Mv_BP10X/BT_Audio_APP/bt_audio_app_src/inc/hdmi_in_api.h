/**
 **************************************************************************************
 * @file    hdmi_in_api.h
 * @brief   hdmi in
 *
 * @author  Cecilia Wang
 * @version V1.0.0
 *
 * $Created: 2018-1-5 11:40:00$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __HDMI_IN_API_H__
#define __HDMI_IN_API_H__

#include "dma.h"
#include "gpio.h"
#include "spdif.h"
#include "pwm.h"
#include "pwc.h"
#include "mcu_circular_buf.h"
#include "cec.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus


typedef struct __HDMIInfo
{
	uint8_t			hdmiHPDCheck;
	uint8_t         hdmiReportStatus;
	uint8_t         hdmi_poweron_flag;
	uint8_t			hdmi_arc_flag;
	uint8_t			hdmiReportARCStatus;
	uint16_t        hdmiBusyTime;

	uint8_t         edid_buf[256];

} HDMIInfo;



void 	 HDMI_CEC_DDC_Init(void);
void 	 HDMI_CEC_Scan(void);
void 	 HDMI_CEC_DDC_DeInit(void);

void 	 HDMI_ARC_Init(uint16_t *DMAFifoAddr, uint32_t DMAFifoSize, MCU_CIRCULAR_CONTEXT *ct);
uint8_t  HDMI_ARC_LockStatusGet(void);
uint16_t HDMI_ARC_DataLenGet(void);
uint16_t HDMI_ARC_DataGet(void *pcm_out, uint16_t MaxSize);
void     HDMI_ARC_DeInit(void);
//0: not ready, 1: ready
bool 	 HDMI_ARC_IsReady(void);
void     HDMI_ARC_OnOff(uint8_t ArcOnFlag);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __HDMI_IN_MODE_H__
