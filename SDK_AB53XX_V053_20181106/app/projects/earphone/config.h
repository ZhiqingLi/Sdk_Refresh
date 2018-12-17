/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_FPGA_CONFIG_H
#define USER_FPGA_CONFIG_H
#include "config_define.h"
#include "xcfg.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define FUNC_MUSIC_EN                   0   //是否打开MUSIC功能
#define FUNC_CLOCK_EN                   0   //是否打开时钟功能
#define FUNC_FMRX_EN                    0   //是否打开FM功能
#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_BTHID_EN                   1   //是否打开独立自拍器模式
#define FUNC_AUX_EN                     0   //是否打开AUX功能
#define FUNC_USBDEV_EN                  0   //是否打开USB DEVICE功能
#define FUNC_SPEAKER_EN                 0   //是否打开Speaker模式
#define FUNC_FMAM_FREQ_EN               0   //是否打开FMAM读频率显示功能
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define BUCK_MODE_EN                    1                       //是否BUCK MODE
#define SYS_CLK_SEL                     SYS_26M                 //选择系统时钟
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#define SOFT_POWER_ON_OFF               xcfg_cb.soft_power_on_off_en //是否使用软开关机功能  //外部MOS管软开关机请开宏 USER_PWRKEY_EXT
#define USB_SD_UPDATE_EN                0                       //是否支持UDISK/SD的离线升级
#define EXT_32K_EN                      0                       //是否使用外部32K晶振,否则内部32K OSC。软关机需要闹钟唤醒时必须使用外部32K晶振
#define SYS_ADJ_DIGVOL_EN               0                       //系统是否调数字音量, 四声道车机的系统总音量调数字音量
#define GUI_SELECT                      GUI_NO                  //GUI Display Select
#define FLASH_SIZE                      FSIZE_512K              //LQFP48芯片内置1MB，其它封装芯片内置512KB(实际导出prd文件要小于492K)
#define UART0_PRINTF_SEL                PRINTF_NONE             //选择UART打印信息输出IO，或关闭打印信息输出

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#define MUSIC_UDISK_EN                  0   //是否支持播放UDISK
#define MUSIC_SDCARD_EN                 0   //是否支持播放SDCARD

#define MUSIC_WAV_SUPPORT               0   //是否支持WAV格式解码
#define MUSIC_WMA_SUPPORT               0   //是否支持WMA格式解码
#define MUSIC_APE_SUPPORT               0   //是否支持APE格式解码
#define MUSIC_FLAC_SUPPORT              0   //是否支持FLAC格式解码
#define MUSIC_SBC_SUPPORT               0   //是否支持SBC格式解码

#define MUSIC_FOLDER_SELECT_EN          0   //文件夹选择功能
#define MUSIC_AUTO_SWITCH_DEVICE        0   //双设备循环播放
#define MUSIC_BREAKPOINT_EN             0   //音乐断点记忆播放
#define MUSIC_QSKIP_EN                  0   //快进快退功能
#define MUSIC_PLAYMODE_NUM              4   //音乐播放模式总数
#define MUSIC_MODE_RETURN               1   //退出音乐模式之后是否返回原来的模式
#define MUSIC_PLAYDEV_BOX_EN            0   //是否显示“USB”, "SD"界面
#define MUSIC_ID3_TAG_EN                0   //是否获取MP3 ID3信息
#define MUSIC_REC_FILE_FILTER           0   //是否区分录音文件与非录音文件分别播放

#define IPHONE_POWER_VAL                50  //苹果充电电流设置
#define IPHONE_POWER_INDEX              190 //苹果充电电流设置

/*****************************************************************************
 * Module    : 蓝牙功能配置
 *****************************************************************************/
