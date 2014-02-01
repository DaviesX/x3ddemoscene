/* vertprocessor.c: all vertex processing during rasterization go here */
#include <logout.h>
#include <algorithm.h>
#include "rasterization.h"
#include "vibuffer.h"
#include "vertprocessor.h"
#include "main.h"


/* TODO (davis#3#): <vertprocessor> write unit test for all vertprocessor code */


#define INIT_VB_COUNT			1024
#define INIT_IB_COUNT			3*1024

const int SizeOfVertF[] = {
	[VERTEX_ELM_INT8] = 1,
	[VERTEX_ELM_INT16] = 2,
	[VERTEX_ELM_INT32] = 4,
	[VERTEX_ELM_FLOAT] = 4,
	[VERTEX_ELM_DOUBLE] = 8
};
const int SizeOfIndexF[] = {
	[INDEX_ELEMENT_INT8] = 1,
	[INDEX_ELEMENT_INT16] = 2,
	[INDEX_ELEMENT_INT32] = 4
};

static uint32_t *g_ordered_arr = nullptr;
int g_arr_cnt = 0;

static void vertattri_fix ( struct vertattri *vattri );
static void *gen_ordered_stream ( int count );


static void *gen_ordered_stream ( int count )
{
	if ( g_ordered_arr ) {
		g_ordered_arr = expand2_var ( g_ordered_arr, count );
	} else {
		g_ordered_arr = alloc_var ( sizeof ( uint32_t ), count );
	}
	for ( ; g_arr_cnt < count; g_arr_cnt ++ ) {
		g_ordered_arr[g_arr_cnt] = g_arr_cnt;
	}
	return g_ordered_arr;
}

/* vertex/index buffer */
struct vertex_buffer *create_vbuffer ( bool is_static )
{
	struct vertex_buffer *vbuffer = alloc_fix ( sizeof *vbuffer, 1 );
	memset ( vbuffer, 0, sizeof *vbuffer );
	vbuffer->buffer = alloc_var ( 1, INIT_VB_COUNT );
	vbuffer->curr = vbuffer->buffer;
	vbuffer->is_static = is_static;
	return vbuffer;
}

struct index_buffer *create_ibuffer ( bool is_static )
{
	struct index_buffer *ibuffer = alloc_fix ( sizeof *ibuffer, 1 );
	memset ( ibuffer, 0, sizeof *ibuffer );
	ibuffer->buffer = alloc_var ( 1, INIT_IB_COUNT );
	ibuffer->curr = ibuffer->buffer;
	ibuffer->is_static = is_static;
	return ibuffer;
}

void free_vbuffer ( struct vertex_buffer *vbuffer )
{
	free_var ( vbuffer->buffer );
	free_fix ( vbuffer );
}

void free_ibuffer ( struct index_buffer *ibuffer )
{
	free_var ( ibuffer->buffer );
	free_fix ( ibuffer );
}

void vertattri_init ( struct vertattri *vattri )
{
	memset ( vattri, 0, sizeof *vattri );
}

void vertattri_set ( int i, enum VERTEX_ELM_FORMAT format, int n_elm, struct vertattri *vattri )
{
	vattri->format[i] = format;
	vattri->n_elm[i] = n_elm;
}

static void vertattri_fix ( struct vertattri *vattri )
{
	int i;
	for ( i = 0; vattri->n_elm[i] != 0; i ++ ) {
		vattri->offset[i] = vattri->vertsize;
		vattri->vertsize +=
			vattri->n_elm[i]*SizeOfVertF[vattri->format[i]];
	}
	vattri->n_attri = i;
}

void vbuffer_bind_vertattri ( struct vertattri *vattri, struct vertex_buffer *vbuffer )
{
	vertattri_fix ( vattri );
	memcpy ( &vbuffer->attri, vattri, sizeof *vattri );
}

void ibuffer_set_format ( enum INDEX_ELEMENT_FORMAT format, struct index_buffer *ibuffer )
{
	ibuffer->format = format;
}

void vbuffer_add_vertex ( struct vertex_buffer *vbuffer )
{
	int i = vbuffer->curr - (untyped *) vbuffer->buffer;
	vbuffer->buffer = add_var ( vbuffer->buffer, vbuffer->attri.vertsize );
	vbuffer->curr = (untyped *) vbuffer->buffer + i;
	vbuffer->elm_cnt ++;
}

