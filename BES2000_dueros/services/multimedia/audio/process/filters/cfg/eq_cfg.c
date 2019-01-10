#include "plat_types.h"
#include "tgt_hardware.h"
#include "hal_trace.h"
#include "iir_process.h"
#include "eq_cfg.h"
#include "hw_codec_iir_process.h"
#include "hw_iir_process.h"
#include "math.h"

#define IIR_FIXED_Q (1<<27)

#define PI                                  (3.14159265358979)

static HW_CODEC_IIR_CFG_T  hw_codec_iir_eq;
static HW_IIR_CFG_T  hw_iir_eq;

void iir_lowshelf_coefs_generate(float gain, float fn, float Q, float *coefs)
{
    float A = (float)sqrt(pow(10, gain / 20));
    float w0 = 2 * PI*fn;
    float alpha = (float)sin(w0)/(2*Q);

    float a0 = (A + 1) + (A - 1)*(float)cos(w0) + 2 * (float)sqrt(A)*alpha;
    float a1 = (-2 * ((A - 1) + (A + 1)*(float)cos(w0)));
    float a2 = ((A + 1) + (A - 1)*(float)cos(w0) - 2 * (float)sqrt(A)*alpha);
    float b0 = (A*((A + 1) - (A - 1)*(float)cos(w0) + 2 *(float)sqrt(A)*alpha));
    float b1 = (2 * A*((A - 1) - (A + 1)*(float)cos(w0)));
    float b2 = (A*((A + 1) - (A - 1)*(float)cos(w0) - 2 * (float)sqrt(A)*alpha));

    coefs[0] = 1;
    coefs[1] = a1 / a0;
    coefs[2] = a2 / a0;
    coefs[3] = b0 / a0;
    coefs[4] = b1 / a0;
    coefs[5] = b2 / a0;
    
//    IIR_TRACE("fn = %f, gain = %f, Q = %f, A = %f, w0 = %f, alpha = %f", fn, gain, Q, A, w0, alpha);
//    TRACE("[%s] %f, %f, %f, %f, %f, %f", __func__, (double)coefs[0], (double)coefs[1], (double)coefs[2], (double)coefs[3], (double)coefs[4], (double)coefs[5]);
}

void iir_highshelf_coefs_generate(float gain, float fn, float Q, float *coefs)
{
    float A = (float)sqrt(pow(10, gain / 20));
    float w0 = 2 * PI*fn;
    float alpha = (float)sin(w0) / (2 * Q);

    float a0 = (A + 1) - (A - 1)*(float)cos(w0) + 2 * (float)sqrt(A)*alpha;  //  a0
    float a1 = (2 * ((A - 1) - (A + 1)*(float)cos(w0)));  // a1
    float a2 = ((A + 1) - (A - 1)*(float)cos(w0) - 2 * (float)sqrt(A)*alpha);  //a2
    float b0 = (A*((A + 1) + (A - 1)*(float)cos(w0) + 2 * (float)sqrt(A)*alpha));  //b0
    float b1 = (-2 * A*((A - 1) + (A + 1)*(float)cos(w0)));   // b1
    float b2 = (A*((A + 1) + (A - 1)*(float)cos(w0) - 2 * (float)sqrt(A)*alpha));  // b2

    coefs[0] = 1;
    coefs[1] = a1 / a0;
    coefs[2] = a2 / a0;
    coefs[3] = b0 / a0;
    coefs[4] = b1 / a0;
    coefs[5] = b2 / a0;
    
//    IIR_TRACE("fn = %f, gain = %f, Q = %f, A = %f, w0 = %f, alpha = %f", fn, gain, Q, A, w0, alpha);
 //   TRACE("[%s] %f, %f, %f, %f, %f, %f", __func__, (double)coefs[0], (double)coefs[1], (double)coefs[2], (double)coefs[3], (double)coefs[4], (double)coefs[5]);
}

// fn: Normalized frequency
void iir_peak_notch_coefs_generate(float gain, float fn, float Q, float *coefs)
{
//    float Fs = 1000.0 / 48000.0;
//    float gain = -5.0;
//    float fo = 1000.0;
//    float Q = 0.7;
    float A = (float)sqrt(pow(10, gain/20));
    float w0 = 2*PI*fn; 
    float alpha = (float)sin(w0)/(2*Q); 

    float a0 = 1 + alpha/A; 
    float a1 = -2*(float)cos(w0); 
    float a2 = 1 - alpha/A;
    float b0 = 1 + alpha*A; 
    float b1 = -2*(float)cos(w0); 
    float b2 = 1 - alpha*A; 

    coefs[0] = 1;
    coefs[1] = a1 / a0;
    coefs[2] = a2 / a0;
    coefs[3] = b0 / a0;
    coefs[4] = b1 / a0;
    coefs[5] = b2 / a0;
    
//    IIR_TRACE("fn = %f, gain = %f, Q = %f, A = %f, w0 = %f, alpha = %f", fn, gain, Q, A, w0, alpha);
 //   TRACE("[%s] %f, %f, %f, %f, %f, %f", __func__, (double)coefs[0], (double)coefs[1], (double)coefs[2], (double)coefs[3], (double)coefs[4], (double)coefs[5]);
}

