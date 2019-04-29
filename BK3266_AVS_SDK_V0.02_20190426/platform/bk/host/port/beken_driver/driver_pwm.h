#ifndef _DRIVER_PWM_H_
#define _DRIVER_PWM_H_
#include "bk3000_reg.h"

//#define ENABLE_PWM	// ��PWM

#define PWM_PERIOD	(0x7fff) //0-0xffff   300
#define PWM_HIGH     (1)//PWM_HIGH must less than PWM_PERIOD
#define PWM_MAX_HOLD (10)//�ﵽ������ʱ�򱣳�һ��ʱ��
#define PWM_MIN_HOLD (20)//�ﵽ���ʱ�򱣳�һ��ʱ��



#define PWM0_EN (1<<0)
#define PWM1_EN (1<<1)
#define PWM2_EN (1<<2)
#define PWM3_EN (1<<3)
#define PWM4_EN (1<<4)
#define PWM5_EN (1<<5)

typedef enum
{
	PWM_DEC,	//PWM ��С
	PWM_INC,	//PWM ���
	PWM_MAX, 	//PWM �ﵽ���
	PWM_MIN	//PWM �ﵽ��С
}pwmDir;

typedef struct pwm_t
{	
    pwmDir   pwmDirect;	// PWM����С����
    uint16   pwmCount;
    int16   pwmHoldTime;
}pwmData;
typedef pwmData* PpwmData;

extern pwmData pwmdata;

void PWMxinit(uint8 PWMindex, uint8 enableWork,uint16 PWMperiod,uint16 dutyCycle);
void PWMxOrGpio(uint8 IsPWM,uint8  PWMindex,uint8 gpioDir);
void PWMxEnableWork(uint8 PWMindex,uint8 enableWork);
RAM_CODE void PWMxdutyCycleSet(uint8 PWMindex,uint16 dutyCycle);
RAM_CODE void PWMxPeriodSet(uint8 PWMindex,uint16 PWMperiod);
RAM_CODE void PWMGradualChange(uint8 pwmDirect); // PWM ����
#endif