#define BT_BACKSTAGE_EN                 0   //蓝牙后台管理（全模式使用蓝牙）
#define BT_BACKSTAGE_PLAY_DETECT_EN     0   //非蓝牙模式下检测到手机蓝牙播放音乐，则切换到蓝牙模式
#define BT_NAME_DEFAULT                 "BT-BOX"     //默认蓝牙名称（不超过31个字符）
#define BT_NAME_WITH_ADDR_EN            0   //蓝牙名称是否附加地址信息（调试用，例如：btbox-***）
#define BT_POWER_UP_RECONNECT_TIMES     xcfg_cb.bt_power_up_reconnect_times   //上电回连次数
#define BT_TIME_OUT_RECONNECT_TIMES     xcfg_cb.bt_time_out_reconnect_times   //掉线回连次数
#define BT_SIMPLE_PAIR_EN               1   //是否打开蓝牙简易配对功能（关闭时需要手机端输入PIN码）
#define BT_DISCOVER_CTRL_EN             0   //是否使用按键打开可被发现（按一下按键才能被连接配对）
#define BT_PWRKEY_5S_DISCOVER_EN        1   //是否使用长按5S开机进入可被发现(耳机长按开机功能)
#define BT_DISCOVER_TIMEOUT             100 //按键打开可被发现后，多久后仍无连接自动关闭，0不自动关闭，单位100ms
#define BT_ANTI_LOST_EN                 0   //是否打开蓝牙防丢报警
#define BT_CBT_TEST_EN                  0   //蓝牙CBT测试使能
#define BT_FCC_TEST_EN                  0   //蓝牙FCC测试使能
#define BT_LOCAL_ADDR                   0   //蓝牙是否使用本地地址，0使用配置工具地址
#define BT_PAIR_SLEEP_EN                xcfg_cb.bt_pair_sleep_en    //配对模式下是否进入休眠模式

#define BT_TWS_EN                       1   //是否支持TWS
#define BT_2ACL_EN                      1   //是否支持连接两部手机
#define BT_A2DP_EN                      1   //是否打开蓝牙音乐服务
#define BT_HFP_EN                       1   //是否打开蓝牙通话服务
#define BT_SPP_EN                       1   //是否打开蓝牙串口服务
#define BT_HID_EN                       1   //是否打开蓝牙HID服务（自拍器）
#define BT_PBAP_EN                      0   //是否打开蓝牙电话簿服务
#define BT_HID_MENU_EN                  1   //蓝牙HID是否需要手动连接/断开
#define BT_HID_DISCON_DEFAULT_EN        1   //蓝牙HID服务默认不连接，需要手动进行连接。
#define BT_HFP_CALL_PRIVATE_EN          1   //是否使能私密接听与蓝牙接听切换功能
#define BT_HFP_CALL_PRIVATE_FORCE_EN    0   //是否强制使用私密接听（手机端接听）
#define BT_HFP_RING_NUMBER_EN           1   //是否支持来电报号
#define BT_HFP_PLAY_IOS_RING_EN         0   //是否支持ios来电铃声（android默认用RING提示音）
#define BT_HFP_BAT_REPORT_EN            1   //是否支持电量显示
#define BT_HFP_MSBC_EN                  0   //是否打开宽带语音功能
#define BT_SCO_DBG_EN                   0   //是否打开无线调试通话参数功能
#define BT_A2DP_VOL_CTRL_EN             1   //是否支持A2DP音量与手机同步
#define BT_HSF_CALL_NUMBER_EN           0   //是否支持获取回拔,主动拨出电话的号码

//通话参数
#define BT_PLC_EN                       1
#define BT_NOISE_THRESHOID              xcfg_cb.bt_noise_threshoid  //环境噪声阈值（低于此阈值便认为是噪声）(0~65535)
#define BT_ANL_GAIN                     xcfg_cb.bt_anl_gain         //MIC模拟增益(0~31)
#define BT_DIG_GAIN                     xcfg_cb.bt_dig_gain         //MIC数字增益(0-31),且于0~3DB范围细调,步进3/32DB
#define BT_CALL_MAX_GAIN                xcfg_cb.bt_call_max_gain    //配置通话时DAC最大模拟增益

