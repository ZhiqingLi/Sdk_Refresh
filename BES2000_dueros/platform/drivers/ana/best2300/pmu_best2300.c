#include "pmu.h"
#include "analog.h"
#include "cmsis.h"
#include "cmsis_nvic.h"
#include "hal_aud.h"
#include "hal_cache.h"
#include "hal_chipid.h"
#include "hal_cmu.h"
#include "hal_location.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "norflash_drv.h"
#include "tgt_hardware.h"
#include "usbphy.h"

#ifdef __PMU_VIO_DYNAMIC_CTRL_MODE__
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_2V
#else
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_6V
#endif
#ifdef DIGMIC_HIGH_VOLT
#define IO_VOLT_ACTIVE_RISE             PMU_IO_3_0V
#else
#define IO_VOLT_ACTIVE_RISE             PMU_IO_2_6V
#endif
#define IO_VOLT_SLEEP                   PMU_IO_2_7V

#define PMU_DCDC_ANA_2_1V               0xF0
#define PMU_DCDC_ANA_2_0V               0xE0
#define PMU_DCDC_ANA_1_9V               0xD0
#define PMU_DCDC_ANA_1_8V               0xC0
#define PMU_DCDC_ANA_1_7V               0xB0
#define PMU_DCDC_ANA_1_6V               0xA0
#define PMU_DCDC_ANA_1_5V               0x90
#define PMU_DCDC_ANA_1_4V               0x80
#define PMU_DCDC_ANA_1_3V               0x70
#define PMU_DCDC_ANA_1_35V              0x77
#define PMU_DCDC_ANA_1_2V               0x60
#define PMU_DCDC_ANA_1_1V               0x50
#define PMU_DCDC_ANA_1_0V               0x40

#define PMU_DCDC_ANA_SLEEP_1_3V         PMU_DCDC_ANA_1_3V
#define PMU_DCDC_ANA_SLEEP_1_2V         PMU_DCDC_ANA_1_2V
#define PMU_DCDC_ANA_SLEEP_1_1V         PMU_DCDC_ANA_1_1V
#define PMU_DCDC_ANA_SLEEP_1_0V         PMU_DCDC_ANA_1_0V

#define PMU_DCDC_DIG_1_05V              0xF0
#define PMU_DCDC_DIG_1_0V               0xE0
#define PMU_DCDC_DIG_0_95V              0xD0
#define PMU_DCDC_DIG_0_9V               0xC0
#define PMU_DCDC_DIG_0_85V              0xB0
#define PMU_DCDC_DIG_0_8V               0xA0
#define PMU_DCDC_DIG_0_75V              0x90
#define PMU_DCDC_DIG_0_7V               0x80
#define PMU_DCDC_DIG_0_65V              0x70
#define PMU_DCDC_DIG_0_6V               0x60
#define PMU_DCDC_DIG_0_55V              0x50
#define PMU_DCDC_DIG_0_5V               0x40
#define PMU_DCDC_DIG_0_45V              0x30
#define PMU_DCDC_DIG_0_4V               0x20

#define PMU_VDIG_1_2V                   0x24
#define PMU_VDIG_1_05V                  0x1E
#define PMU_VDIG_1_0V                   0x1C
#define PMU_VDIG_0_95V                  0x1A
#define PMU_VDIG_0_9V                   0x18
#define PMU_VDIG_0_85V                  0x16
#define PMU_VDIG_0_8V                   0x14
#define PMU_VDIG_0_75V                  0x12
#define PMU_VDIG_0_7V                   0x10
#define PMU_VDIG_0_65V                  0x0E
#define PMU_VDIG_0_6V                   0x0C
#define PMU_VDIG_0_55V                  0x0A
#define PMU_VDIG_0_5V                   0x08
#define PMU_VDIG_0_45V                  0x06
#define PMU_VDIG_0_4V                   0x04
#define PMU_VDIG_MAX                    PMU_VDIG_1_2V

#define PMU_IO_3_3V                     0x15
#define PMU_IO_3_2V                     0x14
#define PMU_IO_3_1V                     0x13
#define PMU_IO_3_0V                     0x12
#define PMU_IO_2_9V                     0x11
#define PMU_IO_2_8V                     0x10
#define PMU_IO_2_7V                     0xF
#define PMU_IO_2_6V                     0xE
#define PMU_IO_2_5V                     0xD
#define PMU_IO_2_4V                     0xC
#define PMU_IO_2_3V                     0xB
#define PMU_IO_2_2V                     0xA
#define PMU_IO_2_1V                     0x9
#define PMU_IO_2_0V                     0x8
#define PMU_IO_1_9V                     0x7
#define PMU_IO_1_8V                     0x6
#define PMU_IO_1_7V                     0x5

#define PMU_VMEM_2_8V                   0x10
#define PMU_VMEM_2_0V                   0x8
#define PMU_VMEM_1_9V                   0x7
#define PMU_VMEM_1_8V                   0x6
#define PMU_VMEM_1_7V                   0x5
#define PMU_VMEM_1_6V                   0x4

#define PMU_CODEC_2_7V                  0xF
#define PMU_CODEC_2_6V                  0xE
#define PMU_CODEC_2_5V                  0xD
#define PMU_CODEC_2_4V                  0xC
#define PMU_CODEC_2_3V                  0xB
#define PMU_CODEC_2_2V                  0xA
#define PMU_CODEC_2_1V                  0x9
#define PMU_CODEC_2_0V                  0x8
#define PMU_CODEC_1_9V                  0x7
#define PMU_CODEC_1_8V                  0x6
#define PMU_CODEC_1_7V                  0x5
#define PMU_CODEC_1_6V                  0x4
#define PMU_CODEC_1_5V                  0x3

#define PMU_HPPA2CODEC_2_3V             0x1C
#define PMU_HPPA2CODEC_2_2V             0x18
#define PMU_HPPA2CODEC_2_1V             0x14
#define PMU_HPPA2CODEC_2_0V             0x10
#define PMU_HPPA2CODEC_1_95V            0xE
#define PMU_HPPA2CODEC_1_9V             0xC
#define PMU_HPPA2CODEC_1_8V             0x8
#define PMU_HPPA2CODEC_1_7V             0x4
#define PMU_HPPA2CODEC_1_6V             0x0

#define PMU_USB_3_4V                    0xA
#define PMU_USB_3_3V                    0x9
#define PMU_USB_3_2V                    0x8
#define PMU_USB_3_1V                    0x7
#define PMU_USB_3_0V                    0x6
#define PMU_USB_2_9V                    0x5
#define PMU_USB_2_8V                    0x4
#define PMU_USB_2_7V                    0x3
#define PMU_USB_2_6V                    0x2

#define PMU_DCDC_HPPA_2_1V              0xF0
#define PMU_DCDC_HPPA_2_0V              0xE0
#define PMU_DCDC_HPPA_1_95V             0xD8
#define PMU_DCDC_HPPA_1_9V              0xD0
#define PMU_DCDC_HPPA_1_8V              0xC0
#define PMU_DCDC_HPPA_1_7V              0xB0
#define PMU_DCDC_HPPA_1_6V              0xA0
#define PMU_DCDC_HPPA_1_5V              0x90
#define PMU_DCDC_HPPA_1_4V              0x80
#define PMU_DCDC_HPPA_1_3V              0x70
#define PMU_DCDC_HPPA_1_2V              0x60

#define PMU_VMIC_3_3V                   0xF
#define PMU_VMIC_2_8V                   0xA
#define PMU_VMIC_2_2V                   0x4
#define PMU_VMIC_2_1V                   0x3
#define PMU_VMIC_2_0V                   0x2
#define PMU_VMIC_1_9V                   0x1
#define PMU_VMIC_1_8V                   0x0

#define PMU_VMIC_BIAS_2_2V              0x9

// 00   PMU_REG_METAL_ID
#define PMU_METAL_ID_SHIFT              0
#define PMU_METAL_ID_MASK               (0xF << PMU_METAL_ID_SHIFT)
#define PMU_METAL_ID(n)                 (((n) & 0xF) << PMU_METAL_ID_SHIFT)

// 02   PMU_REG_POWER_KEY_CFG
#define REG_PU_VBAT_DIV                     (1 << 15)
#define PU_LPO_DR                           (1 << 14)
#define PU_LPO_REG                          (1 << 13)
#define POWERKEY_WAKEUP_OSC_EN              (1 << 12)
#define RTC_POWER_ON_EN                     (1 << 11)
#define PU_ALL_REG                          (1 << 10)
#define CLK_32K_SEL_SHIFT                   8
#define CLK_32K_SEL_MASK                    (0x3 << CLK_32K_SEL_SHIFT)
#define CLK_32K_SEL(n)                      BITFIELD_VAL(CLK_32K_SEL, n)
#define DEEPSLEEP_MODE_DIGI_DR              (1 << 7)
#define DEEPSLEEP_MODE_DIGI_REG             (1 << 6)
#define REG_VCORE_SSTIME_MODE_SHIFT         4
#define REG_VCORE_SSTIME_MODE_MASK          (0x3 << REG_VCORE_SSTIME_MODE_SHIFT)
#define REG_VCORE_SSTIME_MODE(n)            BITFIELD_VAL(REG_VCORE_SSTIME_MODE, n)
#define REG_LDO_SS_BIAS_EN_DR               (1 << 3)
#define REG_LDO_SS_BIAS_EN                  (1 << 2)
#define REG_LP_EN_VHPPA_DSLEEP              (1 << 1)
#define REG_LP_EN_VHPPA_NORMAL              (1 << 0)

// 03   PMU_REG_BIAS_CFG
#define REG_VSENSE_SEL_VMEM                 (1 << 15)
#define PU_LP_BIAS_LDO_DSLEEP               (1 << 14)
#define PU_LP_BIAS_LDO_DR                   (1 << 13)
#define PU_LP_BIAS_LDO_REG                  (1 << 12)
#define PU_BIAS_LDO_DR                      (1 << 11)
#define PU_BIAS_LDO_REG                     (1 << 10)
#define BG_VBG_SEL_DR                       (1 << 9)
#define BG_VBG_SEL_REG                      (1 << 8)
#define BG_CONSTANT_GM_BIAS_DR              (1 << 7)
#define BG_CONSTANT_GM_BIAS_REG             (1 << 6)
#define BG_CORE_EN_DR                       (1 << 5)
#define BG_CORE_EN_REG                      (1 << 4)
#define REG_PU_LDO_VANA_DR                  (1 << 3)
#define REG_PU_LDO_VANA                     (1 << 2)
#define BG_NOTCH_EN_DR                      (1 << 1)
#define BG_NOTCH_EN_REG                     (1 << 0)

#define REG_PU_LDO_VANA_REG                 REG_PU_LDO_VANA

// 05   PMU_REG_CHARGER_CFG
#define REG_PU_LDO_VRTC_RF_DSLEEP           (1 << 15)
#define REG_PU_LDO_VRTC_RF_DR               (1 << 14)
#define REG_PU_LDO_VRTC_RF_REG              (1 << 13)
#define REG_CHARGE_OUT_INTR_MSK             (1 << 12)
#define REG_CHARGE_IN_INTR_MSK              (1 << 11)
#define REG_AC_ON_OUT_EN                    (1 << 10)
#define REG_AC_ON_IN_EN                     (1 << 9)
#define REG_CHARGE_INTR_EN                  (1 << 8)
#define REG_AC_ON_DB_VALUE_SHIFT            0
#define REG_AC_ON_DB_VALUE_MASK             (0xFF << REG_AC_ON_DB_VALUE_SHIFT)
#define REG_AC_ON_DB_VALUE(n)               BITFIELD_VAL(REG_AC_ON_DB_VALUE, n)

// 07   PMU_REG_ANA_CFG
#define REG_LIGHT_LOAD_VANA_BUCK_LDO        (1 << 15)
#define REG_PULLDOWN_VANA_LDO               (1 << 14)
#define LP_EN_VANA_LDO_DSLEEP               (1 << 13)
#define LP_EN_VANA_LDO_DR                   (1 << 12)
#define LP_EN_VANA_LDO_REG                  (1 << 11)
#define REG_PU_LDO_VANA_DSLEEP              (1 << 10)
#define LDO_ANA_VBIT_DSLEEP_SHIFT           5
#define LDO_ANA_VBIT_DSLEEP_MASK            (0x1F << LDO_ANA_VBIT_DSLEEP_SHIFT)
#define LDO_ANA_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_ANA_VBIT_DSLEEP, n)
#define LDO_ANA_VBIT_NORMAL_SHIFT           0
#define LDO_ANA_VBIT_NORMAL_MASK            (0x1F << LDO_ANA_VBIT_NORMAL_SHIFT)
#define LDO_ANA_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_ANA_VBIT_NORMAL, n)

#define LDO_VANA_VBIT_DSLEEP_SHIFT          LDO_ANA_VBIT_DSLEEP_SHIFT
#define LDO_VANA_VBIT_DSLEEP_MASK           LDO_ANA_VBIT_DSLEEP_MASK
#define LDO_VANA_VBIT_NORMAL_SHIFT          LDO_ANA_VBIT_NORMAL_SHIFT
#define LDO_VANA_VBIT_NORMAL_MASK           LDO_ANA_VBIT_NORMAL_MASK

// 08   PMU_REG_DIG_CFG
#define REG_DCDC_CLK_SE_EN                  (1 << 15)
#define LP_EN_VCORE_LDO_DSLEEP              (1 << 14)
#define LP_EN_VCORE_LDO_DR                  (1 << 13)
#define LP_EN_VCORE_LDO_REG                 (1 << 12)
#define LDO_DIG_VBIT_DSLEEP_SHIFT           6
#define LDO_DIG_VBIT_DSLEEP_MASK            (0x3F << LDO_DIG_VBIT_DSLEEP_SHIFT)
#define LDO_DIG_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_DIG_VBIT_DSLEEP, n)
#define LDO_DIG_VBIT_NORMAL_SHIFT           0
#define LDO_DIG_VBIT_NORMAL_MASK            (0x3F << LDO_DIG_VBIT_NORMAL_SHIFT)
#define LDO_DIG_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_DIG_VBIT_NORMAL, n)

