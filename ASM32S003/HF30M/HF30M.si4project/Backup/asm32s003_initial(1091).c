/*
  ******************************************************************************
  * @file    asm32s003_initial.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
#include "asm32s003_syscon.h"
#include "asm32s003_coret.h"
#include "asm32s003_gpio.h"
#include "asm32s003_uart.h"
#include "asm32s003_gtc.h"
#include "asm32s003_adc.h"
#include "asm32s003_led.h"
#include "asm32s003_i2c.h"
#include "asm32s003_counta.h"
#include "asm32s003_ifc.h"
#include "app_config.h"

/* define --------------------------------------------------------------------*/
#define debug_Mode                                          //run in debug mode
//#define EXReset_Mode                                      //define PA0.0 as reset pin
/* externs--------------------------------------------------------------------*/
extern void TKey_Parameter_Init(void);

/*************************************************************/
//software delay
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void delay_nms(unsigned int t)
{
    volatile unsigned int i,j ,k=0;
    j = 50* t;
    for ( i = 0; i < j; i++ )
    {
        k++;
    }
}
/*************************************************************/
//GPIO Initial
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void GPIO_CONFIG(void)
{
    GPIO_Init(GPIOA0,0,0);                         //PA0.0 as output
    GPIO_Init(GPIOB0,1,0);                         //PB0.1 as output
    GPIO_Init(GPIOC0,0,0);                         //PC0.0 as output
	GPIO_Init(GPIOB0,0,1);                         //PB0.0 as input
    
	GPIO_Set_Value(GPIOA0,0,1);						//PA0.0 output high
    GPIO_Set_Value(GPIOB0,1,0);						//PB0.1 output low
//------------  EXI FUNTION  --------------------------------/
//EXI0_INT= EXI0,EXI1_INT= EXI1, EXI2_INT=EXI2~EXI3, EXI3_INT=EXI4~EXI8, EXI4_INT=EXI9~EXI13    
    GPIO_IntGroup_Set(PA0);									 //EXI group select:PA0/PB0/PC0
    GPIOA0_EXI_Init(EXI6);                                   	 //PA0.5 as input
    EXTI_trigger_CMD(ENABLE,EXI_PIN6,_EXIFT);                  	//ENABLE falling edge
	EXTI_trigger_CMD(ENABLE,EXI_PIN6,_EXIRT);                  	//ENABLE rising edge
    EXTI_interrupt_CMD(ENABLE,EXI_PIN6);                	   	//enable EXI
    GPIO_EXTI_interrupt(GPIOA0,0b00000001000000);				  //enable GPIOA05 as EXI
	
    //EXI0_Int_Enable();                                          //EXI0 INT Vector
    //EXI1_Int_Enable();                                          //EXI1 INT Vector
    //EXI2_Int_Enable();                                          //EXI2~EXI3 INT Vector
    EXI3_Int_Enable();                                          //EXI4~EXI8 INT Vector
    //EXI4_Int_Enable();                                          //EXI9~EXI13 INT Vector
	
    //EXI0_WakeUp_Enable();										  //EXI0 interrupt wake up enable
	//EXI1_WakeUp_Enable();										  //EXI1 interrupt wake up enable
	//EXI2_WakeUp_Enable();										  //EXI2~EXI3 interrupt wake up enable
	//EXI3_WakeUp_Enable();										  //EXI4~EXI8 interrupt wake up enable
	//EXI4_WakeUp_Enable();										  //EXI9~EXI13 interrupt wake up enable
}
/*************************************************************/
//LED Init
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/  
void LED_CONFIG(void)  
{
	GPIOA0->PUDR = 0x00550000;												//PA0.8,P0.9,PA0.10,PA0.11  PULL_UP 
	LED_RESET_VALUE();                                                      //clear all led Register
	//LED_INT_CMD(IPEND,ENABLE);
	LED_Control_Config(LEDCLK_DIV32,LED_ComMode_4COM,DISKEY_SCEN,DISTkeyShare,0,LED_Bright_100,80,3,0);     //evey com time selected 3ms 
	LED_SEGDATX_data_write(SEGDAT_NUM0 , _CHAR_[0]);                        //show 0
	LED_SEGDATX_data_write(SEGDAT_NUM1 , _CHAR_[0]);                        //show 0
	LED_SEGDATX_data_write(SEGDAT_NUM2 , _CHAR_[0]);                        //show 0
	LED_SEGDATX_data_write(SEGDAT_NUM3 , _CHAR_[0]);                        //show 0
	LED_SCAN_ENABLE(ENABLE);                                                //LED Scan enable
	LED_Int_Enable();														//LED INT enable		
}
/*************************************************************/
//GCOUNTA Initial
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void COUNTA_CONFIG(void)
{
    COUNT_DeInit();                                                                       //clear all countA Register
    COUNTA_IO_Init(0);                                                                    //set PA0.07 as counter IO
    COUNTA_Init(40,38,Period_H_L,DIV1,REPEAT_MODE,CARRIER_ON,OSP_LOW);                    //Data_H=Time/(1/sysclock)
    COUNTA_Config(SW_STROBE,PENDREM_OFF,MATCHREM_OFF,REMSTAT_0,ENVELOPE_0);               //countA mode set  
    COUNTA_Start();                                                                       //countA start
    //COUNTA_Stop();                                                                      //countA stop  
    //COUNTA_Int_Enable();                                                                //countA INT enable
}

