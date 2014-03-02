/* rasterizer.c low-level fragment rasterization processes go here */
#include <x3d/common.h>
#include <math.h>
#include <algorithm.h>
#include <math/math.h>
#include <renderer/shader.h>
#include <logout.h>
#include "rasterization.h"
#include "rasterizer.h"
#include "vertattri_lerp.h"
#include "surface.h"

/*
typedef bool (*f_Depth_Func) ( int x, int y, float inv_w, struct intn_buffers *buf );
typedef bool (*f_Stencil_func) ( int x, int y, int ref, struct intn_buffers *buf );
*/

struct intn_buffers {
	int buf_x, buf_y;
	int pixel_size;
	untyped *color;
	float *depth;
	uint8_t *stencil;
};

struct intn_testfunc {
/* NOTE (davis#3#): no alpha test for the moment */
	enum RT_EQUALITY dt_func;
	enum RT_EQUALITY st_func;
};

struct intn_vformat {
	int offset[MAX_VERT_COMP];
	int format[MAX_VERT_COMP];
	int size;
	int n_comp;
};

struct intn_rtctx {
	struct shader *shader;
	untyped *clip_cache;
	int frag_size;
	int draw_mode;
	f_Generic compiled_rasterizer;
};

struct rasterizer {
	struct intn_buffers buf;
	struct intn_vformat vf;
	struct intn_rtctx ctx;
	struct intn_testfunc func;
};


static bool depth_func ( int x, int y, float inv_w,
			struct intn_testfunc *func,
			struct intn_buffers *buf )
{
	switch ( func->dt_func ) {
	case RT_EQUALITY_FALSE: {
		return false;
	}
	case RT_EQUALITY_G: {
		return inv_w < buf->depth[x + y*buf->buf_x];
	}
	case RT_EQUALITY_GEQ: {
		return inv_w <= buf->depth[x + y*buf->buf_x];
	}
	case RT_EQUALITY_EQ: {
		return inv_w == buf->depth[x + y*buf->buf_x];
	}
	case RT_EQUALITY_LEQ: {
		return inv_w <= buf->depth[x + y*buf->buf_x];
	}
	case RT_EQUALITY_L: {
		return inv_w < buf->depth[x + y*buf->buf_x];
	}
	}
	assert ( false );
}

static void ndc_to_scc ( struct point4d *ndc, struct ipoint2d *scc,
			struct intn_buffers *buf )
{
	scc->x = (int) ((ndc->x + 1.0f)*0.5f*buf->buf_x);
	scc->y = (int) ((1.0 - ndc->y)*0.5f*buf->buf_y);
}

static void ndc_to_fragc ( struct point4d *ndc, struct point4d *fragc )
{
	fragc->x = ndc->x;
	fragc->y = ndc->y;
	fragc->w = 1.0f/ndc->w;
}

struct rasterizer *create_rasterizer ( void )
{
	struct rasterizer *rt = alloc_fix ( sizeof *rt, 1 );
	memset ( rt, 0, sizeof *rt );
	return rt;
}

void free_rasterizer ( struct rasterizer *rt )
{
	memset ( rt, 0, sizeof *rt );
	free_fix ( rt );
}

