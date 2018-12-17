/*! \cond MMM_MP_API*/
/********************************************************************************
 *                              5110
 *                            Module: music_player
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       jpliao    2010-08-27 15:00     1.0             build this file
 ********************************************************************************/

/*! \endcond*/
#ifndef __MMM_DAE_DEF__
#define __MMM_DAE_DEF__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * \brief
 *      加载系统头文件
 */
#ifdef WIN32
/* VC */
#define __FAR__
#else
/* MIPS */
#include <psp_includes.h>
#endif

/*! \cond MMM_MP_API */

#define MAX_PEQ_SEG    EQ_NUM_BANDS //PEQ有效band数
#define MAX_GEQ_START  7            //GEQ起始点
#define MAX_GEQ_SEG    7            //GEQ有效band数
#define MAX_DEW_SEG    6  //6段音效增强或减弱
#define DEW_BAND_HIGH_PASS  0 //高通滤波
#define DEW_BAND_LOW_FREQ   1 //低音效果
#define DEW_BAND_HIGH_FREQ  2 //高音效果

//for default value
#define DEAULT_FADEIN_TIME  100

#define DEAULT_FADEOUT_TIME 50

#define MAX_MDRC_SEG   MDRC_NUM_BANDS  //3段DRC

#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)
#define MFBE_BANDS     10
#define MAX_FREQ_POINT 10
#endif

typedef enum
{
   OUTPUT_CHANNEL_NORMAL = 0, 					//正常，中间件会初始化为正常
   OUTPUT_CHANNEL_L_R_SWITCH = 1, 				//左右声道互换 L = R0, R = L0
   OUTPUT_CHANNEL_L_R_MERGE = 2,				//左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
   OUTPUT_CHANNEL_L_R_ALL_L = 3,				//左右声道都输出左声道 L = R = L0
   OUTPUT_CHANNEL_L_R_ALL_R = 4, 				//左右声道都输出右声道 L = R = R0
}output_channel_e;

typedef struct
{
    /*中心频点（或者高通滤波器的截止频率），单位Hz*/
    uint16 cutoff;
    /*Q值，单位0.1，推荐范围是3到30*/
    uint16 q;
    /*增益，单位0.1db，推荐范围是-240到+120*/
    int16 gain;
    /*滤波器类型：0是无效，1是常规EQ滤波器，2是高通滤波器，其它值为reserve*/
    uint16 type;
} peq_band_t;

typedef struct
{
    short threshold;
    short attack_time;
    short release_time;
    short reserve;//确保4字节对齐
} mdrc_band_t;

typedef struct
{
    /*! 频率，对于高通滤波，表示偏移值；对于低频和高频，表示截止频率，最高bit表示虚拟低音和高音增强；对于PEQ点，表示中心频点 */
    uint16 cutoff;
    /*! 0表示无效 */
    int16 gain;
} dae_enh_weaken_band_t;

#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)
//该结构体DSP、中间件和case共用：case维护一份用于设置，中间件维护一份用户设置给DSP，DSP在获取DAE更新时拷贝一份
typedef struct
{
    uint8 num_band;                         //频段数,算法内部最多支持10段
    uint8 num_freq_point;                   //频点数,算法内部最多支持12点
    short duration_ms;                      //统计时长，单位ms
    short f_c[MFBE_BANDS];					//带通中心频率
    short freq_point[MAX_FREQ_POINT];       //各频点数组 
} dae_attributes_t;
#endif


/*********为兼容标准音效模型增加的DAE结构体********/
#define MAX_MDRC_SEG_STANDARD   3

typedef struct
{
    short threshold1;      //阈值1 
    short ratio1;          //压缩比1 
	short threshold2;      //阈值2
    short ratio2;          //压缩比2
	short tav;             //RMS检测平均时间(ms)
	short attack_time;     //启动时间 
	short release_time;    //释放时间	
}dae_compressor_standard_mode_t;

typedef struct
{
    short threshold1;
    short compress_ratio1;
	short threshold2;
    short compress_ratio2;
	short tav_time;
    short attack_time;
    short release_time;
    short reserve;//确保4字节对齐
}dae_mdrc_band_standard_mode_t;

typedef struct
{
	int16 signal_adjust;
	short mdrc_crossover_freq0; //MDRC低中分频点
	short mdrc_crossover_freq1; //MDRC中高分频点
	int16 makeup_gain; 
}mdrc_extend_para_standard_mode_t;


typedef struct
{
    short MDRC_compensation_threshold;                  //default: -100,step 0.1dB,setting: -10*10 = -100
    short MDRC_compensation_peak_detect_attack_time;    //default: 0.5ms, step: 0.01ms, setting: 5*10
    short MDRC_compensation_peak_detect_release_time;   //default: 500ms, step: 1ms, setting: 500
    short MDRC_compensation_filter_Q;                   //default: 1,step 0.1, setting:0.1*10 = 1, 0.1~0.9
}mdrc_peak_detect_standard_mode_t;

