#include "stddef.h"
#include "cmsis_nvic.h"
#include "hal_gpadc.h"
#include "hal_trace.h"
#include "hal_analogif.h"
#include "pmu.h"

#define HAL_GPADC_TRACE(s,...) //hal_trace_printf(s, ##__VA_ARGS__)

#define VBAT_DIV_ALWAYS_ON

#define gpadc_reg_read(reg,val)  hal_analogif_reg_read(reg,val)
#define gpadc_reg_write(reg,val) hal_analogif_reg_write(reg,val)

// Battery voltage = gpadc voltage * 4
// adc rate 0~2v(10bit)
// Battery_voltage:Adc_rate = 4:1
#define HAL_GPADC_MVOLT_A   800
#define HAL_GPADC_MVOLT_B   1050
#define HAL_GPADC_CALIB_DEFAULT_A   428
#define HAL_GPADC_CALIB_DEFAULT_B   565

#if 0
#elif defined(CHIP_BEST3001) || defined(CHIP_BEST2000) || defined(CHIP_BEST2300)

enum GPADC_REG_T {
    GPADC_REG_VBAT_EN = 0x02,
    GPADC_REG_INTVL_EN = 0x1F,
    GPADC_REG_INTVL_VAL = 0x23,
    GPADC_REG_START = 0x4F,
    GPADC_REG_CH_EN = 0x24,
    GPADC_REG_INT_MASK = 0x26,
    GPADC_REG_INT_EN = 0x27,
#ifdef CHIP_BEST2300
    GPADC_REG_INT_RAW_STS = 0x52,
    GPADC_REG_INT_CLR = 0x53,
#else
    GPADC_REG_INT_RAW_STS = 0x50,
    GPADC_REG_INT_CLR = 0x51,
#endif
    GPADC_REG_CH0_DATA = 0x56,
};

// 02   GPADC_REG_VBAT_EN
#define REG_PU_VBAT_DIV                     (1 << 15)

// 1F   GPADC_REG_INTVL_EN
#define GPADC_INTERVAL_MODE                 (1 << 12)

// 4F   GPADC_REG_START
#define GPADC_START                         (1 << 5)
#define KEY_START                           (1 << 4)

// 24   GPADC_REG_CH_EN
#define CHAN_EN_REG_SHIFT                   0
#define CHAN_EN_REG_MASK                    (0xFF << CHAN_EN_REG_SHIFT)
#define CHAN_EN_REG(n)                      BITFIELD_VAL(CHAN_EN_REG, n)

// 26   GPADC_REG_INT_MASK
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)

// 27   GPADC_REG_INT_EN
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// 50   GPADC_REG_INT_RAW_STS
// 52   GPADC_REG_INT_RAW_STS
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)

// 51   GPADC_REG_INT_CLR
// 53   GPADC_REG_INT_CLR
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)

// 56   GPADC_REG_CH0_DATA
#define DATA_CHAN0_SHIFT                    0
#define DATA_CHAN0_MASK                     (0x3FF << DATA_CHAN0_SHIFT)
#define DATA_CHAN0(n)                       BITFIELD_VAL(DATA_CHAN0, n)

#elif defined(CHIP_BEST1000)

enum GPADC_REG_T {
    GPADC_REG_VBAT_EN = 0x45,
    GPADC_REG_INTVL_EN = 0x60,
    GPADC_REG_INTVL_VAL = 0x64,
    GPADC_REG_START = 0x65,
    GPADC_REG_CH_EN = 0x65,
    GPADC_REG_INT_MASK = 0x67,
    GPADC_REG_INT_EN = 0x68,
    GPADC_REG_INT_RAW_STS = 0x69,
    GPADC_REG_INT_CLR = 0x6A,
    GPADC_REG_CH0_DATA = 0x78,
};

// 45   GPADC_REG_VBAT_EN
#define REG_PU_VBAT_DIV                     (1 << 0)

// 60   GPADC_REG_INTVL_EN
#define GPADC_INTERVAL_MODE                 (1 << 12)

