/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      fiona       2014-09-25 16:40     1.0                build this file
 ********************************************************************************/

#include "ud_intern.h"
#include "ud_asrc.h"

asrc_adjust_record_t g_asrc_out1_record;

uint32 g_asrc_dec0_init;

uint32 g_asrc_dec1_init;

uint32 g_asrc_freq_in;

uint32 g_asrc_freq_out;

/******************************************************************************/
/*!
 * \par  Description:
 *  deal asrc variable init .
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_asrc_adjust.c
 * \par
 * \note
 *******************************************************************************/
void init_channel_record(asrc_adjust_record_t *pCh)
{
    uint16 i;

    //uint32 tem_dec0, tem_dec1;

    g_asrc_freq_in = g_play_sample;

#if(SYS_BOARD_TYPE == FOR_FPGA)
    g_asrc_freq_out = 48000;
#else
    //g_asrc_freq_out = g_play_sample;
    if(g_play_sample == 96000) {
        g_asrc_freq_out = 48000;
    } else {
        g_asrc_freq_out = g_play_sample;
    }
#endif

    // init record
    for(i = 0; i < K_RECORD_LEN; i++) {
        pCh->nHeToHfVec[i] = 0;
        pCh->nHfToHeVec[i] = 0;
    }
    //pCh->nGlbHeToHfCounter2 = 0;
    //pCh->nGlbHfToHeCounter2 = 0;
    pCh->nGlbHeToHfCounter = 0;
    pCh->nGlbHfToHeCounter = 0;
    pCh->nAverageHe2Hf = 0;
    pCh->nAverageHf2He = 0;
    //pCh->nGlbRecordStartCount = K_REC_START_POS;
    pCh->nFreqLow = (uint32)((long long) g_asrc_freq_in * (K_ASRC_OUT1_DEC0 - K_DELTA_INIT_DECIMATION0) / 1048576);
    pCh->nFreqHigh = (uint32)((long long) g_asrc_freq_in * (K_ASRC_OUT1_DEC1 + K_DELTA_INIT_DECIMATION1) / 1048576);
    // calculate nCenterfrequecy
    pCh->nCenterfrequecy = (pCh->nFreqHigh + pCh->nFreqLow) / 2;
    // clear decimation ratio modification counter
    pCh->nModCounter = 0;
    // last time record (K_NO_RECORD,  K_HF2HE,  K_HE2HF)
    pCh->nRecord = K_NO_RECORD;
    // init sample rate parameter
    pCh->nSampleRate = g_asrc_freq_out;

    if(g_asrc_adjust_timer == -1) {
        g_asrc_dec0_init = *((REG32)(ASRC_OUT1_DEC0));

        g_asrc_dec1_init = *((REG32)(ASRC_OUT1_DEC1));
    }

    //for test
    //*((REG32) (GPIOAOUTEN)) |= (1 << 23);
}

/******************************************************************************/
/*!
 * \par  Description:
 *  deal check asrc fifo full
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_asrc_adjust.c
 * \par
 * \note
 *******************************************************************************/
