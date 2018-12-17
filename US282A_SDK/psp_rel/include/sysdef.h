/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：系统共享内存定义
 * 作者：liminxian
 ********************************************************************************/

#ifndef _SYSDEF_H
#define _SYSDEF_H

#include <storage_device.h>
#include <typeext.h>
#include <sys_board_def.h>

//clear watch dog
#define CLEAR_WATCH_DOG         (act_writel(act_readl(WD_CTL) | 0x01, WD_CTL))
#define DISABLE_WATCH_DOG       (act_writel(0x69, WD_CTL))
#define ENABLE_WATCH_DOG(CLK)   (act_writel(0x51 | (((CLK)&0x07) << 1), WD_CTL))

/*************system*******************************************************************/
#define SYS_CHECK_BANKSWITCH_ON         //打开项目监控bank切换代码
/*********************************************************************************************************************
 * 系统全局变量 32 字节(0x9fc09f60~0x9fc09f7f)
 提供给card uhost等全局定义，固化代码会用到，不可修改
 *********************************************************************************************************************/

/*
 g_card_type bit使用：
 bit0(card type):      0--1_line, 1--4_line;
 bit1(sdvccout power): 0--direct, 1--GPIO;
 bit2(DAT3 pull high): 0--hardware pull, 1--GPIO pull;
 bit3(write_fast):     0--not continuous, 1--continuous write;
 bit4(read fast):      0--not continuous,1--continuous read;
 */
#define     g_card_type     (*(uint8 *)(RDATA_ADDR_BASE + 0x09f70))

//g_card_para 应用需要在装载卡驱动之前设置这个变量。如果不设置则会使用前一次设置的值
#define     g_card_para     (*(uint8 *)(RDATA_ADDR_BASE + 0x09f71))
#define     g_card_rca      (*(uint16 *)(RDATA_ADDR_BASE + 0x09f72))
//bit 0 for continuous read or write
//bit 1 for is already init
#define     g_card_state    (*(uint8 *)(RDATA_ADDR_BASE + 0x09f74))
#define     g_stub_setup    (*(uint8 *)(RDATA_ADDR_BASE + 0x09f78))

/*bitmap  for g_uhost_state  */
#define GL_DEVICE_IN                   (1 << 0) // bit0,USB中断用来标志是否有U盘插入
#define GL_CONTROLLER_ENABLED          (1 << 1) // bit1,控制器使能标记
#define GL_ENUMERATED                  (1 << 2) // bit2,
#define GL_SPEED_FULL                  (0)      // bit[3]: 0--U盘使能为高速，1--U盘使能为高速
#define GL_SPEED_HIGH                  (1 << 3) // bit[3]: 0--U盘使能为高速，1--U盘使能为高速
#define GL_DP_PULLHIGH                 (1 << 4) // bit4,Timer中断用来标志是否有U盘插入
#define GL_SPEED_HIGH_INTENDED         (1 << 5) // bit5,初始化时上层想要支持高速(但实际上不一定成功)
#define GL_PROCESS_INITIAL             (1 << 6) // bit6,uhost驱动操作中，屏蔽系统检测标记
#define     g_uhost_state         (*(uint8 *)(RDATA_ADDR_BASE + 0x09f60))

//for some comptile cnfig
#define     g_uhost_cfg           (*(uint8 *)(RDATA_ADDR_BASE + 0x09f61))

// VFS GLOBAL
#define g_fs_scan_space           (*(uint32 *)(RDATA_ADDR_BASE + 0x09f6c))

/*********************************************************************************************************************
 * 引用系统的全局变量地址
 *********************************************************************************************************************/

/* check map, must not change address of following variables, if add new variables after them.  */
#define GLOBAL_AP_BANK_HANDLE_HANDLER     (*(uint32 *)(RDATA_ADDR_BASE + 0x1a8d0)) //ap_bank_handle_handler
#define GLOBAL_CARD_BANK_SWITCH_HANDLER   (*(uint32 *)(RDATA_ADDR_BASE + 0x201e8))

