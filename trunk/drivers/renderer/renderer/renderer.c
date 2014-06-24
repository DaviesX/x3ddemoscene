/* renderer.c: rendering system interface */
#include <x3d/renderer.h>
#include <renderer/geocache.h>
#include <renderer/renderer.h>
#include "../renderer/main.h"
#include "../shader/main.h"
#include "irenderer.h"
#include "rtrenderer.h"


struct irenderer_ops {
        void *(*create) ( void );
        void (*free) ( void *r );
        void (*update) ( struct alg_llist *command, void *r );
        void (*begin) ( void *r );
        void (*render) ( struct probe *probe, struct render_out *ro, void *r );
        void (*end) ( void *r );
};

const struct irenderer_ops RendOps[] = {
        [RENDERER_RASTERIZER].create = cast(RendOps->create) 	create_rt_renderer,
        [RENDERER_RASTERIZER].free = cast(RendOps->free) 	free_rt_renderer,
        [RENDERER_RASTERIZER].update = cast(RendOps->update) 	update_rt_renderer,
        [RENDERER_RASTERIZER].begin = cast(RendOps->begin) 	rt_renderer_begin,
        [RENDERER_RASTERIZER].render = cast(RendOps->render) 	rt_renderer_render,
        [RENDERER_RASTERIZER].end = cast(RendOps->end) 		rt_renderer_end
};

void init_irenderer (
        enum RENDERER_IDR idr, enum RENDERER_THREAD_STATE_IDR thr_state,
        enum RENDER_SPEC_IDR spec,void *_rend )
{
        struct irenderer *rend = _rend;
        rend->idr = idr;
        rend->thr_state = thr_state;
        rend->spec = spec;
        init_shader_library ();
}

void free_irenderer ( void *_rend )
{
}

struct irenderer *icreate_renderer ( enum RENDERER_IDR method )
{
        return RendOps[method].create ();
}

void ifree_renderer ( struct irenderer *r )
{
        RendOps[r->idr].free ( r );
}

void iupdate_renderer ( struct alg_llist *command, struct irenderer *r )
{
        RendOps[r->idr].update ( command, r );
}

void irenderer_begin ( struct irenderer *r )
{
        RendOps[r->idr].begin ( r );
}

void irenderer_render ( struct probe *probe, struct render_out *ro, struct irenderer *r )
{
        RendOps[r->idr].render ( probe, ro, r );
}

void irenderer_end ( struct irenderer *r )
{
        RendOps[r->idr].end ( r );
}

struct geocache *irenderer_export_geocache ( struct irenderer *r )
{
        return nullptr;
}

struct probe *irenderer_export_probe ( struct irenderer *r )
{
        return nullptr;
}
