#include "cmsis.h"
#include "cmsis_nvic.h"
#include "pmu.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_chipid.h"
#include "hal_location.h"
#include "tgt_hardware.h"

// Recommended codec voltage in tgt_hardware.h:
// 1.7/1.8V     BT
// 2.3/2.7V     USB-AUDIO

#ifndef CODEC_VOLT_DEFAULT
#ifdef CODEC_HIGH_QUALITY
#define CODEC_VOLT_DEFAULT              PMU_CODEC_2_7V
#else
#define CODEC_VOLT_DEFAULT              PMU_CODEC_2_7V
#endif
#endif

#ifndef CODEC_VOLT_ANC
#ifdef AUD_PLL_DOUBLE
#define CODEC_VOLT_ANC                  PMU_CODEC_3_0V
#else
#define CODEC_VOLT_ANC                  CODEC_VOLT_DEFAULT
#endif
#endif

#ifdef CODEC_HIGH_QUALITY
#if (CODEC_VOLT_DEFAULT < PMU_CODEC_2_3V)
#error "CODEC_VOLT_DEFAULT is too low"
#endif
#if (CODEC_VOLT_ANC < PMU_CODEC_2_3V)
#error "CODEC_VOLT_ANC is too low"
#endif
#endif

#ifdef __PMU_VIO_DYNAMIC_CTRL_MODE__
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_2V
#else
#define IO_VOLT_ACTIVE_NORMAL           PMU_IO_2_6V
#endif
#define IO_VOLT_ACTIVE_RISE             PMU_IO_2_6V
#define IO_VOLT_SLEEP                   PMU_IO_2_7V

#define PMU_DCDC_ANA2_FLAG              (1 << 7)

#define  PMU_VMEM_2_8V   0x16
#define  PMU_VMEM_2_1V   0x9
#define  PMU_VMEM_2_0V   0x8
#define  PMU_VMEM_1_9V   0x7
#define  PMU_VMEM_1_8V   0x6
#define  PMU_VMEM_1_7V   0x5
#define  PMU_VMEM_1_6V   0x4

#define  PMU_VDIG_1_4V    0xF
#define  PMU_VDIG_1_3V    0xE
#define  PMU_VDIG_1_2V    0xC
#define  PMU_VDIG_1_1V    0xA
#define  PMU_VDIG_1_0V    0x8
#define  PMU_VDIG_0_9V    0x6
#define  PMU_VDIG_0_8V    0x4
#define  PMU_VDIG_0_7V    0x2
#define  PMU_VDIG_0_6V    0x0

#define  PMU_DCDC_ANA_2_3V   0xF
#define  PMU_DCDC_ANA_2_2V   0xE
#define  PMU_DCDC_ANA_2_1V   0xD
#define  PMU_DCDC_ANA_2_0V   0xC
#define  PMU_DCDC_ANA_1_9V   0xB
#define  PMU_DCDC_ANA_1_8V   0xA
#define  PMU_DCDC_ANA_1_7V   0x9
#define  PMU_DCDC_ANA_1_6V   0x8
#define  PMU_DCDC_ANA_1_5V   0x7
#define  PMU_DCDC_ANA_1_4V   0x6

#define  PMU_DCDC_ANA_0_8V   0x3
#define  PMU_DCDC_ANA_0_7V   0x2
#define  PMU_DCDC_ANA_0_6V   0x1

#define  PMU_DCDC_ANA_SLEEP_1_6V   0xF

#define PMU_DCDC_DIG_1_3V    0x5
#define PMU_DCDC_DIG_1_2V    0x4
#define PMU_DCDC_DIG_1_1V    0x3
#define PMU_DCDC_DIG_1_0V    0x2
#define PMU_DCDC_DIG_0_9V    0x1
#define PMU_DCDC_DIG_0_8V    0x0

#define PMU_IO_2_8V         0x10
#define PMU_IO_2_7V         0xF
#define PMU_IO_2_6V         0xE
#define PMU_IO_2_5V         0xD
#define PMU_IO_2_4V         0xC
#define PMU_IO_2_3V         0xB
#define PMU_IO_2_2V         0xa
#define PMU_IO_2_1V         0x9
#define PMU_IO_2_0V         0x8
#define PMU_IO_1_9V         0x7
#define PMU_IO_1_8V         0x6
#define PMU_IO_1_7V         0x5

#define PMU_CODEC_3_5V      0xD
#define PMU_CODEC_3_4V      0xC
#define PMU_CODEC_3_3V      0xB
#define PMU_CODEC_3_2V      0xA
#define PMU_CODEC_3_1V      0x9
#define PMU_CODEC_3_0V      0x8
#define PMU_CODEC_2_9V      0x7
#define PMU_CODEC_2_8V      0x6
#define PMU_CODEC_2_7V      0x5
#define PMU_CODEC_2_6V      0x4
#define PMU_CODEC_2_5V      0x3
#define PMU_CODEC_2_4V      0x2
#define PMU_CODEC_2_3V      0x1
#define PMU_CODEC_2_2V      0x0

#define PMU_USB_3_9V      0xF
#define PMU_USB_3_8V      0xE
#define PMU_USB_3_7V      0xD
#define PMU_USB_3_6V      0xC
#define PMU_USB_3_5V      0xB
#define PMU_USB_3_4V      0xA
#define PMU_USB_3_3V      0x9
#define PMU_USB_3_2V      0x8
#define PMU_USB_3_1V      0x7
#define PMU_USB_3_0V      0x6
#define PMU_USB_2_9V      0x5
#define PMU_USB_2_8V      0x4
#define PMU_USB_2_7V      0x3
#define PMU_USB_2_6V      0x2
#define PMU_USB_2_5V      0x1
#define PMU_USB_2_4V      0x0

enum PMU_DCDC_ANA2_VOLT_T {
    PMU_DCDC_ANA2_2_3V = 0x8 | PMU_DCDC_ANA2_FLAG,
    PMU_DCDC_ANA2_2_5V,
    PMU_DCDC_ANA2_2_6V,
    PMU_DCDC_ANA2_2_8V,
    PMU_DCDC_ANA2_3_0V,
    PMU_DCDC_ANA2_3_1V,
    PMU_DCDC_ANA2_3_4V,
    PMU_DCDC_ANA2_3_5V,
};

enum PMU_DCDC_ANA2_SLEEP_VOLT_T {
    PMU_DCDC_ANA2_SLEEP_1_6V = 0x8 | PMU_DCDC_ANA2_FLAG,
    PMU_DCDC_ANA2_SLEEP_1_7V,
    PMU_DCDC_ANA2_SLEEP_1_8V,
    PMU_DCDC_ANA2_SLEEP_1_9V,
    PMU_DCDC_ANA2_SLEEP_2_0V,
    PMU_DCDC_ANA2_SLEEP_2_1V,
    PMU_DCDC_ANA2_SLEEP_2_2V,
    PMU_DCDC_ANA2_SLEEP_2_3V,
};

///6C   PMU_REG_CHARGER_STATUS
#define PMU_CHARGER_STATUS_IN           (1 << 0)

///6F   PMU_REG_EFUSE_SEL
#define PMU_EFUSE_TRIGGER               (1 << 15)
#define PMU_EFUSE_CLK_EN                (1 << 12)
#define PMU_EFUSE_WRITE2                (1 << 11)
#define PMU_EFUSE_MODE                  (1 << 10)
#define PMU_EFUSE_REF_RES_BIT(n)        (((n) & 3) << 8)
#define PMU_EFUSE_REF_RES_BIT_MASK      (3 << 8)
#define PMU_EFUSE_REF_RES_BIT_SHIFT     (8)
#define PMU_EFUSE_PROG_TIME(n)          (((n) & 0xF) << 4)
#define PMU_EFUSE_PROG_TIME_MASK        (0xF << 4)
#define PMU_EFUSE_PROG_TIME_SHIFT       (4)
#define PMU_EFUSE_WORD_SEL(n)           (((n) & 0xF) << 0)
#define PMU_EFUSE_WORD_SEL_MASK         (0xF << 0)
#define PMU_EFUSE_WORD_SEL_SHIFT        (0)

