/**
 *****************************************************************************
 * @file     sleep.h
 * @author   Fenston
 * @version  V1.0.0
 * @date     16-april-2015
 * @maintainer Fenston
 * @brief    sleep module driver head file
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

 /**
 * @addtogroup SLEEP_WAKEUP
 * @{
 * @defgroup sleep sleep.h
 * @{
 */
 
#ifndef __SLEEP_H__
#define __SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"



/**
 * WakeUp Flag.
 */
#define WAKEUP_FLAG_POR_LDOIN		0x00001
#define WAKEUP_FLAG_WDG				0x00002
#define WAKEUP_FLAG_RTC				0x00004		//RTC闹钟定时唤醒(RTC Alarm WakeUp)
#define WAKEUP_FLAG_POWERKEY		0x00008		//POWER-KEY唤醒(POWER-KEY WakeUp)
#define WAKEUP_FLAG_SYSRST			0x00010		//软复位(System Reset)
#define WAKEUP_FLAG_RSTPIN			0x00020
#define WAKEUP_FLAG_GPIOA10			0x00040
#define WAKEUP_FLAG_GPIOB5			0x00080
#define WAKEUP_FLAG_GPIOB6			0x00100
#define WAKEUP_FLAG_GPIOB7			0x00200
#define WAKEUP_FLAG_GPIOB22			0x00400
#define WAKEUP_FLAG_GPIOB23			0x00800
#define WAKEUP_FLAG_GPIOB24			0x01000
#define WAKEUP_FLAG_GPIOB25			0x02000
#define WAKEUP_FLAG_GPIOC2			0x04000
#define WAKEUP_FLAG_GPIOC7			0x08000
#define WAKEUP_FLAG_GPIOC8			0x10000
#define WAKEUP_FLAG_GPIOC9			0x20000
#define WAKEUP_FLAG_POR_RTC			0x040000	//RTC闹钟定时开机(RTC Alarm WakeUp Core)
#define WAKEUP_FLAG_POR_POWERKEY	0x080000	//POWER-KEY开机(POWER-KEY PowerOn)
#define WAKEUP_FLAG_POR_8SRST		0x100000	//POWER-KEY 8S复位(POWER-KEY 8S Reset)

/**
 * WakeUp Source0(in deep Sleep mode) Select.
 */
#define WAKEUP_SRC0_SLEEP_A0			0x0001
#define WAKEUP_SRC0_SLEEP_A1			0x0002
#define WAKEUP_SRC0_SLEEP_A2			0x0004
#define WAKEUP_SRC0_SLEEP_A3			0x0008
#define WAKEUP_SRC0_SLEEP_A4			0x0010
#define WAKEUP_SRC0_SLEEP_A5			0x0020
#define WAKEUP_SRC0_SLEEP_A6			0x0040
#define WAKEUP_SRC0_SLEEP_A7			0x0080
#define WAKEUP_SRC0_SLEEP_A8			0x0100
#define WAKEUP_SRC0_SLEEP_A9			0x0200
#define WAKEUP_SRC0_SLEEP_A10			0x0400
#define WAKEUP_SRC0_SLEEP_A11			0x0800
#define WAKEUP_SRC0_SLEEP_A12			0x1000
#define WAKEUP_SRC0_SLEEP_A13			0x2000
#define WAKEUP_SRC0_SLEEP_A14			0x4000
#define WAKEUP_SRC0_SLEEP_A15			0x8000
#define WAKEUP_SRC0_SLEEP_A16			0x010000
#define WAKEUP_SRC0_SLEEP_A17			0x020000
#define WAKEUP_SRC0_SLEEP_A18			0x040000
#define WAKEUP_SRC0_SLEEP_A19			0x080000
#define WAKEUP_SRC0_SLEEP_A20			0x100000
#define WAKEUP_SRC0_SLEEP_A21			0x200000
#define WAKEUP_SRC0_SLEEP_A22			0x400000
#define WAKEUP_SRC0_SLEEP_A23			0x800000
#define WAKEUP_SRC0_SLEEP_A24			0x01000000
#define WAKEUP_SRC0_SLEEP_A25			0x02000000
#define WAKEUP_SRC0_SLEEP_A26			0x04000000
#define WAKEUP_SRC0_SLEEP_A27			0x08000000
#define WAKEUP_SRC0_SLEEP_A28			0x10000000
#define WAKEUP_SRC0_SLEEP_A29			0x20000000
#define WAKEUP_SRC0_SLEEP_A30			0x40000000
#define WAKEUP_SRC0_SLEEP_A31			0x80000000

