/********************************************************************************
 *            Copyright(c) 2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * ÃèÊö£ºGL5116 IC SPEC
 * ×÷Õß£ºliminxian
 ********************************************************************************/

/*
* Automatically generated register definition: don't edit
* GL5116 Spec Version_V1.0
* Sat 03-28-2015  10:18:29
*/
#ifndef __GL5116_REG_DEFINITION_H___
#define __GL5116_REG_DEFINITION_H___


//--------------DSP_Private_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     DSP_Private_Register_BASE                                         0x00000000
#define     DSP_TIMER_CTL                                                     (DSP_Private_Register_BASE+0x0000)
#define     DSP_TIMER_CMP                                                     (DSP_Private_Register_BASE+0x0004)
#define     DSP_TIMER_VAL                                                     (DSP_Private_Register_BASE+0x0008)
#define     DSP_ASRC_INCH_NUM                                                 (DSP_Private_Register_BASE+0x0100)
#define     DSP_ASRC_INCH_RFIFO                                               (DSP_Private_Register_BASE+0x0104)
#define     DSP_ASRC_OUTCH0_NUM                                               (DSP_Private_Register_BASE+0x0108)
#define     DSP_ASRC_OUTCH0_WFIFO                                             (DSP_Private_Register_BASE+0x010C)
#define     DSP_ASRC_OUTCH1_NUM                                               (DSP_Private_Register_BASE+0x0110)
#define     DSP_ASRC_OUTCH1_WFIFO                                             (DSP_Private_Register_BASE+0x0114)
#define     DSP_ASRC_IN_IP                                                    (DSP_Private_Register_BASE+0x0118)
#define     DSP_ASRC_OUT0_IP                                                  (DSP_Private_Register_BASE+0x011C)
#define     DSP_ASRC_OUT1_IP                                                  (DSP_Private_Register_BASE+0x0120)
#define     DSP_DAC_STAT                                                      (DSP_Private_Register_BASE+0x0200)
#define     DSP_DAC_DAT_FIFO0                                                 (DSP_Private_Register_BASE+0x0204)
#define     DSP_DAC_DAT_FIFO1                                                 (DSP_Private_Register_BASE+0x0208)
#define     DSP_ADC_STAT                                                      (DSP_Private_Register_BASE+0x020C)
#define     DSP_ADC_DAT                                                       (DSP_Private_Register_BASE+0x0210)
#define     DSP_AUDIO_PLL_CTL                                                 (DSP_Private_Register_BASE+0x0300)
#define     DSP_SLAVERESET_CTL                                                (DSP_Private_Register_BASE+0x1000)

//--------------Bits Location------------------------------------------//
#define     DSP_TIMER_CTL_PD                                                  8
#define     DSP_TIMER_CTL_IRQEN                                               4
#define     DSP_TIMER_CTL_EN                                                  0

#define     DSP_TIMER_CMP_CMP_e                                               31
#define     DSP_TIMER_CMP_CMP_SHIFT                                           0
#define     DSP_TIMER_CMP_CMP_MASK                                            (0xFFFFFFFF<<0)

#define     DSP_TIMER_VAL_VAL_e                                               31
#define     DSP_TIMER_VAL_VAL_SHIFT                                           0
#define     DSP_TIMER_VAL_VAL_MASK                                            (0xFFFFFFFF<<0)

#define     DSP_ASRC_INCH_NUM_ReadableNum_e                                   5
#define     DSP_ASRC_INCH_NUM_ReadableNum_SHIFT                               0
#define     DSP_ASRC_INCH_NUM_ReadableNum_MASK                                (0x3F<<0)

#define     DSP_ASRC_INCH_RFIFO_SignExt_e                                     31
#define     DSP_ASRC_INCH_RFIFO_SignExt_SHIFT                                 24
#define     DSP_ASRC_INCH_RFIFO_SignExt_MASK                                  (0xFF<<24)
#define     DSP_ASRC_INCH_RFIFO_ReadData_e                                    23
#define     DSP_ASRC_INCH_RFIFO_ReadData_SHIFT                                0
#define     DSP_ASRC_INCH_RFIFO_ReadData_MASK                                 (0xFFFFFF<<0)

#define     DSP_ASRC_OUTCH0_NUM_WriteableNum_e                                5
#define     DSP_ASRC_OUTCH0_NUM_WriteableNum_SHIFT                            0
#define     DSP_ASRC_OUTCH0_NUM_WriteableNum_MASK                             (0x3F<<0)

#define     DSP_ASRC_OUTCH0_WFIFO_WriteData_e                                 23
#define     DSP_ASRC_OUTCH0_WFIFO_WriteData_SHIFT                             0
#define     DSP_ASRC_OUTCH0_WFIFO_WriteData_MASK                              (0xFFFFFF<<0)

#define     DSP_ASRC_OUTCH1_NUM_WriteableNum_e                                10
#define     DSP_ASRC_OUTCH1_NUM_WriteableNum_SHIFT                            0
#define     DSP_ASRC_OUTCH1_NUM_WriteableNum_MASK                             (0x7FF<<0)

#define     DSP_ASRC_OUTCH1_WFIFO_WriteData_e                                 23
#define     DSP_ASRC_OUTCH1_WFIFO_WriteData_SHIFT                             0
#define     DSP_ASRC_OUTCH1_WFIFO_WriteData_MASK                              (0xFFFFFF<<0)

#define     DSP_ASRC_IN_IP_PCMFIFOFull                                        4
#define     DSP_ASRC_IN_IP_PCMFIFOEmpty                                       3
#define     DSP_ASRC_IN_IP_WFIFOEmpty                                         2
#define     DSP_ASRC_IN_IP_HFtoHEIP                                           1
#define     DSP_ASRC_IN_IP_HEtoHFIP                                           0

#define     DSP_ASRC_OUT0_IP_PCMFIFOFull                                      4
#define     DSP_ASRC_OUT0_IP_PCMFIFOEmpty                                     3
#define     DSP_ASRC_OUT0_IP_RFIFOFull                                        2
#define     DSP_ASRC_OUT0_IP_HFtoHEIP                                         1
#define     DSP_ASRC_OUT0_IP_HEtoHFIP                                         0

#define     DSP_ASRC_OUT1_IP_PCMFIFOFull                                      4
#define     DSP_ASRC_OUT1_IP_PCMFIFOEmpty                                     3
#define     DSP_ASRC_OUT1_IP_RFIFOFull                                        2
#define     DSP_ASRC_OUT1_IP_HFtoHEIP                                         1
#define     DSP_ASRC_OUT1_IP_HEtoHFIP                                         0

#define     DSP_DAC_STAT_DAF1IP                                               15
#define     DSP_DAC_STAT_DAF1F                                                14
#define     DSP_DAC_STAT_DAF1S_e                                              13
#define     DSP_DAC_STAT_DAF1S_SHIFT                                          8
#define     DSP_DAC_STAT_DAF1S_MASK                                           (0x3F<<8)
#define     DSP_DAC_STAT_DAF0IP                                               7
#define     DSP_DAC_STAT_DAF0F                                                6
#define     DSP_DAC_STAT_DAF0S_e                                              5
#define     DSP_DAC_STAT_DAF0S_SHIFT                                          0
#define     DSP_DAC_STAT_DAF0S_MASK                                           (0x3F<<0)

#define     DSP_DAC_DAT_FIFO0_DAF0DAT_e                                       31
#define     DSP_DAC_DAT_FIFO0_DAF0DAT_SHIFT                                   8
#define     DSP_DAC_DAT_FIFO0_DAF0DAT_MASK                                    (0xFFFFFF<<8)

#define     DSP_DAC_DAT_FIFO1_DAF1DAT_e                                       31
#define     DSP_DAC_DAT_FIFO1_DAF1DAT_SHIFT                                   8
#define     DSP_DAC_DAT_FIFO1_DAF1DAT_MASK                                    (0xFFFFFF<<8)

#define     DSP_ADC_STAT_ADFEF                                                7
#define     DSP_ADC_STAT_ADFIP                                                6
#define     DSP_ADC_STAT_ADFS_e                                               5
#define     DSP_ADC_STAT_ADFS_SHIFT                                           0
#define     DSP_ADC_STAT_ADFS_MASK                                            (0x3F<<0)

#define     DSP_ADC_DAT_ADDAT_e                                               31
#define     DSP_ADC_DAT_ADDAT_SHIFT                                           8
#define     DSP_ADC_DAT_ADDAT_MASK                                            (0xFFFFFF<<8)

#define     DSP_AUDIO_PLL_CTL_APS_e                                           3
#define     DSP_AUDIO_PLL_CTL_APS_SHIFT                                       0
#define     DSP_AUDIO_PLL_CTL_APS_MASK                                        (0xF<<0)

#define     DSP_SLAVERESET_CTL_TIMERRESET                                     0

//--------------MemoryController-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     MemoryController_BASE                                             0xc00a0000
#define     MemoryCtl                                                         (MemoryController_BASE+0x00000000)
#define     AlternateInstr0L                                                  (MemoryController_BASE+0x00000004)
#define     AlternateInstr0H                                                  (MemoryController_BASE+0x00000008)
#define     AlternateInstr1L                                                  (MemoryController_BASE+0x0000000c)
#define     AlternateInstr1H                                                  (MemoryController_BASE+0x00000010)
#define     AlternateInstr2L                                                  (MemoryController_BASE+0x00000014)
#define     AlternateInstr2H                                                  (MemoryController_BASE+0x00000018)
#define     AlternateInstr3L                                                  (MemoryController_BASE+0x0000001c)
#define     AlternateInstr3H                                                  (MemoryController_BASE+0x00000020)
#define     FixAddr0                                                          (MemoryController_BASE+0x00000024)
#define     FixAddr1                                                          (MemoryController_BASE+0x00000028)
#define     FixAddr2                                                          (MemoryController_BASE+0x0000002c)
#define     FixAddr3                                                          (MemoryController_BASE+0x00000030)
#define     bist_en                                                           (MemoryController_BASE+0x0000003c)
#define     bist_fin                                                          (MemoryController_BASE+0x00000040)
#define     bist_info                                                         (MemoryController_BASE+0x00000044)
#define     PageAddr0                                                         (MemoryController_BASE+0x0000004c)
#define     PageAddr1                                                         (MemoryController_BASE+0x00000050)
#define     PageAddr2                                                         (MemoryController_BASE+0x00000054)
#define     PageAddr3                                                         (MemoryController_BASE+0x00000058)
#define     PageAddr4                                                         (MemoryController_BASE+0x0000005c)
#define     PageAddr5                                                         (MemoryController_BASE+0x00000060)
#define     PageAddr6                                                         (MemoryController_BASE+0x00000064)
#define     PageAddr7                                                         (MemoryController_BASE+0x00000068)
#define     PageAddr8                                                         (MemoryController_BASE+0x0000006c)
#define     PageAddr9                                                         (MemoryController_BASE+0x00000070)
#define     PageAddr10                                                        (MemoryController_BASE+0x00000074)
#define     PageAddr11                                                        (MemoryController_BASE+0x00000078)
#define     PageAddr12                                                        (MemoryController_BASE+0x0000007c)
#define     PageAddr13                                                        (MemoryController_BASE+0x00000080)
#define     PageAddr14                                                        (MemoryController_BASE+0x00000084)
#define     PageAddr15                                                        (MemoryController_BASE+0x00000088)
#define     PageAddr16                                                        (MemoryController_BASE+0x0000008c)
#define     PageAddr17                                                        (MemoryController_BASE+0x00000090)
#define     PageAddr18                                                        (MemoryController_BASE+0x00000094)
#define     PageAddr19                                                        (MemoryController_BASE+0x00000098)
#define     PageAddr20                                                        (MemoryController_BASE+0x0000009c)
#define     PageAddr21                                                        (MemoryController_BASE+0x000000a0)
#define     DspPageAddr0                                                      (MemoryController_BASE+0x000000c0)
#define     DspPageAddr1                                                      (MemoryController_BASE+0x000000c4)
#define     DspPageAddr2                                                      (MemoryController_BASE+0x000000c8)
#define     DspPageAddr3                                                      (MemoryController_BASE+0x000000cc)
#define     MPUIE                                                             (MemoryController_BASE+0x000000d0)
#define     MPUIP                                                             (MemoryController_BASE+0x000000d4)
#define     MPUCtl0                                                           (MemoryController_BASE+0x000000d8)
#define     MPUBase0                                                          (MemoryController_BASE+0x000000dc)
#define     MPUEnd0                                                           (MemoryController_BASE+0x000000e0)
#define     MPUErrAddr0                                                       (MemoryController_BASE+0x000000e4)
#define     MPUCtl1                                                           (MemoryController_BASE+0x000000e8)
#define     MPUBase1                                                          (MemoryController_BASE+0x000000ec)
#define     MPUEnd1                                                           (MemoryController_BASE+0x000000f0)
#define     MPUErrAddr1                                                       (MemoryController_BASE+0x000000f4)
#define     MPUCtl2                                                           (MemoryController_BASE+0x000000f8)
#define     MPUBase2                                                          (MemoryController_BASE+0x000000fc)
#define     MPUEnd2                                                           (MemoryController_BASE+0x00000100)
#define     MPUErrAddr2                                                       (MemoryController_BASE+0x00000104)
#define     MPUCtl3                                                           (MemoryController_BASE+0x00000108)
#define     MPUBase3                                                          (MemoryController_BASE+0x0000010c)
#define     MPUEnd3                                                           (MemoryController_BASE+0x00000110)
#define     MPUErrAddr3                                                       (MemoryController_BASE+0x00000114)
#define     bist_en1                                                          (MemoryController_BASE+0x00000200)
#define     bist_fin1                                                         (MemoryController_BASE+0x00000204)
#define     bist_info1                                                        (MemoryController_BASE+0x00000208)
#define     DSP_D_B2_BANK0_ROM_bist_info                                      (MemoryController_BASE+0x0000020c)
#define     DSP_D_B2_BANK1_ROM_bist_info                                      (MemoryController_BASE+0x00000210)
#define     DSP_D_B2_BANK2_ROM_bist_info                                      (MemoryController_BASE+0x00000214)
#define     DSP_D_B2_BANK3_ROM_bist_info                                      (MemoryController_BASE+0x00000218)

//--------------Bits Location------------------------------------------//
#define     MemoryCtl_Jtag_disable                                            5
#define     MemoryCtl_Spi_boot_disable                                        4
#define     MemoryCtl_FixEn3                                                  3
#define     MemoryCtl_FixEn2                                                  2
#define     MemoryCtl_FixEn1                                                  1
#define     MemoryCtl_FixEn0                                                  0

#define     AlternateInstr0L_AlternatInstr0L_e                                31
#define     AlternateInstr0L_AlternatInstr0L_SHIFT                            0
#define     AlternateInstr0L_AlternatInstr0L_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr0H_AlternatInstr0H_e                                31
#define     AlternateInstr0H_AlternatInstr0H_SHIFT                            0
#define     AlternateInstr0H_AlternatInstr0H_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr1L_AlternatInstr1L_e                                31
#define     AlternateInstr1L_AlternatInstr1L_SHIFT                            0
#define     AlternateInstr1L_AlternatInstr1L_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr1H_AlternatInstr1H_e                                31
#define     AlternateInstr1H_AlternatInstr1H_SHIFT                            0
#define     AlternateInstr1H_AlternatInstr1H_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr2L_AlternatInstr2L_e                                31
#define     AlternateInstr2L_AlternatInstr2L_SHIFT                            0
#define     AlternateInstr2L_AlternatInstr2L_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr2H_AlternatInstr2H_e                                31
#define     AlternateInstr2H_AlternatInstr2H_SHIFT                            0
#define     AlternateInstr2H_AlternatInstr2H_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr3L_AlternatInstr3L_e                                31
#define     AlternateInstr3L_AlternatInstr3L_SHIFT                            0
#define     AlternateInstr3L_AlternatInstr3L_MASK                             (0xFFFFFFFF<<0)

#define     AlternateInstr3H_AlternatInstr3H_e                                31
#define     AlternateInstr3H_AlternatInstr3H_SHIFT                            0
#define     AlternateInstr3H_AlternatInstr3H_MASK                             (0xFFFFFFFF<<0)

#define     FixAddr0_FixAddr0_e                                               17
#define     FixAddr0_FixAddr0_SHIFT                                           2
#define     FixAddr0_FixAddr0_MASK                                            (0xFFFF<<2)

#define     FixAddr1_FixAddr1_e                                               17
#define     FixAddr1_FixAddr1_SHIFT                                           2
#define     FixAddr1_FixAddr1_MASK                                            (0xFFFF<<2)

#define     FixAddr2_FixAddr2_e                                               17
#define     FixAddr2_FixAddr2_SHIFT                                           2
#define     FixAddr2_FixAddr2_MASK                                            (0xFFFF<<2)

#define     FixAddr3_FixAddr3_e                                               17
#define     FixAddr3_FixAddr3_SHIFT                                           2
#define     FixAddr3_FixAddr3_MASK                                            (0xFFFF<<2)

#define     bist_en_DSP_D_B2_BANK3_ROM_bist_en                                31
#define     bist_en_DSP_D_B2_BANK2_ROM_bist_en                                30
#define     bist_en_DSP_D_B2_BANK1_ROM_bist_en                                29
#define     bist_en_DSP_D_B2_BANK0_ROM_bist_en                                28
#define     bist_en_DSP_D_B1_BANK3_RAM_bist_en                                27
#define     bist_en_DSP_D_B1_BANK2_RAM_bist_en                                26
#define     bist_en_DSP_D_B1_BANK1_RAM_bist_en                                25
#define     bist_en_DSP_D_B1_BANK0_RAM_bist_en                                24
#define     bist_en_DSP_D_B0_BANK3_bist_en                                    23
#define     bist_en_DSP_D_B0_BANK2_bist_en                                    22
#define     bist_en_DSP_D_B0_BANK1_bist_en                                    21
#define     bist_en_DSP_D_B0_BANK0_bist_en                                    20
#define     bist_en_DSP_I_CACHE_BANK3_bist_en                                 19
#define     bist_en_DSP_I_CACHE_BANK2_bist_en                                 18
#define     bist_en_DSP_I_CACHE_BANK1_bist_en                                 17
#define     bist_en_DSP_I_CACHE_BANK0_bist_en                                 16
#define     bist_en_ASRC_DATA_RAM_bist_en                                     15
#define     bist_en_ASRCBUF1_bist_en                                          14
#define     bist_en_ASRCBUF0_bist_en                                          13
#define     bist_en_CARD_BUF01_bist_en                                        12
#define     bist_en_DSP_ICACHE_TAG_RAM_bist_en                                11
#define     bist_en_URAM01_bist_en                                            10
#define     bist_en_PCMRAM2_bist_en                                           9
#define     bist_en_PCMRAM1_bist_en                                           8
#define     bist_en_PCMRAM0_bist_en                                           7
#define     bist_en_RAM6_bist_en                                              6
#define     bist_en_RAM5_bist_en                                              5
#define     bist_en_RAM4_bist_en                                              4
#define     bist_en_RAM3_bist_en                                              3
#define     bist_en_RAM2_bist_en                                              2
#define     bist_en_RAM1_bist_en                                              1
#define     bist_en_RAM0_bist_en                                              0

#define     bist_fin_DSP_D_B2_BANK3_ROM_bist_fin                              31
#define     bist_fin_DSP_D_B2_BANK2_ROM_bist_fin                              30
#define     bist_fin_DSP_D_B2_BANK1_ROM_bist_fin                              29
#define     bist_fin_DSP_D_B2_BANK0_ROM_bist_fin                              28
#define     bist_fin_DSP_D_B1_BANK3_RAM_bist_fin                              27
#define     bist_fin_DSP_D_B1_BANK2_RAM_bist_fin                              26
#define     bist_fin_DSP_D_B1_BANK1_RAM_bist_fin                              25
#define     bist_fin_DSP_D_B1_BANK0_RAM_bist_fin                              24
#define     bist_fin_DSP_D_B0_BANK3_bist_fin                                  23
#define     bist_fin_DSP_D_B0_BANK2_bist_fin                                  22
#define     bist_fin_DSP_D_B0_BANK1_bist_fin                                  21
#define     bist_fin_DSP_D_B0_BANK0_bist_fin                                  20
#define     bist_fin_DSP_I_CACHE_BANK3_bist_fin                               19
#define     bist_fin_DSP_I_CACHE_BANK2_bist_fin                               18
#define     bist_fin_DSP_I_CACHE_BANK1_bist_fin                               17
#define     bist_fin_DSP_I_CACHE_BANK0_bist_fin                               16
#define     bist_fin_ASRC_DATA_RAM_bist_fin                                   15
#define     bist_fin_ASRCBUF1_bist_fin                                        14
#define     bist_fin_ASRCBUF0_bist_fin                                        13
#define     bist_fin_CARD_BUF01_bist_fin                                      12
#define     bist_fin_DSP_ICACHE_TAG_RAM_bist_fin                              11
#define     bist_fin_URAM01_bist_fin                                          10
#define     bist_fin_PCMRAM2_bist_fin                                         9
#define     bist_fin_PCMRAM1_bist_fin                                         8
#define     bist_fin_PCMRAM0_bist_fin                                         7
#define     bist_fin_RAM6_bist_fin                                            6
#define     bist_fin_RAM5_bist_fin                                            5
#define     bist_fin_RAM4_bist_fin                                            4
#define     bist_fin_RAM3_bist_fin                                            3
#define     bist_fin_RAM2_bist_fin                                            2
#define     bist_fin_RAM1_bist_fin                                            1
#define     bist_fin_RAM0_bist_fin                                            0

#define     bist_info_DSP_D_B1_BANK3_RAM_bist_info                            27
#define     bist_info_DSP_D_B1_BANK2_RAM_bist_info                            26
#define     bist_info_DSP_D_B1_BANK1_RAM_bist_info                            25
#define     bist_info_DSP_D_B1_BANK0_RAM_bist_info                            24
#define     bist_info_DSP_D_B0_BANK3_bist_info                                23
#define     bist_info_DSP_D_B0_BANK2_bist_info                                22
#define     bist_info_DSP_D_B0_BANK1_bist_info                                21
#define     bist_info_DSP_D_B0_BANK0_bist_info                                20
#define     bist_info_DSP_I_CACHE_BANK3_bist_info                             19
#define     bist_info_DSP_I_CACHE_BANK2_bist_info                             18
#define     bist_info_DSP_I_CACHE_BANK1_bist_info                             17
#define     bist_info_DSP_I_CACHE_BANK0_bist_info                             16
#define     bist_info_ASRC_DATA_RAM_bist_info                                 15
#define     bist_info_ASRCBUF1_bist_info                                      14
#define     bist_info_ASRCBUF0_bist_info                                      13
#define     bist_info_CARD_BUF01_bist_info                                    12
#define     bist_info_DSP_ICACHE_TAG_RAM_bist_info                            11
#define     bist_info_URAM01_bist_info                                        10
#define     bist_info_PCMRAM2_bist_info                                       9
#define     bist_info_PCMRAM1_bist_info                                       8
#define     bist_info_PCMRAM0_bist_info                                       7
#define     bist_info_RAM6_bist_info                                          6
#define     bist_info_RAM5_bist_info                                          5
#define     bist_info_RAM4_bist_info                                          4
#define     bist_info_RAM3_bist_info                                          3
#define     bist_info_RAM2_bist_info                                          2
#define     bist_info_RAM1_bist_info                                          1
#define     bist_info_RAM0_bist_info                                          0

#define     PageAddr0_PageAddr_e                                              31
#define     PageAddr0_PageAddr_SHIFT                                          18
#define     PageAddr0_PageAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr1_PageAddr_e                                              31
#define     PageAddr1_PageAddr_SHIFT                                          18
#define     PageAddr1_PageAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr2_PageAddr_e                                              31
#define     PageAddr2_PageAddr_SHIFT                                          18
#define     PageAddr2_PageAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr3_PagAddr_e                                               31
#define     PageAddr3_PagAddr_SHIFT                                           18
#define     PageAddr3_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr4_PagAddr_e                                               31
#define     PageAddr4_PagAddr_SHIFT                                           18
#define     PageAddr4_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr5_PagAddr_e                                               31
#define     PageAddr5_PagAddr_SHIFT                                           18
#define     PageAddr5_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr6_PagAddr_e                                               31
#define     PageAddr6_PagAddr_SHIFT                                           18
#define     PageAddr6_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr7_PagAddr_e                                               31
#define     PageAddr7_PagAddr_SHIFT                                           18
#define     PageAddr7_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr8_PagAddr_e                                               31
#define     PageAddr8_PagAddr_SHIFT                                           18
#define     PageAddr8_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr9_PagAddr_e                                               31
#define     PageAddr9_PagAddr_SHIFT                                           18
#define     PageAddr9_PagAddr_MASK                                            (0x3FFF<<18)

#define     PageAddr10_PagAddr_e                                              31
#define     PageAddr10_PagAddr_SHIFT                                          18
#define     PageAddr10_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr11_PagAddr_e                                              31
#define     PageAddr11_PagAddr_SHIFT                                          18
#define     PageAddr11_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr12_PagAddr_e                                              31
#define     PageAddr12_PagAddr_SHIFT                                          18
#define     PageAddr12_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr13_PagAddr_e                                              31
#define     PageAddr13_PagAddr_SHIFT                                          18
#define     PageAddr13_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr14_PagAddr_e                                              31
#define     PageAddr14_PagAddr_SHIFT                                          18
#define     PageAddr14_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr15_PagAddr_e                                              31
#define     PageAddr15_PagAddr_SHIFT                                          18
#define     PageAddr15_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr16_PagAddr_e                                              31
#define     PageAddr16_PagAddr_SHIFT                                          18
#define     PageAddr16_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr17_PagAddr_e                                              31
#define     PageAddr17_PagAddr_SHIFT                                          18
#define     PageAddr17_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr18_PagAddr_e                                              31
#define     PageAddr18_PagAddr_SHIFT                                          18
#define     PageAddr18_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr19_PagAddr_e                                              31
#define     PageAddr19_PagAddr_SHIFT                                          18
#define     PageAddr19_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr20_PagAddr_e                                              31
#define     PageAddr20_PagAddr_SHIFT                                          18
#define     PageAddr20_PagAddr_MASK                                           (0x3FFF<<18)

#define     PageAddr21_PagAddr_e                                              31
#define     PageAddr21_PagAddr_SHIFT                                          18
#define     PageAddr21_PagAddr_MASK                                           (0x3FFF<<18)

#define     DspPageAddr0_DspPagAddr_e                                         31
#define     DspPageAddr0_DspPagAddr_SHIFT                                     18
#define     DspPageAddr0_DspPagAddr_MASK                                      (0x3FFF<<18)

#define     DspPageAddr1_DspPagAddr_e                                         31
#define     DspPageAddr1_DspPagAddr_SHIFT                                     18
#define     DspPageAddr1_DspPagAddr_MASK                                      (0x3FFF<<18)

#define     DspPageAddr2_DspPagAddr_e                                         31
#define     DspPageAddr2_DspPagAddr_SHIFT                                     18
#define     DspPageAddr2_DspPagAddr_MASK                                      (0x3FFF<<18)

#define     DspPageAddr3_DspPagAddr_e                                         31
#define     DspPageAddr3_DspPagAddr_SHIFT                                     18
#define     DspPageAddr3_DspPagAddr_MASK                                      (0x3FFF<<18)

#define     MPUIE_MPU3_DSP_PR_ErrIE                                           28
#define     MPUIE_MPU3_DMA_W_ErrIE                                            27
#define     MPUIE_MPU3_CPU_DW_ErrIE                                           26
#define     MPUIE_MPU3_DSP_DR_ErrIE                                           25
#define     MPUIE_MPU3_DSP_DW_ErrIE                                           24
#define     MPUIE_MPU2_DSP_PR_ErrIE                                           20
#define     MPUIE_MPU2_DMA_W_ErrIE                                            19
#define     MPUIE_MPU2_CPU_DW_ErrIE                                           18
#define     MPUIE_MPU2_DSP_DR_ErrIE                                           17
#define     MPUIE_MPU2_DSP_DW_ErrIE                                           16
#define     MPUIE_MPU1_DSP_PR_ErrIE                                           12
#define     MPUIE_MPU1_DMA_W_ErrIE                                            11
#define     MPUIE_MPU1_CPU_DW_ErrIE                                           10
#define     MPUIE_MPU1_DSP_DR_ErrIE                                           9
#define     MPUIE_MPU1_DSP_DW_ErrIE                                           8
#define     MPUIE_MPU0_DSP_PR_ErrIE                                           4
#define     MPUIE_MPU0_DMA_W_ErrIE                                            3
#define     MPUIE_MPU0_CPU_DW_ErrIE                                           2
#define     MPUIE_MPU0_DSP_DR_ErrIE                                           1
#define     MPUIE_MPU0_DSP_DW_ErrIE                                           0

#define     MPUIP_MPU3_DSP_PR_ErrIP                                           28
#define     MPUIP_MPU3_DMA_W_ErrIP                                            27
#define     MPUIP_MPU3_CPU_DW_ErrIP                                           26
#define     MPUIP_MPU3_DSP_DR_ErrIP                                           25
#define     MPUIP_MPU3_DSP_DW_ErrIP                                           24
#define     MPUIP_MPU2_DSP_PR_ErrIP                                           20
#define     MPUIP_MPU2_DMA_W_ErrIP                                            19
#define     MPUIP_MPU2_CPU_DW_ErrIP                                           18
#define     MPUIP_MPU2_DSP_DR_ErrIP                                           17
#define     MPUIP_MPU2_DSP_DW_ErrIP                                           16
#define     MPUIP_MPU1_DSP_PR_ErrIP                                           12
#define     MPUIP_MPU1_DMA_W_ErrIP                                            11
#define     MPUIP_MPU1_CPU_DW_ErrIP                                           10
#define     MPUIP_MPU1_DSP_DR_ErrIP                                           9
#define     MPUIP_MPU1_DSP_DW_ErrIP                                           8
#define     MPUIP_MPU0_DSP_PR_ErrIP                                           4
#define     MPUIP_MPU0_DMA_W_ErrIP                                            3
#define     MPUIP_MPU0_CPU_DW_ErrIP                                           2
#define     MPUIP_MPU0_DSP_DR_ErrIP                                           1
#define     MPUIP_MPU0_DSP_DW_ErrIP                                           0

#define     MPUCtl0_DSP_PR_ACCESS_CTL                                         5
#define     MPUCtl0_DMA_W_ACCESS_CTL                                          4
#define     MPUCtl0_CPU_DW_ACCESS_CTL                                         3
#define     MPUCtl0_DSP_DR_ACCESS_CTL                                         2
#define     MPUCtl0_DSP_DW_ACCESS_CTL                                         1
#define     MPUCtl0_MPUEN0                                                    0

#define     MPUBase0_BaseAddress_e                                            17
#define     MPUBase0_BaseAddress_SHIFT                                        0
#define     MPUBase0_BaseAddress_MASK                                         (0x3FFFF<<0)

#define     MPUEnd0_EndAddress_e                                              17
#define     MPUEnd0_EndAddress_SHIFT                                          0
#define     MPUEnd0_EndAddress_MASK                                           (0x3FFFF<<0)

#define     MPUErrAddr0_ErrAddress_e                                          17
#define     MPUErrAddr0_ErrAddress_SHIFT                                      0
#define     MPUErrAddr0_ErrAddress_MASK                                       (0x3FFFF<<0)

#define     MPUCtl1_DSP_PR_ACCESS_CTL                                         5
#define     MPUCtl1_DMA_W_ACCESS_CTL                                          4
#define     MPUCtl1_CPU_DW_ACCESS_CTL                                         3
#define     MPUCtl1_DSP_DR_ACCESS_CTL                                         2
#define     MPUCtl1_DSP_DW_ACCESS_CTL                                         1
#define     MPUCtl1_MPUEN1                                                    0

#define     MPUBase1_BaseAddress_e                                            17
#define     MPUBase1_BaseAddress_SHIFT                                        0
#define     MPUBase1_BaseAddress_MASK                                         (0x3FFFF<<0)

#define     MPUEnd1_EndAddress_e                                              17
#define     MPUEnd1_EndAddress_SHIFT                                          0
#define     MPUEnd1_EndAddress_MASK                                           (0x3FFFF<<0)

#define     MPUErrAddr1_ErrAddress_e                                          17
#define     MPUErrAddr1_ErrAddress_SHIFT                                      0
#define     MPUErrAddr1_ErrAddress_MASK                                       (0x3FFFF<<0)

#define     MPUCtl2_DSP_PR_ACCESS_CTL                                         5
#define     MPUCtl2_DMA_W_ACCESS_CTL                                          4
#define     MPUCtl2_CPU_DW_ACCESS_CTL                                         3
#define     MPUCtl2_DSP_DR_ACCESS_CTL                                         2
#define     MPUCtl2_DSP_DW_ACCESS_CTL                                         1
#define     MPUCtl2_MPUEN2                                                    0

#define     MPUBase2_BaseAddress_e                                            17
#define     MPUBase2_BaseAddress_SHIFT                                        0
#define     MPUBase2_BaseAddress_MASK                                         (0x3FFFF<<0)

#define     MPUEnd2_EndAddress_e                                              17
#define     MPUEnd2_EndAddress_SHIFT                                          0
#define     MPUEnd2_EndAddress_MASK                                           (0x3FFFF<<0)

#define     MPUErrAddr2_ErrAddress_e                                          17
#define     MPUErrAddr2_ErrAddress_SHIFT                                      0
#define     MPUErrAddr2_ErrAddress_MASK                                       (0x3FFFF<<0)

#define     MPUCtl3_DSP_PR_ACCESS_CTL                                         5
#define     MPUCtl3_DMA_W_ACCESS_CTL                                          4
#define     MPUCtl3_CPU_DW_ACCESS_CTL                                         3
#define     MPUCtl3_DSP_DR_ACCESS_CTL                                         2
#define     MPUCtl3_DSP_DW_ACCESS_CTL                                         1
#define     MPUCtl3_MPUEN3                                                    0

#define     MPUBase3_BaseAddress_e                                            17
#define     MPUBase3_BaseAddress_SHIFT                                        0
#define     MPUBase3_BaseAddress_MASK                                         (0x3FFFF<<0)

#define     MPUEnd3_EndAddress_e                                              17
#define     MPUEnd3_EndAddress_SHIFT                                          0
#define     MPUEnd3_EndAddress_MASK                                           (0x3FFFF<<0)

#define     MPUErrAddr3_ErrAddress_e                                          17
#define     MPUErrAddr3_ErrAddress_SHIFT                                      0
#define     MPUErrAddr3_ErrAddress_MASK                                       (0x3FFFF<<0)

#define     bist_en1_DAC_RAM_bist_en                                          7
#define     bist_en1_ADC_RAM_bist_en                                          6

#define     bist_fin1_DAC_RAM_bist_fin                                        7
#define     bist_fin1_ADC_RAM_bist_fin                                        6

#define     bist_info1_DAC_RAM_bist_info                                      7
#define     bist_info1_ADC_RAM_bist_info                                      6

#define     DSP_D_B2_BANK0_ROM_bist_info_value_e                              31
#define     DSP_D_B2_BANK0_ROM_bist_info_value_SHIFT                          0
#define     DSP_D_B2_BANK0_ROM_bist_info_value_MASK                           (0xFFFFFFFF<<0)

#define     DSP_D_B2_BANK1_ROM_bist_info_value_e                              31
#define     DSP_D_B2_BANK1_ROM_bist_info_value_SHIFT                          0
#define     DSP_D_B2_BANK1_ROM_bist_info_value_MASK                           (0xFFFFFFFF<<0)

#define     DSP_D_B2_BANK2_ROM_bist_info_value_e                              31
#define     DSP_D_B2_BANK2_ROM_bist_info_value_SHIFT                          0
#define     DSP_D_B2_BANK2_ROM_bist_info_value_MASK                           (0xFFFFFFFF<<0)

#define     DSP_D_B2_BANK3_ROM_bist_info_value_e                              31
#define     DSP_D_B2_BANK3_ROM_bist_info_value_SHIFT                          0
#define     DSP_D_B2_BANK3_ROM_bist_info_value_MASK                           (0xFFFFFFFF<<0)

//--------------DMAController-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     DMAController_BASE                                                0xC00C0000
#define     DMAPriority                                                       (DMAController_BASE+0x00000000)
#define     DMAIP                                                             (DMAController_BASE+0x00000004)
#define     DMAIE                                                             (DMAController_BASE+0x00000008)
#define     DMADBG                                                            (DMAController_BASE+0x0000000c)
#define     DMA0CTL                                                           (DMAController_BASE+0x00000010)
#define     DMA0SADDR0                                                        (DMAController_BASE+0x00000014)
#define     DMA0SADDR1                                                        (DMAController_BASE+0x00000018)
#define     DMA0DADDR0                                                        (DMAController_BASE+0x0000001c)
#define     DMA0DADDR1                                                        (DMAController_BASE+0x00000020)
#define     DMA0FrameLen                                                      (DMAController_BASE+0x00000024)
#define     DMA1CTL                                                           (DMAController_BASE+0x00000028)
#define     DMA1SADDR0                                                        (DMAController_BASE+0x0000002c)
#define     DMA1SADDR1                                                        (DMAController_BASE+0x00000030)
#define     DMA1DADDR0                                                        (DMAController_BASE+0x00000034)
#define     DMA1DADDR1                                                        (DMAController_BASE+0x00000038)
#define     DMA1FrameLen                                                      (DMAController_BASE+0x0000003c)
#define     DMA2CTL                                                           (DMAController_BASE+0x00000040)
#define     DMA2SADDR0                                                        (DMAController_BASE+0x00000044)
#define     DMA2SADDR1                                                        (DMAController_BASE+0x00000048)
#define     DMA2DADDR0                                                        (DMAController_BASE+0x0000004c)
#define     DMA2DADDR1                                                        (DMAController_BASE+0x00000050)
#define     DMA2FrameLen                                                      (DMAController_BASE+0x00000054)
#define     DMA3CTL                                                           (DMAController_BASE+0x00000058)
#define     DMA3SADDR0                                                        (DMAController_BASE+0x0000005c)
#define     DMA3SADDR1                                                        (DMAController_BASE+0x00000060)
#define     DMA3DADDR0                                                        (DMAController_BASE+0x00000064)
#define     DMA3DADDR1                                                        (DMAController_BASE+0x00000068)
#define     DMA3FrameLen                                                      (DMAController_BASE+0x0000006c)
#define     DMA4CTL                                                           (DMAController_BASE+0x00000070)
#define     DMA4SADDR0                                                        (DMAController_BASE+0x00000074)
#define     DMA4SADDR1                                                        (DMAController_BASE+0x00000078)
#define     DMA4DADDR0                                                        (DMAController_BASE+0x0000007c)
#define     DMA4DADDR1                                                        (DMAController_BASE+0x00000080)
#define     DMA4FrameLen                                                      (DMAController_BASE+0x00000084)
#define     DMA5CTL                                                           (DMAController_BASE+0x00000088)
#define     DMA5DADDR                                                         (DMAController_BASE+0x0000008c)
#define     DMA5FrameLen                                                      (DMAController_BASE+0x00000090)
#define     DMA5CONT                                                          (DMAController_BASE+0x00000094)

