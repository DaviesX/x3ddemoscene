#include <stdarg.h>
#include <system/allocator.h>
#include <system/log.h>
#include "shadergenerator.h"
#include "shadergeneratorstatic.h"
#include "shadergeneratorx64.h"
#include "shadergeneratorogl.h"

struct shader_generator* shagen_fact_new(enum RenderSpecType spec, const char* code)
{
        switch (spec) {
        case RenderSpecSWBuiltin: {
                struct shader_generator_static* obj = alloc_obj(obj);
                shagen_static_init(obj, code);
                return &obj->_parent;
        }
        case RenderSpecSWAdvBuiltin: {
                struct shader_generator_x64* obj = alloc_obj(obj);
                shagen_x64_init(obj, code);
                return &obj->_parent;
        }
        case RenderSpecHWOpenGL: {
                struct shader_generator_ogl* obj = alloc_obj(obj);
                shagen_ogl_init(obj, code);
                return &obj->_parent;
        }
        default: {
                log_mild_err("Render spec %d not supported by shader generator", spec);
                return nullptr;
        }
        }
}

void shagen_init(struct shader_generator* self, const char* code)
{
        self->code = (char*) code;
}

void shagen_free(struct shader_generator* self)
{
        self->shagen_free(self);
        free_fix(self);
}

bool shagen_link(struct shader_generator* self)
{
        return self->shagen_link(self);
}

bool shagen_compile(struct shader_generator* self)
{
        return self->shagen_compile(self);
}

bool shagen_add_funcall_to(struct shader_generator* self, const char* dest, const char* funcall, ...)
{
        const char* paramlist[16];
        int n_params = 0;
        
        va_list vl;
        va_start(vl, funcall);
        while (true) {
                paramlist[n_params] = va_arg(vl, const char*);
                if (paramlist[n_params][0] == '\0') break;
                n_params ++;
        }
        va_end(vl);
        
        return self->shagen_add_funcall_to(self, dest, funcall, paramlist, n_params);
}

bool shagen_has_variable(struct shader_generator* self, const char* var_name)
{
        return self->shagen_has_input_variable(self, var_name);
}

const char* shagen_get_code(struct shader_generator* self)
{
        return self->code;
}