/********************************************************************************
 *                              GL5116
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       fiona    2015-01-27 15:00     1.0             build this file
 ********************************************************************************/
#ifndef __AUDIO_DEVICE_DEFINE_H__
#define __AUDIO_DEVICE_DEFINE_H__

#include <attr.h>
#include <typeext.h>

#define K_SAMPLERATE_8k      0
#define K_SAMPLERATE_11k     1
#define K_SAMPLERATE_12k     2
#define K_SAMPLERATE_16k     3
#define K_SAMPLERATE_22k     4
#define K_SAMPLERATE_24k     5
#define K_SAMPLERATE_32k     6
#define K_SAMPLERATE_44k     7
#define K_SAMPLERATE_48k     8
#define K_SAMPLERATE_88k     9
#define K_SAMPLERATE_96k     10
#define K_SAMPLERATE_192k    11
#define K_SAMPLERATE_INDEX   K_SAMPLERATE_48k
#if K_SAMPLERATE_INDEX==K_SAMPLERATE_11k
#define K_AUDIO_PLL_CTL_APS 0x0   // 44.005kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_22k
#define K_AUDIO_PLL_CTL_APS 0x1   // 44.063kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_44k
#define K_AUDIO_PLL_CTL_APS 0x2   // 44.118kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_88k
#define K_AUDIO_PLL_CTL_APS 0x3   // 44.196kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_8k
#define K_AUDIO_PLL_CTL_APS 0x4   // 47.894kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_12k
#define K_AUDIO_PLL_CTL_APS 0x5   // 47.965kHz
#elif K_SAMPLERATE_INDEX==K_SAMPLERATE_16k
#define K_AUDIO_PLL_CTL_APS 0x6   // 48.018kHz
#else
#define K_AUDIO_PLL_CTL_APS 0x7   // 48.109kHz
#endif

/***********************  IN CHANNEL register *********************/

// Table size selection of filter coefficients:
// 0: 512 coefficients
// 1: 1024 coefficients
#define K_TAB_SEL             (0x0<<3)
// Ram select bits of in channel:
// 00,11: PCMRAM1 and PCMRAM2 (total: 2k*2*16bit)
// 01:    PCMRAM2 (total: 1k*2*16bit)
// 10:    URAM1 (total: 0.5k*2*16bit)
#define K_IN_RAM_SEL          (0x0<<4)
// IN channel read clock select bit:
// 00,01: CPU?ˉs AHB clock
// 10: DMA clock
// 11: DSP?ˉs APB clock
#define K_IN_R_CLKSEL         (0x3<<6)
// IN channel write clock select bit:
// 00: CPU?ˉs AHB clock
// 01:reserved
// 10:IIS-rx clock
// 11:PCM-rx clock
#define K_IN_W_CLKSEL         (0x2<<8)
// Mode select bits of in channel:
// 0: SRC mode (select decimation rate 0)
// 1: ASRC mode (select decimation rate by HF/HE signal)
#define K_IN_MOD_SEL          (0x0<<10)
#define K_IN_AVERAGE_NUM      (0x1<<16)
// decimation rate of ASRC_IN_DEC0 register
#define K_ASRC_IN_DEC0          1048576
// decimation rate of ASRC_IN_DEC1 register
#define K_ASRC_IN_DEC1          1048576
// left channel gain
#define K_ASRC_IN_LGAIN         1048576
// right channel gain
#define K_ASRC_IN_RGAIN         1048576
#define K_AVERAGE_NUM         (0x1<<16)
#define K_ASRC_IN_CTL      (K_TAB_SEL|K_IN_RAM_SEL|K_IN_R_CLKSEL|K_IN_W_CLKSEL|K_IN_MOD_SEL|K_AVERAGE_NUM)

/***********************  OUT0 CHANNEL register *********************/
// decimation rate of ASRC_OUT0_DEC0 register
#define K_ASRC_OUT0_DEC0        1048576
// decimation rate of ASRC_OUT0_DEC1 register
#define K_ASRC_OUT0_DEC1        1048576
// left channel gain
#define K_ASRC_OUT0_LGAIN       1048576
// right channel gain
#define K_ASRC_OUT0_RGAIN       1048576
// Ram select bits of out channel:
// 00: PCMRAM0, PCMRAM1 and PCMRAM2 (total: 2k*2*24bit)
// 01: PCMRAM0 and PCMRAM1 (total: 2k*2*16bit)
// 10: PCMRAM0 (total: 1k*2*16bit)
// 11: URAM0 (total: 1k*2*16bit)
#define K_OUT0_RAM_SEL        (0x0<<4)
// OUT channel write clock select bit:
// 00: CPU’s AHB clock
// 01: CPU’s AHB clock
// 10: DMA clock
// 11: DSP’s APB clock
#define K_OUT0_W_CLKSEL       (0x3<<6)
// OUT channel read clock select bit:
// 00: CPU’s AHB clock
// 01: CPU’s AHB clock
// 10: IIS-tx clock 0
// 11: PCM-tx clock
#define K_OUT0_R_CLKSEL       (0x2<<8)
// Mode select bits of out channel:
// 0: SRC mode (select decimation rate 0)
// 1: ASRC mode (select decimation rate by HF/HE signal)
#define K_OUT0_MOD_SEL        (0x0<<10)
#define K_OUT0_AVERAGE_NUM    (0x1<<16)
#define K_ASRC_OUT0_CTL       (K_OUT0_RAM_SEL|K_OUT0_W_CLKSEL|K_OUT0_R_CLKSEL|K_OUT0_MOD_SEL|K_OUT0_AVERAGE_NUM)

