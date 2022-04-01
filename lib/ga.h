#define BITMASK 0xffffff
typedef void (*ga_case_t)(const u32*, f64*, const u32);

struct registry {
  u32 *chromosomes;
  u32 *childrens;
  u32 num_childrens;
  u32 *argsort;
  f64 *fitness;
  u32 size;
  ga_case_t fun;
};

struct registry
ga_create(u32 popsize) {
  struct registry reg = {
    .size = popsize,
    .chromosomes = calloc(popsize, sizeof(u32)),
    .childrens   = calloc(popsize, sizeof(u32)),
    .argsort     = calloc(popsize, sizeof(u32)),
    .fitness     = calloc(popsize, sizeof(f64)),
    .num_childrens = 0
  };

  for_range(i, popsize) {
    reg.chromosomes[i] = (rand() & BITMASK); //| BIT(31);
    reg.argsort[i] = i;
    reg.fitness[i] = INFINITY;
  }

  return reg;
}
subroutine
ga_evaluate(struct registry reg) {
  reg.fun(reg.chromosomes, reg.fitness, reg.size);
}

u64
crossover_offspring(u32 parent1, u32 parent2) {
  u32 crossover_point = rand() % 24;
  u32 bitmask = 0xffffff >> crossover_point;
  u64 c1 = parent1  &  bitmask;
  u64 c2 = parent2  &  bitmask;
  u64 c1b = parent1 & ~bitmask;
  u64 c2b = parent2 & ~bitmask;
  //printf("%x %d\n", bitmask, crossover_point);
  //print_bin32(bitmask);
  //print_bin32(~bitmask);
  u64 offspring = ((c1 | c2b) << 32) | (c2 | c1b);
  return offspring;
}
subroutine
ga_selection_crossover(struct registry *reg) {
  // We sort the individuals by fitness
  for_range(k, 10) lazy_argsort(reg->fitness, reg->argsort, reg->size);
  // We select parents 25 times among the bests
  for_range(i, reg->size / 2) {
    // Two parents are elected.
    u32 p1 = reg->argsort[rand() % (reg->size / 4)];
    u32 p2 = reg->argsort[rand() % (reg->size / 4)];
    u64 childrens = crossover_offspring(reg->chromosomes[p1],
        reg->chromosomes[p2]);
    HARD_ASSERT(reg->num_childrens < reg->size-1, "Registry size outmatched");
    reg->childrens[reg->num_childrens] = childrens & 0xffffffff;
    reg->childrens[reg->num_childrens+1] = 
      (childrens >> 32) & 0xffffffff;
    //print_bin32(reg->childrens[reg->num_childrens]);
    //print_bin32(reg->childrens[reg->num_childrens+1]);
    reg->num_childrens += 2;
  }

}

subroutine
ga_mutate(struct registry *reg) {
  for_range_min(j, 5, reg->num_childrens) {
    // Flip a mutated bit
    u32 mutation = rand() % 24;
    reg->childrens[j] ^= BIT(mutation);
  }
}

subroutine
ga_update(struct registry *reg) {
  for_range(i, reg->size) {
    reg->chromosomes[i] = reg->childrens[i];
  }
  reg->num_childrens = 0;
}

