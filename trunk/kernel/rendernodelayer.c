#include <system/allocator.h>
#include <x3d/rendertree.h>
#include <x3d/rendernodelayer.h>


struct render_node* render_node_layer_create(const char* name)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_layer* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         nullptr,
                         nullptr,
                         nullptr,
                         nullptr,
                         RenderNodeLayerLoader, name, c_NumInput, c_NumOutput);
        render_node_ex_init((struct render_node_ex*) node, RenderNodeLayerLoader);
        return (struct render_node*) node;
}
