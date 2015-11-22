/* ptshader.c: shaders for path tracing */
#include "ptshader.h"


/* system i/o variables */
////IVAR float                    g_xy[2];                I
//IVAR float*                     g_uv[2];                I
//OVAR ray3*                      g_recur[2];             O
//OVAR int*                       g_n_recur;              O
//OVAR ray3*                      g_illum[1024];          O
//OVAR int*                       g_n_illum;              O
//IVAR bool*                      g_is_vis[1024];         I
//IVAR ray3*                      g_incident;             I
////IVAR ray3                       g_emit;                 I
//OVAR vec3*                      g_radiance;             O
//
///* custom i/o variables */
//IVAR float*                   g_position[3];          I
//IVAR float*                   g_normal[3];            I
//IVAR int*                     g_mater_id;             I
//
//VAR int*                      g_nbounce;

IVAR float*                     g_uv;                //I[]
OVAR ray3*                      g_recur;             //O[]
OVAR int*                       g_n_recur;           //O
OVAR ray3*                      g_illum;             //O[]
OVAR int*                       g_n_illum;           //O
IVAR bool*                      g_is_vis;            //I[]
IVAR ray3*                      g_incident;          //I
OVAR vec3*                      g_radiance;          //O

/* custom i/o variables */
IVAR float*                     g_position;               I
IVAR float*                     g_normal;                 I
IVAR int*                       g_mater_id;               I

struct material {
        int                     brdf;
        float                   gloss;
        vec3                    diff;
        vec3                    spec;
//        struct color_spec       tran;
};

struct point_light {
        float   p[3];
        float   power;
};

struct rect_light {
        float   p[4][3];
        float   in[3];
        float   area;
        float   power;
};

#define RED     0
#define GREEN   1
#define BLUE    2
#define WHITE   3
#define MIRROR  4

#define DIFFUSE            0
#define SPECULAR           1
#define GLOSSY             2
#define TRANSPARENT        3

/* uniforms */
UNIFORM struct point_light       uni_point_light[1] = {
        [0].p = {275.0f, 548.8f, 249.5f},
        [0].power = 80000.0f
};

UNIFORM struct rect_light        uni_rect_light[1] = {
        [0].p[0] = {343.0f, 548.8f, 227.0f},
        [0].p[1] = {343.0f, 548.8f, 332.0f},
        [0].p[2] = {213.0f, 548.8f, 332.0f},
        [0].p[3] = {213.0f, 548.8f, 227.0f},
        [0].in = {0.0f, 1.0f, 0.0f},
        [0].area = 105.0f*130.0f,
        [0].power = 6.0f
};
UNIFORM int                      uni_n_point_light = 0;
UNIFORM int                      uni_n_area_light = 1;
UNIFORM float                    uni_width = 0.025f;
UNIFORM float                    uni_height = 0.025f;
UNIFORM float                    uni_focal = 0.035f*2.0f;
UNIFORM struct material          uni_material[] = {
        /* red */
        [0].diff = {1.0f, 0.0f, 0.0f},
        [0].spec = {0.0f, 0.0f, 0.0f},
        [0].brdf = DIFFUSE,
        /* green */
        [1].diff = {0.0f, 1.0f, 0.0f},
        [1].spec = {0.0f, 0.0f, 0.0f},
        [1].brdf = DIFFUSE,
        /* blue */
        [2].diff = {0.0f, 0.0f, 1.0f},
        [2].spec = {0.0f, 0.0f, 0.0f},
        [2].brdf = DIFFUSE,
        /* white */
        [3].diff = {1.0f, 0.94f, 0.701176471f},
        [3].spec = {0.0f, 0.0f, 0.0f},
        [3].brdf = DIFFUSE,
        /* perfect reflection */
        [4].diff = {1.0f, 1.0f, 1.0f},
        [4].spec = {1.0f, 1.0f, 1.0f},
        [4].brdf = DIFFUSE,
        [4].gloss = 0.4f
};
UNIFORM unsigned short           uni_xi[3] = {1, 100, 21021};
UNIFORM int                     g_nbounce;


