#ifndef RAYTRACEPIPELINEX64_H_INCLUDED
#define RAYTRACEPIPELINEX64_H_INCLUDED

#include "raytracepipeline.h"

/*
 * <raytrace_pipeline_x64> decl
 */
struct raytrace_pipeline_x64 {
        struct raytrace_pipeline        _parent;
};
/*
 * <raytrace_pipeline_x64> public
 */
void raytpipe_x64_init(struct raytrace_pipeline_x64* self);


#endif  // RAYTRACEPIPELINEX64_H_INCLUDED