//--------------Bits Location------------------------------------------//
#define     DMAPriority_PriorityTAB_e                                         2
#define     DMAPriority_PriorityTAB_SHIFT                                     0
#define     DMAPriority_PriorityTAB_MASK                                      (0x7<<0)

#define     DMAIP_DMA5HFIP                                                    13
#define     DMAIP_DMA4HFIP                                                    12
#define     DMAIP_DMA3HFIP                                                    11
#define     DMAIP_DMA2HFIP                                                    10
#define     DMAIP_DMA1HFIP                                                    9
#define     DMAIP_DMA0HFIP                                                    8
#define     DMAIP_DMA5TCIP                                                    5
#define     DMAIP_DMA4TCIP                                                    4
#define     DMAIP_DMA3TCIP                                                    3
#define     DMAIP_DMA2TCIP                                                    2
#define     DMAIP_DMA1TCIP                                                    1
#define     DMAIP_DMA0TCIP                                                    0

#define     DMAIE_DMA5HFIE                                                    13
#define     DMAIE_DMA4HFIE                                                    12
#define     DMAIE_DMA3HFIE                                                    11
#define     DMAIE_DMA2HFIE                                                    10
#define     DMAIE_DMA1HFIE                                                    9
#define     DMAIE_DMA0HFIE                                                    8
#define     DMAIE_DMA5TCIE                                                    5
#define     DMAIE_DMA4TCIE                                                    4
#define     DMAIE_DMA3TCIE                                                    3
#define     DMAIE_DMA2TCIE                                                    2
#define     DMAIE_DMA1TCIE                                                    1
#define     DMAIE_DMA0TCIE                                                    0

#define     DMADBG_DMADBG_e                                                   5
#define     DMADBG_DMADBG_SHIFT                                               0
#define     DMADBG_DMADBG_MASK                                                (0x3F<<0)

#define     DMA0CTL_AUDIOTYPE                                                 16
#define     DMA0CTL_DATAWIDTH_e                                               15
#define     DMA0CTL_DATAWIDTH_SHIFT                                           14
#define     DMA0CTL_DATAWIDTH_MASK                                            (0x3<<14)
#define     DMA0CTL_DSTTYPE_e                                                 11
#define     DMA0CTL_DSTTYPE_SHIFT                                             8
#define     DMA0CTL_DSTTYPE_MASK                                              (0xF<<8)
#define     DMA0CTL_SRCTYPE_e                                                 7
#define     DMA0CTL_SRCTYPE_SHIFT                                             4
#define     DMA0CTL_SRCTYPE_MASK                                              (0xF<<4)
#define     DMA0CTL_reload                                                    1
#define     DMA0CTL_DMA0START                                                 0

#define     DMA0SADDR0_DMA0SADDR0_e                                           17
#define     DMA0SADDR0_DMA0SADDR0_SHIFT                                       0
#define     DMA0SADDR0_DMA0SADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA0SADDR1_DMA0SADDR1_e                                           17
#define     DMA0SADDR1_DMA0SADDR1_SHIFT                                       0
#define     DMA0SADDR1_DMA0SADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA0DADDR0_DMA0DADDR0_e                                           17
#define     DMA0DADDR0_DMA0DADDR0_SHIFT                                       0
#define     DMA0DADDR0_DMA0DADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA0DADDR1_DMA0DADDR1_e                                           17
#define     DMA0DADDR1_DMA0DADDR1_SHIFT                                       0
#define     DMA0DADDR1_DMA0DADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA0FrameLen_DMA0FrameLen_e                                       17
#define     DMA0FrameLen_DMA0FrameLen_SHIFT                                   0
#define     DMA0FrameLen_DMA0FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA1CTL_AUDIOTYPE                                                 16
#define     DMA1CTL_DATAWIDTH_e                                               15
#define     DMA1CTL_DATAWIDTH_SHIFT                                           14
#define     DMA1CTL_DATAWIDTH_MASK                                            (0x3<<14)
#define     DMA1CTL_DSTTYPE_e                                                 11
#define     DMA1CTL_DSTTYPE_SHIFT                                             8
#define     DMA1CTL_DSTTYPE_MASK                                              (0xF<<8)
#define     DMA1CTL_SRCTYPE_e                                                 7
#define     DMA1CTL_SRCTYPE_SHIFT                                             4
#define     DMA1CTL_SRCTYPE_MASK                                              (0xF<<4)
#define     DMA1CTL_reload                                                    1
#define     DMA1CTL_DMA1START                                                 0

#define     DMA1SADDR0_DMA1SADDR0_e                                           17
#define     DMA1SADDR0_DMA1SADDR0_SHIFT                                       0
#define     DMA1SADDR0_DMA1SADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA1SADDR1_DMA1SADDR1_e                                           17
#define     DMA1SADDR1_DMA1SADDR1_SHIFT                                       0
#define     DMA1SADDR1_DMA1SADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA1DADDR0_DMA1DADDR0_e                                           17
#define     DMA1DADDR0_DMA1DADDR0_SHIFT                                       0
#define     DMA1DADDR0_DMA1DADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA1DADDR1_DMA1DADDR1_e                                           17
#define     DMA1DADDR1_DMA1DADDR1_SHIFT                                       0
#define     DMA1DADDR1_DMA1DADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA1FrameLen_DMA1FrameLen_e                                       17
#define     DMA1FrameLen_DMA1FrameLen_SHIFT                                   0
#define     DMA1FrameLen_DMA1FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA2CTL_AUDIOTYPE                                                 16
#define     DMA2CTL_DATAWIDTH_e                                               15
#define     DMA2CTL_DATAWIDTH_SHIFT                                           14
#define     DMA2CTL_DATAWIDTH_MASK                                            (0x3<<14)
#define     DMA2CTL_DSTTYPE_e                                                 11
#define     DMA2CTL_DSTTYPE_SHIFT                                             8
#define     DMA2CTL_DSTTYPE_MASK                                              (0xF<<8)
#define     DMA2CTL_SRCTYPE_e                                                 7
#define     DMA2CTL_SRCTYPE_SHIFT                                             4
#define     DMA2CTL_SRCTYPE_MASK                                              (0xF<<4)
#define     DMA2CTL_reload                                                    1
#define     DMA2CTL_DMA2START                                                 0

#define     DMA2SADDR0_DMA2SADDR0_e                                           17
#define     DMA2SADDR0_DMA2SADDR0_SHIFT                                       0
#define     DMA2SADDR0_DMA2SADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA2SADDR1_DMA2SADDR1_e                                           17
#define     DMA2SADDR1_DMA2SADDR1_SHIFT                                       0
#define     DMA2SADDR1_DMA2SADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA2DADDR0_DMA2DADDR0_e                                           17
#define     DMA2DADDR0_DMA2DADDR0_SHIFT                                       0
#define     DMA2DADDR0_DMA2DADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA2DADDR1_DMA2DADDR1_e                                           17
#define     DMA2DADDR1_DMA2DADDR1_SHIFT                                       0
#define     DMA2DADDR1_DMA2DADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA2FrameLen_DMA2FrameLen_e                                       17
#define     DMA2FrameLen_DMA2FrameLen_SHIFT                                   0
#define     DMA2FrameLen_DMA2FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA3CTL_AUDIOTYPE                                                 16
#define     DMA3CTL_DATAWIDTH_e                                               15
#define     DMA3CTL_DATAWIDTH_SHIFT                                           14
#define     DMA3CTL_DATAWIDTH_MASK                                            (0x3<<14)
#define     DMA3CTL_DSTTYPE_e                                                 11
#define     DMA3CTL_DSTTYPE_SHIFT                                             8
#define     DMA3CTL_DSTTYPE_MASK                                              (0xF<<8)
#define     DMA3CTL_SRCTYPE_e                                                 7
#define     DMA3CTL_SRCTYPE_SHIFT                                             4
#define     DMA3CTL_SRCTYPE_MASK                                              (0xF<<4)
#define     DMA3CTL_reload                                                    1
#define     DMA3CTL_DMA3START                                                 0

#define     DMA3SADDR0_DMA3SADDR0_e                                           17
#define     DMA3SADDR0_DMA3SADDR0_SHIFT                                       0
#define     DMA3SADDR0_DMA3SADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA3SADDR1_DMA3SADDR1_e                                           17
#define     DMA3SADDR1_DMA3SADDR1_SHIFT                                       0
#define     DMA3SADDR1_DMA3SADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA3DADDR0_DMA3DADDR0_e                                           17
#define     DMA3DADDR0_DMA3DADDR0_SHIFT                                       0
#define     DMA3DADDR0_DMA3DADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA3DADDR1_DMA3DADDR1_e                                           17
#define     DMA3DADDR1_DMA3DADDR1_SHIFT                                       0
#define     DMA3DADDR1_DMA3DADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA3FrameLen_DMA3FrameLen_e                                       17
#define     DMA3FrameLen_DMA3FrameLen_SHIFT                                   0
#define     DMA3FrameLen_DMA3FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA4CTL_AUDIOTYPE                                                 16
#define     DMA4CTL_DATAWIDTH_e                                               15
#define     DMA4CTL_DATAWIDTH_SHIFT                                           14
#define     DMA4CTL_DATAWIDTH_MASK                                            (0x3<<14)
#define     DMA4CTL_DSTTYPE_e                                                 11
#define     DMA4CTL_DSTTYPE_SHIFT                                             8
#define     DMA4CTL_DSTTYPE_MASK                                              (0xF<<8)
#define     DMA4CTL_SRCTYPE_e                                                 7
#define     DMA4CTL_SRCTYPE_SHIFT                                             4
#define     DMA4CTL_SRCTYPE_MASK                                              (0xF<<4)
#define     DMA4CTL_reload                                                    1
#define     DMA4CTL_DMA4START                                                 0

#define     DMA4SADDR0_DMA4SADDR0_e                                           17
#define     DMA4SADDR0_DMA4SADDR0_SHIFT                                       0
#define     DMA4SADDR0_DMA4SADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA4SADDR1_DMA4SADDR1_e                                           17
#define     DMA4SADDR1_DMA4SADDR1_SHIFT                                       0
#define     DMA4SADDR1_DMA4SADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA4DADDR0_DMA4DADDR0_e                                           17
#define     DMA4DADDR0_DMA4DADDR0_SHIFT                                       0
#define     DMA4DADDR0_DMA4DADDR0_MASK                                        (0x3FFFF<<0)

#define     DMA4DADDR1_DMA4DADDR1_e                                           17
#define     DMA4DADDR1_DMA4DADDR1_SHIFT                                       0
#define     DMA4DADDR1_DMA4DADDR1_MASK                                        (0x3FFFF<<0)

#define     DMA4FrameLen_DMA4FrameLen_e                                       17
#define     DMA4FrameLen_DMA4FrameLen_SHIFT                                   0
#define     DMA4FrameLen_DMA4FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA5CTL_SRCTYPE                                                   4
#define     DMA5CTL_reload                                                    1
#define     DMA5CTL_DMA5START                                                 0

#define     DMA5DADDR_DMA5DADDR_e                                             17
#define     DMA5DADDR_DMA5DADDR_SHIFT                                         0
#define     DMA5DADDR_DMA5DADDR_MASK                                          (0x3FFFF<<0)

#define     DMA5FrameLen_DMA5FrameLen_e                                       17
#define     DMA5FrameLen_DMA5FrameLen_SHIFT                                   0
#define     DMA5FrameLen_DMA5FrameLen_MASK                                    (0x3FFFF<<0)

#define     DMA5CONT_DMA5CONT_e                                               17
#define     DMA5CONT_DMA5CONT_SHIFT                                           0
#define     DMA5CONT_DMA5CONT_MASK                                            (0x3FFFF<<0)

//--------------PMU-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     PMU_BASE                                                          0xc0020000
#define     VOUT_CTL                                                          (PMU_BASE+0x00)
#define     MULTI_USED                                                        (PMU_BASE+0x04)
#define     VD15_DCDC_CTL                                                     (PMU_BASE+0x08)
#define     CHG_CTL                                                           (PMU_BASE+0x0c)
#define     CHG_DET                                                           (PMU_BASE+0x10)
#define     PMUADC_CTL                                                        (PMU_BASE+0x14)
#define     BATADC_DATA                                                       (PMU_BASE+0x18)
#define     TEMPADC_DATA                                                      (PMU_BASE+0x1c)
#define     DC5VADC_DATA                                                      (PMU_BASE+0x20)
#define     SENSADC_DATA                                                      (PMU_BASE+0x24)
#define     LRADC1_DATA                                                       (PMU_BASE+0x28)
#define     LRADC2_DATA                                                       (PMU_BASE+0x2c)
#define     LRADC3_DATA                                                       (PMU_BASE+0x30)
#define     LRADC4_DATA                                                       (PMU_BASE+0x34)
#define     BDG_CTL                                                           (PMU_BASE+0x38)
#define     LDO_CTL                                                           (PMU_BASE+0x3c)
#define     SYSTEM_SET                                                        (PMU_BASE+0x40)
#define     POWER_CTL                                                         (PMU_BASE+0x44)
#define     TIMER_CTL                                                         (PMU_BASE+0x48)
#define     WKEN_CTL                                                          (PMU_BASE+0x4c)
#define     WAKE_PD                                                           (PMU_BASE+0x50)
#define     ONOFF_KEY                                                         (PMU_BASE+0x54)
#define     BIAS_CTL                                                          (PMU_BASE+0x58)
#define     NFC_CTL                                                           (PMU_BASE+0x5C)
#define     DCDC_CTL2                                                         (PMU_BASE+0X60)
#define     SPD_CTL                                                           (PMU_BASE+0X64)

//--------------Bits Location------------------------------------------//
#define     VOUT_CTL_TK_IBIAS                                                 19
#define     VOUT_CTL_AVDD_PD                                                  18
#define     VOUT_CTL_AVDD_VOL_e                                               17
#define     VOUT_CTL_AVDD_VOL_SHIFT                                           16
#define     VOUT_CTL_AVDD_VOL_MASK                                            (0x3<<16)
#define     VOUT_CTL_AVCC_BIASEN                                              14
#define     VOUT_CTL_AVCC_DROP_e                                              13
#define     VOUT_CTL_AVCC_DROP_SHIFT                                          12
#define     VOUT_CTL_AVCC_DROP_MASK                                           (0x3<<12)
#define     VOUT_CTL_VCCOC_SET                                                9
#define     VOUT_CTL_VDDOC_SET                                                8
#define     VOUT_CTL_VCC_SET_e                                                6
#define     VOUT_CTL_VCC_SET_SHIFT                                            4
#define     VOUT_CTL_VCC_SET_MASK                                             (0x7<<4)
#define     VOUT_CTL_VDD_SET_e                                                3
#define     VOUT_CTL_VDD_SET_SHIFT                                            0
#define     VOUT_CTL_VDD_SET_MASK                                             (0xF<<0)

#define     MULTI_USED_UVDD_EN                                                8
#define     MULTI_USED_UVDD_V_e                                               7
#define     MULTI_USED_UVDD_V_SHIFT                                           5
#define     MULTI_USED_UVDD_V_MASK                                            (0x7<<5)
#define     MULTI_USED_USBVDD_PD                                              4
#define     MULTI_USED_USBVDD_PD2                                             3
#define     MULTI_USED_USBVDD_PD3                                             2
#define     MULTI_USED_SEG_DISP_VCC_EN                                        1
#define     MULTI_USED_SEG_LED_EN                                             0

#define     VD15_DCDC_CTL_VD15_MODE_S3BT_e                                    31
#define     VD15_DCDC_CTL_VD15_MODE_S3BT_SHIFT                                30
#define     VD15_DCDC_CTL_VD15_MODE_S3BT_MASK                                 (0x3<<30)
#define     VD15_DCDC_CTL_FREQ_SOURCE                                         24
#define     VD15_DCDC_CTL_CLAMP_EN                                            23
#define     VD15_DCDC_CTL_DC_DBSEL                                            22
#define     VD15_DCDC_CTL_VISO_SEL                                            21
#define     VD15_DCDC_CTL_PDT_e                                               20
#define     VD15_DCDC_CTL_PDT_SHIFT                                           19
#define     VD15_DCDC_CTL_PDT_MASK                                            (0x3<<19)
#define     VD15_DCDC_CTL_NDT_e                                               18
#define     VD15_DCDC_CTL_NDT_SHIFT                                           17
#define     VD15_DCDC_CTL_NDT_MASK                                            (0x3<<17)
#define     VD15_DCDC_CTL_Anti_aduio_e                                        16
#define     VD15_DCDC_CTL_Anti_aduio_SHIFT                                    15
#define     VD15_DCDC_CTL_Anti_aduio_MASK                                     (0x3<<15)
#define     VD15_DCDC_CTL_ZERO_IBSEL_EN                                       13
#define     VD15_DCDC_CTL_VD15_MODE_S1_e                                      12
#define     VD15_DCDC_CTL_VD15_MODE_S1_SHIFT                                  11
#define     VD15_DCDC_CTL_VD15_MODE_S1_MASK                                   (0x3<<11)
#define     VD15_DCDC_CTL_ZERO_DETEN                                          10
#define     VD15_DCDC_CTL_ZERO_CTL_e                                          9
#define     VD15_DCDC_CTL_ZERO_CTL_SHIFT                                      8
#define     VD15_DCDC_CTL_ZERO_CTL_MASK                                       (0x3<<8)
#define     VD15_DCDC_CTL_DCDC_MODU_e                                         7
#define     VD15_DCDC_CTL_DCDC_MODU_SHIFT                                     6
#define     VD15_DCDC_CTL_DCDC_MODU_MASK                                      (0x3<<6)
#define     VD15_DCDC_CTL_DCDC_MAX_I_e                                        5
#define     VD15_DCDC_CTL_DCDC_MAX_I_SHIFT                                    4
#define     VD15_DCDC_CTL_DCDC_MAX_I_MASK                                     (0x3<<4)
#define     VD15_DCDC_CTL_DCDC_FS_e                                           3
#define     VD15_DCDC_CTL_DCDC_FS_SHIFT                                       1
#define     VD15_DCDC_CTL_DCDC_FS_MASK                                        (0x7<<1)
#define     VD15_DCDC_CTL_PFM_FS                                              0

#define     CHG_CTL_UVLO_T                                                    20
#define     CHG_CTL_DC5VOV_EN                                                 19
#define     CHG_CTL_DC5VOV_SET_e                                              18
#define     CHG_CTL_DC5VOV_SET_SHIFT                                          17
#define     CHG_CTL_DC5VOV_SET_MASK                                           (0x3<<17)
#define     CHG_CTL_CHGOP_SEL                                                 16
#define     CHG_CTL_CHGEN                                                     15
#define     CHG_CTL_ENTKLE                                                    14
#define     CHG_CTL_CHG_CURRENT_e                                             13
#define     CHG_CTL_CHG_CURRENT_SHIFT                                         11
#define     CHG_CTL_CHG_CURRENT_MASK                                          (0x7<<11)
#define     CHG_CTL_ENBATDT                                                   10
#define     CHG_CTL_ENFASTCHG_e                                               9
#define     CHG_CTL_ENFASTCHG_SHIFT                                           7
#define     CHG_CTL_ENFASTCHG_MASK                                            (0x7<<7)
#define     CHG_CTL_STOPV_e                                                   6
#define     CHG_CTL_STOPV_SHIFT                                               5
#define     CHG_CTL_STOPV_MASK                                                (0x3<<5)
#define     CHG_CTL_ENSAMP                                                    4
#define     CHG_CTL_STDY_SET_e                                                3
#define     CHG_CTL_STDY_SET_SHIFT                                            2
#define     CHG_CTL_STDY_SET_MASK                                             (0x3<<2)
#define     CHG_CTL_ENCHGATDT                                                 1
#define     CHG_CTL_DTSEL                                                     0

#define     CHG_DET_DC5VOV_DET                                                9
#define     CHG_DET_UVLO                                                      7
#define     CHG_DET_CHGPHASE_e                                                6
#define     CHG_DET_CHGPHASE_SHIFT                                            5
#define     CHG_DET_CHGPHASE_MASK                                             (0x3<<5)
#define     CHG_DET_CHG_STA0                                                  4
#define     CHG_DET_CHG_STA1                                                  3
#define     CHG_DET_CHGEND                                                    2
#define     CHG_DET_BATEXT                                                    1
#define     CHG_DET_DTOVER                                                    0

#define     PMUADC_CTL_BATADC_FS                                              7
#define     PMUADC_CTL_LRADC_FS                                               6
#define     PMUADC_CTL_LRADC234_EN                                            5
#define     PMUADC_CTL_LRADC1_EN                                              4
#define     PMUADC_CTL_SENSORADC_EN                                           3
#define     PMUADC_CTL_DC5VADC_EN                                             2
#define     PMUADC_CTL_TEMPADC_EN                                             1
#define     PMUADC_CTL_BATADC_EN                                              0

#define     BATADC_DATA_BATADC_e                                              6
#define     BATADC_DATA_BATADC_SHIFT                                          0
#define     BATADC_DATA_BATADC_MASK                                           (0x7F<<0)

#define     TEMPADC_DATA_TEMPADC_e                                            6
#define     TEMPADC_DATA_TEMPADC_SHIFT                                        0
#define     TEMPADC_DATA_TEMPADC_MASK                                         (0x7F<<0)

#define     DC5VADC_DATA_DC5VADC_e                                            6
#define     DC5VADC_DATA_DC5VADC_SHIFT                                        0
#define     DC5VADC_DATA_DC5VADC_MASK                                         (0x7F<<0)

#define     SENSADC_DATA_SENSADC_e                                            6
#define     SENSADC_DATA_SENSADC_SHIFT                                        0
#define     SENSADC_DATA_SENSADC_MASK                                         (0x7F<<0)

#define     LRADC1_DATA_LRADC1_e                                              6
#define     LRADC1_DATA_LRADC1_SHIFT                                          0
#define     LRADC1_DATA_LRADC1_MASK                                           (0x7F<<0)

#define     LRADC2_DATA_LRADC2_e                                              6
#define     LRADC2_DATA_LRADC2_SHIFT                                          0
#define     LRADC2_DATA_LRADC2_MASK                                           (0x7F<<0)

#define     LRADC3_DATA_LRADC3_e                                              6
#define     LRADC3_DATA_LRADC3_SHIFT                                          0
#define     LRADC3_DATA_LRADC3_MASK                                           (0x7F<<0)

#define     LRADC4_DATA_LRADC4_e                                              6
#define     LRADC4_DATA_LRADC4_SHIFT                                          0
#define     LRADC4_DATA_LRADC4_MASK                                           (0x7F<<0)

#define     BDG_CTL_BDG_EN                                                    8
#define     BDG_CTL_BDG_FILTER                                                6
#define     BDG_CTL_BDG_PDR                                                   5
#define     BDG_CTL_BDG_VOL_e                                                 4
#define     BDG_CTL_BDG_VOL_SHIFT                                             0
#define     BDG_CTL_BDG_VOL_MASK                                              (0x1F<<0)

#define     LDO_CTL_BTVDD_BIAS                                                23
#define     LDO_CTL_OSCVDD_BIAS                                               22
#define     LDO_CTL_OSCVDD_PD                                                 21
#define     LDO_CTL_OSCVDD_SET_e                                              20
#define     LDO_CTL_OSCVDD_SET_SHIFT                                          19
#define     LDO_CTL_OSCVDD_SET_MASK                                           (0x3<<19)
#define     LDO_CTL_VD15OC_SET                                                18
#define     LDO_CTL_VD15_SET_e                                                17
#define     LDO_CTL_VD15_SET_SHIFT                                            14
#define     LDO_CTL_VD15_SET_MASK                                             (0xF<<14)
#define     LDO_CTL_BTVDD_PD                                                  13
#define     LDO_CTL_BTVDD_EN                                                  12
#define     LDO_CTL_BTVDD_VOL_e                                               11
#define     LDO_CTL_BTVDD_VOL_SHIFT                                           8
#define     LDO_CTL_BTVDD_VOL_MASK                                            (0xF<<8)
#define     LDO_CTL_SVCC_SET_e                                                7
#define     LDO_CTL_SVCC_SET_SHIFT                                            5
#define     LDO_CTL_SVCC_SET_MASK                                             (0x7<<5)
#define     LDO_CTL_BTVCCOC_SET                                               4
#define     LDO_CTL_BTVCC_VOL_e                                               3
#define     LDO_CTL_BTVCC_VOL_SHIFT                                           1
#define     LDO_CTL_BTVCC_VOL_MASK                                            (0x7<<1)
#define     LDO_CTL_BTVCC_EN                                                  0

#define     SYSTEM_SET_WVLOWPD_T_e                                            10
#define     SYSTEM_SET_WVLOWPD_T_SHIFT                                        9
#define     SYSTEM_SET_WVLOWPD_T_MASK                                         (0x3<<9)
#define     SYSTEM_SET_VOLT_RESIST                                            8
#define     SYSTEM_SET_LB_EN                                                  7
#define     SYSTEM_SET_LB_VOL_e                                               6
#define     SYSTEM_SET_LB_VOL_SHIFT                                           5
#define     SYSTEM_SET_LB_VOL_MASK                                            (0x3<<5)
#define     SYSTEM_SET_OC_EN                                                  4
#define     SYSTEM_SET_LVPRO_EN                                               3
#define     SYSTEM_SET_SVCC_LVPRO_EN                                          2
#define     SYSTEM_SET_EN_FASTOFF                                             1
#define     SYSTEM_SET_EN_FASTON                                              0

#define     POWER_CTL_EN_S3                                                   2
#define     POWER_CTL_EN_S3BT                                                 1
#define     POWER_CTL_EN_S1                                                   0

#define     TIMER_CTL_S3_timer_EN                                             21
#define     TIMER_CTL_S3timer                                                 20
#define     TIMER_CTL_S3BT_ON_EN                                              15
#define     TIMER_CTL_S3BT_ON_TIMER_e                                         14
#define     TIMER_CTL_S3BT_ON_TIMER_SHIFT                                     8
#define     TIMER_CTL_S3BT_ON_TIMER_MASK                                      (0x7F<<8)
#define     TIMER_CTL_S3BT_OFF_EN                                             7
#define     TIMER_CTL_S3BT_OFF_TIMER_e                                        6
#define     TIMER_CTL_S3BT_OFF_TIMER_SHIFT                                    0
#define     TIMER_CTL_S3BT_OFF_TIMER_MASK                                     (0x7F<<0)

#define     WKEN_CTL_BATWK_EN                                                 10
#define     WKEN_CTL_REMOTE_WKEN                                              9
#define     WKEN_CTL_HDSW_BLOCK                                               8
#define     WKEN_CTL_HDSWOFF_EN                                               7
#define     WKEN_CTL_UVLO_SEL                                                 6
#define     WKEN_CTL_BT_WK_EN                                                 5
#define     WKEN_CTL_NFC_WK_EN                                                4
#define     WKEN_CTL_RESET_WKEN                                               3
#define     WKEN_CTL_SHORT_WKEN                                               2
#define     WKEN_CTL_LONG_WKEN                                                1
#define     WKEN_CTL_HDSW_WKEN                                                0

#define     WAKE_PD_BATIN_PD                                                  8
#define     WAKE_PD_REMOTE_PD                                                 7
#define     WAKE_PD_LONG_PLAY                                                 6
#define     WAKE_PD_S3BT_TON_PD                                               5
#define     WAKE_PD_HDSWOFF_PD                                                4
#define     WAKE_PD_HDSWON_PD                                                 3
#define     WAKE_PD_ONOFF_PD                                                  2
#define     WAKE_PD_NFC_PD                                                    1
#define     WAKE_PD_BT_PD                                                     0

#define     ONOFF_KEY_KEY_SPEED                                               15
#define     ONOFF_KEY_P_ONOFF_SEL                                             11
#define     ONOFF_KEY_RESTART_SET                                             10
#define     ONOFF_KEY_ONOFF_PRESS_TIME_e                                      9
#define     ONOFF_KEY_ONOFF_PRESS_TIME_SHIFT                                  7
#define     ONOFF_KEY_ONOFF_PRESS_TIME_MASK                                   (0x7<<7)
#define     ONOFF_KEY_ONOFF_RST_EN                                            6
#define     ONOFF_KEY_ONOFF_RST_T_SEL_e                                       5
#define     ONOFF_KEY_ONOFF_RST_T_SEL_SHIFT                                   4
#define     ONOFF_KEY_ONOFF_RST_T_SEL_MASK                                    (0x3<<4)
#define     ONOFF_KEY_ONOFF_STATE                                             3
#define     ONOFF_KEY_HDSWOFF_2_3                                             2
#define     ONOFF_KEY_HDSWON_1_3                                              1
#define     ONOFF_KEY_ONOFF_PRESS_0                                           0

#define     BIAS_CTL_SIM_A_EN                                                 2
#define     BIAS_CTL_PMU_BIAS_e                                               1
#define     BIAS_CTL_PMU_BIAS_SHIFT                                           0
#define     BIAS_CTL_PMU_BIAS_MASK                                            (0x3<<0)

#define     NFC_CTL_BT_WAKE_DET                                               8
#define     NFC_CTL_NFC_DET                                                   7
#define     NFC_CTL_NFCPU_CTL                                                 6
#define     NFC_CTL_NFCPD_CTL                                                 4
#define     NFC_CTL_NFCDT_SET_e                                               2
#define     NFC_CTL_NFCDT_SET_SHIFT                                           1
#define     NFC_CTL_NFCDT_SET_MASK                                            (0x3<<1)
#define     NFC_CTL_NFCTM_SET                                                 0

#define     DCDC_CTL2_PWM_OV_SEL                                              14
#define     DCDC_CTL2_PFM_RP_SEL_e                                            13
#define     DCDC_CTL2_PFM_RP_SEL_SHIFT                                        11
#define     DCDC_CTL2_PFM_RP_SEL_MASK                                         (0x7<<11)
#define     DCDC_CTL2_PWM_RP_SEL                                              10
#define     DCDC_CTL2_DC_CNSL_e                                               9
#define     DCDC_CTL2_DC_CNSL_SHIFT                                           8
#define     DCDC_CTL2_DC_CNSL_MASK                                            (0x3<<8)
#define     DCDC_CTL2_SENSOR_IB                                               7
#define     DCDC_CTL2_EN_S3SVCC                                               5
#define     DCDC_CTL2_SYSOV_IB                                                4
#define     DCDC_CTL2_ZERO_SYSDL                                              3
#define     DCDC_CTL2_ZERO_IBSF                                               2
#define     DCDC_CTL2_EN_PFMRP                                                0

#define     SPD_CTL_DC5V_SYS_VOL_e                                            8
#define     SPD_CTL_DC5V_SYS_VOL_SHIFT                                        7
#define     SPD_CTL_DC5V_SYS_VOL_MASK                                         (0x3<<7)
#define     SPD_CTL_BAT_SYS_MODE                                              6
#define     SPD_CTL_BTVCCPD                                                   5
#define     SPD_CTL_SVCCPD                                                    4
#define     SPD_CTL_AVCCPD                                                    3
#define     SPD_CTL_VD15PD                                                    2
#define     SPD_CTL_VDDPD                                                     1
#define     SPD_CTL_VCCPD                                                     0

//--------------EFUSE-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     EFUSE_BASE                                                        0xc0010000
#define     EFUSE_CTL0                                                        (EFUSE_BASE+0x00)
#define     EFUSE_CTL1                                                        (EFUSE_BASE+0x04)
#define     EFUSE_CTL2                                                        (EFUSE_BASE+0x08)
#define     EFUSE_DATA0                                                       (EFUSE_BASE+0x0C)
#define     EFUSE_DATA1                                                       (EFUSE_BASE+0x10)
#define     EFUSE_DATA2                                                       (EFUSE_BASE+0x14)
#define     EFUSE_DATA3                                                       (EFUSE_BASE+0x18)

//--------------Bits Location------------------------------------------//
#define     EFUSE_CTL0_CHIPID_CLKD_e                                          5
#define     EFUSE_CTL0_CHIPID_CLKD_SHIFT                                      3
#define     EFUSE_CTL0_CHIPID_CLKD_MASK                                       (0x7<<3)
#define     EFUSE_CTL0_RE_LOAD                                                2
#define     EFUSE_CTL0_RD_PSD_OK                                              1
#define     EFUSE_CTL0_DATA_READY                                             0

#define     EFUSE_CTL1_EFUSE25_VOL_e                                          22
#define     EFUSE_CTL1_EFUSE25_VOL_SHIFT                                      20
#define     EFUSE_CTL1_EFUSE25_VOL_MASK                                       (0x7<<20)
#define     EFUSE_CTL1_PGM_TIMER_e                                            19
#define     EFUSE_CTL1_PGM_TIMER_SHIFT                                        17
#define     EFUSE_CTL1_PGM_TIMER_MASK                                         (0x7<<17)
#define     EFUSE_CTL1_PGM_AUTO_EN                                            16
#define     EFUSE_CTL1_EFUSE25_PD_e                                           15
#define     EFUSE_CTL1_EFUSE25_PD_SHIFT                                       14
#define     EFUSE_CTL1_EFUSE25_PD_MASK                                        (0x3<<14)
#define     EFUSE_CTL1_EN_EFUSE25                                             11
#define     EFUSE_CTL1_WR_PSD_OK                                              10
#define     EFUSE_CTL1_AEN                                                    9
#define     EFUSE_CTL1_PGMEN                                                  8
#define     EFUSE_CTL1_RDEN                                                   7
#define     EFUSE_CTL1_ADDRESS_e                                              6
#define     EFUSE_CTL1_ADDRESS_SHIFT                                          0
#define     EFUSE_CTL1_ADDRESS_MASK                                           (0x7F<<0)

#define     EFUSE_CTL2_Password_e                                             31
#define     EFUSE_CTL2_Password_SHIFT                                         0
#define     EFUSE_CTL2_Password_MASK                                          (0xFFFFFFFF<<0)

#define     EFUSE_DATA0_EFSUE_DATA0_e                                         31
#define     EFUSE_DATA0_EFSUE_DATA0_SHIFT                                     0
#define     EFUSE_DATA0_EFSUE_DATA0_MASK                                      (0xFFFFFFFF<<0)

#define     EFUSE_DATA1_EFSUE_DATA1_e                                         31
#define     EFUSE_DATA1_EFSUE_DATA1_SHIFT                                     0
#define     EFUSE_DATA1_EFSUE_DATA1_MASK                                      (0xFFFFFFFF<<0)

#define     EFUSE_DATA2_EFSUE_DATA2_e                                         31
#define     EFUSE_DATA2_EFSUE_DATA2_SHIFT                                     0
#define     EFUSE_DATA2_EFSUE_DATA2_MASK                                      (0xFFFFFFFF<<0)

#define     EFUSE_DATA3_EFSUE_DATA3_e                                         31
#define     EFUSE_DATA3_EFSUE_DATA3_SHIFT                                     0
#define     EFUSE_DATA3_EFSUE_DATA3_MASK                                      (0xFFFFFFFF<<0)

//--------------ChipVersion-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     ChipVersion_BASE                                                  0xc00000a0
#define     ChipVersion                                                       (ChipVersion_BASE+0x00000000)

//--------------Bits Location------------------------------------------//
#define     ChipVersion_ChipVersion_e                                         3
#define     ChipVersion_ChipVersion_SHIFT                                     0
#define     ChipVersion_ChipVersion_MASK                                      (0xF<<0)

//--------------RMU_digital-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     RMU_digital_BASE                                                  0xc0000000
#define     MRCR                                                              (RMU_digital_BASE+0x00000000)
#define     DSP_VCT_ADDR                                                      (RMU_digital_BASE+0x00000008)

//--------------Bits Location------------------------------------------//
#define     MRCR_PCMReset                                                     20
#define     MRCR_UART1Reset                                                   17
#define     MRCR_USBReset2                                                    16
#define     MRCR_BISTReset                                                    15
#define     MRCR_SEGLCDReset                                                  14
#define     MRCR_TouchKeyReset                                                13
#define     MRCR_PWM_LIGHT_Reset                                              12
#define     MRCR_AudioIOReset                                                 11
#define     MRCR_SPI1Reset                                                    10
#define     MRCR_USBReset                                                     9
#define     MRCR_ASRCReset                                                    8
#define     MRCR_LCDReset                                                     7
#define     MRCR_SPI0Reset                                                    6
#define     MRCR_I2CReset                                                     5
#define     MRCR_UART0Reset                                                   4
#define     MRCR_SDReset                                                      3
#define     MRCR_DSP_PART                                                     2
#define     MRCR_DSP_ALL                                                      1
#define     MRCR_DMA012345Reset                                               0

#define     DSP_VCT_ADDR_DSP_VECTOR_ADDRESS_e                                 31
#define     DSP_VCT_ADDR_DSP_VECTOR_ADDRESS_SHIFT                             0
#define     DSP_VCT_ADDR_DSP_VECTOR_ADDRESS_MASK                              (0xFFFFFFFF<<0)

//--------------CMU_Analog_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     CMU_Analog_Register_BASE                                          0xC0000100
#define     HOSC_CTL                                                          (CMU_Analog_Register_BASE+0x00)
#define     HCL_CTL                                                           (CMU_Analog_Register_BASE+0x04)
#define     _24MPLL_CTL                                                       (CMU_Analog_Register_BASE+0x08)
#define     CORE_PLL_CTL                                                      (CMU_Analog_Register_BASE+0x0c)
#define     AUDIO_PLL_CTL                                                     (CMU_Analog_Register_BASE+0x10)
#define     CORE_PLL_DEBUG                                                    (CMU_Analog_Register_BASE+0x14)
#define     AUDIO_PLL_DEBUG                                                   (CMU_Analog_Register_BASE+0x18)
#define     _24MPLL_DEBUG                                                     (CMU_Analog_Register_BASE+0x1c)
#define     CMU_DEBUG_CTL                                                     (CMU_Analog_Register_BASE+0x20)

//--------------Bits Location------------------------------------------//
#define     HOSC_CTL_BT_HOSC_SEL                                              22
#define     HOSC_CTL_VDD_HOSC_SEL                                             21
#define     HOSC_CTL_HGMC_e                                                   20
#define     HOSC_CTL_HGMC_SHIFT                                               16
#define     HOSC_CTL_HGMC_MASK                                                (0x1F<<16)
#define     HOSC_CTL_HOSCI_BC_SEL_e                                           15
#define     HOSC_CTL_HOSCI_BC_SEL_SHIFT                                       13
#define     HOSC_CTL_HOSCI_BC_SEL_MASK                                        (0x7<<13)
#define     HOSC_CTL_HOSCI_TC_SEL_e                                           12
#define     HOSC_CTL_HOSCI_TC_SEL_SHIFT                                       8
#define     HOSC_CTL_HOSCI_TC_SEL_MASK                                        (0x1F<<8)
#define     HOSC_CTL_HOSCO_BC_SEL_e                                           7
#define     HOSC_CTL_HOSCO_BC_SEL_SHIFT                                       5
#define     HOSC_CTL_HOSCO_BC_SEL_MASK                                        (0x7<<5)
#define     HOSC_CTL_HOSCO_TC_SEL_e                                           4
#define     HOSC_CTL_HOSCO_TC_SEL_SHIFT                                       0
#define     HOSC_CTL_HOSCO_TC_SEL_MASK                                        (0x1F<<0)

