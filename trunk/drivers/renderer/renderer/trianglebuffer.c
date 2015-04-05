/* trianglebuffer.c: manages an aggregate of triangles */
#include <allocator.h>
#include <logout.h>
#include <algorithm.h>
#include "trianglebuffer.h"



void tri_buf_init ( struct triangle_buffer* tbuffer )
{
        tbuffer->is_dirty = false;
        tbuffer->format   = TRIBUFFER_ATTR_NULL;
        tbuffer->acc_type = TRIBUFFER_ACC_LINEAR;
        tbuffer->acc      = nullptr;
        alg_init ( list, &tbuffer->indices, sizeof(int), 1 );
        alg_init ( list, &tbuffer->vertex, sizeof(struct point3d), 1 );
        alg_init ( list, &tbuffer->normal, sizeof(struct vector3d), 1 );
        alg_init ( list, &tbuffer->tangent, sizeof(struct vector3d), 1 );
        alg_init ( list, &tbuffer->mod_attri, sizeof(bool), 1 );
}

void tri_buf_free ( struct triangle_buffer* tbuffer )
{
        alg_free ( list, &tbuffer->indices );
        alg_free ( list, &tbuffer->vertex );
        alg_free ( list, &tbuffer->normal );
        alg_free ( list, &tbuffer->tangent );
        alg_free ( list, &tbuffer->mod_attri );
        zero_obj ( tbuffer );
}

void tri_buf_iter ( struct triangle_buffer* tbuffer, struct triangle_iter* iter )
{
        iter->src       = tbuffer;
        iter->attr_iter = alg_n ( list, &tbuffer->vertex );
        iter->tri_iter  = alg_n ( list, &tbuffer->indices );
}

void tri_buf_modify_attri ( struct triangle_buffer* tbuffer, enum TRIBUFFER_ATTR_IDR type,
                            struct triangle_iter* base,
                            int distance, void* data )
{
        struct alg_list* stream = nullptr;
        int size;
        switch ( type ) {
        case TRIBUFFER_ATTR_VERTEX: {
                stream = alg_array(list, &tbuffer->vertex);
                size   = sizeof(struct point3d);
                break;
        }
        case TRIBUFFER_ATTR_NORMAL: {
                stream = alg_array(list, &tbuffer->normal);
                size   = sizeof(struct vector3d);
                break;
        }
        case TRIBUFFER_ATTR_TANGENT: {
                stream = alg_array(list, &tbuffer->tangent);
                size   = sizeof(struct vector3d);
                break;
                default: {
                        log_severe_err_dbg ( "invalid buffer type as: %d", type );
                        return ;
                }
        }
        }
        int len = alg_n ( list, stream );
        int end = max(base->attr_iter + distance, len );
        alg_expand ( list, stream, end );
        alg_expand ( list, &tbuffer->mod_attri, end );
        memcpy ( alg_array(list, stream), data, size*distance );
        memset ( &tbuffer->mod_attri, 1, sizeof(bool)*distance );
}

void tri_buf_add_triangle ( struct triangle_buffer* tbuffer,
                            struct triangle_iter* base, int i0, int i1, int i2 )
{
}

void tri_buf_flush_triangle ( struct triangle_buffer* tbuffer )
{
}

void tri_buf_flush ( struct triangle_buffer* tbuffer )
{
}

void tri_buf_generate_vbo ( struct triangle_buffer* tbuffer )
{
}

void tri_buf_generate_accessor ( struct triangle_buffer* tbuffer, enum TRIBUFFER_ACC_IDR type )
{
}

void tri_buf_access ( struct triangle_buffer* tbuffer, void* data, struct triangle_iter* iter,
                      f_Access_Box acc_box, f_Access_Triangle acc_tri )
{
}

void* tri_buf_get_vbo ( struct triangle_buffer* tbuffer )
{
        return tbuffer->vbo;
}

int tri_iter_i ( struct triangle_iter* iter, int v )
{
        return -1;
}

void* tri_iter_access ( struct triangle_iter* iter, enum TRIBUFFER_ATTR_IDR type )
{
        return nullptr;
}