typedef struct
{
  short threshold1;
  short threshold2;
}mdrc_threshold_base_max_t;

typedef struct
{
  short threshold1;
  short threshold2;
}compressor_threshold_base_t;

typedef enum
{
    //智能模式    
    SMART_MODE = 0,
    //标准模式
    STANDARD_MODE = 1,
} audiopp_type_e;

typedef struct
{
    int16 mdrc_threshold_base_max_trans_standard_mode;
    mdrc_threshold_base_max_t mdrc_threshold_base_trans_standard_mode[MDRC_NUM_BANDS_STANDARD_MODE];
    short MDRC_compensation_threshold_base_trans_standard_mode;

    compressor_threshold_base_t compressor_threshold_base;
    compressor_threshold_base_t compressor_threshold_base_trans;
}dae_mdrc_para_trans_sm_t;

typedef struct
{
    short precut_standard_mode;
    short compressor_th1;
    short compressor_th2;
    
    short signal_fine_tune;                 //信号微调0.1db
    short vol_fine_tune;                    //音量微调0.1db    
    short gain_compensation;                //增益补偿0.1db
    short compensation_filter_threshold;    //补偿滤波器阈值0.1db

    short mdrc_low_freq_th1;                
    short mdrc_low_freq_th2;
    short mdrc_middle_freq_th1;
    short mdrc_middle_freq_th2;
    short mdrc_high_freq_th1;
    short mdrc_high_freq_th2;   
}vol_dae_para_sm_t;

/**************************************************/

typedef struct
{
    bool enable; //使能DAE功能，如果禁止就加载空的DAE库，以释放DAE库占用的空间
    bool bypass; //bypass打开表示不做任何后处理，其他配置忽略掉；一般只用于调试
    uint16 precut_ratio; //预衰减幅度，0.1db为单位，取值范围 0 ~ 120；通过音量提升补偿
    uint16 equivalent_gain; //DAE等效增益，0.1db为单位，取值范围 0 ~ 120
    uint8 mdrc_precut_ratio; //MDRC最大阈值对应的precut，0.1db为单位，取值范围 0 ~ 120；通过“音量微调”或外部功放增益补偿
    uint16 makeup_gain; //makeup gain，将信号提升到0DB，以降低音量，提升信噪比，0.1db为单位，取值范围 0 ~ 120
    int8 post_precut_ratio; //后级信号衰减，用来做信号微调，一般为负值，0.1db为单位
    bool peq_enable; //PEQ
    bool spk_compensation_enable; //喇叭补偿
    bool vbass_enable; //虚拟低音
    bool vsurround_enable; //环绕音效
    bool treble_enhance_enable; //高音增强
    bool mdrc_enable; //MDRC算法
    bool limiter_enable; //限幅算法
    bool energy_detect_enable; //信号能量检测
    bool noise_reduction_enable;
    bool enable_2823T;
    bool dae_print_enable;
    
    uint8 run_freq; //运行频率

    peq_band_t peq_bands[MAX_PEQ_SEG]; //PEQ 滤波器参数
    uint8 peq_band_enable_type[MAX_PEQ_SEG]; //peq类型和enable选项，1表示Speaker PEQ，2表示Post PEQ，0表示不执行

    uint16 treble_enhance_cut_freq;//高频截止频率
    int16 treble_enhance_ratio;    //增益 单位为0.1dB

    uint8 spk_comp_source; //滤波器参数来源，0表示SD区文件，1表示VRAM
    short spk_comp_filter_order; //滤波器阶数
    uint8 spk_comp_filter_QvalLeft; //左声道滤波器Q值
    uint8 spk_comp_filter_QvalRight; //右声道滤波器Q值

    uint16 vbass_cut_freq; //低频截止频率
    int16 vbass_ratio; //虚拟低音的增益 单位为0.1dB 0.0dB -24dB 方案端要减去12.0dB再传递给DSP
    int8 original_bass_ratio; //原始低音的增益
    
    uint8 vsurround_angle; //环绕角度
    int8 vsurround_ratio; //增益

    uint16 mdrc_crossover_freq[MAX_MDRC_SEG-1]; //mdrc分频点
    mdrc_band_t mdrc_bands[MAX_MDRC_SEG]; //MDRC参数

    int16 limiter_threshold; //限幅阈值
    uint16 limiter_attack_time; //启动时间
    uint16 limiter_release_time; //释放时间

    dae_enh_weaken_band_t dew_bands[MAX_DEW_SEG]; //音效增强或减弱

    uint8 period_count; //有效周期数
    uint8 preadjust_count; //开始预调降低信号大小的周期数，然后每个周期降低相应比例
    short period; //统计周期时长，以1ms为单位
    short preadjust_db; //信号大小降低较多才启动预调机制，以0.1db为单位
    short signal_energy_init; //初始信号能量值

    int16 mdrc_threshold_base[MAX_MDRC_SEG]; //mdrc 动态阈值基值
    int16 mdrc_threshold_base_max; //mdrc 阈值基值最大值
    int16 limiter_threshold_diff; //mdrc 模式下Limiter阈值与mdrc最大阈值的差值

    uint8 MultiFreqBandEnergy_enable;       //多频段能量计算开关
    uint8 FreqSpetrumDisplay_enable;        //多频点能量计算开关
    
#if (SUPPORT_MULTI_FREQ_MULTI_BAND_SETTING == 1)
    dae_attributes_t* dae_attributes;
#endif

   /*********为兼容标准音效模型增加的DAE参数**********/
    bool compressor_enable_standard_mode;        //压缩器算法
    bool mdrc_enable_standard_mode;              //mdrc算法
    bool audiopp_type_update_flag;               //音效模型更新标志 
    uint8 audiopp_type;                          //audiopp_type_e
    uint16  precut_standard_mode;                  //标准模式下precut值
    int8  dynamic_precut_standard_mode;          //小音量下动态precut衰减   
    
    int16 compressor_threshold_diff; //mdrc 模式下compressor阈值与mdrc最大阈值的差值
    uint16 makeup_gain_standard_mode;      
    dae_compressor_standard_mode_t   compressor_standard_mode;
    mdrc_extend_para_standard_mode_t mdrc_extend_para_standard_mode;
    mdrc_peak_detect_standard_mode_t        mdrc_peak_standard_mode;
    dae_mdrc_band_standard_mode_t*   p_mdrc_band_standard_mode;
   /**************************************************/
    uint8 volume_current;
} dae_config_t;