/*************************************************************/
//ADC12_Init
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void ADC12_CONFIG(void)
{
    ADC12_RESET_VALUE();
    ADC12_CLK_CMD(ADC_CLK_CR , ENABLE);                                                 //ENABLE ADC CLK
    ADC12_Software_Reset();                                                             //software reset
    ADC12_Configure_Mode(ADC12_12BIT , ADC12_8CYCLES , One_shot_mode , 2 , 1);        //select 12BIT ADC ,SAMPLEING CYCLE=3 , one shot mode, ADC_CLK=PCLK/2*1 , NumConver=2
    ADC12_ConversionChannel_Config(ADC12_ADCIN2,1);                                     //set ADC2 ,1st scan channel
    //ADC12_ConversionChannel_Config(ADC12_ADCIN3,2);                                     //set ADC3 ,2nd scan channel
    ADC12_CMD(ENABLE);                                                                  //ENABLE ADC
    ADC12_ready_wait();                                                                 // Waiting for ADC Ready
    //adc date output=last number of Conversions
    /*ADC12_Control(ADC12_START);                                                       // Start conversion
    ADC12_EOC_wait();                                                                   //End of conversion wait
    adc_dr=ADC12_DATA_OUPUT();                                                          //ADC =ADC1 data
    ADC12_Control(ADC12_STOP);*/	
	//ADC12_ConfigInterrupt_CMD(ADC12_EOC,ENABLE);										//ENABLE ADC EOC INT
    //ADC_Int_Enable();                                                                 //INT Vector
}

