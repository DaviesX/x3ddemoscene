/* perspective.c: Operations to a perspective probe go here */
#include <container/paramset.h>
#include <renderer/proj_probe.h>
#include <math/math.h>
#include <system/allocator.h>
#include "perspective.h"



struct persp_probe {
        struct point3d target;
        struct point3d pos;
        struct vector3d up;
        struct vector3d u, v, n;

        float f_dist, u_dist, v_dist;
        float apert;
        float plate_ratio;
        float nz, fz;
        float tan_hfov;

        struct matrix4x4 t_view;
        struct matrix4x4 t_proj;
        struct matrix4x4 inv_t_view;
        struct matrix4x4 inv_t_proj;
        struct matrix4x4 t_inv_all;
};


static void update ( struct persp_probe *probe );

struct persp_probe *create_perspective ( void )
{
        struct persp_probe *probe = alloc_fix ( sizeof *probe, 1 );
        memset ( probe, 0, sizeof *probe );
        return probe;
}

void free_perspective ( struct persp_probe *probe )
{
        free_fix ( probe );
}

void perspective_set_optics ( float focal_len, float v_dist, float apert,
                              struct persp_probe *probe )
{
        assert ( fabsf ( focal_len ) >= 1e-5 );
        probe->f_dist = focal_len;

        assert ( fabsf ( v_dist - focal_len ) >= 1e-5 );
        assert ( fabsf ( v_dist ) >= 1.0f );
        probe->v_dist = v_dist;

        probe->u_dist = probe->f_dist*probe->v_dist/(probe->v_dist - probe->f_dist);
        probe->tan_hfov = 0.5f/mm_to_unit ( probe->v_dist );

        probe->apert = apert;
        if ( *(int *) &apert != 0 ) {
                probe->tan_hfov =
                        min ( probe->tan_hfov, probe->apert/probe->f_dist );
        }
        probe->nz = 0.5f/probe->tan_hfov;
}

void perspective_set_pos ( struct point3d *pos, struct persp_probe *probe )
{
        copy_point3d ( pos, &probe->pos );
}

void perspective_set_base ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
                            struct persp_probe *probe )
{
        copy_vector3d ( u, &probe->u );
        copy_vector3d ( v, &probe->v );
        copy_vector3d ( n, &probe->n );
}

void perspective_set_dir ( struct vector3d *up, struct point3d *target,
                           struct persp_probe *probe )
{
        copy_vector3d ( up, &probe->up );
        copy_vector3d ( target, &probe->target );

        build_vector3d ( &probe->pos, &probe->target, &probe->n );
        cross_vector3d ( &probe->up, &probe->n, &probe->u );
        cross_vector3d ( &probe->n, &probe->u, &probe->v );
}

void perspective_set_range ( float x, float y, float z,
                             struct persp_probe *probe )
{
        probe->plate_ratio = x/y;
        probe->fz = z;
}

static void update ( struct persp_probe *probe )
{
        /* t_view */
        struct matrix4x4 t;
        struct matrix4x4 r;
        get_translate_matrix4x4 ( probe->pos.x, probe->pos.y, probe->pos.z, &t );
        get_rotate_to_uvn_matrix4x4 ( &probe->u, &probe->v, &probe->n, &r );
        mul_matrix4x4 ( &r, &t, &probe->t_view );

        /* t_proj: (NL*x, NT*y, az + b, z)
         * x' = nx/z * 1/R (R = 1/2*len)
         * y' = ny/z * r/R (r = len/height)
         * z' = 1/z * [(f + n)/(f - n)z + 2fn/(n - f)] */
        const float inv_R = 1.0f;	/* R = 1.0f/2.0f*2.0f */
        float nl = probe->nz * inv_R;
        float nt = probe->nz * probe->plate_ratio*inv_R;
        float a = (probe->nz + probe->fz)/(probe->fz - probe->nz);
        float b = 2.0f*probe->nz*probe->fz/(probe->nz - probe->fz);
        set_matrix4x4 ( &probe->t_proj,
                        nl, 0.0f, 0.0f, 0.0f,
                        0.0f, nt, 0.0f, 0.0f,
                        0.0f, 0.0f, a,   1.0f,
                        0.0f, 0.0f, b,   0.0f );

        /* t_corr */
        /* sx = (x + 1)*w/2
         * sy = (y + 1)*h/2 */
        /*	float xa = plate_len*0.5f;
        	float ya = plate_len*probe->plate_ratio*0.5f;
        	set_matrix4x4 ( &probe->t_corr,
        			xa,   0.0f, 0.0f, 0.0f,
        			0.0f, ya,   0.0f, 0.0f,
        			0.0f, 0.0f, 1.0f, 0.0f,
        			xa,   ya,   0.0f, 1.0f );
         We are to transform to ndc but not to the screen coordinate  */
        /* Inverse process */
        inv_matrix4x4 ( &probe->t_view, &probe->inv_t_view );
        inv_matrix4x4 ( &probe->t_proj, &probe->inv_t_proj );
        mul_matrix4x4 ( &probe->inv_t_view, &probe->inv_t_proj,
                        &probe->t_inv_all );
        /*	mul_matrix4x4_u ( &probe->t_inv_all, &probe->inv_t_corr ); */
}

void perspective_get_trans ( struct persp_probe *probe,
                             struct matrix4x4 *t_view,
                             struct matrix4x4 *t_proj )
{
        update ( probe );
        copy_matrix4x4 ( &probe->t_view, t_view );
        copy_matrix4x4 ( &probe->t_proj, t_proj );
        /*	copy_matrix4x4 ( &probe->t_corr, t_corr ); */
}

/* output params order: up, pos, target, w/nz, width, nz, fz */
void perspective_get_params ( struct persp_probe *probe, struct alg_named_params *params )
{
        reset_named_params ( params );
        push_named_params ( probe->up, "up", params );
        push_named_params ( probe->pos, "position", params );
        push_named_params ( probe->target, "target", params );
        push_named_params ( probe->plate_ratio, "plate-ratio", params );
        push_named_params ( probe->f_dist, "focal-length", params );
        push_named_params ( probe->v_dist, "image-distance", params );
        push_named_params ( probe->u_dist, "object-distance", params );
        push_named_params ( probe->apert, "aperture", params );
        push_named_params ( probe->u, "u", params );
        push_named_params ( probe->v, "v", params );
        push_named_params ( probe->n, "n", params );
}

void perspective_emit_ray ( float ndc_x, float ndc_y, struct persp_probe *probe,
                            struct line3d *ray )
{
        /* Fake a 4d homogeneous point right on the projection plane,
         * and then bring it back to world space as pt
         * Fake another point right at the origin of the view space,
         * bring it back to world space, also, as p0 */
        /* P0(x, y, z, w) = (x'R, y'R/r, n, 1) */
        struct point4d p_plane;
        const float R = 1.0f;
        const float r = probe->plate_ratio;
        set_point4d ( ndc_x*R, ndc_y*R/r, probe->nz, 1.0f, &p_plane );
        mul_point4d_matrix4x4_u ( &p_plane, &probe->inv_t_view );

        struct point4d p_ori;
        set_point4d ( 0.0f, 0.0f, 0.0f, 1.0f, &p_ori );
        mul_point4d_matrix4x4_u ( &p_ori, &probe->inv_t_view );

        build_line3d_t ( to_3d(&p_ori), to_3d(&p_plane), 0.0f, FLOAT_MAX, ray );
}