#define BT_AEC_EN                       1
#define BT_ECHO_LEVEL                   xcfg_cb.bt_echo_level       //回声消除级别（级别越高，回声衰减越明显，但通话效果越差）(0~15)
#define BT_FAR_OFFSET                   xcfg_cb.bt_far_offset       //远端补偿值(0~255)

#define BT_ALC_EN                       1                           //是否使能ALC
#define BT_ALC_FADE_IN_DELAY            xcfg_cb.bt_alc_in_delay     //近端淡入延时
#define BT_ALC_FADE_IN_STEP             xcfg_cb.bt_alc_in_step      //近端淡入速度
#define BT_ALC_FADE_OUT_DELAY           xcfg_cb.bt_alc_out_delay    //远端淡入延时
#define BT_ALC_FADE_OUT_STEP            xcfg_cb.bt_alc_out_step     //远端淡入速度
#define BT_ALC_VOICE_THR                0x20000

#define LE_EN                           0   //是否打开BLE功能
#define LE_LIGHTING_EN                  0   //是否打开BLE灯光控制服务
#define LE_MUSIC_CTRL_EN                0   //是否打开BLE音乐控制服务


/*****************************************************************************
 * Module    : 时钟功能配置
 *****************************************************************************/
#define ALARM_WAKEUP_EN                 0   //是否使能ALARM软关机唤醒(需要使用外部32K晶振)


/*****************************************************************************
 * Module    : FMRX功能配置
 *****************************************************************************/
#define FMRX_HALF_SEEK_EN               1   //是否打开半自动搜台
#define FMRX_INSIDE_EN                  1   //是否使用内置收音
#define FMRX_QN8035_EN                  0

///外接收音芯片相关配置
#define FMRX_2_SDADC_EN                 1                               //外接收音是否进SDADC，否则直通DAC。进SDADC可以调EQ，FMRX录音等功能。
#define FMRX_CHANNEL_CFG               (CH_AUXL_PF0 | CH_AUXR_PF1)      //选择FMRX的通路
#define FMRX_AUX_ANL_GAIN               AUX_N0DB                        //AUX输入的模拟增益
#define FMRX_CLOCK                      FM_USE_CRYSTAL_32K
#define FMRX_CLKOMAP                    CLKOMAP_PB1

///内置收音的参数配置
#define FMRX_RFCAP_VAL                  0          						//范围0~31, 设置外部电感的匹配电容。无外部电感时，配置为0。
#define FMRX_THRESHOLD_VAL              108      						//内置FMRX搜台阈值, 值越小台越多，假台也可能增多。
#define FMRX_THRESHOLD_Z                1700                            //想减少很弱的台，可以适当改小
#define FMRX_THRESHOLD_FZ               800                             //想减少很弱的台，可以适当改小
#define FMRX_THRESHOLD_D                3000


/*****************************************************************************
* Module    : AUX功能配置
******************************************************************************/
#define AUX_CHANNEL_CFG                 (CH_AUXL_PF0 | CH_AUXR_PF1) //选择LINEIN通路
#define FM_CHANNEL_CFG                  (CH_AUXL_PF0 | CH_AUXR_PF1) //选择FM的通路
#define MIC_CHANNEL_CFG                 CH_MICL0                    //选择MIC的通路
#define AUX_2_SDADC_EN                  1   //AUX是否进SDADC, 否则直通DAC。进SDADC可以调AUX EQ, 及AUX录音等功能
#define LINEIN_DETECT_EN                1   //是否打开LINEIN检测
#define SDCMD_MUX_DETECT_LINEIN         0   //是否复用SDCMD检测LINEIN插入
#define MICL_MUX_DETECT_LINEIN          1   //是否复用MICL/PF2检测LINEIN插入
#define LINEIN_2_PWRDOWN_EN             1   //是否插入Linein后直接软关机（大耳包功能）
#define LINEIN_2_PWRDOWN_TONE_EN        xcfg_cb.linein_2_pwrdown_tone_en   //插入Linein直接关机，是否播放关机提示音。

