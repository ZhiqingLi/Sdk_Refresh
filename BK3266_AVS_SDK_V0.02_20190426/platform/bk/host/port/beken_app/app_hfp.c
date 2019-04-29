#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#ifdef CONFIG_APP_HALFDUPLEX

/**********************************************************************
 *
 * echo erase part
 *
 ***********************************************************************/
#define ECHO_RX_BUFFER_SIZE     256
#define ECHO_TX_BUFFER_SIZE     128

static char __attribute__((aligned(2))) echo_rx_buff[ECHO_RX_BUFFER_SIZE];
static char __attribute__((aligned(2))) echo_tx_buff[ECHO_TX_BUFFER_SIZE]; 

typedef struct _app_echo_erase_s
{
    int32   v_ts_n_1;
    int32   v_rs_n_1;
    int32   v_rs_n_2;
    int32   v_tn_n_1;
    int32   v_rn_n_1;
    int32   v_tn_min;
    int32   v_rn_min;
    int16  noise_tn_interval;
    int16  noise_rn_interval;
    uint16  hold;
    uint8   state_changing[2];
    uint8   state;
    uint8   attenuation_mode;
    uint16 attenuation_interval;
    uint8   attenuation_rx;
    uint8   attenuation_tx;
    uint8   attenuation_count;
    uint8   flag_snr_tx;
    uint8   flag_snr_rx;
    uint16  samples_to_send;
    driver_ringbuff_t  tx_samples;
    driver_ringbuff_t  rx_samples;
}app_echo_erase_t;


#define APP_HFP_A_S     4
#define APP_HFP_A_S1    7
#define APP_HFP_A_N     5
#define APP_HFP_NOISE_DETECT_INTERVAL_IDLE   3200
#define APP_HFP_HOLD_TIME                   600
#define APP_HFP_A_CSPN_TX         4
#define APP_HFP_ST                  1024
#define APP_HFP_FT                  2
#define APP_HFP_A_CPTR                1
#define APP_HFP_HYT_RX         16
#define APP_HFP_HYT_TX          8


enum {
    APP_HFP_ECHO_STATE_IDLE = 0,
    APP_HFP_ECHO_STATE_RX,
    APP_HFP_ECHO_STATE_TX
};

static app_echo_erase_t app_echo_erase;
static app_echo_cfg_t *app_echo_cfg = NULL;

static uint8 tx_gain[64] = { 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5,
                           5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
                           19, 21, 23, 26, 28, 31, 34, 37, 41, 45, 50, 55, 60, 64};

static void app_hfp_agc_update(int16 sample);

#ifdef BEKEN_DEBUG
static uint8  app_echo_debug = 0;
#endif

#ifndef ABS
#define ABS(a)    (((a) > 0)?(a):(-a))
#endif

static __inline int16 app_hfp_clip16(int32 s)
{
	if (s > 0x7FFF)
		return 0x7FFF;
	else if (s < -0x8000)
		return -0x8000;
	else
		return s;
}

static void app_hfp_echo_default_para(app_echo_cfg_t * cfg)
{
    cfg->used = 0;
    cfg->hfp_a_s = APP_HFP_A_S;
    cfg->hfp_a_n = APP_HFP_A_N;
    cfg->hfp_noise_interval = APP_HFP_NOISE_DETECT_INTERVAL_IDLE;
    cfg->hold_time = APP_HFP_HOLD_TIME;
    cfg->hfp_a_cspn_tx = APP_HFP_A_CSPN_TX;
    cfg->st = APP_HFP_ST;
    cfg->ft = APP_HFP_FT;
    cfg->hfp_a_cptr = APP_HFP_A_CPTR;
    cfg->hfp_hyt_rx = APP_HFP_HYT_RX;
    cfg->hfp_hyt_tx = APP_HFP_HYT_TX;

    return;
}

void app_hfp_echo_cfg_ptr_set(app_echo_cfg_t * ptr)
{
    app_echo_cfg = ptr;

    if((app_echo_cfg == NULL) || (app_echo_cfg->used != 0x01))
        app_hfp_echo_default_para(app_echo_cfg);

    return;
}

