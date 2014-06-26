/* ptrenderer.c: interface of path tracing renderer go here */
#include <x3d/renderer.h>
#include "lcrenderer.h"
#include "ptrenderer.h"


struct pt_renderer {
        void* something;
};

void init_ptrenderer ( struct lcrenderer_ops *ops )
{
        ops->lcrenderer_create = cast (ops->lcrenderer_create) create_pt_renderer;
        ops->lcrenderer_free   = cast (ops->lcrenderer_free)   free_pt_renderer;
        ops->lcrenderer_update = cast (ops->lcrenderer_update) pt_renderer_update;
        ops->lcrenderer_render = cast (ops->lcrenderer_render) pt_renderer_render;
        ops->lcrenderer_output = cast (ops->lcrenderer_output) pt_renderer_output;
}

struct pt_renderer *create_pt_renderer ( enum RENDERER_IDR type )
{
        struct pt_renderer* t_rend = alloc_fix ( sizeof *t_rend, 1 );
        zero_obj ( t_rend );
        return t_rend;
}

void free_pt_renderer ( struct pt_renderer* r )
{
        zero_obj ( r );
        free_fix ( r );
}

void pt_renderer_update ( struct render_bytecode* bytecode, struct pt_renderer* r )
{
}

void pt_renderer_render ( struct pt_renderer* r )
{
}

void pt_renderer_output ( struct pt_renderer* r )
{
}