/***********************  OUT1 CHANNEL register *********************/// decimation rate of ASRC_OUT1_DEC0 register
#define K_ASRC_OUT1_DEC0        1048576
// decimation rate of ASRC_OUT1_DEC1 register
#define K_ASRC_OUT1_DEC1        1048576
// left channel gain
#define K_ASRC_OUT1_LGAIN       1048576
// right channel gain
#define K_ASRC_OUT1_RGAIN       1048576

// OUT channel write clock select bit:
// 00: CPU’s AHB clock
// 01: CPU’s AHB clock
// 10: DMA clock
// 11: DSP’s APB clock
#define K_OUT1_W_CLKSEL       (0x3<<6)
// OUT channel read clock select bit:
// 00: CPU’s AHB clock
// 01: CPU’s AHB clock
// 10: IIS-tx clock 0
// 11: PCM-tx clock
#define K_OUT1_R_CLKSEL       (0x2<<8)
// Mode select bits of out channel:
// 0: SRC mode (select decimation rate 0)
// 1: ASRC mode (select decimation rate by HF/HE signal)
#define K_OUT1_MOD_SEL        (0x0<<10)
#define K_OUT1_AVERAGE_NUM    (0x1<<16)
#define K_ASRC_OUT1_CTL       (K_OUT1_W_CLKSEL|K_OUT1_R_CLKSEL|K_OUT1_MOD_SEL|K_OUT1_AVERAGE_NUM)

/************************* 半空半满 ****************************/
// number of half full sample pairs
#define K_HF_SAMP_PAIR_NUM      768
// number of half empty sample pairs
#define K_HE_SAMP_PAIR_NUM      256

/********************* ASRC table size  *******************/

#define K_ASRC0_TAB_SIZE        (3584/4)
#define K_ASRC1_TAB_SIZE        (1539-K_ASRC0_TAB_SIZE)

/*************************PCM register******************************/

