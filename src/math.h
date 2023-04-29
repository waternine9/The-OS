#ifndef H_TOS_MATH
#define H_TOS_MATH
#include <stdint.h>
#include <stdbool.h>

float InvSqrt(float x);

float abs(float x);
float sin(float x);
float cos(float x);
uint8_t ilog2(uint32_t val);
uint32_t rand();

typedef struct
{
    float x, y, z;
} vector3;

typedef struct
{
    float x, y;
} vector2;

typedef struct
{
    float x, y, z, w;
} vector4;

typedef struct
{
    vector3 Position;
    vector3 Direction;
} ray;

typedef struct 
{
    vector3 Position;
    vector3 Color;
    float Radius;
    float Roughness; // MUST BE 0.0f to 1.0f
} sphere;

vector3 Normalize3(vector3 x);
vector2 Normalize2(vector2 x);
vector4 Normalize4(vector4 x);

float Dot3(vector3 x, vector3 y);
float Dot2(vector2 x, vector2 y);
float Dot4(vector4 x, vector4 y);

vector3 Cross(vector3 x, vector3 y);

vector3 Sub3(vector3 x, vector3 y);
vector2 Sub2(vector2 x, vector2 y);
vector4 Sub4(vector4 x, vector4 y);

vector3 Add3(vector3 x, vector3 y);
vector2 Add2(vector2 x, vector2 y);
vector4 Add4(vector4 x, vector4 y);

vector3 Div3(vector3 x, vector3 y);
vector2 Div2(vector2 x, vector2 y);
vector4 Div4(vector4 x, vector4 y);

vector3 Mul3(vector3 x, vector3 y);
vector2 Mul2(vector2 x, vector2 y);
vector4 Mul4(vector4 x, vector4 y);

vector3 Scale3(vector3 x, float y);
vector2 Scale2(vector2 x, float y);
vector4 Scale4(vector4 x, float y);

float Length3(vector3 x);
float Length2(vector2 x);
float Length4(vector4 x);

vector3 Reflect(vector3 x, vector3 normal);


bool RaySphereIntersect(ray Ray, sphere Sphere, float *Distance);

#endif // H_TOS_MATH