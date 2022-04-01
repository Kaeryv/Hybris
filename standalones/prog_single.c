#include "lib.c"
#include "cnpy.h"



i32
main(i32 argc, str argv[]) {
    TestCase *problems = get_filtered_testcases(TRAINING);
    TestCase prob = problems[0];
    free(problems);

    registry_t *reg = registry_create(40, 8, 0, 1000);
    reg->upper_bound = prob.upper;
    reg->lower_bound = prob.lower;
    registry_init(reg, atoi(argv[1]));
    for_range(i, 1000) {
        reg_update_population(reg);
        struct fun_state fun_state = { .prng= reg->prng };
        prob.function(reg->position, 10, 8, reg->aptitude, fun_state);
        reg_process_fitness(reg);
    }
    printf("f optimal = %lf\n", reg->profile[999]);

    registry_free(reg);
    return EXIT_SUCCESS;
}
