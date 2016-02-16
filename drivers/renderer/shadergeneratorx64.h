#ifndef SHADERGENERATORX64_H_INCLUDED
#define SHADERGENERATORX64_H_INCLUDED

#include "shadergenerator.h"

/*
 * <shader_generator_x64> decl
 */
struct shader_generator_x64 {
        struct shader_generator _parent;
};
/*
 * <shader_generator_x64> public
 */
void shagen_x64_init(struct shader_generator_x64* self, const char* code);


#endif // SHADERGENERATORX64_H_INCLUDED