/* rt_renderer.c: interface of rasterizer renderer goes here */
#include <memory.h>
#include <renderer/renderer.h>
#include <renderer/proj_probe.h>
#include <renderer/geocache.h>
#include <renderer/out.h>
#include "rtrenderer.h"



struct rt_renderer *create_rt_renderer ( struct renderer *parent )
{
        struct rt_renderer *rt_rend =
                alloc_fix ( sizeof *rt_rend, 1 );
        memset ( rt_rend, 0, sizeof *rt_rend );
        rt_rend->parent = parent;
        parent->probe =
                alloc_fix ( sizeof ( struct proj_probe ), 1 );
        memset ( parent->probe, 0, sizeof ( struct proj_probe ) );
        parent->ro = alloc_fix ( sizeof ( struct render_out ), 1 );
        memset ( parent->ro, 0, sizeof ( struct render_out ) );
        parent->gc =
                alloc_fix ( sizeof ( struct geocache ), 1 );
        init_geocache ( parent->gc );
        return rt_rend;
}

void free_rt_renderer ( struct rt_renderer *r )
{
        struct renderer *parent = r->parent;
        free_proj_probe ( (struct proj_probe *) parent->probe );
        free_geocache ( (struct geocache *) parent->gc );
        free_render_out ( (struct render_out *) parent->ro );
        free_fix ( parent->probe );
        free_fix ( parent->gc );
        free_fix ( parent->ro );
        memset ( parent->rend, 0, sizeof *parent->rend );
        free_fix ( parent->rend );
}

void update_rt_renderer ( struct alg_named_params *params, struct rt_renderer *r )
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















