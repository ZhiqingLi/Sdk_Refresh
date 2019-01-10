#ifndef EXPINT_APPROX_H
#define EXPINT_APPROX_H

#include "log_approx.h"
#include "exp_approx.h"

/*
 * This function computes an approximation to the exponential integral
 * e ~integral from x to infinity of exp(-t) / t
 * References:
 * [1] Abramowitz, M. and I. A. Stegun. Handbook of Mathematical Functions.
 *     Chapter 5, New York: Dover Publications, 1965.
 */
float expint_approx(float x)
{
    float e;
    if (x < 1.f) {
        // Uses the log() plus a polynomial for small values of x.
        e = -log_approx(x) - 0.57721566f + x * (0.99999193f
            - x * (0.24991055f - x * (0.05519968f
                - x * (0.00976004f - x * 0.00107857f))));
    } else {
        // Uses a ratio of polynomials for larger values of x.
        e = exp_approx(-x)* (0.250621f + x * (2.334733f + x)) /
            ((1.681534f + x * (3.330657f + x))*x);
    }

    return e;
}

#endif
