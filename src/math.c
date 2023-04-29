#include <stdint.h>
#include <stdbool.h>
#include "math.h"

float InvSqrt(float x)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );
	return y;
}

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

uint8_t ilog2(uint32_t val)
{
    uint8_t result = 0;
    while (val >>= 1) result++;
    return result;
}

uint32_t seed = 10;
uint32_t rand()
{
    uint32_t x = seed << 4;
    seed -= x * x + (seed >> 16);
    return seed;
}

vector3 Normalize3(vector3 x)
{
    float mag = InvSqrt(x.x * x.x + x.y * x.y + x.z * x.z);
    x.x *= mag;
    x.y *= mag;
    x.z *= mag;
    return x;
}
vector2 Normalize2(vector2 x)
{
    float mag = InvSqrt(x.x * x.x + x.y * x.y);
    x.x *= mag;
    x.y *= mag;
    return x;
}
vector4 Normalize4(vector4 x)
{
    float mag = InvSqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
    x.x *= mag;
    x.y *= mag;
    x.z *= mag;
    x.w *= mag;
    return x;
}

float Dot3(vector3 x, vector3 y)
{
    return x.x * y.x + x.y * y.y + x.z * y.z;
}

float Dot2(vector2 x, vector2 y)
{
    return x.x * y.x + x.y * y.y;
}

float Dot4(vector4 x, vector4 y)
{
    return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
}

vector3 Cross(vector3 x, vector3 y)
{
    return (vector3){
        x.y * y.z - x.z * y.y,
        x.z * y.x - x.x * y.z,
        x.x * y.y - x.y * y.x
    };
}

vector3 Sub3(vector3 x, vector3 y)
{
    return (vector3){ x.x - y.x, x.y - y.y, x.z - y.z };
}
vector2 Sub2(vector2 x, vector2 y)
{
    return (vector2){ x.x - y.x, x.y - y.y };
}
vector4 Sub4(vector4 x, vector4 y)
{
    return (vector4){ x.x - y.x, x.y - y.y, x.z - y.z, x.w - y.w };
}

vector3 Add3(vector3 x, vector3 y)
{
    return (vector3){ x.x + y.x, x.y + y.y, x.z + y.z };
}
vector2 Add2(vector2 x, vector2 y)
{
    return (vector2){ x.x + y.x, x.y + y.y };
}
vector4 Add4(vector4 x, vector4 y)
{
    return (vector4){ x.x + y.x, x.y + y.y, x.z + y.z, x.w + y.w };
}


vector3 Div3(vector3 x, vector3 y)
{
    return (vector3){ x.x / y.x, x.y / y.y, x.z / y.z };
}
vector2 Div2(vector2 x, vector2 y)
{
    return (vector2){ x.x / y.x, x.y / y.y };
}
vector4 Div4(vector4 x, vector4 y)
{
    return (vector4){ x.x / y.x, x.y / y.y, x.z / y.z, x.w / y.w };
}

vector3 Mul3(vector3 x, vector3 y)
{
    return (vector3){ x.x * y.x, x.y * y.y, x.z * y.z };
}
vector2 Mul2(vector2 x, vector2 y)
{
    return (vector2){ x.x * y.x, x.y * y.y };
}
vector4 Mul4(vector4 x, vector4 y)
{
    return (vector4){ x.x * y.x, x.y * y.y, x.z * y.z, x.w * y.w };
}

vector3 Scale3(vector3 x, float y)
{
    return (vector3){ x.x * y, x.y * y, x.z * y };
}
vector2 Scale2(vector2 x, float y)
{
    return (vector2){ x.x * y, x.y * y };
}
vector4 Scale4(vector4 x, float y)
{
    return (vector4){ x.x * y, x.y * y, x.z * y, x.w * y };
}

float Length3(vector3 x)
{
    return 1.0f / InvSqrt(x.x * x.x + x.y * x.y + x.z * x.z);
}

float Length2(vector2 x)
{
    return 1.0f / InvSqrt(x.x * x.x + x.y * x.y);
}

float Length4(vector4 x)
{
    return 1.0f / InvSqrt(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

vector3 Reflect(vector3 x, vector3 normal)
{
    return Sub3(x, Scale3(normal, 2 * Dot3(normal, x)));
}

bool RaySphereIntersect(ray Ray, sphere Sphere, float *Distance)
{
    float T0, T1; // solutions for t if the ray intersects
    vector3 L = Sub3(Sphere.Position, Ray.Position);
    float Tca = Dot3(L, Ray.Direction);
    float D2 = Dot3(L, L) - Tca * Tca;
    if (D2 > Sphere.Radius * Sphere.Radius) return false;
    float Thc = 1.0f / InvSqrt(Sphere.Radius * Sphere.Radius - D2);
    T0 = Tca - Thc;
    T1 = Tca + Thc;
    
    if (T0 > T1) 
    {
        float Temp = T0;
        T0 = T1;
        T1 = Temp; 
    }

    if (T0 < 0.0f) 
    {
        T0 = T1; // if t0 is negative, let's use t1 instead
        if (T0 < 0.0f) return false; // both t0 and t1 are negative
    }

    *Distance = T0;

    return true;
}