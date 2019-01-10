#ifndef __AUDIO_DRC_H__
#define __AUDIO_DRC_H_

#include <stdbool.h>

#define AUD_DRC_LOC __attribute__((section(".overlay_text1")))


#define DRC_BAND_NUM (3)

struct drc_band_settings{
    int threshold;       //阈值
    float makeup_gain; //增益
    int ratio;              //压缩率
    int attack_time;
    int release_time;
    float scale_fact;
};


enum {
  DRC_FREQ_NULL =  -1,
  DRC_FREQ_50HZ =   1, 
  DRC_FREQ_63HZ,     
  DRC_FREQ_79HZ,     
  DRC_FREQ_100HZ,    
  DRC_FREQ_125HZ,    
  DRC_FREQ_158HZ,    
  DRC_FREQ_199HZ,    
  DRC_FREQ_251HZ,    
  DRC_FREQ_316HZ,    
  DRC_FREQ_398HZ,    
  DRC_FREQ_501HZ,    
  DRC_FREQ_630HZ,    
  DRC_FREQ_794HZ,    
  DRC_FREQ_1000HZ,   
  DRC_FREQ_1258HZ,   
  DRC_FREQ_1584HZ,   
  DRC_FREQ_1995HZ,   
  DRC_FREQ_2511HZ,   
  DRC_FREQ_3162HZ,   
  DRC_FREQ_3981HZ,   
  DRC_FREQ_5011HZ,   
  DRC_FREQ_6309HZ,   
  DRC_FREQ_7943HZ,   
  DRC_FREQ_10000HZ,  
  DRC_FREQ_12589HZ,  
  DRC_FREQ_15848HZ,  
  DRC_FREQ_19952HZ,  
};

#define     MAX_DRC_FILTER_CNT_SUPPORT 2

typedef struct {
    int knee;
    int filter_type[MAX_DRC_FILTER_CNT_SUPPORT];
    int band_num;
	int look_ahead_time;
    struct drc_band_settings band_settings[DRC_BAND_NUM];
} drc_settings_t;


struct drc_compressor_settings{
    int ch_num;             //channel number
    int fs;                       //frequency
    int depth;                //audio depth
    int frame_size;       //if change this only, do not need call  drc_setup
    int knee;
    int band_num;
	int look_ahead_time;
    struct drc_band_settings band_settings[DRC_BAND_NUM];
};

//#define DRC_DEBUG 1

#if defined(DRC_DEBUG)
#define DRC_FRAME_NUM (1024)
#endif

#if defined(__cplusplus)
extern "C" 
{
#endif

enum DRC_ACTION_T
{
    DRC_ACTION_BOOST_ON = 0,
    DRC_ACTION_BOOST_OFF,
};

typedef int (*drc_action)(int action);
void app_key_set_drc_param(drc_settings_t *drc_cfg );
int drc_set_cfg(const drc_settings_t* cfg);
int update_drc_cfg(drc_settings_t *drc_cfg, uint32_t  len);

void audio_drc_scale(short *buf);
int dynamic_drc_set_cfg(void);
//void audio_drc_set_makeup_gain(int gain);
//void audio_drc_set_framesize(int frame_size);
int audio_drc_process(uint8_t *buf, uint32_t len);
void audio_drc_open(int sample_rate, uint32_t frame_size);
#if defined(__cplusplus)
}
#endif

#endif /* __AUDIO_DRC_H_ */
