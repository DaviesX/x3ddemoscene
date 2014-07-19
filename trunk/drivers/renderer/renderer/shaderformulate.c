/* shaderformulate.c: formulate a runnable shader from source code/function pointer */
#include <algorithm.h>
#include <math/math.h>
#include <memory.h>
#include <logout.h>
#include <symlib.h>
#include <x3d/common.h>
#include "shaderformulate.h"



static const int cSize[] = {
        [ATTRI_NULL]   = 0,
        [ATTRI_INT]    = sizeof(int),
        [ATTRI_FLOAT]  = sizeof(float),
        [ATTRI_FLOAT2] = sizeof(float)*2,
        [ATTRI_FLOAT3] = sizeof(float)*3,
        [ATTRI_FLOAT4] = sizeof(float)*4
};

void attr_init ( struct shader_attribute* attr )
{
        zero_obj ( attr );
}

void attr_set_i ( struct shader_attribute* attr, int i, enum ATTRI_IDR type )
{
        attr->n_comp = max(attr->n_comp, i);
        attr->offset[i] = cSize[type];
}

void attr_finalize ( struct shader_attribute* attr )
{
        attr->size = 0;
        int i;
        for ( i = 0; i < attr->n_comp; i ++ ) {
                int tsize = attr->offset[i];
                attr->offset[i] = attr->size;
                attr->size += tsize;
        }
}

int attr_size ( struct shader_attribute* attr )
{
        return attr->size;
}

int attr_n ( struct shader_attribute* attr )
{
        return attr->n_comp;
}

void attr_bind_dest ( struct shader_attribute* attr, int i, int dloc )
{
        attr->dloc[i] = dloc;
}

void attr_bind_src ( struct shader_attribute* attr, void* src )
{
        attr->src = src;
}
/*
void attr_link_individual ( struct shader_attribute* attr )
{
        int i;
        for ( i = 0; i < attr->n_comp; i ++ )
                *attr->dest[i] = ((untyped*) attr->src) + attr->offset[i];
}

void attr_link_block ( struct shader_attribute* attr )
{
        *attr->dest[0] = attr->src;
}
*/
void attr_set_lerp_operator ( struct shader_attribute* attr, f_Attri_Lerp op, f_Attri_TLerp op2 )
{
        attr->lerp_op = op;
        attr->tlerp_op = op2;
}

void attr_op_lerp ( struct shader_attribute* attr, void* v0, void* v1, float t, void* r )
{
        attr->lerp_op ( v0, v1, t, r );
}

void attr_op_trilerp ( struct shader_attribute* attr, void* v0, void* v1, void* v2,
                       float b0, float b1, float b2, void* r )
{
        attr->tlerp_op ( v0, v1, v2, b0, b1, b2, r );
}

void attr_auto_lerp ( struct shader_attribute* attr, void* v0, void* v1, float t, void* r )
{
        int i;
        for ( i = 0; i < attr->n_comp; i ++ ) {
                switch ( attr->format[i] ) {
                case ATTRI_INT:
                        *(int*) r = *(int*) v0 + (*(int*) v1 - *(int*) v0)*t;
                        break;
                case ATTRI_FLOAT:
                        *(float*) r = *(float*) v0 + (*(float*) v1 - *(float*) v0)*t;
                        break;
                default:
                        return ;
                }
        }
}

void shaform_init ( struct shader_form* shader, char* name, enum SHADER_C_IDR compiler, struct symbol_set* dl )
{
        zero_obj ( shader );
        shader->name    = alg_alloc_string ( name );
        shader->c_type  = compiler;
        shader->dl      = dl;
        int i;
        for ( i = 0; i < 3; i ++ ) {
                alg_init ( list, &shader->loc[i], sizeof(address_t), 1 );
                alg_init ( list, &shader->mapper[i], sizeof(struct shader_dict), 1 );
//        alg_init(hash_llist, &shader->dict[i], sizeof(struct shader_dict), 1 );
        }
}

void shaform_free ( struct shader_form* shader )
{
        free_fix ( shader->name );
        int i;
        for ( i = 0; i < 3; i ++ ) {
                struct shader_dict* mapper = alg_array(list, &shader->mapper[i]);
                int j;
                for ( j = 0; j < alg_n(list, &shader->mapper[i]); j ++ ) {
                        free_fix ( mapper[j].var_name );
                        free_fix ( mapper[j].type );
                }
                alg_free ( list, &shader->loc[i] );
                alg_free ( list, &shader->mapper[i] );
                alg_free ( hash_llist, &shader->dict[i] );
        }
        zero_obj ( shader );
}