///C0   PMU_REG_METAL_ID
#define PMU_METAL_ID_SHIFT              0
#define PMU_METAL_ID_MASK               (0xF << PMU_METAL_ID_SHIFT)
#define PMU_METAL_ID(n)                 (((n) & 0xF) << PMU_METAL_ID_SHIFT)

///40   PMU_REG_POWER_OFF
#define PMU_SOFT_POWER_OFF              (1 << 1)

///42   PMU_REG_POWER_KEY_CFG
#define PMU_POWERKEY_WAKEUP_OSC_EN      (1 << 12)
#define PMU_RTC_POWER_ON_EN             (1 << 11)
#define PMU_PU_LPO_DR                   (1 << 5)
#define PMU_PU_LPO_REG                  (1 << 4)

///45   PMU_REG_USB_PIN_CFG
#define PMU_USB_PU_RX_SE                (1 << 13)
#define PMU_USB_POL_RX_DP               (1 << 12)
#define PMU_USB_POL_RX_DM               (1 << 11)
#define PMU_USB_DEBOUNCE_EN             (1 << 10)
#define PMU_USB_NOLS_MODE               (1 << 9)
#define PMU_USB_INSERT_DET_EN           (1 << 8)

///48   ana setting
#define PMU_ANA_LP_MODE_EN    (1<<13)
#define PMU_ANA_DSLEEP_ON          (1<<10)
#define PMU_ANA_VANA_DR          (1<<9)
#define PMU_ANA_VANA_EN          (1<<8)
#define PMU_ANA_DSLEEP_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_ANA_DSLEEP_VOLT_MASK        (0xF << 4)
#define PMU_ANA_DSLEEP_VOLT_SHIFT       (4)
#define PMU_ANA_NORMAL_VOLT(n)           (((n) & 0xF) << 0)
#define PMU_ANA_NORMAL_VOLT_MASK         (0xF << 0)
#define PMU_ANA_NORMAL_VOLT_SHIFT        (0)

///49   dig setting
#define PMU_DIG_LP_MODE_EN    (1<<13)
#define PMU_DIG_DSLEEP_ON          (1<<10)
#define PMU_DIG_VDIG_DR          (1<<9)
#define PMU_DIG_VDIG_EN          (1<<8)
#define PMU_DIG_DSLEEP_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_DIG_DSLEEP_VOLT_MASK        (0xF << 4)
#define PMU_DIG_DSLEEP_VOLT_SHIFT       (4)
#define PMU_DIG_NORMAL_VOLT(n)           (((n) & 0xF) << 0)
#define PMU_DIG_NORMAL_VOLT_MASK         (0xF << 0)
#define PMU_DIG_NORMAL_VOLT_SHIFT        (0)

///4A  IO SETTING
#define PMU_IO_LP_MODE_EN    (1<<15)
#define PMU_IO_DSLEEP_ON          (1<<12)
#define PMU_IO_VIO_DR          (1<<11)
#define PMU_IO_VIO_EN          (1<<10)
#define PMU_IO_NORMAL_VOLT(n)          (((n) & 0x1F) << 5)
#define PMU_IO_NORMAL_VOLT_MASK        (0x1F << 5)
#define PMU_IO_NORMAL_VOLT_SHIFT       (5)
#define PMU_IO_DSLEEP_VOLT(n)           (((n) & 0x1F) << 0)
#define PMU_IO_DSLEEP_VOLT_MASK         (0x1F << 0)
#define PMU_IO_DSLEEP_VOLT_SHIFT        (0)

///4B  VMEM SETTING
#define PMU_MEM_LP_MODE_EN    (1<<15)
#define PMU_MEM_DSLEEP_ON          (1<<12)
#define PMU_MEM_VMEM_DR          (1<<11)
#define PMU_MEM_VMEM_EN          (1<<10)
#define PMU_MEM_NORMAL_VOLT(n)          (((n) & 0x1F) << 5)
#define PMU_MEM_NORMAL_VOLT_MASK        (0x1F << 5)
#define PMU_MEM_NORMAL_VOLT_SHIFT       (5)
#define PMU_MEM_DSLEEP_VOLT(n)           (((n) & 0x1F) << 0)
#define PMU_MEM_DSLEEP_VOLT_MASK         (0x1F << 0)
#define PMU_MEM_DSLEEP_VOLT_SHIFT        (0)

////4C  GP SETTING
#define PMU_GP_LP_MODE_EN    (1<<15)
#define PMU_GP_DSLEEP_ON          (1<<12)
#define PMU_GP_VGP_DR          (1<<11)
#define PMU_GP_VGP_EN          (1<<10)
#define PMU_GP_NORMAL_VOLT(n)          (((n) & 0x1F) << 5)
#define PMU_GP_NORMAL_VOLT_MASK        (0x1F << 5)
#define PMU_GP_NORMAL_VOLT_SHIFT       (5)
#define PMU_GP_DSLEEP_VOLT(n)           (((n) & 0x1F) << 0)
#define PMU_GP_DSLEEP_VOLT_MASK         (0x1F << 0)
#define PMU_GP_DSLEEP_VOLT_SHIFT        (0)

////4D  USB SETTING
#define PMU_USB_LP_MODE_EN    (1<<13)
#define PMU_USB_DSLEEP_ON          (1<<10)
#define PMU_USB_VUSB_DR          (1<<9)
#define PMU_USB_VUSB_EN          (1<<8)
#define PMU_USB_NORMAL_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_USB_NORMAL_VOLT_MASK        (0xF << 4)
#define PMU_USB_NORMAL_VOLT_SHIFT       (4)
#define PMU_USB_DSLEEP_VOLT(n)           (((n) & 0xF) << 0)
#define PMU_USB_DSLEEP_VOLT_MASK         (0xF << 0)
#define PMU_USB_DSLEEP_VOLT_SHIFT        (0)

////4E  CRYSTAL SETTING
#define PMU_CRYSTAL_LP_MODE_EN    (1<<10)
#define PMU_CRYSTAL_DSLEEP_ON          (1<<13)
#define PMU_CRYSTAL_VCRYSTAL_DR          (1<<12)
#define PMU_CRYSTAL_VCRYSTAL_EN          (1<<11)
#define PMU_CRYSTAL_NORMAL_VOLT(n)          (((n) & 0x7) << 4)
#define PMU_CRYSTAL_NORMAL_VOLT_MASK        (0x7 << 4)
#define PMU_CRYSTAL_NORMAL_VOLT_SHIFT       (3)
#define PMU_CRYSTAL_DSLEEP_VOLT(n)           (((n) & 0x7) << 0)
#define PMU_CRYSTAL_DSLEEP_VOLT_MASK         (0x7 << 0)
#define PMU_CRYSTAL_DSLEEP_VOLT_SHIFT        (0)

////4F  FM SETTING
#define PMU_FM_LP_MODE_EN    (1<<10)
#define PMU_FM_DSLEEP_ON          (1<<13)
#define PMU_FM_VFM_DR          (1<<12)
#define PMU_FM_VFM_EN          (1<<11)
#define PMU_FM_NORMAL_VOLT(n)          (((n) & 0x7) << 4)
#define PMU_FM_NORMAL_VOLT_MASK        (0x7 << 4)
#define PMU_FM_NORMAL_VOLT_SHIFT       (3)
#define PMU_FM_DSLEEP_VOLT(n)           (((n) & 0x7) << 0)
#define PMU_FM_DSLEEP_VOLT_MASK         (0x7 << 0)
#define PMU_FM_DSLEEP_VOLT_SHIFT        (0)

////50  PA SETTING
#define PMU_PA_VPA_EN          (1<<4)
#define PMU_PA_NORMAL_VOLT(n)           (((n) & 0x7) << 0)
#define PMU_PA_NORMAL_VOLT_MASK         (0x7 << 0)
#define PMU_PA_NORMAL_VOLT_SHIFT        (0)

