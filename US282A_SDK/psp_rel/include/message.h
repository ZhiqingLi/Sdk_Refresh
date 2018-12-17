/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：消息类型定义
 * 作者：liminxian
 ********************************************************************************/


#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_SYS_RTC2HZ               0x0080
#define MSG_SYS_BAT_V_CHG            0x0081
#define MSG_SYS_LOW_POWER            0x0082
#define MSG_SYS_RTCALARM             0x0083
#define MSG_SYS_POWER_OFF            0x0084  /*! 关机消息, 等待前台应用和后台应用退出后启动 config 应用进入关机流程 */
#define MSG_SYS_RECLAIM              0x0085
#define MSG_SYS_DSP_RESET            0x0086
#define MSG_SYS_MPU_ERROR            0x0087

#define MSG_SYS_USB_STICK            0x0090
#define MSG_SYS_USB_UNSTICK          0x0091

#define MSG_SYS_ADAPTOR_IN           0x00a0  /*! 充电器 ADAPTOR 插入消息 */
#define MSG_SYS_ADAPTOR_OUT          0x00a1  /*! 充电器 ADAPTOR 拔出消息 */

#define MSG_SYS_SD_IN                0x00b0  /*插卡状态*/
#define MSG_SYS_SD_OUT               0x00b1  /*拨卡状态*/

#define MSG_SYS_UH_IN				 0x00c0	 /*U盘插入*/
#define MSG_SYS_UH_OUT				 0x00c1	 /*U盘拔出*/

#define MSG_SYS_LINEIN_IN            0x00d0  /*LineIn插入*/
#define MSG_SYS_LINEIN_OUT           0x00d1  /*LineIn拔出*/

#define MSG_SYS_HP_IN                0x00e0  /*Headphone插入*/
#define MSG_SYS_HP_OUT               0x00e1  /*Headphone拔出*/

#endif

