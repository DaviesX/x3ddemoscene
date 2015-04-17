#include <logout.h>
#include <algorithm.h>
#include <x3d/renderer.h>
#include <renderer/renderer.h>


#define MAX_RENDER_CONTEXT		32

struct lcrenderer;
struct renderable;
struct render_out;
struct probe;


struct renderer_ops {
        f_LCRenderer_Init       lcrenderer_init;
        f_LCRenderer_Create     lcrenderer_create;
        f_LCRenderer_Free       lcrenderer_free;
        f_LCRenderer_Update     lcrenderer_update;
        f_LCRenderer_Render     lcrenderer_render;
        f_LCRenderer_Output     lcrenderer_output;
};

static struct renderer_ops              g_rend_ops;


/* container's */
/** \brief initialize the global renderer container.
 *
 * Note that: this operation is *NOT* thread-safe
 * \param void
 * \return void
 */
void renderer_kernel_init ( void )
{
        g_rend_ops.lcrenderer_init ();
}

/* register ABIs */
/** \brief Import ABIs from loaded symbols.
 *
 * \param symbols struct symbol_set* [in] loaded symbols
 * \return bool false if any symbol is not validate.
 */
bool renderer_import ( struct symbol_set *symbols )
{
        struct renderer_ops *t_ops = &g_rend_ops;
        bool ret = true;

        if ( !(t_ops->lcrenderer_init =
                       (f_LCRenderer_Init) symlib_ret_abi ( symbols, "lcrenderer_init" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_init" );
                ret = false;
        }
        if ( !(t_ops->lcrenderer_create =
                       (f_LCRenderer_Create) symlib_ret_abi ( symbols, "lcrenderer_create" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_create" );
                ret = false;
        }
        if ( !(t_ops->lcrenderer_free =
                       (f_LCRenderer_Free) symlib_ret_abi ( symbols, "lcrenderer_free" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_free" );
                ret = false;
        }
        if ( !(t_ops->lcrenderer_update =
                       (f_LCRenderer_Update) symlib_ret_abi ( symbols, "lcrenderer_update" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_free" );
                ret = false;
        }
        if ( !(t_ops->lcrenderer_render =
                       (f_LCRenderer_Render) symlib_ret_abi ( symbols, "lcrenderer_render" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_render" );
                ret = false;
        }
        if ( !(t_ops->lcrenderer_output =
                       (f_LCRenderer_Output) symlib_ret_abi ( symbols, "lcrenderer_output" )) ) {
                log_severe_err_dbg ( "fail to retrieve abi: lcrenderer_output" );
                ret = false;
        }

        return ret;
}

/* renderer's */
void renderer_init(struct renderer* rend, enum RENDERER_IDR type)
{
        zero_obj ( rend );
        if ( type != RENDERER_UNDETERMINATE ) {
                rend->rend = g_rend_ops.lcrenderer_create ( type, nullptr );
        }
}

void renderer_free(struct renderer *rend)
{
        g_rend_ops.lcrenderer_free ( rend->rend );
}

/** \brief bind a rendererable context with the renderer
 *
 * Note that: renderable context will not be utilized before renderer_update() is called. therefore, any changes will not apply if they are made after the renderer_update() call.
 * \param ctx void* [in] struct renderable_ctx to be sent into.
 * \param rend struct renderer* [in,out] renderer that the renderable context will bind onto.
 * \return void
 */
void renderer_bind_renderable_context ( void *ctx, struct renderer *rend )
{
}

struct probe *renderer_get_probe ( struct renderer *rend )
{
        return nullptr;
}

struct render_out *renderer_get_render_out ( struct renderer *rend )
{
        return nullptr;
}

void renderer_set_pipeline ( enum RENDER_PIPE_IDR pipeline, struct renderer *rend )
{
}

/** \brief set the type of hardware will be used by the renderer.
 * \param spec enum RENDER_SPEC_IDR [in] specification of the hardware.
 * \param rend struct renderer* [out] renderer to be specified.
 * \return void
 */
void renderer_set_spec ( enum RENDER_SPEC_IDR spec, struct renderer *rend )
{
}

/** \brief set the expected draw mode to the renderer.
 *
 * Note that: this function is only useful for the RENDERER_RASTERIZER renderer.
 * \param draw_mode enum DRAW_MODE_IDR [in] draw mode selected.
 * \param rend struct renderer* [out] renderer to be specified.
 * \return void
 */
void renderer_set_draw_mode ( enum GEOMETRY_MODEL_IDR draw_mode, struct renderer *rend )
{
}

void renderer_set_crop ( bool to_crop, float x0, float x1, float y0, float y1,
                         struct renderer *rend )
{
}

void renderer_new_render_layer ( int importance, float alpha, struct renderer *rend )
{
}

void renderer_set_thread ( enum RENDERER_THREAD_STATE_IDR state, struct renderer *rend )
{
}

void renderer_set_light_model ( enum LIGHT_MODEL_IDR model, struct renderer *rend )
{
}

void renderer_set_antialias ( float level, struct renderer *rend )
{
}

void renderer_set_filter ( float level, struct renderer *rend )
{
}

void renderer_set_hdr ( bool to_apply, struct renderer *rend )
{
}

void renderer_set_len_flare ( bool to_apply, struct renderer *rend )
{
}

void renderer_set_dof ( bool to_apply, struct renderer *rend )
{
}

void renderer_retype ( enum RENDERER_IDR type, struct renderer *rend )
{
}

/** \brief update all the changes to the renderer.
 *
 * Note that: this is an immediate operation. it also means this function is *NOT* thread-safe. any other member function call of the renderer class and changes to the renderable context and probe class during renderer_update() will result undefined behavior. As well, any changes made before this function call is not applied.
 * \param rend struct renderer*
 * \return void
 *
 */
void renderer_update ( struct renderer *rend )
{
        g_rend_ops.lcrenderer_update ( &rend->bytecode, rend->rend );
}

/** \brief run the renderer and render all the updated information.
 *
 * Note that: this is an immediate operation. *DON'T* call renderer_update() during the function call. but any other member function call of the renderer class is proper. Also, changes to renderable context and probe class is allowed during this function call.
 * \param rend struct renderer* [in] renderer to be specified.
 * \return void
 */
void renderer_render ( struct renderer *rend )
{
        if (rend->has_bytecode)
                g_rend_ops.lcrenderer_render ( rend->rend );
}

/** \brief output the rendering result thru the method specified by render_out
 *
 * Note that: this is an immediate operation. *DON'T* call renderer_update() during the function call. any other function call is proper.
 * \param rend struct renderer* [in] renderer to be specified.
 * \return void
 */
void renderer_commit ( struct renderer *rend )
{
        g_rend_ops.lcrenderer_output ( rend->rend );
}

static void generate_byte_code ( struct render_tree *tree, struct render_bytecode *bytecode )
{
}

__dlexport void renderer_renderscript ( const char* script, struct renderer *rend )
{
}

#define emit_instr( _instr, _unit ) \
{ \
        const typeof(_unit) _var = (_unit); \
        *(typeof(_unit)*) (_instr) = _var; \
        (_instr) += sizeof(_var); \
}

__dlexport void renderer_render_tree(struct render_tree *tree, struct renderer *rend)
{
        if (render_tree_compile(tree, &rend->bytecode)) {
                rend->has_bytecode = true;
        } else {
                log_mild_err_dbg("cannot inject bytecode because compilation is failed");
        }
#if 0
        struct render_bytecode* bc = &rend->bytecode;
        char* instr = bc->instr;
        /* RAD          dest, rda, pipe, blending */
        emit_instr ( instr, RENDER_OP_RADIANCE );
        emit_instr ( instr, 0 );
        emit_instr ( instr, *(struct rda_context**) &tree->reserve[0] );
        emit_instr ( instr, RENDER_PIPE_DIRECT_LIGHTING );
        emit_instr ( instr, 0.0f );
        /* COMP         dest, type, src  */
        emit_instr ( instr, RENDER_OP_COMPOSITE );
        emit_instr ( instr, 1 );
        emit_instr ( instr, 1 );
        emit_instr ( instr, 0 );
        /* OUT          probe, src */
        emit_instr ( instr, RENDER_OP_OUTPUT );
        emit_instr ( instr, 1 );
#endif // 0
}

#undef emit_instr
/*
__dlexport struct render_operand* render_tree_create ( struct render_tree *tree )
{
        return nullptr;
}

__dlexport struct render_operand* render_tree_find ( int pos_id, struct render_tree *tree )
{
        return nullptr;
}

__dlexport int render_tree_create_child ( struct render_operand* parent,
                struct render_operand* child,
                struct render_tree *tree )
{
        return -1;
}

__dlexport void render_tree_create_environment ( struct render_operand* env,
                struct render_tree* tree )
{
}
__dlexport int render_tree_first ( struct render_tree *tree )
{
        return -1;
}

__dlexport int render_tree_next ( int pos_id, struct render_tree *tree )
{
        return -1;
}

__dlexport struct alg_list* render_tree_environment ( struct render_tree *tree )
{
        return nullptr;
}
*/
