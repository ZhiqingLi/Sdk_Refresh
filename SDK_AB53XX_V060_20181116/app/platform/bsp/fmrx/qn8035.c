#include "include.h"
#include "qn8035.h"

#if FMRX_QN8035_EN

u8  qnd_PreNoiseFloor = 40;
u32 osc_setup_ticks;

AT(.text.qn8035)
static u8 QND_ReadReg(u8 adr)
{
   	u8 rdat;

    bsp_i2c_start();
    bsp_i2c_tx_byte(0x20);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(adr);
    bsp_i2c_rx_ack();
    bsp_i2c_stop();
    delay_us(60);

    bsp_i2c_start();
    bsp_i2c_tx_byte(0x21);
    bsp_i2c_rx_ack();
    rdat = bsp_i2c_rx_byte();
    bsp_i2c_tx_nack();
    bsp_i2c_stop();
    delay_us(60);

    return rdat;
}

AT(.text.qn8035)
static void QND_WriteReg(u8 adr, u8 value)
{
    bsp_i2c_start();
    bsp_i2c_tx_byte(0x20);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(adr);
    bsp_i2c_rx_ack();
    bsp_i2c_tx_byte(value);
    bsp_i2c_rx_ack();
    bsp_i2c_stop();
    delay_us(60);
}

AT(.text.qn8035)
static u16 FREQ2CHREG(u16 freq)
{
    return 	(freq - 6000) / 5;
}

AT(.text.qn8035)
static void QNF_SetMute(u8 On)
{
    QND_WriteReg(REG_DAC, On?0x1B:0x10);
}

/**********************************************************************
u8 QNF_SetCh(u16 freq)
**********************************************************************
Description: set channel frequency

Parameters:
    freq:  channel frequency to be set
Return Value:
    1: success
**********************************************************************/
#if USING_VALID_CH
AT(.text.qn8035)
static void QNF_SetCh(u16 freq)
{
    u8 temp;

	freq = FREQ2CHREG(freq);
	//writing lower 8 bits of CCA channel start index
	QND_WriteReg(CH_START, (u8)freq);
	//writing lower 8 bits of CCA channel stop index
	QND_WriteReg(CH_STOP, (u8)freq);
	//writing lower 8 bits of channel index
	QND_WriteReg(CH, (u8)freq);
	//writing higher bits of CCA channel start,stop and step index
	temp = (u8) ((freq >> 8) & CH_CH);
	temp |= ((u8)(freq >> 6) & CH_CH_START);
	temp |= ((u8) (freq >> 4) & CH_CH_STOP);
	temp |= QND_STEP_CONSTANT;
	QND_WriteReg(CH_STEP, temp);
}
#else
AT(.text.qn8035)
static void QNF_SetCh(u16 start, u16 stop, u8 step)
{
    u8 temp;

	start = FREQ2CHREG(start);
	stop = FREQ2CHREG(stop);
	//writing lower 8 bits of CCA channel start index
	QND_WriteReg(CH_START, (u8)start);
	//writing lower 8 bits of CCA channel stop index
	QND_WriteReg(CH_STOP, (u8)stop);
	//writing lower 8 bits of channel index
	QND_WriteReg(CH, (u8)start);
	//writing higher bits of CCA channel start,stop and step index
	temp = (u8) ((start >> 8) & CH_CH);
	temp |= ((u8)(start >> 6) & CH_CH_START);
	temp |= ((u8) (stop >> 4) & CH_CH_STOP);
	temp |= (step << 6);
	QND_WriteReg(CH_STEP, temp);
}
#endif // USING_VALID_CH

/**********************************************************************
void QND_Init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.

Parameters:
    None
Return Value:
    1: Device is ready to use.
    0: Device is not ready to serve function.
**********************************************************************/
AT(.text.qn8035)
void qn8035_xosc_setup(void)
{
#if (FMRX_CLOCK == FM_USE_CRYSTAL)
    QND_WriteReg(0x00, 0x81);
    delay_5ms(5);

    QNF_SetMute(1);
    QND_WriteReg(0x58, 0x93);           //mod PLL setting

    osc_setup_ticks = tick_get();       //qn8075,使用独立晶振时，等待起振时间需要2S
#endif
}