/**
 * WakeUp Source1(in deep Sleep mode) Select.
 */
#define WAKEUP_SRC1_SLEEP_B0			0x0001
#define WAKEUP_SRC1_SLEEP_B1			0x0002
#define WAKEUP_SRC1_SLEEP_B2			0x0004
#define WAKEUP_SRC1_SLEEP_B3			0x0008
#define WAKEUP_SRC1_SLEEP_B4			0x0010
#define WAKEUP_SRC1_SLEEP_B5			0x0020
#define WAKEUP_SRC1_SLEEP_B6			0x0040
#define WAKEUP_SRC1_SLEEP_B7			0x0080
#define WAKEUP_SRC1_SLEEP_B8			0x0100
#define WAKEUP_SRC1_SLEEP_B9			0x0200
#define WAKEUP_SRC1_SLEEP_B10			0x0400
#define WAKEUP_SRC1_SLEEP_B11			0x0800
#define WAKEUP_SRC1_SLEEP_B12			0x1000
#define WAKEUP_SRC1_SLEEP_B13			0x2000
#define WAKEUP_SRC1_SLEEP_B14			0x4000
#define WAKEUP_SRC1_SLEEP_B15			0x8000
#define WAKEUP_SRC1_SLEEP_B16			0x010000
#define WAKEUP_SRC1_SLEEP_B17			0x020000
#define WAKEUP_SRC1_SLEEP_B18			0x040000
#define WAKEUP_SRC1_SLEEP_B19			0x080000
#define WAKEUP_SRC1_SLEEP_B20			0x100000
#define WAKEUP_SRC1_SLEEP_B21			0x200000
#define WAKEUP_SRC1_SLEEP_B22			0x400000
#define WAKEUP_SRC1_SLEEP_B23			0x800000
#define WAKEUP_SRC1_SLEEP_B24			0x01000000
#define WAKEUP_SRC1_SLEEP_B25			0x02000000
#define WAKEUP_SRC1_SLEEP_B26			0x04000000
#define WAKEUP_SRC1_SLEEP_B27			0x08000000
#define WAKEUP_SRC1_SLEEP_B28			0x10000000
#define WAKEUP_SRC1_SLEEP_B29			0x20000000
#define WAKEUP_SRC1_SLEEP_B30			0x40000000
#define WAKEUP_SRC1_SLEEP_B31			0x80000000

/**
 * WakeUp Source2(in deep Sleep mode) Select.
 */
#define WAKEUP_SRC2_SLEEP_C0			0x0001
#define WAKEUP_SRC2_SLEEP_C1			0x0002
#define WAKEUP_SRC2_SLEEP_C2			0x0004
#define WAKEUP_SRC2_SLEEP_C3			0x0008
#define WAKEUP_SRC2_SLEEP_C4			0x0010
#define WAKEUP_SRC2_SLEEP_C5			0x0020
#define WAKEUP_SRC2_SLEEP_C6			0x0040
#define WAKEUP_SRC2_SLEEP_C7			0x0080
#define WAKEUP_SRC2_SLEEP_C8			0x0100
#define WAKEUP_SRC2_SLEEP_C9			0x0200
#define WAKEUP_SRC2_SLEEP_C10			0x0400
#define WAKEUP_SRC2_SLEEP_C11			0x0800
#define WAKEUP_SRC2_SLEEP_RTC			0x1000


/**
 * Pin(GPIO、POWER-KEY WakeUp) wakeUp Trig Polarity(1'b0: low wakeup, 1'b1: high wakeup).
 */
