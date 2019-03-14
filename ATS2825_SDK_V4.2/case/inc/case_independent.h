/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：CASE 最基本的定制选项头文件，每个具体的案子根据自己的功能规格仔细配置好
 *       这些宏开关；该头文件可以让 CASE 层所有驱动和应用包含。
 * 作者：cailizhen
 ********************************************************************************/

#ifndef _case_independent_H
#define _case_independent_H

#include "psp_includes.h"

#define CASE_BOARD_EVB              0
#define CASE_BOARD_DEMO             1
#define CASE_BOARD_ATS2823          2
#define CASE_BOARD_DVB_ATS2825      3
#define CASE_BOARD_DVB_ATS2823      4
#define CASE_BOARD_TYPE         CASE_BOARD_DVB_ATS2825

#define CASE_LINEIN_CHANNEL_SEL 1  //LINEIN AA/ADDA通道选择，0表示AA，1表示ADDA
#define CASE_FM_CHANNEL_SEL     1  //FM AA/ADDA通道选择，0表示AA，1表示ADDA

#if ((CASE_BOARD_TYPE == CASE_BOARD_EVB) || (CASE_BOARD_TYPE == CASE_BOARD_ATS2823))
#define SPEAKER_HEADPHONE_FIX   1  //EVB固定从耳机输出，需要开启DAE
#else
#define SPEAKER_HEADPHONE_FIX   0
#endif

#define ENABLE_LEVEL_LOW        0  //低电平使能
#define ENABLE_LEVEL_HIGH       1  //高电平使能
#define ENABLE_FIX_ACTIVE       2  //直接拉高或拉低始终使能
#define DETECT_NO_SUPPORT       0 //不做热拔插检测
#define DETECT_BY_ADCKEY        1 //ADCKEY检测
#define DETECT_BY_GPIO          2 //GPIO检测
#define DETECT_BY_GPIOB0        3 //GPIOB0检测
#define DETECT_BY_CMD           4 //SD卡命令检测
#define DETECT_BY_SIO           5 //SIO检测
//数组有几个元素
#define ARR_COUNT(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define SUPPORT_MUSIC_FOLDER    0 //是否支持特殊目录功能
#define SUPPORT_RECORD          0 //是否支持录音功能
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
#define SUPPORT_RECORD_PLAY     0 //是否支持录音回放功能
#define SUPPORT_RING_FOLDER     0 //是否支持闹铃目录功能
#define SUPPORT_PLIST_FILE      0 //是否支持文件名列表
#else
#define SUPPORT_RECORD_PLAY     0 //是否支持录音回放功能
#define SUPPORT_RING_FOLDER     1 //是否支持闹铃目录功能
#define SUPPORT_PLIST_FILE      1 //是否支持文件名列表
#endif
#define SUPPORT_PLIST_TITLE     0 //是否支持标题列表
#define SUPPORT_PLIST_ARTIST    0 //是否支持演唱者列表
#define SUPPORT_PLIST_ALBUM     0 //是否支持专辑列表
#define SUPPORT_PLIST_GENRE     0 //是否支持风格列表
#define SUPPORT_DETECT_ENERGY   1 //是否支持能量检测
#define SUPPORT_RCP_FUNC        1 //是否支持RCP协议
#define SUPPORT_MULTI_FREQ_MULTI_BAND   1//是否支持多频点多频段 !!!这个宏打开的时候要注意同步到PSP头文件mmm_dae_def.h SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING

//是否是ESD固件，在config.sh中配置
//ESD模式不响应热插拔消息
//#define __ESD_MODE_

//#define __SUPPORT_HID_

//#define ENABLE_DOUBLE_PHONE_

#ifndef __BQB_TEST_PROFILE_
//是否支持3方通话
#define __SUPPORT_3_WAY_CALL_

//是否支持siri
#define __SUPPORT_SIRI_         1

//是否支持linein tws，在config.sh中配置
//case/cfg根据这个宏决定编译linein的目录
#define __SUPPORT_LINEIN_TWS

#endif

//是否支持低延时版本linwin tws
//#define __SUPPORT_LINEIN_LOW_DELAY

//是否进入BQB测试模式
//#define __BQB_TEST_PROFILE_

//选择语音识别路径
#define SMART_BOX_OPUS_MODE

//#define SMART_BOX_BUF_MODE

#define SUPPORT_OTA

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SUPPORT_AUX_DETECT      DETECT_BY_GPIOB0 //是否支持AUX拔插检测
#elif (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
#define SUPPORT_AUX_DETECT      DETECT_NO_SUPPORT //是否支持AUX拔插检测
#else
#define SUPPORT_AUX_DETECT      DETECT_NO_SUPPORT //是否支持AUX拔插检测
#endif

