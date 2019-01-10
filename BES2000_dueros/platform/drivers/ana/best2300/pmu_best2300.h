#ifndef __PMU_BEST2300_H__
#define __PMU_BEST2300_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PMU_IRQ_HANDLER_T)(void);

enum PMU_IRQ_TYPE_T {
    PMU_IRQ_TYPE_PWRKEY,
    PMU_IRQ_TYPE_GPADC,
    PMU_IRQ_TYPE_RTC,
    PMU_IRQ_TYPE_CHARGER,

    PMU_IRQ_TYPE_QTY
};

int pmu_set_irq_handler(enum PMU_IRQ_TYPE_T type, PMU_IRQ_HANDLER_T hdlr);

void pmu_codec_hppa_enable(int enable);

void pmu_codec_mic_bias_enable(uint32_t map);

void pmu_nfmi_config(int enable);

#ifdef __cplusplus
}
#endif

#endif