//#if SDCMD_MUX_DETECT_LINEIN
//    #define LINEIN_DETECT_INIT()
//    #define LINEIN_IS_ONLINE()          SD_CMD_MUX_IS_ONLINE()
//    #define IS_DET_LINEIN_BUSY()        SD_MUX_IS_BUSY()
//#elif MICL_MUX_DETECT_LINEIN
//    #define LINEIN_DETECT_INIT()        {GPIOFDE |= BIT(2); GPIOFPU |= BIT(2); GPIOFDIR |= BIT(2);}
//    #define LINEIN_IS_ONLINE()          linein_micl_is_online()
//    #define IS_DET_LINEIN_BUSY()        (0)
//#else
//    #define LINEIN_DETECT_INIT()        {GPIOFDE |= BIT(4); GPIOFPU |= BIT(4); GPIOFDIR |= BIT(4);}
//    #define LINEIN_IS_ONLINE()          (!(GPIOF & BIT(4)))
//    #define IS_DET_LINEIN_BUSY()        (0)
//#endif

#define LINEIN_DETECT_INIT()        linein_detect_init()
#define LINEIN_IS_ONLINE()          linein_is_online()
#define IS_DET_LINEIN_BUSY()        is_detect_linein_busy()


/*****************************************************************************
* Module    : DAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      xcfg_cb.dac_sel         //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_FAST_SETUP_EN               0                       //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain    //配置DAC最大模拟增益，默认设置为dac_vol_table[VOL_MAX]
#define DAC_MAX_DIGVOL                  xcfg_cb.dac_max_digvol  //系统最大数字音量 0x7e00
#define DAC_OUT_SPR                     DAC_OUT_44K1            //dac out sample rate
#define DAC_LDOH_SEL                    xcfg_cb.dac_ldoh_sel
#define DAC_VCM_CAPLESS_EN              xcfg_cb.dac_vcm_less_en //DAC VCM省电容方案,使用内部VCM
#define DAC_PULL_DOWN_DELAY             xcfg_cb.dac_pull_down_delay     //控制DAC隔直电容放电等待时间, 无电容时可设为0，减少开机时间。
#define DAC_LOWPWR_EN                   1
#define DAC_DNR_EN                      1                       //是否使能动态降噪
#define DAC_AUCAR_EN                    0                       //是否四声道车机前后左右音量调节及音效设置
#define DAC_DRC_EN                      0                       //是否使能DRC功能（系统时钟需要大于48MHz）

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#define FUNC_REC_EN                     0   //录音功能总开关
#define FMRX_REC_EN                     0   //是否打开FM录音功能
#define AUX_REC_EN                      0   //是否打开AUX录音功能
#define MIC_REC_EN                      0   //是否打开MIC录音功能
#define BT_REC_EN                       0   //是否打开蓝牙音乐录音功能
#define KARAOK_REC_EN                   0   //是否打开KARAOK录音功能，需同步打开KARAOK功能
#define REC_ONE_FOLDER_EN               0   //是否各模式录音放到同一目录下
#define REC_DIR_LFN                     0   //是否打开长文件名目录
#define REC_AUTO_PLAY                   0   //录音结束是否回放当前录音内容
#define REC_FAST_PLAY                   0   //播卡播U下快速播放最新的录音文件
#define REC_STOP_MUTE_1S                0   //录音停止时, MUTE 1S功放. //提醒客户录音结束.
#define REC_TYPE_SEL                    REC_MP3

/*****************************************************************************
 * Module    : K歌功能配置
 *****************************************************************************/