void shaform_bind_code ( struct shader_form* shader, f_Generic func, const char* code )
{
        shader->func = func;
        shader->code = (char*) code;
}

enum {
        FIND_VAR_NORMAL,
        FIND_VAR_ERROR,
        FIND_VAR_DONE
};

static int find_var_name ( char* qualifier, char* name, char* tname, char** pos )
{
        if ( !(*pos) ) {
                log_severe_err ( "no shader code was found" );
                return FIND_VAR_ERROR;
        }
        char* code = *pos;
        char* marker;
        declare_stack ( stack, 20*sizeof(int) );
        int state;
reset:
        marker = *pos;
        state = 0;
        init_stack ( &stack );

        while ( *code != '\0' ) {
                switch ( state ) {
                case 0: /* to eliminate redundant characters */
                        if ( *code == ' ' ) {
                                code ++;
                        } else {
                                if ( stack_is_empty ( &stack ) )
                                        state = 1;
                                else
                                        pop_stack ( &stack, state );
                        }
                        break;
                case 1: /* to match qualifier */
                        if ( !strncmp ( code, qualifier, strlen(qualifier) ) ) {
                                state = 2;
                                push_stack ( &stack, state );
                                state = 0;

                                code += strlen ( qualifier );
                        } else {
                                code ++;
                                goto reset;
                        }
                        break;
                case 2: /* to identity typename alphabetical initial */
                        if ( isalpha(code) ) {
                                marker = code;
                                state = 3;
                        } else {
                                code ++;
                                goto reset;
                        }
                        break;
                case 3: /* to identify type name */
                        if ( *code == ';' ) {
                                code ++;
                                goto reset;
                        }

                        if ( *code == ' ' ) {
                                strncpy ( tname, marker, code - marker );

                                state = 4;
                                push_stack ( &stack, state );
                                state = 0;
                        } else
                                code ++;
                        break;
                case 4: /* to identity variable name alphabetical initial */
                        if ( isalpha(code) ) {
                                marker = code ++;
                                state = 5;
                        } else {
                                code ++;
                                goto reset;
                        }
                        break;
                case 5: /* to capture variable name */
                        if ( isalpha(code) || isdigit(code) )
                                code ++;
                        else {
                                strncpy ( name, marker, code - marker );
                                goto end;
                        }
                        break;
                }
        }
end:
        *pos = code;
        if ( *code == '\0' )
                return FIND_VAR_DONE;
        else
                return FIND_VAR_NORMAL;
}

enum {
        ITEM_INVAR,
        ITEM_OUTVAR,
        ITEM_UNIFORM
};

bool shaform_compile ( struct shader_form* shader )
{
        if ( shader->func == nullptr ) {
                log_normal_dbg ( "skip compiling shader: %s. because the shader has been statically compiled",
                                 shader->name );

                char* pos = shader->code;
                char* qualifier[3] = {
                        "invar",
                        "outvar",
                        "uniform"
                };
                char name[64];
                char tname[64];
                int i;  /* each item: invar, outvar, uniform */
                for ( i = 0; i < 3; i ++ ) {
                        while ( true ) {
                                int err = find_var_name ( qualifier[i], name, tname, &pos );
                                if ( FIND_VAR_ERROR == err )
                                        goto failed;
                                else if ( FIND_VAR_DONE == err )
                                        break;

                                struct shader_dict var_info;
                                var_info.var_name = alg_alloc_string ( name );
                                var_info.type     = alg_alloc_string ( tname );
                                var_info.loc      = -1;
                                alg_push_back ( list, &shader->mapper[i], &var_info );
                        }
                }
        } else {
                log_normal_dbg ( "dynamically compiling shader: %s", shader->name );
                log_severe_err_dbg ( "dynamic shader has not been supported" );
                goto failed;
        }
        shader->is_compiled = true;
        return true;
failed:
        log_severe_err ( "failed to compile shader %s",shader->name );
        return false;
}

static struct shader_dict* find_shader_dict ( struct shader_form* shader,
                int item, char* name )
{
        struct shader_dict* c = alg_array( list, &shader->mapper[item] );
        int i;
        for ( i = 0;
              i < alg_n(list, &shader->mapper[item]);
              i ++ ) {
                if ( !strcmp ( name, c[i].var_name ) )
                        return &c[i];
        }
        return nullptr;
}

