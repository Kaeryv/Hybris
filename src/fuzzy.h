
#include "core.h"

#define NO_CONTROL BIT(31)
#define NELEMENTS 8


// Représente le contrôlleur dans les tables de permutations
union Controller {
  i32 elements[NELEMENTS];
  struct {
    i32 iq1; // 2 BITS
    i32 io1; // 1 BIT
    i32 iq2; // 2 BITS
    i32 io2; // 1 BIT
    i32 iq3; // 2 BITS
    i32 ip1; // Priority of operations 2 BITS
    i32 ip2; // Priority of operations 2 BITS
    i32 ip3; // Priority of operations 2 BITS
  };
};

str*
cli2controller(str* argv, union Controller* controller);

subroutine 
EvaluateMembership (const f64* abc, const f64 value, f64 *noalias membership);

f64 
EvaluateContinuousValue(const f64 definition[3], f64 membership[3]);

subroutine 
EvaluateRule(u32 rule, const f64 * state, f64 * output_state);

subroutine 
WriteRuleStr(u32 rule, str text);

u32
fuzz_get_num_qualities_combinations();

u32 
rule_from_points(union Controller);

#if defined(FUZZY_IMPLEMENTATION)

internal
f64(*maxormin[2])(f64 x, f64 y) = {fmax, fmin};

str*
cli2controller(str* argv, union Controller* controller)
{
    for_range(i,LENOF(controller->elements)) {
        controller->elements[i] = atoi(*(argv++));
    }
    return argv;
}
// The qualities, slow dimension is the different combinaitons for a variable, fast one is the LOW, MEDIUM, HIGH outputs.
internal const i32 qualities_ids[] = {0, 1, 2, 3};
internal const i32 set3_permutations[][3] = {
  {0, 1, 2}, {0, 2, 1}, {1, 2, 0}, {1, 0, 2}, {2, 0, 1}, {2, 1, 0}
};
internal const i32 set3_associations[][3] = {
  {0, 1, 2}, {0, 2, 1}, {1, 2, 0}
};

#ifdef PSO_EVEN_MORE_COMBINATIONS
internal const i32 qualities_combinations[][3] = {
  {0, 0, 1}, {0, 0, 2}, {1, 1, 0}, {1, 1, 2}, {2, 2, 0}, {2, 2, 1}, // Double 01
  {0, 1, 0}, {0, 2, 0}, {1, 0, 1}, {1, 2, 1}, {2, 0, 2}, {2, 1, 2}, // Double 02
  {1, 0, 0}, {2, 0, 0}, {0, 1, 1}, {2, 1, 1}, {0, 2, 2}, {1, 2, 2}, // Double 12
  {3, 0, 2}, {0, 1, 2}, {0, 3, 1}, {0, 3, 2}, {0, 2, 3}, {0, 2, 1}, 
  {0, 1, 3}, {3, 1, 2}, {1, 0, 2}, {1, 2, 0}, {1, 2, 3}, {1, 3, 2}, {3, 0, 1},
  // Quasi neural
  {0, 3, 3}, {1, 3, 3}, {2, 3, 3}, {3, 0, 3}, {3, 1, 3}, {3, 2, 3},
  {3, 3, 0}, {3, 3, 1}, {3, 3, 2}, {3, 3, 3},
  // Descending
  {1, 0, 3},{1, 3, 0}, {2, 0, 1},  {2, 1, 3}, {2, 3, 0}, {2, 3, 1}, 
  {3, 1, 0},  {3, 2, 0}, {3, 2, 1}, {2, 0, 3}, {2, 1, 0}, 
};
#else
#ifndef PSO_MORE_COMBINATIONS
internal const i32 qualities_combinations[][3] = {
  {0, 1, 2}, {0, 1, 3}, {0, 2, 1}, {0, 2, 3}, {0, 3, 1}, {0, 3, 2},
  {1, 0, 2}, {1, 0, 3}, {1, 2, 0}, {1, 2, 3}, {1, 3, 0}, {1, 3, 2},
  {2, 0, 1}, {2, 0, 3}, {2, 1, 0}, {2, 1, 3}, {2, 3, 0}, {2, 3, 1}, 
  {3, 0, 1}, {3, 0, 2}, {3, 1, 0}, {3, 1, 2}, {3, 2, 0}, {3, 2, 1}
};
#else
internal const i32 qualities_combinations[][3] = {
  {0, 1, 2}, {0, 1, 3}, {0, 2, 1}, {0, 2, 3}, {0, 3, 1}, {0, 3, 2},
  {1, 0, 2}, {1, 0, 3}, {1, 2, 0}, {1, 2, 3}, {1, 3, 0}, {1, 3, 2},
  {2, 0, 1}, {2, 0, 3}, {2, 1, 0}, {2, 1, 3}, {2, 3, 0}, {2, 3, 1}, 
  {3, 0, 1}, {3, 0, 2}, {3, 1, 0}, {3, 1, 2}, {3, 2, 0}, {3, 2, 1},
  {0, 3, 3}, {1, 3, 3}, {2, 3, 3}, {3, 0, 3}, {3, 1, 3}, {3, 2, 3},
  {3, 3, 0}, {3, 3, 1}, {3, 3, 2}, {3, 3, 3}
};
#endif
#endif