#define LDO_VCORE_VBIT_DSLEEP_SHIFT         LDO_DIG_VBIT_DSLEEP_SHIFT
#define LDO_VCORE_VBIT_DSLEEP_MASK          LDO_DIG_VBIT_DSLEEP_MASK
#define LDO_VCORE_VBIT_NORMAL_SHIFT         LDO_DIG_VBIT_NORMAL_SHIFT
#define LDO_VCORE_VBIT_NORMAL_MASK          LDO_DIG_VBIT_NORMAL_MASK

// Real bit is in REG_101 -- but we never change the default value
#define REG_PU_LDO_VCORE_DSLEEP             0

// 09   PMU_REG_IO_CFG
#define LP_EN_VIO_LDO_DSLEEP                (1 << 15)
#define LP_EN_VIO_LDO_DR                    (1 << 14)
#define LP_EN_VIO_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VIO_DSLEEP               (1 << 12)
#define REG_PU_LDO_VIO_DR                   (1 << 11)
#define REG_PU_LDO_VIO                      (1 << 10)
#define LDO_VIO_VBIT_NORMAL_SHIFT           5
#define LDO_VIO_VBIT_NORMAL_MASK            (0x1F << LDO_VIO_VBIT_NORMAL_SHIFT)
#define LDO_VIO_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_VIO_VBIT_NORMAL, n)
#define LDO_VIO_VBIT_DSLEEP_SHIFT           0
#define LDO_VIO_VBIT_DSLEEP_MASK            (0x1F << LDO_VIO_VBIT_DSLEEP_SHIFT)
#define LDO_VIO_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_VIO_VBIT_DSLEEP, n)

#define REG_PU_LDO_VIO_REG                  REG_PU_LDO_VIO

// 0A   PMU_REG_MEM_CFG
#define LP_EN_VMEM_LDO_DSLEEP               (1 << 15)
#define LP_EN_VMEM_LDO_DR                   (1 << 14)
#define LP_EN_VMEM_LDO_REG                  (1 << 13)
#define REG_PU_LDO_VMEM_DSLEEP              (1 << 12)
#define REG_PU_LDO_VMEM_DR                  (1 << 11)
#define REG_PU_LDO_VMEM_REG                 (1 << 10)
#define LDO_VMEM_VBIT_NORMAL_SHIFT          5
#define LDO_VMEM_VBIT_NORMAL_MASK           (0x1F << LDO_VMEM_VBIT_NORMAL_SHIFT)
#define LDO_VMEM_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VMEM_VBIT_NORMAL, n)
#define LDO_VMEM_VBIT_DSLEEP_SHIFT          0
#define LDO_VMEM_VBIT_DSLEEP_MASK           (0x1F << LDO_VMEM_VBIT_DSLEEP_SHIFT)
#define LDO_VMEM_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VMEM_VBIT_DSLEEP, n)

// 0B   PMU_REG_GP_CFG
#define LP_EN_VGP_LDO_DSLEEP                (1 << 15)
#define LP_EN_VGP_LDO_DR                    (1 << 14)
#define LP_EN_VGP_LDO_REG                   (1 << 13)
#define REG_PU_LDO_VGP_DSLEEP               (1 << 12)
#define REG_PU_LDO_VGP_DR                   (1 << 11)
#define REG_PU_LDO_VGP_REG                  (1 << 10)
#define LDO_VGP_VBIT_NORMAL_SHIFT           5
#define LDO_VGP_VBIT_NORMAL_MASK            (0x1F << LDO_VGP_VBIT_NORMAL_SHIFT)
#define LDO_VGP_VBIT_NORMAL(n)              BITFIELD_VAL(LDO_VGP_VBIT_NORMAL, n)
#define LDO_VGP_VBIT_DSLEEP_SHIFT           0
#define LDO_VGP_VBIT_DSLEEP_MASK            (0x1F << LDO_VGP_VBIT_DSLEEP_SHIFT)
#define LDO_VGP_VBIT_DSLEEP(n)              BITFIELD_VAL(LDO_VGP_VBIT_DSLEEP, n)

// 0C   PMU_REG_USB_CFG
#define REG_LDO_VANA_LV_MODE                (1 << 15)
#define REG_PULLDOWN_VUSB                   (1 << 14)
#define LP_EN_VUSB_LDO_DSLEEP               (1 << 13)
#define LP_EN_VUSB_LDO_DR                   (1 << 12)
#define LP_EN_VUSB_LDO_REG                  (1 << 11)
#define PU_LDO_VUSB_DSLEEP                  (1 << 10)
#define PU_LDO_VUSB_DR                      (1 << 9)
#define PU_LDO_VUSB_REG                     (1 << 8)
#define LDO_VUSB_VBIT_NORMAL_SHIFT          4
#define LDO_VUSB_VBIT_NORMAL_MASK           (0xF << LDO_VUSB_VBIT_NORMAL_SHIFT)
#define LDO_VUSB_VBIT_NORMAL(n)             BITFIELD_VAL(LDO_VUSB_VBIT_NORMAL, n)
#define LDO_VUSB_VBIT_DSLEEP_SHIFT          0
#define LDO_VUSB_VBIT_DSLEEP_MASK           (0xF << LDO_VUSB_VBIT_DSLEEP_SHIFT)
#define LDO_VUSB_VBIT_DSLEEP(n)             BITFIELD_VAL(LDO_VUSB_VBIT_DSLEEP, n)

#define REG_PU_LDO_VUSB_DSLEEP              PU_LDO_VUSB_DSLEEP
#define REG_PU_LDO_VUSB_DR                  PU_LDO_VUSB_DR
#define REG_PU_LDO_VUSB_REG                 PU_LDO_VUSB_REG

// 0D   PMU_REG_BAT2DIG_CFG
#define LP_EN_VBAT2VCORE_LDO_DR             (1 << 15)
#define LP_EN_VBAT2VCORE_LDO                (1 << 14)
#define REG_PU_VBAT2VCORE_LDO_DR            (1 << 13)
#define REG_PU_VBAT2VCORE_LDO               (1 << 12)
#define LDO_VBAT2VCORE_VBIT_DSLEEP_SHIFT    6
#define LDO_VBAT2VCORE_VBIT_DSLEEP_MASK     (0x3F << LDO_VBAT2VCORE_VBIT_DSLEEP_SHIFT)
#define LDO_VBAT2VCORE_VBIT_DSLEEP(n)       BITFIELD_VAL(LDO_VBAT2VCORE_VBIT_DSLEEP, n)
#define LDO_VBAT2VCORE_VBIT_NORMAL_SHIFT    0
#define LDO_VBAT2VCORE_VBIT_NORMAL_MASK     (0x3F << LDO_VBAT2VCORE_VBIT_NORMAL_SHIFT)
#define LDO_VBAT2VCORE_VBIT_NORMAL(n)       BITFIELD_VAL(LDO_VBAT2VCORE_VBIT_NORMAL, n)

#define LP_EN_VBAT2VCORE_LDO_REG            LP_EN_VBAT2VCORE_LDO
#define REG_PU_LDO_VBAT2VCORE_DR            REG_PU_VBAT2VCORE_LDO_DR
#define REG_PU_LDO_VBAT2VCORE_REG           REG_PU_VBAT2VCORE_LDO
// Real bit is in REG_101 -- but we never change the default value
#define REG_PU_LDO_VBAT2VCORE_DSLEEP        0

// 0E   PMU_REG_HPPA_LDO_EN
#define REG_PULL_DOWN_VHPPA                 (1 << 15)
#define REG_PU_LDO_VHPPA_DSLEEP             (1 << 14)
#define REG_PU_LDO_VHPPA_EN                 (1 << 13)
#define REG_RES_SEL_VHPPA_SHIFT             8
#define REG_RES_SEL_VHPPA_MASK              (0x1F << REG_RES_SEL_VHPPA_SHIFT)
#define REG_RES_SEL_VHPPA(n)                BITFIELD_VAL(REG_RES_SEL_VHPPA, n)
#define REG_LP_BIAS_SEL_LDO_SHIFT           6
#define REG_LP_BIAS_SEL_LDO_MASK            (0x3 << REG_LP_BIAS_SEL_LDO_SHIFT)
#define REG_LP_BIAS_SEL_LDO(n)              BITFIELD_VAL(REG_LP_BIAS_SEL_LDO, n)
#define REG_BYPASS_VBUCK2ANA                (1 << 5)
#define REG_PULLDOWN_VBUCK2ANA              (1 << 4)
#define REG_PULLDOWN_VIO                    (1 << 3)
#define REG_PULLDOWN_VMEM                   (1 << 2)
#define REG_PULLDOWN_VCORE                  (1 << 1)
#define REG_PULLDOWN_VHPPA2VCODEC           (1 << 0)

// 0F   PMU_REG_HPPA2CODEC_CFG
#define PU_LDO_VCODEC_DSLEEP                (1 << 15)
#define REG_BYPASS_VCORE                    (1 << 14)
#define REG_PU_LDO_VHPPA2VCODEC_DR          (1 << 13)
#define REG_PU_LDO_VHPPA2VCODEC             (1 << 12)
#define LP_EN_VHPPA2VCODEC_LDO_DR           (1 << 11)
#define LP_EN_VHPPA2VCODEC_LDO              (1 << 10)
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP_SHIFT  5
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP_MASK   (0x1F << LDO_VHPPA2VCODEC_VBIT_DSLEEP_SHIFT)
#define LDO_VHPPA2VCODEC_VBIT_DSLEEP(n)     BITFIELD_VAL(LDO_VHPPA2VCODEC_VBIT_DSLEEP, n)
#define LDO_VHPPA2VCODEC_VBIT_NORMAL_SHIFT  0
#define LDO_VHPPA2VCODEC_VBIT_NORMAL_MASK   (0x1F << LDO_VHPPA2VCODEC_VBIT_NORMAL_SHIFT)
#define LDO_VHPPA2VCODEC_VBIT_NORMAL(n)     BITFIELD_VAL(LDO_VHPPA2VCODEC_VBIT_NORMAL, n)

#define REG_PU_LDO_VHPPA2VCODEC_REG         REG_PU_LDO_VHPPA2VCODEC
#define LP_EN_VHPPA2VCODEC_LDO_REG          LP_EN_VHPPA2VCODEC_LDO
#define REG_PU_LDO_VCODEC_DSLEEP            PU_LDO_VCODEC_DSLEEP

// 10   PMU_REG_CODEC_CFG
#define PU_LDO_VCODEC_DR                    (1 << 15)
#define PU_LDO_VCODEC_REG                   (1 << 14)
#define LP_EN_VCODEC_LDO_DSLEEP             (1 << 13)
#define LP_EN_VCODEC_LDO_DR                 (1 << 12)
#define LP_EN_VCODEC_LDO_REG                (1 << 11)
#define REG_PULLDOWN_VCODEC                 (1 << 10)
#define LDO_VCODEC_VBIT_NORMAL_SHIFT        5
#define LDO_VCODEC_VBIT_NORMAL_MASK         (0x1F << LDO_VCODEC_VBIT_NORMAL_SHIFT)
#define LDO_VCODEC_VBIT_NORMAL(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_NORMAL, n)
#define LDO_VCODEC_VBIT_DSLEEP_SHIFT        0
#define LDO_VCODEC_VBIT_DSLEEP_MASK         (0x1F << LDO_VCODEC_VBIT_DSLEEP_SHIFT)
#define LDO_VCODEC_VBIT_DSLEEP(n)           BITFIELD_VAL(LDO_VCODEC_VBIT_DSLEEP, n)

#define REG_PU_LDO_VCODEC_DR                PU_LDO_VCODEC_DR
#define REG_PU_LDO_VCODEC_REG               PU_LDO_VCODEC_REG

// 15   PMU_REG_DCDC_DIG_EN
#define REG_UVLO_SEL_SHIFT                  14
#define REG_UVLO_SEL_MASK                   (0x3 << REG_UVLO_SEL_SHIFT)
#define REG_UVLO_SEL(n)                     BITFIELD_VAL(REG_UVLO_SEL, n)
#define REG_PU_LDO_DIG_DR                   (1 << 13)
#define REG_PU_LDO_DIG_REG                  (1 << 12)
#define REG_DCDC1_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC1_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC1_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC1_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC1_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC1_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC1_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC1_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC1_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC1_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC1_VREF_SEL_DSLEEP, n)
#define REG_DCDC1_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC1_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC1_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC1_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC1_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC1_DR                     (1 << 1)
#define REG_PU_DCDC1                        (1 << 0)

#define REG_PU_LDO_VCORE_DR                 REG_PU_LDO_DIG_DR
#define REG_PU_LDO_VCORE_REG                REG_PU_LDO_DIG_REG

// 21   PMU_REG_PWR_SEL
#define REG_PMU_VSEL1_SHIFT                 13
#define REG_PMU_VSEL1_MASK                  (0x7 << REG_PMU_VSEL1_SHIFT)
#define REG_PMU_VSEL1(n)                    BITFIELD_VAL(REG_PMU_VSEL1, n)
#define REG_POWER_SEL_CNT_SHIFT             8
#define REG_POWER_SEL_CNT_MASK              (0x1F << REG_POWER_SEL_CNT_SHIFT)
#define REG_POWER_SEL_CNT(n)                BITFIELD_VAL(REG_POWER_SEL_CNT, n)
#define REG_PWR_SEL_DR                      (1 << 7)
#define REG_PWR_SEL                         (1 << 6)
#define CLK_BG_EN                           (1 << 5)
#define CLK_BG_DIV_VALUE_SHIFT              0
#define CLK_BG_DIV_VALUE_MASK               (0x1F << CLK_BG_DIV_VALUE_SHIFT)
#define CLK_BG_DIV_VALUE(n)                 BITFIELD_VAL(CLK_BG_DIV_VALUE, n)