#define SYS_KARAOK_EN                   0   //是否打开K歌功能
#define SYS_KARAOK_BUFMODE              0   //使用降采样来加倍混响深度
#define BT_HFP_CALL_KARAOK_EN           0   //通话是否支持KARAOK.
#define SYS_ECHO_EN                     0   //是否使能混响功能
#define SYS_ECHO_DELAY                  16  //混响间隔级数
#define SYS_BASS_TREBLE_EN              0   //是否使能高低音调节
#define SYS_MAGIC_VOICE_EN              0   //是否使能魔音功能

#define MIC_DETECT_EN                   0   //是否使能MIC检测
#define MIC_DNR_EN                      0   //是否使能MIC动态降噪

///通过配置工具选择检测GPIO
#define MIC_DETECT_INIT()               //mic_detect_init()
#define MIC_IS_ONLINE()                 //mic_is_online()
#define IS_DET_MIC_BUSY()               //is_detect_mic_busy()

/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
#define USER_ADKEY                      1           //ADKEY的使用， 0为不使用
#define USER_ADKEY2                     0           //ADKEY2的使用，0为不使用
#define USER_PWRKEY                     1           //PWRKEY的使用，0为不使用
#define USER_IOKEY                      1           //IOKEY的使用， 0为不使用
#define USER_ADKEY_MUX_SDCLK            0           //是否使用复用SDCLK的ADKEY, 共用USER_ADKEY的按键table
#define ADKEY_PU10K_EN                  0           //ADKEY是否使用内部10K上拉, 按键数量及阻值见port_key.c

#define USER_KEY_DOUBLE_EN              1           //按键双击使能
#define USER_KEY_THRICE_EN              0           //按键三击功能，需要先开双击功能
#define USER_KEY_DOUBLE                 {KU_PLAY, KU_VOL_DOWN_PREV, KU_VOL_UP_NEXT}     //支持双击的按键
#define ADKEY_CH                        xcfg_cb.adkey_ch    //ADCCH_PE7
#define ADKEY2_CH                       xcfg_cb.adkey2_ch   //ADCCH_PE6

#define PWRON_PRESS_TIME                (1500+500*xcfg_cb.pwron_press_time)             //长按PWRKEY多长时间开机？
#define PWROFF_PRESS_TIME               (3+3*xcfg_cb.pwroff_press_time)                 //长按PWRKEY多长时间关机？
#define DOUBLE_KEY_TIME                 (xcfg_cb.double_key_time)

#define KEY_MSG_REMAP_EN               1             //支持按键重映射, 在配置工具中选则映射按键

#define USER_PWRKEY_EXT                 0           //是否使用外部软开关机功能, 需要定义控制MOS管的IO口.
#define IS_PWRKEY_PRESS()               0 //(0 == (GPIOA & BIT(3)))
#define PWRKEY_EXT_INIT()               //{GPIOADE |= BIT(3); GPIOASET = BIT(3); GPIOADIR &= ~BIT(3);}
#define PWRKEY_EXT_POWEROFF()           //{GPIOACLR = BIT(3); while(1) { WDT_CLR();}}
#define PWRKEY_EXT_SLEEP_HOLD()         //GPIOADE = BIT(3);   //进入遥控休眠需要保持有电
/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SDCLK_MUX_DETECT_SD             0           //是否复用SDCLK检测SD卡
#define SD0_MAPPING                     SD0MAP_G2   //选择SD0 mapping

//不复用检测时，需要定义SD检测引脚
#if !SDCLK_MUX_DETECT_SD
#define SD_DETECT_INIT()                {GPIOEDE |= BIT(5); GPIOEPU  |= BIT(5); GPIOEDIR |= BIT(5);}
#define SD_IS_ONLINE()                  (!(GPIOE & BIT(5)))
#define IS_DET_SD_BUSY()                (0)         //不复用SDCLK检测时，恒为0
#else
#define SD_DETECT_INIT()                SD_MUX_DETECT_INIT()
#define SD_IS_ONLINE()                  SD_MUX_IS_ONLINE()
#define IS_DET_SD_BUSY()                SD_MUX_IS_BUSY()
#endif