// 65   GPADC_REG_START
#define KEY_START                           (1 << 9)
#define GPADC_START                         (1 << 8)

// 65   GPADC_REG_CH_EN
#define CHAN_EN_REG_SHIFT                   0
#define CHAN_EN_REG_MASK                    (0xFF << CHAN_EN_REG_SHIFT)
#define CHAN_EN_REG(n)                      BITFIELD_VAL(CHAN_EN_REG, n)

// 67   GPADC_REG_INT_MASK
#define KEY_ERR1_INTR_MSK                   (1 << 12)
#define KEY_ERR0_INTR_MSK                   (1 << 11)
#define KEY_PRESS_INTR_MSK                  (1 << 10)
#define KEY_RELEASE_INTR_MSK                (1 << 9)
#define SAMPLE_DONE_INTR_MSK                (1 << 8)
#define CHAN_DATA_INTR_MSK_SHIFT            0
#define CHAN_DATA_INTR_MSK_MASK             (0xFF << CHAN_DATA_INTR_MSK_SHIFT)
#define CHAN_DATA_INTR_MSK(n)               BITFIELD_VAL(CHAN_DATA_INTR_MSK, n)

// 68   GPADC_REG_INT_EN
#define KEY_ERR1_INTR_EN                    (1 << 12)
#define KEY_ERR0_INTR_EN                    (1 << 11)
#define KEY_PRESS_INTR_EN                   (1 << 10)
#define KEY_RELEASE_INTR_EN                 (1 << 9)
#define SAMPLE_DONE_INTR_EN                 (1 << 8)
#define CHAN_DATA_INTR_EN_SHIFT             0
#define CHAN_DATA_INTR_EN_MASK              (0xFF << CHAN_DATA_INTR_EN_SHIFT)
#define CHAN_DATA_INTR_EN(n)                BITFIELD_VAL(CHAN_DATA_INTR_EN, n)

// 69   GPADC_REG_INT_RAW_STS
#define KEY_ERR1_INTR                       (1 << 12)
#define KEY_ERR0_INTR                       (1 << 11)
#define KEY_PRESS_INTR                      (1 << 10)
#define KEY_RELEASE_INTR                    (1 << 9)
#define SAMPLE_PERIOD_DONE_INTR             (1 << 8)
#define CHAN_DATA_VALID_INTR_SHIFT          0
#define CHAN_DATA_VALID_INTR_MASK           (0xFF << CHAN_DATA_VALID_INTR_SHIFT)
#define CHAN_DATA_VALID_INTR(n)             BITFIELD_VAL(CHAN_DATA_VALID_INTR, n)

// 6A   GPADC_REG_INT_CLR
#define KEY_ERR1_INTR_MSKED                 (1 << 12)
#define KEY_ERR0_INTR_MSKED                 (1 << 11)
#define KEY_PRESS_INTR_MSKED                (1 << 10)
#define KEY_RELEASE_INTR_MSKED              (1 << 9)
#define SAMPLE_DONE_INTR_MSKED              (1 << 8)
#define CHAN_DATA_INTR_MSKED_SHIFT          0
#define CHAN_DATA_INTR_MSKED_MASK           (0xFF << CHAN_DATA_INTR_MSKED_SHIFT)
#define CHAN_DATA_INTR_MSKED(n)             BITFIELD_VAL(CHAN_DATA_INTR_MSKED, n)

// 78   GPADC_REG_CH0_DATA
#define DATA_CHAN0_SHIFT                    0
#define DATA_CHAN0_MASK                     (0x3FF << DATA_CHAN0_SHIFT)
#define DATA_CHAN0(n)                       BITFIELD_VAL(DATA_CHAN0, n)

#else
#error "Please update GPADC register definitions"
#endif

static int32_t g_adcSlope = 0;
static int32_t g_adcIntcpt = 0;
static bool gpadc_enabled = false;
static bool adckey_enabled = false;
static bool irq_enabled = false;
static bool g_adcCalibrated = false;
static HAL_GPADC_EVENT_CB_T gpadc_event_cb[HAL_GPADC_CHAN_QTY];
static enum HAL_GPADC_ATP_T gpadc_atp[HAL_GPADC_CHAN_QTY];

