#ifndef __ANALOG_BEST3001_H__
#define __ANALOG_BEST3001_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ISPI_ANA_REG(reg)                   (((reg) & 0xFFF) | 0x1000)

#define MAX_ANA_MIC_CH_NUM                  5

void analog_aud_pll_set_dig_div(uint32_t div);

#ifdef __cplusplus
}
#endif

#endif

