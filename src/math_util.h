#pragma once

#include <stdint.h>

#define MATH_PI 3.14159265358979323846f
#define EPSILON 0.000001f

static uint32_t rng_state;

void TinyRT_InitRNG(void);
float TinyRT_RandomFloat(void);
float TinyRT_RandomFloatRange(float min, float max);

static inline uint32_t xorshift32(uint32_t* state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}
