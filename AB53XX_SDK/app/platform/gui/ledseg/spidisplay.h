#ifndef _FUNC_DISPLAY_H_
#define _FUNC_DISPLAY_H_

/***********************************************************************************/
//常用控制命令
/***********************************************************************************/
#define   MAX_SEND_DATA_LEN  16     //最大数据长度
#define   CHAR_ACTER_MASK    BIT(7) //字符MASK

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
/***********************************************************************************

#define   ICON_COM_COLON     (1*8+SEGB_BITNO)	//GRID1 SEG13 = 1*8+4(01H，BIT4)
#define   ICON_COM_A1MARK    (11*8+SEGF_BITNO)	//GRID6 SEG9 = 11*8+0(0BH，BIT0)
#define   ICON_COM_A2MARK    (11*8+SEGB_BITNO)	//GRID6 SEG13 = 11*8+4(0BH，BIT4)
#define   ICON_COM_24H_12H   (15*8+SEGD_BITNO)	//GRID8 SEG11 = 15*8+2(0FH，BIT2)
#define   ICON_COM_AUX       (13*8+SEGG_BITNO)	//GRID7 SEG15 = 13*8+6(0DH，BIT6)
#define   ICON_COM_TF        (15*8+SEGF_BITNO)	//GRID8 SEG9 = 15*8+0(0FH，BIT0)
#define   ICON_COM_BLUE      (13*8+SEGD_BITNO)	//GRID7 SEG11 = 13*8+2(0DH，BIT2)
#define   ICON_COM_USB       (13*8+SEGF_BITNO)	//GRID7 SEG9 = 13*8+0(0DH，BIT0)
#define   ICON_COM_FM        (13*8+SEGB_BITNO)	//GRID7 SEG13 = 13*8+4(0DH，BIT4)

***********************************************************************************/
//变量及函数声明
/***********************************************************************************/	

extern void  SpiDisplayClr(void);
extern void  SpiDisplayInit(void);
extern void  RefreshSpiDisplayData(void);
//extern void  AIP1629A_BackLightPwmSet(bool IsOnOff);

#endif