#define     HCL_CTL_HCL_CCLK_SEL                                              11
#define     HCL_CTL_HCL_INTERVAL_e                                            10
#define     HCL_CTL_HCL_INTERVAL_SHIFT                                        8
#define     HCL_CTL_HCL_INTERVAL_MASK                                         (0x7<<8)
#define     HCL_CTL_HOSC_OSC_TIME_e                                           7
#define     HCL_CTL_HOSC_OSC_TIME_SHIFT                                       6
#define     HCL_CTL_HOSC_OSC_TIME_MASK                                        (0x3<<6)
#define     HCL_CTL_CAL_DELAY_TIME_e                                          5
#define     HCL_CTL_CAL_DELAY_TIME_SHIFT                                      4
#define     HCL_CTL_CAL_DELAY_TIME_MASK                                       (0x3<<4)
#define     HCL_CTL_HCL_PRECISION_e                                           3
#define     HCL_CTL_HCL_PRECISION_SHIFT                                       2
#define     HCL_CTL_HCL_PRECISION_MASK                                        (0x3<<2)
#define     HCL_CTL_HOSC_4HZ_DIV_EN                                           1
#define     HCL_CTL_HOSC_CAL_EN                                               0

#define     _24MPLL_CTL_HOSC_EN                                               3
#define     _24MPLL_CTL_24MPLL_PMD                                            1
#define     _24MPLL_CTL_24MPLL_EN                                             0

#define     CORE_PLL_CTL_CORE_PLL_PMD                                         9
#define     CORE_PLL_CTL_CORE_PLL_SCLK_SEL                                    8
#define     CORE_PLL_CTL_CORE_PLL_EN                                          7
#define     CORE_PLL_CTL_SCORE_e                                              6
#define     CORE_PLL_CTL_SCORE_SHIFT                                          0
#define     CORE_PLL_CTL_SCORE_MASK                                           (0x7F<<0)

#define     AUDIO_PLL_CTL_AUDIO_PLL_DIV_SEL                                   7
#define     AUDIO_PLL_CTL_AUDIO_PLL_PMD                                       6
#define     AUDIO_PLL_CTL_AUDPLL_SCLK_SEL                                     5
#define     AUDIO_PLL_CTL_AUDPLLEN                                            4
#define     AUDIO_PLL_CTL_APS_e                                               3
#define     AUDIO_PLL_CTL_APS_SHIFT                                           0
#define     AUDIO_PLL_CTL_APS_MASK                                            (0xF<<0)

#define     CORE_PLL_DEBUG_CORE_PLL_PU_Force                                  31
#define     CORE_PLL_DEBUG_CORE_PLL_SET_forcevc_e                             30
#define     CORE_PLL_DEBUG_CORE_PLL_SET_forcevc_SHIFT                         29
#define     CORE_PLL_DEBUG_CORE_PLL_SET_forcevc_MASK                          (0x3<<29)
#define     CORE_PLL_DEBUG_IB_CORE_PLL_e                                      28
#define     CORE_PLL_DEBUG_IB_CORE_PLL_SHIFT                                  27
#define     CORE_PLL_DEBUG_IB_CORE_PLL_MASK                                   (0x3<<27)
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_CS_e                                  26
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_CS_SHIFT                              25
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_CS_MASK                               (0x3<<25)
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_RS_e                                  24
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_RS_SHIFT                              23
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_RS_MASK                               (0x3<<23)
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_ICPP_e                                22
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_ICPP_SHIFT                            21
#define     CORE_PLL_DEBUG_Reg_CORE_PLL_ICPP_MASK                             (0x3<<21)
#define     CORE_PLL_DEBUG_REG_CORE_PLL_Fband_e                               20
#define     CORE_PLL_DEBUG_REG_CORE_PLL_Fband_SHIFT                           19
#define     CORE_PLL_DEBUG_REG_CORE_PLL_Fband_MASK                            (0x3<<19)
#define     CORE_PLL_DEBUG_TESTSEL_e                                          18
#define     CORE_PLL_DEBUG_TESTSEL_SHIFT                                      17
#define     CORE_PLL_DEBUG_TESTSEL_MASK                                       (0x3<<17)
#define     CORE_PLL_DEBUG_TEST_EN                                            16
#define     CORE_PLL_DEBUG_CORE_PLL_Bypass                                    15
#define     CORE_PLL_DEBUG_SSC_EN                                             14
#define     CORE_PLL_DEBUG_SSC_DIV                                            13
#define     CORE_PLL_DEBUG_SDM_ORDER                                          12
#define     CORE_PLL_DEBUG_HNUM_e                                             11
#define     CORE_PLL_DEBUG_HNUM_SHIFT                                         9
#define     CORE_PLL_DEBUG_HNUM_MASK                                          (0x7<<9)
#define     CORE_PLL_DEBUG_PSTEP_e                                            8
#define     CORE_PLL_DEBUG_PSTEP_SHIFT                                        7
#define     CORE_PLL_DEBUG_PSTEP_MASK                                         (0x3<<7)
#define     CORE_PLL_DEBUG_FSTEP_e                                            6
#define     CORE_PLL_DEBUG_FSTEP_SHIFT                                        1
#define     CORE_PLL_DEBUG_FSTEP_MASK                                         (0x3F<<1)
#define     CORE_PLL_DEBUG_FCODE_EXT_EN                                       0

#define     AUDIO_PLL_DEBUG_REG_COREPLL_ICPI_e                                23
#define     AUDIO_PLL_DEBUG_REG_COREPLL_ICPI_SHIFT                            22
#define     AUDIO_PLL_DEBUG_REG_COREPLL_ICPI_MASK                             (0x3<<22)
#define     AUDIO_PLL_DEBUG_REG_AUDPLL_ICPI_e                                 21
#define     AUDIO_PLL_DEBUG_REG_AUDPLL_ICPI_SHIFT                             20
#define     AUDIO_PLL_DEBUG_REG_AUDPLL_ICPI_MASK                              (0x3<<20)
#define     AUDIO_PLL_DEBUG_REG_24MPLL_ICPI_e                                 19
#define     AUDIO_PLL_DEBUG_REG_24MPLL_ICPI_SHIFT                             18
#define     AUDIO_PLL_DEBUG_REG_24MPLL_ICPI_MASK                              (0x3<<18)
#define     AUDIO_PLL_DEBUG_AUDPLL_BYPASS                                     16
#define     AUDIO_PLL_DEBUG_AUDPLL_PU_Force                                   15
#define     AUDIO_PLL_DEBUG_AUDPLL_SET_forcevc_e                              14
#define     AUDIO_PLL_DEBUG_AUDPLL_SET_forcevc_SHIFT                          13
#define     AUDIO_PLL_DEBUG_AUDPLL_SET_forcevc_MASK                           (0x3<<13)
#define     AUDIO_PLL_DEBUG_IB_AUDPLL_e                                       12
#define     AUDIO_PLL_DEBUG_IB_AUDPLL_SHIFT                                   11
#define     AUDIO_PLL_DEBUG_IB_AUDPLL_MASK                                    (0x3<<11)
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_CS_e                                   10
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_CS_SHIFT                               9
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_CS_MASK                                (0x3<<9)
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_RS_e                                   8
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_RS_SHIFT                               7
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_RS_MASK                                (0x3<<7)
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_ICPP_e                                 6
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_ICPP_SHIFT                             5
#define     AUDIO_PLL_DEBUG_Reg_AUDPLL_ICPP_MASK                              (0x3<<5)
#define     AUDIO_PLL_DEBUG_CORE_PLL_R3_e                                     4
#define     AUDIO_PLL_DEBUG_CORE_PLL_R3_SHIFT                                 3
#define     AUDIO_PLL_DEBUG_CORE_PLL_R3_MASK                                  (0x3<<3)
#define     AUDIO_PLL_DEBUG_TESTSEL_e                                         2
#define     AUDIO_PLL_DEBUG_TESTSEL_SHIFT                                     1
#define     AUDIO_PLL_DEBUG_TESTSEL_MASK                                      (0x3<<1)
#define     AUDIO_PLL_DEBUG_TEST_EN                                           0

#define     _24MPLL_DEBUG_24MPLL_LPF_SEL                                      18
#define     _24MPLL_DEBUG_24MPLL_BYPASS                                       17
#define     _24MPLL_DEBUG_24MPLL_DIV_SEL                                      16
#define     _24MPLL_DEBUG_24MPLL_PU_Force                                     15
#define     _24MPLL_DEBUG_24MPLL_SET_forcevc_e                                14
#define     _24MPLL_DEBUG_24MPLL_SET_forcevc_SHIFT                            13
#define     _24MPLL_DEBUG_24MPLL_SET_forcevc_MASK                             (0x3<<13)
#define     _24MPLL_DEBUG_IB_24MPLL_e                                         12
#define     _24MPLL_DEBUG_IB_24MPLL_SHIFT                                     11
#define     _24MPLL_DEBUG_IB_24MPLL_MASK                                      (0x3<<11)
#define     _24MPLL_DEBUG_Reg_24MPLL_CS_e                                     10
#define     _24MPLL_DEBUG_Reg_24MPLL_CS_SHIFT                                 9
#define     _24MPLL_DEBUG_Reg_24MPLL_CS_MASK                                  (0x3<<9)
#define     _24MPLL_DEBUG_Reg_24MPLL_RS_e                                     8
#define     _24MPLL_DEBUG_Reg_24MPLL_RS_SHIFT                                 7
#define     _24MPLL_DEBUG_Reg_24MPLL_RS_MASK                                  (0x3<<7)
#define     _24MPLL_DEBUG_Reg_24MPLL_ICPP_e                                   6
#define     _24MPLL_DEBUG_Reg_24MPLL_ICPP_SHIFT                               5
#define     _24MPLL_DEBUG_Reg_24MPLL_ICPP_MASK                                (0x3<<5)
#define     _24MPLL_DEBUG_REG_24MPLL_R3_e                                     4
#define     _24MPLL_DEBUG_REG_24MPLL_R3_SHIFT                                 3
#define     _24MPLL_DEBUG_REG_24MPLL_R3_MASK                                  (0x3<<3)
#define     _24MPLL_DEBUG_TESTSEL_e                                           2
#define     _24MPLL_DEBUG_TESTSEL_SHIFT                                       1
#define     _24MPLL_DEBUG_TESTSEL_MASK                                        (0x3<<1)
#define     _24MPLL_DEBUG_TEST_EN                                             0

#define     CMU_DEBUG_CTL_COREPLL_VB_AMSEL                                    13
#define     CMU_DEBUG_CTL_DBG_CORE_PLL_POSTDIV_e                              12
#define     CMU_DEBUG_CTL_DBG_CORE_PLL_POSTDIV_SHIFT                          11
#define     CMU_DEBUG_CTL_DBG_CORE_PLL_POSTDIV_MASK                           (0x3<<11)
#define     CMU_DEBUG_CTL_DBG_AUDPLL_POSTDIV_e                                10
#define     CMU_DEBUG_CTL_DBG_AUDPLL_POSTDIV_SHIFT                            9
#define     CMU_DEBUG_CTL_DBG_AUDPLL_POSTDIV_MASK                             (0x3<<9)
#define     CMU_DEBUG_CTL_CMU_TST_A0D1                                        8
#define     CMU_DEBUG_CTL_CMU_TST_SEL_e                                       7
#define     CMU_DEBUG_CTL_CMU_TST_SEL_SHIFT                                   5
#define     CMU_DEBUG_CTL_CMU_TST_SEL_MASK                                    (0x7<<5)
#define     CMU_DEBUG_CTL_CORE_PLL_ICP_SEL                                    4
#define     CMU_DEBUG_CTL_CMU_DEBUG_SEL_e                                     3
#define     CMU_DEBUG_CTL_CMU_DEBUG_SEL_SHIFT                                 0
#define     CMU_DEBUG_CTL_CMU_DEBUG_SEL_MASK                                  (0xF<<0)

//--------------CMU_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     CMU_Control_Register_BASE                                         0xC0001000
#define     CMU_SYSCLK                                                        (CMU_Control_Register_BASE+0x0000)
#define     CMU_DEVCLKEN                                                      (CMU_Control_Register_BASE+0x0004)
#define     CMU_ASRCCLK                                                       (CMU_Control_Register_BASE+0x0008)
#define     CMU_ADDACLK                                                       (CMU_Control_Register_BASE+0x000C)
#define     CMU_PCMCLK                                                        (CMU_Control_Register_BASE+0x0010)
#define     CMU_SDCLK                                                         (CMU_Control_Register_BASE+0x0018)
#define     CMU_LCDCLK                                                        (CMU_Control_Register_BASE+0x001C)
#define     CMU_SEGLCDCLK                                                     (CMU_Control_Register_BASE+0x0020)
#define     CMU_FMCLK                                                         (CMU_Control_Register_BASE+0x0024)
#define     CMU_PWM0CLK                                                       (CMU_Control_Register_BASE+0x0028)
#define     CMU_PWM1CLK                                                       (CMU_Control_Register_BASE+0x002C)
#define     CMU_PWM2CLK                                                       (CMU_Control_Register_BASE+0x0030)
#define     CMU_SPICLK                                                        (CMU_Control_Register_BASE+0x0034)
#define     CMU_UART0CLK                                                      (CMU_Control_Register_BASE+0x0038)
#define     CMU_UART1CLK                                                      (CMU_Control_Register_BASE+0x003C)
#define     CMU_MEMCLKEN                                                      (CMU_Control_Register_BASE+0x0040)
#define     CMU_MEMCLKSEL                                                     (CMU_Control_Register_BASE+0x0044)
#define     CMU_DIGITALDEBUG                                                  (CMU_Control_Register_BASE+0x0048)
#define     CMU_PWM3CLK                                                       (CMU_Control_Register_BASE+0x004C)

//--------------Bits Location------------------------------------------//
#define     CMU_SYSCLK_DSPAPBCLKDIV_e                                         13
#define     CMU_SYSCLK_DSPAPBCLKDIV_SHIFT                                     12
#define     CMU_SYSCLK_DSPAPBCLKDIV_MASK                                      (0x3<<12)
#define     CMU_SYSCLK_AHBCLKDIV                                              8
#define     CMU_SYSCLK_CPUCLKDIV_e                                            5
#define     CMU_SYSCLK_CPUCLKDIV_SHIFT                                        4
#define     CMU_SYSCLK_CPUCLKDIV_MASK                                         (0x3<<4)
#define     CMU_SYSCLK_CORE_CLKSEL_e                                          1
#define     CMU_SYSCLK_CORE_CLKSEL_SHIFT                                      0
#define     CMU_SYSCLK_CORE_CLKSEL_MASK                                       (0x3<<0)

#define     CMU_DEVCLKEN_DSPCLKEN                                             31
#define     CMU_DEVCLKEN_SPI1CLKEN                                            24
#define     CMU_DEVCLKEN_EFUSECLKEN                                           23
#define     CMU_DEVCLKEN_I2SSCLKEN                                            22
#define     CMU_DEVCLKEN_I2SMCLKEN                                            21
#define     CMU_DEVCLKEN_FMCLKEN                                              20
#define     CMU_DEVCLKEN_PCMCLKEN                                             19
#define     CMU_DEVCLKEN_SEGLCDCLKEN                                          18
#define     CMU_DEVCLKEN_TOUCHKEYCLKEN                                        17
#define     CMU_DEVCLKEN_PWMCLKEN                                             16
#define     CMU_DEVCLKEN_IRCLKEN                                              15
#define     CMU_DEVCLKEN_DACCLKEN                                             13
#define     CMU_DEVCLKEN_ADCCLKEN                                             12
#define     CMU_DEVCLKEN_TIMERCLKEN                                           11
#define     CMU_DEVCLKEN_USBCLKEN                                             10
#define     CMU_DEVCLKEN_UART1CLKEN                                           9
#define     CMU_DEVCLKEN_LCDCLKEN                                             8
#define     CMU_DEVCLKEN_SPICLKEN                                             7
#define     CMU_DEVCLKEN_SPIBOOTCLKEN                                         6
#define     CMU_DEVCLKEN_I2CCLKEN                                             5
#define     CMU_DEVCLKEN_UART0CLKEN                                           4
#define     CMU_DEVCLKEN_SDCLKEN                                              3
#define     CMU_DEVCLKEN_ASRCCLKEN                                            2
#define     CMU_DEVCLKEN_SPDIFCLKEN                                           1
#define     CMU_DEVCLKEN_DMACLKEN                                             0

#define     CMU_ASRCCLK_ASRCCLKSEL                                            4
#define     CMU_ASRCCLK_ASRCCLKDIV_e                                          1
#define     CMU_ASRCCLK_ASRCCLKDIV_SHIFT                                      0
#define     CMU_ASRCCLK_ASRCCLKDIV_MASK                                       (0x3<<0)

#define     CMU_ADDACLK_ADCCLKDIV_e                                           6
#define     CMU_ADDACLK_ADCCLKDIV_SHIFT                                       4
#define     CMU_ADDACLK_ADCCLKDIV_MASK                                        (0x7<<4)
#define     CMU_ADDACLK_DACCLKDIV_e                                           2
#define     CMU_ADDACLK_DACCLKDIV_SHIFT                                       0
#define     CMU_ADDACLK_DACCLKDIV_MASK                                        (0x7<<0)

#define     CMU_PCMCLK_CLKSEL                                                 4

#define     CMU_SDCLK_SDCLKSEL1                                               8
#define     CMU_SDCLK_SDCLKSEL0                                               4
#define     CMU_SDCLK_SDCLKDIV_e                                              3
#define     CMU_SDCLK_SDCLKDIV_SHIFT                                          0
#define     CMU_SDCLK_SDCLKDIV_MASK                                           (0xF<<0)

#define     CMU_LCDCLK_LCDCLKSEL                                              4
#define     CMU_LCDCLK_LCDCLKDIV_e                                            1
#define     CMU_LCDCLK_LCDCLKDIV_SHIFT                                        0
#define     CMU_LCDCLK_LCDCLKDIV_MASK                                         (0x3<<0)

#define     CMU_SEGLCDCLK_SEGLCDCLKSEL                                        4
#define     CMU_SEGLCDCLK_SEGLCDCLKDIV_e                                      2
#define     CMU_SEGLCDCLK_SEGLCDCLKDIV_SHIFT                                  0
#define     CMU_SEGLCDCLK_SEGLCDCLKDIV_MASK                                   (0x7<<0)

#define     CMU_FMCLK_FMCLKSEL_e                                              1
#define     CMU_FMCLK_FMCLKSEL_SHIFT                                          0
#define     CMU_FMCLK_FMCLKSEL_MASK                                           (0x3<<0)

#define     CMU_PWM0CLK_PWMCLKSEL                                             12
#define     CMU_PWM0CLK_PWMCLKDIV_e                                           8
#define     CMU_PWM0CLK_PWMCLKDIV_SHIFT                                       0
#define     CMU_PWM0CLK_PWMCLKDIV_MASK                                        (0x1FF<<0)

#define     CMU_PWM1CLK_PWMCLKSEL                                             12
#define     CMU_PWM1CLK_PWMCLKDIV_e                                           8
#define     CMU_PWM1CLK_PWMCLKDIV_SHIFT                                       0
#define     CMU_PWM1CLK_PWMCLKDIV_MASK                                        (0x1FF<<0)

#define     CMU_PWM2CLK_PWMCLKSEL                                             12
#define     CMU_PWM2CLK_PWMCLKDIV_e                                           8
#define     CMU_PWM2CLK_PWMCLKDIV_SHIFT                                       0
#define     CMU_PWM2CLK_PWMCLKDIV_MASK                                        (0x1FF<<0)

#define     CMU_SPICLK_SEL1_e                                                 5
#define     CMU_SPICLK_SEL1_SHIFT                                             4
#define     CMU_SPICLK_SEL1_MASK                                              (0x3<<4)
#define     CMU_SPICLK_SEL_e                                                  1
#define     CMU_SPICLK_SEL_SHIFT                                              0
#define     CMU_SPICLK_SEL_MASK                                               (0x3<<0)

#define     CMU_UART0CLK_SEL                                                  0

#define     CMU_UART1CLK_SEL                                                  0

#define     CMU_MEMCLKEN_URAM1CLKEN                                           8
#define     CMU_MEMCLKEN_URAM0CLKEN                                           7
#define     CMU_MEMCLKEN_SDBUFCLKEN                                           6
#define     CMU_MEMCLKEN_ASRCBUF1CLKEN                                        5
#define     CMU_MEMCLKEN_ASRCBUF0CLKEN                                        4
#define     CMU_MEMCLKEN_PCMRAM2CLKEN                                         3
#define     CMU_MEMCLKEN_PCMRAM1CLKEN                                         2
#define     CMU_MEMCLKEN_PCMRAM0CLKEN                                         1
#define     CMU_MEMCLKEN_ROMRAMCLKEN                                          0

#define     CMU_MEMCLKSEL_URAM1CLKSEL_e                                       25
#define     CMU_MEMCLKSEL_URAM1CLKSEL_SHIFT                                   24
#define     CMU_MEMCLKSEL_URAM1CLKSEL_MASK                                    (0x3<<24)
#define     CMU_MEMCLKSEL_URAM0CLKSEL_e                                       21
#define     CMU_MEMCLKSEL_URAM0CLKSEL_SHIFT                                   20
#define     CMU_MEMCLKSEL_URAM0CLKSEL_MASK                                    (0x3<<20)
#define     CMU_MEMCLKSEL_SDBUFCLKSEL                                         16
#define     CMU_MEMCLKSEL_ASRCBUF1CLKSEL                                      13
#define     CMU_MEMCLKSEL_ASRCBUF0CLKSEL                                      12
#define     CMU_MEMCLKSEL_PCMRAM2CLKSEL_e                                     9
#define     CMU_MEMCLKSEL_PCMRAM2CLKSEL_SHIFT                                 8
#define     CMU_MEMCLKSEL_PCMRAM2CLKSEL_MASK                                  (0x3<<8)
#define     CMU_MEMCLKSEL_PCMRAM1CLKSEL_e                                     5
#define     CMU_MEMCLKSEL_PCMRAM1CLKSEL_SHIFT                                 4
#define     CMU_MEMCLKSEL_PCMRAM1CLKSEL_MASK                                  (0x3<<4)
#define     CMU_MEMCLKSEL_PCMRAM0CLKSEL_e                                     1
#define     CMU_MEMCLKSEL_PCMRAM0CLKSEL_SHIFT                                 0
#define     CMU_MEMCLKSEL_PCMRAM0CLKSEL_MASK                                  (0x3<<0)

#define     CMU_DIGITALDEBUG_DBGEN                                            31
#define     CMU_DIGITALDEBUG_DEWS                                             8
#define     CMU_DIGITALDEBUG_SEL_e                                            5
#define     CMU_DIGITALDEBUG_SEL_SHIFT                                        0
#define     CMU_DIGITALDEBUG_SEL_MASK                                         (0x3F<<0)

#define     CMU_PWM3CLK_PWMCLKSEL                                             12
#define     CMU_PWM3CLK_PWMCLKDIV_e                                           8
#define     CMU_PWM3CLK_PWMCLKDIV_SHIFT                                       0
#define     CMU_PWM3CLK_PWMCLKDIV_MASK                                        (0x1FF<<0)

//--------------RTC-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     RTC_BASE                                                          0xC0120000
#define     RTC_CTL                                                           (RTC_BASE+0x00)
#define     RTC_REGUPDATA                                                     (RTC_BASE+0x04)
#define     RTC_DHMSALM                                                       (RTC_BASE+0x08)
#define     RTC_DHMS                                                          (RTC_BASE+0x0C)
#define     RTC_YMD                                                           (RTC_BASE+0x10)
#define     RTC_ACCESS                                                        (RTC_BASE+0x14)
#define     Hz2_CTL                                                           (RTC_BASE+0x18)
#define     WD_CTL                                                            (RTC_BASE+0x1c)
#define     T0_CTL                                                            (RTC_BASE+0x20)
#define     T0_VAL                                                            (RTC_BASE+0x24)
#define     T1_CTL                                                            (RTC_BASE+0x28)
#define     T1_VAL                                                            (RTC_BASE+0x2C)
#define     RTC_BAK0                                                          (RTC_BASE+0X30)
#define     RTC_BAK1                                                          (RTC_BASE+0X34)
#define     RTC_BAK2                                                          (RTC_BASE+0X38)
#define     RTC_BAK3                                                          (RTC_BASE+0X3C)

//--------------Bits Location------------------------------------------//
#define     RTC_CTL_TEST_EN                                                   16
#define     RTC_CTL_LEAP                                                      7
#define     RTC_CTL_CAL_EN                                                    4
#define     RTC_CTL_Hosc4hz_select                                            3
#define     RTC_CTL_Hcl_select                                                2
#define     RTC_CTL_ALIE                                                      1
#define     RTC_CTL_ALIP                                                      0

#define     RTC_REGUPDATA_UPDATA_e                                            15
#define     RTC_REGUPDATA_UPDATA_SHIFT                                        0
#define     RTC_REGUPDATA_UPDATA_MASK                                         (0xFFFF<<0)

#define     RTC_DHMSALM_HOUEAL_e                                              20
#define     RTC_DHMSALM_HOUEAL_SHIFT                                          16
#define     RTC_DHMSALM_HOUEAL_MASK                                           (0x1F<<16)
#define     RTC_DHMSALM_MINAL_e                                               13
#define     RTC_DHMSALM_MINAL_SHIFT                                           8
#define     RTC_DHMSALM_MINAL_MASK                                            (0x3F<<8)
#define     RTC_DHMSALM_SECAL_e                                               5
#define     RTC_DHMSALM_SECAL_SHIFT                                           0
#define     RTC_DHMSALM_SECAL_MASK                                            (0x3F<<0)

#define     RTC_DHMS_HOUR_e                                                   20
#define     RTC_DHMS_HOUR_SHIFT                                               16
#define     RTC_DHMS_HOUR_MASK                                                (0x1F<<16)
#define     RTC_DHMS_MIN_e                                                    13
#define     RTC_DHMS_MIN_SHIFT                                                8
#define     RTC_DHMS_MIN_MASK                                                 (0x3F<<8)
#define     RTC_DHMS_SEC_e                                                    5
#define     RTC_DHMS_SEC_SHIFT                                                0
#define     RTC_DHMS_SEC_MASK                                                 (0x3F<<0)

#define     RTC_YMD_YEAR_e                                                    22
#define     RTC_YMD_YEAR_SHIFT                                                16
#define     RTC_YMD_YEAR_MASK                                                 (0x7F<<16)
#define     RTC_YMD_MON_e                                                     11
#define     RTC_YMD_MON_SHIFT                                                 8
#define     RTC_YMD_MON_MASK                                                  (0xF<<8)
#define     RTC_YMD_DATE_e                                                    4
#define     RTC_YMD_DATE_SHIFT                                                0
#define     RTC_YMD_DATE_MASK                                                 (0x1F<<0)

#define     RTC_ACCESS_ACCESS_e                                               7
#define     RTC_ACCESS_ACCESS_SHIFT                                           0
#define     RTC_ACCESS_ACCESS_MASK                                            (0xFF<<0)

#define     Hz2_CTL_2HIE                                                      1
#define     Hz2_CTL_2HIP                                                      0

#define     WD_CTL_EJTAG_F                                                    31
#define     WD_CTL_IRQP                                                       6
#define     WD_CTL_SIGS                                                       5
#define     WD_CTL_WDEN                                                       4
#define     WD_CTL_CLKSEL_e                                                   3
#define     WD_CTL_CLKSEL_SHIFT                                               1
#define     WD_CTL_CLKSEL_MASK                                                (0x7<<1)
#define     WD_CTL_CLR                                                        0

#define     T0_CTL_EN                                                         5
#define     T0_CTL_RELO                                                       2
#define     T0_CTL_ZIEN                                                       1
#define     T0_CTL_ZIPD                                                       0

#define     T0_VAL_T0_e                                                       23
#define     T0_VAL_T0_SHIFT                                                   0
#define     T0_VAL_T0_MASK                                                    (0xFFFFFF<<0)

#define     T1_CTL_En                                                         5
#define     T1_CTL_RELO                                                       2
#define     T1_CTL_ZIEN                                                       1
#define     T1_CTL_ZIPD                                                       0

#define     T1_VAL_T1_e                                                       23
#define     T1_VAL_T1_SHIFT                                                   0
#define     T1_VAL_T1_MASK                                                    (0xFFFFFF<<0)





//--------------InterruptController-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     InterruptController_BASE                                          0xC00B0000
#define     INTC_PD                                                           (InterruptController_BASE+0x00000000)
#define     INTC_MSK                                                          (InterruptController_BASE+0x00000004)
#define     INTC_CFG0                                                         (InterruptController_BASE+0x00000008)
#define     INTC_CFG1                                                         (InterruptController_BASE+0x0000000c)
#define     INTC_CFG2                                                         (InterruptController_BASE+0x00000010)
#define     INTC_EXTCTL                                                       (InterruptController_BASE+0x00000014)
#define     INTC_EXTIP                                                        (InterruptController_BASE+0x00000018)
#define     REQ_INT_OUT                                                       (InterruptController_BASE+0x0000001c)
#define     REQ_IN                                                            (InterruptController_BASE+0x00000020)
#define     REQ_IN_PD                                                         (InterruptController_BASE+0x00000024)
#define     REQ_OUT                                                           (InterruptController_BASE+0x00000028)

//--------------Bits Location------------------------------------------//
#define     INTC_PD_OUT_USER4_IP                                              31
#define     INTC_PD_OUT_USER3_IP                                              30
#define     INTC_PD_OUT_USER2_IP                                              29
#define     INTC_PD_OUT_USER1_IP                                              28
#define     INTC_PD_OUT_USER0_IP                                              27
#define     INTC_PD_SPI1_IP                                                   26
#define     INTC_PD_PCM_tx_IP                                                 25
#define     INTC_PD_PCM_rx_IP                                                 24
#define     INTC_PD_DMA5_IP                                                   23
#define     INTC_PD_DMA4_IP                                                   22
#define     INTC_PD_DMA3_IP                                                   21
#define     INTC_PD_DMA2_IP                                                   20
#define     INTC_PD_DMA1_IP                                                   19
#define     INTC_PD_DMA0_IP                                                   18
#define     INTC_PD_SD_IP                                                     17
#define     INTC_PD_MPU_IP                                                    16
#define     INTC_PD_ADC_IIS_RX_IP                                             15
#define     INTC_PD_DAC_IIS_TX_IP                                             14
#define     INTC_PD_SIRQ1_IP                                                  13
#define     INTC_PD_UART1_IP                                                  12
#define     INTC_PD_IIC_IP                                                    11
#define     INTC_PD_USB_IP                                                    10
#define     INTC_PD_SPI0_IP                                                   9
#define     INTC_PD_TouchKey_IP                                               8
#define     INTC_PD_SIRQ0_IP                                                  7
#define     INTC_PD_UART0_IP                                                  6
#define     INTC_PD_RTC_IP                                                    5
#define     INTC_PD_TIMER0_IP                                                 4
#define     INTC_PD_TIMER1_IP                                                 3
#define     INTC_PD_2Hz_IP                                                    2
#define     INTC_PD_NFC_IP                                                    1
#define     INTC_PD_BT_IP                                                     0

#define     INTC_MSK_OUT_USER4_IM                                             31
#define     INTC_MSK_OUT_USER3_IM                                             30
#define     INTC_MSK_OUT_USER2_IM                                             29
#define     INTC_MSK_OUT_USER1_IM                                             28
#define     INTC_MSK_OUT_USER0_IM                                             27
#define     INTC_MSK_SPI1_IM                                                  26
#define     INTC_MSK_PCM_tx_IM                                                25
#define     INTC_MSK_PCM_rx_IM                                                24
#define     INTC_MSK_DMA5_IM                                                  23
#define     INTC_MSK_DMA4_IM                                                  22
#define     INTC_MSK_DMA3_IM                                                  21
#define     INTC_MSK_DMA2_IM                                                  20
#define     INTC_MSK_DMA1_IM                                                  19
#define     INTC_MSK_DMA0_IM                                                  18
#define     INTC_MSK_SD_IM                                                    17
#define     INTC_MSK_MPU_IM                                                   16
#define     INTC_MSK_ADC_IIS_RX_IM                                            15
#define     INTC_MSK_DAC_IIS_TX_IM                                            14
#define     INTC_MSK_SIRQ1_IM                                                 13
#define     INTC_MSK_UART1_IM                                                 12
#define     INTC_MSK_IIC_IM                                                   11
#define     INTC_MSK_USB_IM                                                   10
#define     INTC_MSK_SPI0_IM                                                  9
#define     INTC_MSK_TouchKey_IM                                              8
#define     INTC_MSK_SIRQ0_IM                                                 7
#define     INTC_MSK_UART0_IM                                                 6
#define     INTC_MSK_RTC_IM                                                   5
#define     INTC_MSK_TIMER0_IM                                                4
#define     INTC_MSK_TIMER1_IM                                                3
#define     INTC_MSK_2Hz_IM                                                   2
#define     INTC_MSK_NFC_IM                                                   1
#define     INTC_MSK_BT_IM                                                    0

#define     INTC_CFG0_OUT_USER4_CFG0                                          31
#define     INTC_CFG0_OUT_USER3_CFG0                                          30
#define     INTC_CFG0_OUT_USER2_CFG0                                          29
#define     INTC_CFG0_OUT_USER1_CFG0                                          28
#define     INTC_CFG0_OUT_USER0_CFG0                                          27
#define     INTC_CFG0_SPI1_CFG0                                               26
#define     INTC_CFG0_PCM_tx_CFG0                                             25
#define     INTC_CFG0_PCM_rx_CFG0                                             24
#define     INTC_CFG0_DMA5_CFG0                                               23
#define     INTC_CFG0_DMA4_CFG0                                               22
#define     INTC_CFG0_DMA3_CFG0                                               21
#define     INTC_CFG0_DMA2_CFG0                                               20
#define     INTC_CFG0_DMA1_CFG0                                               19
#define     INTC_CFG0_DMA0_CFG0                                               18
#define     INTC_CFG0_SD_CFG0                                                 17
#define     INTC_CFG0_MPU_CFG0                                                16
#define     INTC_CFG0_ADC_IIS_RX_CFG0                                         15
#define     INTC_CFG0_DAC_IIS_TX_CFG0                                         14
#define     INTC_CFG0_SIRQ1_CFG0                                              13
#define     INTC_CFG0_UART1_CFG0                                              12
#define     INTC_CFG0_IIC_CFG0                                                11
#define     INTC_CFG0_USB_CFG0                                                10
#define     INTC_CFG0_SPI0_CFG0                                               9
#define     INTC_CFG0_TouchKey_CFG0                                           8
#define     INTC_CFG0_SIRQ0_CFG0                                              7
#define     INTC_CFG0_UART0_CFG0                                              6
#define     INTC_CFG0_RTC_CFG0                                                5
#define     INTC_CFG0_TIMER0_CFG0                                             4
#define     INTC_CFG0_TIMER1_CFG0                                             3
#define     INTC_CFG0_2Hz_CFG0                                                2
#define     INTC_CFG0_NFC_CFG0                                                1
#define     INTC_CFG0_BT_CFG0                                                 0

#define     INTC_CFG1_OUT_USER4_CFG1                                          31
#define     INTC_CFG1_OUT_USER3_CFG1                                          30
#define     INTC_CFG1_OUT_USER2_CFG1                                          29
#define     INTC_CFG1_OUT_USER1_CFG1                                          28
#define     INTC_CFG1_OUT_USER0_CFG1                                          27
#define     INTC_CFG1_SPI1_CFG1                                               26
#define     INTC_CFG1_PCM_tx_CFG1                                             25
#define     INTC_CFG1_PCM_rx_CFG1                                             24
#define     INTC_CFG1_DMA5_CFG1                                               23
#define     INTC_CFG1_DMA4_CFG1                                               22
#define     INTC_CFG1_DMA3_CFG1                                               21
#define     INTC_CFG1_DMA2_CFG1                                               20
#define     INTC_CFG1_DMA1_CFG1                                               19
#define     INTC_CFG1_DMA0_CFG1                                               18
#define     INTC_CFG1_SD_CFG1                                                 17
#define     INTC_CFG1_MPU_CFG1                                                16
#define     INTC_CFG1_ADC_IIS_RX_CFG1                                         15
#define     INTC_CFG1_DAC_IIS_TX_CFG1                                         14
#define     INTC_CFG1_SIRQ1_CFG1                                              13
#define     INTC_CFG1_UART1_CFG1                                              12
#define     INTC_CFG1_IIC_CFG1                                                11
#define     INTC_CFG1_USB_CFG1                                                10
#define     INTC_CFG1_SPI0_CFG1                                               9
#define     INTC_CFG1_TouchKey_CFG1                                           8
#define     INTC_CFG1_SIRQ0_CFG1                                              7
#define     INTC_CFG1_UART0_CFG1                                              6
#define     INTC_CFG1_RTC_CFG1                                                5
#define     INTC_CFG1_TIMER0_CFG1                                             4
#define     INTC_CFG1_TIMER1_CFG1                                             3
#define     INTC_CFG1_2Hz_CFG1                                                2
#define     INTC_CFG1_NFC_CFG1                                                1
#define     INTC_CFG1_BT_CFG1                                                 0

#define     INTC_CFG2_OUT_USER4_CFG2                                          31
#define     INTC_CFG2_OUT_USER3_CFG2                                          30
#define     INTC_CFG2_OUT_USER2_CFG2                                          29
#define     INTC_CFG2_OUT_USER1_CFG2                                          28
#define     INTC_CFG2_OUT_USER0_CFG2                                          27
#define     INTC_CFG2_SPI1_CFG2                                               26
#define     INTC_CFG2_PCM_tx_CFG2                                             25
#define     INTC_CFG2_PCM_rx_CFG2                                             24
#define     INTC_CFG2_DMA5_CFG2                                               23
#define     INTC_CFG2_DMA4_CFG2                                               22
#define     INTC_CFG2_DMA3_CFG2                                               21
#define     INTC_CFG2_DMA2_CFG2                                               20
#define     INTC_CFG2_DMA1_CFG2                                               19
#define     INTC_CFG2_DMA0_CFG2                                               18
#define     INTC_CFG2_SD_CFG2                                                 17
#define     INTC_CFG2_MPU_CFG2                                                16
#define     INTC_CFG2_ADC_IIS_RX_CFG2                                         15
#define     INTC_CFG2_DAC_IIS_TX_CFG2                                         14
#define     INTC_CFG2_SIRQ1_CFG2                                              13
#define     INTC_CFG2_UART1_CFG2                                              12
#define     INTC_CFG2_IIC_CFG2                                                11
#define     INTC_CFG2_USB_CFG2                                                10
#define     INTC_CFG2_SPI0_CFG2                                               9
#define     INTC_CFG2_TouchKey_CFG2                                           8
#define     INTC_CFG2_SIRQ0_CFG2                                              7
#define     INTC_CFG2_UART0_CFG2                                              6
#define     INTC_CFG2_RTC_CFG2                                                5
#define     INTC_CFG2_TIMER0_CFG2                                             4
#define     INTC_CFG2_TIMER1_CFG2                                             3
#define     INTC_CFG2_2Hz_CFG2                                                2
#define     INTC_CFG2_NFC_CFG1                                                1
#define     INTC_CFG2_BT_CFG1                                                 0

