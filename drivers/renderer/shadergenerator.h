#ifndef SHADERGENERATOR_H_INCLUDED
#define SHADERGENERATOR_H_INCLUDED

#include <stdbool.h>
#include <x3d/renderenvconsts.h>

struct shader_generator;

/*
 * <shader_generator> decl
 */
struct shader_generator {
    void (*shagen_free) (struct shader_generator* self);
    bool (*shagen_link) (struct shader_generator* self);
    bool (*shagen_compile) (struct shader_generator* self);
    bool (*shagen_add_funcall_to) (struct shader_generator* self, const char* dest, 
                                   const char* funcall, const char* params[], int n_params);
    bool (*shagen_has_input_variable) (struct shader_generator* self, const char* var_name);
    char*       code;
};

/*
 * <shader_generator> public
 */
struct shader_generator* shagen_fact_new(enum RenderSpecType spec, const char* code);
void shagen_init(struct shader_generator* self, const char* code);
void shagen_free(struct shader_generator* self);
bool shagen_link(struct shader_generator* self);
bool shagen_compile(struct shader_generator* self);
bool shagen_add_funcall_to(struct shader_generator* self, const char* dest, const char* funcall, ...);
bool shagen_has_input_variable(struct shader_generator* self, const char* var_name);

/*
 * <shader_generator> impl
 */
const char* shagen_get_code(struct shader_generator* self);


#endif // SHADERGENERATOR_H_INCLUDED