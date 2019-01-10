#ifndef __AK7755_H__
#define __AK7755_H__


#ifdef __cplusplus  
    extern "C"  {
#endif 


#ifdef EXTRAL_CODEC_AK7755_ENABLE

//AK7755 command
//read
#define AK7755_R_CONT01  0x41
#define AK7755_R_CONT08  0x48
#define AK7755_R_CONT0F  0x4f
#define AK7755_R_CONT1A  0x5a
#define AK7755_READ_IC   0x60
//write
#define AK7755_W_CONT00  0xc0
#define AK7755_W_CONT01  0xc1
#define AK7755_W_CONT08  0xc8
#define AK7755_W_CONT09  0xc9
#define AK7755_W_CONT0F  0xcf
#define AK7755_W_CONT1A  0xda
#define AK7755_ADL_VOL   0xd5 
#define AK7755_ADR_VOL   0xd6
#define AK7755_ADM_VOL   0xd7

#define AK7755_DAL_VOL 0xd8
#define AK7755_DAR_VOL 0xd9
//during reset

#define AK7755_W_PRAM  0xb8
#define AK7755_W_CRAM  0xb4
#define AK7755_W_OFREG 0xb2
#define AK7755_W_ACRAM 0xbb  

#define AK7755_R_PRAM  0x38
#define AK7755_R_CRAM  0x34
#define AK7755_R_OFREG 0x32
#define AK7755_R_ACRAM 0x3b  
//run or set
#define AK7755_W_CRC   0xf2
#define AK7755_R_CRC   0x72
//run
//CRAM write  0x80~0x8F 16bit
#define CRAM_W_CMD_BASE  0x80

#define AK7755_W_EQ_SIGNAL   0x84
#define AK7755_W_EQ_DOUBLE   0x89

#define CRAM_W_RUN      0xa4

#define READ_MIR1      0x76


/////////////////////////////
//CRAM 地址，查阅CRA文件
#define CRAM_VOLUME_ADD_H   0x00
#define CRAM_VOLUME_ADD_L	0x00

#define CRAM_MON_ADD  0x105

#define L_LEVEL_OUT_ADD   0x103
#define R_LEVEL_OUT_ADD   0x104


void ak7755_init(void);
void ak7755_application_init(void);
void ak7755_power_off(void);
int32_t ak7755_read_id(void);

int32_t ak7755_control_register_write(void);

void ak7755_reset(void);
void app_ak7755_volumeset(uint8_t vol);
int8_t ak7755_get_volume(void);

void ak7755_bt_stream_volumeup(void);
void ak7755_bt_stream_volumedown(void);

void ak7755_adc_lch_volume(uint8_t volume);

void ak7755_dac_lch_volume(uint8_t volume);

//DISABLE = 0, ENABLE
int32_t ak7755_clock_control(uint8_t state);

int32_t ak7755_write_pram(uint8_t *ram_buffer,uint16_t length);
int32_t ak7755_write_cram_during_reset(const uint8_t *ram_buffer,uint16_t length);

int32_t ak7755_codec_control(uint8_t state);

int32_t ak7755_dac_input(uint8_t channel);

int32_t ak7755_dsp_download_cmd(uint8_t state);

int32_t ak7755_dsp_cmd(uint8_t state);

void ak7755_dsp_vol_set(uint8_t vol);

int32_t ak7755_set_slave(void);


void dsp_volume_set_during_run(uint8_t volum);

void dsp_volume_set_during_reset(uint8_t volum);

void ak7755_adc_mute(void);
void ak7755_adc_normal(void);
void ak7755_dac_mute(void);
void ak7755_dac_normal(void);
void ak7755_bick_set(uint8_t flag);


void ak7755_write_cram_monitor(uint16_t address);
//void ak7755_write_cram_monitor(uint8_t address);

void ak7755_read_mir(uint8_t mir,uint8_t *mir_data);

#endif

#ifdef __cplusplus  
    }
#endif 

#endif

