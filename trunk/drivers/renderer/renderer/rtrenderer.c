/* rt_renderer.c: interface of rasterizer renderer goes here */
#include <memory.h>
#include <renderer/proj_probe.h>
#include <renderer/geocache.h>
#include <renderer/out.h>
#include "irenderer.h"
#include "rtrenderer.h"


/*
 * Structures
 */
struct rt_renderer {
        struct irenderer _rend;
        struct geocache gc;
        struct proj_probe probe;
        struct render_out ro;
};


struct rt_renderer *create_rt_renderer ( void )
{
        struct rt_renderer *rt_rend = alloc_fix ( sizeof *rt_rend, 1 );
        memset ( rt_rend, 0, sizeof *rt_rend );
        init_irenderer (
                RENDERER_RASTERIZER, RENDERER_THREAD_MUTUAL | RENDERER_THREAD_SINGLE,
                RENDER_SPEC_SW_BUILTIN, rt_rend );

        memset ( &rt_rend->probe, 0, sizeof rt_rend->probe );
        memset ( &rt_rend->ro, 0, sizeof rt_rend->ro );
        init_geocache ( &rt_rend->gc );
        return rt_rend;
}

void free_rt_renderer ( struct rt_renderer *rt_rend )
{
        free_irenderer ( rt_rend );
        free_proj_probe ( &rt_rend->probe );
        free_geocache ( &rt_rend->gc );
        free_render_out ( &rt_rend->ro );
        memset ( rt_rend, 0, sizeof *rt_rend );
        free_fix ( rt_rend );
}

void update_rt_renderer ( struct alg_llist *command, struct rt_renderer *r )
{
}

void rt_renderer_begin ( struct rt_renderer *r )
{
}

void rt_renderer_render ( struct proj_probe *probe, struct render_out *ro,
                          struct rt_renderer *r )
{
}

void rt_renderer_end ( struct rt_renderer *r )
{
}
