#ifndef __REG_IOMUX_BEST3001_H_
#define __REG_IOMUX_BEST3001_H_

#include "plat_types.h"

struct IOMUX_T {
    __IO uint32_t REG_000;
    __IO uint32_t REG_004;
    __IO uint32_t REG_008;
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
    __IO uint32_t REG_038;
    __IO uint32_t REG_03C;
    __IO uint32_t REG_040;
    __IO uint32_t REG_044;
    __IO uint32_t REG_048;
    __IO uint32_t REG_04C;
    __IO uint32_t REG_050;
    __IO uint32_t REG_054;
    __IO uint32_t REG_058;
    __IO uint32_t REG_05C;
    __IO uint32_t REG_060;
    __IO uint32_t REG_064;
    __IO uint32_t REG_068;
    __IO uint32_t REG_06C;
    __IO uint32_t REG_070;
    __IO uint32_t REG_074;
};

// reg_00
#define IOMUX_REV_ID(n)                         (((n) & 0xF) << 0)
#define IOMUX_REV_ID_MASK                       (0xF << 0)
#define IOMUX_REV_ID_SHIFT                      (0)
#define IOMUX_CHIP_ID(n)                        (((n) & 0xFFFF) << 16)
#define IOMUX_CHIP_ID_MASK                      (0xFFFF << 16)
#define IOMUX_CHIP_ID_SHIFT                     (16)

// reg_04
#define IOMUX_GPIO_P00_SEL(n)                   (((n) & 0xF) << 0)
#define IOMUX_GPIO_P00_SEL_MASK                 (0xF << 0)
#define IOMUX_GPIO_P00_SEL_SHIFT                (0)
#define IOMUX_GPIO_P01_SEL(n)                   (((n) & 0xF) << 4)
#define IOMUX_GPIO_P01_SEL_MASK                 (0xF << 4)
#define IOMUX_GPIO_P01_SEL_SHIFT                (4)
#define IOMUX_GPIO_P02_SEL(n)                   (((n) & 0xF) << 8)
#define IOMUX_GPIO_P02_SEL_MASK                 (0xF << 8)
#define IOMUX_GPIO_P02_SEL_SHIFT                (8)
#define IOMUX_GPIO_P03_SEL(n)                   (((n) & 0xF) << 12)
#define IOMUX_GPIO_P03_SEL_MASK                 (0xF << 12)
#define IOMUX_GPIO_P03_SEL_SHIFT                (12)
#define IOMUX_GPIO_P04_SEL(n)                   (((n) & 0xF) << 16)
#define IOMUX_GPIO_P04_SEL_MASK                 (0xF << 16)
#define IOMUX_GPIO_P04_SEL_SHIFT                (16)
#define IOMUX_GPIO_P05_SEL(n)                   (((n) & 0xF) << 20)
#define IOMUX_GPIO_P05_SEL_MASK                 (0xF << 20)
#define IOMUX_GPIO_P05_SEL_SHIFT                (20)
#define IOMUX_GPIO_P06_SEL(n)                   (((n) & 0xF) << 24)
#define IOMUX_GPIO_P06_SEL_MASK                 (0xF << 24)
#define IOMUX_GPIO_P06_SEL_SHIFT                (24)
#define IOMUX_GPIO_P07_SEL(n)                   (((n) & 0xF) << 28)
#define IOMUX_GPIO_P07_SEL_MASK                 (0xF << 28)
#define IOMUX_GPIO_P07_SEL_SHIFT                (28)

// reg_08
#define IOMUX_GPIO_P0_SEL_VIO(n)                (((n) & 0xFF) << 0)
#define IOMUX_GPIO_P0_SEL_VIO_MASK              (0xFF << 0)
#define IOMUX_GPIO_P0_SEL_VIO_SHIFT             (0)

// reg_0c
#define IOMUX_GPIO_P0_PU(n)                     (((n) & 0xFF) << 0)
#define IOMUX_GPIO_P0_PU_MASK                   (0xFF << 0)
#define IOMUX_GPIO_P0_PU_SHIFT                  (0)

// reg_14
#define IOMUX_ANA_TEST_DIR(n)                   (((n) & 0xFF) << 0)
#define IOMUX_ANA_TEST_DIR_MASK                 (0xFF << 0)
#define IOMUX_ANA_TEST_DIR_SHIFT                (0)

