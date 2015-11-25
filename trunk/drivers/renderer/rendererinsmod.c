#include <x3d/common.h>
#include <x3d/rendertree.h>
#include <x3d/renderer.h>
#include <renderer/rendererinsmod.h>
#include "ptrenderer.h"
#include "rtrenderer.h"


__dlexport void rendererinsmod_x3d();
__dlexport void rendererrmmod_x3d();


void rendererinsmod_x3d()
{
        // install pt renderer to the core
        renderer_install_render_node_factory(RendererPathTracer, RenderNodeRadiance, pt_radiance_node_creator);
        renderer_install_render_node_factory(RendererPathTracer, RenderNodeRenderableLoader, pt_renderable_loader_node_creator);
        renderer_install_render_node_factory(RendererPathTracer, RenderNodeOutput, pt_render_output_node_creator);
        // install rt renderer to the core
        renderer_install_render_node_factory(RendererRasterizer, RenderNodeRadiance, rt_radiance_node_creator);
        renderer_install_render_node_factory(RendererRasterizer, RenderNodeRenderableLoader, rt_renderable_loader_node_creator);
        renderer_install_render_node_factory(RendererRasterizer, RenderNodeOutput, rt_render_output_node_creator);
}

void rendererrmmod_x3d()
{
        // remove pt renderer from the core
        renderer_remove_render_node_factory(RendererPathTracer, RenderNodeRadiance);
        renderer_remove_render_node_factory(RendererPathTracer, RenderNodeRenderableLoader);
        renderer_remove_render_node_factory(RendererPathTracer, RenderNodeOutput);
        // remove rt_renderer from the core
        renderer_remove_render_node_factory(RendererRasterizer, RenderNodeRadiance);
        renderer_remove_render_node_factory(RendererRasterizer, RenderNodeRenderableLoader);
        renderer_remove_render_node_factory(RendererRasterizer, RenderNodeOutput);
}
