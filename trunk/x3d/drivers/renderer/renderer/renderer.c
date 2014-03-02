/* renderer.c: rendering system interface */
#include <x3d/renderer.h>
#include <renderer/geocache.h>
#include <renderer/renderer.h>
#include "../renderer/main.h"
#include "../shader/main.h"
#include "rtrenderer.h"


struct renderer_operations {
	void *(*create) ( struct renderer *r );
	void (*free) ( void *r );
	void (*update) ( struct alg_named_params *params, void *r );
	void (*begin) ( void *r );
	void (*render) ( struct probe *probe, struct rend_out *ro, void *r );
	void (*end) ( void *r );
};

const struct renderer_operations RendOps[] = {
	[RENDERER_IDR_RASTERIZER].create = cast(RendOps->create) 	create_rt_renderer,
	[RENDERER_IDR_RASTERIZER].free = cast(RendOps->free) 		free_rt_renderer,
	[RENDERER_IDR_RASTERIZER].update = cast(RendOps->update) 	update_rt_renderer,
	[RENDERER_IDR_RASTERIZER].begin = cast(RendOps->begin) 		rt_renderer_begin,
	[RENDERER_IDR_RASTERIZER].render = cast(RendOps->render) 	rt_renderer_render,
	[RENDERER_IDR_RASTERIZER].end = cast(RendOps->end) 		rt_renderer_end
};

struct renderer *create_renderer ( enum RENDERER_IDR method )
{
	struct renderer *r = alloc_fix ( sizeof *r, 1 );
	r->idr = method;
	r->thr_state = RENDERER_THREAD_MUTUAL | RENDERER_THREAD_SINGLE;
	r->spec = RENDER_SPEC_SW_BUILTIN;
	r->rend = RendOps[r->idr].create ( r );
	dbg_renderer_add_all ();
	dbg_shader_add_all ();
	return r;
}

void free_renderer ( struct renderer *r )
{
	RendOps[r->idr].free ( r->rend );
	free_fix ( r );
}

void update_renderer ( struct alg_named_params *params, struct renderer *r )
{
	RendOps[r->idr].update ( params, r->rend );
}

void renderer_begin ( struct renderer *r )
{
	RendOps[r->idr].begin ( r->rend );
}

void renderer_render ( struct probe *probe, struct rend_out *ro, struct renderer *r )
{
	RendOps[r->idr].render ( probe, ro, r->rend );
}

void renderer_end ( struct renderer *r )
{
	RendOps[r->idr].end ( r->rend );
}

struct geocache *renderer_export_geocache ( struct renderer *r )
{
	return r->gc;
}

struct probe *renderer_export_probe ( struct renderer *r )
{
	return r->probe;
}