#define     INTC_EXTCTL_EXTYPE1                                               3
#define     INTC_EXTCTL_EXTYPE0                                               1

#define     INTC_EXTIP_E1PD                                                   1
#define     INTC_EXTIP_E0PD                                                   0

#define     REQ_INT_OUT_DSP_INT3                                              0

#define     REQ_IN_OUT_USER4                                                  4
#define     REQ_IN_OUT_USER3                                                  3
#define     REQ_IN_OUT_USER2                                                  2
#define     REQ_IN_OUT_USER1                                                  1
#define     REQ_IN_OUT_USER0                                                  0

#define     REQ_IN_PD_OUT_USER4_PD                                            4
#define     REQ_IN_PD_OUT_USER3_PD                                            3
#define     REQ_IN_PD_OUT_USER2_PD                                            2
#define     REQ_IN_PD_OUT_USER1_PD                                            1
#define     REQ_IN_PD_OUT_USER0_PD                                            0

#define     REQ_OUT_IN_USER1                                                  1
#define     REQ_OUT_IN_USER0                                                  0

//--------------SD-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SD_BASE                                                           0xC0060000
#define     SD_EN                                                             (SD_BASE+0x00)
#define     SD_CTL                                                            (SD_BASE+0x04)
#define     SD_STATE                                                          (SD_BASE+0x08)
#define     SD_CMD                                                            (SD_BASE+0x0C)
#define     SD_ARG                                                            (SD_BASE+0x10)
#define     SD_RSPBUF0                                                        (SD_BASE+0x14)
#define     SD_RSPBUF1                                                        (SD_BASE+0x18)
#define     SD_RSPBUF2                                                        (SD_BASE+0x1C)
#define     SD_RSPBUF3                                                        (SD_BASE+0x20)
#define     SD_RSPBUF4                                                        (SD_BASE+0x24)
#define     SD_DAT                                                            (SD_BASE+0x28)
#define     SD_BLK_SIZE                                                       (SD_BASE+0x2C)
#define     SD_BLK_NUM                                                        (SD_BASE+0x30)

//--------------Bits Location------------------------------------------//
#define     SD_EN_S18EN                                                       12
#define     SD_EN_CLKS                                                        8
#define     SD_EN_EN                                                          7
#define     SD_EN_AHB_DMA_SEL                                                 6
#define     SD_EN_SDIOEN                                                      3
#define     SD_EN_DATAWID_e                                                   1
#define     SD_EN_DATAWID_SHIFT                                               0
#define     SD_EN_DATAWID_MASK                                                (0x3<<0)

#define     SD_CTL_TOUTEN                                                     31
#define     SD_CTL_TOUTCNT                                                    30-24
#define     SD_CTL_RDELAY                                                     23-20
#define     SD_CTL_WDELAY                                                     19-16
#define     SD_CTL_CMDLEN                                                     13
#define     SD_CTL_SCC                                                        12
#define     SD_CTL_TCN_e                                                      11
#define     SD_CTL_TCN_SHIFT                                                  8
#define     SD_CTL_TCN_MASK                                                   (0xF<<8)
#define     SD_CTL_TS                                                         7
#define     SD_CTL_LBE                                                        6
#define     SD_CTL_C7EN                                                       5
#define     SD_CTL_TM_e                                                       3
#define     SD_CTL_TM_SHIFT                                                   0
#define     SD_CTL_TM_MASK                                                    (0xF<<0)

#define     SD_STATE_TOUTE                                                    15
#define     SD_STATE_BAEP                                                     14
#define     SD_STATE_MEMRDY                                                   12
#define     SD_STATE_CMDS                                                     11
#define     SD_STATE_DAT1S                                                    10
#define     SD_STATE_SDIOP                                                    9
#define     SD_STATE_SDIOEN                                                   8
#define     SD_STATE_DAT0S                                                    7
#define     SD_STATE_TEIE                                                     6
#define     SD_STATE_TEI                                                      5
#define     SD_STATE_CLNR                                                     4
#define     SD_STATE_CLC                                                      3
#define     SD_STATE_WC16ER                                                   2
#define     SD_STATE_RC16ER                                                   1
#define     SD_STATE_CRC7ER                                                   0

#define     SD_CMD_CMD_e                                                      7
#define     SD_CMD_CMD_SHIFT                                                  0
#define     SD_CMD_CMD_MASK                                                   (0xFF<<0)

#define     SD_ARG_ARG_e                                                      31
#define     SD_ARG_ARG_SHIFT                                                  0
#define     SD_ARG_ARG_MASK                                                   (0xFFFFFFFF<<0)

#define     SD_RSPBUF0_RSP0_e                                                 31
#define     SD_RSPBUF0_RSP0_SHIFT                                             0
#define     SD_RSPBUF0_RSP0_MASK                                              (0xFFFFFFFF<<0)

#define     SD_RSPBUF1_RSP1_e                                                 31
#define     SD_RSPBUF1_RSP1_SHIFT                                             0
#define     SD_RSPBUF1_RSP1_MASK                                              (0xFFFFFFFF<<0)

#define     SD_RSPBUF2_RSP2_e                                                 31
#define     SD_RSPBUF2_RSP2_SHIFT                                             0
#define     SD_RSPBUF2_RSP2_MASK                                              (0xFFFFFFFF<<0)

#define     SD_RSPBUF3_RSP3_e                                                 31
#define     SD_RSPBUF3_RSP3_SHIFT                                             0
#define     SD_RSPBUF3_RSP3_MASK                                              (0xFFFFFFFF<<0)

#define     SD_RSPBUF4_RSP4_e                                                 7
#define     SD_RSPBUF4_RSP4_SHIFT                                             0
#define     SD_RSPBUF4_RSP4_MASK                                              (0xFF<<0)

#define     SD_DAT_DATA_e                                                     31
#define     SD_DAT_DATA_SHIFT                                                 0
#define     SD_DAT_DATA_MASK                                                  (0xFFFFFFFF<<0)

#define     SD_BLK_SIZE_BC_e                                                  9
#define     SD_BLK_SIZE_BC_SHIFT                                              0
#define     SD_BLK_SIZE_BC_MASK                                               (0x3FF<<0)

#define     SD_BLK_NUM_BN_e                                                   15
#define     SD_BLK_NUM_BN_SHIFT                                               0
#define     SD_BLK_NUM_BN_MASK                                                (0xFFFF<<0)

//--------------USB_Controller_Registers-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     USB_Controller_Registers_BASE                                     0xC0080000
#define     USBIRQ                                                            (USB_Controller_Registers_BASE+0x1BC)
#define     USBstate                                                          (USB_Controller_Registers_BASE+0x1BD)
#define     USBCtrl                                                           (USB_Controller_Registers_BASE+0x1BE)
#define     USBstatus                                                         (USB_Controller_Registers_BASE+0x1BF)
#define     USBien                                                            (USB_Controller_Registers_BASE+0x1C0)
#define     Hcep0ctrl                                                         (USB_Controller_Registers_BASE+0x0C0)
#define     Hcout1ctrl                                                        (USB_Controller_Registers_BASE+0x0C4)
#define     Hcout2ctrl                                                        (USB_Controller_Registers_BASE+0x0C8)
#define     Hcout3ctrl                                                        (USB_Controller_Registers_BASE+0x0CC)
#define     Hcout0err                                                         (USB_Controller_Registers_BASE+0x0C1)
#define     Hcout1err                                                         (USB_Controller_Registers_BASE+0x0C5)
#define     Hcout2err                                                         (USB_Controller_Registers_BASE+0x0C9)
#define     Hcout3err                                                         (USB_Controller_Registers_BASE+0x0CD)
#define     Hcin1ctrl                                                         (USB_Controller_Registers_BASE+0x0C6)
#define     Hcin2ctrl                                                         (USB_Controller_Registers_BASE+0x0CA)
#define     Hcin3ctrl                                                         (USB_Controller_Registers_BASE+0x0CE)
#define     Hcin0err                                                          (USB_Controller_Registers_BASE+0x0C3)
#define     Hcin1err                                                          (USB_Controller_Registers_BASE+0x0C7)
#define     Hcin2err                                                          (USB_Controller_Registers_BASE+0x0CB)
#define     Hcin3err                                                          (USB_Controller_Registers_BASE+0x0CF)
#define     Hcportctrl                                                        (USB_Controller_Registers_BASE+0x1AB)
#define     Hcfrmnl                                                           (USB_Controller_Registers_BASE+0x1AC)
#define     Hcfrmnh                                                           (USB_Controller_Registers_BASE+0x1AD)
#define     Hcfrmremainl                                                      (USB_Controller_Registers_BASE+0x1AE)
#define     Hcfrmremainh                                                      (USB_Controller_Registers_BASE+0x1AF)
#define     HCIN03ERRIRQ                                                      (USB_Controller_Registers_BASE+0x1B4)
#define     HCOUT03ERRIRQ                                                     (USB_Controller_Registers_BASE+0x1B6)
#define     HCIN03ERRIEN                                                      (USB_Controller_Registers_BASE+0x1B8)
#define     HCOUT03ERRIEN                                                     (USB_Controller_Registers_BASE+0x1BA)
#define     Hcin0maxpck                                                       (USB_Controller_Registers_BASE+0x1E0)
#define     Hcin1maxpckl                                                      (USB_Controller_Registers_BASE+0x1E2)
#define     Hcin2maxpckl                                                      (USB_Controller_Registers_BASE+0x1E4)
#define     Hcin3maxpckl                                                      (USB_Controller_Registers_BASE+0x1E6)
#define     Hcin1maxpckh                                                      (USB_Controller_Registers_BASE+0x1E3)
#define     Hcin2maxpckh                                                      (USB_Controller_Registers_BASE+0x1E5)
#define     Hcin3maxpckh                                                      (USB_Controller_Registers_BASE+0x1E7)
#define     Hcout1maxpckl                                                     (USB_Controller_Registers_BASE+0x3E2)
#define     Hcout2maxpckl                                                     (USB_Controller_Registers_BASE+0x3E4)
#define     Hcout3maxpckl                                                     (USB_Controller_Registers_BASE+0x3E6)
#define     Hcout1maxpckh                                                     (USB_Controller_Registers_BASE+0x3E3)
#define     Hcout2maxpckh                                                     (USB_Controller_Registers_BASE+0x3E5)
#define     Hcout3maxpckh                                                     (USB_Controller_Registers_BASE+0x3E7)
#define     Out0bc_hcin0bc                                                    (USB_Controller_Registers_BASE+0x000)
#define     In0bc_hcout0bc                                                    (USB_Controller_Registers_BASE+0x001)
#define     Ep0cs_hcep0cs                                                     (USB_Controller_Registers_BASE+0x002)
#define     Out1bcl_hcin1bcl                                                  (USB_Controller_Registers_BASE+0x008)
#define     Out2bcl_hcin2bcl                                                  (USB_Controller_Registers_BASE+0x010)
#define     Out3bcl_hcin3bcl                                                  (USB_Controller_Registers_BASE+0x018)
#define     Out1bch_hcin1bch                                                  (USB_Controller_Registers_BASE+0x009)
#define     Out2bch_hcin2bch                                                  (USB_Controller_Registers_BASE+0x011)
#define     Out3bch_hcin3bch                                                  (USB_Controller_Registers_BASE+0x019)
#define     Out1cs_hcin1cs                                                    (USB_Controller_Registers_BASE+0x00B)
#define     Out2cs_hcin2cs                                                    (USB_Controller_Registers_BASE+0x013)
#define     Out3cs_hcin3cs                                                    (USB_Controller_Registers_BASE+0x01B)
#define     Out1ctrl_hcin1ctrl                                                (USB_Controller_Registers_BASE+0x00A)
#define     Out2ctrl_hcin2ctrl                                                (USB_Controller_Registers_BASE+0x012)
#define     Out3ctrl_hcin3ctrl                                                (USB_Controller_Registers_BASE+0x01A)
#define     In1bcl_hcout1bcl                                                  (USB_Controller_Registers_BASE+0x00C)
#define     In2bcl_hcout2bcl                                                  (USB_Controller_Registers_BASE+0x014)
#define     In3bcl_hcout3bcl                                                  (USB_Controller_Registers_BASE+0x01C)
#define     In1bch_hcout1bch                                                  (USB_Controller_Registers_BASE+0x00D)
#define     In2bch_hcout2bch                                                  (USB_Controller_Registers_BASE+0x015)
#define     In3bch_hcout3bch                                                  (USB_Controller_Registers_BASE+0x01D)
#define     In1cs_hcout1cs                                                    (USB_Controller_Registers_BASE+0x00F)
#define     In2cs_hcout2cs                                                    (USB_Controller_Registers_BASE+0x017)
#define     In3cs_hcout3cs                                                    (USB_Controller_Registers_BASE+0x01F)
#define     In1ctrl_hcout1ctrl                                                (USB_Controller_Registers_BASE+0x00E)
#define     In2ctrl_hcout2ctrl                                                (USB_Controller_Registers_BASE+0x016)
#define     In3ctrl_hcout3ctrl                                                (USB_Controller_Registers_BASE+0x01E)
#define     Fifo1dat                                                          (USB_Controller_Registers_BASE+0x084)
#define     Fifo2dat                                                          (USB_Controller_Registers_BASE+0x088)
#define     Fifo3dat                                                          (USB_Controller_Registers_BASE+0x08C)
#define     Ep0indata                                                         (USB_Controller_Registers_BASE+0x100)
#define     Ep0outdata                                                        (USB_Controller_Registers_BASE+0x140)
#define     Setupdat0                                                         (USB_Controller_Registers_BASE+0x180)
#define     Setupdat1                                                         (USB_Controller_Registers_BASE+0x181)
#define     Setupdat2                                                         (USB_Controller_Registers_BASE+0x182)
#define     Setupdat3                                                         (USB_Controller_Registers_BASE+0x183)
#define     Setupdat4                                                         (USB_Controller_Registers_BASE+0x184)
#define     Setupdat5                                                         (USB_Controller_Registers_BASE+0x185)
#define     Setupdat6                                                         (USB_Controller_Registers_BASE+0x186)
#define     Setupdat7                                                         (USB_Controller_Registers_BASE+0x187)
#define     OUT03IRQ                                                          (USB_Controller_Registers_BASE+0x1A8)
#define     IN03IRQ                                                           (USB_Controller_Registers_BASE+0x188)
#define     Usbirq_hcusbirq                                                   (USB_Controller_Registers_BASE+0x18C)
#define     OUT03PNGIRQ                                                       (USB_Controller_Registers_BASE+0x18E)
#define     OUT03IEN                                                          (USB_Controller_Registers_BASE+0x196)
#define     IN03IEN                                                           (USB_Controller_Registers_BASE+0x194)
#define     Usbien_hcusbien                                                   (USB_Controller_Registers_BASE+0x198)
#define     OUT03PNGIEN                                                       (USB_Controller_Registers_BASE+0x19A)
#define     IN03TOKIRQ                                                        (USB_Controller_Registers_BASE+0x190)
#define     OUT03TOKIRQ                                                       (USB_Controller_Registers_BASE+0x191)
#define     IN03TOKIEN                                                        (USB_Controller_Registers_BASE+0x19C)
#define     OUT03TOKIEN                                                       (USB_Controller_Registers_BASE+0x19D)
#define     IVECT                                                             (USB_Controller_Registers_BASE+0x1A0)
#define     EPRST                                                             (USB_Controller_Registers_BASE+0x1A2)
#define     UsbCTRL_STUS                                                      (USB_Controller_Registers_BASE+0x1A3)
#define     FrmCNTL                                                           (USB_Controller_Registers_BASE+0x1A4)
#define     FrmCNTH                                                           (USB_Controller_Registers_BASE+0x1A5)
#define     Fnaddr                                                            (USB_Controller_Registers_BASE+0x1A6)
#define     Clkgate                                                           (USB_Controller_Registers_BASE+0x1A7)
#define     Fifoctrl                                                          (USB_Controller_Registers_BASE+0x1A8)
#define     OUT1STADDR                                                        (USB_Controller_Registers_BASE+0x304)
#define     OUT2STADDR                                                        (USB_Controller_Registers_BASE+0x308)
#define     OUT3STADDR                                                        (USB_Controller_Registers_BASE+0x30C)
#define     IN1STADDR                                                         (USB_Controller_Registers_BASE+0x344)
#define     IN2STADDR                                                         (USB_Controller_Registers_BASE+0x348)
#define     IN3STADDR                                                         (USB_Controller_Registers_BASE+0x34C)
#define     USBEIRQ                                                           (USB_Controller_Registers_BASE+0x400)
#define     NAKOUTCTRL                                                        (USB_Controller_Registers_BASE+0x401)
#define     HCINCTRL                                                          (USB_Controller_Registers_BASE+0x402)
#define     HCINENDINT                                                        (USB_Controller_Registers_BASE+0x41F)
#define     OSHRTPCKIR                                                        (USB_Controller_Registers_BASE+0x403)
#define     USBDEBUG                                                          (USB_Controller_Registers_BASE+0x404)
#define     HCOUTEMPINTCTRL                                                   (USB_Controller_Registers_BASE+0x406)
#define     VDCTRL                                                            (USB_Controller_Registers_BASE+0x408)
#define     VDSTAT                                                            (USB_Controller_Registers_BASE+0x409)
#define     UDMAM                                                             (USB_Controller_Registers_BASE+0X40A)
#define     BKDOOR                                                            (USB_Controller_Registers_BASE+0x40B)
#define     DMAEPSEL                                                          (USB_Controller_Registers_BASE+0x40C)
#define     OTGSTAIEN                                                         (USB_Controller_Registers_BASE+0x410)
#define     OTGSTAIR                                                          (USB_Controller_Registers_BASE+0x411)
#define     HCIN1_COUNTL                                                      (USB_Controller_Registers_BASE+0x414)
#define     HCIN1_COUNTH                                                      (USB_Controller_Registers_BASE+0x415)
#define     HCIN2_COUNTL                                                      (USB_Controller_Registers_BASE+0x416)
#define     HCIN2_COUNTH                                                      (USB_Controller_Registers_BASE+0x417)
#define     HCIN3_COUNTL                                                      (USB_Controller_Registers_BASE+0x420)
#define     HCIN3_COUNTH                                                      (USB_Controller_Registers_BASE+0x421)
#define     IDVBUSCTRL                                                        (USB_Controller_Registers_BASE+0x418)
#define     LINESTATUS                                                        (USB_Controller_Registers_BASE+0x419)
#define     DPDMCTRL                                                          (USB_Controller_Registers_BASE+0x41A)
#define     USB_PHYCTRL                                                       (USB_Controller_Registers_BASE+0x41B)
#define     DMANORMALLENL                                                     (USB_Controller_Registers_BASE+0x41C)
#define     DMANORMALLENH                                                     (USB_Controller_Registers_BASE+0x41D)
#define     AUTOINTIMER                                                       (USB_Controller_Registers_BASE+0x41E)
#define     USB_EFUSE_REF                                                     (USB_Controller_Registers_BASE+0x422)

//--------------Bits Location------------------------------------------//
#define     USBIRQ_periphirq                                                  4
#define     USBIRQ_locsofirq                                                  2
#define     USBIRQ_idleirq                                                    0

#define     USBstate_st3_st0_e                                                3
#define     USBstate_st3_st0_SHIFT                                            0
#define     USBstate_st3_st0_MASK                                             (0xF<<0)

#define     USBCtrl_forcebconn                                                7
#define     USBCtrl_busreq                                                    0

#define     USBstatus_id                                                      6
#define     USBstatus_conn                                                    1

#define     USBien_periphien                                                  4
#define     USBien_vbuserrien                                                 3
#define     USBien_locsofien                                                  2
#define     USBien_srpdetien                                                  1
#define     USBien_idleien                                                    0

#define     Hcep0ctrl_HcEp0Ctrl_e                                             3
#define     Hcep0ctrl_HcEp0Ctrl_SHIFT                                         0
#define     Hcep0ctrl_HcEp0Ctrl_MASK                                          (0xF<<0)

#define     Hcout1ctrl_HcEp1Ctrl_e                                            3
#define     Hcout1ctrl_HcEp1Ctrl_SHIFT                                        0
#define     Hcout1ctrl_HcEp1Ctrl_MASK                                         (0xF<<0)

#define     Hcout2ctrl_HcEp2Ctrl_e                                            3
#define     Hcout2ctrl_HcEp2Ctrl_SHIFT                                        0
#define     Hcout2ctrl_HcEp2Ctrl_MASK                                         (0xF<<0)

#define     Hcout3ctrl_HcEp3Ctrl_e                                            3
#define     Hcout3ctrl_HcEp3Ctrl_SHIFT                                        0
#define     Hcout3ctrl_HcEp3Ctrl_MASK                                         (0xF<<0)

#define     Hcout0err_doping                                                  6
#define     Hcout0err_Resend                                                  5
#define     Hcout0err_Errtype_e                                               4
#define     Hcout0err_Errtype_SHIFT                                           2
#define     Hcout0err_Errtype_MASK                                            (0x7<<2)
#define     Hcout0err_Errcount_e                                              1
#define     Hcout0err_Errcount_SHIFT                                          0
#define     Hcout0err_Errcount_MASK                                           (0x3<<0)

#define     Hcout1err_doping                                                  6
#define     Hcout1err_Resend                                                  5
#define     Hcout1err_Errtype_e                                               4
#define     Hcout1err_Errtype_SHIFT                                           2
#define     Hcout1err_Errtype_MASK                                            (0x7<<2)
#define     Hcout1err_Errcount_e                                              1
#define     Hcout1err_Errcount_SHIFT                                          0
#define     Hcout1err_Errcount_MASK                                           (0x3<<0)

#define     Hcout2err_doping                                                  6
#define     Hcout2err_Resend                                                  5
#define     Hcout2err_Errtype_e                                               4
#define     Hcout2err_Errtype_SHIFT                                           2
#define     Hcout2err_Errtype_MASK                                            (0x7<<2)
#define     Hcout2err_Errcount_e                                              1
#define     Hcout2err_Errcount_SHIFT                                          0
#define     Hcout2err_Errcount_MASK                                           (0x3<<0)

#define     Hcout3err_doping                                                  6
#define     Hcout3err_Resend                                                  5
#define     Hcout3err_Errtype_e                                               4
#define     Hcout3err_Errtype_SHIFT                                           2
#define     Hcout3err_Errtype_MASK                                            (0x7<<2)
#define     Hcout3err_Errcount_e                                              1
#define     Hcout3err_Errcount_SHIFT                                          0
#define     Hcout3err_Errcount_MASK                                           (0x3<<0)

#define     Hcin1ctrl_hcinxctrl_e                                             3
#define     Hcin1ctrl_hcinxctrl_SHIFT                                         0
#define     Hcin1ctrl_hcinxctrl_MASK                                          (0xF<<0)

#define     Hcin2ctrl_hcinxctrl_e                                             3
#define     Hcin2ctrl_hcinxctrl_SHIFT                                         0
#define     Hcin2ctrl_hcinxctrl_MASK                                          (0xF<<0)

#define     Hcin3ctrl_hcinxctrl_e                                             3
#define     Hcin3ctrl_hcinxctrl_SHIFT                                         0
#define     Hcin3ctrl_hcinxctrl_MASK                                          (0xF<<0)

#define     Hcin0err_underrien                                                7
#define     Hcin0err_Resend                                                   5
#define     Hcin0err_Errtype_e                                                4
#define     Hcin0err_Errtype_SHIFT                                            2
#define     Hcin0err_Errtype_MASK                                             (0x7<<2)
#define     Hcin0err_Errcount_e                                               1
#define     Hcin0err_Errcount_SHIFT                                           0
#define     Hcin0err_Errcount_MASK                                            (0x3<<0)

#define     Hcin1err_underrien                                                7
#define     Hcin1err_Resend                                                   5
#define     Hcin1err_Errtype_e                                                4
#define     Hcin1err_Errtype_SHIFT                                            2
#define     Hcin1err_Errtype_MASK                                             (0x7<<2)
#define     Hcin1err_Errcount_e                                               1
#define     Hcin1err_Errcount_SHIFT                                           0
#define     Hcin1err_Errcount_MASK                                            (0x3<<0)

#define     Hcin2err_underrien                                                7
#define     Hcin2err_Resend                                                   5
#define     Hcin2err_Errtype_e                                                4
#define     Hcin2err_Errtype_SHIFT                                            2
#define     Hcin2err_Errtype_MASK                                             (0x7<<2)
#define     Hcin2err_Errcount_e                                               1
#define     Hcin2err_Errcount_SHIFT                                           0
#define     Hcin2err_Errcount_MASK                                            (0x3<<0)

#define     Hcin3err_underrien                                                7
#define     Hcin3err_Resend                                                   5
#define     Hcin3err_Errtype_e                                                4
#define     Hcin3err_Errtype_SHIFT                                            2
#define     Hcin3err_Errtype_MASK                                             (0x7<<2)
#define     Hcin3err_Errcount_e                                               1
#define     Hcin3err_Errcount_SHIFT                                           0
#define     Hcin3err_Errcount_MASK                                            (0x3<<0)

#define     Hcportctrl_RstLengthCtrl_e                                        7
#define     Hcportctrl_RstLengthCtrl_SHIFT                                    6
#define     Hcportctrl_RstLengthCtrl_MASK                                     (0x3<<6)
#define     Hcportctrl_PortRst                                                5
#define     Hcportctrl_Testm_e                                                4
#define     Hcportctrl_Testm_SHIFT                                            0
#define     Hcportctrl_Testm_MASK                                             (0x1F<<0)

#define     Hcfrmnl_HcfrmL_e                                                  7
#define     Hcfrmnl_HcfrmL_SHIFT                                              0
#define     Hcfrmnl_HcfrmL_MASK                                               (0xFF<<0)

#define     Hcfrmnh_HcfrmH_e                                                  7
#define     Hcfrmnh_HcfrmH_SHIFT                                              0
#define     Hcfrmnh_HcfrmH_MASK                                               (0xFF<<0)

#define     Hcfrmremainl_hcfrmrmL_e                                           7
#define     Hcfrmremainl_hcfrmrmL_SHIFT                                       0
#define     Hcfrmremainl_hcfrmrmL_MASK                                        (0xFF<<0)

#define     Hcfrmremainh_hcfrmrmH_e                                           3
#define     Hcfrmremainh_hcfrmrmH_SHIFT                                       0
#define     Hcfrmremainh_hcfrmrmH_MASK                                        (0xF<<0)

#define     HCIN03ERRIRQ_hcinXerrirq_e                                        3
#define     HCIN03ERRIRQ_hcinXerrirq_SHIFT                                    0
#define     HCIN03ERRIRQ_hcinXerrirq_MASK                                     (0xF<<0)

#define     HCOUT03ERRIRQ_hcoutxerrirq_e                                      3
#define     HCOUT03ERRIRQ_hcoutxerrirq_SHIFT                                  0
#define     HCOUT03ERRIRQ_hcoutxerrirq_MASK                                   (0xF<<0)

#define     HCIN03ERRIEN_inxerrien_e                                          3
#define     HCIN03ERRIEN_inxerrien_SHIFT                                      0
#define     HCIN03ERRIEN_inxerrien_MASK                                       (0xF<<0)

#define     HCOUT03ERRIEN_outxerrien_e                                        3
#define     HCOUT03ERRIEN_outxerrien_SHIFT                                    0
#define     HCOUT03ERRIEN_outxerrien_MASK                                     (0xF<<0)

#define     Hcin0maxpck_HcIn0MaxP_e                                           6
#define     Hcin0maxpck_HcIn0MaxP_SHIFT                                       0
#define     Hcin0maxpck_HcIn0MaxP_MASK                                        (0x7F<<0)

#define     Hcin1maxpckl_HcIn1MaxPL_e                                         7
#define     Hcin1maxpckl_HcIn1MaxPL_SHIFT                                     0
#define     Hcin1maxpckl_HcIn1MaxPL_MASK                                      (0xFF<<0)

#define     Hcin2maxpckl_HcIn2MaxPL_e                                         7
#define     Hcin2maxpckl_HcIn2MaxPL_SHIFT                                     0
#define     Hcin2maxpckl_HcIn2MaxPL_MASK                                      (0xFF<<0)

#define     Hcin3maxpckl_HcIn3MaxPL_e                                         7
#define     Hcin3maxpckl_HcIn3MaxPL_SHIFT                                     0
#define     Hcin3maxpckl_HcIn3MaxPL_MASK                                      (0xFF<<0)

#define     Hcin1maxpckh_HcIn1MaxPH_e                                         2
#define     Hcin1maxpckh_HcIn1MaxPH_SHIFT                                     0
#define     Hcin1maxpckh_HcIn1MaxPH_MASK                                      (0x7<<0)

#define     Hcin2maxpckh_HcIn2MaxPH_e                                         2
#define     Hcin2maxpckh_HcIn2MaxPH_SHIFT                                     0
#define     Hcin2maxpckh_HcIn2MaxPH_MASK                                      (0x7<<0)

#define     Hcin3maxpckh_HcIn3MaxPH_e                                         2
#define     Hcin3maxpckh_HcIn3MaxPH_SHIFT                                     0
#define     Hcin3maxpckh_HcIn3MaxPH_MASK                                      (0x7<<0)

#define     Hcout1maxpckl_HcOut1MaxPL_e                                       7
#define     Hcout1maxpckl_HcOut1MaxPL_SHIFT                                   0
#define     Hcout1maxpckl_HcOut1MaxPL_MASK                                    (0xFF<<0)

#define     Hcout2maxpckl_HcOut2MaxPL_e                                       7
#define     Hcout2maxpckl_HcOut2MaxPL_SHIFT                                   0
#define     Hcout2maxpckl_HcOut2MaxPL_MASK                                    (0xFF<<0)

#define     Hcout3maxpckl_HcOut3MaxPL_e                                       7
#define     Hcout3maxpckl_HcOut3MaxPL_SHIFT                                   0
#define     Hcout3maxpckl_HcOut3MaxPL_MASK                                    (0xFF<<0)

#define     Hcout1maxpckh_HcOut1MaxPH_e                                       2
#define     Hcout1maxpckh_HcOut1MaxPH_SHIFT                                   0
#define     Hcout1maxpckh_HcOut1MaxPH_MASK                                    (0x7<<0)

#define     Hcout2maxpckh_HcOut2MaxPH_e                                       2
#define     Hcout2maxpckh_HcOut2MaxPH_SHIFT                                   0
#define     Hcout2maxpckh_HcOut2MaxPH_MASK                                    (0x7<<0)

#define     Hcout3maxpckh_HcOut3MaxPH_e                                       2
#define     Hcout3maxpckh_HcOut3MaxPH_SHIFT                                   0
#define     Hcout3maxpckh_HcOut3MaxPH_MASK                                    (0x7<<0)

#define     Out0bc_hcin0bc_Ep0Outbc_e                                         6
#define     Out0bc_hcin0bc_Ep0Outbc_SHIFT                                     0
#define     Out0bc_hcin0bc_Ep0Outbc_MASK                                      (0x7F<<0)

#define     In0bc_hcout0bc_Ep0Inbc_e                                          6
#define     In0bc_hcout0bc_Ep0Inbc_SHIFT                                      0
#define     In0bc_hcout0bc_Ep0Inbc_MASK                                       (0x7F<<0)

#define     Ep0cs_hcep0cs_hcsettoggle                                         6
#define     Ep0cs_hcep0cs_hcclrtoggle                                         5
#define     Ep0cs_hcep0cs_hcset                                               4
#define     Ep0cs_hcep0cs_Outbsy_hcinbsy                                      3
#define     Ep0cs_hcep0cs_Inbsy_hcoutbsy                                      2
#define     Ep0cs_hcep0cs_hsnak                                               1
#define     Ep0cs_hcep0cs_stall                                               0

#define     Out1bcl_hcin1bcl_Ep1bcL_e                                         7
#define     Out1bcl_hcin1bcl_Ep1bcL_SHIFT                                     0
#define     Out1bcl_hcin1bcl_Ep1bcL_MASK                                      (0xFF<<0)

#define     Out2bcl_hcin2bcl_Ep2bcL_e                                         7
#define     Out2bcl_hcin2bcl_Ep2bcL_SHIFT                                     0
#define     Out2bcl_hcin2bcl_Ep2bcL_MASK                                      (0xFF<<0)

#define     Out3bcl_hcin3bcl_Ep3bcL_e                                         7
#define     Out3bcl_hcin3bcl_Ep3bcL_SHIFT                                     0
#define     Out3bcl_hcin3bcl_Ep3bcL_MASK                                      (0xFF<<0)

#define     Out1bch_hcin1bch_Ep1bcH_e                                         3
#define     Out1bch_hcin1bch_Ep1bcH_SHIFT                                     0
#define     Out1bch_hcin1bch_Ep1bcH_MASK                                      (0xF<<0)

#define     Out2bch_hcin2bch_Ep2bcH_e                                         3
#define     Out2bch_hcin2bch_Ep2bcH_SHIFT                                     0
#define     Out2bch_hcin2bch_Ep2bcH_MASK                                      (0xF<<0)

#define     Out3bch_hcin3bch_Ep3bcH_e                                         3
#define     Out3bch_hcin3bch_Ep3bcH_SHIFT                                     0
#define     Out3bch_hcin3bch_Ep3bcH_MASK                                      (0xF<<0)

#define     Out1cs_hcin1cs_Autoin_hcautoout                                   4
#define     Out1cs_hcin1cs_Npak_hcnpak_e                                      3
#define     Out1cs_hcin1cs_Npak_hcnpak_SHIFT                                  2
#define     Out1cs_hcin1cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     Out1cs_hcin1cs_Busy_hcbusy                                        1
#define     Out1cs_hcin1cs_err                                                0

#define     Out2cs_hcin2cs_Autoin_hcautoout                                   4
#define     Out2cs_hcin2cs_Npak_hcnpak_e                                      3
#define     Out2cs_hcin2cs_Npak_hcnpak_SHIFT                                  2
#define     Out2cs_hcin2cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     Out2cs_hcin2cs_Busy_hcbusy                                        1
#define     Out2cs_hcin2cs_err                                                0

#define     Out3cs_hcin3cs_Autoin_hcautoout                                   4
#define     Out3cs_hcin3cs_Npak_hcnpak_e                                      3
#define     Out3cs_hcin3cs_Npak_hcnpak_SHIFT                                  2
#define     Out3cs_hcin3cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     Out3cs_hcin3cs_Busy_hcbusy                                        1
#define     Out3cs_hcin3cs_err                                                0

#define     Out1ctrl_hcin1ctrl_val                                            7
#define     Out1ctrl_hcin1ctrl_stall                                          6
#define     Out1ctrl_hcin1ctrl_Hcinisod_e                                     5
#define     Out1ctrl_hcin1ctrl_Hcinisod_SHIFT                                 4
#define     Out1ctrl_hcin1ctrl_Hcinisod_MASK                                  (0x3<<4)
#define     Out1ctrl_hcin1ctrl_Type_Hctype_e                                  3
#define     Out1ctrl_hcin1ctrl_Type_Hctype_SHIFT                              2
#define     Out1ctrl_hcin1ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     Out1ctrl_hcin1ctrl_Buf_hcbuf_e                                    1
#define     Out1ctrl_hcin1ctrl_Buf_hcbuf_SHIFT                                0
#define     Out1ctrl_hcin1ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     Out2ctrl_hcin2ctrl_val                                            7
#define     Out2ctrl_hcin2ctrl_stall                                          6
#define     Out2ctrl_hcin2ctrl_Hcinisod_e                                     5
#define     Out2ctrl_hcin2ctrl_Hcinisod_SHIFT                                 4
#define     Out2ctrl_hcin2ctrl_Hcinisod_MASK                                  (0x3<<4)
#define     Out2ctrl_hcin2ctrl_Type_Hctype_e                                  3
#define     Out2ctrl_hcin2ctrl_Type_Hctype_SHIFT                              2
#define     Out2ctrl_hcin2ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     Out2ctrl_hcin2ctrl_Buf_hcbuf_e                                    1
#define     Out2ctrl_hcin2ctrl_Buf_hcbuf_SHIFT                                0
#define     Out2ctrl_hcin2ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     Out3ctrl_hcin3ctrl_val                                            7
#define     Out3ctrl_hcin3ctrl_stall                                          6
#define     Out3ctrl_hcin3ctrl_Hcinisod_e                                     5
#define     Out3ctrl_hcin3ctrl_Hcinisod_SHIFT                                 4
#define     Out3ctrl_hcin3ctrl_Hcinisod_MASK                                  (0x3<<4)
#define     Out3ctrl_hcin3ctrl_Type_Hctype_e                                  3
#define     Out3ctrl_hcin3ctrl_Type_Hctype_SHIFT                              2
#define     Out3ctrl_hcin3ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     Out3ctrl_hcin3ctrl_Buf_hcbuf_e                                    1
#define     Out3ctrl_hcin3ctrl_Buf_hcbuf_SHIFT                                0
#define     Out3ctrl_hcin3ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     In1bcl_hcout1bcl_Ep1bcL_e                                         7
#define     In1bcl_hcout1bcl_Ep1bcL_SHIFT                                     0
#define     In1bcl_hcout1bcl_Ep1bcL_MASK                                      (0xFF<<0)

#define     In2bcl_hcout2bcl_Ep2bcL_e                                         7
#define     In2bcl_hcout2bcl_Ep2bcL_SHIFT                                     0
#define     In2bcl_hcout2bcl_Ep2bcL_MASK                                      (0xFF<<0)

#define     In3bcl_hcout3bcl_Ep3bcL_e                                         7
#define     In3bcl_hcout3bcl_Ep3bcL_SHIFT                                     0
#define     In3bcl_hcout3bcl_Ep3bcL_MASK                                      (0xFF<<0)

#define     In1bch_hcout1bch_Ep1bcH_e                                         2
#define     In1bch_hcout1bch_Ep1bcH_SHIFT                                     0
#define     In1bch_hcout1bch_Ep1bcH_MASK                                      (0x7<<0)

