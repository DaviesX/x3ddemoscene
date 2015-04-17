#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include <math/math.h>
#include <staging.h>
#include <symlib.h>
#include <algorithm.h>

#include <x3d/rendertree.h>


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

struct lcrenderer;
struct renderer;
struct rda_context;
struct probe;
struct render_out;


/*
 * structures
 */
typedef struct renderer {
        uuid_t                  rend_id;
        bool                    has_bytecode;
        struct render_bytecode  bytecode;
        struct lcrenderer*      rend;
} *p_renderer_t;

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
__dlexport void renderer_init(struct renderer* rend, enum RENDERER_IDR type);
__dlexport void renderer_free(struct renderer *rend);
__dlexport void renderer_retype ( enum RENDERER_IDR type, struct renderer *rend );

__dlexport void renderer_update ( struct renderer *rend );
__dlexport void renderer_render ( struct renderer *rend );
__dlexport void renderer_commit ( struct renderer *rend );

__dlexport void renderer_renderscript ( const char* script, struct renderer *rend );
__dlexport void renderer_render_tree ( struct render_tree *tree, struct renderer *rend );


#endif // RENDERER_H_INCLUDED
