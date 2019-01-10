#ifndef __REG_NORFLASHIP_V2_H__
#define __REG_NORFLASHIP_V2_H__

#include "plat_types.h"

struct NORFLASH_CTRL_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    union TXDATA_REG_T {
        __IO uint32_t TXWORD;
        __IO uint16_t TXHALFWORD;
        __IO uint8_t TXBYTE;
    } REG_008;
    __IO uint32_t REG_00C;
    __IO uint32_t REG_010;
    __IO uint32_t REG_014;
    __IO uint32_t REG_018;
    __IO uint32_t REG_01C;
    __IO uint32_t REG_020;
    __IO uint32_t REG_024;
    __IO uint32_t REG_028;
    __IO uint32_t REG_02C;
    __IO uint32_t REG_030;
    __IO uint32_t REG_034;
};

// REG_000
#define REG_000_ADDR_SHIFT                  8
#define REG_000_ADDR_MASK                   (0xFFFFFF << REG_000_ADDR_SHIFT)
#define REG_000_ADDR(n)                     BITFIELD_VAL(REG_000_ADDR, n)
#define REG_000_CMD_SHIFT                   0
#define REG_000_CMD_MASK                    (0xFF << REG_000_CMD_SHIFT)
#define REG_000_CMD(n)                      BITFIELD_VAL(REG_000_CMD, n)

#define REG_000_NEW_CMD_RX_LEN_SHIFT        13
#define REG_000_NEW_CMD_RX_LEN_MASK         (0x3FFF << REG_000_NEW_CMD_RX_LEN_SHIFT)
#define REG_000_NEW_CMD_RX_LEN(n)           BITFIELD_VAL(REG_000_NEW_CMD_RX_LEN, n)
#define REG_000_NEW_CMD_RX_LINE_SHIFT       11
#define REG_000_NEW_CMD_RX_LINE_MASK        (0x3 << REG_000_NEW_CMD_RX_LINE_SHIFT)
#define REG_000_NEW_CMD_RX_LINE(n)          BITFIELD_VAL(REG_000_NEW_CMD_RX_LINE, n)
#define REG_000_NEW_CMD_TX_LINE_SHIFT       9
#define REG_000_NEW_CMD_TX_LINE_MASK        (0x3 << REG_000_NEW_CMD_TX_LINE_SHIFT)
#define REG_000_NEW_CMD_TX_LINE(n)          BITFIELD_VAL(REG_000_NEW_CMD_TX_LINE, n)
#define REG_000_NEW_CMD_RX_EN               (1 << 8)

#define NEW_CMD_LINE_4X                     2
#define NEW_CMD_LINE_2X                     1
#define NEW_CMD_LINE_1X                     0

// REG_004
#define REG_004_RES26_SHIFT                 26
#define REG_004_RES26_MASK                  (0x3F << REG_004_RES26_SHIFT)
#define REG_004_RES26(n)                    BITFIELD_VAL(REG_004_RES26, n)
#define REG_004_CONTINUOUS_MODE             (1 << 25)
#define REG_004_BLOCK_SIZE_SHIFT            12
#define REG_004_BLOCK_SIZE_MASK             (0x1FFF << REG_004_BLOCK_SIZE_SHIFT)
#define REG_004_BLOCK_SIZE(n)               BITFIELD_VAL(REG_004_BLOCK_SIZE, n)
#define REG_004_MODEBIT_SHIFT               4
#define REG_004_MODEBIT_MASK                (0xFF << REG_004_MODEBIT_SHIFT)
#define REG_004_MODEBIT(n)                  BITFIELD_VAL(REG_004_MODEBIT, n)
#define REG_004_RES1_SHIFT                  1
#define REG_004_RES1_MASK                   (0x7 << REG_004_RES1_SHIFT)
#define REG_004_RES1(n)                     BITFIELD_VAL(REG_004_RES1, n)
#define REG_004_NEW_CMD_EN                  (1 << 0)

// REG_008
#define REG_008_TXDATA_SHIFT                0
#define REG_008_TXDATA_MASK                 (0xFFFFFFFF << REG_008_TXDATA_SHIFT)
#define REG_008_TXDATA(n)                   BITFIELD_VAL(REG_008_TXDATA, n)