/*************************************************************/
//UART Initial
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void UART_CONFIG(void)
{
    UART_DeInit();                                                              //clear all UART Register
    UART_IO_Init(IO_UART0 ,1);                                                //use UART0 group 1
    UARTInitRxTxIntEn(UART0,174);	                                            //baudrate=sysclock/174=115200
    UART0_Int_Enable();														    //uart0 INT Enable
    UART0_WakeUp_Enable();                                                      //uart0 wakeup Enable*/
    UARTTxByte(UART0,0X55);
	
    UART_IO_Init(IO_UART1 ,1);                                                  //use UART1 group 1
    UARTInitRxTxIntEn(UART1,174);	                                            //baudrate=sysclock/174=115200
    UART1_Int_Enable();														    //uart1 INT Enable
    UART1_WakeUp_Enable();                                                      //uart1 wakeup Enable*/
    UARTTxByte(UART1,0X55);														//send 0x55 test
}
/*************************************************************/
//gtc0 Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void GTC0_CONFIG(void)
{
    GTC_RESET_VALUE(GTC0);                                                        //GTC0 all register reset value
    GTC0_IO_Init(GTC_IO_TXOUT , 4 );                                            //set tcout
    GTC_Configure(GTC0 , GTC_FIN_PCK , 1 , 9 , Counter_Size_16BIT  , 100 , 1);   //TCCLK=sysclock/2^1/10茂录?0m-->1us
    GTC_ControlSet_Configure(GTC0,GTC_ControlSet_REPEAT,ENABLE);                  //loop mode
    GTC_ControlSet_Configure(GTC0,GTC_ControlSet_PWMEN,ENABLE);                 //pwm Enable
    GTC_ControlSet_Configure(GTC0,GTC_ControlSet_IDLEST,DISABLE);                //Idle status high enable
    GTC_ControlSet_Configure(GTC0,GTC_ControlSet_OUTST,ENABLE);                 //output status
    //GTC_ControlSet_Configure(GTC0,GTC_ControlSet_HWTRIG,ENABLE);                //hardware auto-trigger enable
    //GTC_ControlSet_Configure(GTC0,GTC_ControlSet_CA1semi_F,ENABLE);                //capture down enable
    //GTC_ControlSet_Configure(GTC0,GTC_ControlSet_CA1semi_TCAP0,ENABLE);            //capture up enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_STARTI, ENABLE);                          //counter start int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_STOPI, ENABLE);                           //counter stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_PSTARTI, ENABLE);                           //counter period start int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_PENDI, ENABLE);                           //counter period stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_MATI, ENABLE);                            //counter pulse matched int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_OVFI, ENABLE);                            //counter overflow int Enable
    //GTC_ConfigInterrupt_CMD(GTC0 , GTC_CA1semiI, ENABLE);                           //counter capture int Enable
    GTC_Start(GTC0);                                                               //start counter
    //GTC0_Int_Enable();                                                             //enable GTC0 int
	//GTC0_WakeUp_Enable();														   //enable GTC0 wake int	
}
/*************************************************************/
//gtc1 Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void GTC1_CONFIG(void)
{
    GTC_RESET_VALUE(GTC1);                                                        //GTC1 all register reset value
    GTC1_IO_Init(GTC_IO_TXOUT , 0 );                                            //set tcout
    GTC_Configure(GTC1 , GTC_FIN_PCK , 1 , 9 , Counter_Size_16BIT  , 100 , 50);   //TCCLK=sysclock/2^1/10茂录?0m-->1us
    GTC_ControlSet_Configure(GTC1,GTC_ControlSet_REPEAT,ENABLE);                  //loop mode
    GTC_ControlSet_Configure(GTC1,GTC_ControlSet_PWMEN,ENABLE);                   //pwm Enable
    GTC_ControlSet_Configure(GTC1,GTC_ControlSet_IDLEST,ENABLE);                //Idle status high enable
    GTC_ControlSet_Configure(GTC1,GTC_ControlSet_OUTST,ENABLE);                 //output status
    //GTC_ControlSet_Configure(GTC1,GTC_ControlSet_HWTRIG,ENABLE);                //hardware auto-trigger enable
    //GTC_ControlSet_Configure(GTC1,GTC_ControlSet_CA1semi_F,ENABLE);                //capture down enable
    //GTC_ControlSet_Configure(GTC1,GTC_ControlSet_CA1semi_TCAP1,ENABLE);            //capture up enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_STARTI, ENABLE);                          //counter start int Enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_STOPI, ENABLE);                           //counter stop int Enable
    GTC_ConfigInterrupt_CMD(GTC1 , GTC_PSTARTI, ENABLE);                           //counter period start int Enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_PENDI, ENABLE);                           //counter period stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_MATI, ENABLE);                            //counter pulse matched int Enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_OVFI, ENABLE);                            //counter overflow int Enable
    //GTC_ConfigInterrupt_CMD(GTC1 , GTC_CA1semiI, ENABLE);                           //counter capture int Enable
    GTC_Start(GTC1);                                                               //start counter
    GTC1_Int_Enable();                                                             //enable GTC1 int
	//GTC1_WakeUp_Enable();														   //enable GTC1 wake int	
}
/*************************************************************/
//gtc2 Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void GTC2_CONFIG(void)
{
    GTC_RESET_VALUE(GTC2);                                                        //GTC2 all register reset value
    GTC2_IO_Init(GTC_IO_TCAPX , 0 );                                              //set tcout
    GTC_Configure(GTC2 , GTC_FIN_PCK , 0 , 9 , Counter_Size_16BIT  , 1000 , 300);   //TCCLK=sysclock/2^0/10茂录聦TCCLK=0.5US
	GTC_ControlSet_Configure(GTC2,GTC_ControlSet_STOPCLEAR,ENABLE);
	GTC_ControlSet_Configure(GTC2,GTC_ControlSet_CNTM,ENABLE);					  //
    GTC_ControlSet_Configure(GTC2,GTC_ControlSet_REPEAT,ENABLE);                  //loop mode
    //GTC_ControlSet_Configure(GTC2,GTC_ControlSet_PWMEN,ENABLE);                 //pwm Enable
    //GTC_ControlSet_Configure(GTC2,GTC_ControlSet_IDLEST,ENABLE);                //Idle status high enable
    //GTC_ControlSet_Configure(GTC2,GTC_ControlSet_OUTST,ENABLE);                 //output status
    //GTC_ControlSet_Configure(GTC2,GTC_ControlSet_HWTRIG,ENABLE);                //hardware auto-trigger enable
    GTC_ControlSet_Configure(GTC2,GTC_ControlSet_CA1semi_F,ENABLE);                //capture down enable
	//GTC_ControlSet_Configure(GTC2,GTC_ControlSet_CA1semi_R,ENABLE);                //capture down enable
    GTC_ControlSet_Configure(GTC2,GTC_ControlSet_CA1semi_TCAP2,ENABLE);            //capture up enable
    //GTC_ConfigInterrupt_CMD(GTC2 , GTC_STARTI, ENABLE);                          //counter start int Enable
    //GTC_ConfigInterrupt_CMD(GTC2 , GTC_STOPI, ENABLE);                           //counter stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC2 , GTC_PSTARTI, ENABLE);                           //counter period start int Enable
    //GTC_ConfigInterrupt_CMD(GTC2, GTC_PENDI, ENABLE);                           //counter period stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC2 , GTC_MATI, ENABLE);                            //counter pulse matched int Enable
    //GTC_ConfigInterrupt_CMD(GTC2 , GTC_OVFI, ENABLE);                            //counter overflow int Enable
    GTC_ConfigInterrupt_CMD(GTC2 , GTC_CA1semiI, ENABLE);                           //counter capture int Enable
    GTC_Start(GTC2);                                                               //start counter
    GTC2_Int_Enable();                                                             //enable GTC2 int
}
/*************************************************************/
//gtc3 Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void GTC3_CONFIG(void)
{
    GTC_RESET_VALUE(GTC3);                                                        //GTC3 all register reset value
    GTC3_IO_Init(GTC_IO_TXOUT , 0 );                                              //set tcout
    GTC_Configure(GTC3 , GTC_FIN_PCK , 1 , 9 , Counter_Size_32BIT  , 1000 , 500);   //TCCLK=sysclock/2^1/10茂录?0m-->1us
    GTC_ControlSet_Configure(GTC3,GTC_ControlSet_REPEAT,ENABLE);                  //loop mode
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_PWMEN,ENABLE);                 //pwm Enable
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_IDLEST,ENABLE);                //Idle status high enable
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_OUTST,ENABLE);                 //output status
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_HWTRIG,ENABLE);                //hardware auto-trigger enable
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_CA1semi_F,ENABLE);                //capture down enable
    //GTC_ControlSet_Configure(GTC3,GTC_ControlSet_CA1semi_TCAP3,ENABLE);            //capture up enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_STARTI, ENABLE);                          //counter start int Enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_STOPI, ENABLE);                           //counter stop int Enable
    GTC_ConfigInterrupt_CMD(GTC3 , GTC_PSTARTI, ENABLE);                           //counter period start int Enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_PENDI, ENABLE);                           //counter period stop int Enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_MATI, ENABLE);                            //counter pulse matched int Enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_OVFI, ENABLE);                            //counter overflow int Enable
    //GTC_ConfigInterrupt_CMD(GTC3 , GTC_CA1semiI, ENABLE);                           //counter capture int Enable
    GTC_Start(GTC3);                                                               //start counter
    GTC3_Int_Enable();                                                             //enable GTC3 int
	//GTC3_WakeUp_Enable();														   //enable GTC3 wake int	
}
/*************************************************************/
//I2C MASTER Initial
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void I2C_MASTER_CONFIG(void)
{
    I2C_DeInit();                                           //clear all I2C Register
    I2C_Master_Init(I2C_G1,FAST_MODE,0x040,0x20);           //use I2C group 1茂录聦fast mode茂录聦speed=sysclock/0x040,0x20=setup time
    I2C_Int_Enable();                                       //use interrupt mode 
}
/*************************************************************/
//I2C SLAVE Initial
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/	
void I2C_SLAVE_CONFIG(void)
{
    I2C_DeInit();                                           //clear all I2C Register
    I2C_Slave_Init(I2C_G1,FAST_MODE,0x4A,0x20,0xAE);        //slave address=0xac(8bit)茂录聦standard mode茂录聦speed=sysclock/0x138,0x78=setup time
    I2C_Int_Enable();                                       //use interrupt mode 
}
/*************************************************************/
//CORET Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/ 
void CORET_CONFIG(void)
{
	CORET_DeInit();
	SYSCON_General_CMD(ENABLE,ENDIS_SYSTICK);
	CK802->CORET_RVR=2499;									//CORETCLK=sysclock/8=20M/8=0.4us  e.g:1ms=(CORET_RVR+1)*(8/20M);CORET_RVR=2500000-1=2499999
	CORET_reload();																
	CORET_CLKSOURCE_EX();									//use ex clock
	CORET_TICKINT_Enable();									//clear CORET counter
	//CORET_stop();	
	CORET_start();	
	NVIC_SetPriority(INTC_CORETIM_IRQn, 0x00);
	CORET_Int_Enable();
	//CORET_WakeUp_Enable();
	APP_DBG("CORTE_CPU config 1ms timer\n");
}