// 26   PMU_REG_INT_MASK
#define RESET_EN_VCORE_LOW                  (1 << 15)
#define RTC_INT1_MSK                        (1 << 14)
#define RTC_INT0_MSK                        (1 << 13)
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)

// 27   PMU_REG_INT_EN
#define REG_BG_SLEEP_MSK                    (1 << 15)
#define RTC_INT_EN_1                        (1 << 14)
#define RTC_INT_EN_0                        (1 << 13)
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// 38   PMU_REG_MIC_BIAS_E
#define REG_MIC_BIASE_CHANSEL_SHIFT         14
#define REG_MIC_BIASE_CHANSEL_MASK          (0x3 << REG_MIC_BIASE_CHANSEL_SHIFT)
#define REG_MIC_BIASE_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASE_CHANSEL, n)
#define REG_MIC_BIASE_EN                    (1 << 13)
#define REG_MIC_BIASE_ENLPF                 (1 << 12)
#define REG_MIC_BIASE_LPFSEL_SHIFT          10
#define REG_MIC_BIASE_LPFSEL_MASK           (0x3 << REG_MIC_BIASE_LPFSEL_SHIFT)
#define REG_MIC_BIASE_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASE_LPFSEL, n)
#define REG_MIC_BIASE_VSEL_SHIFT            5
#define REG_MIC_BIASE_VSEL_MASK             (0x1F << REG_MIC_BIASE_VSEL_SHIFT)
#define REG_MIC_BIASE_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASE_VSEL, n)
#define REG_MIC_LDOE_RES_SHIFT              1
#define REG_MIC_LDOE_RES_MASK               (0xF << REG_MIC_LDOE_RES_SHIFT)
#define REG_MIC_LDOE_RES(n)                 BITFIELD_VAL(REG_MIC_LDOE_RES, n)
#define REG_MIC_LDOE_LOOPCTRL               (1 << 0)

// 39   PMU_REG_MIC_LDO_EN
#define REG_LED_IO1_AIO_EN                  (1 << 15)
#define REG_LED_IO2_AIO_EN                  (1 << 14)
#define REG_LED_IO1_RX_EN                   (1 << 13)
#define REG_LED_IO2_RX_EN                   (1 << 12)
#define REG_MIC_LDOA_EN                     (1 << 11)
#define REG_MIC_LDOB_EN                     (1 << 10)
#define REG_MIC_LDOC_EN                     (1 << 9)
#define REG_MIC_LDOD_EN                     (1 << 8)
#define REG_MIC_LDOE_EN                     (1 << 7)
#define DCDC1_OFFSET_CURRENT_EN             (1 << 6)
#define DCDC2_OFFSET_CURRENT_EN             (1 << 5)
#define DCDC3_OFFSET_CURRENT_EN             (1 << 4)
#define DCDC4_OFFSET_CURRENT_EN             (1 << 3)

// 3A   PMU_REG_LED_CFG
#define REG_LED_IO1_IBIT_SHIFT              14
#define REG_LED_IO1_IBIT_MASK               (0x3 << REG_LED_IO1_IBIT_SHIFT)
#define REG_LED_IO1_IBIT(n)                 BITFIELD_VAL(REG_LED_IO1_IBIT, n)
#define REG_LED_IO1_OENB                    (1 << 13)
#define REG_LED_IO1_PDEN                    (1 << 12)
#define REG_LED_IO1_PU                      (1 << 11)
#define REG_LED_IO1_PUEN                    (1 << 10)
#define REG_LED_IO1_SEL_SHIFT               8
#define REG_LED_IO1_SEL_MASK                (0x3 << REG_LED_IO1_SEL_SHIFT)
#define REG_LED_IO1_SEL(n)                  BITFIELD_VAL(REG_LED_IO1_SEL, n)
#define REG_LED_IO2_IBIT_SHIFT              6
#define REG_LED_IO2_IBIT_MASK               (0x3 << REG_LED_IO2_IBIT_SHIFT)
#define REG_LED_IO2_IBIT(n)                 BITFIELD_VAL(REG_LED_IO2_IBIT, n)
#define REG_LED_IO2_OENB                    (1 << 5)
#define REG_LED_IO2_PDEN                    (1 << 4)
#define REG_LED_IO2_PU                      (1 << 3)
#define REG_LED_IO2_PUEN                    (1 << 2)
#define REG_LED_IO2_SEL_SHIFT               0
#define REG_LED_IO2_SEL_MASK                (0x3 << REG_LED_IO2_SEL_SHIFT)
#define REG_LED_IO2_SEL(n)                  BITFIELD_VAL(REG_LED_IO2_SEL, n)

// 3B   PMU_REG_MIC_BIAS_A
#define REG_MIC_BIASA_CHANSEL_SHIFT         14
#define REG_MIC_BIASA_CHANSEL_MASK          (0x3 << REG_MIC_BIASA_CHANSEL_SHIFT)
#define REG_MIC_BIASA_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASA_CHANSEL, n)
#define REG_MIC_BIASA_EN                    (1 << 13)
#define REG_MIC_BIASA_ENLPF                 (1 << 12)
#define REG_MIC_BIASA_LPFSEL_SHIFT          10
#define REG_MIC_BIASA_LPFSEL_MASK           (0x3 << REG_MIC_BIASA_LPFSEL_SHIFT)
#define REG_MIC_BIASA_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASA_LPFSEL, n)
#define REG_MIC_BIASA_VSEL_SHIFT            5
#define REG_MIC_BIASA_VSEL_MASK             (0x1F << REG_MIC_BIASA_VSEL_SHIFT)
#define REG_MIC_BIASA_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASA_VSEL, n)
#define REG_MIC_LDOA_RES_SHIFT              1
#define REG_MIC_LDOA_RES_MASK               (0xF << REG_MIC_LDOA_RES_SHIFT)
#define REG_MIC_LDOA_RES(n)                 BITFIELD_VAL(REG_MIC_LDOA_RES, n)
#define REG_MIC_LDOA_LOOPCTRL               (1 << 0)

// 3C   PMU_REG_MIC_BIAS_B
#define REG_MIC_BIASB_CHANSEL_SHIFT         14
#define REG_MIC_BIASB_CHANSEL_MASK          (0x3 << REG_MIC_BIASB_CHANSEL_SHIFT)
#define REG_MIC_BIASB_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASB_CHANSEL, n)
#define REG_MIC_BIASB_EN                    (1 << 13)
#define REG_MIC_BIASB_ENLPF                 (1 << 12)
#define REG_MIC_BIASB_LPFSEL_SHIFT          10
#define REG_MIC_BIASB_LPFSEL_MASK           (0x3 << REG_MIC_BIASB_LPFSEL_SHIFT)
#define REG_MIC_BIASB_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASB_LPFSEL, n)
#define REG_MIC_BIASB_VSEL_SHIFT            5
#define REG_MIC_BIASB_VSEL_MASK             (0x1F << REG_MIC_BIASB_VSEL_SHIFT)
#define REG_MIC_BIASB_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASB_VSEL, n)
#define REG_MIC_LDOB_RES_SHIFT              1
#define REG_MIC_LDOB_RES_MASK               (0xF << REG_MIC_LDOB_RES_SHIFT)
#define REG_MIC_LDOB_RES(n)                 BITFIELD_VAL(REG_MIC_LDOB_RES, n)
#define REG_MIC_LDOB_LOOPCTRL               (1 << 0)

// 3D   PMU_REG_MIC_BIAS_C
#define REG_MIC_BIASC_CHANSEL_SHIFT         14
#define REG_MIC_BIASC_CHANSEL_MASK          (0x3 << REG_MIC_BIASC_CHANSEL_SHIFT)
#define REG_MIC_BIASC_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASC_CHANSEL, n)
#define REG_MIC_BIASC_EN                    (1 << 13)
#define REG_MIC_BIASC_ENLPF                 (1 << 12)
#define REG_MIC_BIASC_LPFSEL_SHIFT          10
#define REG_MIC_BIASC_LPFSEL_MASK           (0x3 << REG_MIC_BIASC_LPFSEL_SHIFT)
#define REG_MIC_BIASC_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASC_LPFSEL, n)
#define REG_MIC_BIASC_VSEL_SHIFT            5
#define REG_MIC_BIASC_VSEL_MASK             (0x1F << REG_MIC_BIASC_VSEL_SHIFT)
#define REG_MIC_BIASC_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASC_VSEL, n)
#define REG_MIC_LDOC_RES_SHIFT              1
#define REG_MIC_LDOC_RES_MASK               (0xF << REG_MIC_LDOC_RES_SHIFT)
#define REG_MIC_LDOC_RES(n)                 BITFIELD_VAL(REG_MIC_LDOC_RES, n)
#define REG_MIC_LDOC_LOOPCTRL               (1 << 0)

// 3E   PMU_REG_MIC_BIAS_D
#define REG_MIC_BIASD_CHANSEL_SHIFT         14
#define REG_MIC_BIASD_CHANSEL_MASK          (0x3 << REG_MIC_BIASD_CHANSEL_SHIFT)
#define REG_MIC_BIASD_CHANSEL(n)            BITFIELD_VAL(REG_MIC_BIASD_CHANSEL, n)
#define REG_MIC_BIASD_EN                    (1 << 13)
#define REG_MIC_BIASD_ENLPF                 (1 << 12)
#define REG_MIC_BIASD_LPFSEL_SHIFT          10
#define REG_MIC_BIASD_LPFSEL_MASK           (0x3 << REG_MIC_BIASD_LPFSEL_SHIFT)
#define REG_MIC_BIASD_LPFSEL(n)             BITFIELD_VAL(REG_MIC_BIASD_LPFSEL, n)
#define REG_MIC_BIASD_VSEL_SHIFT            5
#define REG_MIC_BIASD_VSEL_MASK             (0x1F << REG_MIC_BIASD_VSEL_SHIFT)
#define REG_MIC_BIASD_VSEL(n)               BITFIELD_VAL(REG_MIC_BIASD_VSEL, n)
#define REG_MIC_LDOD_RES_SHIFT              1
#define REG_MIC_LDOD_RES_MASK               (0xF << REG_MIC_LDOD_RES_SHIFT)
#define REG_MIC_LDOD_RES(n)                 BITFIELD_VAL(REG_MIC_LDOD_RES, n)
#define REG_MIC_LDOD_LOOPCTRL               (1 << 0)

// 3F   PMU_REG_EFUSE_CTRL
#define TSMC_EFUSE_A_SHIFT                  0
#define TSMC_EFUSE_A_MASK                   (0x3F << TSMC_EFUSE_A_SHIFT)
#define TSMC_EFUSE_A(n)                     BITFIELD_VAL(TSMC_EFUSE_A, n)
#define TSMC_EFUSE_CLK_EN                   (1 << 8)
#define TSMC_EFUSE_NR                       (1 << 9)
#define TSMC_EFUSE_MR                       (1 << 10)
#define MANUAL_MODE                         (1 << 11)
#define TSMC_EFUSE_PGENB                    (1 << 12)
#define REG_MARGIN_READ                     (1 << 13)

// 40   PMU_REG_EFUSE_SEL
#define TSMC_EFUSE_STROBE_TRIG              (1 << 0)
#define TSMC_EFUSE_STROBE_WIDTH_SHIFT       1
#define TSMC_EFUSE_STROBE_WIDTH_MASK        (0xFF << TSMC_EFUSE_STROBE_WIDTH_SHIFT)
#define TSMC_EFUSE_STROBE_WIDTH(n)          BITFIELD_VAL(TSMC_EFUSE_STROBE_WIDTH, n)
#define REG_READ_STROBE_WIDTH_SHIFT         9
#define REG_READ_STROBE_WIDTH_MASK          (0xF << REG_READ_STROBE_WIDTH_SHIFT)
#define REG_READ_STROBE_WIDTH(n)            BITFIELD_VAL(REG_READ_STROBE_WIDTH, n)
#define EFUSE_SEL_SHIFT                     13
#define EFUSE_SEL_MASK                      (0x7 << EFUSE_SEL_SHIFT)
#define EFUSE_SEL(n)                        BITFIELD_VAL(EFUSE_SEL, n)

// 43   PMU_REG_BUCK2ANA_CFG
#define REG_PU_VBUCK2ANA_LDO_DR             (1 << 15)
#define REG_PU_VBUCK2ANA_LDO                (1 << 14)
#define LP_EN_VBUCK2ANA_LDO_DR              (1 << 13)
#define LP_EN_VBUCK2ANA_LDO                 (1 << 12)
#define LDO_VBUCK2ANA_VBIT_DSLEEP_SHIFT     6
#define LDO_VBUCK2ANA_VBIT_DSLEEP_MASK      (0x3F << LDO_VBUCK2ANA_VBIT_DSLEEP_SHIFT)
#define LDO_VBUCK2ANA_VBIT_DSLEEP(n)        BITFIELD_VAL(LDO_VBUCK2ANA_VBIT_DSLEEP, n)
#define LDO_VBUCK2ANA_VBIT_NORMAL_SHIFT     0
#define LDO_VBUCK2ANA_VBIT_NORMAL_MASK      (0x3F << LDO_VBUCK2ANA_VBIT_NORMAL_SHIFT)
#define LDO_VBUCK2ANA_VBIT_NORMAL(n)        BITFIELD_VAL(LDO_VBUCK2ANA_VBIT_NORMAL, n)

