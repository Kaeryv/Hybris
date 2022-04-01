#include "lib.c"

#define POW2(X) ((X)*(X))
// MANUAL
//const u32 controlled_weights[] = {WINERTIA, WINERTIA, WHYBRIDATION, WINERTIA, WHYBRIDATION, WHYBRIDATION, WINERTIA, WHYBRIDATION};
// const union Controller crtls[] = {
//   { 29, 3, 11, 4, 2,  0 }, // Inertia
//   { 28, 3, 26, 3, 28, 0 }, // Inertia with hybridization
//   { 0,  4, 19, 6, 27, 0 }, // Hybridization with inertia
//   {31 ,0 ,16 ,0 ,0 ,2},   //3
//   {5 ,4 ,7 ,7 , 27 , 0},  //4
//   { 1,  2, 27, 6, 27, 1 },  // Hybridization
//   { 2 , 4 , 22 ,6 ,7 , 1 },  // Inertia 2
//   { 24 ,1 , 33 ,0 , 27 , 0} // Hy 2
//   //{ 3,  3, 13, 4, 25, 0 }  // Hybridization
// };
// BEST FOR homega
const union Controller crtls[] = {
//{26, 6, 25, 6, 7, 2, 1, 1,},
// {0, 6, 11, 7, 31, 2, 0, 0}//, nice!
//{31,4,23,0, 31, 1, 2, 1} // nice2new
//{4,2,33,0, 3, 0, 2, 2} // nice2
{43, 4, 18, 3, 21, 2, 1, 2 } // nice2
//{3,5,17,3, 12, 0, 1, 1} // nice2
//{19,5,27,1, 3, 2, 2, 0} // nice2
//	{6,7,32,4, 17, 1, 1, 2} // 
//	{30,4,31,2, 24, 2, 2, 1}
	//{31,4,29,3, 6, 2, 1, 2}
};
const u32 controlled_weights[] = {WINERTIA};
// BESTS FOR OMEGA ALONE
//const u32 controlled_weights[] = {WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, WINERTIA, };
//const union Controller crtls[] = {
//{1, 7, 15, 3, 12, 1, },
//{2, 5, 14, 0, 29, 1, },
//{23, 7, 33, 6, 27, 2, },
//{33, 0, 31, 1, 15, 0, },
//{23, 2, 12, 7, 2, 2, },
//{17, 3, 11, 0, 33, 2, },
//{33, 3, 29, 7, 31, 1, },
//{2, 7, 26, 3, 30, 0, },
//{16, 2, 30, 1, 2, 2, },
//{32, 0, 31, 4, 30, 0, },
//{4, 7, 15, 6, 23, 1, },
//{12, 7, 26, 7, 29, 2, },
//{24, 1, 15, 7, 23, 1, },
//{33, 7, 29, 4, 31, 1, },
//{2, 1, 22, 7, 26, 1, },
//{17, 5, 15, 1, 5, 2, },
//{28, 0, 31, 3, 28, 0, },
//{33, 4, 20, 1, 22, 1, },
//{24, 7, 31, 7, 15, 0, },
//{17, 7, 15, 3, 9, 2, },
//};
static int controlled = 0;
f64
EvaluateParameter(f64 probes[3], i32 rule) {
    f64 state[3*3];
    for_range(i, NUM_PROBES) {
        EvaluateMembership(conf_probes_membership[i], probes[i], &state[3*i]);
    }
    f64 ostate[3];

    EvaluateRule(rule, state, ostate);
    int wid = controlled_weights[controlled];
    return EvaluateContinuousValue(conf_controllers_membership[wid], ostate);
}

#ifndef RESOLUTION
#define RESOLUTION (200)
#endif

i32 
main(i32 argc, str argv[]) {
    for (i32 controlled = 0; controlled < lenof(controlled_weights); controlled++)
    {
        union Controller crtl = crtls[controlled];
        // We want a 2D map taking as arguments phi and delta, multiple maps over time.
        i32 rule = rule_from_points(crtl);

        

        f64 map[3*POW2(RESOLUTION)];
        f64 values[] = { 0.2, 0.55, 0.8 }; 
        for_range(t, 3) {
            for_range(i, RESOLUTION) {
                for_range(j, RESOLUTION) {
                    f64 current_probes[3] = {-1 + j * (2./(float)RESOLUTION), -5.5+i*(5.5/(float)RESOLUTION), values[t]};
                    f64 val = EvaluateParameter(current_probes, rule);
                    map[t*POW2(RESOLUTION)+i*RESOLUTION+j] = val;
                }
            }
        }
        char buffer[256];
        sprintf(buffer, "new_controlsuface_%d.npy", controlled);
        printf("%s\n", buffer);
        NumpyBinarySave(map, buffer, ARRSHAPE(3, RESOLUTION, RESOLUTION));
    }
    return EXIT_SUCCESS;
}
