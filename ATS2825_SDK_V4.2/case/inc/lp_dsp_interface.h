#ifndef __LP_DSP_INTERFACE__
#define __LP_DSP_INTERFACE__

#ifdef __LP_DSP_INTERFACE_C__
#define __LP_DSP_INTERFACE_DEC__
#else
#define __LP_DSP_INTERFACE_DEC__ extern
#endif

typedef enum {
    DSP_PROCESSED,
    DSP_SRC_MIC01,
    DSP_SRC_MIC23,
    DSP_SRC_REFERENCE
} DSP_RECORD_SOURCE;

__LP_DSP_INTERFACE_DEC__ int lp_dsp_get_version(char *version);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_source(DSP_RECORD_SOURCE src);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_pre_gain(int gain);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_post_gain(int gain);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_get_pre_gain(int *gain);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_get_post_gain(int *gain);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_call_mode(void);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_asr_mode(void);
__LP_DSP_INTERFACE_DEC__ int lp_dsp_set_mic_distance(int mm);       //unit is millimeter
__LP_DSP_INTERFACE_DEC__ int lp_dsp_get_mic_distance(int *mm);       //unit is millimeter

#endif