////51  CODEC SETTING
#define PMU_CODEC_LP_MODE_EN    (1<<11)
#define PMU_CODEC_DSLEEP_ON          (1<<14)
#define PMU_CODEC_VCODEC_DR          (1<<13)
#define PMU_CODEC_VCODEC_EN          (1<<12)
#define PMU_CODEC_PULLDOWN_VCODEC         (1 << 8)
#define PMU_CODEC_NORMAL_VOLT(n)          (((n) & 0xF) << 4)
#define PMU_CODEC_NORMAL_VOLT_MASK        (0xF << 4)
#define PMU_CODEC_NORMAL_VOLT_SHIFT       (4)
#define PMU_CODEC_DSLEEP_VOLT(n)           (((n) & 0xF) << 0)
#define PMU_CODEC_DSLEEP_VOLT_MASK         (0xF << 0)
#define PMU_CODEC_DSLEEP_VOLT_SHIFT        (0)

////56 BUCK VOLT SETTING
#define PMU_BUCK_VCORE_NORMAL(n)           (((n) & 0xF) << 12)
#define PMU_BUCK_VCORE_NORMAL_MASK         (0xF << 12)
#define PMU_BUCK_VCORE_NORMAL_SHIFT        (12)
#define PMU_BUCK_VCORE_DSLEEP(n)           (((n) & 0xF) << 8)
#define PMU_BUCK_VCORE_DSLEEP_MASK         (0xF << 8)
#define PMU_BUCK_VCORE_DSLEEP_SHIFT        (8)
#define PMU_BUCK_VANA_NORMAL(n)           (((n) & 0xF) << 4)
#define PMU_BUCK_VANA_NORMAL_MASK         (0xF << 4)
#define PMU_BUCK_VANA_NORMAL_SHIFT        (4)
#define PMU_BUCK_VANA_DSLEEP(n)           (((n) & 0xF) << 0)
#define PMU_BUCK_VANA_DSLEEP_MASK         (0xF << 0)
#define PMU_BUCK_VANA_DSLEEP_SHIFT        (0)

////57 DCDC SETTING
#define PMU_BUCK_CC_MODE_EN     (1<<12)
#define PMU_DCDC_ANA_LP_MODE_EN   (1<<11)
#define PMU_DCDC_ANA_DR   (1<<10)
#define PMU_DCDC_ANA_EN   (1<<9)
#define PMU_DCDC_ANA_DSLEEP_ON   (1<<6)

#define PMU_DCDC_DIG_LP_MODE_EN   (1<<5)
#define PMU_DCDC_DIG_DR   (1<<4)
#define PMU_DCDC_DIG_EN   (1<<3)
#define PMU_DCDC_DIG_DSLEEP_ON   (1<<0)

////67  PMU_REG_INT_MASK
#define PMU_INT_MASK_USB_INSERT         (1 << 15)
#define PMU_INT_MASK_RTC1               (1 << 14)
#define PMU_INT_MASK_RTC0               (1 << 13)

////68  PMU_REG_INT_EN
#define PMU_INT_EN_USB_INSERT           (1 << 15)
#define PMU_INT_EN_RTC1                 (1 << 14)
#define PMU_INT_EN_RTC0                 (1 << 13)

////69  PMU_REG_INT_STATUS
#define PMU_INT_STATUS_USB_INSERT       (1 << 15)
#define PMU_INT_STATUS_RTC1             (1 << 14)
#define PMU_INT_STATUS_RTC0             (1 << 13)

////6A  PMU_REG_INT_CLR
#define PMU_INT_CLR_USB_INSERT          (1 << 15)
#define PMU_INT_CLR_RTC1                (1 << 14)
#define PMU_INT_CLR_RTC0                (1 << 13)

////6C  PMU_REG_CHARGER_STATUS
#define PMU_USB_STATUS_RX_DP            (1 << 15)
#define PMU_USB_STATUS_RX_DM            (1 << 14)

////6D SLEEP ENABLE
#define PMU_SLEEP_EN                    (1 << 15)
#define PMU_RTC_PAGE_1                  (1 << 7)
#define PMU_TEST_MODE_SHIFT             0
#define PMU_TEST_MODE_MASK              (7 << PMU_TEST_MODE_SHIFT)
#define PMU_TEST_MODE(n)                (((n) & 7) << PMU_TEST_MODE_SHIFT)

enum PMU_REG_T {
    PMU_REG_METAL_ID = 0xC0, // RF register in fact
    PMU_REG_POWER_OFF = 0x40,
    PMU_REG_POWER_KEY_CFG = 0x42,
    PMU_REG_USB_PIN_CFG = 0x45,
    PMU_REG_MODULE_START  = 0x48,
    PMU_REG_ANA_CFG  = 0x48,
    PMU_REG_DIG_CFG  = 0x49,
    PMU_REG_IO_CFG  = 0x4A,
    PMU_REG_MEM_CFG    = 0x4B,
    PMU_REG_GP_CFG   =0x4C,
    PMU_REG_USB_CFG  = 0x4D,
    PMU_REG_CRYSTAL_CFG  = 0x4E,
    PMU_REG_FM_CFG  = 0x4F,
    PMU_REG_PA_CFG  = 0x50,
    PMU_REG_CODEC_CFG  = 0x51,
    PMU_REG_BUCK_VOLT = 0x56,
    PMU_REG_DCDC_CFG  = 0x57,
    PMU_REG_RTC_DIV_1HZ  = 0x5D,
    PMU_REG_INT_MASK = 0x67,
    PMU_REG_INT_EN = 0x68,
    PMU_REG_INT_STATUS = 0x69,
    PMU_REG_INT_CLR = 0x6A,
    PMU_REG_CHARGER_CFG = 0x6B,
    PMU_REG_CHARGER_STATUS = 0x6C,
    PMU_REG_SLEEP_CFG = 0x6D,
    PMU_REG_EFUSE_SEL = 0x6F,
    PMU_REG_EFUSE_VAL = 0x70,
    PMU_REG_RTC_VAL = 0x71,
    PMU_REG_RTC_LOAD_HIGH = 0x72,
    PMU_REG_RTC_LOAD_LOW = 0x73,
    PMU_REG_RTC_MATCH0_HIGH = 0x74,
    PMU_REG_RTC_MATCH0_LOW = 0x75,
    PMU_REG_GPADC_VAL_CHAN0 = 0x78,
};

enum PMU_MODUAL_T {
    PMU_ANA,
    PMU_DIG,
    PMU_IO,
    PMU_MEM,
    PMU_GP,
    PMU_USB,
    PMU_CRYSTAL,
    PMU_FM,
    PMU_PA,
    PMU_CODEC,
};

struct PMU_MODULE_CFG_T {
    unsigned short manual_bit;
    unsigned short ldo_en;
    unsigned short lp_mode;
    unsigned short dsleep_mode;
    unsigned short dsleep_v;
    unsigned short dsleep_v_shift;
    unsigned short normal_v;
    unsigned short normal_v_shift;
};

#define PMU_DR(MOD)    PMU_##MOD##_V##MOD##_DR
#define PMU_LDO(MOD)    PMU_##MOD##_V##MOD##_EN
#define PMU_LP(MOD)    PMU_##MOD##_LP_MODE_EN
#define PMU_DSLP(MOD)    PMU_##MOD##_DSLEEP_ON
#define PMU_SLPV(MOD)    PMU_##MOD##_DSLEEP_VOLT_MASK
#define PMU_NORV(MOD)    PMU_##MOD##_NORMAL_VOLT_MASK
#define PMU_SLPV_SHFT(MOD)    PMU_##MOD##_DSLEEP_VOLT_SHIFT
#define PMU_NORV_SHFT(MOD)    PMU_##MOD##_NORMAL_VOLT_SHIFT

