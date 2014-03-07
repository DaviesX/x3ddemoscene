#ifndef SHADER_LIB_H_INCLUDED
#define SHADER_LIB_H_INCLUDED

#include <x3d/common.h>

#define SHDLIB_MAX_VAR		32
#define SHDLIB_MAX_UNI		32

enum SHADER_LIB_FORM {
        SHADER_FUNC_STATIC,
        SHADER_FUNC_DYNAMIC,
        SHADER_FUNC_VARIADIC
};

typedef uint32_t shaderlib_t;

/*
 * structures
 */
struct shader_tmpl {
};

struct shader_func {
        char *name;
        int id;
        int lib_form;
        struct shader_tmpl tmpl; /* avaible when library form is variadic */
        f_Shader_Func func;	/* available when it is a static library function */

        int in_var_loc[SHDLIB_MAX_VAR];
        void **in_var[SHDLIB_MAX_VAR];
        int n_in;

        int out_var_loc[SHDLIB_MAX_VAR];
        void **out_var[SHDLIB_MAX_VAR];
        int n_out;

        int uniform_loc[SHDLIB_MAX_UNI];
        void *uni_addr[SHDLIB_MAX_UNI];
        int n_uniform;
        char *body_code;
};

/*
 * functions' declaration
 */
shaderlib_t shaderlib_get ( char *name );
shaderlib_t shaderlib_create_from ( shaderlib_t lib0, char *name, int lib_form );
void shaderlib_define_function ( shaderlib_t lib, char *func_name,
                                 struct shader_func *func );
void shaderlib_set_array_size ( shaderlib_t lib, char *name, int count );

/*
 * RI Interface
 */
shaderlib_t shaderlib_load ( char *name, struct shader_func *func );


#endif // SHADER_LIB_H_INCLUDED
