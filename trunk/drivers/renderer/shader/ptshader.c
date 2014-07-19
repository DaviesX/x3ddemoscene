/* ptshader.c: shaders for path tracing */
#include "ptshader.h"


/* uniforms */
typedef struct tag_emissive {
        int     type;
        float3  emi;
        float3  p0;
        float3  dir;
} emissive_t;

uniform emissive_t      g_emlist[1024];
uniform int             g_nem;

typedef struct tag_censor {
        float   w;
        float   h;
        float   focal_len;
} censor_t;

uniform censor_t        g_censor;

/* i/o variables */
invar float2*   sha_emit_uv;
outvar ray3d*   sha_emit_ray;

invar ray3d*    sha_emit_ray;
outvar ray3d*   incident_ray;
outvar ray3d*   reflect_ray;
outvar float3*  boun_refl;

outvar int*     sha_o_trans_r;


extern const char* code_whitted_emitter;
const char* code_whitted_emitter = {
        "invar float2    sha_emit_uv;"
        "outvar ray3d    sha_emit_ray;"
};

extern void a_whitted_emitter ( void )
{
        float x = g_censor.w*sha_emit_uv->x - 0.5f*g_censor.w;
        float y = g_censor.h*sha_emit_uv->y - 0.5f*g_censor.h;

        float3 p0 = {0, 0, 0};
        float3 p1 = {x, y, g_censor.focal_len};
        build_line3d ( &p0, &p1, sha_emit_ray );
}

extern const char* code_whitted_bounce;
const char* code_whitted_bounce = nullptr;

extern void a_whitted_bounce ( void )
{
}

extern const char* code_whitted_transfer;
const char* code_whitted_transfer = nullptr;

extern void a_whitted_transfer ( void )
{
}