#define WAKEUP_POLAR0_A_LOW				0
#define WAKEUP_POLAR0_A0_HI				0x0001
#define WAKEUP_POLAR0_A1_HI				0x0002
#define WAKEUP_POLAR0_A2_HI				0x0004
#define WAKEUP_POLAR0_A3_HI				0x0008
#define WAKEUP_POLAR0_A4_HI				0x0010
#define WAKEUP_POLAR0_A5_HI				0x0020
#define WAKEUP_POLAR0_A6_HI				0x0040
#define WAKEUP_POLAR0_A7_HI				0x0080
#define WAKEUP_POLAR0_A8_HI				0x0100
#define WAKEUP_POLAR0_A9_HI				0x0200
#define WAKEUP_POLAR0_A10_HI			0x0400
#define WAKEUP_POLAR0_A11_HI			0x0800
#define WAKEUP_POLAR0_A12_HI			0x1000
#define WAKEUP_POLAR0_A13_HI			0x2000
#define WAKEUP_POLAR0_A14_HI			0x4000
#define WAKEUP_POLAR0_A15_HI			0x8000
#define WAKEUP_POLAR0_A16_HI			0x010000
#define WAKEUP_POLAR0_A17_HI			0x020000
#define WAKEUP_POLAR0_A18_HI			0x040000
#define WAKEUP_POLAR0_A19_HI			0x080000
#define WAKEUP_POLAR0_A20_HI			0x100000
#define WAKEUP_POLAR0_A21_HI			0x200000
#define WAKEUP_POLAR0_A22_HI			0x400000
#define WAKEUP_POLAR0_A23_HI			0x800000
#define WAKEUP_POLAR0_A24_HI			0x01000000
#define WAKEUP_POLAR0_A25_HI			0x02000000
#define WAKEUP_POLAR0_A26_HI			0x04000000
#define WAKEUP_POLAR0_A27_HI			0x08000000
#define WAKEUP_POLAR0_A28_HI			0x10000000
#define WAKEUP_POLAR0_A29_HI			0x20000000
#define WAKEUP_POLAR0_A30_HI			0x40000000
#define WAKEUP_POLAR0_A31_HI			0x80000000
/**
 * Pin(GPIO、POWER-KEY WakeUp) wakeUp Trig Polarity(1'b0: low wakeup, 1'b1: high wakeup).
 */
#define WAKEUP_POLAR1_B_LOW				0
#define WAKEUP_POLAR1_B0_HI				0x0001
#define WAKEUP_POLAR1_B1_HI				0x0002
#define WAKEUP_POLAR1_B2_HI				0x0004
#define WAKEUP_POLAR1_B3_HI				0x0008
#define WAKEUP_POLAR1_B4_HI				0x0010
#define WAKEUP_POLAR1_B5_HI				0x0020
#define WAKEUP_POLAR1_B6_HI				0x0040
#define WAKEUP_POLAR1_B7_HI				0x0080
#define WAKEUP_POLAR1_B8_HI				0x0100
#define WAKEUP_POLAR1_B9_HI				0x0200
#define WAKEUP_POLAR1_B10_HI			0x0400
#define WAKEUP_POLAR1_B11_HI			0x0800
#define WAKEUP_POLAR1_B12_HI			0x1000
#define WAKEUP_POLAR1_B13_HI			0x2000
#define WAKEUP_POLAR1_B14_HI			0x4000
#define WAKEUP_POLAR1_B15_HI			0x8000
#define WAKEUP_POLAR1_B16_HI			0x010000
#define WAKEUP_POLAR1_B17_HI			0x020000
#define WAKEUP_POLAR1_B18_HI			0x040000
#define WAKEUP_POLAR1_B19_HI			0x080000
#define WAKEUP_POLAR1_B20_HI			0x100000
#define WAKEUP_POLAR1_B21_HI			0x200000
#define WAKEUP_POLAR1_B22_HI			0x400000
#define WAKEUP_POLAR1_B23_HI			0x800000
#define WAKEUP_POLAR1_B24_HI			0x01000000
#define WAKEUP_POLAR1_B25_HI			0x02000000
#define WAKEUP_POLAR1_B26_HI			0x04000000
#define WAKEUP_POLAR1_B27_HI			0x08000000
#define WAKEUP_POLAR1_B28_HI			0x10000000
#define WAKEUP_POLAR1_B29_HI			0x20000000
#define WAKEUP_POLAR1_B30_HI			0x40000000
#define WAKEUP_POLAR1_B31_HI			0x80000000

/**
 * Pin(GPIO、POWER-KEY WakeUp) wakeUp Trig Polarity(1'b0: low wakeup, 1'b1: high wakeup).
 */
