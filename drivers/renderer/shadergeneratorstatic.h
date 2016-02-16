#ifndef SHADERGENERATORSTATIC_H_INCLUDED
#define SHADERGENERATORSTATIC_H_INCLUDED

#include "shadergenerator.h"

/*
 * <shader_generator_static> decl
 */
struct shader_generator_static {
        struct shader_generator _parent;
};
/*
 * <shader_generator_static> public
 */
void shagen_static_init(struct shader_generator_static* self, const char* code);


#endif // SHADERGENERATORSTATIC_H_INCLUDED
