#pragma once

#include "core.h"

typedef struct {
  u64 state;
  u64 inc;
} pcg32_random_t;

u32
pcg32_random(pcg32_random_t *prng);

subroutine
pcg32_random_seed(pcg32_random_t *prng, u64 s1);

u32
pcg32_random_sample(pcg32_random_t *prng, f64 *distribution, u32 len);


#ifdef RANDOM_IMPLEMENTATION

u32
pcg32_random(pcg32_random_t *prng) {
  HARD_ASSERT(prng, "PRNG state not initialized.");
  u64 oldstate = prng->state;
  prng->state = oldstate * 6364136223846793005ULL + prng->inc;
  u32 xshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  u32 rot = oldstate >> 59u;
  return (xshifted >> rot) | (xshifted << ((-rot) & 31));
}

subroutine
pcg32_random_seed(pcg32_random_t *prng, u64 s1) {
  u64 s2 = 42lu;
  prng->state = 0U;
  prng->inc = (s2 << 1u) | 1u;
  pcg32_random(prng);
  prng->state += s1;
  pcg32_random(prng);
}

u32
pcg32_random_sample(pcg32_random_t *prng, f64 *distribution, u32 len) {
  f64 cumul = 0.0;
  f64 alpha = (((f64) ((1+pcg32_random(prng)) & 0xffffffff)) / ((f64) 0xffffffff)) * arr_f64_sum(distribution, len);
  for_range(i, len) {
    cumul += distribution[i];
    if (alpha < cumul) {
      return i;
    }
  }
  return 0;
}

#endif
