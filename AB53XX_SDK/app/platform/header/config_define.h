/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H
//SDK_AB53XX_V061_20190103
/*****************************************************************************
 * Module    : 显示相关配置选择列表
 *****************************************************************************/
//显示驱动屏选择
#define DISPLAY_NO                      0                                       //无显示模块
#define DISPLAY_LCD                     0x100                                   //选用LCD点阵屏做为显示驱动
#define DISPLAY_LCDSEG                  0x200                                   //选用断码屏做为显示驱动
#define DISPLAY_LEDSEG                  0x400                                   //选用数码管做为显示驱动

#define GUI_NO                          DISPLAY_NO                              //无主题，无显示
#define GUI_LCD                         (DISPLAY_LCD | 0x00)                    //LCD点阵屏(128*64)
#define GUI_LCDSEG                      (DISPLAY_LCDSEG | 0x00)                 //断码屏默认主题
#define GUI_LEDSEG_5C7S                 (DISPLAY_LEDSEG | 0x00)                 //5C7S 数码管
#define GUI_LEDSEG_7P7S                 (DISPLAY_LEDSEG | 0x01)                 //7PIN 数码管 按COM方式扫描
#define GUI_LEDSEG_3P7S                 (DISPLAY_LEDSEG | 0x02)                 //3PIN 7段数码管
#define GUI_SPISEG_16XX                 (DISPLAY_LEDSEG | 0x03)                 //spi断码驱动IC类

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
 #define FSIZE_1M                       0x100000
 #define FSIZE_512K                     0x80000


/*****************************************************************************
 * Module    : 提示音语言配置选择列表
 *****************************************************************************/
#define LANG_EN         0               //英文提示音
#define LANG_ZH         1               //中文提示音
#define LANG_EN_ZH      2               //英文、中文双语提示音

/*****************************************************************************
 * Module    : ADC通路选择列表
 *****************************************************************************/
#define ADCCH_PA5       0               //SARADC channel 0
#define ADCCH_PA6       1               //SARADC channel 1
#define ADCCH_PA7       2               //SARADC channel 2
#define ADCCH_PB1       3               //SARADC channel 3
#define ADCCH_PB2       4               //SARADC channel 4
#define ADCCH_PB3       5               //SARADC channel 5
#define ADCCH_PB4       6               //SARADC channel 6
#define ADCCH_PE5       7               //SARADC channel 7
#define ADCCH_PE6       8               //SARADC channel 8
#define ADCCH_PE7       9               //SARADC channel 9                  ADS7844 ADC0
#define ADCCH_PF2       10              //SARADC channel 10
#define ADCCH_VRTC      11              //SARADC channel 11
#define ADCCH_WKO       12              //SARADC channel 12
#define ADCCH_BGOP      13              //SARADC channel 13
#define ADCCH_VBAT      14              //SARADC channel 14
#define ADCCH_VUSB      15              //SARADC channel 15

/*****************************************************************************
 * Module    : Timer3 Capture Mapping选择列表
 *****************************************************************************/
#define TMR3MAP_PA5     (1 << 4)        //G1  capture mapping: PA5
#define TMR3MAP_PA6     (2 << 4)        //G2  capture mapping: PA6
#define TMR3MAP_PB0     (3 << 4)        //G3  capture mapping: PB0
#define TMR3MAP_PB1     (4 << 4)        //G4  capture mapping: PB1
#define TMR3MAP_PE0     (5 << 4)        //G5  capture mapping: PE0
#define TMR3MAP_PE5     (6 << 4)        //G6  capture mapping: PE5
#define TMR3MAP_PE6     (7 << 4)        //G7  capture mapping: PE6

/*****************************************************************************
 * Module    : IRRX Mapping选择列表
 *****************************************************************************/
#define IRMAP_PA5       (1 << 20)       //G1  capture mapping: PA5
#define IRMAP_PA6       (2 << 20)       //G2  capture mapping: PA6
#define IRMAP_PB0       (3 << 20)       //G3  capture mapping: PB0
#define IRMAP_PB1       (4 << 20)       //G4  capture mapping: PB1
#define IRMAP_PE0       (5 << 20)       //G5  capture mapping: PE0
#define IRMAP_PE5       (6 << 20)       //G6  capture mapping: PE5
#define IRMAP_PE6       (7 << 20)       //G7  capture mapping: PE6
#define IRMAP_PE7       (8 << 20)       //G8  capture mapping: PE7
#define IRMAP_PF2       (9 << 20)       //G9  capture mapping: PF2


