#ifndef VBO_H_INCLUDED
#define VBO_H_INCLUDED

enum VBO_ALLOC_IDR {
        VBO_ALLOC_STATIC,
        VBO_ALLOC_DYNAMIC
};

struct vbo_alloc {
};

struct vbo {
        struct vbo_alloc*       alloc;
        enum VBO_ALLOC_IDR      type;
        int                     ns;
        int                     size;
        int                     stride;
        void*                   x86_vbo[10];
};

/*
 * functions' declaration
 */
struct vbo_alloc* vbo_alloc_create ( int est_static, int est_dynamic );
void vbo_alloc_free ( struct vbo_alloc* alloc );

void vbo_init ( struct vbo* vbo, struct vbo_alloc* alloc );
void vbo_free ( struct vbo* vbo );
int vbo_n_stream ( struct vbo* vbo );
void* vbo_i_x86 ( struct vbo* vbo, int istream, int i );

typedef void (*f_Vbo_Create_Stream) ( struct vbo* vbo, enum VBO_ALLOC_IDR type,
                                      int nstream, int len );
typedef void (*f_Vbo_Free_Stream) ( struct vbo* vbo );
typedef void (*f_Write_Stream) ( struct vbo* vbo, int istream, int stride, void* data );
void vbo_create_stream_builtin ( struct vbo* vbo, enum VBO_ALLOC_IDR type,
                                 int nstream, int len );
void vbo_free_stream_builtin ( struct vbo* vbo );
void vbo_write_stream_builtin ( struct vbo* vbo, int istream, int stride, void* data );


#endif // VBO_H_INCLUDED
