#ifndef H_TOS_MATH
#define H_TOS_MATH
#include <stdint.h>
#include <stdbool.h>

float abs(float x);
float sin(float x);
float cos(float x);
uint8_t ilog2(uint32_t val);
uint32_t rand();

#endif // H_TOS_MATH