#if (SPEAKER_HEADPHONE_FIX == 1)
#define SUPPORT_HP_DETECT       DETECT_NO_SUPPORT //是否支持耳机拔插检测 headphone
#else
#define SUPPORT_HP_DETECT       DETECT_NO_SUPPORT //是否支持耳机拔插检测 headphone
//#define SUPPORT_HP_DETECT       DETECT_BY_GPIO //是否支持耳机拔插检测 headphone I2S DEMO 版本
#endif

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
#define SUPPORT_CARD_DETECT     DETECT_NO_SUPPORT
#elif defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
#define SUPPORT_CARD_DETECT     DETECT_NO_SUPPORT
#else
#define SUPPORT_CARD_DETECT     DETECT_BY_CMD
#endif

#define SUPPORT_RCP             1 //是否支持RCP命令
#define SUPPORT_SLAVE_CONTROL   0 //是否支持I2C/SPI/UART控制
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
#define SUPPORT_LED_DRIVER      0 //是否支持LED驱动
#define SUPPORT_IR_DRIVER       0 //是否支持红外遥控器
#elif defined(BOARD_LINKPLAY_MUVO)
#define SUPPORT_LED_DRIVER      0 //是否支持LED驱动
#define SUPPORT_IR_DRIVER       0 //是否支持红外遥控器
#else
#define SUPPORT_LED_DRIVER      0 //是否支持LED驱动
#define SUPPORT_IR_DRIVER       0 //是否支持红外遥控器
#endif
#define SUPPORT_HPY_DRIVER      0 //是否支持IO按键(矩阵)
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO) || defined(BOARD_JL_36410_FAR)
#define SUPPORT_ADC_KEY         0 //是否支持线控按键
#else
#define SUPPORT_ADC_KEY         1 //是否支持线控按键
#endif

#define TK_DRIVER_OUTER         1 //支持外部触摸按键
#define TK_DRIVER_INNER         2 //支持内部触摸按键
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SUPPORT_TK_DRIVER       TK_DRIVER_OUTER //是否支持触摸按键，1表示支持外部触摸按键，2表示支持内部触摸按键
#else
#define SUPPORT_TK_DRIVER       0 //是否支持触摸按键，1表示支持外部触摸按键，2表示支持内部触摸按键
#endif

#if defined(BOARD_JL_36410_FAR)
#define TK_DRIVER_FTC334F
#define FTC_GPIO_SCAN_PIN_OUT_EN                GPIOAOUTEN
#define FTC_GPIO_SCAN_PIN_IN_EN                 GPIOAINEN
#define FTC_GPIO_SCAN_PIN_DATA                  GPIOADAT
#define FTC_GPIO_SCAN_PIN_PU_EN                 GPIOAPUEN
#define FTC_GPIO_SCAN_PIN_NUMBER                (1 << 21)

#define FTC_GPIO_KEY_DATA_OUT0_IN_EN            GPIOAINEN
#define FTC_GPIO_KEY_DATA_OUT0_OUT_EN           GPIOAOUTEN
#define FTC_GPIO_KEY_DATA_OUT0_DATA             GPIOADAT
#define FTC_GPIO_KEY_DATA_OUT0_PD_EN            GPIOAPDEN
#define FTC_GPIO_KEY_DATA_OUT0_PIN              (1 << 16)

#define FTC_GPIO_KEY_DATA_OUT1_IN_EN            GPIOAINEN
#define FTC_GPIO_KEY_DATA_OUT1_OUT_EN           GPIOAOUTEN
#define FTC_GPIO_KEY_DATA_OUT1_DATA             GPIOADAT
#define FTC_GPIO_KEY_DATA_OUT1_PD_EN            GPIOAPDEN
#define FTC_GPIO_KEY_DATA_OUT1_PIN              (1 << 11)
#endif

#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SUPPORT_PHONE_KEY       1 //是否支持专用的电话键
#else
#define SUPPORT_PHONE_KEY       0 //是否支持专用的电话键
#endif

#define SUPPORT_BTT_TEST          //是否支持频偏测试功能
#define SUPPORT_ASET_TEST         //是否支持ASET测试功能
//#define WAVES_ASET_TOOLS          //是否支持WAVES工具和算法

//打开宏后，脚本fwimage.cfg需增加打包test_chl.drv文件
//代码复用前台ENHANCED1，所以只允许在蓝牙播放应用下使用
//#define SUPPORT_SPPBLE_TEST       //是否支持SPPBLE通道测试


