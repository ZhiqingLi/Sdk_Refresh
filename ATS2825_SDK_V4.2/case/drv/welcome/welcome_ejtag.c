/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：EJTAG 调试口配置，根据CASE使用的IO口而定，CASE应该将EJTAG设置为没有被占用
 *       的EJTAG口；如果CASE将所有EJTAG组都占用了，就必须将EJTAG 口关闭。
 * 作者：cailizhen
 ********************************************************************************/

#include "case_independent.h"

void welcome_ejtag(void)
{
    //EJTAG口1组: B1 ~ B5
    //EJTAG口2组: A1 ~ A4, A7
    //EJTAG口3组: A8 ~ A12
    //EJTAG口4组: B11, A16, A21 ~ A23

    //EJTAG组选择：MFP_CTL2 [28 29], 0表示1组，1表示2组，2表示3组，3表示4组
#if (CASE_BOARD_TYPE == CASE_BOARD_EVB)
    act_writel((act_readl(MFP_CTL2) | (0x80000000)), MFP_CTL2); //开启MEJTAG，1组
#else
    act_writel((act_readl(MFP_CTL2) & (~(0xc0000000))), MFP_CTL2); //关闭EJTAG功能
#endif
}