// PCM Data Mode Select
// 000: u-Law(8bit)
// 001: A-Law(8bit)
// 010: linear PCM(13bit)
// 011: linear PCM(14bit)
// 100: linear PCM(15bit)
// 101: linear PCM(16bit)
#define K_PCM_CTL_DATM        (0x5<<PCM_CTL_DATM_SHIFT)
// PCM Data Output Mode.
// 0: Normal Output
// 1: Forces PCM_OUT to ouput 0
#define K_PCM_CTL_DATO        (0x0<<PCM_CTL_DATO)
// PCM RX DRQ Enable
// 0: Disable
// 1: Enable
#define K_PCM_CTL_RXDE        (0x0<<PCM_CTL_RXDE)
// PCM TX DRQ Enable
// 0: Disable
// 1: Enable
#define K_PCM_CTL_TXDE        (0x0<<PCM_CTL_TXDE)
// PCM RX IRQ Enable.
// 0: Disable
// 1: Enable
#define K_PCM_CTL_RXIE        (0x0<<PCM_CTL_RXIE)
// PCM TX IRQ Enable.
// 0: Disable
// 1: Enable
#define K_PCM_CTL_TXIE        (0x0<<PCM_CTL_TXIE)
// SYNC Suppress Output Enable.
// 0: Enable SYNC output
// 1: Disable SYNC output
// when keeping PCM_CLK running when in master mode. Some CODEC utilize the mode to enter a low power mode.
#define K_PCM_CTL_SSOE        (0x0<<PCM_CTL_SSOE)
// LSB or MSB First.
// 0: MSB first
// 1: LSB first
// when transmitting and receiving voice samples.
#define K_PCM_CTL_LMFR        (0x0<<PCM_CTL_LMFR)
// PCMTX FIFO Input Select.
// 0x0: From CPU
// 0x1: From DMA
// 0x2: From ASRC PCMRAM
// 0x3: Reserved
#define K_PCM_CTL_TXFIS       (0x2<<PCM_CTL_TXFIS_SHIFT)
// Loop Back Enable.
// Set this bit to enable a loop back mode that data coming on the input will be presented on the output.
// 0: Disable
// 1: Enable
#define K_PCM_CTL_LBEN        (0x0<<PCM_CTL_LBEN)
// PCM Frame Mode Select
// 0: Short Frame Sync Mode
// 1: Long Frame Sync Mode
#define K_PCM_CTL_FRMS        (0x0<<PCM_CTL_FRMS)
// PCM Master/Slave Select.
// 0: Master
// 1: Slave
#define K_PCM_CTL_MS          (1<<PCM_CTL_MS)
// Inversion Enable.
// 0: Disable
// 1: Enable.
// When inversion enables, inversion is performed for A-Law even bit andd for u-Law all bit.
#define K_PCM_CTL_IVEN        (1<<PCM_CTL_IVEN)
// Sample Format
// 0: 8bit sample with 8 cycle slot duration
// 1: 8bit sample with 16 cycle slot duraton
#define K_PCM_CTL_SAMF        (0<<PCM_CTL_SAMF)
// Sign Extension Enable(only when 16bit slots are used).
// 0: Select zeros padding or audio gain.
// 1: Select sign extension.
// When writing the bit is 0, the unused bits are audio gain for 13-bit linear sample and zeros padding for 8-bit companded sample.
// When writing the bit is 1, the unused bits are both sign extension.
#define K_PCM_CTL_SEN        (1<<PCM_CTL_SEN)
// PCM Enable.
// 0: Disable
// 1: Enable
#define K_PCM_CTL_EN         (1<<PCM_CTL_EN)
// PCMRX FIFO Output Select.
// 0x0: To CPU
// 0x1: To DMA
// 0x2: To ASRC PCMRAM
// 0x3: Reserved
#define K_PCM_CTL_RXFOS     (0x2<<PCM_CTL_RXFOS_SHIFT)
#define K_PCM_CTL           (K_PCM_CTL_DATM|K_PCM_CTL_DATO|K_PCM_CTL_RXDE|K_PCM_CTL_TXDE|\
                             K_PCM_CTL_RXIE|K_PCM_CTL_TXIE|K_PCM_CTL_SSOE|K_PCM_CTL_LMFR|\
                             K_PCM_CTL_TXFIS|K_PCM_CTL_LBEN|K_PCM_CTL_FRMS|K_PCM_CTL_MS|\
                             K_PCM_CTL_IVEN|K_PCM_CTL_SAMF|K_PCM_CTL_SEN|K_PCM_CTL_EN|K_PCM_CTL_RXFOS)

/******************************RAM select table********************************/

