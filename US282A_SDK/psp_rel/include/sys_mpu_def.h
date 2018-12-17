/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：内存保护机制定义
 * 作者：liminxian
 ********************************************************************************/
#ifndef __SYS_MPU_DEF_H__
#define __SYS_MPU_DEF_H__

#define SYS_MPU_SUPPORT_MAX   4

//mpu_index
#define SYS_MPU_INDEX_0   0
#define SYS_MPU_INDEX_1   1
#define SYS_MPU_INDEX_2   2
#define SYS_MPU_INDEX_3   3

//mpu_err_ip
#define SYS_MPU_DSP_PR_ErrIP    0x10
#define SYS_MPU_DMA_W_ErrIP     0x08
#define SYS_MPU_CPU_DW_ErrIP    0x04
#define SYS_MPU_DSP_DR_ErrIP    0x02
#define SYS_MPU_DSP_DW_ErrIP    0x01

typedef struct
{
    uint8  mpu_index; //mpu单元
    uint8  mpu_err_ip;//配置触发错误中断位
    uint8  mpu_set_ip;//设置写保护错误位
    uint8 resever;   //保留未用
    uint32 start_addr;//起始内存地址
    uint32 end_addr;  //结束内存地址
    void *irq_handle; //中断服务程序,为0默认进入系统死循环
}sys_mpu_param_t;

int32 set_mpu_irq(sys_mpu_param_t *sys_mpu_param) __FAR__;

void del_mpu_irq(uint32 mpu_irq_index) __FAR__;

#endif //#ifndef __SYS_MPU_DEF_H__


