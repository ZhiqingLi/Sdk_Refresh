#ifndef __REG_IOMUX_BEST2000_H_
#define __REG_IOMUX_BEST2000_H_

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
};

// REG_004
#define IOMUX_FUNC_SEL_P0_0_SHIFT               (0)
#define IOMUX_FUNC_SEL_P0_0_MASK                (0xF << 0)
#define IOMUX_FUNC_SEL_P0_0(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_0, n)
#define IOMUX_FUNC_SEL_P0_1_SHIFT               (4)
#define IOMUX_FUNC_SEL_P0_1_MASK                (0xF << 4)
#define IOMUX_FUNC_SEL_P0_1(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_1, n)
#define IOMUX_FUNC_SEL_P0_2_SHIFT               (8)
#define IOMUX_FUNC_SEL_P0_2_MASK                (0xF << 8)
#define IOMUX_FUNC_SEL_P0_2(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_2, n)
#define IOMUX_FUNC_SEL_P0_3_SHIFT               (12)
#define IOMUX_FUNC_SEL_P0_3_MASK                (0xF << 12)
#define IOMUX_FUNC_SEL_P0_3(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_3, n)
#define IOMUX_FUNC_SEL_P0_4_SHIFT               (16)
#define IOMUX_FUNC_SEL_P0_4_MASK                (0xF << 16)
#define IOMUX_FUNC_SEL_P0_4(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_4, n)
#define IOMUX_FUNC_SEL_P0_5_SHIFT               (0)
#define IOMUX_FUNC_SEL_P0_5_MASK                (0xF << 20)
#define IOMUX_FUNC_SEL_P0_5(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_5, n)
#define IOMUX_FUNC_SEL_P0_6_SHIFT               (4)
#define IOMUX_FUNC_SEL_P0_6_MASK                (0xF << 24)
#define IOMUX_FUNC_SEL_P0_6(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_6, n)
#define IOMUX_FUNC_SEL_P0_7_SHIFT               (8)
#define IOMUX_FUNC_SEL_P0_7_MASK                (0xF << 28)
#define IOMUX_FUNC_SEL_P0_7(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P0_7, n)

// REG_00C
#define IOMUX_FUNC_SEL_P2_6_SHIFT               (24)
#define IOMUX_FUNC_SEL_P2_6_MASK                (0xF << 24)
#define IOMUX_FUNC_SEL_P2_6(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P2_6, n)
#define IOMUX_FUNC_SEL_P2_7_SHIFT               (28)
#define IOMUX_FUNC_SEL_P2_7_MASK                (0xF << 28)
#define IOMUX_FUNC_SEL_P2_7(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P2_7, n)

// REG_010
#define IOMUX_FUNC_SEL_P3_0_SHIFT               (0)
#define IOMUX_FUNC_SEL_P3_0_MASK                (0xF << 0)
#define IOMUX_FUNC_SEL_P3_0(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P3_0, n)
#define IOMUX_FUNC_SEL_P3_1_SHIFT               (4)
#define IOMUX_FUNC_SEL_P3_1_MASK                (0xF << 4)
#define IOMUX_FUNC_SEL_P3_1(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P3_1, n)

// REG_01C
#define IOMUX_ANA_TEST_SEL_SHIFT                (0)
#define IOMUX_ANA_TEST_SEL_MASK                 (0xF << 0)
#define IOMUX_ANA_TEST_SEL(n)                   BITFIELD_VAL(IOMUX_ANA_TEST_SEL, n)

// REG_020
#define IOMUX_PAD_VIO_SEL_P0_0                  (1 << 0)

// REG_024
#define IOMUX_PAD_EN_P0_0                       (1 << 0)

// REG_028
#define IOMUX_PAD_DRIVE_P0_0_1_SHIFT            (0)
#define IOMUX_PAD_DRIVE_P0_0_1_MASK             (3 << 0)
#define IOMUX_PAD_DRIVE_P0_0_1(n)               BITFIELD_VAL(IOMUX_PAD_DRIVE_P0_0_1, n)

// REG_02C
#define IOMUX_PAD_PULLUP_P0_0                   (1 << 0)

// REG_030
#define IOMUX_PAD_PULLDOWN_P0_0                 (1 << 0)

// REG_034
#define IOMUX_ANALOG_GPIO_P0_0                  (1 << 0)

// REG_038
#define IOMUX_GPIO_DEBOUNCE_EN_P0_0             (1 << 0)

// REG_03C
#define IOMUX_GPIO_DEBOUNCE_VAL_P0_SHIFT        (0)
#define IOMUX_GPIO_DEBOUNCE_VAL_P0_MASK         (0xF << 0)
#define IOMUX_GPIO_DEBOUNCE_VAL_P0(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_VAL_P0, n)
#define IOMUX_GPIO_DEBOUNCE_VAL_P1_SHIFT        (4)
#define IOMUX_GPIO_DEBOUNCE_VAL_P1_MASK         (0xF << 4)
#define IOMUX_GPIO_DEBOUNCE_VAL_P1(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_VAL_P1, n)
#define IOMUX_GPIO_DEBOUNCE_VAL_P2_SHIFT        (8)
#define IOMUX_GPIO_DEBOUNCE_VAL_P2_MASK         (0xF << 8)
#define IOMUX_GPIO_DEBOUNCE_VAL_P2(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_VAL_P2, n)
#define IOMUX_GPIO_DEBOUNCE_VAL_P3_SHIFT        (12)
#define IOMUX_GPIO_DEBOUNCE_VAL_P3_MASK         (0xF << 12)
#define IOMUX_GPIO_DEBOUNCE_VAL_P3(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_VAL_P3, n)
#define IOMUX_GPIO_DEBOUNCE_VAL_P4_SHIFT        (16)
#define IOMUX_GPIO_DEBOUNCE_VAL_P4_MASK         (0xF << 16)
#define IOMUX_GPIO_DEBOUNCE_VAL_P4(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_VAL_P4, n)
#define IOMUX_GPIO_DEBOUNCE_VAL_DOUBLE          (1 << 24)
#define IOMUX_PWR_KEY_DEBOUNCE_EN               (1 << 25)

