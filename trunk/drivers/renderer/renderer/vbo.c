#include <logout.h>
#include <x3d/common.h>
#include "vbo.h"


static void* vbo_alloc_builtin ( struct vbo_alloc* alloc, void* ptr, enum VBO_ALLOC_IDR type, int size )
{
        switch ( type ) {
        case VBO_ALLOC_STATIC:
                if ( ptr != nullptr )
                        free_fix ( ptr );
                return alloc_fix ( size, 1 );
        case VBO_ALLOC_DYNAMIC:
try_dynamic:
                /* @fixme (davis#2#): <vbo> using pooled memory */
                if ( ptr != nullptr )
                        free_fix ( ptr );
                return alloc_fix ( size, 1 );
        default:
                log_mild_err_dbg ( "unable to identify allocation type as: %d", type );
                goto try_dynamic;
        }
}

static void vbo_free_builtin ( struct vbo_alloc* alloc, void* ptr, enum VBO_ALLOC_IDR type )
{
        switch ( type ) {
        case VBO_ALLOC_STATIC:
                free_fix ( ptr );
                break;
        case VBO_ALLOC_DYNAMIC:
                /* @fixme (davis#2#): <vbo> using pooled memory */
                free_fix ( ptr );
                break;
        }
}

struct vbo_alloc* vbo_alloc_create ( int est_static, int est_dynamic )
{
        struct vbo_alloc* va = alloc_fix ( sizeof *va, 1 );
        zero_obj ( va );
        return va;
}

void vbo_alloc_free ( struct vbo_alloc* alloc )
{
        zero_obj ( alloc );
        free_fix ( alloc );
}

void vbo_init ( struct vbo* vbo, struct vbo_alloc* alloc )
{
        vbo->alloc = alloc;
        vbo->ns = 0;
        vbo->stride = 0;
        int i;
        for ( i = 0; i < 10; i ++ )
                vbo->x86_vbo[i] = nullptr;
}

void vbo_free ( struct vbo* vbo )
{
        vbo_free_stream_builtin ( vbo );
        vbo->alloc = nullptr;
        vbo->stride = 0;
        int i;
        for ( i = 0; i < vbo->ns; i ++ )
                vbo->x86_vbo[i] = nullptr;
        vbo->ns = 0;
}

int vbo_n_stream ( struct vbo* vbo )
{
        return vbo->ns;
}

void* vbo_i_x86 ( struct vbo* vbo, int istream, int i )
{
        return (untyped*) vbo->x86_vbo[istream] + i*vbo->stride;
}

void vbo_create_stream_builtin ( struct vbo* vbo, enum VBO_ALLOC_IDR type,
                                 int nstream, int len )
{
        vbo->ns = nstream;
        vbo->type = type;
        vbo->size = len;
        int i;
        for ( i = 0; i < vbo->ns; i ++ ) {
                vbo->x86_vbo[i] =
                        vbo_alloc_builtin ( vbo->alloc, vbo->x86_vbo[i], type, len );
        }
}

void vbo_free_stream_builtin ( struct vbo* vbo )
{
        int i;
        for ( i = 0; i < vbo->ns; i ++ ) {
                vbo_free_builtin ( vbo->alloc, vbo->x86_vbo[i], vbo->type );
                vbo->x86_vbo[i] = nullptr;
        }
}

void vbo_write_stream_builtin ( struct vbo* vbo, int istream, int stride, void* data )
{
        vbo->stride = stride;
        memcpy ( vbo->x86_vbo[istream], data, vbo->size );
}
