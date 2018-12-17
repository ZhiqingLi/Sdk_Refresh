#ifndef _COMMON_ASET_NEW_FUNC_H
#define _COMMON_ASET_NEW_FUNC_H

#include "case_independent.h"
#include "psp_includes.h"
#include "mmm_dae_def.h"
#include "stub_interface.h"
#include "case_mem_define.h"
#include "stub_command.h"
#include "setting_common.h"
#include "common_func.h"

#define EQMAXPOINTCOUNT          14  //每个命令最多容纳14个PEQ点
#define EQMAXPOINTCOUNT_ALL      20  //pc上最多支持的点数

//自动频响曲线添加
#define MAX_FR_POINT 4097
#define MOD_IR_NUM 1025
#define MAX_INVERSE_IR_NUM 64
#define LPC_ORDER 127

#define ASET_FIR_INFO_LEN 128

typedef struct
{
    uint32  update_flag;            //1表示PC工具更新了系数表，需要传递到DSP LIB
    uint32  length;                 //此次要更新的系数表的长度
    uint32  memory_addr;            //将要更新内存的地址       
}aset_waves_coeff_property_t;

typedef struct
{
    uint8 state; //0: not start 1:start
    uint8 upload_data; //0: not upload 1:upload
    uint8 volume_changed; //0:not changed 1:changed
    uint8 download_data; //0:not download 1:download

    uint8 upload_case_info; //0:not upload 1:upload
    uint8 main_switch_changed; //0:not changed 1:changed
    uint8 update_audiopp; //0:not update 1:update
    uint8 reserved[9];
    //ofs = 16
    uint8 eq_data_changed; //0:not change 1:change
    uint8 vbass_data_changed;
    uint8 te_data_changed;
    uint8 limiter_data_changed;
    // osf = 20
    uint8 spk_compensation_data_changed;
    uint8 surr_data_changed;
    uint8 multiband_drc_data_changed;//智能音效模型下MDRC
    uint8 Left_frcurve_data_changed;
    // ofs = 24
    uint8 see_data_changed;
    uint8 sew_data_changed;
    uint8 SignalD_data_changed;
    uint8 Right_frcurve_data_changed;
   // ofs = 28
    uint8 compressor_changed;
    uint8 standard_mode_mdrc_changed;//标准音效模型下MDRC
    uint8 reserved1[2];
} aset_status_t;

//**********小机端的数据结构********
typedef struct
{
    int16 freq; //EQ频点的频率
    int16 gain; //EQ频点的增益值
    int32 Qval; //EQ频点的Q值----转int
    int16 type; //EQ频点的类型
    int8 enable; //EQ频点是否使能，0表示禁止，1表示Speaker PEQ，2表示Post PEQ
    int8 reserved[1]; //保留
} aset_peq_point_t;

typedef struct
{
    int16 main_gain_value; //主体增益
    int16 equivalent_gain_value; //等效增益
    int8 main_gain_enable; //主体增益使能
    int8 peq_enable; //EQ模块是否使能
    int8 nPointTotal; //点数
    int8 MaxPointTotal; //最大点数 8bytes
    aset_peq_point_t peq_array[EQMAXPOINTCOUNT]; //点列表 12*14
} aset_peq_data_t;

typedef struct
{
    int8 nBlockPointTotal;  //段点数，固定为14个
    int8 nMaxPointTotal;    //小机上报的最多点数
    int8 szRev[2];
    aset_peq_point_t peq_array[EQMAXPOINTCOUNT]; //点列表 12*14
} aset_peq_data_2_3_t;

typedef struct
{
    int16 cutoff_freq; //起：，步长：，终：  default:90
    int16 vbass_gain;  //PC传递过来的虚拟低音增益的范围是0.0dB ~ 24.0dB  精度为0.1dB 方案端要减去12.0dB再传递给DSP
    int8 vbass_enable; //VBass模块使能
    int8 reserved[3]; //保留
} aset_vbass_data_t;

typedef struct
{
    int16 te_gain; //高音增益0-15dB default:7dB
    int16 cutoff_freq; //高音增强截止频率 默认值为10KHz
    int8 te_enable; //TE模块使能
    int8 reserved[3]; //保留
} aset_te_data_t;

typedef struct
{
    int16 head_angle; //双扬声器到人头的角度起：，步长：，终：defualt:10
    int16 surr_gain; //环绕增益0~1   default:0.707
    int8 surr_enable; //Surround模块使能
    int8 reserved[3]; //保留
} aset_surr_data_t;

typedef struct
{
    int16 threhold; //阈值，-60:0.1:0dB，初始化建议-1dB
    int16 attack_time; //启动时间，.05:0.05:10ms，传值扩大倍，即范围:5:1000，初始化为0.1ms
    int16 release_time; //释放时间，:1:1000ms，初始化为300ms
    int8 limiter_enable; //模块使能
    int8 reserved[1]; //保留
} aset_limiter_data_t;