/********system back_door*******/

#define BACKDOOR_EXCEPTION_NOTRESET       (1 << 1) /*if exception, 0--will reset; 1-not reset*/

#define BACKDOOR_DISUSB_DETECT            (1 << 6)
#define BACKDOOR_DISUHOST_DETECT          (1 << 7)
#define BACKDOOR_USB_PLUGIN               (1 << 8)
#define BACKDOOR_UHOST_PLUGIN             (1 << 9)
#define BACKDOOR_USB_STATUS_CHANGED       (1 << 10)
#define BACKDOOR_SYSCLK_LOCKED            (1 << 11)
#define BACKDOOR_DISCARD_DETECT           (1 << 12)
#define BACKDOOR_NOVRAM_WRITE             (1 << 13)

#define BACKDOOR_PRINT_AP_BANKSWITCH             (1 << 16)
#define BACKDOOR_PRINT_SYS_BANKSWITCH            (1 << 17)
#define BACKDOOR_PRINT_DSP_BANKSWITCH            (1 << 18)

#define BACKDOOR_ANTIPOP_START					(1 << 28)
#define BACKDOOR_AUDIO_POW_SAVING               (1 << 29)

#define GLOBAL_KERNEL_BACKDOOR_FLAG     (*(uint32 *)(RDATA_ADDR_BASE + 0x1aa5c))//backdoor_flag
/***************/

#define  SAVE_BT_NONE (0)
#define  SAVE_BT_ALL  (1)
#define  SAVE_BT_NAME (2)
#define  SAVE_BT_ADDR (3)

/*频偏写入efuse中*/
#define  RW_TRIM_CAP_EFUSE  (0)
/*频偏写入norflash中*/
#define  RW_TRIM_CAP_SNOR   (1)

typedef enum
{
    WK_TYPE_BATIN = 0,
    WK_TYPE_REMOTE,
    WK_TYPE_NFC, WK_TYPE_ONOFF,//PLAY KEY
    WK_TYPE_HDSWON,
    WK_TYPE_HDSWOFF,
    WK_TYPE_S3BT_TIMERON,
    WK_TYPE_BT_ACTIVE,
} sys_wake_type_e;

typedef void(*print_func_t)(uint8 *str, uint8 data);
typedef struct
{
    uint32 wake_up;
    uint8 *cfg_buffer;
    print_func_t print;
} welcome_argument_t;

typedef struct
{
    uint8 logical_index;
    uint8 rw_len;
    uint8 *rw_buffer;
} nvram_param_rw_t;

typedef struct
{
    uint8 cap_base_value;
    uint8 cap_trim_value;
} trim_cap_value_t;

typedef enum
{
    TRIM_CAP_WRITE_NO_ERROR,
    TRIM_CAP_WRITE_ERR_HW,
    TRIM_CAP_WRITE_ERR_NO_RESOURSE
} trim_cap_write_result_e;

typedef struct
{
    uint32 fw_cap_size;                 //方案设置的固件区总大小
    uint32 spi_cap_size;                //flash总大小
    uint32 need_write_protect_size;     //需要设置的写保护区域大小
    uint32 set_write_protect_size;      //设置的写保护区域大小
}wp_zone_info_t;


typedef enum
{
    /*读取频偏正确*/
    TRIM_CAP_READ_NO_ERROR,
    /*读取频偏调整值，并非真正频偏值，需要加偏移*/
    TRIM_CAP_READ_ADJUST_VALUE,
    /*读取频偏出错*/
    TRIM_CAP_READ_ERR_HW,
    /*未写频偏值*/
    TRIM_CAP_READ_ERR_NO_WRITE,
    /*频偏值非法*/
    TRIM_CAP_ERAD_ERR_VALUE
} trim_cap_read_result_e;

#endif  /*_SYSDEF_H*/

