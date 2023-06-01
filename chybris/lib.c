#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "core.h"


#define ARRTOOLS_IMPLEMENTATION
#include "arrtools.h"

#define RANDOM_IMPLEMENTATION
#include "random.h"

#define CNPY_IMPLEMENTATION
#include "cnpy.h"

#define FUZZY_IMPLEMENTATION
#include "fuzzy.h"

#ifdef ADD_CECBENCHMARK
#define CECBENCH_IMPLEMENTATION
#include "cecbench.h"
#endif

#define FUNCTIONS_IMPLEMENTATION
#include "functions.h"

#define PSO_IMPLEMENTATION
#include "pso.h"



