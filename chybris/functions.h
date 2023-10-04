#pragma once

#include "core.h"

// If cec module is present, this struct is overriden.
#ifndef CECBENCH_MODULE
typedef struct  {
  pcg32_random_t *prng;
} fun_state;
#else
#endif

typedef void (*testcase_t)(const f64*, const i32, const i32, f64*, fun_state state);

typedef struct {
  const char * name;
  testcase_t function;
  f64 lower;
  f64 upper;
  u32 flags;
} TestCase;


#ifndef HYBRIS_DISABLE_TESTCASES

#define TEST_CASE(X) \
  extern void fn_ ## X (const f64 *noalias x, \
          const i32 num_agents, \
          const i32 num_dimensions, \
          f64 *noalias aptitude,\
          fun_state state)


#define POW2(X) ((X)*(X))

#define AGTPOS(i, j) (x[(i) * num_dimensions + (j)])

#ifdef CECBENCH_MODULE
// If the horrendous cec functions were included we need the wrapper.
#define IMPL_CEC_WRAPPER(N) \
  void cec_##N(const f64 *noalias x,\
               const i32 na, const i32 nd,\
               f64* f,\
               fun_state state)\
  {\
    cec20_test_func(x, f, nd, na, N, state.cecglobals);\
  }
#define DECL_CEC_WRAPPER(N) \
  void cec_##N(const f64 *noalias x,\
               const i32 na, const i32 nd,\
               f64* f,\
               fun_state state)
#endif

TEST_CASE(ackley);
TEST_CASE(alpine1);
TEST_CASE(alpine2);
TEST_CASE(absolute);
TEST_CASE(bentcigar);
TEST_CASE(chungreynolds);
TEST_CASE(eggholder);
TEST_CASE(elliptic);
TEST_CASE(griewank);
TEST_CASE(happycat);
TEST_CASE(hyperellipsoid);
TEST_CASE(michalewicz);
TEST_CASE(norwegian);
TEST_CASE(quadric);
TEST_CASE(quartic);
TEST_CASE(quing);
TEST_CASE(rastrigin);
TEST_CASE(rosenbrock);
TEST_CASE(saloman);
TEST_CASE(schwefel);
//TEST_CASE(schwefel12);
//TEST_CASE(schwefel221);
//TEST_CASE(schwefel222);
TEST_CASE(schaffer6);
TEST_CASE(shubert);
TEST_CASE(sphere);
TEST_CASE(stiblinskitank);
TEST_CASE(step);
TEST_CASE(vincent);
TEST_CASE(xinsheyang1);
TEST_CASE(xinsheyang2);

#ifdef CECBENCH_MODULE
DECL_CEC_WRAPPER(1);
DECL_CEC_WRAPPER(2);
DECL_CEC_WRAPPER(3);
DECL_CEC_WRAPPER(4);
DECL_CEC_WRAPPER(5);
DECL_CEC_WRAPPER(6);
DECL_CEC_WRAPPER(7);
DECL_CEC_WRAPPER(8);
DECL_CEC_WRAPPER(9);
DECL_CEC_WRAPPER(10);
#endif


u32
get_num_filtered_testcases(i32 mask);

u32
get_num_testcases();

TestCase*
get_filtered_testcases(i32 mask);

TestCase
get_testcase_by_id(i32 id);


#define TRAINING     BIT(0)
#define VALIDATION   BIT(1)
#define SURFACEABLE  BIT(2)
#define UNIMODAL     BIT(3)
#define MULTIMODAL   BIT(4)
#define SEPARABLE    BIT(5)
#define NONSEPARABLE BIT(6)

/*
 * Benchmark functions with proposed boundaries.
 * This table provides a gathering of all available functions in this file along with nice defaults for boundaries.
 */
