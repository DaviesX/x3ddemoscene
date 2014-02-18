#ifndef BASIC_RASTER_SHADER_A_H_INCLUDED
#define BASIC_RASTER_SHADER_A_H_INCLUDED


#include <math/math.h>


/* uniforms */
static struct matrix4x4 uni_modelview;

/* input variables */
static struct point3d *in_vertex;

/* output variable */
static struct point4d *out_vertex;

static void vs_to_modelview ( void );

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


#endif // BASIC_RASTER_SHADER_A_H_INCLUDED