u32
fuzz_get_num_qualities_combinations() {
  return LENOF(qualities_combinations);
}

internal const char * probes_names[] ={"fitness", "distance ", "time spent"};
internal const char * operators_names[] ={" or  ", " and "};
internal const char * states_names[4][4] = { 
  {"worse", "stagnating", "better", "nocare"},
  {"close", "medium", "far  ", "nocare"},
  {"begin", "amidst", "ending  ", "nocare"},
  {"low", "medium", "high  ", "nocare"}
};

static const size_t numquals = LENOF(qualities_combinations);

u32 
rule_from_points(union Controller c)
{
  const i32 *q1 = qualities_combinations[c.iq1];
  const i32 *q2 = qualities_combinations[c.iq2];
  const i32 *q3 = qualities_combinations[c.iq3];

  DEBUG_ASSERT((c.ip1 < 3 && c.ip1 >= 0), "Index of permutation 1 is not in [0,2]")
  DEBUG_ASSERT((c.ip2 < 3 && c.ip2 >= 0), "Index of permutation 2 is not in [0,2]")
  DEBUG_ASSERT((c.ip3 < 3 && c.ip3 >= 0), "Index of permutation 3 is not in [0,2]")

  /**
   * Integer encoding of the ruleset:
   * uint32_t
   * ______________________________________________________
   * --------    -- - -- - --   -- - -- - --   -- - -- - --
   * tx ip1 ip2 ip3    q1 o q2 o q3   q1 o q2 o q3   q1 o q2 o q3
   * --------------    ------------   ------------   ------------
   * tags              high           medium         low
   * |                 |              |              |
   * | << 32           | << 24        | << 16        | << 8
   **/

  // LOW
  u8 rule1 = 0b0;
  rule1 |= (q1[0] << 6);
  rule1 |= ((c.io1 & 0b1) << 5);
  rule1 |= (q2[0] << 3);
  rule1 |= ((c.io2 & 0b1) << 2);
  rule1 |= (q3[0]);

  // MEDIUM
  u8 rule2 = 0b0;
  rule2 |= (q1[1] << 6);
  rule2 |= ((c.io1 & 0b10) << 4);
  rule2 |= (q2[1] << 3);
  rule2 |= ((c.io2 & 0b10) << 1);
  rule2 |= (q3[1]);

  // HIGH
  u8 rule3 = 0b0;
  rule3 |= (q1[2] << 6);
  rule3 |= ((c.io1 & 0b100) << 3);
  rule3 |= (q2[2] << 3);
  rule3 |= ((c.io2 & 0b100));
  rule3 |= (q3[2]);

  u32 rule = 0b0;
  // rule    is  UNSET HIGH   MEDIUM  LOW  
  //            [0-7] [8-15] [16-23] [24-31]
  rule |= rule1;
  rule |= rule2 << 8;
  rule |= rule3 << 16;

  rule |= ((u32)c.ip1 & 0b11) << 24;
  rule |= ((u32)c.ip2 & 0b11) << 26;
  rule |= ((u32)c.ip3 & 0b11) << 28;

  return rule;
}

subroutine 
EvaluateMembership (const f64 *noalias abc, f64 value, f64 *noalias membership)
{
  membership[0] = 0.0;
  membership[1] = 0.0;
  membership[2] = 0.0;

  if (value < abc[0])
  {
    membership[0] = 1.0;
    return;
  }
  else if (value < abc[1])
  {
    membership[1] = (value - abc[0]) / (abc[1] - abc[0]);
    membership[0] = 1.0 - membership[1];
    return;
  }
  else if(value < abc[2])
  {
    membership[2] = (value - abc[1]) / (abc[2] - abc[1]);
    membership[1] = 1.0 - membership[2];
    return;
  }
  else
  {
    membership[2] = 1.0; 
    return;
  }
}

