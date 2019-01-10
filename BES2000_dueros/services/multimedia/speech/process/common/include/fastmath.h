#ifndef FASTMATH_H
#define FASTMATH_H

#define FASTMATH_LOG2_TABLE_MIN (1.f)
#define FASTMATH_LOG2_TABLE_MAX (2.f)
#define FASTMATH_LOG2_TABLE_SIZE (33)
#define FASTMATH_LOG2_TABLE_STEP ((FASTMATH_LOG2_TABLE_MAX - FASTMATH_LOG2_TABLE_MIN) / (FASTMATH_LOG2_TABLE_SIZE - 1))
#define FASTMATH_LOG2_TABLE_STEP_INV (1.f / FASTMATH_LOG2_TABLE_STEP)

#define FASTMATH_POW2_TABLE_MIN (0.f)
#define FASTMATH_POW2_TABLE_MAX (1.f)
#define FASTMATH_POW2_TABLE_SIZE (33)
#define FASTMATH_POW2_TABLE_STEP ((FASTMATH_POW2_TABLE_MAX - FASTMATH_POW2_TABLE_MIN) / (FASTMATH_POW2_TABLE_SIZE - 1))
#define FASTMATH_POW2_TABLE_STEP_INV (1.f / FASTMATH_POW2_TABLE_STEP)

float fastlog10(float x);

float fastpow10(float x);

float fastlog2(float x);

float fastpow2(float x);

float fastlog(float x);

float fastexp(float x);

float fastpow(float x, float y);

#endif