static void a_easyrt_probe ( void )
{
        /* reset recursive bounce to 0 */
        g_nbounce = 0;
        /* construct an initial probe ray to recursive ray set */
        g_recur[0].o.v[0] = 278.0f;
        g_recur[0].o.v[1] = 273.0f;
        g_recur[0].o.v[2] = -800.0f;

        g_recur[0].t[0] = 1.0f;
        g_recur[0].t[1] = 50000.0f;
        g_recur[0].v.v[0] = g_uv[0]*uni_width;
        g_recur[0].v.v[1] = g_uv[1]*uni_height;
        g_recur[0].v.v[2] = uni_focal;
        /* put it to recursive ray set */
        *g_n_recur = 1;
}

static void make_illuminate_sample ( void )
{
        int i = 0;
        int j;
        *g_n_illum = 0;
        int c;
        for ( j = 0; j < uni_n_point_light; j ++, i ++ ) {
                for ( c = 0; c < 3; c ++ )
                        g_illum[i].o.v[c] = g_position[c];
                g_illum[i].t[0] = 1e-5f;
                g_illum[i].t[1] = 1.0f - 1e-5f;
                for ( c = 0; c < 3; c ++ )
                        g_illum[i].v.v[c] = uni_point_light[j].p[c] - g_position[c];
        }
        (*g_n_illum) += uni_n_point_light;
        for ( j = 0; j < uni_n_area_light; j ++, i ++ ) {
                for ( c = 0; c < 3; c ++ ) {
                        g_illum[i].o.v[c] = g_position[c];
                }
                g_illum[i].t[0] = 1e-5f;
                g_illum[i].t[1] = 1.0f - 1e-5f;
                float s[4] = {
                        erand48 ( uni_xi ),
                        erand48 ( uni_xi ),
                        erand48 ( uni_xi ),
                        erand48 ( uni_xi )
                };
                float inv = 1.0f/(s[0] + s[1] + s[2] + s[3]);
                for ( c = 0; c < 4; c ++ )
                        s[c] *= inv;
                float samp_p[3];
                for ( c = 0; c < 3; c ++ ) {
                        samp_p[c] = uni_rect_light[j].p[0][c]*s[0] +
                                    uni_rect_light[j].p[1][c]*s[1] +
                                    uni_rect_light[j].p[2][c]*s[2] +
                                    uni_rect_light[j].p[3][c]*s[3];
                }
                for ( c = 0; c < 3; c ++ )
                        g_illum[i].v.v[c] = samp_p[c] - g_position[c];
        }
        *g_n_illum = *g_n_illum + uni_n_area_light;
}

static void mirror_sample_shader ( void )
{
        /* reject when recursive bounce reaches 3 */
        if (g_nbounce ++ >= 3) {
                *g_n_recur = 0;
                goto illuminate_ray;
        }

        int c;
//recursive_ray:
        ray3 t_incident;    // avoid modifying g_incident
        for ( c = 0; c < 3; c ++ )
                t_incident.v.v[c] = -(*g_incident).v.v[c];
        for ( c = 0; c < 3; c ++ )
                g_recur[0].o.v[c] = g_position[c];

        /* generate a mirror ray */
        float iln = 1.0f/sqrtf ( g_normal[0]*g_normal[0] +
                                 g_normal[1]*g_normal[1] +
                                 g_normal[2]*g_normal[2] );
        float ili = 1.0f/sqrtf ( t_incident.v.v[0]*t_incident.v.v[0] +
                                 t_incident.v.v[1]*t_incident.v.v[1] +
                                 t_incident.v.v[2]*t_incident.v.v[2] );
        float t_normal[3];
        for ( c = 0; c < 3; c ++ ) {
                t_normal[c] = g_normal[c]*iln;
                t_incident.v.v[c] *= ili;
        }
        float n_dot_i = 2.0f*(t_normal[0]*t_incident.v.v[0] +
                              t_normal[1]*t_incident.v.v[1] +
                              t_normal[2]*t_incident.v.v[2]);
        for ( c = 0; c < 3; c ++ ) {
                g_recur[0].v.v[c] = t_normal[c]*n_dot_i - t_incident.v.v[c];
        }
        float ilr = 1.0f/sqrtf ( g_recur[0].v.v[0]*g_recur[0].v.v[0] +
                                 g_recur[0].v.v[1]*g_recur[0].v.v[1] +
                                 g_recur[0].v.v[2]*g_recur[0].v.v[2] );
        for ( c = 0; c < 3; c ++ )
                g_recur[0].v.v[c] *= ilr;

        g_recur[0].t[0] = 0.0f + 1.e-2f;
        g_recur[0].t[1] = 50000.0f;
        *g_n_recur = 1;

illuminate_ray:;
        make_illuminate_sample ( );
}