void app_hfp_echo_erase_init(void)
{
    memset(&app_echo_erase, 0, sizeof(app_echo_erase_t));
    rb_init(&app_echo_erase.rx_samples, (uint8 *)echo_rx_buff, ECHO_RX_BUFFER_SIZE);
    rb_init(&app_echo_erase.tx_samples, (uint8 *)echo_tx_buff, ECHO_TX_BUFFER_SIZE);
    app_echo_erase.v_rn_min = 0x7FFFFFFF;
    app_echo_erase.v_tn_min = 0x7FFFFFFF;
    app_echo_erase.state = APP_HFP_ECHO_STATE_IDLE;
    app_echo_erase.attenuation_rx = 65 - tx_gain[32];
    app_echo_erase.attenuation_tx = tx_gain[32];  
    
    return;
}

void app_hfp_echo_fill_rxbuff(uint8 *buff, uint16 len)
{
    if(app_echo_cfg->used == 0x01)
        rb_fill_buffer(&app_echo_erase.rx_samples, buff, len, AEC_RX_FILL);
    else
    {
        if(! bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
        {
            pcm_fill_aud_buff(buff, 16*2);
        }
    }
    return;
}

void app_hfp_echo_fill_txbuff(uint8 *buff, uint16 len)
{
    if(app_echo_cfg->used == 0x01)
    {
        rb_fill_buffer(&app_echo_erase.tx_samples, buff, len, AEC_TX_FILL);
    }
    else    
        pcm_fill_buffer(buff, len);
    return;
}

static void app_hfp_transfer_buffer(int16 *tx_samples, int16 *rx_samples)
{
    int i;

    if(app_echo_erase.attenuation_mode >= 5)
    {
        app_echo_erase.attenuation_interval += app_echo_erase.samples_to_send;
        while(app_echo_erase.attenuation_interval >= app_echo_cfg->st)
        {
            app_echo_erase.attenuation_interval -= app_echo_cfg->st;
            if(app_echo_erase.attenuation_mode == 5) // tx->idle
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 31)
                {
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_tx = tx_gain[32];
                    app_echo_erase.attenuation_rx = 65 - tx_gain[32];
                }
                else
                {
                    app_echo_erase.attenuation_rx = 65 - tx_gain[64 - app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_tx = tx_gain[64 - app_echo_erase.attenuation_count];
                 }
            }
            else if(app_echo_erase.attenuation_mode == 6) // rx->idle
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 32)
                {
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_rx = 65 - tx_gain[32];
                    app_echo_erase.attenuation_tx = tx_gain[32];
                }
                else
                {
                    app_echo_erase.attenuation_tx = tx_gain[app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_rx = 65 - tx_gain[app_echo_erase.attenuation_count];
                 }
            }
        }
 
    }
    else if(app_echo_erase.attenuation_mode > 0)
    {
        app_echo_erase.attenuation_interval += app_echo_erase.samples_to_send;
        while(app_echo_erase.attenuation_interval >= app_echo_cfg->ft)
        {
            app_echo_erase.attenuation_interval -= app_echo_cfg->ft;
            if(app_echo_erase.attenuation_mode == 1)  //  tx->rx
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 64)
                {
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_tx = 1;
                    app_echo_erase.attenuation_rx = 64;
                }
                else
                {
                    app_echo_erase.attenuation_rx = 65 - tx_gain[64 - app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_tx = tx_gain[64 - app_echo_erase.attenuation_count];
                 }
            }
            else if(app_echo_erase.attenuation_mode == 2) // rx->tx
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 64)
                {
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_rx = 1;
                    app_echo_erase.attenuation_tx = 64;
                }
                else
                {
                    app_echo_erase.attenuation_tx = tx_gain[app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_rx = 65 - tx_gain[app_echo_erase.attenuation_count];
                 }
            }
            else if(app_echo_erase.attenuation_mode == 3)  //  idle->rx
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 32)
                {
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_tx = 1;
                    app_echo_erase.attenuation_rx = 64;
                }
                else
                {
                    app_echo_erase.attenuation_rx = 65 - tx_gain[32 - app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_tx = tx_gain[32 - app_echo_erase.attenuation_count];
                 }
            }
            else if(app_echo_erase.attenuation_mode == 4) // idle->tx
            {
                app_echo_erase.attenuation_count++;
                if(app_echo_erase.attenuation_count == 31)
                {
                    app_echo_erase.attenuation_mode = 0;
                    app_echo_erase.attenuation_count = 0;
                    app_echo_erase.attenuation_rx = 1;
                    app_echo_erase.attenuation_tx = 64;
                }
                else
                {
                    app_echo_erase.attenuation_tx = tx_gain[32 + app_echo_erase.attenuation_count];
                    app_echo_erase.attenuation_rx = 65 - tx_gain[32 + app_echo_erase.attenuation_count];
                 }
            }
        }
    }

    /*tx data */
    {
        int ts, rs;
        int16 tsm[16], rsm[16];
        
        for(i = 0; i < app_echo_erase.samples_to_send; i++)
        {
            ts = /*((tx_samples[i]*app_echo_erase.attenuation_tx) >> 4)
                +*/ ((tx_samples[i]*app_echo_erase.attenuation_tx) >> 5);
            tsm[i] = app_hfp_clip16(ts);
            app_hfp_agc_update(tsm[i]);
            rs = (rx_samples[i]*app_echo_erase.attenuation_rx) >> 6;
            rsm[i] = app_hfp_clip16(rs);
        }

        pcm_fill_buffer((uint8 *)tsm, 
            app_echo_erase.samples_to_send*sizeof(int16));

        if(! bt_flag1_is_set(APP_FLAG_WAVE_PLAYING))
        {
            pcm_fill_aud_buff((uint8 *)rsm, app_echo_erase.samples_to_send*2);
        }
    }

}