// ram select table
#define K_OUT0_P012                              (0)  // out0 only, pcmram0/1/2->out0
#define K_OUT0_P012_IN_U1                        (1)  // out0 and in, pcmram0/1/2->out0, uram1->in
#define K_OUT0_P01_IN_U1                         (2)  // out0 and in, pcmram0/1->out0, uram1->in
#define K_OUT0_P01_IN_P2                         (3)  // out0 and in, pcmram0/1->out0, pcmram2->in
#define K_OUT0_P01_OUT1_P2_IN_U1                 (4)  // out0, out1 and in, pcmram0/1->out0, pcmram2->out1, uram1->in
#define K_OUT0_P01_OUT1_P2                       (5)  // out0 and out1, pcmram0/1->out0, pcmram2->out1
#define K_OUT0_P01                               (6)  // out0 only, pcmram0/1->out0
#define K_OUT0_P0                                (7)  // out0 only, pcmram0->out0
#define K_OUT0_P0_IN_P12                         (8)  // out0 and in, pcmram1/2->in
#define K_OUT0_P0_IN_P2                          (9)  // out0 and in, pcmram2->in
#define K_OUT0_P0_IN_U1                          (10) // out0 and in, uram1->in
#define K_OUT0_P0_OUT1_P2_IN_U1                  (11) // out0, out1 and in, pcmram0->out0, pcmram2->out1, uram1->in
#define K_OUT0_P0_OUT1_P2                        (12) // out0 and out1, pcmram0->out0, pcmram2->out1
#define K_OUT0_U0                                (13) // out0, uram0->out0
#define K_OUT0_U0_IN_P12                         (14) // out0 and in, uram0->out0, pcmram1/2->in
#define K_OUT0_U0_IN_P2                          (15) // out0 and in, uram0->out0, pcmram2->in
#define K_OUT0_U0_IN_U1                          (16) // out0 and in, uram0->out0, uram1->in
#define K_OUT0_U0_OUT1_P2_IN_U1                  (17) // out0, out1 and in, uram0->out0, pcmram2->out1, uram1->in
#define K_OUT0_U0_OUT1_P2                        (18) // out0 and out1, uram0->out0, pcmram2->out1
#define K_OUT1_P2                                (19) // out1 only, pcmram2->out1
#define K_IN_P12                                 (20) // in only, pcmram1/2->in
#define K_IN_P2                                  (21) // in only, pcmram2->in
#define K_IN_U1                                  (22) // in only, uram1->in
#define K_OUT1_P2_IN_U1                          (23) // out1 and in, pcmram2->out1, uram1->in
#define K_OUT1_P12                               (24) // out1 only, pcmram1/2->out1
#define K_OUT1_P12_IN_U1                         (25) // out1 and in, pcmram1/2->out1, uram1->in
#define K_OUT0_P0_OUT1_P12                       (26) // out0 and out1, pcmram0->out0, pcmram1/2->out1
#define K_OUT0_U0_OUT1_P12                       (27) // out0 and out1, uram0->out0, pcmram1/2->out1
#define K_OUT0_P0_OUT1_P12_IN_U1                 (28) // out0, out1 and in, pcmram0->out0, pcmram1/2->out1, uram1->in
#define K_OUT0_U0_OUT1_P12_IN_U1                 (29) // out0, out1 and in, uram0->out0, pcmram1/2->out1, uram1->in
///////////////////////////////////////////////////////////////////////////////////
#define K_PLL_FREQ         50  //50 MHz CORE PLL source clock select  26MHz(HOSC)
#define PMU_32K            0
#define HOSC               1
#define CORE_PLL           2
#define CORE_CLKSEL        CORE_PLL
#define K_PLL_FREQ_MUL_10  (K_PLL_FREQ*10)
#if CORE_CLKSEL == CORE_PLL
#define K_COREPLL_VALUE    (0x080|(K_PLL_FREQ_MUL_10/50))
#else
#define K_COREPLL_VALUE    (0x000|(K_PLL_FREQ_MUL_10/50))
#endif

#define fs_dac_96k  (0x00<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_48k  (0x01<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_32k  (0x02<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_24k  (0x03<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_16k  (0x04<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_12k  (0x05<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_8k   (0x06<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_44k  (0x01<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_22k  (0x03<<CMU_ADDACLK_DACCLKDIV_SHIFT)
#define fs_dac_11k  (0x05<<CMU_ADDACLK_DACCLKDIV_SHIFT)

#define fs_adc_96k  (0x00<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_48k  (0x01<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_32k  (0x02<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_24k  (0x03<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_16k  (0x04<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_12k  (0x05<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_8k   (0x06<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_44k  (0x01<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_22k  (0x03<<CMU_ADDACLK_ADCCLKDIV_SHIFT)
#define fs_adc_11k  (0x05<<CMU_ADDACLK_ADCCLKDIV_SHIFT)

#define FS_DAC fs_dac_48k
#define FS_ADC fs_adc_48k

///////////////////////////////////////////////////////////////////////////////////
#define aupll_44k_46_49    (0x00<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_44k_47_50    (0x01<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_44k_32_34    (0x02<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_44k_33_35    (0x03<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_48k_47_46    (0x04<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_48k_44_43    (0x05<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_48k_42_41    (0x06<<AUDIO_PLL_CTL_APS_SHIFT)
#define aupll_48k_39_38    (0x07<<AUDIO_PLL_CTL_APS_SHIFT)

#define FS_AUDIOPLL  aupll_44k_47_50

///////////////////////////////////////////////////////////////////////////////////
#define aupll_from_ck24    (0x00<<AUDIO_PLL_CTL_AUDPLL_SCLK_SEL)
#define aupll_from_hosc    (0x01<<AUDIO_PLL_CTL_AUDPLL_SCLK_SEL)
#define SOURCE_AUDIOPLL    aupll_from_ck24

///////////////////////////////////////////////////////////////////////////////////
//#define ADC_DIG_FROM_DMIC

#ifdef ADC_DIG_FROM_DMIC

#define dmic_left_en               (0x01<<ADC_DIGCTL_DMLEN)
#define dmic_right_en              (0x01<<ADC_DIGCTL_DMREN)
#define dmic_left_right_en         ((0x01<<ADC_DIGCTL_DMLEN)|(0x01<<ADC_DIGCTL_DMREN))
#define DMIC_LEFT_RIGHT_ENABLE     dmic_left_right_en

#define dmic_osr_128x              (~(0x01<<ADC_DIGCTL_DMOSRS))
#define dmic_osr_64x               (0x01<<ADC_DIGCTL_DMOSRS)
#define DMIC_OSR                   dmic_osr_64x