typedef struct
{
    int16 threshold; //阈值，-60:0.1:0dB，初始化建议-1dB
    int16 attack_time; //启动时间，.05:0.05:10ms，传值扩大倍，即范围:5:1000，初始化为0.1ms
    int16 release_time; //释放时间，:1:1000ms，初始化为300ms
} aset_mdrc_band_t;

typedef struct
{
    aset_mdrc_band_t mdrc_band[MAX_MDRC_SEG];
    int8 mdrc_enable;
    int8  mDRC2_enable;//添加DRC模块命令二标志，代表还有一组数据需要发送
    short sIndex;     //添加模组序号，第一组为，以此类推
    char  szRev[2];   //保留
} aset_mdrc_data_t;





/* 生成的补偿滤波器系数，要传给小机的
** 其中fir_order为滤波器阶数
** fir_coeff_Q为滤波器系数的定点位
** fir_coeff为滤波器系数数组
*/
typedef struct{
    int fir_order;
    int fir_coeff_Q;
    uint16 total_size;
    uint16 cuf_size;
    int8 fir_coeff[ASET_FIR_INFO_LEN];
}aset_FIR_info_t;


typedef struct
{
    aset_FIR_info_t stFirInfo;
    int8 next_stFirInfo_index;      //下一包的计数，为0时表示没有下一包
    int8 szRev[5];
    uint8 index;                    //0 ~ 15 包序号，每个包128字节
    bool bEnFRCurve;
}aset_FRCurve_data_t;


typedef struct
{
    short signalPrecut;             //信号后级衰减微调，范围-3db ~ 0db，单位0.1db，默认值为0db
    short fMVolume;                 //音量微调参数，范围-3db  ～ 3db，step 为.1db，默认为db
    short   nMaxMid;                //中频高频分频点：Hz ～20KHz，默认值为KHz
    short   nMidMin;                //低频中频分频点：Hz ～20KHz，默认值为Hz
    short   limiterThresholdDiff;   //MDRC与Limiter阈值的差值
    int8    szRev[6];               //保留
}aset_mDRC2_data_t;



typedef struct
{
    short nHpCutoffFre;
    short nRev;
    short nStartTVal;
    short nHoldTVal;
    short nEnhanceRange;
}aset_HP_t;//高通结构体


typedef struct
{
    short nLfCutoffFre;
    short nEnhanceVBass;
    short nStartTVal;
    short nHoldTVal;
    short nEnhanceRange;
}aset_LF_t;//低频结构体


typedef struct
{
    short nHfCutoffFre;
    short nEnhanceTE;
    short nStartTVal;
    short nHoldTVal;
    short nEnhanceRange;
}aset_HF_t;//高频结构体


typedef struct
{
    short nFrePoint;
    short nRev;
    short nStartTVal;
    short nHoldTVal;
    short nEnhanceRange;
}aset_FrePoint_t;//任意频点


typedef struct
{
    aset_HP_t stHpWeakenInfo;     //高通减弱
    aset_LF_t stLfEnhanceInfo;    //低频增强
    aset_HF_t stHfEnhanceInfo;    //高频增强
    aset_FrePoint_t arFrePoints[3];  //三个任意频点
    bool bEnableSEE;
    uint8 peak_ratio; //音效增强尖峰比率，以0.1倍为单位
    char szRev[2];
}aset_SEEnhance_data_t;




typedef struct
{
    aset_HP_t stHpEnhanceInfo;     //高通增强
    aset_LF_t stLfWeakenInfo;      //低频减弱
    aset_HF_t stHfWeakenInfo;      //高频减弱
    aset_FrePoint_t arFrePoints[3];   //三个任意频点
    bool bEnableSEW;
    char szRev[3];
}aset_SEWeaken_data_t;


typedef struct
{
    short fSignalDetePer;           //信号检测周期（s ～2s，step 为ms，默认值为.5s）
    short nSignalDetePerNum;        //信号检测周期数（～20，step为，默认为）
    short fSignalDesTVal;           //小信号预降阈值（-1db ～-6db，step为0.1db，默认为-20）
    short nSignalDesPerNum;         //小信号预降周期数（～20，step 为，默认值为）
    short nSignalMinVal;            //信号大小有效最小值( -60   ～ 0，step 为0.1db，默认值为-21db)
    bool  bEnableSD;
    char szRev[5];
}aset_SignalDete_data_t;