static __inline void app_hfp_echo_speech_evenlop_tracking(int *v_sl, int sample, int hfp_a_s)
{
    int v_s;

    v_s = (*v_sl << hfp_a_s) - *v_sl + ABS(sample);
    *v_sl = v_s >> hfp_a_s;
}

static __inline void app_hfp_echo_noise_calc(int v_s, int *v_nl, int *v_minl, int16 *interval)
{
    int v_n;

    if(v_s < *v_minl)
          *v_minl = v_s;
    
    (*interval)++;

    if(*interval >= app_echo_cfg->hfp_noise_interval)
    {
        //os_printf("v_nl: %d, v_min: %d\r\n", *v_nl, *v_minl); 
        //v_n = *v_nl - (*v_nl >> app_echo_cfg->hfp_a_n) + (*v_minl  >> app_echo_cfg->hfp_a_n);
        v_n = (*v_nl << app_echo_cfg->hfp_a_n) + *v_minl - *v_nl;
        //os_printf("*v_nl : %d, v_n: %d, v_min: %d\r\n", *v_nl, v_n, *v_minl); 
        *v_nl = v_n >> app_echo_cfg->hfp_a_n;
        *interval = 0;
        *v_minl = 0x7FFFFFFF;
    }
}

void app_hfp_echo_erase(void)
{
    int32 i, v_tx, v_rx; 
    uint8  hyt_flag = 0, noise_calc_flag;
    uint16 samples;
    int16 rx_samples[16];
    int16 tx_samples[16];

    if(!(bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP) 
        && bt_flag1_is_set(APP_FLAG_SCO_CONNECTION)) || app_echo_cfg->used != 0x01)
        return;


    if(app_echo_erase.state == APP_HFP_ECHO_STATE_IDLE) 
        noise_calc_flag = 0x03;
    else if(app_echo_erase.state == APP_HFP_ECHO_STATE_TX)
        noise_calc_flag = 0x03;
    else
        noise_calc_flag = 0x02;


    samples = ECHO_RX_BUFFER_SIZE - rb_get_buffer_size(&app_echo_erase.rx_samples);
    if((ECHO_TX_BUFFER_SIZE - rb_get_buffer_size(&app_echo_erase.tx_samples)) < samples)
        samples = ECHO_TX_BUFFER_SIZE - rb_get_buffer_size(&app_echo_erase.tx_samples);
    if(samples > 32)
        samples = 32;

    app_echo_erase.samples_to_send = (samples >> 1);
    
    for(i = 0; i < app_echo_erase.samples_to_send; i++)
    {
        int16 tx_sample, rx_sample;
        int  v_rs, v_ts;

        rb_get_one_sample(&app_echo_erase.tx_samples, (uint8 *)&tx_sample);
        rb_get_one_sample(&app_echo_erase.rx_samples, (uint8 *)&rx_sample);
        tx_samples[i] = tx_sample;
        rx_samples[i] = rx_sample;
        // 1. speech evenlop track
        app_hfp_echo_speech_evenlop_tracking(&app_echo_erase.v_ts_n_1, tx_sample, app_echo_cfg->hfp_a_s);
        app_hfp_echo_speech_evenlop_tracking(&app_echo_erase.v_rs_n_1, rx_sample, app_echo_cfg->hfp_a_s);
        //app_hfp_echo_speech_evenlop_tracking(&app_echo_erase.v_rs_n_2, app_echo_erase.v_rs_n_1, 7);

        v_ts = app_echo_erase.v_ts_n_1;
        /*if(app_echo_erase.state == APP_HFP_ECHO_STATE_RX)
            v_rs = app_echo_erase.v_rs_n_2;
        else*/
        v_rs = app_echo_erase.v_rs_n_1;
        
        // 2. noise estimation
        
        if(noise_calc_flag & 0x01)  //tx
        {
            app_hfp_echo_noise_calc(v_ts,
                &app_echo_erase.v_tn_n_1, &app_echo_erase.v_tn_min, 
                &app_echo_erase.noise_tn_interval);
        }

        if(noise_calc_flag & 0x02) // rx
        {
            app_hfp_echo_noise_calc(v_rs,
                &app_echo_erase.v_rn_n_1, &app_echo_erase.v_rn_min, 
                &app_echo_erase.noise_rn_interval);
        }

        // 3. S/N CP
        if(v_ts > app_echo_erase.v_tn_n_1*app_echo_cfg->hfp_a_cspn_tx)
    		app_echo_erase.flag_snr_tx = 1;
        else
    		app_echo_erase.flag_snr_tx = 0;

        if(v_rs > app_echo_erase.v_rn_n_1*app_echo_cfg->hfp_a_cspn_tx)
    		app_echo_erase.flag_snr_rx = 1;
        else
    		app_echo_erase.flag_snr_rx = 0;


        // 4. T/R CP
        v_tx = v_ts - app_echo_erase.v_tn_n_1*app_echo_cfg->hfp_a_cptr;
        v_rx = v_rs - app_echo_erase.v_rn_n_1*app_echo_cfg->hfp_a_cptr;
        if(app_echo_erase.state == APP_HFP_ECHO_STATE_RX)
        {   
            if(v_tx > v_rx*app_echo_cfg->hfp_hyt_rx)
                hyt_flag = 1;
            else
                hyt_flag = 0;
        }
        else if(app_echo_erase.state == APP_HFP_ECHO_STATE_TX)
        {
            if(v_rx < v_tx*app_echo_cfg->hfp_hyt_tx)
                hyt_flag = 1;
            else
                hyt_flag = 0;
        }

        // 5 .state decistion
        if(hyt_flag  && app_echo_erase.flag_snr_tx)
            app_echo_erase.state_changing[1] = APP_HFP_ECHO_STATE_TX;
        else if(!hyt_flag && app_echo_erase.flag_snr_rx)
            app_echo_erase.state_changing[1] = APP_HFP_ECHO_STATE_RX;
        else if(!app_echo_erase.flag_snr_tx && app_echo_erase.flag_snr_rx)
            app_echo_erase.state_changing[1] = APP_HFP_ECHO_STATE_RX;
        else if(!app_echo_erase.flag_snr_rx && app_echo_erase.flag_snr_tx)
            app_echo_erase.state_changing[1] = APP_HFP_ECHO_STATE_TX;
        else if(!app_echo_erase.flag_snr_tx && !app_echo_erase.flag_snr_rx)
            app_echo_erase.state_changing[1] = APP_HFP_ECHO_STATE_IDLE;

        if(app_echo_erase.state_changing[1] == app_echo_erase.state_changing[0])
            app_echo_erase.hold++;
        else
            app_echo_erase.hold = 0;

        app_echo_erase.state_changing[0] = app_echo_erase.state_changing[1];

        if(app_echo_erase.hold >= app_echo_cfg->hold_time)
        {
            app_echo_erase.hold = 0;
            if(app_echo_erase.state != app_echo_erase.state_changing[1])
            { 
                app_echo_erase.attenuation_interval = 0;
                if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_TX && 
                    app_echo_erase.state == APP_HFP_ECHO_STATE_RX)
                {
                    app_echo_erase.attenuation_mode = 2;  
                 }      
                else if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_RX &&
                    app_echo_erase.state == APP_HFP_ECHO_STATE_TX)
                {
                    app_echo_erase.attenuation_mode = 1;
                }
                else if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_IDLE &&
                    app_echo_erase.state == APP_HFP_ECHO_STATE_TX)
                    app_echo_erase.attenuation_mode = 5;
                else if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_IDLE &&
                    app_echo_erase.state == APP_HFP_ECHO_STATE_RX)
                    app_echo_erase.attenuation_mode = 6;
                else if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_RX &&
                    app_echo_erase.state == APP_HFP_ECHO_STATE_IDLE)
                    app_echo_erase.attenuation_mode = 3;
                else if(app_echo_erase.state_changing[1] == APP_HFP_ECHO_STATE_TX &&
                    app_echo_erase.state == APP_HFP_ECHO_STATE_IDLE)
                    app_echo_erase.attenuation_mode = 4;
                else
                    app_echo_erase.attenuation_mode = 0;
                

                app_echo_erase.state = app_echo_erase.state_changing[1];
                
            }
        #ifdef BEKEN_DEBUG
            if(app_echo_debug)
            {
                os_printf("%d, %d, %d %d %d %d %d %d %d %s\r\n", app_echo_erase.v_rn_n_1, 
                    app_echo_erase.v_tn_n_1, v_rs, v_ts, app_echo_erase.flag_snr_rx,
                    app_echo_erase.flag_snr_tx, hyt_flag, app_echo_erase.attenuation_rx,
                    app_echo_erase.attenuation_tx,
                    (app_echo_erase.state == 0)?"idle":((app_echo_erase.state==1)?"rx":"tx"));
            }
        #endif
        }

    }

    // 6. attenuation control
    app_hfp_transfer_buffer(tx_samples, rx_samples);
    
    return;
    
}
int app_hfp_echo_ioctl(uint8 oper, uint32 arg)
{
    if(app_echo_cfg == NULL)
        return -1;
    
    switch(oper)
    {
        case ECHO_HFP_ICOTL_ENABLE:
            if(!bt_flag1_is_set(APP_FLAG_HFP_CALLSETUP))
                app_echo_cfg->used = arg&0x01;
            else
            {
                os_printf("Please enable/disable echo cancellation without call establishment.\r\n");
                return -1;
            }
            break;
        case ECHO_HFP_IOCTL_AS:
            app_echo_cfg->hfp_a_s = arg&0x0F;
            break;
        case ECHO_HFP_IOCTL_AN:
            app_echo_cfg->hfp_a_n = arg&0x0F;
            break;
        case ECHO_HFP_IOCTL_NOISEINTER:
            app_echo_cfg->hfp_noise_interval = arg;
            break;
        case ECHO_HFP_IOCTL_HOLD:
            app_echo_cfg->hold_time = arg;
            break;
        case ECHO_HFP_IOCTL_CSPNTX:
            app_echo_cfg->hfp_a_cspn_tx = arg&0x0F;
            break;
        case ECHO_HFP_IOCTL_CSPNRX:
            app_echo_cfg->st = arg;
            break;
        case ECHO_HFP_IOCTL_CSPNIDLE:
            app_echo_cfg->ft = arg;
            break;
        case ECHO_HFP_IOCTL_CPTR:
            app_echo_cfg->hfp_a_cptr = arg&0x0F;
            break;
        case ECHO_HFP_IOCTL_HYTRX:
            app_echo_cfg->hfp_hyt_rx = arg;
            break;
        case ECHO_HFP_IOCTL_HYTTX:
            app_echo_cfg->hfp_hyt_tx = arg;
            break;
        case ECHO_HFP_ADJUST_DAC_GAIN:
            aud_volume_set(AUDIO_VOLUME_MAX - arg&0x0F);
            break;
        case ECHO_HFP_ADJUST_MIC_GAIN:
            aud_mic_volume_set(arg);
            break;
    #ifdef BEKEN_DEBUG
        case ECHO_HFP_DEBUG_ENABLE:
            app_echo_debug = (arg&0x01);
            break;
    #endif
        default:
            return -1;   
    }

    return 0;
}