static const struct PMU_MODULE_CFG_T pmu_module_cfg[] =
{
{PMU_DR(ANA),PMU_LDO(ANA),PMU_LP(ANA),PMU_DSLP(ANA),PMU_SLPV(ANA),PMU_SLPV_SHFT(ANA),PMU_NORV(ANA),PMU_NORV_SHFT(ANA)},
{PMU_DR(DIG),PMU_LDO(DIG),PMU_LP(DIG),PMU_DSLP(DIG),PMU_SLPV(DIG),PMU_SLPV_SHFT(DIG),PMU_NORV(DIG),PMU_NORV_SHFT(DIG)},
{PMU_DR(IO),PMU_LDO(IO),PMU_LP(IO),PMU_DSLP(IO),PMU_SLPV(IO),PMU_SLPV_SHFT(IO),PMU_NORV(IO),PMU_NORV_SHFT(IO)},
{PMU_DR(MEM),PMU_LDO(MEM),PMU_LP(MEM),PMU_DSLP(MEM),PMU_SLPV(MEM),PMU_SLPV_SHFT(MEM),PMU_NORV(MEM),PMU_NORV_SHFT(MEM)},
{PMU_DR(GP),PMU_LDO(GP),PMU_LP(GP),PMU_DSLP(GP),PMU_SLPV(GP),PMU_SLPV_SHFT(GP),PMU_NORV(GP),PMU_NORV_SHFT(GP)},
{PMU_DR(USB),PMU_LDO(USB),PMU_LP(USB),PMU_DSLP(USB),PMU_SLPV(USB),PMU_SLPV_SHFT(USB),PMU_NORV(USB),PMU_NORV_SHFT(USB)},
{PMU_DR(CRYSTAL),PMU_LDO(CRYSTAL),PMU_LP(CRYSTAL),PMU_DSLP(CRYSTAL),PMU_SLPV(CRYSTAL),PMU_SLPV_SHFT(CRYSTAL),PMU_NORV(CRYSTAL),PMU_NORV_SHFT(CRYSTAL)},
{PMU_DR(FM),PMU_LDO(FM),PMU_LP(FM),PMU_DSLP(FM),PMU_SLPV(FM),PMU_SLPV_SHFT(FM),PMU_NORV(FM),PMU_NORV_SHFT(FM)},
{0,PMU_LDO(ANA),0,0,0,0,PMU_NORV(PA),PMU_NORV_SHFT(PA)},
{PMU_DR(CODEC),PMU_LDO(CODEC),PMU_LP(CODEC),PMU_DSLP(CODEC),PMU_SLPV(CODEC),PMU_SLPV_SHFT(CODEC),PMU_NORV(CODEC),PMU_NORV_SHFT(CODEC)},

};

static enum PMU_POWER_MODE_T BOOT_BSS_LOC pmu_power_mode = PMU_POWER_MODE_NONE;
static enum PMU_POWER_MODE_T pmu_active_power_mode = PMU_POWER_MODE_NONE;

static uint8_t pmu_anc_enabled;
static uint8_t pmu_fir_hs_enabled;
static enum PMU_USB_CONFIG_TYPE_T pmu_usb_cfg_type;
static uint8_t pmu_flash_write_enabled;
static uint8_t BOOT_BSS_LOC pmu_flash_freq_high;

static enum PMU_USB_PIN_CHK_STATUS_T usb_pin_status;

static PMU_USB_PIN_CHK_CALLBACK usb_pin_callback;

static PMU_RTC_IRQ_HANDLER_T rtc_irq_handler;

static PMU_CHARGER_IRQ_HANDLER_T charger_irq_handler;

static uint8_t vio_risereq_map;
STATIC_ASSERT(sizeof(vio_risereq_map) * 8 >= PMU_VIORISE_REQ_USER_QTY, "vio_risereq_map size too small");

static uint8_t vio_act_normal = IO_VOLT_ACTIVE_NORMAL;
static uint8_t vio_act_rise = IO_VOLT_ACTIVE_RISE;
static uint8_t vio_lp = IO_VOLT_SLEEP;

static const uint8_t ana_lp_dcdc = PMU_DCDC_ANA_SLEEP_1_6V;

static const uint8_t dig_lp_ldo = PMU_VDIG_0_7V;
static const uint8_t dig_lp_dcdc = PMU_DCDC_DIG_0_8V;

uint32_t BOOT_TEXT_FLASH_LOC read_hw_metal_id(void)
{
    int ret;
    uint16_t val;

    // Set pmu reset
    pmu_read(PMU_REG_POWER_OFF, &val);
    val |= (1 << 0);
    pmu_write(PMU_REG_POWER_OFF, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));
    // Clear pmu reset
    val &= ~(1 << 0);
    pmu_write(PMU_REG_POWER_OFF, val);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    ret = pmu_read(PMU_REG_METAL_ID, &val);
    if (ret) {
        return 0;
    }

    return GET_BITFIELD(val, PMU_METAL_ID);
}

int pmu_get_efuse(enum PMU_EFUSE_PAGE_T page, unsigned short *efuse)
{
    int ret;
    unsigned short val;
    volatile int i;

    val = PMU_EFUSE_CLK_EN;
    ret = pmu_write(PMU_REG_EFUSE_SEL, val);
    if (ret) {
        return ret;
    }

    val = PMU_EFUSE_TRIGGER | PMU_EFUSE_CLK_EN |
          PMU_EFUSE_PROG_TIME(4) | PMU_EFUSE_WORD_SEL(page);
    ret = pmu_write(PMU_REG_EFUSE_SEL, val);
    if (ret) {
        return ret;
    }

    // Delay 10 us
    for (i = 0; i < 100; i++);

    ret = pmu_read(PMU_REG_EFUSE_VAL, efuse);
    if (ret) {
        return ret;
    }

    return 0;
}

static unsigned int pmu_count_zeros(unsigned int val, unsigned int bits)
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
    unsigned short val_pd;
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

    //set pmu reset
    pmu_read(PMU_REG_POWER_OFF,&val_pd);
    val_pd |= (1 << 0);
    pmu_write(PMU_REG_POWER_OFF,val_pd);
    hal_sys_timer_delay(MS_TO_TICKS(1));

    //set pmu power off config
    //reduce popclick next power on
    pmu_read(PMU_REG_DIG_CFG,&val);
    val |= (1<<8)|(1<<9);
    pmu_write(PMU_REG_DIG_CFG,val);

    //set lpo off delay
    pmu_write(0x41,0xAAF1);
    //set reg_ac_on_db_value (0)
    pmu_write(0x6b,0x1F00);

    //clr reset
    val_pd &= ~(1 << 0);
    pmu_write(PMU_REG_POWER_OFF,val_pd);
    hal_sys_timer_delay(MS_TO_TICKS(1));

#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    // Restore RTC after PMU reset
    if (rtc_enabled) {
        pmu_rtc_enable();
        if (rtc_alarm_set) {
            pmu_rtc_set_alarm(rtc_alarm_val);
        }
    }
#endif

    //set sw power down
    val_pd |= (1 << 1);
    pmu_write(PMU_REG_POWER_OFF,val_pd);
    // Set sw power down twice to avoid automatically shutdown during next booting up
    pmu_write(PMU_REG_POWER_OFF,val_pd);
    hal_sys_timer_delay(MS_TO_TICKS(2000));

    int_unlock(lock);
    //can't reach here
    ASSERT(0, "shutdown failed");
    SAFE_PROGRAM_STOP();
}

void pmu_module_config(enum PMU_MODUAL_T module,unsigned short is_manual,unsigned short ldo_on,unsigned short lp_mode,unsigned short dpmode)
{
    int ret;
    unsigned short val;

    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

#ifdef VCODEC_OFF
    if (module == PMU_CODEC) {
        ldo_on = PMU_LDO_OFF;
    }
#endif

    module_address = module+PMU_REG_MODULE_START;
    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
        if(is_manual)
        {
            val |= module_cfg_p->manual_bit;
        }
        else
        {
            val &= ~module_cfg_p->manual_bit;
        }
        if (PMU_FM == module){
            ldo_on = ldo_on > 0 ? PMU_LDO_OFF : PMU_LDO_ON;
        }
        if(ldo_on)
        {
            val |=module_cfg_p->ldo_en;
        }
        else
        {
            val &= ~module_cfg_p->ldo_en;
        }
        if(lp_mode)
        {
            val |=module_cfg_p->lp_mode;
        }
        else
        {
            val &= ~module_cfg_p->lp_mode;
        }
        if(dpmode)
        {
            val |=module_cfg_p->dsleep_mode;
        }
        else
        {
            val &= ~module_cfg_p->dsleep_mode;
        }
        pmu_write(module_address, val);
    }
}

