#ifndef _FUNC_AIP1629A_H_
#define _FUNC_AIP1629A_H_

/***********************************************************************************/
//SPI GPIO定义
/***********************************************************************************/
#define AIP1629A_CS_PORT_OUT			   GPIO_B_OUT
#define AIP1629A_CS_PORT_BIT				 GPIOB2

#define AIP1629A_CS_PORT_IE				 (AIP1629A_CS_PORT_OUT + 1)
#define AIP1629A_CS_PORT_OE				 (AIP1629A_CS_PORT_OUT + 2)
#define AIP1629A_CS_PORT_PU				 (AIP1629A_CS_PORT_OUT + 5)
#define AIP1629A_CS_PORT_PD				 (AIP1629A_CS_PORT_OUT + 6)

#define AIP1629A_CS_SET() 			   do{\
								   GpioSetRegOneBit(AIP1629A_CS_PORT_PU, AIP1629A_CS_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CS_PORT_PD, AIP1629A_CS_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CS_PORT_IE, AIP1629A_CS_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CS_PORT_OE, AIP1629A_CS_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CS_PORT_OUT, AIP1629A_CS_PORT_BIT);\
							   }while(0)
							   
#define AIP1629A_CS_CLR()		   do{\
								   GpioSetRegOneBit(AIP1629A_CS_PORT_PU, AIP1629A_CS_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CS_PORT_PD, AIP1629A_CS_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CS_PORT_IE, AIP1629A_CS_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CS_PORT_OE, AIP1629A_CS_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CS_PORT_OUT, AIP1629A_CS_PORT_BIT);\
							   }while(0)
							   
//////////////////////////////////////////////////////////////////////////////////////
#define AIP1629A_CLK_PORT_OUT			   GPIO_A_OUT
#define AIP1629A_CLK_PORT_BIT				 GPIOA25

#define AIP1629A_CLK_PORT_IE				 (AIP1629A_CLK_PORT_OUT + 1)
#define AIP1629A_CLK_PORT_OE				 (AIP1629A_CLK_PORT_OUT + 2)
#define AIP1629A_CLK_PORT_PU				 (AIP1629A_CLK_PORT_OUT + 5)
#define AIP1629A_CLK_PORT_PD				 (AIP1629A_CLK_PORT_OUT + 6)

#define AIP1629A_CLK_SET() 			   do{\
								   GpioSetRegOneBit(AIP1629A_CLK_PORT_PU, AIP1629A_CLK_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CLK_PORT_PD, AIP1629A_CLK_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CLK_PORT_IE, AIP1629A_CLK_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CLK_PORT_OE, AIP1629A_CLK_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CLK_PORT_OUT, AIP1629A_CLK_PORT_BIT);\
							   }while(0)
							   
#define AIP1629A_CLK_CLR()		   do{\
								   GpioSetRegOneBit(AIP1629A_CLK_PORT_PU, AIP1629A_CLK_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CLK_PORT_PD, AIP1629A_CLK_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CLK_PORT_IE, AIP1629A_CLK_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_CLK_PORT_OE, AIP1629A_CLK_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_CLK_PORT_OUT, AIP1629A_CLK_PORT_BIT);\
							   }while(0)

//////////////////////////////////////////////////////////////////////////////////////
#define AIP1629A_DAT_PORT_OUT			   GPIO_A_OUT
#define AIP1629A_DAT_PORT_BIT				 GPIOA24

#define AIP1629A_DAT_PORT_IE				 (AIP1629A_DAT_PORT_OUT + 1)
#define AIP1629A_DAT_PORT_OE				 (AIP1629A_DAT_PORT_OUT + 2)
#define AIP1629A_DAT_PORT_PU				 (AIP1629A_DAT_PORT_OUT + 5)
#define AIP1629A_DAT_PORT_PD				 (AIP1629A_DAT_PORT_OUT + 6)

