#ifndef INCLUDE_PSO_H
#define INCLUDE_PSO_H

#include "arrtools.h"
#include "random.h"

#define conf_max_num_cat_values 100
/**
 * Some seeds generated by RANDOM.ORG
 */

const global i32 conf_seeds[] = { 90808690 , 188454989, 141573185, 64511191 , 180084193, 574809672,
  498777999, 389776217, 361204898, 982276679, 973529711, 428403474, 358129852, 980030902,
  355656872, 815996624, 608707374, 941596564, 768732565, 344825190, 981324803, 917392714,
  44403656 , 296138135, 552282150, 207011042, 941150958, 427854330, 302066645, 107678969,
  34007723 , 714754511, 150632509, 364682177, 40537802 , 504118701, 589830149, 306089559,
  701157266, 474087180, 628286401, 582070536, 144907275, 379938584, 783744329, 995405136,
  649750208, 347936886, 504580502, 959228330, 17243816 , 152118667, 717321480, 498352993,
  833228172, 682444871, 34840287 , 103327964, 731291187, 502554158, 562789368, 800472269,
  713384393, 63701703 , 840277026, 458126970, 563238044, 233254107, 494613598, 707263250,
  500574214, 274809054, 694299144, 422547765, 799607464, 623558200, 651298295, 462941455,
  536109305, 519343233, 866735930, 776708595, 610058471, 107808006, 944547032, 753805766,
  69170362 , 695820288, 303116428, 998892099, 560266972, 141837589, 147860980, 996760536,
  65703151 , 540171401, 25313745 , 523001716, 697686920, 267419808
};


/**
 * @brief Probes values.
 * Just add your probe here and start using it!
 * Don't forget to add its boundaries below.
 */
enum {
  PIMPROVEMENT = 0,
  PCLUSTERING1,
  PTIME,
  NUM_PROBES
};


/**
 * @brief Parameters values.
 */
enum {
  WINERTIA = 0, WCONFIDENCE, WSOCIAL, WHYBRIDATION,
  WLOWER_SPEED_LIMIT, WUPPER_SPEED_LIMIT,
  WFRIENDS_CAPACITY, NUM_WEIGHTS
};

typedef subroutine (*iteration_cb_t)(ptr);

/**
 * @brief The registry struct that contains the swarm's state.
 * 
 */
typedef struct  {
    // Design points in the search space
    f64  *noalias   position;
    f64  *noalias   mem_position;
    f64  *noalias   prev_position;

    // PSO buffers
    f64  *noalias   speed;
    f64  *noalias   prev_speed;

    // Design points evaluation
    f64  *noalias   aptitude;
    f64  *noalias   mem_aptitude;
    f64  *noalias   prev_aptitude;

    // Topology related buffers
    i32  *noalias   friends;
    i32  *noalias   best_friend;
    i32  *noalias   friends_count;

    i32  *noalias   rank;
    i32  *noalias   argsort;
    i32  *noalias   prev_rank;
    
    i32  *noalias   cat_var_nvals;
    f64  *noalias   cat_var_valprob;

    f64  *noalias   weights;
    f64  *noalias   fuzzy_props;
    f64  *noalias   fuzzy_states;

    f64  *noalias   profile;

    f64             initial_weights[NUM_WEIGHTS];

    f64             controllers_membership[NUM_WEIGHTS][3]; 

    u32             controllers[NUM_WEIGHTS];
    u32             num_agents;
    u32             num_dimensions;
    u32             num_discrete_dimensions;

    i32             num_stagnating_iterations_without_reevaluation;
    i32             num_stagnating_iterations;

    i32             cur_iteration;
    i32             max_iterations;

    f64  *noalias   lower_bound;
    f64  *noalias   upper_bound;

    pcg32_random_t *prng;
} registry_t;

registry_t*
registry_create (i32 num_agents, i32 num_dimensions, i32 num_discrete_dimensions, i32 max_iterations);

subroutine
registry_init(registry_t * reg, u32 seed);

