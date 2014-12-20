/*! \file include/x3d/renderer.h
    \brief all kernel renderer classes and enumerations go here. */

#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <math/math.h>
#include <staging.h>
#include <symlib.h>
#include <algorithm.h>


/* The following rendering method is expected to engage in the renderer */
/** \brief available renderer type
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
#define cNumRenderer            9

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

enum RENDER_ACC_IDR {
        RENDER_ACC_LINEAR,
        RENDER_ACC_GRID,
        RENDER_ACC_BVH
};

enum RENDER_ENVIRONMENT {
        RENDER_ENV_SPEC,
        RENDER_ENV_PROBE,
        RENDER_ENV_RENDER_OUT,
        RENDER_ENV_THREAD,
        RENDER_ENV_ANTIALIAS,
        RENDER_ENV_FILTERING
};

enum RENDER_OP {
        RENDER_OP_RADIANCE,             /* RAD          dest, rda, pipe, blending */
//        RENDER_OP_PROBE,                /* prob         probe */
        RENDER_OP_COMPOSITE,            /* COMP         dest, type, src  */
        RENDER_OP_OUTPUT                /* OUT          probe, src */
};

struct lcrenderer;
struct renderer;
struct rda_context;
struct probe;
struct render_out;

struct render_node;


/*
 * structures
 */

enum RENDER_NODE_IDR {
        RENDER_LAYER,
        RENDER_RADIANCE
};

struct render_node {
        char*                   name;
        int                     pos_id;
        enum RENDER_NODE_IDR    type;
        struct render_node*     input;
        int                     n_input;
        struct render_node*     output;
        int                     n_output;
        void*                   lc_data;
};

struct render_layer {
        struct render_node      _node;
};

enum RENDER_PIPE_IDR {
        RENDER_PIPE_FORWARD,
        RENDER_PIPE_FORWARD_PLUS,
        RENDER_PIPE_WHITTED,
        RENDER_PIPE_DIRECT_LIGHTING,
        RENDER_PIPE_METROPOLIS,
        RENDER_PIPE_IMAGE_SPACE
};

enum LIGHT_MODEL_IDR {
        LIGHT_MODE_DIRECT     = 0X1,
        LIGHT_MODE_SHADOW     = 0X2,
        LIGHT_MODE_LIGHTMAP   = 0X4,
        LIGHT_MODE_SH_PROBE   = 0X8,
        LIGHT_MODE_SVO        = 0X10
};

enum GEOMETRY_MODEL_IDR {
        GEOMETRY_MODEL_WIREFRAME,
        GEOMETRY_MODEL_SOLID
};

struct render_radiance {
        struct render_node      _node;
        struct rda_context*     rda_list;
        enum RENDER_PIPE_IDR    pipe;
        float                   blend;
};

struct render_post_ao {
        struct render_node      _node;
};

struct render_filmic_hdr {
        struct render_node      _node;
        const float     A;// = 0.15f;
        const float     B;// = 0.50f;
        const float     C;// = 0.10f;
        const float     D;// = 0.20f;
        const float     E;// = 0.02f;
        const float     F;// = 0.30f;
        const float     W;// = 11.2f;
        float           key;
        float           exp;
};

struct render_post_aa {
};

struct render_dof {
};

struct render_color_grading {
};

struct render_mask {
};

struct render_blending {
};

struct render_gamma {
};

struct render_tree {
        struct render_operand*                  output;
        d_alg_llist(render_operand)             nodes;
        d_alg_list(render_operand)              env;
        uint32_t                                reserve[4];
};

struct render_bytecode {
        char*   instr;
};

/*
 * functions' declaration
 */
/* container's */
void renderer_kernel_init ( void );
void renderer_kernel_free ( void );

/* ABIs */
typedef void (*f_LCRenderer_Init) ( void );
typedef struct lcrenderer* (*f_LCRenderer_Create) ( enum RENDERER_IDR method, struct probe* probe );
typedef void (*f_LCRenderer_Free) ( struct lcrenderer *rend );
typedef void (*f_LCRenderer_Update) ( struct render_bytecode *bytecode, struct lcrenderer *rend );
typedef void (*f_LCRenderer_Render) ( struct lcrenderer *rend );
typedef void (*f_LCRenderer_Output) ( struct lcrenderer *rend );

bool renderer_import ( struct symbol_set *symbols );

/* renderer's */
__dlexport struct renderer *create_renderer ( enum RENDERER_IDR type, void* probe );
__dlexport void free_renderer ( struct renderer *rend );
__dlexport void renderer_retype ( enum RENDERER_IDR type, struct renderer *rend );

__dlexport void renderer_update ( struct renderer *rend );
__dlexport void renderer_render ( struct renderer *rend );
__dlexport void renderer_commit ( struct renderer *rend );

__dlexport void renderer_renderscript ( const char* script, struct renderer *rend );
__dlexport void renderer_render_tree ( struct render_tree *tree, struct renderer *rend );

__dlexport struct render_operand* render_tree_create ( struct render_tree *tree );
__dlexport struct render_operand* render_tree_find ( int pos_id, struct render_tree *tree );
__dlexport int render_tree_create_child ( struct render_operand* parent,
                struct render_operand* child,
                struct render_tree *tree );
__dlexport void render_tree_create_environment ( struct render_operand* env,
                struct render_tree* tree );
__dlexport int render_tree_first ( struct render_tree *tree );
__dlexport int render_tree_next ( int pos_id, struct render_tree *tree );
__dlexport struct alg_list* render_tree_environment ( struct render_tree *tree );

__dlexport const char** renderer_query_renderer_names ( int* n );
__dlexport const char** renderer_query_param_names ( const char* renderer, int* n );


#endif // RENDERER_H_INCLUDED
