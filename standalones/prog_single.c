#include "lib.c"
#include "cnpy.h"



i32
main(i32 argc, str argv[]) {
  int popsize = 40;
    TestCase *problems = get_filtered_testcases(TRAINING);
    TestCase prob = problems[1];
    free(problems);

    registry_t *reg = registry_create(popsize, 10, 0, 100*100);
    reg->upper_bound = 2.0;//prob.upper;
    reg->lower_bound = -3.0; //prob.lower;
    registry_init(reg, atoi(argv[1]));
    for_range(i, 100) {
        reg_update_population(reg);
        fun_state fs = { .prng= reg->prng };
        prob.function(reg->position, popsize, 10, reg->aptitude, fs);
        reg_process_fitness(reg);
    }
    printf("%d\n", 100*100/popsize-1);
    printf("f optimal = %e\n", reg->profile[-1+100*100/popsize]);

    registry_free(reg);
    return EXIT_SUCCESS;
}