/*************************************************************/
//syscon Functions
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void SYSCON_CONFIG(void)
{
//------SYSTEM CLK AND PCLK FUNTION---------------------------/
	SYSCON_General_CMD(ENABLE,ENDIS_ISOSC);                                     //enable or disable ISOSC茂录聦ISOSC=internal sub crystal
	SYSCON_General_CMD(ENABLE,ENDIS_IMOSC);										//enable or disable IMOSC茂录聦IMOSC=internal main crystal
	SYSCON_General_CMD(DISABLE,ENDIS_EMOSC);									//enable or disable EMOSC茂录聦EMOSC=ext crystal
	SystemCLK_HCLKDIV_PCLKDIV_Config(SYSCLK_IMOSC,HCLK_DIV_1,PCLK_DIV_1);       //system clock set, Hclk div ,Pclk div  set system clock=SystemCLK/Hclk div/Pclk div
//------------  WDT FUNTION  --------------------------------/
    SYSCON_IWDCNT_Config(DISABLE_IWDT_SHORT,IWDT_TIME_1S,IWDT_INTW_DIV_1);      //WDT TIME 1s,WDT alarm interrupt time=1s-1s*1/8=0.875S
    SYSCON_WDT_CMD(DISABLE);                                                   //enable WDT		
    //SYSCON_IWDCNT_Reload();                                                   //reload WDT
	//IWDT_Int_Enable();														//Enable WDT INT
	//SYSCON_WakeUp_Enable();													//Enable WDT wakeup INT
//------------  LVD FUNTION  --------------------------------/ 
    //SYSCON_LVD_Config(ENABLE_LVDEN,INTDET_LVL_3_0V,RSTDET_LVL_2_2V,ENABLE_LVD_INT);
    //LVD_Int_Enable();
} 

