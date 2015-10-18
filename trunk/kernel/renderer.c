#include <system/log.h>
#include <system/symlib.h>
#include <container/paramset.h>
#include <x3d/debug.h>
#include <x3d/renderer.h>
#include <renderer/rendererinsmod.h>
#include "rendernodedummyimpl.h"


#define MAX_RENDER_CONTEXT		32

struct renderer_ops {
        f_Rendererinsmod        f_insmod;
        f_Rendererrmmod         f_rmmod;
};

#define c_Num_Render_Node_Type          32
#define c_Num_Render_Node_Impl          32

struct renderer_node_ops_metadata {
        f_Render_Node_Creator   f_creator[c_Num_Render_Node_Type][c_Num_Render_Node_Impl];
        int                     entries[c_Num_Render_Node_Type][c_Num_Render_Node_Impl];
        int                     count[c_Num_Render_Node_Type];
};

static struct renderer_ops                      g_rend_ops = {0};
static struct renderer_node_ops_metadata        g_rend_ops_metadata = {0};


void renderer_kernel_init()
{
        g_rend_ops.f_insmod();
}

void renderer_kernel_free()
{
        g_rend_ops.f_rmmod();
}

static bool match_mod_abi(void* data, const char* symbol)
{
        const char* base_str = data;
        return !strncmp(symbol, base_str, strlen(base_str));
}

bool renderer_import(struct symbol_set* symbols)
{
        struct renderer_ops *t_ops = &g_rend_ops;
        bool ret = true;

        if (!(t_ops->f_insmod = (f_Rendererinsmod) symlib_ret_abi2(symbols, "rendererinsmod_x3d", match_mod_abi))) {
                log_severe_err_dbg("fail to retrieve abi: rendererinsmod_x3d");
                ret = false;
        }
        if (!(t_ops->f_rmmod = (f_Rendererrmmod) symlib_ret_abi2(symbols, "rendererrmmod_x3d", match_mod_abi))) {
                log_severe_err_dbg("fail to retrieve abi: rendererinsmod_x3d");
                ret = false;
        }
        return ret;
}

/* renderer's */
struct render_tree_data {
        struct render_tree*     tree;
        bool                    is_succeeded;
};

static void render_tree_find_compatible(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        struct render_tree_data* data   = cast(data) dataptr;
        if (!data->is_succeeded) {
                return;
        }
        enum RenderNodeType type        = render_node_get_type(node);
        struct render_node_ex* node_ex  = cast(node_ex) node;
        int first_compatible            = -1;
        char* errors[c_Num_Render_Node_Impl];
        int i;
        for (i = 0; i < g_rend_ops_metadata.count[type]; i ++) {
                if (node_ex->metainst[i]) {
                        if (nullptr == (errors[i] = node_ex->metainst[i]->ops.f_is_compatible(node_ex->metainst[i], data->tree))) {
                                first_compatible = i;
                                break;
                        }
                }
        }
        if (first_compatible == -1) {
                if (type != RenderNodeRoot) {
                        log_severe_err_dbg("no compatible implementation for render node: %s of type %d, rendering will be abort, below are the errors reported from each implementation: ", render_node_get_name(node), render_node_get_type(node));
                        for (i = 0; i < g_rend_ops_metadata.count[type]; i ++) {
                                log_severe_err_dbg("\t%s", errors[i]);
                        }
                        data->is_succeeded = false;
                } else {
                        // root node implementation is optional, using dummy node implementation to fake the root node
                        first_compatible = -1;
/* @fixme (davis#2#): <render_tree_find_compatible> find a way to make render root node optional and remove all render root node check */

                        // node_ex->metainst[first_compatible] = dummy_root_node_impl_create();
                        data->is_succeeded = true;
                }
        } else {
                data->is_succeeded = true;
        }
        node_ex->first_compatible = first_compatible;
}