#define AIP1629A_DAT_SET() 			   do{\
								   GpioSetRegOneBit(AIP1629A_DAT_PORT_PU, AIP1629A_DAT_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_DAT_PORT_PD, AIP1629A_DAT_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_DAT_PORT_IE, AIP1629A_DAT_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_DAT_PORT_OE, AIP1629A_DAT_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_DAT_PORT_OUT, AIP1629A_DAT_PORT_BIT);\
							   }while(0)
							   
#define AIP1629A_DAT_CLR()		   do{\
								   GpioSetRegOneBit(AIP1629A_DAT_PORT_PU, AIP1629A_DAT_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_DAT_PORT_PD, AIP1629A_DAT_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_DAT_PORT_IE, AIP1629A_DAT_PORT_BIT);\
								   GpioSetRegOneBit(AIP1629A_DAT_PORT_OE, AIP1629A_DAT_PORT_BIT);\
								   GpioClrRegOneBit(AIP1629A_DAT_PORT_OUT, AIP1629A_DAT_PORT_BIT);\
							   }while(0)


/***********************************************************************************/
//常用控制命令
/***********************************************************************************/
#define   MAX_SEND_DATA_LEN  16     //最大数据长度
#define   AIP1629A_DELAY_US  2      //Aip1629A SPI时钟延时

#define   DATA_ADDR_AUTO     0x40   //传输数据，地址自动增加
#define   DATA_ADDR_FIXED    0x44   //传输数据，地址固定模式
#define   ADDR_STA_OFFSET    0xC0   //地址设置命令，数据基地址
#define   LED_DISP_POWOFF    0x80   //关闭LED显示
#define   LED_DISP_PWM_01    0x88   //开LED显示，设置脉冲宽度为1/16
#define   LED_DISP_PWM_02    0x89   //开LED显示，设置脉冲宽度为2/16
#define   LED_DISP_PWM_04    0x8A   //开LED显示，设置脉冲宽度为4/16
#define   LED_DISP_PWM_10    0x8B   //开LED显示，设置脉冲宽度为10/16
#define   LED_DISP_PWM_11    0x8C   //开LED显示，设置脉冲宽度为11/16
#define   LED_DISP_PWM_12    0x8D   //开LED显示，设置脉冲宽度为12/16
#define   LED_DISP_PWM_13    0x8E   //开LED显示，设置脉冲宽度为13/16
#define   LED_DISP_PWM_14    0x8F   //开LED显示，设置脉冲宽度为14/16

/***********************************************************************************/
//同一个LED显示位，显示缓存中不同sing位定义
/***********************************************************************************/

#define   ICON_COM_COLON     (1*8+SEGB_BITNO)	//GRID1 SEG13 = 1*8+4(01H，BIT4)
#define   ICON_COM_A1MARK    (11*8+SEGF_BITNO)	//GRID6 SEG9 = 11*8+0(0BH，BIT0)
#define   ICON_COM_A2MARK    (11*8+SEGB_BITNO)	//GRID6 SEG13 = 11*8+4(0BH，BIT4)
#define   ICON_COM_24H_12H   (15*8+SEGD_BITNO)	//GRID8 SEG11 = 15*8+2(0FH，BIT2)
#define   ICON_COM_AUX       (13*8+SEGG_BITNO)	//GRID7 SEG15 = 13*8+6(0DH，BIT6)
#define   ICON_COM_TF        (15*8+SEGF_BITNO)	//GRID8 SEG9 = 15*8+0(0FH，BIT0)
#define   ICON_COM_BLUE      (13*8+SEGD_BITNO)	//GRID7 SEG11 = 13*8+2(0DH，BIT2)
#define   ICON_COM_USB       (13*8+SEGF_BITNO)	//GRID7 SEG9 = 13*8+0(0DH，BIT0)
#define   ICON_COM_FM        (13*8+SEGB_BITNO)	//GRID7 SEG13 = 13*8+4(0DH，BIT4)

/***********************************************************************************/
//变量及函数声明
/***********************************************************************************/

extern DISP_TYPE	gDispBuff[]; 	

extern void  AIP1629ADisplayClr(void);
extern void  AIP1629ADisplayInit(void);
extern void  AIP1629A_FlushDispData(void);
extern void  AIP1629A_BackLightPwmSet(bool IsOnOff);

#endif