//是否使用外挂充电电路
#if (CASE_BOARD_TYPE == CASE_BOARD_DEMO)
#define SUPPORT_OUTER_CHARGE    1
#elif defined(BOARD_DPI_7C370_FAR) || defined(BOARD_JL_36410_FAR)
#define SUPPORT_OUTER_CHARGE    1
#else
#define SUPPORT_OUTER_CHARGE    0
#endif

#define BAT_CHARGE_MODE_INNER          (0) //内部充电
#define BAT_CHARGE_MODE_OUTER          (1) //外部充电
#if defined(BOARD_DPI_7C370_FAR)
#define EXTERN_CHARGE_DETECT_GPIOUPEN  GPIOAPUEN
#define EXTERN_CHARGE_DETECT_GPIOIN    GPIOAINEN
#define EXTERN_CHARGE_DETECT_GPIODAT   GPIOADAT
#define EXTERN_CHARGE_DETECT_PIN       (1 << 16)

#define IS_EXTERN_CAHREGE_IN()         ((act_readl(EXTERN_CHARGE_DETECT_GPIODAT) & EXTERN_CHARGE_DETECT_PIN) == 0)
#define IS_EXTERN_BATTERY_IN()         (1)
#elif defined(BOARD_JL_36410_FAR)
#define EXTERN_CHARGE_DETECT_GPIOUPEN  GPIOAPUEN
#define EXTERN_CHARGE_DETECT_GPIOIN    GPIOAINEN
#define EXTERN_CHARGE_DETECT_GPIODAT   GPIOADAT
#define EXTERN_CHARGE_DETECT_PIN       (1 << 5)

#define IS_EXTERN_CAHREGE_IN()         ((act_readl(EXTERN_CHARGE_DETECT_GPIODAT) & EXTERN_CHARGE_DETECT_PIN) == 0)
#define IS_EXTERN_BATTERY_IN()         (1)
#else
#define EXTERN_CHARGE_DETECT_GPIOIN    GPIOAINEN
#define EXTERN_CHARGE_DETECT_GPIODAT   GPIOADAT
#define EXTERN_CHARGE_DETECT_PIN       (1 << 11)
#define EXTERN_CHARGE_FULL_GPIOIN      GPIOAINEN
#define EXTERN_CHARGE_FULL_GPIODAT     GPIOADAT
#define EXTERN_CHARGE_FULL_PIN         (1 << 9)

#define EXTERN_BATTERY_DETECT_GPIOIN   GPIOAINEN
#define EXTERN_BATTERY_DETECT_GPIODAT  GPIOADAT
#define EXTERN_BATTER_DETECT_PIN       (1 << 10)
#define IS_EXTERN_CAHREGE_IN()         ((act_readl(EXTERN_CHARGE_DETECT_GPIODAT) & EXTERN_CHARGE_DETECT_PIN) != 0)
#define IS_EXTERN_BATTERY_IN()         ((act_readl(EXTERN_BATTERY_DETECT_GPIODAT) & EXTERN_BATTER_DETECT_PIN) == 0)
#endif

#define EXTERN_CHARGE_VAL_MSK          (~(3 << 0))
#define EXTERN_CHARGE_VAL_SEL          (1 << 0)

//for tk plane power control
#define EXTERN_TK_PLANE_GPIOOUT       GPIOBOUTEN
#define EXTERN_TK_PLANE_GPIODAT       GPIOBDAT
#define EXTERN_TK_PLANE_PIN           (1 << 7)

//是否使用拨码开关检测是否需要关机(软关机)
//#define SWITCH_KEY_POWER_OFF

//BT BURN LED
#define BT_BURN_IO_EN_REG               GPIOAOUTEN
#define BT_BURN_IO_DATA_REG             GPIOADAT
#define BT_BURN_SET_BIT                 (1 << 11)
#define BT_BURN_CLEAR_BIT               (~(1 << 11))

//AUX检测相关配置
//ADCKEY检测方式
#define AUX_ADC_VALUE       (0x6d) //正负偏差3
//GPIO检测方式
#define AUX_GPIO_UPEN       GPIOAPUEN
#define AUX_GPIO_IN         GPIOAINEN
#define AUX_GPIO_DATA       GPIOADAT
#define AUX_GPIO_PIN        (1 << 15)
#define AUX_GPIO_EN_LEVEL   ENABLE_LEVEL_LOW
//SIO检测方式
#define AUX_SIO_PIN         (1 << 7)
#define AUX_SIO_EN_LEVEL    ENABLE_LEVEL_LOW

