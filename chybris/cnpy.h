#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <stdarg.h>

#define LENOF(x) ((sizeof((x))) / sizeof(((x)[0])))

typedef struct {
  size_t order;
  size_t shape[10];
  double * data;
} shaped_array_t;


subroutine 
npy_save(f64 *data, const char *fname, u32 order, ...);

shaped_array_t 
npy_load(const char * path);

subroutine
npy_free(shaped_array_t arr);


#ifdef CNPY_IMPLEMENTATION

subroutine
npy_free(shaped_array_t arr) {
  free(arr.data);
}
typedef struct {
  size_t order;
  size_t shape[10];
  size_t size;
} npy_header_t;

npy_header_t NumpyParseHeader(char* header, int len)
{
    npy_header_t ret;
    char * pshapev = strstr(header, "shape");
    HARD_ASSERT(pshapev, "Error occured while collecting .npy shape string.");

    // Search for opening and closing parenthesis.
    char * ppareno = strchr(pshapev, '(');
    char * pparenc = strchr(ppareno, ')');

    HARD_ASSERT(ppareno + 1 != pparenc, "Not supporting scalars for now.\n");

    char* p = ppareno;
    // We start at the '('
    ret.order = 0;
    while(p <= pparenc && ret.order < LENOF(ret.shape))
    {
        if (isdigit((*(++p))))
        {
            size_t shape_i = strtol(p, &p, 10);
            ret.shape[ret.order++] = shape_i;
        }
        // We are at the first character after the number.
    }

    HARD_ASSERT(ret.order != 10, "Order schould be lower than 10");

    // We compute the size of the array
    ret.size = sizeof(double);
    for(size_t i = 0; i < ret.order; i++)
        ret.size*=ret.shape[i];
 
    return ret;
}

shaped_array_t npy_load(const char * path)
{
  // Open the binary file
  FILE* handle = NULL;
  handle = fopen(path,"rb");
  assert(handle);

  // Check the file size
  fseek(handle, 0L, SEEK_END);
  long size = ftell(handle);
  assert(size);
  
  // Allocate the buffer
  uint8_t * buffer = (uint8_t*) malloc(size * sizeof(uint8_t));
  assert(buffer);

  // Read the file to the buffer
  rewind(handle);
  size_t count = fread(buffer, sizeof(uint8_t), size, handle);
  fclose(handle);

  HARD_ASSERT(count == size, "File length check failed");
  HARD_ASSERT(buffer[0] == 0x93, "This is not a npy file.");

  uint8_t major_version = buffer[6];
  uint8_t minor_version = buffer[7];
  HARD_ASSERT(major_version==1, "We only support 1.0 npys");
  HARD_ASSERT(minor_version==0, "We only support 1.0 npys");

  uint16_t header_lenght = (((uint16_t)buffer[8] & 0x00FF)) | ((uint16_t) buffer[9] & 0x00FF) << 8;
  HARD_ASSERT(!((header_lenght + 10) % 64), "Numpy binary files schould be aligned, strange"); // Verify integrity of documented alignment.

  npy_header_t header_struct = NumpyParseHeader((char*)(buffer + 10), header_lenght);
 
  shaped_array_t array;
  array.data = (double*)malloc(header_struct.size);

  memcpy(array.data, buffer+ 10 + header_lenght, header_struct.size);


  for(int i = 0; i < 3; i++) array.shape[i] = header_struct.shape[i];
  array.order = header_struct.order;

  return array;
}


#define ARRSHAPE(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int)),__VA_ARGS__


static const char * npy_magic_str[]  = { "{'descr': '<f8', 'fortran_order': False, 'shape': (", "), }" };

subroutine
npy_save(f64 *data, const char *fname, u32 order, ...)
{
  #define HEADER_LEN 128
  u8 header[HEADER_LEN] = { 
    0x93,
    'N', 'U', 'M', 'P', 'Y', 
    1, 0, 
    [8 ... HEADER_LEN-2] = 0x20,
    [HEADER_LEN-1]       = 0x0A,
  };

  u32 nelements = 1;
  va_list args;
  va_start(args, order);
  char *cursor = (char*)header + 10;

  DEBUG_ASSERT(order > 0, "Incorrect shape order specified (at least 1).\n")

  memcpy(cursor, npy_magic_str[0], strlen(npy_magic_str[0]) * sizeof(u8));
  cursor += strlen(npy_magic_str[0]);
  for (size_t i = 0; i < order; i++) {
    u32 size = va_arg(args, int);
    nelements *= size;
    cursor += sprintf(cursor, "%d,", size);
  }

  memcpy(cursor, npy_magic_str[1], strlen(npy_magic_str[1]) * sizeof(u8));
  cursor += strlen(npy_magic_str[1]);
  va_end(args);


  header[8] = ((HEADER_LEN-10) & 0x00FF);
  header[9] = ((HEADER_LEN-10) & 0xFF00) >> 8;
  
  {
    FILE * of = NULL;
    of = fopen(fname, "wb");
    HARD_ASSERT(of, "Could not open [%s] for writing binary.\n", fname);
    fwrite(header, sizeof(u8), HEADER_LEN, of);
    fwrite(data, sizeof(u8), nelements  * sizeof(f64), of);
    fclose(of);
  }
  #undef HEADER_LEN
}
#endif