/*********************************************************************************/
//ASM32S003_init                                                                  /
//EntryParameter:NONE                                                             /
//ReturnValue:NONE                                                                /
/*********************************************************************************/
/*********************************************************************************/  
/*********************************************************************************/
void ASM32S003_init(void) 
{
//------------------------------------------------------------/
//IO mode clr all register 
//------------------------------------------------------------/
    GPIO_DeInit();                                                //GPIO register clear,ALL IO set as output
//------------------------------------------------------------/
//IO debug mode for IC
//------------------------------------------------------------/
#if defined (debug_Mode) 
    GPIO_Debug_IO_12_13();                                          //PA0.12,PA0.13 as debug IO
    //GPIO_Debug_IO_01_02();                                        //PA0.1,PA0.2 as debug IO
#endif
//------------------------------------------------------------/
//IO PA0.O AS RESET for IC(PC0.1 set as Reset pin use Writer only)
//only used in debug mode
//------------------------------------------------------------/
#if defined EXReset_Mode
	//GPIOA00_Set_ResetPin();                                      	//PA0.0 as Reset PIN
	//SIFC_CMR_Set(Option_Erase);									//user option Exreset function Erase
	//IFC_CMR_Set(ExRST_EN);										//user option Exreset function enable
#endif	
//------------------------------------------------------------/
//Peripheral clock enable and disable
//EntryParameter:NONE
//ReturnValue:NONE
//------------------------------------------------------------/
    SYSCON_WDT_CMD(DISABLE);                                        //Disable Watchdog
    SYSCON->PCER0=0xFFFFFFF;                                        //PCLK Enable  0x00410071
    SYSCON->PCER1=0xFFFFFFF;                                        //PCLK Enable
    while(!(SYSCON->PCSR0&0x1));                                    //Wait PCLK enabled	
//------------------------------------------------------------/
//ISOSC/IMOSC/EMOSC/SYSCLK/IWDT/LVD/EM_CMFAIL/EM_CMRCV/CMD_ERR OSC stable interrupt
//EntryParameter:NONE
//ReturnValue:NONE
//------------------------------------------------------------/
    SYSCON_Int_Enable();                                             //INT Vector
    SYSCON->IECR=ISOSC_ST|IMOSC_ST|EMOSC_ST|SYSCLK_ST;               //enable ISOSC Statues INT,IMOSC Statues INT,EMOSC Status INT
    CK_CPU_EnAllNormalIrq();                                         //enable all IRQ
    SYSCON_CONFIG();
	//syscon  initial
//------------------------------------------------------------/
//系统滴答定时器初始化，所有软定时器均基于此实现，所以时钟配
//置完成后必须立即启动，不要修改这个部分
//------------------------------------------------------------/
	CORET_CONFIG();
//------------------------------------------------------------/
//系统开关机初始化及开机检测
//如果不满足开机条件，则程序不会返回，直接进入休眠
//------------------------------------------------------------/
#ifdef FUNC_GPIO_POWER_CONTROL_EN
    SystemPowerControlInit();
    SystemPowerOnDetect();
#endif
//------------------------------------------------------------/
//Uart debug config
//------------------------------------------------------------/
#ifdef FUNC_APP_DEBUG_EN
	UartAppDebugConfig();
#endif
//------------------------------------------------------------/
//Other IP config
//------------------------------------------------------------/
    //GPIO_CONFIG();                                                //GPIO initial 
    //LED_CONFIG();                                                 //Led initial                    
    //COUNTA_CONFIG();                                              //CountA initial
    //GTC0_CONFIG();                                                //GTC0 initial 
    //GTC1_CONFIG();                                                //GTC1 initial 
    //GTC2_CONFIG();                                                //GTC2 initial 
    //GTC3_CONFIG();                                                //GTC3 initial  
    //ADC12_CONFIG();                                               //ADC initial
#ifdef FUNC_IIC_EN
#if defined(FUNC_IIC_MASTER_EN)
    I2C_MASTER_CONFIG();                                          //I2C harware master initial 
#elif defined(FUNC_IIC_SLAVE_EN)
	I2C_SLAVE_CONFIG();                                           //I2C harware slave initial 
#endif /* FUNC_IIC_MASTER_EN */
#ifdef FUNC_IIC_INT_INFORM_EN   
	GPIO_PullLow_Init(GPIO_INT_INFORM_PORT_OUT,GPIO_INT_INFORM_PORT_BIT);
	GPIO_Init(GPIO_INT_INFORM_PORT_OUT,GPIO_INT_INFORM_PORT_BIT,Output); 
    GPIO_Write_Low(GPIO_INT_INFORM_PORT_OUT,GPIO_INT_INFORM_PORT_BIT);
#endif
#endif /* FUNC_IIC_EN */
    //UART_CONFIG();                                                //UART initial 
//------------------------------------------------------------/
//按键功能初始化
//------------------------------------------------------------/
#ifdef FUNC_KEY_EN
	KeyInit();
#endif
//------------------------------------------------------------/
//PWM功能初始化
//------------------------------------------------------------/
#ifdef FUNC_PWM_EN
	GTC_PwmControl_Init();
#endif
}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/