// REG_00C
#define REG_00C_RES_SHIFT                   13
#define REG_00C_RES_MASK                    (0x7FFFF << REG_00C_RES_SHIFT)
#define REG_00C_RES(n)                      BITFIELD_VAL(REG_00C_RES, n)
#define REG_00C_TXFIFO_EMPCNT_SHIFT         8
#define REG_00C_TXFIFO_EMPCNT_MASK          (0x1F << REG_00C_TXFIFO_EMPCNT_SHIFT)
#define REG_00C_TXFIFO_EMPCNT(n)            BITFIELD_VAL(REG_00C_TXFIFO_EMPCNT, n)
#define REG_00C_RXFIFO_COUNT_SHIFT          4
#define REG_00C_RXFIFO_COUNT_MASK           (0xF << REG_00C_RXFIFO_COUNT_SHIFT)
#define REG_00C_RXFIFO_COUNT(n)             BITFIELD_VAL(REG_00C_RXFIFO_COUNT, n)
#define REG_00C_RXFIFO_EMPTY                (1 << 3)
#define REG_00C_TXFIFO_FULL                 (1 << 2)
#define REG_00C_TXFIFO_EMPTY                (1 << 1)
#define REG_00C_BUSY                        (1 << 0)

// REG_010
#define REG_010_RXDATA_SHIFT                0
#define REG_010_RXDATA_MASK                 (0xFFFFFFFF << REG_010_RXDATA_SHIFT)
#define REG_010_RXDATA(n)                   BITFIELD_VAL(REG_010_RXDATA, n)

// REG_014
#define REG_014_RES29_SHIFT                 29
#define REG_014_RES29_MASK                  (0x7 << REG_014_RES29_SHIFT)
#define REG_014_RES29(n)                    BITFIELD_VAL(REG_014_RES29, n)
#define REG_014_EXTRA_SHSL_SHIFT            25
#define REG_014_EXTRA_SHSL_MASK             (0xF << REG_014_EXTRA_SHSL_SHIFT)
#define REG_014_EXTRA_SHSL(n)               BITFIELD_VAL(REG_014_EXTRA_SHSL, n)
#define REG_014_EXTRA_SHSL_EN               (1 << 24)
#define REG_014_CLKDIV_SHIFT                16
#define REG_014_CLKDIV_MASK                 (0xFF << REG_014_CLKDIV_SHIFT)
#define REG_014_CLKDIV(n)                   BITFIELD_VAL(REG_014_CLKDIV, n)
#define REG_014_SAMPLESEL_SHIFT             12
#define REG_014_SAMPLESEL_MASK              (0xF << REG_014_SAMPLESEL_SHIFT)
#define REG_014_SAMPLESEL(n)                BITFIELD_VAL(REG_014_SAMPLESEL, n)
#define REG_014_DUMMY_CYCLE_SHIFT           8
#define REG_014_DUMMY_CYCLE_MASK            (0xF << REG_014_DUMMY_CYCLE_SHIFT)
#define REG_014_DUMMY_CYCLE(n)              BITFIELD_VAL(REG_014_DUMMY_CYCLE, n)
#define REG_014_DUMMY_CYCLE_EN              (1 << 7)
#define REG_014_CMDQUAD                     (1 << 6)
#define REG_014_RAM_DUALMODE                (1 << 5)
#define REG_014_RAM_QUADMODE                (1 << 4)
#define REG_014_FOUR_BYTE_ADDR_EN           (1 << 3)
#define REG_014_CANCEL_EN                   (1 << 2)
#define REG_014_RES2                        (1 << 2)
#define REG_014_HOLDPIN                     (1 << 1)
#define REG_014_WPROPIN                     (1 << 0)

// REG_018
#define REG_018_RES_SHIFT                   2
#define REG_018_RES_MASK                    (0x3FFFFFFF << REG_018_RES_SHIFT)
#define REG_018_RES(n)                      BITFIELD_VAL(REG_018_RES, n)
#define REG_018_TXFIFOCLR                   (1 << 1)
#define REG_018_RXFIFOCLR                   (1 << 0)

// REG_01C
#define REG_01C_RES18_SHIFT                 18
#define REG_01C_RES18_MASK                  (0x3FFF << REG_01C_RES18_SHIFT)
#define REG_01C_RES18(n)                    BITFIELD_VAL(REG_01C_RES18, n)
#define REG_01C_DMA_RX_SIZE_SHIFT           16
#define REG_01C_DMA_RX_SIZE_MASK            (0x3 << REG_01C_DMA_RX_SIZE_SHIFT)
#define REG_01C_DMA_RX_SIZE(n)              BITFIELD_VAL(REG_01C_DMA_RX_SIZE, n)
#define REG_01C_TX_THRESHOLD_SHIFT          8
#define REG_01C_TX_THRESHOLD_MASK           (0x1F << REG_01C_TX_THRESHOLD_SHIFT)
#define REG_01C_TX_THRESHOLD(n)             BITFIELD_VAL(REG_01C_TX_THRESHOLD, n)
#define REG_01C_RX_THRESHOLD_SHIFT          4
#define REG_01C_RX_THRESHOLD_MASK           (0xF << REG_01C_RX_THRESHOLD_SHIFT)
#define REG_01C_RX_THRESHOLD(n)             BITFIELD_VAL(REG_01C_RX_THRESHOLD, n)
#define REG_01C_RES3                        (1 << 3)
#define REG_01C_DMACTRL_RX_EN               (1 << 2)
#define REG_01C_DMACTRL_TX_EN               (1 << 1)
#define REG_01C_NAND_SEL                    (1 << 0)

