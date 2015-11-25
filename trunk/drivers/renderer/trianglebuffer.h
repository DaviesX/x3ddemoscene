#ifndef TRIANGLEBUFFER_H_INCLUDED
#define TRIANGLEBUFFER_H_INCLUDED

#include <container/arraylist.h>
#include <math/math.h>

enum TRIBUFFER_ATTR_IDR {
        TRIBUFFER_ATTR_NULL     = 0,
        TRIBUFFER_ATTR_VERTEX   = (1 << 0),
        TRIBUFFER_ATTR_NORMAL   = (1 << 1),
        TRIBUFFER_ATTR_TANGENT  = (1 << 2)
};

enum TRIBUFFER_ACC_IDR {
        TRIBUFFER_ACC_LINEAR,
        TRIBUFFER_ACC_GRID,
        TRIBUFFER_ACC_BVH
};

struct vbo;
struct tri_accessor;

struct triangle_buffer {
        bool                            is_dirty;
        enum TRIBUFFER_ATTR_IDR         format;
        enum TRIBUFFER_ACC_IDR          acc_type;
        d_alg_list(int)                 indices;
        d_alg_list(point3d)             vertex;
        d_alg_list(vector3d)            normal;
        d_alg_list(vector3d)            tangent;
        d_alg_list(bool)                mod_attri;
        struct vbo*                     vbo;
        struct tri_accessor*            acc;
};

struct triangle_iter {
        int                     tri_iter;
        int                     attr_iter;
        struct triangle_buffer* src;
};


/*
 * functions' declaration
 */
typedef bool (*f_Access_Box) ( void* data, struct box3d* bound );
typedef bool (*f_Access_Triangle) ( void* data, struct triangle_iter* iter );

void tri_buf_init ( struct triangle_buffer* tbuffer );
void tri_buf_free ( struct triangle_buffer* tbuffer );
void tri_buf_iter ( struct triangle_buffer* tbuffer, struct triangle_iter* iter );
void tri_buf_modify_attri ( struct triangle_buffer* tbuffer, enum TRIBUFFER_ATTR_IDR type,
                            struct triangle_iter* base,
                            int distance, void* data );
void tri_buf_add_triangle ( struct triangle_buffer* tbuffer,
                            struct triangle_iter* base, int i0, int i1, int i2 );
void tri_buf_flush_triangle ( struct triangle_buffer* tbuffer );
void tri_buf_flush ( struct triangle_buffer* tbuffer );

void tri_buf_generate_vbo ( struct triangle_buffer* tbuffer );
void tri_buf_generate_accessor ( struct triangle_buffer* tbuffer, enum TRIBUFFER_ACC_IDR type );
void tri_buf_access ( struct triangle_buffer* tbuffer, void* data, struct triangle_iter* iter,
                      f_Access_Box acc_box, f_Access_Triangle acc_tri );
void* tri_buf_get_vbo ( struct triangle_buffer* tbuffer );

int tri_iter_i ( struct triangle_iter* iter, int v );
void* tri_iter_access ( struct triangle_iter* iter, enum TRIBUFFER_ATTR_IDR type );


#endif // TRIANGLEBUFFER_H_INCLUDED
