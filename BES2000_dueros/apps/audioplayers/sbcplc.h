/********************************************************
SBC Example PLC ANSI-C Source Code
File: sbcplc.h
*****************************************************************************/
#ifndef SBCPLC_H
#define SBCPLC_H

#include "plat_types.h"

#define FS 120 /* Frame Size */
#define N 256 /* 16ms - Window Length for pattern matching */
#define M 64 /* 4ms - Template for matching */
#define LHIST (N+FS-1) /* Length of history buffer required */
#define SBCRT 36 /* SBC Reconvergence Time (samples) */
#define OLAL 32 /* OverLap-Add Length (samples) */
/* PLC State Information */
typedef struct _PLC_State
{
	short hist[LHIST + FS + SBCRT + OLAL];
	short bestlag;
	int nbf;
}PLC_State;
extern unsigned char indices0[57];
/* Prototypes */
void InitPLC(PLC_State *plc_state);
void PLC_bad_frame(PLC_State *plc_state, short *ZIRbuf, short *out);
void PLC_good_frame(PLC_State *plc_state, short *in, short *out);
#endif /* SBCPLC_H */
