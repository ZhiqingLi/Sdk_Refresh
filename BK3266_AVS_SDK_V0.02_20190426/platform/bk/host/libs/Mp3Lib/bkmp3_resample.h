#ifndef RESAMPLE_H
#define RESAMPLE_H
#include "uw_types.h"

#if (CONFIG_NOUSE_RESAMPLE == 0)
#define FIR_HARD_CORE
#define FIR_HARD_SOFT
#endif

#define DOWNSMPL1_4		(0x2000)
#define DOWNSMPL1_2		(0x4000)
#define UPSMPL4_1		(4)
#define UPSMPL2_1		(2)
#define NORESMPL		(1)
#if (CONFIG_APP_MP3PLAYER == 1)

#ifdef FIR_HARD_CORE
#define CORE_NFIR       127
int16_t fir_hardcore_config(uint16_t nRatio);
int16_t fir_hardcore_resample(int16_t *smpl,uint16_t nSmpls,uint16_t nRatio);
int16_t fir_hardcore_resample_mono(int16_t *smpl,uint16_t nSmpls,uint16_t nRatio);
int16_t fir_hardcore_free(void);
#else
#define NFIR     31
typedef struct fir_state {
	int16_t ratio;
	int16_t taps;
	int16_t curr_pos;
	int16_t *coef;
	int16_t laststate[NFIR];
}FIR_STATE,*pFIR_STATE;
typedef struct mfir_state {

	FIR_STATE L_fir;
	//FIR_STATE R_fir;
	FIR_STATE L2_fir;
	//FIR_STATE R2_fir;
}MFIR_STATE,*pMFIR_STATE;


int16_t resample_init(pFIR_STATE pfir_stat);
int16_t resample_block(pFIR_STATE L_fir,pFIR_STATE R_fir,int16_t *smpl,uint16_t nSmpls,uint16_t nRatio);
#endif
#endif /* CONFIG_APP_MP3PLAYER */
#endif
