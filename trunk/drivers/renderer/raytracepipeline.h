#ifndef RAYTRACEPIPELINE_H_INCLUDED
#define RAYTRACEPIPELINE_H_INCLUDED

#include <x3d/renderable.h>
#include "fbo.h"

struct renderable;
struct pathtrace_pipeline;

/*
 * functions' declaration
 */
struct pathtrace_pipeline* create_pathtrace_pipeline ( void );
void free_pathtrace_pipeline ( struct pathtrace_pipeline* pipeline );
void pathtrace_radiance ( struct pathtrace_pipeline* pipeline,
                          struct fbo* target,
                          struct renderable* rda, int n );


#endif // RAYTRACEPIPELINE_H_INCLUDED