void rasterizer_finalize ( struct rtcontext *cont, struct rasterizer *rt )
{
	/* setting up vertex format */
	struct intn_vformat *vformat = &rt->vf;
	struct vertdefn *vdefn = &cont->vert_defn;
	memcpy ( vformat->format, vdefn->format, MAX_VERT_COMP );
	memcpy ( vformat->offset, vdefn->offset, MAX_VERT_COMP );
	vformat->size = vdefn->vertsize;
	vformat->n_comp = vdefn->n_comp;
	/* surfaces */
	struct intn_buffers *buffers = &rt->buf;
	if ( !(cont->buffer_state[RT_COLOR_BUFFER] & RT_BUFFER_ENABLE) ) {
		log_critical_err_dbg ( "color buffer has to be enabled" );
		return ;
	} else {
		buffers->color = surface_get_addr ( cont->color_buffer );
	}
	if ( cont->buffer_state[RT_DEPTH_BUFFER] & RT_BUFFER_ENABLE ) {
		buffers->depth = surface_get_addr ( cont->depth_buffer );
	} else {
		buffers->depth = nullptr;
	}
	if ( cont->buffer_state[RT_STENCIL_BUFFER] & RT_BUFFER_ENABLE ) {
		buffers->stencil = surface_get_addr ( cont->stencil_buffer );
	} else {
		buffers->stencil = nullptr;
	}
	buffers->pixel_size = surface_get_pixel_size ( cont->color_buffer );
	int w, h;
	surface_get_dimension ( cont->color_buffer, &w, &h );
	buffers->buf_x = w;
	buffers->buf_y = h;
	/* test functions (depth, alpha, stencil) */
	struct intn_testfunc *func = &rt->func;
	if ( cont->buffer_state[RT_DEPTH_BUFFER] & RT_BUFFER_ENABLE ) {
		func->dt_func = cont->depth_func;
	} else {
		func->dt_func = RT_EQUALITY_FALSE;
	}
	if ( cont->buffer_state[RT_STENCIL_BUFFER] & RT_BUFFER_ENABLE ) {
		func->st_func = cont->stencil_func;
	} else {
		func->st_func = RT_EQUALITY_FALSE;
	}
	/* clip memory, may produce pentagon when clipping triangles */
	struct intn_rtctx *ctx = &rt->ctx;
	if ( !ctx->clip_cache ) {
		ctx->clip_cache = alloc_var ( 1, vformat->size*3*3 );
	} else {
		ctx->clip_cache = expand2_var ( ctx->clip_cache, vformat->size*3*3 );
	}
}

static int clip_line ( untyped *v0, untyped *v1, struct intn_vformat *vformat,
		untyped *clip_mem )
{
	return 0;
}

static int clip_triangle ( untyped *v0, untyped *v1, untyped *v2,
			   struct intn_vformat *vformat,
			   untyped *clip_mem, int *index )
{
	return 0;
}

static void draw_point ( untyped *v, struct rasterizer *rt )
{
	struct intn_buffers *buf = &rt->buf;
	struct intn_vformat *vformat = &rt->vf;
	/* to screen coordinate */
	struct point4d *ndc = to_4d(v);
	struct ipoint2d icoord;
	ndc_to_scc ( ndc, &icoord, buf );
	ndc_to_fragc ( ndc, ndc );
	if ( depth_func ( icoord.x, icoord.y, ndc->w, &rt->func, buf ) ) {
		int j;
		untyped *dest =
			buf->color + buf->pixel_size*(icoord.x + icoord.y*buf->buf_x);
		for ( j = 0; j < vformat->n_comp; j ++ ) {
			shader_set_invar ( j, v + vformat->offset[j], rt->ctx.shader );
		}
		shader_set_outvar ( 0, dest, rt->ctx.shader );
		shader_run ( rt->ctx.shader );
	}
}

