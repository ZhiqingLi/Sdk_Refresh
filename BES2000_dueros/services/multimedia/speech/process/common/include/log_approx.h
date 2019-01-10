#ifndef LOG_APPROX_H
#define LOG_APPROX_H

#include <stdint.h>
#include <math.h>

/*
 * Reference:
 * https://github.com/jhjourdan/SIMD-math-prims/blob/master/simd_math_prims.h
 */

/* Absolute error bounded by 1e-6 for normalized inputs
   Returns a finite number for +inf input
   Returns -inf for nan and <= 0 inputs.
   Continuous error. */
static inline float log_approx(float val) {
    union { float f; int32_t i; } valu;
    float exp, addcst, x;
    valu.f = val;
    exp = valu.i >> 23;
    /* 89.970756366f = 127 * log(2) - constant term of polynomial */
    addcst = val > 0 ? -89.970756366f : -(float)INFINITY;
    valu.i = (valu.i & 0x7FFFFF) | 0x3F800000;
    x = valu.f;


    /* Generated in Sollya using:
    > f = remez(log(x)-(x-1)*log(2),
    [|1,(x-1)*(x-2), (x-1)*(x-2)*x, (x-1)*(x-2)*x*x,
    (x-1)*(x-2)*x*x*x|], [1,2], 1, 1e-8);
    > plot(f+(x-1)*log(2)-log(x), [1,2]);
    > f+(x-1)*log(2)
    */
    return
        x * (3.529304993f + x * (-2.461222105f +
            x * (1.130626167f + x * (-0.288739945f +
                x * 3.110401639e-2f))))
        + (addcst + 0.69314718055995f*exp);
}

#endif