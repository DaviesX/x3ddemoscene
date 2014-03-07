/* rasterization.c: all contextual settings and controlling procedures of the rasterization pipeline go here */
#include <algorithm.h>
#include <logout.h>
#include <memory.h>
#include "rasterization.h"
#include "vertex_processor.h"
#include "rasterizer.h"


struct rt_info {
        struct rtcontext rc;
        enum RT_PRIMITIVE_TYPE type;
};

static void *builtin_rt_run ( struct rt_info *info );
static void *opengl_rt_run ( struct rt_info *info );

/*
 * rtcontext's definition
 */
void init_rtcontext ( struct rtcontext *rc )
{
        memset ( rc, 0, sizeof *rc );
        rc->spec = RT_BUILTIN_SW;
        int i;
        for ( i = 0; i < RT_BUFFER_NUM; i ++ ) {
                rc->buffer_state[i] = 0X0;
        }
        rc->cull_state = CULL_IDR_NULL;
        rc->clip_method = CLIP_IDR_PRECISE;
        identity_matrix4x4 ( &rc->view );
        identity_matrix4x4 ( &rc->proj );
        rc->draw_mode = RT_WIREFRAME_MODE;
        rc->vpr = create_vertprocessor ();
        rc->rtr = create_rasterizer ();
}

void free_rtcontext ( struct rtcontext *rc )
{
        free_vertprocessor ( rc->vpr );
        free_rasterizer ( rc->rtr );
        memset ( rc, 0, sizeof *rc );
}

void reset_rtcontext ( struct rtcontext *rc )
{
        free_rtcontext ( rc );
        init_rtcontext ( rc );
}

void rtcontext_set_view ( struct matrix4x4 *t, struct rtcontext *rc )
{
        copy_matrix4x4 ( t, &rc->view );
}

void rtcontext_set_proj ( struct matrix4x4 *t, struct rtcontext *rc )
{
        copy_matrix4x4 ( t, &rc->proj );
}

void rtcontext_bind_vertex_buffer ( struct vertex_buffer *vb, struct rtcontext *rc )
{
        rc->vbuffer = vb;
}

void rtcontext_bind_index_buffer ( struct index_buffer *ib, struct rtcontext *rc )
{
        rc->ibuffer = ib;
}

void rtcontext_bind_color_buffer ( struct surface *cb, struct rtcontext *rc )
{
        rc->color_buffer = cb;
}

void rtcontext_bind_depth_buffer ( struct surface *db, struct rtcontext *rc )
{
        rc->depth_buffer = db;
}

void rtcontext_bind_stencil_buffer ( struct surface *sb, struct rtcontext *rc )
{
        rc->stencil_buffer = sb;
}

void rtcontext_cull ( enum RT_CULL_IDR idr, struct rtcontext *rc )
{
        rc->cull_state = idr;
}

void rtcontext_draw_mode ( enum RT_DRAWMODE_IDR draw_mode, struct rtcontext *rc )
{
        rc->draw_mode = draw_mode;
}

void rtcontext_set_primitive_type ( enum RT_PRIMITIVE_TYPE type, struct rtcontext *rc )
{
        rc->prim_type = type;
}

void rtcontext_set_buffer ( enum RT_BUFFER_IDR idr, enum RT_BUFFER_STATE state,
                            struct rtcontext *rc )
{
        rc->buffer_state[idr] = state;
}

void rtcontext_set_depth_func ( enum RT_EQUALITY func, struct rtcontext *rc )
{
        rc->depth_func = func;
}

void rtcontext_set_stencil_func ( enum RT_EQUALITY func, struct rtcontext *rc )
{
        rc->stencil_func = func;
}

void rtcontext_bind_shader ( struct shader *shader, enum RT_SHADER_STAGE stage,
                             struct rtcontext *rc )
{
        rc->shader[stage] = shader;
}

void rtcontext_set_spec ( enum RT_SPEC_IDR spec, struct rtcontext *rc )
{
        rc->spec = spec;
}

void rtcontext_finalize_pipeline ( struct rtcontext *rc )
{
        vertprocessor_finalize ( rc, rc->vpr );
        rasterizer_finalize ( rc, rc->rtr );
}

void rtcontext_bind_vertdefn ( struct vertdefn *defn, struct rtcontext *rc )
{
        memcpy ( &rc->vert_defn, defn, sizeof *defn );
}

void vertdefn_init ( struct vertdefn *defn )
{
        memset ( defn, 0, sizeof *defn );
}

void vertdefn_set ( int icomp, enum VERTEX_DEFN_FORMAT format,
                    struct vertdefn *defn )
{
        defn->format[icomp] = format;
}

void vertdefn_fix ( struct vertdefn *defn )
{
        defn->n_comp = 0;
        defn->vertsize = 0;
        int i;
        for ( i = 0; defn->format[i] != VERTEX_DEFN_NULL; i ++ ) {
                defn->n_comp ++;
                defn->offset[i] = defn->vertsize;
                defn->vertsize +=
                        SizeofVertDefn[vertdefn_mask(defn->format[i])];
        }
}

int vertdefn_query_size ( struct vertdefn *defn )
{
        return defn->vertsize;
}

/*
 * rasterization's definition
 */
void rasterization_run_pipeline ( struct rtcontext *rc, bool async )
{
        struct rt_info *info = alloc_fix ( sizeof *info, 1 );
        info->rc = *rc;
        info->type = rc->prim_type;
        switch ( rc->spec ) {
        case RT_BUILTIN_SW: {
                if ( !async ) {
                        builtin_rt_run ( info );
                } else {
                        /* TODO (davis#3#): <rasterization> implement after adding threading support */
                        log_critical_err_dbg ( "asynchronous rasterization not supported yet" );
                }
                break;
        }
        case RT_OPENGL_HW: {
                if ( !async ) {
                        opengl_rt_run ( info );
                } else {
                        /* TODO (davis#3#): <rasterization> implement after adding threading support */
                        log_critical_err_dbg ( "asynchronous rasterization not supported yet" );
                }
        }
        default: {
                log_mild_err_dbg ( "no such specification as: %d, rasterization will not continue", rc->spec );
                break;
        }
        }
}

#define VERT_CACHE_SIZE			10200

static void *builtin_rt_run ( struct rt_info *info )
{
        struct rtcontext *rc = &info->rc;
        enum RT_PRIMITIVE_TYPE type = info->type;
        uint32_t vert_size = vertdefn_query_size ( &rc->vert_defn );
        vertprocessor_reset_progress ( rc->vpr );
        rt_vertex *vert_list = alloc_fix ( vert_size, VERT_CACHE_SIZE );
        rt_vertex *tmp_vlist = vert_list;
        int i;
        do {
                int nvert = vertprocessor_run ( rc->vpr, type, VERT_CACHE_SIZE,
                                                vert_list );
                for ( i = 0; i < nvert; i += type*1 ) {
                        int a;
                        for ( a = 0; a < type; a ++ ) {
                                log_normal ( "%d", i + a );
                                print_vector4d ( vert_list + a*vert_size );
                        }
                        rasterizer_run ( rc->rtr, vert_list, type, 1 );
                        vert_list += type*vert_size;
                }
        } while ( i != 0 );
        free_fix ( tmp_vlist );
        free_fix ( info );
        return nullptr;
}

#undef rt_vertex

static void *opengl_rt_run ( struct rt_info *info )
{
        free_fix ( info );
        return nullptr;
}
