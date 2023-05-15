#include "lib.c"
#include "cnpy.h"



i32
main(i32 argc, str argv[]) {
    int popsize = 40;
    int fevals = 40 * 50;
    assert(fevals % popsize == 0);
    int iters = fevals / popsize;
    TestCase *problems = get_filtered_testcases(TRAINING);
    TestCase prob = problems[1];
    free(problems);

    conf_initial_weights[WHYBRIDATION] = 0.1;
    registry_t *reg = registry_create(popsize, 10, 0, iters);
    for_range(j, 10) reg->upper_bound[j] = 2.0;//prob.upper;
    for_range(j, 10) reg->lower_bound[j] = -3.0; //prob.lower;
    registry_init(reg, atoi(argv[1]));
    for_range(i, iters) {
        reg_update_population(reg);
        fun_state fs = { .prng= reg->prng };
        prob.function(reg->position, popsize, 10, reg->aptitude, fs);
        reg_process_fitness(reg);
        printf("%e\n", reg->profile[i]);
    }
    printf("f optimal = %e\n", reg->profile[iters-1]);

    registry_free(reg);
    return EXIT_SUCCESS;
}