static void lambert_sample_shader ( void )
{
        /* reject when recursive bounce reaches 3 */
        if (g_nbounce ++ >= 3) {
                *g_n_recur = 0;
                goto illuminate_ray;
        }

        int c;
//recursive_ray:
        ray3 t_incident;    // avoid modifying g_incident
        for ( c = 0; c < 3; c ++ )
                t_incident.v.v[c] = -(*g_incident).v.v[c];
        for ( c = 0; c < 3; c ++ )
                g_recur[0].o.v[c] = g_position[c];
#if 1
        /* generate random ray over the hemisphere */
        float rx = erand48 ( uni_xi );
        float ry = erand48 ( uni_xi );
        float p = ry*2*M_PI;
        float t = acosf ( sqrtf ( rx ) );
        float sin_the = sinf ( t );
        float cos_the = cosf ( t );
        float sin_phi = sinf ( p );
        float cos_phi = cosf ( p );
        float x = sin_the*cos_phi;
        float y = sin_the*sin_phi;
        float z = cos_the;
        /* construct basis */
/*
        float iln = 1.0f/sqrtf ( g_normal[0]*g_normal[0] +
                                 g_normal[1]*g_normal[1] +
                                 g_normal[2]*g_normal[2] );
        float ili = 1.0f/sqrtf ( g_incident.v[0]*g_incident.v[0] +
                                 g_incident.v[1]*g_incident.v[1] +
                                 g_incident.v[2]*g_incident.v[2] );

        for ( c = 0; c < 3; c ++ ) {
                g_normal[c] *= iln;
                g_incident.v[c] *= ili;
        }
*/
        float u[3], v[3], t_normal[3];
        vec3_cross ( (vec3*) g_normal, &t_incident.v, (vec3*) u );
        vec3_cross ( (vec3*) u, (vec3*) g_normal, (vec3*) v );
        vec3_norm2 ( (vec3*) g_normal, (vec3*) t_normal);
        vec3_norm ( &t_incident.v );
        vec3_norm ( (vec3*) u );
        vec3_norm ( (vec3*) v );
        /* transform ray */
        for ( c = 0; c < 3; c ++ )
                g_recur[0].v.v[c] = x*u[c] + y*v[c] + z*t_normal[c];
#endif
/*
        g_recur[0].v[0] = 2.0f*erand48 ( uni_xi ) - 1.0f;
        g_recur[0].v[1] = 2.0f*erand48 ( uni_xi ) - 1.0f;
        g_recur[0].v[2] = 2.0f*erand48 ( uni_xi ) - 1.0f;
        if ( dot_vector ( g_recur[0].v, g_normal ) < 0.0f ) {
                for ( c = 0; c < 3; c ++ )
                        g_recur[0].v[c] = -g_recur[0].v[c];
        }
        float ilr = sqrtf ( g_recur[0].v[0]*g_recur[0].v[0] +
                                 g_recur[0].v[1]*g_recur[0].v[1] +
                                 g_recur[0].v[2]*g_recur[0].v[2] );
        for ( c = 0; c < 3; c ++ )
                g_recur[0].v[c] /= ilr;
*/
        g_recur[0].t[0] = 0.0f + 1.e-2f;
        g_recur[0].t[1] = 50000.0f;
        *g_n_recur = 1;

illuminate_ray:;
        make_illuminate_sample ( );
}

