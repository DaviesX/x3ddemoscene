#ifndef RASTERPIPELINE_H_INCLUDED
#define RASTERPIPELINE_H_INCLUDED

#include <x3d/renderable.h>
#include "fbo.h"

struct renderable;

struct raster_pipeline {
};

/*
 * functions' declaration
 */
void raster_radiance ( struct raster_pipeline* pipeline,
                       struct fbo* target,
                       struct renderable* rda, int n );



#endif // RASTERPIPELINE_H_INCLUDED