#define REG_PU_LDO_VBUCK2ANA_DR             REG_PU_VBUCK2ANA_LDO_DR
#define REG_PU_LDO_VBUCK2ANA_REG            REG_PU_VBUCK2ANA_LDO
#define LP_EN_VBUCK2ANA_LDO_REG             LP_EN_VBUCK2ANA_LDO
// No pu dsleep cfg
#define REG_PU_LDO_VBUCK2ANA_DSLEEP         0

// 44   PMU_REG_SLEEP_CFG
#define PMIC_TMODE_1300_SHIFT               0
#define PMIC_TMODE_1300_MASK                (0x7 << PMIC_TMODE_1300_SHIFT)
#define PMIC_TMODE_1300(n)                  BITFIELD_VAL(PMIC_TMODE_1300, n)
#define POWERON_PRESS_EN                    (1 << 3)
#define POWERON_RELEASE_EN                  (1 << 4)
#define SLEEP_ALLOW                         (1 << 5)
#define REG_VHPPA2VCODEC_SLP_ON             (1 << 6)

#define REG_PU_LDO_VHPPA2VCODEC_DSLEEP      REG_VHPPA2VCODEC_SLP_ON

// 46   PMU_REG_DCDC_DIG_VOLT
#define REG_DCDC1_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC1_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC1_VBIT_DSLEEP_SHIFT)
#define REG_DCDC1_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC1_VBIT_DSLEEP, n)
#define REG_DCDC1_VBIT_NORMAL_SHIFT         0
#define REG_DCDC1_VBIT_NORMAL_MASK          (0xFF << REG_DCDC1_VBIT_NORMAL_SHIFT)
#define REG_DCDC1_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC1_VBIT_NORMAL, n)

// 47   PMU_REG_DCDC_ANA_VOLT
#define REG_DCDC2_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC2_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC2_VBIT_DSLEEP_SHIFT)
#define REG_DCDC2_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC2_VBIT_DSLEEP, n)
#define REG_DCDC2_VBIT_NORMAL_SHIFT         0
#define REG_DCDC2_VBIT_NORMAL_MASK          (0xFF << REG_DCDC2_VBIT_NORMAL_SHIFT)
#define REG_DCDC2_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC2_VBIT_NORMAL, n)

// 48   PMU_REG_DCDC_HPPA_VOLT
#define REG_DCDC3_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC3_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC3_VBIT_DSLEEP_SHIFT)
#define REG_DCDC3_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC3_VBIT_DSLEEP, n)
#define REG_DCDC3_VBIT_NORMAL_SHIFT         0
#define REG_DCDC3_VBIT_NORMAL_MASK          (0xFF << REG_DCDC3_VBIT_NORMAL_SHIFT)
#define REG_DCDC3_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC3_VBIT_NORMAL, n)

// 49   PMU_REG_DCDC4_VOLT
#define REG_DCDC4_VBIT_DSLEEP_SHIFT         8
#define REG_DCDC4_VBIT_DSLEEP_MASK          (0xFF << REG_DCDC4_VBIT_DSLEEP_SHIFT)
#define REG_DCDC4_VBIT_DSLEEP(n)            BITFIELD_VAL(REG_DCDC4_VBIT_DSLEEP, n)
#define REG_DCDC4_VBIT_NORMAL_SHIFT         0
#define REG_DCDC4_VBIT_NORMAL_MASK          (0xFF << REG_DCDC4_VBIT_NORMAL_SHIFT)
#define REG_DCDC4_VBIT_NORMAL(n)            BITFIELD_VAL(REG_DCDC4_VBIT_NORMAL, n)

// 4A   PMU_REG_DCDC_ANA_EN
#define REG_DCDC2_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC2_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC2_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC2_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC2_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC2_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC2_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC2_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC2_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC2_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC2_VREF_SEL_DSLEEP, n)
#define REG_DCDC2_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC2_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC2_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC2_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC2_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC2_DR                     (1 << 1)
#define REG_PU_DCDC2                        (1 << 0)

// 4B   PMU_REG_DCDC_HPPA_EN
#define REG_DCDC3_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC3_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC3_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC3_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC3_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC3_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC3_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC3_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC3_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC3_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC3_VREF_SEL_DSLEEP, n)
#define REG_DCDC3_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC3_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC3_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC3_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC3_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC3_DR                     (1 << 1)
#define REG_PU_DCDC3                        (1 << 0)

// 4C   PMU_REG_DCDC4_EN
#define REG_DCDC4_PFM_SEL_DSLEEP            (1 << 11)
#define REG_DCDC4_PFM_SEL_NORMAL            (1 << 10)
#define REG_DCDC4_ULP_MODE_DSLEEP           (1 << 9)
#define REG_DCDC4_ULP_MODE_NORMAL           (1 << 8)
#define REG_DCDC4_BURST_MODE_SEL_DSLEEP     (1 << 7)
#define REG_DCDC4_BURST_MODE_SEL_NORMAL     (1 << 6)
#define REG_DCDC4_VREF_SEL_DSLEEP_SHIFT     4
#define REG_DCDC4_VREF_SEL_DSLEEP_MASK      (0x3 << REG_DCDC4_VREF_SEL_DSLEEP_SHIFT)
#define REG_DCDC4_VREF_SEL_DSLEEP(n)        BITFIELD_VAL(REG_DCDC4_VREF_SEL_DSLEEP, n)
#define REG_DCDC4_VREF_SEL_NORMAL_SHIFT     2
#define REG_DCDC4_VREF_SEL_NORMAL_MASK      (0x3 << REG_DCDC4_VREF_SEL_NORMAL_SHIFT)
#define REG_DCDC4_VREF_SEL_NORMAL(n)        BITFIELD_VAL(REG_DCDC4_VREF_SEL_NORMAL, n)
#define REG_PU_DCDC4_DR                     (1 << 1)
#define REG_PU_DCDC4                        (1 << 0)

// 4F   PMU_REG_POWER_OFF
#define SOFT_POWER_OFF                      (1 << 0)
#define HARDWARE_POWER_OFF_EN               (1 << 1)
#define AC_ON_EN                            (1 << 2)
#define RC_CAL_READY                        (1 << 3)
#define KEY_START                           (1 << 4)
#define GPADC_START                         (1 << 5)
#define EFUSE_READ_DONE                     (1 << 6)
#define EFUSE_READ_BUSY                     (1 << 7)
#define VBAT_OVP                            (1 << 8)
#define VCORE_LOW                           (1 << 9)
#define REG_LED_IO1_DATA_IN                 (1 << 10)
#define REG_LED_IO2_DATA_IN                 (1 << 11)
#define RD_EFUSE_REG                        (1 << 12)
#define REG_WDT_LOAD                        (1 << 13)

#define EFUSE_READ_TRIG_SHIFT               (12)
#define EFUSE_READ_TRIG_MASK                (0xF << EFUSE_READ_TRIG_SHIFT)
#define EFUSE_READ_TRIG(n)                  BITFIELD_VAL(EFUSE_READ_TRIG, n)
#define EFUSE_READ_TRIG_WORD                (0xA)

// 52   PMU_REG_INT_STATUS
#define RD_RES1                             (1 << 15)
#define RTC_INT_1                           (1 << 14)
#define RTC_INT_0                           (1 << 13)
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)

// 53   PMU_REG_INT_CLR
#define PMIC_CODEC_PWM                      (1 << 15)
#define RTC_INT1_MSKED                      (1 << 14)
#define RTC_INT0_MSKED                      (1 << 13)
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)

// 5E   PMU_REG_CHARGER_STATUS
#define DIG_PU_VHPPA                        (1 << 15)
#define DIG_PU_VRTC_RF                      (1 << 14)
#define DIG_PU_VCODEC                       (1 << 13)
#define DIG_PU_VUSB                         (1 << 12)
#define POWER_ON_RELEASE                    (1 << 11)
#define POWER_ON_PRESS                      (1 << 10)
#define POWER_ON                            (1 << 9)
#define DEEPSLEEP_MODE                      (1 << 8)
//#define PMU_LDO_ON                          (1 << 7)
#define PU_OSC_OUT                          (1 << 6)
#define UVLO_LV                             (1 << 5)
#define AC_ON_DET_OUT_MASKED                (1 << 4)
#define AC_ON_DET_IN_MASKED                 (1 << 3)
#define AC_ON                               (1 << 2)
#define AC_ON_DET_OUT                       (1 << 1)
#define AC_ON_DET_IN                        (1 << 0)

// 101  PMU_REG_DCDC_RAMP_EN
#define REG_MIC_BIASA_IX2                   (1 << 15)
#define REG_MIC_BIASB_IX2                   (1 << 14)
#define REG_MIC_BIASC_IX2                   (1 << 13)
#define REG_MIC_BIASD_IX2                   (1 << 12)
#define REG_MIC_BIASE_IX2                   (1 << 11)
#define REG_PULLDOWN_VGP                    (1 << 10)
#define REG_PU_LDO_DIG_DSLEEP               (1 << 9)
#define LDO_VHPPA2VCODEC_BYPASS             (1 << 8)
#define LDO_VGP_LV_MODE                     (1 << 7)
#define IPTAT_EN                            (1 << 6)
#define REG_PU_AVDD25_ANA                   (1 << 5)
#define REG_DCDC1_RAMP_EN                   (1 << 4)
#define REG_DCDC2_RAMP_EN                   (1 << 3)
#define REG_DCDC3_RAMP_EN                   (1 << 2)
#define REG_PU_VMEM_DELAY_DR                (1 << 1)
#define REG_PU_VMEM_DELAY                   (1 << 0)

// 109  PMU_REG_PWM_EN
#define REG_LED0_OUT                        (1 << 15)
#define REG_LED1_OUT                        (1 << 14)
#define PWM_SELECT_EN_SHIFT                 12
#define PWM_SELECT_EN_MASK                  (0x3 << PWM_SELECT_EN_SHIFT)
#define PWM_SELECT_EN(n)                    BITFIELD_VAL(PWM_SELECT_EN, n)
#define PWM_SELECT_INV_SHIFT                10
#define PWM_SELECT_INV_MASK                 (0x3 << PWM_SELECT_INV_SHIFT)
#define PWM_SELECT_INV(n)                   BITFIELD_VAL(PWM_SELECT_INV, n)
#define REG_CLK_PWM_DIV_SHIFT               5
#define REG_CLK_PWM_DIV_MASK                (0x1F << REG_CLK_PWM_DIV_SHIFT)
#define REG_CLK_PWM_DIV(n)                  BITFIELD_VAL(REG_CLK_PWM_DIV, n)

enum PMU_REG_T {
    PMU_REG_METAL_ID            = 0x00,
    PMU_REG_POWER_KEY_CFG       = 0x02,
    PMU_REG_BIAS_CFG            = 0x03,
    PMU_REG_CHARGER_CFG         = 0x05,
    PMU_REG_ANA_CFG             = 0x07,
    PMU_REG_DIG_CFG             = 0x08,
    PMU_REG_IO_CFG              = 0x09,
    PMU_REG_MEM_CFG             = 0x0A,
    PMU_REG_GP_CFG              = 0x0B,
    PMU_REG_USB_CFG             = 0x0C,
    PMU_REG_BAT2DIG_CFG         = 0x0D,
    PMU_REG_HPPA_LDO_EN         = 0x0E,
    PMU_REG_HPPA2CODEC_CFG      = 0x0F,
    PMU_REG_CODEC_CFG           = 0x10,
    PMU_REG_DCDC_DIG_EN         = 0x15,
    PMU_REG_PWR_SEL             = 0x21,
    PMU_REG_INT_MASK            = 0x26,
    PMU_REG_INT_EN              = 0x27,
    PMU_REG_RTC_LOAD_LOW        = 0x2D,
    PMU_REG_RTC_LOAD_HIGH       = 0x2E,
    PMU_REG_RTC_MATCH0_LOW      = 0x2F,
    PMU_REG_RTC_MATCH0_HIGH     = 0x30,
    PMU_REG_RTC_DIV_1HZ         = 0x37,
    PMU_REG_MIC_BIAS_E          = 0x38,
    PMU_REG_MIC_LDO_EN          = 0x39,
    PMU_REG_LED_CFG             = 0x3A,
    PMU_REG_MIC_BIAS_A          = 0x3B,
    PMU_REG_MIC_BIAS_B          = 0x3C,
    PMU_REG_MIC_BIAS_C          = 0x3D,
    PMU_REG_MIC_BIAS_D          = 0x3E,
    PMU_REG_EFUSE_CTRL          = 0x3F,
    PMU_REG_EFUSE_SEL           = 0x40,
    PMU_REG_RESERVED_ANA        = 0x41,
    PMU_REG_BUCK2ANA_CFG        = 0x43,
    PMU_REG_SLEEP_CFG           = 0x44,
    PMU_REG_DCDC_DIG_VOLT       = 0x46,
    PMU_REG_DCDC_ANA_VOLT       = 0x47,
    PMU_REG_DCDC_HPPA_VOLT      = 0x48,
    PMU_REG_DCDC4_VOLT          = 0x49,
    PMU_REG_DCDC_ANA_EN         = 0x4A,
    PMU_REG_DCDC_HPPA_EN        = 0x4B,
    PMU_REG_POWER_OFF           = 0x4F,
    PMU_REG_INT_STATUS          = 0x52,
    PMU_REG_INT_CLR             = 0x53,
    PMU_REG_RTC_VAL_LOW         = 0x54,
    PMU_REG_RTC_VAL_HIGH        = 0x55,
    PMU_REG_CHARGER_STATUS      = 0x5E,

    PMU_REG_DCDC_RAMP_EN        = 0x101,

    PMU_REG_PWM_EN              = 0x109,

    PMU_REG_MODULE_START        = PMU_REG_ANA_CFG,
    PMU_REG_EFUSE_VAL_START     = 0x148,
};