//HP GPIO检测相关配置
#define HP_GPIO_UPEN        GPIOAPUEN
#define HP_GPIO_IN          GPIOAINEN
#define HP_GPIO_DATA        GPIOADAT
#if (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825)
#define HP_GPIO_PIN         (1 << 23)
#else
#define HP_GPIO_PIN         (1 << 8)
#endif
#define HP_GPIO_EN_LEVEL    ENABLE_LEVEL_LOW

//SD卡GPIO检测相关配置
#if ((CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2825) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
#define CARD_GPIO_UPEN      GPIOBPUEN
#define CARD_GPIO_IN        GPIOBINEN
#define CARD_GPIO_DATA      GPIOBDAT
#define CARD_GPIO_PIN       (1 << 11)
#else
#define CARD_GPIO_UPEN      GPIOAPUEN
#define CARD_GPIO_IN        GPIOAINEN
#define CARD_GPIO_DATA      GPIOADAT
#define CARD_GPIO_PIN       (1 << 6)
#endif
#define CARD_GPIO_EN_LEVEL  ENABLE_LEVEL_LOW

//拨码开关的检测GPIO,这里使用A30,注意和uhost的power enable pin有冲突
#ifdef SWITCH_KEY_POWER_OFF
#define SHUTOFF_DETECT_IO_IN_EN_REG     GPIOAINEN
#define SHUTOFF_DETECT_IO_OUT_EN_REG    GPIOAOUTEN
#define SHUTOFF_DETECT_IO_DATA_REG      GPIOADAT
#define SHUTOFF_DETECT_SET_BIT          ((uint32)0x40000000)
#define SHUTOFF_DETECT_CLR_BIT          ((uint32)0xBFFFFFFF)
#define SHUTOFF_DETECT_IO_PU_EN_REG     GPIOAPUEN
#endif

#define ENABLE_USB_DETECT()         (GLOBAL_KERNEL_BACKDOOR_FLAG &= (~(BACKDOOR_DISUSB_DETECT)))
#define DISABLE_USB_DETECT()        (GLOBAL_KERNEL_BACKDOOR_FLAG |= (BACKDOOR_DISUSB_DETECT))

#define ENABLE_UHOST_DETECT()       (GLOBAL_KERNEL_BACKDOOR_FLAG &= (~(BACKDOOR_DISUHOST_DETECT)))
#define DISABLE_UHOST_DETECT()      (GLOBAL_KERNEL_BACKDOOR_FLAG |= (BACKDOOR_DISUHOST_DETECT))

//允许VRAM写操作
#define ENABLE_VRAM_WRITE()         (GLOBAL_KERNEL_BACKDOOR_FLAG &= (~(BACKDOOR_NOVRAM_WRITE)))
//禁止VRAM写操作
#define DISABLE_VRAM_WRITE()        (GLOBAL_KERNEL_BACKDOOR_FLAG |= (BACKDOOR_NOVRAM_WRITE))

#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))

#define AI_SOURCE_LINEIN AI_SOURCE_AUX0

#define AI_SOURCE_FM AI_SOURCE_AUX1

#else

#define AI_SOURCE_FM AI_SOURCE_AUX0

#define AI_SOURCE_LINEIN AI_SOURCE_AUX1

#endif

/*!
 * \brief
 *  usb_port_state_e 线缆状态枚举类型
 */
typedef enum {
    /*! USB Device */
    USB_PORT_DEVICE = 0,
    /*! USB Host */
    USB_PORT_HOST = 1,
} usb_port_state_e;

//获取当前usb端口设备状态
static inline uint8 get_usb_port_state(void)
{
    if((act_readb(DPDMCTRL) & 0x0f) != 0) {
        return USB_PORT_DEVICE;
    } else {
        return USB_PORT_HOST;
    }
}

//pc工具的类型
typedef enum {
    STUB_PC_TOOL_ASQT_MODE = 1,
    STUB_PC_TOOL_ASET_MODE = 2,
    STUB_PC_TOOL_ASET_EQ_MODE = 3,
    STUB_PC_TOOL_ATT_MODE = 4,
    STUB_PC_TOOL_TK_PMODE = 5,
    STUB_PC_TOOL_WAVES_ASET_MODE = 7,
    STUB_PC_TOOL_BTT_MODE = 0x42,
    STUB_PC_TOOL_UNKOWN = 0x7f
} PC_stub_mode_e;

//需要注册到按键驱动，在I2C TK驱动中调用
typedef void (*scan_isr_cbk)(void);

