/////////////////////////////////////////////////////////////////////////
//Packets Loss Concealment                                              /
/////////////////////////////////////////////////////////////////////////
#ifndef _PLC_8000_H_
#define _PLC_8000_H_


typedef struct PlcSt_8000_ PlcSt_8000;

void PlcInit(PlcSt_8000 *lc);
void Dofe(PlcSt_8000 *lc, short *out, int num);
void AddToHistory(PlcSt_8000 *lc, short *s, int num);
PlcSt_8000 *speech_plc_8000_init(void* (* speex_alloc_ext)(int));
int speech_plc_8000(PlcSt_8000 *lc, short *InBuf, int len);

#endif