enum PMU_VCORE_REQ_T {
    PMU_VCORE_FLASH_WRITE_ENABLED   = (1 << 0),
    PMU_VCORE_FLASH_FREQ_HIGH       = (1 << 1),
    PMU_VCORE_PSRAM_FREQ_HIGH       = (1 << 2),
    PMU_VCORE_USB_HS_ENABLED        = (1 << 3),
    PMU_VCORE_IIR_FREQ_HIGH         = (1 << 4),
    PMU_VCORE_SYS_FREQ_HIGH         = (1 << 5),
};

enum PMU_VUSB_REQ_T {
    PMU_VUSB_REQ_INIT               = (1 << 0),
    PMU_VUSB_REQ_USB                = (1 << 1),
    PMU_VUSB_REQ_NFMI               = (1 << 2),
};

enum CRYSTAL_FREQ_T {
    CRYSTAL_FREQ_26M = 0,
    CRYSTAL_FREQ_24M,
    CRYSTAL_FREQ_40M,
    CRYSTAL_FREQ_48M,
};

union BOOT_SETTINGS_T {
    struct {
        unsigned short usb_dld_dis      :1;
        unsigned short uart_dld_en      :1;
        unsigned short uart_trace_en    :1;
        unsigned short pll_dis          :1;
        unsigned short uart_baud_div2   :1;
        unsigned short sec_freq_div2    :1;
        unsigned short crystal_freq     :2;
        unsigned short reserved         :4;
        unsigned short chksum           :4;
    };
    unsigned short reg;
};

enum PMU_MODUAL_T {
    PMU_ANA,
    PMU_DIG,
    PMU_IO,
    PMU_MEM,
    PMU_GP,
    PMU_USB,
    PMU_BAT2DIG,
    PMU_HPPA2CODEC,
    PMU_CODEC,
    PMU_BUCK2ANA,
};

struct PMU_MODULE_CFG_T {
    unsigned short pu_dr;
    unsigned short pu;
    unsigned short lp_en_dr;
    unsigned short lp_en;
    unsigned short pu_dsleep;
    unsigned short vbit_dsleep_mask;
    unsigned short vbit_dsleep_shift;
    unsigned short vbit_normal_mask;
    unsigned short vbit_normal_shift;
};

#define PMU_MOD_CFG_VAL(m)              { \
    REG_PU_LDO_V##m##_DR, REG_PU_LDO_V##m##_REG, \
    LP_EN_V##m##_LDO_DR, LP_EN_V##m##_LDO_REG, \
    REG_PU_LDO_V##m##_DSLEEP, \
    LDO_V##m##_VBIT_DSLEEP_MASK, LDO_V##m##_VBIT_DSLEEP_SHIFT, \
    LDO_V##m##_VBIT_NORMAL_MASK, LDO_V##m##_VBIT_NORMAL_SHIFT }

static const struct PMU_MODULE_CFG_T pmu_module_cfg[] = {
    PMU_MOD_CFG_VAL(ANA),
    PMU_MOD_CFG_VAL(CORE),
    PMU_MOD_CFG_VAL(IO),
    PMU_MOD_CFG_VAL(MEM),
    PMU_MOD_CFG_VAL(GP),
    PMU_MOD_CFG_VAL(USB),
    PMU_MOD_CFG_VAL(BAT2VCORE),
    PMU_MOD_CFG_VAL(HPPA2VCODEC),
    PMU_MOD_CFG_VAL(CODEC),
    PMU_MOD_CFG_VAL(BUCK2ANA),
};

#ifdef ANC_PROD_TEST
#define OPT_TYPE
#else
#define OPT_TYPE                        const
#endif

static OPT_TYPE bool vcodec_off =
#ifdef VCODEC_OFF
    true;
#else
    false;
#endif
static OPT_TYPE uint8_t ana_act_dcdc =
#ifdef VANA_1P2V
    PMU_DCDC_ANA_1_2V;
#elif defined(VANA_1P35V)
    PMU_DCDC_ANA_1_35V;
#elif defined(VANA_1P4V)
    PMU_DCDC_ANA_1_4V;
#elif defined(VANA_1P5V)
    PMU_DCDC_ANA_1_5V;
#elif defined(VANA_1P6V)
    PMU_DCDC_ANA_1_6V;
#else
    PMU_DCDC_ANA_1_3V;
#endif
static OPT_TYPE POSSIBLY_UNUSED uint16_t vcodec_mv =
#ifdef VCODEC_1P6V
    1600;
#elif defined(VCODEC_1P8V)
    1800;
#elif defined(VCODEC_1P9V)
    1900;
#elif defined(VCODEC_1P95V)
    1950;
#else // VCODEC_1P7V
    1700;
#endif
static OPT_TYPE POSSIBLY_UNUSED uint16_t vhppa_mv =
#ifdef VHPPA_1P6V
    1600;
#elif defined(VHPPA_1P7V)
    1700;
#elif defined(VHPPA_1P9V)
    1900;
#elif defined(VHPPA_1P95V)
    1950;
#else // VHPPA_1P8V
    1800;
#endif

static enum PMU_POWER_MODE_T BOOT_BSS_LOC pmu_power_mode = PMU_POWER_MODE_NONE;

static enum PMU_VCORE_REQ_T BOOT_BSS_LOC pmu_vcore_req;

// Move all the data/bss invovled in pmu_open() to .sram_data/.sram_bss,
// so that pmu_open() can be called at the end of BootInit(),
// for data/bss is initialized after BootInit().
static enum PMU_VUSB_REQ_T SRAM_BSS_DEF(pmu_vusb_req);

static PMU_RTC_IRQ_HANDLER_T rtc_irq_handler;

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

static PMU_IRQ_HANDLER_T pmu_irq_hdlrs[PMU_IRQ_TYPE_QTY];

static uint8_t SRAM_BSS_DEF(vio_risereq_map);
STATIC_ASSERT(sizeof(vio_risereq_map) * 8 >= PMU_VIORISE_REQ_USER_QTY, "vio_risereq_map size too small");

static uint8_t SRAM_DATA_DEF(vio_act_normal) = IO_VOLT_ACTIVE_NORMAL;
static uint8_t SRAM_DATA_DEF(vio_act_rise) = IO_VOLT_ACTIVE_RISE;
static uint8_t SRAM_DATA_DEF(vio_lp) = IO_VOLT_SLEEP;

static const uint8_t ana_lp_dcdc = PMU_DCDC_ANA_SLEEP_1_3V;

static const uint8_t dig_lp_ldo = PMU_VDIG_0_6V;
static const uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_6V;

static unsigned int NOINLINE BOOT_TEXT_SRAM_LOC pmu_count_zeros(unsigned int val, unsigned int bits)
{
    int cnt = 0;
    int i;

    for (i = 0; i < bits; i++) {
        if ((val & (1 << i)) == 0) {
            cnt++;
        }
    }

    return cnt;
}

static int BOOT_TEXT_FLASH_LOC pmu_read_efuse_block(unsigned int block)
{
    int ret;
    unsigned short val;
    uint32_t start;

#define EFUSE_READ_TIMEOUT              MS_TO_TICKS(50)

    val = TSMC_EFUSE_PGENB | TSMC_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        return ret;
    }

    val = TSMC_EFUSE_STROBE_WIDTH(20) | REG_READ_STROBE_WIDTH(4) | EFUSE_SEL(block);
    ret = pmu_write(PMU_REG_EFUSE_SEL, val);
    if (ret) {
        return ret;
    }

    ret = pmu_read(PMU_REG_POWER_OFF, &val);
    if (ret) {
        return ret;
    }
    val = SET_BITFIELD(val, EFUSE_READ_TRIG, EFUSE_READ_TRIG_WORD);
    ret = pmu_write(PMU_REG_POWER_OFF, val);
    if (ret) {
        return ret;
    }

    start = hal_sys_timer_get();
    do {
        ret = pmu_read(PMU_REG_POWER_OFF, &val);
        if (ret) {
            return ret;
        }
        if (hal_sys_timer_get() - start > EFUSE_READ_TIMEOUT) {
            return -2;
        }
    } while ((val & (EFUSE_READ_DONE | EFUSE_READ_BUSY)) != EFUSE_READ_DONE);

    val = TSMC_EFUSE_PGENB;
    ret = pmu_write(PMU_REG_EFUSE_CTRL, val);
    if (ret) {
        return ret;
    }

    return 0;
}

uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    int ret;
    uint16_t val;
    uint32_t metal_id;
    int i;
    union BOOT_SETTINGS_T boot;

    // Reset PMU (to recover from a possible insane state, e.g., ESD reset)
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(4);

#define EFUSE_BLOCK_NUM                 6

    for (i = 0; i < EFUSE_BLOCK_NUM; i++) {
        ret = pmu_read_efuse_block(i);
        if (ret) {
            SAFE_PROGRAM_STOP();
        }
    }

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_BOOT, &boot.reg);
    if (ret) {
        boot.reg = 0;
    } else {
        if (pmu_count_zeros(boot.reg, 12) != boot.chksum) {
            boot.reg = 0;
        }
    }
    if (boot.crystal_freq != CRYSTAL_FREQ_26M) {
        if (boot.crystal_freq == CRYSTAL_FREQ_24M) {
            hal_cmu_set_crystal_freq(24000000);
        } else if (boot.crystal_freq == CRYSTAL_FREQ_40M) {
            hal_cmu_set_crystal_freq(40000000);
        } else if (boot.crystal_freq == CRYSTAL_FREQ_48M) {
            hal_cmu_set_crystal_freq(48000000);
        }
    }
    // Update ISPI cfg
    ret = hal_analogif_open();
    if (ret) {
        SAFE_PROGRAM_STOP();
    }

    if (hal_cmu_get_aon_chip_id() != 0xFFFFF) {
        // Enable vtoi power saving mode to ensure pll can be started
        pmu_read(0xC2, &val);
        val |= (1 << 15);
        pmu_write(0xC2, val);
    }

    // Enable 26M doubler (52M)
    pmu_write(0xB5, 0x8000);
#ifdef ANA_26M_X4_ENABLE
    pmu_write(0xBF, 0x0001);
#endif

    // Power up bbpll and audpll clk buf
    // Clk buf bits:
    // 0-rfpll 1-bt_dac 2-codec_resample 3-bbpll 4-audpll 5-usbhspll 6-nfmi 7-dig
    pmu_read(0xC4, &val);
    val &= ~0xFF;
    val |= (1 << 3) | (1 << 4) | (1 << 7);
#ifdef USB_HIGH_SPEED
    val |= (1 << 5);
#endif
    pmu_write(0xC4, val);

    // Cfg bbpll
    pmu_write(0x9F, 0xC22F);
    pmu_write(0xA0, 0x2788);

    // Cfg audpll
    pmu_write(0x79, 0x0011);
    pmu_write(0x77, 0x7191);

    // Set audpll to 24.576M*17
    pmu_write(0x7C, 0x4F16);
    pmu_write(0x7B, 0x011A);
    pmu_write(0x7A, 0x2008);

    // Clear reg_bt_tst_buf_sel_in/out to avoid impacting P00-P03 and P30-P33 pins
    pmu_write(0xA2, 0x01C2);

    // Enable SWD debug mode
    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val = SET_BITFIELD(val, PMIC_TMODE_1300, 2);
    pmu_write(PMU_REG_SLEEP_CFG, val);

    ret = pmu_read(PMU_REG_METAL_ID, &val);
    if (ret) {
        return 0;
    }

    metal_id = GET_BITFIELD(val, PMU_METAL_ID);

    return metal_id;
}

int BOOT_TEXT_SRAM_LOC pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;

    ret = pmu_read(PMU_REG_EFUSE_VAL_START + page, efuse);

    return ret;
}

int pmu_get_security_value(union SECURITY_VALUE_T *val)
{
    int ret;

    ret = pmu_get_efuse(PMU_EFUSE_PAGE_SECURITY, &val->reg);
    if (ret) {
        // Error
        goto _no_security;
    }

    if (!val->security_en) {
        // OK
        goto _no_security;
    }
    ret = 1;
    if (pmu_count_zeros(val->key_id, 3) != val->key_chksum) {
        // Error
        goto _no_security;
    }
    if (pmu_count_zeros(val->vendor_id, 6) != val->vendor_chksum) {
        // Error
        goto _no_security;
    }
    if ((pmu_count_zeros(val->reg, 15) & 1) != val->chksum) {
        // Error
        goto _no_security;
    }

    // OK
    return 0;

_no_security:
    val->reg = 0;

    return ret;
}

void pmu_shutdown(void)
{
    uint16_t val;
    uint32_t lock = int_lock();

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    pmu_mode_change(PMU_POWER_MODE_LDO);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    // Check RTC status
    bool rtc_enabled = false;
    bool rtc_alarm_set = false;
    uint32_t rtc_alarm_val = 0;

    if (pmu_rtc_enabled()) {
        rtc_enabled = true;
        if (pmu_rtc_alarm_status_set()) {
            rtc_alarm_set = true;
            rtc_alarm_val = pmu_rtc_get_alarm();
        }
    }
#endif

    // Reset PMU
    pmu_write(PMU_REG_METAL_ID, 0xCAFE);
    pmu_write(PMU_REG_METAL_ID, 0x5FEE);
    hal_sys_timer_delay(4);

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    // Restore RTC after PMU reset
    if (rtc_enabled) {
        pmu_rtc_enable();
        if (rtc_alarm_set) {
            pmu_rtc_set_alarm(rtc_alarm_val);
        }
    }
#endif

    pmu_read(PMU_REG_PWR_SEL, &val);
    val |= REG_PWR_SEL_DR | REG_PWR_SEL;
    pmu_write(PMU_REG_PWR_SEL, val);

    pmu_read(PMU_REG_POWER_OFF,&val);
    val |= SOFT_POWER_OFF;
    pmu_write(PMU_REG_POWER_OFF,val);
    hal_sys_timer_delay(MS_TO_TICKS(50));

    int_unlock(lock);

    //can't reach here
    TRACE("\nError: pmu_shutdown failed!\n");
    SAFE_PROGRAM_STOP();
}