#endif

///////////////////////////////////////////////////////////////////////////////////
#define hpf_left_en                (0x01<<ADC_DIGCTL_HPFLEN)
#define hpf_right_en               (0x01<<ADC_DIGCTL_HPFREN)
#define hpf_left_right_en          ((0x01<<ADC_DIGCTL_HPFLEN)|(0x01<<ADC_DIGCTL_HPFREN))
#define hpf_left_right_disable     ((0x00<<ADC_DIGCTL_HPFLEN)|(0x00<<ADC_DIGCTL_HPFREN))
#define HPF_LEFT_RIGHT_ENABLE      hpf_left_right_disable

///////////////////////////////////////////////////////////////////////////////////
#define adc_dig_gain_0db           (0x0<<ADC_DIGCTL_ADGC_SHIFT)
#define adc_dig_gain_6db           (0x1<<ADC_DIGCTL_ADGC_SHIFT)
#define adc_dig_gain_12db          (0x2<<ADC_DIGCTL_ADGC_SHIFT)
#define ADC_DIG_GAIN               adc_dig_gain_0db

///////////////////////////////////////////////////////////////////////////////////
#define adc_a_debug                (0x1<<ADC_DIGCTL_AADEN)
#define adc_d_debug                (0x1<<ADC_DIGCTL_ADDEN)
#define no_adc_debug               ((0x0<<ADC_DIGCTL_AADEN)|(0x0<<ADC_DIGCTL_ADDEN))
#define ADC_DEBUG_ENABLE           adc_d_debug

#define adc_left_en                (0x01<<ADC_ANACTL_ADLEN)
#define adc_right_en               (0x01<<ADC_ANACTL_ADREN)
#define adc_left_right_en          ((0x01<<ADC_ANACTL_ADLEN)|(0x01<<ADC_ANACTL_ADREN))
#define ADC_LEFT_RIGHT_ENABLE      adc_left_right_en

///////////////////////////////////////////////////////////////////////////////////
#define ain_op0_left_en            (0x1<<ADC_ANACTL_OP0LEN)
#define ain_op0_right_en           (0x1<<ADC_ANACTL_OP0REN)
#define ain_op0_not_en             (0x0)
#define ain_op0_left_right_en      ((0x01<<ADC_ANACTL_OP0LEN)|(0x01<<ADC_ANACTL_OP0REN))
#define AIN_OP0_ENABLE             ain_op0_left_right_en

//#define ain_op0_gain_-12db         (0x0<<ADC_ANACTL_OP0G_SHIFT)
//#define ain_op0_gain_-3db          (0x1<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_0db           (0x2<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_1p5db         (0x3<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_3p0db         (0x4<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_4p5db         (0x5<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_6p0db         (0x6<<ADC_ANACTL_OP0G_SHIFT)
#define ain_op0_gain_7p5db         (0x7<<ADC_ANACTL_OP0G_SHIFT)
#define AIN_OP0_GAIN               ain_op0_gain_0db

///////////////////////////////////////////////////////////////////////////////////
#define ain_op1_left_en            (0x1<<ADC_ANACTL_OP1LEN)
#define ain_op1_right_en           (0x1<<ADC_ANACTL_OP1REN)
#define ain_op1_not_en             (0x0)
#define ain_op1_left_right_en      ((0x01<<ADC_ANACTL_OP1LEN)|(0x01<<ADC_ANACTL_OP1REN))
#define AIN_OP1_ENABLE             ain_op1_left_right_en

//#define ain_op1_gain_-12db         (0x0<<ADC_ANACTL_OP1G_SHIFT)
//#define ain_op1_gain_-3db          (0x1<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_0db           (0x2<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_1p5db         (0x3<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_3p0db         (0x4<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_4p5db         (0x5<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_6p0db         (0x6<<ADC_ANACTL_OP1G_SHIFT)
#define ain_op1_gain_7p5db         (0x7<<ADC_ANACTL_OP1G_SHIFT)
#define AIN_OP1_GAIN               ain_op1_gain_0db

///////////////////////////////////////////////////////////////////////////////////
#define ain_micop_left_en            (0x1<<ADC_ANACTL_MOPLEN)
#define ain_micop_right_en           (0x1<<ADC_ANACTL_MOPREN)
#define ain_micop_not_en             (0x0)
#define ain_micop_left_right_en      (0x1<<ADC_ANACTL_MOPLEN)|(0x1<<ADC_ANACTL_MOPREN)
#define AIN_MICOP_ENABLE             ain_micop_left_right_en