/*****************************************************************************
 * Module    : I2C配置
 *****************************************************************************/
#define I2C_EN                          0           //是否使能I2C功能
#define I2C_MUX_SD_EN                   0           //是否I2C复用SD卡的IO

#define I2C_SCL_IN()                    SD_CMD_DIR_IN()
#define I2C_SCL_OUT()                   SD_CMD_DIR_OUT()
#define I2C_SCL_H()                     SD_CMD_OUT_H()
#define I2C_SCL_L()                     SD_CMD_OUT_L()

#define I2C_SDA_IN()                    SD_DAT_DIR_IN()
#define I2C_SDA_OUT()                   SD_DAT_DIR_OUT()
#define I2C_SDA_H()                     SD_DAT_OUT_H()
#define I2C_SDA_L()                     SD_DAT_OUT_L()
#define I2C_SDA_IS_H()                  SD_DAT_STA()

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}

/*****************************************************************************
 * Module    : I2S配置
 *****************************************************************************/
#define I2S_EN                          0           //是否使能I2S功能
#define I2S_DEVICE                      I2S_DEV_NO //I2S设备选择
#define I2S_MAPPING_SEL                 I2S_GPIOA   //I2S IO口选择
#define I2S_MODE_SEL                    1           //I2S主从模式选择 0: master; 1:slave
#define I2S_BIT_MODE                    0           //I2S数据位宽选择 0:16bit; 1:32bit
#define I2S_DATA_MODE                   1           //I2S数据格式选择 0:left-justified mode; 1:normal mode
#define I2S_DMA_EN                      1           //I2S数据源选择 0:src; 1:dma
#define I2S_MCLK_EN                     0           //I2S是否打开MCLK
#define I2S_MCLK_SEL                    2           //I2S MCLK选择 0:64fs 1:128fs 2:256fs
#define I2S_PCM_MODE                    0           //I2S是否打开PCM mode


/*****************************************************************************
 * Module    : IRRX配置
 *****************************************************************************/
#define IRRX_HW_EN                      0           //是否打IRRX硬件模块
#define IRRX_SW_EN                      0           //是否打开timer capture ir
#define IR_NUMKEY_EN                    0           //是否打开数字键输入

//可以打开一个或多个
#define IR_ADDR_FF00_EN                 1
#define IR_ADDR_BF00_EN                 0
#define IR_ADDR_FD02_EN                 1
#define IR_ADDR_FE01_EN                 0
#define IR_ADDR_7F80_EN                 0

#define IR_CAPTURE_PORT()               {GPIOEDE |= BIT(6); GPIOEPU  |= BIT(6); GPIOEDIR |= BIT(6);}
#define IRRX_MAPPING                    IRMAP_PE6
#define TMR3CAP_MAPPING                 TMR3MAP_PE6


/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                 1
#define UDE_SPEAKER_EN                 1
#define UDE_HID_EN                     1
#define UDE_MIC_EN                     1


/*****************************************************************************
 * Module    : 系统细节配置
 *****************************************************************************/
#define PWM_RGB_EN                      0           //PWM RGB三色灯功能
#define SYS_PARAM_RTCRAM                0           //是否系统参数保存到RTCRAM
#define PWRON_ENTER_BTMODE_EN           1           //是否上电默认进蓝牙模式
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VBAT2_ADCCH                     ADCCH_VBAT  //ADCCH_VBAT为内部1/2电压通路，带升压应用需要外部ADC通路检测1/2电池电压
#define EQ_MODE_EN                      0           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define EQ_DBG_IN_UART                  0           //是否使能UART在线调节EQ
#define EQ_DBG_IN_SPP                   1           //是否使能SPP在线调节EQ
#define PWRKEY_2_HW_PWRON               0           //用PWRKEY模拟硬开关
#define PWRON_FRIST_BAT_EN              xcfg_cb.pwron_frist_bat_en  //第一次接电池上电是否直接开机
#define SLEEP_DAC_OFF_EN                (is_sleep_dac_off_enable()) //sfunc_sleep是否关闭DAC， 大耳包有些应用方案不能关DAC。
#define SYS_INIT_VOLUME                 xcfg_cb.sys_init_vol        //系统默认音量
#define SYS_LIMIT_VOLUME                SYS_INIT_VOLUME             //开机小于SYS_LIMIT_VOLUME还原到SYS_INIT_VOLUME
#define LPWR_WARNING_VBAT               xcfg_cb.lpwr_warning_vbat   //低电提醒电压
#define LPWR_OFF_VBAT                   xcfg_cb.lpwr_off_vbat       //低电关机电压

