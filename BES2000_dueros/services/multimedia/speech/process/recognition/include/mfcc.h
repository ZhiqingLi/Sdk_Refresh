#ifndef MFCC_H
#define MFCC_H

#define VOICE_SR         (8000)                                        //Sample Rate
#define VOICE_FRAME_TIME (32)                                         //ms
#define VOICE_FRAME_MOVE_TIME  (16)                                   // frame shift time ms
#define VOICE_VALID_TIME_MAX   (1024)                                 //ms
#define VOICE_FRAME_LEN        240  // change by chengxin (VOICE_FRAME_TIME*VOICE_SR/1000)      //Frame length
#define VOICE_FRAME_MOVE_LEN   (VOICE_FRAME_MOVE_TIME*VOICE_SR/1000)// Frame shift length
#define VOICE_FRAME_SIZE       (VOICE_FRAME_LEN*sizeof(short))          //bytes of frame
#define VOICE_FRAME_MOVE_SIZE  (VOICE_FRAME_MOVE_LEN*sizeof(short))//bytes of frame shift
#define VOICE_VALID_FRAME_MAX   110//((VOICE_VALID_TIME_MAX-VOICE_FRAME_TIME)/VOICE_FRAME_MOVE_TIME + 1)

#define MFCC_HP_RATIO  (240)      // Pre-emphasis factor 243/256~=0.95    240/256=0.9375
#define MFCC_HP_SCALE  (256)     // Pre-emphasis
#define MFCC_FFT_POINT (240)    // FFT POINTS
#define MFCC_ORDER_NUM (24)    // MFCC ORDER


#define MFCC_HAMM_NUM       (240)      //Hamming window size
#define MFCC_TRI_NUM        (24)       //Triangle filter size

#define MFCC_HAMM_SCALE     (256)                   
#define MFCC_TRI_SCALE      (256)                   
#define MFCC_DCT_SCALE      (128)                   


typedef struct{
	short mfcc_dat[MFCC_ORDER_NUM];//MFCC Parameters
} mfcc_frame_t;

typedef struct{
	unsigned short flag;
	unsigned short frm_num;
	mfcc_frame_t mfcc_frm[VOICE_VALID_FRAME_MAX];
} mfcc_vct_t;

typedef struct {
    void *fft_table;
    int *pow_spct;    
    short fft_out_buf[MFCC_FFT_POINT];
    short in_mem;
} mfcc_t;

int mfcc_init(mfcc_t *mfcc);
int mfcc_reset(mfcc_t *mfcc);
int mfcc_frame(mfcc_t *mfcc, short *voice_ptr, unsigned int voice_len, mfcc_frame_t *mfcc_out);

#endif
