/************************************************Copyright(c)******************
 **                   Quintic(Nanjing) Microelectronics Co,Ltd.
 **
 **                        http://www.quinticcorp.com
 **
 **-----------------------------File Info---------------------------------------
 ** File Name:                  qndriver.h
 ** subversion number:   1.0
 **-----------------------------------------------------------------------------
 ******************************************************************************/

#ifndef _QNDRIVER_H
#define _QNDRIVER_H

#include "psp_includes.h"

#define QN_8035
#define _QNFUNC_H_

#define CCS_RX                          0
#define CCS_TX                          1

#define FREQ2CHREG(freq)                (((freq) - 6000) / 5)
#define CHREG2FREQ(ch)                  ((ch) * 5 + 6000)
#define _QNCOMMON_H_

#define QND_REG_NUM_MAX                 85
/**********************************QN8035's clock source selection**************
 1.QN8035's default clock source is 32768HZ.
 2.setting QN8035's clock source and clock source type(like sine-wave clock or digital clock).
 3.user need to modify clock source according to actual hardware platform.
 4.clock formula,the details please refer to the QN8035's datasheet
 XTAL_DIV = Round(Clock/32768);
 PLL_DLT = Round((28500000*512*XTAL_DIV)/Clock)-442368
 *******************************************************************************/
#define QND_INPUT_CLOCK                 0x00    //using external clock or oscillator as qn8035's clock 
#define QND_CRYSTAL_CLOCK               0x80    //using crystal as qn8035's clock
#define QND_SINE_WAVE_CLOCK             0x00    //inject sine-wave clock  
#define QND_DIGITAL_CLOCK               0x80    //inject digital clock,default is inject digital clock



/***************************************End************************************/

#define QND_MODE_SLEEP                  0
#define QND_MODE_WAKEUP                 1

// RX / TX value is using upper 8 bit
#define QND_MODE_RX                     0x8000
#define QND_MODE_TX                     0x4000
// AM / FM value is using lower 8 bit
// need to check datasheet to get right bit
#define QND_MODE_FM                     0x0000

#define BAND_FM                         0

// tune
#define QND_FSTEP_50KHZ                 0
#define QND_FSTEP_100KHZ                1
#define QND_FSTEP_200KHZ                2

// audio output format
#define QND_OUTPUT_ANALOG               0
#define QND_OUTPUT_I2S                  1

// stereo mode
#define QND_TX_AUDIO_MONO               0x10
#define QND_TX_AUDIO_STEREO             0x00

#define QND_RX_AUDIO_MONO               0x20
#define QND_RX_AUDIO_STEREO             0x00

#define QND_CONFIG_MONO                 0x01
#define QND_CONFIG_MUTE                 0x02
#define QND_CONFIG_SOFTCLIP             0x03
#define QND_CONFIG_AUTOAGC              0x04
#define QND_CONFIG_AGCGAIN              0x05
#define QND_CONFIG_EQUALIZER            0x06
#define QND_CONFIG_VOLUME               0x07
#define QND_CONFIG_BASS_QUALITY         0x08
#define QND_CONFIG_BASS_FREQ            0x09
#define QND_CONFIG_BASS_GAIN            0x0a
#define QND_CONFIG_MID_QUALITY          0x0b
#define QND_CONFIG_MID_FREQ             0x0c
#define QND_CONFIG_MID_GAIN             0x0d
#define QND_CONFIG_TREBLE_FREQ          0x0e
#define QND_CONFIG_TREBLE_GAIN          0x0f

#define QND_ENABLE_EQUALIZER            0x10
#define QND_DISABLE_EQUALIZER           0x00

#define QND_CONFIG_AUDIOPEAK_DEV        0x11
#define QND_CONFIG_PILOT_DEV            0x12
#define QND_CONFIG_RDS_DEV              0x13
#define QND_CONFIG_I2S                  0x14

// input format
#define QND_INPUT_ANALOG                0
#define QND_INPUT_I2S                   1

// i2s mode
#define QND_RX_ANALOG_OUTPUT            0x00
#define QND_RX_I2S_OUTPUT               0x08
#define QND_TX_ANALOG_INPUT             0x00
#define QND_TX_I2S_INPTUT               0x04

//i2s clock data rate
#define QND_I2S_DATA_RATE_32K           0x00
#define QND_I2S_DATA_RATE_40K           0x10
#define QND_I2S_DATA_RATE_44K           0x20
#define QND_I2S_DATA_RATE_48K           0x30

//i2s clock Bit Wise
#define QND_I2S_BIT_8                   0x00
#define QND_I2S_BIT_16                  0x40
#define QND_I2S_BIT_24                  0x80
#define QND_I2S_BIT_32                  0xc0

