#ifndef _DRIVER_AUDIO_H_
#define _DRIVER_AUDIO_H_

#include "app_equ.h"
#include "driver_dma_fft.h"
#include "config.h"
#include "driver_ringbuff.h"

#define ADC_FIRST_DELAY         //to control whether discard the first adc data to avoid openning noise
//#define IS_INTERNAL_PA_OPEN      ((BK3000_A3_CONFIG&(1<<6))? 0:1)

#define PAMUTE_GPIO_PIN         9
#define EXPA_VALID_STATUS       1
#define EXPA_UNVALID_STATUS     0    // High ---Mute

#define MUTE_LOW_THD            2
#define MUTE_HIGH_THD           7

typedef struct _aud_mute_cfg_s
{
    uint8 mute_pin;
    uint8 mute_high_flag;
    uint8 mute_status;
    uint8 backup_mute_status;
    uint8 shade_flag;
    uint8 mute_outside;
    uint8 auto_mute_flag; //自动(快速)静音标识
    uint32 mute_mask;
}aud_mute_cfg_t;
typedef struct aud_volome_s
{
    uint8_t ana_gain;
    uint8_t dig_gain;
}__PACKED_POST__ aud_volume_t;

#define AUDIO_BUFF_LEN     4096  // 4096//3072//2560	/**< 2.5k */
#define PCM_BUFF_LEN    1536	/**< 1k */

#define AUDIO_EMPTY_COUNT_THRE  20

#define AUDIO_DMA_BLOCK_NUM_512     8

enum
{
    DMA_BUF_TYPE_MUSIC = 0,
    DMA_BUF_TYPE_VOICECALL = 1,
    DMA_BUF_TYPE_RING = 2
};

typedef struct
{
    uint8 data_buff[AUDIO_BUFF_LEN];
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
    struct dma_struct aud_dmaconf;
#endif
    driver_ringbuff_t   aud_rb;
    int   empty_count;
    int   channels;
#ifdef BEKEN_DEBUG
    int aud_empty_count;
#endif
}AUDIO_CTRL_BLK;

typedef struct
{
    uint8 data_buff[PCM_BUFF_LEN];
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
#endif
    driver_ringbuff_t   aud_rb;
    int   empty_count;
    int   channels;
#ifdef BEKEN_DEBUG
    int aud_empty_count;
#endif
}PCM_CTRL_BLK;



#define AUDIO_VOLUME_MAX        16

#define AUDIO_DIV_441K          0x049B2368
#define AUDIO_DIV_441K_SLOW     0x049B2970
#define AUDIO_DIV_441K_FAST     0x049B1D5C

#define AUDIO_DIV_48K           0x043B5554
#define AUDIO_DIV_48K_SLOW      0x043B5AE0
#define AUDIO_DIV_48K_FAST      0x043B4FC8

#define AUDIO_CLK_DIV_441K   2
#define AUDIO_CLK_DIV_48K     3
#define AUDIO_CLK_DIV_8K       0
#if(CONFIG_APP_MSBC_RESAMPLE == 1)
#define AUDIO_CLK_DIV_16K     0
#else
#define AUDIO_CLK_DIV_16K     1
#endif

#define AUDIO_DIV_8K    0x06590000
#define AUDIO_DIV_16K   0x06590000

#ifdef CONFIG_TWS
#define AUDIO_SYNC_INTVAL 		3000

#define AUDIO_DIV_COVER 		1
#define AUDIO_DIV_441K_Dot ((175*AUDIO_SYNC_INTVAL/10000)*AUDIO_DIV_COVER)

#define AUDIO_DIV_48K_Dot ((142*AUDIO_SYNC_INTVAL/10000)*AUDIO_DIV_COVER)

#define AUDIO_DIV_INIT ((AUDIO_DIV_441K_SLOW-AUDIO_DIV_441K)/AUDIO_DIV_441K_Dot)
#define AUDIO_DIV_MAX (AUDIO_DIV_INIT*3)

#endif