#define WAKEUP_POLAR3_C_LOW				0
#define WAKEUP_POLAR3_C0_HI				0x0001
#define WAKEUP_POLAR3_C1_HI				0x0002
#define WAKEUP_POLAR3_C2_HI				0x0004
#define WAKEUP_POLAR3_C3_HI				0x0008
#define WAKEUP_POLAR3_C4_HI				0x0010
#define WAKEUP_POLAR3_C5_HI				0x0020
#define WAKEUP_POLAR3_C6_HI				0x0040
#define WAKEUP_POLAR3_C7_HI				0x0080
#define WAKEUP_POLAR3_C8_HI				0x0100
#define WAKEUP_POLAR3_C9_HI				0x0200
#define WAKEUP_POLAR3_C10_HI			0x0400
#define WAKEUP_POLAR3_C11_HI			0x0800


/**
 * Pin(GPIO、POWER-KEY) wakeup minimal time mode(0: no minimal time limitation, 1: minimal time is 1ms).
 */
#define WAKEUP_TMODE_0MS			0
#define WAKEUP_TMODE_1MS			1

	
/**
 * @brief  sleep模式唤醒源中断响应使能
 * @param  无
 * @return 无
 */
extern void Sleep_WakeUpIntEnable(void);


/**
 * @brief  sleep模式唤醒源中断响应关闭
 * @param  无
 * @return 无
 */
extern void Sleep_WakeUpIntDisable(void);

 
/**
 * @brief  sleep模式唤醒源识别消抖时间
 * @param  time [5:0] 消抖时间设置
 * @return 无
 */
extern void Sleep_WakeUpTrgTimeSet(uint8_t time);


/**
 * @brief  Set the wakeup source in deep sleep mode.
 * @param  WakeSrc0：指定GPIOA[31:0]的哪个引脚作为唤醒源，相应的位置1表示该位在条件满足时唤醒MCU.
 * @param  WakeSrc1：指定GPIOB[31:0]的哪个引脚作为唤醒源，相应的位置1表示该位在条件满足时唤醒MCU
 * @param  WakeSrc2：指定GPIOC[11:0]的哪个引脚作为唤醒源，相应的位置1表示该位在条件满足时唤醒MCU
 * @param  Polarity0：设置GPIOA唤醒MCU的极性，为0表示下降沿唤醒.为1表示上升沿唤醒
 * @param  Polarity1：设置GPIOB唤醒MCU的极性，为0表示下降沿唤醒.为1表示上升沿唤醒
 * @param  Polarity2：设置GPIOB唤醒MCU的极性，为0表示下降沿唤醒.为1表示上升沿唤醒
 * @param  Tmode：设置唤醒源信号保持的最小时间，为0无限制， 为1则要求唤醒信号至少保持1ms.
 * @return None.
 */ 
//extern void Sleep_SetWkUpSrc(uint32_t WakeSrc0, uint32_t WakeSrc1, uint32_t Polarity0, uint32_t Polarity1, bool Tmode);
extern void Sleep_WakeUpSrcSet(\
                             uint32_t WakeSrc0, uint32_t WakeSrc1,\
                             uint32_t WakeSrc2, uint32_t Polarity0, uint32_t Polarity1, uint32_t Polarity2, bool Tmode);
							 
/**
 * @brief  进入sleep状态
 * @param  无
 * @return 无
 */
extern void Sleep_GoToSleep(void);

/**
 * @brief  清除sleep的唤醒源
 * @param  WakeSrc0 -> 清除GPIOA的唤醒源，设置后，唤醒源失效.
 * @param  WakeSrc1 -> 清除GPIOB的唤醒源，设置后，唤醒源失效
 * @param  WakeSrc2 -> 清除GPIOC和RTC的唤醒源
 * @return 无
 */
extern void Sleep_WakeUpSrcClear(uint32_t WakeSrc0, uint32_t WakeSrc1, uint32_t WakeSrc2);
 
/**
 * @brief  获取sleep唤醒后的唤醒源标志
 * @param  无
 * @return 无
 * @Note   This function will Get Global Wakeup Flag.
 */ 
extern void Sleep_WakeUpSrcFlagGet( uint32_t* WakeSrc0Flag,  uint32_t* WakeSrc1Flag, uint32_t*WakeSrc2Flag );

/**
 * @brief 清除sleep唤醒标识，在唤醒后，此函数必须被调用，否则若wakeup中断使能，会反复中断
 * @param 无
 * @return 无
 */
extern void WakeUpFlagClear();
#endif

/**
 * @}
 * @}
 */