subroutine
registry_free (registry_t * reg);

subroutine
reg_update_population(registry_t* reg);

subroutine
reg_process_fitness(registry_t* reg);

subroutine
reg_set_num_categories(registry_t *reg, const i32 variable_id, const i32 num);

inline f64*
reg_get_categorical_probability(registry_t *reg, i32 i) {
    return &(reg->cat_var_valprob[i * conf_max_num_cat_values]);
}

internal u32
reg_get_num_categories(registry_t *reg, const i32 variable_id);

subroutine
reg_minimize_problem(registry_t* reg, TestCase test_case, u32 seed);


#ifdef PSO_IMPLEMENTATION


/**
 * @brief Definitions of the Fuzzy probes' membership values
 * 
 */
global f64 conf_probes_membership[NUM_PROBES][3] = {
  [PIMPROVEMENT] = { -1.0,  0.0,  1.0},
//  [PCLUSTERING1] = {  -4.,  -2, -0.5},
  [PCLUSTERING1] = {  -5.,  -2, -1.5},
  [PTIME] =        {  0.3,  0.5,  0.8}
};
/**
 * @brief Parameters boundaries.
 * 
 */

/**
 * @brief Parameters default (static) values.
 * 
 */
global f64 conf_initial_weights[] = {
  [WINERTIA]           = 0.7298,
  [WCONFIDENCE]        = 1.49618,
  [WSOCIAL]            = 1.49618,
  [WHYBRIDATION]       = 0.0,
  [WLOWER_SPEED_LIMIT] = -16,
  [WUPPER_SPEED_LIMIT] = 0.6,
  [WFRIENDS_CAPACITY]  = 3.0,
};
/**
 * Getters and Setters for python API.
 */
subroutine
set_default_weight(i32 wid, f64 value) {
  conf_initial_weights[wid] = value;
}

subroutine
set_range_weight(registry_t *reg, i32 wid, f64 vlow, f64 vmed, f64 vhig) {
  reg->controllers_membership[wid][0] = vlow;
  reg->controllers_membership[wid][1] = vmed;
  reg->controllers_membership[wid][2] = vhig;
}

subroutine
set_range_probe(i32 pid, f64 value[3]) {
  conf_probes_membership[pid][0] = value[0];
  conf_probes_membership[pid][1] = value[1];
  conf_probes_membership[pid][2] = value[2];
}

// Useful macros
// uniform rand in [0, 1[
#define drandco() (((f64) (pcg32_random(reg->prng) & 0xffffffff)) / ((f64) 0xffffffff))
// uniform rand in ]0, 1[
#define drandoo() (((f64) ((1+pcg32_random(reg->prng)) & 0xffffffff)) / ((f64) 0xffffffff))
#define random_sample(distr, len) pcg32_random_sample(reg->prng, distr, len)

#define urand()   (pcg32_random(reg->prng))
#define FSTATE_SIZE (NUM_PROBES*3)


internal inline f64
reg_get_fuzzy_probe(registry_t *reg, i32 agent, i32 probe) {
  return reg->fuzzy_props[agent*NUM_PROBES + probe];
}

internal inline subroutine
RegSetFuzzyProbe(registry_t *reg, i32 agent, i32 probe, f64 value) {
  reg->fuzzy_props[agent*NUM_PROBES + probe] = value;
}

internal inline f64 *
reg_position(registry_t * reg, const i32 agent) {
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  return reg->position + agent * reg->num_dimensions;
}

internal inline f64 *
reg_get_memory(registry_t * reg, const i32 agent) {
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  return reg->mem_position + agent * reg->num_dimensions;
}

internal inline f64
GetWeight(registry_t* reg, const i32 agent, const i32 weight) {
  DEBUG_ASSERT(weight < NUM_WEIGHTS, "Weight id is too big.");
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  return reg->weights[(weight * reg->num_agents) + agent];
}