static inline uint16_t pmu_get_module_addr(enum PMU_MODUAL_T module)
{
    if (module == PMU_HPPA2CODEC || module == PMU_CODEC) {
        return PMU_REG_MODULE_START + module + 1;
    } else if (module == PMU_BUCK2ANA) {
        return PMU_REG_BUCK2ANA_CFG;
    } else {
        return PMU_REG_MODULE_START + module;
    }
}

void pmu_module_config(enum PMU_MODUAL_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    if (module == PMU_ANA) {
        pmu_read(PMU_REG_BIAS_CFG, &val);
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
        pmu_write(PMU_REG_BIAS_CFG, val);
    } else if (module == PMU_DIG) {
        pmu_read(PMU_REG_DCDC_DIG_EN, &val);
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
        pmu_write(PMU_REG_DCDC_DIG_EN, val);
    } else if (module == PMU_HPPA2CODEC) {
        pmu_read(PMU_REG_SLEEP_CFG, &val);
        if(dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
        pmu_write(PMU_REG_SLEEP_CFG, val);
    } else if (module == PMU_CODEC) {
        pmu_read(PMU_REG_HPPA2CODEC_CFG, &val);
        if(dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
        pmu_write(PMU_REG_HPPA2CODEC_CFG, val);
    }

    pmu_read(module_address, &val);

    if (module != PMU_ANA && module != PMU_DIG) {
        if(is_manual) {
            val |= module_cfg_p->pu_dr;
        } else {
            val &= ~module_cfg_p->pu_dr;
        }

        if(ldo_on) {
            val |= module_cfg_p->pu;
        } else {
            val &= ~module_cfg_p->pu;
        }
    }

    if(lp_mode) {
        val &= ~module_cfg_p->lp_en_dr;
    } else {
        val = (val & ~module_cfg_p->lp_en) | module_cfg_p->lp_en_dr;
    }

    if (module != PMU_HPPA2CODEC && module != PMU_CODEC) {
        if(dpmode) {
            val |= module_cfg_p->pu_dsleep;
        } else {
            val &= ~module_cfg_p->pu_dsleep;
        }
    }

    pmu_write(module_address, val);
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    pmu_read(module_address, &val);
    val &= ~module_cfg_p->vbit_normal_mask;
    val |= (normal_v << module_cfg_p->vbit_normal_shift) & module_cfg_p->vbit_normal_mask;
    val &= ~module_cfg_p->vbit_dsleep_mask;
    val |= (sleep_v << module_cfg_p->vbit_dsleep_shift) & module_cfg_p->vbit_dsleep_mask;
    pmu_write(module_address, val);
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    int ret;
    unsigned short val;
    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = pmu_get_module_addr(module);

    ret = pmu_read(module_address, &val);
    if (ret == 0) {
        if (normal_vp) {
            *normal_vp = (val & module_cfg_p->vbit_normal_mask) >> module_cfg_p->vbit_normal_shift;
        }
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->vbit_dsleep_mask) >> module_cfg_p->vbit_dsleep_shift;
        }
    }

    return ret;
}

static void pmu_dcdc_ana_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_DCDC2_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_DCDC2_VBIT_DSLEEP);
    }
}

static void pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_VOLT, &val);
    val &= ~REG_DCDC2_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC2_VBIT_NORMAL_MASK;
    val |= REG_DCDC2_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC2_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_ANA_VOLT, val);
}

static void pmu_ana_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint16_t old_dcdc_volt;

    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_ana_get_volt(&old_dcdc_volt, NULL);
        if (old_dcdc_volt < ana_act_dcdc) {
            while (old_dcdc_volt++ < ana_act_dcdc) {
                pmu_dcdc_ana_set_volt(old_dcdc_volt, ana_lp_dcdc);
            }
        } else if (old_dcdc_volt > ana_act_dcdc) {
            while (old_dcdc_volt-- > ana_act_dcdc) {
                pmu_dcdc_ana_set_volt(old_dcdc_volt, ana_lp_dcdc);
            }
        }
    }
}

static void pmu_dcdc_dig_get_volt(unsigned short *normal_vp, unsigned short *dsleep_vp)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    if (normal_vp) {
        *normal_vp = GET_BITFIELD(val, REG_DCDC1_VBIT_NORMAL);
    }
    if (dsleep_vp) {
        *dsleep_vp = GET_BITFIELD(val, REG_DCDC1_VBIT_DSLEEP);
    }
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_DIG_VOLT, &val);
    val &= ~REG_DCDC1_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC1_VBIT_NORMAL_MASK;
    val |= REG_DCDC1_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC1_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_DIG_VOLT, val);
}

static void pmu_volt_change_delay(int cnt)
{
    while (cnt-- > 0) {
        // About 10us when SYS_FREQ=26M
        for (int i = 0; i < 70; i++) {
            asm volatile("nop");
        }
    }
}

static void pmu_dig_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;
    uint16_t dcdc_volt, old_dcdc_volt;
    uint16_t ldo_volt, old_ldo_volt;
    uint16_t ldo_real, dcdc_real;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

#if defined(HIGH_VCORE_MODE)
    ldo_volt = PMU_VDIG_1_1V;
    dcdc_volt = PMU_DCDC_DIG_1_1V;
#else
    if (0) {
    } else if (pmu_vcore_req & (PMU_VCORE_USB_HS_ENABLED)) {
        ldo_volt = PMU_VDIG_0_9V;
        dcdc_volt = PMU_DCDC_DIG_0_9V;
    } else if (pmu_vcore_req & (PMU_VCORE_IIR_FREQ_HIGH |
            PMU_VCORE_FLASH_FREQ_HIGH | PMU_VCORE_PSRAM_FREQ_HIGH |
            PMU_VCORE_SYS_FREQ_HIGH | PMU_VCORE_FLASH_WRITE_ENABLED)) {
        ldo_volt = PMU_VDIG_0_8V;
        dcdc_volt = PMU_DCDC_DIG_0_8V;
    } else {
        // Common cases
        // Vcore DCDC -> LDO, or LDO 0.95V -> 0.9V, might occasionaly cause a negative
        // glitch (about 50mV) . Flash controller is sensitive to vcore, and might not
        // work below 0.9V.
        // WORKAROUND: Increase vcore LDO to 1.0V
        ldo_volt = PMU_VDIG_0_7V;
        dcdc_volt = PMU_DCDC_DIG_0_7V;
    }
#endif

#ifdef PROGRAMMER
    // Try to keep the same vcore voltage as ROM (hardware default)
    if (ldo_volt < PMU_VDIG_0_9V) {
        ldo_volt = PMU_VDIG_0_9V;
    }
    if (dcdc_volt < PMU_DCDC_DIG_0_9V) {
        dcdc_volt = PMU_DCDC_DIG_0_9V;
    }
#endif

    pmu_module_get_volt(PMU_DIG, NULL, &old_ldo_volt);
    pmu_dcdc_dig_get_volt(&old_dcdc_volt, NULL);

    if (mode_change) {
        // Both DCDC and LDO might have been enabled
        ldo_real = (old_ldo_volt - PMU_VDIG_0_4V) / 0x2;
        dcdc_real = (old_dcdc_volt - PMU_DCDC_DIG_0_4V) / 0x10;
        if (dcdc_real < ldo_real) {
            dcdc_real = ldo_real;
        }
        ldo_real = PMU_VDIG_0_4V + dcdc_real * 0x2;
        dcdc_real = PMU_DCDC_DIG_0_4V + dcdc_real * 0x10;

        // Set the voltage to current real voltage
        if (old_ldo_volt < ldo_real) {
            old_ldo_volt = ldo_real;
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_ldo_volt);
        }
        if (old_dcdc_volt < dcdc_real) {
            old_dcdc_volt = dcdc_real;
            pmu_dcdc_dig_set_volt(old_dcdc_volt, dig_lp_dcdc);
        }
    }

    if (mode_change || mode == PMU_POWER_MODE_DIG_DCDC) {
        if (old_dcdc_volt < dcdc_volt) {
            while (old_dcdc_volt++ < dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_dcdc_volt, dig_lp_dcdc);
            }
        } else if (old_dcdc_volt > dcdc_volt) {
            while (old_dcdc_volt-- > dcdc_volt) {
                pmu_dcdc_dig_set_volt(old_dcdc_volt, dig_lp_dcdc);
            }
        }
    }

    if (mode_change || mode != PMU_POWER_MODE_DIG_DCDC) {
        if (old_ldo_volt < ldo_volt) {
            while (old_ldo_volt++ < ldo_volt) {
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_ldo_volt);
            }
        } else if (old_ldo_volt > ldo_volt) {
            while (old_ldo_volt-- > ldo_volt) {
                pmu_module_set_volt(PMU_DIG, dig_lp_ldo, old_ldo_volt);
            }
        }
    }

    if (mode_change == 0) {
        // Update the voltage of the other mode
        if (mode == PMU_POWER_MODE_DIG_DCDC) {
            pmu_module_set_volt(PMU_DIG, dig_lp_ldo, ldo_volt);
        } else {
            pmu_dcdc_dig_set_volt(dcdc_volt, dig_lp_dcdc);
        }
    }

    int_unlock(lock);
}

static void pmu_ldo_mode_en(void)
{
    unsigned short val;

    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

    pmu_ana_set_volt(1, PMU_POWER_MODE_LDO);
    pmu_dig_set_volt(1, PMU_POWER_MODE_LDO);

    pmu_volt_change_delay(3);

    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    val = (val & ~REG_PU_DCDC1) | REG_PU_DCDC1_DR;
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val = (val & ~REG_PU_DCDC2) | REG_PU_DCDC2_DR;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);
}

static void pmu_dcdc_ana_mode_en(void)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val = (val & ~REG_DCDC2_ULP_MODE_NORMAL) | REG_PU_DCDC2_DR | REG_PU_DCDC2;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);
    // Enable vcore ldo
    pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

    pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
    pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

    if (pmu_power_mode != PMU_POWER_MODE_DIG_DCDC) {
        pmu_volt_change_delay(3);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

static void pmu_dcdc_dual_mode_en(void)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_ANA_EN, &val);
    val = (val & ~REG_DCDC2_ULP_MODE_NORMAL) | REG_PU_DCDC2_DR | REG_PU_DCDC2;
    pmu_write(PMU_REG_DCDC_ANA_EN, val);

    pmu_read(PMU_REG_DCDC_DIG_EN, &val);
    val = (val & ~REG_DCDC1_ULP_MODE_NORMAL) | REG_PU_DCDC1_DR | REG_PU_DCDC1;
    pmu_write(PMU_REG_DCDC_DIG_EN, val);

    pmu_ana_set_volt(1, PMU_POWER_MODE_DIG_DCDC);
    pmu_dig_set_volt(1, PMU_POWER_MODE_DIG_DCDC);

    pmu_volt_change_delay(3);

    // Disable vana and vcore LDO
    pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    pmu_module_config(PMU_DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;

    if (pmu_power_mode == mode || mode == PMU_POWER_MODE_NONE) {
        return;
    }

    lock = int_lock();

    if (mode == PMU_POWER_MODE_ANA_DCDC) {
        pmu_dcdc_ana_mode_en();
    } else if (mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_dcdc_dual_mode_en();
    } else if (mode == PMU_POWER_MODE_LDO) {
        pmu_ldo_mode_en();
    }

    pmu_power_mode = mode;

    int_unlock(lock);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    unsigned short val;

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    if (sleep_en) {
        val |= SLEEP_ALLOW;
    } else {
        val &= ~SLEEP_ALLOW;
    }
    pmu_write(PMU_REG_SLEEP_CFG, val);
}

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
static uint32_t pmu_vcodec_mv_to_val(uint16_t mv)
{
#ifdef VHPPA2VCODEC
    if (mv == 2000) {
        return PMU_HPPA2CODEC_2_0V;
    } else if (mv == 2300) {
        return PMU_HPPA2CODEC_1_95V;
    } else {
        return PMU_HPPA2CODEC_1_7V;
    }
#else
    if (mv == 1900) {
        return PMU_CODEC_1_9V;
    } else if (mv == 1950) {
        return PMU_CODEC_1_9V;
    } else {
        return PMU_CODEC_1_7V;
    }
#endif
}

static void pmu_dcdc_hppa_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    unsigned short val;

    pmu_read(PMU_REG_DCDC_HPPA_VOLT, &val);
    val &= ~REG_DCDC3_VBIT_DSLEEP_MASK;
    val &= ~REG_DCDC3_VBIT_NORMAL_MASK;
    val |= REG_DCDC3_VBIT_DSLEEP(dsleep_v);
    val |= REG_DCDC3_VBIT_NORMAL(normal_v);
    pmu_write(PMU_REG_DCDC_HPPA_VOLT, val);
}

static void pmu_dcdc_hppa_en(int enable)
{
    uint16_t val;

    pmu_read(PMU_REG_DCDC_HPPA_EN, &val);
    if (enable) {
        val = (val & ~REG_DCDC3_ULP_MODE_NORMAL) | REG_PU_DCDC3_DR | REG_PU_DCDC3;
    } else {
        val = (val & ~(REG_DCDC3_ULP_MODE_NORMAL | REG_PU_DCDC3)) | REG_PU_DCDC3_DR;
    }
    pmu_write(PMU_REG_DCDC_HPPA_EN, val);
}

