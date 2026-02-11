#include "math_util.h"
#include <time.h>

void TinyRT_InitRNG(void)
{
    rng_state = (uint32_t)time(NULL);
}

float TinyRT_RandomFloat(void) {
    uint32_t value = xorshift32(&rng_state);
    return (float)value / (float)UINT32_MAX;
}

float TinyRT_RandomFloatRange(float min, float max) {
    return min + TinyRT_RandomFloat() * (max - min);
}