internal inline f64*
GetWeightArray(registry_t* reg, const i32 weight) {
  DEBUG_ASSERT(weight < NUM_WEIGHTS, "Weight id is too big.");
  return reg->weights + (weight * reg->num_agents);
}

internal inline subroutine
SetWeight(registry_t* reg, const i32 agent, const i32 weight, f64 value) {
  DEBUG_ASSERT(weight < NUM_WEIGHTS, "Weight id is too big.");
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  reg->weights[(weight * reg->num_agents) + agent] = value;
}

internal inline f64*
reg_speed(registry_t * reg, const i32 agent) {
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  return reg->speed + agent * reg->num_dimensions;
}

internal inline f64*
GetBestFriendMemory(registry_t* reg, const i32 agent) {
  DEBUG_ASSERT(agent < reg->num_agents, "Agent id is too big.");
  return reg->mem_position + (reg->best_friend[agent] * reg->num_dimensions);
}


internal inline u32
reg_get_num_categories(registry_t *reg, const i32 variable_id) {
    return reg->cat_var_nvals[variable_id];
}

subroutine
reg_refresh_rand_topology(registry_t * reg, const i32 num_agents, i32 initial_run)
{
  for (i32 i = 0; i < num_agents; i++)
  {
    reg->friends_count[i] = 0;

    while (reg->friends_count[i] < GetWeight(reg, i, WFRIENDS_CAPACITY))
    {
        i32 picked_friend = urand() % reg->num_agents;
        set_put_i32(&reg->friends[i*num_agents], &reg->friends_count[i], num_agents, picked_friend);
    }
  }
}

registry_t*
registry_create (i32 num_agents, i32 num_dimensions, i32 num_discrete_dimensions, i32 max_iterations) {
  registry_t *reg = Alloc(registry_t);
  *reg = (registry_t) {
    controllers:    { [0 ... NUM_WEIGHTS-1] = NO_CONTROL},
    num_agents:     num_agents,
    num_dimensions: num_dimensions,
    num_discrete_dimensions: num_discrete_dimensions,
    max_iterations: max_iterations,
    controllers_membership: {
      [WINERTIA]           = {  0.3,  0.7,        0.9  },
      [WCONFIDENCE]        = {  0.7,  1.49618,    2.0  },
      [WSOCIAL]            = {  0.7,  1.49618,    2.0  },
      [WHYBRIDATION]       = {  0.0,  0.35,       0.7  },
      [WLOWER_SPEED_LIMIT] = { -5.,  -3.,        -2.5  },
      [WUPPER_SPEED_LIMIT] = {  0.2,  0.6,        1.0  },
      [WFRIENDS_CAPACITY]  = {  3.0,  5.0,        9.0  },
    }
  };
  const u32 num_continuous_dimensions = num_dimensions - reg->num_discrete_dimensions;

  reg->position       = calloc(num_agents * num_dimensions,   sizeof(f64));
  reg->prev_position  = calloc(num_agents * num_dimensions,   sizeof(f64));
  reg->mem_position   = calloc(num_agents * num_dimensions,   sizeof(f64));

  reg->speed          = calloc(num_agents * num_dimensions,   sizeof(f64));
  reg->prev_speed     = calloc(num_agents * num_dimensions,   sizeof(f64));

  reg->aptitude       = calloc(num_agents,                    sizeof(f64));
  reg->mem_aptitude   = calloc(num_agents,                    sizeof(f64));
  reg->prev_aptitude  = calloc(num_agents,                    sizeof(f64));

  reg->friends        = calloc(num_agents * num_agents,       sizeof(i32));
  reg->best_friend    = calloc(num_agents,                    sizeof(i32));
  reg->friends_count  = calloc(num_agents,                    sizeof(i32));

  reg->rank           = calloc(num_agents,                    sizeof(i32));
  reg->argsort        = calloc(num_agents,                    sizeof(i32));
  reg->prev_rank      = calloc(num_agents,                    sizeof(i32));
  reg->cat_var_nvals = calloc(num_discrete_dimensions, sizeof(i32));
  reg->cat_var_valprob = calloc(num_discrete_dimensions * conf_max_num_cat_values, sizeof(f64));

  reg->weights        = calloc(num_agents * NUM_WEIGHTS,      sizeof(f64));
  reg->fuzzy_props    = calloc(num_agents * NUM_PROBES,       sizeof(f64));
  reg->fuzzy_states   = calloc(num_agents * FSTATE_SIZE,      sizeof(f64));

  reg->profile        = calloc(max_iterations,                sizeof(f64));
  
  // Set-up th PRNG
  reg->prng           = Alloc(pcg32_random_t);
  reg->prng->state    = 0x853c49e6748fea9bULL;
  reg->prng->inc      = 0xda3e39cb94b95bdbULL;

  reg->upper_bound    = calloc(num_continuous_dimensions,      sizeof(f64));
  reg->lower_bound    = calloc(num_continuous_dimensions,      sizeof(f64));

  // Default the upper bound to 1
  for_range(j, num_continuous_dimensions) {
    reg->upper_bound[j] = 1.0;
  }
  // Fill the starting weights vectors with their default values
  for_range(wid, NUM_WEIGHTS) {
    reg->initial_weights[wid] = conf_initial_weights[wid];
  }

  pcg32_random_seed(reg->prng, 43);
  return reg;
}