void ibuffer_add_index ( struct index_buffer *ibuffer )
{
	int i = ibuffer->curr - (untyped *) ibuffer->buffer;
	ibuffer->buffer = add_var ( ibuffer->buffer, SizeOfIndexF[ibuffer->format] );
	ibuffer->curr = (untyped *) ibuffer->buffer + i;
	ibuffer->elm_cnt ++;
}

/* vertex processor */
struct vertprocessor *create_vertprocessor ( void )
{
	struct vertprocessor *vp = alloc_fix ( sizeof *vp, 1 );
	memset ( vp, 0, sizeof *vp );
	return vp;
}

void free_vertprocessor ( struct vertprocessor *vp )
{
	free_fix ( vp );
}

void vertprocessor_finalize ( struct rtcontext *cont, struct vertprocessor *vp )
{
	memset ( vp, 0, sizeof *vp );
	/* checkings */
	struct vertex_buffer *vbuffer = cont->vbuffer;
	struct index_buffer *ibuffer = cont->ibuffer;
	if ( !(cont->buffer_state[RT_VERTEX_BUFFER] & RT_BUFFER_ENABLE) ) {
		log_critical_err_dbg ( "vertex buffer is not enabled" );
	}
	if ( !vbuffer ) {
		log_critical_err_dbg ( "vertex buffer is enabled whereas data is not specified" );
	}
	if ( !ibuffer &&
	     (cont->buffer_state[RT_INDEX_BUFFER] & RT_BUFFER_ENABLE) ) {
		log_critical_err_dbg ( "index buffer is enabled whereas data is not specified" );
	}
	/* setting up vertex/index stream */
	vp->vert_stream = vbuffer->buffer;
	if ( cont->buffer_state[RT_INDEX_BUFFER] & RT_BUFFER_ENABLE ) {
		vp->n_total = ibuffer->elm_cnt;
		vp->index_stream = ibuffer->buffer;
		vp->index_size = SizeOfIndexF[ibuffer->format];
	} else {
		vp->n_total = vbuffer->elm_cnt;
		vp->index_stream = gen_ordered_stream ( vp->n_total );
		vp->index_size = SizeOfIndexF[INDEX_ELEMENT_INT32];
	}
	/* setting up vertex format */
	struct vertattri *attri = &vbuffer->attri;
	vp->n_in_attri = attri->n_attri;
	vp->v_in_size = attri->vertsize;
	memcpy ( vp->v_in_off, attri->offset, attri->n_attri*sizeof ( int ) );
	struct vertdefn *vdefn = &cont->vert_defn;
	vp->n_out_comp = vdefn->n_comp;
	vp->v_out_size = vdefn->vertsize;
	memcpy ( vp->v_out_format, vdefn->format, vdefn->n_comp*sizeof ( int ) );
	memcpy ( vp->v_out_off, vdefn->offset, vdefn->n_comp*sizeof ( int ) );
	/* setting up transform */
	switch ( cont->cull_state ) {
	case CULL_IDR_NULL: {
		vp->cull_factor = 0.0f;
		break;
	}
	case CULL_IDR_BACKFACE: {
		vp->cull_factor = 1.0f;
		break;
	}
	case CULL_IDR_FONTFACE: {
		vp->cull_factor = -1.0f;
		break;
	}
	}
	copy_matrix4x4 ( &cont->view, &vp->t_view );
	copy_matrix4x4 ( &cont->proj, &vp->t_proj );
	/* cache allocation */
	vp->cache = alloc_var ( 1, vp->n_done*vp->v_out_size );
	vp->clip_cache = alloc_var ( 1, 8*vp->v_out_size );
	/* TODO (davis#3#): generate vertex processor and set up vertex shader */
}

