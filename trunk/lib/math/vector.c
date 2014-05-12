// vector.c: All vector functions go here
#include <math.h>
#include <logout.h>
#include "vector.h"



void add_vector2d ( struct vector2d *v0, struct vector2d *v1, struct vector2d *v_out )
{
        v_out->x = v0->x + v1->x;
        v_out->y = v0->y + v1->y;
}

void add_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *v_out )
{
        v_out->x = v0->x + v1->x;
        v_out->y = v0->y + v1->y;
        v_out->z = v0->z + v1->z;
}

void add_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *v_out )
{
        v_out->x = v0->x + v1->x;
        v_out->y = v0->y + v1->y;
        v_out->z = v0->z + v1->z;
}

void sub_vector2d ( struct vector2d *v0, struct vector2d *v1, struct vector2d *v_out )
{
        v_out->x = v0->x - v1->x;
        v_out->y = v0->y - v1->y;
}

void sub_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *v_out )
{
        v_out->x = v0->x - v1->x;
        v_out->y = v0->y - v1->y;
        v_out->z = v0->z - v1->z;
}

void sub_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *v_out )
{
        v_out->x = v0->x - v1->x;
        v_out->y = v0->y - v1->y;
        v_out->z = v0->z - v1->z;
}

/* Compute the reciprocal of each compoent */
void inv_vector2d ( struct vector2d *v, struct vector2d *v_out )
{
        v_out->x = 1.0f/v->x;
        v_out->y = 1.0f/v->y;
}

void inv_vector3d ( struct vector3d *v, struct vector3d *v_out )
{
        v_out->x = 1.0f/v->x;
        v_out->y = 1.0f/v->y;
        v_out->z = 1.0f/v->z;
}

void inv_vector4d ( struct vector4d *v, struct vector4d *v_out )
{
        v_out->x = 1.0f/v->x;
        v_out->y = 1.0f/v->y;
        v_out->z = 1.0f/v->z;
}

void scale_vector2d ( float k, struct vector2d *v, struct vector2d *v_out )
{
        v_out->x = v->x*k;
        v_out->y = v->y*k;
}

void scale_vector3d ( float k, struct vector3d *v, struct vector3d *v_out )
{
        v_out->x = v->x*k;
        v_out->y = v->y*k;
        v_out->z = v->z*k;
}

void scale_vector4d ( float k, struct vector4d *v, struct vector4d *v_out )
{
        v_out->x = v->x*k;
        v_out->y = v->y*k;
        v_out->z = v->z*k;
}

/* single scale, or MAD */
void sscale_add_vector2d ( struct vector2d *v0, float k, struct vector2d *v1, struct vector2d *v_out )
{
        v_out->x = k*v1->x + v0->x;
        v_out->y = k*v1->y + v0->y;
}

void sscale_add_vector3d ( struct vector3d *v0, float k, struct vector3d *v1, struct vector3d *v_out )
{
        v_out->x = k*v1->x + v0->x;
        v_out->y = k*v1->y + v0->y;
        v_out->z = k*v1->z + v0->z;
}

void sscale_add_vector4d ( struct vector4d *v0, float k, struct vector4d *v1, struct vector4d *v_out )
{
        v_out->x = k*v1->x + v0->x;
        v_out->y = k*v1->y + v0->y;
        v_out->z = k*v1->z + v0->z;
}

/* double scale */
void dscale_add_vector2d ( float k0, struct vector2d *v0, float k1, struct vector2d *v1, struct vector2d *v_out )
{
        v_out->x = k0*v0->x + k1*v1->x;
        v_out->y = k0*v0->y + k1*v1->y;
}

void dscale_add_vector3d ( float k0, struct vector3d *v0, float k1, struct vector3d *v1, struct vector3d *v_out )
{
        v_out->x = k0*v0->x + k1*v1->x;
        v_out->y = k0*v0->y + k1*v1->y;
        v_out->z = k0*v0->z + k1*v1->z;
}