#define     In2bch_hcout2bch_Ep2bcH_e                                         2
#define     In2bch_hcout2bch_Ep2bcH_SHIFT                                     0
#define     In2bch_hcout2bch_Ep2bcH_MASK                                      (0x7<<0)

#define     In3bch_hcout3bch_Ep3bcH_e                                         2
#define     In3bch_hcout3bch_Ep3bcH_SHIFT                                     0
#define     In3bch_hcout3bch_Ep3bcH_MASK                                      (0x7<<0)

#define     In1cs_hcout1cs_Autoin_hcautoout                                   4
#define     In1cs_hcout1cs_Npak_hcnpak_e                                      3
#define     In1cs_hcout1cs_Npak_hcnpak_SHIFT                                  2
#define     In1cs_hcout1cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     In1cs_hcout1cs_Busy_hcbusy                                        1
#define     In1cs_hcout1cs_err                                                0

#define     In2cs_hcout2cs_Autoin_hcautoout                                   4
#define     In2cs_hcout2cs_Npak_hcnpak_e                                      3
#define     In2cs_hcout2cs_Npak_hcnpak_SHIFT                                  2
#define     In2cs_hcout2cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     In2cs_hcout2cs_Busy_hcbusy                                        1
#define     In2cs_hcout2cs_err                                                0

#define     In3cs_hcout3cs_Autoin_hcautoout                                   4
#define     In3cs_hcout3cs_Npak_hcnpak_e                                      3
#define     In3cs_hcout3cs_Npak_hcnpak_SHIFT                                  2
#define     In3cs_hcout3cs_Npak_hcnpak_MASK                                   (0x3<<2)
#define     In3cs_hcout3cs_Busy_hcbusy                                        1
#define     In3cs_hcout3cs_err                                                0

#define     In1ctrl_hcout1ctrl_val                                            7
#define     In1ctrl_hcout1ctrl_stall                                          6
#define     In1ctrl_hcout1ctrl_Isod_hcisod_e                                  5
#define     In1ctrl_hcout1ctrl_Isod_hcisod_SHIFT                              4
#define     In1ctrl_hcout1ctrl_Isod_hcisod_MASK                               (0x3<<4)
#define     In1ctrl_hcout1ctrl_Type_Hctype_e                                  3
#define     In1ctrl_hcout1ctrl_Type_Hctype_SHIFT                              2
#define     In1ctrl_hcout1ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     In1ctrl_hcout1ctrl_Buf_hcbuf_e                                    1
#define     In1ctrl_hcout1ctrl_Buf_hcbuf_SHIFT                                0
#define     In1ctrl_hcout1ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     In2ctrl_hcout2ctrl_val                                            7
#define     In2ctrl_hcout2ctrl_stall                                          6
#define     In2ctrl_hcout2ctrl_Isod_hcisod_e                                  5
#define     In2ctrl_hcout2ctrl_Isod_hcisod_SHIFT                              4
#define     In2ctrl_hcout2ctrl_Isod_hcisod_MASK                               (0x3<<4)
#define     In2ctrl_hcout2ctrl_Type_Hctype_e                                  3
#define     In2ctrl_hcout2ctrl_Type_Hctype_SHIFT                              2
#define     In2ctrl_hcout2ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     In2ctrl_hcout2ctrl_Buf_hcbuf_e                                    1
#define     In2ctrl_hcout2ctrl_Buf_hcbuf_SHIFT                                0
#define     In2ctrl_hcout2ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     In3ctrl_hcout3ctrl_val                                            7
#define     In3ctrl_hcout3ctrl_stall                                          6
#define     In3ctrl_hcout3ctrl_Isod_hcisod_e                                  5
#define     In3ctrl_hcout3ctrl_Isod_hcisod_SHIFT                              4
#define     In3ctrl_hcout3ctrl_Isod_hcisod_MASK                               (0x3<<4)
#define     In3ctrl_hcout3ctrl_Type_Hctype_e                                  3
#define     In3ctrl_hcout3ctrl_Type_Hctype_SHIFT                              2
#define     In3ctrl_hcout3ctrl_Type_Hctype_MASK                               (0x3<<2)
#define     In3ctrl_hcout3ctrl_Buf_hcbuf_e                                    1
#define     In3ctrl_hcout3ctrl_Buf_hcbuf_SHIFT                                0
#define     In3ctrl_hcout3ctrl_Buf_hcbuf_MASK                                 (0x3<<0)

#define     Fifo1dat_Fifo1dat_e                                               31
#define     Fifo1dat_Fifo1dat_SHIFT                                           0
#define     Fifo1dat_Fifo1dat_MASK                                            (0xFFFFFFFF<<0)

#define     Fifo2dat_Fifo2dat_e                                               31
#define     Fifo2dat_Fifo2dat_SHIFT                                           0
#define     Fifo2dat_Fifo2dat_MASK                                            (0xFFFFFFFF<<0)

#define     Fifo3dat_Fifo2dat_e                                               31
#define     Fifo3dat_Fifo2dat_SHIFT                                           0
#define     Fifo3dat_Fifo2dat_MASK                                            (0xFFFFFFFF<<0)

#define     Ep0indata_Ep0indata_e                                             511
#define     Ep0indata_Ep0indata_SHIFT                                         0
#define     Ep0indata_Ep0indata_MASK                                          (0xFFFFFFFF<<0)

#define     Ep0outdata_Ep0outdata_e                                           511
#define     Ep0outdata_Ep0outdata_SHIFT                                       0
#define     Ep0outdata_Ep0outdata_MASK                                        (0xFFFFFFFF<<0)

#define     Setupdat0_Setupdat_e                                              7
#define     Setupdat0_Setupdat_SHIFT                                          0
#define     Setupdat0_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat1_Setupdat_e                                              7
#define     Setupdat1_Setupdat_SHIFT                                          0
#define     Setupdat1_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat2_Setupdat_e                                              7
#define     Setupdat2_Setupdat_SHIFT                                          0
#define     Setupdat2_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat3_Setupdat_e                                              7
#define     Setupdat3_Setupdat_SHIFT                                          0
#define     Setupdat3_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat4_Setupdat_e                                              7
#define     Setupdat4_Setupdat_SHIFT                                          0
#define     Setupdat4_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat5_Setupdat_e                                              7
#define     Setupdat5_Setupdat_SHIFT                                          0
#define     Setupdat5_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat6_Setupdat_e                                              7
#define     Setupdat6_Setupdat_SHIFT                                          0
#define     Setupdat6_Setupdat_MASK                                           (0xFF<<0)

#define     Setupdat7_Setupdat_e                                              7
#define     Setupdat7_Setupdat_SHIFT                                          0
#define     Setupdat7_Setupdat_MASK                                           (0xFF<<0)

#define     OUT03IRQ_Outxirq_hcinxirq_e                                       3
#define     OUT03IRQ_Outxirq_hcinxirq_SHIFT                                   0
#define     OUT03IRQ_Outxirq_hcinxirq_MASK                                    (0xF<<0)

#define     IN03IRQ_Inxirq_hcoutxirq_e                                        3
#define     IN03IRQ_Inxirq_hcoutxirq_SHIFT                                    0
#define     IN03IRQ_Inxirq_hcoutxirq_MASK                                     (0xF<<0)

#define     Usbirq_hcusbirq_NTRIRQ                                            6
#define     Usbirq_hcusbirq_HSirq_hcHSirq                                     5
#define     Usbirq_hcusbirq_RstIrq                                            4
#define     Usbirq_hcusbirq_SuspIrq                                           3
#define     Usbirq_hcusbirq_SutokIrq                                          2
#define     Usbirq_hcusbirq_sofir                                             1
#define     Usbirq_hcusbirq_sudavir                                           0

#define     OUT03PNGIRQ_outxpng_e                                             3
#define     OUT03PNGIRQ_outxpng_SHIFT                                         0
#define     OUT03PNGIRQ_outxpng_MASK                                          (0xF<<0)

#define     OUT03IEN_Outxien_hcinxien_e                                       3
#define     OUT03IEN_Outxien_hcinxien_SHIFT                                   0
#define     OUT03IEN_Outxien_hcinxien_MASK                                    (0xF<<0)

#define     IN03IEN_Inxien_hcoutxien_e                                        3
#define     IN03IEN_Inxien_hcoutxien_SHIFT                                    0
#define     IN03IEN_Inxien_hcoutxien_MASK                                     (0xF<<0)

#define     Usbien_hcusbien_NTRIEN                                            6
#define     Usbien_hcusbien_HSien_hcHSien                                     5
#define     Usbien_hcusbien_RstIen                                            4
#define     Usbien_hcusbien_SuspIen                                           3
#define     Usbien_hcusbien_SutokIen                                          2
#define     Usbien_hcusbien_SOFIen                                            1
#define     Usbien_hcusbien_SudavIen                                          0

#define     OUT03PNGIEN_outxpie_e                                             3
#define     OUT03PNGIEN_outxpie_SHIFT                                         0
#define     OUT03PNGIEN_outxpie_MASK                                          (0xF<<0)

#define     IN03TOKIRQ_In03tokirq_e                                           3
#define     IN03TOKIRQ_In03tokirq_SHIFT                                       0
#define     IN03TOKIRQ_In03tokirq_MASK                                        (0xF<<0)

#define     OUT03TOKIRQ_Out03tokirq_e                                         3
#define     OUT03TOKIRQ_Out03tokirq_SHIFT                                     0
#define     OUT03TOKIRQ_Out03tokirq_MASK                                      (0xF<<0)

#define     IN03TOKIEN_In03tokien_e                                           3
#define     IN03TOKIEN_In03tokien_SHIFT                                       0
#define     IN03TOKIEN_In03tokien_MASK                                        (0xF<<0)

#define     OUT03TOKIEN_out03tokien_e                                         3
#define     OUT03TOKIEN_out03tokien_SHIFT                                     0
#define     OUT03TOKIEN_out03tokien_MASK                                      (0xF<<0)

#define     IVECT_Iv_e                                                        7
#define     IVECT_Iv_SHIFT                                                    1
#define     IVECT_Iv_MASK                                                     (0x7F<<1)

#define     EPRST_Fiforst                                                     6
#define     EPRST_togrst                                                      5
#define     EPRST_Io_hcio                                                     4
#define     EPRST_Epnum_e                                                     3
#define     EPRST_Epnum_SHIFT                                                 0
#define     EPRST_Epnum_MASK                                                  (0xF<<0)

#define     UsbCTRL_STUS_discon                                               6
#define     UsbCTRL_STUS_sigrsume                                             5
#define     UsbCTRL_STUS_usbspeed                                             1
#define     UsbCTRL_STUS_hclsmode                                             0

#define     FrmCNTL_FrL_e                                                     7
#define     FrmCNTL_FrL_SHIFT                                                 3
#define     FrmCNTL_FrL_MASK                                                  (0x1F<<3)
#define     FrmCNTL_MfrCNT_e                                                  2
#define     FrmCNTL_MfrCNT_SHIFT                                              0
#define     FrmCNTL_MfrCNT_MASK                                               (0x7<<0)

#define     FrmCNTH_FrH_e                                                     5
#define     FrmCNTH_FrH_SHIFT                                                 0
#define     FrmCNTH_FrH_MASK                                                  (0x3F<<0)

#define     Fnaddr_Fa_e                                                       6
#define     Fnaddr_Fa_SHIFT                                                   0
#define     Fnaddr_Fa_MASK                                                    (0x7F<<0)

#define     Clkgate_gosuspend                                                 0

#define     Fifoctrl_fifoauto                                                 5
#define     Fifoctrl_Io_hcio                                                  4
#define     Fifoctrl_Epnum_e                                                  3
#define     Fifoctrl_Epnum_SHIFT                                              0
#define     Fifoctrl_Epnum_MASK                                               (0xF<<0)

#define     OUT1STADDR_OUT1staddr_e                                           15
#define     OUT1STADDR_OUT1staddr_SHIFT                                       0
#define     OUT1STADDR_OUT1staddr_MASK                                        (0xFFFF<<0)

#define     OUT2STADDR_OUT2staddr_e                                           15
#define     OUT2STADDR_OUT2staddr_SHIFT                                       0
#define     OUT2STADDR_OUT2staddr_MASK                                        (0xFFFF<<0)

#define     OUT3STADDR_OUT3staddr_e                                           15
#define     OUT3STADDR_OUT3staddr_SHIFT                                       0
#define     OUT3STADDR_OUT3staddr_MASK                                        (0xFFFF<<0)

#define     IN1STADDR_IN1staddr_e                                             15
#define     IN1STADDR_IN1staddr_SHIFT                                         0
#define     IN1STADDR_IN1staddr_MASK                                          (0xFFFF<<0)

#define     IN2STADDR_IN2staddr_e                                             15
#define     IN2STADDR_IN2staddr_SHIFT                                         0
#define     IN2STADDR_IN2staddr_MASK                                          (0xFFFF<<0)

#define     IN3STADDR_IN3staddr_e                                             15
#define     IN3STADDR_IN3staddr_SHIFT                                         0
#define     IN3STADDR_IN3staddr_MASK                                          (0xFFFF<<0)

#define     USBEIRQ_usbirq                                                    7
#define     USBEIRQ_wakeupirq                                                 6
#define     USBEIRQ_resumeirq                                                 5
#define     USBEIRQ_con_disconir                                              4
#define     USBEIRQ_usbien                                                    3
#define     USBEIRQ_wakeupien                                                 2
#define     USBEIRQ_resumeien                                                 1
#define     USBEIRQ_con_disconien                                             0

#define     NAKOUTCTRL_Nakout3                                                2
#define     NAKOUTCTRL_Nakout2                                                1
#define     NAKOUTCTRL_nakout1                                                0

#define     HCINCTRL_Hcin3start                                               5
#define     HCINCTRL_Hcin3shpctrl                                             4
#define     HCINCTRL_Hcin2start                                               3
#define     HCINCTRL_Hcin2shpctrl                                             2
#define     HCINCTRL_Hcin1start                                               1
#define     HCINCTRL_Hcin1shpctrl                                             0

#define     HCINENDINT_Hcin3endir                                             6
#define     HCINENDINT_Hcin2endir                                             5
#define     HCINENDINT_Hcin1endir                                             4
#define     HCINENDINT_Hcin3endien                                            2
#define     HCINENDINT_Hcin2endien                                            1
#define     HCINENDINT_Hcin1endien                                            0

#define     OSHRTPCKIR_Oshrtpackir3                                           7
#define     OSHRTPCKIR_Oshrtpackir2                                           6
#define     OSHRTPCKIR_Oshrtpackir1                                           5
#define     OSHRTPCKIR_Oshrtpackien3                                          3
#define     OSHRTPCKIR_Oshrtpackien2                                          2
#define     OSHRTPCKIR_oshrtpackien1                                          1

#define     USBDEBUG_Debugen                                                  4
#define     USBDEBUG_dbgmode_e                                                3
#define     USBDEBUG_dbgmode_SHIFT                                            0
#define     USBDEBUG_dbgmode_MASK                                             (0xF<<0)

#define     HCOUTEMPINTCTRL_Hcout3rq                                          6
#define     HCOUTEMPINTCTRL_Hcout2irq                                         5
#define     HCOUTEMPINTCTRL_Hcout1irq                                         4
#define     HCOUTEMPINTCTRL_Hcout3irqen                                       2
#define     HCOUTEMPINTCTRL_Hcout2irqen                                       1
#define     HCOUTEMPINTCTRL_Hcout1irqen                                       0

#define     VDCTRL_vload                                                      4
#define     VDCTRL_vcontrol_e                                                 3
#define     VDCTRL_vcontrol_SHIFT                                             0
#define     VDCTRL_vcontrol_MASK                                              (0xF<<0)

#define     VDSTAT_vstatus_e                                                  7
#define     VDSTAT_vstatus_SHIFT                                              0
#define     VDSTAT_vstatus_MASK                                               (0xFF<<0)

#define     UDMAM_DMAFIFORST                                                  7
#define     UDMAM_MEMORYCTRL                                                  0

#define     BKDOOR_hsdisable                                                  7
#define     BKDOOR_spdrqd                                                     6
#define     BKDOOR_a_ndiscon                                                  5
#define     BKDOOR_Clk_120M_Neg                                               4
#define     BKDOOR_Hnp_b_switch                                               3
#define     BKDOOR_forcefs                                                    1
#define     BKDOOR_forcehs                                                    0

#define     DMAEPSEL_dmaepsel_e                                               3
#define     DMAEPSEL_dmaepsel_SHIFT                                           0
#define     DMAEPSEL_dmaepsel_MASK                                            (0xF<<0)

#define     OTGSTAIEN_bwtaconien                                              2
#define     OTGSTAIEN_awtbconien                                              1
#define     OTGSTAIEN_asuspien                                                0

#define     OTGSTAIR_bwtaconirq                                               2
#define     OTGSTAIR_awtbconirq                                               1
#define     OTGSTAIR_asuspirq                                                 0

#define     HCIN1_COUNTL_hcin1cntl_e                                          7
#define     HCIN1_COUNTL_hcin1cntl_SHIFT                                      0
#define     HCIN1_COUNTL_hcin1cntl_MASK                                       (0xFF<<0)

#define     HCIN1_COUNTH_hcin1cnth_e                                          7
#define     HCIN1_COUNTH_hcin1cnth_SHIFT                                      0
#define     HCIN1_COUNTH_hcin1cnth_MASK                                       (0xFF<<0)

#define     HCIN2_COUNTL_Hcin2cntl_e                                          7
#define     HCIN2_COUNTL_Hcin2cntl_SHIFT                                      0
#define     HCIN2_COUNTL_Hcin2cntl_MASK                                       (0xFF<<0)

#define     HCIN2_COUNTH_Hcin2cnth_e                                          7
#define     HCIN2_COUNTH_Hcin2cnth_SHIFT                                      0
#define     HCIN2_COUNTH_Hcin2cnth_MASK                                       (0xFF<<0)

#define     HCIN3_COUNTL_Hcin3cntl_e                                          7
#define     HCIN3_COUNTL_Hcin3cntl_SHIFT                                      0
#define     HCIN3_COUNTL_Hcin3cntl_MASK                                       (0xFF<<0)

#define     HCIN3_COUNTH_Hcin3cnth_e                                          7
#define     HCIN3_COUNTH_Hcin3cnth_SHIFT                                      0
#define     HCIN3_COUNTH_Hcin3cnth_MASK                                       (0xFF<<0)

#define     IDVBUSCTRL_SoftID                                                 3
#define     IDVBUSCTRL_SoftID_EN                                              2

#define     LINESTATUS_USB_ls_e                                               4
#define     LINESTATUS_USB_ls_SHIFT                                           3
#define     LINESTATUS_USB_ls_MASK                                            (0x3<<3)
#define     LINESTATUS_otgreset                                               0

#define     DPDMCTRL_Plugin                                                   6
#define     DPDMCTRL_LineDetEN                                                4
#define     DPDMCTRL_DMPuEn                                                   3
#define     DPDMCTRL_DPPuEn                                                   2
#define     DPDMCTRL_DMPdDis                                                  1
#define     DPDMCTRL_DPPdDis                                                  0

#define     USB_PHYCTRL_Phy_PLLEN                                             7
#define     USB_PHYCTRL_Phy_DALLUALLEN                                        6

#define     DMANORMALLENL_NM_LengthL_e                                        7
#define     DMANORMALLENL_NM_LengthL_SHIFT                                    0
#define     DMANORMALLENL_NM_LengthL_MASK                                     (0xFF<<0)

#define     DMANORMALLENH_NM_LengthH_e                                        7
#define     DMANORMALLENH_NM_LengthH_SHIFT                                    0
#define     DMANORMALLENH_NM_LengthH_MASK                                     (0xFF<<0)

#define     AUTOINTIMER_AUTOINTIMER_e                                         7
#define     AUTOINTIMER_AUTOINTIMER_SHIFT                                     0
#define     AUTOINTIMER_AUTOINTIMER_MASK                                      (0xFF<<0)

#define     USB_EFUSE_REF_Efacen                                              7
#define     USB_EFUSE_REF_R_SEL                                               6
#define     USB_EFUSE_REF_UREF_e                                              5
#define     USB_EFUSE_REF_UREF_SHIFT                                          0
#define     USB_EFUSE_REF_UREF_MASK                                           (0x3F<<0)

//--------------I2C-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     I2C_BASE                                                          0xc0130000
#define     I2C_CTL                                                           (I2C_BASE+0x0000)
#define     I2C_STAT                                                          (I2C_BASE+0x0004)
#define     I2C_ADDR                                                          (I2C_BASE+0x0008)
#define     I2C_DAT                                                           (I2C_BASE+0x000c)

//--------------Bits Location------------------------------------------//
#define     I2C_CTL_EN                                                        7
#define     I2C_CTL_MS                                                        6
#define     I2C_CTL_CLKSEL                                                    5
#define     I2C_CTL_IRQE                                                      4
#define     I2C_CTL_GBCC_e                                                    3
#define     I2C_CTL_GBCC_SHIFT                                                2
#define     I2C_CTL_GBCC_MASK                                                 (0x3<<2)
#define     I2C_CTL_RB                                                        1
#define     I2C_CTL_GACK                                                      0

#define     I2C_STAT_TCB                                                      8
#define     I2C_STAT_STPD                                                     7
#define     I2C_STAT_STAD                                                     6
#define     I2C_STAT_RWST                                                     5
#define     I2C_STAT_LBST                                                     4
#define     I2C_STAT_IRQP                                                     3
#define     I2C_STAT_BBB                                                      2
#define     I2C_STAT_BEB                                                      1
#define     I2C_STAT_RACK                                                     0

#define     I2C_ADDR_SDAD_e                                                   7
#define     I2C_ADDR_SDAD_SHIFT                                               1
#define     I2C_ADDR_SDAD_MASK                                                (0x7F<<1)

#define     I2C_DAT_TXRXDAT_e                                                 7
#define     I2C_DAT_TXRXDAT_SHIFT                                             0
#define     I2C_DAT_TXRXDAT_MASK                                              (0xFF<<0)

//--------------SPI_BOOT-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SPI_BOOT_BASE                                                     0xc0110000
#define     SPIBT_CTL                                                         (SPI_BOOT_BASE+0x0000)

//--------------Bits Location------------------------------------------//
#define     SPIBT_CTL_SPIBT_DBG_e                                             16
#define     SPIBT_CTL_SPIBT_DBG_SHIFT                                         14
#define     SPIBT_CTL_SPIBT_DBG_MASK                                          (0x7<<14)
#define     SPIBT_CTL_ROM5_SEL                                                13
#define     SPIBT_CTL_ROM4_SEL                                                12
#define     SPIBT_CTL_ROM3_SEL                                                11
#define     SPIBT_CTL_ROM2_SEL                                                10
#define     SPIBT_CTL_ROM1_SEL                                                9
#define     SPIBT_CTL_ROM0_SEL                                                8
#define     SPIBT_CTL_SPI_DELAY_e                                             5
#define     SPIBT_CTL_SPI_DELAY_SHIFT                                         4
#define     SPIBT_CTL_SPI_DELAY_MASK                                          (0x3<<4)
#define     SPIBT_CTL_SPIBT_MODE                                              3
#define     SPIBT_CTL_SPIBT_CLKDIV_e                                          2
#define     SPIBT_CTL_SPIBT_CLKDIV_SHIFT                                      1
#define     SPIBT_CTL_SPIBT_CLKDIV_MASK                                       (0x3<<1)
#define     SPIBT_CTL_SPIBT_EN                                                0

//--------------IRC-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     IRC_BASE                                                          0xc00f0000
#define     IRC_CTL                                                           (IRC_BASE+0x0050)
#define     IRC_STA                                                           (IRC_BASE+0x0054)
#define     IRC_CC                                                            (IRC_BASE+0x0058)
#define     IRC_KDC                                                           (IRC_BASE+0x005C)

//--------------Bits Location------------------------------------------//
#define     IRC_CTL_DBB_EN                                                    16
#define     IRC_CTL_DBC_e                                                     15
#define     IRC_CTL_DBC_SHIFT                                                 4
#define     IRC_CTL_DBC_MASK                                                  (0xFFF<<4)
#define     IRC_CTL_IRE                                                       3
#define     IRC_CTL_IIE                                                       2
#define     IRC_CTL_ICMS_e                                                    1
#define     IRC_CTL_ICMS_SHIFT                                                0
#define     IRC_CTL_ICMS_MASK                                                 (0x3<<0)

#define     IRC_STA_UCMP                                                      6
#define     IRC_STA_KDCM                                                      5
#define     IRC_STA_RCD                                                       4
#define     IRC_STA_IIP                                                       2
#define     IRC_STA_IREP                                                      0

#define     IRC_CC_CCRCV_e                                                    31
#define     IRC_CC_CCRCV_SHIFT                                                16
#define     IRC_CC_CCRCV_MASK                                                 (0xFFFF<<16)
#define     IRC_CC_ICCC_e                                                     15
#define     IRC_CC_ICCC_SHIFT                                                 0
#define     IRC_CC_ICCC_MASK                                                  (0xFFFF<<0)

#define     IRC_KDC_IKDC_e                                                    15
#define     IRC_KDC_IKDC_SHIFT                                                0
#define     IRC_KDC_IKDC_MASK                                                 (0xFFFF<<0)

//--------------UART0-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     UART0_BASE                                                        0xc01a0000
#define     UART0_CTL                                                         (UART0_BASE+0x0000)
#define     UART0_RXDAT                                                       (UART0_BASE+0x0004)
#define     UART0_TXDAT                                                       (UART0_BASE+0x0008)
#define     UART0_STA                                                         (UART0_BASE+0x000c)
#define     UART0_BR                                                          (UART0_BASE+0x0010)
#define     H5_CTL                                                            (UART0_BASE+0x0014)
#define     H5_BUFFER                                                         (UART0_BASE+0x0018)
#define     H5_STA                                                            (UART0_BASE+0x001C)
#define     H5_RXCONFIG                                                       (UART0_BASE+0x0020)
#define     H5_TXCONFIG                                                       (UART0_BASE+0x0024)
#define     H5_RSDTIMEOUT                                                     (UART0_BASE+0x0028)
#define     H5_ACKTIMEOUT                                                     (UART0_BASE+0x002c)
#define     H5_STA2                                                           (UART0_BASE+0x0030)

//--------------Bits Location------------------------------------------//
#define     UART0_CTL_RX0DISABLE                                              31
#define     UART0_CTL_TX0DISABLE                                              30
#define     UART0_CTL_DBGSEL0_e                                               25
#define     UART0_CTL_DBGSEL0_SHIFT                                           22
#define     UART0_CTL_DBGSEL0_MASK                                            (0xF<<22)
#define     UART0_CTL_TXAHB_DMA_SEL0                                          21
#define     UART0_CTL_LBEN0                                                   20
#define     UART0_CTL_TXIE0                                                   19
#define     UART0_CTL_RXIE0                                                   18
#define     UART0_CTL_TXDE0                                                   17
#define     UART0_CTL_RXDE0                                                   16
#define     UART0_CTL_EN0                                                     15
#define     UART0_CTL_RXAHB_DMA_SEL0                                          14
#define     UART0_CTL_RTSE0                                                   13
#define     UART0_CTL_AFE0                                                    12
#define     UART0_CTL_RDIC0_e                                                 11
#define     UART0_CTL_RDIC0_SHIFT                                             10
#define     UART0_CTL_RDIC0_MASK                                              (0x3<<10)
#define     UART0_CTL_TDIC0_e                                                 9
#define     UART0_CTL_TDIC0_SHIFT                                             8
#define     UART0_CTL_TDIC0_MASK                                              (0x3<<8)
#define     UART0_CTL_PRS0_e                                                  6
#define     UART0_CTL_PRS0_SHIFT                                              4
#define     UART0_CTL_PRS0_MASK                                               (0x7<<4)
#define     UART0_CTL_STPS0                                                   2
#define     UART0_CTL_DWLS0_e                                                 1
#define     UART0_CTL_DWLS0_SHIFT                                             0
#define     UART0_CTL_DWLS0_MASK                                              (0x3<<0)

#define     UART0_RXDAT_RXDAT0_e                                              7
#define     UART0_RXDAT_RXDAT0_SHIFT                                          0
#define     UART0_RXDAT_RXDAT0_MASK                                           (0xFF<<0)

#define     UART0_TXDAT_TXDAT0_e                                              7
#define     UART0_TXDAT_TXDAT0_SHIFT                                          0
#define     UART0_TXDAT_TXDAT0_MASK                                           (0xFF<<0)

#define     UART0_STA_UTBB0                                                   21
#define     UART0_STA_TXFL0_e                                                 20
#define     UART0_STA_TXFL0_SHIFT                                             16
#define     UART0_STA_TXFL0_MASK                                              (0x1F<<16)
#define     UART0_STA_RXFL0_e                                                 15
#define     UART0_STA_RXFL0_SHIFT                                             11
#define     UART0_STA_RXFL0_MASK                                              (0x1F<<11)
#define     UART0_STA_TFES0                                                   10
#define     UART0_STA_RFFS0                                                   9
#define     UART0_STA_RTSS0                                                   8
#define     UART0_STA_CTSS0                                                   7
#define     UART0_STA_TFFU0                                                   6
#define     UART0_STA_RFEM0                                                   5
#define     UART0_STA_RXST0                                                   4
#define     UART0_STA_TFER0                                                   3
#define     UART0_STA_RXER0                                                   2
#define     UART0_STA_TIP0                                                    1
#define     UART0_STA_RIP0                                                    0

#define     UART0_BR_TXBRDIV0_e                                               31
#define     UART0_BR_TXBRDIV0_SHIFT                                           16
#define     UART0_BR_TXBRDIV0_MASK                                            (0xFFFF<<16)
#define     UART0_BR_RXBRDIV0_e                                               15
#define     UART0_BR_RXBRDIV0_SHIFT                                           0
#define     UART0_BR_RXBRDIV0_MASK                                            (0xFFFF<<0)

#define     H5_CTL_H5ACKWT_e                                                  14
#define     H5_CTL_H5ACKWT_SHIFT                                              12
#define     H5_CTL_H5ACKWT_MASK                                               (0x7<<12)
#define     H5_CTL_H5SEQWT_e                                                  10
#define     H5_CTL_H5SEQWT_SHIFT                                              8
#define     H5_CTL_H5SEQWT_MASK                                               (0x7<<8)
#define     H5_CTL_H5OOFCTL                                                   7
#define     H5_CTL_H5OOFON                                                    6
#define     H5_CTL_H5OOFOFF                                                   5
#define     H5_CTL_H5DIC                                                      4
#define     H5_CTL_H5SLID_e                                                   3
#define     H5_CTL_H5SLID_SHIFT                                               1
#define     H5_CTL_H5SLID_MASK                                                (0x7<<1)
#define     H5_CTL_H5EN                                                       0

#define     H5_BUFFER_TXBUF6FULL                                              27
#define     H5_BUFFER_TXBUF6EMPTY                                             26
#define     H5_BUFFER_RXBUF6FULL                                              25
#define     H5_BUFFER_RXBUF6EMPTY                                             24
#define     H5_BUFFER_TXBUF5FULL                                              23
#define     H5_BUFFER_TXBUF5EMPTY                                             22
#define     H5_BUFFER_RXBUF5FULL                                              21
#define     H5_BUFFER_RXBUF5EMPTY                                             20
#define     H5_BUFFER_TXBUF4FULL                                              19
#define     H5_BUFFER_TXBUF4EMPTY                                             18
#define     H5_BUFFER_RXBUF4FULL                                              17
#define     H5_BUFFER_RXBUF4EMPTY                                             16
#define     H5_BUFFER_TXBUF3FULL                                              15
#define     H5_BUFFER_TXBUF3EMPTY                                             14
#define     H5_BUFFER_RXBUF3FULL                                              13
#define     H5_BUFFER_RXBUF3EMPTY                                             12
#define     H5_BUFFER_TXBUF2FULL                                              11
#define     H5_BUFFER_TXBUF2EMPTY                                             10
#define     H5_BUFFER_RXBUF2FULL                                              9
#define     H5_BUFFER_RXBUF2EMPTY                                             8
#define     H5_BUFFER_TXBUF1FULL                                              7
#define     H5_BUFFER_TXBUF1EMPTY                                             6
#define     H5_BUFFER_RXBUF1FULL                                              5
#define     H5_BUFFER_RXBUF1EMPTY                                             4
#define     H5_BUFFER_TXBUF0FULL                                              3
#define     H5_BUFFER_TXBUF0EMPTY                                             2
#define     H5_BUFFER_RXBUF0FULL                                              1
#define     H5_BUFFER_RXBUF0EMPTY                                             0

#define     H5_STA_H5RXLENRD_e                                                31
#define     H5_STA_H5RXLENRD_SHIFT                                            16
#define     H5_STA_H5RXLENRD_MASK                                             (0xFFFF<<16)
#define     H5_STA_H5ACKTX_e                                                  14
#define     H5_STA_H5ACKTX_SHIFT                                              12
#define     H5_STA_H5ACKTX_MASK                                               (0x7<<12)
#define     H5_STA_H5SEQTX_e                                                  10
#define     H5_STA_H5SEQTX_SHIFT                                              8
#define     H5_STA_H5SEQTX_MASK                                               (0x7<<8)
#define     H5_STA_H5RXLENERR                                                 4
#define     H5_STA_H5TXBUFFULL                                                3
#define     H5_STA_H5TXBUFEMPTY                                               2
#define     H5_STA_H5RXBUFFULL                                                1
#define     H5_STA_H5RXBUFEMPTY                                               0

#define     H5_RXCONFIG_H5RXADDR_e                                            31
#define     H5_RXCONFIG_H5RXADDR_SHIFT                                        16
#define     H5_RXCONFIG_H5RXADDR_MASK                                         (0xFFFF<<16)
#define     H5_RXCONFIG_H5RMAXLEN_e                                           15
#define     H5_RXCONFIG_H5RMAXLEN_SHIFT                                       4
#define     H5_RXCONFIG_H5RMAXLEN_MASK                                        (0xFFF<<4)
#define     H5_RXCONFIG_H5RXNUM_e                                             3
#define     H5_RXCONFIG_H5RXNUM_SHIFT                                         0
#define     H5_RXCONFIG_H5RXNUM_MASK                                          (0xF<<0)

#define     H5_TXCONFIG_H5TXADDR_e                                            31
#define     H5_TXCONFIG_H5TXADDR_SHIFT                                        16
#define     H5_TXCONFIG_H5TXADDR_MASK                                         (0xFFFF<<16)
#define     H5_TXCONFIG_H5TMAXLEN_e                                           15
#define     H5_TXCONFIG_H5TMAXLEN_SHIFT                                       4
#define     H5_TXCONFIG_H5TMAXLEN_MASK                                        (0xFFF<<4)
#define     H5_TXCONFIG_H5TXNUM_e                                             3
#define     H5_TXCONFIG_H5TXNUM_SHIFT                                         0
#define     H5_TXCONFIG_H5TXNUM_MASK                                          (0xF<<0)

#define     H5_RSDTIMEOUT_H5RSDTOCNT_e                                        23
#define     H5_RSDTIMEOUT_H5RSDTOCNT_SHIFT                                    0
#define     H5_RSDTIMEOUT_H5RSDTOCNT_MASK                                     (0xFFFFFF<<0)

#define     H5_ACKTIMEOUT_H5ACKTOCNT_e                                        23
#define     H5_ACKTIMEOUT_H5ACKTOCNT_SHIFT                                    0
#define     H5_ACKTIMEOUT_H5ACKTOCNT_MASK                                     (0xFFFFFF<<0)

#define     H5_STA2_H5ACKRX_e                                                 14
#define     H5_STA2_H5ACKRX_SHIFT                                             12
#define     H5_STA2_H5ACKRX_MASK                                              (0x7<<12)
#define     H5_STA2_H5SEQRX_e                                                 10
#define     H5_STA2_H5SEQRX_SHIFT                                             8
#define     H5_STA2_H5SEQRX_MASK                                              (0x7<<8)
#define     H5_STA2_EXPRXSEQ_e                                                6
#define     H5_STA2_EXPRXSEQ_SHIFT                                            4
#define     H5_STA2_EXPRXSEQ_MASK                                             (0x7<<4)
#define     H5_STA2_CRCERR                                                    3
#define     H5_STA2_LENERR                                                    2
#define     H5_STA2_HCKERR                                                    1
#define     H5_STA2_SEQERR                                                    0

//--------------UART1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     UART1_BASE                                                        0xc00f0000
#define     UART1_CTL                                                         (UART1_BASE+0x0000)
#define     UART1_RXDAT                                                       (UART1_BASE+0x0004)
#define     UART1_TXDAT                                                       (UART1_BASE+0x0008)
#define     UART1_STA                                                         (UART1_BASE+0x000c)
#define     UART1_BR                                                          (UART1_BASE+0x0010)

//--------------Bits Location------------------------------------------//
#define     UART1_CTL_RX1DISABLE                                              31
#define     UART1_CTL_TX1DISABLE                                              30
#define     UART1_CTL_DBGSEL1_e                                               25
#define     UART1_CTL_DBGSEL1_SHIFT                                           22
#define     UART1_CTL_DBGSEL1_MASK                                            (0xF<<22)
#define     UART1_CTL_TXAHB_DMA_SEL1                                          21
#define     UART1_CTL_LBEN1                                                   20
#define     UART1_CTL_TXIE1                                                   19
#define     UART1_CTL_RXIE1                                                   18
#define     UART1_CTL_TXDE1                                                   17
#define     UART1_CTL_RXDE1                                                   16
#define     UART1_CTL_EN1                                                     15
#define     UART1_CTL_RXAHB_DMA_SEL1                                          14
#define     UART1_CTL_RTSE1                                                   13
#define     UART1_CTL_AFE1                                                    12
#define     UART1_CTL_RDIC1_e                                                 11
#define     UART1_CTL_RDIC1_SHIFT                                             10
#define     UART1_CTL_RDIC1_MASK                                              (0x3<<10)
#define     UART1_CTL_TDIC1_e                                                 9
#define     UART1_CTL_TDIC1_SHIFT                                             8
#define     UART1_CTL_TDIC1_MASK                                              (0x3<<8)
#define     UART1_CTL_PRS1_e                                                  6
#define     UART1_CTL_PRS1_SHIFT                                              4
#define     UART1_CTL_PRS1_MASK                                               (0x7<<4)
#define     UART1_CTL_STPS1                                                   2
#define     UART1_CTL_DWLS1_e                                                 1
#define     UART1_CTL_DWLS1_SHIFT                                             0
#define     UART1_CTL_DWLS1_MASK                                              (0x3<<0)

#define     UART1_RXDAT_RXDAT1_e                                              7
#define     UART1_RXDAT_RXDAT1_SHIFT                                          0
#define     UART1_RXDAT_RXDAT1_MASK                                           (0xFF<<0)

#define     UART1_TXDAT_TXDAT1_e                                              7
#define     UART1_TXDAT_TXDAT1_SHIFT                                          0
#define     UART1_TXDAT_TXDAT1_MASK                                           (0xFF<<0)

