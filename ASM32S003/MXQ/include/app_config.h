//脥篓脫脙鹿娄脛脺潞锚露篓脪氓录掳潞炉脢媒脡霉脙梅

#ifndef _APP_CONFIG_H_
#define _APP_CONFIG_H_

#ifdef _APP_CONFIG_H_
#include "asm32s003.h"
#include "asm32s003_i2c.h"
#include "asm32s003_uart.h"
#include "asm32s003_syscon.h"
#include "asm32s003_gtc.h"
#include "asm32s003_pwm.h"
#include "asm32s003_adc.h"
#include "asm32s003_led.h"
#include "asm32s003_gpio.h"
#include "asm32s003_ifc.h"
#include "stdio.h"
#include "timeout.h"
#include "app_msg.h"
#include "key.h"
#include "power_key.h"

/////////////////////////////////////////////////////////////////////////////////////
//系统工作频率
/////////////////////////////////////////////////////////////////////////////////////
#define  WorkFreq						20000000ul

/////////////////////////////////////////////////////////////////////////////////////
//UART 串口打印功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define  FUNC_APP_DEBUG_EN

#ifdef FUNC_APP_DEBUG_EN
	#define APP_DEBUG_UART_PROT_SEL		UART0
	//UART gpio端口选择，UART0：0=A12、A13；1=A5、A4;UART1:0=B1、B2；1=C2、C3；
	#define APP_DEBUG_UART_GPIO_SEL		0				    
	#define APP_DEBUG_UART_BAUDRATE		115200

	extern void APP_DBG(const char *format,...);
#else
    #define APP_DBG(format, ...)
#endif

/////////////////////////////////////////////////////////////////////////////////////
//开机时检测进入升级模式
/////////////////////////////////////////////////////////////////////////////////////
#define FUNC_POWERON_DETECT_UPGRADE_EN				//UPGRADE

#ifdef FUNC_POWERON_DETECT_UPGRADE_EN
	#define 	GPIO_UPGRADE_DETECT_PORT_OUT		GPIOB0		
	#define 	GPIO_UPGRADE_DETECT_PORT_BIT		2	
	#define 	EXTE_UPGRADE_PORT_OUT_EN	

	#ifdef EXTE_UPGRADE_PORT_OUT_EN
		#define 	EXTE_UPGRADE_OUT_PORT_OUT		GPIOA0
		#define 	EXTE_UPGRADE_OUT_PORT_BIT		3
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////
//电源控制功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define FUNC_GPIO_POWER_CONTROL_EN					//GPIO控制系统电源

#ifdef FUNC_GPIO_POWER_CONTROL_EN
    //GPIO按键开关机定义
	#define FUNC_GPIO_ON_OFF_EN   					//GPIO开关机按键
	#ifdef FUNC_GPIO_ON_OFF_EN
		#define 	GPIO_ON_OFF_PORT_OUT			GPIOA0		
		#define 	GPIO_ON_OFF_PORT_BIT			4	
        #define     GPIO_POWER_DETECT_TIME          2000            //开关机检测时间  
	#endif

    //GPIO控制系统电源
	#define FUNC_GPIO_POWERON_EN   					//GPIO系统电源控制
	#ifdef FUNC_GPIO_POWERON_EN
		#define 	GPIO_POWERON_PORT_OUT			GPIOC0		
		#define 	GPIO_POWERON_PORT_BIT			3	
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////
//按键检测功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define  FUNC_KEY_EN

#ifdef FUNC_KEY_EN
//ADC KEY定义
//	#define FUNC_ADC_KEY_EN							//ADC KEY宏开关
	#ifdef FUNC_ADC_KEY_EN	
		#define		ADC_KEY_PORT_CH1	ADC_CHANNEL_B7		//如果只使用1路ADC按键，则保留本行
//		#define		ADC_KEY_PORT_CH2	ADC_CHANNEL_B5		//如果只使用2路ADC按键，则保留本行
	#endif
	
//IR KEY定义
//	#define FUNC_IR_KEY_EN   						//IR KEY宏开关
	#ifdef FUNC_IR_KEY_EN 
		#define 	IR_KEY_PORT						IR_USE_GPIOB7		//IR使用的端口：IR_USE_GPIOA10, IR_USE_GPIOB7 or IR_USE_GPIOC2
	#endif
	
//IIC从机按键定义
//	#define FUNC_IIC_KEY_EN   						//IIC 从机按键
	#ifdef FUNC_IIC_KEY_EN
		#define 	I2C_KEY_SCL_PORT_OUT			GPIOA0		
		#define 	I2C_KEY_SCL_PORT_BIT			1		
		#define 	I2C_KEY_SDA_PORT_OUT			GPIOA0	    
		#define 	I2C_KEY_SDA_PORT_BIT			2	     	
	#endif

