/************************************************Copyright(c)***********************************
**                                   Quintic(Nanjing) Microelectronics Co,Ltd.
**
**                                   http://www.quinticcorp.com
**
**--------------------File Info----------------------------------------------------------------
** File Name:                  qndriver.h
** subversion number:   1.2
**----------------------------------------------------------------------------------------
************************************************************************************************/

#ifndef _QNDRIVER_H
#define _QNDRIVER_H

#include <include.h>


//假台多可以打开
#define PILOT_CCA                       1  // 0: disable stereo judgement for the country has many mono FM stations
#define USING_VALID_CH                  0
#define CCA_PILOT_SNR_FILTER            35


/**********************************QN8035's clock source selection**************
1.QN8035's default clock source is 32768HZ.
2.setting QN8035's clock source and clock source type(like sine-wave clock or digital clock).
3.user need to modify clock source according to actual hardware platform.
4.clock formula,the details please refer to the QN8035's datasheet
  XTAL_DIV = Round(Clock/32768);
  PLL_DLT = Round((28500000*512*XTAL_DIV)/Clock)-442368
*******************************************************************************/
#define QND_SINE_WAVE_CLOCK             0x00    //inject sine-wave clock
#define QND_DIGITAL_CLOCK               0x80    //inject digital clock,default is inject digital clock

#if (FMRX_CLOCK == FM_USE_CRYSTAL_12M)
//crystal clock is 12MHZ
#define QND_XTAL_DIV0                   0x6E
#define QND_XTAL_DIV1                   0x01
#define QND_XTAL_DIV2                   0x54
#else
//crystal clock is 32768HZ
#define QND_XTAL_DIV0                   0x01
#define QND_XTAL_DIV1                   0x08
#define QND_XTAL_DIV2                   0x5C
#endif

//crystal clock is 24MHZ
//#define QND_XTAL_DIV0                   0xDC
//#define QND_XTAL_DIV1                   0x02
//#define QND_XTAL_DIV2                   0x54   //3


//following is definition step
#define QND_STEP_CONSTANT               0x40 //(QND_FSTEP_100KHZ << 6)
#define CHIPID_QN8035                   0x84


/*******************************************************************************
Performance configuration
*******************************************************************************/
#define SMSTART_VAL                     15//10
#define HCCSTART_VAL                    20//13
#define SNCSTART_VAL                    51

/*******************************************************************************
limitation configuration
*******************************************************************************/
#define QND_READ_RSSI_DELAY             100
#define SLEEP_TO_WAKEUP_DELAY_TIME      500
#define CH_SETUP_DELAY_TIME             300
/***************************************End************************************/

/*******************************************************************************
 definition register
*******************************************************************************/
#define SYSTEM1         				0x00
#define CCA             				0x01
#define SNR            					0x02
#define RSSISIG         				0x03
#define STATUS1         				0x04
#define CID1            				0x05
#define CID2            				0x06
#define	CH								0x07
#define	CH_START						0x08
#define	CH_STOP							0x09
#define	CH_STEP							0x0A
#define	RDSD0							0x0B
#define	RDSD1							0x0C
#define	RDSD2							0x0D
#define	RDSD3							0x0E
#define	RDSD4							0x0F
#define	RDSD5							0x10
#define	RDSD6							0x11
#define	RDSD7							0x12
#define	STATUS2							0x13
#define	VOL_CTL							0x14
#define	XTAL_DIV0						0x15
#define	XTAL_DIV1						0x16
#define	XTAL_DIV2						0x17
#define INT_CTRL						0x18
#define SMP_HLD_THRD					0x19
#define	RXAGC_GAIN						0x1A
#define GAIN_SEL						0x1B
#define	SYSTEM_CTL1						0x1C
#define	SYSTEM_CTL2						0x1D
#define RDSCOSTAS						0x1E
#define REG_TEST						0x1F
#define STATUS4							0x20
#define	CCA1							0x27
#define PLT1                            0X2F
#define	PLT2                            0x30
#define	SMSTART							0x34
#define	SNCSTART						0x35
#define	HCCSTART						0x36
#define	CCA_CNT1					    0x37
#define	CCA_CNT2					    0x38
#define	CCA_SNR_TH_1					0x39
#define	CCA_SNR_TH_2					0x3A
#define NCCFIR3         				0x40
#define REG_REF                         0x49
#define REG_DAC							0x4C
/***************************************End************************************/

/*******************************************************************************
 definition operation bit of register
*******************************************************************************/
#define CCA_CH_DIS      				0x01
#define CHSC            				0x02
#define RDSEN           				0x08
#define CH_CH		    				0x03
#define CH_CH_START     				0x0c
#define CH_CH_STOP      				0x30
#define STNBY_MODE           		    0x20
#define RX_MODE         				0x10
#define IMR             				0x40
#define RDS_RXUPD       				0x80
#define ST_MO_RX                        0x01
#define STNBY_RX_MASK                   0x30
#define RXCCA_MASK                      0x03
#define RX_CCA                          0x02
#define RXCCA_FAIL                      0x08
#define RX_MONO                         0x04
#define ICPREF                          0x0F
/***************************************End************************************/

#define QNM_GetMonoMode()               QND_ReadReg(STATUS1) & ST_MO_RX
#define QNM_GetRssi()                   QND_ReadReg(RSSISIG)

bool qn8035_is_online(void);
void qn8035_init(void);
void qn8035_off(void);
void qnd8035_set_freq(u16 freq);
u8 qn8035_seek(u16 freq);
void qn8035_set_vol(u8 volume);
void qn8035_unmute(void);
void qn8035_mute(void);

#endif
