#ifndef __VAD_H__
#define __VAD_H__

#include "mfcc.h"

#define ADAPT_THL_N_RATIO       (256)   // Noise threshold ratio n_thl=n_max_mean*ADAPT_THL_N_RATIO
#define ADAPT_THL_S_RATIO       (282)   // Short amplitude threshold ratio s_thl=sum_mean*ADAPT_THL_S_RATIO ~1.1
#define ADAPT_THL_Z_RATIO       (3)     // Short zero-crossing rate ~0.012
#define ADAPT_THL_SCALE         (256)   // 

#define ADAPT_TIME              (128)   // ms Background noise adapting time
#define VAD_SND_TIME_MIN        (96)    // ms shortest effective speech time 
#define VAD_MUTE_TIME_MAX       (128)   // ms longest silence time 

#define ADAPT_LEN               (ADAPT_TIME * VOICE_SR / 1000)              // Background noise adapting length
#define VAD_SND_FRAME_MIN       (VAD_SND_TIME_MIN / VOICE_FRAME_MOVE_TIME)  // Shortest effective speech frame counts
#define VAD_MUTE_FRAME_MAX      (VAD_MUTE_TIME_MAX / VOICE_FRAME_MOVE_TIME) // Longest Sillence frame counts

#define VAD_FLAG_VALID          (0x1)   // 
#define VAD_FLAG_BEGIN          (0x2)   // Speech begin
#define VAD_FLAG_END            (0x4)   // Speech end

#define VAD_MAXSILENCE          25
#define VAD_MINLEN              32
#define VAD_ZRC                 20

// VAD struct
typedef enum {
	VAD_STA_SILENT,     // silent
	VAD_STA_PRE,        // pre transitional portion
	VAD_STA_VALID,      // speech portion
	VAD_STA_POST,       // post transitional portion
	VAD_STA_MAX
} vad_sta_t;

// VAD threshold
typedef enum {
	VAD_THL_NONE,
	VAD_THL_BELOW,      // zero-crossing rate  below threshold
	VAD_THL_UPPER,      // zero-crossing rate  upper threshold
	VAD_THL_MAX         // maximum threshold
} vad_thl_sig_t;

// Adaptation Parameters
typedef struct {
	int ampt_max;   //ini value is 0
	int ampt_sum;  //ini value is 0
	int ampt1 ; // noise threshold
	int ampt2;
	int ampt;//
	short zrc; // short zero-crossing rate threshold   ini value is 0
	unsigned short vad_ini_num; //ini value is 13
} adapt_arg_t;



// VAD state
typedef struct {
	vad_sta_t state;            // VAD state
	unsigned short flag;          // current frame status
	vad_thl_sig_t last_sig;     // last state
	adapt_arg_t adapt_arg;      // arguments of noise signal
	unsigned int dualSilent_cnt;      // frame counts of transition lasts
	unsigned int total_cnt;     // frame counts of total effective speech lasts
	unsigned short status2_count; //确信进入语音段后帧数
} vad_t;

int vad_init(vad_t *vad);
int vad_reset(vad_t *vad);
int vad_adapt(vad_t *vad, const short *voice_ptr, unsigned int voice_len);
int vad_frame(vad_t *vad, const short *voice_ptr, unsigned int voice_len);

#endif // __VAD_H__