//GPIO按键定义
	#define FUNC_GPIO_KEY_EN   						//GPIO按键
	#ifdef FUNC_GPIO_KEY_EN
		#define 	GPIO_KEY1_PORT_OUT				GPIOA0		
		#define 	GPIO_KEY1_PORT_BIT				5	

		#define 	GPIO_KEY2_PORT_OUT				GPIOA0		
		#define 	GPIO_KEY2_PORT_BIT				6	

        #define 	GPIO_KEY3_PORT_OUT				GPIOB0		
		#define 	GPIO_KEY3_PORT_BIT				2	

		#define 	GPIO_KEY4_PORT_OUT				GPIOB0		
		#define 	GPIO_KEY4_PORT_BIT				3

        #define 	GPIO_KEY5_PORT_OUT				GPIOA0		
		#define 	GPIO_KEY5_PORT_BIT				8
	#endif

//CODING KEY定义
//	#define FUNC_CODING_KEY_EN						//CODING KEY宏开关
	#ifdef FUNC_CODING_KEY_EN
		#define 	CODING_KEY_A_PORT_OUT			GPIOA0	
		#define		CODING_KEY_A_PORT_BIT			6			

		#define 	CODING_KEY_B_PORT_OUT			GPIOA0	
		#define		CODING_KEY_B_PORT_BIT			7	
	#endif
												
    #define FUNC_KEY_CPH_EN							    //按键长按保持功能
    #define FUNC_KEY_DBCK_EN							//按键双击
    #define FUNC_POWER_KEY_EN				  		    //开关机键复合短按功能

#endif

/////////////////////////////////////////////////////////////////////////////////////
//按键检测功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define FUNC_AUDIO_MENU_EN							//将案件切换为不同功能

#ifdef FUNC_AUDIO_MENU_EN
	#define MENU_TIMEOUT 							500
#endif

/////////////////////////////////////////////////////////////////////////////////////
//PWM功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define  FUNC_PWM_EN

#ifdef FUNC_PWM_EN
//PWM0 配置
	#define FUNC_PWM0_OUT_EN								//PWM0宏开关
	#ifdef FUNC_PWM0_OUT_EN	
		#define		FUNC_PWM0_GPIO_PORT	GTC0_PWMPIN_GPIOA11	//如果只使用0路PWM，则保留本行
		//PWM输出功能配置：重载、PWM使能、输出有效状态
		#define		FUNC_PWM0_CONFIGURE	(GTC_ControlSet_REPEAT|GTC_ControlSet_PWMEN|GTC_ControlSet_OUTST)
		//PWM中断配置
		#define		FUNC_PWM0_INT_CONFIG	0
	#endif
	
//PWM1 配置
	#define FUNC_PWM1_OUT_EN								//PWM1宏开关
	#ifdef FUNC_PWM1_OUT_EN
		#define		FUNC_PWM1_GPIO_PORT	GTC1_PWMPIN_GPIOA9	//如果只使用1路PWM，则保留本行
		//PWM输出功能配置：重载、PWM使能、输出有效状态
		#define		FUNC_PWM1_CONFIGURE	(GTC_ControlSet_REPEAT|GTC_ControlSet_PWMEN|GTC_ControlSet_OUTST)
		//PWM中断配置
		#define		FUNC_PWM1_INT_CONFIG	0
	#endif
	
//PWM2 配置
	#define FUNC_PWM2_OUT_EN								//PWM2宏开关
	#ifdef FUNC_PWM2_OUT_EN	
		#define		FUNC_PWM2_GPIO_PORT	GTC2_PWMPIN_GPIOA7	//如果只使用2路PWM，则保留本行
		//PWM输出功能配置：重载、PWM使能、输出有效状态
		#define		FUNC_PWM2_CONFIGURE	(GTC_ControlSet_REPEAT|GTC_ControlSet_PWMEN|GTC_ControlSet_OUTST)
		//PWM中断配置
		#define		FUNC_PWM2_INT_CONFIG	0
	#endif

//PWM3 配置
//	#define FUNC_PWM3_OUT_EN								//PWM3宏开关
	#ifdef FUNC_PWM3_OUT_EN	
		#define		FUNC_PWM3_GPIO_PORT	GTC0_PWMPIN_GPIOC3	//如果只使用3路PWM，则保留本行
		//PWM输出功能配置：重载、PWM使能、输出有效状态
		#define		FUNC_PWM3_CONFIGURE	(GTC_ControlSet_REPEAT|GTC_ControlSet_PWMEN|GTC_ControlSet_OUTST)
		//PWM中断配置
		#define		FUNC_PWM3_INT_CONFIG	0

	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////
//IIC通讯功功能配置
/////////////////////////////////////////////////////////////////////////////////////
#define FUNC_IIC_EN							                //IIC功能

#ifdef FUNC_IIC_EN	
//	#define FUNC_IIC_MASTER_EN                              //IIC 主模式
    #define FUNC_IIC_SLAVE_EN                               //IIC 从模式
    #define FUNC_IIC_INT_INFORM_EN                          //IIC 有数据中断通知主机

    #ifdef FUNC_IIC_INT_INFORM_EN       
        #define     GPIO_INT_INFORM_PORT_OUT  GPIOC0      
        #define     GPIO_INT_INFORM_PORT_BIT  2
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////
#endif
#endif //_APP_CONFIG_H_