AT(.text.qn8035)
void qn8035_init(void)
{
#if ((FMRX_CLOCK == FM_USE_CRYSTAL_32K) || (FMRX_CLOCK == FM_USE_CRYSTAL_12M))
    ///qn8075,使用独立晶振时，等待起振时间需要2S
    while (!tick_check_expire(osc_setup_ticks, 2000)) {
        delay_5ms(2);
        WDT_CLR();
    }
#endif

    QND_WriteReg(0x00, 0x81);
    delay_ms(2);

#if (FMRX_CLOCK == FM_USE_IO_32KHZ_CLK)
    QND_WriteReg(0x01, QND_DIGITAL_CLOCK);   //方波
#elif (FMRX_CLOCK == FM_SHARE_CRYSTAL_32KHz)
    QND_WriteReg(0x01, QND_SINE_WAVE_CLOCK); //正弦波
#elif ((FMRX_CLOCK == FM_USE_CRYSTAL_32K) || (FMRX_CLOCK == FM_USE_CRYSTAL_12M))
    QNF_SetMute(1);
    QND_WriteReg(0x58, 0x93);//mod PLL setting
    delay_5ms(25);
    QND_WriteReg(XTAL_DIV0, QND_XTAL_DIV0);//01
    QND_WriteReg(XTAL_DIV1, QND_XTAL_DIV1);//08
    QND_WriteReg(XTAL_DIV2, QND_XTAL_DIV2);//5c
#endif

    QND_WriteReg(0x54, 0x47);//mod PLL setting
    //select SNR as filter3,SM step is 2db
    QND_WriteReg(0x19, 0xc4);
    QND_WriteReg(0x40,0x70);	//set SNR as SM,SNC,HCC MPX   /////没有
    QND_WriteReg(0x33, 0x9c);//9e;//set HCC and SM Hystersis 5db
    QND_WriteReg(0x2d, 0xD6);//notch filter threshold adjusting
    QND_WriteReg(0x43, 0x10);//notch filter threshold enable
    QND_WriteReg(0x47,0x39);
    //enter receiver mode directly
    QND_WriteReg(0x00, 0x11);
    //Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
    QND_WriteReg(0x1D,0xA9);
    QND_WriteReg(0x4f, 0x40);//dsiable auto tuning
    QND_WriteReg(0x34,SMSTART_VAL); ///set SMSTART
    QND_WriteReg(0x35,SNCSTART_VAL); ///set SNCSTART
    QND_WriteReg(0x36,HCCSTART_VAL); ///set HCCSTART
    QNF_SetMute(1);
}

/************************************************************************
void QND_RXConfigAudio(u8 optiontype, u8 option )
*************************************************************************
Description: config audio
Parameters:
  optiontype: option
    QND_CONFIG_MONO; ‘option’control mono, 0: stereo receive mode ,1: mono receiver mode
    QND_CONFIG_MUTE; ‘option’control mute, 0:mute disable,1:mute enable
    QND_CONFIG_VOLUME: 'option' control the volume gain,range : 0~47(-47db~0db)

Return Value:
    none
**********************************************************************/
AT(.text.qn8035)
static void QND_RXConfigAudio(u8 option)
{
    u8 regVal;

    regVal = QND_ReadReg(VOL_CTL);
	regVal = (regVal & 0xC0) | (option / 6) | ((5-option%6)<<3);
	QND_WriteReg(VOL_CTL, regVal);
}

