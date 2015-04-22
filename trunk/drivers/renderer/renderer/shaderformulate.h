#ifndef SHADERFORMULATE_H_INCLUDED
#define SHADERFORMULATE_H_INCLUDED


#include <container/hashmap.h>
#include <x3d/common.h>


struct symbol_set;

enum ATTRI_IDR {
        ATTRI_NULL,
        ATTRI_INT,
        ATTRI_FLOAT,
        ATTRI_FLOAT2,
        ATTRI_FLOAT3,
        ATTRI_FLOAT4
};

typedef void (*f_Attri_Lerp) ( void* v0, void* v1, float t, void* r );
typedef void (*f_Attri_TLerp) ( void* v0, void* v1, void* v2,
                                float b0, float b1, float b2, void* r );

struct shader_attribute {
        int             offset[10];
        void*           src;
        int             dloc[10];
//        void**          dest[10];
        int             n_comp;
        int             size;
        f_Attri_Lerp    lerp_op;
        f_Attri_TLerp   tlerp_op;
        enum ATTRI_IDR  format[10];
};

/*
 * functions' declaration
 */
void attr_init ( struct shader_attribute* attr );
void attr_set_i ( struct shader_attribute* attr, int i, enum ATTRI_IDR type );
void attr_finalize ( struct shader_attribute* attr );
int attr_size ( struct shader_attribute* attr );
int attr_n ( struct shader_attribute* attr );
void attr_bind_dest ( struct shader_attribute* attr, int i, int dloc );
void attr_bind_src ( struct shader_attribute* attr, void* src );
void attr_link_individual ( struct shader_attribute* attr );
void attr_link_block ( struct shader_attribute* attr );
void attr_set_lerp_operator ( struct shader_attribute* attr, f_Attri_Lerp op, f_Attri_TLerp op2 );
void attr_op_lerp ( struct shader_attribute* attr, void* v0, void* v1, float t, void* r );
void attr_op_trilerp ( struct shader_attribute* attr, void* v0, void* v1, void* v2,
                       float b0, float b1, float b2, void* r );


enum SHADER_C_IDR {
        SHADER_C_FAKE,
        SHADER_C_X86,
        SHADER_C_OPENGL
};

enum {  /* builtin inputs */
        SHADER_INPUT_FRAGCOOR,
        SHADER_INPUT_UV,
};

enum {  /* builtin outputs */
        SHADER_OUTPUT_VERTEX,
        SHADER_OUTPUT_RADIANCE
};

struct shader_dict {
        char*   var_name;
        char*   type;
        int     loc;
};

struct shader_form {
        char*                   name;
        enum SHADER_C_IDR       c_type;
        bool                    is_compiled;
        bool                    is_linked;
        struct symbol_set*      dl;
        f_Generic               func;
        char*                   code;
        struct shader_attribute attri[10];
        d_alg_list(void*)       loc[3];
        struct shader_attribute out_attri;
        int                     alloc[3];
        d_alg_list(shader_dict) mapper[3];
        d_alg_hash_llist(shader_dict) dict[3];
};

/*
 * functions' declaration
 */
void shaform_init ( struct shader_form* shader, char* name, enum SHADER_C_IDR compiler, struct symbol_set* dl );
void shaform_free ( struct shader_form* shader );
void shaform_bind_code ( struct shader_form* shader, f_Generic func, const char* code );
bool shaform_compile ( struct shader_form* shader );
int shaform_uniform_id ( struct shader_form* shader, char* var );
int shaform_input_id ( struct shader_form* shader, char* var );
int shaform_output_id ( struct shader_form* shader, char* var );

void shaform_bind_shader_attribute ( struct shader_form* shader, int istream,
                                     struct shader_attribute* attr );

void shaform_input_addr_x86 ( struct shader_form* shader, int loc, void* addr );
void shaform_output_addr_x86 ( struct shader_form* shader, int loc, void* addr );
void shaform_input_buffer_x86 ( struct shader_form* shader, int istream, void* buffer );
void shaform_output_buffer_x86 ( struct shader_form* shader, void* buffer );
bool shaform_link ( int n_stages, ... );

f_Generic shaform_exe_x86 ( struct shader_form* shader );
bool shaform_is_compiled ( struct shader_form* shader );
bool shaform_is_linked ( struct shader_form* shader );

/* typedefs */
typedef void (*f_Emitter) ( void );
typedef bool (*f_Bounce) ( void );
typedef void (*f_Transfer) ( void );


#endif // SHADERFORMULATE_H_INCLUDED
