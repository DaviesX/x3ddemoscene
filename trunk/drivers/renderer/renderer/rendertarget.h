#ifndef RENDERTARGET_H_INCLUDED
#define RENDERTARGET_H_INCLUDED


#include <algorithm.h>

struct fbo;

struct render_target {
        uuid_t          id;
        struct fbo*     fbo;
};

/*
 * functions' declaration
 */
void rendt_init ( struct render_target* target );
void rendt_free ( struct render_target* target );


#endif // RENDERTARGET_H_INCLUDED