subroutine
registry_free (registry_t * reg) {
    zfree(reg->position          );
    zfree(reg->prev_position     );
    zfree(reg->mem_position      );
    zfree(reg->speed             );
    zfree(reg->prev_speed        );
    zfree(reg->aptitude          );
    zfree(reg->mem_aptitude      );
    zfree(reg->prev_aptitude     );
    zfree(reg->friends           );
    zfree(reg->best_friend       );
    zfree(reg->friends_count     );
    zfree(reg->weights           );
    zfree(reg->fuzzy_props       );
    zfree(reg->rank              );
    zfree(reg->argsort           );
    zfree(reg->prev_rank         );
    zfree(reg->fuzzy_states      );
    zfree(reg->profile           );
    zfree(reg->prng              );
    zfree(reg->cat_var_nvals     );
    zfree(reg->cat_var_valprob   );
    zfree(reg->lower_bound       );
    zfree(reg->upper_bound       );

    // !! Keep at the end 
    free(reg);
}

subroutine
registry_init(registry_t * reg, u32 seed) {
  const u32 num_agents = reg->num_agents;
  const u32 num_dimensions = reg->num_dimensions;
  const u32 num_continuous_dimensions = num_dimensions - reg->num_discrete_dimensions;

  // Reset the prng state
  HARD_ASSERT(reg->prng, "PRNG state not initialized");
  reg->prng->state  = 0x853c49e6748fea9bULL;
  reg->prng->inc    = 0xda3e39cb94b95bdbULL;
  pcg32_random_seed(reg->prng, seed);


  arrfill(reg->mem_aptitude,      INFINITY,                 num_agents);
  arrfill(reg->aptitude,          INFINITY,                 num_agents);
  arrfill(reg->fuzzy_states,      0.0,                      num_agents * FSTATE_SIZE);

  // Fill the weights vectors with their defaults values
  for_range(wid, NUM_WEIGHTS) {
    arrfill(GetWeightArray(reg, wid), reg->initial_weights[wid], num_agents);
  }

  reg->num_stagnating_iterations = 0;
  reg->num_stagnating_iterations_without_reevaluation = 0;

  // Init positions
  for_range (i, num_agents) {
    for_range(j, num_continuous_dimensions) {
      f64 extent = reg->upper_bound[j] - reg->lower_bound[j];
      f64 offset = reg->lower_bound[j];
      reg_position(reg, i)[j] = drandoo();
      reg_position(reg, i)[j] *= extent;
      reg_position(reg, i)[j] += offset;
    }
    for_range_min(j, num_continuous_dimensions, num_dimensions) {
      reg_position(reg, i)[j] = urand() % reg_get_num_categories(reg, j-num_continuous_dimensions);
    }
  }


  // Init speeds
  for_range(i, num_agents) {
      for_range(j, num_continuous_dimensions) {
        f64 min_i = reg->lower_bound[j] - reg_position(reg, i)[j];
        f64 max_i = reg->upper_bound[j] - reg_position(reg, i)[j];
        f64 extent_i = max_i - min_i;
        reg_speed(reg, i)[j] = drandoo();
        reg_speed(reg, i)[j] *= extent_i;
        reg_speed(reg, i)[j] += min_i;
      }
  }

  // Init Max number of friends
  for (i32 i = 0; i < num_agents; i++)
      reg->friends_count[i] = 0;

  iarange(reg->rank, num_agents);
  iarange(reg->argsort, num_agents);
  iarange(reg->prev_rank, num_agents);

  memcpy(reg->mem_position, reg->position, num_agents * num_dimensions * sizeof(f64));
  reg->cur_iteration = 0;

  reg_refresh_rand_topology(reg, num_agents, 1);
}


