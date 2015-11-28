#ifndef SHADERGENERATOROGL_H_INCLUDED
#define SHADERGENERATOROGL_H_INCLUDED

#include "shadergenerator.h"

/*
 * <shader_generator_ogl> decl
 */
struct shader_generator_ogl {
        struct shader_generator _parent;
};
/*
 * <shader_generator_ogl> public
 */
void shagen_ogl_init(struct shader_generator_ogl* self, const char* code);

        
#endif  // SHADERGENERATOROGL_H_INCLUDED