#ifndef PTSHADER_H_INCLUDED
#define PTSHADER_H_INCLUDED

#include <x3d/common.h>
#include <math/math.h>


typedef struct {
        float v[2];
} vec2;

typedef struct {
        float v[3];
} vec3;

typedef struct {
        float v[4];
} vec4;

typedef struct {
        vec3    o;
        vec3    v;
        float   t[2];
} ray3;


static float vec3_dot ( vec3* v0, vec3* v1 )
{
        return v0->v[0]*v1->v[0] + v0->v[1]*v1->v[1] + v0->v[2]*v1->v[2];
}

static void vec3_cross ( vec3* v0, vec3* v1, vec3* e )
{
        e->v[0] =   v0->v[1]*v1->v[2] - v1->v[1]*v0->v[2];
        e->v[1] = -(v0->v[0]*v1->v[2] - v0->v[2]*v1->v[0]);
        e->v[2] =   v0->v[0]*v1->v[1] - v1->v[0]*v0->v[1];
}

static float inv_sqrtf ( float x )
{
        float xhalf = 0.5f*x;
        int32_t i = *(int32_t*)&x;
        i = 0x5f375a86 - (i >> 1);
        x = *(float*) &i;
        x = x*(1.5f - xhalf*x*x);
        x = x*(1.5f - xhalf*x*x);
        return x;
}

static double inv_sqrt ( double x )
{
        double xhalf = 0.5f*x;
        int64_t i = *(int64_t*)&x;
        i = 0x5fe6ec85e7de30da - (i >> 1);
        x = *(double*) &i;
        x = x*(1.5f - xhalf*x*x);
        x = x*(1.5f - xhalf*x*x);
        return x;
}

static void vec3_norm ( vec3* vi )
{
        float is = inv_sqrt ( vec3_dot ( vi, vi ) );
        int c;
        for ( c = 0; c < 3; c ++ )
                vi->v[c] *= is;
}

#define VAR             static
#define IVAR            static
#define OVAR            static
#define UNIFORM         static
#define I
#define O


#endif // PTSHADER_H_INCLUDED