int shaform_uniform_id ( struct shader_form* shader, char* var )
{
        struct shader_dict* c = find_shader_dict ( shader, ITEM_UNIFORM, var );
        return c->loc;
}

int shaform_input_id ( struct shader_form* shader, char* var )
{
        struct shader_dict* c = find_shader_dict ( shader, ITEM_INVAR, var );
        return c->loc;
}

int shaform_output_id ( struct shader_form* shader, char* var )
{
        struct shader_dict* c = find_shader_dict ( shader, ITEM_OUTVAR, var );
        return c->loc;
}

void shaform_bind_shader_attribute ( struct shader_form* shader, int istream,
                                     struct shader_attribute* attr )
{
        shader->attri[istream] = *attr;
}

void shaform_input_addr_x86 ( struct shader_form* shader, int i, void* addr )
{
        void** loc = alg_array(list, &shader->loc[ITEM_INVAR]);
        void** dest = loc[i];
        *dest = addr;
}

void shaform_output_addr_x86 ( struct shader_form* shader, int i, void* addr )
{
        void** loc = alg_array(list, &shader->loc[ITEM_OUTVAR]);
        void** dest = loc[i];
        *dest = addr;
}

void shaform_input_buffer_x86 ( struct shader_form* shader, int istream, void* buffer )
{
        untyped* buf = buffer;
        struct shader_attribute* attri = &shader->attri[istream];
        void** loc = alg_array(list, &shader->loc[ITEM_INVAR] );
        int i;
        for ( i = 0; i < attri->n_comp; i ++ ) {
                int iloc = attri->dloc[i];
                void** dest = loc[iloc];
                *dest = (void*) (buf + attri->offset[i]);
        }
}

void shaform_output_buffer_x86 ( struct shader_form* shader, void* buffer )
{
        untyped* buf = buffer;
        struct shader_attribute* attri = &shader->out_attri;
        void** loc = alg_array(list, &shader->loc[ITEM_OUTVAR] );
        int i;
        for ( i = 0; i < attri->n_comp; i ++ ) {
                int iloc = attri->dloc[i];
                void** dest = loc[iloc];
                *dest = (void*) (buf + attri->offset[i]);
        }
}

static const char* cDefault[] = {
        "sha_i_emit_uv",
        "sha_i_emit_ray",
        "sha_o_emit_ray"
};
static const int cNumDefault = 3;

static int default_var_loc ( char* name )
{
        int i;
        for ( i = 0; i < sizeof(cDefault)/sizeof(char*); i ++ ) {
                if ( !strcmp ( name, cDefault[i] ) )
                        return i;
        }
        return -1;
}

static enum ATTRI_IDR get_attribute_type ( char* stype, int* size )
{
        if ( strcmp ( "float", stype ) ) {
                *size = 4;
                return ATTRI_FLOAT;
        } else if ( strcmp ( "float2", stype ) ) {
                *size = 2*4;
                return ATTRI_FLOAT2;
        } else if ( strcmp ( "float3", stype ) ) {
                *size = 3*4;
                return ATTRI_FLOAT3;
        } else if ( strcmp ( "float4", stype ) ) {
                *size = 4*4;
                return ATTRI_FLOAT4;
        } else if ( strcmp ( "int", stype ) ) {
                *size = 4;
                return ATTRI_INT;
        } else {
                log_severe_err_dbg ( "unkown type" );
                return ATTRI_NULL;
        }
}