subroutine
reg_update_fuzzy_states(registry_t * reg, const i32 num_agents) {
  for(i32 i = 0; i < num_agents; i++) {
    for_range(probe, NUM_PROBES) {
      EvaluateMembership(conf_probes_membership[probe], reg_get_fuzzy_probe(reg, i, probe), &reg->fuzzy_states[i * FSTATE_SIZE + probe * 3]);
    }
  }
}

subroutine
reg_fuzzy_control(registry_t * reg, const i32 num_agents) {
  // We apply the fuzzy control for all active rules.
  for_range(wid, NUM_WEIGHTS) {
    u32 rule = reg->controllers[wid];
    if (!(rule & NO_CONTROL)) {
      for_range(i, num_agents) {
        f64 state[3] = {0};
        EvaluateRule(rule, reg->fuzzy_states + (i * FSTATE_SIZE), state);
        SetWeight(reg, i, wid, EvaluateContinuousValue(reg->controllers_membership[wid], state));
        log_debug("%lf : %lf : %lf : rule: %u\n", state[0], state[1], state[2], rule);
      }
    }
    else {
      log_debug("Not controlling weight [%d].\n", wid);
    }
  }

}

subroutine
reg_speed_quantum(registry_t *noalias reg, const f64 localization, const i32 num_agents, const i32 num_dimensions, i32 i)
{
    f64 r1 = drandoo();
    f64 r2 = drandoo();
    for(i32 j = 0; j < num_dimensions; j++)
    {
      f64 attractor = r1 * GetBestFriendMemory(reg, i)[j]
        + r2 * reg_get_memory(reg, i)[j];
      attractor /= r1 + r2;
      f64 u = drandoo();
      f64 L = (1.0/localization)  * AbsoluteValue(reg_position(reg, i)[j] - attractor);

      if (urand() % 2)
        reg_speed(reg, i)[j] = - reg_position(reg, i)[j]
          + attractor - L * log(1.0/u);
      else
        reg_speed(reg, i)[j] = - reg_position(reg, i)[j]
          + attractor + L * log(1.0/u);
    }
}

subroutine
reg_speed_classical(registry_t* reg, const i32 num_agents, const i32 num_dimensions, i32 i) {
  for(i32 j = 0; j < num_dimensions; j++)
  {
    f64 r1 = drandoo();
    f64 r2 = drandoo();
    reg_speed(reg, i)[j] *= GetWeight(reg, i, WINERTIA);
    reg_speed(reg, i)[j] += GetWeight(reg, i, WCONFIDENCE) * r1 * (reg_get_memory(reg, i)[j] - reg_position(reg, i)[j]);
    reg_speed(reg, i)[j] += GetWeight(reg, i, WSOCIAL) * r2 * (GetBestFriendMemory(reg, i)[j] - reg_position(reg, i)[j]);
  }
}