void pmu_module_set_volt(unsigned char module, unsigned short sleep_v,unsigned short normal_v)
{
    int ret;
    unsigned short val;

    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = module+PMU_REG_MODULE_START;
    ret = pmu_read(module_address, &val);
    if(ret == 0)
    {
        val &= ~module_cfg_p->normal_v;

        val |= (normal_v<<module_cfg_p->normal_v_shift);
        val &= ~module_cfg_p->dsleep_v;
        val |= sleep_v<<module_cfg_p->dsleep_v_shift;

        pmu_write(module_address, val);
    }
}

int pmu_module_get_volt(unsigned char module, unsigned short *sleep_vp,unsigned short *normal_vp)
{
    int ret;
    unsigned short val;

    unsigned char module_address;
    const struct PMU_MODULE_CFG_T *module_cfg_p = &pmu_module_cfg[module];

    module_address = module+PMU_REG_MODULE_START;
    ret = pmu_read(module_address, &val);
    if (ret == 0) {
        if (normal_vp) {
            *normal_vp = (val & module_cfg_p->normal_v) >> module_cfg_p->normal_v_shift;
        }
        if (sleep_vp) {
            *sleep_vp = (val & module_cfg_p->dsleep_v) >> module_cfg_p->dsleep_v_shift;
        }
    }

    return ret;
}

void pmu_dcdc_ana_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

#ifdef PMU_DCDC_ANA2_SUPPORT
    ASSERT(((normal_v & PMU_DCDC_ANA2_FLAG) ^ (dsleep_v & PMU_DCDC_ANA2_FLAG)) == 0,
        "ANA normal and sleep volt must both be alternative setting: normal=0x%x sleep=0x%x",
        normal_v, dsleep_v);
    // NOTE: BIT-2 cannot be read back
    pmu_read(PMU_REG_GPADC_VAL_CHAN0, &val);
    if (normal_v & PMU_DCDC_ANA2_FLAG) {
        normal_v &= ~PMU_DCDC_ANA2_FLAG;
        dsleep_v &= ~PMU_DCDC_ANA2_FLAG;
        val = (val & 0xFC00) | (1 << 2);
    } else {
        val = (val & 0xFC00);
    }
    pmu_write(PMU_REG_GPADC_VAL_CHAN0, val);
#endif

    ret = pmu_read(PMU_REG_BUCK_VOLT, &val);
    if(ret == 0)
    {
        val &= ~PMU_BUCK_VANA_DSLEEP_MASK;
        val &= ~PMU_BUCK_VANA_NORMAL_MASK;
        val |= PMU_BUCK_VANA_DSLEEP(dsleep_v);
        val |= PMU_BUCK_VANA_NORMAL(normal_v);
        pmu_write(PMU_REG_BUCK_VOLT, val);
    }
}

static void pmu_ana_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    if (mode == PMU_POWER_MODE_ANA_DCDC || mode == PMU_POWER_MODE_DIG_DCDC) {
#ifdef _BEST1000_QUAL_DCDC_
        if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) {
            pmu_dcdc_ana_set_volt(PMU_DCDC_ANA_1_8V,ana_lp_dcdc);
        } else {
#ifndef __BEST_FLASH_VIA_ANA__
            pmu_dcdc_ana_set_volt(PMU_DCDC_ANA_2_0V,ana_lp_dcdc);
#else
            pmu_dcdc_ana_set_volt(PMU_DCDC_ANA_2_2V,ana_lp_dcdc);
#endif
        }
#else // !_BEST1000_QUAL_DCDC_
        pmu_dcdc_ana_set_volt(PMU_DCDC_ANA_1_8V,ana_lp_dcdc);
#endif // !_BEST1000_QUAL_DCDC_
    }
}

static void pmu_dcdc_dig_set_volt(unsigned short normal_v,unsigned short dsleep_v)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_BUCK_VOLT, &val);
    if(ret == 0)
    {
        val &= ~PMU_BUCK_VCORE_NORMAL_MASK;
        val &= ~PMU_BUCK_VCORE_DSLEEP_MASK;
        val |= PMU_BUCK_VCORE_NORMAL(normal_v);
        val |= PMU_BUCK_VCORE_DSLEEP(dsleep_v);
        pmu_write(PMU_REG_BUCK_VOLT, val);
    }
}

static void pmu_dig_set_volt(int mode_change, enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;

    lock = int_lock();

    if (mode == PMU_POWER_MODE_NONE) {
        mode = pmu_power_mode;
    }

    if (pmu_anc_enabled || pmu_fir_hs_enabled) {
        // ANC/FIR_HS needs 1.3+V vcore
        if (mode_change == 0) {
            pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_1_3V);
            pmu_dcdc_dig_set_volt(PMU_DCDC_DIG_1_3V,dig_lp_dcdc);
        }
    } else if (pmu_usb_cfg_type != PMU_USB_CONFIG_TYPE_NONE || pmu_flash_write_enabled) {
        // USB/FLASH_WRITE needs 1.1+V
        if (mode_change == 0) {
            pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_1_1V);
            pmu_dcdc_dig_set_volt(PMU_DCDC_DIG_1_1V,dig_lp_dcdc);
        }
    } else if (pmu_flash_freq_high) {
        if (mode_change == 0) {
            pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_1_1V);
            pmu_dcdc_dig_set_volt(PMU_DCDC_DIG_1_0V,dig_lp_dcdc);
        }
    } else {
        // Common cases
#ifdef PROGRAMMER
        // Keep the same vcore voltage as ROM (hardware default)
        pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_1_1V);
        pmu_dcdc_dig_set_volt(PMU_DCDC_DIG_1_1V,dig_lp_dcdc);
#else
        if (mode == PMU_POWER_MODE_LDO) {
            pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_1_1V);
        } else if (mode == PMU_POWER_MODE_ANA_DCDC) {
            pmu_module_set_volt(PMU_DIG,dig_lp_ldo,PMU_VDIG_0_9V);
        } else if (mode == PMU_POWER_MODE_DIG_DCDC) {
            pmu_dcdc_dig_set_volt(PMU_DCDC_DIG_0_9V,dig_lp_dcdc);
        }
#endif
    }

    int_unlock(lock);
}

static void pmu_ldo_mode_en(void)
{
    // Enable vana ldo
    pmu_module_config(PMU_ANA,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);
    // Enable vcore ldo
    pmu_module_config(PMU_DIG,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON);

    pmu_ana_set_volt(1, PMU_POWER_MODE_LDO);
    pmu_dig_set_volt(1, PMU_POWER_MODE_LDO);

    pmu_write(PMU_REG_DCDC_CFG, 0);
}

