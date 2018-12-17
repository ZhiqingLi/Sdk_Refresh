//这是QN提供的代码， 最大限度地保留原样，这样便于问题的排查
#include "fm_qn8035.h"

UINT8 qnd_div1;
UINT8 qnd_div2;
UINT8 qnd_nco;


/**********************************************************************
 void QNF_RXInit(void)
 **********************************************************************
 Description: set to SNR based MPX control. Call this function before
 tune to one specific channel

 Parameters:
 None
 Return Value:
 None
 **********************************************************************/
void __section__(".rocode") QNF_RXInit(void)
{
    QNF_SetRegBit(0x1b, 0x08, 0x00); //Let NFILT adjust freely
    QNF_SetRegBit(0x2c, 0x3f, 0x12);
    QNF_SetRegBit(0x1d, 0x40, 0x00);
    QNF_SetRegBit(0x41, 0x0f, 0x0a);
    QND_WriteReg(0x45, 0x50);
    QNF_SetRegBit(0x3e, 0x80, 0x80);
    QNF_SetRegBit(0x41, 0xe0, 0xc0);
#if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        QNF_SetRegBit(0x42, 0x10, 0x10);
    }
#endif
}

/**********************************************************************
 void QNF_SetMute(UINT8 On)
 **********************************************************************
 Description: set register specified bit

 Parameters:
 On:        1: mute, 0: unmute
 Return Value:
 None
 **********************************************************************/
void QNF_SetMute(UINT8 On)
{
    if (On != 0)
    {
#if(qnd_ChipID == CHIPSUBID_QN8035A0)
        {
            QNF_SetRegBit(REG_DAC, 0x0b, 0x0b);
        }
#else
        {
            QNF_SetRegBit(0x4a, 0x20, 0x20);
        }
#endif
    }
    else
    {
#if(qnd_ChipID == CHIPSUBID_QN8035A0)
        {
            QNF_SetRegBit(REG_DAC, 0x0b, 0x00);
        }
#else
        {
            QNF_SetRegBit(0x4a, 0x20, 0x00);
        }
#endif
    }
}

/**********************************************************************
 void QNF_SetRegBit(UINT8 reg, UINT8 bitMask, UINT8 data_val)
 **********************************************************************
 Description: set register specified bit

 Parameters:
 reg:        register that will be set
 bitMask:    mask specified bit of register
 data_val:    data will be set for specified bit
 Return Value:
 None
 **********************************************************************/
void __section__(".rocode") QNF_SetRegBit(UINT8 reg, UINT8 bitMask, UINT8 data_val)
{
    UINT8 temp;
    temp = QND_ReadReg(reg);
    temp &= (UINT8) (~bitMask);
    temp |= data_val & bitMask;
    QND_WriteReg(reg, temp);
}

/**********************************************************************
 UINT16 QNF_GetCh(void)
 **********************************************************************
 Description: get current channel frequency

 Parameters:
 None
 Return Value:
 channel frequency
 **********************************************************************/
UINT16 __section__(".rocode") QNF_GetCh(void)
{
    UINT8 tCh;
    UINT8 tStep;
    UINT16 ch = 0;
    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= CH_CH;
    ch = tStep;
    tCh = QND_ReadReg(CH);
    ch = (ch << 8) + tCh;
    return CHREG2FREQ(ch);
}

/**********************************************************************
 void QNF_SetCh(UINT16 freq)
 **********************************************************************
 Description: set channel frequency

 Parameters:
 freq:  channel frequency to be set
 Return Value:
 None
 **********************************************************************/
void __section__(".rocode") QNF_SetCh(UINT16 freq)
{
    UINT8 tStep;
    UINT8 tCh;
    UINT16 f;
    //UINT16 pll_dlt;

    if (freq == 8550)
    {
        //QND_WriteReg(XTAL_DIV1, QND_XTAL_DIV1_855);
       // QND_WriteReg(XTAL_DIV2, QND_XTAL_DIV2_855);
        QND_WriteReg(NCO_COMP_VAL, 0x69);
        freq = 8570;
        g_freq_855 = 1;
    }
    else
    {
        QND_WriteReg(XTAL_DIV1, qnd_div1);
        QND_WriteReg(XTAL_DIV2, qnd_div2);
        QND_WriteReg(NCO_COMP_VAL, qnd_nco);
        g_freq_855 = 0;
    }
    //Manually set RX Channel index
    QNF_SetRegBit(SYSTEM1, CCA_CH_DIS, CCA_CH_DIS);
    f = FREQ2CHREG(freq);
    // set to reg: CH
    tCh = (UINT8) f;
    QND_WriteReg(CH, tCh);
    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= ~CH_CH;
    tStep |= ((UINT8) (f >> 8) & CH_CH);
    QND_WriteReg(CH_STEP, tStep);
}