static void NOINLINE SRAM_TEXT_DEF(pmu_flash_calib)(void)
{
    uint32_t lock;
    uint32_t samp_cnt;
    uint32_t samp_idx;

    lock = int_lock();

    // Wait at least 2ms until both vcore and flash voltage are stable
    hal_sys_timer_delay(MS_TO_TICKS(5));

    norflash_sample_delay_calib(&samp_cnt, &samp_idx);

    if (samp_cnt) {
        // vcore voltage might be increased later, so select a medium-smaller sample delay
        samp_idx += (samp_cnt  - 1) / 2;
        norflash_set_sample_delay(samp_idx);
    }

    int_unlock(lock);
}
#endif

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint16_t val;
    enum PMU_POWER_MODE_T mode;
    enum HAL_CHIP_METAL_ID_T POSSIBLY_UNUSED metal_id;
    enum PMU_MODUAL_T codec;

    ASSERT(!vcodec_off || vcodec_mv == vhppa_mv,
        "Invalid vcodec/vhppa cfg: vcodec_off=%d vcodec_mv=%u vhppa_mv=%u", vcodec_off, vcodec_mv, vhppa_mv);
    ASSERT(vcodec_mv == 1600 || vcodec_mv == 1700 || vcodec_mv == 1800 || vcodec_mv == 1900 || vcodec_mv == 1950,
        "Invalid vcodec cfg: vcodec_mv=%u", vcodec_mv);
    ASSERT(vhppa_mv == 1600 || vhppa_mv == 1700 || vhppa_mv == 1800 || vhppa_mv == 1900 || vhppa_mv == 1950,
        "Invalid vhppa cfg: vhppa_mv=%u", vhppa_mv);

    metal_id = hal_get_chip_metal_id();

    // Disable all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);

    // Allow PMU to sleep when power key is pressed
    pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    val &= ~POWERKEY_WAKEUP_OSC_EN;
    pmu_write(PMU_REG_POWER_KEY_CFG, val);

    // Allow low power bandgap
    pmu_read(PMU_REG_BIAS_CFG, &val);
    val &= ~BG_VBG_SEL_DR;
    pmu_write(PMU_REG_BIAS_CFG, val);

    // Enable DCDC ramp
    pmu_read(PMU_REG_DCDC_RAMP_EN, &val);
    val |= (REG_DCDC1_RAMP_EN | REG_DCDC2_RAMP_EN | REG_DCDC3_RAMP_EN);
    pmu_write(PMU_REG_DCDC_RAMP_EN, val);

#ifndef NO_SLEEP
    pmu_sleep_en(true);
#endif

    // Disable vhppa ldo
    pmu_read(PMU_REG_HPPA_LDO_EN, &val);
    val &= ~(REG_PU_LDO_VHPPA_DSLEEP | REG_PU_LDO_VHPPA_EN);
    pmu_write(PMU_REG_HPPA_LDO_EN, val);
    // Disable vbat2vcore
    pmu_module_config(PMU_BAT2DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vbuck2ana
    pmu_module_config(PMU_BUCK2ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

#ifdef VMEM_ON
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    pmu_module_set_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
    pmu_dcdc_hppa_en(false);
#else
    // vmem and vhppa is shorted -- enable vhppa dcdc and disable vmem ldo
    pmu_dcdc_hppa_en(true);
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif

    pmu_module_config(PMU_GP,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);

#ifndef PROGRAMMER
#ifdef VUSB_ON
    pmu_vusb_req |= PMU_VUSB_REQ_INIT;
#endif
    // Disable usbphy power, and vusb if possible
    pmu_usb_config(PMU_USB_CONFIG_TYPE_NONE);
#ifdef VUSB_ON
    pmu_vusb_req &= ~PMU_VUSB_REQ_INIT;
#endif
#endif

#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#else
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
#endif
    pmu_module_set_volt(PMU_IO, vio_lp, vio_act_normal);

    // Disable vcodec
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    // Disable vhppa2vcodec
    pmu_module_config(PMU_HPPA2CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#ifdef VHPPA2VCODEC
    codec = PMU_HPPA2CODEC;
#else
    codec = PMU_CODEC;
#endif
#ifdef ANC_APP
    if (!vcodec_off) {
        // Enable codec
        pmu_module_config(codec,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
#endif
    val = pmu_vcodec_mv_to_val(vcodec_mv);
    pmu_module_set_volt(codec, val, val);

    if (vhppa_mv == 1600) {
        val = PMU_DCDC_HPPA_1_6V;
    } else if (vhppa_mv == 1700) {
        val = PMU_DCDC_HPPA_1_7V;
    } else if (vhppa_mv == 1900) {
        val = PMU_DCDC_HPPA_1_9V;
    } else if (vhppa_mv == 1950) {
        val = PMU_DCDC_HPPA_1_95V;
    } else {
        val = PMU_DCDC_HPPA_1_8V;
    }
    pmu_dcdc_hppa_set_volt(val, val);

#ifdef DIG_DCDC_MODE
    mode = PMU_POWER_MODE_DIG_DCDC;
#elif defined(ANA_DCDC_MODE)
    mode = PMU_POWER_MODE_ANA_DCDC;
#else // LDO_MODE
    mode = PMU_POWER_MODE_LDO;
#endif

    pmu_mode_change(mode);

    pmu_flash_calib();

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_sleep(void)
{
    return;
}

void pmu_wakeup(void)
{
    return;
}

void pmu_codec_config(int enable)
{
    enum PMU_MODUAL_T codec;

#ifdef VHPPA2VCODEC
    codec = PMU_HPPA2CODEC;
#else
    codec = PMU_CODEC;
#endif

    if (vcodec_off) {
#ifdef VMEM_ON
        pmu_dcdc_hppa_en(enable);
#endif
    } else {
        if (enable) {
            pmu_module_config(codec,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        } else {
            pmu_module_config(codec,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        }
    }
}

void pmu_codec_hppa_enable(int enable)
{
    if (!vcodec_off) {
#ifdef VMEM_ON
        pmu_dcdc_hppa_en(enable);
#endif
    }
}

void pmu_codec_mic_bias_enable(uint32_t map)
{
    uint16_t val;
    int i;
    enum PMU_REG_T bias_reg;
    uint16_t ldo_en;
    uint8_t volt;
    uint8_t bias;
    static const uint16_t ldo_mask = REG_MIC_LDOA_EN | REG_MIC_LDOB_EN | REG_MIC_LDOC_EN | REG_MIC_LDOD_EN | REG_MIC_LDOE_EN;

#ifdef DIGMIC_HIGH_VOLT
    volt = PMU_VMIC_3_3V;
#else
    volt = PMU_VMIC_2_2V;
#endif

    if (volt > PMU_VMIC_2_8V) {
        bias = PMU_VMIC_BIAS_2_2V + (volt - PMU_VMIC_2_8V);
    } else {
        bias = PMU_VMIC_BIAS_2_2V;
    }

    ldo_en = 0;
    for (i = 0; i < MAX_ANA_MIC_CH_NUM; i++) {
        if (i == MAX_ANA_MIC_CH_NUM - 1) {
            bias_reg = PMU_REG_MIC_BIAS_E;
        } else {
            bias_reg = PMU_REG_MIC_BIAS_A + i;
        }
        pmu_read(bias_reg, &val);
        if (map & (AUD_VMIC_MAP_VMIC1 << i)) {
            ldo_en |= (REG_MIC_LDOA_EN << i);
            val = (val & ~(REG_MIC_BIASA_VSEL_MASK | REG_MIC_LDOA_RES_MASK)) |
                REG_MIC_BIASA_VSEL(bias) | REG_MIC_LDOA_RES(volt) | REG_MIC_BIASA_EN;
        } else {
            val &= ~REG_MIC_BIASA_EN;
        }
        pmu_write(bias_reg, val);
    }
    pmu_read(PMU_REG_MIC_LDO_EN, &val);
    val = (val & ~ldo_mask) | ldo_en;
    pmu_write(PMU_REG_MIC_LDO_EN, val);
}

void pmu_codec_adc_pre_start(void)
{
}

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_codec_dac_pre_start(void)
{
}

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t))
{
}

void pmu_flash_write_config(void)
{
    uint32_t lock;

    if (pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req |= PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void pmu_flash_read_config(void)
{
    uint32_t lock;

    if ((pmu_vcore_req & PMU_VCORE_FLASH_WRITE_ENABLED) == 0) {
        return;
    }

    lock = int_lock();
    pmu_vcore_req &= ~PMU_VCORE_FLASH_WRITE_ENABLED;
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);

    // Uncached access workarond: Always access via cache
    hal_cache_enable(HAL_CACHE_ID_I_CACHE, HAL_CACHE_NO);
    hal_cache_enable(HAL_CACHE_ID_I_CACHE, HAL_CACHE_YES);
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
#ifdef OSC_26M_X4_AUD2BB
        // The real max freq is 52M
#else
        // The real max freq is 120M
        //pmu_vcore_req |= PMU_VCORE_FLASH_FREQ_HIGH;
#endif
    } else {
        pmu_vcore_req &= ~PMU_VCORE_FLASH_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_psram_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    uint32_t lock;

    lock = int_lock();
    if (freq > 52000000) {
        pmu_vcore_req |= PMU_VCORE_PSRAM_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_PSRAM_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void pmu_anc_config(int enable)
{
}

void pmu_fir_high_speed_config(int enable)
{
}

void pmu_iir_freq_config(uint32_t freq)
{
    uint32_t lock;

    lock = int_lock();
    if (freq >= 90000000) {
        pmu_vcore_req |= PMU_VCORE_IIR_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_IIR_FREQ_HIGH;
    }
    int_unlock(lock);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
#ifndef OSC_26M_X4_AUD2BB
    uint32_t lock;

    lock = int_lock();
    if (freq > HAL_CMU_FREQ_104M) {
        pmu_vcore_req |= PMU_VCORE_SYS_FREQ_HIGH;
    } else {
        pmu_vcore_req &= ~PMU_VCORE_SYS_FREQ_HIGH;
    }
    int_unlock(lock);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
#endif
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
    uint32_t lock;
    bool update;
    unsigned short ldo_on, deep_sleep_on;

    update = false;

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        usbphy_ldo_config(false);

        ldo_on = PMU_LDO_OFF;
        deep_sleep_on = PMU_DSLEEP_MODE_OFF;
    } else {
        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
    }

    lock = int_lock();

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        pmu_vusb_req &= ~PMU_VUSB_REQ_USB;
        if (pmu_vusb_req == 0) {
            update = true;
        }
    } else {
        if (pmu_vusb_req == 0) {
            update = true;
        }
        pmu_vusb_req |= PMU_VUSB_REQ_USB;
    }

    if (update) {
        pmu_module_config(PMU_USB, PMU_MANUAL_MODE, ldo_on, PMU_LP_MODE_ON, deep_sleep_on);
    }

#ifdef USB_HIGH_SPEED
    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        pmu_vcore_req &= ~PMU_VCORE_USB_HS_ENABLED;
    } else {
        pmu_vcore_req |= PMU_VCORE_USB_HS_ENABLED;
    }
#endif

    int_unlock(lock);

    if (pmu_power_mode != PMU_POWER_MODE_NONE) {
        // PMU has been inited
        pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
    }

    if (type != PMU_USB_CONFIG_TYPE_NONE) {
        usbphy_ldo_config(true);
    }
}

void pmu_nfmi_config(int enable)
{
    uint32_t lock;
    bool update;
    unsigned short ldo_on, deep_sleep_on;

    update = false;

    if (enable) {
        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
    } else {
        ldo_on = PMU_LDO_OFF;
        deep_sleep_on = PMU_DSLEEP_MODE_OFF;
    }

    lock = int_lock();

    if (enable) {
        if (pmu_vusb_req == 0) {
            update = true;
        }
        pmu_vusb_req |= PMU_VUSB_REQ_NFMI;
    } else {
        pmu_vusb_req &= ~PMU_VUSB_REQ_NFMI;
        if (pmu_vusb_req == 0) {
            update = true;
        }
    }

    if (update) {
        pmu_module_config(PMU_USB, PMU_MANUAL_MODE, ldo_on, PMU_LP_MODE_ON, deep_sleep_on);
    }

    int_unlock(lock);
}

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~(REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK |
        REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= REG_AC_ON_OUT_EN | REG_AC_ON_IN_EN | REG_CHARGE_INTR_EN;
    readval_cfg = SET_BITFIELD(readval_cfg, REG_AC_ON_DB_VALUE, 0);
    pmu_write(PMU_REG_CHARGER_CFG ,readval_cfg);
    int_unlock(lock);
}

static void pmu_charger_irq_handler(void)
{
    enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;
    unsigned short readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    pmu_write(PMU_REG_CHARGER_STATUS ,readval);
    int_unlock(lock);
    TRACE("%s REG_%02X=0x%04X", __func__, PMU_REG_CHARGER_STATUS, readval);

    if ((readval & (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) == 0){
        TRACE("%s SKIP", __func__);
        return;
    } else if ((readval & (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) ==
            (AC_ON_DET_IN_MASKED | AC_ON_DET_OUT_MASKED)) {
        TRACE("%s DITHERING", __func__);
        hal_sys_timer_delay(2);
    } else {
        TRACE("%s NORMAL", __func__);
    }

    status = pmu_charger_get_status();

    if (charger_irq_handler) {
        charger_irq_handler(status);
    }
}

void pmu_charger_set_irq_handler(PMU_CHARGER_IRQ_HANDLER_T handler)
{
    uint32_t lock;
    uint16_t val;

    charger_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &val);
    val |= REG_CHARGE_IN_INTR_MSK | REG_CHARGE_OUT_INTR_MSK;
    pmu_write(PMU_REG_CHARGER_CFG, val);
    int_unlock(lock);

    pmu_set_irq_handler(PMU_IRQ_TYPE_CHARGER, pmu_charger_irq_handler);
}

void pmu_charger_plugin_config(void)
{
    if (IO_VOLT_ACTIVE_RISE < PMU_IO_3_1V) {
        vio_act_rise = PMU_IO_3_1V;
    }
    if (IO_VOLT_SLEEP < PMU_IO_3_2V) {
        vio_lp = PMU_IO_3_2V;
    }
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, true);
}

void pmu_charger_plugout_config(void)
{
    vio_act_rise = IO_VOLT_ACTIVE_RISE;
    vio_lp = IO_VOLT_SLEEP;
    pmu_viorise_req(PMU_VIORISE_REQ_USER_CHARGER, false);
}

enum PMU_CHARGER_STATUS_T pmu_charger_get_status(void)
{
    unsigned short readval;
    enum PMU_CHARGER_STATUS_T status;

    pmu_read(PMU_REG_CHARGER_STATUS, &readval);
    if (readval & AC_ON)
        status = PMU_CHARGER_PLUGIN;
    else
        status = PMU_CHARGER_PLUGOUT;

    return status;
}

void pmu_rtc_enable(void)
{
    uint16_t readval;
    uint32_t lock;

#ifdef SIMU
    // Set RTC counter to 1KHz
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32 - 2);
#else
    // Set RTC counter to 1Hz
    pmu_write(PMU_REG_RTC_DIV_1HZ, 32000 - 2);
#endif

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval |= RTC_POWER_ON_EN | PU_LPO_DR | PU_LPO_REG;
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

void pmu_rtc_disable(void)
{
    uint16_t readval;
    uint32_t lock;

    pmu_rtc_clear_alarm();

    lock = int_lock();
    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);
    readval &= ~(RTC_POWER_ON_EN | PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & RTC_POWER_ON_EN);
}

void pmu_rtc_set(uint32_t seconds)
{
    uint16_t high, low;

    // Need 3 seconds to load a new value
    seconds += 3;

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_RTC_LOAD_LOW, low);
    pmu_write(PMU_REG_RTC_LOAD_HIGH, high);
}

uint32_t pmu_rtc_get(void)
{
    uint16_t high, low, high2;

    pmu_read(PMU_REG_RTC_VAL_HIGH, &high);
    pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    // Handle counter wrap
    pmu_read(PMU_REG_RTC_VAL_HIGH, &high2);
    if (high != high2) {
        high = high2;
        pmu_read(PMU_REG_RTC_VAL_LOW, &low);
    }

    return (high << 16) | low;
}

void pmu_rtc_set_alarm(uint32_t seconds)
{
    uint16_t readval;
    uint16_t high, low;
    uint32_t lock;

    // Need 1 second to raise the interrupt
    if (seconds > 0) {
        seconds -= 1;
    }

    high = seconds >> 16;
    low = seconds & 0xFFFF;

    pmu_write(PMU_REG_INT_CLR, RTC_INT0_MSKED);

    pmu_write(PMU_REG_RTC_MATCH0_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH0_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= RTC_INT_EN_0;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);
}

uint32_t pmu_rtc_get_alarm(void)
{
    uint16_t high, low;

    pmu_read(PMU_REG_RTC_MATCH0_LOW, &low);
    pmu_read(PMU_REG_RTC_MATCH0_HIGH, &high);

    // Compensate the alarm offset
    return (uint32_t)((high << 16) | low) + 1;
}

void pmu_rtc_clear_alarm(void)
{
    uint16_t readval;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval &= ~RTC_INT_EN_0;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, RTC_INT0_MSKED);
}

int pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & RTC_INT_EN_0);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & RTC_INT_0);
}