/*****************************************************************************
 * Module    : LPWM(Breathing-lamp) Mapping选择列表
 *****************************************************************************/
#define LPWM0MAP_PE4    (1 << 16)       //G1 LPWM0 mapping: PE4
#define LPWM1MAP_PE5    (1 << 20)       //G1 LPWM1 mapping: PE5
#define LPWM2MAP_PE6    (1 << 24)       //G1 LPWM2 mapping: PE6
#define LPWM3MAP_PE7    (1 << 28)       //G1 LPWM3 mapping: PE7

#define LPWM0MAP_PB3    (2 << 16)       //G2 LPWM0 mapping: PB3
#define LPWM1MAP_PB4    (2 << 20)       //G2 LPWM1 mapping: PB4
#define LPWM2MAP_PE0    (2 << 24)       //G2 LPWM2 mapping: PE0
#define LPWM3MAP_PF2    (2 << 28)       //G2 LPWM3 mapping: PF2

#define LPWM0MAP_PA0    (3 << 16)       //G3 LPWM0 mapping: PA0
#define LPWM1MAP_PA1    (3 << 20)       //G3 LPWM1 mapping: PA1
#define LPWM2MAP_PA2    (3 << 24)       //G3 LPWM2 mapping: PA2
#define LPWM3MAP_PA3    (3 << 28)       //G3 LPWM3 mapping: PA3


/*****************************************************************************
 * Module    : Clock output Mapping选择列表
 *****************************************************************************/
#define CLKOMAP_PA5     (1 << 0)        //G1 Clock output mapping: PA5
#define CLKOMAP_PA6     (2 << 0)        //G2 Clock output mapping: PA6
#define CLKOMAP_PB0     (3 << 0)        //G3 Clock output mapping: PB0
#define CLKOMAP_PB1     (4 << 0)        //G4 Clock output mapping: PB1
#define CLKOMAP_PE5     (5 << 0)        //G5 Clock output mapping: PE5
#define CLKOMAP_PE6     (6 << 0)        //G6 Clock output mapping: PE6


/*****************************************************************************
 * Module    : sd0 Mapping选择列表
 *****************************************************************************/
#define SD0MAP_G1       (1 << 0)       //G1  SDCLK(PA6), SDCMD(PA5), SDDAT0(PA7), SDDAT1(PA4), SDDAT2(PA3), SDDAT3(PA2)
#define SD0MAP_G2       (2 << 0)       //G2  SDCLK(PB1), SDCMD(PB0), SDDAT0(PB2)
#define SD0MAP_G3       (3 << 0)       //G3  SDCLK(PE6), SDCMD(PE5), SDDAT0(PE7), SDDAT1(PE4), SDDAT2(PE3), SDDAT3(PE2)
#define SD0MAP_G4       (4 << 0)       //G4  SDCLK(PA6), SDCMD(PA5), SDDAT0(PB4)
#define SD0MAP_G5       (5 << 0)       //G5  SDCLK(PA6), SDCMD(PA5), SDDAT0(PB3)
#define SD0MAP_G6       (6 << 0)       //G6  SDCLK(PA6), SDCMD(PB3), SDDAT0(PB4)


/*****************************************************************************
 * Module    : spi1 Mapping选择列表 (FUNCMCON1)
 *****************************************************************************/
#define SPI1MAP_G1     (1 << 12)        //G1 SPI1CLK(PA3), SPI1DI(PA2), SPI1DO(PA4)
#define SPI1MAP_G2     (2 << 12)        //G2 SPI1CLK(PA6), SPI1DI(PA5), SPI1DO(PA7)
#define SPI1MAP_G3     (3 << 12)        //G3 SPI1CLK(PB1), SPI1DI(PB0), SPI1DO(PB2)
#define SPI1MAP_G4     (4 << 12)        //G4 SPI1CLK(PE6), SPI1DI(PE5), SPI1DO(PE7)
#define SPI1MAP_G5     (5 << 12)        //G5 SPI1CLK(PF1), SPI1DI(PF0), SPI1DO(PF2)
#define SPI1MAP_G6     (6 << 12)        //G6 SPI1CLK(PE3), SPI1DI(PE4), SPI1DO(PE2)


/*****************************************************************************
 * Module    : uart0 Mapping选择列表
 *****************************************************************************/
