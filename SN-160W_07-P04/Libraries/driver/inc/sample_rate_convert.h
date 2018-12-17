#ifndef __SAMPLE_RATE_CONVERT_H__
#define __SAMPLE_RATE_CONVERT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"
    
    
/***************************************************************
            以下两个函数用于所有采样率转44.1KHZ使用
***************************************************************/    
    
//采样率转换模块初始化
//SrcIndex:      提供1个采样率转换实体，编号为0
//InSampleRate:  采样率转换前的采样率，9种采样率
//OutSampleRate: 采样率转换后的采样率，目前只允许44100
//ChannelNum:	 声道个数，1 or 2
bool SampleRateConvertInit(uint8_t SrcIndex, uint16_t InSampleRate, uint16_t OutSampleRate, uint8_t ChannelNum);   

////采样率转换处理函数
////SrcIndex:      提1个采样率转换实体，编号为0
////InBuf:         采样率转换前的PCM数据
////OutBuf:        采样率转换后的PCM数据
////SampleCnt:	   采样率转换前的采样点数
////return:        转换后的OutBuf缓冲区中有效采样点数
uint16_t SampleRateConvert(uint8_t SrcIndex, int16_t* InBuf, int16_t* OutBuf, uint16_t SampleCnt);


/***************************************************************
            以下函数用于所有采样率升4倍用
***************************************************************/ 

//采样率升4倍初始化
//W_Addr: 在升采样率时需要用到的FIR滤波器缓存地址
//w_Sample: FIR滤波器缓存大小，单位为：Sample
//Channel: 通道号，1 -- 单声道， 2 -- 双声道，目前仅支持单声道
void SampleRateUpSampleby4Init(int16_t *W_Addr, int32_t W_Sample, int16_t Channel);

//采样率升4，目前只支持单声道
//BufIn -- 输入地址
//BufOut -- 输出地址
//n -- 输入的采样点，注意n * 4 + 30不大于W_Sample
//返回值 -- >=0输出的采样点数, -1 输入参数错误
int32_t SampleRateUpSampleby4(int16_t *BufIn, int16_t *BufOut, int32_t n);
    
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif   //