/**********************************************************************
 void QNF_ConfigScan(UINT16 start,UINT16 stop, UINT8 step)
 **********************************************************************
 Description: config start, stop, step register for FM/AM CCA or CCS

 Parameters:
 start
 Set the frequency (10kHz) where scan to be started,
 e.g. 7600 for 76.00MHz.
 stop
 Set the frequency (10kHz) where scan to be stopped,
 e.g. 10800 for 108.00MHz
 step
 1: set leap step to (FM)100kHz / 10kHz(AM)
 2: set leap step to (FM)200kHz / 1kHz(AM)
 0:  set leap step to (FM)50kHz / 9kHz(AM)
 Return Value:
 None
 **********************************************************************/
void __section__(".rocode") QNF_ConfigScan(UINT16 start, UINT16 stop, UINT8 step)
{
    // calculate ch para
    UINT8 tStep = 0;
    UINT8 tS;
    UINT16 fStart;
    UINT16 fStop;

    fStart = FREQ2CHREG(start);
    fStop = FREQ2CHREG(stop);
    // set to reg: CH_START
    tS = (UINT8) fStart;
    QND_WriteReg(CH_START, tS);
    tStep |= ((UINT8) (fStart >> 6) & CH_CH_START);
    // set to reg: CH_STOP
    tS = (UINT8) fStop;
    QND_WriteReg(CH_STOP, tS);
    tStep |= ((UINT8) (fStop >> 4) & CH_CH_STOP);
    // set to reg: CH_STEP
    tStep |= step << 6;
    QND_WriteReg(CH_STEP, tStep);
}

/**********************************************************************
 int QN_ChipInitialization(void)
 **********************************************************************
 Description: chip first step initialization, called only by QND_Init()

 Parameters:
 None
 Return Value:
 None
 **********************************************************************/
int QN_ChipInitialization(void)
{
    uint8 chipid;
    uint8 IsQN8035B;
    uint8 rev;
    if (QND_WriteReg(0x00, 0x81) == (UINT8) -1)//复位
    {
        return -1;
    }
    QND_Delay(50);

    //根据这里打印出来的值，取设置宏定义qnd_ChipID, qnd_IsQN8035B
    chipid = (QND_ReadReg(CID1) & 0x03);
    IsQN8035B = QND_ReadReg(0x58) & 0x1f;

    if(qnd_ChipID != qnd_ChipID || IsQN8035B != qnd_IsQN8035B)
    {
        PRINTD_INFO_INT("qnd_ChipID = ", chipid);
        PRINTD_INFO_INT("qnd_IsQN8035B = ", IsQN8035B);
        return -1;
    }
    QND_WriteReg(0x58, 0x13);
    if(fm_init_parm.freq_mode == 0)//外晶振32.768k
    {
        //Following is where change the input clock type.as crystal or oscillator.
        QNF_SetRegBit(0x58, 0x80, QND_CRYSTAL_CLOCK);
        //Following is where change the input clock wave type,as sine-wave or square-wave.
        QNF_SetRegBit(0x01, 0x80,  QND_SINE_WAVE_CLOCK);//正弦波
        qnd_xtal_div0 = 0x01;
        qnd_xtal_div1 = 0x08;
        qnd_xtal_div2 = 0x5C;
    }
    else//cpu 输出13M
    {
        //Following is where change the input clock type.as crystal or oscillator.
        QNF_SetRegBit(0x58, 0x80, QND_INPUT_CLOCK);
        //Following is where change the input clock wave type,as sine-wave or square-wave.
        QNF_SetRegBit(0x01, 0x80,  QND_DIGITAL_CLOCK);//方波
        qnd_xtal_div0 = 0x8D;
        qnd_xtal_div1 = 0x89;
        qnd_xtal_div2 = 0x65;
    }



    //Following is where change the input clock frequency.
    QND_WriteReg(XTAL_DIV0, qnd_xtal_div0);
    QND_WriteReg(XTAL_DIV1, qnd_xtal_div1);
    QND_WriteReg(XTAL_DIV2, qnd_xtal_div2);
    QND_Delay(10);
    /********User sets chip working clock end ********/
    QND_WriteReg(0x54, 0x47); //mod PLL setting
    QND_WriteReg(SMP_HLD_THRD, 0xc4); //select SNR as filter3,SM step is 2db
    QNF_SetRegBit(0x40, 0x70, 0x70);
    QND_WriteReg(0x33, 0x9c); //set HCC Hystersis to 5db
    QND_WriteReg(0x2d, 0xd6); //notch filter threshold adjusting
    QND_WriteReg(0x43, 0x10); //notch filter threshold enable
    QNF_SetRegBit(SMSTART, 0x7f, SMSTART_VAL);
    QNF_SetRegBit(SNCSTART, 0x7f, SNCSTART_VAL);
    QNF_SetRegBit(HCCSTART, 0x7f, HCCSTART_VAL);
#if(qnd_ChipID == CHIPSUBID_QN8035A1)
    {
        QNF_SetRegBit(0x47, 0x0c, 0x08);
    }
#endif
    //these variables are used in QNF_SetCh() function.
    qnd_div1 = QND_ReadReg(XTAL_DIV1);
    qnd_div2 = QND_ReadReg(XTAL_DIV2);
    qnd_nco = QND_ReadReg(NCO_COMP_VAL);
    //libc_print("fm-test",(qnd_div1<<8)+qnd_div2,2);
    return 0;
}