static void render_tree_compute(struct render_node* node, struct render_node* input[], struct render_node* output[], void* dataptr)
{
        /* @fixme (davis#2#): <render_tree_compute> this hack should be removed after root node is fixed */
        if (render_node_get_type(node) == RenderNodeRoot) {
                return ;
        }
        struct render_tree* tree        = cast(tree) dataptr;
        struct render_node_ex* node_ex  = cast(node_ex) node;

        const struct render_node_ex_impl** input_list  = alloc_fix(sizeof(input_list), node->n_input);
        const struct render_node_ex_impl** output_list = alloc_fix(sizeof(output_list), node->n_output);
        int j;
        for (j = 0; j < node->n_input; j ++) {
                struct render_node_ex* input_ex = (struct render_node_ex*) input[j];
                input_list[j] = input_ex->metainst[input_ex->first_compatible];
        }
        /* @fixme (davis#2#): <render_tree_compute> output node couldn't be passed until root node if fixed  */
        /*
        for (j = 0; j < node->n_output; j ++) {
                struct render_node_ex* output_ex = (struct render_node_ex*) output[j];
                output_list[j] = output_ex->metainst[output_ex->first_compatible];
        }*/
        if (node_ex->metainst[node_ex->first_compatible]) {
                node_ex->metainst[node_ex->first_compatible]->ops.
                        f_compute(node_ex->metainst[node_ex->first_compatible], input_list, output_list);
        }
        free_fix(output_list);
        free_fix(input_list);
}

bool renderer_render(struct render_tree* tree)
{
        debugger_invoke_begin();
        struct alg_var_set      dbg_params;
        alg_var_set_init(&dbg_params);
        alg_var_set_declare(&dbg_params, "tree", tree, sizeof(tree));
        debugger_invoke(Debug_renderer_c_renderer_render, (struct alg_var_set*) tree);

        if (!render_tree_verify(tree)) {
                log_severe_err_dbg("couldn't verify render tree");
                return false;
        }
        struct render_tree_data data;
        data.tree               = tree;
        data.is_succeeded       = true;

        struct render_tree_visitor visitor_compatibility;
        render_tree_visitor_init2(&visitor_compatibility, nullptr, render_tree_find_compatible, &data);
        render_tree_visit(tree, &visitor_compatibility);
        if (!data.is_succeeded) {
                return false;
        }

        data.is_succeeded       = true;
        struct render_tree_visitor visitor_compute;
        render_tree_visitor_init2(&visitor_compute, nullptr, render_tree_compute, &data);
        render_tree_visit(tree, &visitor_compute);
        if (!data.is_succeeded) {
                return false;
        }

        debugger_invoke_end();
        alg_var_set_free(&dbg_params);
        return true;
}

/* render_node_factory's */
void renderer_install_render_node_factory(int id, enum RenderNodeType type, f_Render_Node_Creator creator)
{
        int count = g_rend_ops_metadata.count[type];
        g_rend_ops_metadata.f_creator[type][id] = creator;
        g_rend_ops_metadata.entries[type][count] = id;
        g_rend_ops_metadata.count[type] = ++ count;
}

void renderer_remove_render_node_factory(int id, enum RenderNodeType type)
{
        g_rend_ops_metadata.f_creator[type][id] = nullptr;
}

void render_node_ex_init(struct render_node_ex* self, enum RenderNodeType type)
{
        self->type = type;
        int i;
        for (i = 0; i < g_rend_ops_metadata.count[type]; i ++) {
                int k = g_rend_ops_metadata.entries[type][i];
                self->metainst[i] = g_rend_ops_metadata.f_creator[type][k](self);
                self->metainst[i]->ref = self;
        }
}

void render_node_ex_free(struct render_node_ex* self)
{
        int j;
        for (j = 0; j < c_Num_Render_Node_Type; j ++) {
                int i;
                for (i = 0; i < g_rend_ops_metadata.count[j]; i ++) {
                        self->metainst[i]->ops.f_free(self->metainst[i]);
                }
        }
}

struct render_node_ex* render_node_ex_impl_get_ex(struct render_node_ex_impl* self)
{
        return self->ref;
}

struct render_node_ex_impl* render_node_ex_get_ex_impl(struct render_node_ex* self, int id)
{
        return self->metainst[id];
}



// Test Cases:


// renderer_tree_test
__dlexport void __callback                  renderer_tree_test_init(struct alg_var_set* envir) {}
__dlexport void __callback                  renderer_tree_test_free(struct alg_var_set* envir) {}
__dlexport enum DebugPosition* __callback   renderer_tree_test_pos(struct alg_var_set* envir, int* n_pos, int* num_run, bool* is_skipped)
{
        static enum DebugPosition pos[] = {
                Debug_renderer_c_renderer_render,
        };
        *n_pos = sizeof(pos)/sizeof(enum DebugPosition);
        *num_run = 1;
        *is_skipped = false;
        return pos;
}
__dlexport void __callback                  renderer_tree_test(struct alg_var_set* envir)
{
        log_normal_dbg("renderer tree test commence");
        struct render_tree* tree = (struct render_tree*) envir; //alg_var_set_use(envir, "tree");
        render_tree_print(tree);
}