/*----------------------------------------------------------------------------*/
/**@brief FM模块QN8035设置音量
   @param volume 音量值
   @return 无
   @note 音量范围0~15
*/
/*----------------------------------------------------------------------------*/
AT(.text.qn8035)
void qn8035_set_vol(u8 volume)
{
    volume = volume*3;
    if(volume > 47) {
        volume = 47;    //qn8035 volume range is from 0~47
    }
    QND_RXConfigAudio(volume);

    if(volume == 0) {
        QNF_SetMute(1);
    } else {
        QNF_SetMute(0);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief FM模块QN8035关闭
   @param 无
   @return 无
   @note
*/
/*----------------------------------------------------------------------------*/
AT(.text.qn8035)
void qn8035_off(void)
{
    QND_WriteReg(0x00, 0x01);
    delay_5ms(4);

    QND_WriteReg(0x00, 0x01);
    delay_5ms(2);

    QND_WriteReg(0x00, 0x01);
    delay_5ms(2);
}


/**********************************************************************
void qnd8035_set_freq(u16 freq)
**********************************************************************
Description: Tune to the specific channel. call QND_SetSysMode() before
call this function
Parameters:
ch
Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
None
**********************************************************************/
AT(.text.qn8035)
void qnd8035_set_freq(u16 freq)
{
    u8 reg;

    //increase reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x7A);

    /********** QNF_RXInit ****************/
    QND_WriteReg(0x1B,0x70);  //Let NFILT adjust freely
    //QNF_SetRegBit(0x2C,0x3F,0x12);  ///When SNR<ccth31, ccfilt3 will work
    //setting the threshold of Filter3 will be worked.
    QND_WriteReg(0x2C,0x52);
    //QNF_SetRegBit(0x1D,0x40,0x00);  ///Let ccfilter3 adjust freely
    //QNF_SetRegBit(0x41,0x0F,0x0A);  ///Set a hcc index to trig ccfilter3's adjust
    QND_WriteReg(0x45,0x50);        ///Set aud_thrd will affect ccfilter3's tap number
    //QNF_SetRegBit(0x40,0x70,0x70);  ///snc/hcc/sm snr_rssi_sel; snc_start=0x40; hcc_start=0x30; sm_start=0x20
    QND_WriteReg(0x40,0x70);
    //QNF_SetRegBit(0x19,0x80,0x80);  ///Use SNR for ccfilter selection criterion
    //selecting SNR as filter3 filter condition
    //QND_WriteReg(0x19, 0xC2);
    //QNF_SetRegBit(0x3E,0x80,0x80);  ///it is decided by programming this register
    //QNF_SetRegBit(0x41,0xE0,0xC0);  ///DC notching High pass filter bandwidth; remove low freqency dc signals

#if 1//INVERSE_IMR
    reg = QND_ReadReg(CCA)&~0x40;
    if((freq == 9340) || (freq == 9390) || (freq == 9530)/*||(freq == 9980)||(freq == 10480)*/) {
        reg |= 0x40;	// inverse IMR.
    } else {
        reg &= ~0x40;
    }
    QND_WriteReg(CCA, reg);
#endif
    QND_WriteReg(0x41,0xCA);
    QND_WriteReg(0x34,SMSTART_VAL);     ///set SMSTART
    QND_WriteReg(0x35,SNCSTART_VAL);    ///set SNCSTART
    QND_WriteReg(0x36,HCCSTART_VAL);    ///set HCCSTART
    /********** End of QNF_RXInit ****************/

    QNF_SetMute(1);
#if USING_VALID_CH
    QNF_SetCh(freq);
#else
    QNF_SetCh(freq, freq, 1);
#endif
    //enable CCA mode with user write into frequency
    QND_WriteReg(0x00, 0x13);
    //Auto tuning
    QND_WriteReg(0x4F, 0x80);
    reg = QND_ReadReg(0x4F);
    reg >>= 1;
    QND_WriteReg(0x4F, reg);
    ///avoid the "POP" noise.
    delay_5ms(30);  //150ms
    ///decrease reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x70);
    QNF_SetMute(0);
}

/***********************************************************************
Description: scan a noise floor from 87.5M to 108M by step 200K
Parameters:
Return Value:
 1: scan a noise floor successfully.
 0: chip would not normally work.
**********************************************************************/
AT(.text.qn8035)
static u8 QND_ScanNoiseFloor(u16 start,u16 stop)
{
    u8 regValue;
    u8 qnd_NoiseFloor;
    u8 timeOut = 100;

    QND_WriteReg(CCA_SNR_TH_1,0x00);
    QND_WriteReg(CCA_SNR_TH_2,0x05);
    QND_WriteReg(0x40,0xf0);
    //config CCS frequency rang by step 200KHZ
    QNF_SetCh(start,stop,2);
    /*
    QND_WriteReg(CH_START,0x26);
    QND_WriteReg(CH_STOP,0xc0);
    QND_WriteReg(CH_STEP,0xb8);
    */
    //enter CCA mode,channel index is decided by internal CCA
    QND_WriteReg(SYSTEM1,0x12);

    while(1) {
        regValue = QND_ReadReg(SYSTEM1);

        //if it seeks a potential channel, the loop will be quited
        if((regValue & CHSC) == 0) break;
        delay_5ms(4);
        WDT_CLR();
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0) {
            QND_WriteReg(0x40,0x70);
            return 0;
        }
    }

    QND_WriteReg(0x40,0x70);
    delay_ms(5);
    qnd_NoiseFloor = QND_ReadReg(0x3f);
    if(((qnd_PreNoiseFloor-qnd_NoiseFloor) >= 2) ||((qnd_NoiseFloor-qnd_PreNoiseFloor) >= 2)) {
        qnd_PreNoiseFloor = qnd_NoiseFloor;
    }
    return 1;
}
/***********************************************************************
void QND_RXSetTH(u8 th)
***********************************************************************
Description: Setting the threshold value of automatic scan channel
th:
  Setting threshold for quality of channel to be searched,
  the range of th value:CCA_SENSITIVITY_LEVEL_0 ~ CCA_SENSITIVITY_LEVEL_9
Return Value:
  None
***********************************************************************/
AT(.text.qn8035)
static void QND_RXSetTH(u8 th)
{
    u8 rssi_th,cca_snr_th_1,cca_snr_th_2 = 0x05;
    rssi_th = (qnd_PreNoiseFloor-26);//27
    ///increase reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x7A);
    //NFILT program is enabled
    QND_WriteReg(0x1B,0x78);
    //using Filter3
    QND_WriteReg(CCA1,0x75);
    //setting CCA IF counter error range value(256).
    QND_WriteReg(CCA_CNT2,0x05);//
#if PILOT_CCA
    QND_WriteReg(PLT1,0x00);
#endif
    cca_snr_th_1 = 0x40;//0x00
	//selection the time of CCA FSM wait SNR calculator to settle:20ms
	//0x00:	    20ms(default)
	//0x40:	    40ms
	//0x80:	    60ms
	//0xC0:	    100m
	//    QNF_SetRegBit(CCA_SNR_TH_1 , 0xC0, 0x00);
    //selection the time of CCA FSM wait RF front end and AGC to settle:20ms
    //0x00:     10ms
	//0x40:     20ms(default)
    //0x80:     40ms
	//0xC0:     60ms
	//    QNF_SetRegBit(CCA_SNR_TH_2, 0xC0, 0x40);
	//    QNF_SetRegBit(CCA, 30);
       //setting CCA RSSI threshold is 30
	//QND_WriteReg(CCA,QND_ReadReg(CCA)&0xC0|25);  // 20 ~ 30
    cca_snr_th_2 |= 0xc0;
    QND_WriteReg(CCA_SNR_TH_2,cca_snr_th_2);

    QND_WriteReg(CCA, (QND_ReadReg(CCA) & 0xc0) | rssi_th);
#if PILOT_CCA                             //6
    QND_WriteReg(CCA_SNR_TH_1,cca_snr_th_1|(7 +th)); //setting SNR threshold for CCA
#else
    QND_WriteReg(CCA_SNR_TH_1,cca_snr_th_1|(9+th)); //setting SNR threshold for CCA
#endif
}

/***********************************************************************
u16 QND_RXValidCH(u16 freq, u8 step);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  eg: 108.00MHz will be set to 10800.
Step:
  FM:
  QND_FMSTEP_100KHZ: set leap step to 100kHz
  QND_FMSTEP_200KHZ: set leap step to 200kHz
  QND_FMSTEP_50KHZ:  set leap step to 50kHz
Return Value:
  0: not a valid channel
  other: a valid channel at this frequency
***********************************************************************/
AT(.text.qn8035)
static u8 QND_RXValidCH(u16 freq)
{
 	u8 regValue;
  	u8 timeOut;
  	u8 isValidChannelFlag;
  	u8 readCnt,stereoCount=0;

    //printf("qn8035: %d\n", freq);
    if(freq == 8750) {
        QND_ScanNoiseFloor(8750,8800);
        QND_RXSetTH(0);
    } else if( ((freq - 8810) % 200) == 0) {
        QND_ScanNoiseFloor(freq,freq+190);
        QND_RXSetTH(0);
    }

#if USING_VALID_CH
  	QNF_SetCh(freq);
#else
    QNF_SetCh(freq,freq,1);
#endif

    //Auto tuning
    QND_WriteReg(0x00, 0x11);
    QND_WriteReg(0x4f, 0x80);
    delay_5ms(4);
    regValue = QND_ReadReg(0x4f);
    regValue = (regValue >> 1);
    QND_WriteReg(0x4f, regValue);

  	//QNF_SetRegBit(SYSTEM1,0x03,0x02);//enter channel scan mode,channel frequency is decided by internal CCA
  	//entering into RX mode and CCA mode,channels index decide by CCA.
    timeOut = 20;

    QND_WriteReg(0x00, 0x12);

    while(1)
    {
        regValue = QND_ReadReg(SYSTEM1);
        //if it seeks a potential channel, the loop will be quited
        if((regValue & CHSC) == 0) break;
        delay_5ms(4);
        WDT_CLR();
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0)
        {
            return 0;
        }
    }

    //reading out the rxcca_fail flag of RXCCA status
    //isValidChannelFlag = (QND_ReadReg(STATUS1) & RXCCA_FAIL ? 0:1) && timeOut;

    isValidChannelFlag = (QND_ReadReg(STATUS1) & RXCCA_FAIL ? 0:1);

    if(isValidChannelFlag)
    {
#if PILOT_CCA
    	delay_5ms(2);
    	u8 snr = QND_ReadReg(SNR);
        if(snr> 20) {
            return 1;
        }
    	for(readCnt=10;readCnt>0;readCnt--)
    	{
            delay_ms(5);
      		stereoCount += ((QND_ReadReg(STATUS1) & ST_MO_RX) ? 0:1);
      		if(stereoCount >= 3) return 1;
    	}
#else
    	return 1;
#endif
  	}
  	return 0;
}

AT(.text.qn8035)
u8 qn8035_seek(u16 freq)
{
    QNF_SetMute(1);
    return QND_RXValidCH(freq);//qn8035 step frequency unit is 10KHZ
}

AT(.text.qn8035)
void qn8035_mute(void)
{
    QNF_SetMute(1);
}

AT(.text.qn8035)
void qn8035_unmute(void)
{
    QNF_SetMute(0);
}


/*----------------------------------------------------------------------------*/
/**@brief FM模块QN8035检测
   @param 无
   @return 检测到QN8035模块返回1，否则返回0
   @note
*/
/*----------------------------------------------------------------------------*/
AT(.text.qn8035)
bool qn8035_is_online(void)
{
  	u8 temp;
  	QND_ReadReg(0x06);
    for(u8 i = 0; i < 3; i++) {
        temp = QND_ReadReg(0x06) & 0xFC;
        printf("\nqn8035 id = %02x\n",temp);
        if (temp == CHIPID_QN8035) {
            qn8035_xosc_setup();
            return true;
        }
    }
    return false;
}
#endif // FMRX_QN8035_EN