#define ain_micop_gain_26db          (0x0<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_30db          (0x1<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_31p5db        (0x2<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_33db          (0x3<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_34p5db        (0x4<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_36db          (0x5<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_37p5db        (0x6<<ADC_ANACTL_MOPG_SHIFT)
#define ain_micop_gain_39db          (0x7<<ADC_ANACTL_MOPG_SHIFT)
#define AIN_MICOP_GAIN               ain_micop_gain_26db

///////////////////////////////////////////////////////////////////////////////////
#define K_PLL_FREQ         50  //50 MHz CORE PLL source clock select  26MHz(HOSC)
#define PMU_32K            0
#define HOSC               1
#define CORE_PLL           2
#define CORE_CLKSEL        CORE_PLL
#define K_PLL_FREQ_MUL_10  (K_PLL_FREQ*10)
#if CORE_CLKSEL == CORE_PLL
#define K_COREPLL_VALUE    (0x080|(K_PLL_FREQ_MUL_10/50))
#else
#define K_COREPLL_VALUE    (0x000|(K_PLL_FREQ_MUL_10/50))
#endif

typedef void* (*audio_device_op_i)(void*, void*, void*);

typedef enum
{
    ENABLE_PA = 0, DISABLE_PA,
    //ENABLE_PA_DDV,
    //DISABLE_PA_DDV,
    SET_PA_VOLUME,
    GET_PA_VOLUME,
    ENABLE_DAC,
    DISABLE_DAC,
    SET_DAC_RATE,
    GET_DAC_RATE,
    ENABLE_AIN,
    DISABLE_AIN,
    ENABLE_AIN_OUT,
    DISABLE_AIN_OUT,
    SET_AIN_GAIN,
    //SET_ADC_GAIN,
    ENABLE_ADC,
    DISABLE_ADC,
    SET_ADC_RATE,
    CONFIG_ASRC,
    CLOSE_ASRC,
    SET_ASRC_RATE,
    //SET_EFFECT_INFO,
    //GET_FEATURE_INFO,
} audio_device_cmd_e;

struct audio_device_operations
{
    audio_device_op_i enable_pa;
    audio_device_op_i disable_pa;
    //audio_device_op_i enable_pa_ddv;
    //audio_device_op_i disable_pa_ddv;
    audio_device_op_i set_pa_volume;
    audio_device_op_i get_pa_volume;
    audio_device_op_i enable_dac;
    audio_device_op_i disable_dac;
    audio_device_op_i set_dac_rate;
    audio_device_op_i get_dac_rate;
    audio_device_op_i enable_ain;
    audio_device_op_i disable_ain;
    audio_device_op_i enable_ain_out;
    audio_device_op_i disable_ain_out;
    audio_device_op_i set_ain_gain;
    audio_device_op_i set_adc_gain;
    audio_device_op_i enable_adc;
    audio_device_op_i disable_adc;
    audio_device_op_i set_adc_rate;
    audio_device_op_i config_asrc;
    audio_device_op_i close_asrc;
    audio_device_op_i set_asrc_rate;
    //audio_device_op_i set_effect_param;
    //audio_device_op_i get_feature_info;
};

/*
 audio device 向上层提供统一的接口，即只提供一个API，并以ID的方式区分
 */
extern void* audio_device_op_entry(void *param1, void *param2, void *param3, audio_device_cmd_e cmd_type) __FAR__;

/*开pa根据参数设置内部pa、i2s、spdif */
/*ddv_sel--类型相关的选择、直驱非直驱;i2s、spdif的选组;
 i2s: 0--dac复用组;1--a0~a3;3--a0~a2 vro_s;4--b7~b9,a6
 pa_swing--内部pa使用,bit6(1<<6)采用2.8pp,(0<<6)采用1.6pp,bit0—1用于选择dac通道：0都开，1左声道，2右声道;
 aout_type--区分i2s 内部pa spfid*/
/*int32 audio_device_enable_pa(uint32 ddv_sel, uint32 pa_swing, uint32 aout_type)*/
#define enable_pa(a,b,c)      audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(uint32)(c), ENABLE_PA)

/*关闭pa根据参数设置内部pa、i2s、spdif aout_type 详见aout_type_e*/
/*int32 audio_device_disable_pa(uint32 aout_type, void *null2, void *null3) */
#define disable_pa(a)         audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), DISABLE_PA)

/*vol_pa:表示PA VOL，vol_da:表示DA VOL*/
/*int32 audio_device_set_pa_volume(uint32 vol_pa, uint32 vol_da, void *null3) */
#define set_pa_volume(a,b)    audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), SET_PA_VOLUME)

