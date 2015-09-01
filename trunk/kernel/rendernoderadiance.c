#include <system/allocator.h>
#include <system/log.h>
#include <x3d/rendertree.h>
#include <x3d/rendernoderadiance.h>


static const int c_RenderableLoaderSlot = 0;

static bool render_node_radiance_free(struct render_node* self)
{
        render_node_free(self);
        free_fix(self);
        return true;
}

static bool render_node_radiance_insert(struct render_node* self, struct render_node* input)
{
        enum RenderNodeType type = render_node_get_type(input);
        switch(type) {
        case RenderNodeRenderableLoader:
                {
                render_node_set_input(self, input, c_RenderableLoaderSlot);
                return true;
                }
        default:
                {
                log_mild_err_dbg("unsupported input type: %d", type);
                return false;
                }
        }
}

static bool render_node_radiance_remove(struct render_node* self, struct render_node* input)
{
        render_node_remove_input(self, input);
        return true;
}

static bool render_node_radiance_self(struct render_tree* tree, struct render_node* self)
{
        return true;
}

void render_node_radiance_set_pipe(struct render_radiance* node, enum RenderPipeType pipe)
{
        node->pipe = pipe;
}

enum RenderPipeType render_node_radiance_get_pipe(struct render_radiance* node)
{
        return node->pipe;
}

const int render_node_radiance_get_input_slot(enum RenderNodeType type)
{
        switch(type) {
        case RenderNodeRenderableLoader:
                {
                return c_RenderableLoaderSlot;
                }
        default:
                {
                log_severe_err_dbg("render radiance doesn't contain input nodes of type: %d", type);
                return -1;
                }
        }
}

struct render_node* render_node_radiance_create(const char* name, enum RenderPipeType pipe)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_radiance* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_radiance_free,
                         render_node_radiance_self,
                         render_node_radiance_insert,
                         render_node_radiance_remove,
                         RenderNodeRadiance, name, c_NumInput, c_NumOutput);
        render_node_ex_init((struct render_node_ex*) node, RenderNodeRadiance);
        node->pipe = pipe;
        return (struct render_node*) node;
}