void check_asrc_chanel_fifo_full(void)
{
    if((*((REG32)(ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_PCMFIFOFull)) != 0) {
        //libc_print("full", 0, 0);
        // init channel parameter again
        init_channel_record(&g_asrc_out1_record);
        // config default decimation ratio 0
        *((REG32)(ASRC_OUT1_DEC0)) = g_asrc_dec0_init;
        // config default decimation ratio 1
        *((REG32)(ASRC_OUT1_DEC1)) = g_asrc_dec1_init;
        //while ((*((REG32) (ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_PCMFIFOFull)) != 0)
        //{
        // clear pcm full pending
        *((REG32)(ASRC_OUT1_IP)) = (1 << ASRC_OUT1_IP_PCMFIFOFull);
        //}
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  deal check asrc fifo empty
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_asrc_adjust.c
 * \par
 * \note
 *******************************************************************************/
void check_asrc_chanel_fifo_empty(void)
{
    if((*((REG32)(ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_PCMFIFOEmpty)) != 0) {
        //libc_print("empty", 0, 0);
        // init channel parameter again
        init_channel_record(&g_asrc_out1_record);
        // config default decimation ratio 0
        *((REG32)(ASRC_OUT1_DEC0)) = g_asrc_dec0_init;
        // config default decimation ratio 1
        *((REG32)(ASRC_OUT1_DEC1)) = g_asrc_dec1_init;
        // clear pcm empty pending
        //while ((*((REG32) (ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_PCMFIFOEmpty)) != 0)
        //{
        // clear pcm full pending
        *((REG32)(ASRC_OUT1_IP)) = (1 << ASRC_OUT1_IP_PCMFIFOEmpty);
        //}
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  deal modify asrc dec ratio
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_asrc_adjust.c
 * \par
 * \note
 *******************************************************************************/
void modify_asrc_chanel_decimation_ratio(void)
{
    int32 i;
    uint16 nTmp16Val;

    //check hf to he
    if((*((REG32)(ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_HFtoHEIP)) != 0) {

        //check if is record already
        if(g_asrc_out1_record.nRecord != K_HF2HE) {
            //low half-word
            nTmp16Val = (uint16)(*((REG32)(ASRC_OUT1_TIME)) >> 16);

            if(nTmp16Val != 0) {
                //save
                g_asrc_out1_record.nHfToHeVec[g_asrc_out1_record.nGlbHfToHeCounter] = nTmp16Val;

                //g_asrc_out1_record.nGlbHfToHeCounter++;
                if(g_asrc_out1_record.nGlbHfToHeCounter < K_RECORD_LEN) {
                    g_asrc_out1_record.nGlbHfToHeCounter++;
                }

                //set flag
                g_asrc_out1_record.nRecord = K_HF2HE;
            }
        }
        //libc_print("ftoe", asrc_out1_record.nGlbHfToHeCounter, 2);
        //*((REG32) (GPIOADAT)) &= (~(1 << 23));
        //clear pending bit
        *((REG32)(ASRC_OUT1_IP)) = (1 << ASRC_OUT1_IP_HFtoHEIP);

    }
    //check he to hf
    if((*((REG32)(ASRC_OUT1_IP)) & (1 << ASRC_OUT1_IP_HEtoHFIP)) != 0) {

        if(g_asrc_out1_record.nRecord != K_HE2HF) {
            //check low half-word
            nTmp16Val = (uint16)(*((REG32)(ASRC_OUT1_TIME)));

            if(nTmp16Val != 0) {
                //save
                g_asrc_out1_record.nHeToHfVec[g_asrc_out1_record.nGlbHeToHfCounter] = nTmp16Val;

                if(g_asrc_out1_record.nGlbHeToHfCounter < K_RECORD_LEN) {
                    g_asrc_out1_record.nGlbHeToHfCounter++;
                }

                //set flag
                g_asrc_out1_record.nRecord = K_HE2HF;
            }
        }
        //libc_print("etof", asrc_out1_record.nGlbHeToHfCounter, 2);
        //*((REG32) (GPIOADAT)) |= (1 << 23);
        //clear pending bit
        *((REG32)(ASRC_OUT1_IP)) = (1 << ASRC_OUT1_IP_HEtoHFIP);

    }
    if((g_asrc_out1_record.nGlbHeToHfCounter == K_RECORD_LEN)
            && (g_asrc_out1_record.nGlbHfToHeCounter >= K_RECORD_LEN)) {
        //asrc_out1_record.nValidRecordNum = asrc_out1_record.nGlbHfToHeCounter;
        // calculate frequency difference
        g_asrc_out1_record.nDiffFreq = (g_asrc_out1_record.nFreqHigh - g_asrc_out1_record.nFreqLow);
        // calculate average time
        g_asrc_out1_record.nAverageHe2Hf = 0;

        g_asrc_out1_record.nAverageHf2He = 0;

        //libc_print("adjust", 0, 0);

        for(i = 1; i < K_RECORD_LEN; i++) {
            g_asrc_out1_record.nAverageHe2Hf += g_asrc_out1_record.nHeToHfVec[i];
            g_asrc_out1_record.nAverageHf2He += g_asrc_out1_record.nHfToHeVec[i];
        }
        g_asrc_out1_record.nAverageHe2Hf /= (K_RECORD_LEN - 1);
        g_asrc_out1_record.nAverageHf2He /= (K_RECORD_LEN - 1);

        if((g_asrc_out1_record.nAverageHe2Hf <= K_NO_NEED_MOD_THRESHOLD) || (g_asrc_out1_record.nAverageHf2He
                <= K_NO_NEED_MOD_THRESHOLD)) {

            // clear counter
            g_asrc_out1_record.nGlbHfToHeCounter = g_asrc_out1_record.nGlbHeToHfCounter = 0;

            // calculate nFreqHigh to nCenterfrequecy
            if((g_asrc_out1_record.nAverageHe2Hf + g_asrc_out1_record.nAverageHf2He) != 0) {
                g_asrc_out1_record.nFreqHighToCenter = g_asrc_out1_record.nDiffFreq * g_asrc_out1_record.nAverageHe2Hf
                                                       / (g_asrc_out1_record.nAverageHe2Hf + g_asrc_out1_record.nAverageHf2He);
            }
            // calculate nCenterfrequecy to nFreqLow
            //asrc_out1_record.nFreqCenterToLow = (asrc_out1_record.nDiffFreq - asrc_out1_record.nFreqHighToCenter);
            // refresh center frequency
            g_asrc_out1_record.nCenterfrequecy = (g_asrc_out1_record.nFreqHigh - g_asrc_out1_record.nFreqHighToCenter);

            if((g_asrc_out1_record.nAverageHe2Hf < 0x100) || (g_asrc_out1_record.nAverageHf2He < 0x100)) {
                // refresh nFreqHigh
                g_asrc_out1_record.nFreqHigh = (g_asrc_out1_record.nCenterfrequecy + g_asrc_freq_in / 1000);
                // refresh nFreqLow
                g_asrc_out1_record.nFreqLow = (g_asrc_out1_record.nCenterfrequecy - g_asrc_freq_in / 1000);
            } else {
                // refresh nFreqHigh
                g_asrc_out1_record.nFreqHigh = (g_asrc_out1_record.nCenterfrequecy + g_asrc_freq_in / 4000);
                // refresh nFreqLow
                g_asrc_out1_record.nFreqLow = (g_asrc_out1_record.nCenterfrequecy - g_asrc_freq_in / 4000);
            }
            if(g_asrc_out1_record.nSampleRate != 0) {
                // calculate decimation rate 1 accroding to nFreqHigh
                g_asrc_out1_record.nDecimationRatio1 = ((g_asrc_out1_record.nFreqHigh << 14)
                                                        / g_asrc_out1_record.nSampleRate) << 6;
                // calculate decimation rate 0 accroding to nFreqLow
                g_asrc_out1_record.nDecimationRatio0 = ((g_asrc_out1_record.nFreqLow << 14)
                                                        / g_asrc_out1_record.nSampleRate) << 6;
            }
            // refresh ASRC_OUT0_DEC0
            *((REG32)(ASRC_OUT1_DEC0)) = g_asrc_out1_record.nDecimationRatio0;
            // refresh ASRC_OUT0_DEC1
            *((REG32)(ASRC_OUT1_DEC1)) = g_asrc_out1_record.nDecimationRatio1;

        }
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *  deal timer interrupt
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_asrc_adjust.c
 * \par
 * \note
 *******************************************************************************/
void deal_timer_process(void)
{

    // timer0 interrupt
    check_asrc_chanel_fifo_full();
    check_asrc_chanel_fifo_empty();
    modify_asrc_chanel_decimation_ratio();

}

