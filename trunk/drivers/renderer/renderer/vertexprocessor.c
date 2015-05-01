/* vertprocessor.c: all vertex processing during rasterization go here */
#include <system/log.h>
#include <system/allocator.h>
#include <renderer/shader.h>
#include "rasterization.h"
#include "vibuffer.h"
#include "vertexprocessor.h"
#include "vertattri_lerp.h"
#include "dbg_vertex_processor.h"


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

void vertattri_set ( int i, enum VERTEX_ELM_FORMAT format, int n_elm,
                     struct vertattri *vattri )
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
        vbuffer->buffer = alloc_add_var ( vbuffer->buffer, vbuffer->attri.vertsize );
        vbuffer->curr = (untyped *) vbuffer->buffer + i;
        vbuffer->elm_cnt ++;
}

void ibuffer_add_index ( struct index_buffer *ibuffer )
{
        int i = ibuffer->curr - (untyped *) ibuffer->buffer;
        ibuffer->buffer = alloc_add_var ( ibuffer->buffer, SizeOfIndexF[ibuffer->format] );
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
        memset ( vp, 0, sizeof *vp );
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
        vertprocessor_reset_progress ( vp );
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
        case CULL_IDR_FRONTFACE: {
                vp->cull_factor = -1.0f;
                break;
        }
        }
        copy_matrix4x4 ( &cont->view, &vp->t_view );
        copy_matrix4x4 ( &cont->proj, &vp->t_proj );
        /* cache allocation */
        if ( !vp->cache ) {
                vp->cache = alloc_var ( 1, vp->n_total*vp->v_out_size );
        } else {
                vp->cache = expand2_var ( vp->cache, vp->n_total*vp->v_out_size );
        }
//	vp->clip_cache = alloc_var ( 1, 8*vp->v_out_size );
        /* set up vertex shader */
        vp->shader = cont->shader[RT_VERTEX_SHADER];
        shader_finalize ( vp->shader );
        /* TODO (davis#3#): generate vertex processor */
}

void vertprocessor_reset_progress ( struct vertprocessor *vp )
{
        vp->n_done = 0;
}

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
        /* NOTE (davis#4#): don't know if this would help, needs further profiling **/
#if 1
        /*	const bool tr_accept =
        		bc[0].x >= -1.0f && bc[1].x >= -1.0f &&
        		bc[0].x <= 1.0f && bc[1].x <= 1.0f &&
        		bc[0].y >= -1.0f && bc[1].y >= -1.0f &&
        		bc[0].y <= 1.0f && bc[1].y <= 1.0f &&
        		bc[0].z >= -1.0f && bc[1].z >= -1.0f &&
        		bc[0].z <= 1.0f && bc[1].z <= 1.0f; */
        const bool tr_accept =
                bc[0].z >= -1.0f && bc[1].z >= 1.0f;
        if ( tr_accept ) {
                /* trivial accept, when all points are inside of all planes */
                /* to ndc */
                div_w_point4d_p ( v_pos[0] );
                div_w_point4d_p ( v_pos[1] );
                memcpy ( vaddro[0], vaddr[0], vert_size );
                memcpy ( vaddro[1], vaddr[1], vert_size );
                return 2;
        }
#endif
        /* uses near plane clipping only
         * (P0 + V*t)/(P0w + Vw*t) = +1, and
         * (P0 + V*t)/(P0w + Vw*t) = -1
         * and when z = -1 */
        float t = -(v_pos[0]->w + v_pos[0]->z)/
                  ((v_pos[1]->z - v_pos[0]->z) + (v_pos[1]->w - v_pos[0]->w));
        /* checks that if p0 is inside */
        if ( bc[0].z >= -1.0f ) {
                memcpy ( vaddro[0], vaddr[0], vert_size );
                div_w_point4d_p ( to_4d(vaddro[0]) );
                /* interpolate p1 toward p0 */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             vaddr[0], vaddr[1], t, vaddro[1] );
                div_w_point4d_p ( to_4d(vaddro[1]) );
        } else {
                /* interpolate p0 toward p1 */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             vaddr[0], vaddr[1], t, vaddro[0] );
                div_w_point4d_p ( to_4d(vaddro[0]) );
                memcpy ( vaddro[1], vaddr[1], vert_size );
                div_w_point4d_p ( to_4d(vaddro[1]) );
        }
        return 2;
}