// reg_18
#define IOMUX_ANA_TEST_SEL(n)                   (((n) & 0xF) << 0)
#define IOMUX_ANA_TEST_SEL_MASK                 (0xF << 0)
#define IOMUX_ANA_TEST_SEL_SHIFT                (0)

// reg_1c
#define IOMUX_RESERVED(n)                       (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_RESERVED_MASK                     (0xFFFFFFFF << 0)
#define IOMUX_RESERVED_SHIFT                    (0)

// reg_20
#define IOMUX_RESERVED2(n)                      (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_RESERVED2_MASK                    (0xFFFFFFFF << 0)
#define IOMUX_RESERVED2_SHIFT                   (0)

// reg_24
#define IOMUX_RESERVED3(n)                      (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_RESERVED3_MASK                    (0xFFFFFFFF << 0)
#define IOMUX_RESERVED3_SHIFT                   (0)

// reg_28
#define IOMUX_RESERVED4(n)                      (((n) & 0xFFFFFFFF) << 0)
#define IOMUX_RESERVED4_MASK                    (0xFFFFFFFF << 0)
#define IOMUX_RESERVED4_SHIFT                   (0)

// reg_2c
#define IOMUX_R_GPIO_P0_PU(n)                   (((n) & 0xFF) << 0)
#define IOMUX_R_GPIO_P0_PU_MASK                 (0xFF << 0)
#define IOMUX_R_GPIO_P0_PU_SHIFT                (0)

// reg_30
#define IOMUX_R_GPIO_P0_PD(n)                   (((n) & 0xFF) << 0)
#define IOMUX_R_GPIO_P0_PD_MASK                 (0xFF << 0)
#define IOMUX_R_GPIO_P0_PD_SHIFT                (0)

// reg_34
#define IOMUX_AGPIO_EN(n)                       (((n) & 0xFF) << 0)
#define IOMUX_AGPIO_EN_MASK                     (0xFF << 0)
#define IOMUX_AGPIO_EN_SHIFT                    (0)

// reg_38
#define IOMUX_MCUIO_DB_ACTIVE(n)                (((n) & 0x1FF) << 0)
#define IOMUX_MCUIO_DB_ACTIVE_MASK              (0x1FF << 0)
#define IOMUX_MCUIO_DB_ACTIVE_SHIFT             (0)

// reg_3c
#define IOMUX_MCUIO_DB_VALUE(n)                 (((n) & 0xFF) << 0)
#define IOMUX_MCUIO_DB_VALUE_MASK               (0xFF << 0)
#define IOMUX_MCUIO_DB_VALUE_SHIFT              (0)
#define IOMUX_PWK_DB_VALUE(n)                   (((n) & 0xFF) << 8)
#define IOMUX_PWK_DB_VALUE_MASK                 (0xFF << 8)
#define IOMUX_PWK_DB_VALUE_SHIFT                (8)
#define IOMUX_PWK_DB_ACTIVE                     (1 << 16)

// reg_40
#define IOMUX_POWER_KEY_ON_INT_STATUS           (1 << 0)
#define IOMUX_POWER_KEY_OFF_INT_STATUS          (1 << 1)
#define IOMUX_R_POWER_KEY_INTR_U                (1 << 2)
#define IOMUX_R_POWER_KEY_INTR_D                (1 << 3)
#define IOMUX_POWER_KEY_ON_INT_EN               (1 << 4)
#define IOMUX_POWER_KEY_OFF_INT_EN              (1 << 5)
#define IOMUX_POWER_KEY_ON_INT_MSK              (1 << 6)
#define IOMUX_POWER_KEY_OFF_INT_MSK             (1 << 7)
#define IOMUX_POWER_KEY_DB                      (1 << 30)
#define IOMUX_POWER_ON_FEEDOUT                  (1 << 31)

#define IOMUX_PWR_KEY_DOWN_INT_CLR              (1 << 0)
#define IOMUX_PWR_KEY_UP_INT_CLR                (1 << 1)
#define IOMUX_PWR_KEY_DOWN_INT_ST               (1 << 2)
#define IOMUX_PWR_KEY_UP_INT_ST                 (1 << 3)
#define IOMUX_PWR_KEY_DOWN_INT_EN               (1 << 4)
#define IOMUX_PWR_KEY_UP_INT_EN                 (1 << 5)
#define IOMUX_PWR_KEY_DOWN_INT_MASK             (1 << 6)
#define IOMUX_PWR_KEY_UP_INT_MASK               (1 << 7)
#define IOMUX_PWR_KEY_DBOUNCE_ST                (1 << 30)
#define IOMUX_PWR_KEY_VAL                       (1 << 31)

