/*! \file include/x3d/renderer.h
    \brief all kernel renderer classes and enumerations go here. */

#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <math/math.h>
#include <staging.h>
#include <algorithm.h>


/* The following rendering method is expected to engage in the renderer */
/** \brief availble renderer type
 */
enum RENDERER_IDR {
        RENDERER_UNDETERMINATE,         /**< a memory block with undefined renderer */
        RENDERER_RASTERIZER,            /**< rasterization renderer */
        RENDERER_PATH_TRACER,           /**< path tracing renderer */
        RENDERER_PHOTON_TRACER,         /**< photon tracing renderer */
        RENDERER_PHOTON_MAP,            /**< photon light map generation renderer */
        RENDERER_RADIOSITY,             /**< radiosity light map generation renderer */
        RENDERER_RADIANCE_CACHE,        /**< radiance cache generation renderer */
        RENDERER_PRT,                   /**< precomputed radiance transfer renderer */
        RENDERER_SELECTION              /**< object selection renderer */
};

enum RENDERER_THREAD_STATE_IDR {
        /* Renderer should not handle threading from these constants itself,
         * this are the states telling which situation
         * the renderer is being created with */
        RENDERER_THREAD_MUTUAL = 0X0,
        RENDERER_THREAD_SEPARATED = 0X1,

        /* These are what renderer expected to behave internally */
        RENDERER_THREAD_SINGLE = 0X2,
        RENDERER_THREAD_MULTIPLE = 0X4,
};

enum RENDER_SPEC_IDR {
        RENDER_SPEC_SW_BUILTIN,
        RENDER_SPEC_HW_OPENGL,
        RENDER_SPEC_HW_DIRECTX
};

enum DRAW_MODE_IDR {
        DRAW_MODE_WIREFRAME,
        DRAW_MODE_SOLID
};

enum LIGHT_MODEL_IDR {
        LIGHT_MODE_DIRECT     = 0X1,
        LIGHT_MODE_SHADOW     = 0X2,
        LIGHT_MODE_INDIRECT0  = 0X4,
        LIGHT_MODE_INDIRECT1  = 0X8,
        LIGHT_MODE_INDIRECT2  = 0X10
};

enum RENDER_COMMAND {
        RENDER_COMMAND_SPEC,
        RENDER_COMMAND_DRAW_MODE,
        RENDER_COMMAND_RENDER_REGION,
        RENDER_COMMAND_RENDERABLE_CONTEXT,
        RENDER_COMMAND_CROP,
        RENDER_COMMAND_RENDER_LAYER,
        RENDER_COMMAND_THREAD,
        RENDER_COMMAND_LIGHTING,
        RENDER_COMMAND_ANTIALIAS,
        RENDER_COMMAND_FILTERING,
        RENDER_COMMAND_HDR,
        RENDER_COMMAND_LEN_FLARE,
        RENDER_COMMAND_DOF
};

struct lcrenderer;
struct renderer;
struct renderable_ctx;
struct probe;
struct render_out;


/*
 * structures
 */
struct rend_coomand {
        enum RENDER_COMMAND type;
        struct alg_named_params params;
};

struct renderer_ops {
        struct lcrenderer *(*create) ( enum RENDERER_IDR method );
        void (*free) ( struct lcrenderer *r );
        void (*update) ( struct alg_llist *command, struct lcrenderer *r );
        void (*render) ( struct probe *probe, struct render_out *ro, struct lcrenderer *r );
};

struct render_out_ops {
        void (*output) ( struct render_out *ro );
};

/*
 * functions' declaration
 */
/* container's */
void renderer_kernel_init ( void );
void renderer_kernel_free ( void );
uuid_t renderer_container_add ( struct renderer *rend );
void renderer_container_remove ( uuid_t id );
struct renderer *renderer_container_find ( uuid_t id );
struct renderer *renderer_container_i ( int i );

/* registrations */
void renderer_import_renderer ( struct renderer_ops *ops );
void renderer_import_render_out ( struct render_out_ops *ops );

/* renderer's */
struct renderer *create_renderer ( enum RENDERER_IDR type );
void free_renderer ( struct renderer *rend );

void renderer_bind_renderable_context ( void *ctx, struct renderer *rend );
struct probe *renderer_get_probe ( struct renderer *rend );
struct render_out *renderer_get_render_out ( struct renderer *rend );

void renderer_set_spec ( enum RENDER_SPEC_IDR spec, struct renderer *rend );
void renderer_set_draw_mode ( enum DRAW_MODE_IDR draw_mode, struct renderer *rend );
void renderer_set_crop (
        bool to_crop, float x0, float x1, float y0, float y1, struct renderer *rend );
void renderer_new_render_layer ( int importance, float alpha, struct renderer *rend );
void renderer_set_thread ( enum RENDERER_THREAD_STATE_IDR state, struct renderer *rend );
void renderer_set_light_model ( enum LIGHT_MODEL_IDR model, struct renderer *rend );
void renderer_set_antialias ( float level, struct renderer *rend );
void renderer_set_filter ( float level, struct renderer *rend );
void renderer_set_hdr ( bool to_apply, struct renderer *rend );
void renderer_set_len_flare ( bool to_apply, struct renderer *rend );
void renderer_set_dof ( bool to_apply, struct renderer *rend );
void renderer_retype ( enum RENDERER_IDR type, struct renderer *rend );

void renderer_update ( struct renderer *rend );
void renderer_render ( struct renderer *rend );
void renderer_commit ( struct renderer *rend );


#endif // RENDERER_H_INCLUDED
