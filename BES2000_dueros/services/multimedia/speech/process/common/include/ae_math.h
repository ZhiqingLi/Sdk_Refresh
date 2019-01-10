#ifndef AE_MATH_H
#define AE_MATH_H

#include <math.h>

#ifndef VQE_SIMULATE
#include "arm_math.h"
#endif

#define AE_PI       3.14159265358979323846f

#define EPS (1e-10f)

#define AE_CLAMP(x,lo,hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

#define AE_SSAT16(x) AE_CLAMP(x,-32768,32767)

#define AE_ABS(x) ((x) > 0 ? (x) : (-(x)))

#define AE_FLOOR(x) (floorf(x))

#define AE_ROUND(x) (roundf(x))

#define AE_INT(x) ((int)(x))

#define AE_FRAC(x) ((int)((x - floorf(x)) * 1000000))

#define AE_MIN(a,b) ((a) < (b) ? (a) : (b))

#define AE_MAX(a,b) ((a) > (b) ? (a) : (b))

#define SQUARE(x) ((x) * (x))

#define DB2LIN(x) (powf(10.f, (x) / 20.f))

#ifdef VQE_SIMULATE
#define AE_SIN(x) sinf(x)
#define AE_COS(x) cosf(x)
#else
#define AE_SIN(x) arm_sin_f32(x)
#define AE_COS(x) arm_cos_f32(x)
#endif

int ae_gcd(int u, int v);

int ipow(int base, int exp);

float ipowf(float base, int exp);

float pow_int(float base, int exp);

float expint(int n, float x);

float expint_approx(float x);

float sqrt_approx(float z);

#endif
