/////////////////////////////////////////////////////////////////////////
//Packets Loss Concealment                                              /
/////////////////////////////////////////////////////////////////////////
#ifndef _PLC_16000_H_
#define _PLC_16000_H_

typedef struct PlcSt_16000_ PlcSt_16000;

void speech_plc_16000_Dofe(PlcSt_16000 *lc, short *out, int num);
void speech_plc_16000_AddToHistory(PlcSt_16000 *lc, short *s, int num);
PlcSt_16000 *speech_plc_16000_init(void* (* speex_alloc_ext)(int));

#endif












