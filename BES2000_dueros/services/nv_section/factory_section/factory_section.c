#include "factory_section.h"

const factory_section g_factory_section_default = {
    .hw_info = {
        .sn = 0,
        .bn = 0,
        .version = 0,
        .reserved1 = 0,
    },
    .bt_config = {
        .bt_name = {
            .name = "WT_BOX",
            .len = 6,
        },
        .bt_address = {0x99,0x99,0x34,0x45,0x56,0x67},
        .ble_address = {0xBE,0x99,0x34,0x45,0x56,0x67},
        .rf_config = {
            .reg_config[0] = {
                .reg = 0xc2,
                .val = 0x8000,
                .delayms = 0,
            },
            .counter = 1
        },
    },
    .aud_config = {
        .coded_dac_config = {
            {0x10,0x03,0x00},
            {0x10,0x03,0x03},
            {0x10,0x03,0x06},
            {0x10,0x03,0x09},
            {0x10,0x03,0x0c},
            {0x10,0x03,0x0f},
            {0x10,0x03,0x11},
            {0x10,0x03,0x12},
            {0x10,0x03,0x13},
            {0x10,0x03,0x14},
            {0x10,0x03,0x15},
            {0x10,0x03,0x16},
            {0x10,0x03,0x17},
            {0x10,0x03,0x18},
            {0x10,0x03,0x19},
            {0x10,0x03,0x1a},
        },
        .coded_adc_config = {
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
        },
    },
};



