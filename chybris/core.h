#ifndef CORE_MODULE
#define CORE_MODULE

#include <stdint.h>
typedef double   f64;
typedef float    f32;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t  i64;
typedef int32_t  i32;
typedef int16_t  i16;
typedef int8_t   i8;

typedef char*    str;
typedef void*    ptr;

#define noalias 
#define internal static
#define global static
#define persistent static
#define subroutine void

#define zfree(X) { free(X); X = NULL; }

#ifndef M_PI
  #define M_E        2.71828182845904523536
  #define M_LOG2E    1.44269504088896340736
  #define M_LOG10E   0.434294481903251827651
  #define M_LN2      0.693147180559945309417
  #define M_LN10     2.30258509299404568402
  #define M_PI       3.14159265358979323846
  #define M_PI_2     1.57079632679489661923
  #define M_PI_4     0.785398163397448309616
  #define M_1_PI     0.318309886183790671538
  #define M_2_PI     0.636619772367581343076
  #define M_2_SQRTPI 1.12837916709551257390
  #define M_SQRT2    1.41421356237309504880
  #define M_SQRT1_2  0.707106781186547524401
#endif

#define BIT(X) (1 << (X))

#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)

#define modctl(NAME,ACTION,...) NAME##_##ACTION(__VA_ARGS__);
#define Alloc(X) malloc(sizeof(X))


#define for_range(var, max) for(i32 var = 0; var < (max); var++)
#define for_range_min(var, min, max) for(i32 var = (min); var < (max); var++)

#define str(s) #s
#define xstr(s) str(s)

#define AbsoluteValue(X) _Generic((X), float: (((X) < 0.0f) ? (-(X)): (X)), double: (((X) < 0.0) ? (-(X)): (X)), default:(((X) < 0) ? (-(X)): (X)))
#define POW2(X) ((X)*(X))
#define POW3(X) ((X)*(X)*(X))
//void nx_mkdir(const char * path)
//{
//  struct stat st = {0};
//  if (stat(path, &st) == -1) 
//    mkdir(path, 0700);
//}


#if !defined NDEBUG || defined LOGLEVEL_ALL
  #define log_error(...)  fprintf(stderr, __VA_ARGS__)
  #define log_debug(...)  fprintf(stderr, __VA_ARGS__)
  #define log_info(...)   fprintf(stdout,  __VA_ARGS__)
#elif defined LOGLEVEL_INFO
  #define log_error(...)  fprintf(stderr, __VA_ARGS__)
  #define log_debug(...)  
  #define log_info(...)   fprintf(stdout,  __VA_ARGS__)
#else
  #define log_error(...)  fprintf(stderr, __VA_ARGS__)
  #define log_debug(...) 
  #define log_info(...) 
#endif

#define HARD_ASSERT(X, ...) { if (!(X)) { fprintf(stderr, "%s:%d(%s): error: ", __FILE__, __LINE__, __FUNCTION__); fprintf(stderr, __VA_ARGS__); fprintf(stderr," | aborting.\n"); exit(EXIT_FAILURE); }}

#if !defined NDEBUG || defined ENABLE_DEBUG_ASSERTS
#define DEBUG_ASSERT(X, ...) { if (!(X)) { fprintf(stderr, "%s:%d(%s): error: ", __FILE__, __LINE__, __FUNCTION__); fprintf(stderr, __VA_ARGS__); fprintf(stderr," | aborting.\n"); exit(EXIT_FAILURE); }}
#else
#define DEBUG_ASSERT(X, ...)
#endif

#define CONCAT2(A, B) A ## B
#define CONCAT(A,B) CONCAT2(A,B)
#define macro_var(name) CONCAT(name, __LINE__)
#define defer(start, end) for(int macro_var(_i_)=(start,0); !macro_var(_i_);(macro_var(_i_) += 1),end)
#define lenof(X) (sizeof(X)/sizeof((X)[0]))
typedef struct {
  double x;
  double y;
} v2f64;

// subroutine 
// print_bin32(uint32_t byte)
// {
//   int i = 8 * sizeof(uint32_t);
//   while(i--) {
//     putchar('0' + ((byte >> i) & 1)); /* loop through and print the bits */
//     if (!(i%8)) putchar(' ');
//   }
//   putchar('\n');
// }

#endif