subroutine
reg_speed_crossover(registry_t* reg, const i32 num_agents, const i32 num_dimensions, i32 i) {
  f64 r1 = drandoo();
  u32 cross_position = rand() % num_dimensions;
  double director[num_dimensions];
  for(i32 j = 0; j < num_dimensions; j++) {
    if(j < cross_position) {
      director[j] = reg_get_memory(reg, i)[j];
    } else {
      director[j] = GetBestFriendMemory(reg, i)[j];
    }
  }

  for(i32 j = 0; j < num_dimensions; j++)
  {
    reg_speed(reg, i)[j] *= GetWeight(reg, i, WINERTIA);
    reg_speed(reg, i)[j] += .5 * (GetWeight(reg, i, WCONFIDENCE) + GetWeight(reg, i, WSOCIAL)) * r1 * (director[j] - reg_position(reg, i)[j]);
  }
}


extern f64*
reg_get_categorical_probability(registry_t *reg, i32 i);


subroutine
reg_set_num_categories(registry_t *reg, const i32 variable_id, const i32 num) {
  HARD_ASSERT(num < conf_max_num_cat_values, "Too much categorical values, please recompile.");
  HARD_ASSERT(variable_id < reg->num_discrete_dimensions, "Discrete dimension index is too big!");
  reg->cat_var_nvals[variable_id] = num;
}

/** 
 * Implementation from
 * https://doi.org/10.1016/j.swevo.2020.100808
 */
subroutine
reg_update_discrete(registry_t *reg, const i32 num_agents, const i32 num_dimensions, const i32 num_discrete_dimensions, f64 alpha, f64 min_prob) {
    // Dimensions 0 to num_continuous_dimensions -1 are continuous
    // Dimensions n num_continuous_dimensions to num_dimensions -1 are discrete
    const i32 start_discrete_dimensions = num_dimensions - num_discrete_dimensions;
    for_range_min(j, start_discrete_dimensions, num_dimensions) {
        u32 variable_id = j - start_discrete_dimensions;
        for_range(k, reg_get_num_categories(reg, variable_id)) {
            f32 count = 0;
            for_range(i, num_agents /2) { // Why not num_agents, /2 in publi
                i32 agent = reg->argsort[i];
                if (GetBestFriendMemory(reg, agent)[j] == (float) k) {
                    count +=  1; // (num_agents-i) or f (i) to account for ranking even more ? 1 is publi
                }
            }
            f64 current_alpha = pcg32_random_boxmuller(reg->prng, alpha, 0.1);
            reg_get_categorical_probability(reg, variable_id)[k] *= current_alpha; // alpha = 0.5 works great
            reg_get_categorical_probability(reg, variable_id)[k] += (1-current_alpha)  * 2 * count / num_agents;
            reg_get_categorical_probability(reg, variable_id)[k] = fmax(reg_get_categorical_probability(reg, variable_id)[k], min_prob);
            // However, this minimal probability is ESSENTIAL when the number of agents is low (diversity enabler)
            // If not present, depending on the see, some values will never appear!
        }
    }
    for_range(i, num_agents) {
      for_range_min(j, start_discrete_dimensions, num_dimensions) {
        u32 variable_id = j - start_discrete_dimensions;
        reg_position(reg, i)[j] = (float) random_sample(reg_get_categorical_probability(reg, variable_id), reg_get_num_categories(reg, variable_id));
      }
    }
}