global TestCase test_cases_map[] = {
  { name: "sphere",         function: fn_sphere,          lower: -    5.0,   upper:    5.0   , flags: TRAINING   | SURFACEABLE },
  { name: "ackley",         function: fn_ackley,          lower: -   32.775, upper:   32.775 , flags: TRAINING   | SURFACEABLE },
  { name: "rastrigin",      function: fn_rastrigin,       lower: -   5.12,   upper:    5.12   , flags: TRAINING   | SURFACEABLE },
  { name: "rosenbrock",     function: fn_rosenbrock,      lower: -   2.048,  upper:    2.048  , flags: TRAINING   | SURFACEABLE },
  { name: "stiblinskitank", function: fn_stiblinskitank,  lower: -   5.0,    upper:    5.0    , flags: TRAINING   | SURFACEABLE },
  { name: "schwefel",       function: fn_schwefel,        lower: - 500.0,    upper:  500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "griewank",       function: fn_griewank,        lower: - 600.0,    upper:  600.0    , flags: TRAINING   | SURFACEABLE },
  { name: "chungreynolds",  function: fn_chungreynolds,   lower: - 500.0,    upper:  500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "alpine1",        function: fn_alpine1,         lower:     0.0,    upper:   10.0    , flags: 0          | SURFACEABLE },
  { name: "alpine2",        function: fn_alpine2,         lower:     0.0,    upper:   10.0    , flags: 0          | SURFACEABLE },
  { name: "quing",          function: fn_quing,           lower: - 500.0,    upper:  500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "happycat",       function: fn_happycat,        lower: -   2.0,    upper:    2.0    , flags: TRAINING   | SURFACEABLE },
  { name: "saloman",        function: fn_saloman,         lower: -  30.0,    upper:   30.0    , flags: TRAINING   | SURFACEABLE },
  { name: "xinsheyang1",    function: fn_xinsheyang1,     lower: -   5.0,    upper:    5.0    , flags: TRAINING   | SURFACEABLE },
  { name: "xinsheyang2",    function: fn_xinsheyang2,     lower: -   6.2832, upper:    6.2832 , flags: TRAINING   | SURFACEABLE },
  { name: "bentcigar",      function: fn_bentcigar,       lower: - 100.0   , upper:  100.0    , flags: TRAINING   | SURFACEABLE },
  { name: "absolute",       function: fn_absolute,        lower: - 100.0   , upper:  100.0    , flags: TRAINING   | SURFACEABLE },
  { name: "eggholder",      function: fn_eggholder,       lower: - 512.0   , upper:  512.0    , flags: TRAINING   | SURFACEABLE },
  { name: "elliptic",       function: fn_elliptic,        lower: - 100.0   , upper:  100.0    , flags: TRAINING   | SURFACEABLE },
  { name: "hyperellipsoid", function: fn_hyperellipsoid,  lower: -   5.12  , upper:    5.12   , flags: TRAINING   | SURFACEABLE },
  { name: "michalewicz",    function: fn_michalewicz,     lower:     0.0  ,  upper:    M_PI   , flags: TRAINING   | SURFACEABLE },
  { name: "norwegian",      function: fn_norwegian,       lower: -   1.1  ,  upper:    1.1    , flags: TRAINING   | SURFACEABLE },
  { name: "quadric",        function: fn_quadric,         lower: - 100.0  ,  upper:  100.0    , flags: TRAINING   | SURFACEABLE },
  { name: "quartic",        function: fn_quartic,         lower: -   1.28  , upper:  1.28   , flags: TRAINING   | SURFACEABLE },
  { name: "schaffer6",      function: fn_schaffer6,       lower: - 100.00  , upper: 100.0    , flags: TRAINING   | SURFACEABLE },
  //{ name: "schwefel12",     function: fn_schwefel12,      lower: - 100.00  , upper: 100.0    , flags: TRAINING   | SURFACEABLE },
  //{ name: "schwefel221",    function: fn_schwefel221,     lower: - 100.00  , upper: 100.0    , flags: TRAINING   | SURFACEABLE },
  //{ name: "schwefel222",    function: fn_schwefel222,     lower: -  10.00  , upper:  10.0    , flags: TRAINING   | SURFACEABLE },
  { name: "shubert",        function: fn_shubert,         lower: -  10.00  , upper:  10.0    , flags: TRAINING   | SURFACEABLE },
  { name: "step",           function: fn_step,            lower: - 100.00  , upper: 100.0    , flags: TRAINING   | SURFACEABLE },
  { name: "vincent",        function: fn_vincent,         lower:     0.25  , upper:  10.0    , flags: TRAINING   | SURFACEABLE },
#ifdef CECBENCH_MODULE
  { name: "cec_1",          function: cec_1,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_2",          function: cec_2,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_3",          function: cec_3,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_4",          function: cec_4,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_5",          function: cec_5,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION               },
  { name: "cec_6",          function: cec_6,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION               },
  { name: "cec_7",          function: cec_7,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION               },
  { name: "cec_8",          function: cec_8,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_9",          function: cec_9,           lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
  { name: "cec_10",         function: cec_10,          lower: - 100.0   , upper: 100.0    , flags: VALIDATION | SURFACEABLE },
#endif
};

#ifdef FUNCTIONS_IMPLEMENTATION
#ifndef CECBENCH_MODULE
volatile int cec_bench_present = false;
#else
#endif

#ifdef CECBENCH_MODULE
IMPL_CEC_WRAPPER(1)
IMPL_CEC_WRAPPER(2)
IMPL_CEC_WRAPPER(3)
IMPL_CEC_WRAPPER(4)
IMPL_CEC_WRAPPER(5)
IMPL_CEC_WRAPPER(6)
IMPL_CEC_WRAPPER(7)
IMPL_CEC_WRAPPER(8)
IMPL_CEC_WRAPPER(9)
IMPL_CEC_WRAPPER(10)
#endif


u32
get_num_testcases() {
  return lenof(test_cases_map);
}
u32
get_num_filtered_testcases(i32 mask)
{
  u32 total_num_cases = sizeof(test_cases_map) / sizeof(TestCase);
  u32 result = 0;

  for_range(i, total_num_cases) {
    if (test_cases_map[i].flags & mask)
      result++;
  }

  return result;
}

TestCase*
get_filtered_testcases(i32 mask)
{
  u32 num_cases = get_num_filtered_testcases(mask);
  TestCase * result = malloc(num_cases*sizeof(TestCase));
  u32 total_num_cases = sizeof(test_cases_map) / sizeof(TestCase);
  u32 counter = 0;
  for_range(i, total_num_cases) {
    if (test_cases_map[i].flags & mask)
      result[counter++] = test_cases_map[i];
  }

  return result;
}

TestCase
get_testcase_by_id(i32 id) {
  return test_cases_map[id];
}


TEST_CASE(absolute)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += AbsoluteValue(AGTPOS(i,j));
     aptitude[i] = acc;
   }
}