/**********************************************************************
 int QND_Init(void)
 **********************************************************************
 Description: Initialize device to make it ready to have all functionality ready for use.

 Parameters:
 None
 **********************************************************************/
UINT8 QND_Init(void)
{
    if (-1 == QN_ChipInitialization())
    {
        return -1;
    }
    return 0;
}

/**********************************************************************
 void QND_TuneToCH(UINT16 ch)
 **********************************************************************
 Description: Tune to the specific channel. call QND_SetSysMode() before
 call this function
 Parameters:
 ch
 Set the frequency (10kHz) to be tuned,
 e.g. 101.30MHz will be set to 10130.
 Return Value:
 None
 **********************************************************************/
void __section__(".rocode") QND_TuneToCH(UINT16 ch)
{
    UINT8 reg;
    UINT8 imrFlag = 0;

    QNF_SetRegBit(REG_REF, ICPREF, 0x0a);
    QNF_RXInit();
    //QNF_SetMute(1);
#if(qnd_IsQN8035B == 0x13)
    {
        if ((ch == 7630) || (ch == 8580) || (ch == 9340) || (ch == 9390) || (ch == 9530) || (ch == 9980)
                || (ch == 10480))
        {
            imrFlag = 1;
        }
    }
#elif((qnd_ChipID == CHIPSUBID_QN8035A0) || (qnd_ChipID == CHIPSUBID_QN8035A1))
    {
        if((ch == 6910) || (ch == 7290) || (ch == 8430))
        {
            imrFlag = 1;
        }
        else if(qnd_ChipID == CHIPSUBID_QN8035A1)
        {
            if((ch == 7860) || (ch == 10710))
            {
                imrFlag = 1;
            }
        }
    }
#endif
    if (imrFlag == 1)
    {
        QNF_SetRegBit(CCA, IMR, IMR);
    }
    else
    {
        QNF_SetRegBit(CCA, IMR, 0x00);
    }
    QNF_ConfigScan(ch, ch, g_set_value.step);
    QNF_SetCh(ch);

#if((qnd_ChipID == CHIPSUBID_QN8035A0) || (qnd_ChipID == CHIPSUBID_QN8035A1))
    {
        QNF_SetRegBit(SYSTEM1, CHSC, CHSC);
    }
#else
    {
        QNF_SetRegBit(0x55, 0x80, 0x80);
        QND_Delay(ENABLE_2K_SPEED_PLL_DELAY);
        QNF_SetRegBit(0x55, 0x80, 0x00);
    }
#endif

    //Auto tuning
    QND_WriteReg(0x4f, 0x80);
    reg = QND_ReadReg(0x4f);
    reg >>= 1;
    QND_WriteReg(0x4f, reg);
#if(qnd_ChipID == CHIPSUBID_QN8035A0)
    {
        QND_Delay(300);
    }
#endif
    QND_Delay(CH_SETUP_DELAY_TIME / 10);//改短时间，解决按键处理不过来情况
    QNF_SetRegBit(REG_REF, ICPREF, 0x00);
    //QNF_SetMute(0);
}

/***********************************************************************
 void QND_RXSetTH(UINT8 th)
 ***********************************************************************
 Description: Setting the threshold value of automatic scan channel
 th:
 Setting threshold for quality of channel to be searched,
 the range of th value:CCA_SENSITIVITY_LEVEL_0 ~ CCA_SENSITIVITY_LEVEL_9
 Return Value:
 None
 ***********************************************************************/
