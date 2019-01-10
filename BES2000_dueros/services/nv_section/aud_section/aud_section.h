#ifndef __aud_section_h__
#define __aud_section_h__

#include "section_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define aud_section_debug
#ifdef aud_section_debug
#define aud_trace     TRACE
#else
#define aud_trace(...)
#endif
#include "hal_aud.h"
#define audsec_tag  "audsec_tag"

#define aud_section_magic   0xdad1
#define aud_section_struct_version  1

typedef struct{
    float agc_level;
    int32_t enable_agc;
    int32_t enable_vd;
    int32_t fixed_gain;
    int32_t max_gain;
    float noise_prob;
    float init_max;
    int32_t noise_suppress;
    int32_t echo_suppress;
    int32_t echo_suppress_active;
    int32_t fixed_leak_estimate;
}noise_echo_supprss;

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


#define eq_config_num               5
#define eq_config_coef_len        512       //384 fir
typedef enum
{
    audtool_usb_samprate_44_1,
    audtool_usb_samprate_48,
    audtool_usb_samprate_96,
    audtool_samprate_count
}audtool_samprate_enum;

typedef enum
{
    audtool_bt_samprate_32,
    audtool_bt_samprate_44_1,
    audtool_bt_samprate_48,
}audtool_bt_samprate_enum;

#define AUD_COEF_LEN        500

#if (AUD_SECTION_STRUCT_VERSION == 1)
#define AUD_IIR_NUM   (6)
#elif (AUD_SECTION_STRUCT_VERSION == 2)
#define AUD_IIR_NUM   (8)
#endif

typedef struct _anc_rir_coefs
{
    int32_t coef_b[3];
	int32_t coef_a[3];
	
}anc_iir_coefs;

typedef struct _aud_item
{
    int32_t total_gain;
	
	uint16_t iir_bypass_flag;
	uint16_t iir_counter;
    anc_iir_coefs iir_coef[AUD_IIR_NUM];    
    
#if (AUD_SECTION_STRUCT_VERSION == 1)
	uint16_t fir_bypass_flag;
    uint16_t fir_len;
    int16_t fir_coef[AUD_COEF_LEN];
    int8_t pos_tab[16];
#elif (AUD_SECTION_STRUCT_VERSION == 2)
    int32_t reserved_for_drc[32];
#endif
    int16_t reserved1;
    int8_t dac_gain_offset; // in qdb (quater of dB)
    int8_t adc_gain_offset; // in qdb (quater of dB)
} aud_item;

typedef struct{
	aud_item anc_cfg_ff_l;
  	aud_item anc_cfg_ff_r;
   	aud_item anc_cfg_fb_l;
  	aud_item anc_cfg_fb_r;
#if (AUD_SECTION_STRUCT_VERSION == 2)	    
   	aud_item anc_cfg_tt_l;
  	aud_item anc_cfg_tt_r;
   	aud_item anc_cfg_mc_l;
  	aud_item anc_cfg_mc_r;
#endif

}struct_anc_cfg;

typedef struct _eq_iir_config
{
    int32_t type;
    int32_t order;
    int32_t num;
    int32_t coef[253];//int or float
}eq_iir_config;

typedef struct{
    eq_iir_config iir_config;
    aud_item fir_config[5][audtool_samprate_count];
}eq_config_t;

enum ANC_INDEX{
    ANC_INDEX_0 = 0,
    ANC_INDEX_1,
    ANC_INDEX_2,
    ANC_INDEX_3,
    ANC_INDEX_TOTAL,
};

typedef struct
{
    struct_anc_cfg anc_config_arr[ANC_INDEX_TOTAL];
}anc_config_t;

#define AUDSEC_RESERVED_LEN (1024 - sizeof(section_head_t) - sizeof(aud_config_t) - sizeof(noise_echo_supprss))
typedef struct{
    aud_config_t aud_config;
    noise_echo_supprss suppress;
    uint8_t reserved_area[AUDSEC_RESERVED_LEN];
    anc_config_t anc_config;
    eq_config_t eq_config;
}aud_section;

