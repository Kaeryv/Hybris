#include "core.h"

#define typechecker(T) _Generic( (T), char: 1, int: 2, default: 0)

main() {
  int macro_var(test);
  printf("Hello world %s", "hi\n");

  int *test = NULL;
  defer(test = calloc(100, sizeof(int)), free(test)){
    test[50] = 4;
    printf("Hello world %d\n",test[50]);
    printf("typechecker: %d\n", typechecker(32));
  }

  int a = 0b00100000;
}