/* interpolators*/
static void int8_interpolator ( uint8_t *s, uint8_t *e, float t, int do_int, uint8_t *r )
{
	*r = (do_int) ? (uint8_t) ((float) *s + (float) (*e - *s)*t) : (*s);
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

typedef void (*f_Interpolator) ( void *s, void *e, float t, int do_int, void *r );
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

static int process_point ( rt_vertex *vi,
			   struct matrix4x4 *t_all, int vert_size,
			   rt_vertex *vo )
{
	/* transform to cvv */
	struct point4d *v_pos = to_4d (vi);
	mul_point4d_matrix4x4_u ( v_pos, t_all );
	/* clipping in cvv */
	/* to ndc */
	div_w_point4d_p ( v_pos );
	if ( v_pos->x >= -1.0f && v_pos->x <= 1.0f &&
	     v_pos->y >= -1.0f && v_pos->y <= 1.0f &&
	     v_pos->z >= -1.0f && v_pos->z <= 1.0f ) {
		memcpy ( vo, vi, vert_size );
		return 1;
	} else {
		return 0;
	}
}

static int process_line ( rt_vertex *vi,
			  struct matrix4x4 *t_all,
			  int *comp_offset, int *comp_format, int n_comp,
			  int vert_size, rt_vertex *vo )
{
	rt_vertex *vaddr[2];
	rt_vertex *vaddro[2];
	struct point4d *v_pos[2];
	/* transform to cvv */
	int v;
	for ( v = 0; v < 2; v ++ ) {
		v_pos[v] = to_4d(vi + v*vert_size);
		vaddr[v] = vi + v*vert_size;
		vaddro[v] = vo + v*vert_size;
		mul_point4d_matrix4x4_u ( v_pos[v], t_all );
	}
	/* cvv clipping */
	/* trivial accept/reject */
	const float inv_pw[2] = {
		1.0f/v_pos[0]->w,
		1.0f/v_pos[1]->w
	};
	struct point3d bc[2];
	set_point3d ( v_pos[0]->x*inv_pw[0], v_pos[0]->y*inv_pw[0], v_pos[0]->z*inv_pw[0], &bc[0] );
	set_point3d ( v_pos[1]->x*inv_pw[1], v_pos[1]->y*inv_pw[1], v_pos[1]->z*inv_pw[1], &bc[1] );
	const bool tr_accept =
		bc[0].x >= -1.0f && bc[1].x >= -1.0f &&
		bc[0].x <= 1.0f && bc[1].x <= 1.0f &&
		bc[0].y >= -1.0f && bc[1].y >= -1.0f &&
		bc[0].y <= 1.0f && bc[1].y <= 1.0f &&
		bc[0].z >= -1.0f && bc[1].z >= -1.0f &&
		bc[0].z <= 1.0f && bc[1].z <= 1.0f;
	if ( tr_accept ) {
		/* trivial accept, when all points are inside of all planes */
		/* to ndc */
		div_w_point4d_p ( v_pos[0] );
		div_w_point4d_p ( v_pos[1] );
		memcpy ( vaddro[0], vaddr[0], vert_size );
		memcpy ( vaddro[1], vaddr[1], vert_size );
		return 2;
	}
	const bool tr_reject =
		(bc[0].x < -1.0f && bc[1].x < -1.0f) ||
		(bc[0].x > 1.0f && bc[1].x > 1.0f) ||
		(bc[0].y < -1.0f && bc[1].y > -1.0f) ||
		(bc[0].y > 1.0f && bc[1].y > 1.0f) ||
		(bc[0].z < -1.0f && bc[1].z < -1.0f) ||
		(bc[0].z > 1.0f && bc[1].z > 1.0f);
	if ( tr_reject ) {
		/* trivial reject, when all points are outside of the same plane */
		return 0;
	}
	/* clipping */
	float tmin = -FLOAT_INFINITE;
	float tmax = FLOAT_INFINITE;
	int i;
	for ( i = 0; i < 3; i ++ ) {
		/* (P0 + V*t)/(P0w + Vw*t) = +1, and
		 * (P0 + V*t)/(P0w + Vw*t) = -1 for plane-pair intersection,
		 * where V = P1 - P0, Vw = P1w - P0w */
		float t0 = (v_pos[0]->w - v_pos[0]->p[i])/
			   ((v_pos[0]->w - v_pos[0]->p[i]) -
			    (v_pos[1]->w - v_pos[1]->p[i]));
		float t1 = (v_pos[0]->w + v_pos[0]->p[i])/
			   ((v_pos[0]->p[i] - v_pos[0]->w) +
			    (v_pos[1]->p[i] - v_pos[1]->w));
		if ( t0 > t1 ) {
			swap ( t0, t1 );
		}
		/* marching toward the middle of the line */
		tmin = max ( t0, tmin );
		tmax = min ( t1, tmax );
		if ( tmin > tmax ) {
			/* out of the corner, no valid intersection */
			return 0;
		}
	}
	tmin = clamp ( tmin, 0.0f, 1.0f );
	tmax = clamp ( tmax, 0.0f, 1.0f );
	/* Interpolation */
	int k;
	for ( k = 0; k < n_comp; k ++ ) {
		/* interpolates from original point0 to new point0 */
		VertIntOps[vertdefn_mask(comp_format[k])] (
			vaddr[0] + comp_offset[k],
			vaddr[1] + comp_offset[k],
			tmin, vertdefn_int(comp_format[k]),
			vaddro[0] + comp_offset[k] );
		/* interpolates from original point0 to new point1 */
		VertIntOps[vertdefn_mask(comp_format[k])] (
			vaddr[0] + comp_offset[k],
			vaddr[1] + comp_offset[k],
			tmax, vertdefn_int(comp_format[k]),
			vaddro[1] + comp_offset[k] );
	}
	/* to ndc */
	div_w_point4d_p ( to_4d(vaddro[0]) );
	div_w_point4d_p ( to_4d(vaddro[1]) );
	return 2;
}

static int process_triangle ( rt_vertex *vi,
			      struct matrix4x4 *t_view, struct matrix4x4 *t_proj,
			      float cull_factor,
			      int *comp_offset, int *comp_format, int n_comp,
			      int vert_size, rt_vertex *cache, rt_vertex *vo )
{
	rt_vertex *vaddr[3];
	rt_vertex *vaddro[3];
	struct point4d *v_pos[3];
	int v;
	for ( v = 0; v < 3; v ++ ) {
		v_pos[v] = to_4d(vi + v*vert_size);
		vaddr[v] = vi + v*vert_size;
		vaddro[v] = vo + v*vert_size;
		/* transform to view */
		mul_point4d_matrix4x4_u ( v_pos[v], t_view );
	}
	/* face culling */
	struct vector4d r, s, t;
	build_vector4d ( v_pos[0], v_pos[1], &r );
	build_vector4d ( v_pos[0], v_pos[2], &s );
	cross_vector4d ( &r, &s, &t );
	struct vector4d u;
	scale_vector4d ( -1.0f, v_pos[0], &u );		/* u = o - p */
	float visible = cull_factor*dot_vector4d ( &t, &u );
	if ( visible < 0.0f ) {
		return 0;
	}
	/* transform to cvv */
	for ( v = 0; v < 3; v ++ ) {
		mul_point4d_matrix4x4_u ( v_pos[v], t_proj );
	}
	/* cvv clipping */
	/* trivial accept/reject */
	const float inv_pw[3] = {
		1.0f/v_pos[0]->w,
		1.0f/v_pos[1]->w,
		1.0f/v_pos[2]->w
	};
	struct point3d bc[3];
	set_point3d ( v_pos[0]->x*inv_pw[0], v_pos[0]->y*inv_pw[0], v_pos[0]->z*inv_pw[0], &bc[0] );
	set_point3d ( v_pos[1]->x*inv_pw[1], v_pos[1]->y*inv_pw[1], v_pos[1]->z*inv_pw[1], &bc[1] );
	set_point3d ( v_pos[2]->x*inv_pw[2], v_pos[2]->y*inv_pw[2], v_pos[2]->z*inv_pw[2], &bc[2] );
	const bool tr_accept =
		bc[0].x >= -1.0f && bc[1].x >= -1.0f && bc[2].z >= -1.0f &&
		bc[0].x <= 1.0f && bc[1].x <= 1.0f && bc[2].x <= 1.0f &&
		bc[0].y >= -1.0f && bc[1].y >= -1.0f && bc[2].y >= -1.0f &&
		bc[0].y <= 1.0f && bc[1].y <= 1.0f && bc[2].y <= 1.0f &&
		bc[0].z >= -1.0f && bc[1].z >= -1.0f && bc[2].z >= -1.0f &&
		bc[0].z <= 1.0f && bc[1].z <= 1.0f && bc[2].z <= 1.0f;
	if ( tr_accept ) {
		/* trivial accept, when all points are inside of all planes */
		/* to ndc */
		div_w_point4d_p ( v_pos[0] );
		div_w_point4d_p ( v_pos[1] );
		div_w_point4d_p ( v_pos[2] );
		memcpy ( vaddro[0], vaddr[0], vert_size );
		memcpy ( vaddro[1], vaddr[1], vert_size );
		memcpy ( vaddro[2], vaddr[2], vert_size );
		return 3;
	}
	const bool tr_reject =
		(bc[0].x < -1.0f && bc[1].x < -1.0f && bc[2].x < -1.0f) ||
		(bc[0].x > 1.0f && bc[1].x > 1.0f && bc[2].x > 1.0f) ||
		(bc[0].y < -1.0f && bc[1].y > -1.0f && bc[2].y > -1.0f) ||
		(bc[0].y > 1.0f && bc[1].y > 1.0f && bc[2].y > 1.0f) ||
		(bc[0].z < -1.0f && bc[1].z < -1.0f && bc[2].z < -1.0f) ||
		(bc[0].z > 1.0f && bc[1].z > 1.0f && bc[2].z > 1.0f);
	if ( tr_reject ) {
		/* trivial reject, when all points are outside of the same plane */
		return 0;
	}
	struct point4d *vlist[2][8];
	vlist[0][0] = v_pos[0];
	vlist[0][2] = v_pos[1];
	vlist[0][4] = v_pos[2];
	int n_vert = 3;
	int i_list = 0;
	rt_vertex *curr_cache = cache;
	int i;
	for ( i = 0; i < 3; i ++ ) {
		int j;
		int k = 0;	/* new vertices (output list index) */
		for ( j = 0; j < n_vert; j ++ ) {
			/* (P0 + V*t)/(P0w + Vw*t) = +1, and
			 * (P0 + V*t)/(P0w + Vw*t) = -1 for plane-pair intersection,
			 * where V = P1 - P0, Vw = P1w - P0w */
			struct point4d *p0 = vlist[i_list&1][j + 0];
			struct point4d *p1 = vlist[i_list&1][(j + 1)%n_vert];
			float t0 = (p0->w - p0->p[i])/
				   ((p0->w - p0->p[i]) - (p1->w - p1->p[i]));
			float t1 = (p0->w + p0->p[i])/
				   ((p0->p[i] - p0->w) + (p1->p[i] - p1->w));
			if ( t0 > t1 ) {
				swap ( t0, t1 );
			}
			t0 = clamp ( t0, 0.0f, 1.0f );
			t1 = clamp ( t1, 0.0f, 1.0f );
			/* save p0 to output list when the original input is visible */
			float inv_pw = 1.0f/p0->w;
			if ( p0->p[i]*inv_pw >= -1.0f && p0->p[i]*inv_pw <= 1.0f ) {
				vlist[(i_list + 1)&1][k ++] = p0;
			}
			/* interpolates to new p0 when the new point is visible */
			float new_pi = p0->p[i] + t0*(p1->p[i] - p0->p[i]);
			inv_pw = 1.0f/(p0->w + t0*(p1->w - p0->w));
			if ( new_pi*inv_pw >= -1.0f && new_pi*inv_pw <= 1.0f ) {
				curr_cache += k*vert_size;
				int c;
				for ( c = 0; c < n_comp; c ++ ) {
					/* Lerp */
					VertIntOps[vertdefn_mask(comp_format[c])] (
						(untyped *) p0 + comp_offset[c],
						(untyped *) p1 + comp_offset[c],
						t0, vertdefn_int(comp_format[c]),
						curr_cache + comp_offset[c] );
				}
				vlist[(i_list + 1)&1][k ++] = to_4d(curr_cache);
			}
			/* interpolates to new p1 when the new point is visible */
			new_pi = p0->p[i] + t1*(p1->p[i] - p0->p[i]);
			inv_pw = 1.0f/(p0->w + t1*(p1->w - p0->w));
			if ( new_pi*inv_pw >= -1.0f && new_pi*inv_pw <= 1.0f ) {
				curr_cache += k*vert_size;
				int c;
				for ( c = 0; c < n_comp; c ++ ) {
					/* Lerp */
					VertIntOps[vertdefn_mask(comp_format[c])] (
						(untyped *) p0 + comp_offset[c],
						(untyped *) p1 + comp_offset[c],
						t1, vertdefn_int(comp_format[c]),
						curr_cache + comp_offset[c] );
				}
				vlist[(i_list + 1)&1][k ++] = to_4d(curr_cache);
			}
			/* save p1 to output list when the original input is visible */
			inv_pw = 1.0f/p1->w;
			if ( p1->p[i]*inv_pw >= -1.0f && p1->p[i]*inv_pw <= 1.0f ) {
				vlist[(i_list + 1)&1][k ++] = p1;
			}
		}
		n_vert = k;
		i_list ++;
	}
	/* contruct the new triangles and send them out to vo */
	int n_tri = 0;		/* number of triangles constructed */
	struct point4d **new_list = vlist[(i_list + 1)&1];	/* Avoid negative wrap: (i - 1)%2 = (i + 1)%2*/
	div_w_point4d_p ( new_list[0] );	/* transform the first point to ndc to save redundant division */
	int n;
	for ( n = 1; n + 1 < n_vert; n ++ ) {
		rt_vertex *curr_addr = vo + n_tri*3;
		memcpy ( curr_addr + 0, new_list[0], vert_size );
		memcpy ( curr_addr + 1, new_list[n], vert_size );
		memcpy ( curr_addr + 2, new_list[n + 1], vert_size );
		/* to ndc */
		div_w_point4d_p ( to_4d(curr_addr + 1) );
		div_w_point4d_p ( to_4d(curr_addr + 2) );
		n_tri ++;
	}
	return n_tri*3;
}

int vertprocessor_run ( struct vertprocessor *vp, enum RT_PRIMITIVE_TYPE prim_type,
			int count, rt_vertex **vo )
{
	/* vertex shader stage */
	count = min ( count, vp->n_total - vp->n_done );
	count = count - count%prim_type;
	flush_var ( vp->cache );
	vp->cache = expand2_var ( vp->cache, count );
	untyped *vert_out = vp->cache;
	untyped *index_pos = vp->index_stream;
	int i;
	for ( i = 0; i < count; i ++ ) {
		int curr_i;
		switch ( vp->index_size ) {
		case 8: {
			curr_i = *(uint8_t *) index_pos;
			break;
		}
		case 16: {
			curr_i = *(uint16_t *) index_pos;
			break;
		}
		case 32: {
			curr_i = *(uint32_t *) index_pos;
			break;
		}
		}
		untyped *curr_v_pos = vp->vert_stream + curr_i*vp->v_in_size;
		struct alg_cdecl_stack curr_params;
		init_cdecl_stack ( &curr_params );
		int j;
		for ( j = 0; j < vp->n_in_attri; j ++ ) {
			push_cdecl_stack ( &curr_params, curr_v_pos + vp->v_in_off[j] );
		}
		vp->shader_func ( &curr_params, vert_out );
		index_pos += vp->index_size;
		vert_out += vp->v_out_size;
	}
	vp->n_done += count;
	/* post vertex processing */
	untyped *vert_in = vert_out;
	vert_out = *vo;
	i = 0;
	int j = 0;
	switch ( prim_type ) {
	case POINT_PRIMITIVE: {
		flush_var ( vert_out );
		vert_out = expand_var ( vert_out, count );
		struct matrix4x4 t_all;
		mul_matrix4x4 ( &vp->t_proj, &vp->t_view, &t_all );
		while ( j < count ) {
			int n = process_point ( &vert_in[j*vp->v_out_size],
						&t_all, vp->v_out_size,
						&vert_out[i*vp->v_out_size] );
			j += 1;
			i += n;
		}
		break;
	}
	case LINE_PRIMITIVE: {
		flush_var ( vert_out );
		vert_out = expand_var ( vert_out, count );
		struct matrix4x4 t_all;
		mul_matrix4x4 ( &vp->t_proj, &vp->t_view, &t_all );
		while ( j < count ) {
			int n = process_line ( &vert_in[j*vp->v_out_size], &t_all,
					       vp->v_out_off, vp->v_out_format, vp->n_out_comp,
					       vp->v_out_size, &vert_out[i*vp->v_out_size] );
			j += 2;
			i += n;
		}
		break;
	}
	case TRIANGLE_PRIMITIVE: {
		/* splitting may increase the number of triangle output
		 * worse case would result in 6 times more vertices */
		flush_var ( vert_out );
		vert_out = expand_var ( vert_out, 6*count );
		while ( j < count ) {
			int n = process_triangle ( &vert_in[j*vp->v_out_size],
						   &vp->t_view, &vp->t_proj, vp->cull_factor,
						   vp->v_out_off, vp->v_out_format, vp->n_out_comp,
						   vp->v_out_size, vp->clip_cache,
						   &vert_out[i*vp->v_out_size] );
			j += 3;
			i += n;
		}
		break;
	}
	}
	*vo = vert_out;
	return i;
}

void vertprocessor_symbol_lib ( void )
{
}
