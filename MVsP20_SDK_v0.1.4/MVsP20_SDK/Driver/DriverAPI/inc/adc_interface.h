/**
 **************************************************************************************
 * @file    adc_interface.h
 * @brief   audio adc interface
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2015-09-23 14:01:05$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__ADC_INTERFACE_H__
#define		__ADC_INTERFACE_H__


void AudioADC_Init(ADC_MODULE Module);
uint16_t AudioADC_GetDataLen(ADC_MODULE Module);
uint16_t AudioADC_GetData(ADC_MODULE Module, void* Buf, uint16_t MaxLen);


#endif
