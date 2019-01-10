
#if defined(WIN32) && !defined(__cplusplus)

#define inline __inline

#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
// #include "arch.h"

#define D0 16384
#define D1 11356
#define D2 3726
#define D3 1301

#define C0 3634
#define C1 21173
#define C2 -12627
#define C3 4204

/*
typedef short spx_int16_t;
typedef unsigned short spx_uint16_t;
typedef int spx_int32_t;
typedef unsigned int spx_uint32_t;


typedef spx_int16_t spx_word16_t;
typedef spx_int32_t   spx_word32_t;


*/
#define SHR16F(a,shift) ((a) >> (shift))
#define SHL16F(a,shift) ((a) << (shift))
#define ADD16F(a,b) ((short)((short)(a)+(short)(b)))

#define SHRF(a,shift) ((a) >> (shift))
#define MULT16_16F(a,b)     (((int)(short)(a))*((int)(short)(b)))
#define MULT16_16_Q14F(a,b) (SHRF(MULT16_16F((a),(b)),14))

#define ADD32F(a,b) ((int)(a)+(int)(b))
#define MULT16_16_P14F(a,b) (SHRF(ADD32F(8192,MULT16_16F((a),(b))),14))

#define MULT16_32_Q14F(a,b) ADD32F(MULT16_16F((a),SHRF((b),14)), SHRF(MULT16_16F((a),((b)&0x00003fff)),14))

#define EXTEND32F(x) ((int)(x))


#define SHR32F(a,shift) ((a) >> (shift))
#define SHL32F(a,shift) ((a) << (shift))
#define VSHR32F(a, shift) (((shift)>0) ? SHR32F(a, shift) : SHL32F(a, -(shift)))


#define VSHR32F(a, shift) (((shift)>0) ? SHR32F(a, shift) : SHL32F(a, -(shift)))
#define MULT16_16_Q14F(a,b) (SHRF(MULT16_16F((a),(b)),14))

int log10_20fix(int in); //log10_20fix(in * 1024) = 10 * log10(in) * 32
int ExpIntfix(short in);//(0:5]*256 ExpIntfix(x*256) = expint(x)*1024
/* Input in Q11 format, output in Q16 */
int exp_fix(short x);// ‰»Î*2^11   ‰≥ˆ/2^14
int log10dotfun(short in);//log10dotfun(in*1024) = 10*log10(in)*16
short sqrt_fix(int x);