f64 
EvaluateContinuousValue(const f64 definition[3], f64 membership[3])
{
  f64 numerator = 
               definition[0] * membership[0];
  numerator += definition[1] * membership[1];
  numerator += definition[2] * membership[2];
  log_debug("OUT: %lf %lf %lf\n", membership[0], membership[1], membership[2]);

  f64 denominator = membership[0] + membership[1] + membership[2];

  if (denominator < 1e-6)
  {
    return definition[1];
  } else {
    DEBUG_ASSERT(!isnan(numerator/denominator), "Should not be here");
    return numerator / denominator;
  }
}

subroutine 
print_bin(uint8_t byte)
{
    int i = CHAR_BIT * sizeof(uint8_t);
    while(i--) {
        putchar('0' + ((byte >> i) & 1)); /* loop through and print the bits */
    }
    putchar('\n');
}

subroutine 
EvaluateRule(uint32_t rule, const f64 * input_state, f64 * output_state)
{
  #define ISNONE(X) ((X) > 2)
  #define PROBESTATE(PROBE, QUALITY) input_state[(PROBE)*3+(QUALITY)]

  u32 permut_codes[3] = { (rule >> 24) & 0b11, (rule >> 26) & 0b11, (rule >> 28) & 0b11 };
  DEBUG_ASSERT(permut_codes[0] < 3, "Permutation id 0 is not comprised in [0,2]")
  DEBUG_ASSERT(permut_codes[1] < 3, "Permutation id 1 is not comprised in [0,2]")
  DEBUG_ASSERT(permut_codes[2] < 3, "Permutation id 2 is not comprised in [0,2]")
  log_debug("Permut codes: %d, %d, %d\n", permut_codes[0], permut_codes[1], permut_codes[2]);

  for(int i = 0; i < 3; i++) // OUTPUT: LOW, MEDIUM, HIGH
  {
    const int *map = set3_associations[permut_codes[i]];
    output_state[i] = NAN;

    uint32_t sentence = (rule >> (i * 8)) & 0xFFFFFFFF;
    uint32_t probe_state_id = (sentence >> 6) & 0b11;                 // q1
    log_debug("Rule output: %lf, %u\n", *output_state, probe_state_id);

    if (likely(!ISNONE(probe_state_id))) {
      output_state[i] = PROBESTATE(map[0], probe_state_id);
    }

    probe_state_id = (sentence >> 3) & 0b11;                           // q2
    log_debug("Rule output: %lf, %d\n", *output_state, probe_state_id);

    if (likely(!ISNONE(probe_state_id))) {
      output_state[i] = maxormin[(sentence & BIT(5)) >> 5](output_state[i], PROBESTATE(map[1], probe_state_id));
    }

    probe_state_id = (sentence & 0b11);                                // q3
    log_debug("Rule output: %lf, %d\n", *output_state, probe_state_id);
    if (likely(!ISNONE(probe_state_id))) {
      output_state[i] = maxormin[(sentence & BIT(2)) >> 2](output_state[i], PROBESTATE(map[2], probe_state_id));
    }
    if(isnan(output_state[i])) output_state[i] = 0.0;
    DEBUG_ASSERT(!isnan(output_state[i]), "Output state is NAN");
  }
  #undef ISNONE
  #undef PROBESTATE
}

subroutine 
WriteRuleStr(uint32_t rule, char * text)
{
  
  static const char * template = "%s : if %s is %s %s %s is %s %s %s is %s;\n";

  char *current = text;

  for(int out_qualifier = 0; out_qualifier < 3; out_qualifier++)
  {
    // LOW: imporvement is low and clustering is medium or time is high
    uint8_t sentence = (rule >> (out_qualifier * 8));
    uint8_t quality0 = (sentence >> 6) & 0b11;
    uint8_t quality1 = (sentence >> 3) & 0b11;
    uint8_t quality2 = (sentence >> 0) & 0b11; 
    current += sprintf(current, template, 
        states_names[3][out_qualifier],
        probes_names[0],
        states_names[0][quality0],
        operators_names[((sentence & BIT(5)) >> 5 )],
        probes_names[1],
        states_names[1][quality1],
        operators_names[((sentence & BIT(2)) >> 2 )],
        probes_names[2],
        states_names[2][quality2]
        );

    
  }
}
#endif