void dscale_add_vector4d ( float k0, struct vector4d *v0, float k1, struct vector4d *v1, struct vector4d *v_out )
{
        v_out->x = k0*v0->x + k1*v1->x;
        v_out->y = k0*v0->y + k1*v1->y;
        v_out->z = k0*v0->z + k1*v1->z;
}

/* dot product of two vector v0.v1 = |v0||v1|cos a */
float dot_vector2d ( struct vector2d *v0, struct vector2d *v1 )
{
        return (v0->x*v1->x + v0->y*v1->y);
}

float dot_vector3d ( struct vector3d *v0, struct vector3d *v1 )
{
        return (v0->x*v1->x + v0->y*v1->y + v0->z*v1->z);
}

float dot_vector4d ( struct vector4d *v0, struct vector4d *v1 )
{
        return (v0->x*v1->x + v0->y*v1->y + v0->z*v1->z);
}

/*
 * cross product of two vector v_orthogonal = v0xv1
 * |i  j  k |
 * |x0 y0 z0| = i(y0*z1 - y1*z0) + j(z0*x1 - z1*x0) + k(x0*y1 - x1*y0)
 * |x1 y1 z1|
 */

void cross_vector3d ( struct vector3d *v0, struct vector3d *v1, struct vector3d *vec_orth )
{
        vec_orth->x = v0->y*v1->z - v1->y*v0->z;
        vec_orth->y = v0->z*v1->x - v1->z*v0->x;
        vec_orth->z = v0->x*v1->y - v1->x*v0->y;
}

void cross_vector4d ( struct vector4d *v0, struct vector4d *v1, struct vector4d *vec_orth )
{
        vec_orth->x = v0->y*v1->z - v1->y*v0->z;
        vec_orth->y = v0->z*v1->x - v1->z*v0->x;
        vec_orth->z = v0->x*v1->y - v1->x*v0->y;
}

/*
 * Only count for area in 2d case |v0xv1| = |v0||v1|sin a
 * |i  j |
 * |x0 y0| = i(y0*x1 - y1*x0) + j(x0*y1 - x1*y0)
 * |x1 y1|
 */
float cross_vector2d ( struct vector2d *v0, struct vector2d *v1 )
{
        struct vector2d tmp;
        tmp.x = v0->y*v1->x - v1->y*v0->x;
        tmp.y = v0->x*v1->y - v1->x*v0->y;
        return length_vector2d ( &tmp );
}

float length_vector2d ( struct vector2d *v )
{
        return sqrtf ( v->x*v->x + v->y*v->y );
}

float length_vector3d ( struct vector3d *v )
{
        return sqrtf ( v->x*v->x + v->y*v->y + v->z*v->z );
}

float length_vector4d ( struct vector4d *v )
{
        return sqrtf ( v->x*v->x + v->y*v->y + v->z*v->z );
}

void normalize_vector2d ( struct vector2d *v, struct vector2d *vec_out )
{
        float inv_len = 1.0f/length_vector2d ( v );
        vec_out->x = v->x*inv_len;
        vec_out->y = v->y*inv_len;
}

void normalize_vector3d ( struct vector3d *v, struct vector3d *vec_out )
{
        float inv_len = 1.0f/length_vector3d ( v );
        vec_out->x = v->x*inv_len;
        vec_out->y = v->y*inv_len;
        vec_out->z = v->z*inv_len;
}

void normalize_vector4d ( struct vector4d *v, struct vector4d *vec_out )
{
        float inv_len = 1.0f/length_vector4d ( v );
        vec_out->x = v->x*inv_len;
        vec_out->y = v->y*inv_len;
        vec_out->z = v->z*inv_len;
}

void init_vector2d ( struct vector2d *v )
{
        v->x = 0.0f;
        v->y = 0.0f;
}

void init_vector3d ( struct vector3d *v )
{
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
}

void init_vector4d ( struct vector4d *v )
{
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
        v->w = 0.0f;
}

void init_point4d ( struct point4d *p )
{
        p->x = 0.0f;
        p->y = 0.0f;
        p->z = 0.0f;
        p->w = 1.0f;
}