/// REG0x0
typedef union {
    struct {
        volatile unsigned int samprate   : 2;  /**< 采样率配置 0：8K 1：16K 2：44.1K 3：48K */
        volatile unsigned int dacenable  : 1;  /**< DAC使能 1：使能 */
        volatile unsigned int adcenable  : 1;  /**< ADC使能 1：使能 */
        volatile unsigned int dtmfenable : 1;  /**< LINE IN使能 1：使能 */
        volatile unsigned int lineenable : 1;  /**< DTMF使能 1: 使能 */
        volatile unsigned int Reserved   : 26; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x0_TypeDef;

/// REG0x1
typedef union {
    struct {
        volatile unsigned int tonepattern      : 1;  /**< 1：Tone1(Active_Time) + Tone2(Pause_Time) 0: DTMF(Active_Time) + Zeros(Pause_Time) */
        volatile unsigned int tonemode         : 1;  /**< 1：连续模式， 0：单次模式 */
        volatile unsigned int tonepausetime    : 4;  /**< DTMF的间歇时间 N*10 ms */
        volatile unsigned int toneactivetime   : 4;  /**< DTMF的工作时间 N*10 ms */
        volatile unsigned int Reserved         : 22; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x1_TypeDef;

/// REG0x2
typedef union {
    struct {
        volatile unsigned int tone1step    : 16; /**< Tone1的频率设置 Step = F(KHz) * 8191 */
        volatile unsigned int tone1attu    : 4;  /**< Tone1的衰减设置 0 ：-1 ：-15 dB */
        volatile unsigned int tone1enable  : 1;  /**< Tone1的使能设置 */
        volatile unsigned int Reserved     : 11; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x2_TypeDef;

/// REG0x3
typedef union {
    struct {
        volatile unsigned int tone2step    : 16; /**< Tone2的频率设置 Step = F(KHz) * 8191 */
        volatile unsigned int tone2attu    : 4;  /**< Tone2的衰减设置 0 ：-1 ：-15 dB */
        volatile unsigned int tone2enable  : 1;  /**< Tone2的使能设置 */
        volatile unsigned int Reserved     : 11; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x3_TypeDef;

/// REG0x4
typedef union {
    struct {
        volatile unsigned int adchpf2coefB2    : 16; /**< ADC HPF2的系数 B2 */
        volatile unsigned int adchpf2bypass    : 1;  /**< ADC HPF2禁能位，1：绕开HPF2 */
        volatile unsigned int adchpf1bypass    : 1;  /**< ADC HPF1禁能位，1：绕开HPF1 */
        volatile unsigned int adcsetgain       : 6;  /**< ADC 设置的增益 */
        volatile unsigned int adcsampeedge     : 1;  /**< ADC数据采样的时钟边沿选择 1：下降沿，0：上升沿 */
        volatile unsigned int Reserved         : 7;  /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x4_TypeDef;

/// REG0x5
typedef union {
    struct {
        volatile unsigned int adchpf2coefB0 : 16; /**< ADC HPF2的系数 B2 */
        volatile unsigned int adchpf2coefB1 : 16; /**< ADC HPF2的系数 B1 */
    };
    volatile unsigned int value;
} AUDIO_REG0x5_TypeDef;

/// REG0x6
typedef union {
    struct {
        volatile unsigned int adchpf2coefA0 : 16; /**< ADC HPF2的系数 A0 */
        volatile unsigned int adchpf2coefA1 : 16; /**< ADC HPF2的系数 A1 */
    };
    volatile unsigned int value;
} AUDIO_REG0x6_TypeDef;

/// REG0x7
typedef union {
    struct {
        volatile unsigned int dachpf2coefB2    : 16; /**< DAC HPF2的系数 B2 */
        volatile unsigned int dachpf2bypass    : 1;  /**< DAC HPF2禁能位，1：绕开HPF2 */
        volatile unsigned int dachpf1bypass    : 1;  /**< DAC HPF1禁能位，1：绕开HPF1 */
        volatile unsigned int dacsetgain       : 6;  /**< DAC 设置的增益 */
        volatile unsigned int dacclkinvert     : 1;  /**< DAC 输出时钟边沿选择 0：下降沿，1：上升沿 */
        volatile unsigned int Reserved         : 7;  /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x7_TypeDef;

/// REG0x8
typedef union {
    struct {
        volatile unsigned int dachpf2coefB0 : 16; /**< DAC HPF2的系数 B2 */
        volatile unsigned int dachpf2coefB1 : 16; /**< DAC HPF2的系数 B1 */
    };
    volatile unsigned int value;
} AUDIO_REG0x8_TypeDef;

/// REG0x9
typedef union {
    struct {
        volatile unsigned int dachpf2coefA0 : 16; /**< DAC HPF2的系数 A0 */
        volatile unsigned int dachpf2coefA1 : 16; /**< DAC HPF2的系数 A1 */
    };
    volatile unsigned int value;
} AUDIO_REG0x9_TypeDef;

/// REG0xA
typedef union {
    struct {
        volatile unsigned int dacrrdthreshold : 5; /**< DAC右声道读FIFO时，数目低于该门限，发出中断请求 */
        volatile unsigned int daclrdthreshold : 5; /**< DAC左声道读FIFO时，数目低于该门限，发出中断请求 */
        volatile unsigned int dtmfwrthreshold : 5; /**< DTMF自动写FIFO时，数目高于该门限，发出中断请求 */
        volatile unsigned int adclwrthreshold : 5; /**< ADC自动写FIFO时，数目高于该门限，发出中断请求 */
        volatile unsigned int dacrinten       : 1; /**< DAC右声道中断请求使能位 */
        volatile unsigned int daclinten       : 1; /**< DAC左声道中断请求使能位 */
        volatile unsigned int dtmfinten       : 1; /**< DTMF中断请求使能位 */
        volatile unsigned int adclinten       : 1; /**< ADC中断请求使能位 */
        volatile unsigned int loopton2dac     : 1; /**< DTMF到DAC环回测试使能位 */
        volatile unsigned int loopadc2dac     : 1; /**< ADC到DAC环回测试使能位 */
        volatile unsigned int Reserved        : 6; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0xA_TypeDef;

/// REG0xB
typedef union {
    struct {
        volatile unsigned int agcnoisethrd : 10; /**< noise gating 执行时刻的门限，noise gating要在audio值比较小的时候执行 */
        volatile unsigned int agcnoisehigh : 10; /**< 对应MIC_RSSI低10 bit */
        volatile unsigned int agcnoiselow  : 10; /**< 对应MIC_RSSI低10 bit，比如MIC_RSSI[15:10]任何一位为1，都说明信号电平超过NOISE_LOW */
        volatile unsigned int agcstep10    : 2;  /**< 没有用 */
    };
    volatile unsigned int value;
} AUDIO_REG0xB_TypeDef;

/// REG0xC
typedef union {
    struct {
        volatile unsigned int agcnoisemin    : 7; /**< {GAIN2[2:0] GAIN1[3:0]} when signal level below NOISE_LOW, when noise gating is enabled */
        volatile unsigned int agcnoisetout   : 3; /**< 1: 4 (~0.5 ms), 2: 8, 3: 16, 4: 32, 5: 64, 6: 128, 7: 256 (~32 ms) */
        volatile unsigned int agchighdur     : 3; /**< 0: 0 (0 ms，一旦发生MIC_RSSI超过NOISE_HIGH，立刻触发Noise gating，离开noise状态);1: 8 (~1 ms), 2: 16, 3: 32, 4: 64, 5: 128, 6: 256, 7: 256 (~32 ms) */
        volatile unsigned int agclowdur      : 3; /**< 0: 0 (0 ms，一旦发生MIC_RSSI小于NOISE_LOW，立刻触发noise gating);1: 8 (~1 ms), 2: 16, 3: 32, 4: 64, 5: 128, 6: 256, 7: 256 (~32 ms) */
        volatile unsigned int agcmin         : 7; /**< Minimum value of {GAIN2[2:0] GAIN1[3:0]} */
        volatile unsigned int agcmax         : 7; /**< Maximum value of {GAIN2[2:0] GAIN1[3:0]};Also the default gain setting when AGC is disabled */
        volatile unsigned int agcngmethod    : 1; /**< 0: 直接mute或者demute (default);1: 逐渐降低增益或者增加增益 */
        volatile unsigned int agcngenable    : 1; /**< 使能 noise gating */
    };
    volatile unsigned int value;
} AUDIO_REG0xC_TypeDef;

/// REG0xD
typedef union {
    struct {
        volatile unsigned int agcdecaytime    : 3; /**< 0: 128 (~2.7 ms), 1: 256, 2: 512, 3: 1024, 4:2048, 5: 4096, 6: 8192, 7: 16384 (~340 ms) */
        volatile unsigned int agcattacktime   : 3; /**< 0: 8 (~0.1667ms), 1: 16, 2: 32, 3: 64, 4: 128, 5: 256, 6: 512, 7: 1024 (~21 ms) */
        volatile unsigned int agchighthrd     : 5; /**< 对应到MIC_RSSI的最高5位 */
        volatile unsigned int agclowthrd      : 5; /**< 对应到MIC_RSSI的最高5位 */
        volatile unsigned int agciircoef      : 3; /**< IIR系数选择 0:1/32 1:1/64 2:1/128 3:1/256 4:1/512 5:1/1024 6:1/2048 7:1/4096 */
        volatile unsigned int agcenable       : 1; /**< AGC使能位 */
        volatile unsigned int manualpgavalue  : 7; /**< 手动PGA的值 */
        volatile unsigned int manualpga       : 1; /**< 手动PGA模式 */
        volatile unsigned int Reserved        : 4; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0xD_TypeDef;

/// REG0xE
typedef union {
    struct {
        volatile unsigned int dacrnearfull  : 1;  /**< DAC右声道FIFO快要满 （默认大于3/4的容量） */
        volatile unsigned int daclnearfull  : 1;  /**< DAC左声道FIFO快要满 */
        volatile unsigned int adclnearfull  : 1;  /**< ADC左声道FIFO快要满 */
        volatile unsigned int dtmfnearfull  : 1;  /**< DTMF的FIFO快要满 */
        volatile unsigned int dacrnearempty : 1;  /**< DAC右声道FIFO快要空 （默认小于1/4的容量） */
        volatile unsigned int daclnearempty : 1;  /**< DAC左声道FIFO快要空 */
        volatile unsigned int adclnearempty : 1;  /**< ADC左声道FIFO快要空 */
        volatile unsigned int dtmfnearempty : 1;  /**< DTMF的FIFO快要空 */
        volatile unsigned int dacrfifofull  : 1;  /**< DAC右声道FIFO已满 (达到FIFO容量的最大值） */
        volatile unsigned int daclfifofull  : 1;  /**< DAC左声道FIFO已满 */
        volatile unsigned int adclfifofull  : 1;  /**< ADC左声道FIFO已满 */
        volatile unsigned int dtmffifofull  : 1;  /**< DTMF的FIFO已满 */
        volatile unsigned int dacrfifoempty : 1;  /**< DAC右声道FIFO已空 (达到FIFO容量的最小值） */
        volatile unsigned int daclfifoempty : 1;  /**< DAC左声道FIFO已空 */
        volatile unsigned int adclfifoempty : 1;  /**< ADC左声道FIFO已空 */
        volatile unsigned int dtmffifoempty : 1;  /**< DTMF的FIFO已空 */
        volatile unsigned int dacrintflag   : 1;  /**< DAC右声道中断标志 */
        volatile unsigned int daclintflag   : 1;  /**< DAC左声道中断标志 */
        volatile unsigned int adclintflag   : 1;  /**< ADC左声道中断标志 */
        volatile unsigned int dtmfintflag   : 1;  /**< DTMF的中断标志 */
        volatile unsigned int Reserved      : 12; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0xE_TypeDef;

/// REG0xF
typedef union {
    struct {
        volatile unsigned int rssi    : 8;  /**< Absolutely microphone signal power in dB unit; Read only */
        volatile unsigned int micpga  : 8;  /**< {1 b0, GAIN2[2:0] GAIN1[3:0]} (Read only) */
        volatile unsigned int micrssi : 16; /**< Microphone level; MIC_RSSI[15:0]; Read only */
    };
    volatile unsigned int value;
} AUDIO_REG0xF_TypeDef;

/// REG0x10
typedef union {
    struct {
        volatile unsigned int dtmfport : 16; /**< DTMF的FIFO读出地址 */
        volatile unsigned int Reserved : 16; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x10_TypeDef;

/// REG0x11
typedef union {
    struct {
        volatile unsigned int adcport  : 16; /**< ADC的FIFO读出地址 */
        volatile unsigned int Reserved : 16; /**< Reserved */
    };
    volatile unsigned int value;
} AUDIO_REG0x11_TypeDef;

/// REG0x12
typedef union {
    struct {
        volatile unsigned int daclport : 16; /**< DACL的FIFO写入地址 */
        volatile unsigned int dacrport : 16; /**< DACR的FIFO写入地址 */
    };
    volatile unsigned int value;
} AUDIO_REG0x12_TypeDef;

/// AUDIO
struct AUDIO_TypeDef {
    volatile AUDIO_REG0x0_TypeDef  *r0;
    volatile AUDIO_REG0x1_TypeDef  *r1;
    volatile AUDIO_REG0x2_TypeDef  *r2;
    volatile AUDIO_REG0x3_TypeDef  *r3;
    volatile AUDIO_REG0x4_TypeDef  *r4;
    volatile AUDIO_REG0x5_TypeDef  *r5;
    volatile AUDIO_REG0x6_TypeDef  *r6;
    volatile AUDIO_REG0x7_TypeDef  *r7;
    volatile AUDIO_REG0x8_TypeDef  *r8;
    volatile AUDIO_REG0x9_TypeDef  *r9;
    volatile AUDIO_REG0xA_TypeDef  *r10;
    volatile AUDIO_REG0xB_TypeDef  *r11;
    volatile AUDIO_REG0xC_TypeDef  *r12;
    volatile AUDIO_REG0xD_TypeDef  *r13;
    volatile AUDIO_REG0xE_TypeDef  *r14;
    volatile AUDIO_REG0xF_TypeDef  *r15;
    volatile AUDIO_REG0x10_TypeDef *r16;
    volatile AUDIO_REG0x11_TypeDef *r17;
    volatile AUDIO_REG0x12_TypeDef *r18;
};

extern struct AUDIO_TypeDef AUDIO;
uint16 RAM_CODE pcm_get_buffer_size(void);
int aud_initial(uint32 freq, uint32 channels, uint32 bits_per_sample);
int aud_adc_initial(uint32 freq, uint32 channels, uint32 bits_per_sample);
void aud_open( void );
int aud_close(void);
void aud_mute_init( void );
// add begin by cai.zhong 20190227 for get pcm ring buffer
driver_ringbuff_t *get_pcm_ringbuf(void);
uint8 *get_pcm_buffer_base(void);
// add end by cai.zhong 20190227 for get pcm ring buffer
#ifdef BT_DUALMODE
void RAM_CODE pcm_fill_buffer( uint8 *buff, uint16 size );
void RAM_CODE cvsd_fill_buffer(uint8 *buff, uint8 fid,uint16 size );
void RAM_CODE aud_isr(int mode_sel );
void RAM_CODE aud_fill_buffer( uint8 *buff, uint16 size );
uint8 RAM_CODE aud_discard_sco_data(void);
uint16 RAM_CODE aud_get_buffer_size(void);
#else
void DRAM_CODE pcm_fill_buffer( uint8 *buff, uint16 size );
void DRAM_CODE cvsd_fill_buffer(uint8 *buff, uint8 fid,uint16 size );
void DRAM_CODE aud_isr(int mode_sel );
void DRAM_CODE aud_fill_buffer( uint8 *buff, uint16 size );
uint8 DRAM_CODE aud_discard_sco_data(void);
uint16 DRAM_CODE aud_get_buffer_size(void);
#endif

#if(CONFIG_AUD_FADE_IN_OUT == 1)
typedef enum
{
    AUD_FADE_NONE       = 0,
    AUD_FADE_IN         = 1,
    AUD_FADE_OUT        = 2,
    AUD_FADE_FINISHED   = 4
}t_aud_fade_state;
__INLINE__ void  set_aud_fade_in_out_state(t_aud_fade_state state);
__INLINE__ t_aud_fade_state get_aud_fade_in_out_state(void);
void aud_fade_in_out_process(void);
#endif
void aud_volume_mute( uint8 enable );
void aud_volume_set( int8 volume );
void aud_clr_PAmute(void);
void aud_volume_mute_ana( uint8 enable );
void BK3000_Ana_dac_enable_delay( uint8 enable );
#if(CONFIG_ANA_DAC_CLOSE_IN_IDLE == 1)
void BK3000_dig_dac_close(void);
void aud_ana_dac_close_in_idle(void);
#endif
void aud_PAmute_oper( int enable );

void aud_mic_open( int enable );
void aud_mic_volume_set( uint8 volume );
int8 aud_min_volume_get( void );
void aud_mute_update( int16 samplel, int16 sampler );
void aud_mic_mute( uint8 enable );
#if (CONFIG_AUDIO_TRANSFER_DMA == 1)
void aud_dma_initial(void);
void aud_dma_start(void);
void aud_dma_stop(void);
void aud_dma_reset(void);
#endif

#define AUDIO_VOLUME_MIN    0//aud_min_volume_get()

#define AUDIO_MAX_FILT_NUM      5

void aud_filt_enable( uint8 index, uint8 enable );
int aud_filt_conf( uint8 index, app_eq_para_t *conf );
void RAM_CODE wav_fill_buffer( uint8 *buff, uint16 size );
void dac_clk_adjust( int mode );

void dac_init_buf(void);
#ifdef CONFIG_TWS
typedef struct{
	union {
	    struct sync_start_s{
			uint32 start_time : 28;
			uint32 flag : 4;
			uint32 clk_val :28;
			uint32 vol :4;
	    } __PACKED_POST__ sync_start;

	    struct sync_send_s{
			uint32 bt_clk : 28;
			uint32 clk_mode : 4;
			uint32 aud_num;
	    } __PACKED_POST__ sync_send;
	}u;
	int16 aud_num_tmp;
}__PACKED_POST__ sync_data_TypeDef;
extern sync_data_TypeDef sync_data;
void RAM_CODE dac_set_clk( uint32_t clk_val);
void dac_init_clk(void);
void show_dac_clk(int pos);
void RAM_CODE dac_clk_adjust_tws( int mode );

#ifdef TWS_CONFIG_LINEIN_BT_A2DP_SOURCE

typedef struct _driver_lineindata_s
{
    driver_ringbuff_t   data_rb;
}DRIVER_LINEINDATA_T;
DRIVER_LINEINDATA_T linein_data_blk;

void  aud_linein_fill_buffer( uint8 *buff, uint16 size );
driver_ringbuff_t *get_line_ringbuf(void);
void line_in_fill_aud_buf(uint8 *buff, uint16 size );

#endif


#endif


#ifdef CONFIG_APP_AEC
void RAM_CODE app_aec_adj_latency(int rx_size,int tx_size);
#endif
void audio_dac_analog_init(uint32_t type);
void audio_dac_analog_gain_set(uint32_t gain, uint32_t dc_offset_l, uint32_t dc_offset_r);
void audio_dac_analog_enable(uint32_t enable);
#if (CONFIG_PRE_EQ == 1)
void app_set_pre_eq_gain(uint8 *para);
void app_set_pre_eq(uint8 *para);
void app_show_pre_eq(void);

typedef struct _aud_pre_equ_para_s
{
    int    a[2];
    int    b[3];
}__PACKED_POST__ aud_pre_equ_para_t;

typedef struct _aud_pre_equ_s
{
	int online_flag;
	int totle_EQ;
	uint32 globle_gain;
}__PACKED_POST__ aud_pre_equ_t;
#endif

#if (CONFIG_APP_MSBC_RESAMPLE == 1)
#define NFIR_BT		65
#define N_IIR	    13

typedef struct fir_bt_state {
	int16_t ratio;
	int16_t taps;
	int16_t curr_pos;
	int16_t *coef;
	int16_t laststate[NFIR_BT];
}t_FIR_STATE,*pFIR_BT_STATE;
typedef struct
{
	int16_t *b;
	int16_t *a;
	int16_t y[N_IIR];
	int16_t x[N_IIR];
}t_IIR_State;

int16_t fir_hardcore_init(void);
int16_t fir_hardcore_close(void);
int16_t fir_hardcore_filter(int16_t *smpl,uint16_t nSmpls);
int16_t FIR_sw_filter_init(pFIR_BT_STATE p_fir_st);
int16_t FIR_sw_filter_exe(pFIR_BT_STATE p_fir_st,int16_t *in,int16_t *out,int16_t len);
void IIR_filter_init(t_IIR_State *st);
void IIR_filter_exe(t_IIR_State *st,int16_t *x,int16_t *y,int16_t len);
#endif
#endif
