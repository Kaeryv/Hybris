#pragma once
#include "core.h"
#include "arrtools.h"

void map2img(const double* map, const size_t len, uint8_t* img)
{
  double min = map[argmin(map, len)];
  double max = map[argmax(map, len)];

  double scale = 255.0 / (max - min);
  double offset = min;

  for (size_t i = 0; i < len; i++)
    img[i] = (uint8_t)((map[i] - offset) * scale);
}