typedef struct
{
    int8 bEQ_v_1_0;
    int8 bVBass_v_1_0;
    int8 bTE_v_1_0;
    int8 bSurround_v_1_0;

    int8 bLimiter_v_1_0;
    int8 bMDRC_v_1_0;
    int8 bSRC_v_1_0;
    int8 bSEE_v_1_0;

    int8 bSEW_v_1_0;
    int8 bSD_v_1_0;
    int8 bEQ_v_1_1;
    int8 bMS_v_1_0;
    
    int8 bVBass_v_1_1;
    int8 bTE_v_1_1;

    int8 bEQ_v_1_2;
    int8 bMDRC_v_1_1;
    int8 bComPressor_v_1_0;   
    int8 szRev[119];
    int8 szVerInfo[8];//方案上传的方案名和版本号，统一大写   
}aset_interface_config_t;



typedef struct
{
    int8 peq_point_num;        //支持的PEQ点数
    int8 download_data_over;   //是否已读完数据
    int8 reserved[30];         //保留字节
    aset_interface_config_t stInterface;
} aset_case_info_t;

//兼容标准音效模型增加的结构体
typedef struct
{
    short nThreshold_01;           //阈值，-60::0dB，初始化-15dB
    short   nCpRatio_01;           //压缩比，1~1000  初始化值为
    short nThreshold_02;           //阈值，-60::0dB，初始化-15dB
    short   nCpRatio_02;           //压缩比，1~1000  初始化值为
    short nRmsAvgTime;             //RMS检测平均时间(ms):   0.02~100.00   初始化值为.00
    short fAttack_time;            //启动时间，.02~100ms，传值扩大倍，初始化为.00
    short nRelease_time;           //释放时间，~5000ms，初始化为

    bool  bLmEnable;               //模块使能
    char  nIndex;                  //标志，为aux，为非aux
    char  szRev[16];               //保留
}Compressor_TP_Info_M;

typedef struct
{
    short nThreshold_01;           //阈值，-60::0dB，初始化-15dB
    short   nCpRatio_01;           //压缩比，1~1000  初始化值为
    short nThreshold_02;           //阈值，-60::0dB，初始化-15dB
    short   nCpRatio_02;           //压缩比，1~1000  初始化值为
    short nRmsAvgTime;             //RMS检测平均时间(ms):   0.02~100.00   初始化值为.00
    short fAttack_time;            //启动时间，.02~100ms，传值扩大倍，初始化为.00
    short nRelease_time;           //释放时间，~5000ms，初始化为
}DRC_CpVal_Info_M;

typedef struct
{
    DRC_CpVal_Info_M stDrcVal_M[3];//三段drc
    bool  bDrcEnable;              //DRC模块是否使能
    char cDataFlag;                //添加DRC模块命令二标志，x5A代表还有一组数据需要发送给小机，其他则没有
    short sIndex;                  //添加模组序号，第一组为，以此类推
    char  szRev[14];               //保留 
} DRC_CP_Info_M;

//添加drc命令三结构体
typedef struct
{
    short fSignalPrecut;            //信号后级衰减微调，范围-3db ~ 0db，单位.1db，默认值为db
    short fMVolume;                 //音量微调参数，范围-3db  ～ 3db，step 为.1db，默认为db
    short   nMaxMid;                //中频高频分频点：Hz ～20KHz，默认值为KHz
    short   nMidMin;                //低频中频分频点：Hz ～20KHz，默认值为Hz
    short  nLimiterThresholdDiff;   //MDRC与Limiter阈值的差值

    short fTsVal;                   //补偿滤波器阈值：-60  ～0.0，默认值为
    short fQVal;                    //补偿滤波器Q值：.1  ～0.9，默认值为.1
    short fStTime;                  //补偿滤波器启动时间：.02  ～100ms，默认值为.5
    short nReleaseTime;             //补偿滤波器释放时间：ms   ～5000ms，默认值为
    short fGainCom;                 //增益补偿：        0.0   ～20.0db，默认值为db
    char    szRev[12];              //保留
}DRC_TP_Info_M3;

typedef enum
{
   UNAUX_MODE = 0,
   AUX_MODE   = 1,
}aux_mode_e;

typedef struct
{
   uint8 aux_mode;      //aux_mode_e
   uint8 reserved[7];
}application_properties_t;

extern int32 aset_read_data(uint16 cmd, void *data_buffer, uint32 data_len) __FAR__;
extern int32 aset_cmd_deal(aset_status_t *aset_status) __FAR__;
extern int32 aset_write_case_info(void) __FAR__;
extern int32 aset_upload_sound_param(void) __FAR__;
extern void  aset_set_dae_init(void) __FAR__;
extern int32  __section__(".bank_2")aset_update_audiopp(void);


extern uint8 *g_aset_rw_buffer;


#ifdef SUPPORT_ASET_TEST
extern void  aset_reconnect_deal(void) __FAR__;
extern uint8 get_audiopp_type(void)__FAR__;

#endif

#endif
