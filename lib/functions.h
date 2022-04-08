#pragma once

#include "core.h"

// If cec module is present, this struct is overriden.
#ifndef CECBENCH_MODULE
struct fun_state {
  struct pcg32_random *prng;
};
#endif

typedef void (*testcase_t)(const f64*, const i32, const i32, f64*, struct fun_state state);

typedef struct {
  const char * name;
  testcase_t function;
  f64 lower;
  f64 upper;
  u32 flags;
} TestCase;


#ifndef HYBRIS_DISABLE_TESTCASES

#define TEST_CASE(X) \
  void X (const f64 *noalias x, \
          const i32 num_agents, \
          const i32 num_dimensions, \
          f64 *noalias aptitude,\
          struct fun_state state)


#define POW2(X) ((X)*(X))

#define AGTPOS(i, j) (x[(i) * num_dimensions + (j)])

#ifdef CECBENCH_MODULE
// If the horrendous cec functions were included we need the wrapper.
#define IMPL_CEC_WRAPPER(N) \
  void cec_##N(const f64 *noalias x,\
               const i32 na, const i32 nd,\
               f64* f,\
               struct fun_state state)\
  {\
    cec20_test_func(x, f, nd, na, N, state.cecglobals);\
  }
#define DECL_CEC_WRAPPER(N) \
  void cec_##N(const f64 *noalias x,\
               const i32 na, const i32 nd,\
               f64* f,\
               struct fun_state state)
  

#endif

TEST_CASE(sphere);
TEST_CASE(ackley);
TEST_CASE(rastrigin);
TEST_CASE(rosenbrock);
TEST_CASE(stiblinskitank);
TEST_CASE(schwefel);
TEST_CASE(chungreynolds);
TEST_CASE(alpine);
TEST_CASE(alpine2);
TEST_CASE(griewank);
TEST_CASE(quing);
TEST_CASE(salomon);
TEST_CASE(happycat);
TEST_CASE(xinsheyang1);
TEST_CASE(xinsheyang2);
TEST_CASE(bentcigar);
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



u32
get_num_filtered_testcases(i32 mask);

TestCase*
get_filtered_testcases(i32 mask);

TestCase
get_testcase_by_id(i32 id);


#define TRAINING BIT(0)
#define VALIDATION BIT(1)
#define SURFACEABLE BIT(2)

global TestCase test_cases_map[] = {
  { name: "sphere",         function: sphere,          lower: -    5.0,   upper:    5.0   , flags: TRAINING   | SURFACEABLE },
  { name: "ackley",         function: ackley,          lower: -   32.775, upper:   32.775 , flags: TRAINING   | SURFACEABLE },
  { name: "rastrigin",      function: rastrigin,       lower: -   5.12,   upper:   5.12   , flags: TRAINING   | SURFACEABLE },
  { name: "rosenbrock",     function: rosenbrock,      lower: -   2.048,  upper:   2.048  , flags: TRAINING   | SURFACEABLE },
  { name: "stiblinskitank", function: stiblinskitank,  lower: -   5.0,    upper:   5.0    , flags: TRAINING   | SURFACEABLE },
  { name: "schwefel",       function: schwefel,        lower: - 500.0,    upper: 500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "griewank",       function: griewank,        lower: -  50.0,    upper:  50.0    , flags: TRAINING   | SURFACEABLE },
  { name: "chungreynolds",  function: chungreynolds,   lower: - 500.0,    upper: 500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "alpine",         function: alpine,          lower:     0.0,    upper:  10.0    , flags: 0          | SURFACEABLE },
  { name: "alpine2",        function: alpine2,         lower:     0.0,    upper:  10.0    , flags: 0          | SURFACEABLE },
  { name: "quing",          function: quing,           lower: - 500.0,    upper: 500.0    , flags: TRAINING   | SURFACEABLE },
  { name: "happycat",       function: happycat,        lower: -   2.0,    upper:   2.0    , flags: TRAINING   | SURFACEABLE },
  { name: "salomon",        function: salomon,         lower: -  30.0,    upper:  30.0    , flags: TRAINING   | SURFACEABLE },
  { name: "xinsheyang1",    function: xinsheyang1,     lower: -   5.0,    upper:   5.0    , flags: TRAINING   | SURFACEABLE },
  { name: "xinsheyang2",    function: xinsheyang2,     lower: -   6.2832, upper:   6.2832 , flags: TRAINING   | SURFACEABLE },
  { name: "bentcigar",      function: bentcigar,       lower: - 100.0   , upper: 100.0    , flags: TRAINING   | SURFACEABLE },
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



TEST_CASE(sphere)
{
   for_range(i, num_agents)
   {
     f64 acc = 0.0;
     for_range(j, num_dimensions) acc += POW2(AGTPOS(i,j));
     aptitude[i] = acc / (f64)num_dimensions;
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

TEST_CASE(alpine)
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
    f64 f = 1.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f *= sqrt(x[index]) * sin(x[index]); 
    }
    aptitude[i] = - f + pow(2.8085, num_dimensions); 
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
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    for (i32 j = 0; j < num_dimensions; j++)
    {
      i32 index = i * num_dimensions + j;
      f += pcg32_random(state.prng)  * AbsoluteValue(pow(x[index], j/10));
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

TEST_CASE(salomon) 
{
  for (i32 i = 0; i < num_agents; i++)
  {
    f64 f = 0.0;
    f64 a = 0.0;
    for (i32 j = 0; j < num_dimensions; j++) a += x[i*num_dimensions+j] * x[i*num_dimensions+j];

    f64 sqa = sqrt(a);

    f = 1.0 - cos(2.0 * M_PI * sqa) + (0.1 * sqa);
    
    aptitude[i] = f;
  }
}

TEST_CASE(bentcigar)
{
  for_range(i, num_agents) {
    double f = POW2(x[i*num_dimensions+0]);
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
