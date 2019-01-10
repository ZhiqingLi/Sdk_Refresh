/*
Copyright (c) 2003-2004, Mark Borgerding

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the author nor the names of any contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifdef WIN32


#else 
#ifdef __ICCARM__
#include <intrinsics.h>
#else

#include "cmsis.h"

#endif
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a):(b))
#endif

/* kiss_fft.h
   defines kiss_fft_scalar as either short or a float type
   and defines
   typedef struct { kiss_fft_scalar r; kiss_fft_scalar i; }kiss_fft_cpx; */
#include "kiss_fft.h"
#include "math_approx.h"

#define MAXFACTORS 32
/* e.g. an fft of length 128 has 4 factors 
 as far as kissfft is concerned
 4*4*4*2
 */

struct kiss_fft_state{
    int nfft;
    int inverse;
    int factors[2*MAXFACTORS];
    kiss_fft_cpx twiddles[1];
};

/*
  Explanation of macros dealing with complex math:

   C_MUL(m,a,b)         : m = a*b
   C_FIXDIV( c , div )  : if a fixed point impl., c /= div. noop otherwise
   C_SUB( res, a,b)     : res = a - b
   C_SUBFROM( res , a)  : res -= a
   C_ADDTO( res , a)    : res += a
 * */
#ifdef FIXED_POINT
#include "arch.h"
# define FRACBITS 15
# define SAMPPROD spx_int32_t 
#define SAMP_MAX 32767

#define SAMP_MIN -SAMP_MAX

