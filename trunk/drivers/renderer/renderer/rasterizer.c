/* rasterizer.c low-level fragment rasterization processes go here */
#include <x3d/common.h>
#include <math/math.h>
#include <renderer/shader.h>
#include <system/log.h>
#include <system/allocator.h>
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
        case RT_EQUALITY_TRUE: {
                return true;
        }
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
        scc->x = (int) ((ndc->x + 1.0f)*0.5f*(buf->buf_x - 1));
        scc->y = (int) ((1.0 - ndc->y)*0.5f*(buf->buf_y - 1));
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
        /* surface buffers */
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
                ctx->clip_cache = alloc_var_expand2 ( ctx->clip_cache, vformat->size*3*3 );
        }
        ctx->draw_mode = cont->draw_mode;
        ctx->shader = cont->shader[RT_FRAGMENT_SHADER];
}

static int clip_line ( untyped *v0, untyped *v1, struct intn_vformat *vformat )
{
        struct point4d *v[2] = {to_4d(v0), to_4d(v1)};
        untyped *vaddr[2]    = {v0, v1};
        /* trivial boundary test */
        if ( (v[0]->x < -1.0f && v[1]->x < -1.0f) ||
             (v[0]->x >  1.0f && v[1]->x >  1.0f) ||
             (v[0]->y < -1.0f && v[1]->y < -1.0f) ||
             (v[0]->y >  1.0f && v[1]->y >  1.0f) ) {
                /* trivial reject */
                return 0;
        }
        if ( v[0]->x >= -1.0f && v[0]->x <= 1.0f &&
             v[0]->y >= -1.0f && v[0]->y <= 1.0f &&
             v[1]->x >= -1.0f && v[1]->x <= 1.0f &&
             v[1]->y >= -1.0f && v[1]->y <= 1.0f ) {
                /* trivial accept */
                return 1;
        }
        /* clipping against x and y pair of line boundary respectively */
        float tmin = -FLOAT_INFINITE,
              tmax =  FLOAT_INFINITE;
        const float inv_dv[2] = {
                1.0f/(v[0]->x - v[1]->x),
                1.0f/(v[0]->y - v[1]->y)
        };
        int i;
        for ( i = 0; i < 2; i ++ ) {
                /* t = (V0 - K)/(V0 - V1) */
                float t0 = (v[0]->p[i] + 1.0f)*inv_dv[i];
                float t1 = (v[0]->p[i] - 1.0f)*inv_dv[i];
                if ( t0 > t1 ) {
                        swap ( t0, t1 );
                }
                tmin = max ( tmin, t0 );
                tmax = min ( tmax, t1 );
                if ( tmin > tmax ) {
                        /* reject */
                        return 0;
                }
        }
        /* lerp to the instersection. note that
         * the case where both tmin and tmax are not in [0,1]
         * has been rejected through trivial rejection;
         * v[i] is not going to be lerped with depth correction */
        const float inv_sw = 1.0f/v[0]->w;
        const float inv_ew = 1.0f/v[1]->w;
        if ( tmin > 0.0f && tmin < 1.0f ) {
                float inv_rw = lerp ( inv_sw, inv_ew, tmin );
                v[0]->x = lerp ( v[0]->x, v[1]->x, tmin );
                v[0]->y = lerp ( v[0]->y, v[1]->y, tmin );
                v[0]->w = 1.0f/inv_rw;
                lerp_attri_c ( &vformat->format[1], vformat->n_comp - 1,
                               &vformat->offset[1],
                               vaddr[0] + vformat->offset[0], inv_sw,
                               vaddr[1] + vformat->offset[0], inv_ew,
                               tmin, vaddr[0] + vformat->offset[0], inv_rw );
        }
        if ( tmax > 0.0f && tmax < 1.0f ) {
                float inv_rw = lerp ( inv_sw, inv_ew, tmax );
                v[1]->x = lerp ( v[0]->x, v[1]->x, tmax );
                v[1]->y = lerp ( v[0]->y, v[1]->y, tmax );
                v[1]->w = 1.0f/inv_rw;
                lerp_attri_c ( &vformat->format[1], vformat->n_comp - 1,
                               &vformat->offset[1],
                               vaddr[0] + vformat->offset[0], inv_sw,
                               vaddr[1] + vformat->offset[0], inv_ew,
                               tmax, vaddr[1] + vformat->offset[0], inv_rw );
        }
        return 1;
}

struct clip_cache {
        untyped *cache;
        int stride;
        int i_slot;
};

static void init_clip_cache ( void *mem, int stride, struct clip_cache *cache )
{
        cache->cache = mem;
        cache->stride = stride;
        cache->i_slot = 0;
}

