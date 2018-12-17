#ifndef _MMM_AGC_AEC_DEF_H_
#define _MMM_AGC_AEC_DEF_H_


//AEC性能参数结构体如下：
typedef struct
{
	uint32 nms_in;
	uint32 nms_mic;
	uint32 nms_echo;
	uint32 nms_aec;
	uint32 nms_ns;
	uint32 nms_out;
	uint32 maxrefenv;
	uint32 refisaudible;
	uint32 leak;
	uint32 coeff;
	uint32 dt;
}sv_aec_info_t;


typedef struct
{
    uint32 mic_op_gain;      //MIC模拟增益：0:26db,1:30db,2:31.5db,3:33db,4:34.5db,5:36db,6:37.5db,7:39db，默认为0
    uint32 mic_adc_gain;     //MIC数字增益：0:-70.5db, 47:0db, 每级递增1.5db，默认为47
    uint32 speaker_channel;  //声道选择：1表示右声道，2表示左声道，3表示双声道，默认为1
    uint32 dac_weak;         //声道衰减，0~18db；1-18（衰减1db-18db）表示左喇叭，33-50（衰减1db-18db）表示右喇叭，默认为0
    uint32 pa_volume;        //IC内部pa volume，最大40，最小0，默认为28
    uint32 pre_agc_enable;   //前端AGC使能与否，默认为1
    uint32 pre_agc_level;    //前端AGC level，12000 - 32000，默认20000
    uint32 pos_agc_enable;   //后端AGC使能与否，默认为1
    uint32 pos_agc_level;    //后端AGC level，12000 - 32000，默认20000
    
    uint32 frame_size;       //帧长时间，单位ms，目前仅支持 16ms 帧长
    uint32 sampling_rate;    //采样率，单位Hz，目前仅支持 8000Hz 采样率
    uint32 aec_enable;       //aec模块开关，：打开，：关闭
    uint32 tail_length;      //回声尾长，单位 ms，必须为16的倍数,目前仅支持 32 ms
    uint32 nlp_enable;       //非线性处理子模块开关，：打开，：关闭，为1 时必须保证aec_enable 为1
    uint32 nlp_aggressiveness; //非线性处理程度，整数~15,越大代表非线性处理越厉害
    uint32 dt_threshold;     //双讲近端语音保留程度，整数~15，越大表示双讲是近端语音保留越多，但是可能会带来单讲的回声，需找个折中值。
    uint32 aes_enable;       //残留回声抑制子模块开关，：打开，：关闭，为1 时必须保证aec_enable 为
    uint32 echo_suppress;    //回声抑制子模块单讲回声抑制比，单位dB，0~40
    uint32 echo_suppress_avtive; //回声抑制子模块双讲回声抑制比，单位dB，0~40
    
    uint32 ans_enable;       //降噪开关，：打开，：关闭
    uint32 noise_suppress;   //噪声抑制比，单位dB，0~25
    
    uint32 cng_enable;       //舒适噪声开关，：打开，：关闭
    uint32 noise_floor;      //舒适噪声水平，单位dB Fs, 一般应小于-45 dB
    
    uint32 analysis_mode;    //测试模式，用于分析使用
    uint32 hfp_mode;//通话//0是双工1是单工
}sv_configs_t;



typedef struct
{
	int aec_enable;			//aec模块开关，1：打开，0：关闭
	int tail_length;		//回声尾长，单位 ms，必须为16的倍数,目前仅支持 32 ms
	//int bulk_delay;			//bulk delay
	int nlp_enable;			//非线性处理子模块开关，1：打开，0：关闭，为 1 时必须保证 aec_enable 为 1
	int nlp_aggressiveness; //非线性处理程度，整数0~15,越大代表非线性处理越厉害
	int dt_threshold;	    //双讲近端语音保留程度，整数0~15，越大表示双讲是近端语音保留越多，但是可能会带来单讲的回声，需找个折中值。
	int aes_enable;			//残留回声抑制子模块开关，1：打开，0：关闭，为 1 时必须保证 aec_enable 为1
	int echo_suppress;		//回声抑制子模块单讲回声抑制比，单位dB， 0~40
	int echo_suppress_avtive; //回声抑制子模块双讲回声抑制比，单位dB， 0~40
}aec_prms_t;



typedef struct
{
    int ans_enable;         //降噪开关，1：打开，0：关闭
    int noise_suppress;     //噪声抑制比，单位dB， 0~25
}ns_prms_t;

typedef struct
{
    int cng_enable;     //舒适噪声开关，1：打开，0：关闭
    int noise_floor;    //舒适噪声水平，单位 dB Fs, 一般应小于-45 dB
}cng_prms_t;


#if 1
typedef struct
{
	int pf_enable;		//使能开关，1：打开， 0：关闭
	int blocksize;		//处理块的长度，固定256，可以不用开放
	int overlapsize;	//处理块交叠加长度，固定128，可以不用开放
	int order;			//滤波器阶数;取值范围[2~8]，默认3
	int lamda;			//滤波器平滑系数，越大表示越平滑，但跟踪速度越慢;取值范围[0,31]，默认12
	int belta;			//滤波器处理侵略性参数，越大去回声越到，但可能影响双工性能;取值范围[0~15]，默认10
	int stepsize;		//滤波器系数更新步长,越大更新越快，但失调可能越大；取值范围[0~15]，默认7

}pflt_prms_t;
#endif


typedef struct
{
    uint32 frame_size;   //帧长时间，单位ms，目前仅支持 16ms 帧长
    uint32 sampling_rate; //采样率，单位Hz，目前仅支持 8000Hz 采样率
    // aec prms
    aec_prms_t aec_prms;    //ofs = 4*2B
    //ns prms
    ns_prms_t ns_prms;    // ofs = 4*10B
    //cng prms
    cng_prms_t cng_prms; //ofs = 4*12B
    //powerfilter prms
#if 1
    pflt_prms_t pflt_prms;//ofs = 4*14B
#endif
    uint32  pcm_mode;   //0:上传pcm, 1:不上传；2上传全0数据(mute),ofs = 4*21B
    uint32  air_pkt_len;     //蓝牙空中数据包长ofs = 4*22B
    uint32 speaker_channel; //打电话时喇叭声道使能，1：右声道，2：左声道，3：双声道；
    uint32 speaker_channel_weak_sel;//0表示不衰减，1表示右声道，2表示左声道
    uint32 speaker_channel_weak_val;//衰减幅度，0-18db
    uint32 hfp_mode;//通话0是双工1是单工
    uint32 pre_agc_level;//前端AGC level，[0-15]级
    uint32 pos_agc_level;//后端AGC level，[0-15]级
    uint32 agc_pre_en;//前段agc使能，0关闭，1打开
    uint32 agc_back_en;//后端agc使能，0关闭，1打开
    uint32 hfp_connect_mod;// /0 没接通   1 接通
}sv_prms_t;//dsp内部结构

typedef struct
{
    int nms_in;
    int nms_mic;
    int nms_echo;
    int nms_aec;
    int nms_ns;
    int nms_out;
    int maxrefenv;
    int refisaudible;
    int leak;
    int coeff;
    int dt;
}sv_info_t;

#endif //_MMM_AGC_AEC_DEF_H_