static int process_triangle ( rt_vertex *vi,
                              struct matrix4x4 *t_view, struct matrix4x4 *t_proj,
                              float cull_factor,
                              int *comp_offset, int *comp_format, int n_comp,
                              int vert_size, rt_vertex *vo )
{
        rt_vertex *vaddr[3];
        rt_vertex *vaddro[6];
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
        /* NOTE (davis#4#): don't know if this would help, needs further profiling **/
#if 1
        /*	const bool tr_accept =
        		bc[0].x >= -1.0f && bc[1].x >= -1.0f && bc[2].z >= -1.0f &&
        		bc[0].x <= 1.0f && bc[1].x <= 1.0f && bc[2].x <= 1.0f &&
        		bc[0].y >= -1.0f && bc[1].y >= -1.0f && bc[2].y >= -1.0f &&
        		bc[0].y <= 1.0f && bc[1].y <= 1.0f && bc[2].y <= 1.0f &&
        		bc[0].z >= -1.0f && bc[1].z >= -1.0f && bc[2].z >= -1.0f &&
        		bc[0].z <= 1.0f && bc[1].z <= 1.0f && bc[2].z <= 1.0f;*/
        const bool tr_accept =
                bc[0].z >= -1.0f && bc[1].z >= -1.0f && bc[2].z >= -1.0f;
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
#endif
        /* uses near plane clipping only
         * case 1: two in, one out, two triangles produced
         * case 2: one in, two out, one triangle produced */
        struct point4d *p_in[2];
        struct point4d *p_out[2];
        int j = 0, k = 0;
        for ( v = 0; v < 3; v ++ ) {
                if ( v_pos[v]->z >= -1.0f ) {
                        p_in[k] = v_pos[v];
                        k ++;
                } else {
                        p_out[j] = v_pos[v];
                        j ++;
                }
        }
        /* (P0 + V*t)/(P0w + Vw*t) = -1 and when z = -1 */
        if ( k == 2 ) {
                /* producing two triangles */
                for ( v = 3; v < 6; v ++ ) {
                        vaddro[v] = vo + v*vert_size;
                }
                float t0 = -(p_in[0]->w + p_in[0]->z)/
                           ((p_out[0]->z - p_in[0]->z) +
                            (p_out[0]->w - p_in[0]->w));
                float t1 = -(p_in[1]->w + p_in[1]->z)/
                           ((p_out[0]->z - p_in[1]->z) +
                            (p_out[0]->w - p_in[1]->w));
                /* p0, p0', p2 | p2 p0' p2' */
                /* p0 */
                memcpy ( vaddro[0], p_in[0], vert_size );
                div_w_point4d_p ( to_4d(vaddro[0]) );
                /* p0' */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             p_in[0], p_out[0], t0, vaddro[1] );
                div_w_point4d_p ( to_4d(vaddro[1]) );
                /* p2 */
                memcpy ( vaddro[2], p_in[1], vert_size );
                div_w_point4d_p ( to_4d(vaddro[2]) );
                /* next triangle */
                /* p2 */
                memcpy ( vaddro[3], vaddro[2], vert_size );
                /* p0' */
                memcpy ( vaddro[4], vaddro[1], vert_size );
                /* p2' */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             p_in[1], p_out[0], t1, vaddro[5] );
                div_w_point4d_p ( to_4d(vaddro[5]) );
                return 2*3;
        } else {
                /* producing one triangle (k = 1) */
                float t0 = -(p_in[0]->w + p_in[0]->z)/
                           ((p_out[0]->z - p_in[0]->z) +
                            (p_out[0]->w - p_in[0]->w));
                float t1 = -(p_in[0]->w + p_in[0]->z)/
                           ((p_out[1]->z - p_in[0]->z) +
                            (p_out[1]->w - p_in[0]->w));
                /* p0, p0', p2' */
                /* p0 */
                memcpy ( vaddro[0], p_in[0], vert_size );
                div_w_point4d_p ( to_4d(vaddro[0]) );
                /* p0' */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             p_in[0], p_out[0], t0, vaddro[1] );
                div_w_point4d_p ( to_4d(vaddro[1]) );
                /* p2' */
                lerp_attri ( comp_format, comp_offset, n_comp,
                             p_in[0], p_out[1], t1, vaddro[2] );
                div_w_point4d_p ( to_4d(vaddro[2]) );
                return 3;
        }
}

static uint32_t get_index ( int i, untyped *index_stream, int size )
{
        switch ( size ) {
        case 1: {
                return *((uint8_t *) index_stream + i);
        }
        case 2: {
                return *((uint16_t *) index_stream + i);
        }
        case 4: {
                return *((uint32_t *) index_stream + i);
        }
        default: {
                log_critical_err_dbg ( "no such index format whose size is: %d", size );
                return -1;
        }
        }
}

int vertprocessor_run ( struct vertprocessor *vp, enum RT_PRIMITIVE_TYPE prim_type,
                        int count, rt_vertex *vo )
{
        /* vertex shader stage */
        count = (prim_type == TRIANGLE_PRIMITIVE) ? (count/2) : (count);
        count = min ( count, vp->n_total - vp->n_done );
        count = count - count%prim_type;
        flush_var ( vp->cache );
        vp->cache = expand2_var ( vp->cache, count );
        untyped *vert_out = vp->cache;
        int i;
        for ( i = 0; i < count; i ++ ) {
                int curr_i = get_index ( i, vp->index_stream, vp->index_size );
                untyped *curr_v_pos = vp->vert_stream + curr_i*vp->v_in_size;
                int j;
                for ( j = 0; j < vp->n_in_attri; j ++ ) {
                        shader_set_invar ( j, curr_v_pos + vp->v_in_off[j], vp->shader );
                        shader_set_outvar ( j, vert_out + vp->v_out_off[j], vp->shader );
                }
                shader_run ( vp->shader );
                vert_out += vp->v_out_size;
        }
        vp->n_done += count;
        /* post vertex processing */
        untyped *vert_in = vp->cache;
        vert_out = vo;
        i = 0;
        int j = 0;
        switch ( prim_type ) {
        case POINT_PRIMITIVE: {
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
                 * worse case would result in 2 times more vertices */
                while ( j < count ) {
                        int n = process_triangle ( &vert_in[j*vp->v_out_size],
                                                   &vp->t_view, &vp->t_proj,
                                                   vp->cull_factor,
                                                   vp->v_out_off, vp->v_out_format,
                                                   vp->n_out_comp,
                                                   vp->v_out_size,
                                                   &vert_out[i*vp->v_out_size] );
                        j += 3;
                        i += n;
                }
                break;
        }
        }
        return i;
}

void vertprocessor_symbol_lib ( void )
{
        dbg_process_point = process_point;
        dbg_process_line = process_line;
        dbg_process_triangle = process_triangle;
}
