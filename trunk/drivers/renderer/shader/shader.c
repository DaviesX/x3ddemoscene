/* shader.c: shader interfaces */
#include <math/math.h>
#include <system/log.h>
#include <system/allocator.h>
#include <renderer/shader.h>
#include <renderer/shader_lib.h>
#include "rtshader.h"


const static int SizeOfVarType[] = {
        [SHADER_VAR_POSITION] 	= sizeof(struct point4d),
        [SHADER_VAR_INSTANCEID]	= sizeof(uint32_t),
        [SHADER_VAR_VERTEXID]	= sizeof(uint32_t),
        [SHADER_VAR_FRAGCOORD]	= sizeof(struct point4d),
        [SHADER_VAR_NORMAL] 	= sizeof(struct vector4d),
        [SHADER_VAR_TANGENT] 	= sizeof(struct vector4d),
        [SHADER_VAR_BINORMAL] 	= sizeof(struct vector4d),
        [SHADER_VAR_MODELVIEW] 	= sizeof(struct matrix4x4)
};

const static int SizeOfUniType[] = {
        [SHADER_UNI_WORLD] 		= sizeof(struct matrix4x4),
        [SHADER_UNI_MODELVIEW] 		= sizeof(struct matrix4x4),
        [SHADER_UNI_DIFFUSEMAP]		= sizeof(void*),
        [SHADER_UNI_SPECULARMAP]	= sizeof(void*),
        [SHADER_UNI_NORMALMAP]		= sizeof(void*),
        [SHADER_UNI_OCCLUSIONMAP]	= sizeof(void*),
        [SHADER_UNI_ROUGHNESSMAP]	= sizeof(void*),
        [SHADER_UNI_WORLD_ARRAY]	= -1,
        [SHADER_UNI_MODELVIEW_ARRAY]	= -1
};

static struct shader_func *get_libfunc ( char *name );
static void builtin_finalize ( struct shader *shader );
static void opengl_finalize ( struct shader *shader );
static void alloc_for_so_lib ( struct shader_func *func );
static void free_for_so_lib ( struct shader_func *func );
static void free_shaderlib ( shaderlib_t lib );


static struct shader_func *g_libfunc;
static int g_nlibfunc = 0;

/* shader's */
void init_shader_library ( void )
{
        g_libfunc = alloc_var ( sizeof *g_libfunc, 0 );
        basic_rtshader_a_load ();
        basic_rtshader_so_load ();
}

void free_shader_library ( void )
{
        int i;
        for ( i = 0; i < g_nlibfunc; i ++ ) {
                free_shaderlib ( i );
        }
        free_var ( g_libfunc );
        g_nlibfunc = 0;
}

struct shader *create_shader ( int stage, bool is_dynamic, int spec )
{
        struct shader *shader = alloc_fix ( sizeof *shader, 1 );
        memset ( shader, 0, sizeof *shader );
        shader->stage = stage;
        shader->spec = spec;
        shader->is_dynamic = is_dynamic;
        shader->src_lib = alloc_var ( sizeof *shader->src_lib, 0 );
        shader->uni_buf = alloc_var ( sizeof *shader->uni_buf, 0 );
        int i;
        for ( i = 0; i < SHADER_MAX_VAR; i ++ ) {
                shader->in_var_loc[i] = -1;
                shader->out_var_loc[i] = -1;
        }
        for ( i = 0; i < SHADER_MAX_UNI; i ++ ) {
                shader->uniform_loc[i] = -1;
        }
        return shader;
}

void free_shader ( struct shader *shader )
{
        free_var ( shader->src_lib );
        free_var ( shader->uni_buf );
        memset ( shader, 0, sizeof *shader );
        free_fix ( shader );
}

void shader_add_function ( char *function, struct shader *shader )
{
        struct shader_func *lib = get_libfunc ( function );
        if ( !lib ) {
                log_critical_err_dbg ( "no such shader library function: %s", function );
                return ;
        }
        shader->src_lib = push_var ( &lib, shader->src_lib );
}

void shader_finalize ( struct shader *shader )
{
        switch ( shader->spec ) {
        case SHADER_SPEC_BUILTIN: {
                builtin_finalize ( shader );
                break;
        }
        case SHADER_SPEC_OPENGL: {
                opengl_finalize ( shader );
                break;
        }
        default: {
                log_critical_err_dbg ( "unknown specification" );
                return ;
        }
        }
}

static void builtin_finalize ( struct shader *shader )
{
        if ( shader->is_dynamic ) {
                /* dynamic shader */
        } else {
                /* static shader */
                if ( get_var_len ( shader->src_lib ) > 1 ) {
                        log_critical_err_dbg ( "must not have multiple library functions in a static shader" );
                        return ;
                } else if ( get_var_len ( shader->src_lib ) == 0 ) {
                        log_critical_err_dbg ( "no library function is added to this the currect shader" );
                        return ;
                }
                struct shader_func *curr = shader->src_lib[0];
                int k;
                for ( k = 0; k < SHADER_MAX_VAR; k ++ ) {
                        shader->in_var_loc[k] = curr->in_var_loc[k];
                        shader->in_var[k] = curr->in_var[k];
                        shader->out_var_loc[k] = curr->out_var_loc[k];
                        shader->out_var[k] = curr->out_var[k];
                }
                for ( k = 0; k < SHADER_MAX_UNI; k ++ ) {
                        shader->uniform_loc[k] = curr->uniform_loc[k];
                        shader->uni_addr[k] = curr->uni_addr[k];
                }
                shader->func = curr->func;
        }
}

