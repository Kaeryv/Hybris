#include <hybris.h>

/**
 * @brief An example to show how to meta-optimize the optimizer using the C interface.
 * 
 */

TestCase* problems;
shaped_array_t db;

subroutine
test(f64 *x, u32 num_agents, u32 num_dimensions, f64 *aptitude, struct pcg32_random *prng) {
    const i32 optimized_num_agents = 40;
    const i32 optimized_num_dimensions = 50;
    const i32 optimized_num_iters = 1000;
    for_range(i, num_agents) {
        aptitude[i] = 0.;
        registry_t *reg1 = registry_create(optimized_num_agents, optimized_num_dimensions, 0, optimized_num_iters);
        
        // Create two controllers for the two parameters to control.
        union Controller crtl = { 0 };
        union Controller crtl2 = { 0 };
        // The input of this objective function are the controller's parameters
        // X = [ crtl1[:], crtl2[:] ]
        for_range(j, num_dimensions / 2) {
            crtl.elements[j] = (i32) x[i * num_dimensions + j];
            crtl2.elements[j] = (i32) x[i * num_dimensions + j + num_dimensions / 2];
        }
        
        for_range(k, get_num_filtered_testcases(TRAINING)){
            reg1->controllers[0] = rule_from_points(crtl);
            reg1->controllers[3] = rule_from_points(crtl2);
            reg1->upper_bound = problems[k].upper;
            reg1->lower_bound = problems[k].lower;
            registry_init(reg1, 42);
            for_range(l, optimized_num_iters) {
                reg_update_population(reg1);
                struct fun_state fun_state = { .prng= prng };
                problems[k].function(reg1->position, optimized_num_agents, optimized_num_dimensions, reg1->aptitude, fun_state);
                reg_process_fitness(reg1);
            }
            f64 rank = arr_f64_searchsorted(&db.data[k*db.shape[1]], reg1->profile[optimized_num_iters-1], db.shape[1]);
            aptitude[i] += rank;
        }
        aptitude[i] /= (float) get_num_filtered_testcases(TRAINING);
        registry_free(reg1);
    }
}

subroutine
setup_categories(registry_t *reg, i32 var, u32 num_qualities) {
    reg_set_num_categories(reg, var + 0, num_qualities);
    reg_set_num_categories(reg, var + 1, 8);
    reg_set_num_categories(reg, var + 2, num_qualities);
    reg_set_num_categories(reg, var + 3, 8);
    reg_set_num_categories(reg, var + 4, num_qualities);
    reg_set_num_categories(reg, var + 5, 3);
    reg_set_num_categories(reg, var + 6, 3);
    reg_set_num_categories(reg, var + 7, 3);
}

i32
main(i32 argc, str argv[]) {
    db = npy_load("./db/warmup/full.npy");

    problems = get_filtered_testcases(TRAINING);

    registry_t *reg = registry_create(10, 16, 16, 100);
    u32 num_qualities = fuzz_get_num_qualities_combinations();

    setup_categories(reg, 0, num_qualities);
    setup_categories(reg, 8, num_qualities);

    registry_init(reg, atoi(argv[1]));
    for_range(i, 100) {
        reg_update_population(reg);
        test(reg->position, 10, 16, reg->aptitude, reg->prng);
        reg_process_fitness(reg);
        printf("f_star = %lf\n", reg->profile[i]);
    }

    registry_free(reg);
    return EXIT_SUCCESS;
}