static enum HAL_GPADC_ATP_T hal_gpadc_get_min_atp(void)
{
    enum HAL_GPADC_CHAN_T ch;
    enum HAL_GPADC_ATP_T atp = HAL_GPADC_ATP_NULL;

    for (ch = HAL_GPADC_CHAN_0; ch < HAL_GPADC_CHAN_QTY; ch++) {
        if (gpadc_atp[ch] != HAL_GPADC_ATP_NULL) {
            if (atp == HAL_GPADC_ATP_NULL ||
                    (uint32_t)gpadc_atp[ch] < (uint32_t)atp) {
                atp = gpadc_atp[ch];
            }
        }
    }

    return atp;
}

static void hal_gpadc_update_atp(void)
{
    enum HAL_GPADC_ATP_T atp;
    uint16_t val;

    atp = hal_gpadc_get_min_atp();

    if (atp == HAL_GPADC_ATP_NULL || atp == HAL_GPADC_ATP_ONESHOT) {
        gpadc_reg_read(GPADC_REG_INTVL_EN, &val);
        val &= ~GPADC_INTERVAL_MODE;
        gpadc_reg_write(GPADC_REG_INTVL_EN, val);
    } else {
        gpadc_reg_read(GPADC_REG_INTVL_EN, &val);
        val |= GPADC_INTERVAL_MODE;
        gpadc_reg_write(GPADC_REG_INTVL_EN, val);
        val = atp * 1000 / 1024;
        gpadc_reg_write(GPADC_REG_INTVL_VAL, val);
    }
}

static int hal_gpadc_adc2volt_calib(void)
{
    int32_t y1, y2, x1, x2;
    unsigned short efuse;

    if (!g_adcCalibrated)
    {
        y1 = HAL_GPADC_MVOLT_A*1000;
        y2 = HAL_GPADC_MVOLT_B*1000;

        efuse = 0;
        pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_LV, &efuse);

        x1 = efuse > 0 ? efuse : HAL_GPADC_CALIB_DEFAULT_A;

        efuse = 0;
        pmu_get_efuse(PMU_EFUSE_PAGE_BATTER_HV, &efuse);
        x2 = efuse > 0 ? efuse : HAL_GPADC_CALIB_DEFAULT_B;

        g_adcSlope = (y2-y1)/(x2-x1);
        g_adcIntcpt = ((y1*x2)-(x1*y2))/((x2-x1)*1000);
        g_adcCalibrated = true;

        TRACE("%s LV=%d, HV=%d, Slope:%d Intcpt:%d",__func__, x1, x2, g_adcSlope, g_adcIntcpt);
    }

    return 0;
}

static HAL_GPADC_MV_T hal_gpadc_adc2volt(uint16_t gpadcVal)
{
    int32_t voltage;

    hal_gpadc_adc2volt_calib();
    if (gpadcVal == HAL_GPADC_BAD_VALUE)
    {
        // Bad values from the GPADC are still Bad Values
        // for the voltage-speaking user.
        return HAL_GPADC_BAD_VALUE;
    }
    else
    {
        voltage = (((g_adcSlope*gpadcVal)/1000) + (g_adcIntcpt));

        return (voltage < 0) ? 0 : voltage;
    }
}