static void *clip_cache_new ( struct clip_cache *cache )
{
        return cache->cache + cache->stride*cache->i_slot ++;
}

#define line(_i)                ((_i)/2)
#define is_in_region(_val, _i)  (((_i) & 1) ? (_val) <= 1.0f : (_val) >= -1.0f)
#define ref_val(_i)             (((_i) & 1) ? (1.0f) : (-1.0f))

static inline void clip_compute_intersection (
        const struct point4d *p0,
        const struct point4d *p1,
        struct point4d *p_out, const int axis,
        const struct intn_vformat *vformat )
{
        untyped *vaddr0     = (untyped *) p0;
        untyped *vaddr1     = (untyped *) p1;
        untyped *dest_vaddr = (untyped *) p_out;
        int bl = line ( axis );
        float t = (p0->p[bl] - ref_val(axis))/
                  (p0->p[bl] - p1->p[bl]);
        const float inv_sw = 1.0f/p0->w;
        const float inv_ew = 1.0f/p1->w;
        /* p(x, y) should be lerp without depth correction */
        float inv_rw = lerp ( inv_sw, inv_ew, t );
        p_out->x = lerp ( p0->x, p1->x, t );
        p_out->y = lerp ( p0->y, p1->y, t );
        p_out->w = 1.0f/inv_rw;
        lerp_attri_c ( &vformat->format[1], vformat->n_comp - 1, &vformat->offset[1],
                       vaddr0 + vformat->offset[0], inv_sw,
                       vaddr1 + vformat->offset[0], inv_ew,
                       t, dest_vaddr + vformat->offset[0], inv_rw );
}

static int clip_triangle ( untyped *v0, untyped *v1, untyped *v2,
                           struct intn_vformat *vformat,
                           untyped *clip_mem, untyped **ref )
{
        struct point4d *v[3] = {to_4d(v0), to_4d(v1), to_4d(v2)};
        /* trivial boundary test */
        if ( (v[0]->x < -1.0f && v[1]->x < -1.0f && v[2]->x < -1.0f) ||
             (v[0]->x >  1.0f && v[1]->x >  1.0f && v[2]->x >  1.0f) ||
             (v[0]->y < -1.0f && v[1]->y < -1.0f && v[2]->y < -1.0f) ||
             (v[0]->y >  1.0f && v[1]->y >  1.0f && v[2]->y >  1.0f) ) {
                /* trivial reject */
                return 0;
        }
        if ( v[0]->x >= -1.0f && v[0]->x <= 1.0f &&
             v[0]->y >= -1.0f && v[0]->y <= 1.0f &&
             v[1]->x >= -1.0f && v[1]->x <= 1.0f &&
             v[1]->y >= -1.0f && v[1]->y <= 1.0f &&
             v[2]->x >= -1.0f && v[2]->x <= 1.0f &&
             v[2]->y >= -1.0f && v[2]->y <= 1.0f ) {
                /* trivial accept */
                memcpy ( ref, v, 3*sizeof(untyped*) );
                return 1;
        }
        /* clip triangle */
        struct point4d *ring_ref[2][5 + 1] = {    /* pentagon in the worst case */
                [0][0] = to_4d(v0),
                [0][1] = to_4d(v1),
                [0][2] = to_4d(v2),
                [0][3] = to_4d(v0)
        };
        int r = 0;      /* for switching between the ring buffer */
        int n_in = 3;
        int n_out = 0;

        struct clip_cache cache;
        init_clip_cache ( clip_mem, vformat->size, &cache );

        /* clip against x, y pair of boundary lines */
        int i;
        for ( i = 0; i < 4; i ++ ) {
#define curr_input( _r )        (_r)
#define next_output( _r )       ((_r) = ((_r) + 1) & 1)
                struct point4d **in_ref  = &ring_ref[curr_input(r)][0];
                struct point4d **out_ref = &ring_ref[next_output(r)][0];

                int k;
                for ( k = 0; k < n_in; k ++ ) {
                        int bl = line ( i );     /* boundary line */
                        if ( is_in_region ( in_ref[k]->p[bl], i ) ) {
                                /* send it to output list */
                                out_ref[n_out ++] = in_ref[k];
                                if ( !is_in_region ( in_ref[k + 1]->p[bl], i ) ) {
                                        /* pull the next vertex back to the boundary line
                                         * if the next vertiex is out;
                                         * vertex position is not going to be lerped
                                         * with depth correction */
                                        struct point4d *new_p = clip_cache_new ( &cache );
                                        out_ref[n_out ++] = new_p;
                                        clip_compute_intersection (
                                                in_ref[k + 0], in_ref[k + 1], new_p,
                                                i, vformat );
                                }
                        } else {
                                /* do not send to output list */
                                /* do nothing... */
                                if ( is_in_region ( in_ref[k + 1]->p[bl], i ) ) {
                                        /* push the itself forward to the boundary line
                                         * if the next vertiex is in;
                                         * vertex position is not going to be lerped
                                         * with depth correction */
                                        struct point4d *new_p = clip_cache_new ( &cache );
                                        out_ref[n_out ++] = new_p;
                                        clip_compute_intersection (
                                                in_ref[k + 0], in_ref[k + 1], new_p,
                                                i, vformat );
                                }
                        }
                }
                n_in = n_out;
                /* wrap [n + 1] element back to [0] */
                out_ref[n_out ++] = out_ref[0];
                int a;
                for ( a = 0; a < n_out; a ++ ) {
                        print_vector4d ( out_ref[a] );
                }
                log_normal ( "" );
                n_out = 0;
        }
        int n_out_total = n_in;         /* total number of output vertices */
        if ( n_out_total == 0 ) {
                return 0;
        }
        assert ( n_out_total >= 3 );
        untyped **out_list = (untyped **) &ring_ref[curr_input(r)][0];
        int n, j;
        for ( n = 1, j = 0; n <= n_out_total - 2; n ++ ) {
                ref[j ++] = out_list[0];
                ref[j ++] = out_list[n];
                ref[j ++] = out_list[n + 1];
        }
        return n_out_total - 2;
}