static void opengl_finalize ( struct shader *shader )
{
        if ( !shader->is_dynamic ) {
                log_critical_err_dbg ( "opengl-spec shader has to be dynamic" );
                return ;
        }
        /* NOTE (davis#4#): <shader> add opengl translation */
}

void shader_run ( struct shader *shader )
{
        shader->func ();
}

void shader_set_uniform ( int type, void *value, int size, struct shader *shader )
{
        if ( !shader->uni_addr[type] ) {
                log_critical_err_dbg ( "type address is null, which suggests it should be the array type that must reside in the uniform buffer and be allocated on caller's respond" );
                return ;
        }
        memcpy ( shader->uni_addr[type], value, SizeOfUniType[type] );
}

int shader_get_invar_loc ( int type, struct shader *shader )
{
        return shader->in_var_loc[type];
}

int shader_get_outvar_loc ( int type, struct shader *shader )
{
        return shader->in_var_loc[type];
}

void shader_set_uniform_buffer ( struct uniform_buffer *uni, struct shader *shader )
{
        shader->uni_buf = push_var ( &uni, shader->uni_buf );
}

/* RI Interface */
void shader_set_invar ( int i, void *addr, struct shader *shader )
{
        *shader->in_var[i] = addr;
}

void shader_set_outvar ( int i, void *addr, struct shader *shader )
{
        *shader->out_var[i] = addr;
}

/* uniform's */
struct uniform_buffer *create_uniform_buffer ( void )
{
        return nullptr;
}

void free_uniform_buffer ( struct uniform_buffer *uni )
{
}

void uniform_buffer_add ( int type, int count, struct uniform_buffer *uni )
{
}

void init_uni_buffer_group ( struct uni_buffer_group *group )
{
}

void uniform_buffer_group ( struct uniform_buffer *uni, struct uni_buffer_group *group )
{
}

void uni_buffer_group_assign ( struct uni_buffer_group *group )
{
}

struct uniform_buffer *uni_buffer_group_get ( char *name, struct uni_buffer_group *group )
{
        return nullptr;
}

/* shader library's */
static struct shader_func *get_libfunc ( char *name )
{
        int i;
        for ( i = 0; i < get_var_len ( g_libfunc ); i ++ ) {
                if ( !strcmp ( name, g_libfunc[i].name ) ) {
                        return &g_libfunc[i];
                }
        }
        log_severe_err_dbg ( "shader library: %s not found", name );
        return nullptr;
}

shaderlib_t shaderlib_get ( char *name )
{
        int i;
        for ( i = 0; i < get_var_len ( g_libfunc ); i ++ ) {
                if ( !strcmp ( name, g_libfunc[i].name ) ) {
                        return i;
                }
        }
        log_severe_err_dbg ( "shader library: %s not found", name );
        return -1;
}

shaderlib_t shaderlib_create_from ( shaderlib_t lib0, char *name, int lib_form )
{
        /* initialize new library from the existed */
        g_libfunc = add_var ( g_libfunc, 1 );
        struct shader_func *func = &g_libfunc[g_nlibfunc];
        if ( func->lib_form == SHADER_FUNC_STATIC &&
             lib_form != SHADER_FUNC_STATIC ) {
                log_critical_err_dbg ( "lib0 is static shader library, but the created library is not static. creation failed." );
                return -1;
        }
        memcpy ( func, &g_libfunc[lib0], sizeof *func );

        func->name = alloc_fix ( 1, strlen ( name ) + 1 );
        strcpy ( func->name, name );

        func->id = g_nlibfunc;
        func->lib_form = lib_form;

        g_nlibfunc ++;
        return g_nlibfunc - 1;
}

static void free_for_so_lib ( struct shader_func *func )
{
}

static void free_shaderlib ( shaderlib_t lib )
{
        struct shader_func *func = &g_libfunc[lib];
        if ( (func->lib_form == SHADER_FUNC_DYNAMIC) |
             (func->lib_form == SHADER_FUNC_VARIADIC) ) {
                free_for_so_lib ( func );
        } else {
                /* do nothing */
        }
        free_fix ( func->name );
        memset ( func, 0, sizeof *func );
}

static void alloc_for_so_lib ( struct shader_func *func )
{
}

shaderlib_t shaderlib_load ( char *name, struct shader_func *func )
{
        g_libfunc = add_var ( g_libfunc, 1 );

        func->name = alloc_fix ( 1, strlen ( name ) + 1 );
        strcpy ( func->name, name );

        if ( (func->lib_form == SHADER_FUNC_DYNAMIC) |
             (func->lib_form == SHADER_FUNC_VARIADIC) ) {
                /* dynamic library */
                alloc_for_so_lib ( func );
        } else {
                /* static library */
                memcpy ( &g_libfunc[g_nlibfunc], func, sizeof *func );
        }
        func->id = g_nlibfunc;
        g_nlibfunc ++;
        return g_nlibfunc - 1;
}

void shaderlib_define_function ( shaderlib_t lib, char *func_name,
                                 struct shader_func *func )
{
        if ( func->lib_form != SHADER_FUNC_VARIADIC ) {
                log_severe_err_dbg ( "it has to be a variadic shader library to define function template" );
                return ;
        }
}

void shaderlib_set_array_size ( shaderlib_t lib, char *name, int count )
{
}
