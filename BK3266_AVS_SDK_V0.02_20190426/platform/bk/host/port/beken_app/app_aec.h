#ifndef _APP_AEC_H
#define _APP_AEC_H

//#define AEC_FLASH_TEST
//#define AEC_FFT_SW
//#define AEC_STATIS_TEST

//#define AEC_DEBUG
#ifdef AEC_DEBUG
    #define AEC_PRT      os_printf
#else
    #define AEC_PRT      os_null_printf
#endif

#define FRAME_MALLOC

#define MIC_DELAY                       10
#define MIC_GAIN                         1
#define EAR_GAIN                         1

#include "app_equ.h"

#define FRAME_LEN 256
#define W_LEN 96
#define NS_AVERAGE 9 // used for noise suppression
#define NLPOUT_LEN 64
#define LP_MODE 2 // low power mode, 0 normal; 1 mid power mode; 2 low power mode
#define NS_LEVEL 0 // noise suppression 1 (6 dB) or 2 (12 dB) or 3 (18 dB), 0 (no suppression)

#define APP_AEC_AGC_SCALE_BIT   12
#define APP_AEC_AGC_MAX         3 << APP_AEC_AGC_SCALE_BIT

#ifndef ABS
#define ABS(a)    (((a) > 0)?(a):(-a))
#endif

#define f_mshift(din, n)  (((din) > 0)?(din >>n):(-((-din)>>n)))

//AGC part
typedef struct _app_aec_agc_s
{
    uint8 fast_alpha;
    uint8 slow_alpha;
    int32 target;
    uint16 step_fast;
    uint16 step_slow;
    int32  agc_gain;
    uint32 y_amp;
    uint16 cnt;
}app_aec_agc_t;

typedef struct
{
    int16 oldx;
    int16 oldy;
} iir_first_order_state;

typedef struct
{
    int16 sin[W_LEN];
    int16 rin[W_LEN];
    int16 sout[W_LEN];
    int16 sout_pcd[W_LEN];
    int16 w_online[W_LEN];
    int16 w_offline[W_LEN];
    int16 energy_rin;
    int16 energy_sin;
    int16 energy_rnf;
    int16 energy_snf;
    int16 nlms_state;
    int16 stable_cnt;
    int16 pcd_cnt;
    int16 unstable_cnt;
    int64 k;
    int16 update_cnt;

    int tsin_env;
    int tsout_env;
    int tsout_pcd_env;

    uint8  sin_index;
    uint8  rin_index;
    uint8  sout_index;
    uint8  sout_pcd_index;
} lec_state;

typedef struct
{
    SAMPLE_ALIGN uint32 echo_fft_mem[FRAME_LEN];
    SAMPLE_ALIGN int16 Gw_mem[FRAME_LEN];
    SAMPLE_ALIGN int16 nlp_out[FRAME_LEN];
    SAMPLE_ALIGN int16 ifft_mem[FRAME_LEN];

    SAMPLE_ALIGN int16 ear[FRAME_LEN];
    SAMPLE_ALIGN int16 nlms_resi_echo[FRAME_LEN];
    SAMPLE_ALIGN int16 tear[NLPOUT_LEN];
    SAMPLE_ALIGN int16 tres[NLPOUT_LEN];
    int16 nlp_cnt64;

    uint8 nlp_index;
} nlp_state;
typedef struct _mic_analyse_s
{
	int8 flag;
	uint16 mic_max;
}mic_analyse_t;

#define Mic_Delay_Size FRAME_LEN*6
typedef struct
{
    nlp_state  *p_state;

    SAMPLE_ALIGN int16 rin_buffer[Mic_Delay_Size];
    uint16 rin_buffer_rd_ind;
    uint16 rin_buffer_wr_ind;

    int16 mic_gain;
    int16 ear_gain;
    uint8 init_flag;
    SAMPLE_ALIGN int16 nlpout[NLPOUT_LEN];

    driver_ringbuff_t rx_samples;
    driver_ringbuff_t tx_samples;

    uint8 debug_flag;
#ifdef CONFIG_APP_DATA_CAPTURE
    uint8 sync_send_flag;
#endif
#ifdef ADJUST_AEC_DELAY
    uint8 sync_adjusted_flag;
#endif
    uint8 decay_times;
    uint32 ns_level;
    int32 fft_shift;

    mic_analyse_t mic_analyse;
}aec_t;

#ifdef CONFIG_APP_DATA_CAPTURE
	void app_aec_set_data_capture_enable( uint8 enable );
	uint8 app_aec_get_data_capture_mode(void);
#endif
#ifdef ADJUST_AEC_DELAY
void aec_set_sync_adjused_flag(int a);
#endif

typedef   struct __app_aec_cfg_s
{
   uint16  aec_mic_delay;
   int8    aec_fft_shift;
   uint8   aec_decay_time;
   int8    aec_ear_gain;
   int8    aec_mic_gain;
   int16	hfp_eq_gain;
   uint8	hfp_eq_flag0;
   app_eq_para_t hfp_eq_para[2];
} __PACKED_POST__ app_aec_cfg_t;

#ifdef CONFIG_APP_AEC
//void aec_init( uint8 mic_delay, int16 ear_gain, int16 mic_gain );
#ifdef BT_DUALMODE
void RAM_CODE app_aec_swi( void );
void RAM_CODE app_aec_fill_rxbuff( uint8 *buff, uint8 fid,uint16 len );
void RAM_CODE app_aec_fill_txbuff( uint8 *buff, uint16 len );
#else
void DRAM_CODE app_aec_swi( void );
void DRAM_CODE app_aec_fill_rxbuff( uint8 *buff, uint8 fid,uint16 len );
void DRAM_CODE app_aec_fill_txbuff( uint8 *buff, uint16 len );
#endif
void aec_set_latency(uint16 mic_delay);

void app_aec_debug( uint8 enable );
//void app_aec_init( uint8 mic_delay, int16 ear_gain, int16 mic_gain );
void app_aec_init(uint16 mic_delay, 
                    int8 fft_shift,
                    uint8 decay_times,
                    int16 ear_gain, 
                    int16 mic_gain);

void app_aec_uninit(void);
uint16 aec_get_latency(void);
void aec_set_latency(uint16 mic_delay);
void app_set_aec_para(uint8 *para);

#endif

#endif