bool shaform_link ( int n_stages, ... )
{
        if ( n_stages <= 0 ) {
                log_severe_err_dbg ( "it must have at least 1 stage, but it has %d stages",
                                     n_stages );
                return false;
        }
        va_list arg;
        va_start ( arg, n_stages );
        struct shader_form** stgs = alloc_fix ( sizeof *stgs, n_stages );
        int i;
        for ( i = 0; i < n_stages; i ++ ) {
                stgs[i] = va_arg ( arg, struct shader_form* );
                if ( !stgs[i]->is_compiled ) {
                        log_severe_err_dbg ( "shader %s, stage: %d has not been compiled",
                                             stgs[i]->name, i );
                        return false;
                }
        }
        va_end ( arg );

        bool ret_val = true;

        switch ( stgs[0]->c_type ) {
        case SHADER_C_FAKE:
        case SHADER_C_X86:
                /* allocate locations for uniforms */

                /* allocate locations for builtin variables */
                for ( i = 0; i < n_stages - 1; i ++ ) {
                        struct shader_dict* c =
                                alg_array(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                        int j;
                        for ( j = 0;
                              j < alg_n(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                              j ++ ) {
                                int loc = default_var_loc ( c[i].var_name );
                                if ( loc == -1 )
                                        continue;
                                else
                                        c[i].loc = loc;
                        }
                        stgs[i]->alloc[ITEM_INVAR]      = cNumDefault;
                        stgs[i]->alloc[ITEM_OUTVAR]     = cNumDefault;
                        alg_expand ( list, &stgs[i]->loc[ITEM_INVAR], cNumDefault );
                        alg_expand ( list, &stgs[i]->loc[ITEM_OUTVAR], cNumDefault );
                }
                /* allocate input location for the first stage */
                struct shader_dict* c =
                        alg_array(list, &stgs[0]->mapper[ITEM_INVAR]);
                for ( i = 0;
                      i < alg_n(list, &stgs[0]->mapper[ITEM_INVAR]);
                      i ++ ) {
                        if ( -1 != default_var_loc ( c[i].var_name ) )
                                continue;
                        c[i].loc = stgs[0]->alloc[ITEM_INVAR] ++;
                        alg_expand ( list,
                                     &stgs[0]->loc[ITEM_INVAR],
                                     stgs[0]->alloc[ITEM_INVAR] );
                }
                /* allocate output location for every stage,
                 * and generate output attribute for each stage */
                for ( i = 0; i < n_stages - 1; i ++ ) {
                        struct shader_dict* c =
                                alg_array(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                        struct shader_attribute* attri = &stgs[i]->out_attri;
                        int j;
                        for ( j = 0;
                              j < alg_n(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                              j ++ ) {
                                int s;
                                attr_set_i ( attri, j, get_attribute_type(c[i].type, &s) );
                                if ( -1 != default_var_loc ( c[i].var_name ) )
                                        continue;
                                c[i].loc = stgs[i]->alloc[ITEM_OUTVAR] ++;
                                alg_expand ( list,
                                             &stgs[i]->loc[ITEM_OUTVAR],
                                             stgs[i]->alloc[ITEM_OUTVAR] );
                        }
                        attr_finalize ( attri );
                }

                if ( n_stages == 1 )
                        goto skip;
                /* link output of the current stage to
                 * the input of the next stage */
                for ( i = 0; i < n_stages - 1; i ++ ) {
                        struct shader_dict* c =
                                alg_array(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                        int j;
                        for ( j = 0;
                              j < alg_n(list, &stgs[i]->mapper[ITEM_OUTVAR]);
                              j ++ ) {
                                char* out = c[j].var_name;
                                struct shader_dict* f_next =
                                        find_shader_dict ( stgs[i + 1], ITEM_INVAR, out );

                                if ( f_next == nullptr ) {
                                        log_severe_err_dbg ( "cannot obtain input varriable %s at stage: %d, while it is the output of stage: %d", out, i + 1, i );
                                        ret_val = false;
                                }

                                f_next->loc = i;
                        }
                }
skip:
                /* obtain logical address for each variable and uniform */
                switch ( stgs[0]->c_type ) {
                case SHADER_C_FAKE:
                        for ( i = 0; i < n_stages - 1; i ++ ) {
                                int j;
                                for ( j = 0; j < 3; j ++ ) {
                                        struct shader_dict* c =
                                                alg_array(list, &stgs[i]->mapper[j]);
                                        void** loc = alg_array(list, &stgs[i]->loc[j]);
                                        int k;
                                        for ( k = 0;
                                              k < alg_n(list, &stgs[i]->mapper[j]);
                                              k ++ ) {
                                                loc[c[k].loc] =
                                                        symlib_ret_variable ( stgs[i]->dl,
                                                                              c[k].var_name,
                                                                              nullptr );
                                        }
                                }
                        }
                        break;
                case SHADER_C_X86:
                        break;
                default:
                        ;
                }
                break;
        case SHADER_C_OPENGL:
                break;
        }

        for ( i = 0; i < n_stages; i ++ ) {
                stgs[i]->is_linked = ret_val;
        }
        free_fix ( stgs );
        return ret_val;
}

f_Generic shaform_exe_x86 ( struct shader_form* shader )
{
        return shader->func;
}

bool shaform_is_compiled ( struct shader_form* shader )
{
        return shader->is_compiled;
}

bool shaform_is_linked ( struct shader_form* shader )
{
        return shader->is_linked;
}
