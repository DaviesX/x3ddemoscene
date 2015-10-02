#include <system/allocator.h>
#include <system/log.h>
#include <x3d/rendertree.h>
#include <x3d/rendernoderdaloader.h>


static bool render_node_rdaloader_free(struct render_node* self)
{
        render_node_free(self);
        struct render_rdaloader* node = (struct render_rdaloader*) self;
        free_fix(node->rdacontext);
        free_fix(node);
        return true;
}

static bool render_node_rdaloader_insert(struct render_node* self, struct render_node* input)
{
        log_mild_err_dbg("cannot insert input to renderable context node");
        return false;
}

static bool render_node_rdaloader_remove(struct render_node* self, struct render_node* input)
{
        log_mild_err_dbg("cannot remove input from renderable context node, nothing to be removed");
        return false;
}

static bool render_node_rdaloader_self(struct render_tree* tree, struct render_node* self)
{
        return render_tree_check_environment(tree, RenderEnvRenderable, ((struct render_rdaloader*) self)->rdacontext);
}

void render_node_rdaloader_set_context(struct render_rdaloader* node, const char* context)
{
        if (node->rdacontext) {
                free_fix(node->rdacontext);
        }
        node->rdacontext = alg_alloc_string(context);
}

void render_node_rdaloader_set_access(struct render_rdaloader* node, enum RenderAggregateType access)
{
        node->access = access;
}

const char* render_node_rdaloader_get_context(const struct render_rdaloader* node)
{
        return node->rdacontext;
}

struct render_node* render_node_rdaloader_create(const char* name, const char* rdacontext, enum RenderAggregateType access)
{
        static const int c_NumInput = 0;
        static const int c_NumOutput = 1;

        struct render_rdaloader* node = alloc_obj(node);
        render_node_init((struct render_node*) node,
                         render_node_rdaloader_free,
                         render_node_rdaloader_self,
                         render_node_rdaloader_insert,
                         render_node_rdaloader_remove,
                         RenderNodeRenderableLoader, name, c_NumInput, c_NumOutput);
        render_node_ex_init((struct render_node_ex*) node, RenderNodeRenderableLoader);
        node->access     = access;
        node->rdacontext = alg_alloc_string(rdacontext);
        return (struct render_node*) node;
}
