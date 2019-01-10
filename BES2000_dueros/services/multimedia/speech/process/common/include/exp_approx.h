#ifndef EXP_APPROX_H
#define EXP_APPROX_H

#include <stdint.h>

/*
 * Reference:
 * https://github.com/jhjourdan/SIMD-math-prims/blob/master/simd_math_prims.h
 */

/* Workaround a lack of optimization in gcc */
float exp_cst1 = 2139095040.f;
float exp_cst2 = 0.f;

/* Relative error bounded by 1e-5 for normalized outputs
Returns invalid outputs for nan inputs
Continuous error */
static inline float exp_approx(float val) {
    union { int32_t i; float f; } xu, xu2;
    float val2, val3, val4, b;
    int32_t val4i;
    val2 = 12102203.1615614f*val + 1065353216.f;
    val3 = val2 < exp_cst1 ? val2 : exp_cst1;
    val4 = val3 > exp_cst2 ? val3 : exp_cst2;
    val4i = (int32_t)val4;
    xu.i = val4i & 0x7F800000;
    xu2.i = (val4i & 0x7FFFFF) | 0x3F800000;
    b = xu2.f;

    /* Generated in Sollya with:
    > f=remez(1-x*exp(-(x-1)*log(2)),
    [|(x-1)*(x-2), (x-1)*(x-2)*x, (x-1)*(x-2)*x*x|],
    [1.000001,1.999999], exp(-(x-1)*log(2)));
    > plot(exp((x-1)*log(2))/(f+x)-1, [1,2]);
    > f+x;
    */
    return
        xu.f * (0.509871020343597804469416f + b *
        (0.312146713032169896138863f + b *
            (0.166617139319965966118107f + b *
            (-2.19061993049215080032874e-3f + b *
                1.3555747234758484073940937e-2f))));
}

#endif
