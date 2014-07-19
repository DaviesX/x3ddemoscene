#ifndef FBO_H_INCLUDED
#define FBO_H_INCLUDED

enum SURFACE_IDR {
        A
};

struct fbo {
};

/*
 * functions' declaration
 */
void fbo_init ( struct fbo* fbo, int w, int h, int stride, enum SURFACE_IDR type );
void fbo_free ( struct fbo* fbo );
void* fbo_memory ( struct fbo* fbo );
void fbo_dimension ( struct fbo* fbo, int* w, int* h, int* stride );


#endif // FBO_H_INCLUDED