void set_vector2d ( float x, float y, struct vector2d *v )
{
        v->x = x;
        v->y = y;
}

void set_vector3d ( float x, float y, float z, struct vector3d *v )
{
        v->x = x;
        v->y = y;
        v->z = z;
}

void set_vector4d ( float x, float y, float z, float w, struct vector4d *v )
{
        v->x = x;
        v->y = y;
        v->z = z;
        v->w = w;
}

void set_unit_vector2d ( float x, float y, struct vector2d *v )
{
        v->x = x;
        v->y = y;
        normalize_vector2d_u ( v );
}

void set_unit_vector3d ( float x, float y, float z, struct vector3d *v )
{
        v->x = x;
        v->y = y;
        v->z = z;
        normalize_vector3d_u ( v );
}

void set_unit_vector4d ( float x, float y, float z, float w, struct vector4d *v )
{
        v->x = x;
        v->y = y;
        v->z = z;
        v->w = w;
        normalize_vector4d_u ( v );
}

void div_w_point4d ( struct point4d *p )
{
        float inv_w = 1.0f/p->w;
        p->x *= inv_w;
        p->y *= inv_w;
        p->z *= inv_w;
        p->w = 1.0f;
}

/* preserve w value */
void div_w_point4d_p ( struct point4d *p )
{
        float inv_w = 1.0f/p->w;
        p->x *= inv_w;
        p->y *= inv_w;
        p->z *= inv_w;
}

/* convert a 4d homogenuous coordinate to 3d cartesian coordinate */
void point4d_to_point3d ( struct point4d *p_homo, struct point3d *p_carte )
{
        float inv_w = 1.0f/p_homo->w;
        p_carte->x = p_homo->x*inv_w;
        p_carte->y = p_homo->y*inv_w;
        p_carte->z = p_homo->z*inv_w;
}

void copy_vector2d ( struct vector2d *v_src, struct vector2d *v_dest )
{
        v_dest->x = v_src->x;
        v_dest->y = v_src->y;
}


void copy_vector3d ( struct vector3d *v_src, struct vector3d *v_dest )
{
        v_dest->x = v_src->x;
        v_dest->y = v_src->y;
        v_dest->z = v_src->z;
}

void copy_vector4d ( struct vector4d * v_src, struct vector4d * v_dest )
{
        v_dest->x = v_src->x;
        v_dest->y = v_src->y;
        v_dest->z = v_src->z;
        v_dest->w = v_src->w;
}

void build_vector2d ( struct point2d *p_init, struct point2d *p_end, struct vector2d *v_out )
{
        v_out->x = p_end->x - p_init->x;
        v_out->y = p_end->y - p_init->y;
}

void build_vector3d ( struct point3d *p_init, struct point3d *p_end, struct vector3d *v_out )
{
        v_out->x = p_end->x - p_init->x;
        v_out->y = p_end->y - p_init->y;
        v_out->z = p_end->z - p_init->z;
}

void build_vector4d ( struct point4d *p_init, struct point4d *p_end, struct vector4d *v_out )
{
        v_out->x = p_end->x - p_init->x;
        v_out->y = p_end->y - p_init->y;
        v_out->z = p_end->z - p_init->z;
        v_out->w = 0.0f;	/* vector don't mind its position */
}

/* cosine angle between 2 vectors */
float cos_vector2d ( struct vector2d *v0, struct vector2d *v1 )
{
        /* v0.v1 = |v0||v1|cos a, cos a = (v0.v1)/(|v0||v1|) */
        float v0_dot_v1 = dot_vector2d ( v0, v1 );
        float v0_len_sq = v0->x*v0->x + v0->y*v0->y;
        float v1_len_sq = v1->x*v1->x + v1->y*v1->y;
        return v0_dot_v1/sqrtf ( v0_len_sq*v1_len_sq );
}