/*获取音量值，返回值:低8bit表示PA VOL，高8bit表示DA VOL*/
/*int32 audio_device_get_pa_volume(void *null1, void *null2, void *null3) */
#define get_pa_volume()      audio_device_op_entry((void*)(0), (void*)(0), (void*)(0), GET_PA_VOLUME)

/*使能dac src_type,dac的输入源 详见dac_fifo_in_e,dac_chan 选择dac1 or dac0*/
/*int32 audio_device_enable_dac(uint32 src_type, uint32 dac_chan, void *null3) */
#define enable_dac(a,b)       audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), ENABLE_DAC)

/*关闭dac dac_chan 关闭dac1 or dac0*/
/*int32 audio_device_disable_dac(uint32 dac_chan, void *null2, void *null3)*/
#define disable_dac(a)        audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), DISABLE_DAC)

/*设置dac采样率 dac_rate 采样率值,第二个参数暂未使用*/
/*int32 audio_device_set_dac_rate(uint32 dac_rate, uint32 chanel_no, void *null3) */
#define set_dac_rate(a,b)     audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), SET_DAC_RATE)

/*获取dac采样率*/
/*int32 audio_device_get_dac_rate(void *null1, void *null2, void *null3) */
#define get_dac_rate()        audio_device_op_entry((void*)(0), (void*)(0), (void*)(0), GET_DAC_RATE)

/*使能模拟输入*/
/*int32 audio_device_enable_ain(uint32 ain_type, uint32 ain_gain, void *null3) */
/*ain_type:模拟输入类型(如AI_SOURCE_FM);
 ain_gain:模拟输入的增益
 0x0: -12dB
 0x1: -3dB
 0x2: 0dB
 0x3: 1.5dB
 0x4: 3.0dB
 0x5: 4.5dB
 0x6: 6.0dB
 0x7: 7.5db
 */
#define enable_ain(a,b)       audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), ENABLE_AIN)

/*关闭模拟输入*/
/*int32 audio_device_disable_ain(uint32 ain_type, void *null2, void *null3) */
/*ain_type:模拟输入类型;*/
#define disable_ain(a)        audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), DISABLE_AIN)

/*打开aamute*/
/*int32 audio_device_enable_ain_out(uint32 out_mode, void *null2, void *null3) */
/*ain_type:模拟输入类型;*/
#define enable_ain_out(a)     audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), ENABLE_AIN_OUT)

/*关闭aamute*/
/*int32 audio_device_disable_ain_out(uint32 out_mode, void *null2, void *null3) */
/*ain_type:模拟输入类型;*/
#define disable_ain_out(a)    audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), DISABLE_AIN_OUT)

/*设置ain增益*/
/*int32 audio_device_set_ain_gain(uint32 ain_type, uint32 ain_gain, void *null3) */
/*ain_type:模拟输入类型;ain_gain:模拟输入的增益. (这两个参数的意义同enable_ain)*/
#define set_ain_gain(a,b)     audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), SET_AIN_GAIN)

//#define set_adc_gain(a)       audio_device_op_entry((void*)(a), (void*)(0), (void*)(0), SET_ADC_GAIN)

/*使能adc*/
/*int32 audio_device_enable_adc(uint32 src_type, uint32 adc_gain, void *null3) */
/*src_type:adc fifo的输出类型(如ADFOS_DSP 详见adc_fifo_out_e);adc_gain:adc的增益(0表示0dB, 1表示3dB, 2表示6dB... 15表示45dB)*/
#define enable_adc(a,b)       audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), ENABLE_ADC)

/*关闭adc*/
/*int32 audio_device_disable_adc(void *null1, void *null2, void *null3) */
#define disable_adc()         audio_device_op_entry((void*)(0), (void*)(0), (void*)(0), DISABLE_ADC)

/*设置adc采样率*/
/*int32 audio_device_set_adc_rate(uint32 adc_rate, void *null2, void *null3)*/
/*adc_rate:adc采样率*/
#define set_adc_rate(a)       audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), SET_ADC_RATE)

/*配置asrc*/
/*int32 audio_device_config_asrc(uint32 nTestSel, uint8 asrc_mode) */
/*nTestSel:asrc的配置项;asrc_mode:asrc配置值列表索引*/
#define config_asrc(a,b)      audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(0), CONFIG_ASRC)

/*关闭asrc*/
/*int32 audio_device_close_asrc(uint32 nTestSel) */
/*nTestSel:asrc的配置项*/
#define close_asrc(a)         audio_device_op_entry((void*)(uint32)(a), (void*)(0), (void*)(0), CLOSE_ASRC)