static void a_easyrt_sample ( void )
{
        switch ( uni_material[*g_mater_id].brdf ) {
        case DIFFUSE:
                lambert_sample_shader ();
                break;
        case SPECULAR:
                mirror_sample_shader ();
                break;
        case GLOSSY:
                if ( erand48 ( uni_xi ) < uni_material[*g_mater_id].gloss )
                        mirror_sample_shader ();
                else
                        lambert_sample_shader ();
                break;
        }
}

static void a_easyrt_illuminate ( void )
{
        int i;
        for ( i = 0; i < 3; i ++ )
                (*g_radiance).v[i] = 0.0f;
        /* emit point lights */
        i = 0;
        int j;
        for ( j = 0; j < uni_n_point_light; j ++, i ++ ) {
                if (g_is_vis[j] == true ) {
                        float d[3];
                        int c;
                        for ( c = 0; c < 3; c ++ )
                                d[c] = g_illum[i].v.v[c];// uni_point_light[j].p[c] - g_position[c];
                        float s_dist = d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
                        float inv_dist = 1.0f/sqrtf ( s_dist );
                        for ( c = 0; c < 3; c ++ )
                                d[c] *= inv_dist;
/*                        float s_norm = 1.0f/sqrtf ( g_normal[0]*g_normal[0] +
                                                    g_normal[1]*g_normal[1] +
                                                    g_normal[2]*g_normal[2] );*/
                        float cos_flux = /*s_norm*/(g_normal[0]*d[0] +
                                                 g_normal[1]*d[1] +
                                                 g_normal[2]*d[2]);
                        cos_flux = max ( 0.0f, cos_flux );
                        for ( c = 0; c < 3; c ++ ) {
                                (*g_radiance).v[c] += cos_flux*
                                        uni_material[*g_mater_id].diff.v[c]*
                                        uni_point_light[j].power/s_dist;
                        }
                }
        }
        /* emit area lights */
        for ( j = 0; j < uni_n_area_light; j ++, i ++ ) {
                if (g_is_vis[j] == true ) {
                        float d[3];
                        int c;
                        for ( c = 0; c < 3; c ++ )
                                d[c] = g_illum[i].v.v[c];
                        float s_dist = d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
                        float inv_dist = inv_sqrt ( s_dist );
                        for ( c = 0; c < 3; c ++ )
                                d[c] *= inv_dist;
/*                        float s_norm = 1.0f/sqrtf ( g_normal[0]*g_normal[0] +
                                                    g_normal[1]*g_normal[1] +
                                                    g_normal[2]*g_normal[2] );*/
                        float cos_flux = /*s_norm*/(g_normal[0]*d[0] +
                                                 g_normal[1]*d[1] +
                                                 g_normal[2]*d[2]);
                        cos_flux = max ( 0.0f, cos_flux );

                        float cos_area = vec3_dot ( (vec3*) d, (vec3*) uni_rect_light[j].in );
                        cos_area = max ( 0.0f, cos_area );
                        float solid_angle = uni_rect_light[j].area*cos_area/s_dist;
                        for ( c = 0; c < 3; c ++ ) {
                                (*g_radiance).v[c] += cos_flux*
                                        uni_material[*g_mater_id].diff.v[c]*
                                        uni_rect_light[j].power*solid_angle;
                        }
                }
        }
}

static void a_easyrt_transfer ( void )
{
        int i;
        float cos_flux = ((*g_incident).v.v[0])*g_normal[0] +
                         ((*g_incident).v.v[1])*g_normal[1] +
                         ((*g_incident).v.v[2])*g_normal[2];
        cos_flux = max ( 0.0f, cos_flux );
        for ( i = 0; i < 3; i ++ )
                (*g_radiance).v[i] = uni_material[*g_mater_id].diff.v[i]*(*g_radiance).v[i]*cos_flux;
}
