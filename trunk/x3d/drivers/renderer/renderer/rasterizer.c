/* rasterizer.c low-level fragment rasterization processes go here */
#include <math.h>
#include <algorithm.h>
#include <math/math.h>
#include "rasterization.h"
#include "rasterizer.h"

/* TODO (davis#3#): <rasterizer> write test case for the following functions */

enum {
	_DEPTH_TEST,
	_STENCIL_TEST,
	_ALPHA_TEST
};

struct rasterizer *create_rasterizer ( void )
{
}

void free_rasterizer ( struct rasterizer *rt )
{
}

void rasterizer_finalize ( struct rtcontext *cont, struct rasterizer *rt )
{
}

/* Interpolators */
static void int8_interpolator ( uint8_t *s, uint8_t *e, float t,
				float fs, float fe, float ft, int do_int, uint8_t *r )
{
	float pt = ((float) *s*fs + ((float) *e*fe - (float) *s*fe)*t)/ft;
	*r = (do_int) ? (uint8_t) pt : (*s);
}

static void int16_interpolator ( uint16_t *s, uint16_t *e, float t, int do_int, uint16_t *r )
{
	*r = (do_int) ? (uint16_t) ((float) *s + (float) (*e - *s)*t) : (*s);
}

static void int32_interpolator ( uint32_t *s, uint32_t *e, float t, int do_int, uint32_t *r )
{
	*r = (do_int) ? (uint32_t) ((float) *s + (float) (*e - *s)*t) : (*s);
}