#ifdef BEKEN_DEBUG
void app_hfp_debug_show(void)
{
    int8 dacGain = 0;
    int8 micGain = 0;
    int8 micGain12db = 0;


    dacGain = 0 - dacGain*2;
    if(dacGain < -20)
        dacGain = -20;

    micGain = micGain*2;
    if(micGain12db)
        micGain += 12;
    
    os_printf("Echo cancel parameters:\r\n");
    os_printf("Audio DAC Gain: %d dB\r\n", dacGain);
    os_printf("Microphone Gain: %d dB\r\n", micGain);

    if(app_echo_cfg == NULL)
        return;

    os_printf("echo cancel %s\r\n", app_echo_cfg->used?"enabled":"disabled");
    os_printf("A_S: %d, A_N: %d, noise interval: %d, hold: %d\r\n", app_echo_cfg->hfp_a_s,
        app_echo_cfg->hfp_a_n, app_echo_cfg->hfp_noise_interval, app_echo_cfg->hold_time);
    os_printf("CSPN_TX: %d, S_T: %d, F_T:%d, CPTR: %d, hyt_Tx: %d, hyt_Rx: %d\r\n",
        app_echo_cfg->hfp_a_cspn_tx, app_echo_cfg->st, app_echo_cfg->ft,
        app_echo_cfg->hfp_a_cptr, app_echo_cfg->hfp_hyt_tx, app_echo_cfg->hfp_hyt_rx);
    
}
#endif