#undef curr_input
#undef next_output
#undef line
#undef is_in_region
#undef ref_val

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
        const untyped *vaddr[2] = {v0, v1};	/* ndc */
        struct point4d t[2];
        struct point4d *fragc[2];	/* fragment coordinate */
        struct ipoint2d scc[2];		/* screen coordinate */
        int v;
        for ( v = 0; v < 2; v ++ ) {
                fragc[v] = &t[v]; /* to_4d(vaddr[v]); */
                ndc_to_scc ( to_4d(vaddr[v]), &scc[v], buf );
                ndc_to_fragc ( to_4d(vaddr[v]), fragc[v] );
        }
        float dydx = (fragc[1]->y - fragc[0]->y)/(fragc[1]->x - fragc[0]->x);
        if ( fabsf ( dydx ) <= 1.0f ) {
                /* go with x axis */
                int i;
                float j;
                int i_end;
                if ( scc[0].x > scc[1].x ) {
                        i = scc[1].x;
                        j = (float) scc[1].y;
                        i_end = scc[0].x;
                } else {
                        i = scc[0].x;
                        j = (float) scc[0].y;
                        i_end = scc[1].x;
                }
                float sdydx = (float) (scc[1].y - scc[0].y)/(scc[1].x - scc[0].x);

                float inv_dx = 1.0f/(fragc[1]->x - fragc[0]->x);
                float dw = fragc[1]->w - fragc[0]->w;
                float t = 0.0f;		/* parameter for interpolation */
                while ( i <= i_end ) {
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
                float i;
                int j;
                int j_end;
                if ( scc[0].y > scc[1].y ) {
                        i = (float) scc[1].x;
                        j = scc[1].y;
                        j_end = scc[0].y;
                } else {
                        i = (float) scc[0].x;
                        j = scc[0].y;
                        j_end = scc[1].y;
                }
                float sdxdy = (float) (scc[1].x - scc[0].x)/(scc[1].y - scc[0].y);

                float inv_dy = 1.0f/(fragc[1]->y - fragc[0]->y);
                float dw = fragc[1]->w - fragc[0]->w;
                float t = 0.0f;		/* parameter for interpolation */
                while ( j <= j_end ) {
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

static void draw_solid_triangle ( struct rasterizer *rt, untyped **ref, int offset )
{
}

static void draw_wireframe_triangle ( struct rasterizer *rt, untyped **ref, int offset )
{
        draw_line ( ref[offset + 0], ref[offset + 1], rt );
        draw_line ( ref[offset + 1], ref[offset + 2], rt );
        draw_line ( ref[offset + 2], ref[offset + 0], rt );
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
                        int n = clip_line ( v[0], v[1], &rt->vf );
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
                        untyped *ref[3*3];
                        int n = clip_triangle ( v[0], v[1], v[2], &rt->vf,
                                                rt->ctx.clip_cache, ref );
                        int j;
                        for ( j = 0; j < n; j ++ ) {
                                if ( rt->ctx.draw_mode == RT_SOLID_MODE ) {
                                        draw_solid_triangle ( rt, ref, j );
                                } else {
                                        draw_wireframe_triangle ( rt, ref, j*3 );
                                }
                        }
                        vert_list += 3*rt->vf.size;
                }
                break;
        }
        }
}