static void pmu_rtc_irq_handler(void)
{
    uint32_t seconds;

    if (pmu_rtc_alarm_alerted()) {
        pmu_rtc_clear_alarm();

        if (rtc_irq_handler) {
            seconds = pmu_rtc_get();
            rtc_irq_handler(seconds);
        }
    }
}

void pmu_rtc_set_irq_handler(PMU_RTC_IRQ_HANDLER_T handler)
{
    uint16_t readval;
    uint32_t lock;

    rtc_irq_handler = handler;

    lock = int_lock();
    pmu_read(PMU_REG_INT_MASK, &readval);
    if (handler) {
        readval |= RTC_INT0_MSK;
    } else {
        readval &= ~RTC_INT0_MSK;
    }
    pmu_write(PMU_REG_INT_MASK, readval);
    int_unlock(lock);

    pmu_set_irq_handler(PMU_IRQ_TYPE_RTC, pmu_rtc_irq_handler);
}

int pmu_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type)
{
    uint16_t val;
    uint16_t int_en;

    if (type == HAL_PWRKEY_IRQ_NONE) {
        // Clear existing IRQs
        pmu_write(PMU_REG_CHARGER_STATUS, (POWER_ON_PRESS | POWER_ON_RELEASE));
        // Disable future IRQs
        int_en = 0;
    } else if (type == HAL_PWRKEY_IRQ_FALLING_EDGE) {
        int_en = POWERON_PRESS_EN;
    } else if (type == HAL_PWRKEY_IRQ_RISING_EDGE) {
        int_en = POWERON_RELEASE_EN;
    } else if (type == HAL_PWRKEY_IRQ_BOTH_EDGE) {
        int_en = POWERON_PRESS_EN | POWERON_RELEASE_EN;
    } else {
        return 1;
    }

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val = (val & ~(POWERON_PRESS_EN | POWERON_RELEASE_EN)) | int_en;
    pmu_write(PMU_REG_SLEEP_CFG, val);

    return 0;
}
int hal_pwrkey_set_irq(enum HAL_PWRKEY_IRQ_T type) __attribute__((alias("pmu_pwrkey_set_irq")));

bool pmu_pwrkey_pressed(void)
{
    uint16_t val;

    pmu_read(PMU_REG_CHARGER_STATUS, &val);

    return !!(val & POWER_ON);
}
bool hal_pwrkey_pressed(void) __attribute__((alias("pmu_pwrkey_pressed")));

enum HAL_PWRKEY_IRQ_T pmu_pwrkey_get_irq_state(void)
{
    enum HAL_PWRKEY_IRQ_T state = HAL_PWRKEY_IRQ_NONE;
    uint16_t val;

    pmu_read(PMU_REG_CHARGER_STATUS, &val);

    if (val & POWER_ON_PRESS) {
        state |= HAL_PWRKEY_IRQ_FALLING_EDGE;
    }

    if (val & POWER_ON_RELEASE) {
        state |= HAL_PWRKEY_IRQ_RISING_EDGE;
    }

    return state;
}
enum HAL_PWRKEY_IRQ_T hal_pwrkey_get_irq_state(void) __attribute__((alias("pmu_pwrkey_get_irq_state")));

static void pmu_general_irq_handler(void)
{
    uint16_t val;

    pmu_read(PMU_REG_CHARGER_STATUS, &val);
    if (val & (POWER_ON_PRESS | POWER_ON_RELEASE)) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_PWRKEY]();
        }
    }
    if (val & (AC_ON_DET_OUT_MASKED | AC_ON_DET_IN_MASKED)) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_CHARGER]();
        }
    }

    pmu_read(PMU_REG_INT_STATUS, &val);
    if (val & (KEY_ERR1_INTR | KEY_ERR0_INTR | KEY_PRESS_INTR | KEY_RELEASE_INTR |
            SAMPLE_PERIOD_DONE_INTR | CHAN_DATA_VALID_INTR_MASK)) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_GPADC]();
        }
    }
    if (val & (RTC_INT_1 | RTC_INT_0)) {
        if (pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]) {
            pmu_irq_hdlrs[PMU_IRQ_TYPE_RTC]();
        }
    }
}

int pmu_set_irq_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_HANDLER_T hdlr)
{
    bool update;
    uint32_t lock;
    int i;

    if (type >= PMU_IRQ_TYPE_QTY) {
        return 1;
    }

    update = false;

    lock = int_lock();

    for (i = 0; i < PMU_IRQ_TYPE_QTY; i++) {
        if (pmu_irq_hdlrs[i]) {
            break;
        }
    }

    pmu_irq_hdlrs[type] = hdlr;

    if (hdlr) {
        update = (i >= PMU_IRQ_TYPE_QTY);
    } else {
        if (i == type) {
            for (; i < PMU_IRQ_TYPE_QTY; i++) {
                if (pmu_irq_hdlrs[i]) {
                    break;
                }
            }
            update = (i >= PMU_IRQ_TYPE_QTY);
        }
    }

    if (update) {
        if (hdlr) {
            NVIC_SetVector(PMU_IRQn, (uint32_t)pmu_general_irq_handler);
            NVIC_SetPriority(PMU_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(PMU_IRQn);
            NVIC_EnableIRQ(PMU_IRQn);
        } else {
            NVIC_DisableIRQ(PMU_IRQn);
        }
    }

    int_unlock(lock);

    return 0;
}

void pmu_viorise_req(enum PMU_VIORISE_REQ_USER_T user, bool rise)
{
    uint32_t lock;

    lock = int_lock();
    if (rise) {
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_rise);
        }
        vio_risereq_map |= (1 << user);
    } else {
        vio_risereq_map &= ~(1 << user);
        if (vio_risereq_map == 0) {
            pmu_module_set_volt(PMU_IO,vio_lp,vio_act_normal);
        }
    }
    int_unlock(lock);
}

int pmu_debug_config_ana(uint16_t volt)
{
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
#ifdef ANC_PROD_TEST
    if (volt == 1600 || volt == 1700 || volt == 1800 || volt == 1900 || volt == 1950) {
        vcodec_mv = volt;
    } else {
        return 1;
    }

    if (volt <= 1950) {
        if (volt == 1600) {
            vhppa_mv = PMU_DCDC_HPPA_1_6V;
        } else if (volt == 1700) {
            vhppa_mv = PMU_DCDC_HPPA_1_7V;
        } else if (volt == 1800) {
            vhppa_mv = PMU_DCDC_HPPA_1_8V;
        } else if (volt == 1900) {
            vhppa_mv = PMU_DCDC_HPPA_1_9V;
        } else if (volt == 1950) {
            vhppa_mv = PMU_DCDC_HPPA_1_95V;
        }
        vcodec_off = true;
    } else {
        vcodec_off = false;
    }
#endif
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

int pmu_debug_config_audio_output(bool diff)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_2V;
    } else {
        volt = PMU_DCDC_ANA_1_3V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

void pmu_led_set_direction(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_DIR_T dir)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO1_OENB;
            } else {
                val &= ~REG_LED_IO1_OENB;
            }
        } else {
            if (dir == HAL_GPIO_DIR_IN) {
                val |= REG_LED_IO2_OENB;
            } else {
                val &= ~REG_LED_IO2_OENB;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    }
}

enum HAL_GPIO_DIR_T pmu_led_get_direction(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            return (val & REG_LED_IO1_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        } else {
            return (val & REG_LED_IO2_OENB) ? HAL_GPIO_DIR_IN : HAL_GPIO_DIR_OUT;
        }
    } else {
        return HAL_GPIO_DIR_IN;
    }
}

void pmu_led_set_voltage_domains(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_VOLTAGE_DOMAINS_T volt)
{
    enum PMU_LED_VOLT_T {
        PMU_LED_VOLT_ITF,
        PMU_LED_VOLT_VMEM,
        PMU_LED_VOLT_VIO,
    };
    enum PMU_LED_VOLT_T sel;
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        if (volt == HAL_IOMUX_PIN_VOLTAGE_VIO) {
            sel = PMU_LED_VOLT_VIO;
        } else {
            sel = PMU_LED_VOLT_VMEM;
        }

        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val = SET_BITFIELD(val, REG_LED_IO1_SEL, sel);
        } else {
            val = SET_BITFIELD(val, REG_LED_IO2_SEL, sel);
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    }
}

void pmu_led_set_pull_select(enum HAL_IOMUX_PIN_T pin, enum HAL_IOMUX_PIN_PULL_SELECT_T pull_sel)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_IOMUX_PIN_LED1 || pin == HAL_IOMUX_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_LED_CFG, &val);
        if (pin == HAL_IOMUX_PIN_LED1) {
            val &= ~(REG_LED_IO1_PDEN | REG_LED_IO1_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
                val |= REG_LED_IO1_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE) {
                val |= REG_LED_IO1_PDEN;
            }
        } else {
            val &= ~(REG_LED_IO2_PDEN | REG_LED_IO2_PUEN);
            if (pull_sel == HAL_IOMUX_PIN_PULLUP_ENALBE) {
                val |= REG_LED_IO2_PUEN;
            } else if (pull_sel == HAL_IOMUX_PIN_PULLDOWN_ENALBE) {
                val |= REG_LED_IO2_PDEN;
            }
        }
        pmu_write(PMU_REG_LED_CFG, val);
        int_unlock(lock);
    }
}

void pmu_led_set_value(enum HAL_GPIO_PIN_T pin, int data)
{
    uint16_t val;
    uint32_t lock;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        lock = int_lock();
        pmu_read(PMU_REG_PWM_EN, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            if (data) {
                val |= REG_LED0_OUT;
            } else {
                val &= ~REG_LED0_OUT;
            }
        } else {
            if (data) {
                val |= REG_LED1_OUT;
            } else {
                val &= ~REG_LED1_OUT;
            }
        }
        pmu_write(PMU_REG_PWM_EN, val);
        int_unlock(lock);
    }
}

int pmu_led_get_value(enum HAL_GPIO_PIN_T pin)
{
    uint16_t val;
    int data = 0;

    if (pin == HAL_GPIO_PIN_LED1 || pin == HAL_GPIO_PIN_LED2) {
        pmu_read(PMU_REG_POWER_OFF, &val);
        if (pin == HAL_GPIO_PIN_LED1) {
            data = REG_LED_IO1_DATA_IN;
        } else {
            data = REG_LED_IO2_DATA_IN;
        }
        data &= val;
    }

    return !!data;
}