//i2s Control mode
#define QND_I2S_MASTER                  0x08
#define QND_I2S_SLAVE                   0x00

//i2s Control mode
#define QND_I2S_MSB                     0x00
#define QND_I2S_I2S                     0x01
#define QND_I2S_DSP1                    0x02
#define QND_I2S_DSP2                    0x03
#define QND_I2S_LSB                     0x04

#define QND_EQUALIZE_BASS               0x00
#define QND_EQUALIZE_MID                0x01
#define QND_EQUALIZE_TREBLE             0x02

// RDS, TMC
#define QND_EUROPE_FLEXIBILITY_DISABLE  0
#define QND_EUROPE_FLEXIBILITY_ENABLE   1
#define QND_RDS_OFF                     0
#define QND_RDS_ON                      1
#define QND_RDS_BUFFER_NOT_READY        0
#define QND_RDS_BUFFER_READY            1

/****************************Chips ID definition*******************************/
#define CHIPID_QN8000                   0x00
#define CHIPID_QN8005                   0x20
#define CHIPID_QN8005B1                 0x21
#define CHIPID_QN8006                   0x30
#define CHIPID_QN8006LB                 0x71
#define CHIPID_QN8007B1                 0x11
#define CHIPID_QN8007                   0x10
#define CHIPID_QN8006A1                 0x30
#define CHIPID_QN8006B1                 0x31
#define CHIPID_QN8016                   0xe0
#define CHIPID_QN8016_1                 0xb0
#define CHIPID_QN8015                   0xa0
#define CHIPID_QN8065                   0xa0
#define CHIPID_QN8067                   0xd0
#define CHIPID_QN8065N                  0xa0
#define CHIPID_QN8027                   0x40
#define CHIPID_QN8025                   0x80
#define CHIPID_QN8035                   0x84
#define CHIPSUBID_QN8035A0              0x01
#define CHIPSUBID_QN8035A1              0x02
#define CHIPID_QN8026                   0x3C
#define CHIPID_QN8036                   0x34
#define CHIPID_QN8301                   0x44
/***************************************End************************************/

#define _QNCONFIG_H_
/******************************country selection*******************************/
#define COUNTRY_CHINA                   0
#define COUNTRY_USA                     1
#define COUNTRY_JAPAN                   2
/***************************************End************************************/

/*******************************************************************************
 Performance configuration
 *******************************************************************************/
#define SMSTART_VAL                     12
#define HCCSTART_VAL                    18
#define SNCSTART_VAL                    51

enum
{
    // Bit[15-8] of the word: RSSI Threshold
    // Bit[7-0] of the word: SNR Threshold
    // e.g. 0x1E06 => RSSI_TH = 0x1E, SNR_TH = 0x06
    // notice: the rang of RSSI is 0x0A ~ 0x3F
    // notice: the rang of SNR is 0x00 ~ 0x3F

    CCA_SENSITIVITY_LEVEL_0 = 0x1406,
    CCA_SENSITIVITY_LEVEL_1 = 0x1407, //if using the pilot as CCA,reference this item.
    CCA_SENSITIVITY_LEVEL_2 = 0x1408,
    CCA_SENSITIVITY_LEVEL_3 = 0x1409,
    CCA_SENSITIVITY_LEVEL_4 = 0x140a, //if not using the pilot as CCA,reference this item.
    CCA_SENSITIVITY_LEVEL_5 = 0x140b,
    CCA_SENSITIVITY_LEVEL_6 = 0x140c,
    CCA_SENSITIVITY_LEVEL_7 = 0x140d,
    CCA_SENSITIVITY_LEVEL_8 = 0x140e,
    CCA_SENSITIVITY_LEVEL_9 = 0x140f
};
/***************************************End************************************/

/*******************************************************************************
 limitation configuration
 *******************************************************************************/
#define QND_READ_RSSI_DELAY             10
#define ENABLE_2K_SPEED_PLL_DELAY       20
#define SLEEP_TO_WAKEUP_DELAY_TIME      500
#define CH_SETUP_DELAY_TIME             200
/***************************************End************************************/

#define assert(str)
#define QND_LOG(a)
#define QND_LOGA(a,b)
#define QND_LOGB(a,b)
#define QND_LOGHEX(a,b)
#define _QNREG_H_
/*******************************************************************************
 definition register
 *******************************************************************************/
