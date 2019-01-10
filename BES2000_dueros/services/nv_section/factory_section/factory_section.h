#ifndef __FACTORY_SECTIONS_H__
#define __FACTORY_SECTIONS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

typedef struct{
    uint16_t magic;
    uint16_t version;
    uint32_t crc;
    uint32_t reserved0;
    uint32_t reserved1;
}section_head_t;

typedef struct{
    uint32_t sn;
    uint32_t bn;
    uint32_t version;
    uint32_t reserved1;
}hw_info_t;

typedef struct{
    struct BT_NAME_T {
        uint8_t name[255];
        uint8_t len;
    }bt_name;
    uint8_t bt_address[8];
    uint8_t ble_address[8];
    struct RF_CONFIG_T {
        struct REG_WRITE_T {
            uint16_t reg;
            uint16_t val;
            uint16_t delayms;
        }reg_config[20];
        uint8_t counter;
    }rf_config;
}bt_config_t;

typedef struct{
    struct CODEC_DAC_CONFIG_T {
        uint8_t tx_pa_gain;
        uint8_t sdm_gain;
        uint8_t sdac_volume;
    }coded_dac_config[16];
    struct CODEC_ADC_CONFIG_T {
        uint8_t reserved0;
        uint8_t reserved1;
        uint8_t reserved2;
    }coded_adc_config[16];
}aud_config_t;

typedef struct{
    hw_info_t hw_info;
    bt_config_t bt_config;
    aud_config_t aud_config;
} factory_section; //Size : 0x3C


#ifdef __cplusplus
}
#endif
#endif
