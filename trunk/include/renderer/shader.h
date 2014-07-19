#ifndef X3DSHADING_H_INCLUDED
#define X3DSHADING_H_INCLUDED


#include <x3d/common.h>

enum SHADER_STAGE {
        SHADER_STAGE_GEOMETRY,
        SHADER_STAGE_VERTEX,
        SHADER_STAGE_TESSELATE,
        SHADER_STAGE_FRAGMENT,
        SHADER_STAGE_EMITTER,
        SHADER_STAGE_BOUNCE,
        SHADER_STAGE_TRANSFER
};

enum SHADER_SPEC {
        SHADER_SPEC_BUILTIN,
        SHADER_SPEC_OPENGL
};

struct shader_func;
typedef void (*f_Shader_Func) ( void );

#define SHADER_MAX_VAR		32
#define SHADER_MAX_UNI		20

enum SHADER_VAR_TYPE {
        SHADER_VAR_POSITION,
        SHADER_VAR_INSTANCEID,
        SHADER_VAR_VERTEXID,
        SHADER_VAR_FRAGCOORD,
        SHADER_VAR_NORMAL,
        SHADER_VAR_TANGENT,
        SHADER_VAR_BINORMAL,
        SHADER_VAR_MODELVIEW
};

enum SHADER_UNIFORM_TYPE {
        SHADER_UNI_WORLD,
        SHADER_UNI_MODELVIEW,
        SHADER_UNI_DIFFUSEMAP,
        SHADER_UNI_SPECULARMAP,
        SHADER_UNI_NORMALMAP,
        SHADER_UNI_OCCLUSIONMAP,
        SHADER_UNI_ROUGHNESSMAP,
        SHADER_UNI_WORLD_ARRAY,
        SHADER_UNI_MODELVIEW_ARRAY
};

/*
 * structures
 */
struct uniform_buffer {
        bool finalized;
        int id;
        int loc[SHADER_MAX_UNI];
        int size[SHADER_MAX_UNI];
        void *buf_addr[SHADER_MAX_UNI];
};

struct uni_buffer_group {
        int curr_loc;
        struct uniform_buffer *uni_bufs;
        int n_buffer;
};

struct shader {
        f_Shader_Func func;
        int spec;
        int id;
        bool is_dynamic;
        bool is_finalized;
        int stage;

        struct shader_func **src_lib;

        int in_var_loc[SHADER_MAX_VAR];
        void **in_var[SHADER_MAX_VAR];
        int n_in;

        int out_var_loc[SHADER_MAX_VAR];
        void **out_var[SHADER_MAX_VAR];
        int n_out;

        struct uniform_buffer **uni_buf;
        int uniform_loc[SHADER_MAX_UNI];
        void *uni_addr[SHADER_MAX_UNI];
        int n_uniform;

        char *body_code;
};


/*
 * functions' declaration
 */
void init_shader_library ( void );
void free_shader_library ( void );
struct shader *create_shader ( int stage, bool is_dynamic, int spec );
void free_shader ( struct shader *shader );
void shader_add_function ( char *function, struct shader *shader );
void shader_finalize ( struct shader *shader );

void shader_set_uniform ( int type, void *value, int size, struct shader *shader );
int shader_get_invar_loc ( int type, struct shader *shader );
int shader_get_outvar_loc ( int type, struct shader *shader );
void shader_set_uniform_buffer ( struct uniform_buffer *uni, struct shader *shader );

struct uniform_buffer *create_uniform_buffer ( void );
void free_uniform_buffer ( struct uniform_buffer *uni );
void uniform_buffer_add ( int type, int count, struct uniform_buffer *uni );
void init_uni_buffer_group ( struct uni_buffer_group *group );
void uniform_buffer_group ( struct uniform_buffer *uni, struct uni_buffer_group *group );
void uni_buffer_group_assign ( struct uni_buffer_group *group );
struct uniform_buffer *uni_buffer_group_get ( char *name, struct uni_buffer_group *group );

/* RI Interface */
void shader_run ( struct shader *shader );
void shader_set_invar ( int i, void *addr, struct shader *shader );
void shader_set_outvar ( int i, void *addr, struct shader *shader );


#endif // X3DSHADING_H_INCLUDED