void iir_highpass_coefs_generate(float gain, float fn, float Q, float *coefs)
{
    float w0 = 2 * PI*fn;
    float alpha = (float)sin(w0) / (2 * Q);

    float a0 = 1 + alpha;
    float a1 = -2 * (float)cos(w0);
    float a2 = 1 - alpha;
    float b0 = (1 + (float)cos(w0))/2;
    float b1 = -(1 + (float)cos(w0));
    float b2 = (1 + (float)cos(w0))/2;

    coefs[0] = 1;
    coefs[1] = a1/a0;
    coefs[2] = a2/a0;
    coefs[3] = b0/a0;
    coefs[4] = b1/a0;
    coefs[5] = b2/a0;
}

void iir_lowspass_coefs_generate(float gain, float fn, float Q, float *coefs)
{
    float w0 = 2 * PI*fn;
    float alpha = (float)sin(w0) / (2 * Q);

    float a0 = 1 + alpha;
    float a1 = -2 * (float)cos(w0);
    float a2 = 1 - alpha;
    float b0 = (1 - (float)cos(w0))/2;
    float b1 = 1 - (float)cos(w0);
    float b2 = (1 - (float)cos(w0))/2;

    coefs[0] = 1;
    coefs[1] = a1/a0;
    coefs[2] = a2/a0;
    coefs[3] = b0/a0;
    coefs[4] = b1/a0;
    coefs[5] = b2/a0;
}

float iir_convert_db_to_multiple(float db)
{
    return (float)pow(10, db/20);
}

void iir_coefs_generate(IIR_TYPE_T type, float gain, float fn, float Q, float *coefs)
{
    if (type == IIR_TYPE_LOW_SHELF) 
    {  
        iir_lowshelf_coefs_generate(gain, fn, Q, coefs);
    } 
    else if (type == IIR_TYPE_PEAK)
    {
        iir_peak_notch_coefs_generate(gain, fn, Q, coefs);
    } 
    else if (type == IIR_TYPE_HIGH_SHELF)
    {
        iir_highshelf_coefs_generate(gain, fn, Q, coefs);
    }
    else if (type == IIR_TYPE_LOW_PASS)
    {
        iir_lowspass_coefs_generate(gain, fn, Q, coefs);
    }
    else if (type == IIR_TYPE_HIGH_PASS)
    {
        iir_highpass_coefs_generate(gain, fn, Q, coefs);
    }
    else 
    {
        ASSERT(false, "[%s] %d is not supported", __func__, type);
    }
}