static void hal_gpadc_irq_handler(void)
{
    uint32_t lock;
    enum HAL_GPADC_CHAN_T ch;
    unsigned short read_val;
    unsigned short reg_int_clr;
    HAL_GPADC_MV_T adc_val;

    gpadc_reg_read(GPADC_REG_INT_CLR, &reg_int_clr);
    gpadc_reg_write(GPADC_REG_INT_CLR, reg_int_clr);

    if (reg_int_clr & CHAN_DATA_INTR_MSKED((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6))) {
        for (ch = HAL_GPADC_CHAN_0; ch < HAL_GPADC_CHAN_QTY; ch++) {
            if (reg_int_clr & CHAN_DATA_INTR_MSKED(1<<ch)) {
                switch (ch) {
                case HAL_GPADC_CHAN_BATTERY:
                case HAL_GPADC_CHAN_0:
                case HAL_GPADC_CHAN_2:
                case HAL_GPADC_CHAN_3:
                case HAL_GPADC_CHAN_4:
                case HAL_GPADC_CHAN_5:
                case HAL_GPADC_CHAN_6:
                    gpadc_reg_read(GPADC_REG_CH0_DATA + ch, &read_val);
                    read_val = GET_BITFIELD(read_val, DATA_CHAN0);
                    adc_val = hal_gpadc_adc2volt(read_val);
                    if (gpadc_event_cb[ch]) {
                        gpadc_event_cb[ch](read_val, adc_val);
                    }
                    if (gpadc_atp[ch] == HAL_GPADC_ATP_NULL || gpadc_atp[ch] == HAL_GPADC_ATP_ONESHOT) {
                        lock = int_lock();

#ifndef VBAT_DIV_ALWAYS_ON
                        if (ch == HAL_GPADC_CHAN_BATTERY) {
                            gpadc_reg_read(GPADC_REG_VBAT_EN, &read_val);
                            read_val &= ~REG_PU_VBAT_DIV;
                            gpadc_reg_write(GPADC_REG_VBAT_EN, read_val);
                        }
#endif

                        // Int mask
                        gpadc_reg_read(GPADC_REG_INT_MASK, &read_val);
                        read_val &= ~CHAN_DATA_INTR_MSK(1<<ch);
                        gpadc_reg_write(GPADC_REG_INT_MASK, read_val);

                        // Int enable
                        gpadc_reg_read(GPADC_REG_INT_EN, &read_val);
                        read_val &= ~CHAN_DATA_INTR_EN(1<<ch);
                        gpadc_reg_write(GPADC_REG_INT_EN, read_val);

                        // Channel enable
                        gpadc_reg_read(GPADC_REG_CH_EN, &read_val);
                        read_val &= ~CHAN_EN_REG(1<<ch);
                        gpadc_reg_write(GPADC_REG_CH_EN, read_val);

                        int_unlock(lock);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Disable GPADC (GPADC_START will be cleared automatically unless in interval mode)
    lock = int_lock();
    gpadc_reg_read(GPADC_REG_CH_EN, &read_val);
    if ((read_val & CHAN_EN_REG((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6))) == 0) {
        gpadc_reg_read(GPADC_REG_START, &read_val);
        read_val &= ~GPADC_START;
        gpadc_reg_write(GPADC_REG_START, read_val);
    }
    int_unlock(lock);

    if (reg_int_clr & (CHAN_DATA_INTR_MSKED(1<<7)|KEY_RELEASE_INTR_MSKED|KEY_PRESS_INTR_MSKED|KEY_ERR0_INTR_MSKED|KEY_ERR1_INTR_MSKED)) {
        if (gpadc_event_cb[HAL_GPADC_CHAN_ADCKEY]) {
            if (reg_int_clr & CHAN_DATA_INTR_MSKED(1<<7)) {
                lock = int_lock();

                // Int mask
                gpadc_reg_read(GPADC_REG_INT_MASK, &read_val);
                read_val &= ~CHAN_DATA_INTR_MSK(1<<7);
                gpadc_reg_write(GPADC_REG_INT_MASK, read_val);

                // Int enable
                gpadc_reg_read(GPADC_REG_INT_EN, &read_val);
                read_val &= ~CHAN_DATA_INTR_EN(1<<7);
                gpadc_reg_write(GPADC_REG_INT_EN, read_val);

                int_unlock(lock);

                gpadc_reg_read(GPADC_REG_CH0_DATA + HAL_GPADC_CHAN_ADCKEY, &adc_val);
                adc_val = GET_BITFIELD(adc_val, DATA_CHAN0);
            } else {
                adc_val = HAL_GPADC_BAD_VALUE;
            }
            // No voltage conversion
            gpadc_event_cb[HAL_GPADC_CHAN_ADCKEY](reg_int_clr, adc_val);
        }
    }
}

bool hal_gpadc_get_volt(enum HAL_GPADC_CHAN_T ch, HAL_GPADC_MV_T *volt)
{
    bool res = false;
    unsigned short read_val;

    if (ch >= HAL_GPADC_CHAN_QTY) {
        return res;
    }

    gpadc_reg_read(GPADC_REG_INT_RAW_STS, &read_val);

    if (read_val & CHAN_DATA_VALID_INTR(1<<ch)) {
        // Clear the channel valid status
        gpadc_reg_write(GPADC_REG_INT_CLR, CHAN_DATA_INTR_MSKED(1<<ch));

        gpadc_reg_read(GPADC_REG_CH0_DATA + ch, &read_val);
        read_val = GET_BITFIELD(read_val, DATA_CHAN0);
        *volt = hal_gpadc_adc2volt(read_val);
        res = true;
    }

    return res;
}

static void hal_gpadc_irq_control(void)
{
    if (gpadc_enabled || adckey_enabled) {
        if (!irq_enabled) {
            irq_enabled = true;
#ifdef CHIP_HAS_EXT_PMU
            pmu_set_irq_handler(PMU_IRQ_TYPE_GPADC, hal_gpadc_irq_handler);
#else
            NVIC_SetVector(GPADC_IRQn, (uint32_t)hal_gpadc_irq_handler);
            NVIC_SetPriority(GPADC_IRQn, IRQ_PRIORITY_NORMAL);
            NVIC_ClearPendingIRQ(GPADC_IRQn);
            NVIC_EnableIRQ(GPADC_IRQn);
#endif
        }
    } else {
        if (irq_enabled) {
            irq_enabled = false;
#ifdef CHIP_HAS_EXT_PMU
            pmu_set_irq_handler(PMU_IRQ_TYPE_GPADC, NULL);
#else
            NVIC_DisableIRQ(GPADC_IRQn);
#endif
        }
    }
}

int hal_gpadc_open(enum HAL_GPADC_CHAN_T channel, enum HAL_GPADC_ATP_T atp, HAL_GPADC_EVENT_CB_T cb)
{
    uint32_t lock;
    unsigned short val;
    unsigned short reg_start_mask;

    if (channel >= HAL_GPADC_CHAN_QTY) {
        return -1;
    }

    // NOTE: ADCKEY callback is not set here, but in hal_adckey_set_irq_handler()
    if (channel != HAL_GPADC_CHAN_ADCKEY) {
        gpadc_event_cb[channel] = cb;
        gpadc_atp[channel] = atp;
    }

    switch (channel) {
        case HAL_GPADC_CHAN_BATTERY:
            // Enable vbat measurement clock
            // vbat div enable
            lock = int_lock();
            gpadc_reg_read(GPADC_REG_VBAT_EN, &val);
            val |= REG_PU_VBAT_DIV;
            gpadc_reg_write(GPADC_REG_VBAT_EN, val);
            int_unlock(lock);
#ifndef VBAT_DIV_ALWAYS_ON
            // GPADC VBAT needs 10us to be stable and consumes 13mA current
            hal_sys_timer_delay_us(20);
#endif
        case HAL_GPADC_CHAN_0:
        case HAL_GPADC_CHAN_2:
        case HAL_GPADC_CHAN_3:
        case HAL_GPADC_CHAN_4:
        case HAL_GPADC_CHAN_5:
        case HAL_GPADC_CHAN_6:
        case HAL_GPADC_CHAN_ADCKEY:
            lock = int_lock();

            // Int mask
            if (channel == HAL_GPADC_CHAN_ADCKEY || gpadc_event_cb[channel]) {
                // 1) Always enable ADCKEY mask
                // 2) Enable mask if handler is not null
                gpadc_reg_read(GPADC_REG_INT_MASK, &val);
                val |= CHAN_DATA_INTR_MSK(1<<channel);
                gpadc_reg_write(GPADC_REG_INT_MASK, val);
                gpadc_enabled = true;
                hal_gpadc_irq_control();
            }

            // Int enable
            gpadc_reg_read(GPADC_REG_INT_EN, &val);
            val |= CHAN_DATA_INTR_EN(1<<channel);
            gpadc_reg_write(GPADC_REG_INT_EN, val);

            // Clear the channel valid status
            gpadc_reg_write(GPADC_REG_INT_CLR, CHAN_DATA_INTR_MSKED(1<<channel));

            // Channel enable
            if (channel == HAL_GPADC_CHAN_ADCKEY) {
                reg_start_mask = KEY_START;
            } else {
                hal_gpadc_update_atp();
                reg_start_mask = GPADC_START;
                if (GPADC_REG_START == GPADC_REG_CH_EN) {
                    reg_start_mask |= (1<<channel);
                } else {
                    gpadc_reg_read(GPADC_REG_CH_EN, &val);
                    val |= CHAN_EN_REG(1<<channel);
                    gpadc_reg_write(GPADC_REG_CH_EN, val);
                }
            }

            // GPADC enable
            gpadc_reg_read(GPADC_REG_START, &val);
            val |= reg_start_mask;
            gpadc_reg_write(GPADC_REG_START, val);

            int_unlock(lock);
            break;
        default:
            break;
    }

    return 0;
}

int hal_gpadc_close(enum HAL_GPADC_CHAN_T channel)
{
    uint32_t lock;
    unsigned short val;
    unsigned short chan_int_en;
    unsigned short reg_start;

    if (channel >= HAL_GPADC_CHAN_QTY) {
        return -1;
    }

    if (channel != HAL_GPADC_CHAN_ADCKEY) {
        gpadc_atp[channel] = HAL_GPADC_ATP_NULL;
    }

    switch (channel) {
        case HAL_GPADC_CHAN_BATTERY:
#ifndef VBAT_DIV_ALWAYS_ON
            // disable vbat measurement clock
            // vbat div disable
            lock = int_lock();
            gpadc_reg_read(GPADC_REG_VBAT_EN, &val);
            val &= ~REG_PU_VBAT_DIV;
            gpadc_reg_write(GPADC_REG_VBAT_EN, val);
            int_unlock(lock);
#endif
        case HAL_GPADC_CHAN_0:
        case HAL_GPADC_CHAN_2:
        case HAL_GPADC_CHAN_3:
        case HAL_GPADC_CHAN_4:
        case HAL_GPADC_CHAN_5:
        case HAL_GPADC_CHAN_6:
        case HAL_GPADC_CHAN_ADCKEY:
            lock = int_lock();

            // Int mask
            gpadc_reg_read(GPADC_REG_INT_MASK, &val);
            val &= ~CHAN_DATA_INTR_MSK(1<<channel);
            gpadc_reg_write(GPADC_REG_INT_MASK, val);

            // Int enable
            gpadc_reg_read(GPADC_REG_INT_EN, &chan_int_en);
            chan_int_en &= ~CHAN_DATA_INTR_EN(1<<channel);
            gpadc_reg_write(GPADC_REG_INT_EN, chan_int_en);

            // Channel/GPADC enable
            gpadc_reg_read(GPADC_REG_START, &reg_start);
            if (channel == HAL_GPADC_CHAN_ADCKEY) {
                reg_start &= ~KEY_START;
            } else {
                if (GPADC_REG_START == GPADC_REG_CH_EN) {
                    reg_start &= ~(1<<channel);
                    val = reg_start;
                } else {
                    gpadc_reg_read(GPADC_REG_CH_EN, &val);
                    val &= ~CHAN_EN_REG(1<<channel);
                    gpadc_reg_write(GPADC_REG_CH_EN, val);
                }
                if (val & ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6))) {
                    hal_gpadc_update_atp();
                } else {
                    reg_start &= ~GPADC_START;
                }
            }
            gpadc_reg_write(GPADC_REG_START, reg_start);

            if ((chan_int_en & ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7))) == 0) {
                gpadc_enabled = false;
                hal_gpadc_irq_control();
            }

            int_unlock(lock);
            break;
        default:
            break;
    }

    return 0;
}

void hal_gpadc_sleep(void)
{
    unsigned short val;

#ifdef CHIP_BEST2300
    return;
#endif

    gpadc_reg_read(GPADC_REG_START, &val);
    if (val & GPADC_START) {
        val &= ~GPADC_START;
        gpadc_reg_write(GPADC_REG_START, val);
    }
}

void hal_gpadc_wakeup(void)
{
    unsigned short val;

#ifdef CHIP_BEST2300
    return;
#endif

    gpadc_reg_read(GPADC_REG_CH_EN, &val);
    if (val & CHAN_EN_REG((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6))) {
        if (GPADC_REG_START != GPADC_REG_CH_EN) {
            gpadc_reg_read(GPADC_REG_START, &val);
        }
        val |= GPADC_START;
        gpadc_reg_write(GPADC_REG_START, val);
    }
}

void hal_adckey_set_irq_handler(HAL_GPADC_EVENT_CB_T cb)
{
    gpadc_event_cb[HAL_GPADC_CHAN_ADCKEY] = cb;
}

int hal_adckey_set_irq(enum HAL_ADCKEY_IRQ_T type)
{
    uint32_t lock;
    uint16_t val;
    uint16_t set_mask;
    uint16_t clr_mask;
    uint16_t set_en;
    uint16_t clr_en;

    set_mask = 0;
    clr_mask = 0;
    set_en = 0;
    clr_en = 0;
    if (type == HAL_ADCKEY_IRQ_NONE) {
        clr_mask = KEY_RELEASE_INTR_MSK | KEY_PRESS_INTR_MSK | KEY_ERR0_INTR_MSK | KEY_ERR1_INTR_MSK;
        clr_en = KEY_RELEASE_INTR_EN | KEY_PRESS_INTR_EN | KEY_ERR0_INTR_EN | KEY_ERR1_INTR_EN;
        adckey_enabled = false;
    } else if (type == HAL_ADCKEY_IRQ_PRESSED) {
        set_mask = KEY_PRESS_INTR_MSK | KEY_ERR0_INTR_MSK | KEY_ERR1_INTR_MSK;
        clr_mask = KEY_RELEASE_INTR_MSK;
        set_en = KEY_PRESS_INTR_EN | KEY_ERR0_INTR_EN | KEY_ERR1_INTR_EN;
        clr_en = KEY_RELEASE_INTR_EN;
        adckey_enabled = true;
    } else if (type == HAL_ADCKEY_IRQ_RELEASED) {
        set_mask = KEY_RELEASE_INTR_MSK | KEY_ERR0_INTR_MSK | KEY_ERR1_INTR_MSK;
        clr_mask = KEY_PRESS_INTR_MSK;
        set_en = KEY_RELEASE_INTR_EN | KEY_ERR0_INTR_EN | KEY_ERR1_INTR_EN;
        clr_en = KEY_PRESS_INTR_EN;
        adckey_enabled = true;
    } else if (type == HAL_ADCKEY_IRQ_BOTH) {
        set_mask = KEY_RELEASE_INTR_MSK | KEY_PRESS_INTR_MSK | KEY_ERR0_INTR_MSK | KEY_ERR1_INTR_MSK;
        set_en = KEY_RELEASE_INTR_EN | KEY_PRESS_INTR_EN | KEY_ERR0_INTR_EN | KEY_ERR1_INTR_EN;
        adckey_enabled = true;
    } else {
        return 1;
    }

    lock = int_lock();

    gpadc_reg_read(GPADC_REG_INT_MASK, &val);
    val &= ~clr_mask;
    val |= set_mask;
    gpadc_reg_write(GPADC_REG_INT_MASK, val);

    gpadc_reg_read(GPADC_REG_INT_EN, &val);
    val &= ~clr_en;
    val |= set_en;
    gpadc_reg_write(GPADC_REG_INT_EN, val);

    hal_gpadc_irq_control();

    int_unlock(lock);

    return 0;
}

