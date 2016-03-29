/* rt_renderer.c: interface of rasterizer renderer goes here */
#include <system/allocator.h>
#include <x3d/renderer.h>
#include <x3d/renderenvconsts.h>
#include <x3d/rendernoderadiance.h>
#include <renderer/proj_probe.h>
#include <renderer/geocache.h>
#include <renderer/out.h>
#include "rtrenderer.h"


/*
 * Structures
 */
struct rt_renderer {
        struct geocache         gc;
        struct proj_probe       probe;
        struct render_out       ro;
        enum RenderSpecType     spec;
        int                     thread_state;
};
#if 0
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

#endif // 0


void __public rt_renderer_system_init(struct symbol_set* symbol)
{
}

// radiance node
struct render_node_ex_impl* __callback rt_radiance_node_creator(struct render_node_ex* parent)
{
        struct rt_radiance_node* self = alloc_obj(self);
        zero_obj(self);
        // fill in ops for the parent
        struct render_node_ex_ops       ops;
        ops.f_compute           = rt_radiance_node_compute;
        ops.f_free              = rt_radiance_node_free;
        ops.f_get_result        = rt_radiance_node_get_result;
        ops.f_is_compatible     = rt_radiance_node_is_compatible;
        self->_parent.ops       = ops;
        return (struct render_node_ex_impl*) self;
}
char* __implement rt_radiance_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        /* It will be compatible if the radiance node is:
         * 1. software based
         * 2. preferred rasterizer
         * 3. using solid geometry model
         * 4. with image space rendering pipeline */
        enum RenderSpecType* spec = render_tree_retrieve_environment(tree, nullptr, RenderEnvSpec);
        if (spec && *spec != RenderSpecSWBuiltin) {
                return "rt_renderer: spec && *spec != RenderSpecSWBuiltin";
        }
        enum PreferredRendererType* renderer = render_tree_retrieve_environment(tree, nullptr, RenderEnvPreferredRenderer);
        if (renderer && *renderer != RendererRasterizer) {
                return "rt_renderer: renderer && *renderer != RendererRasterizer";
        }
        enum GeometryModelType* model = render_tree_retrieve_environment(tree, nullptr, RenderEnvGeometryModel);
        if (model && *model != GeometryModelSolid) {
                return "rt_renderer: model && *model != GeometryModelSolid";
        }
        struct render_radiance* parent = cast(parent) render_node_ex_impl_get_ex(self);
        enum RenderPipeType pipe = render_node_radiance_get_pipe(parent);
        if (pipe != RenderPipeImageSpace) {
                return "rt_renderer: pipe != RenderPipeImageSpace";
        }
        return nullptr;
}

void __implement rt_radiance_node_compute(struct render_node_ex_impl* self,
                                          const struct render_node_ex_impl* input[],
                                          const struct render_node_ex_impl* output[])
{
        struct rt_radiance_node* node = (struct rt_radiance_node*) self;
        hostimg_free(&node->target);
        hostimg_init(&node->target, 1, ColorHDR32Mode, 800, 600);
        hostimg_alloc(&node->target, 0);
}

void* __implement rt_radiance_node_get_result(const struct render_node_ex_impl* self)
{
        return &((struct rt_radiance_node*) self)->target;
}

void __implement rt_radiance_node_free(struct render_node_ex_impl* self)
{
}

// renderable loader node (use pt renderer's implementation)
struct render_node_ex_impl* __callback rt_renderable_loader_node_creator(struct render_node_ex* parent)
{
        struct rt_renderable_loader_node* self = alloc_obj(self);
        zero_obj(self);
        // fill in ops for the parent
        struct render_node_ex_ops       ops;
        ops.f_compute           = rt_renderable_loader_node_compute;
        ops.f_free              = rt_renderable_loader_node_free;
        ops.f_get_result        = rt_renderable_loader_node_get_result;
        ops.f_is_compatible     = rt_renderable_loader_node_is_compatible;
        self->_parent.ops       = ops;
        return (struct render_node_ex_impl*) self;
}
char* __implement rt_renderable_loader_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        return "rt_renderer: use pt renderer's implementation";
}

void __implement rt_renderable_loader_node_compute(struct render_node_ex_impl* self,
                                                   const struct render_node_ex_impl* input[],
                                                   const struct render_node_ex_impl* output[])
{
}

void* __implement rt_renderable_loader_node_get_result(const struct render_node_ex_impl* self)
{
        return nullptr;
}

void __implement rt_renderable_loader_node_free(struct render_node_ex_impl* self)
{
}

// render output node (use pt renderer's implementation)
struct render_node_ex_impl* __callback rt_render_output_node_creator(struct render_node_ex* parent)
{
        struct rt_renderable_loader_node* self = alloc_obj(self);
        zero_obj(self);
        // fill in ops for the parent
        struct render_node_ex_ops       ops;
        ops.f_compute           = rt_render_output_node_compute;
        ops.f_free              = rt_render_output_node_free;
        ops.f_get_result        = rt_render_output_node_get_result;
        ops.f_is_compatible     = rt_render_output_node_is_compatible;
        self->_parent.ops       = ops;
        return (struct render_node_ex_impl*) self;
}

char* __implement rt_render_output_node_is_compatible(struct render_node_ex_impl* self, struct render_tree* tree)
{
        return "rt_renderer: use pt renderer's implementation";
}

void __implement rt_render_output_node_compute(struct render_node_ex_impl* self,
                                               const struct render_node_ex_impl* input[],
                                               const struct render_node_ex_impl* output[])
{
}

void* __implement rt_render_output_node_get_result(const struct render_node_ex_impl* self)
{
        return nullptr;
}

void __implement rt_render_output_node_free(struct render_node_ex_impl* self)
{
}