/*****************************************************************************
 * Module    : LED指示灯配置
 *****************************************************************************/
#define LED_DISP_EN                     1           //是否使用LED指示灯(蓝灯)
#define LED_PWR_EN                      1           //充电及电源指示灯(红灯)
#define LED_LOWBAT_EN                   xcfg_cb.rled_lowbat_en      //电池低电是否闪红灯
#define RLED_LOWBAT_FOLLOW_EN           xcfg_cb.rled_lowbat_follow_en //红灯是否跟随语音闪烁
#define BLED_FFT_EN                     xcfg_cb.bled_fft_en         //蓝牙音乐播放时蓝灯是否按音乐旋律闪烁
#define BLED_LOW2ON_EN                  xcfg_cb.bled_low2on_en      //蓝牙是否低电平亮
#define BLED_CHARGE_FULL_EN             xcfg_cb.charge_full_bled    //充电满是否亮蓝灯
#define BT_RECONN_LED_EN                xcfg_cb.bt_recon_led_en     //蓝牙回连状态结束后是否切回配对模式闪烁

#define LED_INIT()                      bled_func.port_init(&bled_gpio)
#define LED_SET_ON()                    bled_func.set_on(&bled_gpio)
#define LED_SET_OFF()                   bled_func.set_off(&bled_gpio)

#define LED_PWR_INIT()                  rled_func.port_init(&rled_gpio)
#define LED_PWR_SET_ON()                rled_func.set_on(&rled_gpio)
#define LED_PWR_SET_OFF()               rled_func.set_off(&rled_gpio)

/*****************************************************************************
 * Module    : Loudspeaker mute及耳机检测配置
 *****************************************************************************/
#define EARPHONE_DETECT_EN              0           //是否打开耳机检测
#define SDCMD_MUX_DETECT_EARPHONE       0           //是否复用SDCMD检测耳机插入

#if !SDCMD_MUX_DETECT_EARPHONE
#define EARPHONE_DETECT_INIT()          {GPIOFDE |= BIT(2); GPIOFPU  |= BIT(2); GPIOFDIR |= BIT(2);}
#define EARPHONE_IS_ONLINE()            (!(GPIOF & BIT(2)))
#define IS_DET_EAR_BUSY()               (0)
#else
#define EARPHONE_DETECT_INIT()
#define EARPHONE_IS_ONLINE()            SD_CMD_MUX_IS_ONLINE()
#define IS_DET_EAR_BUSY()               SD_MUX_IS_BUSY()
#endif

#define LOUDSPEAKER_MUTE_EN             1           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_UNMUTE_DELAY        xcfg_cb.loudspeaker_unmute_delay     //UNMUTE延时配置，单位为5ms

#define AMPLIFIER_SEL_INIT()            //{GPIOEDE |= BIT(6); GPIOEDIR &= ~BIT(6); GPIOE |= BIT(6);}
#define AMPLIFIER_SEL_D()               //{GPIOEDIR &= ~BIT(6); GPIOE |= BIT(6);}
#define AMPLIFIER_SEL_AB()              //{GPIOEDIR &= ~BIT(6); GPIOE &= ~BIT(6);}