#define UTX0MAP_PA7     (1 << 8)        //G1 uart0 tx: PA7
#define UTX0MAP_PB2     (2 << 8)        //G2 uart0 tx: PB2
#define UTX0MAP_PB3     (3 << 8)        //G3 uart0 tx: PB3  //USBDP
#define UTX0MAP_PE7     (4 << 8)        //G4 uart0 tx: PE7
#define UTX0MAP_PA1     (5 << 8)        //G5 uart0 tx: PA1
#define UTX0MAP_PE0     (6 << 8)        //G6 uart0 tx: PE0
#define UTX0MAP_PF2     (7 << 8)        //G7 uart0 tx: PF2

#define URX0MAP_PA6     (1 << 12)       //G1 uart0 rx: PA6
#define URX0MAP_PB1     (2 << 12)       //G2 uart0 rx: PB1
#define URX0MAP_PB4     (3 << 12)       //G3 uart0 rx: PB4
#define URX0MAP_PE6     (4 << 12)       //G4 uart0 rx: PE6
#define URX0MAP_PA0     (5 << 12)       //G5 uart0 rx: PA0
#define URX0MAP_PE1     (6 << 12)       //G6 uart0 rx: PE1
#define URX0MAP_TX      (7 << 12)       //G7 uart0 map to TX pin by UT0TXMAP select(1线模式)


/*****************************************************************************
 * Module    : 录音文件类型列表
 *****************************************************************************/
#define REC_NO          0
#define REC_WAV         1              //PCM WAV
#define REC_ADPCM       2              //ADPCM WAV
#define REC_MP3         3
#define REC_SBC         4

/*****************************************************************************
* Module    : I2S设备列表
*****************************************************************************/
#define I2S_DEV_NO      0
#define I2S_DEV_TAS5711 1
#define I2S_DEV_WM8978  2

#define I2S_GPIOA       0
#define I2S_GPIOE       1

/*****************************************************************************
* Module    : SPDIF设备列表
*****************************************************************************/
#define SPF_PA0_CH0         0
#define SPF_PA1_CH1         1
#define SPF_PB0_CH2         2
#define SPF_PE5_CH3         3
#define SPF_PE6_CH4         4

/*****************************************************************************
* Module    : DAC SELECT
*****************************************************************************/
#define DAC_MONO        0              //DAC单声道输出
#define DAC_DUAL        1              //DAC双声道输出
#define DAC_DIFF_MONO   2              //DAC差分单声道输出
#define DAC_DIFF_DUAL   3              //DAC差分双声道输出
#define DAC_VCMBUF_MONO 4              //DAC VCMBUF单声道输出
#define DAC_VCMBUF_DUAL 5              //DAC VCMBUF双声道输出

/*****************************************************************************
* Module    : DAC OUT Sample Rate
*****************************************************************************/
#define DAC_OUT_44K1    0               //dac out sample rate 44.1K
#define DAC_OUT_48K     1               //dac out sample rate 48K

/*****************************************************************************
* Module    : DAC LDOH Select
*****************************************************************************/
#define AU_LDOH_2V4     0               //VDDAUD LDO voltage 2.4V
#define AU_LDOH_2V5     1               //VDDAUD LDO voltage 2.5V
#define AU_LDOH_2V7     2               //VDDAUD LDO voltage 2.7V
#define AU_LDOH_2V9     3               //VDDAUD LDO voltage 2.9V
#define AU_LDOH_3V1     4               //VDDAUD LDO voltage 3.1V
#define AU_LDOH_3V2     5               //VDDAUD LDO voltage 3.2V

/*****************************************************************************
* Module    : 外接收音时钟选择
*****************************************************************************/
#define FM_USE_CRYSTAL_32K          0       //32.768k独立晶振
#define FM_USE_CRYSTAL_12M          1       //12M独立晶振
#define FM_USE_IO_32KHZ_CLK         2       //IO输出32K时钟，主控需要挂32K晶振
#define FM_USE_IO_12MHZ_CLK         3       //IO输出12M时钟
#define FM_SHARE_CRYSTAL_32KHz      4       //共用主按32K晶振，主控需要挂32K晶振

/*****************************************************************************
* Module    : AUX or MIC Left&Right channel list
* AUX: 可以任意左与右搭配，或只选择左，或只选择右
*****************************************************************************/
#define CH_AUXL_PA6         0x01    //AUXL0(PA6) -> left aux
#define CH_AUXL_PB1         0x02    //AUXL1(PB1) -> left aux
#define CH_AUXL_PE6         0x03    //AUXL2(PE6) -> left aux
#define CH_AUXL_PF0         0x04    //AUXL3(PF0) -> left aux
#define CH_AUXL_VOUTLN      0x05    //VOUTLN     -> left aux
#define CH_AUXL_MICL        0x06    //MICL       -> left aux
#define CH_AUXL_MICR        0x07    //MICR       -> left aux
#define CH_AUXL_VOUTRP      0x08    //VOUTRP     -> left aux
#define CH_MICL0            0x0c    //MICL       -> left mic
#define CH_MICL1            0x0d    //MICR       -> left mic

