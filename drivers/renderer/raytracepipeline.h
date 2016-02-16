#ifndef RAYTRACEPIPELINE_H_INCLUDED
#define RAYTRACEPIPELINE_H_INCLUDED

#include <x3d/renderenvconsts.h>
#include "arrayofstreams.h"
#include "shadergenerator.h"
#include "rendertarget.h"


struct array_of_streams;
struct shader_generator;
struct render_target;

/*
 * <raytrace_sample_config> decl
 */
struct raytrace_sample_config {
        int     n_px_samples;
};

/*
 * <raytrace_pipeline> decl
 */
struct raytrace_pipeline {
        void (*raytpipe_free) (struct raytrace_pipeline* self);
        void (*raytpipe_add_aos) (struct raytrace_pipeline* self, struct array_of_streams* aos);
        void (*raytpipe_add_material_shader) (struct raytrace_pipeline* self, struct shader_generator* shagen);
        void (*raytpipe_set_rendertarget) (struct raytrace_pipeline* self, struct render_target* target);
        void (*raytpipe_set_sample_count) (struct raytrace_pipeline* self, struct raytrace_sample_config* config);
        void (*raytpipe_run) (struct raytrace_pipeline* self);
};

/*
 * <raytrace_pipeline> public
 */
struct raytrace_pipeline* raytpipe_factory_new(enum RenderSpecType spec);

void raytpipe_free(struct raytrace_pipeline* self);
void raytpipe_add_aos(struct raytrace_pipeline* self, struct array_of_streams* aos);
void raytpipe_add_material_shader(struct raytrace_pipeline* self, struct shader_generator* shagen);
void raytpipe_set_rendertarget(struct raytrace_pipeline* self, struct render_target* target);
void raytpipe_set_sample_count(struct raytrace_pipeline* self, struct raytrace_sample_config* config);
void raytpipe_run(struct raytrace_pipeline* self);

/*
 * <raytrace_pipeline> impl
 */
void raytpipe_init(struct raytrace_pipeline* self);


#endif // RAYTRACEPIPELINE_H_INCLUDED
