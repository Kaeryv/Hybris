#include <hybris.h>

#define N 10
#define D 5

TEST_CASE(test) {
    for_range(i, num_agents) {
        f64 f = 0.;
        for_range(j, num_dimensions) {
            f = x[i*num_dimensions+j];
        }
        aptitude[i] = f;
    }
}

i32
main(i32 argc, str argv[]) {
    printf("hello\n");
    TestCase prob;
    prob.lower = 4.0;
    prob.upper = 50.0;
    prob.function = test;

    registry_t *reg = registry_create(N, D, 2, 100);
    for_range(i, 2) {
        reg_set_num_categories(reg, i, 5);
    }
    registry_init(reg, atoi(argv[1]));
    reg_minimize_problem(reg, prob, 10);

    for_range(i, reg_get_num_categories(reg, 1))
        printf("%d %lf\n", i, reg_get_categorical_probability(reg, 0)[i]);
    printf("exit\n");
    printf("f_star = %lf\n", reg->profile[99]);
    registry_free(reg);
    return EXIT_SUCCESS;
}