subroutine
reg_update_motion(registry_t *noalias reg, const i32 num_agents, const i32 num_dimensions, const i32 num_discrete_dimensions) {
  const i32 num_continuous_dimensions = num_dimensions - num_discrete_dimensions;

  for (i32 i = 0; i < num_agents; i++)
  {
      if(drandoo() > GetWeight(reg, i, WHYBRIDATION))
      	reg_speed_classical(reg, num_agents, num_continuous_dimensions, i);
      else
        reg_speed_quantum(reg, 0.96, num_agents, num_continuous_dimensions, i);
  }


  reg_update_discrete(reg, num_agents, num_dimensions, num_discrete_dimensions, 0.5, 0.05);
  // Limit the speed
  for_range(i, num_agents) {
    for_range(j, num_continuous_dimensions) {
      f64 upper = GetWeight(reg, i, WUPPER_SPEED_LIMIT)  * (reg->upper_bound[j] - reg->lower_bound[j]);
      f64 lower = pow(10.0, GetWeight(reg, i, WLOWER_SPEED_LIMIT))  * (reg->upper_bound[j] - reg->lower_bound[j]);

      if (AbsoluteValue(reg_speed(reg, i)[j]) > upper)
        reg_speed(reg, i)[j] = (reg_speed(reg, i)[j] > 0.0) ? upper : - upper;
      if (AbsoluteValue(reg_speed(reg, i)[j]) < lower)
        reg_speed(reg, i)[j] = (reg_speed(reg, i)[j] > 0.0) ? lower : -lower;
    }
  }

  // Store previous positions.
  memcpy(reg->prev_position, reg->position, num_agents * num_dimensions * sizeof(f64));

  //  Increment positions with speed.
  for_range(i, num_agents) {
      for_range(j, num_continuous_dimensions) { 
            reg_position(reg, i)[j] += reg_speed(reg, i)[j];
      }
  }

   // Check collision with boundaries, the speed is inverted with a random scaling constant if wall is hit.
  for_range(i, num_agents) {
      f64 * pos = reg_position(reg, i);
      f64 * spd = reg_speed(reg, i);
      for_range(j, num_continuous_dimensions)
      {
        if (pos[j] > reg->upper_bound[j]) {
          pos[j] = reg->upper_bound[j];
          spd[j] *= - drandoo();
        }
        else if (pos[j] < reg->lower_bound[j]) {
          pos[j] = reg->lower_bound[j];
          spd[j] *= - drandoo();
        }
    }
  }
}

subroutine
reg_assign_best_friends(i32 *noalias best_friend, const i32 *noalias friends, const i32 *noalias num_friends, const f64 *noalias mem_aptitude, const i32 num_agents) {
  for(i32 i = 0; i < num_agents; i++)
  {
    best_friend[i] = friends[i * num_agents + subset_argmin(mem_aptitude, &friends[i * num_agents], num_friends[i])];
  }
}


subroutine
reg_update_probes(registry_t * reg, f64 space_diagonal)
{
  i32 iteration = reg->cur_iteration;
  for_range(i, reg->num_agents)
  {
    f64 ttime = (iteration + 1.0) / (float) reg->max_iterations;
    RegSetFuzzyProbe(reg, i, PTIME, ttime);

    f64 * bf_position = reg_get_memory(reg, reg->best_friend[i]);

    f64 dist_to_bf = eucnorm(
        reg_position(reg, i),
        bf_position, reg->num_dimensions);


    f64 clustering = log10(dist_to_bf / space_diagonal);

    i32 iimprovement = - (reg->rank[i] - reg->prev_rank[i]);

    iimprovement -= ((reg->num_stagnating_iterations < 5) ? reg->num_stagnating_iterations : 5);

    if (reg->num_stagnating_iterations == 0 )
      iimprovement += 1;
    if (reg->num_stagnating_iterations == 0 && reg->rank[i] == 0)
      iimprovement += 1;

    f64 improvement = iimprovement / 5.0;

    if(improvement >  1.0) improvement = 1.0;
    if(improvement < -1.0) improvement = -1.0;

    f64 imprtmp = reg_get_fuzzy_probe(reg, i, PIMPROVEMENT);
    imprtmp *= 0.5;
    imprtmp += 0.5 * improvement;
    RegSetFuzzyProbe(reg, i, PIMPROVEMENT, imprtmp);
    RegSetFuzzyProbe(reg, i, PCLUSTERING1, clustering);

  }
}

/**
  * The optimizer is asked to provide promosing points to evaluate.
  */
subroutine
reg_update_population(registry_t* reg) {
  if (reg->cur_iteration)
    reg_update_motion(reg, reg->num_agents, reg->num_dimensions, reg->num_discrete_dimensions);
}