HW_CODEC_IIR_CFG_T *hw_codec_iir_get_cfg(enum AUD_SAMPRATE_T sample_rate, const IIR_CFG_T* ext_cfg)
{
    const IIR_CFG_T *cfg;
    float coefs_l[6],coefs_r[6];
    float gain0,gain1;

    TRACE("%s,sample_rate:%d\n", __func__,sample_rate);

    hw_codec_iir_eq.iir_filtes_l.iir_bypass_flag=1;
    hw_codec_iir_eq.iir_filtes_r.iir_bypass_flag=1;

    if (ext_cfg)
    {
        cfg = ext_cfg;
    }
    else
    {
        TRACE("Error HW IIR CODEC cfg!");
        return NULL;
    }

    // Check parameter
    ASSERT(cfg->num <= AUD_IIR_NUM_EQ, "[%s] num(%d) is too large", __func__, cfg->num);

    gain0 = iir_convert_db_to_multiple(cfg->gain0);
    gain1 = iir_convert_db_to_multiple(cfg->gain1);

    // TRACE("[%s] gain0 = %f, gain1 = %f", __func__, (double)gain0, (double)gain1);
    TRACE("[%s] len = %d", __func__, cfg->num);

    for(int i=0; i<cfg->num; i++)
    {
        iir_coefs_generate(cfg->param[i].type,
                           cfg->param[i].gain,
                           cfg->param[i].fc / sample_rate, 
                           cfg->param[i].Q, 
                           coefs_l);

        coefs_r[0]=coefs_l[0];
        coefs_r[1]=coefs_l[1];
        coefs_r[2]=coefs_l[2];
        coefs_r[3]=coefs_l[3];
        coefs_r[4]=coefs_l[4];
        coefs_r[5]=coefs_l[5];        

        if(i==0)
        {
            coefs_l[3] = coefs_l[3]*gain0;
            coefs_l[4] = coefs_l[4]*gain0;
            coefs_l[5] = coefs_l[5]*gain0;

            coefs_r[3] = coefs_r[3]*gain1;
            coefs_r[4] = coefs_r[4]*gain1;
            coefs_r[5] = coefs_r[5]*gain1;
        }

        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_a[0]=(int32_t)((double)coefs_l[0]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_a[1]=(int32_t)((double)coefs_l[1]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_a[2]=(int32_t)((double)coefs_l[2]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_b[0]=(int32_t)((double)coefs_l[3]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_b[1]=(int32_t)((double)coefs_l[4]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_l.iir_coef[i].coef_b[2]=(int32_t)((double)coefs_l[5]*(int32_t)IIR_FIXED_Q);

        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_a[0]=(int32_t)((double)coefs_r[0]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_a[1]=(int32_t)((double)coefs_r[1]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_a[2]=(int32_t)((double)coefs_r[2]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_b[0]=(int32_t)((double)coefs_r[3]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_b[1]=(int32_t)((double)coefs_r[4]*(int32_t)IIR_FIXED_Q);
        hw_codec_iir_eq.iir_filtes_r.iir_coef[i].coef_b[2]=(int32_t)((double)coefs_r[5]*(int32_t)IIR_FIXED_Q);

    }

    /*
    TRACE("\n[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_a[0]);
    TRACE("[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_a[1]);
    TRACE("[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_a[2]);
    TRACE("[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_b[0]);
    TRACE("[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_b[1]);
    TRACE("[%s] coef_a = %d\n", __func__, hw_codec_iir_eq.iir_filtes_l.iir_coef[0].coef_b[2]);
    */
    if(cfg->num>=0)
    {
        hw_codec_iir_eq.iir_filtes_l.iir_bypass_flag=0;
        hw_codec_iir_eq.iir_filtes_r.iir_bypass_flag=0;

        hw_codec_iir_eq.iir_filtes_l.iir_counter=cfg->num;
        hw_codec_iir_eq.iir_filtes_r.iir_counter=cfg->num;
    }
    
    return &hw_codec_iir_eq;
}

HW_IIR_CFG_T *hw_iir_get_cfg(enum AUD_SAMPRATE_T sample_rate, const IIR_CFG_T* ext_cfg)
{
    const IIR_CFG_T *cfg;
    float coefs_l[6],coefs_r[6];
    float gain0,gain1;

    TRACE("%s,sample_rate:%d\n", __func__,sample_rate);

    if (ext_cfg)
    {
        cfg = ext_cfg;
    }
    else
    {
        TRACE("Error HW IIR cfg!");
        return NULL;
    }

    // Check parameter
    ASSERT(cfg->num <= AUD_IIR_NUM_EQ, "[%s] num(%d) is too large", __func__, cfg->num);

    gain0 = iir_convert_db_to_multiple(cfg->gain0);
    gain1 = iir_convert_db_to_multiple(cfg->gain1);

    // TRACE("[%s] gain0 = %f, gain1 = %f", __func__, (double)gain0, (double)gain1);
    TRACE("[%s] len = %d", __func__, cfg->num);

    for(int i=0; i<cfg->num; i++)
    {
        iir_coefs_generate(cfg->param[i].type,
                           cfg->param[i].gain,
                           cfg->param[i].fc / sample_rate, 
                           cfg->param[i].Q, 
                           coefs_l);

        coefs_r[0]=coefs_l[0];
        coefs_r[1]=coefs_l[1];
        coefs_r[2]=coefs_l[2];
        coefs_r[3]=coefs_l[3];
        coefs_r[4]=coefs_l[4];
        coefs_r[5]=coefs_l[5];        

        if(i==0)
        {
            coefs_l[3] = coefs_l[3]*gain0;
            coefs_l[4] = coefs_l[4]*gain0;
            coefs_l[5] = coefs_l[5]*gain0;

            coefs_r[3] = coefs_r[3]*gain1;
            coefs_r[4] = coefs_r[4]*gain1;
            coefs_r[5] = coefs_r[5]*gain1;
        }

        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_a[0]=(int32_t)((double)coefs_l[0]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_a[1]=(int32_t)((double)coefs_l[1]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_a[2]=(int32_t)((double)coefs_l[2]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_b[0]=(int32_t)((double)coefs_l[3]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_b[1]=(int32_t)((double)coefs_l[4]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_l.iir_coef[i].coef_b[2]=(int32_t)((double)coefs_l[5]*(int32_t)IIR_FIXED_Q);

        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_a[0]=(int32_t)((double)coefs_r[0]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_a[1]=(int32_t)((double)coefs_r[1]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_a[2]=(int32_t)((double)coefs_r[2]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_b[0]=(int32_t)((double)coefs_r[3]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_b[1]=(int32_t)((double)coefs_r[4]*(int32_t)IIR_FIXED_Q);
        hw_iir_eq.iir_filtes_r.iir_coef[i].coef_b[2]=(int32_t)((double)coefs_r[5]*(int32_t)IIR_FIXED_Q);

    }

/*
    TRACE("\n[%s] coef_a0 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_a[0]);
    TRACE("[%s] coef_a1 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_a[1]);
    TRACE("[%s] coef_a2 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_a[2]);
    TRACE("[%s] coef_b0 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_b[0]);
    TRACE("[%s] coef_b1 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_b[1]);
    TRACE("[%s] coef_b2 = %d\n", __func__, hw_iir_eq.iir_filtes_l.iir_coef[0].coef_b[2]);
*/    
    if(cfg->num>=0)
    {
        hw_iir_eq.iir_filtes_l.iir_counter=cfg->num;
        hw_iir_eq.iir_filtes_r.iir_counter=cfg->num;
    }

    return &hw_iir_eq;
}

