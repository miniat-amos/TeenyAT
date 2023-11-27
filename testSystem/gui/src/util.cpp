#include "util.h"
#include <cmath>

int map(int num, int in_min, int in_max, int out_min, int out_max)
{
   return (int)(num - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}