subroutine
reg_process_fitness(registry_t* reg) {
  // The aptitude has been updated by the evaluation, acting
  const u32 num_dimensions          = reg->num_dimensions;
  const u32 num_agents              = reg->num_agents;
  const u32 num_continuous_dimensions = num_dimensions - reg->num_discrete_dimensions;

  f64 space_diagonal = 0.0;
  for_range(j, num_continuous_dimensions) {
    space_diagonal += (reg->upper_bound[j] - reg->lower_bound[j]) * (reg->upper_bound[j] - reg->lower_bound[j]);
  }
  space_diagonal = sqrt(space_diagonal);
  // Update argsort
  for_range(j, 5) lazy_argsort(reg->aptitude, reg->argsort, num_agents);
  // Update ranks
  for_range(j, num_agents) reg->rank[reg->argsort[j]] = j;

  // Update best memories
  for_range(i, num_agents) {
    if (reg->mem_aptitude[i] > reg->aptitude[i]) {
      // The particle improved
      reg->mem_aptitude[i] = reg->aptitude[i];
      memcpy(reg->mem_position + i * num_dimensions, reg->position + i * num_dimensions, num_dimensions * sizeof(f64));
    }
  }
  reg_assign_best_friends(reg->best_friend, reg->friends, reg->friends_count, reg->mem_aptitude, num_agents);

  
  // Get the current best particle id
  // Manage stagnation
  i32 best_mem = argmin(reg->mem_aptitude, num_agents);
  if(reg->profile[(i32)fmax((f32) reg->cur_iteration - 1, 0.)] > reg->mem_aptitude[best_mem]) {
      reg->num_stagnating_iterations = 0;
      reg->num_stagnating_iterations_without_reevaluation = 0;
  }
  else {
      reg->num_stagnating_iterations++;
      reg->num_stagnating_iterations_without_reevaluation++;
  }
  // We update the best
  reg->profile[reg->cur_iteration] = reg->mem_aptitude[best_mem];

  
  if(reg->num_stagnating_iterations_without_reevaluation > 2)
  {
    reg->num_stagnating_iterations_without_reevaluation = 0;
    reg_refresh_rand_topology(reg, num_agents, 0 /* initial_run */);
  }

  // Apply the fuzzy adaptation policy
  reg_update_probes(reg, space_diagonal);

  #ifndef HYBRIS_DISABLE_FUZZY_CONTROL
    reg_update_fuzzy_states(reg, num_agents);
    reg_fuzzy_control(reg, num_agents);
  #else
    #warning "Fuzzy control is DISABLED"
  #endif

  // Save variables for next iteration
  memcpy(reg->prev_aptitude, reg->aptitude, num_agents * sizeof(f64));
  memcpy(reg->prev_rank,     reg->rank,     num_agents * sizeof(i32));

  reg->cur_iteration++;
  log_info("Processed fitness information with best: %lf\n", reg->profile[reg->cur_iteration-1]);
}

subroutine
reg_minimize_problem(registry_t* reg, TestCase test_case, u32 seed)
{
  //const int num_continuous_variables = reg->num_dimensions - reg->nu
  //for_range(j, )
  //reg->lower_bound[j] = test_case.lower;
  //reg->upper_bound[j] = test_case.upper;
  registry_init(reg, seed);
  #ifdef ADD_CECBENCHMARK
  struct cecbench_state cstate;
  cecbench_state_init(&cstate);
  fun_state state = { .prng = reg->prng, .cecglobals=&cstate};
  #else
  fun_state state = { .prng = reg->prng};
  #endif
  while(reg->cur_iteration < reg->max_iterations) {
    reg_update_population(reg);
    test_case.function(reg->position, reg->num_agents, reg->num_dimensions, reg->aptitude, state);
    reg_process_fitness(reg);
  }
}


#undef  drandco
#undef  drandoo
#undef  urand
#undef FSTATE_SIZE

#endif
#endif