typedef struct
{
    short *p_fade_in_time_ms;
    short *p_fade_out_time_ms;
    short *p_fade_in_flag;
    short *p_fade_out_flag;
    short *p_DAE_change_flag;
} dae_fade_param_addr_t;

typedef enum
{
    /*!获取淡出状态*/
    GET_FADE_OUT_STATUS,
    /*!获取淡入淡出参数地址，以便在中断中直接修改*/
    GET_FADE_PARAM_ADDR,
    /*!获取能量值地址，以便在中断中直接读取*/
    GET_ENERGY_VALUE_ADDR,
} get_feature_type_e;

typedef enum
{
    /*!参数初始化*/
    SET_PARAM_INIT = NULL,
    /*!设置音效配置参数*/
    SET_EFFECT_PARAM,
    /*!设置采样率*/
    SET_SAMPLE_INFO,
    /*!设置淡出*/
    SET_FADE_OUT,
    /*!设置淡入*/
    SET_FADE_IN,
    /*!设置噪音消除算法参数*/   
    SET_NOISE_REDUCTION_PARA,
    /*!max*/
    MAX_SET_TYPE
} set_effect_type_e;

#define OUTPUT_CHANNEL_NORMAL      0 //正常，默认初始化为正常
#define OUTPUT_CHANNEL_L_R_SWITCH  1 //左右声道互换 L = R0, R = L0
#define OUTPUT_CHANNEL_L_R_MERGE   2 //左右声道合并后输出到左声道和右声道 L = R = (L0 + R0) / 2
#define OUTPUT_CHANNEL_L_R_ALL_L   3 //左右声道都输出左声道 L = R = L0
#define OUTPUT_CHANNEL_L_R_ALL_R   4 //左右声道都输出右声道 L = R = R0

typedef struct
{
    /*!通道数*/
    uint32 chanel_num;
    /*!采样率*/
    uint32 sample_rate;
    /*!block size*/
    uint32 block_size;
    /*!输出通道处理 1:左右声道交换 2: 单端耳机应用(L=R=原始的(L/2 + R/2)), 0 or 其它值, 跟原始的左右声道一致*/
    int16 output_channel;
     /*!淡入淡出处理句柄*/
    int32 fade_handle;
} init_play_param_t;

typedef struct
{
    short enable;				            //default: 200ms,step 0.01ms, set 200*100 = 20000    
    short DownExp_attack_time;				//default: 200ms,step 0.01ms, set 200*100 = 20000
	short DownExp_release_time;				//default: 10ms,step 1ms, set 10
	short DownExp_set0_threshold;		    //default: 1ms, step 0.01ms, set 100;
	short DownExp_threshold;			    //default: -80dB(min), step 0.1dB, set -800
	short DownExp_ratio;				    //default: 3(greater than 1)  
}init_noise_reduction_t;


/*! \endcond */
#ifdef __cplusplus
}
#endif

#endif // __MMM_DAE_DEF__
