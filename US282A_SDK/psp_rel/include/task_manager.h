
/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：任务栈信息定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <sys_board_def.h>

//note:
//内容修改后，需要同步task_info.rules !!!!!!!!!!!!!!!!!!!!!!

#define SRAM_STACK_ADDR             0x9fc1b600

#define AP_PROCESS_MANAGER_PRIO     0xC
#define AP_PROCESS_MANAGER_STK_SIZE (0x340)
#define AP_PROCESS_MANAGER_STK_POS  (SRAM_STACK_ADDR + AP_PROCESS_MANAGER_STK_SIZE)//1b600 - 1b940

#define AP_BACK_HIGH_PRIO           0x3
#define AP_BACK_HIGH_STK_SIZE       (0x4b0)
#define AP_BACK_HIGH_STK_POS        (AP_PROCESS_MANAGER_STK_POS + AP_BACK_HIGH_STK_SIZE)//1b940 - 1bdf0

#define AP_BACK_LOW_PRIO            0x6	
#define AP_BACK_LOW_STK_SIZE        (0x5e0)//需优化减少
#define AP_BACK_LOW_STK_POS         (AP_BACK_HIGH_STK_POS + AP_BACK_LOW_STK_SIZE)//1bdf0 - 1c3d0

#define AP_BT_HIGH_PRIO             0x7
#define AP_BT_HIGH_STK_SIZE         (0x650)
#define AP_BT_HIGH_STK_POS          (AP_BACK_LOW_STK_POS + AP_BT_HIGH_STK_SIZE)//1c3d0 - 1ca20

///////////////////////////////////////////////////////////////////////////////////////////////
#define AP_BT_LOW_PRIO              0x8	
//#define AP_BT_LOW_STK_SIZE          (0x280)
//#define AP_BT_LOW_STK_POS           (AP_BT_HIGH_STK_POS + AP_BT_LOW_STK_SIZE)//1c920 - 1cba0

#define KEYTONE_PLAY_PRIO           0x4
#define KEYTONE_PLAY_STK_SIZE       (0x300)
//#define KEYTONE_PLAY_STK_POS        (AP_BT_HIGH_STK_POS + KEYTONE_PLAY_STK_SIZE)//1c920 - 1cba0
///////////////////////////////////////////////////////////////////////////////////////////////

#define AP_FRONT_HIGH_PRIO          0x9
#define AP_FRONT_HIGH_STK_SIZE      (0x500)
//#define AP_FRONT_HIGH_STK_POS       (KEYTONE_PLAY_STK_POS + AP_FRONT_HIGH_STK_SIZE)//1cba0 - 1d000

#define AP_FRONT_LOW_PRIO           0xA
#define AP_FRONT_LOW_STK_SIZE       (0x6e0)
#define AP_FRONT_LOW_STK_POS        (AP_BT_HIGH_STK_POS + AP_FRONT_LOW_STK_SIZE)//1ca20 - 1d100


//#define AP_FRONT_SCANFILE_PRIO      0xB
//#define AP_FRONT_SCANFILE_STK_SIZE (0x200)
//#define AP_FRONT_SCANFILE_STK_POS   (0x9fc18d00)




/*0x300 to be distributed*/
#endif