#ifdef BOARD_LINKPLAY_DEMO
#define KEYTONE_VOLUME_SAME_AS_SYSTEM_VOLUME
#define SUPPORT_SPI_FUNCTION

#define SYS_POWER_CTRL_UPEN     GPIOAPUEN
#define SYS_POWER_CTRL_OUTPUT   GPIOAOUTEN
#define SYS_POWER_CTRL_DATA     GPIOADAT
#define SYS_POWER_PIN           (1 << 21)
#endif

#if defined(BOARD_LINKPLAY_FAR_DEMO)
#define SUPPORT_EXTERNAL_INTERRUPT
#define EXTERN_INTERRUPT_RISING_EDGE_TRIG   0
#define EXTERN_INTERRUPT_PIN_A1             0
#define EXTERN_INTERRUPT_PIN_A7             0
#define EXTERN_INTERRUPT_PIN_A22            0
#define EXTERN_INTERRUPT_PIN_B11            1

#define SUPPORT_MIC_MUTE_FUNCTION
#define KEYTONE_VOLUME_SAME_AS_SYSTEM_VOLUME

#define SUPPORT_SPI_FUNCTION

#define ASR_DSP_RESET_GPIO_OUTEN            GPIOAOUTEN
#define ASR_DSP_RESET_GPIO_PUEN             GPIOAPUEN
#define ASR_DSP_RESET_GPIO_DATA             GPIOADAT
#define ASR_DSP_RESET_GPIO_PIN              (1 << 5)
#endif

#if defined(BOARD_DPI_7C370_FAR)
#define SUPPORT_POWER_ONOFF_DETECT_EN                   //开关机检测(长按开关机功能)
#define POWER_GPIO_UPEN       GPIOAPUEN
#define POWER_GPIO_IN         GPIOAINEN
#define POWER_GPIO_DATA       GPIOADAT
#define POWER_GPIO_PIN        (1 << 7)

#define SYS_POWER_CTRL_UPEN     GPIOAPUEN
#define SYS_POWER_CTRL_OUTPUT   GPIOAOUTEN
#define SYS_POWER_CTRL_DATA     GPIOADAT
#define SYS_POWER_PIN           (1 << 2)

#define SUPPORT_EXTERNAL_INTERRUPT
#define EXTERN_INTERRUPT_RISING_EDGE_TRIG   0
#define EXTERN_INTERRUPT_PIN_A1             0
#define EXTERN_INTERRUPT_PIN_A7             0
#define EXTERN_INTERRUPT_PIN_A22            0
#define EXTERN_INTERRUPT_PIN_B11            1

#define SUPPORT_MIC_MUTE_FUNCTION
#define KEYTONE_VOLUME_SAME_AS_SYSTEM_VOLUME

#define ASR_DSP_RESET_GPIO_OUTEN            GPIOAOUTEN
#define ASR_DSP_RESET_GPIO_PUEN             GPIOAPUEN
#define ASR_DSP_RESET_GPIO_DATA             GPIOADAT
#define ASR_DSP_RESET_GPIO_PIN              (1 << 10)

#define MIC_MUTE_GPIO_OUTEN                 GPIOAOUTEN
#define MIC_MUTE_GPIO_DATA                  GPIOADAT
#define MIC_MUTE_GPIO_PIN                   (1 << 13)
#endif

#if defined(BOARD_JL_36410_FAR)
#define SUPPORT_EXTERNAL_INTERRUPT
#define EXTERN_INTERRUPT_RISING_EDGE_TRIG   0
#define EXTERN_INTERRUPT_PIN_A1             0
#define EXTERN_INTERRUPT_PIN_A7             0
#define EXTERN_INTERRUPT_PIN_A22            0
#define EXTERN_INTERRUPT_PIN_B11            1

#define SUPPORT_MIC_MUTE_FUNCTION
#define KEYTONE_VOLUME_SAME_AS_SYSTEM_VOLUME

#define ASR_DSP_RESET_GPIO_OUTEN            GPIOAOUTEN
#define ASR_DSP_RESET_GPIO_PUEN             GPIOAPUEN
#define ASR_DSP_RESET_GPIO_DATA             GPIOADAT
#define ASR_DSP_RESET_GPIO_PIN              (1 << 10)

#define MIC_MUTE_GPIO_OUTEN                 GPIOAOUTEN
#define MIC_MUTE_GPIO_DATA                  GPIOADAT
#define MIC_MUTE_GPIO_PIN                   (1 << 13)

#define ENABLE_ALEXA_LED_NOTIFY
#endif

#endif
