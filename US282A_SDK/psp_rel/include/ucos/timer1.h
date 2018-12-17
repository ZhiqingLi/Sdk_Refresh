/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：在中断服务程序中运行的定时器
 * 作者：liminxian
 ********************************************************************************/
 

#ifndef __TIMER1_H__
#define __TIMER1_H__

#include <ucos/init.h>
#ifndef _ASSEMBLER_

#define MAX_IRQ_TIMERS 6

/*!
 * \brief
 *      中断定时器数据结构
 */
typedef struct
{
    /* 处理函数 */
    void (*handler)(void);
    /* 时间间隔 */
    uint16 period;
    /* 当前计数值 */
    uint16 cur_value;
}irq_timer_t;




#endif /*_ASSEMBLER_*/
#endif /*__IRQ_TIMER_H__*/