static void pmu_dcdc_ana_mode_en(void)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_DCDC_CFG, &val);
    if(ret == 0)
    {
        val |= PMU_DCDC_ANA_LP_MODE_EN;
        val |= PMU_DCDC_ANA_DR;
        val |= PMU_DCDC_ANA_EN;
        pmu_write(PMU_REG_DCDC_CFG, val);

        pmu_ana_set_volt(1, PMU_POWER_MODE_ANA_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_ANA_DCDC);

        // Disable vana ldo
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

static void pmu_dcdc_dual_mode_en(void)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_DCDC_CFG, &val);
    if(ret == 0)
    {
        ////dcdc mode have no lp mode so just open the dcdc dig and dcdc ana and close the vdig and vana
        val |= PMU_BUCK_CC_MODE_EN;
        val |= PMU_DCDC_DIG_DR;
        val |= PMU_DCDC_DIG_EN;
        val |= PMU_DCDC_ANA_DR;
        val |= PMU_DCDC_ANA_EN;
        pmu_write(PMU_REG_DCDC_CFG, val);

        pmu_ana_set_volt(1, PMU_POWER_MODE_DIG_DCDC);
        pmu_dig_set_volt(1, PMU_POWER_MODE_DIG_DCDC);

        // Disable vana and vcore LDO
        pmu_module_config(PMU_ANA,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
        pmu_module_config(PMU_DIG,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
    }
}

void pmu_mode_change(enum PMU_POWER_MODE_T mode)
{
    uint32_t lock;

    if (pmu_power_mode == mode || mode == PMU_POWER_MODE_NONE)
        return;

    lock = int_lock();

    pmu_ldo_mode_en();

    if(mode == PMU_POWER_MODE_ANA_DCDC)
    {
        pmu_dcdc_ana_mode_en();
    }
    else if(mode == PMU_POWER_MODE_DIG_DCDC)
    {
        pmu_dcdc_dual_mode_en();
    }

    pmu_power_mode = mode;

    int_unlock(lock);
}

void pmu_sleep_en(unsigned char sleep_en)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_SLEEP_CFG, &val);
    if(ret == 0)
    {
        if(sleep_en)
        {
            val |= PMU_SLEEP_EN;
        }
        else
        {
            val &= ~PMU_SLEEP_EN;
        }
        pmu_write(PMU_REG_SLEEP_CFG, val);
    }
}

int BOOT_TEXT_FLASH_LOC pmu_open(void)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))

    int ret;
    uint16_t val;
    enum PMU_POWER_MODE_T mode;

    // Disable all PMU irqs by default
    pmu_write(PMU_REG_INT_MASK, 0);
    pmu_write(PMU_REG_INT_EN, 0);

    //resume dig setting (setting by power shundown)
    pmu_read(PMU_REG_DIG_CFG,&val);
    val &= ~((1<<8)|(1<<9));
    pmu_write(PMU_REG_DIG_CFG,val);

    // Allow PMU to sleep when power key is pressed
    ret = pmu_read(PMU_REG_POWER_KEY_CFG, &val);
    if (ret == 0) {
        val &= ~PMU_POWERKEY_WAKEUP_OSC_EN;
        pmu_write(PMU_REG_POWER_KEY_CFG, val);
    }

#ifndef NO_SLEEP
    pmu_sleep_en(1);
#endif