float cos_vector3d ( struct vector3d *v0, struct vector3d *v1 )
{
        /* v0.v1 = |v0||v1|cos a, cos a = (v0.v1)/(|v0||v1|) */
        float v0_dot_v1 = dot_vector3d ( v0, v1 );
        float v0_len_sq = v0->x*v0->x + v0->y*v0->y + v0->z*v0->z;
        float v1_len_sq = v1->x*v1->x + v1->y*v1->y + v1->z*v1->z;
        return v0_dot_v1/sqrtf ( v0_len_sq*v1_len_sq );
}

float cos_vector4d ( struct vector4d *v0, struct vector4d *v1 )
{
        /* v0.v1 = |v0||v1|cos a, cos a = (v0.v1)/(|v0||v1|) */
        float v0_dot_v1 = dot_vector4d ( v0, v1 );
        float v0_len_sq = v0->x*v0->x + v0->y*v0->y + v0->z*v0->z;
        float v1_len_sq = v1->x*v1->x + v1->y*v1->y + v1->z*v1->z;
        return v0_dot_v1/sqrtf ( v0_len_sq*v1_len_sq );
}

/*
 * Calculate the reflected vector of the incident vector over the normal
 * r = 2*n*(n.i) - i
 * Note that the following function compute the reflectance with the same
 * length as the incident as long as the normal is NORMALIZED
 */
void reflect_vector2d ( struct vector2d *normal, struct vector2d *incident, struct vector2d *vec_refl )
{

        float n_dot_i = 2.0f*dot_vector2d ( normal, incident );
        vec_refl->x = normal->x*n_dot_i - incident->x;
        vec_refl->y = normal->y*n_dot_i - incident->y;
}

void reflect_vector3d ( struct vector3d *normal, struct vector3d *incident, struct vector3d *vec_refl )
{
        float n_dot_i = 2.0f*dot_vector3d ( normal, incident );
        vec_refl->x = normal->x*n_dot_i - incident->x;
        vec_refl->y = normal->y*n_dot_i - incident->y;
        vec_refl->z = normal->z*n_dot_i - incident->z;
}

void reflect_vector4d ( struct vector4d *normal, struct vector4d *incident, struct vector4d *vec_refl )
{
        float n_dot_i = 2.0f*dot_vector4d ( normal, incident );
        vec_refl->x = normal->x*n_dot_i - incident->x;
        vec_refl->y = normal->y*n_dot_i - incident->y;
        vec_refl->z = normal->z*n_dot_i - incident->z;
}

/* The following function compute the reflectance when we have a cosine angle already
 * It computes a correct result ONLY when both the incident and normal are UNIT VECTOR */
void reflect_cos_vector2d ( float cos, struct vector2d *normal,
                            struct vector2d *incident, struct vector2d *vec_refl )
{
        cos *= 2.0f;
        vec_refl->x = normal->x*cos - incident->x;
        vec_refl->y = normal->y*cos - incident->y;
}

void reflect_cos_vector3d ( float cos, struct vector3d *normal,
                            struct vector3d *incident, struct vector3d * vec_refl )
{
        cos *= 2.0f;
        vec_refl->x = normal->x*cos - incident->x;
        vec_refl->y = normal->y*cos - incident->y;
        vec_refl->z = normal->z*cos - incident->z;
}

void reflect_cos_vector4d ( float cos, struct vector4d *normal,
                            struct vector4d *incident, struct vector4d *vec_refl )
{
        cos *= 2.0f;
        vec_refl->x = normal->x*cos - incident->x;
        vec_refl->y = normal->y*cos - incident->y;
        vec_refl->z = normal->z*cos - incident->z;
}

void print_vector2d ( struct vector2d *v )
{
        log_normal ( "V<%f, %f>", v->x, v->y );
}

void print_vector3d ( struct vector3d *v )
{
        log_normal ( "V<%f, %f, %f>", v->x, v->y, v->z );
}

void print_vector4d ( struct vector4d *v )
{
        log_normal ( "V<%f, %f, %f, %f>", v->x, v->y, v->z, v->w );
}