static void draw_line ( untyped *v0, untyped *v1, struct rasterizer *rt )
{
	struct intn_buffers *buf = &rt->buf;
	struct intn_vformat *vformat = &rt->vf;
	untyped *tmp_attr[1024];	/* for interpolated attributes */
	int c;
	for ( c = 0; c < vformat->n_comp; c ++ ) {
		shader_set_invar ( c, tmp_attr + vformat->offset[c],
				rt->ctx.shader );
	}
	/* to screen coordinate */
	untyped *vaddr[2] = {v0, v1};	/* ndc */
	struct point4d *fragc[2];	/* fragment coordinate */
	struct ipoint2d scc[2];		/* screen coordinate */
	int v;
	for ( v = 0; v < 2; v ++ ) {
		fragc[v] = to_4d(vaddr[v]);
		ndc_to_scc ( to_4d(vaddr[v]), &scc[v], buf );
		ndc_to_fragc ( to_4d(vaddr[v]), fragc[v] );
	}
	float dydx = (fragc[1]->y - fragc[0]->y)/(fragc[1]->x - fragc[0]->x);
	if ( fabsf ( dydx ) <= 1.0f ) {
		/* go with x axis */
		int i = scc[0].x;
		float j = (float) scc[0].y;
		float sdydx = (float) (scc[1].y - scc[0].y)/(scc[1].x - scc[0].x);

		float inv_dx = 1.0f/(fragc[1]->x - fragc[0]->x);
		float dw = fragc[1]->w - fragc[0]->w;
		float t = 0.0f;		/* parameter for interpolation */
		while ( i <= scc[1].x ) {
			float tx = t*inv_dx;
			float inv_w = lerpd ( fragc[0]->w, dw, tx );
			if ( depth_func ( i, (int) j, inv_w, &rt->func, buf ) ) {
				lerp_attri_c ( vformat->format, vformat->n_comp,
					       vformat->offset,
					       vaddr[0], fragc[0]->w,
					       vaddr[1], fragc[1]->w,
					       tx, tmp_attr, inv_w );
				untyped *dest = buf->color +
					buf->pixel_size*(i + (int) j*buf->buf_x);
				shader_set_outvar ( 0, dest, rt->ctx.shader );
				shader_run ( rt->ctx.shader );
			}
			i ++;
			j += sdydx;
			t ++;
		}
	} else {
		/* go with y axis */
		float i = (float) scc[0].x;
		int j = scc[0].y;
		float sdxdy = (float) (scc[1].x - scc[0].x)/(scc[1].y - scc[0].y);

		float inv_dy = 1.0f/(fragc[1]->y - fragc[0]->y);
		float dw = fragc[1]->w - fragc[0]->w;
		float t = 0.0f;		/* parameter for interpolation */
		while ( j <= scc[1].y ) {
			float ty = t*inv_dy;
			float inv_w = lerpd ( fragc[0]->w, dw, ty );
			if ( depth_func ( (int) i, j, inv_w, &rt->func, buf ) ) {
				lerp_attri_c ( vformat->format, vformat->n_comp,
					       vformat->offset,
					       vaddr[0], fragc[0]->w,
					       vaddr[1], fragc[1]->w,
					       ty, tmp_attr, inv_w );
				untyped *dest = buf->color +
					buf->pixel_size*((int) i + j*buf->buf_x);
				shader_set_outvar ( 0, dest, rt->ctx.shader );
				shader_run ( rt->ctx.shader );
			}
			i += sdxdy;
			j ++;
			t ++;
		}
	}

}

static void draw_upper_triangle ( untyped *v0, untyped *v1, untyped *v2,
				 struct rasterizer *rt )
{
}

static void draw_bottom_triangle ( untyped *v0, untyped *v1, untyped *v2,
				struct rasterizer *rt )
{
}

static void draw_solid_triangle ( struct rasterizer *rt, rt_vertex *vert_list,
				  int j, int *index )
{
}

static void draw_wireframe_triangle ( struct rasterizer *rt, rt_vertex *vert_list,
				      int j, int *index )
{
	untyped *v[3];
	int i;
	for ( i = 0; i < 3; i ++ ) {
		v[i] = vert_list + index[j + i]*rt->vf.size;
	}
	draw_line ( v[0], v[1], rt );
	draw_line ( v[1], v[2], rt );
	draw_line ( v[2], v[0], rt );
}

void rasterizer_run ( struct rasterizer *rt, rt_vertex *vert_list,
		      enum RT_PRIMITIVE_TYPE prim_type, int count )
{
	int i;
	switch ( prim_type ) {
	case POINT_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			draw_point ( vert_list, rt );
			vert_list += rt->vf.size;
		}
		break;
	}
	case LINE_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			untyped *v[3] = {
				vert_list + 0*rt->vf.size,
				vert_list + 1*rt->vf.size
			};
			int n = clip_line ( v[0], v[1], &rt->vf, rt->ctx.clip_cache );
			if ( n > 0 ) {
				draw_line ( v[0], v[1], rt );
			}
			vert_list += 2*rt->vf.size;
		}
		break;
	}
	case TRIANGLE_PRIMITIVE: {
		for ( i = 0; i < count; i ++ ) {
			untyped *v[3] = {
				vert_list + 0*rt->vf.size,
				vert_list + 1*rt->vf.size,
				vert_list + 2*rt->vf.size
			};
			int index[3*3];
			int n = clip_triangle ( v[0], v[1], v[2], &rt->vf,
						rt->ctx.clip_cache, index );
			int j;
			for ( j = 0; j < n; j ++ ) {
				if ( rt->ctx.draw_mode == RT_SOLID_MODE ) {
					draw_wireframe_triangle ( rt, rt->ctx.clip_cache,
								  j, index );
				} else {
					draw_solid_triangle ( rt, rt->ctx.clip_cache,
							      j, index );
				}
			}
			vert_list += 3*rt->vf.size;
		}
		break;
	}
	}
}