/*配置asrc抽取比例*/
/*int32 audio_device_set_asrc_rate(uint32 asrc_rate, uint32 chanel_no, uint32 asrc_offset)*/
/*asrc_rate:asrc输入采样率;chanel_no:asrc通道号;asrc_offset：asrc抽取偏移*/
#define set_asrc_rate(a,b,c)  audio_device_op_entry((void*)(uint32)(a), (void*)(uint32)(b), (void*)(uint32)(c), SET_ASRC_RATE)

/*设置音效参数*/
/*int32 audio_device_set_effect_param(uint32 set_type, void* param_ptr, void* null3)*/
/*set_type:设置类型;param_ptr:音效参数指针*/
//#define set_effect_param(a,b) audio_device_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), SET_EFFECT_INFO)

/*获取音效处理信息*/
/*int32 audio_device_get_feature_info(uint32 set_type, void* info_ptr, void* null3)*/
/*set_type:音效类型;info_ptr:信息指针*/
//#define get_feature_info(a,b) audio_device_op_entry((void*)(uint32)(a), (void*)(b), (void*)(0), GET_FEATURE_INFO)

//for analog in type def
/*!
 * \brief
 *      定义录音源的类型
 */
typedef enum
{
    /*! FM */
    AI_SOURCE_AUX0,//AI_SOURCE_FM,
    /*! LINEIN */
    AI_SOURCE_AUX1,//AI_SOURCE_LINEIN,
    /*! 单端MIC */
    AI_SOURCE_ASEMIC,
    /*! 差分MIC */
    AI_SOURCE_ADFMIC,
    /*! DMIC */
    AI_SOURCE_DMIC,
    /*! 不选择输入 */
    AI_SOURCE_NULL,
} mmm_ai_type_t;

/*!
 * \brief
 *      定义adc fifo输出
 */
typedef enum
{
    /*! cpu 访问 */
    ADFOS_CPU = 0,
    /*! dma访问 */
    ADFOS_DMA,
    /*! asrc访问 */
    ADFOS_ASRC,
    /*! dsp访问 */
    ADFOS_DSP,
} adc_fifo_out_e;

//for analog in type def
/*!
 * \brief
 *      定义录音源的类型
 */
typedef enum
{
    /*! dac */
    AO_SOURCE_DAC,
    /*! i2s */
    AO_SOURCE_I2S,
    /*!spdif */
    AO_SOURCE_SPDIF,
    /*! dac+I2S */
    AO_SOURCE_DACI2S,
    /*! dac+SPDIF */
    AO_SOURCE_DACSPDIF,
    /*! 不选择输出 */
    AO_SOURCE_NULL,
} aout_type_e;

/*!
 * \brief
 *      定义dac fifo输入选择
 */
typedef enum
{
    /*! cpu 访问 */
    DAFIS_CPU = 0,
    /*! dma访问 */
    DAFIS_DMA,
    /*! asrc访问 */
    DAFIS_ASRC,
    /*! dsp访问 */
    DAFIS_DSP,
} dac_fifo_in_e;

/*!
 * \brief
 *      定义dac chenel选择
 */
typedef enum
{
    /*! 通道0 */
    DAF0_EN = 0,
    /*! 通道1 */
    DAF1_EN,

} dac_chenel_e;

//for asrc param defle
typedef struct
{
    uint32 ctl_wr_clk;
    uint32 ctl_rd_clk;
    uint32 ctl_mode;
    uint32 reg_dec0;
    uint32 reg_dec1;
    uint32 reg_hfull;
    uint32 reg_hempty;
    uint32 by_pass;
} asrc_param_t;

//for asrc adjust
typedef struct _asrc_channel
{
    uint16 nHeToHfPcmFifoLevel[5];
    uint16 nHfToHePcmFifoLevel[5];
    uint16 nHeToHfVec[5];
    uint16 nHfToHeVec[5];
    //uint32  nCenterfrequecyVec[K_CENTRE_FREQ_RECORD_NUM];
    uint32 nGlbHeToHfCounter;
    uint32 nGlbHfToHeCounter;
    uint32 nAverageHe2Hf;
    uint32 nAverageHf2He;
    uint32 nGlbRecordStartCount;
    uint32 nFreqLow;
    uint32 nFreqHigh;
    uint32 nDecimationRatio0;
    uint32 nDecimationRatio1;
    uint32 nCenterfrequecy;
    uint32 nFreqCenterToLow;
    uint32 nFreqHighToCenter;
    uint32 nDiffFreq;
    uint32 nValidRecordNum;
    uint32 nModCounter;
    uint32 nRecord;
    //uint32  nGlbHeToHfCounter2;
    //uint32  nGlbHfToHeCounter2;
    uint32 nSampleRate;
} asrc_adjust_record_t;

//内部PA模拟部分宏定义
#define VOL_HARD_PA     40

#endif

