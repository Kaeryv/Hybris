#include "core.h"

#define PGC_RANDOM_IMPLEMENTATION
#include "random.h"

#define CNPY_IMPLEMENTATION
#include "cnpy.h"

#define FUZZY_IMPLEMENTATION
#include "fuzzy.h"


#include "cecbench.h"
#include "functions.h"

#define PSO_IMPLEMENTATION
#include "pso.h"
#include "arrtools.h"
#include "ga.h"

#define optimized WHYBRIDATION


shaped_array_t db;
f64
evaluate_benchmark(u32 control) {
  u32 num_cases = get_num_filtered_testcases(TRAINING);
  TestCase *problems = get_filtered_testcases(TRAINING);
  f64 mean = 0.0;
  registry_t *reg = registry_create(40, 50, 1000);
  for_range(k, num_cases) {
    //reg->controllers[0] = control;
    //HARD_ASSERT(!(reg->controllers[0] & BIT(31)), "CONTROL BIT");
    registry_init(reg, problems[k], 43);
    reg->controllers[optimized] = control;
    registry_iterate_n(reg, problems[k], 1000);
    i32 res = arr_f64_searchsorted(&db.data[k*db.shape[1]], reg->profile[999], db.shape[1]);
    //printf("%u ", res);

    mean += res;
  }
  //printf("\nover %u\n", db.shape[1]);

  registry_free(reg);
  free(problems);
  //log_error("Resulting rank: %lf %lf \n", 1.0 * res, reg->profile[999]);
  return mean / (f64) num_cases;
}

subroutine
metafitness(const u32 *population, f64 *fitness, const u32 popsize) {
  for_range(i, popsize) {
    fitness[i] = evaluate_benchmark(population[i]);
  }
}

i32
main() {
  struct registry reg = ga_create(60);
  reg.fun = metafitness;

  // We need to acquire the database
  db = NumpyLoadBinary("./db/warmup/full.npy");
  log_info("Loaded the database: [%d,%d].\n", db.shape[0], db.shape[1]);

  f64 current_best = INFINITY;
  for_range(t, 30) {
    ga_evaluate(reg);
    ga_selection_crossover(&reg);
    ga_mutate(&reg);
    ga_update(&reg);
    i32 ib = argmin(reg.fitness, reg.size);

    current_best = reg.fitness[ib] < current_best ? reg.fitness[ib] : current_best;
    //printf("%E %E\n", (f64)reg.chromosomes[ib]/(f64)RAND_MAX, reg.fitness[ib]);
    printf("%lf\n", current_best);
  }

  return EXIT_SUCCESS;
}