#define SYSTEM1                         0x00
#define CCA                             0x01
#define SNR                             0x02
#define RSSISIG                         0x03
#define STATUS1                         0x04
#define CID1                            0x05
#define CID2                            0x06
#define	CH                              0x07
#define	CH_START                        0x08
#define	CH_STOP                         0x09
#define	CH_STEP                         0x0a
#define	RDSD0                           0x0b
#define	RDSD1                           0x0c
#define	RDSD2                           0x0d
#define	RDSD3                           0x0e
#define	RDSD4                           0x0f
#define	RDSD5                           0x10
#define	RDSD6                           0x11
#define	RDSD7                           0x12
#define	STATUS2                         0x13
#define	VOL_CTL                         0x14
#define	XTAL_DIV0                       0x15
#define	XTAL_DIV1                       0x16
#define	XTAL_DIV2                       0x17
#define INT_CTRL                        0x18
#define SMP_HLD_THRD                    0x19
#define	RXAGC_GAIN                      0x1a
#define GAIN_SEL                        0x1b
#define	SYSTEM_CTL1                     0x1c
#define	SYSTEM_CTL2                     0x1d
#define RDSCOSTAS                       0x1e
#define REG_TEST                        0x1f
#define STATUS4                         0x20
#define RDSAGC2                         0x21
#define	CCA1                            0x27
#define	CCA2                            0x28
#define	CCA3                            0x29
#define	CCA4                            0x2a
#define	CCA5                            0x2b
#define PLT1                            0x2f
#define	PLT2                            0x30
#define	SMSTART                         0x34
#define	SNCSTART                        0x35
#define	HCCSTART                        0x36
#define	CCA_CNT1                        0x37
#define	CCA_CNT2                        0x38
#define	CCA_SNR_TH_1                    0x39
#define	CCA_SNR_TH_2                    0x3a
#define NCCFIR3                         0x40
#define NCO_COMP_VAL                    0x46
#define REG_REF                         0x49
#define REG_DAC                         0x4c
/***************************************End************************************/

/*******************************************************************************
 definition operation bit of register
 *******************************************************************************/
#define CCA_CH_DIS                      0x01
#define CHSC                            0x02
#define RDSEN                           0x08
#define CH_CH                           0x03
#define CH_CH_START                     0x0c
#define CH_CH_STOP                      0x30
#define STNBY_MODE                      0x20
#define RX_MODE                         0x10
#define IMR                             0x40
#define RDS_RXUPD                       0x80
#define ST_MO_RX                        0x01
#define STNBY_RX_MASK                   0x30
#define RXCCA_MASK                      0x03
#define RX_CCA                          0x02
#define RXCCA_FAIL                      0x08
#define RX_MONO                         0x04
#define ICPREF                          0x0f
/***************************************End************************************/

#define _QNSYS_H_

#define CHANNEL_FILTER

// Triple frequency converter for high frequency (triple) channel receiver
// usage: convert high frequency to normal frequency without FM chip working range
// example:
//        QND_TuneToCH(TRIPLE_FREQ_CONVERTER(high_freq));
//        TX_QND_TuneToCH(TX_TRIPLE_FREQ_CONVERTER(high_freq));
//notice:the variable high_freq unit is 10KHz.
#define TRIPLE_FREQ_CONVERTER(trifreq)  ((((((trifreq) * 10 + 225) / 3 + 225)/10+2)/5)*5)
#define TX_TRIPLE_FREQ_CONVERTER(trifreq)  ((((trifreq)/3 + 2)/5)*5)
// external driver interface
// logical layer
/*****************************************************************************
 Driver API Macro Definition
 *****************************************************************************/
#define QNM_GetMonoMode() \
    QND_ReadReg(STATUS1) & ST_MO_RX
#define QNM_GetRssi() \
    QND_ReadReg(RSSISIG)
#define QNM_GetChipNumber()\
    QND_ReadReg(CID2) & 0xfc

#define QND_READ(adr)           QND_ReadReg(adr)
#define QND_WRITE(adr, value)   QND_WriteReg(adr, value)

extern void QNF_SetRegBit(UINT8 reg, UINT8 bitMask, UINT8 data_val) __FAR__;

extern UINT8 qnd_div1;
extern UINT8 qnd_div2;
extern UINT8 qnd_nco;
extern uint8 g_freq_855;

extern UINT16 QNF_GetCh(void) __FAR__;
#define QND_Delay(ms)       sys_mdelay(ms)
extern UINT8 QND_Init(void) __FAR__;
extern void QND_TuneToCH(UINT16 ch) __FAR__;

#define QN_RX
#define _QNRX_H_

extern void QND_RXSetTH(UINT8 th) __FAR__;
extern INT8 QND_RXValidCH(UINT16 freq, UINT8 step) __FAR__;
INT16 QND_RXSeekCH(UINT16 start, UINT16 stop, UINT8 step) __FAR__;
void QNF_SetMute(UINT8 On) __FAR__;

#endif