// REG_040
#define IOMUX_PWR_KEY_DOWN_INT_CLR              (1 << 0)
#define IOMUX_PWR_KEY_UP_INT_CLR                (1 << 1)
#define IOMUX_PWR_KEY_DOWN_INT_ST               (1 << 2)
#define IOMUX_PWR_KEY_UP_INT_ST                 (1 << 3)
#define IOMUX_PWR_KEY_DOWN_INT_EN               (1 << 4)
#define IOMUX_PWR_KEY_UP_INT_EN                 (1 << 5)
#define IOMUX_PWR_KEY_DOWN_INT_MASK             (1 << 6)
#define IOMUX_PWR_KEY_UP_INT_MASK               (1 << 7)

#define IOMUX_GPIO_DEBOUNCE_EN_P4_0             (1 << 8)

#define IOMUX_PWR_KEY_DBOUNCE_ST                (1 << 30)
#define IOMUX_PWR_KEY_VAL                       (1 << 31)

// REG_044
#define IOMUX_SPI_BT_RF_EN                      (1 << 0)
#define IOMUX_SPI_BT_PMU_EN                     (1 << 1)
#define IOMUX_SPI_BT_ANA_EN                     (1 << 2)
#define IOMUX_SPI_MCU_RF_EN                     (1 << 3)
#define IOMUX_SPI_MCU_PMU_EN                    (1 << 4)
#define IOMUX_SPI_MCU_ANA_EN                    (1 << 5)
#define IOMUX_SPI_WF_RF_EN                      (1 << 6)
#define IOMUX_SPI_WF_PMU_EN                     (1 << 7)
#define IOMUX_SPI_WF_ANA_EN                     (1 << 8)

// REG_048
#define IOMUX_FUNC_SEL_P4_0_SHIFT               (0)
#define IOMUX_FUNC_SEL_P4_0_MASK                (0xF << 0)
#define IOMUX_FUNC_SEL_P4_0(n)                  BITFIELD_VAL(IOMUX_FUNC_SEL_P4_0, n)

// REG_04C
#define IOMUX_PAD_PULLUP_P4_0                   (1 << 0)
#define IOMUX_PAD_PULLDOWN_P4_0                 (1 << 8)
#define IOMUX_PAD_VIO_SEL_P4_SHIFT              (16)
#define IOMUX_PAD_VIO_SEL_P4_MASK               (0xFF << 16)
#define IOMUX_PAD_VIO_SEL_P4(n)                 BITFIELD_VAL(IOMUX_PAD_VIO_SEL_P4, n)
#define IOMUX_PAD_EN_P4_0                       (1 << 24)

// REG_050
#define IOMUX_PAD_DRIVE_P4_0_1_SHIFT            (0)
#define IOMUX_PAD_DRIVE_P4_0_1_MASK             (3 << 0)
#define IOMUX_PAD_DRIVE_P4_0_1(n)               BITFIELD_VAL(IOMUX_PAD_DRIVE_P4_0_1, n)

#define IOMUX_ANALOG_GPIO_P4_0                  (1 << 8)

#define IOMUX_ANALOG_I2C_DIS                    (1 << 16)
#define IOMUX_I2C_M_SEL_DIG_IO                  (1 << 17)
#define IOMUX_GPIO_PCM_MODE                     (1 << 18)
#define IOMUX_BT_RXTX_SW_EN                     (1 << 19)

// REG_054
#define IOMUX_OENB_POL_INV_P0_0                 (1 << 0)

// REG_058
#define IOMUX_OENB_POL_INV_P4_0                 (1 << 0)

// REG_05C
#define IOMUX_AO_GPIO_DEBOUNCE_DIS_P0_0         (1 << 0)

// REG_060
#define IOMUX_AO_GPIO_DEBOUNCE_DIS_P4_0         (1 << 0)

#define IOMUX_GPIO_DEBOUNCE_TARGET_SHIFT        (8)
#define IOMUX_GPIO_DEBOUNCE_TARGET_MASK         (0xFF << 8)
#define IOMUX_GPIO_DEBOUNCE_TARGET(n)           BITFIELD_VAL(IOMUX_GPIO_DEBOUNCE_TARGET, n)

#define IOMUX_WF_WAKE1_EN                       (1 << 16)
#define IOMUX_WF_WAKE1_LOW_ACTIVE               (1 << 17)
#define IOMUX_WF_WAKE2_EN                       (1 << 18)
#define IOMUX_WF_WAKE2_LOW_ACTIVE               (1 << 19)

#define IOMUX_REQ_26M_EN                        (1 << 20)

#define IOMUX_SPI_4WIRE                         (1 << 28)
#define IOMUX_SPILCD_4WIRE                      (1 << 29)
#define IOMUX_I2S_P0_0_3                        (1 << 30)

#endif /* __REG_IOMUXIP_H_ */