static const UINT16 rssi_snr_TH_tbl[10] =
{
    CCA_SENSITIVITY_LEVEL_0, CCA_SENSITIVITY_LEVEL_1,
    CCA_SENSITIVITY_LEVEL_2, CCA_SENSITIVITY_LEVEL_3,
    CCA_SENSITIVITY_LEVEL_4, CCA_SENSITIVITY_LEVEL_5,
    CCA_SENSITIVITY_LEVEL_6, CCA_SENSITIVITY_LEVEL_7,
    CCA_SENSITIVITY_LEVEL_8, CCA_SENSITIVITY_LEVEL_9
};
void QND_RXSetTH(UINT8 th)
{
    UINT8 rssiTH;
    UINT8 snrTH;
    UINT16 rssi_snr_TH;

    rssi_snr_TH = rssi_snr_TH_tbl[th];
    rssiTH = (UINT8) (rssi_snr_TH >> 8);
    snrTH = (UINT8) (rssi_snr_TH & 0xff);
    QND_WriteReg(0x4f, 0x00); //enable auto tunning in CCA mode
    QNF_SetRegBit(REG_REF, ICPREF, 0x0a);
    QNF_SetRegBit(GAIN_SEL, 0x08, 0x08); //NFILT program is enabled
    //selection filter:filter3
    QNF_SetRegBit(CCA1, 0x30, 0x30);
    //Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
    QNF_SetRegBit(SYSTEM_CTL2, 0x40, 0x00);
    QND_WRITE(CCA_CNT1, 0x00);
    QNF_SetRegBit(CCA_CNT2, 0x3f, 0x03);
    //selection the time of CCA FSM wait SNR calculator to settle:20ms
    //0x00:     20ms(default)
    //0x40:     40ms
    //0x80:     60ms
    //0xC0:     100m
    QNF_SetRegBit(CCA_SNR_TH_1, 0xc0, 0x00);
    //selection the time of CCA FSM wait RF front end and AGC to settle:20ms
    //0x00:     10ms
    //0x40:     20ms(default)
    //0x80:     40ms
    //0xC0:     60ms
    QNF_SetRegBit(CCA_SNR_TH_2, 0xc0, 0x40);
    QNF_SetRegBit(CCA, 0x3f, rssiTH); //setting RSSI threshold for CCA
    QNF_SetRegBit(CCA_SNR_TH_1, 0x3f, snrTH); //setting SNR threshold for CCA
}

/***********************************************************************
 INT8 QND_RXValidCH(UINT16 freq, UINT8 step);
 ***********************************************************************
 Description: to validate a ch (frequency)(if it's a valid channel)
 Freq: specific channel frequency, unit: 10Khz
 e.g. 108.00MHz will be set to 10800.
 Step:
 FM:
 QND_FMSTEP_100KHZ: set leap step to 100kHz
 QND_FMSTEP_200KHZ: set leap step to 200kHz
 QND_FMSTEP_50KHZ:  set leap step to 50kHz
 Return Value:
 0: not a valid channel
 1: a valid channel at this frequency
 -1:chip does not normally work.
 ***********************************************************************/
INT8 __section__(".rocode") QND_RXValidCH(UINT16 freq, UINT8 step)
{
#if(HARD_SEEK == 0)
    UINT8 regValue;
    UINT8 timeOut = 5; //time out is 100ms
    UINT8 isValidChannelFlag = 0;
    //如果SPI的读写影响到这里的"锁台"(RXCCA_FAIL), 就在这里延时一段时间
    QND_Delay(3);

    QNF_ConfigScan(freq, freq, step);
    QNF_SetCh(freq);
    QND_WriteReg(0x4f, 0x80);
    regValue = QND_ReadReg(0x4f);
    regValue = (regValue >> 1);
    QND_WriteReg(0x4f, regValue);
    //TRACE("freq:%d  cap:%d\n",freq,regValue&0x3F);
    //enter CCA mode,channel index is decided by internal CCA
    QNF_SetRegBit(SYSTEM1, RXCCA_MASK, RX_CCA);// (dat & (~0x03))|0x02 
    while(1)
    {
        //如果I2C的读写影响到这里的"锁台"(RXCCA_FAIL), 就在这里延时一段时间
        QND_Delay(20);
        regValue = QND_ReadReg(SYSTEM1);
        //if it seeks a potential channel, the loop will be quited
        if((regValue & CHSC) == 0)
        {
            break;
        }
        //QND_Delay(20);//delay 5ms
        //if it was time out,chip would not normally work.
        timeOut--;
        if(timeOut == 0)
        {
            return -1;
        }
    }
    //TRACE("CHSC:%d,timeOut:%d \n",regValue&CHSC,timeOut);
    //reading out the rxcca_fail flag of RXCCA status
    isValidChannelFlag = ((QND_ReadReg(STATUS1) & RXCCA_FAIL) ? 0 : 1);
    PRINTD_DBG_INT("timeOut:", timeOut);
    if(isValidChannelFlag)
    {
        return 1;
    }
    else
#endif
    {
        return 0;
    }
}

//开始搜台(硬件搜台)
//在搜台过程中，读写SPI, I2C等，可能会产生干扰信号，使FM不能停在正确的频点
INT16 __section__(".rocode") QND_RXSeekCH(UINT16 start, UINT16 stop, UINT8 step)
{
#if(HARD_SEEK != 0)
    QNF_ConfigScan(start, stop, step);
    //enter CCA mode,channel index is decided by internal CCA
    QNF_SetRegBit(SYSTEM1, RXCCA_MASK, RX_CCA);
#endif
    return 0;
}