static void int64_interpolator ( uint32_t *s, uint32_t *e, float t, int do_int, uint32_t *r )
{
	int i;
	for ( i = 0; i < 2; i ++ ) {
		r[i] = (do_int) ?
		       (uint32_t) ((float) s[i] + (float) (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void int96_interpolator ( uint32_t *s, uint32_t *e, float t, int do_int, uint32_t *r )
{
	int i;
	for ( i = 0; i < 3; i ++ ) {
		r[i] = (do_int) ?
		       (uint32_t) ((float) s[i] + (float) (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void int128_interpolator ( uint32_t *s, uint32_t *e, float t, int do_int, uint32_t *r )
{
	int i;
	for ( i = 0; i < 4; i ++ ) {
		r[i] = (do_int) ?
		       (uint32_t) ((float) s[i] + (float) (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void float32_interpolator ( float *s, float *e, float t, int do_int, float *r )
{
	int i;
	for ( i = 0; i < 1; i ++ ) {
		r[i] = (do_int) ?
		       (s[i] + (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void float64_interpolator ( float *s, float *e, float t, int do_int, float *r )
{
	int i;
	for ( i = 0; i < 2; i ++ ) {
		r[i] = (do_int) ?
		       (s[i] + (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void float96_interpolator ( float *s, float *e, float t, int do_int, float *r )
{
	int i;
	for ( i = 0; i < 3; i ++ ) {
		r[i] = (do_int) ?
		       (s[i] + (e[i] - s[i])*t) :
		       (s[i]);
	}
}

static void float128_interpolator ( float *s, float *e, float t, int do_int, float *r )
{
	int i;
	for ( i = 0; i < 4; i ++ ) {
		r[i] = (do_int) ?
		       (s[i] + (e[i] - s[i])*t) :
		       (s[i]);
	}
}


typedef void (*f_Interpolator) ( void *s, void *e, float t,
				 float fs, float fe, float ft, int do_int, void *r );
static const f_Interpolator VertIntOps[] = {
	[VERTEX_DEFN_NULL] = nullptr,
	[VERTEX_DEFN_INT8] = (f_Interpolator)		int8_interpolator,
	[VERTEX_DEFN_INT16] = (f_Interpolator)		int16_interpolator,
	[VERTEX_DEFN_INT32] = (f_Interpolator)		int32_interpolator,
	[VERTEX_DEFN_INT64] = (f_Interpolator)		int64_interpolator,
	[VERTEX_DEFN_INT96] = (f_Interpolator)		int96_interpolator,
	[VERTEX_DEFN_INT128] = (f_Interpolator)		int128_interpolator,
	[VERTEX_DEFN_FLOAT32] = (f_Interpolator)	float32_interpolator,
	[VERTEX_DEFN_FLOAT64] = (f_Interpolator)	float64_interpolator,
	[VERTEX_DEFN_FLOAT96] = (f_Interpolator)	float96_interpolator,
	[VERTEX_DEFN_FLOAT128] = (f_Interpolator)	float128_interpolator

};

static void draw_point ( untyped *v0, struct matrix4x4 *t_screen,
			 int *v_in_off, int v_in_size, int n_in_comp,
			 untyped *color, untyped *depth, int8_t *stencil,
			 int buf_x, int buf_y, int out_size,
			 FragShader shader_func, TestFunc test_func[3] )
{
	/* to screen coordinate */
	struct point4d *c_frag = v0;
	mul_point4d_matrix4x4_u ( c_frag, t_screen );
	c_frag->w = 1.0f/c_frag->w;
	uint8_t frag_out[32];
	struct ipoint2d icoord;
	icoord.x = buf_x*c_frag->x;
	icoord.y = buf_y*c_frag->y;
	if ( test_func[_DEPTH_TEST] ( icoord.x, icoord.y, c_frag->w, depth ) ) {
		struct alg_cdecl_stack curr_params;
		init_cdecl_stack ( &curr_params );
		int j;
		for ( j = 0; j < n_in_comp; j ++ ) {
			push_cdecl_stack ( &curr_params, v0 + v_in_off[j] );
		}
		shader_func ( &curr_params, frag_out );
		untyped *dest = color + out_size*(icoord.x + icoord.y*buf_x);
		memcpy ( dest, frag_out, out_size );
	}
}

static void draw_line ( untyped *v0, untyped *v1, struct matrix4x4 *t_screen,
			int *v_in_off, int *v_in_format, int v_in_size, int n_in_comp,
			untyped *color, untyped *depth, int8_t *stencil,
			int buf_x, int buf_y, int out_size,
			FragShader shader_func, TestFunc test_func[3] )
{
	uint8_t frag_out[32];
	untyped *tmp_attr[1024];
	struct alg_cdecl_stack stack;
	int c;
	for ( c = 0; c < n_in_comp; c ++ ) {
		push_cdecl_stack ( &stack, tmp_attr + v_in_off[c] );
	}
	/* to screen coordinate */
	untyped *vaddr[2];
	struct point4d *c_frag[2];
	struct ipoint2d icoord[2];
	vaddr[0] = v0;
	vaddr[1] = v1;
	int v;
	for ( v = 0; v < 2; v ++ ) {
		c_frag[v] = vaddr[v];
		mul_point4d_matrix4x4_u ( c_frag[v], t_screen );
		c_frag[v]->x = clamp ( c_frag[v]->x, 0.0f, 1.0f );
		c_frag[v]->y = clamp ( c_frag[v]->y, 0.0f, 1.0f );
		c_frag[v]->w = 1.0f/c_frag[v]->w;
		icoord[v].x = buf_x*c_frag[v]->x;
		icoord[v].y = buf_y*c_frag[v]->y;
	}
	float dydx = (c_frag[1]->y - c_frag[0]->y)/(c_frag[1]->x - c_frag[0]->x);
	if ( fabsf ( dydx ) <= 1.0f ) {
		/* go with x axis */
		float inv_dx = 1.0f/(float) (icoord[1].x - icoord[0].x);
		float dy = icoord[1].y - icoord[0].y;
		int i, j;
		float k = 0.0f;
		for ( i = icoord[0].x; i <= icoord[1].x; i ++ ) {
			float t = k*inv_dx;
			j = icoord[0].y + t*dy;
			float inv_w = c_frag[0]->w + (c_frag[1]->w - c_frag[0]->w)*t;
			if ( test_func[_DEPTH_TEST] ( i, j, inv_w, depth ) ) {
				for ( c = 0; c < n_in_comp; c ++ ) {
					VertIntOps[vertdefn_mask(v_in_format[c])] (
						vaddr[0] + v_in_off[c], vaddr[1] + v_in_off[c],
						t, c_frag[0]->w, c_frag[1]->w, inv_w,
						vertdefn_int(v_in_format[c]), tmp_attr );
				}
				untyped *dest = color + v_in_size*(i + j*buf_x);
				shader_func ( &stack, dest );
			}
			k ++;
		}
	} else {
		/* go with y axis */
		float inv_dy = 1.0f/(float) (icoord[1].y - icoord[0].y);
		float dx = icoord[1].x - icoord[0].x;
		int i, j;
		float k = 0.0f;
		for ( j = icoord[0].y; i <= icoord[1].y; j ++ ) {
			float t = k*inv_dy;
			i = icoord[0].x + t*dx;
			float inv_w = c_frag[0]->w + (c_frag[1]->w - c_frag[0]->w)*t;
			if ( test_func[_DEPTH_TEST] ( i, j, inv_w, depth ) ) {
				for ( c = 0; c < n_in_comp; c ++ ) {
					VertIntOps[vertdefn_mask(v_in_format[c])] (
						vaddr[0] + v_in_off[c], vaddr[1] + v_in_off[c],
						t, c_frag[0]->w, c_frag[1]->w, inv_w,
						vertdefn_int(v_in_format[c]), tmp_attr );
				}
				untyped *dest = color + v_in_size*(i + j*buf_x);
				shader_func ( &stack, dest );
			}
			k ++;
		}
	}

}

static void draw_upper_triangle ( untyped *v, struct matrix4x4 *t_screen,
				  int *v_in_off, int *v_in_format, int v_in_size, int n_in_comp,
				  untyped *color, untyped *depth, int8_t *stencil,
				  int buf_x, int buf_y, int out_size,
				  FragShader shader_func, TestFunc test_func[3] )
{
}

static void draw_bottom_triangle ( untyped *v, struct matrix4x4 *t_screen,
				   int *v_in_off, int *v_in_format, int v_in_size, int n_in_comp,
				   untyped *color, untyped *depth, int8_t *stencil,
				   int buf_x, int buf_y, int out_size,
				   FragShader shader_func, TestFunc test_func[3] )
{
}

static void draw_solid_triangle ( struct rasterizer *rt, rt_vertex *vert_list )
{
}

static void draw_wireframe_triangle ( struct rasterizer *rt, rt_vertex *vert_list )
{
	untyped *v[3];
	int i;
	for ( i = 0; i < 3; i ++ ) {
		v[i] = vert_list + i*rt->v_in_size;
	}
	draw_line ( v[0], v[1], &rt->t_screen,
		    rt->v_in_off, rt->v_in_format, rt->v_in_size, rt->n_in_comp,
		    rt->color_buf, rt->depth_buf, rt->stencil_buf,
		    rt->buf_x, rt->buf_y, rt->f_out_size,
		    rt->shader_func, rt->test_func );
	draw_line ( v[1], v[2], &rt->t_screen,
		    rt->v_in_off, rt->v_in_format, rt->v_in_size, rt->n_in_comp,
		    rt->color_buf, rt->depth_buf, rt->stencil_buf,
		    rt->buf_x, rt->buf_y, rt->f_out_size,
		    rt->shader_func, rt->test_func );
	draw_line ( v[2], v[0], &rt->t_screen,
		    rt->v_in_off, rt->v_in_format, rt->v_in_size, rt->n_in_comp,
		    rt->color_buf, rt->depth_buf, rt->stencil_buf,
		    rt->buf_x, rt->buf_y, rt->f_out_size,
		    rt->shader_func, rt->test_func );
}

void rasterizer_run ( struct rasterizer *rt, rt_vertex *vert_list,
		      enum RT_PRIMITIVE_TYPE prim_type, int count )
{
	int i;
	switch ( prim_type ) {
	case POINT_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			draw_point ( vert_list, &rt->t_screen,
				     rt->v_in_off, rt->v_in_size, rt->n_in_comp,
				     rt->color_buf, rt->depth_buf, rt->stencil_buf,
				     rt->buf_x, rt->buf_y, rt->f_out_size,
				     rt->shader_func, rt->test_func );
			vert_list += rt->v_in_size;
		}
		break;
	}
	case LINE_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			draw_line ( vert_list, vert_list + rt->v_in_size, &rt->t_screen,
				    rt->v_in_off, rt->v_in_format, rt->v_in_size, rt->n_in_comp,
				    rt->color_buf, rt->depth_buf, rt->stencil_buf,
				    rt->buf_x, rt->buf_y, rt->f_out_size,
				    rt->shader_func, rt->test_func );
			vert_list += 2*rt->v_in_size;
		}
		break;
	}
	case TRIANGLE_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			if ( rt->draw_mode == RT_SOLID_MODE ) {
				draw_wireframe_triangle ( rt, vert_list );
			} else {
				draw_solid_triangle ( rt, vert_list );
			}
			vert_list += 3*rt->v_in_size;
		}
		break;
	}
	}
}
