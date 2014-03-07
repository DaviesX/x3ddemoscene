#ifndef X3DRENDERING_H_INCLUDED
#define X3DRENDERING_H_INCLUDED


#include <x3d/renderer.h>
#include <renderer/geocache.h>


struct geocache;
struct probe;
struct rend_out;

/*
 * Structures
 */
struct renderer {
        enum RENDERER_IDR idr;
        enum RENDERER_THREAD_STATE_IDR thr_state;
        enum RENDER_SPEC_IDR spec;
        void *rend;
        struct geocache *gc;
        struct probe *probe;
        struct rend_out *ro;
};

/*
 * Functions' declaration
 */
struct renderer *create_renderer (  enum RENDERER_IDR method );
void free_renderer ( struct renderer *r );
void update_renderer ( struct alg_named_params *params, struct renderer *r );
void renderer_begin ( struct renderer *r );
void renderer_render ( struct probe *probe, struct rend_out *ro, struct renderer *r );
void renderer_end ( struct renderer *r );
struct geocache *renderer_export_geocache ( struct renderer *r );
struct probe *renderer_export_probe ( struct renderer *r );


#endif // X3DRENDERING_H_INCLUDED
