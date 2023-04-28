#include <stdint.h>
#include <stdbool.h>

float abs(float x)
{
    return x < 0.0f ? -x : x;
}

float sin(float x)
{
    x = x * 0.318310f;
    float x_frac = x - abs((int)x);
    float x_sqr = x_frac * x_frac;
    float x_1 = x_frac - 1.0f;
    float result = 3.2f * (x_frac - x_sqr + x_sqr * x_1 * x_1);
    return ((int)x % 2) ? -result : result;
}
float cos(float x)
{
    return sin(x + 1.57079f);
}
uint32_t seed = 10;
uint32_t rand()
{
    uint32_t x = seed << 4;
    seed -= x * x + (seed >> 16);
    return seed;
}