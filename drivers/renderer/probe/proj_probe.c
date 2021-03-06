/* proj_probe.c: abstraction of projective probe */
#include <misc.h>
#include <math/math.h>
#include <renderer/proj_probe.h>
#include "perspective.h"
#include "orthogonal.h"
#include "fisheye.h"
#include "spherical.h"


struct probe_ops {
        struct proj_probe *(*create) ( void );
        void (*free) ( struct proj_probe *probe );
        void (*set_pos) ( struct point3d *pos, struct proj_probe *probe );
        void (*set_base) ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
                           struct proj_probe *probe );
        void (*set_dir) ( struct vector3d *up, struct vector3d *target,
                          struct proj_probe *probe );
        void (*set_optics) ( float focal_len, float v_dist, float apert,
                             struct proj_probe *probe );
        void (*set_range) ( float x, float y, float z0, struct proj_probe *probe );
        void (*get_transform) ( struct proj_probe *probe,
                                struct matrix4x4 *view, struct matrix4x4 *proj );
        void (*emit_ray) ( float ndc_x, float ndc_y, struct proj_probe *probe, struct line3d *ray );
};

const struct probe_ops ProbeOps[] = {
        [PERSPECTIVE_PROBE].create 	= cast(ProbeOps->create) create_perspective,
        [PERSPECTIVE_PROBE].free 	= cast(ProbeOps->free) free_perspective,
        [PERSPECTIVE_PROBE].set_pos	= cast(ProbeOps->set_pos) perspective_set_pos,
        [PERSPECTIVE_PROBE].set_base	= cast(ProbeOps->set_base) perspective_set_base,
        [PERSPECTIVE_PROBE].set_dir	= cast(ProbeOps->set_dir) perspective_set_dir,
        [PERSPECTIVE_PROBE].set_optics	= cast(ProbeOps->set_optics) perspective_set_optics,
        [PERSPECTIVE_PROBE].set_range	= cast(ProbeOps->set_range) perspective_set_range,
        [PERSPECTIVE_PROBE].get_transform = cast(ProbeOps->get_transform) perspective_get_trans,
        [PERSPECTIVE_PROBE].emit_ray	= cast(ProbeOps->emit_ray) perspective_emit_ray
};

void init_proj_probe ( enum PROJ_PROBE_IDR idr, struct proj_probe *probe )
{
        probe->probe = ProbeOps[idr].create ();
}

void free_proj_probe ( struct proj_probe *probe )
{
        ProbeOps[probe->idr].free ( probe );
        memset ( probe, 0, sizeof *probe );
}

void proj_probe_init ( struct proj_probe* probe, enum PROJ_PROBE_IDR idr,
                       int xres, int yres, int bits,
                       enum DisplayType display_type, void* widget )
{
        probe->idr      = idr;
        probe->dis_type = display_type;
        probe->widget   = widget;
        probe->xres     = xres;
        probe->yres     = yres;
        probe->res_size = bits/8;
        switch ( display_type ) {
        case DisplayGtk:
                break;
        case DisplayGtkOpenGL:
                break;
        case DisplayNull:
        default:
                break;
        }
        probe->probe = ProbeOps[idr].create ();
}

void proj_probe_free ( struct proj_probe* probe )
{
        ProbeOps[probe->idr].free ( probe );
        memset ( probe, 0, sizeof *probe );
}

void proj_probe_pos ( struct point3d *pos, struct proj_probe *probe )
{
        ProbeOps[probe->idr].set_pos ( pos, probe->probe );
}

void proj_probe_base ( struct vector3d *u, struct vector3d *v, struct vector3d *n,
                       struct proj_probe *probe )
{
        ProbeOps[probe->idr].set_base ( u, v, n, probe->probe );
}

void proj_probe_dir ( struct vector3d *up, struct vector3d *target,
                      struct proj_probe *probe )
{
        ProbeOps[probe->idr].set_dir ( up, target, probe->probe );
}

void proj_probe_optics ( float focal_len, float v_dist, float apert, struct proj_probe *probe )
{
        ProbeOps[probe->idr].set_optics ( focal_len, v_dist, apert, probe->probe );
}

void proj_probe_range ( float x, float y, float z, struct proj_probe *probe )
{
        ProbeOps[probe->idr].set_range ( x, y, z, probe->probe );
}

void proj_probe_get_transform ( struct proj_probe *probe,
                                struct matrix4x4 *view, struct matrix4x4 *proj )
{
        ProbeOps[probe->idr].get_transform ( probe->probe, view, proj );
}

void proj_probe_emit_ray ( float ndc_x, float ndc_y, struct proj_probe *probe, struct line3d *ray )
{
        ProbeOps[probe->idr].emit_ray ( ndc_x, ndc_y, probe->probe, ray );
}

/* @fixme (davis#1#): <proj_probe> implement display */

void proj_probe_take_this_fbo ( struct proj_probe* probe, struct fbo* fbo )
{
        probe->proj_data = fbo;
}

void proj_probe_display ( struct proj_probe* probe, void* widget )
{
}

void proj_probe_save_to_file ( struct proj_probe* probe, char* filename )
{
}

int proj_probe_get_xres ( struct proj_probe* probe )
{
        return probe->xres;
}

int proj_probe_get_yres ( struct proj_probe* probe )
{
        return probe->yres;
}

int proj_probe_get_bits ( struct proj_probe* probe )
{
        return probe->res_size*8;
}
