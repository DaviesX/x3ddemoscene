/* rt_renderer.c: interface of rasterizer renderer goes here */
#include <memory.h>
#include <x3d/renderer.h>
#include <renderer/proj_probe.h>
#include <renderer/geocache.h>
#include <renderer/out.h>
#include "lcrenderer.h"
#include "rtrenderer.h"


/*
 * Structures
 */
struct rt_renderer {
        struct geocache         gc;
        struct proj_probe       probe;
        struct render_out       ro;
        enum RENDER_SPEC_IDR    spec;
        enum RENDERER_THREAD_STATE_IDR thr_state;
};

void init_rtrenderer ( struct lcrenderer_ops *ops )
{
        ops->lcrenderer_create = cast (ops->lcrenderer_create) create_rt_renderer;
        ops->lcrenderer_free   = cast (ops->lcrenderer_free)   free_rt_renderer;
        ops->lcrenderer_update = cast (ops->lcrenderer_update) rt_renderer_update;
        ops->lcrenderer_render = cast (ops->lcrenderer_render) rt_renderer_render;
        ops->lcrenderer_output = cast (ops->lcrenderer_output) rt_renderer_output;
}

struct rt_renderer *create_rt_renderer ( enum RENDERER_IDR type )
{
        struct rt_renderer *rt_rend = alloc_fix ( sizeof *rt_rend, 1 );
        zero_obj ( rt_rend );
        zero_obj ( &rt_rend->probe );
        zero_obj ( &rt_rend->ro );
        init_geocache ( &rt_rend->gc );
        return rt_rend;
}

void free_rt_renderer ( struct rt_renderer *rt_rend )
{
        free_proj_probe ( &rt_rend->probe );
        free_geocache ( &rt_rend->gc );
        free_render_out ( &rt_rend->ro );
        zero_obj ( rt_rend );
        free_fix ( rt_rend );
}

void rt_renderer_update ( struct render_bytecode* bytecode, struct rt_renderer* r )
{
}

void rt_renderer_render ( struct rt_renderer* r )
{
}

void rt_renderer_output ( struct rt_renderer* r )
{
}