TEST_CASE(elliptic)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += pow(1e6, j / (f64) (num_dimensions - 1) ) * AGTPOS(i, j);
     aptitude[i] = acc;
   }
}

TEST_CASE(hyperellipsoid)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += ((f64) j + 1.0) * AGTPOS(i,j) * AGTPOS(i, j);
     aptitude[i] = acc;
   }
}

TEST_CASE(michalewicz)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += sin(AGTPOS(i, j)) * pow(sin((f64) (j+1) * POW2(AGTPOS(i, j)) / M_PI), 20.0);
     aptitude[i] = - acc;
   }
}

TEST_CASE(norwegian)
{
   for_range(i, num_agents)
   {
     f64 acc = 1.0;
     for_range(j, num_dimensions) acc *= cos(M_PI*POW3(AGTPOS(i,j))) * ( (99.0 + AGTPOS(i,j)) / 100.0 );
     aptitude[i] = acc;
   }
}

TEST_CASE(quadric)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions)
     {
       f64 f1 = 0.0;
       for_range(k, j+1) f1 += AGTPOS(i,k);
       acc += POW2(f1);
     }
     aptitude[i] =  acc;
   }
}

TEST_CASE(quartic)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += (f64) (j+1) * POW2(POW2(AGTPOS(i, j)));
     aptitude[i] =  acc;
   }
}

TEST_CASE(eggholder)
{
   DEBUG_ASSERT(num_dimensions > 1, "Not enough dimensions for this function");
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions-1) 
     {
       float xj  = AGTPOS(i,j);
       float xj1 = AGTPOS(i,j+1);
       acc += - (xj1 + 47) * sin(sqrt(AbsoluteValue(xj1 + xj / 2.0 + 47)));
       acc += - xj * sin(sqrt(AbsoluteValue(xj-(xj1+47))));
     }
     aptitude[i] = acc;
   }
}

TEST_CASE(sphere)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += POW2(AGTPOS(i,j));
     aptitude[i] = acc;
   }
}

TEST_CASE(ackley)
{
  const f64 a = 20.0;
  const f64 b = 0.2;
  
  for_range(i, num_agents)
  {
    f64 sum_x2 = 0.0;
    f64 sum_coscx = 0.0;
    for_range(j, num_dimensions)
    {
      sum_x2 += POW2(AGTPOS(i, j));
      sum_coscx += cos(2.0 * M_PI * AGTPOS(i,j));
    }

    aptitude[i] = + a + M_E - a * exp(-b * sqrt(sum_x2 / (f64) num_dimensions)) - exp(sum_coscx / (f64) num_dimensions);
  }
}

TEST_CASE(rastrigin) 
{
    for_range(i, num_agents) {
      f64 s = 10.0 * (f64) num_dimensions;
      for_range(j, num_dimensions) {
        const i32 index = num_dimensions * i + j;
        s += POW2(x[index]) - 10.0 * cos(2.0 * M_PI * x[index]);
      }

      aptitude[i] = s;
    }
}

TEST_CASE(rosenbrock)
{
  for_range(i, num_agents) {
    f64 y = 0.0;
    for_range(j, num_dimensions-1) {
      y += POW2(1.0 - AGTPOS(i,j));
      y += 100.0 * POW2(AGTPOS(i,j+1) - POW2(AGTPOS(i,j)));
    }
    aptitude[i] = y;
  }
}

TEST_CASE(schwefel)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 418.982887 * (f64) num_dimensions;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f -= x[index] * sin(sqrt(AbsoluteValue(x[index])));
    }
    aptitude[i] = f;
  }
}