// reg_50
#define IOMUX_GPIO_I2C_MODE                     (1 << 0)
#define IOMUX_I2C0_M_SEL_GPIO                   (1 << 1)
#define IOMUX_SPI_WM3                           (1 << 2)
#define IOMUX_CFG_EN_CLK_REQIN                  (1 << 3)
#define IOMUX_CFG_POL_CLK_REQIN                 (1 << 4)
#define IOMUX_CFG_EN_CLK_REQOUT                 (1 << 5)
#define IOMUX_CFG_POL_CLK_REQOUT                (1 << 6)
#define IOMUX_PD_CTRL                           (1 << 7)
#define IOMUX_SPIFLASH_PU                       (1 << 8)

// reg_54
#define IOMUX_CFG_GPIO_OENB_P0_POL(n)           (((n) & 0xFF) << 0)
#define IOMUX_CFG_GPIO_OENB_P0_POL_MASK         (0xFF << 0)
#define IOMUX_CFG_GPIO_OENB_P0_POL_SHIFT        (0)

// reg_64
#define IOMUX_CFG_CODEC_TRIG_SEL(n)             (((n) & 0x7) << 0)
#define IOMUX_CFG_CODEC_TRIG_SEL_MASK           (0x7 << 0)
#define IOMUX_CFG_CODEC_TRIG_SEL_SHIFT          (0)
#define IOMUX_CFG_CODEC_TRIG_POL                (1 << 3)

// reg_74
#define IOMUX_GPIO_P0_DRV0_SEL(n)               (((n) & 0x3) << 0)
#define IOMUX_GPIO_P0_DRV0_SEL_MASK             (0x3 << 0)
#define IOMUX_GPIO_P0_DRV0_SEL_SHIFT            (0)
#define IOMUX_GPIO_P0_DRV1_SEL(n)               (((n) & 0x3) << 2)
#define IOMUX_GPIO_P0_DRV1_SEL_MASK             (0x3 << 2)
#define IOMUX_GPIO_P0_DRV1_SEL_SHIFT            (2)
#define IOMUX_GPIO_P0_DRV2_SEL(n)               (((n) & 0x3) << 4)
#define IOMUX_GPIO_P0_DRV2_SEL_MASK             (0x3 << 4)
#define IOMUX_GPIO_P0_DRV2_SEL_SHIFT            (4)
#define IOMUX_GPIO_P0_DRV3_SEL(n)               (((n) & 0x3) << 6)
#define IOMUX_GPIO_P0_DRV3_SEL_MASK             (0x3 << 6)
#define IOMUX_GPIO_P0_DRV3_SEL_SHIFT            (6)
#define IOMUX_GPIO_P0_DRV4_SEL(n)               (((n) & 0x3) << 8)
#define IOMUX_GPIO_P0_DRV4_SEL_MASK             (0x3 << 8)
#define IOMUX_GPIO_P0_DRV4_SEL_SHIFT            (8)
#define IOMUX_GPIO_P0_DRV5_SEL(n)               (((n) & 0x3) << 10)
#define IOMUX_GPIO_P0_DRV5_SEL_MASK             (0x3 << 10)
#define IOMUX_GPIO_P0_DRV5_SEL_SHIFT            (10)
#define IOMUX_GPIO_P0_DRV6_SEL(n)               (((n) & 0x3) << 12)
#define IOMUX_GPIO_P0_DRV6_SEL_MASK             (0x3 << 12)
#define IOMUX_GPIO_P0_DRV6_SEL_SHIFT            (12)
#define IOMUX_GPIO_P0_DRV7_SEL(n)               (((n) & 0x3) << 14)
#define IOMUX_GPIO_P0_DRV7_SEL_MASK             (0x3 << 14)
#define IOMUX_GPIO_P0_DRV7_SEL_SHIFT            (14)

#endif /* __REG_IOMUXIP_H_ */
