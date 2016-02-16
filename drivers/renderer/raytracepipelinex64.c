#include "raytracepipelinex64.h"

/*
 * <raytrace_pipeline_x64> private
 */
static void __raytpipe_x64_free(struct raytrace_pipeline* self)
{
}

static void __raytpipe_x64_add_aos(struct raytrace_pipeline* self, struct array_of_streams* aos)
{
}

static void __raytpipe_x64_add_material_shader(struct raytrace_pipeline* self, struct shader_generator* shagen)
{
}

static void __raytpipe_x64_set_rendertarget(struct raytrace_pipeline* self, struct render_target* target)
{
}

static void __raytpipe_x64_set_sample_count(struct raytrace_pipeline* self, struct raytrace_sample_config* config)
{
}

static void __raytpipe_x64_run(struct raytrace_pipeline* self)
{
}

/*
 * <raytrace_pipeline_x64> public
 */
void raytpipe_x64_init(struct raytrace_pipeline_x64* self)
{
        self->_parent.raytpipe_add_aos = __raytpipe_x64_add_aos;
        self->_parent.raytpipe_add_material_shader = __raytpipe_x64_add_material_shader;
        self->_parent.raytpipe_free = __raytpipe_x64_free;
        self->_parent.raytpipe_run = __raytpipe_x64_run;
        self->_parent.raytpipe_set_rendertarget = __raytpipe_x64_set_rendertarget;
        self->_parent.raytpipe_set_sample_count = __raytpipe_x64_set_sample_count;
}