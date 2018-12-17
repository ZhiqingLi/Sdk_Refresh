/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：fm驱动接口
 * 作者：Gary Wang
 ********************************************************************************/
#ifndef _FM_QN8035_H
#define _FM_QN8035_H

#include "TWI.h"
#include "qndriver.h"
#include "qnio.h"
#include "case_type.h"

//设备地址
#define TWI_WRITE_ADDR                  (I2C_DEV0_ADDRESS)  //(0x20)

#define qnd_ChipID                      (CHIPSUBID_QN8035A1)
#define qnd_IsQN8035B                   (0x13)

//用硬件搜台
#define HARD_SEEK           (0)


//value to be set
typedef struct
{
    uint32 freq_low; //frequency lowest
    uint32 freq_high; //frequency highest
    uint8 band; //band mode: 0 US, 1 JAPAN, 2 europ
    uint8 span; //channel span: 100KHZ,or 50KHZ
    uint8 step; //QND_FSTEP_50KHZ, QND_FSTEP_100KHZ等
} set_value_t;
extern set_value_t g_set_value;

extern FM_init_parm_t fm_init_parm;
extern uint8 qnd_xtal_div0;
extern uint8 qnd_xtal_div1;
extern uint8 qnd_xtal_div2;




#endif /*_FM_QN8035_H */

