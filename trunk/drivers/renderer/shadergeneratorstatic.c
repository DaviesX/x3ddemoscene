#include <stdbool.h>
#include "shadergenerator.h"
#include "shadergeneratorstatic.h"


typedef struct shader_generator_static  current_impl_t;

/*
 * <shader_generator_static> private
 */
static bool __shagen_static_add_funcall_to(struct shader_generator* self, const char* dest, 
                                           const char* funcall, const char* params[], int n_params)
{
        return true;
}

static void __shagen_static_free(struct shader_generator* self)
{
}

static bool __shagen_static_compile(struct shader_generator* self)
{
        return true;
}

static bool __shagen_static_link(struct shader_generator* self)
{
        return true;
}

static bool __shagen_has_input_variable(struct shader_generator* self, const char* var_name)
{
        return true;
}

static void __shagen_analyze_code_header(struct shader_generator_static* self, const char* code) 
{
}

/*
 * <shader_generator_static> public
 */
void shagen_static_init(struct shader_generator_static* self, const char* code)
{
        shagen_init(&self->_parent, code);
        self->_parent.shagen_add_funcall_to = __shagen_static_add_funcall_to;
        self->_parent.shagen_compile = __shagen_static_compile;
        self->_parent.shagen_free = __shagen_static_free;
        self->_parent.shagen_link = __shagen_static_link;
        self->_parent.shagen_has_input_variable = __shagen_has_input_variable;
        
        __shagen_analyze_code_header(self, code);
}