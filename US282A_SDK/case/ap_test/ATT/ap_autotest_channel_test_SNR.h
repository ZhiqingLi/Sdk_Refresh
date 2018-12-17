#ifndef __AP_AUTOTEST_CHANNEL_TEST_THD_H
#define __AP_AUTOTEST_CHANNEL_TEST_THD_H

//定义该宏将使用查表法求正、余弦
#define USE_TABLE

//定义做FFT的点数
#define LENGTH  128

//定义这么多点的FFT需要多少个二进制位
//bL=log2(LENGTH)
#define bL 7

#define IN_TYPE  short int
#define OUT_TYPE long int
#define LEN_TYPE long int


typedef long long __int64 ;

#define PI 3.1415926535897932384626433832795

typedef struct
{   
    //计算得到的信噪比数值
    int32 SNR;    
    //幅值最大点
    int32 MaX_Sig_Point;
    //时域最大值
    int32 AmpMax;
    //时域最小值
    int32 AmpMin;
    //频域信号幅度高32bit
    int32 SigHigh;    
    //频域信号幅度低32bit
    int32 SigLow;
    //频域噪声幅度高32bit
    int32 NoiseHigh;  
    //频域噪声幅度低32bit
    int32 NoiseLow;  
}snr_result_t;

extern LEN_TYPE BitRev[LENGTH]; 

//存放实部数据
extern OUT_TYPE Re[LENGTH];    

//存放虚部数据
extern OUT_TYPE Im[LENGTH]; 

extern const short int COS_TABLE[];
extern const short int SIN_TABLE[];

extern void InitBitRev(void);
extern void FftInput(IN_TYPE *pIn);
extern void FftExe(IN_TYPE *pIn, OUT_TYPE *pRe, OUT_TYPE *pIm);

extern uint32 thd_test(void *buffer_addr, channel_test_arg_t *channel_test_arg) __FAR__;

#endif