#define CH_AUXR_PA7         0x10    //AUXR0(PA7) -> right aux
#define CH_AUXR_PB2         0x20    //AUXR1(PB2) -> right aux
#define CH_AUXR_PE7         0x30    //AUXR2(PE7) -> right aux
#define CH_AUXR_PF1         0x40    //AUXR3(PF1) -> right aux
#define CH_AUXR_VOUTLN      0x50    //VOUTLN     -> right aux
#define CH_AUXR_MICL        0x60    //MICL       -> right aux
#define CH_AUXR_MICR        0x70    //MICR       -> right aux
#define CH_AUXR_VOUTRN      0x80    //VOUTRN     -> right aux
#define CH_MICR0            0xc0    //MICR       -> right mic
#define CH_MICR1            0xd0    //MICL       -> right mic


/*****************************************************************************
* Module    : AUX Analog gain list
*****************************************************************************/
#define AUX_N6DB            0       //AUX Analog -6DB
#define AUX_N3DB            1       //AUX Analog -3DB
#define AUX_N0DB            2       //AUX Analog 0DB
#define AUX_P3DB            3       //AUX Analog +3DB
#define AUX_P6DB            4       //AUX Analog +6DB


/*****************************************************************************
* Module    : 电池低电电压列表
*****************************************************************************/
#define VBAT_2V8            0       //2.8v
#define VBAT_2V9            1       //2.9v
#define VBAT_3V0            2       //3.0v
#define VBAT_3V1            3       //3.1v
#define VBAT_3V2            4       //3.2v
#define VBAT_3V3            5       //3.3v
#define VBAT_3V4            6       //3.4v
#define VBAT_3V5            7       //3.5v
#define VBAT_3V6            8       //3.6v
#define VBAT_3V7            9       //3.7v
#define VBAT_3V8            10      //3.8v

/*****************************************************************************
* Module    : uart0 printf IO列表
*****************************************************************************/
#define PRINTF_NONE         0           //关闭UART0打印信息
#define PRINTF_PA7          1           //与SD0MAP_G1同一IO
#define PRINTF_PB2          2           //与SD0MAP_G2同一IO
#define PRINTF_PB3          3           //与USBDP, SD0MAP_G5, SD0MAP_G6同一IO
#define PRINTF_PE7          4
#define PRINTF_PA1          5
#define PRINTF_PE0          6
#define PRINTF_PF2          7

/*****************************************************************************
* Module    : GPIO list
*****************************************************************************/
#define IO_NONE             0
#define IO_PA0              1
#define IO_PA1              2
#define IO_PA2              3
#define IO_PA3              4
#define IO_PA4              5
#define IO_PA5              6
#define IO_PA6              7
#define IO_PA7              8
#define IO_PB0              9
#define IO_PB1              10
#define IO_PB2              11
#define IO_PB3              12
#define IO_PB4              13
#define IO_PE0              14
#define IO_PE1              15
#define IO_PE2              16
#define IO_PE3              17
#define IO_PE4              18
#define IO_PE5              19
#define IO_PE6              20
#define IO_PE7              21
#define IO_PF0              22
#define IO_PF1              23
#define IO_PF2              24
#define IO_PF3              25
#define IO_PF4              26
#define IO_PF5              27

#define IO_MUX_SDCLK        28
#define IO_MUX_SDCMD        29
#define IO_MUX_PWRKEY       30
#define IO_MUX_MICL         31
#define IO_MUX_ADKEY        32
#define IO_MUX_SDDAT        33


/*****************************************************************************
* Module    : 充电仓充电操作
*****************************************************************************/
#define CHARGE_FINISH_2_LOWPOWER    1
#define CHARGE_OUT_2_POWERON        2

/*****************************************************************************
* Module    : EX_SPIFLASH_SUPPORT
*****************************************************************************/
#define EXSPI_NOT_SUPPORT   0
#define EXSPI_REC           (1<<0)    //外接SPIFLASH支持录音
#define EXSPI_MUSIC         (1<<1)    //外接SPIFLASH支持MP3音乐镜像文件播放

#endif //CONFIG_DEFINE_H


