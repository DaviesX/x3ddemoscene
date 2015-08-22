#include <x3d/common.h>
#include <x3d/rendertree.h>
#include <x3d/renderer.h>
#include <renderer/rendererinsmod.h>
#include "ptrenderer.h"


__dlexport void rendererinsmod_x3d();
__dlexport void rendererrmmod_x3d();


void rendererinsmod_x3d()
{
        renderer_install_render_node_factory(RendererPathTracer, RenderNodeRadiance, pt_radiance_node_creator);
        renderer_finalize_render_node_factory();
}

void rendererrmmod_x3d()
{
        renderer_remove_render_node_factory(RendererPathTracer, RenderNodeRadiance);
}