#define     UART1_STA_UTBB1                                                   21
#define     UART1_STA_TXFL1_e                                                 20
#define     UART1_STA_TXFL1_SHIFT                                             16
#define     UART1_STA_TXFL1_MASK                                              (0x1F<<16)
#define     UART1_STA_RXFL1_e                                                 15
#define     UART1_STA_RXFL1_SHIFT                                             11
#define     UART1_STA_RXFL1_MASK                                              (0x1F<<11)
#define     UART1_STA_TFES1                                                   10
#define     UART1_STA_RFFS1                                                   9
#define     UART1_STA_RTSS1                                                   8
#define     UART1_STA_CTSS1                                                   7
#define     UART1_STA_TFFU1                                                   6
#define     UART1_STA_RFEM1                                                   5
#define     UART1_STA_RXST1                                                   4
#define     UART1_STA_TFER1                                                   3
#define     UART1_STA_RXER1                                                   2
#define     UART1_STA_TIP1                                                    1
#define     UART1_STA_RIP1                                                    0

#define     UART1_BR_TXBRDIV1_e                                               31
#define     UART1_BR_TXBRDIV1_SHIFT                                           16
#define     UART1_BR_TXBRDIV1_MASK                                            (0xFFFF<<16)
#define     UART1_BR_RXBRDIV1_e                                               15
#define     UART1_BR_RXBRDIV1_SHIFT                                           0
#define     UART1_BR_RXBRDIV1_MASK                                            (0xFFFF<<0)

//--------------SPI-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SPI_BASE                                                          0xC0100000
#define     SPI_CTL                                                           (SPI_BASE+0x0000)
#define     SPI_DRQ                                                           (SPI_BASE+0x0004)
#define     SPI_STA                                                           (SPI_BASE+0x0008)
#define     SPI_CLKDIV                                                        (SPI_BASE+0x000C)
#define     SPI_TXDAT                                                         (SPI_BASE+0x0010)
#define     SPI_RXDAT                                                         (SPI_BASE+0x0014)
#define     SPI_BCL                                                           (SPI_BASE+0x0018)
#define     SPI_BCH                                                           (SPI_BASE+0x001C)
#define     SPI_DEBUG                                                         (SPI_BASE+0x0020)
#define     SPI_RAND                                                          (SPI_BASE+0x0024)
#define     SPI_SEED                                                          (SPI_BASE+0x0028)

//--------------Bits Location------------------------------------------//
#define     SPI_CTL_RLRS0_e                                                   11
#define     SPI_CTL_RLRS0_SHIFT                                               8
#define     SPI_CTL_RLRS0_MASK                                                (0xF<<8)
#define     SPI_CTL_SPI_EN                                                    7
#define     SPI_CTL_SPI_MS                                                    6
#define     SPI_CTL_SPI_LM                                                    5
#define     SPI_CTL_SPI_SS                                                    4
#define     SPI_CTL_SPI_MODE_e                                                3
#define     SPI_CTL_SPI_MODE_SHIFT                                            2
#define     SPI_CTL_SPI_MODE_MASK                                             (0x3<<2)
#define     SPI_CTL_SPI_WR_e                                                  1
#define     SPI_CTL_SPI_WR_SHIFT                                              0
#define     SPI_CTL_SPI_WR_MASK                                               (0x3<<0)

#define     SPI_DRQ_SPI_TDRQ_EN                                               7
#define     SPI_DRQ_SPI_RDRQ_EN                                               6
#define     SPI_DRQ_TXAHB_DMA_SEL                                             5
#define     SPI_DRQ_RXAHB_DMA_SEL                                             4
#define     SPI_DRQ_SPI_TIRQ_EN                                               3
#define     SPI_DRQ_SPI_RIRQ_EN                                               2
#define     SPI_DRQ_SPI_TIRQ_PD                                               1
#define     SPI_DRQ_SPI_RIRQ_PD                                               0

#define     SPI_STA_TXEM                                                      7
#define     SPI_STA_TXFU                                                      6
#define     SPI_STA_RXEM                                                      5
#define     SPI_STA_RXFU                                                      4
#define     SPI_STA_SPI_BUSY                                                  3
#define     SPI_STA_TXER                                                      2
#define     SPI_STA_RXER                                                      1

#define     SPI_CLKDIV_DAWS                                                   6
#define     SPI_CLKDIV_TXCEB                                                  5
#define     SPI_CLKDIV_RXCEB                                                  4
#define     SPI_CLKDIV_SPI_CLKDIV_e                                           3
#define     SPI_CLKDIV_SPI_CLKDIV_SHIFT                                       0
#define     SPI_CLKDIV_SPI_CLKDIV_MASK                                        (0xF<<0)

#define     SPI_TXDAT_SPI_TXDAT_e                                             31
#define     SPI_TXDAT_SPI_TXDAT_SHIFT                                         0
#define     SPI_TXDAT_SPI_TXDAT_MASK                                          (0xFFFFFFFF<<0)

#define     SPI_RXDAT_SPI_RXDAT_e                                             31
#define     SPI_RXDAT_SPI_RXDAT_SHIFT                                         0
#define     SPI_RXDAT_SPI_RXDAT_MASK                                          (0xFFFFFFFF<<0)

#define     SPI_BCL_SPI_BCL_e                                                 15
#define     SPI_BCL_SPI_BCL_SHIFT                                             0
#define     SPI_BCL_SPI_BCL_MASK                                              (0xFFFF<<0)

#define     SPI_BCH_SPI_IO                                                    7
#define     SPI_BCH_SPI_DELAY_EN                                              6
#define     SPI_BCH_SPI_DELAY_e                                               5
#define     SPI_BCH_SPI_DELAY_SHIFT                                           4
#define     SPI_BCH_SPI_DELAY_MASK                                            (0x3<<4)
#define     SPI_BCH_SPI_RS                                                    3

#define     SPI_DEBUG_SPI_LB                                                  7
#define     SPI_DEBUG_SPI_DEBUG_e                                             6
#define     SPI_DEBUG_SPI_DEBUG_SHIFT                                         0
#define     SPI_DEBUG_SPI_DEBUG_MASK                                          (0x7F<<0)

#define     SPI_RAND_RAND_PAUSE                                               5
#define     SPI_RAND_spi_3wire_rw                                             4
#define     SPI_RAND_spi_3wire_en                                             3
#define     SPI_RAND_RAND_SEED                                                2
#define     SPI_RAND_RAND_TXEN                                                1
#define     SPI_RAND_RAND_RXEN                                                0

#define     SPI_SEED_RAND_TST_SEED_e                                          31
#define     SPI_SEED_RAND_TST_SEED_SHIFT                                      0
#define     SPI_SEED_RAND_TST_SEED_MASK                                       (0xFFFFFFFF<<0)

//--------------SPI1-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SPI1_BASE                                                         0xC0150000
#define     SPI1_CTL                                                          (SPI1_BASE+0x0000)
#define     SPI1_DRQ                                                          (SPI1_BASE+0x0004)
#define     SPI1_STA                                                          (SPI1_BASE+0x0008)
#define     SPI1_CLKDIV                                                       (SPI1_BASE+0x000C)
#define     SPI1_TXDAT                                                        (SPI1_BASE+0x0010)
#define     SPI1_RXDAT                                                        (SPI1_BASE+0x0014)
#define     SPI1_BCL                                                          (SPI1_BASE+0x0018)
#define     SPI1_BCH                                                          (SPI1_BASE+0x001C)
#define     SPI1_DEBUG                                                        (SPI1_BASE+0x0020)
#define     SPI1_RAND                                                         (SPI1_BASE+0x0024)
#define     SPI1_SEED                                                         (SPI1_BASE+0x0028)

//--------------Bits Location------------------------------------------//
#define     SPI1_CTL_RLRS1_e                                                  11
#define     SPI1_CTL_RLRS1_SHIFT                                              8
#define     SPI1_CTL_RLRS1_MASK                                               (0xF<<8)
#define     SPI1_CTL_SPI1_EN                                                  7
#define     SPI1_CTL_SPI1_MS                                                  6
#define     SPI1_CTL_SPI1_LM                                                  5
#define     SPI1_CTL_SPI1_SS                                                  4
#define     SPI1_CTL_SPI1_MODE_e                                              3
#define     SPI1_CTL_SPI1_MODE_SHIFT                                          2
#define     SPI1_CTL_SPI1_MODE_MASK                                           (0x3<<2)
#define     SPI1_CTL_SPI1_WR_e                                                1
#define     SPI1_CTL_SPI1_WR_SHIFT                                            0
#define     SPI1_CTL_SPI1_WR_MASK                                             (0x3<<0)

#define     SPI1_DRQ_SPI1_TDRQ_EN                                             7
#define     SPI1_DRQ_SPI1_RDRQ_EN                                             6
#define     SPI1_DRQ_TXAHB_DMA_SEL                                            5
#define     SPI1_DRQ_RXAHB_DMA_SEL                                            4
#define     SPI1_DRQ_SPI1_TIRQ_EN                                             3
#define     SPI1_DRQ_SPI1_RIRQ_EN                                             2
#define     SPI1_DRQ_SPI1_TIRQ_PD                                             1
#define     SPI1_DRQ_SPI1_RIRQ_PD                                             0

#define     SPI1_STA_TXEM                                                     7
#define     SPI1_STA_TXFU                                                     6
#define     SPI1_STA_RXEM                                                     5
#define     SPI1_STA_RXFU                                                     4
#define     SPI1_STA_SPI1_BUSY                                                3
#define     SPI1_STA_TXER                                                     2
#define     SPI1_STA_RXER                                                     1

#define     SPI1_CLKDIV_DAWS                                                  6
#define     SPI1_CLKDIV_TXCEB                                                 5
#define     SPI1_CLKDIV_RXCEB                                                 4
#define     SPI1_CLKDIV_SPI1_CLKDIV_e                                         3
#define     SPI1_CLKDIV_SPI1_CLKDIV_SHIFT                                     0
#define     SPI1_CLKDIV_SPI1_CLKDIV_MASK                                      (0xF<<0)

#define     SPI1_TXDAT_SPI1_TXDAT_e                                           31
#define     SPI1_TXDAT_SPI1_TXDAT_SHIFT                                       0
#define     SPI1_TXDAT_SPI1_TXDAT_MASK                                        (0xFFFFFFFF<<0)

#define     SPI1_RXDAT_SPI1_RXDAT_e                                           31
#define     SPI1_RXDAT_SPI1_RXDAT_SHIFT                                       0
#define     SPI1_RXDAT_SPI1_RXDAT_MASK                                        (0xFFFFFFFF<<0)

#define     SPI1_BCL_SPI1_BCL_e                                               15
#define     SPI1_BCL_SPI1_BCL_SHIFT                                           0
#define     SPI1_BCL_SPI1_BCL_MASK                                            (0xFFFF<<0)

#define     SPI1_BCH_SPI1_IO                                                  7
#define     SPI1_BCH_SPI1_DELAY_EN                                            6
#define     SPI1_BCH_SPI1_DELAY_e                                             5
#define     SPI1_BCH_SPI1_DELAY_SHIFT                                         4
#define     SPI1_BCH_SPI1_DELAY_MASK                                          (0x3<<4)
#define     SPI1_BCH_SPI1_RS                                                  3

#define     SPI1_DEBUG_SPI1_LB                                                7
#define     SPI1_DEBUG_SPI1_DEBUG_e                                           6
#define     SPI1_DEBUG_SPI1_DEBUG_SHIFT                                       0
#define     SPI1_DEBUG_SPI1_DEBUG_MASK                                        (0x7F<<0)

#define     SPI1_RAND_RAND_PAUSE                                              5
#define     SPI1_RAND_SPI1_3wire_rw                                           4
#define     SPI1_RAND_SPI1_3wire_en                                           3
#define     SPI1_RAND_RAND_SEED                                               2
#define     SPI1_RAND_RAND_TXEN                                               1
#define     SPI1_RAND_RAND_RXEN                                               0

#define     SPI1_SEED_RAND_TST_SEED_e                                         31
#define     SPI1_SEED_RAND_TST_SEED_SHIFT                                     0
#define     SPI1_SEED_RAND_TST_SEED_MASK                                      (0xFFFFFFFF<<0)

//--------------LCDC-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     LCDC_BASE                                                         0xc00d0000
#define     LCD_CTL                                                           (LCDC_BASE+0x0000)
#define     LCD_CLKCTL                                                        (LCDC_BASE+0x0004)
#define     EXTMEM_CTL                                                        (LCDC_BASE+0x0008)
#define     EXTMEM_CLKCTL                                                     (LCDC_BASE+0x000c)
#define     EXTMEM_DATA                                                       (LCDC_BASE+0x0010)
#define     LCD_IF_PCS                                                        (LCDC_BASE+0x0014)

//--------------Bits Location------------------------------------------//
#define     LCD_CTL_LCDFI                                                     31
#define     LCD_CTL_PC_EN                                                     30
#define     LCD_CTL_FOVF                                                      17
#define     LCD_CTL_FIFOET                                                    10
#define     LCD_CTL_EMDE                                                      7
#define     LCD_CTL_FORMATS                                                   4
#define     LCD_CTL_SEQ                                                       3
#define     LCD_CTL_SDT                                                       1
#define     LCD_CTL_EN                                                        0

#define     LCD_CLKCTL_CLKHDU_e                                               11
#define     LCD_CLKCTL_CLKHDU_SHIFT                                           8
#define     LCD_CLKCTL_CLKHDU_MASK                                            (0xF<<8)
#define     LCD_CLKCTL_CLKLDU_e                                               3
#define     LCD_CLKCTL_CLKLDU_SHIFT                                           0
#define     LCD_CLKCTL_CLKLDU_MASK                                            (0xF<<0)

#define     EXTMEM_CTL_CESEL_e                                                31
#define     EXTMEM_CTL_CESEL_SHIFT                                            29
#define     EXTMEM_CTL_CESEL_MASK                                             (0x7<<29)
#define     EXTMEM_CTL_IFSEL                                                  8
#define     EXTMEM_CTL_RS                                                     0

#define     EXTMEM_CLKCTL_EXCLKH_e                                            11
#define     EXTMEM_CLKCTL_EXCLKH_SHIFT                                        8
#define     EXTMEM_CLKCTL_EXCLKH_MASK                                         (0xF<<8)
#define     EXTMEM_CLKCTL_EXCLKL_e                                            3
#define     EXTMEM_CLKCTL_EXCLKL_SHIFT                                        0
#define     EXTMEM_CLKCTL_EXCLKL_MASK                                         (0xF<<0)

#define     EXTMEM_DATA_EXT_DATAH_e                                           15
#define     EXTMEM_DATA_EXT_DATAH_SHIFT                                       8
#define     EXTMEM_DATA_EXT_DATAH_MASK                                        (0xFF<<8)
#define     EXTMEM_DATA_EXT_DATAL_e                                           7
#define     EXTMEM_DATA_EXT_DATAL_SHIFT                                       0
#define     EXTMEM_DATA_EXT_DATAL_MASK                                        (0xFF<<0)

#define     LCD_IF_PCS_PCS_e                                                  7
#define     LCD_IF_PCS_PCS_SHIFT                                              0
#define     LCD_IF_PCS_PCS_MASK                                               (0xFF<<0)

//--------------SEG_SREEN-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SEG_SREEN_BASE                                                    0xc00e0000
#define     SEG_SREEN_CTL                                                     (SEG_SREEN_BASE+0x0000)
#define     SEG_SREEN_DATA0                                                   (SEG_SREEN_BASE+0x0004)
#define     SEG_SREEN_DATA1                                                   (SEG_SREEN_BASE+0x0008)
#define     SEG_SREEN_DATA2                                                   (SEG_SREEN_BASE+0x000C)
#define     SEG_SREEN_DATA3                                                   (SEG_SREEN_BASE+0x0010)
#define     SEG_SREEN_DATA4                                                   (SEG_SREEN_BASE+0x0014)
#define     SEG_SREEN_DATA5                                                   (SEG_SREEN_BASE+0x0018)

//--------------Bits Location------------------------------------------//
#define     SEG_SREEN_CTL_LCD_POWER                                           31
#define     SEG_SREEN_CTL_LED_COM_DZ_e                                        9
#define     SEG_SREEN_CTL_LED_COM_DZ_SHIFT                                    8
#define     SEG_SREEN_CTL_LED_COM_DZ_MASK                                     (0x3<<8)
#define     SEG_SREEN_CTL_SEGOFF                                              7
#define     SEG_SREEN_CTL_LCD_OUT_EN                                          5
#define     SEG_SREEN_CTL_REFRSH                                              4
#define     SEG_SREEN_CTL_MODE_SEL_e                                          3
#define     SEG_SREEN_CTL_MODE_SEL_SHIFT                                      0
#define     SEG_SREEN_CTL_MODE_SEL_MASK                                       (0xF<<0)

#define     SEG_SREEN_DATA0_COM0_byte3_e                                      31
#define     SEG_SREEN_DATA0_COM0_byte3_SHIFT                                  24
#define     SEG_SREEN_DATA0_COM0_byte3_MASK                                   (0xFF<<24)
#define     SEG_SREEN_DATA0_COM0_byte2_e                                      23
#define     SEG_SREEN_DATA0_COM0_byte2_SHIFT                                  16
#define     SEG_SREEN_DATA0_COM0_byte2_MASK                                   (0xFF<<16)
#define     SEG_SREEN_DATA0_COM0_byte1_e                                      15
#define     SEG_SREEN_DATA0_COM0_byte1_SHIFT                                  8
#define     SEG_SREEN_DATA0_COM0_byte1_MASK                                   (0xFF<<8)
#define     SEG_SREEN_DATA0_COM0_byte0_e                                      7
#define     SEG_SREEN_DATA0_COM0_byte0_SHIFT                                  0
#define     SEG_SREEN_DATA0_COM0_byte0_MASK                                   (0xFF<<0)

#define     SEG_SREEN_DATA1_COM1_byte3_e                                      31
#define     SEG_SREEN_DATA1_COM1_byte3_SHIFT                                  24
#define     SEG_SREEN_DATA1_COM1_byte3_MASK                                   (0xFF<<24)
#define     SEG_SREEN_DATA1_COM1_byte2_e                                      23
#define     SEG_SREEN_DATA1_COM1_byte2_SHIFT                                  16
#define     SEG_SREEN_DATA1_COM1_byte2_MASK                                   (0xFF<<16)
#define     SEG_SREEN_DATA1_COM1_byte1_e                                      15
#define     SEG_SREEN_DATA1_COM1_byte1_SHIFT                                  8
#define     SEG_SREEN_DATA1_COM1_byte1_MASK                                   (0xFF<<8)
#define     SEG_SREEN_DATA1_COM1_byte0_e                                      7
#define     SEG_SREEN_DATA1_COM1_byte0_SHIFT                                  0
#define     SEG_SREEN_DATA1_COM1_byte0_MASK                                   (0xFF<<0)

#define     SEG_SREEN_DATA2_COM2_word_e                                       31
#define     SEG_SREEN_DATA2_COM2_word_SHIFT                                   0
#define     SEG_SREEN_DATA2_COM2_word_MASK                                    (0xFFFFFFFF<<0)

#define     SEG_SREEN_DATA3_COM3_word_e                                       31
#define     SEG_SREEN_DATA3_COM3_word_SHIFT                                   0
#define     SEG_SREEN_DATA3_COM3_word_MASK                                    (0xFFFFFFFF<<0)

#define     SEG_SREEN_DATA4_COM4_word_e                                       31
#define     SEG_SREEN_DATA4_COM4_word_SHIFT                                   0
#define     SEG_SREEN_DATA4_COM4_word_MASK                                    (0xFFFFFFFF<<0)

#define     SEG_SREEN_DATA5_COM5_word_e                                       31
#define     SEG_SREEN_DATA5_COM5_word_SHIFT                                   0
#define     SEG_SREEN_DATA5_COM5_word_MASK                                    (0xFFFFFFFF<<0)

//--------------TK-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     TK_BASE                                                           0xc0040000
#define     TK_CTL                                                            (TK_BASE+0x0000)
#define     TK_IRQ_STATUS                                                     (TK_BASE+0x0004)
#define     TK_DET_STATUS                                                     (TK_BASE+0x0008)
#define     TK_ND_DB_CTL                                                      (TK_BASE+0x000c)
#define     TK_KEY_TH                                                         (TK_BASE+0x0010)
#define     TK_IDA_ADJUST_TH                                                  (TK_BASE+0x0014)
#define     TK_IDA_CTL                                                        (TK_BASE+0x0018)
#define     TK_KEY0_DAT                                                       (TK_BASE+0x001c)
#define     TK_KEY1_DAT                                                       (TK_BASE+0x0020)
#define     TK_KEY2_DAT                                                       (TK_BASE+0x0024)
#define     TK_KEY3_DAT                                                       (TK_BASE+0x0028)
#define     TK_KEY4_DAT                                                       (TK_BASE+0x002C)
#define     TK_KEY5_DAT                                                       (TK_BASE+0x0030)
#define     TK_KEY6_DAT                                                       (TK_BASE+0x0034)
#define     TK_KEY7_DAT                                                       (TK_BASE+0x0038)
#define     TK_KEY0_RAW_BL                                                    (TK_BASE+0x003C)
#define     TK_KEY1_RAW_BL                                                    (TK_BASE+0x0040)
#define     TK_KEY2_RAW_BL                                                    (TK_BASE+0x0044)
#define     TK_KEY3_RAW_BL                                                    (TK_BASE+0x0048)
#define     TK_KEY4_RAW_BL                                                    (TK_BASE+0x004c)
#define     TK_KEY5_RAW_BL                                                    (TK_BASE+0x0050)
#define     TK_KEY6_RAW_BL                                                    (TK_BASE+0x0054)
#define     TK_KEY7_RAW_BL                                                    (TK_BASE+0x0058)

//--------------Bits Location------------------------------------------//
#define     TK_CTL_TK_EN                                                      31
#define     TK_CTL_TK_INT_EN                                                  30
#define     TK_CTL_TK7_EN                                                     29
#define     TK_CTL_TK6_EN                                                     28
#define     TK_CTL_TK5_EN                                                     27
#define     TK_CTL_TK4_EN                                                     26
#define     TK_CTL_TK3_EN                                                     25
#define     TK_CTL_TK2_EN                                                     24
#define     TK_CTL_TK1_EN                                                     23
#define     TK_CTL_TK0_EN                                                     22
#define     TK_CTL_IDA_ADJUST_EN                                              21
#define     TK_CTL_TK_LOCK                                                    20
#define     TK_CTL_Noise_det_initial_en                                       19
#define     TK_CTL_Noise_det_normal_en                                        18
#define     TK_CTL_Noise_det_press_en                                         17
#define     TK_CTL_MF_LENGTH                                                  16
#define     TK_CTL_TK_KEY_SP_MODE                                             8
#define     TK_CTL_TK_FILTER_SEL_e                                            7
#define     TK_CTL_TK_FILTER_SEL_SHIFT                                        5
#define     TK_CTL_TK_FILTER_SEL_MASK                                         (0x7<<5)
#define     TK_CTL_TK_IBUF_SEL                                                4
#define     TK_CTL_TK_IB_SEL                                                  3
#define     TK_CTL_TK_TCAP_EN                                                 2
#define     TK_CTL_TK_SP_REF                                                  1
#define     TK_CTL_TK_SP_EN                                                   0

#define     TK_IRQ_STATUS_TK7_IRQ_PD                                          15
#define     TK_IRQ_STATUS_TK6_IRQ_PD                                          14
#define     TK_IRQ_STATUS_TK5_IRQ_PD                                          13
#define     TK_IRQ_STATUS_TK4_IRQ_PD                                          12
#define     TK_IRQ_STATUS_TK3_IRQ_PD                                          11
#define     TK_IRQ_STATUS_TK2_IRQ_PD                                          10
#define     TK_IRQ_STATUS_TK1_IRQ_PD                                          9
#define     TK_IRQ_STATUS_TK0_IRQ_PD                                          8
#define     TK_IRQ_STATUS_TK7_PRESS                                           7
#define     TK_IRQ_STATUS_TK6_PRESS                                           6
#define     TK_IRQ_STATUS_TK5_PRESS                                           5
#define     TK_IRQ_STATUS_TK4_PRESS                                           4
#define     TK_IRQ_STATUS_TK3_PRESS                                           3
#define     TK_IRQ_STATUS_TK2_PRESS                                           2
#define     TK_IRQ_STATUS_TK1_PRESS                                           1
#define     TK_IRQ_STATUS_TK0_PRESS                                           0

#define     TK_DET_STATUS_TK7_ND_PD                                           23
#define     TK_DET_STATUS_TK6_ND_PD                                           22
#define     TK_DET_STATUS_TK5_ND_PD                                           21
#define     TK_DET_STATUS_TK4_ND_PD                                           20
#define     TK_DET_STATUS_TK3_ND_PD                                           19
#define     TK_DET_STATUS_TK2_ND_PD                                           18
#define     TK_DET_STATUS_TK1_ND_PD                                           17
#define     TK_DET_STATUS_TK0_ND_PD                                           16
#define     TK_DET_STATUS_TK7_UPDAT_PD                                        15
#define     TK_DET_STATUS_TK6_UPDAT_PD                                        14
#define     TK_DET_STATUS_TK5_UPDAT_PD                                        13
#define     TK_DET_STATUS_TK4_UPDAT_PD                                        12
#define     TK_DET_STATUS_TK3_UPDAT_PD                                        11
#define     TK_DET_STATUS_TK2_UPDAT_PD                                        10
#define     TK_DET_STATUS_TK1_UPDAT_PD                                        9
#define     TK_DET_STATUS_TK0_UPDAT_PD                                        8
#define     TK_DET_STATUS_TK7_RAW_PD                                          7
#define     TK_DET_STATUS_TK6_RAW_PD                                          6
#define     TK_DET_STATUS_TK5_RAW_PD                                          5
#define     TK_DET_STATUS_TK4_RAW_PD                                          4
#define     TK_DET_STATUS_TK3_RAW_PD                                          3
#define     TK_DET_STATUS_TK2_RAW_PD                                          2
#define     TK_DET_STATUS_TK1_RAW_PD                                          1
#define     TK_DET_STATUS_TK0_RAW_PD                                          0

#define     TK_ND_DB_CTL_ND_CNT_SEL_e                                         31
#define     TK_ND_DB_CTL_ND_CNT_SEL_SHIFT                                     29
#define     TK_ND_DB_CTL_ND_CNT_SEL_MASK                                      (0x7<<29)
#define     TK_ND_DB_CTL_ND_TH_e                                              28
#define     TK_ND_DB_CTL_ND_TH_SHIFT                                          25
#define     TK_ND_DB_CTL_ND_TH_MASK                                           (0xF<<25)
#define     TK_ND_DB_CTL_BL_CAL_DB_e                                          24
#define     TK_ND_DB_CTL_BL_CAL_DB_SHIFT                                      22
#define     TK_ND_DB_CTL_BL_CAL_DB_MASK                                       (0x7<<22)
#define     TK_ND_DB_CTL_KEY_PRESS_DB_e                                       21
#define     TK_ND_DB_CTL_KEY_PRESS_DB_SHIFT                                   19
#define     TK_ND_DB_CTL_KEY_PRESS_DB_MASK                                    (0x7<<19)
#define     TK_ND_DB_CTL_KEY_UP_DB_e                                          18
#define     TK_ND_DB_CTL_KEY_UP_DB_SHIFT                                      16
#define     TK_ND_DB_CTL_KEY_UP_DB_MASK                                       (0x7<<16)
#define     TK_ND_DB_CTL_TK_PWM_CNT_e                                         15
#define     TK_ND_DB_CTL_TK_PWM_CNT_SHIFT                                     0
#define     TK_ND_DB_CTL_TK_PWM_CNT_MASK                                      (0xFFFF<<0)

#define     TK_KEY_TH_TK_PRESS_TH_e                                           28
#define     TK_KEY_TH_TK_PRESS_TH_SHIFT                                       24
#define     TK_KEY_TH_TK_PRESS_TH_MASK                                        (0x1F<<24)
#define     TK_KEY_TH_BL_CAL_TH_e                                             23
#define     TK_KEY_TH_BL_CAL_TH_SHIFT                                         20
#define     TK_KEY_TH_BL_CAL_TH_MASK                                          (0xF<<20)
#define     TK_KEY_TH_NOISE_TH_e                                              19
#define     TK_KEY_TH_NOISE_TH_SHIFT                                          16
#define     TK_KEY_TH_NOISE_TH_MASK                                           (0xF<<16)
#define     TK_KEY_TH_BUCKET_TH_e                                             14
#define     TK_KEY_TH_BUCKET_TH_SHIFT                                         12
#define     TK_KEY_TH_BUCKET_TH_MASK                                          (0x7<<12)
#define     TK_KEY_TH_BL_STEP_e                                               11
#define     TK_KEY_TH_BL_STEP_SHIFT                                           10
#define     TK_KEY_TH_BL_STEP_MASK                                            (0x3<<10)
#define     TK_KEY_TH_UPLIFT_TH_e                                             9
#define     TK_KEY_TH_UPLIFT_TH_SHIFT                                         5
#define     TK_KEY_TH_UPLIFT_TH_MASK                                          (0x1F<<5)

#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH1_e                                  31
#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH1_SHIFT                              16
#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH1_MASK                               (0xFFFF<<16)
#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH2_e                                  15
#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH2_SHIFT                              0
#define     TK_IDA_ADJUST_TH_TK_ADJUST_TH2_MASK                               (0xFFFF<<0)

#define     TK_IDA_CTL_TK7_IDA_CTL_e                                          31
#define     TK_IDA_CTL_TK7_IDA_CTL_SHIFT                                      28
#define     TK_IDA_CTL_TK7_IDA_CTL_MASK                                       (0xF<<28)
#define     TK_IDA_CTL_TK6_IDA_CTL_e                                          27
#define     TK_IDA_CTL_TK6_IDA_CTL_SHIFT                                      24
#define     TK_IDA_CTL_TK6_IDA_CTL_MASK                                       (0xF<<24)
#define     TK_IDA_CTL_TK5_IDA_CTL_e                                          23
#define     TK_IDA_CTL_TK5_IDA_CTL_SHIFT                                      20
#define     TK_IDA_CTL_TK5_IDA_CTL_MASK                                       (0xF<<20)
#define     TK_IDA_CTL_TK4_IDA_CTL_e                                          19
#define     TK_IDA_CTL_TK4_IDA_CTL_SHIFT                                      16
#define     TK_IDA_CTL_TK4_IDA_CTL_MASK                                       (0xF<<16)
#define     TK_IDA_CTL_TK3_IDA_CTL_e                                          15
#define     TK_IDA_CTL_TK3_IDA_CTL_SHIFT                                      12
#define     TK_IDA_CTL_TK3_IDA_CTL_MASK                                       (0xF<<12)
#define     TK_IDA_CTL_TK2_IDA_CTL_e                                          11
#define     TK_IDA_CTL_TK2_IDA_CTL_SHIFT                                      8
#define     TK_IDA_CTL_TK2_IDA_CTL_MASK                                       (0xF<<8)
#define     TK_IDA_CTL_TK1_IDA_CTL_e                                          7
#define     TK_IDA_CTL_TK1_IDA_CTL_SHIFT                                      4
#define     TK_IDA_CTL_TK1_IDA_CTL_MASK                                       (0xF<<4)
#define     TK_IDA_CTL_TK0_IDA_CTL_e                                          3
#define     TK_IDA_CTL_TK0_IDA_CTL_SHIFT                                      0
#define     TK_IDA_CTL_TK0_IDA_CTL_MASK                                       (0xF<<0)

#define     TK_KEY0_DAT_TK0_ND_CALC_e                                         31
#define     TK_KEY0_DAT_TK0_ND_CALC_SHIFT                                     16
#define     TK_KEY0_DAT_TK0_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY0_DAT_TK0_DAT_e                                             15
#define     TK_KEY0_DAT_TK0_DAT_SHIFT                                         0
#define     TK_KEY0_DAT_TK0_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY1_DAT_TK1_ND_CALC_e                                         31
#define     TK_KEY1_DAT_TK1_ND_CALC_SHIFT                                     16
#define     TK_KEY1_DAT_TK1_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY1_DAT_TK1_DAT_e                                             15
#define     TK_KEY1_DAT_TK1_DAT_SHIFT                                         0
#define     TK_KEY1_DAT_TK1_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY2_DAT_TK2_ND_CALC_e                                         31
#define     TK_KEY2_DAT_TK2_ND_CALC_SHIFT                                     16
#define     TK_KEY2_DAT_TK2_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY2_DAT_TK2_DAT_e                                             15
#define     TK_KEY2_DAT_TK2_DAT_SHIFT                                         0
#define     TK_KEY2_DAT_TK2_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY3_DAT_TK3_ND_CALC_e                                         31
#define     TK_KEY3_DAT_TK3_ND_CALC_SHIFT                                     16
#define     TK_KEY3_DAT_TK3_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY3_DAT_TK3_DAT_e                                             15
#define     TK_KEY3_DAT_TK3_DAT_SHIFT                                         0
#define     TK_KEY3_DAT_TK3_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY4_DAT_TK4_ND_CALC_e                                         31
#define     TK_KEY4_DAT_TK4_ND_CALC_SHIFT                                     16
#define     TK_KEY4_DAT_TK4_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY4_DAT_TK4_DAT_e                                             15
#define     TK_KEY4_DAT_TK4_DAT_SHIFT                                         0
#define     TK_KEY4_DAT_TK4_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY5_DAT_TK5_ND_CALC_e                                         31
#define     TK_KEY5_DAT_TK5_ND_CALC_SHIFT                                     16
#define     TK_KEY5_DAT_TK5_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY5_DAT_TK5_DAT_e                                             15
#define     TK_KEY5_DAT_TK5_DAT_SHIFT                                         0
#define     TK_KEY5_DAT_TK5_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY6_DAT_TK6_ND_CALC_e                                         31
#define     TK_KEY6_DAT_TK6_ND_CALC_SHIFT                                     16
#define     TK_KEY6_DAT_TK6_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY6_DAT_TK6_DAT_e                                             15
#define     TK_KEY6_DAT_TK6_DAT_SHIFT                                         0
#define     TK_KEY6_DAT_TK6_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY7_DAT_TK7_ND_CALC_e                                         31
#define     TK_KEY7_DAT_TK7_ND_CALC_SHIFT                                     16
#define     TK_KEY7_DAT_TK7_ND_CALC_MASK                                      (0xFFFF<<16)
#define     TK_KEY7_DAT_TK7_DAT_e                                             15
#define     TK_KEY7_DAT_TK7_DAT_SHIFT                                         0
#define     TK_KEY7_DAT_TK7_DAT_MASK                                          (0xFFFF<<0)

#define     TK_KEY0_RAW_BL_TK0_RAW_e                                          31
#define     TK_KEY0_RAW_BL_TK0_RAW_SHIFT                                      16
#define     TK_KEY0_RAW_BL_TK0_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY0_RAW_BL_TK0_BL_e                                           15
#define     TK_KEY0_RAW_BL_TK0_BL_SHIFT                                       0
#define     TK_KEY0_RAW_BL_TK0_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY1_RAW_BL_TK1_RAW_e                                          31
#define     TK_KEY1_RAW_BL_TK1_RAW_SHIFT                                      16
#define     TK_KEY1_RAW_BL_TK1_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY1_RAW_BL_TK1_BL_e                                           15
#define     TK_KEY1_RAW_BL_TK1_BL_SHIFT                                       0
#define     TK_KEY1_RAW_BL_TK1_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY2_RAW_BL_TK2_RAW_e                                          31
#define     TK_KEY2_RAW_BL_TK2_RAW_SHIFT                                      16
#define     TK_KEY2_RAW_BL_TK2_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY2_RAW_BL_TK2_BL_e                                           15
#define     TK_KEY2_RAW_BL_TK2_BL_SHIFT                                       0
#define     TK_KEY2_RAW_BL_TK2_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY3_RAW_BL_TK3_RAW_e                                          31
#define     TK_KEY3_RAW_BL_TK3_RAW_SHIFT                                      16
#define     TK_KEY3_RAW_BL_TK3_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY3_RAW_BL_TK3_BL_e                                           15
#define     TK_KEY3_RAW_BL_TK3_BL_SHIFT                                       0
#define     TK_KEY3_RAW_BL_TK3_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY4_RAW_BL_TK4_RAW_e                                          31
#define     TK_KEY4_RAW_BL_TK4_RAW_SHIFT                                      16
#define     TK_KEY4_RAW_BL_TK4_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY4_RAW_BL_TK4_BL_e                                           15
#define     TK_KEY4_RAW_BL_TK4_BL_SHIFT                                       0
#define     TK_KEY4_RAW_BL_TK4_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY5_RAW_BL_TK5_RAW_e                                          31
#define     TK_KEY5_RAW_BL_TK5_RAW_SHIFT                                      16
#define     TK_KEY5_RAW_BL_TK5_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY5_RAW_BL_TK5_BL_e                                           15
#define     TK_KEY5_RAW_BL_TK5_BL_SHIFT                                       0
#define     TK_KEY5_RAW_BL_TK5_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY6_RAW_BL_TK6_RAW_e                                          31
#define     TK_KEY6_RAW_BL_TK6_RAW_SHIFT                                      16
#define     TK_KEY6_RAW_BL_TK6_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY6_RAW_BL_TK6_BL_e                                           15
#define     TK_KEY6_RAW_BL_TK6_BL_SHIFT                                       0
#define     TK_KEY6_RAW_BL_TK6_BL_MASK                                        (0xFFFF<<0)

#define     TK_KEY7_RAW_BL_TK7_RAW_e                                          31
#define     TK_KEY7_RAW_BL_TK7_RAW_SHIFT                                      16
#define     TK_KEY7_RAW_BL_TK7_RAW_MASK                                       (0xFFFF<<16)
#define     TK_KEY7_RAW_BL_TK7_BL_e                                           15
#define     TK_KEY7_RAW_BL_TK7_BL_SHIFT                                       0
#define     TK_KEY7_RAW_BL_TK7_BL_MASK                                        (0xFFFF<<0)

//--------------DAC_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     DAC_Control_Register_BASE                                         0xc0050000
#define     DAC_DIGCTL                                                        (DAC_Control_Register_BASE+0x00)
#define     DAC_FIFOCTL                                                       (DAC_Control_Register_BASE+0x04)
#define     DAC_STAT                                                          (DAC_Control_Register_BASE+0x08)
#define     DAC_DAT_FIFO0                                                     (DAC_Control_Register_BASE+0x0C)
#define     DAC_DAT_FIFO1                                                     (DAC_Control_Register_BASE+0x10)
#define     VOL_LCH                                                           (DAC_Control_Register_BASE+0x14)
#define     VOL_RCH                                                           (DAC_Control_Register_BASE+0x18)
#define     DAC_ANACTL                                                        (DAC_Control_Register_BASE+0x1C)
#define     DAC_BIAS                                                          (DAC_Control_Register_BASE+0x20)
#define     PA_VOLUME                                                         (DAC_Control_Register_BASE+0x24)

