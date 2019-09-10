/**
 **************************************************************************************
 * @file    Key.h
 * @brief   key 
 *
 * @author  pi
 * @version V1.0.0
 *
 * $Created: 2018-1-11 17:40:00$
 *
 * @Copyright (C) 2018, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include "app_config.h"
#include "code_key.h"
#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//按键消息表中index总数
//其中power key adc index为5个；adc key index为22个；ir key index为21个
//用户可根据需要在消息表中增加，并对应修改此总数值；


#define KEY_MSG_DEFAULT_NUM         6+22+21+3


#define BIT(n)  1 << n	

enum
{
	ADC_GPIOA20     = BIT(1),
	ADC_GPIOA21     = BIT(2),
	ADC_GPIOA22     = BIT(3),
	ADC_GPIOA23     = BIT(4),
	ADC_GPIOA24     = BIT(5),
	ADC_GPIOA25     = BIT(6),
	ADC_GPIOA26     = BIT(7),
	ADC_GPIOA27     = BIT(8),
	ADC_GPIOA28     = BIT(9),
	ADC_GPIOA29     = BIT(10),
	ADC_GPIOA30     = BIT(11),
	ADC_GPIOA31     = BIT(12),
	ADC_GPIOB0      = BIT(13),
	ADC_GPIOB1      = BIT(14),
};

#define KEY_CODE_KEY1				0xf0	//旋钮1

#define	KEY_CODE_KEY_EVENT_TYPE		0x20
#define KEY_CODE_KEY_NONE			(KEY_CODE_KEY_EVENT_TYPE + CODE_KEY_NONE)
#define KEY_CODE_KEY_FORWARD		(KEY_CODE_KEY_EVENT_TYPE + CODE_KEY_FORWARD)
#define KEY_CODE_KEY_BACKWARD		(KEY_CODE_KEY_EVENT_TYPE + CODE_KEY_BACKWARD)

#define KEY_POWER_KEY_EVENT_TYPE	0x50

extern const uint16_t MEDIA_PLAY_KEY_MAP[][KEY_MSG_DEFAULT_NUM];//msg table @ key.c
extern const uint16_t KEY_DEFAULT_MAP[][KEY_MSG_DEFAULT_NUM];
extern const uint16_t BT_PLAY_KEY_MAP[][KEY_MSG_DEFAULT_NUM];
extern const uint16_t BT_HF_KEY_MAP[][KEY_MSG_DEFAULT_NUM];
extern const uint16_t RADIO_PLAY_KEY_MAP[][KEY_MSG_DEFAULT_NUM];

typedef struct _DBKeyMsg_1_
{
	uint16_t dbclick_en;
	uint16_t dbclick_counter;
	uint16_t dbclick_timeout;

	uint32_t KeyMsg;
	uint32_t dbclick_msg;
}KEYBOARD_MSG;

void DbclickInit(void);
void DbclickProcess(void);
uint8_t DbclickGetMsg(uint16_t Msg);
void SetKeyMap(uint16_t (*Table)[KEY_MSG_DEFAULT_NUM]);
void KeyInit(void);
MessageId KeyScan(void);
void BeepEnable(void);

#ifdef __cplusplus
}
#endif//__cplusplus


#endif




