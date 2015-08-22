/* renderer.c: rendering system interface */
#include <x3d/renderer.h>
#include <renderer/shader.h>
#include <renderer/proj_probe.h>
#include "lcrenderer.h"


static struct lcrenderer_ops g_render_ops[cNumRenderer];

void lcrenderer_register()
{
}

void lcrenderer_init ( void )
{
        init_shader_library ();
        zero_obj ( &g_render_ops[RENDERER_UNDETERMINATE] );
        init_rtrenderer ( &g_render_ops[RENDERER_RASTERIZER] );
        init_ptrenderer ( &g_render_ops[RENDERER_PATH_TRACER] );
}

struct lcrenderer* lcrenderer_create ( enum RENDERER_IDR method, struct proj_probe* probe )
{
        struct lcrenderer* t_rend = alloc_fix ( sizeof *t_rend, 1 );
        t_rend->ops      = g_render_ops[method];
        t_rend->renderer = g_render_ops[method].lcrenderer_create ( method, (struct probe*) probe );
        return t_rend;
}

void lcrenderer_free ( struct lcrenderer *rend )
{
        rend->ops.lcrenderer_free ( rend->renderer );
        zero_obj ( rend );
        free_fix ( rend );
}

void lcrenderer_update ( struct render_bytecode *bytecode, struct lcrenderer *rend )
{
        rend->ops.lcrenderer_update ( bytecode, rend->renderer );
}

void lcrenderer_render ( struct lcrenderer *rend )
{
        rend->ops.lcrenderer_render ( rend->renderer );
}

void lcrenderer_output ( struct lcrenderer *rend )
{
        rend->ops.lcrenderer_output ( rend->renderer );
}