// REG_020
#define REG_020_DUALCMD_SHIFT               24
#define REG_020_DUALCMD_MASK                (0xFF << REG_020_DUALCMD_SHIFT)
#define REG_020_DUALCMD(n)                  BITFIELD_VAL(REG_020_DUALCMD, n)
#define REG_020_READCMD_SHIFT               16
#define REG_020_READCMD_MASK                (0xFF << REG_020_READCMD_SHIFT)
#define REG_020_READCMD(n)                  BITFIELD_VAL(REG_020_READCMD, n)
#define REG_020_FREADCMD_SHIFT              8
#define REG_020_FREADCMD_MASK               (0xFF << REG_020_FREADCMD_SHIFT)
#define REG_020_FREADCMD(n)                 BITFIELD_VAL(REG_020_FREADCMD, n)
#define REG_020_QUADCMD_SHIFT               0
#define REG_020_QUADCMD_MASK                (0xFF << REG_020_QUADCMD_SHIFT)
#define REG_020_QUADCMD(n)                  BITFIELD_VAL(REG_020_QUADCMD, n)

// REG_024
#define REG_024_CMD_SEQ1_SHIFT              0
#define REG_024_CMD_SEQ1_MASK               (0xFFFFFFFF << REG_024_CMD_SEQ1_SHIFT)
#define REG_024_CMD_SEQ1(n)                 BITFIELD_VAL(REG_024_CMD_SEQ1, n)

// REG_028
#define REG_028_CMD_SEQ_EN                  (1 << 4)
#define REG_028_CMD_SEQ2_SHIFT              0
#define REG_028_CMD_SEQ2_MASK               (0xF << REG_028_CMD_SEQ2_SHIFT)
#define REG_028_CMD_SEQ2(n)                 BITFIELD_VAL(REG_028_CMD_SEQ2, n)

// REG_02C
#define REG_02C_RES_SHIFT                   1
#define REG_02C_RES_MASK                    (0x7FFFFFFF << REG_02C_RES_SHIFT)
#define REG_02C_RES(n)                      BITFIELD_VAL(REG_02C_RES, n)
#define REG_02C_FETCH_EN                    (1 << 0)

// REG_030
#define REG_030_RES_SHIFT                   2
#define REG_030_RES_MASK                    (0x3FFFFFFF << REG_030_RES_SHIFT)
#define REG_030_RES(n)                      BITFIELD_VAL(REG_030_RES, n)
#define REG_030_ADDR_25_24_SHIFT            0
#define REG_030_ADDR_25_24_MASK             (0x3 << REG_030_ADDR_25_24_SHIFT)
#define REG_030_ADDR_25_24(n)               BITFIELD_VAL(REG_030_ADDR_25_24, n)

// REG_034
#define REG_034_RES_SHIFT                   18
#define REG_034_RES_MASK                    (0x3FFF << REG_034_RES_SHIFT)
#define REG_034_RES(n)                      BITFIELD_VAL(REG_034_RES, n)
#define REG_034_SPI_IODRV_SHIFT             16
#define REG_034_SPI_IODRV_MASK              (0x3 << REG_034_SPI_IODRV_SHIFT)
#define REG_034_SPI_IODRV(n)                BITFIELD_VAL(REG_034_SPI_IODRV, n)
#define REG_034_SPI_IORES_SHIFT             8
#define REG_034_SPI_IORES_MASK              (0xFF << REG_034_SPI_IORES_SHIFT)
#define REG_034_SPI_IORES(n)                BITFIELD_VAL(REG_034_SPI_IORES, n)
#define REG_034_SPI_RDEN_SHIFT              4
#define REG_034_SPI_RDEN_MASK               (0xF << REG_034_SPI_RDEN_SHIFT)
#define REG_034_SPI_RDEN(n)                 BITFIELD_VAL(REG_034_SPI_RDEN, n)
#define REG_034_SPI_RUEN_SHIFT              0
#define REG_034_SPI_RUEN_MASK               (0xF << REG_034_SPI_RUEN_SHIFT)
#define REG_034_SPI_RUEN(n)                 BITFIELD_VAL(REG_034_SPI_RUEN, n)

#endif

