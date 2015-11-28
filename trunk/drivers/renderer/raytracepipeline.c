#include <x3d/renderenvconsts.h>
#include <system/log.h>
#include <system/allocator.h>
#include "shadergenerator.h"
#include "raytracepipeline.h"
#include "raytracepipelinex64.h"


/*
 * <raytrace_pipeline> public
 */
struct raytrace_pipeline* raytpipe_factory_new(enum RenderSpecType spec)
{
        switch (spec) {
        case RenderSpecSWBuiltin:
        case RenderSpecSWAdvBuiltin: {
                struct raytrace_pipeline_x64* obj = alloc_obj(obj);
                raytpipe_x64_init(obj);
                return &obj->_parent;
        }
        case RenderSpecHWOpenGL: {
                log_mild_err("OpenGl pipeline not supported yet");
                return nullptr;
        }
        }
}

void raytpipe_free(struct raytrace_pipeline* self)
{
        self->raytpipe_free(self);
        free_fix(self);
}

void raytpipe_add_aos(struct raytrace_pipeline* self, struct array_of_streams* aos)
{
        self->raytpipe_add_aos(self, aos);
}

void raytpipe_add_material_shader(struct raytrace_pipeline* self, struct shader_generator* shagen)
{
        self->raytpipe_add_material_shader(self, shagen);
}

void raytpipe_set_rendertarget(struct raytrace_pipeline* self, struct render_target* target)
{
        self->raytpipe_set_rendertarget(self, target);
}

void raytpipe_set_sample_count(struct raytrace_pipeline* self, struct raytrace_sample_config* config)
{
        self->raytpipe_set_sample_count(self, config);
}

void raytpipe_run(struct raytrace_pipeline* self)
{
        self->raytpipe_run(self);
}

/*
 * <raytrace_pipeline> impl
 */
void raytpipe_init(struct raytrace_pipeline* self)
{
}