#if defined(CHECK_OVERFLOW)
#  define CHECK_OVERFLOW_OP(a,op,b)  \
	if ( (SAMPPROD)(a) op (SAMPPROD)(b) > SAMP_MAX || (SAMPPROD)(a) op (SAMPPROD)(b) < SAMP_MIN ) { \
		fprintf(stderr,"WARNING:overflow @ " __FILE__ "(%d): (%d " #op" %d) = %ld\n",__LINE__,(a),(b),(SAMPPROD)(a) op (SAMPPROD)(b) );  }
#endif


#   define smul(a,b) ( (SAMPPROD)(a)*(b) )
#   define sround( x )  (kiss_fft_scalar)( ( (x) + (1<<(FRACBITS-1)) ) >> FRACBITS )

#   define S_MUL(a,b) sround( smul(a,b) )

#   define C_MUL(m,a,b) \
        do{ (m).dshort.r = sround( __SMUSD((a).sreg,(b).sreg));\
          (m).dshort.i = sround( __SMUADX((a).sreg,(b).sreg)); }while(0)        
/*
      do{ (m).dshort.r = sround( smul((a).dshort.r,(b).dshort.r) - smul((a).dshort.i,(b).dshort.i) ); \
          (m).dshort.i = sround( smul((a).dshort.r,(b).dshort.i) + smul((a).dshort.i,(b).dshort.r) ); }while(0)
*/
#   define C_MUL4(m,a,b) \
               do{ (m).dshort.r = PSHR32( __SMUSD((a).sreg,(b).sreg),17 ); \
               (m).dshort.i = PSHR32( __SMUADX((a).sreg,(b).sreg),17 ); }while(0)
            
/*
               do{ (m).dshort.r = PSHR32( smul((a).dshort.r,(b).dshort.r) - smul((a).dshort.i,(b).dshort.i),17 ); \
               (m).dshort.i = PSHR32( smul((a).dshort.r,(b).dshort.i) + smul((a).dshort.i,(b).dshort.r),17 ); }while(0)
*/
#   define DIVSCALAR(x,k) \
	(x) = sround( smul(  x, SAMP_MAX/k ) )

#   define C_FIXDIV(c,div) \
	do {    DIVSCALAR( (c).dshort.r , div);  \
		DIVSCALAR( (c).dshort.i  , div); }while (0)

#   define C_MULBYSCALAR( c, s ) \
    do{ (c).dshort.r =  sround( smul( (c).dshort.r , s ) ) ;\
        (c).dshort.i =  sround( smul( (c).dshort.i , s ) ) ; }while(0)

#else  /* not FIXED_POINT*/

#   define S_MUL(a,b) ( (a)*(b) )
#define C_MUL(m,a,b) \
    do{ (m).dshort.r = (a).dshort.r*(b).dshort.r - (a).dshort.i*(b).dshort.i;\
        (m).dshort.i = (a).dshort.r*(b).dshort.i + (a).dshort.i*(b).dshort.r; }while(0)

#define C_MUL4(m,a,b) C_MUL(m,a,b)

#   define C_FIXDIV(c,div) /* NOOP */
#   define C_MULBYSCALAR( c, s ) \
    do{ (c).dshort.r *= (s);\
        (c).dshort.i *= (s); }while(0)
#endif

#ifndef CHECK_OVERFLOW_OP
#  define CHECK_OVERFLOW_OP(a,op,b) /* noop */
#endif

#define  C_ADD( res, a,b)\
 do { \
	    CHECK_OVERFLOW_OP((a).dshort.r,+,(b).dshort.r)\
	    CHECK_OVERFLOW_OP((a).dshort.i,+,(b).dshort.i)\
            (res).sreg=__QADD16((a).sreg,(b).sreg);\
    }while(0)
          
          
  /*  do { \
	    CHECK_OVERFLOW_OP((a).dshort.r,+,(b).dshort.r)\
	    CHECK_OVERFLOW_OP((a).dshort.i,+,(b).dshort.i)\
	    (res).dshort.r=(a).dshort.r+(b).dshort.r;  (res).dshort.i=(a).dshort.i+(b).dshort.i; \
    }while(0)
      */
#define  C_SUB( res, a,b)\
    do { \
	    CHECK_OVERFLOW_OP((a).dshort.r,-,(b).dshort.r)\
	    CHECK_OVERFLOW_OP((a).dshort.i,-,(b).dshort.i)\
	    (res).sreg=__QSUB16((a).sreg,(b).sreg);\
    }while(0)
      

    /*do { \
	    CHECK_OVERFLOW_OP((a).dshort.r,-,(b).dshort.r)\
	    CHECK_OVERFLOW_OP((a).dshort.i,-,(b).dshort.i)\
	    (res).dshort.r=SATURATE32(SUB32((a).dshort.r,(b).dshort.r),32767);  (res).dshort.i=SATURATE32(SUB32((a).dshort.i,(b).dshort.i),32767); \
    }while(0)*/
      
      
#define C_ADDTO( res , a)\
    do { \
	    CHECK_OVERFLOW_OP((res).dshort.r,+,(a).dshort.r)\
	    CHECK_OVERFLOW_OP((res).dshort.i,+,(a).dshort.i)\
	    (res).sreg=__QADD16((res).sreg,(a).sreg);\
    }while(0)

 /*   do { \
	    CHECK_OVERFLOW_OP((res).dshort.r,+,(a).dshort.r)\
	    CHECK_OVERFLOW_OP((res).dshort.i,+,(a).dshort.i)\
	    (res).dshort.r += (a).dshort.r;  (res).dshort.i += (a).dshort.i;\
    }while(0)
*/      
      
      
#define C_SUBFROM( res , a)\
    do {\
	    CHECK_OVERFLOW_OP((res).dshort.r,-,(a).dshort.r)\
	    CHECK_OVERFLOW_OP((res).dshort.i,-,(a).dshort.i)\
	    (res).sreg=__QSUB16((res).sreg,(a).sreg);\
    }while(0)

/*      
    do {\
	    CHECK_OVERFLOW_OP((res).dshort.r,-,(a).dshort.r)\
	    CHECK_OVERFLOW_OP((res).dshort.i,-,(a).dshort.i)\
	    (res).dshort.r -= (a).dshort.r;  (res).dshort.i -= (a).dshort.i; \
    }while(0)
  */    
      
      
#ifdef FIXED_POINT
#  define KISS_FFT_COS(phase)  floor(MIN(32767,MAX(-32767,.5+32768 * cos (phase))))
#  define KISS_FFT_SIN(phase)  floor(MIN(32767,MAX(-32767,.5+32768 * sin (phase))))
#  define HALF_OF(x) ((x)>>1)
#elif defined(USE_SIMD)
#  define KISS_FFT_COS(phase) _mm_set1_ps( cos(phase) )
#  define KISS_FFT_SIN(phase) _mm_set1_ps( sin(phase) )
#  define HALF_OF(x) ((x)*_mm_set1_ps(.5))
#else
#  define KISS_FFT_COS(phase) (kiss_fft_scalar) cos(phase)
#  define KISS_FFT_SIN(phase) (kiss_fft_scalar) sin(phase)
#  define HALF_OF(x) ((x)*.5)
#endif

#define  kf_cexp(x,phase) \
	do{ \
		(x)->dshort.r = KISS_FFT_COS(phase);\
		(x)->dshort.i = KISS_FFT_SIN(phase);\
	}while(0)
#define  kf_cexp2(x,phase) \
               do{ \
               (x)->dshort.r = spx_cos_norm((phase));\
               (x)->dshort.i = spx_cos_norm((phase)-32768);\
}while(0)


/* a debugging function */
#define pcpx(c)\
    fprintf(stderr,"%g + %gi\n",(double)((c)->dshort.r),(double)((c)->dshort.i) )