typedef struct _struct_audsec
{
    section_head_t sec_head;
    aud_section sec_data;
}struct_audsec;

#define AUD_COEF_LEN    500
#if (AUD_SECTION_STRUCT_VERSION == 1)
#define AUD_IIR_NUM   (6)
#elif (AUD_SECTION_STRUCT_VERSION == 2)
#define AUD_IIR_NUM   (8)
#endif

typedef struct
{
    unsigned char anc_ver[16];
    unsigned char batch_info[16];
    unsigned char serial[16];
}anc_ident;

typedef struct _best2000_anc_iir_coefs
{
    int32_t coef_b[3];
    int32_t coef_a[3];
}best2000_anc_iir_coefs;

typedef struct _best2000_aud_item
{
    int32_t total_gain;
    uint16_t iir_bypass_flag;
    uint16_t iir_counter;
    best2000_anc_iir_coefs iir_coef[AUD_IIR_NUM];
#if (AUD_SECTION_STRUCT_VERSION == 1)
    uint16_t fir_bypass_flag;
    uint16_t fir_len;
    int16_t fir_coef[AUD_COEF_LEN];
    int8_t pos_tab[16];
#elif (AUD_SECTION_STRUCT_VERSION == 2)
    int32_t reserved_for_drc[32];
#endif
    int16_t reserved1;
    int8_t dac_gain_offset; // in qdb (quater of dB)
    int8_t adc_gain_offset; // in qdb (quater of dB)
} best2000_aud_item;

enum auditem_side_enum_t{
    PCTOOL_AUDITEM_SIDE_LEFT,
    PCTOOL_AUDITEM_SIDE_RIGHT,
    PCTOOL_AUDITEM_SIDE_COUNT
};

typedef struct{
    best2000_aud_item anccfg[PCTOOL_AUDITEM_SIDE_COUNT];
}pctool_anccfg_sruct;

enum auditem_anc_type_enum_t{
    PCTOOL_AUDITEM_ANC_TYPE_FF,
    PCTOOL_AUDITEM_ANC_TYPE_FB,
    PCTOOL_AUDITEM_ANC_TYPE_COUNT
};

typedef struct{
    pctool_anccfg_sruct anc_type_cfg[PCTOOL_AUDITEM_ANC_TYPE_COUNT];
}pctool_anccfg_struct;

enum auditem_sample_enum_t{
    PCTOOL_SAMPLERATE_44_1X8K,
    PCTOOL_SAMPLERATE_48X8K,
    PCTOOL_SAMPLERATE_50_7X8K = PCTOOL_SAMPLERATE_48X8K,
    PCTOOL_AUDITEM_SAMPLERATE_COUNT
};
typedef struct{
    pctool_anccfg_struct anc_cfg[PCTOOL_AUDITEM_SAMPLERATE_COUNT];
}best2000_struct_anc_cfg;

#define BEST2000_ANC_APPMODE_COUNT      4
typedef struct
{
    best2000_struct_anc_cfg anc_config_arr[BEST2000_ANC_APPMODE_COUNT];
}best2000_anc_config_t;

#define BEST2000_AUDSEC_RESERVED_LEN (0x10000 - sizeof(section_head_t) - sizeof(anc_ident) - sizeof(best2000_anc_config_t))
typedef struct{
    anc_ident ancIdent;
    best2000_anc_config_t anc_config;
    unsigned char reserved[BEST2000_AUDSEC_RESERVED_LEN];
}audsec_body;

typedef struct{
    section_head_t sec_head;
    audsec_body sec_body;
}best2000_aud_section;

typedef struct{
    uint8_t io_pin;
    uint8_t set_flag;
}pctool_iocfg;
#ifdef __cplusplus
}
#endif

#endif