/*****************************************************************************
 * Module    : 充电功能选择
 *****************************************************************************/
#define CHARGE_EN                       1           //是否打开充电功能
#define CHARGE_FINISH_INTO_LOWPOWER     0           //充电完成进入低功耗.
#define CHARGE_OUT_POWER_ON             0           //充电时, 拔出充电则开机(DC不断电才支持)
#define CHARGE_TRICK_EN                 xcfg_cb.charge_trick_en     //是否打开涓流充电功能
#define CHARGE_DC_RESET                 xcfg_cb.charge_dc_reset     //是否打开DC插入复位功能
#define CHARGE_DC_NOT_PWRON             xcfg_cb.charge_dc_not_pwron //DC插入，是否软开机。 1: DC IN时不能开机
#define SUPPORT_EXT_CHARGE_IC           xcfg_cb.support_ext_charge_ic //支持外接充电IC时,外接充电IC和芯片充电引脚并连在一起,此时会关掉内部充电. "插入DC复位系统" 或 "插入DC禁止软开机" 则可以根据需要配置"
#define CHARGE_DC_IN()                  ((RTCCON >> 20) & 0x01)

//充电截止电流：0:2.5mA; 1:5mA; 2:10mA; 3:15mA; 4:20mA; 5:25mA; 6:30mA; 7:35mA
#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr    //0~7
//充电截止电压：0:3.9v；1:4v；2:4.1v 3:4.15v
#define CHARGE_STOP_VOLT                3    //0~3
//恒流充电（电池电压大于2.9v）电流：2:30mA; 3:40mA; 4:50mA; 5:60mA; 6:70mA; 7:80mA; 8:90mA; 9:100mA;
//                                  10:110mA; 11:120mA; 12:140mA; 13:160mA; 14:180mA; 15:200mA
#define CHARGE_CONSTANT_CURR            (xcfg_cb.charge_constant_curr+2)
//涓流截止电压：0:2.9v; 1:3v
#define CHARGE_TRICK_STOP_VOLT          1
//涓流充电（电池电压小于2.9v）电流：2:30mA; 3:40mA; 4:50mA;
#define CHARGE_TRICKLE_CURR             (xcfg_cb.charge_trickle_curr+2)


/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 1            //是否打开提示音功能, 总开关
#define WARING_MAXVOL_MP3               xcfg_cb.maxvol_tone_is_mp3  //最大音量提示音WAV或MP3选择， 播放WAV可以与MUSIC叠加播放。
#define WARNING_VOLUME                  xcfg_cb.warning_volume      //播放提示音的音量级数
#define LANG_SELECT                     LANG_EN_ZH   //提示音语言选择

#define WARNING_POWER_ON                1
#define WARNING_POWER_OFF               1
#define WARNING_FUNC_MUSIC              0
#define WARNING_FUNC_BT                 1
#define WARNING_FUNC_CLOCK              0
#define WARNING_FUNC_FMRX               0
#define WARNING_FUNC_AUX                1
#define WARNING_FUNC_USBDEV             0
#define WARNING_FUNC_SPEAKER            0
#define WARNING_LOW_BATTERY             1
#define WARNING_BT_WAIT_CONNECT         1
#define WARNING_BT_CONNECT              1
#define WARNING_BT_DISCONNECT           1
#define WARNING_BT_INCALL               1
#define WARNING_USB_SD                  0
#define WARNING_MAX_VOLUME              1
#define WARNING_MIN_VOLUME              1
#define WARNING_ADJ_VOLUME              1
#define WARNING_BT_HID_MENU             1            //BT HID MENU手动连接/断开HID Profile提示音
#define WARNING_BTHID_CONN              0            //BTHID模式是否有独立的连接/断开提示音
#define WARNING_BT_PAIR                 1            //BT PAIRING提示音


#include "config_extra.h"

#endif // USER_FPGA_CONFIG_H