//--------------Bits Location------------------------------------------//
#define     DAC_DIGCTL_DADCS                                                  15
#define     DAC_DIGCTL_DADEN                                                  14
#define     DAC_DIGCTL_DDDEN                                                  13
#define     DAC_DIGCTL_AD2DALPEN                                              12
#define     DAC_DIGCTL_I2SLPEN                                                11
#define     DAC_DIGCTL_I2SM                                                   10
#define     DAC_DIGCTL_I2STXEN                                                9
#define     DAC_DIGCTL_I2SRXEN                                                8
#define     DAC_DIGCTL_INTFRS_e                                               7
#define     DAC_DIGCTL_INTFRS_SHIFT                                           6
#define     DAC_DIGCTL_INTFRS_MASK                                            (0x3<<6)
#define     DAC_DIGCTL_DAF1M2DAEN                                             5
#define     DAC_DIGCTL_DAF0M2DAEN                                             4
#define     DAC_DIGCTL_OSRDA_e                                                3
#define     DAC_DIGCTL_OSRDA_SHIFT                                            2
#define     DAC_DIGCTL_OSRDA_MASK                                             (0x3<<2)
#define     DAC_DIGCTL_ENDITH                                                 1
#define     DAC_DIGCTL_DDEN                                                   0

#define     DAC_FIFOCTL_DAF1IS_e                                              13
#define     DAC_FIFOCTL_DAF1IS_SHIFT                                          12
#define     DAC_FIFOCTL_DAF1IS_MASK                                           (0x3<<12)
#define     DAC_FIFOCTL_DAF1EIE                                               10
#define     DAC_FIFOCTL_DAF1EDE                                               9
#define     DAC_FIFOCTL_DAF1RT                                                8
#define     DAC_FIFOCTL_DAF0IS_e                                              5
#define     DAC_FIFOCTL_DAF0IS_SHIFT                                          4
#define     DAC_FIFOCTL_DAF0IS_MASK                                           (0x3<<4)
#define     DAC_FIFOCTL_DAF0EIE                                               2
#define     DAC_FIFOCTL_DAF0EDE                                               1
#define     DAC_FIFOCTL_DAF0RT                                                0

#define     DAC_STAT_DAF1IP                                                   15
#define     DAC_STAT_DAF1F                                                    14
#define     DAC_STAT_DAF1S_e                                                  13
#define     DAC_STAT_DAF1S_SHIFT                                              8
#define     DAC_STAT_DAF1S_MASK                                               (0x3F<<8)
#define     DAC_STAT_DAF0IP                                                   7
#define     DAC_STAT_DAF0F                                                    6
#define     DAC_STAT_DAF0S_e                                                  5
#define     DAC_STAT_DAF0S_SHIFT                                              0
#define     DAC_STAT_DAF0S_MASK                                               (0x3F<<0)

#define     DAC_DAT_FIFO0_DAF0DAT_e                                           31
#define     DAC_DAT_FIFO0_DAF0DAT_SHIFT                                       8
#define     DAC_DAT_FIFO0_DAF0DAT_MASK                                        (0xFFFFFF<<8)

#define     DAC_DAT_FIFO1_DAF1DAT_e                                           31
#define     DAC_DAT_FIFO1_DAF1DAT_SHIFT                                       8
#define     DAC_DAT_FIFO1_DAF1DAT_MASK                                        (0xFFFFFF<<8)

#define     VOL_LCH_VOLLZCTOEN                                                9
#define     VOL_LCH_VOLLZCEN                                                  8
#define     VOL_LCH_VOLL_e                                                    7
#define     VOL_LCH_VOLL_SHIFT                                                0
#define     VOL_LCH_VOLL_MASK                                                 (0xFF<<0)

#define     VOL_RCH_VOLRZCTOEN                                                9
#define     VOL_RCH_VOLRZCEN                                                  8
#define     VOL_RCH_VOLR_e                                                    7
#define     VOL_RCH_VOLR_SHIFT                                                0
#define     VOL_RCH_VOLR_MASK                                                 (0xFF<<0)

#define     DAC_ANACTL_MOPPAMT                                                26
#define     DAC_ANACTL_OP1PAMT                                                25
#define     DAC_ANACTL_OP0PAMT                                                24
#define     DAC_ANACTL_RPAOSNEN                                               23
#define     DAC_ANACTL_RPANEN                                                 22
#define     DAC_ANACTL_LPAOSNEN                                               21
#define     DAC_ANACTL_LPANEN                                                 20
#define     DAC_ANACTL_PAIQ_e                                                 17
#define     DAC_ANACTL_PAIQ_SHIFT                                             16
#define     DAC_ANACTL_PAIQ_MASK                                              (0x3<<16)
#define     DAC_ANACTL_P2IB                                                   15
#define     DAC_ANACTL_ATP2CE                                                 14
#define     DAC_ANACTL_PAVDC                                                  13
#define     DAC_ANACTL_ATPLP2                                                 12
#define     DAC_ANACTL_OVLS                                                   11
#define     DAC_ANACTL_DDOVV                                                  10
#define     DAC_ANACTL_DDATPR                                                 9
#define     DAC_ANACTL_OPVROEN                                                8
#define     DAC_ANACTL_ZERODT                                                 7
#define     DAC_ANACTL_AIN2DMT                                                6
#define     DAC_ANACTL_DPBM                                                   5
#define     DAC_ANACTL_BIASEN                                                 4
#define     DAC_ANACTL_PAOSEN                                                 3
#define     DAC_ANACTL_PAEN                                                   2
#define     DAC_ANACTL_DAENR                                                  1
#define     DAC_ANACTL_DAENL                                                  0

#define     DAC_BIAS_OPINV_e                                                  25
#define     DAC_BIAS_OPINV_SHIFT                                              23
#define     DAC_BIAS_OPINV_MASK                                               (0x7<<23)
#define     DAC_BIAS_OPDAIB_e                                                 22
#define     DAC_BIAS_OPDAIB_SHIFT                                             20
#define     DAC_BIAS_OPDAIB_MASK                                              (0x7<<20)
#define     DAC_BIAS_OPDAVB_e                                                 19
#define     DAC_BIAS_OPDAVB_SHIFT                                             18
#define     DAC_BIAS_OPDAVB_MASK                                              (0x3<<18)
#define     DAC_BIAS_OPVBIB_e                                                 15
#define     DAC_BIAS_OPVBIB_SHIFT                                             14
#define     DAC_BIAS_OPVBIB_MASK                                              (0x3<<14)
#define     DAC_BIAS_OPCM1IB_e                                                13
#define     DAC_BIAS_OPCM1IB_SHIFT                                            12
#define     DAC_BIAS_OPCM1IB_MASK                                             (0x3<<12)
#define     DAC_BIAS_ALLBIAS_e                                                9
#define     DAC_BIAS_ALLBIAS_SHIFT                                            8
#define     DAC_BIAS_ALLBIAS_MASK                                             (0x3<<8)
#define     DAC_BIAS_PAIB_e                                                   7
#define     DAC_BIAS_PAIB_SHIFT                                               6
#define     DAC_BIAS_PAIB_MASK                                                (0x3<<6)
#define     DAC_BIAS_OPVROOSIQ_e                                              5
#define     DAC_BIAS_OPVROOSIQ_SHIFT                                          3
#define     DAC_BIAS_OPVROOSIQ_MASK                                           (0x7<<3)
#define     DAC_BIAS_OPVROIB_e                                                2
#define     DAC_BIAS_OPVROIB_SHIFT                                            0
#define     DAC_BIAS_OPVROIB_MASK                                             (0x7<<0)

#define     PA_VOLUME_PALRTOEN                                                8
#define     PA_VOLUME_PALRZCDEN                                               7
#define     PA_VOLUME_PALRSW                                                  6
#define     PA_VOLUME_PALRVOL_e                                               5
#define     PA_VOLUME_PALRVOL_SHIFT                                           0
#define     PA_VOLUME_PALRVOL_MASK                                            (0x3F<<0)

//--------------ADC_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     ADC_Control_Register_BASE                                         0xc0051000
#define     ADC_DIGCTL                                                        (ADC_Control_Register_BASE+0x00)
#define     ADC_FIFOCTL                                                       (ADC_Control_Register_BASE+0x04)
#define     ADC_STAT                                                          (ADC_Control_Register_BASE+0x08)
#define     ADC_DAT                                                           (ADC_Control_Register_BASE+0x0C)
#define     ADC_ANACTL                                                        (ADC_Control_Register_BASE+0x10)
#define     ADC_BIAS                                                          (ADC_Control_Register_BASE+0x14)

//--------------Bits Location------------------------------------------//
#define     ADC_DIGCTL_ADCGC_e                                                14
#define     ADC_DIGCTL_ADCGC_SHIFT                                            11
#define     ADC_DIGCTL_ADCGC_MASK                                             (0xF<<11)
#define     ADC_DIGCTL_DMOSRS                                                 10
#define     ADC_DIGCTL_DMLEN                                                  9
#define     ADC_DIGCTL_DMREN                                                  8
#define     ADC_DIGCTL_HPFLEN                                                 7
#define     ADC_DIGCTL_HPFREN                                                 6
#define     ADC_DIGCTL_ADDEN                                                  3
#define     ADC_DIGCTL_AADEN                                                  2

#define     ADC_FIFOCTL_ADFOS_e                                               5
#define     ADC_FIFOCTL_ADFOS_SHIFT                                           4
#define     ADC_FIFOCTL_ADFOS_MASK                                            (0x3<<4)
#define     ADC_FIFOCTL_ADFIS                                                 3
#define     ADC_FIFOCTL_ADFFIE                                                2
#define     ADC_FIFOCTL_ADFFDE                                                1
#define     ADC_FIFOCTL_ADFRT                                                 0

#define     ADC_STAT_ADFEF                                                    7
#define     ADC_STAT_ADFIP                                                    6
#define     ADC_STAT_ADFS_e                                                   5
#define     ADC_STAT_ADFS_SHIFT                                               0
#define     ADC_STAT_ADFS_MASK                                                (0x3F<<0)

#define     ADC_DAT_ADDAT_e                                                   31
#define     ADC_DAT_ADDAT_SHIFT                                               8
#define     ADC_DAT_ADDAT_MASK                                                (0xFFFFFF<<8)

#define     ADC_ANACTL_FDMCVREN                                               29
#define     ADC_ANACTL_ADLPFEN                                                28
#define     ADC_ANACTL_ADSDMS                                                 27
#define     ADC_ANACTL_ADVREN                                                 26
#define     ADC_ANACTL_PARMADL                                                25
#define     ADC_ANACTL_PALMADR                                                24
#define     ADC_ANACTL_PARMADR                                                23
#define     ADC_ANACTL_PALMADL                                                22
#define     ADC_ANACTL_MRMLEN                                                 21
#define     ADC_ANACTL_MLMREN                                                 20
#define     ADC_ANACTL_MOPGTEN                                                19
#define     ADC_ANACTL_MOPG_e                                                 18
#define     ADC_ANACTL_MOPG_SHIFT                                             16
#define     ADC_ANACTL_MOPG_MASK                                              (0x7<<16)
#define     ADC_ANACTL_MFDSES                                                 15
#define     ADC_ANACTL_MOPGBEN                                                14
#define     ADC_ANACTL_MOPREN                                                 13
#define     ADC_ANACTL_MOPLEN                                                 12
#define     ADC_ANACTL_OP1G_e                                                 11
#define     ADC_ANACTL_OP1G_SHIFT                                             9
#define     ADC_ANACTL_OP1G_MASK                                              (0x7<<9)
#define     ADC_ANACTL_OP1REN                                                 8
#define     ADC_ANACTL_OP1LEN                                                 7
#define     ADC_ANACTL_OP0G_e                                                 6
#define     ADC_ANACTL_OP0G_SHIFT                                             4
#define     ADC_ANACTL_OP0G_MASK                                              (0x7<<4)
#define     ADC_ANACTL_OP0REN                                                 3
#define     ADC_ANACTL_OP0LEN                                                 2
#define     ADC_ANACTL_ADLEN                                                  1
#define     ADC_ANACTL_ADREN                                                  0

#define     ADC_BIAS_LPFBC_e                                                  31
#define     ADC_BIAS_LPFBC_SHIFT                                              29
#define     ADC_BIAS_LPFBC_MASK                                               (0x7<<29)
#define     ADC_BIAS_LPFBUFBC_e                                               28
#define     ADC_BIAS_LPFBUFBC_SHIFT                                           27
#define     ADC_BIAS_LPFBUFBC_MASK                                            (0x3<<27)
#define     ADC_BIAS_ADCBIAS                                                  26
#define     ADC_BIAS_FD1BC_e                                                  23
#define     ADC_BIAS_FD1BC_SHIFT                                              22
#define     ADC_BIAS_FD1BC_MASK                                               (0x3<<22)
#define     ADC_BIAS_FD1BUFBC_e                                               19
#define     ADC_BIAS_FD1BUFBC_SHIFT                                           18
#define     ADC_BIAS_FD1BUFBC_MASK                                            (0x3<<18)
#define     ADC_BIAS_OP0BC_e                                                  15
#define     ADC_BIAS_OP0BC_SHIFT                                              14
#define     ADC_BIAS_OP0BC_MASK                                               (0x3<<14)
#define     ADC_BIAS_OP1BC_e                                                  13
#define     ADC_BIAS_OP1BC_SHIFT                                              12
#define     ADC_BIAS_OP1BC_MASK                                               (0x3<<12)
#define     ADC_BIAS_OPBC1                                                    7£º5
#define     ADC_BIAS_OPBC23_e                                                 4
#define     ADC_BIAS_OPBC23_SHIFT                                             3
#define     ADC_BIAS_OPBC23_MASK                                              (0x3<<3)
#define     ADC_BIAS_VRDABC_e                                                 2
#define     ADC_BIAS_VRDABC_SHIFT                                             0
#define     ADC_BIAS_VRDABC_MASK                                              (0x7<<0)

//--------------SPDIFTX_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     SPDIFTX_Control_Register_BASE                                     0xc0052000
#define     SPDTX_CTL                                                         (SPDIFTX_Control_Register_BASE+0x00)
#define     SPDTX_CSL                                                         (SPDIFTX_Control_Register_BASE+0x04)
#define     SPDTX_CSH                                                         (SPDIFTX_Control_Register_BASE+0x08)

//--------------Bits Location------------------------------------------//
#define     SPDT0_CTL_SPDEN                                                   0

#define     SPDT0_CSL_SPDCSL_e                                                31
#define     SPDT0_CSL_SPDCSL_SHIFT                                            0
#define     SPDT0_CSL_SPDCSL_MASK                                             (0xFFFFFFFF<<0)

#define     SPDT0_CSH_SPDCSH_e                                                15
#define     SPDT0_CSH_SPDCSH_SHIFT                                            0
#define     SPDT0_CSH_SPDCSH_MASK                                             (0xFFFF<<0)

//--------------PCM_Control_Register-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     PCM_Control_Register_BASE                                         0xc0170000
#define     PCM_CTL                                                           (PCM_Control_Register_BASE+0x00)
#define     PCM_STAT                                                          (PCM_Control_Register_BASE+0x0004)
#define     PCM_RXDAT                                                         (PCM_Control_Register_BASE+0x0008)
#define     PCM_TXDAT                                                         (PCM_Control_Register_BASE+0x000C)

//--------------Bits Location------------------------------------------//
#define     PCM_CTL_RXFOS_e                                                   21
#define     PCM_CTL_RXFOS_SHIFT                                               20
#define     PCM_CTL_RXFOS_MASK                                                (0x3<<20)
#define     PCM_CTL_EN                                                        19
#define     PCM_CTL_SEN                                                       18
#define     PCM_CTL_SAMF                                                      17
#define     PCM_CTL_IVEN                                                      16
#define     PCM_CTL_MS                                                        15
#define     PCM_CTL_FRMS                                                      14
#define     PCM_CTL_LBEN                                                      12
#define     PCM_CTL_TXFIS_e                                                   11
#define     PCM_CTL_TXFIS_SHIFT                                               10
#define     PCM_CTL_TXFIS_MASK                                                (0x3<<10)
#define     PCM_CTL_LMFR                                                      9
#define     PCM_CTL_SSOE                                                      8
#define     PCM_CTL_TXIE                                                      7
#define     PCM_CTL_RXIE                                                      6
#define     PCM_CTL_TXDE                                                      5
#define     PCM_CTL_RXDE                                                      4
#define     PCM_CTL_DATO                                                      3
#define     PCM_CTL_DATM_e                                                    2
#define     PCM_CTL_DATM_SHIFT                                                0
#define     PCM_CTL_DATM_MASK                                                 (0x7<<0)

#define     PCM_STAT_TFES                                                     7
#define     PCM_STAT_RFFS                                                     6
#define     PCM_STAT_TFFU                                                     5
#define     PCM_STAT_RFEM                                                     4
#define     PCM_STAT_TFEP                                                     3
#define     PCM_STAT_RFEP                                                     2
#define     PCM_STAT_TIP                                                      1
#define     PCM_STAT_RIP                                                      0

#define     PCM_RXDAT_RXDAT_e                                                 15
#define     PCM_RXDAT_RXDAT_SHIFT                                             0
#define     PCM_RXDAT_RXDAT_MASK                                              (0xFFFF<<0)

#define     PCM_TXDAT_TXDAT_e                                                 15
#define     PCM_TXDAT_TXDAT_SHIFT                                             0
#define     PCM_TXDAT_TXDAT_MASK                                              (0xFFFF<<0)

//--------------ASRCController-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     ASRCController_BASE                                               0xc0160000
#define     ASRC_IN_CTL                                                       (ASRCController_BASE+0x00000000)
#define     ASRC_IN_DEC0                                                      (ASRCController_BASE+0x00000004)
#define     ASRC_IN_DEC1                                                      (ASRCController_BASE+0x00000008)
#define     ASRC_IN_NUM                                                       (ASRCController_BASE+0x0000000c)
#define     ASRC_IN_RFIFO                                                     (ASRCController_BASE+0x00000010)
#define     ASRC_IN_WFIFO                                                     (ASRCController_BASE+0x00000014)
#define     ASRC_IN_LGAIN                                                     (ASRCController_BASE+0x00000018)
#define     ASRC_IN_RGAIN                                                     (ASRCController_BASE+0x0000001c)
#define     ASRC_IN_IP                                                        (ASRCController_BASE+0x00000020)
#define     ASRC_IN_TIME                                                      (ASRCController_BASE+0x00000024)
#define     ASRC_IN_THRES_HF                                                  (ASRCController_BASE+0x00000028)
#define     ASRC_IN_THRES_HE                                                  (ASRCController_BASE+0x0000002c)
#define     ASRC_OUT0_CTL                                                     (ASRCController_BASE+0x00000030)
#define     ASRC_OUT0_DEC0                                                    (ASRCController_BASE+0x00000034)
#define     ASRC_OUT0_DEC1                                                    (ASRCController_BASE+0x00000038)
#define     ASRC_OUT0_NUM                                                     (ASRCController_BASE+0x0000003c)
#define     ASRC_OUT0_RFIFO                                                   (ASRCController_BASE+0x00000040)
#define     ASRC_OUT0_WFIFO                                                   (ASRCController_BASE+0x00000044)
#define     ASRC_OUT0_LGAIN                                                   (ASRCController_BASE+0x00000048)
#define     ASRC_OUT0_RGAIN                                                   (ASRCController_BASE+0x0000004c)
#define     ASRC_OUT0_IP                                                      (ASRCController_BASE+0x00000050)
#define     ASRC_OUT0_TIME                                                    (ASRCController_BASE+0x00000054)
#define     ASRC_OUT0_THRES_HF                                                (ASRCController_BASE+0x00000058)
#define     ASRC_OUT0_THRES_HE                                                (ASRCController_BASE+0x0000005c)
#define     ASRC_OUT1_CTL                                                     (ASRCController_BASE+0x00000060)
#define     ASRC_OUT1_DEC0                                                    (ASRCController_BASE+0x00000064)
#define     ASRC_OUT1_DEC1                                                    (ASRCController_BASE+0x00000068)
#define     ASRC_OUT1_NUM                                                     (ASRCController_BASE+0x0000006c)
#define     ASRC_OUT1_RFIFO                                                   (ASRCController_BASE+0x00000070)
#define     ASRC_OUT1_WFIFO                                                   (ASRCController_BASE+0x00000074)
#define     ASRC_OUT1_LGAIN                                                   (ASRCController_BASE+0x00000078)
#define     ASRC_OUT1_RGAIN                                                   (ASRCController_BASE+0x0000007c)
#define     ASRC_OUT1_IP                                                      (ASRCController_BASE+0x00000080)
#define     ASRC_OUT1_TIME                                                    (ASRCController_BASE+0x00000084)
#define     ASRC_OUT1_THRES_HF                                                (ASRCController_BASE+0x00000088)
#define     ASRC_OUT1_THRES_HE                                                (ASRCController_BASE+0x0000008c)

//--------------Bits Location------------------------------------------//
#define     ASRC_IN_CTL_AverageNum_e                                          27
#define     ASRC_IN_CTL_AverageNum_SHIFT                                      16
#define     ASRC_IN_CTL_AverageNum_MASK                                       (0xFFF<<16)
#define     ASRC_IN_CTL_ResetWFIFO                                            12
#define     ASRC_IN_CTL_ResetRFIFO                                            11
#define     ASRC_IN_CTL_ModeSel                                               10
#define     ASRC_IN_CTL_IN_W_CLKSEL_e                                         9
#define     ASRC_IN_CTL_IN_W_CLKSEL_SHIFT                                     8
#define     ASRC_IN_CTL_IN_W_CLKSEL_MASK                                      (0x3<<8)
#define     ASRC_IN_CTL_IN_R_CLKSEL_e                                         7
#define     ASRC_IN_CTL_IN_R_CLKSEL_SHIFT                                     6
#define     ASRC_IN_CTL_IN_R_CLKSEL_MASK                                      (0x3<<6)
#define     ASRC_IN_CTL_RamSel_e                                              5
#define     ASRC_IN_CTL_RamSel_SHIFT                                          4
#define     ASRC_IN_CTL_RamSel_MASK                                           (0x3<<4)
#define     ASRC_IN_CTL_TableSel                                              3
#define     ASRC_IN_CTL_INEn                                                  1
#define     ASRC_IN_CTL_BypassEn                                              0

#define     ASRC_IN_DEC0_DecLow_e                                             22
#define     ASRC_IN_DEC0_DecLow_SHIFT                                         0
#define     ASRC_IN_DEC0_DecLow_MASK                                          (0x7FFFFF<<0)

#define     ASRC_IN_DEC1_DecHigh_e                                            22
#define     ASRC_IN_DEC1_DecHigh_SHIFT                                        0
#define     ASRC_IN_DEC1_DecHigh_MASK                                         (0x7FFFFF<<0)

#define     ASRC_IN_NUM_ReadableNum_e                                         5
#define     ASRC_IN_NUM_ReadableNum_SHIFT                                     0
#define     ASRC_IN_NUM_ReadableNum_MASK                                      (0x3F<<0)

#define     ASRC_IN_RFIFO_ReadData_e                                          31
#define     ASRC_IN_RFIFO_ReadData_SHIFT                                      16
#define     ASRC_IN_RFIFO_ReadData_MASK                                       (0xFFFF<<16)

#define     ASRC_IN_WFIFO_WriteData_e                                         31
#define     ASRC_IN_WFIFO_WriteData_SHIFT                                     8
#define     ASRC_IN_WFIFO_WriteData_MASK                                      (0xFFFFFF<<8)

#define     ASRC_IN_LGAIN_LGAIN_e                                             23
#define     ASRC_IN_LGAIN_LGAIN_SHIFT                                         0
#define     ASRC_IN_LGAIN_LGAIN_MASK                                          (0xFFFFFF<<0)

#define     ASRC_IN_RGAIN_RGAIN_e                                             23
#define     ASRC_IN_RGAIN_RGAIN_SHIFT                                         0
#define     ASRC_IN_RGAIN_RGAIN_MASK                                          (0xFFFFFF<<0)

#define     ASRC_IN_IP_PCMFIFOLevel_e                                         28
#define     ASRC_IN_IP_PCMFIFOLevel_SHIFT                                     16
#define     ASRC_IN_IP_PCMFIFOLevel_MASK                                      (0x1FFF<<16)
#define     ASRC_IN_IP_PCMFIFOFull                                            4
#define     ASRC_IN_IP_PCMFIFOEmpty                                           3
#define     ASRC_IN_IP_WFIFOEmpty                                             2
#define     ASRC_IN_IP_HFtoHEIP                                               1
#define     ASRC_IN_IP_HEtoHFIP                                               0

#define     ASRC_IN_TIME_TimeHFtoHE_e                                         31
#define     ASRC_IN_TIME_TimeHFtoHE_SHIFT                                     16
#define     ASRC_IN_TIME_TimeHFtoHE_MASK                                      (0xFFFF<<16)
#define     ASRC_IN_TIME_TimeHEtoHF_e                                         15
#define     ASRC_IN_TIME_TimeHEtoHF_SHIFT                                     0
#define     ASRC_IN_TIME_TimeHEtoHF_MASK                                      (0xFFFF<<0)

#define     ASRC_IN_THRES_HF_Threshold_e                                      23
#define     ASRC_IN_THRES_HF_Threshold_SHIFT                                  0
#define     ASRC_IN_THRES_HF_Threshold_MASK                                   (0xFFFFFF<<0)

#define     ASRC_IN_THRES_HE_Threshold_e                                      23
#define     ASRC_IN_THRES_HE_Threshold_SHIFT                                  0
#define     ASRC_IN_THRES_HE_Threshold_MASK                                   (0xFFFFFF<<0)

#define     ASRC_OUT0_CTL_AverageNum_e                                        27
#define     ASRC_OUT0_CTL_AverageNum_SHIFT                                    16
#define     ASRC_OUT0_CTL_AverageNum_MASK                                     (0xFFF<<16)
#define     ASRC_OUT0_CTL_ResetRFIFO                                          12
#define     ASRC_OUT0_CTL_ResetWFIFO                                          11
#define     ASRC_OUT0_CTL_ModeSel                                             10
#define     ASRC_OUT0_CTL_OUTCH0_R_CLKSEL_e                                   9
#define     ASRC_OUT0_CTL_OUTCH0_R_CLKSEL_SHIFT                               8
#define     ASRC_OUT0_CTL_OUTCH0_R_CLKSEL_MASK                                (0x3<<8)
#define     ASRC_OUT0_CTL_OUTCH0_W_CLKSEL_e                                   7
#define     ASRC_OUT0_CTL_OUTCH0_W_CLKSEL_SHIFT                               6
#define     ASRC_OUT0_CTL_OUTCH0_W_CLKSEL_MASK                                (0x3<<6)
#define     ASRC_OUT0_CTL_RamSel_e                                            5
#define     ASRC_OUT0_CTL_RamSel_SHIFT                                        4
#define     ASRC_OUT0_CTL_RamSel_MASK                                         (0x3<<4)
#define     ASRC_OUT0_CTL_OutCh0En                                            1
#define     ASRC_OUT0_CTL_BypassEn                                            0

#define     ASRC_OUT0_DEC0_DecLow_e                                           22
#define     ASRC_OUT0_DEC0_DecLow_SHIFT                                       0
#define     ASRC_OUT0_DEC0_DecLow_MASK                                        (0x7FFFFF<<0)

#define     ASRC_OUT0_DEC1_DecHigh_e                                          22
#define     ASRC_OUT0_DEC1_DecHigh_SHIFT                                      0
#define     ASRC_OUT0_DEC1_DecHigh_MASK                                       (0x7FFFFF<<0)

#define     ASRC_OUT0_NUM_WriteableNum_e                                      5
#define     ASRC_OUT0_NUM_WriteableNum_SHIFT                                  0
#define     ASRC_OUT0_NUM_WriteableNum_MASK                                   (0x3F<<0)

#define     ASRC_OUT0_RFIFO_ReadData_e                                        31
#define     ASRC_OUT0_RFIFO_ReadData_SHIFT                                    8
#define     ASRC_OUT0_RFIFO_ReadData_MASK                                     (0xFFFFFF<<8)

#define     ASRC_OUT0_WFIFO_WriteData_e                                       31
#define     ASRC_OUT0_WFIFO_WriteData_SHIFT                                   8
#define     ASRC_OUT0_WFIFO_WriteData_MASK                                    (0xFFFFFF<<8)

#define     ASRC_OUT0_LGAIN_LGAIN_e                                           23
#define     ASRC_OUT0_LGAIN_LGAIN_SHIFT                                       0
#define     ASRC_OUT0_LGAIN_LGAIN_MASK                                        (0xFFFFFF<<0)

#define     ASRC_OUT0_RGAIN_RGAIN_e                                           23
#define     ASRC_OUT0_RGAIN_RGAIN_SHIFT                                       0
#define     ASRC_OUT0_RGAIN_RGAIN_MASK                                        (0xFFFFFF<<0)

#define     ASRC_OUT0_IP_PCMFIFOLevel_e                                       28
#define     ASRC_OUT0_IP_PCMFIFOLevel_SHIFT                                   16
#define     ASRC_OUT0_IP_PCMFIFOLevel_MASK                                    (0x1FFF<<16)
#define     ASRC_OUT0_IP_PCMFIFOFull                                          4
#define     ASRC_OUT0_IP_PCMFIFOEmpty                                         3
#define     ASRC_OUT0_IP_RFIFOFull                                            2
#define     ASRC_OUT0_IP_HFtoHEIP                                             1
#define     ASRC_OUT0_IP_HEtoHFIP                                             0

#define     ASRC_OUT0_TIME_TimeHFtoHE_e                                       31
#define     ASRC_OUT0_TIME_TimeHFtoHE_SHIFT                                   16
#define     ASRC_OUT0_TIME_TimeHFtoHE_MASK                                    (0xFFFF<<16)
#define     ASRC_OUT0_TIME_TimeHEtoHF_e                                       15
#define     ASRC_OUT0_TIME_TimeHEtoHF_SHIFT                                   0
#define     ASRC_OUT0_TIME_TimeHEtoHF_MASK                                    (0xFFFF<<0)

#define     ASRC_OUT0_THRES_HF_Threshold_e                                    23
#define     ASRC_OUT0_THRES_HF_Threshold_SHIFT                                0
#define     ASRC_OUT0_THRES_HF_Threshold_MASK                                 (0xFFFFFF<<0)

#define     ASRC_OUT0_THRES_HE_Threshold_e                                    23
#define     ASRC_OUT0_THRES_HE_Threshold_SHIFT                                0
#define     ASRC_OUT0_THRES_HE_Threshold_MASK                                 (0xFFFFFF<<0)

#define     ASRC_OUT1_CTL_AverageNum_e                                        27
#define     ASRC_OUT1_CTL_AverageNum_SHIFT                                    16
#define     ASRC_OUT1_CTL_AverageNum_MASK                                     (0xFFF<<16)
#define     ASRC_OUT1_CTL_ResetRFIFO                                          12
#define     ASRC_OUT1_CTL_ResetWFIFO                                          11
#define     ASRC_OUT1_CTL_ModeSel                                             10
#define     ASRC_OUT1_CTL_OUTCH1_R_CLKSEL_e                                   9
#define     ASRC_OUT1_CTL_OUTCH1_R_CLKSEL_SHIFT                               8
#define     ASRC_OUT1_CTL_OUTCH1_R_CLKSEL_MASK                                (0x3<<8)
#define     ASRC_OUT1_CTL_OUTCH1_W_CLKSEL_e                                   7
#define     ASRC_OUT1_CTL_OUTCH1_W_CLKSEL_SHIFT                               6
#define     ASRC_OUT1_CTL_OUTCH1_W_CLKSEL_MASK                                (0x3<<6)
#define     ASRC_OUT1_CTL_RamSel                                              5
#define     ASRC_OUT1_CTL_OutCh1En                                            1
#define     ASRC_OUT1_CTL_BypassEn                                            0

#define     ASRC_OUT1_DEC0_DecLow_e                                           22
#define     ASRC_OUT1_DEC0_DecLow_SHIFT                                       0
#define     ASRC_OUT1_DEC0_DecLow_MASK                                        (0x7FFFFF<<0)

#define     ASRC_OUT1_DEC1_DecHigh_e                                          22
#define     ASRC_OUT1_DEC1_DecHigh_SHIFT                                      0
#define     ASRC_OUT1_DEC1_DecHigh_MASK                                       (0x7FFFFF<<0)

#define     ASRC_OUT1_NUM_WriteableNum_e                                      10
#define     ASRC_OUT1_NUM_WriteableNum_SHIFT                                  0
#define     ASRC_OUT1_NUM_WriteableNum_MASK                                   (0x7FF<<0)

#define     ASRC_OUT1_RFIFO_WriteableNum_e                                    5
#define     ASRC_OUT1_RFIFO_WriteableNum_SHIFT                                0
#define     ASRC_OUT1_RFIFO_WriteableNum_MASK                                 (0x3F<<0)

#define     ASRC_OUT1_WFIFO_WriteData_e                                       31
#define     ASRC_OUT1_WFIFO_WriteData_SHIFT                                   8
#define     ASRC_OUT1_WFIFO_WriteData_MASK                                    (0xFFFFFF<<8)

#define     ASRC_OUT1_LGAIN_LGAIN_e                                           23
#define     ASRC_OUT1_LGAIN_LGAIN_SHIFT                                       0
#define     ASRC_OUT1_LGAIN_LGAIN_MASK                                        (0xFFFFFF<<0)

#define     ASRC_OUT1_RGAIN_RGAIN_e                                           23
#define     ASRC_OUT1_RGAIN_RGAIN_SHIFT                                       0
#define     ASRC_OUT1_RGAIN_RGAIN_MASK                                        (0xFFFFFF<<0)

#define     ASRC_OUT1_IP_PCMFIFOLevel_e                                       28
#define     ASRC_OUT1_IP_PCMFIFOLevel_SHIFT                                   16
#define     ASRC_OUT1_IP_PCMFIFOLevel_MASK                                    (0x1FFF<<16)
#define     ASRC_OUT1_IP_PCMFIFOFull                                          4
#define     ASRC_OUT1_IP_PCMFIFOEmpty                                         3
#define     ASRC_OUT1_IP_RFIFOFull                                            2
#define     ASRC_OUT1_IP_HFtoHEIP                                             1
#define     ASRC_OUT1_IP_HEtoHFIP                                             0

#define     ASRC_OUT1_TIME_TimeHFtoHE_e                                       31
#define     ASRC_OUT1_TIME_TimeHFtoHE_SHIFT                                   16
#define     ASRC_OUT1_TIME_TimeHFtoHE_MASK                                    (0xFFFF<<16)
#define     ASRC_OUT1_TIME_TimeHEtoHF_e                                       15
#define     ASRC_OUT1_TIME_TimeHEtoHF_SHIFT                                   0
#define     ASRC_OUT1_TIME_TimeHEtoHF_MASK                                    (0xFFFF<<0)

#define     ASRC_OUT1_THRES_HF_Threshold_e                                    23
#define     ASRC_OUT1_THRES_HF_Threshold_SHIFT                                0
#define     ASRC_OUT1_THRES_HF_Threshold_MASK                                 (0xFFFFFF<<0)

#define     ASRC_OUT1_THRES_HE_Threshold_e                                    23
#define     ASRC_OUT1_THRES_HE_Threshold_SHIFT                                0
#define     ASRC_OUT1_THRES_HE_Threshold_MASK                                 (0xFFFFFF<<0)

//--------------GPIO_MFP-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     GPIO_MFP_BASE                                                     0xc0090000
#define     GPIOAOUTEN                                                        (GPIO_MFP_BASE+0x0000)
#define     GPIOAINEN                                                         (GPIO_MFP_BASE+0x0004)
#define     GPIOADAT                                                          (GPIO_MFP_BASE+0x0008)
#define     GPIOAPUEN                                                         (GPIO_MFP_BASE+0x000C)
#define     GPIOAPDEN                                                         (GPIO_MFP_BASE+0x0010)
#define     GPIOBOUTEN                                                        (GPIO_MFP_BASE+0x0014)
#define     GPIOBINEN                                                         (GPIO_MFP_BASE+0x0018)
#define     GPIOBDAT                                                          (GPIO_MFP_BASE+0x001C)
#define     GPIOBPUEN                                                         (GPIO_MFP_BASE+0x0020)
#define     GPIOBPDEN                                                         (GPIO_MFP_BASE+0x0024)
#define     SIO_OUTEN                                                         (GPIO_MFP_BASE+0x0028)
#define     SIO_INEN                                                          (GPIO_MFP_BASE+0x002C)
#define     SIO_DAT                                                           (GPIO_MFP_BASE+0x0030)
#define     SIO_PUEN                                                          (GPIO_MFP_BASE+0x0034)
#define     SIO_PDEN                                                          (GPIO_MFP_BASE+0x0038)
#define     GPIOB0_CTL                                                        (GPIO_MFP_BASE+0x003C)
#define     PADDRV4                                                           (GPIO_MFP_BASE+0x0040)
#define     PWM3_CTL                                                          (GPIO_MFP_BASE+0x0044)
#define     PWM0_CTL                                                          (GPIO_MFP_BASE+0x0048)
#define     PWM1_CTL                                                          (GPIO_MFP_BASE+0x004C)
#define     PWM2_CTL                                                          (GPIO_MFP_BASE+0x0050)
#define     MFP_CTL0                                                          (GPIO_MFP_BASE+0x0054)
#define     MFP_CTL1                                                          (GPIO_MFP_BASE+0x0058)
#define     MFP_CTL2                                                          (GPIO_MFP_BASE+0x005C)
#define     MFP_CTL3                                                          (GPIO_MFP_BASE+0x0060)
#define     AD_Select                                                         (GPIO_MFP_BASE+0x0064)
#define     PADPUPD                                                           (GPIO_MFP_BASE+0x0068)
#define     PAD_SMIT                                                          (GPIO_MFP_BASE+0x006C)
#define     PADDRV0                                                           (GPIO_MFP_BASE+0x0070)
#define     PADDRV1                                                           (GPIO_MFP_BASE+0x0074)
#define     PADDRV2                                                           (GPIO_MFP_BASE+0x0078)
#define     PADDRV3                                                           (GPIO_MFP_BASE+0x007C)
#define     DEBUGIE                                                           (GPIO_MFP_BASE+0x0080)
#define     DEBUGOE                                                           (GPIO_MFP_BASE+0x0084)
#define     DEBUGSEL                                                          (GPIO_MFP_BASE+0x0088)
#define     LED_SEG_RC_EN                                                     (GPIO_MFP_BASE+0x008C)
#define     LED_SEG_BIAS_EN                                                   (GPIO_MFP_BASE+0x0090)
#define     DEBUGSIOIE                                                        (GPIO_MFP_BASE+0x0094)
#define     DEBUGSIOOE                                                        (GPIO_MFP_BASE+0x0098)
#define     DEBUGBIE                                                          (GPIO_MFP_BASE+0x009c)
#define     DEBUGBOE                                                          (GPIO_MFP_BASE+0x00a0)
#define     AD_Select1                                                        (GPIO_MFP_BASE+0x00a4)