//AGC part
#define  AGC_IIR_COEF_SHIFT_BIT     8
#define  AGC_HIGH_THD              0x2000
#define  AGC_LOW_THD                0x1000
#define  AGC_MAX_MIN_STEP         16
#define  AGC_HIGH_INTERVAL         32
#define  AGC_LOW_INTERVAL         1024

typedef struct _app_hfp_agc_s
{
    uint32   mic_rssi;
    int16   agc_high_count;
    int16   agc_low_count;
    int8    agc_gain;
    int8    agc_min;
    int8    agc_max;
}app_hfp_agc_t;

static app_hfp_agc_t app_hfp_agc;

void app_hfp_agc_init(uint8 agc_max)
{
    memset(&app_hfp_agc, 0, sizeof(app_hfp_agc));

    app_hfp_agc.agc_max = agc_max;

    app_hfp_agc.agc_min = app_hfp_agc.agc_max - AGC_MAX_MIN_STEP;

    if(app_hfp_agc.agc_min <= 1)
        app_hfp_agc.agc_min = 1;

    app_hfp_agc.agc_gain = app_hfp_agc.agc_max;
    
    return;
}

static void app_hfp_agc_update(int16 sample)
{ 
    app_hfp_agc.mic_rssi -= (app_hfp_agc.mic_rssi >> AGC_IIR_COEF_SHIFT_BIT);
    app_hfp_agc.mic_rssi += (ABS(sample) >> AGC_IIR_COEF_SHIFT_BIT);

    app_hfp_agc.agc_high_count++;
    app_hfp_agc.agc_low_count++;

    if(app_hfp_agc.agc_high_count >= AGC_HIGH_INTERVAL)
    {
        app_hfp_agc.agc_high_count = 0;
        if(app_hfp_agc.mic_rssi >= AGC_HIGH_THD)
        {
            app_hfp_agc.agc_gain--;
            if(app_hfp_agc.agc_gain <= app_hfp_agc.agc_min)
                app_hfp_agc.agc_gain = app_hfp_agc.agc_min;

        #ifdef BEKEN_DEBUG
            //os_printf("mic_rssi : %d, sample: %d, AGC gain - : %d\r\n", 
            //    app_hfp_agc.mic_rssi, sample, app_hfp_agc.agc_gain);
        #endif

            aud_mic_volume_set(app_hfp_agc.agc_gain);
        }
    }

    if(app_hfp_agc.agc_low_count >= AGC_LOW_INTERVAL)
    {
        app_hfp_agc.agc_low_count = 0;
        if(app_hfp_agc.mic_rssi <= AGC_LOW_THD)
        {
            app_hfp_agc.agc_gain++;

            if(app_hfp_agc.agc_gain >= app_hfp_agc.agc_max)
                app_hfp_agc.agc_gain = app_hfp_agc.agc_max;

        #ifdef BEKEN_DEBUG
            //os_printf("mic_rssi: %d, AGC gain + : %d\r\n", 
            //    app_hfp_agc.mic_rssi, app_hfp_agc.agc_gain);
        #endif

            aud_mic_volume_set(app_hfp_agc.agc_gain);
        }
    }

    
}

#endif
