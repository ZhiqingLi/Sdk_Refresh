#include "bt_sco_chain_echo_suppress.h"
#include "stddef.h"
#include "app_audio.h"
#include "hal_trace.h"


#define VQE_PROFILE

#ifdef VQE_PROFILE
#include "hal_timer.h"
#endif



#if defined(SPEECH_TX_AEC2) //&& defined(SPEECH_TX_AEC) 
#include "med_aec_main.h"
#endif


#if defined(USE_MED_MALLOC)
#include "med_memory.h"
#endif

#if defined(SPEECH_TX_AEC2) //&& defined(SPEECH_TX_AEC) 
static VOS_VOID *aec_st = NULL;
#endif



#ifdef VQE_PROFILE
#define TICKS_TO_MIPS(tick) ((tick) * 1000.0 / 16000 * 52 / 80 * 8000)
static float max_mips_aec = 0, max_mips_ns = 0, max_mips_agc = 0;
#endif


extern int hwfft_sleep_ticks;
uint32_t bt_sco_chain_echo_suppress_sendchain(int16_t *buf, int16_t *echo_ref_buf, uint32_t len)
{
	//int err = 0;

#ifdef VQE_PROFILE
	uint32_t stime, etime;
	float mips_now;
#endif

#ifdef VQE_PROFILE
	stime = hal_sys_timer_get();
#endif
#if defined(SPEECH_TX_AEC2) //&& defined(SPEECH_TX_AEC) 
    MED_AEC_Main(aec_st, buf, echo_ref_buf, buf);
#endif
#ifdef VQE_PROFILE
	etime = hal_sys_timer_get();
	mips_now = TICKS_TO_MIPS(etime - stime - hwfft_sleep_ticks);
	max_mips_aec = mips_now > max_mips_aec ? mips_now : max_mips_aec;
#endif


    return len;
}

/*
TEST1:
// 16k, MIPS
// 39, enable NLP, disable AF
// 34, disable NLP, enable AF(1 block)
// 44, disable NLP, enable AF(2 block)
// 62, disable NLP, enable AF(4 block)
// AF(1 block): about 10M

TEST2:
        CONFIGURE      MIPS(x1.85M)   DY RAM      BSS        FLASH                  RAM +6792-384 = +6408   
AEC2-OFF                    7           384     0x2002cc78  0x3c091e40              
AEC2-ON1-NO-NLP-NO-AF1      8           26528   0x2002e700  0x3c0992bc      1         
AEC2-ON2-YES-NLP-NO-AF1     30          27584   0x2002e700  0x3c0992bc      23
AEC2-ON3-NO-NLP-YES-AF1     24          30720   0x2002e700  0x3c0992bc      17 
AEC2-ON4-YES-NLP-YES-AF1    45          30720   0x2002e700  0x3c0992bc      38
AEC2-ON5-YES-NLP-YES-AF2    51          36864   0x2002e700  0x3c09942c      44
AEC2-ON6-YES-NLP-YES-AF4    62          49152   0x2002e700  0x3c099424      55



|-----|---------------|---------|-------------|---------------|------|
| NLP | AF(Block Num) | MIPS(M) | RAM(k Byte) | FLASH(k Byte) | Note |
|-----|---------------|---------|-------------|---------------|------|
| NO  | NO            |       2 |          33 |            30 | \    |
|-----|---------------|---------|-------------|---------------|------|
| YES | NO            |      43 |          34 |            30 |      |
|-----|---------------|---------|-------------|---------------|------|
| NO  | YES(1)        |      32 |          37 |            30 |      |
|-----|---------------|---------|-------------|---------------|------|
| YES | YES(1)        |      71 |          37 |            30 |      |
|-----|---------------|---------|-------------|---------------|------|
| YES | YES(2)        |      82 |          43 |            30 |      |
|-----|---------------|---------|-------------|---------------|------|
| YES | YES(4)        |     102 |          55 |            30 |      |
|-----|---------------|---------|-------------|---------------|------|
*/
int bt_sco_chain_echo_suppress_init(int sampleRate)
{	
	//int err = 0;

#if defined(SPEECH_TX_AEC2) //&& defined(SPEECH_TX_AEC) 
	MED_AEC_NV_STRU AecParam;
	short bin_range;

	if (sampleRate == 8000)
		bin_range = 46;
	else
		bin_range = 91;
		
	AecParam.enEAecEnable = 1;
	AecParam.enHpfEnable = 1;
	AecParam.enAfEnable = 0;//adaptive filter
	AecParam.enNsEnable = 0;
	AecParam.enNlpEnable = 1;//nonlinear 
	AecParam.enCngEnable = 0;
	AecParam.shwDelayLength = 0;
	AecParam.shwNlpRefCnt = 100;
	AecParam.shwNlpRefAmp1 = 100;
	AecParam.shwNlpExOverdrive = (6<<10);
	AecParam.shwNlpResdPowAlph = (VOS_INT16)(0.1*32768);
	AecParam.shwNlpSmoothGainDod = 0; //(6 << 10);
	AecParam.shwNlpResdPowThd = 0;
	AecParam.shwNlpBandSortIdx = bin_range*3/4;
	AecParam.shwNlpBandSortIdxLow = bin_range/2;
	AecParam.shwNlpTargetSupp = (3<<10);
	AecParam.shwNlpMinOvrd = (1<<10);
	AecParam.shwNlpOvrdHigh = 31130; //(VOS_INT16)(0.95 * 32768);
	AecParam.shwNlpOvrdLow = 29490; //(VOS_INT16)(0.9 * 32768);

	aec_st = MED_AEC_Create();

    TRACE("%s aec_st %x", __func__, aec_st);

	MED_AEC_SetPara(aec_st, &AecParam, sampleRate);

    MED_AEC_SetExtenalNsHandle(aec_st, NULL, NULL);
#endif

	return 0;
}

int bt_sco_chain_echo_suppress_deinit(void)
{
#if defined(SPEECH_TX_AEC2) //&& defined(SPEECH_TX_AEC) 
	MED_AEC_Destroy(&aec_st);
#endif

#ifdef USE_MED_MALLOC
	uint32_t total = 0, used = 0, max_used = 0;
	med_memory_info(&total, &used, &max_used);
	TRACE("MED MALLOC MEM: total - %d, used - %d, max_used - %d.",
		total, used, max_used);
#endif

#ifdef VQE_PROFILE
	TRACE("VQE PROFILE: aec - %f, ns - %f, agc - %f.",
		(double)max_mips_aec, (double)max_mips_ns, (double)max_mips_agc);
#endif

	return 0;
}
