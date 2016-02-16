#ifndef BASIC_RASTER_SHADER_A_H_INCLUDED
#define BASIC_RASTER_SHADER_A_H_INCLUDED


#include <math/math.h>
#include "../colorspectrum.h"
#include "../surface.h"


/* uniforms */
static struct matrix4x4 uni_modelview;

/* input variables */
static struct point3d *in_vertex;
static struct point4d *in_fragcoord;

/* output variable */
static struct point4d *out_vertex;
static pixf_ia8r8g8b8 *out_argb;

static void vs_to_modelview ( void );
static void fs_pure_color ( void );

static struct shader_func lib_vs_to_modelview = {
        .name = "vs_to_modelview_a",
        /* shader function */
        .func = vs_to_modelview,
        /* input variables */
        .in_var[0] = (void **) &in_vertex,
        .n_in = 1,
        /* output variables */
        .out_var[0] = (void **) &out_vertex,
        .n_out = 1,
};
static void vs_to_modelview ( void )
{
        mul_point3d_matrix4x4 ( in_vertex, &uni_modelview, to_3d(out_vertex) );
        out_vertex->w = 1.0f;
}

static struct shader_func lib_fs_pure_color = {
        .name = "fs_pure_color_a",
        /* shader function */
        .func = fs_pure_color,
        /* input variables */
        .in_var[0] = (void **) &in_fragcoord,
        .n_in = 1,
        /* output variables */
        .out_var[0] = (void **) &out_argb,
        .n_out = 1,
};
static void fs_pure_color ( void )
{
        struct float_color4 tc;
        set_color4 ( 1.0f, 0.0f, 0.0f, 1.0, &tc );
        rgb_to_ia8r8g8b8 ( &tc, out_argb );
}


#endif // BASIC_RASTER_SHADER_A_H_INCLUDED