//--------------Bits Location------------------------------------------//
#define     GPIOAOUTEN_GPIOAOUTEN_e                                           31
#define     GPIOAOUTEN_GPIOAOUTEN_SHIFT                                       0
#define     GPIOAOUTEN_GPIOAOUTEN_MASK                                        (0xFFFFFFFF<<0)

#define     GPIOAINEN_GPIOAINEN_e                                             31
#define     GPIOAINEN_GPIOAINEN_SHIFT                                         0
#define     GPIOAINEN_GPIOAINEN_MASK                                          (0xFFFFFFFF<<0)

#define     GPIOADAT_GPIOADAT_e                                               31
#define     GPIOADAT_GPIOADAT_SHIFT                                           0
#define     GPIOADAT_GPIOADAT_MASK                                            (0xFFFFFFFF<<0)

#define     GPIOAPUEN_GPIOAPUEN_e                                             31
#define     GPIOAPUEN_GPIOAPUEN_SHIFT                                         0
#define     GPIOAPUEN_GPIOAPUEN_MASK                                          (0xFFFFFFFF<<0)

#define     GPIOAPDEN_GPIOAPDEN_e                                             31
#define     GPIOAPDEN_GPIOAPDEN_SHIFT                                         0
#define     GPIOAPDEN_GPIOAPDEN_MASK                                          (0xFFFFFFFF<<0)

#define     GPIOBOUTEN_GPIOBOUTEN_e                                           12
#define     GPIOBOUTEN_GPIOBOUTEN_SHIFT                                       1
#define     GPIOBOUTEN_GPIOBOUTEN_MASK                                        (0xFFF<<1)

#define     GPIOBINEN_GPIOBINEN_e                                             12
#define     GPIOBINEN_GPIOBINEN_SHIFT                                         1
#define     GPIOBINEN_GPIOBINEN_MASK                                          (0xFFF<<1)

#define     GPIOBDAT_GPIOBDAT_e                                               12
#define     GPIOBDAT_GPIOBDAT_SHIFT                                           1
#define     GPIOBDAT_GPIOBDAT_MASK                                            (0xFFF<<1)

#define     GPIOBPUEN_GPIOBPUEN_e                                             12
#define     GPIOBPUEN_GPIOBPUEN_SHIFT                                         1
#define     GPIOBPUEN_GPIOBPUEN_MASK                                          (0xFFF<<1)

#define     GPIOBPDEN_GPIOBPDEN_e                                             12
#define     GPIOBPDEN_GPIOBPDEN_SHIFT                                         1
#define     GPIOBPDEN_GPIOBPDEN_MASK                                          (0xFFF<<1)

#define     SIO_OUTEN_SIO_OUTEN1_e                                            9
#define     SIO_OUTEN_SIO_OUTEN1_SHIFT                                        0
#define     SIO_OUTEN_SIO_OUTEN1_MASK                                         (0x3FF<<0)

#define     SIO_INEN_SIO_INEN_e                                               9
#define     SIO_INEN_SIO_INEN_SHIFT                                           0
#define     SIO_INEN_SIO_INEN_MASK                                            (0x3FF<<0)

#define     SIO_DAT_SIO_DAT_e                                                 9
#define     SIO_DAT_SIO_DAT_SHIFT                                             0
#define     SIO_DAT_SIO_DAT_MASK                                              (0x3FF<<0)

#define     SIO_PUEN_SIO_PUEN_e                                               9
#define     SIO_PUEN_SIO_PUEN_SHIFT                                           0
#define     SIO_PUEN_SIO_PUEN_MASK                                            (0x3FF<<0)

#define     SIO_PDEN_SIO_PDEN_e                                               9
#define     SIO_PDEN_SIO_PDEN_SHIFT                                           0
#define     SIO_PDEN_SIO_PDEN_MASK                                            (0x3FF<<0)

#define     GPIOB0_CTL_GPIOB0_MFP_e                                           10
#define     GPIOB0_CTL_GPIOB0_MFP_SHIFT                                       9
#define     GPIOB0_CTL_GPIOB0_MFP_MASK                                        (0x3<<9)
#define     GPIOB0_CTL_GPIOB0_DRV_e                                           8
#define     GPIOB0_CTL_GPIOB0_DRV_SHIFT                                       7
#define     GPIOB0_CTL_GPIOB0_DRV_MASK                                        (0x3<<7)
#define     GPIOB0_CTL_GPIOB0OUTEN                                            4
#define     GPIOB0_CTL_GPIOB0INEN                                             3
#define     GPIOB0_CTL_GPIOB0DAT                                              2
#define     GPIOB0_CTL_GPIOB0PUEN                                             1
#define     GPIOB0_CTL_GPIOB0PDEN                                             0

#define     PADDRV4_SPIBT_SS_e                                                31
#define     PADDRV4_SPIBT_SS_SHIFT                                            30
#define     PADDRV4_SPIBT_SS_MASK                                             (0x3<<30)
#define     PADDRV4_SPIBT_CLK_e                                               29
#define     PADDRV4_SPIBT_CLK_SHIFT                                           28
#define     PADDRV4_SPIBT_CLK_MASK                                            (0x3<<28)
#define     PADDRV4_SPIBT_MOSI_e                                              27
#define     PADDRV4_SPIBT_MOSI_SHIFT                                          26
#define     PADDRV4_SPIBT_MOSI_MASK                                           (0x3<<26)
#define     PADDRV4_SPIBT_MISO_e                                              25
#define     PADDRV4_SPIBT_MISO_SHIFT                                          24
#define     PADDRV4_SPIBT_MISO_MASK                                           (0x3<<24)
#define     PADDRV4_GPIOA31_e                                                 23
#define     PADDRV4_GPIOA31_SHIFT                                             21
#define     PADDRV4_GPIOA31_MASK                                              (0x7<<21)
#define     PADDRV4_GPIOA30_e                                                 20
#define     PADDRV4_GPIOA30_SHIFT                                             18
#define     PADDRV4_GPIOA30_MASK                                              (0x7<<18)
#define     PADDRV4_GPIOB12_e                                                 17
#define     PADDRV4_GPIOB12_SHIFT                                             15
#define     PADDRV4_GPIOB12_MASK                                              (0x7<<15)
#define     PADDRV4_GPIOB11_e                                                 14
#define     PADDRV4_GPIOB11_SHIFT                                             12
#define     PADDRV4_GPIOB11_MASK                                              (0x7<<12)
#define     PADDRV4_SIO9_e                                                    11
#define     PADDRV4_SIO9_SHIFT                                                9
#define     PADDRV4_SIO9_MASK                                                 (0x7<<9)
#define     PADDRV4_SIO8_e                                                    8
#define     PADDRV4_SIO8_SHIFT                                                6
#define     PADDRV4_SIO8_MASK                                                 (0x7<<6)
#define     PADDRV4_SIO7_e                                                    5
#define     PADDRV4_SIO7_SHIFT                                                3
#define     PADDRV4_SIO7_MASK                                                 (0x7<<3)
#define     PADDRV4_SIO6_e                                                    2
#define     PADDRV4_SIO6_SHIFT                                                0
#define     PADDRV4_SIO6_MASK                                                 (0x7<<0)

#define     PWM3_CTL_POL_SEL                                                  27
#define     PWM3_CTL_Mode_SEL                                                 26
#define     PWM3_CTL_Q_e                                                      25
#define     PWM3_CTL_Q_SHIFT                                                  24
#define     PWM3_CTL_Q_MASK                                                   (0x3<<24)
#define     PWM3_CTL_H_e                                                      23
#define     PWM3_CTL_H_SHIFT                                                  16
#define     PWM3_CTL_H_MASK                                                   (0xFF<<16)
#define     PWM3_CTL_L_e                                                      15
#define     PWM3_CTL_L_SHIFT                                                  8
#define     PWM3_CTL_L_MASK                                                   (0xFF<<8)
#define     PWM3_CTL_DUTY_e                                                   7
#define     PWM3_CTL_DUTY_SHIFT                                               0
#define     PWM3_CTL_DUTY_MASK                                                (0xFF<<0)

#define     PWM0_CTL_POL_SEL                                                  27
#define     PWM0_CTL_Mode_SEL                                                 26
#define     PWM0_CTL_Q_e                                                      25
#define     PWM0_CTL_Q_SHIFT                                                  24
#define     PWM0_CTL_Q_MASK                                                   (0x3<<24)
#define     PWM0_CTL_H_e                                                      23
#define     PWM0_CTL_H_SHIFT                                                  16
#define     PWM0_CTL_H_MASK                                                   (0xFF<<16)
#define     PWM0_CTL_L_e                                                      15
#define     PWM0_CTL_L_SHIFT                                                  8
#define     PWM0_CTL_L_MASK                                                   (0xFF<<8)
#define     PWM0_CTL_DUTY_e                                                   7
#define     PWM0_CTL_DUTY_SHIFT                                               0
#define     PWM0_CTL_DUTY_MASK                                                (0xFF<<0)

#define     PWM1_CTL_POL_SEL                                                  27
#define     PWM1_CTL_Mode_SEL                                                 26
#define     PWM1_CTL_Q_e                                                      25
#define     PWM1_CTL_Q_SHIFT                                                  24
#define     PWM1_CTL_Q_MASK                                                   (0x3<<24)
#define     PWM1_CTL_H_e                                                      23
#define     PWM1_CTL_H_SHIFT                                                  16
#define     PWM1_CTL_H_MASK                                                   (0xFF<<16)
#define     PWM1_CTL_L_e                                                      15
#define     PWM1_CTL_L_SHIFT                                                  8
#define     PWM1_CTL_L_MASK                                                   (0xFF<<8)
#define     PWM1_CTL_DUTY_e                                                   7
#define     PWM1_CTL_DUTY_SHIFT                                               0
#define     PWM1_CTL_DUTY_MASK                                                (0xFF<<0)

#define     PWM2_CTL_POL_SEL                                                  27
#define     PWM2_CTL_Mode_SEL                                                 26
#define     PWM2_CTL_Q_e                                                      25
#define     PWM2_CTL_Q_SHIFT                                                  24
#define     PWM2_CTL_Q_MASK                                                   (0x3<<24)
#define     PWM2_CTL_H_e                                                      23
#define     PWM2_CTL_H_SHIFT                                                  16
#define     PWM2_CTL_H_MASK                                                   (0xFF<<16)
#define     PWM2_CTL_L_e                                                      15
#define     PWM2_CTL_L_SHIFT                                                  8
#define     PWM2_CTL_L_MASK                                                   (0xFF<<8)
#define     PWM2_CTL_DUTY_e                                                   7
#define     PWM2_CTL_DUTY_SHIFT                                               0
#define     PWM2_CTL_DUTY_MASK                                                (0xFF<<0)

#define     MFP_CTL0_P_SEL0                                                   31
#define     MFP_CTL0_P_SEL2                                                   30
#define     MFP_CTL0_GPIOA23_e                                                29
#define     MFP_CTL0_GPIOA23_SHIFT                                            27
#define     MFP_CTL0_GPIOA23_MASK                                             (0x7<<27)
#define     MFP_CTL0_GPIOA22_e                                                26
#define     MFP_CTL0_GPIOA22_SHIFT                                            24
#define     MFP_CTL0_GPIOA22_MASK                                             (0x7<<24)
#define     MFP_CTL0_GPIOA21_e                                                23
#define     MFP_CTL0_GPIOA21_SHIFT                                            21
#define     MFP_CTL0_GPIOA21_MASK                                             (0x7<<21)
#define     MFP_CTL0_GPIOA20_e                                                20
#define     MFP_CTL0_GPIOA20_SHIFT                                            18
#define     MFP_CTL0_GPIOA20_MASK                                             (0x7<<18)
#define     MFP_CTL0_GPIOA19                                                  17
#define     MFP_CTL0_GPIOA18_e                                                16
#define     MFP_CTL0_GPIOA18_SHIFT                                            15
#define     MFP_CTL0_GPIOA18_MASK                                             (0x3<<15)
#define     MFP_CTL0_GPIOA17_e                                                14
#define     MFP_CTL0_GPIOA17_SHIFT                                            13
#define     MFP_CTL0_GPIOA17_MASK                                             (0x3<<13)
#define     MFP_CTL0_GPIOA16_e                                                12
#define     MFP_CTL0_GPIOA16_SHIFT                                            11
#define     MFP_CTL0_GPIOA16_MASK                                             (0x3<<11)
#define     MFP_CTL0_GPIOA15_e                                                10
#define     MFP_CTL0_GPIOA15_SHIFT                                            9
#define     MFP_CTL0_GPIOA15_MASK                                             (0x3<<9)
#define     MFP_CTL0_GPIOA14_e                                                8
#define     MFP_CTL0_GPIOA14_SHIFT                                            7
#define     MFP_CTL0_GPIOA14_MASK                                             (0x3<<7)
#define     MFP_CTL0_GPIOA13_e                                                6
#define     MFP_CTL0_GPIOA13_SHIFT                                            5
#define     MFP_CTL0_GPIOA13_MASK                                             (0x3<<5)
#define     MFP_CTL0_GPIOA12_e                                                4
#define     MFP_CTL0_GPIOA12_SHIFT                                            3
#define     MFP_CTL0_GPIOA12_MASK                                             (0x3<<3)
#define     MFP_CTL0_GPIOA11_e                                                2
#define     MFP_CTL0_GPIOA11_SHIFT                                            0
#define     MFP_CTL0_GPIOA11_MASK                                             (0x7<<0)

#define     MFP_CTL1_GPIOA10_e                                                31
#define     MFP_CTL1_GPIOA10_SHIFT                                            29
#define     MFP_CTL1_GPIOA10_MASK                                             (0x7<<29)
#define     MFP_CTL1_GPIOA9_e                                                 28
#define     MFP_CTL1_GPIOA9_SHIFT                                             26
#define     MFP_CTL1_GPIOA9_MASK                                              (0x7<<26)
#define     MFP_CTL1_GPIOA8_e                                                 25
#define     MFP_CTL1_GPIOA8_SHIFT                                             23
#define     MFP_CTL1_GPIOA8_MASK                                              (0x7<<23)
#define     MFP_CTL1_GPIOA7_e                                                 22
#define     MFP_CTL1_GPIOA7_SHIFT                                             20
#define     MFP_CTL1_GPIOA7_MASK                                              (0x7<<20)
#define     MFP_CTL1_GPIOA6_e                                                 19
#define     MFP_CTL1_GPIOA6_SHIFT                                             18
#define     MFP_CTL1_GPIOA6_MASK                                              (0x3<<18)
#define     MFP_CTL1_GPIOA5_e                                                 17
#define     MFP_CTL1_GPIOA5_SHIFT                                             16
#define     MFP_CTL1_GPIOA5_MASK                                              (0x3<<16)
#define     MFP_CTL1_GPIOA4_e                                                 15
#define     MFP_CTL1_GPIOA4_SHIFT                                             13
#define     MFP_CTL1_GPIOA4_MASK                                              (0x7<<13)
#define     MFP_CTL1_GPIOA3_e                                                 12
#define     MFP_CTL1_GPIOA3_SHIFT                                             10
#define     MFP_CTL1_GPIOA3_MASK                                              (0x7<<10)
#define     MFP_CTL1_GPIOA2_e                                                 9
#define     MFP_CTL1_GPIOA2_SHIFT                                             7
#define     MFP_CTL1_GPIOA2_MASK                                              (0x7<<7)
#define     MFP_CTL1_GPIOA1_e                                                 6
#define     MFP_CTL1_GPIOA1_SHIFT                                             3
#define     MFP_CTL1_GPIOA1_MASK                                              (0xF<<3)
#define     MFP_CTL1_GPIOA0_e                                                 2
#define     MFP_CTL1_GPIOA0_SHIFT                                             0
#define     MFP_CTL1_GPIOA0_MASK                                              (0x7<<0)

#define     MFP_CTL2_MEJTAGEN                                                 31
#define     MFP_CTL2_DEJTAGEN                                                 30
#define     MFP_CTL2_EJTAGMAP_e                                               29
#define     MFP_CTL2_EJTAGMAP_SHIFT                                           28
#define     MFP_CTL2_EJTAGMAP_MASK                                            (0x3<<28)
#define     MFP_CTL2_GPIOA27_MFP_e                                            27
#define     MFP_CTL2_GPIOA27_MFP_SHIFT                                        26
#define     MFP_CTL2_GPIOA27_MFP_MASK                                         (0x3<<26)
#define     MFP_CTL2_GPIOA26_MFP_e                                            25
#define     MFP_CTL2_GPIOA26_MFP_SHIFT                                        24
#define     MFP_CTL2_GPIOA26_MFP_MASK                                         (0x3<<24)
#define     MFP_CTL2_GPIOA25_MFP_e                                            23
#define     MFP_CTL2_GPIOA25_MFP_SHIFT                                        22
#define     MFP_CTL2_GPIOA25_MFP_MASK                                         (0x3<<22)
#define     MFP_CTL2_GPIOA24_MFP_e                                            21
#define     MFP_CTL2_GPIOA24_MFP_SHIFT                                        20:
#define     MFP_CTL2_GPIOA24_MFP_MASK                                         (0x3<<20:)
#define     MFP_CTL2_GPIO_B12                                                 19
#define     MFP_CTL2_GPIO_B11_e                                               18
#define     MFP_CTL2_GPIO_B11_SHIFT                                           16
#define     MFP_CTL2_GPIO_B11_MASK                                            (0x7<<16)
#define     MFP_CTL2_GPIO_B5                                                  15
#define     MFP_CTL2_GPIO_B4_e                                                14
#define     MFP_CTL2_GPIO_B4_SHIFT                                            13
#define     MFP_CTL2_GPIO_B4_MASK                                             (0x3<<13)
#define     MFP_CTL2_GPIO_B3                                                  12
#define     MFP_CTL2_GPIO_B2                                                  11
#define     MFP_CTL2_GPIO_B1                                                  10
#define     MFP_CTL2_SPIBT                                                    9
#define     MFP_CTL2_GPIOA31_e                                                8
#define     MFP_CTL2_GPIOA31_SHIFT                                            7
#define     MFP_CTL2_GPIOA31_MASK                                             (0x3<<7)
#define     MFP_CTL2_GPIOA30_e                                                6
#define     MFP_CTL2_GPIOA30_SHIFT                                            5
#define     MFP_CTL2_GPIOA30_MASK                                             (0x3<<5)
#define     MFP_CTL2_GPIOA29_e                                                4
#define     MFP_CTL2_GPIOA29_SHIFT                                            2
#define     MFP_CTL2_GPIOA29_MASK                                             (0x7<<2)
#define     MFP_CTL2_GPIOA28_e                                                1
#define     MFP_CTL2_GPIOA28_SHIFT                                            0
#define     MFP_CTL2_GPIOA28_MASK                                             (0x3<<0)

#define     MFP_CTL3_GPIOA16                                                  31
#define     MFP_CTL3_GPIOA14                                                  30
#define     MFP_CTL3_GPIOA13                                                  29
#define     MFP_CTL3_GPIOA12                                                  28
#define     MFP_CTL3_GPIOA3                                                   27
#define     MFP_CTL3_GPIOA2                                                   26
#define     MFP_CTL3_GPIOA1                                                   25
#define     MFP_CTL3_GPIOA0                                                   24
#define     MFP_CTL3_GPIOB9_e                                                 23
#define     MFP_CTL3_GPIOB9_SHIFT                                             22
#define     MFP_CTL3_GPIOB9_MASK                                              (0x3<<22)
#define     MFP_CTL3_GPIOB8_e                                                 21
#define     MFP_CTL3_GPIOB8_SHIFT                                             20
#define     MFP_CTL3_GPIOB8_MASK                                              (0x3<<20)
#define     MFP_CTL3_VROS_e                                                   19
#define     MFP_CTL3_VROS_SHIFT                                               18
#define     MFP_CTL3_VROS_MASK                                                (0x3<<18)
#define     MFP_CTL3_AOUTR_e                                                  17
#define     MFP_CTL3_AOUTR_SHIFT                                              16
#define     MFP_CTL3_AOUTR_MASK                                               (0x3<<16)
#define     MFP_CTL3_VRO_e                                                    15
#define     MFP_CTL3_VRO_SHIFT                                                14
#define     MFP_CTL3_VRO_MASK                                                 (0x3<<14)
#define     MFP_CTL3_AOUTL_e                                                  13
#define     MFP_CTL3_AOUTL_SHIFT                                              12
#define     MFP_CTL3_AOUTL_MASK                                               (0x3<<12)
#define     MFP_CTL3_GPIOB7_e                                                 9
#define     MFP_CTL3_GPIOB7_SHIFT                                             8
#define     MFP_CTL3_GPIOB7_MASK                                              (0x3<<8)
#define     MFP_CTL3_AUX1R                                                    7
#define     MFP_CTL3_AUX1L                                                    6
#define     MFP_CTL3_AUX0R                                                    5
#define     MFP_CTL3_AUX0L                                                    4
#define     MFP_CTL3_MICINR_e                                                 3
#define     MFP_CTL3_MICINR_SHIFT                                             2
#define     MFP_CTL3_MICINR_MASK                                              (0x3<<2)
#define     MFP_CTL3_MICINL_e                                                 1
#define     MFP_CTL3_MICINL_SHIFT                                             0
#define     MFP_CTL3_MICINL_MASK                                              (0x3<<0)

#define     AD_Select_GPIOA15_e                                               31
#define     AD_Select_GPIOA15_SHIFT                                           30
#define     AD_Select_GPIOA15_MASK                                            (0x3<<30)
#define     AD_Select_GPIOA14_e                                               29
#define     AD_Select_GPIOA14_SHIFT                                           28
#define     AD_Select_GPIOA14_MASK                                            (0x3<<28)
#define     AD_Select_GPIOA13_e                                               27
#define     AD_Select_GPIOA13_SHIFT                                           26
#define     AD_Select_GPIOA13_MASK                                            (0x3<<26)
#define     AD_Select_GPIOA12_e                                               25
#define     AD_Select_GPIOA12_SHIFT                                           24
#define     AD_Select_GPIOA12_MASK                                            (0x3<<24)
#define     AD_Select_GPIOA11_e                                               23
#define     AD_Select_GPIOA11_SHIFT                                           22
#define     AD_Select_GPIOA11_MASK                                            (0x3<<22)
#define     AD_Select_GPIOA10_e                                               21
#define     AD_Select_GPIOA10_SHIFT                                           20
#define     AD_Select_GPIOA10_MASK                                            (0x3<<20)
#define     AD_Select_GPIOA9_e                                                19
#define     AD_Select_GPIOA9_SHIFT                                            18
#define     AD_Select_GPIOA9_MASK                                             (0x3<<18)
#define     AD_Select_GPIOA8_e                                                17
#define     AD_Select_GPIOA8_SHIFT                                            16
#define     AD_Select_GPIOA8_MASK                                             (0x3<<16)
#define     AD_Select_GPIOA23_e                                               15
#define     AD_Select_GPIOA23_SHIFT                                           14
#define     AD_Select_GPIOA23_MASK                                            (0x3<<14)
#define     AD_Select_GPIOA6_e                                                13
#define     AD_Select_GPIOA6_SHIFT                                            12
#define     AD_Select_GPIOA6_MASK                                             (0x3<<12)
#define     AD_Select_GPIOA5_e                                                11
#define     AD_Select_GPIOA5_SHIFT                                            10
#define     AD_Select_GPIOA5_MASK                                             (0x3<<10)
#define     AD_Select_GPIOB10_e                                               9
#define     AD_Select_GPIOB10_SHIFT                                           8
#define     AD_Select_GPIOB10_MASK                                            (0x3<<8)
#define     AD_Select_GPIOB9_e                                                7
#define     AD_Select_GPIOB9_SHIFT                                            6
#define     AD_Select_GPIOB9_MASK                                             (0x3<<6)
#define     AD_Select_GPIOB8_e                                                5
#define     AD_Select_GPIOB8_SHIFT                                            4
#define     AD_Select_GPIOB8_MASK                                             (0x3<<4)
#define     AD_Select_GPIOB7_e                                                3
#define     AD_Select_GPIOB7_SHIFT                                            2
#define     AD_Select_GPIOB7_MASK                                             (0x3<<2)
#define     AD_Select_GPIOA21_e                                               1
#define     AD_Select_GPIOA21_SHIFT                                           0
#define     AD_Select_GPIOA21_MASK                                            (0x3<<0)

#define     PADPUPD_SD_DATA4_7                                                9
#define     PADPUPD_SD_CMD                                                    7
#define     PADPUPD_SD_DATA0_3                                                6
#define     PADPUPD_SIRQ1PD                                                   5
#define     PADPUPD_SIRQ1PU                                                   4
#define     PADPUPD_SIRQ0PD                                                   3
#define     PADPUPD_SIRQ0PU                                                   2
#define     PADPUPD_I2C                                                       1
#define     PADPUPD_UART_RX1                                                  0

#define     PAD_SMIT_SPIBT_SS                                                 22
#define     PAD_SMIT_GPIOA30                                                  21
#define     PAD_SMIT_GPIOA29                                                  20
#define     PAD_SMIT_GPIOA28                                                  19
#define     PAD_SMIT_GPIOA22                                                  18
#define     PAD_SMIT_GPIOA19                                                  17
#define     PAD_SMIT_GPIOA18                                                  16
#define     PAD_SMIT_GPIOA11                                                  15
#define     PAD_SMIT_GPIOA10                                                  14
#define     PAD_SMIT_GPIOA9                                                   13
#define     PAD_SMIT_GPIOA7                                                   12
#define     PAD_SMIT_GPIOA3                                                   11
#define     PAD_SMIT_GPIOA2                                                   10
#define     PAD_SMIT_GPIOA1                                                   9
#define     PAD_SMIT_GPIOA0                                                   8
#define     PAD_SMIT_GPIOB12                                                  7
#define     PAD_SMIT_GPIOB11                                                  6
#define     PAD_SMIT_GPIOB9                                                   5
#define     PAD_SMIT_GPIOB8                                                   4
#define     PAD_SMIT_GPIOB7                                                   3
#define     PAD_SMIT_GPIOB4                                                   2
#define     PAD_SMIT_GPIOB3                                                   1
#define     PAD_SMIT_GPIOB2                                                   0

#define     PADDRV0_GPIOA9_e                                                  29
#define     PADDRV0_GPIOA9_SHIFT                                              27
#define     PADDRV0_GPIOA9_MASK                                               (0x7<<27)
#define     PADDRV0_GPIOA8_e                                                  26
#define     PADDRV0_GPIOA8_SHIFT                                              24
#define     PADDRV0_GPIOA8_MASK                                               (0x7<<24)
#define     PADDRV0_GPIOA7_e                                                  23
#define     PADDRV0_GPIOA7_SHIFT                                              21
#define     PADDRV0_GPIOA7_MASK                                               (0x7<<21)
#define     PADDRV0_GPIOA6_e                                                  20
#define     PADDRV0_GPIOA6_SHIFT                                              18
#define     PADDRV0_GPIOA6_MASK                                               (0x7<<18)
#define     PADDRV0_GPIOA5_e                                                  17
#define     PADDRV0_GPIOA5_SHIFT                                              15
#define     PADDRV0_GPIOA5_MASK                                               (0x7<<15)
#define     PADDRV0_GPIOA4_e                                                  14
#define     PADDRV0_GPIOA4_SHIFT                                              12
#define     PADDRV0_GPIOA4_MASK                                               (0x7<<12)
#define     PADDRV0_GPIOA3_e                                                  11
#define     PADDRV0_GPIOA3_SHIFT                                              9
#define     PADDRV0_GPIOA3_MASK                                               (0x7<<9)
#define     PADDRV0_GPIOA2_e                                                  8
#define     PADDRV0_GPIOA2_SHIFT                                              6
#define     PADDRV0_GPIOA2_MASK                                               (0x7<<6)
#define     PADDRV0_GPIOA1_e                                                  5
#define     PADDRV0_GPIOA1_SHIFT                                              3
#define     PADDRV0_GPIOA1_MASK                                               (0x7<<3)
#define     PADDRV0_GPIOA0_e                                                  2
#define     PADDRV0_GPIOA0_SHIFT                                              0
#define     PADDRV0_GPIOA0_MASK                                               (0x7<<0)

#define     PADDRV1_GPIOA19_e                                                 29
#define     PADDRV1_GPIOA19_SHIFT                                             27
#define     PADDRV1_GPIOA19_MASK                                              (0x7<<27)
#define     PADDRV1_GPIOA18_e                                                 26
#define     PADDRV1_GPIOA18_SHIFT                                             24
#define     PADDRV1_GPIOA18_MASK                                              (0x7<<24)
#define     PADDRV1_GPIOA17_e                                                 23
#define     PADDRV1_GPIOA17_SHIFT                                             21
#define     PADDRV1_GPIOA17_MASK                                              (0x7<<21)
#define     PADDRV1_GPIOA16_e                                                 20
#define     PADDRV1_GPIOA16_SHIFT                                             18
#define     PADDRV1_GPIOA16_MASK                                              (0x7<<18)
#define     PADDRV1_GPIOA15_e                                                 17
#define     PADDRV1_GPIOA15_SHIFT                                             15
#define     PADDRV1_GPIOA15_MASK                                              (0x7<<15)
#define     PADDRV1_GPIOA14_e                                                 14
#define     PADDRV1_GPIOA14_SHIFT                                             12
#define     PADDRV1_GPIOA14_MASK                                              (0x7<<12)
#define     PADDRV1_GPIOA13_e                                                 11
#define     PADDRV1_GPIOA13_SHIFT                                             9
#define     PADDRV1_GPIOA13_MASK                                              (0x7<<9)
#define     PADDRV1_GPIOA12_e                                                 8
#define     PADDRV1_GPIOA12_SHIFT                                             6
#define     PADDRV1_GPIOA12_MASK                                              (0x7<<6)
#define     PADDRV1_GPIOA11_e                                                 5
#define     PADDRV1_GPIOA11_SHIFT                                             3
#define     PADDRV1_GPIOA11_MASK                                              (0x7<<3)
#define     PADDRV1_GPIOA10_e                                                 2
#define     PADDRV1_GPIOA10_SHIFT                                             0
#define     PADDRV1_GPIOA10_MASK                                              (0x7<<0)

#define     PADDRV2_GPIOA29_e                                                 29
#define     PADDRV2_GPIOA29_SHIFT                                             27
#define     PADDRV2_GPIOA29_MASK                                              (0x7<<27)
#define     PADDRV2_GPIOA28_e                                                 26
#define     PADDRV2_GPIOA28_SHIFT                                             24
#define     PADDRV2_GPIOA28_MASK                                              (0x7<<24)
#define     PADDRV2_GPIOA27_e                                                 23
#define     PADDRV2_GPIOA27_SHIFT                                             21
#define     PADDRV2_GPIOA27_MASK                                              (0x7<<21)
#define     PADDRV2_GPIOA26_e                                                 20
#define     PADDRV2_GPIOA26_SHIFT                                             18
#define     PADDRV2_GPIOA26_MASK                                              (0x7<<18)
#define     PADDRV2_GPIOA25_e                                                 17
#define     PADDRV2_GPIOA25_SHIFT                                             15
#define     PADDRV2_GPIOA25_MASK                                              (0x7<<15)
#define     PADDRV2_GPIOA24_e                                                 14
#define     PADDRV2_GPIOA24_SHIFT                                             12
#define     PADDRV2_GPIOA24_MASK                                              (0x7<<12)
#define     PADDRV2_GPIOA23_e                                                 11
#define     PADDRV2_GPIOA23_SHIFT                                             9
#define     PADDRV2_GPIOA23_MASK                                              (0x7<<9)
#define     PADDRV2_GPIOA22_e                                                 8
#define     PADDRV2_GPIOA22_SHIFT                                             6
#define     PADDRV2_GPIOA22_MASK                                              (0x7<<6)
#define     PADDRV2_GPIOA21_e                                                 5
#define     PADDRV2_GPIOA21_SHIFT                                             3
#define     PADDRV2_GPIOA21_MASK                                              (0x7<<3)
#define     PADDRV2_GPIOA20_e                                                 2
#define     PADDRV2_GPIOA20_SHIFT                                             0
#define     PADDRV2_GPIOA20_MASK                                              (0x7<<0)

#define     PADDRV3_GPIOB10_e                                                 29
#define     PADDRV3_GPIOB10_SHIFT                                             27
#define     PADDRV3_GPIOB10_MASK                                              (0x7<<27)
#define     PADDRV3_GPIOB9_e                                                  26
#define     PADDRV3_GPIOB9_SHIFT                                              24
#define     PADDRV3_GPIOB9_MASK                                               (0x7<<24)
#define     PADDRV3_GPIOB8_e                                                  23
#define     PADDRV3_GPIOB8_SHIFT                                              21
#define     PADDRV3_GPIOB8_MASK                                               (0x7<<21)
#define     PADDRV3_GPIOB7_e                                                  20
#define     PADDRV3_GPIOB7_SHIFT                                              18
#define     PADDRV3_GPIOB7_MASK                                               (0x7<<18)
#define     PADDRV3_GPIOB6_e                                                  17
#define     PADDRV3_GPIOB6_SHIFT                                              15
#define     PADDRV3_GPIOB6_MASK                                               (0x7<<15)
#define     PADDRV3_GPIOB5_e                                                  14
#define     PADDRV3_GPIOB5_SHIFT                                              12
#define     PADDRV3_GPIOB5_MASK                                               (0x7<<12)
#define     PADDRV3_GPIOB4_e                                                  11
#define     PADDRV3_GPIOB4_SHIFT                                              9
#define     PADDRV3_GPIOB4_MASK                                               (0x7<<9)
#define     PADDRV3_GPIOB3_e                                                  8
#define     PADDRV3_GPIOB3_SHIFT                                              6
#define     PADDRV3_GPIOB3_MASK                                               (0x7<<6)
#define     PADDRV3_GPIOB2_e                                                  5
#define     PADDRV3_GPIOB2_SHIFT                                              3
#define     PADDRV3_GPIOB2_MASK                                               (0x7<<3)
#define     PADDRV3_GPIOB1_e                                                  2
#define     PADDRV3_GPIOB1_SHIFT                                              0
#define     PADDRV3_GPIOB1_MASK                                               (0x7<<0)

#define     DEBUGIE_GPIOA_IN_DB_EN_e                                          31
#define     DEBUGIE_GPIOA_IN_DB_EN_SHIFT                                      0
#define     DEBUGIE_GPIOA_IN_DB_EN_MASK                                       (0xFFFFFFFF<<0)

#define     DEBUGOE_GPIOA_OUT_DB_EN_e                                         31
#define     DEBUGOE_GPIOA_OUT_DB_EN_SHIFT                                     0
#define     DEBUGOE_GPIOA_OUT_DB_EN_MASK                                      (0xFFFFFFFF<<0)

#define     DEBUGSEL_DBGSE_e                                                  4
#define     DEBUGSEL_DBGSE_SHIFT                                              0
#define     DEBUGSEL_DBGSE_MASK                                               (0x1F<<0)

#define     LED_SEG_RC_EN_LED_SEG7                                            7
#define     LED_SEG_RC_EN_LED_SEG6                                            6
#define     LED_SEG_RC_EN_LED_SEG5                                            5
#define     LED_SEG_RC_EN_LED_SEG4                                            4
#define     LED_SEG_RC_EN_LED_SEG3                                            3
#define     LED_SEG_RC_EN_LED_SEG2                                            2
#define     LED_SEG_RC_EN_LED_SEG1                                            1
#define     LED_SEG_RC_EN_LED_SEG0                                            0

#define     LED_SEG_BIAS_EN_LED_SEG_ALL_EN                                    3
#define     LED_SEG_BIAS_EN_LED_Cathode_Anode_Mode                            2
#define     LED_SEG_BIAS_EN_LED_SEG_BIAS_e                                    1
#define     LED_SEG_BIAS_EN_LED_SEG_BIAS_SHIFT                                0
#define     LED_SEG_BIAS_EN_LED_SEG_BIAS_MASK                                 (0x3<<0)

#define     DEBUGSIOIE_SIO_IN_DB_EN_e                                         9
#define     DEBUGSIOIE_SIO_IN_DB_EN_SHIFT                                     0
#define     DEBUGSIOIE_SIO_IN_DB_EN_MASK                                      (0x3FF<<0)

#define     DEBUGSIOOE_SIO_OUT_DB_EN_e                                        9
#define     DEBUGSIOOE_SIO_OUT_DB_EN_SHIFT                                    0
#define     DEBUGSIOOE_SIO_OUT_DB_EN_MASK                                     (0x3FF<<0)

#define     DEBUGBIE_GPIOB_IN_DB_EN_e                                         12
#define     DEBUGBIE_GPIOB_IN_DB_EN_SHIFT                                     1
#define     DEBUGBIE_GPIOB_IN_DB_EN_MASK                                      (0xFFF<<1)

#define     DEBUGBOE_GPIOB_OUT_DB_EN_e                                        12
#define     DEBUGBOE_GPIOB_OUT_DB_EN_SHIFT                                    1
#define     DEBUGBOE_GPIOB_OUT_DB_EN_MASK                                     (0xFFF<<1)

#define     AD_Select1_GPIOA22_e                                              2
#define     AD_Select1_GPIOA22_SHIFT                                          1
#define     AD_Select1_GPIOA22_MASK                                           (0x3<<1)
#define     AD_Select1_GPIOA2                                                 0

//--------------BT-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     BT_BASE                                                           0xc0120100
#define     BT_CFG                                                            (BT_BASE+0x00)

//--------------Bits Location------------------------------------------//
#define     BT_CFG_BT_EN                                                      31
#define     BT_CFG_shut_dwon_bt_n                                             29
#define     BT_CFG_PD12_RDY_N                                                 28
#define     BT_CFG_Host_wake_bt                                               7
#define     BT_CFG_PCM_err_flag                                               6
#define     BT_CFG_EEPROM_SEL                                                 5
#define     BT_CFG_TEST_SEL                                                   4
#define     BT_CFG_ICFG_SRC_e                                                 3
#define     BT_CFG_ICFG_SRC_SHIFT                                             0
#define     BT_CFG_ICFG_SRC_MASK                                              (0xF<<0)

//--------------TEST_MODE-------------------------------------------//
//--------------Register Address---------------------------------------//
#define     TEST_MODE_BASE                                                    0xc01b0000
#define     TEST_MODE_CTL                                                     (TEST_MODE_BASE+0x0000)

//--------------Bits Location------------------------------------------//
#define     TEST_MODE_CTL_Enable                                              4
#define     TEST_MODE_CTL_Sel_e                                               2
#define     TEST_MODE_CTL_Sel_SHIFT                                           0
#define     TEST_MODE_CTL_Sel_MASK                                            (0x7<<0)

//---------------------------------------------------------------------//
#endif
