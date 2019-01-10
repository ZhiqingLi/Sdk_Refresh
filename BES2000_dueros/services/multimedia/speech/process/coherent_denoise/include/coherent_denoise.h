#ifndef __COHERENT_DENOISE_H__
#define __COHERENT_DENOISE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // MIC L/R增益补偿
    float   left_gain;
    float   right_gain;
    // mic L/R delay sample. 0: 适用于麦克距离为<2cm; 1: 适用于麦克距离为2cm左右; 2: 适用于麦克距离为4cm左右
    int     delaytaps;
    // 1: 频域滤波打开; 0: 频域滤波关闭; 默认打开
    int     freq_smooth_enable; 
    // 默认关闭
    int     low_quality_enable;  
} COHERENT_DENOISE_CFG_T;

int coherent_denoise_init(int frame_size, const COHERENT_DENOISE_CFG_T *cfg);
int coherent_denoise_run(short *in, int len, short *out);
int coheren_denoise_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
