#pragma once

#include "core.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

f64
arr_f64_sum(f64 *arr, u32 len);

i32
arr_f64_searchsorted(f64 *arr, f64 val, u32 len);

i32 
arrcontains(int *noalias arr, int len, int value);

subroutine
iarrpush(int *array, int *len, int max_len, int value);

i32
set_put_i32(i32 * set, i32 * len, const u32 max_len, const i32 value);

i32 
argmin(const f64 *noalias array, const i32 len);

i32 
argmax(const f64 *noalias array, const int len);

i32 
subset_argmin(const f64 *noalias array, const i32 *noalias subset, const int subset_len);

#define arrfill(array, value, nelements) \
  for(int i_arrfill = 0; i_arrfill < nelements; i_arrfill++)\
  { array[i_arrfill] = value; }

subroutine 
iarange(int * array, const int len);

f64 
eucnorm(const f64 *noalias a, const f64 *noalias b, const int len);

subroutine 
lazy_argsort(const double *noalias arr, int * rank, const int len);

str* 
strvec2i(char** argv, i32 *integers, u32 len);

#ifdef ARRTOOLS_IMPLEMENTATION
f64
arr_f64_sum(f64 *arr, u32 len) {
  f64 sum = 0.0;
  for_range(i, len) {
    sum += arr[i];
  }
  return sum;
}

i32
arr_f64_searchsorted(f64 *arr, f64 val, u32 len) {
  // Array is sorted in ascending order
  DEBUG_ASSERT(len >= 2, 
      "Sorted array too short.");
  DEBUG_ASSERT(arr[len-2] <= arr[len-1], 
      "Array clearly not in ascending order.");
  
  for_range(i, len) {
    if (val < arr[i]) {
      return i;
    }
  }

  return len;
}
i32 
arrcontains(int *noalias arr, int len, int value)
{
    for(int i = 0; i < len; i++)
    {
        if (arr[i] == value)
            return 1;
    }
    return 0;
}

subroutine
iarrpush(int *array, int *len, int max_len, int value)
{
    HARD_ASSERT(*len < max_len, "Array length exceeds maximum reserved length.");
    array[(*len)++] = value;
}

i32
set_put_i32(i32 * set, i32 * len, const u32 max_len, const i32 value)
{
    // If the item is present in the set, we do nothing, else we push the item
    int included = arrcontains(set, *len, value);
    if(!included)
        iarrpush(set, len, max_len, value);
    return included;
}


i32 
argmin(const f64 *noalias array, const i32 len)
{
    DEBUG_ASSERT(len > 0, "Array is too short");
    int min_location = 0;
    for(int i = 1; i < len; i++)
    {
        if (array[min_location] > array[i])
        {
            min_location = i;
        }
    }
    return min_location;
}

i32 
argmax(const f64 *noalias array, const int len)
{
    int max_location = 0;
    for(int i = 1; i < len; i++)
    {
        if (array[max_location] < array[i])
        {
            max_location = i;
        }
    }
    return max_location;
}

i32 
subset_argmin(const f64 *noalias array, const i32 *noalias subset, const int subset_len)
{
    int min_location = 0;
    for (int i = 1; i < subset_len; i++)
    {
        if(array[subset[min_location]] > array[subset[i]])
        {
            min_location = i;
        }
    }
    return min_location;
}


subroutine 
iarange(int * array, const int len)
{
  for(int i = 0; i < len; i++)
    array[i] = i;
}



f64 
eucnorm(const f64 *noalias a, const f64 *noalias b, const int len)
{
    f64 acc = 0.0;
    for (i32 j = 0; j < len; j++)
    {
        const f64 dist = b[j] - a[j];
        acc += dist * dist;
    }
    return sqrt(acc);
}


subroutine 
lazy_argsort(const double *noalias arr, int * rank, const int len)
{
  for(int i = 0; i < len-1; i++)
  {
    int ra = rank[i];
    int rb = rank[i+1];
    double a = arr[ra];
    double b = arr[rb];

    if (b < a)
    {
      rank[i] = rb; rank[i+1] = ra;
    }
  }
}

str* 
strvec2i(char** argv, i32 *integers, u32 len)
{
  for (u32 i = 0; i < len; i++) {
    integers[i] = atoi(*(argv++));
  }
  return argv;
}

#endif