TEST_CASE(stiblinskitank)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      const i32 index = i * num_dimensions + j;
      f += (x[index]*x[index]*x[index]*x[index]-16.0*x[index]*x[index]+5.0*x[index]);
    }

    aptitude[i] = 39.1661658 * (f64) num_dimensions + 0.5 * f;

  }
}

TEST_CASE(chungreynolds)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    for (i32 j = 0; j < num_dimensions; j++) f += x[i*num_dimensions+j] * x[i*num_dimensions+j];
    aptitude[i] = f * f; 
  }
}

TEST_CASE(alpine1)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f += AbsoluteValue((x[index] * 0.1) + (x[index] * sin(x[index])));
    }
    aptitude[i] = f; 
  }
}

TEST_CASE(alpine2)
{
  for (i32 i = 0; i < num_agents; i++)
  {

    f64 f1 = 1.0; // Product op of x_i
    f64 f2 = 1.0; // Product of sin(x_i)
    for_range(j, num_dimensions) {
      f1 *= AGTPOS(i, j);
      f2 *= sin(AGTPOS(i,j)); 
    }
    aptitude[i] = sqrt(f1) * f2;
  }
}

TEST_CASE(xinsheyang2)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    f64 a = 0.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f += sin(x[index] * x[index]);
      a += AbsoluteValue(x[index]);
    }
    aptitude[i] = a * exp(-f);
  }
}

TEST_CASE(xinsheyang1) 
{ 
  f64 rng = (pcg32_random(state.prng) % 100000) / (100000.-1.);
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      
      f += rng  * AbsoluteValue(pow(x[index], j/10));
    }
    aptitude[i] = f;
  }
}

TEST_CASE(griewank) 
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 a = 0.0;
    f64 b = 1.0;

    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      b *= cos(x[index] / sqrt((f64) (j+1.0)));
      a += x[i*num_dimensions+j] * x[i*num_dimensions+j];
    }
    a /= 4000.0;
    aptitude[i] = a-b+1.;
  }
}

TEST_CASE(happycat) 
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 xx = 0.0;
    for (i32 j = 0; j < num_dimensions; j++) xx += x[i*num_dimensions+j] * x[i*num_dimensions+j];
    f64 alpha = 0.125;
    f64 n = (f64) num_dimensions;
    f64 sum = 0.0;
    for(i32 j = 0; j < num_dimensions; j++) sum+=x[i*num_dimensions+j];
    aptitude[i] = pow(POW2(xx-n),alpha)+(0.5*xx+sum)/n+0.5;
  }
}

TEST_CASE(quing)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f += POW2(x[index] * x[index] - j - 1.0);
    }
    aptitude[i] = f;
  }
}

TEST_CASE(saloman) 
{
  for_range(i, num_agents)
  {
    f64 f = 0.0;
    f64 a = 0.0;
    for_range(j, num_dimensions) a += x[i*num_dimensions+j] * x[i*num_dimensions+j];

    f64 sqa = sqrt(a);

    f = 1.0 - cos(2.0 * M_PI * sqa) + (0.1 * sqa);
    
    aptitude[i] = f;
  }
}

TEST_CASE(schaffer6) 
{
  for_range(i, num_agents)
  {
    f64 f = 0.0;
    for_range(j, num_dimensions-1) {
      float xj  = AGTPOS(i,j);
      float xj1 = AGTPOS(i,j+1);
      f += 0.5 + (POW2(sin(POW2(xj) + POW2(xj1))) - 0.5) / POW2(1 + 0.001 * (POW2(xj) + POW2(xj1) ));
    }
    aptitude[i] = f;
  }
}

TEST_CASE(shubert) 
{
  for_range(i, num_agents)
  {
    f64 f = 1.0;
    for_range(j, num_dimensions) {
      f64 f1 = 0.0;
      for_range(k, 5) {
        f1 += (k+1) * cos((k+2)*AGTPOS(i, j) + k + 1);
      }
      f *= f1;
    }
    aptitude[i] = f;
  }
}

TEST_CASE(step)
{
  for_range(i, num_agents) {
    f64 f = 0.0;
    for_range(j, num_dimensions) f += POW2(AGTPOS(i,j) + 0.5);
    aptitude[i] = f;
  }
}

TEST_CASE(vincent)
{
  for_range(i, num_agents) {
    f64 f = 0.0;
    for_range(j, num_dimensions) f += sin(10.0 * sqrt(AGTPOS(i,j)));
    aptitude[i] = - 1.0 - f;
  }
}

TEST_CASE(bentcigar)
{
  for_range(i, num_agents) {
    f64 f = POW2(x[i*num_dimensions+0]);
    for(i32 j = 1; j < num_dimensions; j++) {
      f += 10e+6 * POW2(x[i*num_dimensions+j]);
    }
    aptitude[i] = f;
  }
}
#undef POW2
#undef AGTPOS
#endif
#endif
