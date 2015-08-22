#include <system/allocator.h>
#include <x3d/rendertree.h>
#include <x3d/rendernodeoutput.h>


static bool render_node_output_free(struct render_node* self)
{
        render_node_ex_free((struct render_node_ex*) self);
        render_node_free(self);
        struct render_output* node = (struct render_output*) self;
        free_fix(node->probe);
        free_fix(node);
        return true;
}

static bool render_node_output_insert(struct render_node* self, struct render_node* input)
{
        render_node_set_input(self, input, 0);
        return true;
}

static bool render_node_output_remove(struct render_node* self, struct render_node* input)
{
        render_node_remove_input(self, input);
        return true;
}

static bool render_node_verify_self(struct render_tree* tree, struct render_node* self)
{
        return render_tree_check_environment(tree, RENDER_ENV_PROBE, ((struct render_output*) self)->probe);
}

void render_node_output_set_probe(struct render_output* node, const char* probe)
{
        if (node->probe) {
                free_fix(node->probe);
        }
        node->probe = alg_alloc_string(probe);
}

struct render_node* render_node_output_create(const char* name, const char* probe)
{
        static const int c_NumInput = 1;
        static const int c_NumOutput = 1;

        struct render_output* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_output_free,
                         render_node_verify_self,
                         render_node_output_insert,
                         render_node_output_remove,
                         RnederNodeOutput, name, c_NumInput, c_NumOutput);
        render_node_ex_init((struct render_node_ex*) node, RenderNodeOutput);
        node->probe = alg_alloc_string(probe);
        return (struct render_node*) node;
}