#ifdef VCRYSTAL_OFF
    pmu_module_config(PMU_CRYSTAL,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable 26m ldo (short with vana)
#else
    pmu_module_config(PMU_CRYSTAL,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable 26m ldo in sleep
#endif

#ifdef VMEM_ON
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VMEM
    pmu_module_set_volt(PMU_MEM, PMU_VMEM_1_8V, PMU_VMEM_1_8V);
#else
    pmu_module_config(PMU_MEM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VMEM
#endif

    pmu_module_config(PMU_GP,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VGP

#if !defined(PROGRAMMER) && !defined(VUSB_ON)
    pmu_module_config(PMU_USB,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VUSB
#endif

    //pmu_module_config(PMU_FM,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_OFF,PMU_DSLEEP_MODE_OFF); ///disable VFM

#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VIO in sleep
#else
    pmu_module_config(PMU_IO,PMU_AUTO_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_ON); //enable VIO in sleep
#endif
    pmu_module_set_volt(PMU_IO, vio_lp, vio_act_normal);

#ifdef LDO_MODE
    mode = PMU_POWER_MODE_LDO;
#elif defined(DIG_DCDC_MODE)
    mode = PMU_POWER_MODE_DIG_DCDC;
#else // defined(ANA_DCDC_MODE)
    mode = PMU_POWER_MODE_ANA_DCDC;
#endif // defined(ANA_DCDC_MODE)

    pmu_mode_change(mode);

#ifdef ANC_APP
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //enable VCODEC
#else
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
#endif

    // Init vcodec
    pmu_module_set_volt(PMU_CODEC, CODEC_VOLT_DEFAULT, CODEC_VOLT_DEFAULT);
    ret = pmu_read(PMU_REG_CODEC_CFG, &val);
    if (ret == 0) {
        val |= PMU_CODEC_PULLDOWN_VCODEC;
        pmu_write(PMU_REG_CODEC_CFG, val);
    }

#endif // PMU_INIT || (!FPGA && !PROGRAMMER)

    return 0;
}

void pmu_sleep(void)
{
    if (pmu_power_mode == PMU_POWER_MODE_DIG_DCDC) {
        pmu_active_power_mode = pmu_power_mode;
        pmu_mode_change(PMU_POWER_MODE_ANA_DCDC);
    }
}

void pmu_wakeup(void)
{
    if (pmu_active_power_mode != PMU_POWER_MODE_NONE) {
        pmu_mode_change(pmu_active_power_mode);
        pmu_active_power_mode = PMU_POWER_MODE_NONE;
    }
}

void pmu_codec_config(int enable)
{
#ifndef _BEST1000_QUAL_DCDC_
    if (enable) {
        pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //enable VCODEC
    } else {
        pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
    }
#endif
}

void pmu_codec_adc_pre_start(void)
{
    //Vcodec ramp up to 2.7v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_7V, PMU_CODEC_2_7V);
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //enable VCODEC
}

void pmu_codec_adc_post_start(void (*delay_ms)(uint32_t))
{
    delay_ms(2);
    //Vcodec ramp down to 2.5v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_5V, PMU_CODEC_2_5V);
    delay_ms(2);
    //Vcodec ramp down to 2.3v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_3V, PMU_CODEC_2_3V);
    delay_ms(2);
    //Vcodec ramp down to 2.2v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_2V, PMU_CODEC_2_2V);
    delay_ms(2);
    //Vcodec close
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
}

void pmu_codec_dac_pre_start(void)
{
    //Vcodec ramp up to 2.7v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_7V, PMU_CODEC_2_7V);
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //enable VCODEC
}

void pmu_codec_dac_post_start(void (*delay_ms)(uint32_t))
{
    //Vcodec ramp down to 2.5v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_5V, PMU_CODEC_2_5V);
    delay_ms(10);
    //Vcodec ramp down to 2.3v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_3V, PMU_CODEC_2_3V);
    delay_ms(10);
    //Vcodec ramp down to 2.2v
    pmu_module_set_volt(PMU_CODEC, PMU_CODEC_2_2V, PMU_CODEC_2_2V);
    delay_ms(5);
    //Vcodec close
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF); //disable VCODEC
}

void pmu_flash_write_config(void)
{
    if (pmu_flash_write_enabled) {
        return;
    }
    pmu_flash_write_enabled = 1;

#if defined(__PMU_VIO_DYNAMIC_CTRL_MODE__)
    pmu_viorise_req(PMU_VIORISE_REQ_USER_FLASH, true);
#endif
#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_ON,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif
    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void pmu_flash_read_config(void)
{
    if (pmu_flash_write_enabled == 0) {
        return;
    }
    pmu_flash_write_enabled = 0;

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#ifdef __BEST_FLASH_VIA_ANA__
    pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_ON,PMU_DSLEEP_MODE_OFF);
#endif
#if defined(__PMU_VIO_DYNAMIC_CTRL_MODE__)
    pmu_viorise_req(PMU_VIORISE_REQ_USER_FLASH, false);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_flash_freq_config(uint32_t freq)
{
#if defined(PMU_INIT) || (!defined(FPGA) && !defined(PROGRAMMER))
    pmu_flash_freq_high = (freq > 52000000);

    if (pmu_power_mode == PMU_POWER_MODE_NONE) {
        // PMU not init yet
        return;
    }

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
#endif
}

void BOOT_TEXT_FLASH_LOC pmu_psram_freq_config(uint32_t freq)
{
}

void pmu_anc_config(int enable)
{
    enable = !!enable;

    if (pmu_anc_enabled == enable) {
        return;
    }

    pmu_anc_enabled = (uint8_t)enable;

#if (CODEC_VOLT_ANC > CODEC_VOLT_DEFAULT)
    if (enable) {
        pmu_module_set_volt(PMU_CODEC, CODEC_VOLT_ANC, CODEC_VOLT_ANC);
    } else {
        pmu_module_set_volt(PMU_CODEC, CODEC_VOLT_DEFAULT, CODEC_VOLT_DEFAULT);
    }
#endif

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void pmu_fir_high_speed_config(int enable)
{
    enable = !!enable;

    if (pmu_fir_hs_enabled == enable) {
        return;
    }

    pmu_fir_hs_enabled = (uint8_t)enable;

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

void BOOT_TEXT_SRAM_LOC pmu_sys_freq_config(enum HAL_CMU_FREQ_T freq)
{
}

void pmu_usb_config(enum PMU_USB_CONFIG_TYPE_T type)
{
    unsigned short ldo_on, deep_sleep_on;

    if (pmu_usb_cfg_type == type) {
        return;
    }

    pmu_usb_cfg_type = type;

    if (type == PMU_USB_CONFIG_TYPE_NONE) {
        ldo_on = PMU_LDO_OFF;
        deep_sleep_on = PMU_DSLEEP_MODE_OFF;
    } else {
        // Only for metal 0
        //pmu_write(0x45, 0x645A);

        // REG 0x46:
        // bit 15 - usb_rpd_dn_en: usb dn pull down resistor enable
        // bit 14 - usb_rpd_dp_en: usb dp pull down resistor enable
        // bit 13 - usb_rpdwk_dn_en: usb dn weak pull down enable
        // bit 12 - usb_rpdwk_dp_en: usb dp weak pull down enable
        // bit 11 - usb_rpuwk_dn_en: usb dn weak pull up enable
        // bit 10 - usb_rpuwk_dp_en: usb dp weak pull up enable
        // bit [9:5] - usb_pd_res: usb pull down resistor selection (ohm is larger when value is increased)
        // bit [4:0] - usb_pu_res: usb pull up resistor selection (ohm is larger when value is increased)
        if (type == PMU_USB_CONFIG_TYPE_DEVICE) {
            pmu_write(0x46, 0x01EF); // decrease pull-up resistor
        } else if (type == PMU_USB_CONFIG_TYPE_HOST) {
            pmu_write(0x46, 0xC1EF); // decrease pull-up resistor
        }

        //pmu_write(0x47, 0x1A61); // default usb tx io drive (2)
        //pmu_write(0x47, 0x1661); // reduce usb tx io drive (1)
        //pmu_write(0x47, 0x1E61); // increase usb tx io drive (3)

        //pmu_write(0x4D, 0x29C9); // increase ldo voltage for USB host

        ldo_on = PMU_LDO_ON;
        deep_sleep_on = PMU_DSLEEP_MODE_ON;
    }

    pmu_module_config(PMU_USB, PMU_MANUAL_MODE, ldo_on, PMU_LP_MODE_ON, deep_sleep_on);

    pmu_dig_set_volt(0, PMU_POWER_MODE_NONE);
}

static int pmu_usb_check_pin_status(enum PMU_USB_PIN_CHK_STATUS_T status)
{
    int dp, dm;

    pmu_usb_get_pin_status(&dp, &dm);

    //TRACE("[%X] %s: status=%d dp=%d dm=%d", hal_sys_timer_get(), __FUNCTION__, status, dp, dm);

    // HOST_RESUME: (resume) dp == 0 && dm == 1, (reset) dp == 0 && dm == 0

    if ( (status == PMU_USB_PIN_CHK_DEV_CONN && (dp == 1 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_DEV_DISCONN && (dp == 0 && dm == 0)) ||
            (status == PMU_USB_PIN_CHK_HOST_RESUME && dp == 0) ) {
        return 1;
    }

    return 0;
}

static void pmu_usb_pin_irq_handler(void)
{
    //TRACE("[%X] %s", hal_sys_timer_get(), __FUNCTION__);

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);

    if (usb_pin_callback) {
        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
        }
    }
}

int pmu_usb_config_pin_status_check(enum PMU_USB_PIN_CHK_STATUS_T status, PMU_USB_PIN_CHK_CALLBACK callback, int enable)
{
    uint16_t val;
    uint32_t lock;

    //TRACE("[%X] %s: status=%d", hal_sys_timer_get(), __FUNCTION__, status);

    if (status >= PMU_USB_PIN_CHK_STATUS_QTY) {
        return 1;
    }

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    usb_pin_status = status;
    usb_pin_callback = callback;

    // Mask the irq
    pmu_read(PMU_REG_INT_MASK, &val);
    val &= ~PMU_INT_MASK_USB_INSERT;
    pmu_write(PMU_REG_INT_MASK, val);

    // Config pin check
    pmu_read(PMU_REG_USB_PIN_CFG, &val);
    val &= ~(PMU_USB_POL_RX_DP | PMU_USB_POL_RX_DM);
    val |= PMU_USB_DEBOUNCE_EN | PMU_USB_NOLS_MODE | PMU_USB_INSERT_DET_EN;
    if (status == PMU_USB_PIN_CHK_DEV_CONN) {
        // Check dp 0->1, dm x->0
    } else if (status == PMU_USB_PIN_CHK_DEV_DISCONN) {
        // Check dp 1->0, dm x->0
        val |= PMU_USB_POL_RX_DP;
    } else if (status == PMU_USB_PIN_CHK_HOST_RESUME) {
        // Check dp 1->0, dm 0->1 (resume) or dm 0->0 (reset)
        val |= PMU_USB_POL_RX_DP;
    }
    pmu_write(PMU_REG_USB_PIN_CFG, val);

    if (status != PMU_USB_PIN_CHK_NONE && callback) {
        pmu_read(PMU_REG_INT_EN, &val);
        val |= PMU_INT_EN_USB_INSERT;
        pmu_write(PMU_REG_INT_EN, val);

        pmu_read(PMU_REG_INT_MASK, &val);
        val |= PMU_INT_MASK_USB_INSERT;
        pmu_write(PMU_REG_INT_MASK, val);

        pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);
    }

    int_unlock(lock);

    if (enable) {
        // Wait at least 10 cycles of 32K clock for the new status when signal checking polarity is changed
        hal_sys_timer_delay(5);
        pmu_usb_enable_pin_status_check();
    }

    return 0;
}

void pmu_usb_enable_pin_status_check(void)
{
    if (usb_pin_status != PMU_USB_PIN_CHK_NONE && usb_pin_callback) {
        pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_USB_INSERT);
        NVIC_ClearPendingIRQ(USB_PIN_IRQn);

        if (pmu_usb_check_pin_status(usb_pin_status)) {
            pmu_usb_disable_pin_status_check();
            usb_pin_callback(usb_pin_status);
            return;
        }

        NVIC_SetVector(USB_PIN_IRQn, (uint32_t)pmu_usb_pin_irq_handler);
        NVIC_SetPriority(USB_PIN_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_EnableIRQ(USB_PIN_IRQn);
    }
}

void pmu_usb_disable_pin_status_check(void)
{
    uint16_t val;
    uint32_t lock;

    NVIC_DisableIRQ(USB_PIN_IRQn);

    lock = int_lock();

    pmu_read(PMU_REG_INT_EN, &val);
    val &= ~PMU_INT_EN_USB_INSERT;
    pmu_write(PMU_REG_INT_EN, val);

    pmu_read(PMU_REG_USB_PIN_CFG, &val);
    val &= ~PMU_USB_INSERT_DET_EN;
    pmu_write(PMU_REG_USB_PIN_CFG, val);

    int_unlock(lock);
}

void pmu_usb_get_pin_status(int *dp, int *dm)
{
    uint16_t pol, val;

    pmu_read(PMU_REG_USB_PIN_CFG, &pol);
    pmu_read(PMU_REG_CHARGER_STATUS, &val);

    *dp = (!(pol & PMU_USB_POL_RX_DP)) ^ (!(val & PMU_USB_STATUS_RX_DP));
    *dm = (!(pol & PMU_USB_POL_RX_DM)) ^ (!(val & PMU_USB_STATUS_RX_DM));
}

void pmu_charger_init(void)
{
    unsigned short readval_cfg;
    uint32_t lock;

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg &= ~((1<<8)|(1<<9)|(1<<10)|(1<<11)|(1<<12));
    pmu_write(PMU_REG_CHARGER_CFG, readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));

    lock = int_lock();
    pmu_read(PMU_REG_CHARGER_CFG, &readval_cfg);
    readval_cfg |= (1<<8)|(1<<9)|(1<<10);
    readval_cfg &= ~0xFF;
    pmu_write(PMU_REG_CHARGER_CFG, readval_cfg);
    int_unlock(lock);

    hal_sys_timer_delay(MS_TO_TICKS(1));
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

    if ((readval & ((1<<2)|(1<<3))) == 0){
        TRACE("%s SKIP", __func__);
        return;
    } else if ((readval & ((1<<2)|(1<<3))) == ((1<<2)|(1<<3))) {
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
    val |= (1<<11)|(1<<12);
    pmu_write(PMU_REG_CHARGER_CFG, val);
    int_unlock(lock);

    if (handler) {
        NVIC_SetVector(CHARGER_IRQn, (uint32_t)pmu_charger_irq_handler);
        NVIC_SetPriority(CHARGER_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(CHARGER_IRQn);
        NVIC_EnableIRQ(CHARGER_IRQn);
    } else {
        NVIC_DisableIRQ(CHARGER_IRQn);
    }
}

void pmu_charger_plugin_config(void)
{
    if (IO_VOLT_ACTIVE_RISE < 0x13) {
        vio_act_rise = 0x13;
    }
    if (IO_VOLT_SLEEP < 0x14) {
        vio_lp = 0x14;
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

#ifdef SUPPORT_OLD_CHIPS
    if(hal_get_chip_metal_id() < HAL_CHIP_METAL_ID_2)
    {
        // Disable and then re-enable charger int
        // In fact charger status cannot be read back, unless charger signal
        // is changed while charger int is enabled.
        pmu_read(PMU_REG_CHARGER_CFG, &readval);
        readval &= ~(1<<8);
        pmu_write(PMU_REG_CHARGER_CFG ,readval);
        hal_sys_timer_delay(2);
        readval |= (1<<8)|(1<<9)|(1<<10);
        pmu_write(PMU_REG_CHARGER_CFG ,readval);
        hal_sys_timer_delay(2);
        pmu_read(PMU_REG_CHARGER_STATUS, &readval);
        pmu_write(PMU_REG_CHARGER_STATUS ,readval);

        if (readval & (1<<0)) {
            return PMU_CHARGER_PLUGIN;
        }

        return PMU_CHARGER_PLUGOUT;
    }
    else
#endif
    {
        enum PMU_CHARGER_STATUS_T status = PMU_CHARGER_UNKNOWN;

        pmu_read(0x65, &readval);
        if (readval&(1<<10))
            status = PMU_CHARGER_PLUGIN;
        else
            status = PMU_CHARGER_PLUGOUT;

        TRACE("%s 0x65=0x%04x val:%d status:%d", __func__, readval, readval&(1<<10)?1:0,status);

        return status;
    }
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
    readval |= PMU_RTC_POWER_ON_EN | PMU_PU_LPO_DR | PMU_PU_LPO_REG;
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
    readval &= ~(PMU_RTC_POWER_ON_EN | PMU_PU_LPO_DR);
    pmu_write(PMU_REG_POWER_KEY_CFG, readval);
    int_unlock(lock);
}

int pmu_rtc_enabled(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_POWER_KEY_CFG, &readval);

    return !!(readval & PMU_RTC_POWER_ON_EN);
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
    uint16_t val;
    uint16_t high, low, high2;
    uint32_t lock;

    lock = int_lock();

    pmu_read(PMU_REG_SLEEP_CFG, &val);
    val |= PMU_RTC_PAGE_1;
    pmu_write(PMU_REG_SLEEP_CFG, val);
    pmu_read(PMU_REG_RTC_VAL, &high);

    val &= ~PMU_RTC_PAGE_1;
    pmu_write(PMU_REG_SLEEP_CFG, val);
    pmu_read(PMU_REG_RTC_VAL, &low);

    // Handle counter wrap
    val |= PMU_RTC_PAGE_1;
    pmu_write(PMU_REG_SLEEP_CFG, val);
    pmu_read(PMU_REG_RTC_VAL, &high2);
    if (high != high2) {
        high = high2;
        val &= ~PMU_RTC_PAGE_1;
        pmu_write(PMU_REG_SLEEP_CFG, val);
        pmu_read(PMU_REG_RTC_VAL, &low);
    }

    int_unlock(lock);

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

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_RTC0);

    pmu_write(PMU_REG_RTC_MATCH0_LOW, low);
    pmu_write(PMU_REG_RTC_MATCH0_HIGH, high);

    lock = int_lock();
    pmu_read(PMU_REG_INT_EN, &readval);
    readval |= PMU_INT_EN_RTC0;
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
    readval &= ~PMU_INT_EN_RTC0;
    pmu_write(PMU_REG_INT_EN, readval);
    int_unlock(lock);

    pmu_write(PMU_REG_INT_CLR, PMU_INT_CLR_RTC0);
}

int pmu_rtc_alarm_status_set(void)
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_EN, &readval);

    return !!(readval & PMU_INT_EN_RTC0);
}

int pmu_rtc_alarm_alerted()
{
    uint16_t readval;

    pmu_read(PMU_REG_INT_STATUS, &readval);

    return !!(readval & PMU_INT_STATUS_RTC0);
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
        readval |= PMU_INT_MASK_RTC0;
    } else {
        readval &= ~PMU_INT_MASK_RTC0;
    }
    pmu_write(PMU_REG_INT_MASK, readval);
    int_unlock(lock);

    if (handler) {
        NVIC_SetVector(RTC_IRQn, (uint32_t)pmu_rtc_irq_handler);
        NVIC_SetPriority(RTC_IRQn, IRQ_PRIORITY_NORMAL);
        NVIC_ClearPendingIRQ(RTC_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    } else {
        NVIC_DisableIRQ(RTC_IRQn);
    }
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

void pmu_fm_config(int enable)
{
    uint16_t ldo;

    if (enable) {
        ldo = PMU_LDO_ON;
    } else {
        ldo = PMU_LDO_OFF;
    }
    pmu_module_config(PMU_FM,PMU_MANUAL_MODE,ldo,PMU_LP_MODE_ON,ldo);
}

void pmu_jtag_config(void)
{
    int ret;
    unsigned short val;

    ret = pmu_read(PMU_REG_SLEEP_CFG, &val);
    if (ret == 0) {
        val = (val & ~PMU_TEST_MODE_MASK) | PMU_TEST_MODE(1);
        pmu_write(PMU_REG_SLEEP_CFG, val);
    }
}
void hal_iomux_set_jtag(void) __attribute__((alias("pmu_jtag_config")));

int pmu_debug_config_ana(uint16_t volt)
{
    if (volt < PMU_DCDC_ANA_0_6V || volt > PMU_DCDC_ANA_2_3V) {
        return 1;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
    return 0;
}

int pmu_debug_config_codec(uint16_t volt)
{
    if (volt == 0xFF) {
        pmu_module_config(PMU_CODEC,PMU_MANUAL_MODE,PMU_LDO_OFF,PMU_LP_MODE_OFF,PMU_DSLEEP_MODE_OFF);
    } else {
        if (volt > PMU_CODEC_3_5V) {
            return 1;
        }
        pmu_module_set_volt(PMU_CODEC, volt, volt);
    }
    return 0;
}

int pmu_debug_config_vcrystal(bool on)
{
    return 0;
}

void pmu_debug_reliability_test(int stage)
{
    uint16_t volt;

    if (stage == 0) {
        volt = PMU_DCDC_ANA_1_8V;
    } else {
        volt = PMU_DCDC_ANA_1_9V;
    }
    pmu_dcdc_ana_set_volt(volt, ana_lp_dcdc);
}

