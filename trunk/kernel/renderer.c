/*! \file kernel/renderer.c
    \brief all kernel renderer interfaces go here. */

#include <logout.h>
#include <algorithm.h>
#include <x3d/renderer.h>
#include <renderer/renderer.h>


#define MAX_RENDER_CONTEXT		32

struct lcrenderer;
struct renderable;
struct render_out;
struct probe;

typedef struct renderer {
        uuid_t                  rend_id;
        struct render_bytecode  bytecode;
        struct lcrenderer*      rend;
//        struct alg_llist command;
} *p_renderer_t;

struct renderer_container {
        d_alg_llist(struct renderer*)    renderer;
};

struct renderer_ops {
        f_LCRenderer_Init       lcrenderer_init;
        f_LCRenderer_Create     lcrenderer_create;
        f_LCRenderer_Free       lcrenderer_free;
        f_LCRenderer_Update     lcrenderer_update;
        f_LCRenderer_Render     lcrenderer_render;
        f_LCRenderer_Output     lcrenderer_output;
};

static struct renderer_ops              g_rend_ops;
static struct renderer_container        g_rend_cont;


/* container's */
/** \brief initialize the global renderer container.
 *
 * Note that: this operation is *NOT* thread-safe
 * \param void
 * \return void
 */
void renderer_kernel_init ( void )
{
        alg_init ( llist, &g_rend_cont.renderer, sizeof(p_renderer_t), 1 );
}

/** \brief delete the global renderer container.
 * \param void
 * \return void
 */
void renderer_kernel_free ( void )
{
        alg_iter(struct renderer*) iter;
        alg_iter(struct renderer*) last;
        alg_first ( llist, &g_rend_cont.renderer, iter );
        alg_last ( llist, &g_rend_cont.renderer, last );
        while ( iter != last ) {
                free_renderer ( alg_access ( iter ) );
                alg_next ( llist, &g_rend_cont.renderer, iter );
        }
        alg_free ( llist, &g_rend_cont.renderer );
        zero_obj ( &g_rend_cont );
}

/** \brief add in a renderer to the global container.
 *
 * Note that: only the p_renderer_t(pointer to the struct renderer) is added, but not the content of the structure. nullptr is allowed, but please remember to specify the pointer at some moment of the time. the location address can be found by renderer_container_find(). you may specify the pointer to that address manually.
 * \param rend struct renderer* [in] pointer to the renderer structure; it can be obtained by create_renderer ().
 * \return uuid_t unique id that identifies the location of that added renderer
 *
 */
uuid_t renderer_container_add ( struct renderer *rend )
{
        d_alg_llist(renderer*)* rend_cont = &g_rend_cont.renderer;
        if ( rend == nullptr )
                rend = create_renderer ( RENDERER_UNDETERMINATE );

        rend->rend_id = alg_gen_uuid ();
        alg_push_back ( llist, rend_cont, &rend );
        return rend->rend_id;
}

#define cmp_rend_id( _data, _iter )     ((_data) == alg_access(_iter)->rend_id)
/** \brief remove the added renderer from the global container.
 *
 * Note that: the location as well as the content of the struct renderer will be removed.
 * \param id uuid_t [in] the unique id of the to-be-remove renderer
 * \return void
 */
void renderer_container_remove ( uuid_t id )
{
        struct alg_llist *rend_cont = &g_rend_cont.renderer;
        alg_iter(struct renderer*) iter;
        alg_find ( llist, rend_cont, id, iter, cmp_rend_id );
        alg_remove ( llist, rend_cont, iter );
        free_renderer ( alg_access ( iter ) );
}

/** \brief remove the added renderer from the global container.
 *
 * \param id uuid_t [in] the unique id of the renderer that has added to the global container formerly.
 * \return struct renderer* nullptr if the given uuid_t id cannot be found, otherwise, will be the pointer to struct renderer.
 */
struct renderer *renderer_container_find ( uuid_t id )
{
        struct alg_llist *rend_cont = &g_rend_cont.renderer;
        alg_iter(struct renderer*) iter;
        alg_llist_find ( id, iter, cmp_rend_id, rend_cont );
        return alg_access ( iter );
}
#undef cmp_rend_id

struct renderer *renderer_container_i ( int i )
{
        return nullptr;
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
/** \brief create a renderer.
 *
 * Note that: once the type of the renderer is attached(it will be attached on its creation), it cannot easily be changed. a call of renderer_retype() can change the type of the renderer, but all the settings formerly applied to that renderer will be lost.
 * \param type enum RENDERER_IDR [in] the type of renderer to be created.
 * \return struct renderer* the created renderer.
 */
struct renderer *create_renderer ( enum RENDERER_IDR type )
{
        struct renderer *rend = alloc_fix ( sizeof *rend, 1 );
        zero_obj ( rend );
        if ( type != RENDERER_UNDETERMINATE ) {
                rend->rend = g_rend_ops.lcrenderer_create ( type );
        }
        return rend;
}

/** \brief delete the renderer
 * \param rend struct renderer* [in] pointer to the renderer
 * \return void
 */
void free_renderer ( struct renderer *rend )
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
void renderer_set_draw_mode ( enum GEOMETRY_PIPE_IDR draw_mode, struct renderer *rend )
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

__dlexport void renderer_render_tree ( struct render_tree *tree, struct renderer *rend )
{
}

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
